/**
*	created:		2012-6-19   21:24
*	filename: 		WindowFont
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
/*
** 思路：采取缓存技术绘制文字
** 实现：当要绘制一个字符时，先在缓存里面查找(二分查找)，找到直接绘制，
		找不到则加入(或替代)一个字符缓存，并通过TextOut绘制字符轮廓在
		内存中，然后根据需要(如加轮廓)生成一个字符轮廓的映射表，一个字
		符的缓存就这样产生了。字符在进行MMX绘制时，需要的是这个字符的
		映射表。
	
** 修改记录：
	1.原来每个字符的映射表都是实时new出来的，现在改为在创建字体时一次性创建。
	2.原来不支持绘制文字的轮廓线，现在加入了支持。
*/
//------------------------------------------------------------------------
#include "../../FKTrace/IFKTrace.h"
#include "../IFKCanvas.h"
#include "../Include/StructCommon.h"
#include "../Include/WindowChar.h"
#include "../Include/WindowFont.h"
#include "../asm/asm.h"
//------------------------------------------------------------------------
// 销毁缓存
void XFont::Destroy()
{
	if (m_nCacheSize == 0)
		return;

	delete[] m_CharBuf;
	delete[] m_Data;
	delete[] m_FontData;

	DeleteObject(m_FontBmp);
	DeleteDC(m_FontDC);
	m_nCacheNum = m_nCacheSize = 0;
	m_nFreq = 0;
}
//------------------------------------------------------------------------
// 扩展缓存的大小
void XFont::ExpandCacheSize(int newsize)
{
	Data* newbuf;
	WFontData* new_font_buf;
	newbuf = new Data[newsize];
	new_font_buf = new WFontData[newsize];
	memcpy(newbuf, m_Data, m_nCacheSize*sizeof(Data));
	memcpy(new_font_buf, m_FontData, m_nCacheSize*sizeof(WFontData));

	TEXTMETRIC tm;
	GetTextMetrics(m_FontDC, &tm);
	int nCharSize;
	if (m_bOutline)
		nCharSize = (tm.tmHeight+2)*(tm.tmMaxCharWidth+2);
	else
		nCharSize = tm.tmHeight*tm.tmMaxCharWidth;
	nCharSize = (nCharSize + 3) & -4; // 4字节对齐,便于快速定位
	int nAllCacheSize = nCharSize*newsize;
	BYTE* newCharBuf = new BYTE[nAllCacheSize];
	memcpy(newCharBuf, m_CharBuf, m_nCacheSize*nCharSize);
	m_FontData->data = newCharBuf;
	for (int i=m_nCacheSize; i<newsize; i++)
		m_FontData[i].data = m_FontData[i-1].data + nCharSize;

	delete[] m_Data;
	delete[] m_FontData;
	delete[] m_CharBuf;

	m_nCacheSize = newsize;
	m_Data = newbuf;
	m_FontData = new_font_buf;
	m_CharBuf = newCharBuf;
}
//------------------------------------------------------------------------
// 在缓存中搜索一个字符，如果不存在返回<0的值
// 二分搜索
int XFont::SearchChar(unsigned int c)const
{
	int begin, end, middle;
	unsigned int Char;
	begin = 0;
	end = m_nCacheNum - 1;
	while (begin <= end)
	{
		middle = (begin + end)/2;
		Char = m_Data[middle].c;
		if (c == Char)
			return middle;
		else if (c < Char)
			end = middle - 1;
		else
			begin = middle + 1;
	}
	return -begin-1;
}
//------------------------------------------------------------------------
// 得到字符在缓存中的位置
int XFont::GetCachePos(unsigned int c, int pos)
{
	int nPos = 0;
	if (m_nCacheNum >= m_nCacheSize) // Cache满,删掉最早进Cache的
	{
		int min = m_nFreq + 1, min_idx = -1;
		
		// 查找freq最小的那个id
		for (int i=0; i<m_nCacheNum; i++)
		{
			if (m_Data[i].freq == 1)
			{
				min = 1;
				min_idx = i;
				break;
			}
			if (m_Data[i].freq < min)
			{
				min = m_Data[i].freq;
				min_idx = i;
			}
		}
		
		if (m_Locked != 0 && min >= m_Locked)
		{
			if (m_nCacheSize < 1024)
				ExpandCacheSize(m_nCacheSize*2);
			else
				ExpandCacheSize(m_nCacheSize + 1024);
			
			for (int i=m_nCacheNum; i>pos; i--)
				m_Data[i] = m_Data[i-1];
			
			nPos = m_nCacheNum;
			m_Data[pos].freq = m_nFreq;
			m_Data[pos].c = c;
			m_Data[pos].idx = m_nCacheNum;
			++ m_nCacheNum;
		}
		else // 删除最早进入Cache的，并插入新的
		{
			int idx = m_Data[min_idx].idx;
			nPos = idx;
			
			if (pos == m_nCacheNum)	// 替代最后一个
				-- pos;
			
			if (pos > min_idx)	// 插入位置在删除点的后面
			{
				for (int i=min_idx; i<pos; i++)
					m_Data[i] = m_Data[i+1];
				m_Data[pos].freq = m_nFreq;
				m_Data[pos].c = c;
				m_Data[pos].idx = idx;
			}
			else// 插入位置在删除点的前面或正好是删除点
			{
				for (int i=min_idx; i>pos; i--)
					m_Data[i] = m_Data[i-1];
				m_Data[pos].freq = m_nFreq;
				m_Data[pos].c = c;
				m_Data[pos].idx = idx;
			}
			
			// 各个数据的freq均进行调整
			m_nFreq -= min;
			if (m_Locked > 0)
				m_Locked -= min;
			for (int i=0; i<m_nCacheNum; i++)
				m_Data[i].freq -= min;
		}
	}
	else // Cache不满,添加
	{
		nPos = m_nCacheNum;
		for (int i=m_nCacheNum; i>pos; i--)
			m_Data[i] = m_Data[i-1];
		m_Data[pos].freq = m_nFreq;
		m_Data[pos].c = c;
		m_Data[pos].idx = m_nCacheNum;
		++ m_nCacheNum;
	}
	return nPos;
}
//------------------------------------------------------------------------
// 得到一个字符数据，先搜索缓存，如果找不到则新创建一个
void* XFont::GetChar(unsigned int c)
{
	int i, j, pos;
	WFontData* temp;
	if (m_nCacheSize == 0)
		return 0;
	++ m_nFreq;

	// 先在缓存中找
	pos = SearchChar(c);

	// 找到!
	if (pos >= 0)
	{
		m_Data[pos].freq = m_nFreq;
		return &m_FontData[m_Data[pos].idx];
	}

	// 未找到
	pos = -pos - 1;

	int nPos = GetCachePos(c, pos);
	temp = &m_FontData[nPos];
	
	// 计算字符数据到 temp->data;
	// 从 DC 取 (无反混淆)
	static RECT rect;
	static char str[3] = {0, 0, 0};
	SIZE si;

	rect.top = 0;
	rect.left = 0;
	rect.right = m_MaxWidth;
	rect.bottom = m_Height;

	// 绘制字符并得到字符大小
	if (c < 256) // 单字节字符
	{
		str[0] = c;
		str[1] = 0;
		TextOut(m_FontDC, 0, 0, str, 1); // 这个函数的速度是 DrawText 的6倍
		GetTextExtentPoint32(m_FontDC, str, 1, &si);
	}
	else // 双字节字符
	{
		str[0] = c >> 8;
		str[1] = c & 0xff;
		TextOut(m_FontDC, 0, 0, str, 2);
		GetTextExtentPoint32(m_FontDC, str, 2, &si);
	}

	// 检测字符上部分有多少空白行
	WORD* line = m_Buffer;
	int from;
	for (i=0; i<si.cy; i++,line+=m_MaxWidth)
	{
		for (j=0; j<si.cx; j++)
			if (line[j] != 0)
				goto _find;
	}
_find:
		from = i;
		si.cy -= from;
		int ox = si.cx;
		int oy = si.cy;
		if (m_bOutline) // 考虑轮廓
		{
			si.cx += 2;	
			si.cy += 2;
		}
		if (si.cx > m_MaxWidth)
			si.cx = m_MaxWidth;
		if (si.cy > m_Height)
			si.cy = m_Height;
		temp->w = si.cx;
		temp->h = si.cy;
		temp->width = si.cx;
		temp->kx = 0;
		temp->ky = -from;
		temp->size = si.cx*si.cy;
		line = m_Buffer + from*m_MaxWidth;
		BYTE* des = temp->data;

		// 字符用0x10表示,轮廓用0xFF表示
		if (m_bOutline)
		{
			memset(temp->data, 0, temp->size);
			line = m_Buffer + from*m_MaxWidth;
			des = temp->data;
			des += si.cx;	// 考虑轮廓
			for (i=0; i<oy; i++,line+=m_MaxWidth,des+=si.cx)
			{
				for (j=0; j<ox; j++)
				{
					if (line[j] != 0) // 构造字符的轮廓
					{
						int k = j + 1;	// 考虑轮廓
						des[k] = 0x10;
						
						// left-top
						if (des[k-si.cx-1] == 0)
							des[k-si.cx-1] = 0xFF;
						// top
						if (des[k-si.cx] == 0)
							des[k-si.cx] = 0xFF;
						// right-top
						if (des[k-si.cx+1] == 0)
							des[k-si.cx+1] = 0xFF;
						// left
						if (des[k-1] == 0)
							des[k-1] = 0xFF;
						// right
						if (des[k+1] == 0)
							des[k+1] = 0xFF;
						// left-bottom
						if (des[k+si.cx-1] == 0)
							des[k+si.cx-1] = 0xFF;
						// bottom
						if (des[k+si.cx] == 0)
							des[k+si.cx] = 0xFF;
						// right-bottom
						if (des[k+si.cx+1] == 0)
							des[k+si.cx+1] = 0xFF;
					}
				}
			}
		}
		else
		{
			for (i=0; i<oy; i++,line+=m_MaxWidth,des+=si.cx)
			{
				for (j=0; j<ox; j++)
					des[j] = (line[j] == 0) ? 0 : 0x10;
				
				for (; j<si.cx; j++)
					des[j] = 0;
			}
		}

	return temp;
}
//------------------------------------------------------------------------
// 字体创建(初始化)
// 目前游戏没有使用带alpha的字体，这里alpha参数用于指定是否带轮廓(alpha>0)
// cachesize: 缓存大小，一般来讲，如果这种字体在屏幕上绘制的文字大概是多少，那么
//            就可以将缓存大小设为多少，为了不影响性能，一般可设置为128~256之间
BOOL XFont::Create(HFONT font, int alpha /* = 16 */, int cachesize /* = 128 */)
{
	if (alpha > 1) alpha = 1;
	if (alpha < 0) alpha = 0;
	if (cachesize <= 0) cachesize = 128;
	int BMPINFO16[13] =
	{
		sizeof(BITMAPINFOHEADER),
			0,	// 宽度
			0,	// 高度
			0x100001,BI_BITFIELDS,0,0,0,0,0,
			0xf800,0x7e0,0x1f
	};

	if (m_nCacheSize != 0)
		Destroy();

	// 创建一个兼容DC
	m_FontDC = CreateCompatibleDC(NULL);
	if (font != 0)
		SelectObject(m_FontDC, font);

	// 获取文字的尺寸
	SIZE si;
	TEXTMETRIC tm;
	GetTextMetrics(m_FontDC, &tm);
	// 由于有的英文字体在绘制汉字时，实际是调用的系统默认的汉字字体，
	// 所以导致tm.tmMaxCharWidth < 默认汉字字体的宽度，故此处进行修正
	GetTextExtentPoint32(m_FontDC, "峰", 2, &si);
	m_MaxWidth = (tm.tmMaxCharWidth < si.cx) ? si.cx : tm.tmMaxCharWidth;
	m_Height = (tm.tmHeight < si.cy) ? si.cy : tm.tmHeight;
	m_bOutline = alpha;
	if (m_bOutline)	// 考虑轮廓
	{
		m_Height += 2;
		m_MaxWidth += 2;
	}
	int nCharSize = m_MaxWidth*m_Height; // 一个字符的映射表的大小
	nCharSize = (nCharSize + 3) & -4; // 4字节对齐,便于快速定位
	m_MaxWidth = (m_MaxWidth + 3) & -4; // 4像素对齐

	// 缓存设置
	m_nCacheNum = 0;
	m_Locked = 0;
	m_nCacheSize = cachesize;
	m_Data = new Data[m_nCacheSize];
	m_FontData = new WFontData[m_nCacheSize];

	// 创建所有字符的数据缓存
	int nAllCacheSize = nCharSize*cachesize;
	m_CharBuf = new BYTE[nAllCacheSize];
	m_FontData->data = m_CharBuf;
	for (int i=1; i<m_nCacheSize; i++)
		m_FontData[i].data = m_FontData[i-1].data + nCharSize;
	if (m_Data == 0 || m_FontData == 0 || m_CharBuf == 0)
		return FALSE;

	// 创建一个小位图用于绘制字符(黑背景、白色文字)
	BMPINFO16[1] = m_MaxWidth;
	BMPINFO16[2] = -m_Height;
	m_FontBmp = CreateDIBSection(m_FontDC, (BITMAPINFO*)&(BMPINFO16[0]),
		DIB_RGB_COLORS, (void**)&m_Buffer, 0, 0);
	SelectObject(m_FontDC, m_FontBmp);
	SetTextColor(m_FontDC, 0xffffff);
	SetBkColor(m_FontDC, 0);
	
	return TRUE;
}
//------------------------------------------------------------------------
// 获取字符串的像素宽度
DWORD XFont::GetLength(const char* str)
{
	DWORD len = 0;

	for (int i=0; str[i]; i++)
	{
		unsigned int a, b;
		a = (unsigned char)str[i];
		if (a > 0x80) // 双字节文字
		{
			++i;
			b = (unsigned char)str[i];
			if (b == 0)
				break;
			a = a*256 + b;
		}

		len += GetCharLength(a);
	}
	
	return len;
}
//------------------------------------------------------------------------
// 获取字符的像素宽度
DWORD XFont::GetCharLength(const WORD c)
{
	WChar tmp;
	tmp.Create((ITTFont*)this, c, 0);
	return tmp.GetWidth();
}
//------------------------------------------------------------------------
// 创建字体接口
extern "C" __declspec(dllexport) 
BOOL CreateITTFont(DWORD dwVersion, ITTFont** ppITTFont, ITrace* pTrace)
{
#ifdef _DEBUG
	if (pTrace)
		g_DefaultTrace.Create(pTrace);
#endif
	XFont* pFont = new XFont;
	if (!pFont)
		return FALSE;
	*ppITTFont = static_cast<ITTFont*>(pFont);
	return *ppITTFont != NULL;
}
//------------------------------------------------------------------------
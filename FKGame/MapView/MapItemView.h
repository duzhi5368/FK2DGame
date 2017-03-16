/**
*	created:		2012-6-29   19:29
*	filename: 		MapItemView
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include "../MapInfo/MapItem.h"
#include <list>
using namespace std;
//------------------------------------------------------------------------
struct ICanvas;
struct IBitmap;
struct IMapView;
//------------------------------------------------------------------------
enum MouseEvent{mouseMove = 1,moveLeftClick = 2, mouseRightClick = 3,mouseLeave/*失去焦点*/ = 4};
//------------------------------------------------------------------------
class CMapItemView	: public CMapItem// 地上物对象的视图接口
{
public:
	CMapItemView()
	{
		m_dwFlag = 0;
		m_nFrameOld = -1;
		m_bNeedSetOldDirty = FALSE;
		SetRectEmpty(&m_rcArea);
		SetRectEmpty(&m_rcAreaOld);
		(__int64&)m_ptLeft = 0;
		(__int64&)m_ptRight = 0;
		(__int64&)m_ptWold = 0;
	}
	virtual ~CMapItemView()	{}

	// 绘图循环请求对象绘制到指定的ICanvas上
	// dwFlag : 如人物绘制时有是否半透明绘制的选择
	virtual void OnDraw(ICanvas* pCanvas, const POINT& ptTileCenter, DWORD dwFlag = 0) = NULL;

	// 获取对象矩形大小范围，以锚点偏移为(0,0)，是相对于锚点的，跟屏幕无关
	// 如果要获得对象在屏幕上的范围，调用OffsetRect(pItemView->GetShowRect(),ptTileCenter);
	virtual RECT GetShowRect() = NULL;

	// 获取锚点
	// 锚点 ： 对象图片左上角 与 对象图片跟放置块中心点重叠的点 的偏移
	virtual POINT GetAnchor() const = NULL;

	// 实现CMapItem::GetLayerValue()，大部分对象处于中层
	// 对象在地图中的显示层，如尸体层比地上物层要低。
	// 有层次关系的对象通过CMapContainer::AddLayerItem()加入到地图容器中
	// 服务器实现的对象不需要考虑层值
	virtual DWORD GetLayerValue()	{return milvMidle;}

	// 获取当前显示的图像
	virtual IBitmap* GetBitmap() = NULL;

	// 保存或读取数据，用于从文件中创建对象
	// bSave为TRUE时，表示请求保存数据，否则为从buf中读取对象现场进行创建对象
	// 
	BOOL OnSerialize(BOOL bWrite,IArchive* pArchive){return FALSE;};

	/* 功能： 当对象进入视口和离开视口时被回调
	 对象在这里可以预读资源和释放资源
	 bVisable为TRUE时表示对象即象被显示，否则表示对象离开视口
	 返回：
			如果进入视野或离开视野成功，地图系统根据返回值决定是否将对象加入显示队列或从显示队列中删除
	 说明：
			如果对象总是可见，只需要在离开视野时返回FALSE
			如果对象总是不可见，只需要在进入视野时返回FALSE
	*/ 
	virtual BOOL OnEnterViewport(BOOL bVisable) = NULL;

	// 测试当用户点中某个小区域时是否选择此对象
	// ptScreen为相对于此对象落脚块中心点坐标的偏移值
	virtual BOOL HitTest(const RECT& rcScreen)	{return FALSE;}

	// 设置对象选中状态
	// 如果对象不可被选中，对象返回FALSE。如果被选中成功，返回TRUE
	virtual BOOL SetSelected(BOOL bSelected = TRUE, BOOL bHighLight = TRUE){return FALSE;}

	// 设置对象锁定状态 参数（0：不锁定 1：红圈锁定 2：绿圈锁定 3：黄圈锁定 4：蓝圈锁定）
	// 如果对象不可被锁定，对象返回FALSE。如果被锁定成功，返回TRUE
	virtual BOOL SetLocked(int nLockType = 1){return FALSE;}

	// 按位定义的item标志
	enum
	{
		flagNone=0,
		flagVisable=0x1,			// 对象是否在显示列表中
		flagCanbeSelected = 0x2,	// 对象是否可选中
		flagSelected = 0x4,			// 对象是否选中
		flagRectangle=0x8,			// 显示对象的矩形框
		flagInfo=0x10,				// 显示对象的信息(可能不仅仅是名字)
		flagHiliht=0x20,			// 高亮显示对象
		flagBloodSlot=0x40,			// 显示血槽
		flagName=0x80,				// 显示对象名字
		flagPaoPao=0x100,			// 显示paopao信息
		flagGroup=0x200,			// 显示帮会信息
	};
	/*
	 *	功能位作用说明：
	 *		flagVisable : 当对象进入或退出预读区域时，通过设置此位记住是否在显示列表中的状态
	 *		flagCanbeSelected : 地图编辑器可以选择任何itemview对象，但游戏主程序只能选择逻辑对象，所以游戏主程序创建对象时设置此位
	 */
	virtual void AddFlag(DWORD dwFlag)
	{
		m_dwFlag |= dwFlag;
	}
	virtual void RemoveFlag(DWORD dwFlag = 0xFFFFFFFF)
	{
		m_dwFlag &= ~dwFlag;
	}
	virtual DWORD GetFlag()
	{
		return m_dwFlag;
	}

	// 一些辅助函数
	BOOL IsVisable()
	{
		return m_dwFlag & flagVisable;
	}
	BOOL IsSelected()
	{
		return m_dwFlag & flagSelected;
	}
	
	// 绘制其他一些顶层对象，如血槽、聊天内容、物品名等，这些对象永远放在最上层
	virtual void OnDrawTopmostObject(ICanvas* pCanvas, const POINT& ptTileCenter, DWORD dwFlag = 0) {}

	// item在地图上的位置相关
	virtual void SetTile(POINT& ptTile, IMapView* pMapView) = NULL;
	virtual POINT GetTile() = NULL;

	// 比较显示排序顺序
	BOOL operator < (const CMapItemView& item);

	// 资源ID
	virtual DWORD GetResID() = NULL;

	// 设脏
	virtual void SetDirty(ICanvas* pCanvas, const POINT& ptViewTop) {}

	// 获取旧的脏矩阵，主要用在把对象从地图上删除时获取它的脏矩阵
	virtual void GetOldDirtyRect(list<RECT> &DirtyRectList) {}

	// 是否潜水，哇哈哈
	virtual BOOL IsSinking() {return FALSE;}
private:
	DWORD	m_dwFlag;
public:
	// 下面用于排序
	RECT	m_rcArea;			// 当前矩阵绝对位置，相对于整个地图，(left,top)即可落脚点绝对tile坐标,(right,bottom)为右端点绝对tile坐标
	POINT	m_ptLeft;			// 世界象素坐标，左端
	POINT	m_ptRight;			// 世界象素坐标，右端
	POINT	m_ptWold;			// 世界象素坐标，Tile的

	RECT	m_rcAreaOld;		// 上次画图的矩阵位置
	int		m_nFrameOld;		// 旧祯
	BOOL	m_bNeedSetOldDirty;	// 是否需要把旧的矩阵设脏
};
//------------------------------------------------------------------------
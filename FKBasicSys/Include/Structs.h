/**
*	created:		2012-7-1   19:51
*	filename: 		Structs
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		包中用到的数据结构
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#define SHADOW_ALPHA		180
//------------------------------------------------------------------------
// 包类型 (support bmp files and tga files)
enum PackType{
	ptUnknow=0,					// unknow
	ptSingleFramePrivatePalette,// a frame, private palette
	ptSingleFramePublicPalette,	// a frame, public palette
	ptMultiFramePrivatePalette,	// many frames, private palette
	ptMultiFramePublicPalette,	// many frames, public palette
	ptMultiStatePrivatePalette, // many states, many frames per state, private palette
	ptMultiStatePublicPalette,	// many states, many frames per state, public palette

	ptMaxPackType,
};
//------------------------------------------------------------------------
#include <pshpack1.h>
//------------------------------------------------------------------------
// RLE数据头
typedef struct __RLEHEADER // 20 bytes
{
	WORD wFlags;		// RLE压缩数据标识(GF) (Graphic Flag? Gao Feng?)
	DWORD dwTotalSize;	// 本结构及其调色板和数据的整个大小
	DWORD dwWidth;		// RLE图片的宽
	DWORD dwHeight;		// RLE图片的高
	BYTE nUseColor;		// 本图片使用的调色板的颜色数(规定最大255种颜色,idx=255的点看作透明点)
	BYTE bHaveAlphaChannel;// 是否有alpha通道数据
	DWORD dwDataOffset;	// 图片数据的偏移量
	__RLEHEADER(){memset(this, 0, sizeof(__RLEHEADER));wFlags='FG';}
	BOOL HavePalette(){return nUseColor!=0;}
	BOOL IsValid(){
		return (wFlags=='FG' && dwTotalSize>0 && dwWidth>0 && 
			dwHeight>0 && dwDataOffset>=sizeof(__RLEHEADER));}
} RLEHEADER;
//------------------------------------------------------------------------
// 包文件头
typedef struct PACKFILEHEADER // 16 bytes
{
	unsigned int	identify;		// 标识
	unsigned int	data_offset;	// 数据在文件中的偏移
	unsigned char	pack_type;		// 包类型
	unsigned char	color_depth;	// 色彩深度(8?15?16?24?32?)
	unsigned short	version;		// 版本(高字节为主版本，低字节为副版本)
	unsigned short	reserved1;		// 保留1
	unsigned short	reserved2;		// 保留2

	PACKFILEHEADER() {memset(this, 0, sizeof(PACKFILEHEADER)),identify='KAEP',version=(1<<8);}
	BOOL IsValid(){return (version==(1<<8) && pack_type<ptMaxPackType && identify=='KAEP');}
} stPackFileHeader;
//------------------------------------------------------------------------
// 动画包信息头(如果带调色板，则紧跟本结构后面)
typedef struct ANIINFOHEADER // 12 bytes
{
	unsigned short	struct_size;	// struct's size(maybe include palette, if have pal, follow this struct)
	unsigned short	use_color;		// palette use color count
	unsigned int	total_size;		// total frames size
	unsigned int	total_frame;	// total frame count 
	//unsigned char	pal[1];			// palette pointer
} stAniInfoHeader;
//------------------------------------------------------------------------
// 动画包中每一帧的相关数据
typedef struct FRAMEINDEX // 16 bytes
{
	unsigned int	data_offset;	// bits data offset in the data zone(在动画包数据区中的偏移)
	unsigned int	data_size;		// frame size
	unsigned short	clip_offx;		// the x-coord offset with clipped picture and original picture
	unsigned short	clip_offy;		// the y-coord offset with clipped picture and original picture
	unsigned short	width;			// frame width
	unsigned short	height;			// frame height
} stFrameIndex;
//------------------------------------------------------------------------
// 组信息头(如果带调色板，则紧跟本结构后面)
typedef struct GROUPINFOHEADER // 16 bytes
{
	unsigned short	struct_size;	// struct's size(maybe include palette)
	unsigned short	use_color;		// palette use color count
	unsigned int	total_size;		// all frames size
	unsigned int	total_ani;		// all ani count 
	unsigned int	total_frame;	// all frame count 
	//unsigned char	pal[1];			// palette pointer
} stGroupInfoHeader;
//------------------------------------------------------------------------
// 动画包索引
typedef struct ANIINDEX // 12 bytes
{
	unsigned int	ani_frames;		// ani's frame count
	unsigned int	key_frame;		// 本动画包的第1帧在动画包组所有帧中的序号（单位帧）
	unsigned int	ani_offset;		// 本动画包数据在数据区中的偏移位置（单位字节）
} stAniIndex;
//------------------------------------------------------------------------
#include <poppack.h>
//------------------------------------------------------------------------
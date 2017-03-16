/**
*	created:		2012-7-1   19:54
*	filename: 		GlobalFunc
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		一些全局函数
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#include <math.h>
#include <windows.h>
//------------------------------------------------------------------------
int  Fun_PlaySound(POINT& ptTile, int nSoundID, BOOL bLoop = FALSE);
void Fun_StopSound(int& nChunnelID);

void Fun_CalcStepTile(int nDir, int nStep, POINT& ptStep);
void Fun_CalcStepPixel(int nDir, int nStep, POINT& ptStep);

// 计算某个方向上的速度(受45度倾斜的影响)
POINT Fun_CalcDirSpeedDefaultSpeed32_45(int Dir);

// 按方向增长率矫正方向
void Fun_CalcNewDir(float &fOldDir, float fNewDir, float fIncDir);

//	+	|  -
//	3	|  4
// ------------> x
//	2	|  1
//	-	|  +
//		v  y
// 计算从一个点到另一个点的方向(使用屏幕坐标)
template<class T>
int Fun_CalcDirection8_ex(T* pptFrom, T* pptTo)
{
	int nDeltaX = pptTo->x - pptFrom->x;
	int nDeltaY = pptTo->y - pptFrom->y;
	if (nDeltaX > 0)
	{
		if (nDeltaY > 0)		return 3;
		else if (nDeltaY < 0)	return 1;
		else					return 2;
	}
	else if (nDeltaX < 0)
	{
		if (nDeltaY > 0)		return 5;
		else if (nDeltaY < 0)	return 7;
		else					return 6;
	}
	else
	{
		if (nDeltaY > 0)		return 4;
		else					return 0;
	}
	return 0; // North;
}

// 计算从一个点到另一个点的方向(使用TILE坐标)
template<class T>
int Fun_CalcDirection8_tile(T* pptFrom, T* pptTo)
{
	int nDeltaX = pptTo->x - pptFrom->x;
	int nDeltaY = pptTo->y - pptFrom->y;
	if (nDeltaX > 0)
	{
		if (nDeltaY > 0)		return 2; // East;
		else if (nDeltaY < 0)	return 4; // South;
		else					return 3; // EastSouth;
	}
	else if (nDeltaX < 0)
	{
		if (nDeltaY > 0)		return 0; // North;
		else if (nDeltaY < 0)	return 6; // West;
		else					return 7; // WestNorth;
	}
	else
	{
		if (nDeltaY > 0)		return 1; // EastNorth;
		else					return 5; // WestSouth;
	}
	return 0; // North;
}

//	+	|  -
//	3	|  4
// ------------> x
//	2	|  1
//	-	|  +
//		v  y
// 根据范围区间模糊确定(笛卡儿视角，不受45度倾斜的影响)
template<class T>
int Fun_CalcDirection8(T* pptFrom, T* pptTo)
{
	int nDeltaX = pptTo->x - pptFrom->x;
	int nDeltaY = pptTo->y - pptFrom->y;
	float fSlope; // 斜率
	if (nDeltaX == 0)
		return (nDeltaY >= 0) ? 4 : 0;
	else
		fSlope = (float)nDeltaY/nDeltaX;
	
	if (fSlope >= 0.0f) // 1,3区间
	{
		if (nDeltaX >= 0) // 1
		{
			if (fSlope >= 2.414f)	return  4; // 90	(67.5～90.0) < tan(67.5) = 2.414 >
			if (fSlope >= 0.414f)	return  3; // 45	(22.5～67.5) < tan(22.5) = 0.414 >
			if (fSlope >=   0.0f)	return  2; // 0		(00.0～22.5)
		}
		else // 3
		{
			if (fSlope >= 2.414f)	return 0;
			if (fSlope >= 0.414f)	return 7;
			if (fSlope >=   0.0f)	return 6;
		}
	}
	else // 2,4
	{
		fSlope = -fSlope;
		if (nDeltaX >= 0) // 4
		{
			if (fSlope >= 2.414f)	return  0;
			if (fSlope >= 0.414f)	return  1;
			if (fSlope >=   0.0f)	return  2;
		}
		else // 2
		{
			if (fSlope >= 2.414f)	return 4;
			if (fSlope >= 0.414f)	return 5;
			if (fSlope >=   0.0f)	return 6;		
		}
	}
	return 0;
}

//	+	|  -
//	3	|  4
// ------------> x
//	2	|  1
//	-	|  +
//		v  y
// 根据范围区间模糊确定(45度视角)
template<class T>
int Fun_CalcDirection8_45(T* pptFrom, T* pptTo)
{
	int nDeltaX = pptTo->x - pptFrom->x;
	int nDeltaY = pptTo->y - pptFrom->y;
	float fSlope; // 斜率
	if (nDeltaX == 0)
		return (nDeltaY >= 0) ? 4 : 0;
	else
		fSlope = (float)nDeltaY/nDeltaX;
	
	if (fSlope >= 0.0f) // 1,3区间
	{
		if (nDeltaX >= 0) // 1
		{
			if (fSlope >= 1.618f)	return  4; // 90
			if (fSlope >= 0.236f)	return  3; // 45
			if (fSlope >=  0.0f)	return  2; // 0
		}
		else // 3
		{
			if (fSlope >= 1.618f)	return 0;
			if (fSlope >= 0.236f)	return 7;
			if (fSlope >=  0.0f)	return 6;
		}
	}
	else // 2,4
	{
		fSlope = -fSlope;
		if (nDeltaX >= 0) // 4
		{
			if (fSlope >= 1.618f)	return  0;
			if (fSlope >= 0.236f)	return  1;
			if (fSlope >=  0.0f)	return  2;
		}
		else // 2
		{
			if (fSlope >= 1.618f)	return 4;
			if (fSlope >= 0.236f)	return 5;
			if (fSlope >=  0.0f)	return 6;		
		}
	}
	return 0;
}


//	+	|  -
//	3	|  4
// ------------> x
//	2	|  1
//	-	|  +
//		v  y
// 根据范围区间模糊确定(笛卡儿视角，不受45度倾斜的影响)
template<class T>
int Fun_CalcDirection32(T* pptFrom, T* pptTo)
{
	int nDeltaX = pptTo->x - pptFrom->x;
	int nDeltaY = pptTo->y - pptFrom->y;
	float fSlope;
	if (nDeltaX == 0)
		return (nDeltaY >= 0) ? 16 : 0;
	else
		fSlope = (float)nDeltaY/nDeltaX;
	
	if (fSlope >= 0.0f) // 1,3区间
	{
		if (nDeltaX >= 0) // 1
		{
			if (fSlope >= 10.153f)	return 16; // 90	(84.375 ～ 90.000) < tan(84.375) = 10.153 >
			if (fSlope >=  3.297f)	return 15; // 78.75	(73.125 ～ 84.375) < tan(73.125) =  3.297 >
			if (fSlope >=  1.871f)	return 14; // 67.50	(61.875 ～ 73.125) < tan(61.875) =  1.871 >
			if (fSlope >=  1.219f)	return 13; // 56.25	(50.625 ～ 61.875) < tan(50.625) =  1.219 >
			if (fSlope >=  0.821f)	return 12; // 45.00	(39.375 ～ 50.625) < tan(39.375) =  0.821 >
			if (fSlope >=  0.535f)	return 11; // 33.75	(28.125 ～ 39.375) < tan(28.125) =  0.535 >
			if (fSlope >=  0.303f)	return 10; // 22.50	(16.875 ～ 28.125) < tan(16.875) =  0.303 >
			if (fSlope >=  0.098f)	return  9; // 11.25	(05.625 ～ 16.875) < tan(05.625) =  0.098 >
			if (fSlope >=  0.000f)	return  8; // 0		(00.000 ～ 05.625) < tan(00.000) =  0.000 >
		}
		else // 3
		{
			if (fSlope >= 10.153f)	return 0;
			if (fSlope >=  3.297f)	return 31;
			if (fSlope >=  1.871f)	return 30;
			if (fSlope >=  1.219f)	return 29;
			if (fSlope >=  0.821f)	return 28;
			if (fSlope >=  0.535f)	return 27;
			if (fSlope >=  0.303f)	return 26;
			if (fSlope >=  0.098f)	return 25;
			if (fSlope >=  0.000f)	return 24;		
		}
	}
	else // 2,4
	{
		fSlope = -fSlope;
		if (nDeltaX >= 0) // 4
		{
			if (fSlope >= 10.153f)	return  0;
			if (fSlope >=  3.297f)	return  1;
			if (fSlope >=  1.871f)	return  2;
			if (fSlope >=  1.219f)	return  3;
			if (fSlope >=  0.821f)	return  4;
			if (fSlope >=  0.535f)	return  5;
			if (fSlope >=  0.303f)	return  6;
			if (fSlope >=  0.098f)	return  7;
			if (fSlope >=  0.000f)	return  8;
		}
		else // 2
		{
			if (fSlope >= 10.153f)	return 16;
			if (fSlope >=  3.297f)	return 17;
			if (fSlope >=  1.871f)	return 18;
			if (fSlope >=  1.219f)	return 19;
			if (fSlope >=  0.821f)	return 20;
			if (fSlope >=  0.535f)	return 21;
			if (fSlope >=  0.303f)	return 22;
			if (fSlope >=  0.098f)	return 23;
			if (fSlope >=  0.000f)	return 24;		
		}
	}
	return 0;
}


//	+	|  -
//	3	|  4
// ------------> x
//	2	|  1
//	-	|  +
//		v  y
// 根据范围区间模糊确定(传入TILE中心点坐标,受45度倾斜的影响)
template<class T>
int Fun_CalcDirection32_45(T* pptFrom, T* pptTo)
{
	int nDeltaX = pptTo->x - pptFrom->x;
	int nDeltaY = pptTo->y - pptFrom->y;
	float fSlope;
	if (nDeltaX == 0)
		return (nDeltaY >= 0) ? 16 : 0;
	else
		fSlope = (float)nDeltaY*2/nDeltaX;
	
	if (fSlope >= 0.0f) // 1,3区间
	{
		if (nDeltaX >= 0) // 1
		{
			if (fSlope >= 10.153f)	return 16; // 90	(84.375 ～ 90.000) < tan(84.375) = 10.153 >
			if (fSlope >=  3.297f)	return 15; // 78.75	(73.125 ～ 84.375) < tan(73.125) =  3.297 >
			if (fSlope >=  1.871f)	return 14; // 67.50	(61.875 ～ 73.125) < tan(61.875) =  1.871 >
			if (fSlope >=  1.219f)	return 13; // 56.25	(50.625 ～ 61.875) < tan(50.625) =  1.219 >
			if (fSlope >=  0.821f)	return 12; // 45.00	(39.375 ～ 50.625) < tan(39.375) =  0.821 >
			if (fSlope >=  0.535f)	return 11; // 33.75	(28.125 ～ 39.375) < tan(28.125) =  0.535 >
			if (fSlope >=  0.303f)	return 10; // 22.50	(16.875 ～ 28.125) < tan(16.875) =  0.303 >
			if (fSlope >=  0.098f)	return  9; // 11.25	(05.625 ～ 16.875) < tan(05.625) =  0.098 >
			if (fSlope >=  0.000f)	return  8; // 0		(00.000 ～ 05.625) < tan(00.000) =  0.000 >
		}
		else // 3
		{
			if (fSlope >= 10.153f)	return 0;
			if (fSlope >=  3.297f)	return 31;
			if (fSlope >=  1.871f)	return 30;
			if (fSlope >=  1.219f)	return 29;
			if (fSlope >=  0.821f)	return 28;
			if (fSlope >=  0.535f)	return 27;
			if (fSlope >=  0.303f)	return 26;
			if (fSlope >=  0.098f)	return 25;
			if (fSlope >=  0.000f)	return 24;		
		}
	}
	else // 2,4
	{
		fSlope = -fSlope;
		if (nDeltaX >= 0) // 4
		{
			if (fSlope >= 10.153f)	return  0;
			if (fSlope >=  3.297f)	return  1;
			if (fSlope >=  1.871f)	return  2;
			if (fSlope >=  1.219f)	return  3;
			if (fSlope >=  0.821f)	return  4;
			if (fSlope >=  0.535f)	return  5;
			if (fSlope >=  0.303f)	return  6;
			if (fSlope >=  0.098f)	return  7;
			if (fSlope >=  0.000f)	return  8;
		}
		else // 2
		{
			if (fSlope >= 10.153f)	return 16;
			if (fSlope >=  3.297f)	return 17;
			if (fSlope >=  1.871f)	return 18;
			if (fSlope >=  1.219f)	return 19;
			if (fSlope >=  0.821f)	return 20;
			if (fSlope >=  0.535f)	return 21;
			if (fSlope >=  0.303f)	return 22;
			if (fSlope >=  0.098f)	return 23;
			if (fSlope >=  0.000f)	return 24;		
		}
	}
	return 0;
}
//------------------------------------------------------------------------
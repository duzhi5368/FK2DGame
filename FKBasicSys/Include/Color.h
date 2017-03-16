/**
*	created:		2012-7-2   1:10
*	filename: 		Color
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		ÑÕÉ«×ª»»
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#define v 255.0f
//------------------------------------------------------------------------
class CColor
{
public:
	void HSLtoRGB(int hue, int sat, int light, BYTE &r, BYTE &g, BYTE &b)
	{
		double h = hue / 360.0, s = sat / v, l = light / v;
		double red, green, blue;
		double t1, t2, tr, tg, tb;
		static const double v13 = 1 / 3.0;
		
		if (s == 0) red = green = blue = l;
		else {
			if (l < 0.5) t2 = l * (1 + s);
			else t2 = l + s - l * s;
			t1 = 2 * l - t2;
			tr = h + v13; if (tr > 1) tr -= 1;
			tg = h; 
			tb = h - v13; if (tb < 0) tb += 1;
			red   = rgb(t1, t2, tr);
			green = rgb(t1, t2, tg);
			blue  = rgb(t1, t2, tb);
		}
		r = (BYTE)(red   * 255 + 0.5);
		g = (BYTE)(green * 255 + 0.5); 
		b = (BYTE)(blue  * 255 + 0.5); 
	}
	void RGBtoHSL(BYTE r, BYTE g, BYTE b, UINT &hue, UINT &sat, UINT &light)
	{
		double red = r / 255.0, green = g / 255.0, blue = b / 255.0;
		double h, s, l, maxCol, minCol, difCol, sumCol;
		
		maxCol = max(red, max(green, blue));
		minCol = min(red, min(green, blue));
		difCol = maxCol - minCol;
		sumCol = maxCol + minCol;
		l = sumCol / 2;
		if (maxCol == minCol) s = h = 0;
		else {
			if (l < 0.5) s = difCol / sumCol;
			else s = difCol / (2 - sumCol);
			if (red == maxCol) h = (green - blue) / difCol;
			else if (green == maxCol) h = 2 + (blue - red) / difCol;
			else if (blue == maxCol) h = 4 + (red - green) / difCol;
		}
		hue = (int)(h * 60 + 0.5); if (hue < 0) hue += 360;
		sat = (int)(s * v + 0.5);
		light = (int)(l * v + 0.5);
	}
private:
	double rgb(double t1, double t2, double t3)
	{
		double col;
		static const double v23 = 2 / 3.0;
		
		if (6 * t3 <1) col = t1 + (t2 - t1) * 6 * t3;
		else if (2 * t3 < 1) col = t2;
		else if (3 * t3 < 2) col = t1 + (t2 -t1) * (v23 - t3) * 6;
		else col = t1;
		
		return col;
	}
};
//------------------------------------------------------------------------
/**
*	created:		2012-6-24   20:09
*	filename: 		PrintScreen
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../../../Depend/ijl/ijl.h"
#pragma comment(lib,"../../Depend/ijl/ijl15.lib")
//------------------------------------------------------------------------
BOOL SaveJpg(IBitmap* pBmp, const char* szFileName, long lQuality)
{
	if (!pBmp)
		return FALSE;

	int w = pBmp->GetWidth();
	int h = pBmp->GetHeight();

	BYTE* pSrc = new BYTE[w*h*3];

	BYTE* pTmp = pSrc;
	DWORD color;

	for(int i=0; i<h; i++)
	{
		for(int j=0; j<w; j++)
		{
			color = (DWORD)(*pBmp)[i][j];
			*(DWORD*)pTmp = color;
			pTmp += 3;
		}
	}

	JPEG_CORE_PROPERTIES jcprops;
	memset(&jcprops, 0, sizeof(jcprops));

	//Init the IJL
	if (ijlInit( &jcprops ) != IJL_OK)
		return FALSE;

	jcprops.DIBWidth  = w;
	jcprops.DIBHeight = h;
	jcprops.DIBChannels = 3;
	jcprops.DIBPadBytes = ((w * 24 + 31) / 32) * 4 - w * 3;
	jcprops.DIBColor  = IJL_BGR;
	jcprops.JPGFile   = szFileName;
	jcprops.JPGWidth  = w;
	jcprops.JPGHeight = h;
	jcprops.jquality  = lQuality;
	jcprops.JPGChannels = 3;
	jcprops.JPGColor = IJL_YCBCR;
	jcprops.JPGSubsampling = IJL_411;
	jcprops.DIBBytes = pSrc;			// Ô´Êý¾Ý
	if (ijlWrite(&jcprops, IJL_JFILE_WRITEWHOLEIMAGE ) != IJL_OK)
		return FALSE;

	ijlFree(&jcprops);

	if(pSrc != NULL)
		delete []pSrc;
	return TRUE;
};
//------------------------------------------------------------------------
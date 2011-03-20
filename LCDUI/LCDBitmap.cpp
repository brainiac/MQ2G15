//************************************************************************
//
// LCDBitmap.cpp
//
// The CLCDBitmap class draws bitmaps onto the LCD.
// 
// Logitech LCD SDK
//
// Copyright 2008 Logitech Inc.
//************************************************************************

#include "LCDUI.h"


//************************************************************************
//
// CLCDBitmap::CLCDBitmap
//
//************************************************************************

CLCDBitmap::CLCDBitmap(void)
{
    m_hBitmap = NULL;
    m_dwROP = SRCCOPY;
    m_fZoom = 1.0f;
    m_bAlpha = TRUE;

	m_pGdiPlusBitmap = NULL;

	m_rotate = 0;
	m_objectType = LG_BITMAP;
}


//************************************************************************
//
// CLCDBitmap::CLCDBitmap
//
//************************************************************************

CLCDBitmap::~CLCDBitmap(void)
{
	if (m_pGdiPlusBitmap != NULL)
	{
		delete m_pGdiPlusBitmap;
		m_pGdiPlusBitmap = NULL;
	}

}


//************************************************************************
//
// CLCDBitmap::SetBitmap
//
//************************************************************************

void CLCDBitmap::SetBitmap(HBITMAP hBitmap)
{
    m_hBitmap = hBitmap;
}


//************************************************************************
//
// CLCDBitmap::GetBitmap
//
//************************************************************************
HBITMAP CLCDBitmap::GetBitmap(void)
{
    return m_hBitmap;
}


//************************************************************************
//
// CLCDBitmap::SetBitmap
//
//************************************************************************

void CLCDBitmap::SetROP(DWORD dwROP)
{
    m_dwROP = dwROP;
}


//************************************************************************
//
// CLCDBitmap::SetZoomLevel
//
//************************************************************************

void CLCDBitmap::SetZoomLevel(float fzoom)
{
    m_fZoom = fzoom; 
}


//************************************************************************
//
// CLCDBitmap::GetZoomLevel
//
//************************************************************************

float CLCDBitmap::GetZoomLevel(void)
{
    return m_fZoom; 
}


//************************************************************************
//
// CLCDBitmap::SetAlpha
//
//************************************************************************

void CLCDBitmap::SetAlpha(BOOL bAlpha)
{
    m_bAlpha = bAlpha;
}


//************************************************************************
//
// CLCDBitmap::OnDraw
//
// The logical size is our 'canvas'.
// The control size is our 'window'. The window size can be smaller than the canvas.
// The assumption is that the bitmap size is the size of the canvas.
//************************************************************************

void CLCDBitmap::OnDraw(CLCDGfxBase &rGfx)
{
    if(m_hBitmap)
    {
        HDC hCompatibleDC = CreateCompatibleDC(rGfx.GetHDC());
        HBITMAP hOldBitmap = (HBITMAP)SelectObject(hCompatibleDC, m_hBitmap);
        
        // If monochrome output, don't even bother with alpha blend
        if (LGLCD_BMP_FORMAT_160x43x1 == rGfx.GetLCDScreen()->hdr.Format)
        {
            //BitBlt(rGfx.GetHDC(), 0, 0, m_sizeLogical.cx, m_sizeLogical.cy, hCompatibleDC, 0, 0, m_dwROP);
			RotateBlit(rGfx.GetHDC(), 0, 0, m_sizeLogical.cx, m_sizeLogical.cy, hCompatibleDC, 0, 0, m_dwROP);
        }
        else
        {
            if(0.001f > fabs(1.0f - m_fZoom))
            {
                BOOL b = FALSE;
                if(m_bAlpha)
                {
                    BLENDFUNCTION opblender = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
                    b = AlphaBlend(rGfx.GetHDC(), 0, 0, m_sizeLogical.cx, m_sizeLogical.cy, hCompatibleDC, 0, 0, m_sizeLogical.cx, m_sizeLogical.cy, opblender);
                }
                else
                {
                    BitBlt(rGfx.GetHDC(), 0, 0, m_sizeLogical.cx, m_sizeLogical.cy, hCompatibleDC, 0, 0, m_dwROP);
                }
            }
            else
            {
                if(m_bAlpha)
                {
                    BLENDFUNCTION opblender = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
                    AlphaBlend(rGfx.GetHDC(), 0, 0, (int)(m_fZoom* m_sizeLogical.cx), (int)(m_fZoom*m_sizeLogical.cy), hCompatibleDC, 0, 0, m_sizeLogical.cx, m_sizeLogical.cy, opblender);
                }
                else
                {
                    BLENDFUNCTION opblender = {AC_SRC_OVER, 0, 255, 0};
                    AlphaBlend(rGfx.GetHDC(), 0, 0, (int)(m_fZoom* m_sizeLogical.cx), (int)(m_fZoom*m_sizeLogical.cy), hCompatibleDC, 0, 0, m_sizeLogical.cx, m_sizeLogical.cy, opblender);
                }
            }
        }
        
        // restores
        SelectObject(hCompatibleDC, hOldBitmap);
        DeleteDC(hCompatibleDC);
    }
}

/* added by brainiac */
void CLCDBitmap::RotateBlit(HDC destDC, int x1, int y1, int x2, int y2, HDC srcDC, int dx1, int dy1, DWORD mode)
{
	double theta = (m_rotate + 90) * (3.14159 / 180);

	int w = x2 - x1;
	int h = y2 - y1;

	if (w > h) h = w;
	else w = h;

	double cx = ((double)x2 + (double)x1) / 2.0;
	double cy = ((double)y2 + (double)y1) / 2.0;

	HDC memDC = CreateCompatibleDC(destDC);
	HBITMAP memBmp = CreateCompatibleBitmap(destDC, w, h);
	HBITMAP obmp = (HBITMAP)SelectObject(memDC, memBmp);

	double c = (double)w / 2.0;

	for (int x = x1; x <= x2; x++) 
	{
		for (int y = y1; y <= y2; y++) 
		{
			COLORREF color = GetPixel(srcDC, x, y);

			double nx = ((double)x - c) * sin(theta) - ((double)y - c) * cos(theta);
			double ny = ((double)x - c) * cos(theta) + ((double)y - c) * sin(theta);

			SetPixel(memDC, nx + c + 0.5, ny + c + 0.5, color);
		}
	}

	BitBlt(destDC, dx1, dy1, w, h, memDC, 0, 0, mode);
	SelectObject(memDC, obmp);

	DeleteDC(memDC);
	DeleteObject(memBmp);
}

bool CLCDBitmap::LoadFromResource(HINSTANCE hInstance, DWORD nResourceID, LPCTSTR sResourceType)
{
	// Clear the old bitmap
	SetSize(0, 0);
	if (m_pGdiPlusBitmap != NULL)
	{
		delete m_pGdiPlusBitmap;
		m_pGdiPlusBitmap = NULL;
	}

	HRSRC hResource = FindResource(hInstance, MAKEINTRESOURCE(nResourceID), sResourceType);
	if(NULL == hResource)
	{
		TRACE(_T("CBitmap::LoadImageFromResource(): failed to locate resource 0x%x (type: 0x%x) in instance 0x%x.\n"),
			nResourceID, sResourceType, hInstance);
		return E_FAIL;
	}

	DWORD dwImageSize = SizeofResource(hInstance, hResource);
	if (0 == dwImageSize)
	{
		return E_FAIL;
	}

	const void* pResourceData = LockResource(LoadResource(hInstance, hResource));
	if (NULL == pResourceData)
	{
		return E_FAIL;
	}

	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_DISCARDABLE, dwImageSize);
	if (NULL == hGlobal)
	{
		return E_FAIL;
	}

	void* pBuffer = GlobalLock(hGlobal);
	if (NULL == pBuffer)
	{
		GlobalFree(hGlobal);
		return E_FAIL;
	}

	CopyMemory(pBuffer, pResourceData, dwImageSize);

	// CreateStreamOnHGlobal requires HGLOBAL to be GlobalAlloc, moveable, and discardable 
	if(NULL == hGlobal)
	{
		TRACE(_T("cBitmap::LoadImageFromResource(): failed to load resource 0x%x (type: 0x%x) in instance 0x%x.\n"),
			nResourceID, sResourceType, hInstance);
		GlobalFree(hGlobal);
		return E_FAIL;
	}

	// Create an IStram from the HGLOBAL
	IStream* pStream = NULL;
	DWORD dwRet = S_OK;
	dwRet = ::CreateStreamOnHGlobal(hGlobal, FALSE, &pStream);
	if (FAILED(dwRet))
	{
		TRACE(_T("ERROR: CBitmap::LoadImageFromResource CreateStreamOnHGlobal returned %d\n"), dwRet);
		GlobalFree(hGlobal);
		return dwRet;
	}

	// Use GDI+ to generate a bitmap
	m_pGdiPlusBitmap = Gdiplus::Bitmap::FromStream(pStream);
	pStream->Release();
	if (!m_pGdiPlusBitmap)
	{
		TRACE(_T("ERROR: CBitmap::LoadImageFromResource Gdiplus::Bitmap::FromStream failed\n"));
		GlobalFree(hGlobal);
		return E_FAIL;
	}

	// We don;t need the global memory block anymore
	GlobalFree(hGlobal);
	hGlobal = NULL;

	// Get the bitmap handle
	HBITMAP hBitmap = NULL;
	if (m_pGdiPlusBitmap->GetHBITMAP(RGB(0,0,0), &hBitmap) != Gdiplus::Ok)
	{
		TRACE(_T("ERROR: CBitmap::LoadImageFromResource Gdiplus::Bitmap::GetHBITMAP failed\n"));
		if (m_pGdiPlusBitmap)
		{
			delete m_pGdiPlusBitmap;
			m_pGdiPlusBitmap = NULL;
		}
		return E_FAIL;
	}

	// Get the bitmap dimensions
	BITMAP bmpInfo;
	ZeroMemory(&bmpInfo, sizeof(bmpInfo));
	::GetObject(hBitmap, sizeof(BITMAP), &bmpInfo);

	SetSize(bmpInfo.bmWidth, bmpInfo.bmHeight);

	m_hBitmap = hBitmap;

	return S_OK;
}

//** end of LCDBitmap.cpp ************************************************

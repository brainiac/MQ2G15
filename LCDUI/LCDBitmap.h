//************************************************************************
//
// LCDBitmap.h
//
// The CLCDBitmap class draws bitmaps onto the LCD.
// 
// Logitech LCD SDK
//
// Copyright 2008 Logitech Inc.
//************************************************************************

#ifndef _LCDBITMAP_H_INCLUDED_ 
#define _LCDBITMAP_H_INCLUDED_ 

#include "LCDBase.h"
#include <GdiPlus.h>

class CLCDBitmap : public CLCDBase
{
public:
    CLCDBitmap();
    virtual ~CLCDBitmap(); 

    // CLCDBase
    virtual void OnDraw(CLCDGfxBase &rGfx);

    void SetBitmap(HBITMAP hBitmap);
    HBITMAP GetBitmap(void);
    void SetROP(DWORD dwROP);
    void SetZoomLevel(float fzoom);
    float GetZoomLevel(void);
    void SetAlpha(BOOL bAlpha);

	void SetRotation(DWORD Angle) { m_rotate = Angle; }
	DWORD GetRotation() { return m_rotate; }

	bool LoadFromResource(HINSTANCE hInstance, DWORD nResourceID, LPCTSTR sResourceType);
	//HRESULT LoadFromFile(LPCTSTR sFilename);
	//HRESULT LoadFromMemory(LPVOID pBuffer, size_t nBufferSize);

protected:   
    HBITMAP m_hBitmap;
    DWORD   m_dwROP;
    float   m_fZoom;
    // this indicates the bitmap has an alpha channel
    BOOL    m_bAlpha;
	DWORD	m_rotate;

	Gdiplus::Bitmap* m_pGdiPlusBitmap;

private:
	void RotateBlit(HDC destDC, int x1, int y1, int x2, int y2, HDC srcDC, int dx1, int dy1, DWORD mode);
};


#endif // !_LCDBITMAP_H_INCLUDED_ 

//** end of LCDBitmap.h **************************************************

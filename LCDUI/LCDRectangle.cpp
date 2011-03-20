//************************************************************************
//
// LCDRectangle.cpp
//
// The CLCDRectangle class draws a rectangle to the LCD
// 
// First Appearing in MQ2G15
//
// Copyright (c) 2008 brainiac
//************************************************************************

#include "LCDUI.h"

//************************************************************************

CLCDRectangle::CLCDRectangle()
{
	m_objectType = LG_RECTANGLE;
	Initialize();
}

//************************************************************************

CLCDRectangle::~CLCDRectangle()
{	

}

//************************************************************************

HRESULT CLCDRectangle::Initialize()
{
	m_hBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);

	return CLCDBase::Initialize();
}

//************************************************************************

void CLCDRectangle::OnDraw(CLCDGfxBase &rGfx)
{
	RECT r;
	r.left = m_Origin.x;
	r.top = m_Origin.y;
	r.right = m_Size.cx - m_Origin.x;
	r.bottom = m_Size.cy - m_Origin.y;

	FillRect(rGfx.GetHDC(), &r, m_hBrush);
}

//************************************************************************

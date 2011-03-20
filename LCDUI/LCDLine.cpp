//************************************************************************
//
// LCDLine.cpp
//
// The CLCDLine class draws a simple line onto the LCD.
// 
// First Appearing in MQ2G15
//
// Copyright 2008 Stephen Raub
//
//************************************************************************

#include "LCDUI.h"

//************************************************************************

CLCDLine::CLCDLine()
{
	ZeroMemory(&m_point1, sizeof(POINT));
	ZeroMemory(&m_point2, sizeof(POINT));

	m_objectType = LG_LINE;
}

//************************************************************************

CLCDLine::~CLCDLine()
{
}

//************************************************************************

void CLCDLine::OnDraw(CLCDGfxBase &rGfx)
{
	HDC hDC = rGfx.GetHDC();
	HPEN hPrevPen = (HPEN)SelectObject(hDC, GetStockObject(WHITE_PEN));
	::MoveToEx(hDC, m_point1.x, m_point1.y, NULL);
	::LineTo(hDC, m_point2.x, m_point2.y);
	SelectObject(hDC, hPrevPen);
}

//************************************************************************

void CLCDLine::SetLine(int x1, int y1, int x2, int y2)
{
	SetOrigin(min(x1, x2), min(y1, y2));
	SetSize(max(x1, x2) - m_Origin.x + 1, max(y1, y2) - m_Origin.y + 1);

	m_point1.x = x1 - m_Origin.x;
	m_point1.y = y1 - m_Origin.y;

	m_point2.x = x2 - m_Origin.x;
	m_point2.y = y2 - m_Origin.y;
}

//************************************************************************

void CLCDLine::GetLine(int& x1, int& y1, int& x2, int& y2)
{
	x1 = m_point1.x + m_Origin.x;
	y1 = m_point1.y + m_Origin.y;
	x2 = m_point2.x + m_Origin.x;
	y2 = m_point2.y + m_Origin.y;
}

//************************************************************************

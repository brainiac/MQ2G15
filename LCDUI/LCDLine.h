//************************************************************************
//
// LCDLine.h
//
// The CLCDLine class draws a simple line onto the LCD.
// 
// First Appearing in MQ2G15
//
// Copyright 2008 Stephen Raub
//
//************************************************************************

#ifndef __LCDLINE_H__
#define __LCDLINE_H__

#include "LCDBase.h"

class CLCDLine : public CLCDBase
{

public:
	CLCDLine();
	virtual ~CLCDLine();

	virtual void SetLine(int x1, int y1, int x2, int y2);
	virtual void GetLine(int& x1, int& y1, int& x2, int& y2);

protected:
	POINT m_point1;
	POINT m_point2;

	virtual void OnDraw(CLCDGfxBase &rGfx);
};

#endif // __LCDLINE_H__
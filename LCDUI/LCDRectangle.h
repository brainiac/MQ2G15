//************************************************************************
//
// LCDText.h
//
// The CLCDFont class draws simple text onto the LCD.
// 
// First Appearing in MQ2G15
//
// Copyright 2008 Stephen Raub
//************************************************************************

#ifndef _LCDRECTANGLE_H_INCLUDED_
#define _LCDRECTANGLE_H_INCLUDED_

class CLCDRectangle : public CLCDBase
{

public:
	CLCDRectangle();
	virtual ~CLCDRectangle();

	virtual HRESULT Initialize();

protected:

	virtual void OnDraw(CLCDGfxBase &rGfx);

	HBRUSH m_hBrush;
};

#endif
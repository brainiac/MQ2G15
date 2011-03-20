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

#ifndef _LCDFONT_H_INCLUDED_
#define _LCDFONT_H_INCLUDED_

#include "LCDBitmap.h"

enum LCD_FontSize
{
	Font_6x4,
	Font_7x5,
	Font_8x8
};

class CLCDFont : public CLCDBitmap
{

public:
	CLCDFont();
	virtual ~CLCDFont();

	virtual void SetText(const char* text);
	virtual const char* GetText();
	virtual void SetFontSize(enum LCD_FontSize fontSize);
	virtual enum LCD_FontSize GetFontSize();

	virtual void Invert(bool value);

protected:
	enum { MAX_TEXT = 256 };

protected:

	virtual void OnDraw(CLCDGfxBase &rGfx);

	void DrawCharacter(HDC hDC, unsigned char c, int x, int y, LCD_FontSize size);

	bool m_invalidated;
	char m_text[MAX_TEXT];
	int m_length;
	COLORREF m_crColor;

	enum LCD_FontSize m_fontSize;

};

#endif
//************************************************************************
//
// LCDText.cpp
//
// The CLCDFont class draws simple text onto the LCD.
// 
// First Appearing in MQ2G15
//
// Copyright (c) 2008 brainiac
//************************************************************************

#include "LCDUI.h"

#include "../FontData6x4.h"
#include "../FontData7x5.h"
#include "../FontData8x8.h"

//************************************************************************

CLCDFont::CLCDFont()
{
	ZeroMemory(m_text, sizeof(m_text));	
	m_crColor = RGB(255, 255, 255);
	m_hBitmap = NULL;
	m_fontSize = Font_8x8;
	m_invalidated = true;

	m_objectType = LG_FONT;
}

//************************************************************************

CLCDFont::~CLCDFont()
{
	if (m_hBitmap != NULL)
	{
		DeleteObject(m_hBitmap);
	}
}

//************************************************************************

void CLCDFont::DrawCharacter(HDC hDC, unsigned char c, int sx, int sy, LCD_FontSize size)
{
	int idx;

	switch (size)
	{
	case Font_6x4:
		idx = c * 6 * 4;
		for (int y = 0; y < 6; ++y)
		{
			for (int x = 0; x < 4; ++x)
			{
				unsigned char data = fontdata_6x4[idx + y * 4 + x];
				if ((data && !m_bInverted) || (!data && m_bInverted))
				{
					SetPixel(hDC, x + sx, y + sy, m_crColor);
				}
			}
		}
		break;

	case Font_7x5:
		idx = c * 7 * 5;
		for (int y = 0; y < 7; ++y)
		{
			for (int x = 0; x < 5; ++x)
			{
				unsigned char data = fontdata_7x5[idx + y * 5 + x];
				if ((data && !m_bInverted) || (!data && m_bInverted))
				{
					SetPixel(hDC, x + sx, y + sy, m_crColor);
				}
			}
		}
		break;

	case Font_8x8:
		idx = c * 8;
		for (int y = 0; y < 8; ++y)
		{
			for (int x = 0; x < 8; ++x)
			{
				unsigned char data = fontdata_8x8[idx + y] & 1 << (7 - x);
				if ((data && !m_bInverted) || (!data && m_bInverted))
				{
					SetPixel(hDC, x + sx, y + sy, m_crColor);
				}
			}
		}
		break;
	}	
}

void CLCDFont::OnDraw(CLCDGfxBase &rGfx)
{
	if (m_invalidated && m_length > 0)
	{
		// first clean up old bitmap if it exists
		if (m_hBitmap != NULL)
		{
			DeleteObject(m_hBitmap);
		}

		// calculate the dimensions in pixels of the new bitmap
		int width = 0;
		int height = 0;
		int dh, dw;

		switch (m_fontSize)
		{
		case Font_6x4:
			dw = 4;
			dh = 6;
			break;

		case Font_7x5:
			dw = 5;
			dh = 7;
			break;

		case Font_8x8:
			dw = 8;
			dh = 8;
			break;

		default:
			ASSERT(false);
		}

		width = dw * m_length;
		height = dh;

		m_hBitmap = CreateCompatibleBitmap(rGfx.GetHDC(), width, height);
		ASSERT(m_hBitmap != NULL);

		HDC hDC = CreateCompatibleDC(rGfx.GetHDC());
		HBITMAP hOldBitmap = (HBITMAP)SelectObject(hDC, m_hBitmap);

		// render each character one at a time
		for (int i = 0; i < m_length; i++)
		{
			int xpos = i * dw;
			DrawCharacter(hDC, (unsigned char)m_text[i], xpos, 0, m_fontSize);
		}		
		
		SelectObject(hDC, hOldBitmap);
		DeleteDC(hDC);

		SetSize(width, height);
		m_invalidated = false;
	}

	CLCDBitmap::OnDraw(rGfx);
}

//************************************************************************

void CLCDFont::SetText(const char* text)
{
	ASSERT(text != NULL);
	if (text && strcmp(text, m_text))
	{
		strncpy(m_text, text, MAX_TEXT);
		m_length = strlen(text);

		m_invalidated = true;
	}	
}

//************************************************************************

const char* CLCDFont::GetText()
{
	return m_text;
}

//************************************************************************

void CLCDFont::Invert(bool value)
{
	CLCDBase::Invert(value);
	m_invalidated = true;
}

//************************************************************************

void CLCDFont::SetFontSize(enum LCD_FontSize fontSize)
{
	if (m_fontSize != fontSize)
	{
		m_fontSize = fontSize;
		m_invalidated = true;
	}
};

//************************************************************************

enum LCD_FontSize CLCDFont::GetFontSize()
{
	return m_fontSize;
};
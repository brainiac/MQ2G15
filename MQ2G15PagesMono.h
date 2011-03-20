#pragma once

#include "MQ2G15Pages.h"

class CLCDBitmap;
class CLCDText;
class CLCDStreamingText;
class CLCDProgressBar;
class CLCDFont;
class CLCDRectangle;
class CLCDLine;

// ----------------------------------------------------------------------------
// constants

enum LGTextSize
{
	LG_SMALL, 
	LG_MEDIUM, 
	LG_BIG
};

enum LGProgressBarType
{
	LG_CURSOR, 
	LG_FILLED
};

// text box height for various font sizes
const INT LG_SMALL_FONT_TEXT_BOX_HEIGHT = 12;
const INT LG_MEDIUM_FONT_TEXT_BOX_HEIGHT = 13;
const INT LG_BIG_FONT_TEXT_BOX_HEIGHT = 20;

// corresponding font size
const INT LG_SMALL_FONT_SIZE = 7;
const INT LG_MEDIUM_FONT_SIZE = 8;
const INT LG_BIG_FONT_SIZE = 12;

// logical origin Y value for various font sizes
const INT LG_SMALL_FONT_LOGICAL_ORIGIN_Y = -3;
const INT LG_MEDIUM_FONT_LOGICAL_ORIGIN_Y = -2;
const INT LG_BIG_FONT_LOGICAL_ORIGIN_Y = -4;

// Scrolling text parameters
const INT LG_SCROLLING_SPEED = 7;
const INT LG_SCROLLING_STEP = 7;
const INT LG_SCROLLING_DELAY_MS = 2000;
const TCHAR LG_SCROLLING_GAP_TEXT[] = _T("       ");

// Progress bar parameters
const INT LG_PROGRESS_BAR_RANGE_MIN = 0;
const INT LG_PROGRESS_BAR_RANGE_MAX = 100;
const INT LG_PROGRESS_BAR_INITIAL_HEIGHT = 5;

// Size of the LCD
const INT LG_LCD_WIDTH = 160;
const INT LG_LCD_HEIGHT = 43;



class CStaticDemoState : public CPageState
{	
	// combat status bitmaps
	CLCDBitmap* m_bmpHourGlass;
	HBITMAP m_hHourGlass;

	CLCDBitmap* m_bmpOoc;
	HBITMAP m_hOoc;

	CLCDBitmap* m_bmpResting;
	HBITMAP m_hResting;

	CLCDBitmap* m_bmpSwords;
	HBITMAP m_hSwords;

	CLCDBitmap* m_bmpWaterDrop;
	HBITMAP m_hWaterDrop;

	CLCDBitmap* m_bmpPlaceHolder;
	HBITMAP m_hPlaceHolder;

	CLCDBitmap* m_bmpCompass;
	HBITMAP m_hCompass;

	// InGame mode things
	CLCDFont* m_name;

	CLCDFont* m_statusText;

	CLCDFont* m_hpText;
	CLCDFont* m_mpText;
	CLCDFont* m_epText;
	CLCDProgressBar* m_hpMeter;
	CLCDFont* m_hpPct;
	CLCDProgressBar* m_mpMeter;
	CLCDFont* m_mpPct;
	CLCDProgressBar* m_epMeter;
	CLCDFont* m_epPct;

	CLCDLine* m_topLine1;
	CLCDLine* m_topLine2;

	CLCDRectangle* m_bottomBar;

	CLCDFont* m_xpText;

public:

	CStaticDemoState();
	virtual ~CStaticDemoState();

	virtual void Init();
	virtual void Update();
	string GetName();

	virtual void Activate();
};

class CXmlState : public CPageState
{
	class XmlPageLayout* m_layout;
	vector<struct XmlPageElement*> m_elemdata;

public:
	CXmlState(class XmlPageLayout* layout);
	virtual ~CXmlState();

	virtual void Init();
	virtual void Update();
	virtual string GetName();
};

class CRestrictedState : public CPageState
{
	CLCDStreamingText* m_text;

public:
	virtual void Init();
	virtual string GetName();
	virtual void Update();
};

class CTestState : public CPageState
{
	CLCDFont* m_test;

public:
	virtual void Init();
	virtual string GetName();
	virtual void Update();
};

class CNullState : public CPageState
{
public:
	virtual void Init() {}
	virtual string GetName() { return "Null"; }
};
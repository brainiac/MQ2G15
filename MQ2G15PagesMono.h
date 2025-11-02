#pragma once

#include "MQ2G15Pages.h"

class CLCDBitmap;
class CLCDText;
class CLCDStreamingText;
class CLCDProgressBar;
class CLCDFont;
class CLCDRectangle;
class CLCDLine;

struct XmlPageElement;
class XmlPageLayout;

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
inline constexpr int LG_SMALL_FONT_TEXT_BOX_HEIGHT = 12;
inline constexpr int LG_MEDIUM_FONT_TEXT_BOX_HEIGHT = 13;
inline constexpr int LG_BIG_FONT_TEXT_BOX_HEIGHT = 20;

// corresponding font size
inline constexpr int LG_SMALL_FONT_SIZE = 7;
inline constexpr int LG_MEDIUM_FONT_SIZE = 8;
inline constexpr int LG_BIG_FONT_SIZE = 12;

// logical origin Y value for various font sizes
constexpr int LG_SMALL_FONT_LOGICAL_ORIGIN_Y = -3;
constexpr int LG_MEDIUM_FONT_LOGICAL_ORIGIN_Y = -2;
constexpr int LG_BIG_FONT_LOGICAL_ORIGIN_Y = -4;

// Scrolling text parameters
inline constexpr int LG_SCROLLING_SPEED = 7;
inline constexpr int LG_SCROLLING_STEP = 7;
inline constexpr int LG_SCROLLING_DELAY_MS = 2000;
inline constexpr TCHAR LG_SCROLLING_GAP_TEXT[] = _T("       ");

// Progress bar parameters
inline constexpr int LG_PROGRESS_BAR_RANGE_MIN = 0;
inline constexpr int LG_PROGRESS_BAR_RANGE_MAX = 100;
inline constexpr int LG_PROGRESS_BAR_INITIAL_HEIGHT = 5;

// Size of the LCD
inline constexpr int LG_LCD_WIDTH = 160;
inline constexpr int LG_LCD_HEIGHT = 43;

class CStaticDemoState : public CPageState
{
	// combat status bitmaps
	CLCDBitmap* m_bmpHourGlass = nullptr;
	HBITMAP m_hHourGlass = nullptr;

	CLCDBitmap* m_bmpOoc = nullptr;
	HBITMAP m_hOoc = nullptr;

	CLCDBitmap* m_bmpResting = nullptr;
	HBITMAP m_hResting = nullptr;

	CLCDBitmap* m_bmpSwords = nullptr;
	HBITMAP m_hSwords = nullptr;

	CLCDBitmap* m_bmpWaterDrop = nullptr;
	HBITMAP m_hWaterDrop = nullptr;

	CLCDBitmap* m_bmpCompass = nullptr;
	HBITMAP m_hCompass = nullptr;

	// InGame mode things
	CLCDFont* m_nameLabel = nullptr;

	CLCDFont* m_statusText = nullptr;

	CLCDFont* m_hpText = nullptr;
	CLCDFont* m_mpText = nullptr;
	CLCDFont* m_epText = nullptr;
	CLCDProgressBar* m_hpMeter = nullptr;
	CLCDFont* m_hpPct = nullptr;
	CLCDProgressBar* m_mpMeter = nullptr;
	CLCDFont* m_mpPct = nullptr;
	CLCDProgressBar* m_epMeter = nullptr;
	CLCDFont* m_epPct = nullptr;

	CLCDLine* m_topLine1 = nullptr;
	CLCDLine* m_topLine2 = nullptr;

	CLCDRectangle* m_bottomBar = nullptr;

	CLCDFont* m_xpText = nullptr;

public:
	CStaticDemoState();
	virtual ~CStaticDemoState() override;

	virtual void Init() override;
	virtual void Update() override;

	virtual void Activate() override;
};

class CXmlState : public CPageState
{
	XmlPageLayout* m_layout;
	std::vector<XmlPageElement*> m_elemData;

public:
	CXmlState(XmlPageLayout* layout);
	virtual ~CXmlState() override;

	virtual void Init() override;
	virtual void Update() override;
	virtual const std::string& GetName() const override;
};

class CTestState : public CPageState
{
	CLCDFont* m_test = nullptr;

public:
	virtual void Init() override;
	virtual void Update() override;
};

class CNullState : public CPageState
{
public:
	virtual void Init() override { SetName("Null"); }
};

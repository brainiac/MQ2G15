
#include "stdafx.h"

#include "MQ2G15.h"
#include "MQ2G15Config.h"
#include "MQ2G15PagesMono.h"
#include "MQ2G15Bitmap.h"


// Handle for our bitmap resources
extern HINSTANCE ghModuleInstance;

// ----------------------------------------------------------------------------
// helper functions
CLCDBase* CreateText(LGObjectType type, LGTextSize size, INT alignment, INT maxLengthPixels)
{
	ASSERT(LG_SCROLLING_TEXT == type || LG_STATIC_TEXT == type);

	CLCDText* staticText;
	CLCDStreamingText* streamingText;

	INT boxHeight = LG_MEDIUM_FONT_TEXT_BOX_HEIGHT;
	INT fontSize = LG_MEDIUM_FONT_SIZE;
	INT localOriginY = LG_MEDIUM_FONT_LOGICAL_ORIGIN_Y;

	switch (type)
	{
	case LG_SCROLLING_TEXT:
		streamingText = new CLCDStreamingText();

		streamingText->SetFontFaceName(_T("Microsoft Sans Serif"));
		streamingText->SetAlignment(alignment);
		streamingText->SetText(_T(" "));
		streamingText->SetGapText(LG_SCROLLING_GAP_TEXT);
		streamingText->SetSpeed(LG_SCROLLING_SPEED);
		streamingText->SetScrollingStep(LG_SCROLLING_STEP);
		streamingText->SetStartDelay(LG_SCROLLING_DELAY_MS);

		if (LG_SMALL == size)
		{
			boxHeight = LG_SMALL_FONT_TEXT_BOX_HEIGHT;
			fontSize = LG_SMALL_FONT_SIZE;
			localOriginY = LG_SMALL_FONT_LOGICAL_ORIGIN_Y;
		}
		else if (LG_MEDIUM == size)
		{
			boxHeight = LG_MEDIUM_FONT_TEXT_BOX_HEIGHT;
			fontSize = LG_MEDIUM_FONT_SIZE;
			localOriginY = LG_MEDIUM_FONT_LOGICAL_ORIGIN_Y;
		}
		else if (LG_BIG == size)
		{
			streamingText->SetFontWeight(FW_BOLD);
			boxHeight = LG_BIG_FONT_TEXT_BOX_HEIGHT;
			fontSize = LG_BIG_FONT_SIZE;
			localOriginY = LG_BIG_FONT_LOGICAL_ORIGIN_Y;
		}

		streamingText->SetSize(maxLengthPixels, boxHeight);		
		streamingText->SetFontPointSize(fontSize);
		streamingText->SetLogicalOrigin(0, localOriginY);

		return streamingText;

	case LG_STATIC_TEXT:
		staticText = new CLCDText();

		staticText->SetFontFaceName(_T("Microsoft Sans Serif"));
		staticText->SetAlignment(alignment);
		staticText->SetText(_T(" "));

		if (LG_SMALL == size)
		{
			boxHeight = LG_SMALL_FONT_TEXT_BOX_HEIGHT;
			fontSize = LG_SMALL_FONT_SIZE;
			localOriginY = LG_SMALL_FONT_LOGICAL_ORIGIN_Y;
		}
		else if (LG_MEDIUM == size)
		{
			boxHeight = LG_MEDIUM_FONT_TEXT_BOX_HEIGHT;
			fontSize = LG_MEDIUM_FONT_SIZE;
			localOriginY = LG_MEDIUM_FONT_LOGICAL_ORIGIN_Y;
		}
		else if (LG_BIG == size)
		{
			staticText->SetFontWeight(FW_BOLD);
			boxHeight = LG_BIG_FONT_TEXT_BOX_HEIGHT;
			fontSize = LG_BIG_FONT_SIZE;
			localOriginY = LG_BIG_FONT_LOGICAL_ORIGIN_Y;
		}

		staticText->SetSize(maxLengthPixels, boxHeight);
		staticText->SetFontPointSize(fontSize);
		staticText->SetLogicalOrigin(0, localOriginY);

		return staticText;
	}
	return NULL;
}

CLCDProgressBar* CreateProgressBar(LGProgressBarType type)
{
	ASSERT(type == LG_FILLED || type == LG_CURSOR);

	CLCDProgressBar *progressBar = new CLCDProgressBar();
	progressBar->SetSize(LG_LCD_WIDTH, LG_PROGRESS_BAR_INITIAL_HEIGHT);

	if (LG_FILLED == type)
	{
		progressBar->EnableCursor(FALSE);
	}
	return progressBar;
}

void SetLCDObjectText(CLCDBase* pObj, const char* text)
{
	if (pObj->GetObjectType() == LG_SCROLLING_TEXT)
	{
		CLCDStreamingText* pText = dynamic_cast<CLCDStreamingText*>(pObj);
		ASSERT(pText != NULL);

		pText->SetText(text);
	}
	else if (pObj->GetObjectType() == LG_STATIC_TEXT)
	{
		CLCDText* pText = dynamic_cast<CLCDText*>(pObj);
		ASSERT(pText != NULL);

		pText->SetText(text);
	}
	else if (pObj->GetObjectType() == LG_FONT)
	{
		CLCDFont* pText = dynamic_cast<CLCDFont*>(pObj);
		ASSERT(pText != NULL);

		pText->SetText(text);
	}
}

// ----------------------------------------------------------------------------
#pragma region CStaticDemoState class
void CStaticDemoState::Init()
{
	// create combat state bitmaps
	m_hHourGlass = (HBITMAP)LoadBitmap((HINSTANCE)ghModuleInstance, MAKEINTRESOURCE(IDB_HOURGLASS));
	m_bmpHourGlass = new CLCDBitmap();
	m_bmpHourGlass->SetSize(15, 15);
	m_bmpHourGlass->SetBitmap(m_hHourGlass);
	m_bmpHourGlass->Show(false);
	AddObject(m_bmpHourGlass);

	m_hOoc = (HBITMAP)LoadBitmap((HINSTANCE)ghModuleInstance, MAKEINTRESOURCE(IDB_OOC));
	m_bmpOoc = new CLCDBitmap();
	m_bmpOoc->SetSize(15, 15);
	m_bmpOoc->SetBitmap(m_hOoc);
	m_bmpOoc->Show(false);
	AddObject(m_bmpOoc);

	m_hResting = (HBITMAP)LoadBitmap((HINSTANCE)ghModuleInstance, MAKEINTRESOURCE(IDB_RESTING));
	m_bmpResting = new CLCDBitmap();
	m_bmpResting->SetSize(15, 15);
	m_bmpResting->SetBitmap(m_hResting);
	m_bmpResting->Show(false);
	AddObject(m_bmpResting);

	m_hSwords = (HBITMAP)LoadBitmap((HINSTANCE)ghModuleInstance, MAKEINTRESOURCE(IDB_SWORDS));
	m_bmpSwords = new CLCDBitmap();
	m_bmpSwords->SetSize(15, 15);
	m_bmpSwords->SetBitmap(m_hSwords);
	m_bmpSwords->Show(false);
	AddObject(m_bmpSwords);

	m_hWaterDrop = (HBITMAP)LoadBitmap((HINSTANCE)ghModuleInstance, MAKEINTRESOURCE(IDB_WATERDROP));
	m_bmpWaterDrop = new CLCDBitmap();
	m_bmpWaterDrop->SetSize(15, 15);
	m_bmpWaterDrop->SetBitmap(m_hWaterDrop);
	m_bmpWaterDrop->Show(false);
	AddObject(m_bmpWaterDrop);

	//m_hPlaceHolder = (HBITMAP)LoadBitmap((HINSTANCE)ghModuleInstance, MAKEINTRESOURCE(IDB_PLACEHOLDER));
	//m_bmpPlaceHolder = new CLCDBitmap();
	//m_bmpPlaceHolder->SetOrigin(102, 0);
	//m_bmpPlaceHolder->SetSize(58, 43);
	//m_bmpPlaceHolder->SetBitmap(m_hPlaceHolder);
	//AddObject(m_bmpPlaceHolder);

	m_hCompass = (HBITMAP)LoadBitmap((HINSTANCE)ghModuleInstance, MAKEINTRESOURCE(IDB_COMPASS));
	m_bmpCompass = new CLCDBitmap();
	m_bmpCompass->SetOrigin(122, 2);
	m_bmpCompass->SetSize(20, 20);
	m_bmpCompass->SetBitmap(m_hCompass);
	AddObject(m_bmpCompass);

	// create name
	m_name = new CLCDFont();
	m_name->SetOrigin(16, 0);
	m_name->SetFontSize(Font_8x8);
	m_name->SetText(GetCharInfo()->Name);
	AddObject(m_name);

	m_statusText = new CLCDFont();
	m_statusText->SetOrigin(16, 8);
	m_statusText->SetFontSize(Font_7x5);
	m_statusText->SetText("");
	AddObject(m_statusText);

	m_hpPct = new CLCDFont();
	m_hpPct->SetOrigin(0, 16);
	m_hpPct->SetFontSize(Font_6x4);
	AddObject(m_hpPct);

	m_hpMeter = CreateProgressBar(LG_FILLED);
	m_hpMeter->SetSize(64, 5);
	m_hpMeter->SetOrigin(16, 16);
	AddObject(m_hpMeter);

	m_mpPct = new CLCDFont();
	m_mpPct->SetOrigin(0, 22);
	m_mpPct->SetFontSize(Font_6x4);
	AddObject(m_mpPct);

	m_mpMeter = CreateProgressBar(LG_FILLED);
	m_mpMeter->SetSize(64, 5);
	m_mpMeter->SetOrigin(16, 22);
	AddObject(m_mpMeter);

	m_epPct = new CLCDFont();
	m_epPct->SetOrigin(0, 28);
	m_epPct->SetFontSize(Font_6x4);
	AddObject(m_epPct);

	m_epMeter = CreateProgressBar(LG_FILLED);
	m_epMeter->SetSize(64, 5);
	m_epMeter->SetOrigin(16, 28);
	AddObject(m_epMeter);

	m_hpText = new CLCDFont();
	m_hpText->SetFontSize(Font_6x4);
	m_hpText->SetOrigin(81, 16);
	AddObject(m_hpText);

	m_mpText = new CLCDFont();
	m_mpText->SetFontSize(Font_6x4);
	m_mpText->SetOrigin(81, 22);
	AddObject(m_mpText);
	
	m_epText = new CLCDFont();
	m_epText->SetFontSize(Font_6x4);
	m_epText->SetOrigin(81, 28);
	AddObject(m_epText);

	m_topLine1 = new CLCDLine();
	m_topLine1->SetLine(15, 14, 102, 14);
	AddObject(m_topLine1);

	m_topLine2 = new CLCDLine();
	m_topLine2->SetLine(102, 15, 102, 43);
	AddObject(m_topLine2);

	// positioning
	const int baseline = 36;

	// experience stuff
	m_bottomBar = new CLCDRectangle();
	m_bottomBar->SetOrigin(0, baseline - 1);
	m_bottomBar->SetSize(102, 8);
	AddObject(m_bottomBar);

	m_xpText = new CLCDFont();
	m_xpText->SetOrigin(1, baseline);
	m_xpText->SetFontSize(Font_7x5);
	m_xpText->Invert(true);
	AddObject(m_xpText);
}

CStaticDemoState::CStaticDemoState()
: CPageState()
{
	m_hHourGlass = 0;
	m_hOoc = 0;
	m_hResting = 0;
	m_hSwords = 0;
	m_hWaterDrop = 0;
}

CStaticDemoState::~CStaticDemoState()
{
	if (m_hHourGlass != 0)
	{
		DeleteObject(m_hHourGlass);
	}
	if (m_hOoc != 0)
	{
		DeleteObject(m_hOoc);
	}
	if (m_hResting != 0)
	{
		DeleteObject(m_hResting);
	}
	if (m_hSwords != 0)
	{
		DeleteObject(m_hSwords);
	}
	if (m_hWaterDrop != 0)
	{
		DeleteObject(m_hWaterDrop);
	}
}

void CStaticDemoState::Update()
{
	PCHARINFO pChar = GetCharInfo();
	ASSERT(pChar != NULL);
	
	PSPAWNINFO pSpawn = GetCharInfo()->pSpawn;
	ASSERT(pSpawn != NULL);

	char buffer[256];

	// easier to just set them all to false then enable the one i want
	m_bmpSwords->Show(false);
	m_bmpOoc->Show(false);
	m_bmpResting->Show(false);
	m_bmpHourGlass->Show(false);
	m_bmpWaterDrop->Show(false);

	// Calculate compass heading to target
	if (pTarget != NULL)
	{
		PSPAWNINFO target = (PSPAWNINFO)pTarget;
		double heading = (atan2(pSpawn->X - target->X, pSpawn->Y - target->Y) * 256.0 / PI) - pSpawn->Heading + 256;
		if (heading < 0) 
			heading += 512.0;
		else if (heading > 512.0)
			heading -= 512.0;
		m_bmpCompass->SetRotation((DWORD)(heading / 512 * 360));
		m_bmpCompass->Show(true);
	}
	else
	{
		m_bmpCompass->Show(false);
	}

	int combatstate = -1;
	PCPLAYERWND pWnd = (PCPLAYERWND)pPlayerWnd;
	if (pWnd != NULL)
		combatstate = pWnd->CombatState;

	switch (combatstate)
	{
	case 0:
		m_bmpSwords->Show(true);
		break;

	case 1:
		m_bmpWaterDrop->Show(true);
		break;

	case 2:
		m_bmpHourGlass->Show(true);
		break;

	case 3:
		m_bmpOoc->Show(true);
		break;

	case 4:
		m_bmpResting->Show(true);
		break;

	default:
		// otherwise: nothing is shown (this is ok. Maybe have a box around it later
		break;
	}

	// update hp meter -- calculate hp as a percent and use it for both
	float percent = (float)GetCurHPS() / (float)GetMaxHPS() * 100.0f;
	if (InHoverState())
		percent = 0.0f;
	else if (percent < 0.0f)
		percent = 0.0f;
	m_hpMeter->SetPos(percent);

	// update hp text -- if we're dead shortcut this since sometimes it will be oddly negative.
	if (InHoverState())
	{
		if (pSpawn->Type == SPAWN_CORPSE)
		{
			sprintf_s(buffer, 256, "    0", pSpawn->HPMax);
			m_hpPct->SetText("DEAD");
		}
		else
		{
			sprintf_s(buffer, 256, "%5i", pSpawn->HPCurrent);
			m_hpPct->SetText(" UNC");
		}
		m_hpText->SetText(buffer);

	}
	else
	{
		//sprintf_s(buffer, 256, "%i/%i", pSpawn->HPCurrent, pSpawn->HPMax);
		sprintf_s(buffer, 256, "%5i", pSpawn->HPCurrent);
		m_hpText->SetText(buffer);
		sprintf_s(buffer, 256, "%3i%%", (int)percent);
		m_hpPct->SetText(buffer);
	}

	// only display mana if our character actually has any mana. If no mana then don't draw.
	if (GetMaxMana() != 0)
	{
		// update mp meter
		percent = (float)GetCharInfo2()->Mana / (float)GetMaxMana() * 100.0f;
		m_mpMeter->SetPos(percent);

		//sprintf_s(buffer, 256, "%i/%i", GetCharInfo2()->Mana, GetMaxMana());
		sprintf_s(buffer, 256, "%5i", GetCharInfo2()->Mana);
		m_mpText->SetText(buffer);

		sprintf_s(buffer, 256, "%3i%%", (int)percent);
		m_mpPct->SetText(buffer);
	}

	// update ep meter
	percent = (float)GetCharInfo2()->Endurance / (float)GetMaxEndurance() * 100.0f;
	m_epMeter->SetPos(percent);

	//sprintf_s(buffer, 256, "%i/%i",GetCharInfo2()->Endurance, GetMaxEndurance());
	sprintf_s(buffer, 256, "%5i", GetCharInfo2()->Endurance);
	m_epText->SetText(buffer);

	sprintf_s(buffer, 256, "%3i%%", (int)percent);
	m_epPct->SetText(buffer);

	// update xp bottom bar text
	sprintf_s(buffer, 256, "%6.02f%% %3.0f%% AA:%4i", (float)GetCharInfo()->Exp / 3.30f,
		(float)GetCharInfo()->AAExp / 3.30f, GetCharInfo2()->AAPointsSpent + GetCharInfo2()->AAPoints);
	m_xpText->SetText(buffer);

	// status text:
	if (InHoverState())
	{
		int timer = 300 - ((((PCDISPLAY)pDisplay)->TimeStamp - GetCharInfo()->pSpawn->RespawnTimer) / 1000);
		
		if (timer > 60)
		{
			sprintf_s(buffer, 256, "Rez:%1im%2is", timer / 60, timer % 60);
		} else 
		{
			sprintf_s(buffer, 256, "Rez:  %2is", timer);
		}
		m_statusText->SetText(buffer);
		m_statusText->Show(true);
	}
	else if (pChar->DowntimeStamp)
	{
		int downtime = pChar->Downtime - (GetFastTime() - pChar->DowntimeStamp);

		sprintf_s(buffer, 256, "OOC:%3is", downtime);
		m_statusText->SetText(buffer);
		m_statusText->Show(true);
	}
	else
	{
		m_statusText->Show(false);
	}
}

void CStaticDemoState::Activate()
{
	CPageState::Activate();

	// if this character has no mana then we won't draw these parts.
	if (GetMaxMana() == 0) 
	{
		m_mpMeter->Show(false);
		m_mpText->Show(false);
	}

	// update the character's name when this page is activated. If this isn't enough the user
	// can just switch away and back and it should be updated.
	m_name->SetText(GetCharInfo()->Name);
}

string CStaticDemoState::GetName()
{
	return "Default"; 
}
#pragma endregion
// ----------------------------------------------------------------------------
#pragma region CXmlState class
struct XmlPageElement
{
	XmlBase*	base;
	CLCDBase*	object;
	bool		dynamic;
};

CXmlState::CXmlState(XmlPageLayout *layout)
	: CPageState()
{
	m_layout = layout;
}

CXmlState::~CXmlState()
{

}

void CXmlState::Init()
{
	// initialize the objects in this xml page
	for (int i = 0; i < m_layout->numElements(); i++)
	{
		XmlBase* e = m_layout->getElement(i);
		ASSERT(e != NULL);

		// create local representation
		XmlPageElement* elem = new XmlPageElement;
		elem->base = e;
		
		// now create the handle
		switch (e->getType())
		{
			case Type_StaticText:
				{
					XmlText* text = static_cast<XmlText*>(e);

					// create element
					CLCDBase* newText = CreateText(
						text->isScrolling() ? LG_SCROLLING_TEXT : LG_STATIC_TEXT,
						(LGTextSize)text->getFontSize(),
						text->getAlign(),
						text->getLength());
					newText->SetOrigin(text->getPos().getX(), text->getPos().getY());

					if (newText->GetObjectType() == LG_SCROLLING_TEXT)
					{
						CLCDStreamingText* pText = dynamic_cast<CLCDStreamingText*>(newText);
						ASSERT(pText != NULL);

						pText->SetText(text->getText().c_str());
					}
					else if (newText->GetObjectType() == LG_STATIC_TEXT)
					{
						CLCDText* pText = dynamic_cast<CLCDText*>(newText);
						ASSERT(pText != NULL);

						pText->SetText(text->getText().c_str());
					}

					elem->dynamic = text->isDynamic();
					elem->object = newText;

					// add element to page
					AddObject(elem->object);
				}
				break;

			case Type_ProgressBar:
				{
					XmlProgressBar* bar = static_cast<XmlProgressBar*>(e);

					// create element
					CLCDProgressBar* newBar = CreateProgressBar((LGProgressBarType)bar->getStyle());
					newBar->SetOrigin(bar->getPos().getX(), bar->getPos().getY());
					newBar->SetSize(bar->getSize().getWidth(), bar->getSize().getHeight());
					newBar->SetPos(100);

					elem->dynamic = true;
					elem->object = newBar;
					
					// add element to page
					AddObject(newBar);
				}
				break;

			default:
				ASSERT(false);
		}
		m_elemdata.push_back(elem);
	}
}

void CXmlState::Update()
{
	char buffer[MAX_STRING];

	// loop through elements, updating those that are necessary
	vector<XmlPageElement*>::const_iterator it = m_elemdata.begin();
	while (it != m_elemdata.end())
	{
		if ((*it)->dynamic)
		{
			XmlPageElement* element = *it;

			switch (element->base->getType())
			{
			case Type_StaticText:
				{
					XmlText* text = (XmlText*)element->base;

					strncpy(buffer, text->getText().c_str(), MAX_STRING);
					ParseMacroParameter(NULL, buffer);		

					SetLCDObjectText(element->object, buffer);
					break;
				}

			case Type_ProgressBar:
				{
					XmlProgressBar* bar = (XmlProgressBar*)element->base;

					strncpy(buffer, bar->getValue().c_str(), MAX_STRING);
					ParseMacroParameter(NULL, buffer);

					CLCDProgressBar* pBar = dynamic_cast<CLCDProgressBar*>(element->object);
					ASSERT(pBar != NULL);

					pBar->SetPos((float)atof(buffer));
					break;
				}

			default:
				ASSERT(false);
			}
		}
		++it;
	}
}

string CXmlState::GetName()
{
	ASSERT(m_layout != NULL);
	string name = m_layout->getName();

	ASSERT(name != "");
	return name;
}
#pragma endregion
// ----------------------------------------------------------------------------

void CRestrictedState::Init()
{
	m_text = (CLCDStreamingText*)CreateText(LG_SCROLLING_TEXT, LG_MEDIUM, DT_LEFT, 160);
	m_text->SetOrigin(0, 19);
	AddObject(m_text);
}

void CRestrictedState::Update()
{
	m_text->SetText("Lifetime Subscription Required");
}

string CRestrictedState::GetName()
{
	return "Default";
}

// ----------------------------------------------------------------------------

void CTestState::Init()
{
	m_test = new CLCDFont();
	m_test->Initialize();
	m_test->SetOrigin(0, 0);
	m_test->SetObjectType(LG_FONT);

	AddObject(m_test);
}

void CTestState::Update()
{
	m_test->SetText("123456789abcdefghijk");
}
string CTestState::GetName()
{
	return "Development Testing";
}

// ----------------------------------------------------------------------------

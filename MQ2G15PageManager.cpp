
#include "stdafx.h"

#include "MQ2G15.h"
#include "MQ2G15PageManager.h"
#include "MQ2G15PagesMono.h"
#include "MQ2G15PagesColor.h"

// ----------------------------------------------------------------------------

CMQ2G15PageManager::CMQ2G15PageManager(string title)
{
	m_pages = NULL;

	m_screenSaver = new CScreenSaverState();
	m_nullPage = new CNullState();

	m_currentPage = m_nullPage;

	m_initSucceeded = false;
	m_bringToForeground = false;
	m_title = title;
	m_changeTitle = false;

	/* page never expires */
	SetExpiration(INFINITE);

	/* set up the output device */

	// setup registration and configure contexts
	lgLcdConnectContextEx lgdConnectContext;
	lgLcdConfigureContext lgdConfigureContext;

	// further expansion of registration and configure contexts.
	lgdConfigureContext.configCallback = NULL;
	lgdConfigureContext.configContext = NULL;

	lgdConnectContext.appFriendlyName = m_title.c_str();
	lgdConnectContext.isPersistent = FALSE;
	lgdConnectContext.isAutostartable = FALSE;
	lgdConnectContext.onConfigure = lgdConfigureContext;

	// Change this once dual mode is supported
#ifdef MQ2G19_ENABLED
	lgdConnectContext.dwAppletCapabilitiesSupported = LGLCD_APPLET_CAP_QVGA;
#else
	lgdConnectContext.dwAppletCapabilitiesSupported = LGLCD_APPLET_CAP_BW;
#endif

	lgdConnectContext.dwReserved1 = NULL;
	lgdConnectContext.onNotify.notificationCallback = NULL;
	lgdConnectContext.onNotify.notifyContext = NULL;

	if (m_connection.Initialize(lgdConnectContext, NULL) != TRUE)
	{
		// this means the lcd sdk's lgLcdInit failed, and therefore
		// we will not be able to ever connect to the lcd, even if
		// a g-series keyboard is actually connected
		TRACE("ERROR: LCD SDK initialization failed\n");
		m_initSucceeded = false;
	}
	else
	{
#ifdef MQ2G19_ENABLED
		m_output = m_connection.ColorOutput();
		ASSERT(m_output != NULL);

		m_output->ShowPage(this);
#else
		m_output = m_connection.MonoOutput();
		ASSERT(m_output != NULL);

		m_output->ShowPage(this);
#endif

		m_initSucceeded = true;
	}


	// activate screensaver 
	m_paused = true;
	m_screenSaver->Activate();
	
	// add built-ins to object list
	AddObject(m_screenSaver);
	AddObject(m_nullPage);

	if (m_initSucceeded)
	{
		m_connection.Update();

		// Set this app as the foreground app
		int connectionId = m_output->GetDeviceId();
		lgLcdSetAsLCDForegroundApp(connectionId, TRUE);
	}
}

CMQ2G15PageManager::~CMQ2G15PageManager()
{
	ClearPages();

	m_connection.Shutdown();

	ASSERT(m_screenSaver != NULL);
	delete m_screenSaver;

	ASSERT(m_nullPage != NULL);
	delete m_nullPage;
}

void CMQ2G15PageManager::Update()
{
	// Update display
	if (m_initSucceeded)
	{
		GetCurrentPage().Update();
		m_connection.Update();

		if (m_bringToForeground)
		{
			// Set this app as the foreground app
			int connectionId = m_output->GetDeviceId();
			lgLcdSetAsLCDForegroundApp(connectionId, TRUE);

			m_bringToForeground = false;
		}

		if (m_changeTitle)
		{
			m_connection.SetTitle(m_newTitle.c_str());
			m_changeTitle = false;
			m_bringToForeground = true;
		}
	}
}

void CMQ2G15PageManager::OnLCDButtonDown(int nButton)
{
	switch (nButton)
	{
	case LGLCDBUTTON_BUTTON0:
		SetPagePrevious();
		break;

	case LGLCDBUTTON_BUTTON1:
		SetPageNext();
		break;

		/* old lcd soft-buttons */
	case LGLCDBUTTON_BUTTON2:
	case LGLCDBUTTON_BUTTON3:

		/* color lcd soft-buttons */
	case LGLCDBUTTON_LEFT:
	case LGLCDBUTTON_RIGHT:
	case LGLCDBUTTON_UP:
	case LGLCDBUTTON_DOWN:
	case LGLCDBUTTON_OK:
	case LGLCDBUTTON_CANCEL:
	case LGLCDBUTTON_MENU:
	default:
		/* pass the message on */
		CPageState& currentPage = GetCurrentPage();
		currentPage.OnLCDButtonDown(nButton);
		break;
	}
}

void CMQ2G15PageManager::OnLCDButtonUp(int nButton)
{
	switch (nButton)
	{
	case LGLCDBUTTON_BUTTON0:
	case LGLCDBUTTON_BUTTON1:
		break;

		/* old lcd soft-buttons */
	case LGLCDBUTTON_BUTTON2:
	case LGLCDBUTTON_BUTTON3:

		/* color lcd soft-buttons */
	case LGLCDBUTTON_LEFT:
	case LGLCDBUTTON_RIGHT:
	case LGLCDBUTTON_UP:
	case LGLCDBUTTON_DOWN:
	case LGLCDBUTTON_OK:
	case LGLCDBUTTON_CANCEL:
	case LGLCDBUTTON_MENU:
	default:
		/* pass the message on */
		CPageState& currentPage = GetCurrentPage();
		currentPage.OnLCDButtonUp(nButton);
		break;
	}
}

CPageState& CMQ2G15PageManager::GetFirstPage()
{
	ASSERT(m_pages != NULL);
	return *m_pages;
}

CPageState& CMQ2G15PageManager::GetCurrentPage()
{
	if (m_paused)
	{
		ASSERT(m_screenSaver != NULL);
		return *m_screenSaver;
	}

	ASSERT(m_currentPage != NULL);
	return *m_currentPage;
}

string CMQ2G15PageManager::GetCurrentPageName() 
{
	return GetCurrentPage().GetName();
}

CPageState* CMQ2G15PageManager::GetPageByName(const string& name)
{
	CPageState* page = m_pages;
	while (page != NULL)
	{
		string pname = page->GetName();
		if (!_stricmp(name.c_str(), pname.c_str()))
			return page;

		page = page->Next();
	}

	return NULL;
}

bool CMQ2G15PageManager::SetPageByName(const string& name)
{
	if (name != "")
	{
		CPageState* page = m_pages;
		do 
		{
			ASSERT(page != NULL);

			string pname = page->GetName();
			if (!_stricmp(name.c_str(), pname.c_str()))
			{
				SetActivePage(page);
				return true;
			}
			page = page->Next();
		} while (page != m_pages);
	}
	return false;
}

void CMQ2G15PageManager::SetPageNext()
{
	if (m_currentPage->Next() != m_currentPage)
	{
		SetActivePage(m_currentPage->Next());
	}
}

void CMQ2G15PageManager::SetPagePrevious()
{
	if (m_currentPage->Prev() != m_currentPage)
	{
		SetActivePage(m_currentPage->Prev());
	}
}

void CMQ2G15PageManager::SetPaused(bool paused)
{
	if (m_paused != paused)
	{
		if (paused)
		{
			m_currentPage->Deactivate();
			m_screenSaver->Activate();
		}
		else
		{
			m_screenSaver->Deactivate();
			m_currentPage->Activate();
		}
		m_paused = paused;
	}
}

void CMQ2G15PageManager::SetActivePage(CPageState* page)
{
	if (!m_paused)
	{
		ASSERT(m_currentPage != NULL);
		m_currentPage->Deactivate();
		m_currentPage = page;
		m_currentPage->Activate();
	}
}

void CMQ2G15PageManager::AddPage(CPageState* page)
{
	ASSERT(page != NULL);

	if (m_pages == NULL)
	{
		m_pages = page;
	}
	else
	{
		page->Link(m_pages);
	}

	// if no current page.. that is, current page is the null page
	if (m_currentPage == m_nullPage)
	{
		m_currentPage = page;
		if (!m_paused)
		{
			m_nullPage->Deactivate();
			m_currentPage->Activate();
		}
	}

	AddObject(page);
}

void CMQ2G15PageManager::ClearPages()
{
	ASSERT(m_currentPage != NULL);

	if (!m_paused)
	{
		m_currentPage->Deactivate();
	}
	m_currentPage = m_nullPage;

	if (m_pages != NULL)
	{
		CPageState* start = m_pages;
		do 
		{
			CPageState* tmp = m_pages;
			m_pages = m_pages->Next();

			RemoveObject(tmp);

			ASSERT(tmp != NULL);
			delete tmp;

		} while (start != m_pages);

		m_pages = NULL;
	}

	if (!m_paused)
	{
		// this will be nullpage so should it be activated?
		m_currentPage->Activate();
	}
}

void CMQ2G15PageManager::ListPages()
{
	WriteChatf("\aoPages currently loaded:");

	CPageState* start = m_pages;
	do
	{
		WriteChatf("\ao%s", start->GetName().c_str());
		start = start->Next();
	} while (start != m_pages);
}

void CMQ2G15PageManager::ResetTitle(string newtitle)
{
	if (m_initSucceeded)
	{
		m_newTitle = newtitle;
		m_changeTitle = true;
	}
}

void CMQ2G15PageManager::Screenshot(string filename)
{
#ifndef MQ2G19_ENABLED
	if (filename.length() == 0)
	{
		struct tm	newTime;
		time_t		szClock;
		CHAR buffer[256];

		time(&szClock);
		localtime_s(&newTime, &szClock);

		sprintf_s(buffer, 256, "MQ2G15 %i-%02i-%02i %02i-%02i-%02i.bmp", newTime.tm_year + 1900,
			newTime.tm_mon, newTime.tm_mday, newTime.tm_hour, newTime.tm_min, newTime.tm_sec);
		
		filename = buffer;
	}

	if (filename.rfind(".bmp") != filename.length() - 4)
		filename += ".bmp";

	string full_filename = gszINIPath + string("\\") + filename;

	// Ok, so now we have a filename to save as, all that is left to do before we begin
	// is update the canvas so that our screenshot is up to date
	this->Update();

	int bitmap_dx = 160;
	int bitmap_dy = 43;

	// create the file
	ofstream file(full_filename.c_str(), ios::binary);
	if (!file)
	{
		// problem creating the file
		return;
	}

	// create bitmap file headers
	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;

	fileHeader.bfType		= 0x4d42;
	fileHeader.bfSize		= 0;
	fileHeader.bfReserved1	= 0;
	fileHeader.bfReserved2	= 0;
	fileHeader.bfOffBits	= sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	ZeroMemory(&infoHeader, sizeof(infoHeader));
	infoHeader.biSize			= sizeof(infoHeader);
	infoHeader.biWidth			= bitmap_dx * 2;
	infoHeader.biHeight			= bitmap_dy * 2;
	infoHeader.biPlanes			= 1;
	infoHeader.biBitCount		= 24;
	infoHeader.biCompression	= BI_RGB;
	infoHeader.biClrImportant	= 2;
	infoHeader.biClrUsed		= 2;

	file.write((char*)&fileHeader, sizeof(fileHeader));
	file.write((char*)&infoHeader, sizeof(infoHeader));

	lgLcdBitmap* src = m_output->GetLCDScreen();
	BYTE data[160*43*4][3];
	ZeroMemory(data, sizeof(data));

	for (int i = 0; i < 86; i++)
	{
		for (int j = 319; j >= 0; j--)
		{
			BOOL color = src->bmp_mono.pixels[(43-(i/2)-1)*160+(j/2)];

			data[i*320+j][0] = color ? 0 : 255;
			data[i*320+j][1] = color ? 0 : 255;
			data[i*320+j][2] = color ? 0 : 255;
		}		
	}

	file.write((char*)data, 160*43*3*4);
	file.close();

	WriteChatf("Screenshot saved to %s...", filename.c_str());
#endif
}

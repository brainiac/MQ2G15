#include "pch.h"

#include "MQ2G15.h"
#include "MQ2G15Config.h"
#include "MQ2G15PageManager.h"
#include "MQ2G15PagesColor.h"
#include "MQ2G15PagesMono.h"

#ifdef MQ2G19_ENABLED
#pragma message ("Building for G19 Support")
#else
#pragma message ("Building for G15 Support")
#endif

PreSetup(PLUGIN_MODE)
CMQ2G15* g_plugin = nullptr;

constexpr int LCD_WIDTH = 160;
constexpr int LCD_HEIGHT = 43;

//----------------------------------------------------------------------------

CMQ2G15::CMQ2G15()
{
	m_zoning = false;
	char buffer[256];

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;

	// Initialize GDI+.
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, nullptr);

	/* set the title to the character name if we're loaded in game,
	 * otherwise, just leave the title as the default "MQ2G15" */
	if (pLocalPC != nullptr && pLocalPlayer != nullptr)
	{
		sprintf_s(buffer, 256, PLUGIN_MODE " - %s", pLocalPC->Name);
	}
	else
	{
		strcpy_s(buffer, 256, PLUGIN_MODE);
	}

	/* update delay is in milliseconds. it represents time between updates */
	m_updateDelay = 50;		// subject to change
	m_lastUpdate = 0;

	m_gameState = 0;
	m_lastGameState = 0;

	// create the configuration object
	m_config = new XmlConfig();

	// create the page manager object
	m_pageManager = new CMQ2G15PageManager(buffer);

	// add our g15 command
	AddCommand(PLUGIN_CMD, &CMQ2G15::callg15Command, false, true, true);

	// load all of our pages and add them to the page manager
	loadPages();
}

CMQ2G15::~CMQ2G15()
{
	RemoveCommand(PLUGIN_CMD);

	// clean up page states
	ASSERT(m_pageManager != NULL);
	delete m_pageManager;

	ASSERT(m_config != NULL);
	delete m_config;

	Gdiplus::GdiplusShutdown(m_gdiplusToken);
}

void CMQ2G15::g15Command(const char* line)
{
	char buffer[MAX_STRING];
	GetArg(buffer, line, 1);

	if (ci_equals(buffer, "reload"))
	{
		loadPages();
	}
	else if (ci_equals(buffer, "page"))
	{
		GetArg(buffer, line, 2);
		if (!m_pageManager->SetPageByName(buffer))
		{
			WriteChatf("\ar" PLUGIN_MODE ": Page \"%s\" not found", buffer);
		}
	}
	else if (ci_equals(buffer, "next"))
	{
		m_pageManager->SetPageNext();
	}
	else if (ci_equals(buffer, "prev"))
	{
		m_pageManager->SetPagePrevious();
	}
	else if (ci_equals(buffer, "list"))
	{
		m_pageManager->ListPages();
	}
	else if (ci_equals(buffer, "screenshot"))
	{
#ifdef MQ2G19_ENABLED
		WriteChatf("Screenshot feature not available in G19 mode.");
#else
		GetArg(buffer, line, 2);
		m_pageManager->Screenshot(buffer);		
#endif
	}
#ifdef MQ2G19_ENABLED
	else if (ci_equals(buffer, "map"))
	{
		CPageState* page = m_pageManager->GetPageByName("Map");
		if (page != NULL)
		{
			page->HandleCommands(line);
		}
		else
		{
			WriteChatf("Map Page not loaded!\n");
		}
	}
#endif
	else
	{
		WriteChatf("\ayUsage: \aw" PLUGIN_CMD " <\agreload \aw| \agpage <pagename> \aw| \agprev \aw| \agnext \aw| \aglist \aw| \agscreenshot [filename] \aw>");
#ifdef MQ2G19_ENABLED
		WriteChatf("    " PLUGIN_CMD " map <layer [auto | 0 | 1 | 2 | 3]> | spawns");
#endif
	}
}

void CMQ2G15::callg15Command(PlayerClient* pChar, const char* szLine)
{
	if (g_plugin != nullptr)
		g_plugin->g15Command(szLine);
}

void CMQ2G15::loadPages()
{
	std::string configFile = std::string(gPathConfig) + "\\MQ2G15Config.xml";

	m_pageManager->ClearPages();

#ifdef MQ2G19_ENABLED
	/* temporary, so list isn't empty: */
	m_pageManager->AddPage(new CColorMapDisplayState());
	m_pageManager->AddPage(new CStaticDemoState());
#else
	// load static states here
	m_pageManager->AddPage(new CStaticDemoState());

	// load config here
	m_config->loadConfig(configFile);
	for (uint32_t i = 0; i < m_config->numPages(); i++)
	{
		XmlPageLayout* pLayout = m_config->getPage(i);
		m_pageManager->AddPage(new CXmlState(pLayout));
	}
#endif
}

void CMQ2G15::update()
{
	// Don't need to run every frame.
	int time = GetTickCount();
	if (time - m_lastUpdate > m_updateDelay)
	{
		m_lastUpdate = time;

		if (m_gameState != m_lastGameState)
		{
			m_lastGameState = m_gameState;
			bool paused = m_gameState != GAMESTATE_INGAME;
			m_pageManager->SetPaused(paused);

			if (!paused)
			{
				std::string title = std::string(PLUGIN_MODE " - ") + pLocalPlayer->Name;
				m_pageManager->ResetTitle(title.c_str());
			}
		}
		m_pageManager->Update();
	}
}

void CMQ2G15::onBeginZone()
{
	m_pageManager->SetPaused(true);
	m_pageManager->Update();
	m_zoning = true;
}

void CMQ2G15::onEndZone()
{
	if (m_zoning)
	{
		m_pageManager->SetPaused(false);
		m_zoning = false;
	}
}

PLUGIN_API void InitializePlugin()
{
	DebugSpewAlways("Initializing " PLUGIN_MODE);
	if (!GetPlugin(PLUGIN_OPPOSITE))
	{
		g_plugin = new CMQ2G15();
	}
	else
	{
		WriteChatf("Couldn't load " PLUGIN_MODE ". " PLUGIN_OPPOSITE " is currently loaded. To load this plugin, please unload " PLUGIN_OPPOSITE " first.");
		EzCommand("/plugin " PLUGIN_MODE " unload");
		g_plugin = nullptr;
	}
}

PLUGIN_API void ShutdownPlugin()
{
	DebugSpewAlways("Shutting down " PLUGIN_MODE);
	if (g_plugin != nullptr)
		delete g_plugin;
}

PLUGIN_API void OnPulse()
{
	if (g_plugin != nullptr)
		g_plugin->update();
}

PLUGIN_API void OnBeginZone()
{
	if (g_plugin != nullptr)
		g_plugin->onBeginZone();

}

PLUGIN_API void OnEndZone()
{
	if (g_plugin != nullptr)
		g_plugin->onEndZone();
}

PLUGIN_API void SetGameState(unsigned int GameState)
{
	if (g_plugin != nullptr)
		g_plugin->setGameState(GameState);
}

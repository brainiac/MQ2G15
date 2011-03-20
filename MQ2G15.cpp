#include "stdafx.h"

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

CHAR INIFileName[MAX_PATH] = {0};
CHAR PLUGIN_NAME[MAX_PATH] = PLUGIN_MODE;
HINSTANCE ghModuleInstance = 0;

BOOL APIENTRY DllMain(HINSTANCE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		ghModuleInstance = hModule;
	}
	return TRUE;
}

CMQ2G15* g_plugin = NULL;

const int LCD_WIDTH = 160;
const int LCD_HEIGHT = 43;

CMQ2G15::CMQ2G15()
{
	m_zoning = false;
	char buffer[256];

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           m_gdiplusToken;

	// Initialize GDI+.
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	/* set the title to the character name if we're loaded in game,
	 * otherwise, just leave the title as the default "MQ2G15" */
	if (GetCharInfo() != NULL && GetCharInfo()->pSpawn != NULL)
	{
		sprintf_s(buffer, 256, PLUGIN_MODE " - %s", GetCharInfo()->Name);
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
	AddCommand(PLUGIN_CMD, &CMQ2G15::callg15Command, 0, 1, 1);

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

void CMQ2G15::g15Command(PCHAR line)
{
	char buffer[MAX_STRING];
	GetArg(buffer, line, 1);

	if (!stricmp(buffer, "reload"))
	{
		loadPages();
	}
	else if (!stricmp(buffer, "page"))
	{
		GetArg(buffer, line, 2);
		if (!m_pageManager->SetPageByName(buffer))
		{
			WriteChatf("\ar" PLUGIN_MODE ": Page \"%s\" not found", buffer);
		}
	}
	else if (!stricmp(buffer, "next"))
	{
		m_pageManager->SetPageNext();
	}
	else if (!stricmp(buffer, "prev"))
	{
		m_pageManager->SetPagePrevious();
	}
	else if (!stricmp(buffer, "list"))
	{
		m_pageManager->ListPages();
	}
	else if (!stricmp(buffer, "screenshot"))
	{
#ifdef MQ2G19_ENABLED
		WriteChatf("Screenshot feature not available in G19 mode.");
#else
		GetArg(buffer, line, 2);
		m_pageManager->Screenshot(buffer);		
#endif
	}
#ifdef MQ2G19_ENABLED
	else if (!stricmp(buffer, "map"))
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

void CMQ2G15::callg15Command(PSPAWNINFO pChar, PCHAR szLine)
{
	if (g_plugin != NULL)
		g_plugin->g15Command(szLine);
}

void CMQ2G15::loadPages()
{
	string configFile = string(gszINIPath) + "\\MQ2G15Config.xml";

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
		for (int i = 0; i < m_config->numPages(); i++)
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
				string title = string(PLUGIN_MODE " - ") + GetCharInfo()->pSpawn->Name;
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

bool FindPlugin(PCHAR PluginName)
{
	PMQPLUGIN pPlugin = pPlugins;
	while (pPlugin)
	{
		char szTemp[MAX_STRING];
		strcpy_s(szTemp, pPlugin->szFilename);

		if (!stricmp(szTemp, PluginName))
			return true;
		pPlugin = pPlugin->pNext;
	}
	return false;
}

PLUGIN_API void InitializePlugin()
{
	if (!MMOAllowedPlugin(mqplugin::ghPluginModule, PLUGIN_NAME))
	{
		char szBuffer[MAX_STRING] = {0};
		sprintf(szBuffer, "/timed 10 /plugin %s unload", PLUGIN_NAME);
		EzCommand(szBuffer);
		return;
	}

	DebugSpewAlways("Initializing " PLUGIN_MODE);
	if (!FindPlugin(PLUGIN_OPPOSITE))
	{
		g_plugin = new CMQ2G15();
	}
	else
	{
		WriteChatf("Couldn't load " PLUGIN_MODE ". " PLUGIN_OPPOSITE " is currently loaded. To load this plugin, please unload " PLUGIN_OPPOSITE " first.");
		DoCommand(NULL, "/plugin " PLUGIN_MODE " unload");
		g_plugin = NULL;
	}
}

PLUGIN_API void ShutdownPlugin()
{
	DebugSpewAlways("Shutting down " PLUGIN_MODE);
	if (g_plugin != NULL)
		delete g_plugin;
}

PLUGIN_API void OnPulse()
{
	if (g_plugin != NULL)
		g_plugin->update();	
}

PLUGIN_API void OnBeginZone()
{
	if (g_plugin != NULL)
		g_plugin->onBeginZone();

}

PLUGIN_API void OnEndZone()
{
	if (g_plugin != NULL)
		g_plugin->onEndZone();
}

PLUGIN_API void SetGameState(DWORD GameState)
{
	if (g_plugin != NULL)
		g_plugin->setGameState(GameState);
}

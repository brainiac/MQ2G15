#pragma once

#include "LCDUI/LCDUI.h"

#ifdef MQ2G19_ENABLED
#define PLUGIN_OPPOSITE "MQ2G15"
#define PLUGIN_MODE		"MQ2G19"
#define PLUGIN_CMD		"/g19"
#else
#define PLUGIN_OPPOSITE "MQ2G19"
#define PLUGIN_MODE		"MQ2G15"
#define PLUGIN_CMD		"/g15"
#endif

#pragma pack(push)
#pragma pack(4)

// ----------------------------------------------------------------------------
// Forward Declarations
class XmlConfig;
class CMQ2G15PageManager;

// ----------------------------------------------------------------------------
// Class Definitions

class CMQ2G15
{
protected:	// data

	// the xml config object
	XmlConfig* m_config;

	// the g15 page manager object
	CMQ2G15PageManager* m_pageManager;

	// update time tracking
	unsigned int m_lastUpdate;
	unsigned int m_updateDelay;

	// gamestate tracking
	unsigned int m_gameState;
	unsigned int m_lastGameState;

	ULONG_PTR m_gdiplusToken;
	bool m_zoning;

	// commands
	static void callg15Command(PSPAWNINFO pChar, PCHAR szLine);
	void g15Command(PCHAR line);

private:
	void loadPages();

public:
	// construction / destruction
	CMQ2G15();
	~CMQ2G15();

	// frame render callback
	virtual void update();

	virtual void onEndZone();
	virtual void onBeginZone();

	void setGameState(unsigned int gameState) { m_gameState = gameState; }
	unsigned int getGameState() { return m_gameState; }
};

#pragma pack(pop)
// ----------------------------------------------------------------------------
//

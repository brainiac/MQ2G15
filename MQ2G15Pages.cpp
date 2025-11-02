
#include "pch.h"

#include "MQ2G15.h"
#include "MQ2G15Config.h"
#include "MQ2G15Pages.h"
#include "MQ2G15Bitmap.h"

#include "resource.h"

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)

// ----------------------------------------------------------------------------

namespace mqplugin {
	// Handle for our bitmap resources
	extern HINSTANCE ghPluginModule;
}
using mqplugin::ghPluginModule;

#pragma region CPageState class

CPageState::CPageState()
{
	m_next = this;
	m_prev = this;

	m_initialized = false;

	m_name = "<ERROR> Unnamed Page!";
}

CPageState::~CPageState()
{
	for (CLCDBase* pObj : m_Objects)
	{
		delete pObj;
	}
}

void CPageState::Link(CPageState* where)
{
	m_next = where;
	m_prev = where->m_prev;
	where->m_prev = this;
	m_prev->m_next = this;
}

void CPageState::Deactivate()
{
	this->Show(false);
}

void CPageState::Activate()
{
	if (!m_initialized)
	{
		Init();
		m_initialized = true;
	}

	this->Show(true);
}

void CPageState::Update()
{
	ASSERT(m_initialized);
}

#pragma endregion

// ----------------------------------------------------------------------------

#pragma region CScreenSaverState

static bool BitmapDataInverted = false;

void CScreenSaverState::Init()
{
	SetName("Character Select");

#ifdef MQ2G19_ENABLED
	m_bitmap = new CLCDBitmap();
	m_bitmap->SetOrigin(0, 0);
	m_bitmap->LoadFromResource(ghPluginModule, IDB_MQ2PNG, "PNG");
	m_bitmap->SetObjectType(LG_BITMAP);

	AddObject(m_bitmap);
#else
	// I don't feel like reproducing the header, so I'm just going to invert it 
	// programmatically... 
	if (!BitmapDataInverted)
	{
		BitmapDataInverted = true;
		for (int i = 0; i < 160 * 43 / 8; i++)
			bitmapData[i] ^= 0xffffffff;
	}

	// setup bitmap
	m_bmp = CreateBitmap(160, 43, 1, 1, bitmapData);
	ASSERT(m_bmp != NULL);

	m_bitmap = new CLCDBitmap();
	m_bitmap->SetOrigin(0, 0);
	m_bitmap->SetSize(160, 43);
	m_bitmap->SetBitmap(m_bmp);
	m_bitmap->SetObjectType(LG_BITMAP);

	AddObject(m_bitmap);
#endif
}	

CScreenSaverState::CScreenSaverState()
{
}

CScreenSaverState::~CScreenSaverState()
{
	if (m_bmp != nullptr)
	{
		DeleteObject(m_bmp);
	}
}

#pragma endregion

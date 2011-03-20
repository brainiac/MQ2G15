
#include "stdafx.h"

#include "MQ2G15.h"
#include "MQ2G15Config.h"
#include "MQ2G15PagesColor.h"
#include "MQ2G15Bitmap.h"


// Handle for our bitmap resources
extern HINSTANCE ghModuleInstance;


CLCDMapObject::CLCDMapObject()
{
	Initialize(320, 240);
	
	m_lastTime = 0;
	m_zoom = .75f;

	m_panX = 160;
	m_panY = 120;

	m_showSpawns = true;
	m_targetLine = true;
}

CLCDMapObject::~CLCDMapObject()
{
}

void CLCDMapObject::SetupRendering()
{
    //Let's do some 2D rendering
    int vPort[4];

    glGetIntegerv(GL_VIEWPORT, vPort);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    glClearColor(1.0f,1.0f,1.f,1.0f);

    glOrtho(0, vPort[2], 0, vPort[3], -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
}

void CLCDMapObject::DoRendering()
{
	DWORD timestamp = GetTickCount();

	glClear(GL_COLOR_BUFFER_BIT);   

	// set up translation and zoom
    glLoadIdentity();
	
	PCHARINFO pCharInfo = GetCharInfo();
	glTranslatef(pCharInfo->pSpawn->X * m_zoom + m_panX, m_panY + pCharInfo->pSpawn->Y * m_zoom, 1.0);

	if (m_showSpawns)
		DrawSpawns();

	if (m_targetLine)
		DrawTargetLine();

	// render the character arrow at a constant size
	DrawCharacterPosition();


	// render the zone map
	DrawZoneMap();

	m_lastTime = timestamp;
}

void CLCDMapObject::DrawZoneMap()
{
	glPushMatrix();
	glScalef(m_zoom, m_zoom, 1.0);

	for (int layer = 0; layer <= 3; layer++)
	{
		if (DrawZoneMapLayer(layer) > 0)
			break;
	}

	glPopMatrix();
}

int CLCDMapObject::DrawZoneMapLayer(int layer)
{
	// draw map lines
	glBegin(GL_LINES);

	int count = 0;
	PEQMAPWINDOW map = ((PEQMAPWINDOW)pMapViewWnd);
	PMAPLINE lines = map->pLines;
	while (lines != NULL)
	{
		if (lines->Layer == layer)
		{
			glColor3ub(lines->Color.R, lines->Color.G,lines->Color.B);
			glVertex2f(lines->Start.X, lines->Start.Y);

			glColor3ub(lines->Color.R, lines->Color.G,lines->Color.B);
			glVertex2f(lines->End.X, lines->End.Y);

			count++;
		}
		lines = lines->pNext;
	}
	glEnd();
	return count;
}

void CLCDMapObject::DrawSpawnArrow(float x, float y, float heading, float scale)
{
	glPushMatrix();

	// position the arrow
	glTranslatef(-x * m_zoom, -y * m_zoom, 0);

	// rotate towards heading
	float fixed_heading = 180.0f - (heading / 512.0f * 360.0f);
	glRotatef(fixed_heading, 0.0f, 0.0f, 1.0f);
	glScalef(scale, scale, 1.0f);

	// draw arrow head
	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(0.0f, 1.0f);
	glVertex2f(1.0f, -1.0f);

	glVertex2f(0.0f, 1.0f);
	glVertex2f(-1.0f, -1.0f);

	// draw the arrow tail
	glVertex2f(0.0f, 1.0f);
	glVertex2f(0.0f, -2.0f);
	glEnd();

	glPopMatrix();
}

void CLCDMapObject::DrawCharacterPosition()
{
	PCHARINFO pCharInfo = GetCharInfo();
	ASSERT(pCharInfo != NULL);

	DrawSpawnArrow(pCharInfo->pSpawn->X, pCharInfo->pSpawn->Y, pCharInfo->pSpawn->Heading, 5.0f);
}

ARGBCOLOR CLCDMapObject::GetColorFromSpawn(PSPAWNINFO pSpawn)
{
	ARGBCOLOR color;
	eSpawnType type = GetSpawnType(pSpawn);

	switch (type)
	{
	case PC:
		color.ARGB = 0xFF00FF;
		break;

	case NPC:
		color.ARGB = ConColorToARGB(ConColor(pSpawn));
		break;

	case CORPSE:
		color.ARGB = 0x00C000;
		break;

	case TRIGGER:
	case TRAP:
	case CHEST:
	case TIMER:
		color.ARGB = 0xC08000;
		break;

	case OBJECT:
	case BANNER:
	case AURA:
	case CAMPFIRE:
		color.ARGB = 0x404040;
		break;

	case MERCENARY:
	case PET:
	case MOUNT:
		color.ARGB = 0x707070;
		break;

	case ITEM:
	case UNTARGETABLE:
	default:
		color.ARGB = 0xc0c0c0;
		break;
	}

	if (color.ARGB == 0xFFFFFFFF)
		color.ARGB = 0x0;
	if (color.ARGB == 0xFF505050)
		color.ARGB = 0xFFB0B0B0;

	return color;
}

void CLCDMapObject::DrawSpawns()
{
	PSPAWNINFO pSpawn = pSpawnList;
	PCHARINFO pChar = GetCharInfo();
	ASSERT(pChar != NULL);

	glPointSize(2.5f);

	if (m_zoom < .15f)
		glPointSize(16.0f * m_zoom);

	glBegin(GL_POINTS);

	PSPAWNINFO pTarg = (PSPAWNINFO) pTarget;

	while (pSpawn != NULL)
	{
		if (pSpawn != pChar->pSpawn		/* if not myself */ 
			&& (pTarg == NULL || pTarg != pSpawn || !m_targetLine))		/* and not my target when targetting is enabled */
		{
			ARGBCOLOR color = GetColorFromSpawn(pSpawn);

			glColor3f(color.R / 255.0f, color.G / 255.0f, color.B / 255.0f);
			glVertex2f(-pSpawn->X * m_zoom, -pSpawn->Y * m_zoom);
		}
		pSpawn = pSpawn->pNext;
	}

	glEnd();
}

void CLCDMapObject::DrawTargetLine()
{
	if (pTarget == NULL)
		return;

	PCHARINFO pChar = GetCharInfo();
	ASSERT(pChar != NULL);

	PSPAWNINFO pSpawn = (PSPAWNINFO)pTarget;

	if (pChar->pSpawn == pSpawn)
		return;

	glBegin(GL_LINES);

	glColor3f(1.0f, 0.25f, 0.0f);
	glVertex2f(-pChar->pSpawn->X * m_zoom, -pChar->pSpawn->Y * m_zoom);
	glVertex2f(-pSpawn->X * m_zoom, -pSpawn->Y * m_zoom);

	glEnd();

	glPointSize(2.5f);
	glBegin(GL_POINTS);

	glColor3f(1.0f, .25f, 0.0f);
	glVertex2f(-pSpawn->X * m_zoom, -pSpawn->Y * m_zoom);

	glEnd();
}

void CLCDMapObject::ZoomIn()
{
	m_zoom /= 0.75f;

	//WriteChatf("Zoom: %.2f", m_zoom);
}

void CLCDMapObject::ZoomOut()
{
	m_zoom *= 0.75f;

	//WriteChatf("Zoom: %.2f", m_zoom);
}

void CLCDMapObject::PanDelta(float x, float y)
{
	m_panX += x;
	m_panY += y;
}

// ----------------------------------------------------------------------------

void CColorMapDisplayState::Init()
{
	m_map = new CLCDMapObject();	
	m_map->MakeCurrent();
	m_map->SetupRendering();
	AddObject(m_map);

	m_title = new CLCDText();
	m_title->SetAlignment(DT_CENTER);
	m_title->SetOrigin(0, 0);
	m_title->SetSize(320,25);
	m_title->SetFontPointSize(14);
	m_title->SetFontColor(RGB(255, 0, 0));
	m_title->SetText("<MAP NAME>");

	AddObject(m_title);
}

void CColorMapDisplayState::OnLCDButtonDown(int nButton)
{
	DebugSpew("OnButtonDown: %i\n", nButton);

	switch (nButton)
	{
	case LGLCDBUTTON_OK:
		m_map->ZoomIn();
		break;

	case LGLCDBUTTON_CANCEL:
		m_map->ZoomOut();
		break;

		/* color lcd soft-buttons */
	case LGLCDBUTTON_LEFT:
		m_map->PanDelta(10.0f, 0);
		break;

	case LGLCDBUTTON_RIGHT:
		m_map->PanDelta(-10.0f, 0);
		break;

	case LGLCDBUTTON_UP:
		m_map->PanDelta(0, 10.0f);
		break;

	case LGLCDBUTTON_DOWN:
		m_map->PanDelta(0, -10.0f);
		break;

	case LGLCDBUTTON_MENU:
	default:
		/* do nothing */
		break;
	}
}

void CColorMapDisplayState::OnLCDButtonUp(int nButton)
{
	DebugSpew("OnButtonUp: %i\n", nButton);

	switch (nButton)
	{
		/* color lcd soft-buttons */
	case LGLCDBUTTON_LEFT:
	case LGLCDBUTTON_RIGHT:
	case LGLCDBUTTON_UP:
	case LGLCDBUTTON_DOWN:
	case LGLCDBUTTON_MENU:
	case LGLCDBUTTON_OK:
	case LGLCDBUTTON_CANCEL:
	default:
		/* do nothing */
		break;
	}
}

void CColorMapDisplayState::Update()
{
	PCHARINFO pCharInfo = GetCharInfo();
	if (pCharInfo == NULL)
		return;

	const char* zoneName = GetFullZone(pCharInfo->zoneId);
	m_title->SetText(zoneName);

	m_map->BeginDraw();
	m_map->DoRendering();
	m_map->EndDraw();
}

// ----------------------------------------------------------------------------

string CColorMapDisplayState::GetName()
{
	return "Map";
}

CColorMapDisplayState::CColorMapDisplayState()
{
	m_title = NULL;
	m_map = NULL;
}

CColorMapDisplayState::~CColorMapDisplayState()
{
	if (m_title != NULL)
		DeleteObject(m_title);
	if (m_map != NULL)
		DeleteObject(m_map);
}

void CColorMapDisplayState::HandleCommands(const char *string)
{
	CHAR szTemp[MAX_STRING];
	GetArg(szTemp, string, 2);

	if (!stricmp(szTemp, "spawns"))
	{
		bool showSpawns = !m_map->GetShowSpawns();
		m_map->SetShowSpawns(showSpawns);
		WriteChatf("MQ2G19 Map: Spawns are now %s.", showSpawns ? "visible" : "hidden");
	}
	else if (!stricmp(szTemp, "targetline"))
	{
		bool targetLine = !m_map->GetShowTargetLine();
		m_map->SetShowTargetLine(targetLine);
		WriteChatf("MQ2G19 Map: Target line is now %s.", targetLine ? "visible" : "hidden");
	}
}
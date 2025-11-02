#pragma once

#include "MQ2G15Pages.h"
#include "OGLObject.h"

// ----------------------------------------------------------------------------

class CLCDMapObject : public COGLObject
{
	DWORD m_lastTime;
	float m_zoom;
	float m_panX, m_panY;

	bool m_showSpawns;
	bool m_targetLine;

	void DrawZoneMap();
	int DrawZoneMapLayer(int layer);

	void DrawSpawnArrow(float x, float y, float heading, float scale);
	void DrawCharacterPosition();
	void DrawSpawns();
	void DrawTargetLine();

	ARGBCOLOR GetColorFromSpawn(PSPAWNINFO pSpawn);

public:
	CLCDMapObject();
	virtual ~CLCDMapObject() override;

	void DoRendering();
	void SetupRendering();

	void ZoomIn();
	void ZoomOut();
	void PanDelta(float x, float y);

	bool GetShowSpawns() { return m_showSpawns; }
	void SetShowSpawns(bool showSpawns) { m_showSpawns = showSpawns; }

	bool GetShowTargetLine() { return m_targetLine; }
	void SetShowTargetLine(bool targetLine) { m_targetLine = targetLine; }
};


class CColorMapDisplayState : public CPageState
{
	CLCDText*      m_title;
	CLCDMapObject* m_map;

public:
	CColorMapDisplayState();
	virtual ~CColorMapDisplayState() override;

	virtual void Init() override;
	virtual void Update() override;

	virtual void OnLCDButtonDown(int nButton) override;
	virtual void OnLCDButtonUp(int nButton) override;

	virtual void HandleCommands(const char* string) override;
};

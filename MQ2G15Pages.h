#pragma once

#include "LCDUI/LCDPage.h"

#include <string>

class CPageState : public CLCDPage
{
	CPageState* m_next;
	CPageState* m_prev;

	bool m_initialized;

public:
	CPageState();
	virtual ~CPageState() override;

	virtual void Deactivate();
	virtual void Activate();

	virtual void Update();
	virtual void HandleCommands(const char* string) {}
	virtual const std::string& GetName() const { return m_name; }

	CPageState* Next() { return m_next; }
	CPageState* Prev() { return m_prev; }

	void Link(CPageState* where);

protected:
	void SetName(std::string_view name) { m_name = name; }
	virtual void Init() = 0;

	std::string m_name;
};


class CScreenSaverState : public CPageState
{
	CLCDBitmap* m_bitmap = nullptr;
	HBITMAP m_bmp{};

public:
	CScreenSaverState();
	virtual ~CScreenSaverState() override;

	virtual void Init() override;
};

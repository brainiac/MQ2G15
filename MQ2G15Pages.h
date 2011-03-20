#pragma once

class CPageState : public CLCDPage
{
	CPageState* m_next;
	CPageState* m_prev;

	bool m_initialized;
	string m_name;

public:
	CPageState();
	virtual ~CPageState();

	virtual void Deactivate();
	virtual void Activate();

	virtual void Update();
	virtual void HandleCommands(const char* string) { return; }
	virtual string GetName() { return m_name; }

	CPageState* Next() { return m_next; }
	CPageState* Prev() { return m_prev; }

	void Link(CPageState* where);

protected:

	virtual void Init() = 0;
};


class CScreenSaverState : public CPageState
{
	CLCDBitmap* m_bitmap;
	HBITMAP m_bmp;

public:
	CScreenSaverState();
	virtual ~CScreenSaverState();

	virtual void Init();
	virtual string GetName();
};
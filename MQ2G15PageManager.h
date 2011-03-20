#pragma once

#include "LCDUI/LCDUI.h"

// ----------------------------------------------------------------------------
// Forward Declarations
class CPageState;
class CLCDOutput;

// ----------------------------------------------------------------------------
// Class Definitions
class CMQ2G15PageManager : public CLCDPage
{
protected:
	CPageState* m_pages;
	CPageState* m_currentPage;

	// special pages
	CPageState* m_screenSaver;
	CPageState* m_nullPage;

	string m_title;
	bool m_initSucceeded;

	CLCDConnection m_connection;
	CLCDOutput*    m_output;
	bool m_paused;
	bool m_bringToForeground;

	bool m_changeTitle;
	std::string m_newTitle;

	void SetActivePage(CPageState* page);

	virtual void OnLCDButtonDown(int nButton);
	virtual void OnLCDButtonUp(int nButton);

public:

	CMQ2G15PageManager(string title);
	~CMQ2G15PageManager();

	CPageState& GetFirstPage();
	CPageState& GetCurrentPage();
	string GetCurrentPageName();

	CPageState* GetPageByName(const string& name);

	bool SetPageByName(const string& name);

	void SetPageNext();
	void SetPagePrevious();

	void SetPaused(bool paused);
	bool GetPaused() { return m_paused; }

	void AddPage(CPageState* state);
	void ClearPages();
	void ListPages();

	void Update();
	bool IsLoaded() { return m_initSucceeded; }

	// Resets the title shown on the lcd to a new one
	void ResetTitle(string newTitle);

	// takes a screenshot and saves it as the given filename
	void Screenshot(string filename);
};
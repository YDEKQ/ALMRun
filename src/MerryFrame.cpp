#include "MerryFrame.h"
#include "MerryConfig.h"
#include "MerryMainPanel.h"
#include "MerryListBoxPanel.h"
#include "MerryTaskBarIcon.h"
#include "MerryIcon.xpm"
#include "MerryLua.h"
#include "MerryCommandManager.h"
#include "MerryTimerManager.h"
#include "MerryHotkey.h"
#include "MerryController.h"

//#define DEBUG_ALWAYS_SHOW

BEGIN_EVENT_TABLE(MerryFrame, wxFrame)
	EVT_CLOSE(MerryFrame::OnCloseEvent)
	EVT_ACTIVATE(MerryFrame::OnActivateEvent)
	EVT_SHOW(MerryFrame::OnShowEvent)
	EVT_CONTEXT_MENU(MerryFrame::onContextMenu)
	EVT_MENU(MENU_ITEM_OPEN_CONFIG, MerryTaskBarIcon::onPopMenu)
	EVT_MENU(MENU_ITEM_CONFIG, MerryTaskBarIcon::onPopMenu)
	EVT_MENU(MENU_ITEM_ABOUT, MerryTaskBarIcon::onPopMenu)
	EVT_MENU(MENU_ITEM_EXIT, MerryTaskBarIcon::onPopMenu)
END_EVENT_TABLE()

MerryFrame::MerryFrame():
	wxFrame(NULL, wxID_ANY, MERRY_DEFAULT_TITLE, wxDefaultPosition, wxDefaultSize, wxFRAME_NO_TASKBAR | wxBORDER_NONE
#ifdef __WXOSX__
		| wxSTAY_ON_TOP
#endif
	)
{
	this->SetClientSize(MERRY_DEFAULT_WIDTH, MERRY_DEFAULT_HEIGHT);
	m_mainPanel = new MerryMainPanel(this);
	m_listBoxPanel = new MerryListBoxPanel(this);
	m_taskBarIcon = NULL;
	this->ShowTrayIcon(true);
	m_isCentred = false;
#ifdef DEBUG_ALWAYS_SHOW
	this->Show();
#endif
}

MerryFrame::~MerryFrame()
{
	if (m_mainPanel)
		m_mainPanel->Destroy();
	if (m_listBoxPanel)
		m_listBoxPanel->Destroy();
	if (m_taskBarIcon)
	delete m_taskBarIcon;
}

void MerryFrame::onContextMenu(wxContextMenuEvent& e)
{
	wxMenu* menu = new wxMenu;
	menu->Append(MENU_ITEM_OPEN_CONFIG, wxT("��ʾ����(&S)"));
	menu->Append(MENU_ITEM_CONFIG, wxT("ˢ������(R)"));
	menu->Append(MENU_ITEM_ABOUT, wxT("����ALMRun(&A)"));
	menu->Append(MENU_ITEM_EXIT, wxT("�˳�(&X)"));
	PopupMenu(menu);
    return;
}

void MerryFrame::NewConfig()
{
	if (g_hotkey)
	{
		g_hotkey->OnDelete();
		delete g_hotkey;
		g_hotkey = NewMerryHotkey();
	}

	if (g_timers)
	{
		delete g_timers;
		g_timers = new MerryTimerManager();
	}

	if (g_commands)
	{
		delete g_commands;
		g_commands = new MerryCommandManager();
	}

	if (g_config)
	{
		delete g_config;
		g_config = new ALMRunConfig();
	}

	if (lua_bak)
		delete lua_bak;

	lua_bak = g_lua;

	if (g_lua)
		g_lua->OnClose();

	g_lua = new MerryLua();
}

void MerryFrame::OnInit()
{
	assert(!g_controller);
	g_controller = new MerryController();

	assert(!g_timers);
	g_timers = new MerryTimerManager();

	assert(!g_hotkey);
	g_hotkey = NewMerryHotkey();

	assert(!g_commands);
	g_commands = new MerryCommandManager();

	assert(!g_config);
	g_config = new ALMRunConfig();

	assert(!g_lua);
	g_lua = new MerryLua();
}

void MerryFrame::OpenConfigDir()
{
#ifdef __WXMSW__
	if (!g_config)
		return;
	wxString pathTmp = g_config->Home;
	pathTmp.Append("config");
	::ShellExecute(NULL,_T("explore"),pathTmp.c_str(),NULL, NULL,true);
#endif
}

void MerryFrame::ShowTrayIcon(const bool show)
{
	if (!show && m_taskBarIcon)
	{
		m_taskBarIcon->Destroy();
		m_taskBarIcon = NULL;
	}
	else if (show && !m_taskBarIcon)
	{
		m_taskBarIcon = new MerryTaskBarIcon();
		m_taskBarIcon->SetIcon(wxIcon(MerryIcon_xpm), TASKBARICON_TIP);
	}
}

void MerryFrame::OnClose()
{
	if (lua_bak)
		delete lua_bak;
	if (g_lua)
	{
		g_lua->OnClose();
		delete g_lua;
		g_lua = NULL;
	}

	if (g_commands)
	{
		delete g_commands;
		g_commands = NULL;
	}

	if (g_config)
	{
		delete g_config;
		g_config = NULL;
	}

	if (g_hotkey)
	{
		g_hotkey->OnDelete();
		delete g_hotkey;
		g_hotkey = NULL;
	}

	if (g_timers)
	{
		delete g_timers;
		g_timers = NULL;
	}

	if (g_controller)
	{
		delete g_controller;
		g_controller = NULL;
	}
}

MerryListBoxPanel* MerryFrame::GetListBoxPanel()
{
	return m_listBoxPanel;
}

void MerryFrame::OnCloseEvent(wxCloseEvent& e)
{
	this->OnClose();
	this->Destroy();
}

void MerryFrame::OnActivateEvent(wxActivateEvent& e)
{
#ifndef DEBUG_ALWAYS_SHOW
	if (!e.GetActive())
		this->Hide();
#endif
}

void MerryFrame::OnShowEvent(wxShowEvent& e)
{
	MerryTextCtrl* textCtrl = m_mainPanel->GetTextCtrl();
	assert(textCtrl);
	if (e.IsShown())
	{
		this->CentreOnce();
		m_listBoxPanel->Dismiss();
		this->Raise();
		textCtrl->SetFocus();
	}
	else
		textCtrl->ChangeValue(wxT(""));
	textCtrl->EnterArgs = 0;
}

void MerryFrame::CentreOnce()
{
	if (m_isCentred)
		return;

	this->Centre();
	wxPoint position = this->GetPosition();
	position.y -= MERRY_DEFAULT_LIST_BOX_ITEM_HEIGHT * MERRY_DEFAULT_LIST_BOX_ITEM_MAX_NUM / 4;
	this->SetPosition(position);
	
	m_isCentred = true;
}

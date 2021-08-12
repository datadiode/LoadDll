
// LoadDll.cpp: Definiert das Klassenverhalten f�r die Anwendung.
//

#include "stdafx.h"
#include "LoadDll.h"
#include "LoadDllDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLoadDllApp

BEGIN_MESSAGE_MAP(CLoadDllApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CLoadDllApp-Erstellung

CLoadDllApp::CLoadDllApp()
{
	// TODO: Hier Code zur Konstruktion einf�gen
	// Alle wichtigen Initialisierungen in InitInstance positionieren
}


// Das einzige CLoadDllApp-Objekt

CLoadDllApp theApp;


// CLoadDllApp-Initialisierung

BOOL CLoadDllApp::InitInstance()
{
	CWinApp::InitInstance();
	return FALSE;
}

int CLoadDllApp::ExitInstance()
{
	InitCommonControls(); // Ex(&InitCtrls);

	CLoadDllDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();

	CWinApp::ExitInstance();

	return static_cast<int>(nResponse);
}

// LoadDll.cpp: Definiert das Klassenverhalten f�r die Anwendung.
//

#include "stdafx.h"
#include "LoadDll.h"
#include "LoadDllDlg.h"

// Das einzige CLoadDllApp-Objekt

CLoadDllApp theApp;

int CLoadDllApp::Run()
{
	InitCommonControls();

	CLoadDllDlg dlg;
	m_pMainWnd = &dlg;

	return static_cast<int>(dlg.DoModal());
}

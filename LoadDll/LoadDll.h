
// LoadDll.h: Hauptheaderdatei f�r die PROJECT_NAME-Anwendung
//

#pragma once

#ifndef __AFXWIN_H__
	#error "'stdafx.h' vor dieser Datei f�r PCH einschlie�en"
#endif

#include "resource.h"		// Hauptsymbole


// CLoadDllApp:
// Siehe LoadDll.cpp f�r die Implementierung dieser Klasse
//

class CLoadDllApp : public CWinApp
{
public:
	CLoadDllApp();

// �berschreibungen
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementierung

	DECLARE_MESSAGE_MAP()
};

extern CLoadDllApp theApp;
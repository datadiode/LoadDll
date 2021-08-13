// LoadDll.h: Hauptheaderdatei für die PROJECT_NAME-Anwendung
//

#include "resource.h"		// Hauptsymbole

// CLoadDllApp:
// Siehe LoadDll.cpp für die Implementierung dieser Klasse
//

class CLoadDllApp : public CWinApp
{
// Überschreibungen
public:
	virtual int Run();
};

extern CLoadDllApp theApp;

// LoadDll.h: Hauptheaderdatei f�r die PROJECT_NAME-Anwendung
//

#include "resource.h"		// Hauptsymbole

// CLoadDllApp:
// Siehe LoadDll.cpp f�r die Implementierung dieser Klasse
//

class CLoadDllApp : public CWinApp
{
// �berschreibungen
public:
	virtual int Run();
};

extern CLoadDllApp theApp;

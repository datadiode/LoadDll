#include "stdafx.h"
#include "LoadDll.h"
#include "LoadDllDlg.h"
#include "disasm/LDE64.h"
#include "MemoryModule.h"

//
// Length Disassembler Engine by BeatriX
//
#ifdef _WIN64
#pragma comment(lib, "disasm/LDE64x64.lib")
#else
#pragma comment(lib, "disasm/LDE64.lib")
#endif

//
// Typedefs for functions.
//
typedef int (__stdcall *ProcNoArgStdcall)( );
typedef int (__stdcall *Proc1ArgStdcall)( void* );
typedef int (__stdcall *Proc2ArgStdcall)( void*, void* );
typedef int (__stdcall *Proc3ArgStdcall)( void*, void*, void* );
typedef int (__stdcall *Proc4ArgStdcall)( void*, void*, void*, void* );
typedef int (__stdcall *Proc5ArgStdcall)( void*, void*, void*, void*, void* );

typedef int (__fastcall *ProcNoArgFastcall)( );
typedef int (__fastcall *Proc1ArgFastcall)( void* );
typedef int (__fastcall *Proc2ArgFastcall)( void*, void* );
typedef int (__fastcall *Proc3ArgFastcall)( void*, void*, void* );
typedef int (__fastcall *Proc4ArgFastcall)( void*, void*, void*, void* );
typedef int (__fastcall *Proc5ArgFastcall)( void*, void*, void*, void*, void* );

typedef int (__cdecl *ProcNoArgCdeclcall)( );
typedef int (__cdecl *Proc1ArgCdeclcall)( void* );
typedef int (__cdecl *Proc2ArgCdeclcall)( void*, void* );
typedef int (__cdecl *Proc3ArgCdeclcall)( void*, void*, void* );
typedef int (__cdecl *Proc4ArgCdeclcall)( void*, void*, void*, void* );
typedef int (__cdecl *Proc5ArgCdeclcall)( void*, void*, void*, void*, void* );

typedef int (__thiscall *ProcNoArgThiscall)( );
typedef int (__thiscall *Proc1ArgThiscall)( void* );
typedef int (__thiscall *Proc2ArgThiscall)( void*, void* );
typedef int (__thiscall *Proc3ArgThiscall)( void*, void*, void* );
typedef int (__thiscall *Proc4ArgThiscall)( void*, void*, void*, void* );
typedef int (__thiscall *Proc5ArgThiscall)( void*, void*, void*, void*, void* );



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CLoadDllDlg::CLoadDllDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLoadDllDlg::IDD, pParent)
{
}

void CLoadDllDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DLLPATH, edtDllPath);
	DDX_Control(pDX, IDC_LIST_EXPORTS, lstExports);
	DDX_Control(pDX, IDC_CALL_ENTRYPOINT, rbCallEntrypoint);
	DDX_Control(pDX, IDC_CALL_EXPORT, rbExportedFunction);
	DDX_Control(pDX, IDC_CB_NUMBEROFARGS, cbNumberOfArgs);
	DDX_Control(pDX, IDC_CB_CALLINGCONV, cbCallingConvention);
	DDX_Control(pDX, IDC_ARG1, edtArg[0]);
	DDX_Control(pDX, IDC_ARG2, edtArg[1]);
	DDX_Control(pDX, IDC_ARG3, edtArg[2]);
	DDX_Control(pDX, IDC_ARG4, edtArg[3]);
	DDX_Control(pDX, IDC_ARG5, edtArg[4]);
	DDX_Control(pDX, IDC_CB_ARG1, cbArgType[0]);
	DDX_Control(pDX, IDC_CB_ARG2, cbArgType[1]);
	DDX_Control(pDX, IDC_CB_ARG3, cbArgType[2]);
	DDX_Control(pDX, IDC_CB_ARG4, cbArgType[3]);
	DDX_Control(pDX, IDC_CB_ARG5, cbArgType[4]);
	DDX_Control(pDX, ID_RUN, btnRun);
	DDX_Control(pDX, IDC_LOADFILE, btnLoadFile);
	DDX_Control(pDX, IDC_PAUSE, m_chkPause);
}

BEGIN_MESSAGE_MAP(CLoadDllDlg, CDialog)
	ON_BN_CLICKED(IDC_CALL_ENTRYPOINT, &CLoadDllDlg::OnBnClickedCallEntrypoint)
	ON_BN_CLICKED(IDC_CALL_EXPORT, &CLoadDllDlg::OnBnClickedCallExport)
	ON_CBN_SELCHANGE(IDC_CB_NUMBEROFARGS, &CLoadDllDlg::OnCbnSelchangeCmbNumberofargs)
	ON_BN_CLICKED(ID_RUN, &CLoadDllDlg::OnBnClickedRun)
	ON_NOTIFY(NM_CLICK, IDC_LIST_EXPORTS, &CLoadDllDlg::OnNMClickListExports)
	ON_BN_CLICKED(IDC_LOADFILE, &CLoadDllDlg::OnBnClickedLoadfile)
	ON_BN_CLICKED(IDHELP, &CLoadDllDlg::OnBnClickedHelp)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()



#define ORDINAL_STR			L"<BY_ORDINAL>"

//
// Dialog initialization.
//
BOOL CLoadDllDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (HICON const hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME))
	{
		//SetIcon(hIcon, TRUE);	
		SetIcon(hIcon, FALSE);
	}

	//
	// Set up List View
	//
	lstExports.InsertColumn(0, L"ID", LVCFMT_LEFT, 100 );
	lstExports.InsertColumn(1, L"Function Name", LVCFMT_LEFT, 200);
	lstExports.InsertColumn(2, L"Ordinal", LVCFMT_LEFT, 100);
	lstExports.InsertColumn(3, L"Arguments", LVCFMT_LEFT, 60);
	lstExports.InsertColumn(4, L"Function type", LVCFMT_LEFT, 200);
	lstExports.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP | LVS_EX_ONECLICKACTIVATE);

	//
	// Set Components
	//
	rbCallEntrypoint.SetCheck( TRUE );
	EnableControls( FALSE );
	cbNumberOfArgs.SelectString( 0, L"None" );
	cbCallingConvention.SelectString( 0, L"Stdcall" );

	for ( int i = 0; i < 5; ++i )
	{
		cbArgType[i].SelectString( 0, L"DWORD" );
		edtArg[i].SetWindowText( L"0" );
	}

	//
	// Align column width
	//
	for( int x = 0; x < 4; x++ ) lstExports.SetColumnWidth( x, LVSCW_AUTOSIZE_USEHEADER );


	//Set up the tooltip
	if (m_tooltip.Create(this))
	{
		m_tooltip.SetMaxTipWidth(500);
		m_tooltip.AddTool(this, L"LoadDLL by Esmid Idrizovic");
		m_tooltip.AddTool(&m_chkPause, L"This will call an INT 3 right before the DLL is loaded or the function executed, so don't worry if it looks like LoadDLL has crashed. You can attach with your debugger to LoadDLL and move on with your analysis.");
		m_tooltip.AddTool(&rbCallEntrypoint, L"This will load the DLL directly into memory with a custom LoadLibrary function and will execute all TLS entries and then the DllMain.");
		m_tooltip.AddTool(&rbExportedFunction, L"This will load the DLL using LoadLibrary and will execute the selected function.");
		m_tooltip.AddTool(&btnLoadFile, L"Select your DLL file...");
		m_tooltip.AddTool(&cbCallingConvention, L"You must select the right calling convention of the function.\n\nStdcall: function is going to clean up the stack.\nFastcall: ECX+EDX, rest on stack and function is going to clean up.\nCdecl: you have to clean up the stack.\nThiscall: this-pointer is in ECX, function is going to clean up the stack.");
		m_tooltip.AddTool(&cbNumberOfArgs, L"You can always change the number of the arguments if it's wrong detected by LoadDLL.");
		m_tooltip.Activate(TRUE);
	}

	// accept drag and drop
	DragAcceptFiles(TRUE);

	//
	// load the given argument as a DLL path
	//
	AfxGetApp()->ParseCommandLine(*this);

	return TRUE;
}

void CLoadDllDlg::ParseParam(LPCTSTR pszParam, BOOL bFlag, BOOL bLast)
{
	int const nCurSel = cbNumberOfArgs.GetCurSel();
	if (bFlag)
	{
		int expected = 0;
		if (StrToIntEx(pszParam, STIF_SUPPORT_HEX, &expected))
		{
			int observed = LoadDllAndExecuteFunction();
			EndDialog(observed == expected ? 0 : observed != 0 ? observed : -1);
		}
		else if (cbCallingConvention.SelectString(0, pszParam) == -1)
		{
			cbArgType[nCurSel].SelectString(0, pszParam);
		}
	}
	else if (edtDllPath.GetWindowTextLength() == 0)
	{
		LoadFile(pszParam);
	}
	else if (rbCallEntrypoint.GetCheck())
	{
		rbExportedFunction.SendMessage(BM_CLICK);
		int const n = lstExports.GetItemCount();
		for (int i = 0; i < n; ++i)
		{
			TCHAR text[MAX_PATH];
			lstExports.GetItemText(i, 1, text, _countof(text));
			if (wcscmp(text, pszParam) == 0)
			{
				lstExports.SetItemState(i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
				break;
			}
		}
	}
	else if (nCurSel < 5)
	{
		edtArg[nCurSel].SetWindowText(pszParam);
		cbNumberOfArgs.SetCurSel(nCurSel + 1);
		OnCbnSelchangeCmbNumberofargs();
	}
}

//
// Enable/Disable argument-input controls.
//
void CLoadDllDlg::EnableControls(BOOL fState)
{
	for ( int i = 0; i < 5; ++i )
	{
		edtArg[i].EnableWindow( fState );
		cbArgType[i].EnableWindow( fState );
	}
	cbNumberOfArgs.EnableWindow( fState );
	cbCallingConvention.EnableWindow( fState );
	lstExports.EnableWindow( fState );
}

//
// Call entry point from DLL.
//
void CLoadDllDlg::OnBnClickedCallEntrypoint()
{
	EnableControls( FALSE );
	btnRun.SetWindowText( L"Load DLL" );
	lstExports.DeleteAllItems();
}

//
// Call exported functions.
//
void CLoadDllDlg::OnBnClickedCallExport()
{
	EnableControls( TRUE );
	btnRun.SetWindowText( L"Execute function" );
	EnumerateExportedFunctions();
	OnCbnSelchangeCmbNumberofargs();
}

//
// Enable only needed arguments for function call.
//
void CLoadDllDlg::OnCbnSelchangeCmbNumberofargs()
{
	int const nCurSel = cbNumberOfArgs.GetCurSel();

	for ( int i = 0; i < 5; ++i )
	{
		BOOL const fState = nCurSel > i;
		edtArg[i].EnableWindow( fState );
		cbArgType[i].EnableWindow( fState );
	}
}

//
// Execute function/Load DLL clicked.
//
void CLoadDllDlg::OnBnClickedRun()
{
	if ( rbCallEntrypoint.GetCheck() )
	{
		LoadDll();
	}
	else if ( rbExportedFunction.GetCheck() )
	{
		LoadDllAndExecuteFunction();
	}
}

//
// Returns file offset from virtual address.
//
DWORD64 CLoadDllDlg::GetFOffsetFromRVA( LPBYTE pBuffer, WORD wNumberOfSections, DWORD64 dwAddr )
{
	if ( pBuffer == NULL || dwAddr == 0 )
		return (DWORD64)-1;

	IMAGE_SECTION_HEADER *pSection;
	DWORD64 dwResult = (DWORD64)-1;

	pSection = (IMAGE_SECTION_HEADER *)(pBuffer + ((IMAGE_DOS_HEADER*)pBuffer)->e_lfanew + sizeof(IMAGE_NT_HEADERS));
	for ( WORD n = 0; n < wNumberOfSections; n++ )
	{
		if ( dwAddr >= pSection->VirtualAddress && dwAddr < pSection->VirtualAddress + pSection->Misc.VirtualSize )
		{
			dwResult = (dwAddr - pSection->VirtualAddress) + pSection->PointerToRawData;
			break;
		}
		pSection++;
	}

	return dwResult;
}

//
// Enumerates all exports from a DLL.
//
BOOL CLoadDllDlg::EnumerateExportedFunctions()
{
	int nItem = 0;
	wchar_t szFileName[MAX_PATH +1];
	wchar_t szTemp[ 100 ];
	char * lpszFunctionName = NULL;
	UINT16 wOrdinal = 0, wNumberOfArgs = 0;

	PBYTE pFile = NULL;
	DWORD dwFileSize = 0;
	HANDLE hFile = NULL;
	DWORD dwBytesRead = 0;
	IMAGE_DOS_HEADER* pDos = NULL;
	IMAGE_NT_HEADERS* pNt = NULL;
	IMAGE_EXPORT_DIRECTORY* pExport = NULL;
	DWORD64 dwExportAddress, dwAddressOfFunctions, dwAddressOfNames, dwAddressOfNamesOrdinals, dwFunctionNameOffset, dwFunctionAddress;
	DWORD* lpszExportNameTable = NULL;
	WORD* lpExportOrdinalTable = NULL;
	DWORD* lpExportFunctionTable = NULL;
	WORD wNumberOfPseudoPushArguments = 0;

	lstExports.DeleteAllItems();
	edtDllPath.GetWindowText( szFileName, MAX_PATH );

	hFile = CreateFile( szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0 );
	if ( hFile == INVALID_HANDLE_VALUE )
	{
		return FALSE;
	}

	dwFileSize = GetFileSize( hFile, NULL );
	if ( dwFileSize == 0 || dwFileSize == INVALID_FILE_SIZE )
		goto lblAbort;

	pFile = new BYTE[ dwFileSize ];

	if ( ReadFile(hFile, pFile, dwFileSize, &dwBytesRead, NULL) == FALSE || dwBytesRead != dwFileSize )
		goto lblAbort;

	pDos = (IMAGE_DOS_HEADER*)pFile;
	if ( pDos->e_magic != IMAGE_DOS_SIGNATURE )
		goto lblAbort;

	pNt = (IMAGE_NT_HEADERS*)((char*)pFile + pDos->e_lfanew);
	if ( pNt->Signature != IMAGE_NT_SIGNATURE )
		goto lblAbort;

	//
	// Are there any exported functions?
	//
	if ( pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size == 0 || 
		 pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress == 0 )
		goto lblAbort;

	dwExportAddress = GetFOffsetFromRVA( pFile, pNt->FileHeader.NumberOfSections, pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress );
	if ( dwExportAddress == (DWORD64)-1 )
		goto lblAbort;

	pExport = (IMAGE_EXPORT_DIRECTORY *)((char*)pFile + dwExportAddress);
	
	dwAddressOfFunctions = GetFOffsetFromRVA( pFile, pNt->FileHeader.NumberOfSections, pExport->AddressOfFunctions );
	dwAddressOfNames = GetFOffsetFromRVA( pFile, pNt->FileHeader.NumberOfSections, pExport->AddressOfNames );
	dwAddressOfNamesOrdinals = GetFOffsetFromRVA( pFile, pNt->FileHeader.NumberOfSections, pExport->AddressOfNameOrdinals );
	
	if ( dwAddressOfFunctions == (DWORD64)-1 || dwAddressOfNames == (DWORD64)-1 || dwAddressOfNamesOrdinals == (DWORD64)-1 )
		goto lblAbort;

	lpszExportNameTable = (DWORD*)( (char*)pFile + dwAddressOfNames );
	lpExportOrdinalTable = (WORD*)( (char*)pFile + dwAddressOfNamesOrdinals );
	lpExportFunctionTable = (DWORD*)( (char*)pFile + dwAddressOfFunctions );

	nItem = lstExports.GetItemCount();

	for ( DWORD n = 0; n < pExport->NumberOfNames; n++ )
	{
		//
		// Get exported function name
		//
		dwFunctionNameOffset = (DWORD)lpszExportNameTable[ n ];
		dwFunctionNameOffset = GetFOffsetFromRVA( pFile, pNt->FileHeader.NumberOfSections, dwFunctionNameOffset );

		if ( dwFunctionNameOffset == (DWORD64)-1 )
			lpszFunctionName = "UNKNOWN!";
		else
			lpszFunctionName = (char*)((char*)pFile + dwFunctionNameOffset);

		//
		// Get exported ordinal number
		//
		wOrdinal = lpExportOrdinalTable[ n ];
		dwFunctionAddress = lpExportFunctionTable[ wOrdinal ];

		//
		// Get number of arguments
		//
		dwFunctionAddress = GetFOffsetFromRVA( pFile, pNt->FileHeader.NumberOfSections, dwFunctionAddress );
		if ( dwFunctionAddress == (DWORD64)-1  )
		{
			//
			// Something went wrong, Lol.
			//
			wNumberOfArgs = 0;
		}
		else
		{
			//
			// disassemble our function buffer and find out whats the return code
			//
			PBYTE pPtr = (PBYTE)((char*)pFile + dwFunctionAddress);
			wNumberOfArgs = GetArgNumberFromFunction( pPtr, wNumberOfPseudoPushArguments );
		}

		//
		// Add index
		//
		wsprintf( szTemp, L"%d", nItem );
		if ( lstExports.InsertItem(LVIF_TEXT, nItem, szTemp, 0, 0, 0, NULL) != (int)-1 )
		{
			//
			// Add function name
			//
			wsprintf(szTemp, L"%S", lpszFunctionName);
			lstExports.SetItemText( nItem, 1, szTemp );

			//
			// if the function name starts with an @ we assume its a fastcall, so we have to increment the number of arguments to atleast 2 (eax, ecx are used for first two arguments)
			//
			if ( lpszFunctionName[0] == '@' && wNumberOfArgs != (UINT16)-1 )
				wNumberOfArgs += 2;

			//
			// Add ordinal
			//
			wsprintf( szTemp, L"%d", wOrdinal + pExport->Base );
			lstExports.SetItemText( nItem, 2, szTemp );

			//
			// Add number of arguments
			//
			wsprintf( szTemp, L"%d", wNumberOfArgs != (UINT16)-1 ? wNumberOfArgs : 0 );
			lstExports.SetItemText( nItem, 3, szTemp );

			//
			// Add assumed function type, we can just guess the type
			//
			if ( wNumberOfArgs == 0 )
			{
				if (  wNumberOfPseudoPushArguments == 0 )
				{
					lstExports.SetItemText(nItem, 4, L"Cdecl");
				}
				else
				{
					wsprintf( szTemp, L"Cdecl but found %u push/mov argument[n] in the function", wNumberOfPseudoPushArguments );
					lstExports.SetItemText(nItem, 4, szTemp);
				}
			}
			else if ( lpszFunctionName[0] == '@' )
			{
				lstExports.SetItemText(nItem, 4, L"Fastcall");
			}
			else if ( wNumberOfArgs == (UINT16)-1 )
			{
				lstExports.SetItemText(nItem, 4, L"Exported Variable");
			}
			else
			{
				lstExports.SetItemText(nItem, 4, L"Stdcall");
			}
			nItem++;
		}
	}

lblAbort:
 	delete[] pFile;
	CloseHandle( hFile );

	//
	// Align column width
	//
	for( int x = 0; x < 4; x++ ) lstExports.SetColumnWidth( x, LVSCW_AUTOSIZE_USEHEADER );

	return TRUE;
}

//
// Reads arguments from Text boxes and put them to pData and pArgPtr.
//
BOOL CLoadDllDlg::GetArgument( CEdit &rControl, CComboBox &rComboBox, LPBYTE pData, size_t nLength, PBYTE& pArgPtr )
{
	wchar_t szTemp[ 0x100 ] = { 0 };
	size_t nRet, nBufferSize = nLength;

	//
	// pControl = Argument
	// pComboBox = Argument Type
	// 
	if ( pData == NULL || nLength == 0 )
		return FALSE;

	pArgPtr = NULL;

	nRet = rControl.GetWindowText( szTemp, _countof(szTemp) );
	if ( nRet == 0 )
		return FALSE;

	nLength = __min(nLength, nRet + 1);

	int const nCurSel = rComboBox.GetCurSel();

	if ( nCurSel == -1 )
	{
		//
		// Nothing selected
		//
		rComboBox.SetFocus();
		return FALSE;
	}
	else if ( nCurSel == 0 )
	{
		//
		// DWORD, convert string to number
		//
		DWORD dwValue = 0;
		if ( szTemp[0] == '0' && (szTemp[1] == 'x' || szTemp[1] == 'X') )
		{
			//
			// Hex
			//
			dwValue = wcstol( &szTemp[2], NULL, 16 );
		}
		else
		{
			dwValue = _wtol(szTemp);
		}

		pArgPtr = (PBYTE)dwValue;
	}
	else if ( nCurSel == 1 )
	{
		//
		// STRING
		//
		nLength = WideCharToMultiByte(CP_UTF8, 0, szTemp, (int)nLength, NULL, 0, NULL, NULL);

		if (WideCharToMultiByte(CP_UTF8, 0, szTemp, (int)nLength, (LPSTR)pData, (int)nLength, 0, NULL))
		{
			pArgPtr = pData;
		}
	}
	else if ( nCurSel == 2 )
	{
		//
		// UNICODE STRING
		//
		memcpy(pData, szTemp, __min(nLength*sizeof(wchar_t), nBufferSize));	// max copy nBufferSize, else buffer overflow yey
		pArgPtr = pData;
	}
	else if ( nCurSel == 3 )
	{
		//
		// POINTER TO EMPTY MEMORY ALLOCATION
		//
		memset( pData, 0, nLength );
		pArgPtr = pData;

	}

	return TRUE;
}

BOOL LoadFileInMemory(const wchar_t* szFileName, LPBYTE &pBuffer, DWORD &dwSize)
{
	BOOL fResult = FALSE;

	HANDLE const hFile = CreateFile( szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0 );
	if ( hFile == INVALID_HANDLE_VALUE )
	{
		return fResult;
	}

	DWORD const dwFileSize = GetFileSize( hFile, NULL );
	if ( dwFileSize == 0 || dwFileSize == INVALID_FILE_SIZE )
		goto lblAbort;

	pBuffer = new BYTE[ dwFileSize ];

	DWORD dwBytesRead;
	if ( ReadFile(hFile, pBuffer, dwFileSize, &dwBytesRead, NULL) == FALSE || dwBytesRead != dwFileSize )
		goto lblAbort;

	fResult = TRUE;
	dwSize = dwFileSize;

lblAbort:
	CloseHandle(hFile);
	return fResult;
}


//
// Load DLL and execute main entry point.
//
BOOL CLoadDllDlg::LoadDll( )
{
	HMEMORYMODULE hDLL;
	wchar_t szError[ 0x200 ] = { 0 };
	wchar_t szFileName[MAX_PATH +1] = { 0 };
	BOOL fResult = FALSE;
	LPBYTE pBuffer = NULL;
	DWORD dwSize = 0;

	//
	// Load DLL and execute main entry point.
	//
	edtDllPath.GetWindowText( szFileName, MAX_PATH );

	//
	// do a pause before executed DLL
	//
	if (!LoadFileInMemory(szFileName, pBuffer, dwSize))
	{
		goto lblAbort;
	}

	hDLL = MemoryLoadLibrary(pBuffer, m_chkPause.GetCheck() ? TRUE : FALSE);
	if ( hDLL == NULL )
	{
		wsprintf( szError, L"Can't load DLL \"%s\". Error = %d\n", szFileName, GetLastError() );
		MessageBox( szError, L"Error", MB_ICONERROR );
		goto lblAbort;
	}

	MessageBox( L"DLL loaded. Press OK to unload.", NULL, MB_ICONINFORMATION );

lblAbort:
	if ( hDLL != NULL )
	{
		MemoryFreeLibrary(hDLL);
		fResult = TRUE;
	}
	delete[] pBuffer;
	pBuffer = NULL;

	return fResult;
}

//
// Load DLL and execute exported function.
//
int CLoadDllDlg::LoadDllAndExecuteFunction()
{
#define ARG_ALLOC_SIZE				0x100

	int nFunctionReturn = -1;
	HMODULE hDLL = NULL;
	wchar_t szErrorText[0x100];
	wchar_t szFileName[MAX_PATH];
	wchar_t szFunctionName[0x100] = { 0 };

	BYTE baArg[5][ARG_ALLOC_SIZE] = { 0 };
	PBYTE pArg[5];
	PVOID pFunction = NULL;

	//
	// Load DLL
	//
	edtDllPath.GetWindowText( szFileName, MAX_PATH );
	hDLL = LoadLibraryEx( szFileName, NULL, LOAD_WITH_ALTERED_SEARCH_PATH );
	if ( hDLL == NULL )
	{
		wsprintf( szErrorText, L"Can't load DLL \"%s\". Error = %d\n", szFileName, GetLastError() );
		MessageBox( szErrorText, L"Error", MB_ICONERROR );
		goto lblAbort;
	}

	//
	// Get selected function. If ordinal get ordinal number (decimal).
	//
	if ( !GetTextFromListView(szFunctionName, sizeof(szFunctionName)/sizeof(szFunctionName[0]), 1) )
		goto lblAbort;

	if ( _wcsicmp(szFunctionName, ORDINAL_STR) == 0 )
	{
		if ( !GetTextFromListView(szFunctionName, sizeof(szFunctionName)/sizeof(szFunctionName[0]), 2) )
			goto lblAbort;

		DWORD dwOrdinal = _wtol(szFunctionName);
		pFunction = (PVOID)GetProcAddress( hDLL, (LPCSTR)dwOrdinal );
	}
	else
	{
		char szTemp[0x100];
		int nFunctionLength = WideCharToMultiByte(CP_UTF8, 0, szFunctionName, (int)wcslen(szFunctionName)+1, NULL, 0, NULL, NULL);

		if (WideCharToMultiByte(CP_UTF8, 0, szFunctionName, nFunctionLength, (LPSTR)szTemp, nFunctionLength, 0, NULL))
		{
			pFunction = (PVOID)GetProcAddress( hDLL, szTemp );
		}
	}

	//
	// Check if function can be found.
	//
	if ( pFunction == NULL )
	{
		wsprintf( szErrorText, L"Can't find exported function \"%s\" in file \"%s\". Error = %d\n", szFunctionName, szFileName, GetLastError() );
		MessageBox( szErrorText, L"Error", MB_ICONERROR );
		goto lblAbort;
	}

	//
	// Get all arguments.
	//
	for ( int i = 0; i < 5; ++i )
	{
		GetArgument( edtArg[i], cbArgType[i], baArg[i], sizeof baArg[i], pArg[i] );
	}

	nFunctionReturn = ExecuteFunction( pFunction, pArg[0], pArg[1], pArg[2], pArg[3], pArg[4] );

lblAbort:
	//
	// Unload library.
	//
	if ( hDLL != NULL )
	{
		if ( FreeLibrary(hDLL) == FALSE )
		{
			wsprintf( szErrorText, L"Can't unload DLL. Error = %d\n", GetLastError() );
			MessageBox( szErrorText, L"Error", MB_ICONERROR );
		}
	}

	return nFunctionReturn;
}

//
// Reads selected list view item to lpszBuffer.
//
BOOL CLoadDllDlg::GetTextFromListView( wchar_t * lpszBuffer, int nBufferSize, int nSubItem )
{
	LVITEM lvi = { 0 };

	if ( lpszBuffer == NULL || nBufferSize == 0 )
		return FALSE;

	int nItem = lstExports.GetNextItem( (int)-1, LVNI_SELECTED );
	if ( nItem == (int)-1 )
		return FALSE;

	lvi.iItem = nItem;
	lvi.iSubItem = nSubItem;
	lvi.mask = LVIF_TEXT;
	lvi.pszText = lpszBuffer;
	lvi.cchTextMax = nBufferSize;
	if ( !lstExports.GetItem(&lvi) )
	{
		*lpszBuffer = NULL;
		return FALSE;
	}

	return TRUE;
}

//
// Executes __stdcall function from a DLL.
//
int CLoadDllDlg::ExecuteFunction( PVOID pFunction, PBYTE pArg1, PBYTE pArg2, PBYTE pArg3, PBYTE pArg4, PBYTE pArg5 )
{
	int nFunctionReturn = -1;
	BOOL fException = FALSE;
	int nNumberOfArgsSelection;
	int nCallingConvention;

	//
	// Set last error to zero and execute the function
	//
	SetLastError( 0 );
	nNumberOfArgsSelection = cbNumberOfArgs.GetCurSel();
	nCallingConvention = cbCallingConvention.GetCurSel();

	if ( nNumberOfArgsSelection == 0 )
	{
		//
		// Function with no parameters
		//
		ProcNoArgStdcall	pFunctionStdcall =  (ProcNoArgStdcall)pFunction;
		ProcNoArgFastcall	pFunctionFastcall = (ProcNoArgFastcall)pFunction;
		ProcNoArgCdeclcall	pFunctionCdeclcall = (ProcNoArgCdeclcall)pFunction;
		ProcNoArgThiscall	pFunctionThiscall = (ProcNoArgThiscall)pFunction;

		//
		// call an INT3 if pause is checked
		//
		if ( m_chkPause.GetCheck() ) 
		{
			__debugbreak();
		}
		__try 
		{
			switch (nCallingConvention)
			{
				case 1:
					nFunctionReturn = pFunctionFastcall( );
					break;
				case 2:
					nFunctionReturn = pFunctionCdeclcall( );
					break;
				case 3:
					nFunctionReturn = pFunctionThiscall( );
					break;
				default:
					nFunctionReturn = pFunctionStdcall( );
					break;
			}
		}
		__except ( 1 )
		{ 
			fException = TRUE; 
		}
	}
	else if ( nNumberOfArgsSelection == 1 )
	{
		//
		// Function with 1 parameter
		//
		Proc1ArgStdcall		pFunctionStdcall =   (Proc1ArgStdcall)pFunction;
		Proc1ArgFastcall	pFunctionFastcall =  (Proc1ArgFastcall)pFunction;
		Proc1ArgCdeclcall	pFunctionCdeclcall = (Proc1ArgCdeclcall)pFunction;
		Proc1ArgThiscall	pFunctionThiscall =  (Proc1ArgThiscall)pFunction;

		//
		// call an INT3 if pause is checked
		//
		if ( m_chkPause.GetCheck() ) 
		{
			__debugbreak();
		}

		__try 
		{
			switch (nCallingConvention)
			{
			case 1:
				nFunctionReturn = pFunctionFastcall( pArg1 );
				break;
			case 2:
				nFunctionReturn = pFunctionCdeclcall( pArg1 );
				break;
			case 3:
				nFunctionReturn = pFunctionThiscall( pArg1 );
				break;
			default:
				nFunctionReturn = pFunctionStdcall( pArg1 );
				break;
			}
		}
		__except ( 1 )
		{ 
			fException = TRUE; 
		}
	}
	else if ( nNumberOfArgsSelection == 2 )
	{
		//
		// Function with 2 parameter
		//
		Proc2ArgStdcall		pFunctionStdcall =   (Proc2ArgStdcall)pFunction;
		Proc2ArgFastcall	pFunctionFastcall =  (Proc2ArgFastcall)pFunction;
		Proc2ArgCdeclcall	pFunctionCdeclcall = (Proc2ArgCdeclcall)pFunction;
		Proc2ArgThiscall	pFunctionThiscall =  (Proc2ArgThiscall)pFunction;

		//
		// call an INT3 if pause is checked
		//
		if ( m_chkPause.GetCheck() ) 
		{
			__debugbreak();
		}

		__try 
		{
			switch (nCallingConvention)
			{
			case 1:
				nFunctionReturn = pFunctionFastcall( pArg1, pArg2 );
				break;
			case 2:
				nFunctionReturn = pFunctionCdeclcall( pArg1, pArg2 );
				break;
			case 3:
				nFunctionReturn = pFunctionThiscall( pArg1, pArg2 );
				break;
			default:
				nFunctionReturn = pFunctionStdcall( pArg1, pArg2 );
				break;
			}
		}
		__except ( 1 )
		{ 
			fException = TRUE; 
		}
	}
	else if ( nNumberOfArgsSelection == 3 )
	{
		//
		// Function with 3 parameter
		//
		Proc3ArgStdcall		pFunctionStdcall =   (Proc3ArgStdcall)pFunction;
		Proc3ArgFastcall	pFunctionFastcall =  (Proc3ArgFastcall)pFunction;
		Proc3ArgCdeclcall	pFunctionCdeclcall = (Proc3ArgCdeclcall)pFunction;
		Proc3ArgThiscall	pFunctionThiscall =  (Proc3ArgThiscall)pFunction;

		//
		// call an INT3 if pause is checked
		//
		if ( m_chkPause.GetCheck() ) 
		{
			__debugbreak();
		}

		__try 
		{
			switch (nCallingConvention)
			{
			case 1:
				nFunctionReturn = pFunctionFastcall( pArg1, pArg2, pArg3 );
				break;
			case 2:
				nFunctionReturn = pFunctionCdeclcall( pArg1, pArg2, pArg3 );
				break;
			case 3:
				nFunctionReturn = pFunctionThiscall( pArg1, pArg2, pArg3 );
				break;
			default:
				nFunctionReturn = pFunctionStdcall( pArg1, pArg2, pArg3 );
				break;
			}
		}
		__except ( 1 )
		{ 
			fException = TRUE; 
		}
	}
	else if ( nNumberOfArgsSelection == 4 )
	{
		//
		// Function with 4 parameter
		//
		Proc4ArgStdcall		pFunctionStdcall =   (Proc4ArgStdcall)pFunction;
		Proc4ArgFastcall	pFunctionFastcall =  (Proc4ArgFastcall)pFunction;
		Proc4ArgCdeclcall	pFunctionCdeclcall = (Proc4ArgCdeclcall)pFunction;
		Proc4ArgThiscall	pFunctionThiscall =  (Proc4ArgThiscall)pFunction;

		//
		// call an INT3 if pause is checked
		//
		if ( m_chkPause.GetCheck() ) 
		{
			__debugbreak();
		}

		__try 
		{
			switch (nCallingConvention)
			{
			case 1:
				nFunctionReturn = pFunctionFastcall( pArg1, pArg2, pArg3, pArg4 );
				break;
			case 2:
				nFunctionReturn = pFunctionCdeclcall( pArg1, pArg2, pArg3, pArg4 );
				break;
			case 3:
				nFunctionReturn = pFunctionThiscall( pArg1, pArg2, pArg3, pArg4 );
				break;
			default:
				nFunctionReturn = pFunctionStdcall( pArg1, pArg2, pArg3, pArg4 );
				break;
			}
		}
		__except ( 1 )
		{ 
			fException = TRUE; 
		}
	}
	else if ( nNumberOfArgsSelection == 5 )
	{
		//
		// Function with 5 parameter
		//
		Proc5ArgStdcall		pFunctionStdcall =   (Proc5ArgStdcall)pFunction;
		Proc5ArgFastcall	pFunctionFastcall =  (Proc5ArgFastcall)pFunction;
		Proc5ArgCdeclcall	pFunctionCdeclcall = (Proc5ArgCdeclcall)pFunction;
		Proc5ArgThiscall	pFunctionThiscall =  (Proc5ArgThiscall)pFunction;

		//
		// call an INT3 if pause is checked
		//
		if ( m_chkPause.GetCheck() ) 
		{
			__debugbreak();
		}

		__try 
		{
			switch (nCallingConvention)
			{
			case 1:
				nFunctionReturn = pFunctionFastcall( pArg1, pArg2, pArg3, pArg4, pArg5 );
				break;
			case 2:
				nFunctionReturn = pFunctionCdeclcall( pArg1, pArg2, pArg3, pArg4, pArg5 );
				break;
			case 3:
				nFunctionReturn = pFunctionThiscall( pArg1, pArg2, pArg3, pArg4, pArg5 );
				break;
			default:
				nFunctionReturn = pFunctionStdcall( pArg1, pArg2, pArg3, pArg4, pArg5 );
				break;
			}
		}
		__except ( 1 )
		{ 
			fException = TRUE; 
		}
	}

	//
	// Show information about the executed function
	//
	if (IsWindowVisible())
	{
		ShowExecutedFunctionInformation(nFunctionReturn, fException);
	}
	return nFunctionReturn;
}

//
// Show Information (Return Code/Last Error/Exception/...) about the executed function.
//
void CLoadDllDlg::ShowExecutedFunctionInformation( int nReturn, BOOL fException )
{
	wchar_t szText[ 0x100 ] = { 0 };
	wchar_t* lpszFormat = NULL;
	DWORD dwLastError = GetLastError();

	lpszFormat = fException ? 
		L"Exception occurred. Please check again the arguments (PTR, ...).\n\n"
		L"Return code: %08d (0x%08x)\nLast Error:  %08d (0x%08x)" : 
	L"Return code: %08d (0x%08x)\nLast Error:  %08d (0x%08x)";

	wsprintf( szText, lpszFormat, nReturn, nReturn, dwLastError, dwLastError );
	MessageBox( szText, fException ? L"Exception" : L"Function return", fException ? MB_ICONWARNING : MB_ICONINFORMATION );
}

//
// Disassemble function and get the number of arguments for a function.
//
UINT16 CLoadDllDlg::GetArgNumberFromFunction( PBYTE pAddress, WORD &wNumberOfPseudoPushArguments )
{
#ifdef _WIN64
	DWORD64 dwLength = 0, dwInstructions = 0;
#else
	DWORD dwLength = 0, dwInstructions = 0;
#endif

	int nDisasmType;
	UINT16 wNumberOfArgs = 0;

#ifdef _WIN64
	nDisasmType = 64;	// disassemble as x64 assembler in Win64 compile
#else
	nDisasmType = 32;	// disassemble as x32 assembler in Win32 compile
#endif

	wNumberOfPseudoPushArguments = 0;

	if ( *pAddress == 0x00 )
	{
		return (UINT16)-1;
	}

	while ( TRUE )
	{
		//
		// search for RETN, RET <UINT16>
		//
		if ( *pAddress == 0xC3 )
		{
#ifdef _WIN64
			wNumberOfArgs = 4;	// RCX, RDX, R8, R9
#endif
			break;
		}
		else if ( *pAddress == 0xC2 )
		{
			wNumberOfArgs = *(WORD*)(pAddress + 1);
			wNumberOfArgs /= sizeof(void*);
			break;
		}
		//
		// push [ebp +]
		//
		else if ( *pAddress == 0xFF && *(pAddress + 1) == 0x75 && *(pAddress + 2) >= 0x08 )
		{
			// push from stack ... looks like cdecl function but pushing args
			wNumberOfPseudoPushArguments++;
		}
		//
		// mov ecx, [ebp +]
		// mov eax, [ebp +]
		// mov edx, [ebp +]
		// mov ebx, [ebp +]
		//
		else if ( *pAddress == 0x8B && 
			(*(pAddress +1) == 0x45 || *(pAddress +1) == 0x4d || *(pAddress +1) == 0x55 || *(pAddress+1) == 0x5d) 
			&& *(pAddress + 2) >= 0x08 )
		{
			wNumberOfPseudoPushArguments++;
		}
		else if ( dwInstructions > 0x400 || *pAddress == 0x00 )
		{
			//
			// still nothing found? exit
			//
			break;
		}
		else if ( *pAddress == 0xCC )
		{
			break;
		}
		dwLength = LDE( (DWORD64)pAddress, nDisasmType );
		if ( dwLength == (DWORD64)-1 )
			break;

		dwInstructions++;
		pAddress = (PBYTE)((DWORD64)pAddress + dwLength);
	}

	return wNumberOfArgs;
}

//
// List View item clicked.
//
void CLoadDllDlg::OnNMClickListExports(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	if ( pNMItemActivate )
	{
		wchar_t szArgs[ 20 ];
		if ( GetTextFromListView( szArgs, sizeof(szArgs)/sizeof(szArgs[0]), 3 ) )
		{
			DWORD dwNum = _wtol(szArgs);
			if ( dwNum > 5 )
				wcscpy_s( szArgs, sizeof(szArgs)/sizeof(szArgs[0]), L"5" );

			if ( _wcsicmp(szArgs, L"0") == 0 )
				wcscpy_s( szArgs, sizeof(szArgs)/sizeof(szArgs[0]), L"None" );



			wchar_t szType[100];
			if (GetTextFromListView(szType, sizeof(szType)/sizeof(szType[0]), 4))
			{
				if (_wcsnicmp(L"Stdcall", szType, 7) == 0)
					cbCallingConvention.SetCurSel(0);
				else if (_wcsnicmp(L"Fastcall", szType, 8) == 0)
					cbCallingConvention.SetCurSel(1);
				else if (_wcsnicmp(L"Cdecl", szType, 5) == 0)
				{
					cbCallingConvention.SetCurSel(2);
					wchar_t* szPos = wcsstr(szType, L"Cdecl but found ");
					if ( szPos)
					{
						szPos += wcslen(L"Cdecl but found ");
						const wchar_t* szTemp = wcschr(szPos, ' ');
						memset(szArgs, 0, sizeof(szArgs)/sizeof(szArgs[0]));
						wcsncpy_s(szArgs, szPos, szTemp - szPos);
					}
				}
				else if ( _wcsnicmp(L"Thiscall", szType, 8) == 0 )
				{
					cbCallingConvention.SetCurSel(3);
				}
			}

			cbNumberOfArgs.SelectString( 0, szArgs );


			OnCbnSelchangeCmbNumberofargs();
		}

	}

	*pResult = 0;
}

BOOL CLoadDllDlg::LoadFile(const wchar_t* szFileName)
{
	BOOL fValidDLL = FALSE;

	//
	// Lets see if file is a valid DLL
	//
	fValidDLL = IsValidDLL( szFileName );

	if ( fValidDLL )
	{
		edtDllPath.SetWindowText( szFileName );
		if (  rbExportedFunction.GetCheck() )
		{
			OnBnClickedCallExport();
		}
		else if ( rbCallEntrypoint.GetCheck() )
		{
			OnBnClickedCallEntrypoint();
		}
	}
	else
	{
		EnableControls( FALSE );
	}

	rbExportedFunction.EnableWindow( fValidDLL );
	rbCallEntrypoint.EnableWindow( fValidDLL );
	if (rbExportedFunction.GetCheck())
		lstExports.EnableWindow( fValidDLL );
	btnRun.EnableWindow( fValidDLL );

	return fValidDLL;
}

//
// Show open dialog.
//
void CLoadDllDlg::OnBnClickedLoadfile()
{
	CFileDialog OpenDialog(TRUE, L"DLL", L"", OFN_FILEMUSTEXIST, 
		L"DLL files (*.dll)|*.dll|All files (*.*)|*.*||", this);
	CString cFileName;

	OpenDialog.m_pOFN->lpstrTitle = L"Select DLL file...";
	if( OpenDialog.DoModal() == IDOK )
	{
		cFileName = OpenDialog.GetPathName();
		LoadFile(cFileName.GetString());
	}
}

//
// Is file a valid DLL (PE in Win32 or PE+ in Win64).
//
BOOL CLoadDllDlg::IsValidDLL( const wchar_t * lpszFileName )
{
	wchar_t szErrorText[ MAX_PATH + 0x100 ];
	BYTE baBuffer[ 0x1000 ] = { 0 };
	DWORD dwBytesRead;
	IMAGE_DOS_HEADER* pDos = NULL;
	IMAGE_NT_HEADERS* pNt = NULL;
	BOOL fResult = FALSE;

	HANDLE const hFile = CreateFile( lpszFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );
	if ( hFile == INVALID_HANDLE_VALUE )
	{
		wsprintf( szErrorText, L"Can't find file \"%s\".", lpszFileName );
		MessageBox( szErrorText, L"Error", MB_ICONERROR );
		return fResult;
	}

	if ( ReadFile(hFile, &baBuffer, sizeof(baBuffer), &dwBytesRead, NULL) == FALSE )
	{
		wsprintf( szErrorText, L"Can't read DLL." );
		MessageBox( szErrorText, L"Error", MB_ICONERROR );
		goto lblAbort;
	}

	pDos = (IMAGE_DOS_HEADER*)&baBuffer;
	if ( pDos->e_magic != IMAGE_DOS_SIGNATURE )
	{
		wsprintf( szErrorText, L"File is not a valid DLL." );
		MessageBox( szErrorText, L"Error", MB_ICONERROR );
		goto lblAbort;
	}

	pNt = (IMAGE_NT_HEADERS*)((char*)&baBuffer + pDos->e_lfanew);
	if ( pNt->Signature != IMAGE_NT_SIGNATURE )
	{
		wsprintf( szErrorText, L"File is not a valid DLL." );
		MessageBox( szErrorText, L"Error", MB_ICONERROR );
		goto lblAbort;
	}

#ifdef _WIN64
	if ( pNt->OptionalHeader.Magic != 0x20b )
	{
		wsprintf( szErrorText, L"You must use the x32 version for loading PE files." );
		MessageBox( szErrorText, L"Error", MB_ICONERROR );
	}
#else
	if ( pNt->OptionalHeader.Magic != 0x10b )
	{
		wsprintf( szErrorText, L"You must use the x64 version for loading PE+ files." );
		MessageBox( szErrorText, L"Error", MB_ICONERROR );
	}
#endif
	else if ( !(pNt->FileHeader.Characteristics & IMAGE_FILE_DLL) )
	{
		wsprintf( szErrorText, L"PE is not a DLL file" );
		MessageBox( szErrorText, L"Error", MB_ICONERROR );
	}
	else
	{
		fResult = TRUE;
	}


lblAbort:
	CloseHandle( hFile );
	return fResult;

}


//
// Help
//
void CLoadDllDlg::OnBnClickedHelp()
{
	static wchar_t const szText[] = L"LoadDLL by Esmid Idrizovic, 23. Juni 2014\n\n"
					L"Uses Length Disassembler Engine by BeatriX\n"
					L"Uses MemoryModule by Joachim Bauch\n\n"
					L"Arguments:\n"
					L"DWORD: numeric parameter (start with \"0x\" for hex-numbers)\n"
					L"STR: ASCII string as parameter\n"
					L"STRW: Unicode string as parameter\n"
					L"PTR: Empty memory space (256 bytes)\n\n"
					L"WARNING:\n"
					L"Don't load any suspected DLL on your real system! Use always an virtual machine!\n";

	MessageBox( szText, L"Help", MB_ICONINFORMATION );
}


BOOL CLoadDllDlg::PreTranslateMessage(MSG* pMsg)
{
	m_tooltip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}


void CLoadDllDlg::OnDropFiles(HDROP hDropInfo)
{
	wchar_t szDroppedFile[MAX_PATH] = { 0 };

	if (DragQueryFile(hDropInfo, 0, szDroppedFile, MAX_PATH))
	{
		LoadFile(szDroppedFile);
	}

	CDialog::OnDropFiles(hDropInfo);
}

// LoadDllDlg.h: Headerdatei
//

// CLoadDllDlg-Dialogfeld
class CLoadDllDlg : public CDialog, public CCommandLineInfo
{
// Konstruktion
public:
	CLoadDllDlg(CWnd* pParent = NULL);	// Standardkonstruktor

	enum { IDD = IDD_LOADDLL_DIALOG };

	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void ParseParam(LPCTSTR pszParam, BOOL bFlag, BOOL bLast);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung

	DECLARE_MESSAGE_MAP()

	void EnableControls( BOOL fState );

	CEdit edtDllPath;
	CListCtrl lstExports;
	CButton rbCallEntrypoint;
	CButton rbExportedFunction;
	CComboBox cbNumberOfArgs;
	CComboBox cbCallingConvention;
	CEdit edtArg[5];
	CComboBox cbArgType[5];
	CButton btnRun;
	CButton btnLoadFile;
	CButton m_chkPause;
	CToolTipCtrl m_tooltip;

	BOOL EnumerateExportedFunctions();

	DWORD64 GetFOffsetFromRVA( LPBYTE pBuffer, WORD wNumberOfSections, DWORD64 dwAddr );
	UINT16 GetArgNumberFromFunction( PBYTE pAddress, WORD& wNumberOfPseudoPushArguments );
	BOOL GetArgument( CEdit &rControl, CComboBox &rComboBox, LPBYTE pData, size_t nLength, PBYTE& pArgPtr );
	BOOL LoadDll();
	int LoadDllAndExecuteFunction();

	int ExecuteFunction( PVOID pFunction, PBYTE pArg1, PBYTE pArg2, PBYTE pArg3, PBYTE pArg4, PBYTE pArg5 );
	BOOL GetTextFromListView( wchar_t * lpszBuffer, int nBufferSize, int nSubItem = 1 );
	void ShowExecutedFunctionInformation( int nReturn, BOOL fException = FALSE );

	BOOL IsValidDLL( const wchar_t* lpszFileName );
	BOOL LoadFile( const wchar_t* szFileName );

	// Generierte Funktionen für die Meldungstabellen
	afx_msg void OnBnClickedCallEntrypoint();
	afx_msg void OnBnClickedCallExport();
	afx_msg void OnCbnSelchangeCmbNumberofargs();
	afx_msg void OnBnClickedRun();
	afx_msg void OnNMClickListExports(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedLoadfile();
	afx_msg void OnBnClickedHelp();
	afx_msg void OnDropFiles(HDROP hDropInfo);
};

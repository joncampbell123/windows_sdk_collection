

#define iDefaultReportIntervalSecs  5


BOOL ReportInitializeApplication (void) ;


HWND CreateReportWindow (HWND hWndParent) ;


BOOL ReportInsertLine (HWND hWnd, PLINE pLine) ;


void SetReportPositions (HDC hDC, PREPORT pReport) ;

#if 0
PREPORT ReportData (HWND hWndReport) ;
#endif
#define ReportData(hWndReport)      \
   (&Report)

void SetReportTimer (PREPORT pReport) ;


void PlaybackReport (HWND hWndReport) ;


PLINESTRUCT CurrentReportLine (HWND hWndReport) ;


BOOL AddReport (HWND hWndParent) ;


BOOL EditReport (HWND hWndParent) ;


void ReportTimer (HWND hWnd, BOOL bForce) ;


BOOL ReportRefresh (HWND hWnd) ;
BOOL ToggleReportRefresh (HWND hWnd) ;


BOOL SaveReport (HWND hWndReport, HANDLE hInputFile, BOOL bGetFileName) ;


BOOL OpenReport (HWND hWndReport, 
                 HANDLE hFile, 
                 DWORD dwMajorVersion,
                 DWORD dwMinorVersion,
                 BOOL bReportFile) ;


BOOL PrintReportDisplay (HDC hDC,
                         PREPORT pReport) ;


BOOL QuerySaveReport (HWND hWndParent, 
                      HWND hWndReport) ;

void ResetReport (HWND hWndReport) ;
void ResetReportView (HWND hWndReport) ;


void ClearReportDisplay (HWND hWndReport) ;


BOOL ReportDeleteLine (HWND hWnd, PLINE pLine) ;



BOOL PrintReport (HWND hWndParent,
                  HWND hWndReport) ;

void ExportReport (void) ;

BOOL BuildNewValueListForReport ( void );

void ReportAddAction (PREPORT pReport) ;


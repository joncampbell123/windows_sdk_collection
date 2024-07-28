/*---------------------------- printing stuff -------------------------------*/

BOOL	PASCAL InitPrinting(HDC hDC, HWND hWnd, LPSTR msg);
void	PASCAL TermPrinting(HDC hDC, HWND hWnd);

HDC	PASCAL GetPrinterDC(void);


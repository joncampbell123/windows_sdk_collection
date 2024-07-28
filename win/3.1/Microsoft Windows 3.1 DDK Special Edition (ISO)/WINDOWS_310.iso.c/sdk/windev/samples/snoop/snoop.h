typedef struct tagSYVID
	{
	SYV syv;
	LPSTR lpstr;
	} SYVID;


#define menuSnoop 		1
#define miHold				1000
#define miRelease			1001

#define idGraph			2000
#define idResult			2001

#define ANSI_NEW_LINE	13
#define ANSI_PARA_MARK	182

int PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);
BOOL PASCAL FInitSnoop(HANDLE hInstance, HANDLE hPrevInstance);
BOOL PASCAL FInitInstance(HANDLE hInstance, HANDLE hPrevInstance, int cmdShow);
LONG FAR PASCAL SnoopWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
VOID PASCAL SyvToStr(LPVOID lpData, int cSye, LPSTR lpstr, int cMax, BOOL fSye);
int PASCAL IGestureString(SYV syvGes, LPSTR lpstr, int cMac);
VOID PASCAL RepaintInputData(HDC hdc, HPENDATA hpendataCopy);
BOOL PASCAL FDispatchMenu(HWND hwnd, WORD wParam, LONG lParam);
VOID PASCAL CalcTopology(HWND hwndWindow);
long FAR PASCAL RoHeditProc(HWND hwnd, unsigned message, WORD wParam, long lParam);

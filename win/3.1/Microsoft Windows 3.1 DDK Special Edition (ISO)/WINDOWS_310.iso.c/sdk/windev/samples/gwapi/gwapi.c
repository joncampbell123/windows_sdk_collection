// gwapi.c	DLL illustrates how to get recognition results without messages

/******************* Include-controlling Defines ****************************/
#define NOGDIDCAPMASKS
#define OEMRESOURCE
#define NOMETAFILE
#define NOSOUND
#define NOKANJI
#define NOCOMM
#define NOMINMAX

#define BUILDDLL		// include _loadds in function attr

/******************* Includes ***********************************************/
#include <windows.h>
#include <penwin.h>

/******************* Defines ************************************************/
#define PUBLIC  		FAR PASCAL
#define PRIVATE		NEAR PASCAL

#define fTrue			1
#define fFalse			0

#define szClassGWR	"GWRClass"

/******************* Typedefs ***********************************************/
typedef struct
	{
	LPSTR lpsz;
	int cbSize;
	LPHANDLE lphPendata;
	}
	PARAMINFO, FAR *LPPARAMINFO;

/******************* Macros *************************************************/
#define wRcrtAbort \
	(RCRT_NOSYMBOLMATCH | RCRT_NORECOG | RCRT_ALREADYPROCESSED | RCRT_PRIVATE)

/******************* Module variables ***************************************/
static HANDLE hInstanceDLL = NULL;

/******************* Local procedures ***************************************/
BOOL PRIVATE FInitDLL(HANDLE hInstance);

/******************* Public procedures **************************************/
LONG WINAPI GWRWndProc(HWND, UINT, WPARAM, LPARAM);

// this proto will have to be duplicated in caller:
REC WINAPI GetWriting(LPRC, LPSTR, int, LPHANDLE);

/******************* Export Functions ***************************************/

/*--------------------------------------------------------------------------
PURPOSE: Windows Exit Procedure 
RETURN: UINT 1
CONDITIONS: pro forma
*/
UINT WINAPI WEP(int nParam)
	{
	nParam;	// noref

	return 1;
	}


/*--------------------------------------------------------------------------
PURPOSE: Get writing from user and return string and/or data
RETURN: REC result of recognition
CONDITIONS: creates temp GWRClass window to receive WM_RCRESULT
*/
REC WINAPI GetWriting(
	LPRC lprc,				// initialized RC or NULL
	LPSTR lpsz,				// ptr to buffer for recognized string, or NULL
	int cbSize,				// sizeof lpsz, including NULL terminator
	LPHANDLE lphPendata)	// ptr to handle of pen data, or NULL if not wanted
	{
	RC rc;
	PARAMINFO pi;
	REC rec;

	if (!lprc && !lpsz && !lphPendata)
		return REC_NOINPUT;		// bad parameter combination

	// copy string input parameters into local struct:
	pi.lpsz = lpsz;
	pi.cbSize = cbSize;
	pi.lphPendata = lphPendata;

	// get RC:
	if (lprc)
		rc = *lprc;				// caller's RC
	else
		InitRC(NULL, &rc);	// default RC; we'll set hwnd momentarily

	if (!(rc.hwnd = CreateWindow(
		(LPSTR)szClassGWR,	// send WM_RCRESULT to GWRWndProc()
		(LPSTR)"",				// no text
		WS_POPUP,				// invisible
		0, 0, 0, 0,				// size immaterial
		NULL,						// no parent
		NULL,						// no id or menu
		hInstanceDLL,			// this dll
		NULL)))					// no extra params
			return REC_OOM;	// maybe another problem but we'll report this

	// set up RC options:
	if (lphPendata)
		rc.lRcOptions |= RCO_SAVEHPENDATA;

	// pass address of our info struct in dwAppParam:
	rc.dwAppParam = (DWORD)(LPPARAMINFO)&pi;

	// GWRWndProc will get WM_RCRESULT, and fills param ptrs:
	rec = ProcessWriting(rc.hwnd, &rc);

	DestroyWindow(rc.hwnd);
	return rec;
	}


/*--------------------------------------------------------------------------
PURPOSE: Window proc target for WM_RCRESULT message
RETURN: 
CONDITIONS: This window doesn't get any pen events etc.
*/
LONG WINAPI GWRWndProc(
	HWND hwnd,			// standard params
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
	{
	// we only handle one message:
	if (message == WM_RCRESULT && lParam)		// lParam must be non-NULL
		{
		LPRCRESULT lpr = (LPRCRESULT)lParam;	// ptr to RCRESULT

		if (!(lpr->wResultsType & wRcrtAbort))	// we have a legitimate result
			{
			// get API ptrs from rc.dwAppParam:
			LPPARAMINFO lppi = (LPPARAMINFO)lpr->lprc->dwAppParam;

			// set caller's handle to pen data if requested:
			if (lppi->lphPendata)
				*lppi->lphPendata = lpr->hpendata;

			// fill caller's result [string] buffer if requested:
			if (lppi->lpsz)
				{
				*lppi->lpsz = '\0';	// preterminate for safety

				if (lpr->wResultsType & RCRT_GESTURE)
					// pass gestures back in first 2 bytes - this could 
					//	also be expanded (if room) to include hotspots...
					{
					if (lpr->cSyv == 1						// sanity checks
						&& lpr->lpsyv != NULL
						&& (lppi->cbSize > sizeof(SYV)))	// if room
						{
						*(LPSYV)lppi->lpsz = *lpr->lpsyv;
						lppi->lpsz[sizeof(SYV)] = '\0';	// null terminate
						}
					}

				else	// take recognizer's best guess
					{
					int cb = lpr->cSyv;	// doesn't include null-term

					if (lppi->cbSize -1 < lpr->cSyv)
						cb = lppi->cbSize -1;
					SymbolToCharacter(lpr->lpsyv, cb, lppi->lpsz, NULL);
					lppi->lpsz[cb] = '\0';	// null terminate
					}
				}
			}
		return 1L;
		}

	return DefWindowProc(hwnd, message, wParam, lParam);
	}

/******************* Public Functions ***************************************/

/*--------------------------------------------------------------------------
PURPOSE: Entry point called from libentry.asm
RETURN: BOOL success
CONDITIONS: 
*/
int PUBLIC LibMain(
	HANDLE hInstance,		// instance handle of DLL
	UINT wDataSeg,			// (unused)
	UINT cbHeapSize,		// heapsize must be > 0
	LPSTR lpszCmdLine)	// ptr to command line (unused)
	{
	// noref:
	wDataSeg;
	lpszCmdLine;

	if (!cbHeapSize)
		return fFalse;

	UnlockData(0);
	return FInitDLL(hInstance);	// also sets global hInstanceDLL
	}

/******************* Private Functions **************************************/

/*--------------------------------------------------------------------------
PURPOSE: Register Input Window class
RETURN: TRUE
CONDITIONS: called from LibMain at init
*/
BOOL PRIVATE FInitDLL(HANDLE hInstance)
	{
	WNDCLASS wndClass;

	wndClass.hCursor = LoadCursor(NULL, IDC_PEN);
	wndClass.hIcon = NULL;
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = (LPSTR)szClassGWR;
	wndClass.hbrBackground = NULL;
	wndClass.hInstance = hInstanceDLL = hInstance;	// set global
	wndClass.style = CS_SAVEBITS;
	wndClass.lpfnWndProc = GWRWndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	RegisterClass((LPWNDCLASS) &wndClass);	// may be FALSE if already reg'd

	return fTrue;
	}

#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>
#include <toolhelp.h>
#include "dbwindll.h"

// Max buffer size
#define     CCHBUFFERMAX 8192

BOOL CALLBACK LibMain(HINSTANCE hinst, UINT wDS, UINT cbHeap, DWORD unused);
BOOL CALLBACK _export _loadds WEP(BOOL fSystemExit);

// Private entry points

BOOL CALLBACK _export _loadds NotifyCallback(WORD id, DWORD dwData);

// Buffered output

BOOL BufferOut(LPCSTR lpszOut);
UINT DBStrCpy(char* pszDst, LPCSTR pszSrc, int far* pcLines);

// Monochrome output

BOOL MonoQuery(void);
BOOL MonoOpen(void);
BOOL MonoClose(void);
BOOL MonoOut(LPCSTR lpsz);
BOOL MonoInUse(void);

// COM output

BOOL ComQuery(int port);
BOOL ComOpen(int port);
BOOL ComClose(void);
BOOL ComOut(LPCSTR lpsz);
int  ComIn(void);

// Notification formatting

void FormatLogError(char FAR* lpch, NFYLOGERROR FAR* lple);
void FormatLogParamError(char FAR* lpch, NFYLOGPARAMERROR FAR* lplpe);
void FormatRip(LPSTR lpsz, NFYRIP FAR* lprip);

int _cdecl dbprintf(LPSTR lpszOut, LPCSTR lpszFmt, ...);

int GetProcName(LPSTR lpch, FARPROC lpfn);
int GetTaskName(LPSTR lpch, HTASK htask);
LPSTR DecOut(LPSTR lpch, short value);
LPSTR HexOut(LPSTR lpch, WORD value);

// Task filter

BOOL CheckTaskFilter(void);
void TaskFilterStartTask(void);
void TaskFilterExitTask(void);

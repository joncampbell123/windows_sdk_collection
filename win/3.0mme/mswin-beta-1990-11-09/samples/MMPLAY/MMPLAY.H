/* MMPLAY.H -- Header file for sample movie player app.
 */

#define IDM_OPEN 101
#define IDM_CLOSE 102
#define IDM_EXIT 103
#define IDM_ABOUT 104

#define IDM_START 201
#define IDM_STOP 202
#define IDM_REWIND 203
#define IDM_LOOPMOVIE 204
#define IDM_FULLSCREEN 205

#define IDM_STEPFORWARD 301
#define IDM_STEPBACKWARD 302
#define IDM_FRAMECTR 303

#define WM_FRAME_CTR (WM_USER+1)
#define WM_MONITOR_TICK (WM_USER+2)


int PASCAL WinMain ( HANDLE, HANDLE, LPSTR, int );
HWND AppInit ( HANDLE, HANDLE );

LONG FAR PASCAL MainWndProc ( HWND, unsigned, WORD, LONG );
LONG HandleCommands ( HWND, int, WORD, LONG );
LONG FAR PASCAL FullScreenWndProc ( HWND, unsigned, WORD, LONG );
LONG FAR PASCAL CounterWndProc ( HWND, unsigned, WORD, LONG );
BOOL FAR PASCAL AppAbout( HWND, unsigned, WORD, LONG );

BOOL FAR PASCAL MonitorFrame(MMPID id, short sFrame, short sSubFrame);

BOOL PASCAL CreateFullScreenStage ( HWND );
BOOL OpenMovie ( HWND );
void SetMenuStates ( HWND, WORD );
void PrintError( LPSTR );


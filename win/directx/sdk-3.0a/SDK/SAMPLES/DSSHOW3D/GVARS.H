#ifndef __GVARS_H__
#define __GVARS_H__

#ifdef __cplusplus
extern "C" {
#endif

class FileInfo;
class MainWnd;
class FileInfo3D;
class ListenerInfo;

#include <dsound.h>
#include "DSShow3d.h"
#include "MainWnd.h"

// Make sure to change this if you add format code entries to fdFormats[]
#define NUM_FORMATENTRIES   16

#ifdef INIT_GVARS

char    gszAppName[]        = "DSShow3D";
char    gszAppCaption[]     = "DirectSound 3D Mixing Test";
char    gszAppWndClass[]    = "DSShow3D_MainApp_WndClass";

FORMATDATA   fdFormats[] = { { 8108, ID_OPTIONS_FORMAT_8M8, TRUE },
                { 8116, ID_OPTIONS_FORMAT_8M16, TRUE },
                { 8208, ID_OPTIONS_FORMAT_8S8, TRUE },
                { 8216, ID_OPTIONS_FORMAT_8S16, TRUE },
                { 11108, ID_OPTIONS_FORMAT_11M8, TRUE },
                { 11116, ID_OPTIONS_FORMAT_11M16, TRUE },
                { 11208, ID_OPTIONS_FORMAT_11S8, TRUE },
                { 11216, ID_OPTIONS_FORMAT_11S16, TRUE },
                { 22108, ID_OPTIONS_FORMAT_22M8, TRUE },
                { 22116, ID_OPTIONS_FORMAT_22M16, TRUE },
                { 22208, ID_OPTIONS_FORMAT_22S8, TRUE },
                { 22216, ID_OPTIONS_FORMAT_22S16, TRUE },
                { 44108, ID_OPTIONS_FORMAT_44M8, TRUE },
                { 44116, ID_OPTIONS_FORMAT_44M16, TRUE },
                { 44208, ID_OPTIONS_FORMAT_44S8, TRUE },
                { 44216, ID_OPTIONS_FORMAT_44S16, TRUE } };


DWORD aFormatOrder[] = { 44216, 44116, 44208, 44108, 22216, 22116, 22208, 22108,
                11216, 11116, 11208, 11108, 8216, 8116, 8208, 8108 };

HINSTANCE   ghInst;
HWND        hWndMain        = NULL;
HWND        ghDlgActive     = NULL;
DWORD       gdwTimer        = 0;        // Timer handle.
DWORD       gcbMaxWaveFormatSize= 0;
DWORD       gdwOutputFormat = 0;
BOOL        gfCOMInitialized    = FALSE;
REGSETTINGS grs;

PWAVEFORMATEX   gpwfxFormat = NULL;
ListenerInfo *  gpListenerInfo = NULL;
HWND        ghwndListener = NULL;

LPDIRECTSOUND       gpds = NULL;
LPDIRECTSOUNDBUFFER gpdsbPrimary = NULL;
LPDIRECTSOUND3DLISTENER  gp3DListener = NULL;

MainWnd         AppWnd;

#else   // INIT_GVARS

extern char gszAppName[];
extern char gszAppCaption[];
extern char gszAppWndClass[];

extern FORMATDATA   fdFormats[];
extern DWORD        aFormatOrder[];

extern HINSTANCE    ghInst;
extern HWND     hWndMain;
extern HWND     ghDlgActive;
extern DWORD        gdwTimer;           // Timer handle.
extern DWORD        gcbMaxWaveFormatSize;
extern DWORD        gdwOutputFormat;
extern BOOL     gfCOMInitialized;
extern REGSETTINGS  grs;

extern PWAVEFORMATEX    gpwfxFormat;
extern ListenerInfo *   gpListenerInfo;
extern HWND     ghwndListener;

extern LPDIRECTSOUND        gpds;
extern LPDIRECTSOUNDBUFFER  gpdsbPrimary;
extern LPDIRECTSOUND3DLISTENER   gp3DListener;

extern MainWnd          AppWnd;

#endif  // INIT_GVARS


#ifdef __cplusplus
}
#endif

#endif  // __GVARS_H__



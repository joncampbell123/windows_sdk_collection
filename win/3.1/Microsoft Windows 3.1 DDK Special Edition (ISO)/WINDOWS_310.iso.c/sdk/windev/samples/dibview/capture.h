/*
 *  capture.h
 *
 */

/* Handle to a DIB */
#define HDIB HANDLE

/* Print Area selection */
#define PW_WINDOW        1
#define PW_CLIENT        2


/* Function prototypes */
HDIB    CopyWindowToDIB(HWND, WORD);
HDIB    CopyScreenToDIB(LPRECT);
WORD    DestroyDIB(HDIB);
HWND    SelectWindow(void);
HANDLE  CaptureWindow (HWND hWndParent, BOOL bCaptureClient);
HANDLE  CaptureFullScreen (HWND hWndParent);
BOOL    ToggleCaptureHide (void);




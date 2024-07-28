/*
    Rotary.h
*/

// resource ids

#define IDR_CONTROL     200

// control window dimensions

#define ROTARY_WIDTH    100     // rotary control width
#define ROTARY_HEIGHT   100     // rotary control height

// define the control range

#define MIN_UNITS       1
#define MAX_UNITS       11

// colors for things

#define BLUE RGB(0, 0, 255) 
#define RED  RGB(255, 0, 0)
#define GREEN RGB(0, 255, 0)

// private data structure for each rotary window instance

typedef struct _RINFO {
    WORD wMin;              // minimum control value
    WORD wMax;              // maximum control value
    WORD wPos;              // current control value
    HBITMAP hbmBkGnd;       // background bitmap image
    char szUnits[16];       // label for units (e.g. Hz, WPM ...)
} RINFO, *NPRINFO;

// messages to rotary controls

#define RC_BASE     WM_USER
#define RC_SETMIN   RC_BASE+1
#define RC_SETMAX   RC_BASE+2
#define RC_SETPOS   RC_BASE+3
#define RC_GETPOS   RC_BASE+4
#define RC_SETUNITS RC_BASE+5
#define RC_LAST     RC_BASE+5

// macros to talk to rotary controls

#define rcSetMin(hWnd, w) SendMessage(hWnd, RC_SETMIN, w, NULL)
#define rcSetMax(hWnd, w) SendMessage(hWnd, RC_SETMAX, w, NULL)
#define rcSetPos(hWnd, w) SendMessage(hWnd, RC_SETPOS, w, NULL)
#define rcGetPos(hWnd) SendMessage(hWnd, RC_GETPOS, NULL, NULL)
#define rcSetUnits(hWnd,szUnits) SendMessage(hWnd, RC_SETUNITS, 0, (LONG)(LPSTR)(szUnits))

// messages from rotary controls

#define RCN_BASE    RC_LAST+1
#define RCN_DRAG    RCN_BASE+1
#define RCN_RELEASE RCN_BASE+2


/* (c) Copyright 1987 MICROGRAFX, Inc., All Rights Reserved.  Permission to
   use this work for any purpose must be obtained in writing from MICROGRAFX,
   1820 N. Greenville Ave., Richardson, Tx.  75081.

*******************************************************************************

                                   dialog

*******************************************************************************

*/

#define COLORPRO     100
#define COLORPROGEC  101
#define HP7470A      102
#define HP7475A      103
#define HP7550A      104
#define HP7580A      105
#define HP7585A      106
#define HP7580B      107
#define HP7585B      108
#define HP7586B      109
#define DRAFTPRO     110
#define DMU          111
#define EMU          112
#define DRAFTMASTER1 113
#define DRAFTMASTER2 114
#define YELLOW       120
#define ORANGE       121
#define RED          122
#define GREEN        123
#define RED_VIOLET   124
#define AQUA         125
#define BROWN        126
#define VIOLET       127
#define BLUE         128
#define BLACK        129
#define NO_COLOR     130
#define P3           140
#define P7           141
#define T3           142
#define T6           143
#define V25          144
#define V35          145
#define V50          146
#define V70          147
#define R3           148
#define D018         149
#define D025         150
#define D035         151
#define D050         152
#define D070         153
#define D100         154
#define SIZE_A       560
#define SIZE_B       561
#define SIZE_C       562
#define SIZE_D       563
#define SIZE_E       564
#define SIZE_A4      565
#define SIZE_A3      566
#define SIZE_A2      567
#define SIZE_A1      568
#define SIZE_A0      569
#define ROLL_24      570
#define ROLL_36      571
#define SPEED        180
#define FORCE        181
#define ACCELERATION 182
#define USAGE        183
#define ABOUT        184

#define MANUAL       213
#define AUTOMATIC    214
#define PRELOADED    215
#define PORTRAIT     216
#define LANDSCAPE    217

#define NONE         240
#define LINESTEXT    241
#define FILL         242

#define DRAFT        259

#define CM_SECOND    300
#define GRAMS        301
#define GRAVITY      302
#define CAROUSEL     303

#define DIALOG_BUSY  400
#define PROP_CAPTION 401

#define PAGE_MSG1     402
#define PAGE_MSG2     403

// dialog box control id's
#define ICO_PORTRAIT          1
#define ICO_LANDSCAPE         2

#define IDD_PAPERSIZE_LABEL   100
#define IDD_PAPERSIZE         101
#define IDD_PAPERSOURCE_LABEL 102
#define IDD_PAPERSOURCE       103
#define IDD_PROMPT            104
#define IDD_ORIENT_GROUP      105
#define IDD_ORIENT_ICON       106
#define IDD_ORIENT_PORTRAIT   107
#define IDD_ORIENT_LANDSCAPE  108
#define IDD_DEFAULT           109
#define IDD_LENGTH_LABEL      110
#define IDD_LENGTH            111
#define IDD_LENGTH_LABEL2     112
#define IDD_LABEL_1           115
#define IDD_LABEL_2           116
#define IDD_LABEL_3           117
#define IDD_LABEL_4           118
#define IDD_LABEL_5           119
#define IDD_LABEL_6           120
#define IDD_LABEL_7           121
#define IDD_LABEL_8           122
#define IDD_LABEL_9           123
#define IDD_LABEL_10          124
#define IDD_LABEL_11          125
#define IDD_LABEL_12          126
#define IDD_LABEL_13          127
#define IDD_LABEL_14          128
#define IDD_LABEL_15          129
#define IDD_LABEL_16          130
#define IDD_LABEL_17          131

#define IDD_CURCARSEL         200
#define IDD_ACTCARSEL         201
#define IDD_COLORS            202
#define IDD_TYPES             203
#define IDD_PRIORITY          204
#define IDD_SPEEDBOX          205
#define IDD_SPEEDSB           206
#define IDD_FORCEBOX          207
#define IDD_FORCESB           208
#define IDD_ACCELBOX          209
#define IDD_ACCELSB           210
#define IDD_CARSELGRP         211
#define IDD_CARSELLB          212
#define IDD_DRAFT             213
#define IDD_PENRESET          215
#define IDD_SPEEDFRAME        216
#define IDD_FORCEFRAME        217
#define IDD_ACCELFRAME        218

#define DI_PAPER              0
#define DI_DEVICE             1
#define DI_MAX                2

#define DLG_PAPERS            100
#define DLG_DEVICE_OPTIONS    101

// data structures
typedef struct
{
    short Min,
          Max,
          Increment;
} OPTIONINFO;

typedef struct
{
    BOOL       PenTypeSupport[MAX_PEN_TYPES],
               MediaSizeSupport[MAX_MEDIA_SIZES],
               OptionsSupport[NUM_OPTIONS],
               PaperFeedSupport;

    short      PensInCarousel;
    OPTIONINFO Speed,
               Force,
               Acceleration;
} DEVICEINFO;

#define _MAX_PATH 260
#define _MAX_PORT  32


typedef struct tagDIALOGINFO
{
  LPENVIRONMENT   lpDM;            // Pointer to current devmode
  LPSTR           lpProfile;       // Pointer to lpdi->szProfile, or NULL
  LPENVIRONMENT   lpDMTarget;      // Where to copy final devmode
  WORD            wMode;           // Current wMode from IGlExtDeviceMode
  HANDLE          hResTable;       // Handle to minidriver resources
  HINSTANCE       hInst;           // Instance of driver
  HINSTANCE       hInstShell;      // Instance of shell2.dll
  HINSTANCE       hInstCommon;     // Instance of commctrl.dll
  ATOM            aBlock;          // Atom to provide block
  BOOL            bExtDevMode;     // TRUE iff ExtDeviceMode(PropSheet) call
  BOOL            bInitialized;    // TRUE iff fully initialized
  BOOL            bOK;             // True iff OK was pressed
  char            szDevName[CCHDEVICENAME];  // (Real) Device Name
  char            szPort[_MAX_PORT];  // Port connected (NULL for ASD)
  char            szProfile[_MAX_PATH];  // Profile name (generally NULL)
  WORD            wFlags[DI_MAX];  // Bitflags indicating controls to keep.
  DWORD           dwUser[DI_MAX];  // 32 bits of private data for each dialog
  ENVIRONMENT     dmOrig;          // Original DEVMODE from environment/profile
  ENVIRONMENT     dmMerge;         // Devmode after caller's stuff is merged
  ENVIRONMENT     dmScratch;       // Working copy of devmode--UI changes this
} DIALOGINFO, *PDIALOGINFO, FAR *LPDIALOGINFO, FAR *LPDI;

BOOL FAR PASCAL PaperDlgProc(HWND,UINT,WPARAM,LPARAM);
BOOL FAR PASCAL GraphicsDlgProc(HWND,UINT,WPARAM,LPARAM);
BOOL FAR PASCAL FontsDlgProc(HWND,UINT,WPARAM,LPARAM);
BOOL FAR PASCAL DeviceOptionsDlgProc(HWND,UINT,WPARAM,LPARAM);
BOOL FAR PASCAL about (HANDLE,unsigned,WORD,DWORD);

void InitPapersDlg(HWND,LPDI);
VOID SetOrientation(HWND, WORD, LPDI);
void CheckPaperFeed(HWND);
void SetPaperDefaults(HWND);
BOOL ValidateRollLength(HWND);
void CheckPaperSize(HWND);
void DisplayRollWindows(HWND,BOOL);

void InitDevOptsDlg(HWND, LPDI);
void SetDevOptsDefaults(HWND, LPDI);
void display_pen(HWND,int,LPDI);
void display_pens(HWND,LPDI);
void display_colors(HWND, LPDI);
void display_priorities(HWND,LPDI);
void display_SFA(HWND, int, LPDI);
void UpdateSpinCtrl(HWND, WPARAM, LPARAM);
short FAR PASCAL get_group(short, LPENVIRONMENT);

extern DEVICEINFO DeviceInfo [NUM_PLOTTERS];

/*==========================================================================
 *
 *  Copyright (C) 1995-1996 Microsoft Corporation. All Rights Reserved.
 *
 *  File:       DSShow3D.h
 *  Content:    DirectSound Mixing Test with 3D sound main header file
 *
 ***************************************************************************/
#ifndef __DSSHOW3D_H__
#define __DSSHOW3D_H__

#define QQUOTE(x) #x
#define QUOTE(y) QQUOTE(y)
#define REMIND(str) __FILE__ "(" QUOTE(__LINE__) ") : " str

#include <mmsystem.h>
#include <dsound.h>
#include <d3dtypes.h>

#include "resource.h"
#include "debug.h"

#define MAXCONTROLS             200 // An insanely large number of controls

// To check for stopping of sounds, a timer is set...use this for the rate.
#define TIMERPERIOD     300 // In milliseconds


#define OPENFILENAME_F_LOCALFOCUS   0x00000001
#define OPENFILENAME_F_STICKYFOCUS  0x00000002
#define OPENFILENAME_F_GLOBALFOCUS  0x00000004

#define OPENFILENAME_F_GETPOS       0x00000008
#define OPENFILENAME_F_GETPOS2      0x00000010

#define OPENFILENAME_F_3D       0x00000020


#define MAXVOL_VAL  0
#define MIDPAN_VAL  0

// Registry Keys and Values
#define REG_SETTINGS_KEY        TEXT("Software\\Microsoft\\DSShow3D")
#define REG_SETTING_DEVICE_DEFAULT  TEXT("UseDSoundDefault")
#define REG_SETTING_USE_EXCLUSIVE   TEXT("UseExclusiveMode")
#define REG_SETTING_OUTPUT_FORMAT   TEXT("PreferredOutputFormat")
#define REG_SETTING_FOCUS_FLAG      TEXT("Focus")
#define REG_SETTING_OPEN3D      TEXT("DefaultOpen3D")
#define REG_SETTING_INITIAL_DIR     TEXT("InitialDirectory")
#define REG_SETTING_DEVICE_GUID     TEXT("PreferredDevice")
#define REG_DIRECT3D_KEY        TEXT("Software\\Microsoft\\Direct3D")
#define REG_D3DPATH_VAL         TEXT("D3D Path")


typedef struct tag_rs
{
    BOOL    fDefaultDevice;     // Use the DSound default device?
    BOOL    fOpen3D;            // Default to opening in 3D?
    BOOL    fUseExclusiveMode;      // Use exclusive mode?
    DWORD   dwDefaultFocusFlag;     // The DSBCAPS flag for the default focus
    DWORD   dwPreferredFormat;      // Preferred output format
    char    szInitialDir[MAX_PATH]; // Initial open directory
    GUID    guPreferredDevice;      // GUID of preferred device, if not default
} REGSETTINGS, *PREGSETTINGS;


typedef struct tag_fd
{
    DWORD   dwCode;
    WORD    wCommandID;
    BOOL    fEnable;
} FORMATDATA, *PFORMATDATA;

///////////////////////////////////////////////////////////////////////
// Function prototypes
//

BOOL CALLBACK DSEnumProc( LPGUID, LPSTR, LPSTR, LPVOID );
BOOL CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK SettingsDlgProc(HWND, UINT, WPARAM, LPARAM);
UINT CALLBACK FileOpenCustomTemplateDlgProc(HWND, UINT, WPARAM, LPARAM);

BOOL OpenFileDialog(HWND, LPSTR, int *, LPDWORD);
BOOL IsValidWave(LPSTR);

BOOL FormatCodeToWFX( DWORD, PWAVEFORMATEX );
BOOL FormatCodeToText( DWORD, LPSTR, int );

DWORD FormatCodeFromCommandID( WORD );
WORD CommandIDFromFormatCode( DWORD );
void DisableFormatCode( DWORD );
void EnableFormatCode( DWORD );


#endif  // __DSSHOW3D_H__

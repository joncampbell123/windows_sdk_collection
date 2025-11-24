//************************************************************************
//**
//**  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//**  ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED
//**  TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR
//**  A PARTICULAR PURPOSE.
//**
//**  Copyright (C) 1993 - 1995 Microsoft Corporation. All Rights Reserved.
//**
//**  globals.h
//**
//**  DESCRIPTION:
//**
//**
//**  HISTORY:
//**     04/22/93       created.
//**
//************************************************************************

#ifndef __GLOBALS_H
#define __GLOBALS_H

#define STRICT
#include "preclude.h"
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include "idf.h"

//
// The following macro defines a CODE based pointer to data, which
// is used for constants that do not need to be placed in the read/write
// DATA segment.
//
#ifdef _WIN32
#define __based(a)
#endif

#ifdef _WIN32
   #define  BCODE
#else
   #define  BCODE                __based(__segname("_CODE"))
#endif

#define  FNLOCAL                    NEAR  PASCAL
#define  FNGLOBAL                   FAR   PASCAL
#ifdef _WIN32
#define  FNEXPORT                   FAR   PASCAL
#else
#define  FNEXPORT                   FAR   PASCAL   __export
#endif
#define  CURR_CHANNEL(x)            gpInst[gdwCurrInst].alpChannels[x]
#define  CURR_CHANNEL_TYPE(x)       gpInst[gdwCurrInst].aChannelTypes[x]
#define  CURR_PATCH_MAP(x)          gpInst[gdwCurrInst].aPatchMaps[x]
#define  CURR_KEY_MAP(chnl, map)    gpInst[gdwCurrInst].aKeyMaps[chnl][map]

#define  HWND_CURRENT_INSTRUMENT    GetDlgItem(ghwndMain, ID_CURRENT_INSTRUMENT)
#define  HWND_DRUM_CHANNELS         GetDlgItem(ghwndMain, ID_DRUM_CHANNELS)
#define  HWND_EDIT_ENTER            GetDlgItem(ghwndMain, ID_EDIT_ENTER)
#define  HWND_LIST_BOX              GetDlgItem(ghwndMain, ID_LIST_BOX)

#define  HWND_IDF_CURRENT_SECTION   GetDlgItem(ghwndMain, ID_IDF_CURRENT_SECTION)
#define  HWND_CURRENT_INSTRUMENT    GetDlgItem(ghwndMain, ID_CURRENT_INSTRUMENT)
#define  HWND_EDIT_BOX              GetDlgItem(ghwndMain, ID_EDIT_BOX)
#define  HWND_INSTRUMENT_SCROLL     GetDlgItem(ghwndMain, ID_INSTRUMENT_SCROLL)

#define  DWORD_ROUND(x)             (((x)+3L)&~3L)


//
// GENERAL CONSTANT DEFINITIONS
//
#define  MAX_STR_LEN             128
#define  MAX_PATH_LEN            144
#define  MAX_TITLE_LEN           14
#define  NUM_CHANNELS_DEFINED    3
#define  DRUM_CHANNEL            9
#define  MAX_CHANNELS            16
#define  MAX_PATCHES             128
#define  MAX_KEY_MAPS            128
#define  DRUM_KEY_START          35
#define  DRUM_KEY_END            81
#define  HEX_DIGITS_TO_SHOW      4   

//
// VERSION
//
#define  IDF_FMT_VERSION         0x00000100

//
// SECTIONS
//
#define  IDF_HEADER_INFO         0
#define  IDF_INSTRUMENT_INFO     1
#define  IDF_INSTRUMENT_CAPS     2
#define  IDF_CHANNEL_TYPE        3
#define  IDF_PATCH_MAPS          4
#define  IDF_KEY_MAPS            5

#define  TOTAL_SECTION_ENTRIES   6

//
// HEADER LIST BOX POSITIONS
//
#define  IDF_VERSION             0
#define  IDF_CREATOR             1

#define  HEADER_TOTAL            2

//
// INSTRUMENT LIST BOX POSITIONS
//
#define  INST_MANUFACT           0
#define  INST_PRODUCT            1
#define  INST_MID                2
#define  INST_PID                3
#define  INST_REV                4

#define  INST_TOTAL              5

//
// CHANNEL CAP LIST BOX POSITIONS
//
#define  CCAP_GENERAL            0
#define  CCAP_DRUM               1
#define  CCAP_GENERAL_INIT       2
#define  CCAP_DRUM_INIT          3

#define  CCAP_TOTAL              4


//
// MIDI CAPABILITIES LIST BOX POSITIONS
//
#define  CAPS_BASECHANL          0
#define  CAPS_NUMCHANL           1
#define  CAPS_INSTPOLY           2
#define  CAPS_CHANLPOLY          3
#define  CAPS_GENERAL            4
#define  CAPS_SYSEX              5

#define  CAPS_TOTAL              6

// 
// CHANNEL TYPES
//
#define  CHAN_GENERAL            0
#define  CHAN_DRUM               1
#define  CHAN_TYPES              2


//
// ID's for things in the channel types box. Note: these are *NOT*
// positions since not all items are always displayed.
//
#define CT_IS_GENERAL            0
#define CT_GENERAL_INIT_STR      1
#define CT_IS_DRUM               2 
#define CT_DRUM_INIT_STR         3                   

// 
// NEW TYPE DEFINITIONS
//
typedef struct tag_CHANNEL
{
   DWORD             fdwChannel;
   DWORD             cbGeneralInitData;
   LPSTR             lpGeneralInitData;
   DWORD             cbDrumInitData;
   LPSTR             lpDrumInitData;
} CHANNEL, *PCHANNEL, FAR *LPCHANNEL;

typedef struct tag_INSTRUMENT
{
   DWORD             dwVersion;
   DWORD             dwCreator;

   char              szInstID[MAX_STR_LEN+1];

   DWORD             dwManufactID;
   DWORD             dwProductID;
   DWORD             dwRevision;

   LPSTR             pszManufactASCII;
   LPSTR             pszManufactUNICODE;
   LPSTR             pszProductASCII;
   LPSTR             pszProductUNICODE;

   DWORD             dwBasicChannel;
   DWORD             cNumChannels;
   DWORD             cInstPoly;
   DWORD             cChannelPoly;
   DWORD             fdwFlags;

   LPCHANNEL         alpChannels[MAX_CHANNELS];
   BYTE              aPatchMaps[MAX_PATCHES];
   BYTE              aKeyMaps[CHAN_TYPES][MAX_KEY_MAPS];
} INSTRUMENT, FAR *LPINSTRUMENT;

//
// EXTERNAL FUNCTION PROTOTYPES
//

extern LRESULT  FNEXPORT MainProc(HWND    hwnd, 
                                  UINT    umsg, 
                                  WPARAM  wParam, 
                                  LPARAM  lParam);
extern VOID FNLOCAL InitializeDialog(void);


extern LPVOID   FNLOCAL  AllocPtr(UINT fu, DWORD cb);
extern LPVOID   FNLOCAL  ReAllocPtr(LPVOID pv, UINT fu, DWORD cb);
extern VOID     FNLOCAL  FreePtr(LPVOID FAR *ptr);

extern VOID     FNLOCAL  NotifyUser(UINT u);
extern MMRESULT FNLOCAL  NewConfigFile(VOID);
extern MMRESULT FNLOCAL  OpenConfigFile(VOID);
extern MMRESULT FNLOCAL  SaveConfigFile(VOID);
extern MMRESULT FNLOCAL  SaveConfigFileAs(VOID);
extern MMRESULT FNLOCAL  NewInstrument(VOID);
extern VOID     FNLOCAL  SetupDialog(VOID);

extern VOID     FNLOCAL  CleanUp(VOID);
extern VOID     FNLOCAL  SetWindowTitle(VOID);
extern VOID     FNLOCAL  UpdateListBox(VOID);
extern BOOL     FNLOCAL  AllocateInst(VOID);

extern VOID     FNLOCAL  UpdateInstrument(VOID);



//
// Constant read-only strings (allocated from code segment).
//
extern   char  BCODE    gszClassName[];
extern   char  BCODE    gszSS[];
extern   char  BCODE    gszSU[];
extern   char  BCODE    gszUS[];
extern   char  BCODE    gszUU[];
extern   char  BCODE    gszSX[];
extern   char  BCODE    gszU[];
extern   char  BCODE    gszNULL[];
extern   char  BCODE    gszHexByte[];
extern   char  BCODE    gszEllipsis[];

//
// GLOBAL VARIABLE DECLARATIONS
//
extern   char           gszTitleBar[];
extern   char           gszIDFTitle[];
extern   char           gszIDFName[];
extern   HINSTANCE      ghinst;
extern   HWND           ghwndMain;
extern   UINT           guCurrSelection;
extern   HMMIO          ghmmio;
extern   LPINSTRUMENT   gpInst;
extern   DWORD          gdwNumInsts;
extern   DWORD          gdwCurrInst;
extern   BOOL           gfChanged;
extern   char           gszbuf[];

extern   char           gszChannelTypes[][20];
extern   char           gszYes[];
extern   char           gszNo[];
extern   char           gszGeneralInit[];
extern   char           gszDrumInit[];
extern   BOOL           gfFirstEdit;

#endif // __GLOBALS_H


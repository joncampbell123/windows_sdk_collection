/**[f******************************************************************
* lclstr.h -
*
* Copyright (C) 1990,1991 Hewlett-Packard Company.
*     All rights reserved.
*     Company confidential.
*
**f]*****************************************************************/
  
/*
 * $Header: 
 */

/*
 * $Log:
 */

#define IDOK                 1
#define CANCEL               2
  
#define ID_ABOUT             100
  
#define ID_FACE_LIST         103
#define CURRENT_TF           106
  
#define ID_PT_BASE           114
#define ID_PT_12             114
#define ID_PT_24             115
#define ID_PT_72             116
#define ID_PT_96             117
  
#define PCLEO_RESULT         118
#define ID_PT_OTHER          118
#define ID_ROTATION          125
  
  
#define ID_SET_BASE          119
#define ID_SET_12            119
#define ID_SET_24            120
#define ID_SET_72            121
#define ID_SET_96            122
  
  
#define ID_SET_OTHER         123
#define ID_BOLDING           124
#define ID_ITALIC            128
  
  
#define ID_SS_BASE           130
#define ID_SS_DT             130
#define ID_SS_ASCII          131
#define ID_SS_WIN            132
#define ID_SS_E1             133
#define ID_SS_PC             134
#define ID_SS_DS             135
  
/* O B S O L E T E
#define ID_INSTALL           140
#define ID_DELETE            141
*/
  
#define ID_PCL_BASE          150   /* 08-29-90  jfd */
#define ID_PCL_SIZE          150   /* 08-29-90  jfd */
#define ID_PCL_FILE          151   /* 08-29-90  jfd */
  
#define IDI_FACES            200
#define IDI_INSTALL          201
#define IDI_CANCEL           202
#define IDI_SCROLL           203
#define IDI_TEXTDRIVE        204
#define IDI_READDRIVE        205
  
#define IDD_FACES            300
#define IDD_CANCEL           301
#define IDD_DELETE           302
  
/* main menu */
#define IF_DEMO              10
#define CLEAR_SCRN           20
#define CACHE_DEMO           30
#define LOADER_DEMO          40
#define TEXT_FILE            50
#define PCLEO_DEMO           60    /* 08-29-90  jfd */
  
  
/* Open File Control IDs */
#define     IDC_FILENAME  400
#define     IDC_EDIT      401
#define     IDC_FILES     402
#define     IDC_PATH      403
#define     IDC_LISTBOX   404
  
// Resolution
  
#define     IDC_XRES      501
#define     IDC_YRES      502
  
  
#define         MAX_LINE 100           /* config() */
#define     pi        3.14159265
#define     MAXLINE   80
#define     MAXCODES  2000
#define     MAXFILESIZE 0x7fff     /* maximum file size that can be loaded */
  
  
LPSTR FAR PASCAL lmemcpy(LPSTR, LPSTR, WORD);
LPSTR FAR PASCAL lmemset(LPSTR, BYTE, WORD);
  
/*** Routines in mypcleo.c ***/
  
BOOL     PASCAL    config_bullet(PIFCONFIG);
BOOL FAR PASCAL    WritePcleo(HANDLE, LPSTR, LPSTR, DWORD, LPSTR);
BOOL FAR PASCAL    init_intellifont(HANDLE);
VOID     PASCAL    shut_down_bullet(HANDLE,BOOL);
BOOL FAR PASCAL    build_pcleo_name(HANDLE, DWORD, LPSTR);

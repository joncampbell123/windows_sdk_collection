/**[f******************************************************************
* generic.h -
*
* Copyright (C) 1988,1989 Aldus Corporation
* Copyright (C) 1989-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
* All rights reserved.  Company confidential.
*
**f]*****************************************************************/
  
#define PRD
#define HPPCL
//#define DEBUG
  
#include "printer.h"
#include "gdidefs.inc"
#include "hppcl.h"
#include "debug.h"
#include "pfm.h"
#include "paperfmt.h"
#include "device.h"
  
#define DEVICENAME  "hppcl.exe"
#define SYM_BOLD
  
#define SF_LABEL    "SOFTFONT"
#define CART_LABEL  "CARTRIDGE"
  
#define MS_FLAG 0
#define PRINTER_TYPE 5
#define RESET_B
#define RESET_E
#define RESET_E_M 0
#define FORMLEN_F_M 1
#define LINESP_F
#define LINESP_F_M 4
#define CHARSP_F
#define CHARSP_F_M 8
#define BOLD_B
#define BOLD_B_M 0
#define BOLD_E
#define ITALLIC_B
#define ITALLIC_B_M 0
#define ITALLIC_E
#define UNDERL_B
#define UNDERL_B_M 0
#define UNDERL_E
#define STRIKEOUT_B_M 2
#define DUNDERL_B
#define DUNDERL_B_M 2
#define DUNDERL_E
#define SUPERS_B
#define SUPERS_B_M 0
#define SUPERS_E
#define SUBS_B
#define SUBS_B_M 0
#define SUBS_E
  
int FAR PASCAL OffsetClipRect(LPRECT, short, short);
  
// sjc - ms 7-90
//WORD __WinFlags=KERNEL 178;
extern WORD __WinFlags;
#define WinFlags ((WORD)(&__WinFlags))
#define ProtectMode (WinFlags & 1)
#define RealMode (!ProtectMode)
#define StandardMode (ProtectMode && (WinFlags & 0x0010))
BYTE global_grayscale;
BYTE global_brighten;
  

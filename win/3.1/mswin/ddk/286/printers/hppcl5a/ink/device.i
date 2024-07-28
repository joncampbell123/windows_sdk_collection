; **[f******************************************************************
; * device.i -
; *
; * Copyright (C) 1988-1989 Aldus Corporation
; * Copyright (C) 1988-1990 Microsoft Corporation.
; * Copyright (C) 1989-1990, 1991 Hewlett-Packard Company.
; *             All rights reserved.
; * Company confidential.
; *
; **f]*****************************************************************/
  
;/*********************************************************************
; *
; *  20 sep 91  SD       Removed epByteFill to force epBmp to word align.
; *  15 aug 91  SD       BUG 560:  Added BYTE field epPgChng to LPDEVICE to 
; *                      monitor changes to orientation, paper tray, and 
; *                      paper size that may occur within a document.
; *  11 feb 91  SD      ELI -- added epRearTray and epOffset to DEVICE structure.
; *  30 jan 90  VO      Added epLastSize to the DEVICE structure.
; *  18 dec 89  SD      Added second printer capabilities field to
; *                     DEVICE structure.
; *
; *  05 sep 89  peterbe Added epLineBuf to DEVICE.
; *   1-18-89    jimmat  Now space for epBuf is only allocated if the printer
; *          is in landscape mode.
; */
  
;/*
; */
  
;/*
; Revision 3.603  90/10/25  17:12:33  17:12:33  oakeson (Ken Oakeson)
; Uncommented truetype fields
  
; Revision 3.602  90/08/24  13:21:03  13:21:03  daniels (Susan Daniels)
; ../message.txt
  
; Revision 3.601  90/08/14  15:24:57  15:24:57  oakeson (Ken Oakeson)
; Added TrueType support
  
; Revision 3.600  90/08/03  11:11:26  11:11:26  stevec (Steve Claiborne)
; This is the Aug. 3 release ver. 3.600
  
; Revision 3.550  90/07/27  11:32:43  11:32:43  root ()
; Experimental freeze 3.55
  
; Revision 3.540  90/07/25  12:37:16  12:37:16  stevec (Steve Claiborne)
; Experimental freeze 3.54
  
; Revision 3.522  90/07/21  10:58:38  10:58:38  stevec (Steve Claiborne)
; New vars for huge bitmaps
  
; Revision 3.520  90/06/14  08:24:53  08:24:53  root ()
; This is version 3.520
  
; Revision 3.510  90/06/13  15:33:24  15:33:24  root ()
; 5_0_release
  
  
;      Rev 1.1   20 Feb 1990 15:53:26   daniels
;   Support for downloadables.
; */
  
NAME_LEN    equ 32          ;These constants
SPOOL_SIZE  equ 2048            ;   are defined in
MAX_BAND_WIDTH  equ 3510            ;   HPPCL.H
BYTESIZE    equ 8
  
DEV_LAND    equ 8889H           ;defined in DEVICE.H
  
HPJET       equ 0001H           ;defined in RESOURCE.H
NOBITSTRIP  equ 0040H
DINA4       equ 21
B5      equ 23
  
NULL        equ 0           ;These constants
TRUE        equ 1           ;   are defined in
FALSE       equ 0           ;   PRINTER.H
SUCCESS     equ 0
OPTIONS_DPTEKCARD equ 1             ;defined in RESOURCE.H
OPTIONS_RESETJOB  equ 2
OPTIONS_FORCESOFT equ 4
OPTIONS_VERTCLIP  equ 8
  
FAILED      equ -1          ;defined here only for DUMP.A
    NUMARRAYS   equ 24          ; increased from 10 for LJ2000
        MINBYTES    equ 32
  
  
        PAPERFORMAT     struc           ;This data structure
        ;   is defined in
        xPhys       dw  0       ;   DEVICE.H
yPhys       dw  0
xImage      dw  0
yImage      dw  0
xPrintingOffset dw  0
yPrintingOffset dw  0
select      db  16  dup (0)
  
PAPERFORMAT     ends
  
  
JUSTBREAKREC        struc           ;This data structure
;   is defined in
extra       dw  0       ;   DEVICE.H
rem     dw  0
err     dw  0
count       dw  0
ccount      dw  0
  
JUSTBREAKREC        ends
  
; NOTE  : IMPORTANT - when adding fields to this structure, you MUST ensure
;        that epBmp is on a word boundry.  This can be arrived at by adding
;        up the number of bytes associated with each field element, or can
;        be quessed at by assuming the structure is correct and componsating
;        for ONLY the new fields.  If epBmp is on an odd byte boundry,
        ;        simply uncomment the epByteFill field.  If epBmp is on an even byte
    ;        boundry, comment out the epByteFill field.  The simptom of incorrect
;        boundry calculation is printing "tick marks" on the right hand side
;        of a page in real mode at all DPI's.  Do exactly the same changes to
;        device.h.
  
DEVICE          struc           ;This data structure
;   is defined in
epType          dw  0       ;   DEVICE.H
epBmpHdr        db  SIZE BITMAP     dup(0)
epPF            db  SIZE PAPERFORMAT    dup(0)
ephDC           dw  0
epMode          dw  0
epNband         dw  0
epXOffset       dw  0
epYOffset       dw  0
epJob           dw  0
epDoc           dw  0
epPtr           dw  0
epXerr          dw  0
epYerr          dw  0
ephMd           dw  0
epECtl          dw  0
epLastSize      dw  0       ; /*** Tetra II ***/
  
epCurTTFont     dw  0       ; TrueType support fields
epNextTTFont    dw  0    
epTTRaster      db  0	    ; for TT as raster 6-19 dtk
epTTFSum        dw  0       ; handle to TrueType font summary - dtk
epFontBmpMem    dw  0
  
epCurx          dw  0
epCury          dw  0
epBandDepth     dw  0
epNumBands      dw  0
epLastBandSz    dw  0
epScaleFac      dw  0
epCopies        dw  0
epTray          dw  0
epPaper         dw  0
epPgChng        db  0       ; TRUE = page setup has changed during document
epPubTrans      db  0       ; TRUE = add pub chars to WN symset
epFontSub       db  0
epPgSoftNum     dw  0
epTotSoftNum    dw  0
epMaxPgSoft     dw  0
epMaxSoft       dw  0
epGDItext       db  0
epOpaqText      db  0
epGrxRect       db  SIZE RECT       dup (0)
epRearTray      db  0       ;ELI
epOffset        db  0       ;ELI
epCaps          dw  0
epCaps2         dw  0
epOptions       dw  0
epDuplex        dw  0
epPageCount     dw  0
epAvailMem      dd  0
epFreeMem       dd  0
epTxWhite       dw  0
epHFntSum       dw  0       ;HANDLE from PRINTER.H
epLPFntSum      dd  0       ;LPSTR  from PRINTER.H
epJust          dw  0       ;JUSTBREAKTYPE
epJustWB        db  SIZE JUSTBREAKREC   dup (0)
epJustLTR       db  SIZE JUSTBREAKREC   dup (0)
epHWidths       dw  0       ;HANDLE from PRINTER.H
epDevice        db  NAME_LEN        dup (0)
epPort          db  NAME_LEN        dup (0)
epSpool         db  SPOOL_SIZE      dup (0)
epBuf           dw  0       ; was a buffer, now an OFFSET
epLineBuf       dw  0       ; special graphics buffer
;epByteFill      db  0
epBmp           db  0
  
DEVICE          ends
  
  
ESCTYPE         struc           ;This data structure
;   is defined in HPPCL.H
escx        db  0       ;esc is a "reserved" word in
start1      db  0       ;  5.0 asm. expanded to escx.
start2      db  0
num     db  10  dup (0)
  
ESCTYPE         ends
  
ESCtypeSIZE equ SIZE ESCTYPE
  
  
POSARRAY    struc               ;defined here only for DUMP.A
  
    startpos    dw  0
    endpos      dw  0
  
    POSARRAY    ends
  
    PosArraySIZE    equ SIZE POSARRAY

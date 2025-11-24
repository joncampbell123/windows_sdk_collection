/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1993 - 1995  Microsoft Corporation.  All Rights Reserved.
 * 
 *  MCIQA.H
 *
 *  Sample MCI Device Driver
 *
 *      Driver include file
 *
 ***************************************************************************/

extern HANDLE hModuleInstance;

/* up to four open devices are supported by this driver */
#define MCIQA_MAX_CHANNELS  4

/* used for debug output */
#define MCINAME             "MCIQA:"

/* successful MCI return code */
#define MCIERR_NO_ERROR     0

/* marks invalid channel number */
#define MCIQA_NO_CHANNEL    (-1)

/* return string for product ID */
#define MCIQA_PRODUCT       "MCI Sample Driver"

/* processes all MCI specific messages */
extern DWORD FAR PASCAL mqMCIProc (WORD wDeviceID, WORD wMessage,
                                   DWORD dwParam1, DWORD dwParam2);

/* initialize a channel */
extern BOOL mqInitialize (void);

/****************************************************************************

    memory management macros

 ***************************************************************************/

HANDLE __H;

#define MAKEP(sel,off)      ((LPVOID)MAKELONG(off,sel))
#define GHandle(sel)        (LOWORD(GlobalHandle(sel)))
#define GSelector(h)        (HIWORD((DWORD)GlobalLock(h)))

#define GAllocSelF(f,ulBytes) ((__H=GlobalAlloc(f,(LONG)(ulBytes))) ? GSelector(__H) : NULL )
#define GAllocPtrF(f,ulBytes) MAKEP(GAllocSelF(f,ulBytes),0)
#define GAllocPtr(ulBytes)    GAllocPtrF(GMEM_MOVEABLE,ulBytes)

#define GFreeSel(sel)      (GlobalUnlock(GHandle(sel)),GlobalFree(GHandle(sel)))
#define GFreePtr(lp)       GFreeSel(HIWORD((DWORD)(lp)))

/****************************************************************************

    Debug support

 ***************************************************************************/

#ifdef DEBUG
    extern void NEAR dprintf(PSTR szFormat, ...);
    extern void NEAR dout(PSTR szOutput);
#else
    #define dprintf if (0) ((int (*)(char *, ...)) 0)
    #define dout(string)
#endif

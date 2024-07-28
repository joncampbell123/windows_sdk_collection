/****************************************************************************
 *
 *   MSRLEC.H
 *
 ***************************************************************************/
/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1992, 1993  Microsoft Corporation.  All Rights Reserved.
 * 
 **************************************************************************/

typedef struct {
    DWORD       fccHandler;
} RLESTATE, far *LPRLESTATE, *PRLESTATE;

typedef struct {
    RLESTATE    RleState;
} RLEINST, * PRLEINST;

/****************************************************************************
 ***************************************************************************/

#ifdef _INC_COMPDDK

BOOL        NEAR PASCAL RleLoad(void);
void        NEAR PASCAL RleFree(void);
PRLEINST    NEAR PASCAL RleOpen(ICOPEN FAR * incifo);
DWORD       NEAR PASCAL RleClose(PRLEINST pri);

BOOL        NEAR PASCAL RleQueryAbout(RLEINST * pinst);
DWORD       NEAR PASCAL RleAbout(RLEINST * pinst, HWND hwnd);
BOOL        NEAR PASCAL RleQueryConfigure(RLEINST * pinst);
DWORD       NEAR PASCAL RleConfigure(RLEINST * pinst, HWND hwnd);


DWORD       NEAR PASCAL RleGetState(PRLEINST pri, LPVOID pv, DWORD dwSize);
DWORD       NEAR PASCAL RleSetState(PRLEINST pri, LPVOID pv, DWORD dwSize);

DWORD       NEAR PASCAL RleGetInfo(PRLEINST pri, ICINFO FAR *icinfo, DWORD dwSize);

LRESULT     NEAR PASCAL RleCompressBegin(PRLEINST pri, LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
LRESULT     NEAR PASCAL RleCompressQuery(PRLEINST pri, LPBITMAPINFOHEADER lpbiIn,LPBITMAPINFOHEADER lpbiOut);
DWORD       NEAR PASCAL RleCompressGetFormat(PRLEINST pri, LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
LRESULT     NEAR PASCAL RleCompress(PRLEINST pri,ICCOMPRESS FAR *icinfo, DWORD dwSize);
DWORD       NEAR PASCAL RleCompressGetSize(PRLEINST pri, LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
LRESULT     NEAR PASCAL RleCompressEnd(PRLEINST lpri);


LRESULT     NEAR PASCAL RleDecompressGetPalette(RLEINST * pinst, LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
LRESULT     NEAR PASCAL RleDecompressBegin(PRLEINST pri, LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
LRESULT     NEAR PASCAL RleDecompressQuery(PRLEINST pri, LPBITMAPINFOHEADER lpbiIn,LPBITMAPINFOHEADER lpbiOut);
DWORD       NEAR PASCAL RleDecompressGetFormat(PRLEINST pri, LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
LRESULT     NEAR PASCAL RleDecompress(PRLEINST pri,ICDECOMPRESS FAR *icinfo, DWORD dwSize);
LRESULT     NEAR PASCAL RleDecompressEnd(PRLEINST pri);

BOOL        NEAR PASCAL RleDrawQuery(RLEINST * pinst, LPBITMAPINFOHEADER lpbiInput);
LRESULT     NEAR PASCAL RleDrawBegin(PRLEINST pri,ICDRAWBEGIN FAR *icinfo, DWORD dwSize);
LRESULT     NEAR PASCAL RleDraw(PRLEINST pri,ICDRAW FAR *icinfo, DWORD dwSize);
LRESULT     NEAR PASCAL RleDrawEnd(PRLEINST pri);

#endif

/****************************************************************************

 DEBUGing macros

 ***************************************************************************/

#ifdef DEBUG
    extern void FAR CDECL dprintf(LPSTR, ...);
    #define DPF dprintf
#else
    #define DPF / ## /
#endif

/****************************************************************************
 *
 *   drvproc.c
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

#include <windows.h>
#include <mmsystem.h>
#include <compddk.h>
#include "msrlec.h"

HMODULE ghModule;

/***************************************************************************
 ***************************************************************************/

LRESULT FAR PASCAL _loadds DriverProc(DWORD dwDriverID, HDRVR hDriver, UINT uiMessage, LPARAM lParam1, LPARAM lParam2)
{
    PRLEINST pri = (PRLEINST)(WORD)dwDriverID;

    if (dwDriverID && dwDriverID != -1)
        pri = (PRLEINST)(WORD)dwDriverID;
    else
        pri = NULL;
    
    switch (uiMessage)
    {
        case DRV_LOAD:
            return (LRESULT)RleLoad();

        case DRV_FREE:
            RleFree();
            return (LRESULT)1L;

        case DRV_OPEN:
            // if being opened with no open struct, then return a non-zero
            // value without actually opening
            if (lParam2 == 0L)
                return -1;

            return (LRESULT)(DWORD)(WORD)RleOpen((ICOPEN FAR *) lParam2);

        case DRV_CLOSE:
            if (pri)
                RleClose(pri);

            return (LRESULT)1L;

        /*********************************************************************

            Configure/Info messages

        *********************************************************************/

        case DRV_QUERYCONFIGURE:    // configuration from drivers applet
            return (LRESULT)0L;

        case DRV_CONFIGURE:
            return DRV_OK;

        case ICM_CONFIGURE:
            //
            //  return ICERR_OK if you will do a configure box, error otherwise
            //
            if (lParam1 == -1)
                return RleQueryConfigure(pri) ? ICERR_OK : ICERR_UNSUPPORTED;
            else
                return RleConfigure(pri, (HWND)lParam1);

        case ICM_ABOUT:
            //
            //  return ICERR_OK if you will do a about box, error otherwise
            //
            if (lParam1 == -1)
                return RleQueryAbout(pri) ? ICERR_OK : ICERR_UNSUPPORTED;
            else
                return RleAbout(pri, (HWND)lParam1);

        /*********************************************************************

            state messages

        *********************************************************************/

        case ICM_GETSTATE:
            return RleGetState(pri, (LPVOID)lParam1, (DWORD)lParam2);

        case ICM_SETSTATE:
            return RleSetState(pri, (LPVOID)lParam1, (DWORD)lParam2);

        case ICM_GETINFO:
            return RleGetInfo(pri, (ICINFO FAR *)lParam1, (DWORD)lParam2);

        /*********************************************************************
        *********************************************************************/

        case ICM_GETQUALITY:
        case ICM_SETQUALITY:
        case ICM_GETDEFAULTQUALITY:
            return ICERR_UNSUPPORTED;

        /*********************************************************************
        *********************************************************************/

        case ICM_GETDEFAULTKEYFRAMERATE:
            return ICERR_UNSUPPORTED;
            
        /*********************************************************************

            compression messages

        *********************************************************************/

        case ICM_COMPRESS_QUERY:
            return RleCompressQuery(pri,
                         (LPBITMAPINFOHEADER)lParam1,
                         (LPBITMAPINFOHEADER)lParam2);

        case ICM_COMPRESS_BEGIN:
            return RleCompressBegin(pri,
                         (LPBITMAPINFOHEADER)lParam1,
                         (LPBITMAPINFOHEADER)lParam2);

        case ICM_COMPRESS_GET_FORMAT:
            return RleCompressGetFormat(pri,
                         (LPBITMAPINFOHEADER)lParam1,
                         (LPBITMAPINFOHEADER)lParam2);

        case ICM_COMPRESS_GET_SIZE:
            return RleCompressGetSize(pri,
                         (LPBITMAPINFOHEADER)lParam1,
                         (LPBITMAPINFOHEADER)lParam2);

        case ICM_COMPRESS:
            return RleCompress(pri,
                            (ICCOMPRESS FAR *)lParam1, (DWORD)lParam2);

        case ICM_COMPRESS_END:
            return RleCompressEnd(pri);

        /*********************************************************************

            decompress messages

        *********************************************************************/

        case ICM_DECOMPRESS_QUERY:
            return RleDecompressQuery(pri,
                         (LPBITMAPINFOHEADER)lParam1,
                         (LPBITMAPINFOHEADER)lParam2);

        case ICM_DECOMPRESS_BEGIN:
            return RleDecompressBegin(pri,
                         (LPBITMAPINFOHEADER)lParam1,
                         (LPBITMAPINFOHEADER)lParam2);

        case ICM_DECOMPRESS_GET_FORMAT:
            return RleDecompressGetFormat(pri,
                         (LPBITMAPINFOHEADER)lParam1,
                         (LPBITMAPINFOHEADER)lParam2);

        case ICM_DECOMPRESS_GET_PALETTE:
            return RleDecompressGetPalette(pri,
                         (LPBITMAPINFOHEADER)lParam1,
                         (LPBITMAPINFOHEADER)lParam2);

        case ICM_DECOMPRESS:
            return RleDecompress(pri,
                         (ICDECOMPRESS FAR *)lParam1, (DWORD)lParam2);

        case ICM_DECOMPRESS_END:
            return RleDecompressEnd(pri);

        /*********************************************************************

            draw messages

        *********************************************************************/

        case ICM_DRAW_QUERY:
            return RleDrawQuery(pri,(LPBITMAPINFOHEADER)lParam1)
                ? ICERR_OK : ICERR_UNSUPPORTED;

        case ICM_DRAW_BEGIN:
            return RleDrawBegin(pri,(ICDRAWBEGIN FAR *)lParam1, (DWORD)lParam2);

        case ICM_DRAW:
            return RleDraw(pri,(ICDRAW FAR *)lParam1, (DWORD)lParam2);

        case ICM_DRAW_END:
            return RleDrawEnd(pri);

        /*********************************************************************

            standard driver messages

        *********************************************************************/

        case DRV_DISABLE:
        case DRV_ENABLE:
            return (LRESULT)1L;

        case DRV_INSTALL:
        case DRV_REMOVE:
            return (LRESULT)DRV_OK;
    }

    if (uiMessage < DRV_USER)
        return DefDriverProc(dwDriverID, hDriver, uiMessage,lParam1,lParam2);
    else
        return ICERR_UNSUPPORTED;
}

/****************************************************************************
 ***************************************************************************/
int CALLBACK LibMain(HMODULE hModule, WORD DataSeg, WORD wHeapSize, LPSTR lpCmdLine)
{
    ghModule = hModule;

    return 1;
}

BOOL CALLBACK _loadds WEP(BOOL fSystemExit)
{
    return TRUE;
}

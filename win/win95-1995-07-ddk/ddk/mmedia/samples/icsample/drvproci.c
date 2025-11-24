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
 *  Copyright (c) 1992 - 1995  Microsoft Corporation.  All Rights Reserved.
 * 
 **************************************************************************/

#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <compddk.h>
#include "icsample.h"

HMODULE ghModule;

//
//  we use this driverID to determine when we where opened as a video
//  device or from the control panel, etc....
//
#define BOGUS_DRIVER_ID     1

LRESULT CALLBACK DriverProcVideo(DWORD dwDriverID, HDRVR hDriver, UINT uiMessage, LPARAM lParam1, LPARAM lParam2);

//
//  if you want to have multiple compressors in a single module add them
//  to this list, for example you can have a video capture module and a
//  video codec in the same module.
//
DRIVERPROC  DriverProcs[] = {
    DriverProcVideo,  // proc for Video data

    // add other procedures here...

    NULL              // FENCE: must be last
};

/***************************************************************************
 ***************************************************************************/

LRESULT CALLBACK _loadds DriverProc(DWORD dwDriverID, HDRVR hDriver, UINT uiMessage, LPARAM lParam1, LPARAM lParam2)
{
    INSTINFO *pi;
    int	    i;
    LRESULT dw;

    if ( (dwDriverID == BOGUS_DRIVER_ID) || (dwDriverID == 0))
        pi = NULL;
    else
        pi = (INSTINFO *)(UINT)dwDriverID;
    
    switch (uiMessage)
    {
        case DRV_LOAD:
            DPF("DRV_LOAD");

            /*
               Sent to the driver when it is loaded. Always the first
               message received by a driver.

               dwDriverID is 0L. 
               lParam1 is 0L.
               lParam2 is 0L.
                
               Return 0L to fail the load.

            */

            // put global initialization here...

	    // Pass to all driver procs
            for(i=0; DriverProcs[i]; i++)
            {
                if (DriverProcs[i](dwDriverID, hDriver, uiMessage, lParam1, lParam2) == 0)
                    return 0L;
            }
	    
            return (LRESULT)1L;

        case DRV_FREE:
            DPF("DRV_FREE");

            /*
               Sent to the driver when it is about to be discarded. This
               will always be the last message received by a driver before
               it is freed. 

               dwDriverID is 0L. 
               lParam1 is 0L.
               lParam2 is 0L.
                
               Return value is ignored.
            */

            // put global de-initialization here...

	    // Pass to all driver procs
            for(i=0; DriverProcs[i]; i++)
                DriverProcs[i](dwDriverID, hDriver, uiMessage, lParam1, lParam2);
	    
            return (LRESULT)1L;

        case DRV_OPEN:
            DPF("DRV_OPEN");
             
            /*
               Sent to the driver when it is opened. 

               dwDriverID is 0L.
               
               lParam1 is a far pointer to a zero-terminated string
               containing the name used to open the driver.
               
               lParam2 is passed through from the drvOpen call. It is
               NULL if this open is from the Drivers Applet in control.exe
               It is LPVIDEO_OPEN_PARMS otherwise.
                
               Return 0L to fail the open.
             */

            //
            //  if we were opened without an open structure then just
            //  return a phony (non zero) id so the OpenDriver() will
            //  work.
            //
            if ((LPVOID)lParam2 == NULL)
                return BOGUS_DRIVER_ID;

            // else, ask all procs if they like input type

            for (i=0; DriverProcs[i]; i++)
	    {
                if (dw = DriverProcs[i](dwDriverID, hDriver, uiMessage, lParam1, lParam2))
                {
                    pi = (INSTINFO *)(UINT)dw;

                    pi->DriverProc = DriverProcs[i];
                    pi->fccType = ((ICOPEN FAR *) lParam2)->fccType;

		    return dw;	// they did, return
		}
	    }
	    // nobody liked it,  just return zero
	    
	    return 0L;

        case DRV_QUERYCONFIGURE:
	    // this is a GLOBAL query configure
            return (LRESULT)0L;

        case DRV_CONFIGURE:
	    // this is a GLOBAL configure ('cause we don't get a configure
	    // for each of our procs, we must have just one configure)
	    
            return DRV_OK;

        /*********************************************************************

            standard driver messages

        *********************************************************************/

        case DRV_DISABLE:
        case DRV_ENABLE:
	    // Pass to all driver procs
            for(i=0; DriverProcs[i]; i++)
                DriverProcs[i](dwDriverID, hDriver, uiMessage, lParam1, lParam2);

            return (LRESULT)1L;

        case DRV_INSTALL:
        case DRV_REMOVE:
            return (LRESULT)DRV_OK;

        default:
            if (pi && pi->DriverProc)
                return pi->DriverProc(dwDriverID, hDriver, uiMessage, lParam1, lParam2);
            else
                return DefDriverProc(dwDriverID, hDriver, uiMessage, lParam1, lParam2);
    }
}


/***************************************************************************
 ***************************************************************************/

LRESULT CALLBACK DriverProcVideo(DWORD dwDriverID, HDRVR hDriver, UINT uiMessage, LPARAM lParam1, LPARAM lParam2)
{
    INSTINFO *pi = (INSTINFO *)(UINT)dwDriverID;

    LPBITMAPINFOHEADER lpbiIn;
    LPBITMAPINFOHEADER lpbiOut;
    ICDECOMPRESSEX FAR *px;

    switch (uiMessage)
    {
        case DRV_LOAD:
            return (LRESULT)Load();

        case DRV_FREE:
            Free();
            return (LRESULT)1L;

        case DRV_OPEN:
	    // we know we have an open struct 'cause it is checked
	    // in the driverproc()

            return (UINT)Open((ICOPEN FAR *) lParam2);

        case DRV_CLOSE:
            if (pi)
                Close(pi);

            return (LRESULT)1L;

        /*********************************************************************

            ICM state messages

        *********************************************************************/

        case ICM_CONFIGURE:
            //
            //  return ICERR_OK if you will do a configure box, error otherwise
            //
            if (lParam1 == -1)
                return QueryConfigure(pi) ? ICERR_OK : ICERR_UNSUPPORTED;
            else
                return Configure(pi, (HWND)lParam1);

        case ICM_ABOUT:
            //
            //  return ICERR_OK if you will do a about box, error otherwise
            //
            if (lParam1 == -1)
                return QueryAbout(pi) ? ICERR_OK : ICERR_UNSUPPORTED;
            else
                return About(pi, (HWND)lParam1);

        case ICM_GETSTATE:
            return GetState(pi, (LPVOID)lParam1, (DWORD)lParam2);

        case ICM_SETSTATE:
            return SetState(pi, (LPVOID)lParam1, (DWORD)lParam2);

        case ICM_GETINFO:
            return GetInfo(pi, (ICINFO FAR *)lParam1, (DWORD)lParam2);
	    
	case ICM_SET_STATUS_PROC:
	    // Remember the status procedure
	    pi->Status = ((ICSETSTATUSPROC FAR *) lParam1)->Status;
	    pi->lParam = ((ICSETSTATUSPROC FAR *) lParam1)->lParam;
	    return 0;
	    

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
            return CompressQuery(pi,
                         (LPBITMAPINFOHEADER)lParam1,
                         (LPBITMAPINFOHEADER)lParam2);

        case ICM_COMPRESS_BEGIN:
            return CompressBegin(pi,
                         (LPBITMAPINFOHEADER)lParam1,
                         (LPBITMAPINFOHEADER)lParam2);

        case ICM_COMPRESS_GET_FORMAT:
            return CompressGetFormat(pi,
                         (LPBITMAPINFOHEADER)lParam1,
                         (LPBITMAPINFOHEADER)lParam2);

        case ICM_COMPRESS_GET_SIZE:
            return CompressGetSize(pi,
                         (LPBITMAPINFOHEADER)lParam1,
                         (LPBITMAPINFOHEADER)lParam2);
            
        case ICM_COMPRESS:
            return Compress(pi,
                            (ICCOMPRESS FAR *)lParam1, (DWORD)lParam2);

        case ICM_COMPRESS_END:
            return CompressEnd(pi);

        /*********************************************************************

            decompress format query messages

        *********************************************************************/

        case ICM_DECOMPRESS_GET_FORMAT:
	    return DecompressGetFormat(pi,
                         (LPBITMAPINFOHEADER)lParam1,
                         (LPBITMAPINFOHEADER)lParam2);

        case ICM_DECOMPRESS_GET_PALETTE:
            return DecompressGetPalette(pi,
			 (LPBITMAPINFOHEADER)lParam1,
			 (LPBITMAPINFOHEADER)lParam2);

        /*********************************************************************

            decompress (old) messages, map these to the new (ex) messages

        *********************************************************************/

        case ICM_DECOMPRESS_QUERY:
            lpbiIn  = (LPBITMAPINFOHEADER)lParam1;
            lpbiOut = (LPBITMAPINFOHEADER)lParam2;

            return DecompressQuery(pi,0,
                    lpbiIn,NULL,
                    0,0,-1,-1,
                    lpbiOut,NULL,
                    0,0,-1,-1);

        case ICM_DECOMPRESS_BEGIN:
            lpbiIn  = (LPBITMAPINFOHEADER)lParam1;
            lpbiOut = (LPBITMAPINFOHEADER)lParam2;

            return DecompressBegin(pi,0,
                    lpbiIn,NULL,
                    0,0,-1,-1,
                    lpbiOut,NULL,
                    0,0,-1,-1);

        case ICM_DECOMPRESS:
            px = (ICDECOMPRESSEX FAR *)lParam1;

            return Decompress(pi,0,
                    px->lpbiSrc,px->lpSrc,
                    0, 0, -1, -1,
                    px->lpbiDst,px->lpDst,
                    0, 0, -1, -1);

	case ICM_DECOMPRESS_END:
            return DecompressEnd(pi);

        /*********************************************************************

            decompress (ex) messages

        *********************************************************************/

        case ICM_DECOMPRESSEX_QUERY:
            px = (ICDECOMPRESSEX FAR *)lParam1;

            return DecompressQuery(pi,
                    px->dwFlags,
                    px->lpbiSrc,px->lpSrc,
                    px->xSrc,px->ySrc,px->dxSrc,px->dySrc,
                    px->lpbiDst,px->lpDst,
                    px->xDst,px->yDst,px->dxDst,px->dyDst);

        case ICM_DECOMPRESSEX_BEGIN:
            px = (ICDECOMPRESSEX FAR *)lParam1;

            return DecompressBegin(pi,
                    px->dwFlags,
                    px->lpbiSrc,px->lpSrc,
                    px->xSrc,px->ySrc,px->dxSrc,px->dySrc,
                    px->lpbiDst,px->lpDst,
                    px->xDst,px->yDst,px->dxDst,px->dyDst);

        case ICM_DECOMPRESSEX:
            px = (ICDECOMPRESSEX FAR *)lParam1;

            return Decompress(pi,
                    px->dwFlags,
                    px->lpbiSrc,px->lpSrc,
                    px->xSrc,px->ySrc,px->dxSrc,px->dySrc,
                    px->lpbiDst,px->lpDst,
                    px->xDst,px->yDst,px->dxDst,px->dyDst);

        case ICM_DECOMPRESSEX_END:
	    return DecompressEnd(pi);

        /*********************************************************************

            draw messages

        *********************************************************************/

        case ICM_DRAW_QUERY:
            return DrawQuery(pi,(LPBITMAPINFOHEADER)lParam1)
                ? ICERR_OK : ICERR_UNSUPPORTED;

        case ICM_DRAW_BEGIN:
            return DrawBegin(pi,(ICDRAWBEGIN FAR *)lParam1, (DWORD)lParam2);

        case ICM_DRAW:
            return Draw(pi,(ICDRAW FAR *)lParam1, (DWORD)lParam2);

        case ICM_DRAW_END:
            return DrawEnd(pi);
    }

    if (uiMessage < DRV_USER)
        return DefDriverProc(dwDriverID, hDriver, uiMessage, lParam1, lParam2);
    else
        return ICERR_UNSUPPORTED;
}

/****************************************************************************
 ***************************************************************************/
#ifdef _WIN32
BOOL APIENTRY DllMain(HANDLE, DWORD, LPVOID);
BOOL APIENTRY DllMain(
HANDLE hModule,
DWORD dwReason,
LPVOID lpReserved )
{
        switch( dwReason)
        {
                case DLL_PROCESS_ATTACH:
                        if(ghModule == NULL)
                                ghModule = (HMODULE)hModule;
                        break;
                case DLL_THREAD_ATTACH:
                        break;
                case DLL_THREAD_DETACH:
                        break;
                case DLL_PROCESS_DETACH:
                        break;
        }
        return TRUE;
}

#else
int NEAR PASCAL LibInit(HMODULE hModule)
{
    ghModule = hModule;

    return 1;
}
#endif
/*************************************************************************
 *
 * REVERSE.C
 * 
 * Sample MedWave code.
 * 
 *************************************************************************/

#include <windows.h>
#include <wincom.h>
#include <mmsystem.h>
#include <mediaman.h>
#include <medwave.h>
#include "reverse.h"


/*
 *  GLOBAL VARIABLE DECLARATIONS
 */
char	szAppName[]	= "Reverse";
char	szSampleName[]	= "ReverseSample";
HANDLE	hInstApp	= NULL;
HWND	hwndApp		= NULL;


HWND	hwndName	= NULL;
HWND	hwndPlay	= NULL;
HWND	hwndQuit	= NULL;

#define FILE_NAME_SIZE	80


/* vars used for playing the file */
/* These must be global since they are allocated in the reverse */
/* procedure and freed in the main window procedure */
    HANDLE		hData;
    HPSTR		lpData;
    HANDLE		hWaveHdr;
    LPWAVEHDR		lpWaveHdr;
    HWAVEOUT		hWaveOut;

    
/* 
 * This routine interchanges two samples at the given positions.
 */
void Interchange( HPSTR hpchPos1, HPSTR hpchPos2, WORD wLength )
{
    WORD	wPlace;
    BYTE	bTemp;
    
    for( wPlace = 0; wPlace < wLength; wPlace++ ) {
	bTemp = hpchPos1[wPlace];
	hpchPos1[wPlace] = hpchPos2[wPlace];
	hpchPos2[wPlace] = bTemp;
    }
}





/* 
 * This routine will use the MedWave handler to open the
 * WAVE file and get pertinant information from it.
 */
void ReversePlay() 
{
    MEDID		medid;
    DWORD		dwFmtSize;
    MedReturn		medReturn;
    char		szFileName[ FILE_NAME_SIZE ];
    WORD		wResult;
    HANDLE		hFormat;
    WAVEFORMAT		*pFormat;
    DWORD		dwDataSize;
    WaveReadStruct	wrs;
    HPSTR		hpch1, hpch2;
    WORD		wBlockSize;

    
    /* Get the file name from the window */
    if( !GetWindowText( hwndName, (LPSTR)szFileName, FILE_NAME_SIZE ) ) {
        MessageBox(hwndApp, "Failed to Get File Name",
		NULL, MB_OK | MB_ICONEXCLAMATION);
        return;
    }
    
    /* Locate the file for all future references */
    medid = medLocate(  szFileName,
                        medMEDTYPE('W','A','V','E'),
                        MEDF_LOCATE,
                        NULL);
    if( medid == 0 ) {
        MessageBox(hwndApp, "Failed to locate resource.",
		NULL, MB_OK | MB_ICONEXCLAMATION);
        return;
    }

    /* Access the medid to 'LOAD' the element */
    /* Note the lParam to the handler for the LOAD operation is 0 */
    if( medAccess( medid, 0L, (FPMedReturn)&medReturn, TRUE, NULL, 0L )
			    != MEDF_OK ) {
        MessageBox(hwndApp, "Failed to Access element",
		NULL, MB_OK | MB_ICONEXCLAMATION);
        return;
    }
    
    /* Get the size of the format structure */
    /* The format structure is variable sized so it size is requred */
    /* to be properly allocated */
    /* The parameters to WAVE_GETFMTSIZE are ignored */
    dwFmtSize = medSendMessage( medid, WAVE_GETFMTSIZE, 0L, 0L );
    if( dwFmtSize == 0L ) {
	medRelease( medid, 0L );
        MessageBox(hwndApp, "Failed to get element format size.",
		NULL, MB_OK | MB_ICONEXCLAMATION);
        return;
    }
    
    /* Now that the size is known, allocate and lock the memory  */
    /* for the format structure */
    hFormat = LocalAlloc(LMEM_MOVEABLE, LOWORD(dwFmtSize));
    if (!hFormat) {
	medRelease( medid, 0L );
        MessageBox(hwndApp, "Not enough local memory for header.",
		NULL, MB_OK | MB_ICONEXCLAMATION);
        return;
    }

    pFormat = (WAVEFORMAT *) LocalLock(hFormat);
    if (!pFormat) {
	LocalFree( hFormat );
	medRelease( medid, 0L );
        MessageBox(hwndApp, "Failed to lock memory for header.",
		NULL, MB_OK | MB_ICONEXCLAMATION);
        return;
    }

    
    /* The WAVE_GETFMT message fills the format information into */
    /* the given memory */
    /* Param 1 is a pointer to the memory */
    /* Param 2 is the lenght of the memory buffer given */
    medSendMessage( medid, WAVE_GETFMT, 
	    (LONG)(LPWAVEFORMAT)pFormat, dwFmtSize );

    if( pFormat->wFormatTag != WAVE_FORMAT_PCM ) {
	LocalUnlock( hFormat );
	LocalFree( hFormat );
	medRelease( medid, 0L );
        MessageBox(hwndApp, "Failed to get element size.",
		NULL, MB_OK | MB_ICONEXCLAMATION);
        return;
    }

    /* The WAVE_GETSIZE message is used to get the size of the */
    /* element.  Parameters are ignored and the size returned  */
    /* is in samples */
    dwDataSize = medSendMessage( medid, WAVE_GETSIZE, 0L, 0L );
    if( dwFmtSize == 0L ) {
	LocalUnlock( hFormat );
	LocalFree( hFormat );
	medRelease( medid, 0L );
        MessageBox(hwndApp, "Failed to get element size.",
		NULL, MB_OK | MB_ICONEXCLAMATION);
        return;
    }
    
    /* Open the Wave Output device */
    if( waveOutOpen( (LPHWAVEOUT)&hWaveOut, 0, (LPWAVEFORMAT)pFormat,
		    (LONG)hwndApp, 0L, CALLBACK_WINDOW ) ) {
	medRelease( medid, 0L );
	MessageBox(hwndApp, "Failed to open wave device.",
		NULL, MB_OK | MB_ICONEXCLAMATION);
        return;
    }


    wBlockSize = pFormat->nBlockAlign;
    /* App is done with format - free it */
    LocalUnlock( hFormat );
    LocalFree( hFormat );
    
    
    /* Now the data buffer must be allocated and locked */

    /* The nBlockAlign field of the format structure tells the */
    /* Length in bytes of the sample */
    /* Thus the length in bytes of the element is dwDataSize * wBlockSize */
    hData = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE,
                        ( dwDataSize * wBlockSize ) );
    if (!hData) {
	medRelease( medid, 0L );
        MessageBox(hwndApp, "Not enough memory for data block",
		NULL, MB_OK | MB_ICONEXCLAMATION);
        return;
    }


    lpData = GlobalLock(hData);
    if (!lpData) {
	GlobalFree( hData );
	medRelease( medid, 0L );
        MessageBox(hwndApp, "Failed to lock data memory.",
		NULL, MB_OK | MB_ICONEXCLAMATION);
        GlobalFree(hData);
        return;
    }



    /* The WAVE_READ message will fill the buffer with the data */
 
    wrs.nPosition = 0L;
    	/* Position (in samples) to start reading from */
    wrs.nLength = dwDataSize;
    	/* Length (in samples) to read */
    wrs.fpchBuffer = lpData;
    	/* Pointer to the data buffer */

    /* Param1 to the WAVE_READ is a far pointer to the WaveReadStruct */
    /* Param2 to the WAVE_READ is a far pointer to the MediaWaveFormatBlock */
    /* of the format to read the data in. */
    /* Note this is NULL since we want the existing format */
    if( medSendMessage( medid, WAVE_READ, (LONG)(LPSTR)&wrs, 0L ) 
	    != dwDataSize) { 
	GlobalUnlock( hData );
	GlobalFree( hData );
	medRelease( medid, 0L );
	MessageBox(hwndApp, "Failed to read data.",
		NULL, MB_OK | MB_ICONEXCLAMATION);
        return;
    }

    /* At this point the program is done with the original WAVE file */
    /* so it may be released */
    medRelease( medid, 0L );

    /* At this point 'operations' may be performed on the data */
    /* to produce various effects */
    
    /* reverse the sound for playing */
    hpch1 = lpData;
    hpch2 = lpData + ( ( dwDataSize - 1 ) * wBlockSize );
    while( hpch1 < hpch2 ) {
	Interchange( hpch1, hpch2, wBlockSize );
	hpch1 += wBlockSize;
	hpch2 -= wBlockSize;
    }
    
    
    

    /* The Wave Header must be globally alloced  and locked */
    hWaveHdr = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE | GMEM_NOT_BANKED,
                        (DWORD) sizeof(WAVEHDR));
    if (!hWaveHdr) {
	GlobalUnlock( hData );
	GlobalFree( hData );
	medRelease( medid, 0L );
        MessageBox(hwndApp, "Not enough memory for header.",
		NULL, MB_OK | MB_ICONEXCLAMATION);
        return;
    }

    lpWaveHdr = (LPWAVEHDR) GlobalLock(hWaveHdr);
    if (!lpWaveHdr) {
	GlobalUnlock( hData );
	GlobalFree( hData );
	medRelease( medid, 0L );
        MessageBox(hwndApp, "Failed to lock memory for header.",
		NULL, MB_OK | MB_ICONEXCLAMATION);
        return;
    }

    /* After allocation, the data and header must be prepared for use */
    lpWaveHdr->lpData = lpData;
    lpWaveHdr->dwBufferLength = dwDataSize;
    lpWaveHdr->dwFlags = 0L;
    lpWaveHdr->dwLoops = 0L;

    /*
        Page lock the header and the data blocks so they 
        won't get swapped out.
    */

    waveOutPrepareHeader(hWaveOut, lpWaveHdr, sizeof(WAVEHDR));


    /*
        send it to the driver

    */

    /* Then the data may be 'written' to the output device */
    wResult = waveOutWrite(hWaveOut, lpWaveHdr, sizeof(WAVEHDR));
    if (wResult != 0) {
	waveOutUnprepareHeader( hWaveOut, lpWaveHdr, sizeof(WAVEHDR));
	GlobalUnlock( hData );
	GlobalFree( hData );
	medRelease( medid, 0L );
        MessageBox(hwndApp, "Failed to write block to device",
		NULL, MB_OK | MB_ICONEXCLAMATION);
        return;
    }

    EnableWindow( hwndPlay, FALSE );
    EnableWindow( hwndQuit, FALSE );
}








/**********************************
 *
 *	MAIN WINDOW PROCEDURE
 *
 **********************************/

LONG FAR PASCAL ReverseWndProc(HWND hwnd, unsigned msg, WORD wParam, LONG lParam)
{

  switch (msg) {
    case WM_CREATE:
	/* initalize Global Playing Variables. */
	hData = NULL;
	lpData = NULL;
	hWaveHdr = NULL;
	lpWaveHdr = NULL;
	hWaveOut = NULL;
	break;


    case WM_DESTROY:
	PostQuitMessage(0);
	break;


    /*  Process menu item command messages  */
    case WM_COMMAND:
	switch (wParam) {
	    case IDE_NAME:
		return( 0L );

	    case IDB_PLAY:
		ReversePlay();
		break;

	    case IDB_QUIT:
		PostQuitMessage(0);
		break;
	}
	return( 0L );

    case MM_WOM_DONE:
        if( lpWaveHdr && (lpWaveHdr->dwFlags & WHDR_DONE) ) {
	    /* After the sound block has been played, the header */
	    /* must be 'unprepared', the buffers freed and */
	    /* the WAVE device closed */
	    waveOutUnprepareHeader( hWaveOut, lpWaveHdr, sizeof(WAVEHDR));
	    GlobalUnlock( hData );
	    GlobalFree( hData );
	    waveOutClose( hWaveOut );
	    EnableWindow( hwndPlay, TRUE );
	    EnableWindow( hwndQuit, TRUE );
	    /*  re-initalize global vars */
	    hData = NULL;
	    lpData = NULL;
	    hWaveHdr = NULL;
	    lpWaveHdr = NULL;
	    hWaveOut = NULL;
	}
	break;


	
  }

  return DefWindowProc(hwnd,msg,wParam,lParam);
 
}




/***********************
 * 
 *	WINMAIN
 * 
 ***********************/

int PASCAL WinMain(HANDLE hInst, HANDLE hPrev, LPSTR szCmdLine, WORD sw)
{
  MSG     msg;

  /*  Call initialization procedure.  This will setup application
   *  stuff like the windows window class, create the main application
   *  window, and register with mediaman.
   */
  if (!AppInit(hInst,hPrev,sw,szCmdLine))
	  return FALSE;

  /*
   * Polling messages from event queue
   * (Main message loop)
   */
  while (GetMessage(&msg,NULL,0,0))  {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
  }
  
  /*  Make sure that I de-register myself from mediaman. */
  medClientExit();
  return msg.wParam;
}

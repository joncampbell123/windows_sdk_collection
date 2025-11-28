//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1992 - 1997  Microsoft Corporation.  All Rights Reserved.
//
//--------------------------------------------------------------------------;
//
//   Trivial player console application
//
//   Function:
//       Plays a movie file with options for
//       --  Playing full screen
//       --  Playing on top of normal windows
//       --  Delaying start time
//       Exits when movie playback is complete
//       NO user interface
//
//   Usage: playit [-f] [-t] [-delay nsecs] foo.bar\n

#include <windows.h>
#include <strmif.h>
#include <uuids.h>
#include <control.h>
#include <stdio.h>

BOOL bMax = FALSE;
BOOL bOnTop = FALSE;
DWORD dwDelay = 0;

HRESULT PlayMovie(LPSTR lpszMovie)
{
    int iLen = MultiByteToWideChar(
                      CP_ACP,
                      MB_PRECOMPOSED,
                      lpszMovie,
                      -1,
                      NULL,
                      0);
    LPWSTR lpwszMovie = new WCHAR[iLen + 1];
    MultiByteToWideChar(
        CP_ACP,
        MB_PRECOMPOSED,
        lpszMovie,
        -1,
        lpwszMovie,
        iLen + 1);

    HRESULT hr = S_OK;
    IMediaControl *pMC = NULL;
    hr = CoCreateInstance(CLSID_FilterGraph,
                          NULL,
                          CLSCTX_INPROC,
                          IID_IMediaControl,
                          (void **)&pMC);

    IVideoWindow *pVW = NULL;
    IGraphBuilder *pGB = NULL;
    IMediaEventEx *pME;
    if (SUCCEEDED(hr)) {
        hr = pMC->QueryInterface(IID_IVideoWindow, (void **)&pVW);
    }

    if (SUCCEEDED(hr)) {
        hr = pMC->QueryInterface(IID_IGraphBuilder, (void **)&pGB);
    }
    if (SUCCEEDED(hr)) {
        hr = pMC->QueryInterface(IID_IMediaEventEx, (void **)&pME);
    }

    if (SUCCEEDED(hr)) {
        pVW->put_Visible(0);
        pVW->put_AutoShow(0);
        hr = pGB->RenderFile(lpwszMovie, NULL);
        Sleep(dwDelay * 1000);
    }

    if (bMax) {
        if (SUCCEEDED(hr)) {
            pVW->put_FullScreenMode(-1);
        }
    }

    if (bOnTop) {
        long ExStyle;
        if (SUCCEEDED(pVW->get_WindowStyleEx(&ExStyle))) {
            pVW->put_WindowStyleEx(ExStyle | WS_EX_TOPMOST);
        }
    }


    if (SUCCEEDED(hr)) {
        pMC->Pause();
        pVW->put_Visible(-1);
        hr = pMC->Run();
    }

    if (SUCCEEDED(hr)) {
        long evCode;
        hr = pME->WaitForCompletion(INFINITE, &evCode);
    }

    if (pMC) {
        pMC->Release();
    }
    if (pVW) {
        pVW->Release();
    }
    if (pGB) {
        pGB->Release();
    }
    if (pME) {
        pME->Release();
    }
    return hr;
}

int _CRTAPI1 main(int argc, char *argv[])
{
    argv++;
    while (argc > 2) {
        if (0 == lstrcmpi(argv[0], "-t")) {
            bOnTop = TRUE;
            argc--;
            argv++;
        } else
        if (0 == lstrcmpi(argv[0], "-f")) {
            bMax = TRUE;
            argc--;
            argv++;
        } else 
        if (argc > 3 && 0 == lstrcmpi(argv[0], "-delay")) {
            dwDelay = atoi(argv[1]);
            argc -= 2;
            argv += 2;
        } else {
            break;
        }
    }
    if (argc != 2) {
        printf("Usage playit [-f] [-t] [-delay nsecs] foo.bar\n");
        exit(0);
    }
    CoInitialize(NULL);
    PlayMovie(argv[0]);
    CoUninitialize();
    return 1;
}


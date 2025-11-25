
  //=========================================================================
  //
  //  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
  //  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
  //  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
  //  PURPOSE.
  //
  // Copyright (c) 1996 - 1997  Microsoft Corporation.  All Rights Reserved.
  //
  //=========================================================================

  #include <windows.h>
  #include <mmsystem.h>
  #include <streams.h>
  #include "playfile.h"

  #define APPLICATIONNAME "PlayFile"
  #define CLASSNAME "PlayFile"

  #define WM_GRAPHNOTIFY  WM_USER+13

  #define HELPER_RELEASE(x) { if (x) x->Release(); x = NULL; }

  HWND      ghApp;
  HINSTANCE ghInst;

  HRESULT   hr;
  LONG      evCode;
  LONG      evParam1;
  LONG      evParam2;

  // Collection of interfaces
  IBaseFilter   *pif   = NULL;
  IGraphBuilder *pigb  = NULL;
  IMediaControl *pimc  = NULL;
  IMediaEventEx *pimex = NULL;
  IVideoWindow  *pivw  = NULL;

  void PlayFile (LPSTR szFile)

    { // PlayFile //

      WCHAR wFile[MAX_PATH];
      MultiByteToWideChar( CP_ACP, 0, szFile, -1, wFile, MAX_PATH );

      hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&pigb);

      if (SUCCEEDED(hr))

      { // Graphbuilder instance

        // QueryInterface for some basic interfaces
        pigb->QueryInterface(IID_IMediaControl, (void **)&pimc);
        pigb->QueryInterface(IID_IMediaEventEx, (void **)&pimex);
        pigb->QueryInterface(IID_IVideoWindow, (void **)&pivw);

        // Have the graph construct its the appropriate graph automatically
        hr = pigb->RenderFile(wFile, NULL);

        // Have the graph signal event via window callbacks for performance
        pimex->SetNotifyWindow((OAHWND)ghApp, WM_GRAPHNOTIFY, 0);

        // Run the graph if RenderFile succeeded
        if (SUCCEEDED(hr))
          pimc->Run();

      } // Graphbuilder instance

    } // PlayFile //

  // ---------------------------------------------------------------------------
  // End of multimedia specific code.
  // ---------------------------------------------------------------------------

  BOOL GetClipFileName(LPSTR szName)

    { // GetClipFileName //

      OPENFILENAME ofn;

      ofn.lStructSize       = sizeof(OPENFILENAME);
      ofn.hwndOwner         = ghApp;
      ofn.lpstrFilter       = NULL;
      ofn.lpstrFilter       = "Video files (*.mpg; *.mpeg; *.avi; *.mov; *.qt)\0*.mpg; *.mpeg; *.avi; *.mov; *.qt\0\0";
      ofn.lpstrCustomFilter = NULL;
      ofn.nFilterIndex      = 1;
      *szName = 0;
      ofn.lpstrFile         = szName;
      ofn.nMaxFile          = MAX_PATH;
      ofn.lpstrInitialDir   = NULL;
      ofn.lpstrTitle        = NULL;
      ofn.lpstrFileTitle    = NULL;
      ofn.lpstrDefExt       = "MPG";
      ofn.Flags             = OFN_FILEMUSTEXIST | OFN_READONLY | OFN_PATHMUSTEXIST;

      return GetOpenFileName((LPOPENFILENAME)&ofn);

    } // GetClipFileName //

  LRESULT CALLBACK WndMainProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)

    { // WndMainProc //

      switch(message)

        { // Window msgs handling

          case WM_COMMAND:

            switch(wParam)

              { // Menus

                case ID_FILE_OPENCLIP:

                  TCHAR szFilename[MAX_PATH];

                  if (GetClipFileName(szFilename))
                    PlayFile(szFilename);

                  break;

                case ID_FILE_EXIT:

                  if (pimc)
                    pimc->Stop();

                  if (pivw)
                    pivw->put_Visible(OAFALSE);

                  HELPER_RELEASE(pivw);
                  HELPER_RELEASE(pif);
                  HELPER_RELEASE(pigb);
                  HELPER_RELEASE(pimc);
                  HELPER_RELEASE(pimex);

                  PostQuitMessage(0);

                  break;

              } // Menus

            break;

          case WM_GRAPHNOTIFY:

            while (SUCCEEDED(pimex->GetEvent(&evCode, &evParam1, &evParam2, 0)))

              { // Spin through the events

                hr = pimex->FreeEventParams(evCode, evParam1, evParam2);

                if ((EC_COMPLETE == evCode) || (EC_USERABORT == evCode))

                  { // Finished

                    pivw->put_Visible(OAFALSE);

                    HELPER_RELEASE(pivw);
                    HELPER_RELEASE(pif);
                    HELPER_RELEASE(pigb);
                    HELPER_RELEASE(pimc);
                    HELPER_RELEASE(pimex);

                    break;

                  } // Finished
              
              } // Spin through the events

            break;

          case WM_DESTROY:
            PostQuitMessage(0);
            break;

          default:
          return DefWindowProc(hWnd, message, wParam, lParam);

      } // Window msgs handling

      return FALSE;

    } // WndMainProc //

  int PASCAL WinMain(HINSTANCE hInstC, HINSTANCE hInstP, LPSTR lpCmdLine, int nCmdShow)

    { // WinMain //

      MSG msg;
      WNDCLASS wc;

      // OLE subsystem requires applications to initialize things first!
      CoInitialize(NULL);

      ZeroMemory(&wc, sizeof wc);
      wc.lpfnWndProc = WndMainProc;
      ghInst = wc.hInstance = hInstC;
      wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
      wc.lpszClassName = CLASSNAME;
      wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU);
      wc.hCursor = LoadCursor(NULL, IDC_ARROW);
      wc.hIcon = LoadIcon(hInstC, MAKEINTRESOURCE(IDI_PLAYFILE));
      RegisterClass(&wc);

      ghApp = CreateWindow(CLASSNAME,
        APPLICATIONNAME,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        0,
        0,
        ghInst,
        0);

      ShowWindow(ghApp, SW_NORMAL);
      UpdateWindow(ghApp);

      while (GetMessage(&msg,NULL,0,0))

        {  // Message loop

           TranslateMessage(&msg);
           DispatchMessage(&msg);

        }  // Message loop

      // Finished with OLE subsystem
      CoUninitialize();

      return msg.wParam;

    } // WinMain //

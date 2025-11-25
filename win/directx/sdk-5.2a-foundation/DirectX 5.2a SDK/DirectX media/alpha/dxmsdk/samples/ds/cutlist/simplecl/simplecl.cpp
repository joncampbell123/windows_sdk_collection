 
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
  #include <streams.h>
  #include <strmif.h>
  #include <cutlist.h>
  #include "simplecl.h"

  ICutListGraphBuilder  *pCLGraphBuilder  = NULL;
  IStandardCutList      *pVideoCL         = NULL;
  IStandardCutList      *pAudioCL         = NULL;
  IGraphBuilder         *pigb             = NULL;
  IMediaControl         *pimc             = NULL;
  IMediaEventEx         *pimex            = NULL;
  IVideoWindow          *pivw             = NULL;

  IFileClip             *pVidFileClip[MAX_CLIPS];
  IFileClip             *pAudFileClip[MAX_CLIPS];
  IAMCutListElement       *pVidCLElem[MAX_CLIPS];
  IAMCutListElement       *pAudCLElem[MAX_CLIPS];

  HRESULT hr;

  int nVidElems, nAudElems;
 
  void SimpleCutList ()

    { // SimpleCutList //

      WCHAR wFile[MAX_PATH];  // file name

      // Initialize video and audio file clips and elements to NULL
      // so we can easily free objects later.
      for (int x = 0; x < MAX_CLIPS; ++x)

        { 
          pVidFileClip[x] = NULL; 
          pAudFileClip[x] = NULL; 
            pVidCLElem[x] = NULL; 
            pAudCLElem[x] = NULL; 
        };

      // Create cutlist graph builder object
      hr = CoCreateInstance(CLSID_CutListGraphBuilder, NULL, 
                            CLSCTX_INPROC, IID_ICutListGraphBuilder, 
                            (void**)&pCLGraphBuilder);

      if (FAILED(hr))
        { // CoCreateInstance of CutListGraphBuiler failed
          MessageBox(ghApp, 
                     "CoCreateInstance of CutListGraphBuiler failed",
                     APPLICATIONNAME, MB_OK);
          TearDownTheGraph();
          return;
        } // CoCreateInstance of CutListGraphBuiler failed

      // Create simple (standard) cutlist object for video
      hr = CoCreateInstance(CLSID_SimpleCutList, NULL, 
                            CLSCTX_INPROC, IID_IStandardCutList, 
                            (void**)&pVideoCL);
      if (FAILED(hr))
        { // CoCreateInstance of video SimpleCutList failed
          MessageBox(ghApp, 
                     "CoCreateInstance of video SimpleCutList failed",
                     APPLICATIONNAME, MB_OK);
          TearDownTheGraph();
          return;
        } // CoCreateInstance of video SimpleCutList failed

      // Create simple (standard) cutlist object for audio
      hr = CoCreateInstance(CLSID_SimpleCutList, NULL, 
                            CLSCTX_INPROC, IID_IStandardCutList, 
                            (void**)&pAudioCL);

      if (FAILED(hr))
        { // CoCreateInstance of audio SimpleCutList failed
          MessageBox(ghApp, 
                     "CoCreateInstance of audio SimpleCutList failed",
                     APPLICATIONNAME, MB_OK);
          TearDownTheGraph();
          return;
        } // CoCreateInstance of audio SimpleCutList failed

      // Create the individual clips and add them to the cutlist
      nVidElems = nAudElems = 0;
      for (x = 0; x < gTheSet.nNumClips; ++x)

        { // Individual clips

          MultiByteToWideChar(CP_ACP, 0, 
                              gTheSet.List[x].szFilename, 
                              -1, wFile, MAX_PATH );

          // Create a video clip object and give it the file and stream 
          // to read from.
          // SetFileAndStream will fail if we call it from a video clip 
          // object and the clip is not a video clip.
          hr = CoCreateInstance(CLSID_VideoFileClip, NULL, 
                                CLSCTX_INPROC, IID_IFileClip, 
                                (void**)&pVidFileClip[nVidElems]);

          hr = pVidFileClip[nVidElems]->SetFileAndStream(wFile, 0);

          if (SUCCEEDED(hr))

            { // Create video cut and add the clip (element) to the cutlist

              hr = pVidFileClip[nVidElems]->CreateCut(&pVidCLElem[nVidElems], 
                      gTheSet.List[x].start*SCALE, 
                      gTheSet.List[x].stop*SCALE, 
                      0, 
                      (gTheSet.List[x].stop-gTheSet.List[x].start)*SCALE, 
                      0);

              if (SUCCEEDED(hr))

                { // Add the element to the cutlist

                  hr = pVideoCL->AddElement(pVidCLElem[nVidElems], CL_DEFAULT_TIME, CL_DEFAULT_TIME);

                  if (SUCCEEDED(hr))
                    ++nVidElems;

                  else

                    { // AddElement failed so release associated objects

                      HELPER_RELEASE(pVidCLElem[nVidElems]);
                      HELPER_RELEASE(pVidFileClip[nVidElems]);
                      MessageBox(ghApp, "AddElement (video) failed!", APPLICATIONNAME, MB_OK);

                    } // AddElement failed so release associated objects

                } // Add the element to the cutlist

              else MessageBox(ghApp, "CreateCut (video) failed!", APPLICATIONNAME, MB_OK);

            } // Create video cut

          else

            { // Problems creating video stream

              HELPER_RELEASE(pVidFileClip[nVidElems]);
              MessageBox(ghApp, "SetFileAndStream (video) failed!", APPLICATIONNAME, MB_OK);

            } // Problems creating video stream

          // Create an audio clip object and give it the file and stream 
          // to read from.
          // SetFileAndStream will fail if we call it from an audio clip 
          // object and the clip is not an audio clip
          hr = CoCreateInstance(CLSID_AudioFileClip, NULL, 
                                CLSCTX_INPROC, IID_IFileClip, 
                                (void**)&pAudFileClip[nAudElems]);

          hr = pAudFileClip[nAudElems]->SetFileAndStream(wFile, 0);

          if (SUCCEEDED(hr))

            { // Create audio cut and add the clip (element) to the cutlist

	            hr = pAudFileClip[nAudElems]->CreateCut(&pAudCLElem[nAudElems], 
                      gTheSet.List[x].start*SCALE, 
                      gTheSet.List[x].stop*SCALE, 
                      0, 
                      (gTheSet.List[x].stop-gTheSet.List[x].start)*SCALE, 
                      0);

              if (SUCCEEDED(hr))

                { // Add the element to the cutlist

                  hr = pAudioCL->AddElement(pAudCLElem[nAudElems],
                                            CL_DEFAULT_TIME, 
                                            CL_DEFAULT_TIME);

                  if (SUCCEEDED(hr))
                    ++nAudElems;

                  else

                    { // AddElement failed so release associated objects

                      HELPER_RELEASE(pAudCLElem[nAudElems]);
                      HELPER_RELEASE(pAudFileClip[nAudElems]);
                      MessageBox(ghApp, "AddElement (audio) failed!", APPLICATIONNAME, MB_OK);

                    } // AddElement failed so release associated objects

                } // Add the element to the cutlist

              else MessageBox(ghApp, "CreateCut (audio) failed!", APPLICATIONNAME, MB_OK);

            } // Create audio cut

          else
          
            { // Problems creating audio stream

              HELPER_RELEASE(pAudFileClip[nAudElems]);
              MessageBox(ghApp, "SetFileAndStream (audio) failed!", APPLICATIONNAME, MB_OK);

            } // Problems creating audio stream

        } // Individual clips

      // Add the video cutlist to the filter graph
      hr = pCLGraphBuilder->AddCutList(pVideoCL, NULL);

      if (FAILED(hr)) // AddCutList (video) failed
          MessageBox(ghApp, "AddCutList (video) failed", APPLICATIONNAME, MB_OK);

      // Add the audio cutlist to the filter graph
      hr = pCLGraphBuilder->AddCutList(pAudioCL, NULL);

      if (FAILED(hr)) // AddCutList (audio) failed
          MessageBox(ghApp, "AddCutList (audio) failed", APPLICATIONNAME, MB_OK);

      if ((!pVideoCL) && (!pAudioCL))

        { // Clean up

          TearDownTheGraph();
          return;

        } // Clean up

      // Let the filter graph manager construct the the appropriate graph 
      // automatically
      hr = pCLGraphBuilder->Render();

      if (FAILED(hr))
        { // Problems rendering the graph
          if (!AMGetErrorText(hr, gszScratch, 2048))
            MessageBox(ghApp, "Problems rendering the graph!", APPLICATIONNAME, MB_OK);
          else 
            MessageBox(ghApp, gszScratch, APPLICATIONNAME, MB_OK);
          TearDownTheGraph();
          return;
        } // Problems rendering the graph

      // Retrieve the filter graph and useful interfaces
      hr = pCLGraphBuilder->GetFilterGraph(&pigb);

      if (FAILED(hr))
        { // Problems retrieving the graph pointer
          if (!AMGetErrorText(hr, gszScratch, 2048))
            MessageBox(ghApp, "Problems retrieving the graph pointer!", APPLICATIONNAME, MB_OK);
          else 
            MessageBox(ghApp, gszScratch, APPLICATIONNAME, MB_OK);
          TearDownTheGraph();
          return;
        } // Problems retrieving the graph pointer

      // QueryInterface for some basic interfaces
      pigb->QueryInterface(IID_IMediaControl, (void **)&pimc);
      pigb->QueryInterface(IID_IMediaEventEx, (void **)&pimex);
      pigb->QueryInterface(IID_IVideoWindow, (void **)&pivw);

      // Decrement the ref count on the filter graph
      pigb->Release();

      // Prepare to play in the main application window's client area

      RECT rc;
      GetClientRect(ghApp, &rc);
      hr = pivw->put_Owner((OAHWND)ghApp);
      hr = pivw->put_WindowStyle(WS_CHILD|WS_CLIPSIBLINGS);
      hr = pivw->SetWindowPosition(rc.left, rc.top, rc.right, rc.bottom);

      // Have the graph signal event via window callbacks for performance
      pimex->SetNotifyWindow((OAHWND)ghApp, WM_GRAPHNOTIFY, 0);

      // Run the graph if RenderFile succeeded
      pimc->Run();

    } // SimpleCutList //

 void TearDownTheGraph (void)

    { // TearDownTheGraph //

      if (pimc)
        pimc->Stop();

      if (pivw)

        { // Hide the playback window first thing

          pivw->put_Visible(OAFALSE);
          pivw->put_Owner(NULL);

        } //

      HELPER_RELEASE(pimex);
      HELPER_RELEASE(pimc);
      HELPER_RELEASE(pivw);

      // Remove the video cutlist from the filter graph to free resources
      if (pCLGraphBuilder && pVideoCL)
        pCLGraphBuilder->RemoveCutList(pVideoCL);

      // Remove the audio cutlist from the filter graph to free resources
      if (pCLGraphBuilder && pAudioCL)
        pCLGraphBuilder->RemoveCutList(pAudioCL);

      for (int x = 0; x < nAudElems; ++x)

        { // Release audio objects

          HELPER_RELEASE(pAudCLElem[x]);
          HELPER_RELEASE(pAudFileClip[x]);

        } // Release audio objects

      for (x = 0; x < nVidElems; ++x)

        { // Release video objects

          HELPER_RELEASE(pVidCLElem[x]);
          HELPER_RELEASE(pVidFileClip[x]);

        } // Release video objects

      HELPER_RELEASE(pVideoCL);
      HELPER_RELEASE(pAudioCL);      
      HELPER_RELEASE(pCLGraphBuilder);

    } // TearDownTheGraph //

  BOOL CALLBACK DialogProc (HWND h, UINT m, WPARAM w, LPARAM l)

    { // DialogProc //

      switch(m)

        { // Msg handling

          case WM_INITDIALOG:
            if (wDlgRes == IDD_MEDIATIMES)
              SetFocus(GetDlgItem(h, IDC_TRIMIN2));
            else return TRUE;
            break;

          case WM_COMMAND:

            switch(w)

              { // Command msg handling

                case IDOKTIMES:

                  gTheSet.List[gTheSet.nNumClips].start = GetDlgItemInt(h, IDC_TRIMIN2, NULL, FALSE);
                  gTheSet.List[gTheSet.nNumClips].stop = GetDlgItemInt(h, IDC_TRIMOUT2, NULL, FALSE);

                  EndDialog(h,1);
                  break;

                case IDOK:
                  EndDialog(h,1);
                  break;

                case IDCANCEL:
                  EndDialog(h,0);
                  break;

              } // Command msg handling

            break;

        } // Msg handling

      return 0;

    } // DialogProc //

  BOOL GetClipFileName (LPSTR szName)

    {   // GetClipFileName //

        OPENFILENAME ofn;

        ofn.lStructSize       = sizeof(OPENFILENAME);
        ofn.hwndOwner         = ghApp;
        ofn.lpstrFilter       = NULL;
        ofn.lpstrFilter       = "AVI/WAV (*.avi; *.wav)\0*.avi;*.wav\0\0\0";
        ofn.lpstrCustomFilter = NULL;
        ofn.nFilterIndex      = 1;
        *szName               = 0;
        ofn.lpstrFile         = szName;
        ofn.nMaxFile          = MAX_PATH;
        ofn.lpstrInitialDir   = NULL;
        ofn.lpstrTitle        = NULL;
        ofn.lpstrFileTitle    = NULL;
        ofn.lpstrDefExt       = "AVI;WAV";
        ofn.Flags             = OFN_FILEMUSTEXIST | OFN_READONLY | OFN_PATHMUSTEXIST;

        return GetOpenFileName((LPOPENFILENAME)&ofn);

    }   // GetClipFileName //

  LRESULT CALLBACK WndMainProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)

    { // WndMainProc //

      LONG evCode;
      LONG evParam1;
      LONG evParam2;

      switch(message)

        { // Window msgs handling

          case WM_COMMAND:

            switch(wParam)

              {  // Program menu option

                case IDM_ADDFILE:

                  if (GetClipFileName(gTheSet.List[gTheSet.nNumClips].szFilename))

                    { // Add file

                      TCHAR szTitleBar[200];

                      DialogBox(ghInst, MAKEINTRESOURCE(wDlgRes = IDD_MEDIATIMES), ghApp, (DLGPROC)DialogProc);
                      gTheSet.nNumClips = gTheSet.nNumClips + 1;
                      wsprintf(szTitleBar, "SimpleCutList - %d clips(s) added.", gTheSet.nNumClips);
                      SetWindowText(ghApp, szTitleBar);

                    } // Add file

                  break;

                case IDM_RUN:
                  if (gTheSet.nNumClips > 1)
                    SimpleCutList();
                  else
                    DialogBox(ghInst, MAKEINTRESOURCE(wDlgRes = IDD_LESSTHAN2), ghApp, (DLGPROC)DialogProc);
                  break;

                case ID_FILE_ABOUT:
                  DialogBox(ghInst, MAKEINTRESOURCE(wDlgRes = IDD_ABOUT), ghApp, (DLGPROC)DialogProc);
                  break;

                case ID_FILE_EXIT:
                  if (gTheSet.nNumClips)
                    TearDownTheGraph();
                  PostQuitMessage(0);
                  break;

              }  // Program menu option

            break;

          case WM_GRAPHNOTIFY:

            if (!pimex)
              break;

            while (SUCCEEDED(pimex->GetEvent(&evCode, &evParam1, &evParam2, 0)))

              { // Spin through the events

                hr = pimex->FreeEventParams(evCode, evParam1, evParam2);

                if ((EC_COMPLETE == evCode) || (EC_USERABORT == evCode))

                  { // Finished

                    TearDownTheGraph();

                    if (EC_USERABORT == evCode)
                      gTheSet.nNumClips = 0;

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

      }  // WndMainProc //

  int PASCAL WinMain(HINSTANCE hInstC, HINSTANCE hInstP, LPSTR lpCmdLine, int nCmdShow)

    { // WinMain //
	  
      MSG msg;
      WNDCLASS wc;

      // OLE subsystem requires applications to initialize things first!
      CoInitialize(NULL);

      ZeroMemory(&wc, sizeof wc);
      wc.lpfnWndProc = WndMainProc;
      ghInst = wc.hInstance = hInstC;
      wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
      wc.lpszClassName = CLASSNAME;
      wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
      wc.hCursor = LoadCursor(NULL, IDC_ARROW);
      wc.hIcon = LoadIcon(hInstC, MAKEINTRESOURCE(IDI_CUTLIST));
      RegisterClass(&wc);

      RECT rc;

      GetWindowRect(GetDesktopWindow(), &rc);
      rc.right >>= 1;
      rc.bottom >>= 1;

      ghApp = CreateWindow(CLASSNAME,
	      APPLICATIONNAME,
	      WS_OVERLAPPEDWINDOW,
	      rc.right-200,
	      rc.bottom-200,
	      400,
	      400,
	      0,
	      0,
	      ghInst,
	      0);
		      
      ShowWindow(ghApp, SW_NORMAL);
      UpdateWindow(ghApp);

      ZeroMemory(&gTheSet, sizeof gTheSet);

      while (GetMessage(&msg,NULL,0,0))

	      {  // Message loop

	         TranslateMessage(&msg);
	         DispatchMessage(&msg);

	      }  // Message loop

      // Finished with OLE subsystem
      CoUninitialize();

      return msg.wParam;

  } // WinMain //

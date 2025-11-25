 
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
  #include <stdio.h>
  #include "cltext.h"

  ICutListGraphBuilder  *pCLGraphBuilder  = NULL;
  IStandardCutList      *pVideoCL         = NULL;
  IStandardCutList      *pAudioCL         = NULL;
  IGraphBuilder         *pigb             = NULL;
  IMediaControl         *pimc             = NULL;
  IMediaEventEx         *pimex            = NULL;
  IVideoWindow          *pivw             = NULL;
  IMediaSeeking         *pims             = NULL;

  IFileClip             *pVidFileClip[MAX_CLIPS];
  IFileClip             *pAudFileClip[MAX_CLIPS];
  IAMCutListElement       *pVidCLElem[MAX_CLIPS];
  IAMCutListElement       *pAudCLElem[MAX_CLIPS];

  HRESULT hr;

  int nVidElems, nAudElems;

  void CutlistFromTextfile ()

    { // CutlistFromTextfile //

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
          MBOX("CoCreateInstance of CutListGraphBuiler failed");
          TearDownTheGraph();
          return;
        } // CoCreateInstance of CutListGraphBuiler failed

      // Create simple (standard) cutlist object for video
      hr = CoCreateInstance(CLSID_SimpleCutList, NULL, 
                            CLSCTX_INPROC, IID_IStandardCutList, 
                            (void**)&pVideoCL);
      if (FAILED(hr))
        { // CoCreateInstance of video CutlistFromTextfile failed
          MBOX("CoCreateInstance of video CLSID_SimpleCutList failed");
          TearDownTheGraph();
          return;
        } // CoCreateInstance of video CLSID_SimpleCutList failed

      // Create simple (standard) cutlist object for audio
      hr = CoCreateInstance(CLSID_SimpleCutList, NULL, 
                            CLSCTX_INPROC, IID_IStandardCutList, 
                            (void**)&pAudioCL);

      if (FAILED(hr))
        { // CoCreateInstance of audio CutlistFromTextfile failed
          MBOX("CoCreateInstance of audio CLSID_SimpleCutList failed");
          TearDownTheGraph();
          return;
        } // CoCreateInstance of audio CutlistFromTextfile failed

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
                      MBOX("AddElement (video) failed!");

                    } // AddElement failed so release associated objects

                } // Add the element to the cutlist

              else MBOX("CreateCut (video) failed!");

            } // Create video cut

          else

            { // Problems creating video stream

              HELPER_RELEASE(pVidFileClip[nVidElems]);
              MBOX("SetFileAndStream (video) failed!");

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
                      MBOX("AddElement (audio) failed!");

                    } // AddElement failed so release associated objects

                } // Add the element to the cutlist

              else MBOX("CreateCut (audio) failed!");

            } // Create audio cut

          else
          
            { // Problems creating audio stream

              HELPER_RELEASE(pAudFileClip[nAudElems]);
              MBOX("SetFileAndStream (audio) failed!");

            } // Problems creating audio stream

        } // Individual clips

      // Add the video cutlist to the filter graph
      hr = pCLGraphBuilder->AddCutList(pVideoCL, NULL);

      if (FAILED(hr)) // AddCutList (video) failed
          MBOX("AddCutList (video) failed");

      // Add the audio cutlist to the filter graph
      hr = pCLGraphBuilder->AddCutList(pAudioCL, NULL);

      if (FAILED(hr)) // AddCutList (audio) failed
          MBOX("AddCutList (audio) failed");

      if ((!gTheSet.nNumClips) || (!pVideoCL) && (!pAudioCL))

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
            MBOX("Problems rendering the graph!");
          else 
            MBOX(gszScratch);
          TearDownTheGraph();
          return;
        } // Problems rendering the graph

      // Retrieve the filter graph and useful interfaces
      hr = pCLGraphBuilder->GetFilterGraph(&pigb);

      if (FAILED(hr))
        { // Problems retrieving the graph pointer
          if (!AMGetErrorText(hr, gszScratch, 2048))
            MBOX("Problems retrieving the graph pointer!");
          else 
            MBOX(gszScratch);
          TearDownTheGraph();
          return;
        } // Problems retrieving the graph pointer

      // QueryInterface for some basic interfaces
      pigb->QueryInterface(IID_IMediaControl, (void **)&pimc);
      pigb->QueryInterface(IID_IMediaEventEx, (void **)&pimex);
      pigb->QueryInterface(IID_IVideoWindow, (void **)&pivw);
      pigb->QueryInterface(IID_IMediaSeeking, (void **)&pims);

      // Decrement the ref count on the filter graph
      pigb->Release();

      // Prepare to play in the main application window's client area

      RECT rc;
      GetClientRect(ghApp, &rc);
      hr = pivw->put_Owner((OAHWND)ghApp);
      hr = pivw->put_WindowStyle(WS_CHILD|WS_CLIPSIBLINGS);
      hr = pivw->SetWindowPosition(rc.left, rc.top, rc.right, rc.bottom);

      hr = pims->SetTimeFormat(&TIME_FORMAT_MEDIA_TIME);
      hr = pims->GetDuration(&glTotalLength);

      SetScrollRange(ghApp, SB_HORZ, 0, 100, TRUE);
 
      //Ready to rumble...
      pimc->Run();

    } // CutlistFromTextfile //

  void TearDownTheGraph (void)

      { // TearDownTheGraph //

        if (gTimerNum)
          timeKillEvent(gTimerNum);

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
        HELPER_RELEASE(pims);

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

        gTheSet.nNumClips = 0;

      } // TearDownTheGraph //

  BOOL GetCliplistTxtFilename (LPSTR szName)

    {   // GetCliplistTxtFilename //

        OPENFILENAME ofn;

        ofn.lStructSize       = sizeof(OPENFILENAME);
        ofn.hwndOwner         = ghApp;
        ofn.lpstrFilter       = NULL;
        ofn.lpstrFilter       = "Text Files (clip list) (*.txt)\0*.txt\0\0\0";
        ofn.lpstrCustomFilter = NULL;
        ofn.nFilterIndex      = 1;
        *szName               = 0;
        ofn.lpstrFile         = szName;
        ofn.nMaxFile          = MAX_PATH;
        ofn.lpstrInitialDir   = NULL;
        ofn.lpstrTitle        = NULL;
        ofn.lpstrFileTitle    = NULL;
        ofn.lpstrDefExt       = "TXT";
        ofn.Flags             = OFN_FILEMUSTEXIST | OFN_READONLY | OFN_PATHMUSTEXIST;

        return GetOpenFileName((LPOPENFILENAME)&ofn);

    }   // GetCliplistTxtFilename //

  LRESULT CALLBACK WndMainProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)

    { // WndMainProc //

      LONGLONG l;

      switch(message)

        { // Window msgs handling

          case WM_COMMAND:

            switch(wParam)

              {  // Program menu option

                case ID_FILE_EXIT:
                  if (gTheSet.nNumClips)
                    TearDownTheGraph();
                  PostQuitMessage(0);
                  break;

                case ID_CUTLIST_PLAY:
                  l = 0;
                  hr = pimc->Stop();
                  hr = pims->SetPositions(&l, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning);
                  hr = pimc->Run();
                  break;

              }  // Program menu option

            break;

          case WM_HSCROLL:

            if (SB_THUMBPOSITION == LOWORD(wParam))

              { // Seeking via horiz. scroll

                l = (glTotalLength * (LONGLONG)HIWORD(wParam))/100;
                pimc->Stop();
                hr = pims->SetPositions(&l, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning);
                pimc->Run();

              } // Seeking via horiz. scroll
            
          break;

          case WM_EXITSIZEMOVE:

            RECT rc;
            GetClientRect(ghApp, &rc);
            if (pivw)
              pivw->SetWindowPosition(rc.left, rc.top, rc.right, rc.bottom);
            break;

          case WM_SIZE:
            if ((SIZE_RESTORED == wParam) || (SIZE_MAXIMIZED == wParam))
              PostMessage(ghApp, WM_EXITSIZEMOVE, 0, 0);
            break;

          case WM_DESTROY:
            if (gTheSet.nNumClips)
              TearDownTheGraph();
            PostQuitMessage(0);
            break;

          default:
            return DefWindowProc(hWnd, message, wParam, lParam);
	          
        } // Window msgs handling

      return FALSE;

      }  // WndMainProc //

  void ReadTextFile (LPCSTR cFilename)

    { // ReadTextFile //

      FILE  *fp;
      TCHAR cString[MAX_PATH];

      if (!(fp = fopen(cFilename, "rt")))
        return; // File not found!

      while (!feof(fp) && (gTheSet.nNumClips < MAX_CLIPS))

        { // Parse individual lines

          ZeroMemory(cString, MAX_PATH);
          fgets(cString, MAX_PATH, fp);

          if (lstrlen(cString) && (cString[0] != TCHAR(';')))

            { // Decipher filename, start, stop times

              int i = lstrlen(cString);

              while (i && (cString[i] != TCHAR(','))) --i;
              gTheSet.List[gTheSet.nNumClips].stop = atol(&cString[i+1]); --i;
              while (i && (cString[i] != TCHAR(','))) --i;
              gTheSet.List[gTheSet.nNumClips].start = atol(&cString[i+1]);
              cString[i] = TCHAR('\0');
              lstrcpy(gTheSet.List[gTheSet.nNumClips].szFilename, cString);
              gTheSet.nNumClips += 1;

            } // Deciphering

        } // Parse individual lines

      fclose(fp);

    } // ReadTextFile //

  void CALLBACK TimerProc (UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
  
    { // TimerProc //

      LONGLONG lPos;

      if (pims)
        pims->GetCurrentPosition(&lPos);
      lPos = (lPos * 100)/glTotalLength;
      SetScrollPos(ghApp, SB_HORZ, (int)lPos, TRUE);

    } // TimerProc //

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
      wc.hCursor = LoadCursor(NULL, IDC_ARROW);
      wc.hIcon = LoadIcon(hInstC, MAKEINTRESOURCE(IDI_CUTLIST));
      RegisterClass(&wc);

      RECT rc;

      GetWindowRect(GetDesktopWindow(), &rc);
      rc.right >>= 1;
      rc.bottom >>= 1;

      ghApp = CreateWindow(CLASSNAME,
	      APPLICATIONNAME,
	      WS_OVERLAPPEDWINDOW|WS_HSCROLL,
	      rc.right-200,
	      rc.bottom-200,
	      400,
	      400,
	      0,
	      0,
	      ghInst,
	      0);
		      
      SetMenu(ghApp, ghMenu = LoadMenu(hInstC, MAKEINTRESOURCE(IDR_MENU1)));

      ShowWindow(ghApp, SW_NORMAL);
      UpdateWindow(ghApp);

      ZeroMemory(&gTheSet, sizeof gTheSet);

      // Text-file

      TCHAR szFilename[MAX_PATH];

      if (lstrlen(lpCmdLine))
        ReadTextFile(lpCmdLine);
      else
        if (GetCliplistTxtFilename(szFilename))
          ReadTextFile(szFilename);
        else
          return FALSE;

      if (gTheSet.nNumClips)
        CutlistFromTextfile();

      // Establish a callback-based tracking timer
      gTimerNum = 0;
      if (nVidElems || nAudElems)
        gTimerNum = timeSetEvent(125, 0, TimerProc, 0, TIME_PERIODIC);

      while (GetMessage(&msg,NULL,0,0))

	      {  // Message loop

	         TranslateMessage(&msg);
	         DispatchMessage(&msg);

	      }  // Message loop

      DestroyMenu(ghMenu);

      // Finished with OLE subsystem
      CoUninitialize();

      return msg.wParam;

  } // WinMain //

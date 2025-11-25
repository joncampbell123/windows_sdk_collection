
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
  #include "setup.h"

  // This sample demonstrates how a 3rd party package can set up the DirectXMedia
  // redist. bits from their setup program. This sample assumes the DXMEDIA.EXE
  // is sitting in the same directory as the compiled version of this code.

  #define TITLE "Sample DXMedia Install Package"

  // Forward prototyping
  BOOL CALLBACK DialogProc (HWND, UINT, WPARAM, LPARAM);

  DWORD ExecuteSynchCmdLine (LPTSTR szProgExe, LPTSTR szCmdLine)

    { // ExecuteSynchCmdLine //

      DWORD dwTimeout = 30;
      DWORD dwRC      = WAIT_TIMEOUT;
      MSG   msg;
      DWORD dw;
      STARTUPINFO StartupInfo;
      PROCESS_INFORMATION ProcessInformation;

      ZeroMemory(&StartupInfo, sizeof STARTUPINFO);
      StartupInfo.cb = sizeof STARTUPINFO; 
      StartupInfo.wShowWindow = SW_NORMAL;
      ZeroMemory(&ProcessInformation, sizeof PROCESS_INFORMATION);

      TCHAR cPad[MAX_PATH];
      lstrcpy(cPad, " ");
      lstrcat(cPad, szCmdLine);

      if (CreateProcess(szProgExe, cPad, NULL, NULL, NULL, NULL, NULL, NULL, &StartupInfo, &ProcessInformation))

        { // Spin off child process

          while (dwRC == WAIT_TIMEOUT)

            { // Poll status every 'dwTimeout' milliseconds

              dwRC = WaitForSingleObject(ProcessInformation.hProcess, dwTimeout);

              while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
                {
                   TranslateMessage(&msg);
                   DispatchMessage(&msg);
                }    

            } // Poll status every 'dwTimeout' milliseconds

        } // Spin off child process

    GetExitCodeProcess(ProcessInformation.hProcess, &dw);

    return dw;

    } // ExecuteSynchCmdLine //

  int WINAPI WinMain (HINSTANCE a, HINSTANCE b, LPSTR c, int d)

	  {	// WinMain //

      HRESULT hr = NOERROR;

	  // Prompt to install the package
      if (!DialogBox(a, MAKEINTRESOURCE(IDD_DIALOG1), GetDesktopWindow(), (DLGPROC)DialogProc))
        return FALSE; // Selected CANCEL

      // DXMedia.exe redist. package needs to be in same (current) directory!
      hr = ExecuteSynchCmdLine("dxmedia.exe","-id:ExampleIdentifier");

      switch(hr)

        { // Most common return codes

          case ERROR_SUCCESS:
            MessageBox(GetDesktopWindow(), "Redist. package installed successfully; no reboot required.", TITLE, MB_OK);
            break;

          case ERROR_SUCCESS_REBOOT_REQUIRED:
            MessageBox(GetDesktopWindow(), "Redist. package installed successfully; reboot required!", TITLE, MB_ICONHAND);
            break;

          case E_FAIL:
            MessageBox(GetDesktopWindow(), "Redist. package failed to install! Is dxmedia.exe in the same directory?", TITLE, MB_ICONHAND);
            break;

          default:

            TCHAR cDbgMsg[255];
            wsprintf(cDbgMsg, "Redist. package returned an unexpected error code (hr=0x%08X)", hr);
            MessageBox(GetDesktopWindow(), cDbgMsg, TITLE, MB_ICONHAND);
            break;

        } // Most common return codes

      return FALSE;

    }	// WinMain //

  BOOL CALLBACK DialogProc (HWND hWnd, UINT mMsg, WPARAM wParam, LPARAM lParam)

    { // DialogProc //

      switch(mMsg)

        { // Msg handling

          case WM_COMMAND:

            switch(wParam)

              { // Command msg handling

                case IDOK:
                  EndDialog(hWnd,1);
                  break;

                case IDCANCEL:
                  EndDialog(hWnd,0);
                  break;

              } // Command msg handling

            break;

        } // Msg handling

      return 0;

    } // DialogProc //

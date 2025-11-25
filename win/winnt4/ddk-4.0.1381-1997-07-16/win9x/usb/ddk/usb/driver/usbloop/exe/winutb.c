/****************************************************************************

Copyright (c) 1995  Microsoft Corporation

Module Name:

    winutb.c

Abstract:

    Ring3 loopback app for USBLOOP.SYS

Environment:

    user mode only

Notes:

  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
  PURPOSE.

  Copyright (c) 1996 Microsoft Corporation.  All Rights Reserved.


Revision History:

    4-4-96  : created
	8-15-96 : added support for ReadFile Writefile interface of USBLOOP
	9-25-96 : first cut of async finished

	The USBLOOP driver now supports a ReadFile WriteFile interface to the
	pipes on the test board. The DeviceIOControl interface is still
	supported, so you can use it for configuration of the test board.
	Now however, you should use ReadFile and WriteFile calls to read and
	write data for pipes. To open the device for ioctl, open as before
	"USBLOOPXXX". To open an individual pipe, open the device with
	the following tacked on:

	\configuration\interface\alt-interface\pipenum

	Here is what one might look like:

	\USBLOOPXXX\0\0\0\1

	Each part of the device number is a decimal integer representing
	the configuration, interface, alt-interface, and pipe number.

	You cannot use ReadFile and WriteFile calls on the device itself
	("USBLOOPXXX"), it is only used for ioctls.

	The ReadFile WriteFile interface offers a few advantages:

	1. Makes USB pipe on test board look more like a "real" device.
	   This allows you to use statndard ReadFile and WriteFile calls
	   without stuffing a data structure and doing an ioctl
	2. Allows reading and writing pipe in separate operations


	If you must use the old DeviceIOControl interface, just get rid of the
	line that defined RDWR_INTERFACE.

****************************************************************************/

#include "winutb.h"
#include <commctrl.h>
#include <memory.h>
#include <conio.h>

#define NUMLINES ((int) (MAX_LOOP / ulCount))

//*------------------------------------------------------------------------
//| WinMain:
//|     Parameters:
//|         hInstance     - Handle to current Data Segment
//|         hPrevInstance - Handle to previous Data Segment (NULL if none)
//|         lpszCmdLine   - Long pointer to command line info
//|         nCmdShow      - Integer value specifying how to start app.,
//|                            (Iconic [7] or Normal [1,5])
//*------------------------------------------------------------------------
int PASCAL WinMain (HANDLE hInstance,
					HANDLE hPrevInstance,
					LPSTR  lpszCmdLine,
					int    nCmdShow)
{
	int nReturn=0;

	if (Init(hInstance, hPrevInstance,lpszCmdLine,nCmdShow))
		nReturn = DoMain(hInstance);

	return nReturn;
}

//*------------------------------------------------------------------------
//| Init
//|     Initialization for the program is done here:
//|     1)  Register the window class (if this is the first instance)
//|     2)  Create the desktop window for the app.
//|     3)  Show the desktop window in the manner requested by the User.
//|
//*------------------------------------------------------------------------
HANDLE Init(HANDLE hInstance,   HANDLE hPrevInstance,
		  LPSTR  lpszCmdLine, int    nCmdShow)
{
	WNDCLASS    rClass;

		// init needed variables
		
	iNumDevices = 0;
	iDevice = 0;
	iInstance = 0;
	iIterations = -1;
	iINpipe = -1;
	iOUTpipe = -1;
	ulNumberDevices = 0;
	bTestRunning = FALSE;
	hUSBLOOP = INVALID_HANDLE_VALUE;
	iInputBuffLen = DEFAULT_TRANSACT_BUFFER;
	iOutputBuffLen = DEFAULT_TRANSACT_BUFFER;
	bAutoClose = FALSE;
	displayFreq = 100;
	bAsyncTest = FALSE;

	strcpy(szDeviceName, "\\\\.\\USBLOOPXXX");
	
	if (!hPrevInstance)
	{
		rClass.style         = CS_HREDRAW | CS_VREDRAW;
		rClass.lpfnWndProc   = (WNDPROC) UTBTestApp;
		rClass.cbClsExtra    = 0;
		rClass.cbWndExtra    = 0;
		rClass.hInstance     = hInstance;
		rClass.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDC_LOOP_ICON));
		rClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
		rClass.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
		rClass.lpszMenuName  = szClassName;
    	rClass.lpszClassName = szClassName;

    	if (!RegisterClass( &rClass))
        	return FALSE;
	}

	hWnd1 = CreateWindow(szClassName, szAppName, 
		WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 
        CW_USEDEFAULT, CW_USEDEFAULT, 
		NULL, NULL, hInstance, NULL);


	hMenu1 = LoadMenu(hInstance, MAKEINTRESOURCE(LoopBackUSB));
	CreateMenu();
	SetMenu(hWnd1, hMenu1);

	// init common controls for track bar
	InitCommonControls();
	ShowWindow(hWnd1,nCmdShow);
	UpdateWindow(hWnd1);
	hMenu1 = GetMenu(hWnd1);
	
	EnableMenuItem(hMenu1,IDM_OPEN_USBLOOP, MF_ENABLED);
	EnableMenuItem(hMenu1,IDM_STOP_LOOPBACK, MF_GRAYED);
	EnableMenuItem(hMenu1,IDM_CLOSE_USBLOOP, MF_GRAYED);
	EnableMenuItem(hMenu1,IDM_SHOW_DEV_DESC, MF_GRAYED);
	EnableMenuItem(hMenu1,IDM_SHOW_CONF_DESC, MF_GRAYED);
	EnableMenuItem(hMenu1,IDM_SHOW_INTF_DESC, MF_GRAYED);
	EnableMenuItem(hMenu1,IDM_SHOW_ENDP_DESC, MF_GRAYED);
	EnableMenuItem(hMenu1,IDM_DO_LOOPBACK, MF_GRAYED);
	EnableMenuItem(hMenu1,IDM_GET_VERSION, MF_GRAYED);


		// If this is to be automated, then dialogs will 
		// not be presented. The values passed on the 
		// command line will be used.
		
	bAutomate = ctlParseCmdLine(lpszCmdLine);
	
	if(bAutomate == TRUE)
	{

		SendMessage(hWnd1, WM_COMMAND, IDM_OPEN_USBLOOP, 0L);
		SendMessage(hWnd1, WM_COMMAND, IDM_DO_LOOPBACK, 0L);
		SendMessage(hWnd1, WM_COMMAND, IDM_CLOSE_USBLOOP, 0L);
		if(bAutoClose == TRUE)
    		SendMessage(hWnd1, WM_COMMAND, IDM_EXITAPP, 0L);		    
		bAutomate = FALSE;
	}
	else if (bAutomate == USAGE_ERROR)
	{
		DialogBox(hInstance, "UsageBox", hWnd1, DisplayUsage);
		return FALSE;
	}
	
	return hWnd1;
}


//*------------------------------------------------------------------------
//| DoMain:
//|     This is the main loop for the application:
//*------------------------------------------------------------------------
int  DoMain(HANDLE hInstance)
{
	MSG msg;

	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

    return msg.wParam;
}

//*------------------------------------------------------------------------
//| OverlappedWindowProc1
//|     Parameters:
//|         hWnd    - Handle to Window which message is delivered to.
//|         msgID   - ID number of message
//|         wParam  - 16-bit parameter
//|         lParam  - 32-bit parameter
//|
//*------------------------------------------------------------------------
LONG FAR PASCAL UTBTestApp(HWND hWnd, unsigned wMsgID, 
								WORD wParam, LONG lParam)
{
	static HANDLE hInstance;
	RECT rRect;
	DWORD dwRet = TRUE, cbBytesRet;
	char tmpBuff[10];
	TEXTMETRIC tm;
	HDC hdc;
	static int  cxChar, cxCaps, cyChar, cxClient, cyClient, iMaxWidth,
            iVscrollPos, iVscrollMax;
    int iVscrollInc;
  	RECT rcClient;
	PVOID pvBuffer;
	ULONG ulSiz, ulVersion = 0;
	USHORT usDiv1, usDiv2, usInst;
	USBD_VERSION_INFORMATION VersionInfo;
	unsigned char *outBuff;
	unsigned char *inBuff;
	HANDLE hInpipe;
	HANDLE hOutpipe;
	char pipeDeviceName[30];
	unsigned char rNum;
	int index;
	static ULONG pass = 0, fail = 0;
	OVERLAPPED inOverlapped, outOverlapped;
	int flags;
	
    switch (wMsgID) 
    {
		case WM_CREATE:
			hdc = GetDC (hWnd);
			GetTextMetrics (hdc, &tm);
			cxChar = tm.tmAveCharWidth ;
	    	cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * cxChar / 2 ;
           	cyChar = tm.tmHeight + tm.tmExternalLeading ;

            iMaxWidth = 40 * cxChar + 22 * cxCaps ;
			ReleaseDC (hWnd, hdc);
			
			hInstance = ((LPCREATESTRUCT) lParam)->hInstance;
			GetClientRect(hWnd, &rRect);

				// create window for logging data

	        hLogWindow=CreateWindow("LISTBOX", NULL,
		            WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | LBS_NOINTEGRALHEIGHT,
		            0,0, rRect.right, rRect.bottom, hWnd, NULL,
					(HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE),NULL);
          		
         	if (NULL == hLogWindow)
          		return -1;

			return 0;

		case WM_SIZE:

                GetClientRect(hWnd,&rcClient);
      			SetWindowPos(hLogWindow, NULL, 0, 0,
         			rcClient.right+(GetSystemMetrics(SM_CXBORDER)*2),
         			rcClient.bottom+(GetSystemMetrics(SM_CXBORDER)*2),
         			SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);

               return 0 ;

          case WM_VSCROLL:
               switch (LOWORD (wParam))
                    {
                    case SB_TOP :
                         iVscrollInc = -iVscrollPos ;
                         break ;

                    case SB_BOTTOM :
                         iVscrollInc = iVscrollMax - iVscrollPos ;
                         break ;

                    case SB_LINEUP :
                         iVscrollInc = -1 ;
                         break ;

                    case SB_LINEDOWN :
                         iVscrollInc = 1 ;
                         break ;

                    case SB_PAGEUP :
                         iVscrollInc = min (-1, -cyClient / cyChar) ;
                         break ;

                    case SB_PAGEDOWN :
                         iVscrollInc = max (1, cyClient / cyChar) ;
                         break ;

                    case SB_THUMBTRACK :
                         iVscrollInc = HIWORD (wParam) - iVscrollPos ;
                         break ;

                    default :
                         iVscrollInc = 0 ;
                    }
               iVscrollInc = max (-iVscrollPos,
                             min (iVscrollInc, iVscrollMax - iVscrollPos)) ;

               if (iVscrollInc != 0)
                    {
                    iVscrollPos += iVscrollInc ;
                    ScrollWindow (hWnd, 0, -cyChar * iVscrollInc, NULL, NULL) ;
                    SetScrollPos (hWnd, SB_VERT, iVscrollPos, TRUE) ;
                    UpdateWindow (hWnd) ;
                    }
               return 0 ;
               
		case WM_COMMAND:

			switch (wParam)
			{

				case IDM_OPEN_USBLOOP:

						// call the dialog to allow the user to decide
						// which device to loop to 

					if(!bAutomate)
						dwRet = DialogBox(hInstance, "DevHandl", hWnd, DevHandlDlg);
					else
					{
						usDiv2 = (USHORT) (iInstance / 100);
						usDiv1 = (USHORT) ((iInstance - (usDiv2 * 100)) / 10);
						usInst = (USHORT) (iInstance - ((usDiv2 * 100) + (usDiv1 * 10)));

						szDeviceName[11] = (UCHAR) ('0' + usDiv2);
						szDeviceName[12] = (UCHAR) ('0' + usDiv1);
						szDeviceName[13] = (UCHAR) ('0' + usInst);

							// open that instance and set global handle variable

						hUSBLOOP = CreateFile(szDeviceName,
								GENERIC_WRITE | GENERIC_READ,
								FILE_SHARE_WRITE | FILE_SHARE_READ,
								NULL,
								OPEN_EXISTING,
								0,
								NULL);	
								
						if(hUSBLOOP == INVALID_HANDLE_VALUE)
							DisplayOutput("Failed to open %s in AUTOMATE", szDeviceName); 				
						else						
							DisplayOutput("Opened successfully %s", szDeviceName); 				
					}
					
					if(dwRet)
					{

							EnableMenuItem(hMenu1,IDM_OPEN_USBLOOP, MF_GRAYED);
							EnableMenuItem(hMenu1,IDM_STOP_LOOPBACK, MF_GRAYED);
							EnableMenuItem(hMenu1,IDM_CLOSE_USBLOOP, MF_ENABLED);
							EnableMenuItem(hMenu1,IDM_SHOW_DEV_DESC, MF_ENABLED);
							EnableMenuItem(hMenu1,IDM_SHOW_CONF_DESC, MF_ENABLED);
							EnableMenuItem(hMenu1,IDM_SHOW_INTF_DESC, MF_ENABLED);
							EnableMenuItem(hMenu1,IDM_SHOW_ENDP_DESC, MF_ENABLED);
							EnableMenuItem(hMenu1,IDM_DO_LOOPBACK, MF_ENABLED);
							EnableMenuItem(hMenu1,IDM_GET_VERSION, MF_ENABLED);
					}																	
					break;

					// the following 4 case statements simply 
					// output the descriptors to the screen
					
				case IDM_SHOW_DEV_DESC:

					ulSiz = sizeof(USB_DEVICE_DESCRIPTOR);
					if((pvBuffer = HeapAlloc(GetProcessHeap(), 0,
							ulSiz)) != NULL)
					{

						dwRet = DeviceIoControl
								(hUSBLOOP, 
								 GET_DEVICE_DESCRIPTOR,
	 		 					(PVOID)pvBuffer,   //inbuff
					     		 ulSiz,
	 		 					(PVOID)pvBuffer,   //outbuff
					     		 ulSiz,
								&cbBytesRet,
								NULL);

						OutputDeviceDesc((PUSB_DEVICE_DESCRIPTOR)pvBuffer);
						HeapFree(GetProcessHeap(), 0, pvBuffer);
					}

					
					break;
					
				case IDM_SHOW_CONF_DESC:
				case IDM_SHOW_INTF_DESC:
				case IDM_SHOW_ENDP_DESC:

					ulSiz = sizeof(USB_CONFIGURATION_DESCRIPTOR)+256;
					if((pvBuffer = HeapAlloc(GetProcessHeap(), 0,
							ulSiz)) != NULL)
					{

						dwRet = DeviceIoControl
								(hUSBLOOP, 
								 GET_CONFIG_DESCRIPTOR,
	 		 					(PVOID)pvBuffer,   //inbuff
					     		 ulSiz,
	 		 					(PVOID)pvBuffer,   //outbuff
					     		 ulSiz,
								&cbBytesRet,
								NULL);

						if(wParam == IDM_SHOW_CONF_DESC)
							OutputConfigDesc((PUSB_CONFIGURATION_DESCRIPTOR)pvBuffer);
/*						else if(wParam == IDM_SHOW_INTF_DESC)
						{
							OutputInterfaceDesc((PUSB_CONFIG_DESCRIPTOR)pvBuffer);
						}
						else if(wParam == IDM_SHOW_CONF_DESC)
						{
							OutputEndptDesc((PUSB_CONFIG_DESCRIPTOR)pvBuffer);
						}
	*/					
						HeapFree(GetProcessHeap(), 0, pvBuffer);
					}

					break;
					
				case IDM_DO_LOOPBACK:


						// call the dialog routine to present the user
						// with choices of endpints for loopback
						// and the number of interations

					
					if(!bAutomate)
						if(!DialogBox(hInstance, "SelPipes", hWnd, SelectPipesDlg))
							break;

						// user chose to loop forever

                    strcpy(chBuff, szDeviceName);
                    strcat(chBuff, " ");
                    strcat(chBuff, "IN: ");
                    strcat(chBuff, _itoa(iINpipe, tmpBuff, 10));
                    strcat(chBuff, " OUT: ");
                    strcat(chBuff, _itoa(iOUTpipe, tmpBuff, 10));
		 			SetWindowText(hWnd, chBuff);

					EnableMenuItem(hMenu1,IDM_STOP_LOOPBACK, MF_ENABLED);
					EnableMenuItem(hMenu1,IDM_DO_LOOPBACK, MF_GRAYED);
					EnableMenuItem(hMenu1,IDM_CLOSE_USBLOOP, MF_GRAYED);

					// reset pass fail counts, test running flag and count
					pass = fail = 0;
					bTestRunning = TRUE;
					ulCount = 0;

					// allocate buffers here
					outBuff = (unsigned char *) GlobalAlloc(GMEM_FIXED, iOutputBuffLen);
					if(outBuff == NULL)
					{
						ResetLoopMenu();						
						MessageBox(hWnd, "Unable to allocate memory", szAppName, MB_OK);
						break;
					}

					inBuff = (unsigned char *) GlobalAlloc(GMEM_FIXED, iInputBuffLen);
					if(inBuff == NULL)
					{
						GlobalFree(outBuff);
						ResetLoopMenu();						
						MessageBox(hWnd, "Unable to allocate memory", szAppName, MB_OK);
						break;
					}

					// setup flags for CreateFile, if async flag is set,
					// include FILE_FLAG_OVERLAPPED
					if(bAsyncTest)
						flags = FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_FLAG_OVERLAPPED;
					else
						flags = FILE_SHARE_WRITE | FILE_SHARE_READ;
					// open input pipe, get pipe from when entered in dialog box
					strcpy(pipeDeviceName, szDeviceName);
					// concat pipe number onto device name
					strcat(pipeDeviceName, "\\0\\0\\0\\");
					strcat(pipeDeviceName, _itoa(iINpipe, tmpBuff, 10));
					hInpipe = CreateFile(pipeDeviceName,
							GENERIC_WRITE | GENERIC_READ,
							flags,
							NULL,
							OPEN_EXISTING,
							0,
							NULL);
					if(hInpipe == INVALID_HANDLE_VALUE)
					{
						char errStr[100];

						wsprintf(errStr, "Unable to open device: 0x%x",
								 GetLastError());
						ResetLoopMenu();
						MessageBox(hWnd, errStr, szAppName, MB_OK);
						break;
					}

					// open output pipe, get pipe from when entered in dialog box
					strcpy(pipeDeviceName, szDeviceName);
					// concat pipe number onto device name
					strcat(pipeDeviceName, "\\0\\0\\0\\");
					strcat(pipeDeviceName, _itoa(iOUTpipe, tmpBuff, 10));
					hOutpipe = CreateFile(pipeDeviceName,
							GENERIC_WRITE | GENERIC_READ,
							flags,
							NULL,
							OPEN_EXISTING,
							0,
							NULL);	
					if(hOutpipe == INVALID_HANDLE_VALUE)
					{
						char errStr[100];

						wsprintf(errStr, "Unable to open device: 0x%x",
								 GetLastError());
						CloseHandle(hInpipe);
						ResetLoopMenu();
						MessageBox(hWnd, errStr, szAppName, MB_OK);
						break;
					}

					DisplayOutput("**************************************************");								

					while(bTestRunning)
					{
						MSG    msg;

						// check for messages here, this has to go before
						// incrementing ulCount (in case we get
						// IDM_STOP_LOOPBACK message
						while (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
						{
							TranslateMessage(&msg);
							DispatchMessage(&msg);
						}

						ulCount++;

						if(iOUTpipe >= MIN_PIPE)
						{
							// init buffer starting with random value from 0 to 255
							rNum = (unsigned char) (rand() % 0x100);
							for(index = 0; index < iOutputBuffLen; index++)
								outBuff[index] = rNum++;

							if(bAsyncTest)
							{
								// setup Overlapped structure
								memset(&outOverlapped, 0, sizeof(outOverlapped));
								WriteFile(hOutpipe, outBuff, iOutputBuffLen, &dwRet, &outOverlapped);
								// wait on the handle for the pipe
								WaitForSingleObject(hOutpipe, INFINITE);
							}
							// doing a synchronous write
							else
							{
								WriteFile(hOutpipe, outBuff, iOutputBuffLen, &dwRet, NULL);
								if(!(dwRet))
								{
									DisplayOutput("WriteFile() failed. Error: 0x%x", GetLastError());
									ResetLoopMenu();
									break;
								}
							}
						}

						if(iINpipe >= MIN_PIPE)
						{
							if(bAsyncTest)
							{
								// setup Overlapped structure
								memset(&inOverlapped, 0, sizeof(inOverlapped));
								ReadFile(hInpipe, inBuff, iInputBuffLen, &dwRet, &inOverlapped);
								// wait on the handle for the pipe
								WaitForSingleObject(hInpipe, INFINITE);
							}
							// doing a synchronous read
							else
							{
								ReadFile(hInpipe, inBuff, iInputBuffLen, &dwRet, NULL);
								if(!(dwRet))
								{
									DisplayOutput("ReadFile() failed. Error: 0x%x", GetLastError());
									ResetLoopMenu();
									break;
								}
							}
							// verify buffer
							if(!memcmp(inBuff, outBuff, iInputBuffLen))
								pass++;
							else
							{
								DisplayOutput("READ BUFFERS DID NOT COMPARE:   %ld bytes     #%ld", dwRet, ulCount);
								fail++;
							}

							if((ulCount % displayFreq) == 0)
							{
								DisplayOutput("Pass = %ld   Fail = %ld", pass, fail);
							}
						}
						// if a count was specified, see if we need to stop test
						if(iIterations != -1)
						{
							if(ulCount == (ULONG) iIterations)
							{
								// send ourselves a message to end test
								SendMessage(hWnd, WM_COMMAND, IDM_STOP_LOOPBACK, 0L);
								break;
							}
						}
					}
					DisplayOutput("**************************************************");	

					CloseHandle(hInpipe);
					CloseHandle(hOutpipe);
					GlobalFree(outBuff);
					GlobalFree(inBuff);
					break;

				case IDM_STOP_LOOPBACK:
					bTestRunning = FALSE;
					wsprintf(chBuff, "Total iterations %ld.", ulCount);
					DisplayOutput("Total iterations %ld.", ulCount);
					DisplayOutput("Pass = %ld   Fail = %ld", pass, fail);

					if(ulCount > 0)
						MessageBox(hWnd, chBuff, szAppName, MB_OK);

						// this is an ugly way to stop it
					SetWindowText(hWnd, szAppName);					
					EnableMenuItem(hMenu1,IDM_STOP_LOOPBACK, MF_GRAYED);
					EnableMenuItem(hMenu1,IDM_CLOSE_USBLOOP, MF_ENABLED);
					EnableMenuItem(hMenu1,IDM_DO_LOOPBACK, MF_ENABLED);
				
					break;
					
				case IDM_CLOSE_USBLOOP:

						// close the driver and stop the looping
						
					EnableMenuItem(hMenu1,IDM_OPEN_USBLOOP, MF_ENABLED);
					EnableMenuItem(hMenu1,IDM_STOP_LOOPBACK, MF_GRAYED);
					EnableMenuItem(hMenu1,IDM_CLOSE_USBLOOP, MF_GRAYED);
					EnableMenuItem(hMenu1,IDM_SHOW_DEV_DESC, MF_GRAYED);
					EnableMenuItem(hMenu1,IDM_SHOW_CONF_DESC, MF_GRAYED);
					EnableMenuItem(hMenu1,IDM_SHOW_INTF_DESC, MF_GRAYED);
					EnableMenuItem(hMenu1,IDM_SHOW_ENDP_DESC, MF_GRAYED);
					EnableMenuItem(hMenu1,IDM_DO_LOOPBACK, MF_GRAYED);
					EnableMenuItem(hMenu1,IDM_GET_VERSION, MF_GRAYED);
						
					CleanUp(hWnd);
					break;

				case IDM_GET_VERSION:

					dwRet = DeviceIoControl
							(hUSBLOOP, 
							GET_VERSION,
	 		 				&VersionInfo,   //in buff
					     	sizeof(USBD_VERSION_INFORMATION),
	 		 				&VersionInfo,   //out buff
					     	sizeof(USBD_VERSION_INFORMATION),
							&cbBytesRet,
							NULL);

					DisplayOutput("USBD Version: 0x%x", 
							VersionInfo.USBDI_Version);
							
					DisplayOutput("Support USB Spec Version: 0x%x", 
							VersionInfo.Supported_USB_Version);

					break;
					
				case IDM_ABOUT:
					DialogBox(hInstance, "AboutBox", hWnd, AboutDlgProc);
					break;

				case IDM_EXITAPP:

						// close the driver and stop the looping
						
					CleanUp(hWnd);						
					SendMessage(hWnd, WM_CLOSE, 0, 0L);
					break;

			}
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

    	default:
        	return DefWindowProc(hWnd, wMsgID, wParam, lParam);
			break;
    }
	
	return 0;
}

//*------------------------------------------------------------------------
//| Cleanup:
//|     This is the code to clean up.
//*------------------------------------------------------------------------
void CleanUp(HWND hWnd)
{

		// BUGUG should probably somehow make this an 
		// event that gets signalled before closing 
		// the handle

	bTestRunning = FALSE;
	SetWindowText(hWnd, szAppName);					
	CloseHandle(hUSBLOOP);
	hUSBLOOP = INVALID_HANDLE_VALUE;
	DisplayOutput("Closed successfully %s", szDeviceName); 				

	if(ulCount >= 1 )
	{
		wsprintf(chBuff, "Total iterations %ld.", ulCount);
		MessageBox(hWnd, chBuff, szAppName, MB_OK);
		DisplayOutput("Total iterations %ld.", ulCount);
	}
	ulCount = 0;
}


//*------------------------------------------------------------------------
//| ResetLoopMenu:
//|     This is the code to clean up.
//*------------------------------------------------------------------------
void ResetLoopMenu()
{

	EnableMenuItem(hMenu1,IDM_OPEN_USBLOOP, MF_ENABLED);
	EnableMenuItem(hMenu1,IDM_STOP_LOOPBACK, MF_GRAYED);
	EnableMenuItem(hMenu1,IDM_SHOW_DEV_DESC, MF_ENABLED);
	EnableMenuItem(hMenu1,IDM_SHOW_CONF_DESC, MF_ENABLED);
	EnableMenuItem(hMenu1,IDM_SHOW_INTF_DESC, MF_ENABLED);
	EnableMenuItem(hMenu1,IDM_SHOW_ENDP_DESC, MF_ENABLED);
	EnableMenuItem(hMenu1,IDM_CLOSE_USBLOOP, MF_GRAYED);
	EnableMenuItem(hMenu1,IDM_DO_LOOPBACK, MF_ENABLED);
	EnableMenuItem(hMenu1,IDM_GET_VERSION, MF_ENABLED);

}


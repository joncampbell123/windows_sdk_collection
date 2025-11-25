/****************************************************************************

Copyright (c) 1995  Microsoft Corporation

Module Name:

    dlgutb.c

Abstract:

    Ring3 loopback app dialog routines

Environment:

    user mode only

Notes:

  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
  PURPOSE.

  Copyright (c) 1996 Microsoft Corporation.  All Rights Reserved.


Revision History:

    4-4-96 : created

****************************************************************************/

#include "winutb.h"
#include "resource.h"
#include <commctrl.h>

/************************************************************
FUNCTION: SelectPipesDlg

PURPOSE:	The purpose of this function is to present the
			user with the choice of which endpoints that they
			want to talk to.

DATE:		4/10/96 - created

ABSTRACT:	The dialog is presented with the combo boxes 
			already containing the pipes that exist, which
			is retrieved from a global structure.
************************************************************/
BOOL FAR PASCAL SelectPipesDlg(HWND hDlg, UINT message,
							UINT wParam, LONG lParam)
{
	UCHAR ucAddress, ucPipeType, ucINPipeType, ucOUTPipeType;
	ULONG ulMaxPacket, ulSiz;
	UINT iIndex, x;
	DWORD dwRet, cbBytesRet;
	BOOLEAN bINnone = TRUE, bOUTnone = TRUE;
    PUSBD_INTERFACE_INFORMATION pusbInterfaceInfo = NULL;
	char *PipeTypes[4]= {"Control", "Isoch", "Bulk", "Interrupt"};
	static HWND hTrackWnd;
    UCHAR ucBuff[1024];
	
	switch (message)
	{
		case WM_INITDIALOG:

				// initialize values
			SetDlgItemInt(hDlg, IDD_INPUT_BUFF, iInputBuffLen, FALSE);
			SetDlgItemInt(hDlg, IDD_OUTPUT_BUFF, iOutputBuffLen, FALSE);
			SetDlgItemInt(hDlg, IDD_ITERATIONS, iIterations, TRUE);
			// setup check box for async reads and writes
			CheckDlgButton(hDlg, IDC_ASYNC_CHECK, bAsyncTest);

			// get handles for pipe stuff in dialog box
			hComboIn = GetDlgItem(hDlg, IDD_INPUT_NUM);
			hComboOut = GetDlgItem(hDlg, IDD_OUTPUT_NUM);

			// setup trackbar
			hTrackWnd = GetDlgItem(hDlg, IDC_SCREEN_IO_FREQ);
			SendMessage(hTrackWnd, TBM_SETRANGE, (WPARAM) 1,
						(LPARAM) MAKELONG(1, 1000));
			SendMessage(hTrackWnd, TBM_SETPOS, (WPARAM) 1,
						(LPARAM) displayFreq);

				// BUGBUG need to fix this for multiple interfaces
            // pusbInterfaceInfo = Ring0Extension.Interface[0];

            ulSiz = sizeof(ucBuff);

            dwRet = DeviceIoControl
                             (hUSBLOOP, 
                             GET_INTERFACE_INFO,
                             ucBuff,   //inbuff
                             ulSiz, 
                             ucBuff,   //outbuff
                             ulSiz, 
                             &cbBytesRet,
                             NULL);


            if(!(dwRet))
                    DisplayOutput("DeviceIoControl() failed with %ld in FOR LOOP", GetLastError());
            else
            {                                                                                                
	            pusbInterfaceInfo = (PUSBD_INTERFACE_INFORMATION) ucBuff;

                for(x = 0; x < pusbInterfaceInfo->NumberOfPipes; x++)
				{							
					ucAddress = pusbInterfaceInfo->Pipes[x].EndpointAddress;
					ulMaxPacket = pusbInterfaceInfo->Pipes[x].MaximumPacketSize;
					
					if(ucAddress & USB_ENDPOINT_DIRECTION_MASK)
					{
						hComboBox = hComboIn;
						bINnone = FALSE;
					}
					else 
					{
						hComboBox = hComboOut;
						bOUTnone = FALSE;
					}

					ucPipeType = pusbInterfaceInfo->Pipes[x].PipeType;
					wsprintf(chBuff, "%ld - %s (max packet size = %ld bytes)", 
								x, PipeTypes[ucPipeType], ulMaxPacket);
																			
					iIndex = SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)chBuff);
					SendMessage(hComboBox,CB_SETITEMDATA, iIndex, x);																												
					SendMessage(hComboBox,CB_SETCURSEL, 0, 0);																												
				
				}

				// this code is so the dialog box can remember the last set of
				// pipes used, does not work yet will revisit
#if 0
				// set dialog box to point to current pipes
				SendMessage(hComboIn, CB_SETCURSEL, (iINpipe == -1) ? 0 : iINpipe, 0);
				SendMessage(hComboOut, CB_SETCURSEL, (iINpipe == -1) ? 0 : iINpipe, 0);
#endif
				
				if(bOUTnone)
				{
					EnableWindow(GetDlgItem(hDlg, IDD_OUTPUT_BUFF), FALSE);
					EnableWindow(hComboOut, FALSE);
					iOUTpipe = -1;
				}
	
				if(bINnone)
				{
					EnableWindow(GetDlgItem(hDlg, IDD_OUTPUT_BUFF), FALSE);
					EnableWindow(hComboOut, FALSE);
					iOUTpipe = -1;
				}

			}
			return TRUE;
		
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				// look for CBN messages, change output pipe entry to match in
				case IDD_INPUT_NUM:
					switch(HIWORD(wParam))
					{
						case CBN_SELCHANGE:
							iIndex = SendMessage(hComboIn, CB_GETCURSEL, 0, 0);
							SendMessage(hComboOut, CB_SETCURSEL, iIndex, 0);
							break;
						default:
							break;
					}
					break;
				// look for CBN messages, change inout pipe to match out
				case IDD_OUTPUT_NUM:
					switch(HIWORD(wParam))
					{
						case CBN_SELCHANGE:
							iIndex = SendMessage(hComboOut, CB_GETCURSEL, 0, 0);
							SendMessage(hComboIn, CB_SETCURSEL, iIndex, 0);
							break;
						default:
							break;
					}
					break;
				case IDOK:
					// get position of trackbar
					displayFreq = SendMessage(hTrackWnd, TBM_GETPOS, 0, 0);

					// get state of async test flag
					bAsyncTest = IsDlgButtonChecked(hDlg, IDC_ASYNC_CHECK);

					iInputBuffLen = GetDlgItemInt(hDlg, IDD_INPUT_BUFF, NULL, FALSE);
					iOutputBuffLen = GetDlgItemInt(hDlg, IDD_OUTPUT_BUFF, NULL, FALSE);
					iIterations = GetDlgItemInt(hDlg, IDD_ITERATIONS, NULL, TRUE);

					iINpipe = SendMessage(hComboIn, 
										CB_GETITEMDATA, 
										SendMessage(hComboIn, CB_GETCURSEL, 0, 0), 
										0);

					iOUTpipe = SendMessage(hComboOut, 
										CB_GETITEMDATA, 
										SendMessage(hComboOut, CB_GETCURSEL, 0, 0), 
										0);
										
						// BUGBUG need to fix this for multiple interfaces
                    // pusbInterfaceInfo = Ring0Extension.Interface[0];

                    ulSiz = sizeof(ucBuff);

                    dwRet = DeviceIoControl
                                     (hUSBLOOP, 
                                     GET_INTERFACE_INFO,
                                     ucBuff,   //inbuff
                                     ulSiz, 
                                     ucBuff,   //outbuff
                                     ulSiz, 
                                     &cbBytesRet,
                                     NULL);


                    if(!(dwRet))
                        DisplayOutput("DeviceIoControl() failed with %ld in FOR LOOP", GetLastError());
                    else
                    {                                                                                                
                        pusbInterfaceInfo = (PUSBD_INTERFACE_INFORMATION) ucBuff;
                        ucINPipeType = pusbInterfaceInfo->Pipes[iINpipe].PipeType;
                        ucOUTPipeType = pusbInterfaceInfo->Pipes[iOUTpipe].PipeType;
					
						if ( ((iInputBuffLen == 0) && (iINpipe != -1))|| 
						  	 ((iOutputBuffLen == 0) && (iOUTpipe != -1)) )
							MessageBox(hDlg, "Buffer value too low. ", szAppName, MB_OK);		
						else if ((ucINPipeType == USB_ENDPOINT_TYPE_ISOCHRONOUS) ||
								 (ucINPipeType == USB_ENDPOINT_TYPE_CONTROL))
						{
							MessageBox(hDlg, "Input pipe cannot be of that type...yet", 
								szAppName, MB_OK); 
						}
						else if ((ucOUTPipeType == USB_ENDPOINT_TYPE_ISOCHRONOUS) ||
								 (ucOUTPipeType == USB_ENDPOINT_TYPE_CONTROL))
						{
							MessageBox(hDlg, "Output pipe cannot be of that type...yet", 
								szAppName, MB_OK); 
						}
						else
							EndDialog(hDlg,1);
					}
					
					break;
				case IDCANCEL:
					iINpipe = iOUTpipe = -1;
					iInputBuffLen = iOutputBuffLen = 0;
					iIterations = 0;
					EndDialog(hDlg,0);
					break;
															
				default:
					break;
			}
			break;

	}
	return FALSE;
}

/************************************************************
FUNCTION: 	DevHandlDlg

PURPOSE:	The purpose of this function is to present the
			user with the choice of which device they want
			to talk to.

DATE:		4/10/96 - created

ABSTRACT:	On WM_INITDIALOG this routine assumes that there
			is at least 1 device on the bus, instance 0. If
			not this dialog will fail and close. However, if
			there is at least 1 instance, then the dialog does
			a DevIoCtrl() call to get the total number of
			devices from the Ring0 global variable. The driver
			then proceeds to open each instance and retrieve
			the descriptor values for the vendor_id and
			product_id. This data is then presented to the 
			user who chooses with device to talk to.
************************************************************/
BOOL FAR PASCAL DevHandlDlg(HWND hDlg, UINT message,
							UINT wParam, LONG lParam)
{
    ULONG x = 1, ulSiz;
	USHORT usDiv1, usDiv2, usInst;
	int iIndex;
	DWORD dwRet, cbBytesRet;
	char pDeviceName[16];
    UCHAR ucBuff[1024];
    PUSB_DEVICE_DESCRIPTOR pusbDeviceDesc;
	
	strcpy(pDeviceName,"\\\\.\\USBLOOPXXX");
	
	switch (message)
	{	
		case WM_INITDIALOG:
			hCombo = GetDlgItem(hDlg, IDD_USB_DEVICE);

				// this is currently a hack implementation, until we merge the
				// driver source code with usbdiag.sys
				
			for(x = 1; x < 999; x++)
			{		

				usDiv2 = (USHORT) (x / 100);
				usDiv1 = (USHORT) ((x - (usDiv2 * 100)) / 10);
				usInst = (USHORT) (x - ((usDiv2 * 100) + (usDiv1 * 10)));

				pDeviceName[11] = (UCHAR) ('0' + usDiv2);
				pDeviceName[12] = (UCHAR) ('0' + usDiv1);
				pDeviceName[13] = (UCHAR) ('0' + usInst);
				
				hUSBLOOP = CreateFile(pDeviceName,
							GENERIC_WRITE | GENERIC_READ,
							FILE_SHARE_WRITE | FILE_SHARE_READ,
							NULL,
							OPEN_EXISTING,
							0,
							NULL);	

					// it is ok to fail on an open

				if(hUSBLOOP != INVALID_HANDLE_VALUE)
				{
                                        ulSiz = sizeof(USB_DEVICE_DESCRIPTOR);

					dwRet = DeviceIoControl
								(hUSBLOOP, 
                                 GET_DEVICE_DESCRIPTOR,
                                ucBuff,   //inbuff
                                ulSiz, 
                                ucBuff,   //outbuff
                                ulSiz, 
								&cbBytesRet,
								NULL);
				
					if(!(dwRet))
						DisplayOutput("DeviceIoControl() failed with %ld in FOR LOOP", GetLastError());
					else
					{													
							// retrieve descriptor data

                        pusbDeviceDesc = (PUSB_DEVICE_DESCRIPTOR) ucBuff;

						wsprintf(chBuff,"USB\\Vendor_%X\\Product_%X (%ld)",
                                                        pusbDeviceDesc->idVendor,
                                                        pusbDeviceDesc->idProduct, x);

							// fill in combo box
														
						iIndex = SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)chBuff);
						SendMessage(hCombo,CB_SETITEMDATA, iIndex, x);																												
						SendMessage(hCombo,CB_SETCURSEL, 0, 0);																												
						CloseHandle(hUSBLOOP);
					}
				}
			}

			SetFocus(hCombo);
			break;
			
			
		case WM_COMMAND:
			switch(wParam)
			{
				case IDOK:

						// determine which instance the user chose
						
					iInstance = SendMessage(hCombo, 
										CB_GETITEMDATA, 
										SendMessage(hCombo, CB_GETCURSEL, 0, 0), 
										0);

					usDiv2 = (USHORT) (iInstance / 100);
					usDiv1 = (USHORT) ((iInstance - (usDiv2 * 100)) / 10);
					usInst = (USHORT) (iInstance - ((usDiv2 * 100) + (usDiv1 * 10)));

					pDeviceName[11] = (UCHAR) ('0' + usDiv2);
					pDeviceName[12] = (UCHAR) ('0' + usDiv1);
					pDeviceName[13] = (UCHAR) ('0' + usInst);

						// open that instance and set global handle variable
										
					hUSBLOOP = CreateFile(pDeviceName,
								GENERIC_WRITE | GENERIC_READ,
								FILE_SHARE_WRITE | FILE_SHARE_READ,
								NULL,
								OPEN_EXISTING,
								0,
								NULL);	
								
					if(hUSBLOOP == INVALID_HANDLE_VALUE)
						DisplayOutput("Failed to open %s in IDOK", pDeviceName); 				
					else
					{
						DisplayOutput("Opened successfully %s", pDeviceName); 				
						strcpy(szDeviceName, pDeviceName);
					}

					EndDialog(hDlg,1);
					break;
					
				case IDCANCEL:
					EndDialog(hDlg,0);
					hUSBLOOP = INVALID_HANDLE_VALUE;
					break;
					
				default:
					break;
			}
			break;
	}

	return FALSE;
}

/************************************************************
FUNCTION: DisplayUsage
************************************************************/
BOOL FAR PASCAL DisplayUsage(HWND hDlg, UINT message,
							UINT wParam, LONG lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
			return TRUE;
		
		case WM_COMMAND:
			switch(wParam)
			{
				case IDOK:
					EndDialog(hDlg,1);
					break;					
			}
			break;
	}
	return FALSE;
}

/************************************************************
FUNCTION: AboutDlgProc
************************************************************/
BOOL FAR PASCAL AboutDlgProc(HWND hDlg, UINT message,
							UINT wParam, LONG lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
			return TRUE;
		
		case WM_COMMAND:
			switch(wParam)
			{
				case IDOK:
					EndDialog(hDlg,1);
					break;					
				case IDCANCEL:
					EndDialog(hDlg,0);
					break;					
			}
			break;
	}
	return FALSE;
}


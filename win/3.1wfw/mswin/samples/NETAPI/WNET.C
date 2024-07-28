//**************************************************************************
//
//  Filename: WNET.C
//
//  Purpose:
//      
//      This is just a sample program to call some of the
//      Window for Workgroups (aka WFW) API's as described in the WFW SDK.
//
//      This file is used to call some of the Windows Network functions (WNET).
//
//  Other C Files:
//
//      a) output.c     - to output messages to the test window
//      b) net.c        - to call some of the network (NET) functions
//      c) multinet.c   - to call some of the multiple network (MNET) functions
//      d) wfwapi.c     - main windows file
//      e) utility.c    - general functions
//
//
//                                                                             
//  Copyright (c) 1992-1993, Microsoft Corp.  All rights reserved.       
//                                                                             
//**************************************************************************




//**************************************************************************
// Header Files
//**************************************************************************

#include <WINDOWS.H>        // Windows Header

#include "WFWAPI.H"         // Sample App Header

#include <WFWNET.H>         // Windows for Workgroups Header

#include <WINNET.H>         // Windows Network (WNET) Header


#include "global.h"         // Global Variables




//*************************************************************************************
//*************************************************************************************
//*************************************************************************************
//
// Function:  NetBrowseDialog
//
//     Parameters:      none
//
//     Purpose:
//          Used to bring up the WNetBrowseDialog that enables the user to 
//          select a shared resource
//
//*************************************************************************************
//*************************************************************************************
//*************************************************************************************


void NetBrowseDialog (void)

{
	
	// *************************************************************************
	//  local variables
	// *************************************************************************
	
	char szPathBuffer[132];                // path buffer
	
	WORD wBrowseDialogRet = 0;             // dialog return 

	
	
	char *szType[] = {"WNBD_CONN_DISKTREE","WNBD_CONN_PRINTQ"};    // valid types

	char szFunctionName[] = "WNetBrowseDialog";                    // function name
		  
	
	
	// ************************************************************************
	//  main section of function
	// ************************************************************************
	
	Output(CALLING, 0, szFunctionName, NULL, NULL);

	
	if (DialogBox(hInst,MAKEINTRESOURCE(RESOURCE_TYPE_DIALOG),hWnd1,lpfnDlgProc))
	  {
		  switch (wResourceType)
			 {
				case WNTYPE_DRIVE:                        // set the resource type
					szResourceType = szType[0];
					break;

				case WNTYPE_PRINTER:
					szResourceType = szType[1];
					break;
			 }

		  
		  Output(PARAMETERS, 0, szFunctionName, szResourceType, NULL);

		  
		  wBrowseDialogRet = WNetBrowseDialog(hWnd1,wResourceType,szPathBuffer);
	  
		  
		  Output(RETURN_CODE, wBrowseDialogRet, szFunctionName, szPathBuffer, NULL);
	  
	  }
	else  
	  {
		  Output(CANCELLED,0,szFunctionName,NULL, NULL);
	  }

}





//*************************************************************************************
//*************************************************************************************
//*************************************************************************************
//
// Function:  NetStopShareDialog
//
//     Parameters:      none
//
//     Purpose:
//          Used to bring up the WNetStopShareDialog that enables the user to 
//          remove the given share.
//
//
//
//     Note:  Even though this sample shows that you can use this function to
//            remove both Drive & Printer shares....and it does appear to work,
//            removing Printer Shares with this function is NOT SUPPORTED.
//
//*************************************************************************************
//*************************************************************************************
//*************************************************************************************


void NetStopShareDialog (void)
{   
	
	// *************************************************************************
	//  local variables
	// *************************************************************************
	
	HINSTANCE hinstNetDriver;                    // instance handle for Windows network driver
	
	LPWNETSTOPSHAREDIALOG lpfnStopShareDialog;   // pointer to StopShareDialog

	
	WORD wStopShareRet = 0;                      // dialog return
	
	char szbuffer2[BUFFER_LENGTH];               // output buffer
	
	char szFunctionName[] = "WNetStopShareDialog";     // function name
						
						
	
	// *************************************************************************                  
	//  main section of function
	// *************************************************************************
						
	szPath[0] = STRING_NULL;                                  // init the input path
			

	Output(CALLING, 0, szFunctionName, NULL, NULL);
	
	
	if (DialogBox(hInst,MAKEINTRESOURCE(RESOURCE_TYPE_DIALOG),hWnd1,lpfnDlgProc))
	  {
		  Output(PARAMETERS, 0, szFunctionName, szResourceType, NULL);
		  
		  
		  // ********************************************************************
		  //  get the handle to the network driver.  Then if the handle to network
		  //  driver exists, set the pointer to the StopShareDialog...and then
		  //  bring up the dialog.
		  // ********************************************************************
		  
		  
		  hinstNetDriver = (HINSTANCE)WNetGetCaps(0xFFFF);      
		  
		  if (hinstNetDriver != NULL)
			 {
				  lpfnStopShareDialog = (LPWNETSTOPSHAREDIALOG)GetProcAddress(hinstNetDriver,
												(LPSTR)ORD_WNETSTOPSHAREDIALOG);
	  
	  
				  if (lpfnStopShareDialog != NULL)
					 {
						wStopShareRet = (*lpfnStopShareDialog)(hWnd1,wResourceType,szPath);   // wResourceType = 1
					 }
				  else
					 {
						wsprintf(szbuffer2,"WARNING! lpfnStopShareDialog == NULL! WNetStopShareDialog could not be called!!");
						SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
						
						MessageBox(hWnd1,"lpfnStopShareDialog == NULL! \n\nPlease make sure you are running WFW!!","WARNING!!",MB_ICONSTOP | MB_OK);
					 }  
		 
				  
				  Output(RETURN_CODE, wStopShareRet, szFunctionName, szPath, NULL);
			 }     
		  else
			 {
				  wsprintf(szbuffer2,"WARNING! hinstNetDriver == NULL! WNetStopShareDialog could not be called!!");
				  SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
				  
				  MessageBox(hWnd1,"hinstNetDriver == NULL!! \n\nPlease make sure you are running WFW!!","WARNING!!",MB_ICONSTOP | MB_OK);
			 }
	  }
	else
	  {
		 Output(CANCELLED, 0, "Resource Dialog", "WNetStopShareDialog was not called!!", NULL);
	  }
	
}
	
	
	
	
//*************************************************************************************
//*************************************************************************************
//*************************************************************************************
//
// Function:  NetShareAsDialog
//
//     Parameters:      none
//
//     Purpose:
//          Used to bring up the WNetShareAsDialog that enables the user to 
//          create a shared directory.
//
//
//
//     Note:  Even though this sample shows that you can use this function to
//            share both Drive & Printer shares....and it does not appear to work,
//            for Printers.  This is TRUE.....this function is only for creating
//            shared directories.  This function should only be passed WNTYPE_DRIVE.
//
//*************************************************************************************
//*************************************************************************************
//*************************************************************************************



void NetShareAsDialog (void)
{   
	
	
	// *************************************************************************
	//  local variables
	// *************************************************************************
	
	HINSTANCE hinstNetDriver;                    // instance handle for Windows network driver
	
	LPWNETSHAREASDIALOG lpfnShareAsDialog;       // pointer to ShareAsDialog

	
	
	WORD wShareAsRet = 0;                        // dialog return
	
	char szbuffer2[BUFFER_LENGTH];               // output buffer
	
	char szFunctionName[] = "WNetShareAsDialog"; // function name
	


	// ************************************************************************                  
	//  main section of function
	// ************************************************************************
	
	szPath[0] = STRING_NULL;                                  // init the path
			
	
	Output(CALLING, 0, szFunctionName, NULL, NULL);
	
	
	if (DialogBox(hInst,MAKEINTRESOURCE(SHAREAS_DIALOG),hWnd1,lpfnDlgProc))
	  {
		  
		  Output(PARAMETERS, 0, szFunctionName, szPath, szResourceType);
		  
		  
		  // ********************************************************************
		  //  get the handle to the network driver.  Then if the handle to network
		  //  driver exists, set the pointer to the ShareAsDialog...and then
		  //  bring up the dialog.
		  // ********************************************************************
		  
		  hinstNetDriver = (HINSTANCE)WNetGetCaps(0xFFFF);
		  
		  if (hinstNetDriver != NULL)
			 {
				  lpfnShareAsDialog = (LPWNETSHAREASDIALOG)GetProcAddress(hinstNetDriver,
											 (LPSTR)ORD_WNETSHAREASDIALOG);
	  
	  
				  if (lpfnShareAsDialog != NULL)
					 {
						wShareAsRet = (*lpfnShareAsDialog)(hWnd1,wResourceType,szPath);   
					 }
				  else
					 {
						wsprintf(szbuffer2,"WARNING! lpfnShareAsDialog == NULL! WNetShareAsDialog could not be called!!");
						SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
						
						MessageBox(hWnd1,"lpfnShareAsDialog == NULL! \n\nPlease make sure you are running WFW!!","WARNING!!",MB_ICONSTOP | MB_OK);
					 }  
		 
				  Output(RETURN_CODE, wShareAsRet, szFunctionName, szPath, NULL);
				
				
				  
				  if (wResourceType == WNTYPE_PRINTER)         // extra message
					 {
						 wsprintf(szbuffer2,"Oh..by the way..This API doesn't support anything other than WNTYPE_DRIVE");
						 SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
					 }

			 }     
		  else
			 {
				  wsprintf(szbuffer2,"WARNING! hinstNetDriver == NULL! WNetStopShareDialog could not be called!!");
				  SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
				  
				  MessageBox(hWnd1,"hinstNetDriver == NULL!! \n\nPlease make sure you are running WFW!!","WARNING!!",MB_ICONSTOP | MB_OK);
			 }
	  }
	else
	  {
		 Output(CANCELLED, 0, "ShareAS Dialog", "WNetShareAsDialog was not called!!", NULL);
	  }
	
}   
	




//*************************************************************************************
//*************************************************************************************
//*************************************************************************************
//
// Function:  NetServerBrowseDialog
//
//     Parameters:      none
//
//     Purpose:
//          Used to bring up the WNetServerBrowseDialog that enables the user to 
//          create a shared directory.
//
//
//
//*************************************************************************************
//*************************************************************************************
//*************************************************************************************



void NetServerBrowseDialog (void)
{   
	
	
	// *************************************************************************
	//  local variables
	// *************************************************************************
	
	HINSTANCE hinstNetDriver;                          // Windows network driver
	LPWNETSERVERBROWSEDIALOG lpfnServerBrowseDialog;   // pointer to ServerBrowse dialog

	WORD wBrowseDialogRet = 0;                         // dialog return
	
	char szServer[32];                                 // server name chosen
	char szbuffer2[BUFFER_LENGTH];                     // output buffer
	
	char szFunctionName[] = "WNetServerBrowseDialog";  // function name


	
	// ************************************************************************
	//  main section of function
	// ************************************************************************
	
	szServer[0] = STRING_NULL;                         // init
	szPath[0] = STRING_NULL;                           // init
	
	Output(CALLING, 0, szFunctionName, NULL, NULL);
	
	
	
	// ********************************************************************
	//  get the handle to the network driver.  Then if the handle to network
	//  driver exists, set the pointer to the ServerBrowseDialog...and then
	//  bring up the dialog.
	// ********************************************************************
	
	hinstNetDriver = (HINSTANCE)WNetGetCaps(0xFFFF);

	if (hinstNetDriver != NULL)
	  {
		 lpfnServerBrowseDialog = (LPWNETSERVERBROWSEDIALOG)GetProcAddress(hinstNetDriver,
										  (LPSTR)ORD_WNETSERVERBROWSEDIALOG);
	  
		 if (lpfnServerBrowseDialog != NULL)
			
			wBrowseDialogRet = (*lpfnServerBrowseDialog)(hWnd1,"MRU_Files",szServer,sizeof(szServer),0L);
		 
		 else
			{ 
				wsprintf(szbuffer2,"WARNING! lpfnServerBrowseDialog == NULL! WNetServerBrowseDialog could not be called!!");
				SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
				
				MessageBox(hWnd1,"lpfnServerBrowseDialog == NULL! \n\nPlease make sure you are running WFW!!","WARNING!!",MB_ICONSTOP | MB_OK);

			}
			
	  
		 Output(RETURN_CODE, wBrowseDialogRet, szFunctionName, szServer, NULL);
	  
	  }
	else
	  {
		 wsprintf(szbuffer2,"WARNING! hinstNetDriver == NULL! WNetServerBrowseDialog could not be called!!");
		 SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
		 
		 MessageBox(hWnd1,"hinstNetDriver == NULL!! Please make sure you are running WFW!","WARNING!!",MB_ICONSTOP | MB_OK);
	  }

}




//*************************************************************************************
//*************************************************************************************
//*************************************************************************************
//
// Function:  NetConnectDialog
//
//     Parameters:      none
//
//     Purpose:
//          Used to bring up the WNetConnectDialog that enables the user to 
//          connect to other network  resources
//
//
//
//*************************************************************************************
//*************************************************************************************
//*************************************************************************************


void NetConnectDialog (void)

{
	// *************************************************************************
	//  local variables
	// *************************************************************************
	
	LPWORD lpwLastDriveIndex;                    // pointer
	LPWORD lpwLastPrinterIndex;                  // pointer
	
	WORD   wDriveIndex;                          // for drive index
	WORD   wPrinterIndex;                        // for printer index

	
	WORD   wConnectDialogRet = 0;                // dialog return
	
	char szFunctionName[] = "WNetConnectDialog"; // function name

	
	HINSTANCE hinstNetDriver;                    // instance handle for Windows network driver
	
	LPWNETSETDEFAULTDRIVE   lpfnSetDefaultDrive;       // pointer to this function    
	LPWNETGETLASTCONNECTION lpfnGetLastConnection;     // pointer to this function


	
	
	// ************************************************************************
	// main section of function
	// ************************************************************************
	
	lpwLastDriveIndex   = &wDriveIndex;
	lpwLastPrinterIndex = &wPrinterIndex;


	Output(CALLING, 0, szFunctionName, NULL, NULL);
		  
	
	
	// ***************************************************************************
	//  get the handle to the network driver.  Then if the handle to network
	//  driver exists, set the pointer to the SetDefaultDrive & GetLastConnection
	//  functions.  We want to keep track of the drive connection drive index.
	//  Then bring up the connection dialog.
	// ***************************************************************************
	
	
	hinstNetDriver = (HINSTANCE)WNetGetCaps(0xFFFF);

	lpfnSetDefaultDrive = (LPWNETSETDEFAULTDRIVE) GetProcAddress(hinstNetDriver,
								 (LPSTR)MAKEINTRESOURCE(ORD_WNETSETDEFAULTDRIVE));

	
	lpfnGetLastConnection = (LPWNETGETLASTCONNECTION) GetProcAddress(hinstNetDriver,
									(LPSTR)MAKEINTRESOURCE(ORD_WNETGETLASTCONNECTION));

	
	

	
	if (DialogBox(hInst,MAKEINTRESOURCE(RESOURCE_TYPE_DIALOG),hWnd1,lpfnDlgProc))
	  {
		  
		  Output(PARAMETERS, 0, szFunctionName, szResourceType, NULL);

		  
		  wConnectDialogRet = WNetConnectDialog(hWnd1,wResourceType);
	  
		  
		  Output(RETURN_CODE, wConnectDialogRet, szFunctionName, NULL, NULL);
	  
		  
		  if (wConnectDialogRet == WN_SUCCESS) 
			 {
				 switch (wResourceType)
					{
						case WNTYPE_DRIVE:
							
							(*lpfnGetLastConnection)(wResourceType, lpwLastDriveIndex);
							(*lpfnSetDefaultDrive)(wDriveIndex);
							 
							TranslateIndex(wResourceType, wDriveIndex);       // function in utility.c

							break;

						
						case WNTYPE_PRINTER:
							
							(*lpfnGetLastConnection)(wResourceType, lpwLastPrinterIndex);
							
							TranslateIndex(wResourceType, wPrinterIndex);
							
							break;
					}
			 }

	  }
	else  
	  {
		  Output(CANCELLED,0,szFunctionName,NULL, NULL);
	  }
	
}





//*************************************************************************************
//*************************************************************************************
//*************************************************************************************
//
// Function:  NetConnectDialog
//
//     Parameters:      none
//
//     Purpose:
//          Used to bring up the WNetConnectionDialog that enables the user to 
//          connect to other network  resources
//
//
//
//*************************************************************************************
//*************************************************************************************
//*************************************************************************************



void NetConnectionDialog (void)

{
	// *************************************************************************
	//  local variables
	// *************************************************************************
	
	LPWORD lpwLastDriveIndex;                    // pointer
	LPWORD lpwLastPrinterIndex;                  // pointer
	
	WORD   wDriveIndex;                          // for drive index
	WORD   wPrinterIndex;                        // for printer index
	
	WORD wConnectionDialogRet = 0;               // dialog return
	
	char szFunctionName[] = "WNetConnectionDialog";    // function name
	
	HINSTANCE hinstNetDriver;                          // instance handle to Windows Network Driver
	
	LPWNETSETDEFAULTDRIVE lpfnSetDefaultDrive;         // pointer to this function
	LPWNETGETLASTCONNECTION lpfnGetLastConnection;     // pointer to this function


	
	// ************************************************************************
	// main portion of the routine
	// ************************************************************************
	
	lpwLastDriveIndex   = &wDriveIndex;
	lpwLastPrinterIndex = &wPrinterIndex;

	
	Output(CALLING, 0, szFunctionName, NULL, NULL);
	
	
	// ***************************************************************************
	//  get the handle to the network driver.  Then if the handle to network
	//  driver exists, set the pointer to the SetDefaultDrive & GetLastConnection
	//  functions.  We want to keep track of the drive connection drive index.
	//  Then bring up the connection dialog.
	// ***************************************************************************
	
	
	hinstNetDriver = (HINSTANCE)WNetGetCaps(0xFFFF);

	lpfnSetDefaultDrive = (LPWNETSETDEFAULTDRIVE) GetProcAddress(hinstNetDriver,
								 (LPSTR)MAKEINTRESOURCE(ORD_WNETSETDEFAULTDRIVE));

	
	lpfnGetLastConnection = (LPWNETGETLASTCONNECTION) GetProcAddress(hinstNetDriver,
									(LPSTR)MAKEINTRESOURCE(ORD_WNETGETLASTCONNECTION));
					 
	
	
	if (DialogBox(hInst,MAKEINTRESOURCE(RESOURCE_TYPE_DIALOG),hWnd1,lpfnDlgProc))
	  {
		  
		  Output(PARAMETERS, 0, szFunctionName, szResourceType, NULL);

		  
		  wConnectionDialogRet = WNetConnectionDialog(hWnd1,wResourceType);
	  
		  
		  Output(RETURN_CODE, wConnectionDialogRet, szFunctionName, NULL, NULL);
	  
		  
		  if (wConnectionDialogRet == WN_SUCCESS) 
			 {
				 switch (wResourceType)
					{
						case WNTYPE_DRIVE:
							
							(*lpfnGetLastConnection)(wResourceType, lpwLastDriveIndex);
							(*lpfnSetDefaultDrive)(wDriveIndex);
							 
							TranslateIndex(wResourceType, wDriveIndex);       // function in utility.c

							break;

						
						case WNTYPE_PRINTER:
							
							(*lpfnGetLastConnection)(wResourceType, lpwLastPrinterIndex);
							
							TranslateIndex(wResourceType, wPrinterIndex);
							
							break;
					}
			 }
	  }
	else  
	  {
		  Output(CANCELLED,0,szFunctionName,NULL, NULL);
	  }

}




//*************************************************************************************
//*************************************************************************************
//*************************************************************************************
//
// Function:  NetDisconnectDialog
//
//     Parameters:      none
//
//     Purpose:
//          Used to bring up the WNetDisconnectDialog that enables the user to 
//          disconnect from other network resources.
//
//
//
//*************************************************************************************
//*************************************************************************************
//*************************************************************************************



void NetDisconnectDialog (void)

{
	
	// *************************************************************************
	//  local variables
	// *************************************************************************
	
	WORD wDisconnectDialogRet = 0;                     // dialog return                   
	
	char szFunctionName[] = "WNetDisconnectDialog";    // function name


	
	// ************************************************************************
	//  main portion of the routine
	// ************************************************************************
	
	Output(CALLING, 0, szFunctionName, NULL, NULL);

	
	
	if (DialogBox(hInst,MAKEINTRESOURCE(RESOURCE_TYPE_DIALOG),hWnd1,lpfnDlgProc))
	  {
		  
		  Output(PARAMETERS, 0, szFunctionName, szResourceType, NULL);

		  
		  wDisconnectDialogRet = WNetDisconnectDialog(hWnd1,wResourceType);
	  
		  
		  Output(RETURN_CODE, wDisconnectDialogRet, szFunctionName, NULL, NULL);
	  
	  }
	else  
	  {
		  Output(CANCELLED,0,szFunctionName,NULL, NULL);
	  }
	
}




//*************************************************************************************
//*************************************************************************************
//*************************************************************************************
//
// Function:  NetDisconnectDialog
//
//     Parameters:      none
//
//     Purpose:
//          Used to call the function WNetGetLastConnection. This function returns an
//          index identifying the most recent device to be connected to a network
//          resource.
//
//
//*************************************************************************************
//*************************************************************************************
//*************************************************************************************




void GetLastConnection (void)

{
	
	// *************************************************************************
	//  local variables
	// *************************************************************************
	
	WORD   wDriveIndex;                          // for drive index
	WORD   wPrinterIndex;                        // for printer index

	LPWORD lpwLastDriveIndex;                    // for drive index
	LPWORD lpwLastPrinterIndex;                  // for printer index
	
	
	char szFunctionName[] = "WNetGetLastConnection";  // function name
	
	HINSTANCE hinstNetDriver;                         // instance handle to windows network driver
	
	LPWNETGETLASTCONNECTION lpfnGetLastConnection;    // pointer to this function
	

	
	// ************************************************************************
	// main portion of the routine
	// ************************************************************************
	
	lpwLastDriveIndex   = &wDriveIndex;         // initialize the pointers
	lpwLastPrinterIndex = &wPrinterIndex;


	hinstNetDriver = (HINSTANCE)WNetGetCaps(0xFFFF);

	
	lpfnGetLastConnection = (LPWNETGETLASTCONNECTION) GetProcAddress(hinstNetDriver,
									(LPSTR)MAKEINTRESOURCE(ORD_WNETGETLASTCONNECTION));

	

	Output(CALLING, 0, szFunctionName, NULL, NULL);

	
	(*lpfnGetLastConnection)((WORD)WNTYPE_PRINTER, lpwLastPrinterIndex);    // printer
	
	(*lpfnGetLastConnection)((WORD)WNTYPE_DRIVE, lpwLastDriveIndex);        // drive
	
	
	
	TranslateIndex(WNTYPE_PRINTER, wPrinterIndex);              // function in utility.c
	
	TranslateIndex(WNTYPE_DRIVE, wDriveIndex);                  
	
			
}




//*************************************************************************************
//*************************************************************************************
//*************************************************************************************
//
// Function:  GetShareCount
//
//     Parameters:      none
//
//     Purpose:
//          Used to call the function WNetGetShareCount. This function returns the
//          count of the number of shared resources of the given type.
//
//
//*************************************************************************************
//*************************************************************************************
//*************************************************************************************



void GetShareCount (void)

{
	
	// *************************************************************************
	//  local variables
	// *************************************************************************
	
	WORD wGetShareCountRet = 99;                 // function return
	
	
	char szFunctionName[] = "WNetGetShareCount"; // function name
	char szbuffer2[BUFFER_LENGTH];               // output buffer
	
	
	HINSTANCE hinstNetDriver;                    // instance handle to Windows Network Driver
	
	LPWNETGETSHARECOUNT lpfnGetShareCount;       // pointer to function
	


	// ************************************************************************
	// main portion of the routine
	// ************************************************************************

	
	hinstNetDriver = (HINSTANCE)WNetGetCaps(0xFFFF);

	
	lpfnGetShareCount = (LPWNETGETSHARECOUNT) GetProcAddress(hinstNetDriver,
							  (LPSTR)MAKEINTRESOURCE(ORD_WNETGETSHARECOUNT));

	

	Output(CALLING, 0, szFunctionName, NULL, NULL);

	
	
	if (DialogBox(hInst,MAKEINTRESOURCE(RESOURCE_TYPE_DIALOG),hWnd1,lpfnDlgProc))
	  {
		  
		  Output(PARAMETERS, 0, szFunctionName, szResourceType, NULL);

		  wGetShareCountRet = (*lpfnGetShareCount)(wResourceType);
	  
		  switch (wResourceType)
			 {
				 case WNTYPE_PRINTER:
				 case WNTYPE_DRIVE:
					wsprintf(szbuffer2,"Number of Shared resources = %d", wGetShareCountRet);
					SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
					break; 

				 default:
					Output(RETURN_CODE, wGetShareCountRet, szFunctionName, NULL, NULL);
			 }

	  }
	else  
	  {
		  Output(CANCELLED,0,szFunctionName,NULL, NULL);
	  }
	
}
	


//*************************************************************************************
//*************************************************************************************
//*************************************************************************************
//
// Function:  GetShareName
//
//     Parameters:      none
//
//     Purpose:
//          Used to call the function WNetGetShareName. This function returns the
//          network name of the shared resource associated with the given directory
//          or device.
//
//          This function currently only works for shared directories.  The full path
//          of the directory should be in CAPITAL LETTERs.  The input dialog box
//          does not force all input into CAPS.  You can change the dialog box 
//          behavior...but we want to test all cases.
//
//*************************************************************************************
//*************************************************************************************
//*************************************************************************************


void GetShareName (void)

{
	// *************************************************************************
	//  local variables
	// *************************************************************************
	
	WORD wGetShareNameRet = 0;                   // function return
	
	
	char szFunctionName[] = "WNetGetShareName";  // function name
	
	char szbuffer2[BUFFER_LENGTH];               // buffer
	char szbuffer3[BUFFER_LENGTH];               // buffer

	LPSTR lpszPath;                              // pointer to the path
	LPSTR lpszNetName;                           // pointer to network name
	
	
	
	HINSTANCE hinstNetDriver;                    // instance handle to Win Network Driver
	
	LPWNETGETSHARENAME lpfnGetShareName;         // pointer to function
	
	
	
	// ************************************************************************
	// main portion of the routine
	// ************************************************************************
	
	szbuffer2[0]      = STRING_NULL;             // initialize 
	szPath[0]         = STRING_NULL;
	szPrinterPort[0]  = STRING_NULL;
	lpszPath          = szPath;
	lpszNetName       = szbuffer2;

	
	
	hinstNetDriver = (HINSTANCE)WNetGetCaps(0xFFFF);

	
	lpfnGetShareName = (LPWNETGETSHARENAME) GetProcAddress(hinstNetDriver,
							 (LPSTR)MAKEINTRESOURCE(ORD_WNETGETSHARENAME));

	
	Output(CALLING, 0, szFunctionName, NULL, NULL);

	
	if (DialogBox(hInst,MAKEINTRESOURCE(SELECT_RESOURCE_TYPE_DIALOG),hWnd1,lpfnDlgProc))
	  {
		  switch (wResourceType)
			 {
				
				case WNTYPE_PRINTER:
					
					if (DialogBox(hInst,MAKEINTRESOURCE(PRINTER_NAME_DIALOG),hWnd1,lpfnDlgProc2))
					  {
							Output(PARAMETERS, 0, szFunctionName, szPath, szPrinterPort);
						 
					  }
					else
					  {
							Output(CANCELLED,0,szFunctionName,NULL, NULL);
							wGetShareNameRet = QUIT;
					  }

					break;
				
				
				default:

					if (DialogBox(hInst,MAKEINTRESOURCE(GETSHARENAME_DIALOG),hWnd1,lpfnDlgProc))
					  {
							Output(PARAMETERS, 0, szFunctionName, szPath, NULL);
					  }
					else
					  {
							Output(CANCELLED,0,szFunctionName,NULL, NULL);
							wGetShareNameRet = QUIT;
					  }

					break;

			 }

		  
		  if (wGetShareNameRet != QUIT)
			 {
				wGetShareNameRet = (*lpfnGetShareName)(lpszPath, lpszNetName, (WORD) BUFFER_LENGTH);
		  
				Output(RETURN_CODE, wGetShareNameRet, szFunctionName, NULL, NULL);
		  
		  
				if ((szbuffer2[0]) == STRING_NULL)           // anything returned?
				  {
						wsprintf(szbuffer3,"Path or device is not a shared resource!!");
						SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer3);
				  }
				else
				  {
						wsprintf(szbuffer3,"The returned Share Name = %s", lpszNetName);
						SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer3);
				  }
			 
			 
				
				if (wResourceType == WNTYPE_PRINTER)         // extra message
				  {
						wsprintf(szbuffer3,"Oh..by the way..This API doesn't support anything other than WNTYPE_DRIVE");
						SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer3);
				  }
			 
			 
			 }

	  }
	else  
	  {
		  Output(CANCELLED,0,szFunctionName,NULL, NULL);
	  }

}
	




//*************************************************************************************
//*************************************************************************************
//*************************************************************************************
//
// Function:  GetSharePath
//
//     Parameters:      none
//
//     Purpose:
//          Used to call the function WNetGetSharePath. This function returns the
//          full path or the device name associated with the given resource.
//
//          This function should work for both Drive & Printer shares.  If this
//          particular function does not appear to work, try all CAPITAL LETTERs
//          when specifying the Network Name.  The input dialog box does not require
//          nor convert the input string to CAPs.
//
//*************************************************************************************
//*************************************************************************************
//*************************************************************************************


void GetSharePath (void)

{
	// *************************************************************************
	//  local variables
	// *************************************************************************
	
	WORD wGetSharePathRet = 0;                   // function return
	
	char szFunctionName[] = "WNetGetSharePath";  // function name
	
	char szbuffer2[BUFFER_LENGTH];               // buffer
	char szbuffer3[BUFFER_LENGTH];               // buffer

	LPSTR lpszPath;                              // pointer to the path
	LPSTR lpszNetName;                           // pointer to the Network Name
	
	
	
	HINSTANCE hinstNetDriver;                    // instance handle to Win Network Driver
	
	LPWNETGETSHAREPATH lpfnGetSharePath;         // pointer to function
	
	
	// ************************************************************************
	// main portion of the routine
	// ************************************************************************
	
	szbuffer2[0]  = STRING_NULL;                   // initialize
	szPath[0]     = STRING_NULL;
	lpszPath      = szbuffer2;
	lpszNetName   = szPath;

	
	hinstNetDriver = (HINSTANCE)WNetGetCaps(0xFFFF);

	
	lpfnGetSharePath = (LPWNETGETSHAREPATH) GetProcAddress(hinstNetDriver,
							 (LPSTR)MAKEINTRESOURCE(ORD_WNETGETSHAREPATH));

	
	Output(CALLING, 0, szFunctionName, NULL, NULL);

	
	if (DialogBox(hInst,MAKEINTRESOURCE(GETSHAREPATH_DIALOG),hWnd1,lpfnDlgProc))
	  {
		  Output(PARAMETERS, 0, szFunctionName, szPath, NULL);
		  
		  wGetSharePathRet = (*lpfnGetSharePath)(lpszNetName, lpszPath, (WORD) BUFFER_LENGTH);
	  
		  Output(RETURN_CODE, wGetSharePathRet, szFunctionName, NULL, NULL);
		  
		  
		  if (wGetSharePathRet == WN_SUCCESS)
			  {
					wsprintf(szbuffer3,"The returned Share Path = %s", lpszPath);
					SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer3);
			  }
		  else
			  {
					wsprintf(szbuffer3,"The Network Name is not a valid resource name!!");
					SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer3);
			  }

	  }
	else  
	  {
		  Output(CANCELLED,0,szFunctionName,NULL, NULL);
	  }

}


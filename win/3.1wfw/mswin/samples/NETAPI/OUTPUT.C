//**************************************************************************
//
//  Filename: OUTPUT.C
//
//  Purpose:
//      
//      This is just a sample program to call some of the
//      Window for Workgroups (aka WFW) API's as described in the WFW SDK.
//
//      This file is used to output information out to the ListBox
//
//  Other C Files:
//
//      a) wfwapi.c     - main windows file
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


#include "global.h"         // Global Variables


//*************************************************************************************
//*************************************************************************************
//*************************************************************************************
//
// Function:  NetBrowseDialog
//
//     Parameters:      
//       WORD function   - valid values are:  CANCELLED, CALLING, PARAMETERS & ERRROR
//       WORD error_code - should be the return value of the function call
//       char *fname     - char string #1 to print out - should be the name of the function
//       char *str       - char strine #2 to print out - extra stuff such as the path
//
//
//     Purpose:
//          Called to display various types of messages to the ListBox
//
//*************************************************************************************
//*************************************************************************************
//*************************************************************************************


void Output(WORD function, WORD error_code, char *fname, char *str1, char *str2)



{
	
	// *************************************************************************
	//  local variables
	// *************************************************************************
	
	
	char szbuffer2[BUFFER_LENGTH];   // output buffer
	
	
	// *************************************************************************
	//  main section of the function
	// *************************************************************************
	

	
	
	switch (function)
	  {
			case CANCELLED:
				
				wsprintf(szbuffer2,"%s was CANCELLED!! %s",(LPSTR)fname, (LPSTR)str1);
				SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
				
				break;
		  
			
			
			case CALLING:
				
				wsprintf(szbuffer2,"%s is being called:",(LPSTR)fname);
				SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
				
				break;

			
			
			case PARAMETERS:
				
				switch (wFunction)
				  {
					  case IDM_SHAREAS_DIALOG:
							
							wsprintf(szbuffer2,"%s parameters: lpszPath=%s  AND  iType=%s",(LPSTR)fname,(LPSTR)str1,(LPSTR)str2);
							SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
							
							break;

					  case IDM_GETSHARENAME:
							
							wsprintf(szbuffer2,"%s parameters: lpszPath=%s  AND  Port=%s",(LPSTR)fname,(LPSTR)str1,(LPSTR)str2);
							SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
							
							break;
					  
					  
					  case IDM_GETSHAREPATH:
							
							wsprintf(szbuffer2,"%s parameters: lpszNetName=%s",(LPSTR)fname,(LPSTR)str1);
							SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
							
							break;
					  
					  
					  default:
							
							wsprintf(szbuffer2,"%s parameters: iType=%s",(LPSTR)fname,(LPSTR)str1);
							SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
							
							break;
				  }


				break;


			case RETURN_CODE:
	
				switch (error_code)
					{
						case WN_BAD_POINTER:
							
							wsprintf(szbuffer2,"%s returned WN_BAD_POINTER!",(LPSTR)fname);
							SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
							
							break;
						
						
						case WN_BAD_NETNAME:
							
							wsprintf(szbuffer2,"%s returned WN_BAD_NETNAME!",(LPSTR)fname);
							SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
							
							break;
						
						
						case WN_BAD_VALUE:
							
							wsprintf(szbuffer2,"%s returned WN_BAD_VALUE!",(LPSTR)fname);
							SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
							
							break;
						
						
						case WN_CANCEL:
							
							wsprintf(szbuffer2,"%s returned WN_CANCEL!",(LPSTR)fname);
							SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
							
							break;
						

						case WN_NET_ERROR:
							
							wsprintf(szbuffer2,"%s returned WN_NET_ERROR!",(LPSTR)fname);
							SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
							
							break;


						case WN_NOT_SUPPORTED:
							
							wsprintf(szbuffer2,"%s returned WN_NOT_SUPPORTED!",(LPSTR)fname);
							SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
							
							break;
						
						
						case WN_OUT_OF_MEMORY:
							
							wsprintf(szbuffer2,"%s returned WN_OUT_OF_MEMORY!",(LPSTR)fname);
							SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
							
							break;

	
						case WN_SUCCESS:
							
							wsprintf(szbuffer2,"%s returned WN_SUCCESS!",(LPSTR)fname);
							SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
							
							switch (wFunction)
							  {
									case IDM_SERVER_BROWSE_DIALOG:           
										wsprintf(szbuffer2,"BROWSE DIALOG:  \\\\Server selected = %s",(LPSTR)str1);
										SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
										
										break;
									
									
									case IDM_BROWSE_DIALOG:
										wsprintf(szbuffer2,"BROWSE DIALOG:  \\\\Server\\Share selected = %s",(LPSTR)str1);
										SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
										
										break;
							  }


							break;
	
						case WN_WINDOWS_ERROR:
							wsprintf(szbuffer2,"%s returned WN_WINDOWS_ERROR!",(LPSTR)fname);
							SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
							
							break;
					 
					 }
				
				break;

	  }


}



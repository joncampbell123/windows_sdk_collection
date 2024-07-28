//**************************************************************************
//
//  Filename: NET.C
//
//  Purpose:
//      
//       This is just a part of a sample program to call some of the
//       Window for Workgroups (aka WFW) API's as described in the WFW SDK.
//
//       This section calls some of the network (NET) functions.
//
//
//  Other C Files:
//
//      a) output.c     - to output messages to the test window
//      b) wfwapi.c     - main windows file
//      c) multinet.c   - to call some of the multiple network (MNET) functions
//      d) wnet.c       - to call some of the Windows Network (WNET) functions
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

#include <WINDOWS.H>       // Windows Header

#include "WFWAPI.H"        // Sample App Header


#define INCL_NETWKSTA
#define INCL_NETERRORS
#define INCL_NETSERVER


#include <WFWNET.H>        // Windows for Workgroups Header
#include <LAN.H>           // Windows for Workgroups Network (NET) Header
#include <WINNET.H>        // Windows Network (WNET) Header


#include "global.h"        // Global Variables


//*************************************************************************************
//*************************************************************************************
//*************************************************************************************
//
// Function: WkstaGetInfo
//
//     Parameters:      none
//
//     Purpose:
//          Example of how to obtain information about a computer
//
//
//          Please refer to WFW SDK docs - Chapter 1 titled "Windows for Workgroups
//          Basics", section 1.4 ("Gettiing Information about a Computer").
//
//
//
//*************************************************************************************
//*************************************************************************************
//*************************************************************************************

void WkstaGetInfo (void)

{
   
   // ************************************************************************
   //  Local Variables
   // ************************************************************************
   

   HANDLE hwki0;                             // handle
   struct wksta_info_0 *pwki0;               // pointer to wksta_info_0 structure
   
   int err, cb;
   
   WORD wBrowseRet;
   
   char szbuffer2[BUFFER_LENGTH];            // output buffer
   char szServer[BUFFER_LENGTH];             // server buffer
   
   HINSTANCE hinstNetDriver;                 // instance handle for Windows network driver
   LPWNETSERVERBROWSEDIALOG lpfnDialogAPI;   // pointer to ServerBrowseDialog

                  
   
   // ************************************************************************
   //  main portion of the routine
   // ************************************************************************
   
   szServer[0] = STRING_NULL;                            // init the server buffer

   hinstNetDriver = (HINSTANCE)WNetGetCaps(0xFFFF);      // get the handle to network driver

   if (hinstNetDriver != NULL)
     {
        
        
        // ****************************************************************************      
        // if the handle to network driver exists, then set the pointer to the
        // ServerBrowseDialog.  We will use the ServerBrowseDialog to allow the user to
        // select a computer on the workgroup.
        // ****************************************************************************      
        
        lpfnDialogAPI = (LPWNETSERVERBROWSEDIALOG)GetProcAddress(hinstNetDriver,
                        (LPSTR)ORD_WNETSERVERBROWSEDIALOG);
                    
        if (lpfnDialogAPI == NULL)
          { 
            wsprintf(szbuffer2,"WARNING! lpfnDialogAPI == NULL! WNetServerBrowseDialog could not be called!!");
            SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
        
                          
            MessageBox(hWnd1,"lpfnDialogAPI == NULL! \n\nPlease ensure WFW is running!!","WARNING!!",MB_ICONSTOP | MB_OK);
            return;
          }  
        else
          {
            wFunction = IDM_SERVER_BROWSE_DIALOG;

            Output(CALLING, 0, "NetWkstaGetInfo", NULL, NULL);
            
            wBrowseRet = (*lpfnDialogAPI)(hWnd1,"MRU_Files",szServer,sizeof(szServer),0L);
          }
     }     
          
   
   
   // *****************************************************************************
   //  if the user did not cancel the BrowseServerDialog, then call NetWkstaGetInfo
   //  to get configuration information about a computer on the network
   // ************************************************************************
   
   
   if (wBrowseRet == WN_CANCEL)
     {
       Output(RETURN_CODE, wBrowseRet, "WNetServerBrowseDialog", szServer, NULL);
     }
   else  
     {
       Output(RETURN_CODE, wBrowseRet, "WNetServerBrowseDialog", szServer, NULL);
       
       err = NetWkstaGetInfo(NULL, 0, NULL, 0, &cb);
   
       if (err != NERR_BufTooSmall)
         { 
            wsprintf(szbuffer2,"Error - szbuffer size to small!!");
            SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
         }
       else
         {
            
            // ****************************************************************************
            // Allocate the necessary memory & then call the function
            // ****************************************************************************

            hwki0 = LocalAlloc(LHND, cb);                        
            pwki0 = (struct wksta_info_0 *)LocalLock(hwki0);
              
            err = NetWkstaGetInfo(szServer, 0, (char far *)pwki0, cb, &cb);

            if (err != NERR_Success)
               {
                  wsprintf(szbuffer2,"Could not get info on workstation %s",(LPSTR)szServer);
                  SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
               }
            else
               {
                  
                  // **********************************************************************
                  // function was successful....so let's output some of the information
                  // **********************************************************************
                  
                  wsprintf(szbuffer2,"NetWkstaGetInfo returned NERR_Success!!");
                  SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
                  
                  wsprintf(szbuffer2,"Path to Net Dir = %s",(LPSTR)pwki0->wki0_root);
                  SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
                  
                  wsprintf(szbuffer2,"Computer Name = %s",(LPSTR)pwki0->wki0_computername);
                  SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
                  
                  wsprintf(szbuffer2,"Username = %s",(LPSTR)pwki0->wki0_username);
                  SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
                  
                  wsprintf(szbuffer2,"Workgroup = %s",(LPSTR)pwki0->wki0_langroup);
                  SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
                 
                         
               }
        
            LocalUnlock(hwki0);                       // memory operations
            LocalFree(hwki0);

         }

     }
      
}





//*************************************************************************************
//*************************************************************************************
//*************************************************************************************
//
// Function: NetEnum2
//
//     Parameters:      none
//
//     Purpose:
//          Example of how to obtain a list of all the computers in a workgroup
//
//
//          Please refer to WFW SDK docs - Chapter 1 titled "Windows for Workgroups
//          Basics", section 1.6 ("Gettiing Information about Other Computers").
//
//
//
//*************************************************************************************
//*************************************************************************************
//*************************************************************************************


void NetEnum2 (void)

{
   
   HANDLE hsi0;                        // handle 
   struct server_info_0 *psi0;         // pointer to server_info_0 structure
   
   int cEntriesRead;                   // # of computers returned
   int cTotalAvail;                    // total # of computers avail
   int cb, err;
   
   int nCount;                         // counter 

   char szbuffer2[BUFFER_LENGTH];      // output buffer

   

   
   // ************************************************************************
   //  main portion of the routine
   // ************************************************************************
   
   szPath[0]      = STRING_NULL;    // init - use the szPath to hold the workgroup name

   
   wsprintf(szbuffer2,"NetServerEnum2 is being called:");
   SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
   

   
   // **************************************************************************
   //  get the current workgroup from SYSTEM.INI
   //
   // **************************************************************************

   GetPrivateProfileString("network", "workgroup","", (LPSTR) &szPath,
                            sizeof(szPath), "system.ini");
   
   


   // **************************************************************************
   //  use dialog routine to get workgroup name - current workgroup is displayed
   //
   //  NOTE:  This dialog does not convert any of the user's input into UPPER CASE
   //         characters.  Try a valid workgroup with UPPER CASE characters only 
   //         verses the same valid workgroup with mixed case characters.
   //         See what kind of results you get!!
   // **************************************************************************
   
   
   if (DialogBox(hInst,MAKEINTRESOURCE(WORKGROUP_DIALOG),hWnd1,lpfnDlgProc))
     {
   
       nCount = strlen(szPath);

       if (nCount)
         {
            wsprintf(szbuffer2,"NetServerEnum2 called with WORKGROUP = %s", (LPSTR)szPath);
            SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
         }
       else
         {  
            wsprintf(szbuffer2,"NetServerEnum2 called with WORKGROUP = NULL (current workgroup)");
            SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);

            szPath[0] = STRING_NULL;
         }


       
       // ************************************************************************
       //  call NetServerEnum2 to get some preliminary information
       // ************************************************************************
   

       err = NetServerEnum2(NULL, 0, NULL, 0, &cEntriesRead, &cTotalAvail, 
                            SV_TYPE_ALL, szPath);
  

       if (err != ERROR_MORE_DATA)
         { 
            // error code here......or your own error routine
        
            wsprintf(szbuffer2,"NetServerEnum2 call #1 did not return ERROR_MORE_DATA - no info available!!");
            SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
     
         }
       else
         {
        
            // ***************************************************************
            // allocate the memory to store the info
            // ***************************************************************        
        
            cb = cTotalAvail * sizeof(struct server_info_0);
        
            hsi0 = LocalAlloc(LHND, cb);
            psi0 = (struct server_info_0 *)LocalLock(hsi0);
              
            

            // ***************************************************************
            // get the names of the computers in the current workgroup &
            // display them.  Last parameter is NULL -> current workgroup
            // ***************************************************************        
        

            err = NetServerEnum2(NULL, 0, (char far *)psi0, cb, &cEntriesRead, &cTotalAvail,
                                 SV_TYPE_ALL, szPath);

        
            if ((err == ERROR_MORE_DATA) || (err != NERR_Success))
              {
                  // error code here.......or your own error routine
             
                  wsprintf(szbuffer2,"NetServerEnum2 call #2 returned some ERROR!!");
                  SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
          
              }
            else
              {
                  wsprintf(szbuffer2,"NetServerEnum2 returned NERR_Success!!");
                  SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
             
                  wsprintf(szbuffer2,"Here is a list of computers in your Workgroup:");
                  SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
             
             
                  if (cTotalAvail == 0)
                    {
                       wsprintf(szbuffer2,"NetServerEnum2 returned 0 computers in your workgroup!");
                       SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
                    }
                  else
                    {
                       for (nCount=1; nCount <= cTotalAvail; nCount++, psi0++)
                         {
                           wsprintf(szbuffer2,"Computer Name #%d = %s",nCount,(LPSTR)(char *)psi0->sv0_name);
                           SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
                         }
                    }
              }
        
            LocalUnlock(hsi0);                      // memory operations
            LocalFree(hsi0);

         }
     }
   else
     {
        wsprintf(szbuffer2,"NetServerEnum2 was CANCELLED!!");
        SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
     }
}


//**************************************************************************
//
//  Filename: MULTINET.C
//
//  Purpose:
//      
//      This is just a sample program to call some of the
//      Window for Workgroups (aka WFW) API's as described in the WFW SDK.
//
//      This file deals with the multiple network functions (MNET)
//
//  Other C Files:
//
//      a) output.c     - to output messages to the test window
//      b) net.c        - to call some of the network (NET) functions
//      c) wfwapi.c     - main windows file
//      d) wnet.c       - to call some of the Windows Network (WNET) functions
//      e) utility.c    - general functions
//
//                                                                             
//  Copyright (c) 1992-1993, Microsoft Corp.  All rights reserved.       
//                                                                             
//*****************************************************************************/



//**************************************************************************
// Header Files
//**************************************************************************


#include <WINDOWS.H>        // Windows Header

#include "WFWAPI.H"         // Sample App Header

#include <WFWNET.H>         // Windows for Workgroups Header

#include <MULTINET.H>       // Windows for Workgroups Multiple Network Header


#include "global.h"         // Global Variables


//*************************************************************************************
//*************************************************************************************
//*************************************************************************************
//
// Function: Multinet
//
//     Parameters:      none
//
//     Purpose:
//          Example of how to call some of the multiple network functions (MNet).
//
//
//          Please refer to WFW SDK docs - Chapter 1 titled "Windows for Workgroups
//          Basics", section 1.3 ("Choosing a Target Network").
//
//
//
//*************************************************************************************
//*************************************************************************************
//*************************************************************************************


void Multinet (void)

{
    
    HANDLE hNet;                 // handle of network
    WORD wNetInfo;               // receives type of network
    char szButtonText[80];       // buffer for button text
    int nButton;                 // receives size of button text (bytes)
    HINSTANCE hInstance;         // receives handle for network driver
   
    WORD ret;                    // MNet function return value

    char szbuffer2[BUFFER_LENGTH];         // buffer
    
                  
    
    
    // ************************************************************************
    //  main portion of the routine
    // ************************************************************************

    hNet = 0;

    while (MNetNetworkEnum(&hNet) == WN_SUCCESS)
      {

         nButton = 80;

         ret = MNetGetNetInfo(hNet, &wNetInfo, szButtonText, &nButton, &hInstance);
         
         SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR)" ");
         
         wsprintf(szbuffer2,"Handle of network = %d",hNet);
         SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
         
         wsprintf(szbuffer2,"Network Info = %d",wNetInfo);
         SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
         
         wsprintf(szbuffer2,"ButtonText = %s",(LPSTR)szButtonText);
         SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
         
         wsprintf(szbuffer2,"ButtonText Length = %d",nButton);
         SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
         

         if (ret != WN_SUCCESS)
           {
             wsprintf(szbuffer2,"I am in the WN_BAD_VALUE section");
             SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
         
             
             MessageBox(hLBhwnd,"MNetGetNetInfo returned WN_BAD_VALUE","ERROR!",MB_OK);
           }
         
         else if (wNetInfo == MNM_NET_PRIMARY)
           {
             MNetSetNextTarget(hNet);
           }
         
      }

}

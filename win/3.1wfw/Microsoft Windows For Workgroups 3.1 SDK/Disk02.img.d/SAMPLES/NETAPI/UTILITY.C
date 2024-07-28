   //**************************************************************************
//
//  Filename: UTILITY.C
//
//  Purpose:
//      
//       This is just a part of a sample program to call some of the
//       Window for Workgroups (aka WFW) API's as described in the WFW SDK.
//
//       This section contains just some utility functions called from other
//       parts of the application.
//
//
//  Other C Files:
//
//      a) output.c     - to output messages to the test window
//      b) wfwapi.c     - main windows file
//      c) multinet.c   - to call some of the multiple network (MNET) functions
//      d) wnet.c       - to call some of the Windows Network (WNET) functions
//      e) wfwapi.c     - main windows file
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

#include <WFWNET.H>        // Windows for Workgroups Header

#include <WINNET.H>        // Windows Network (WNET) Header

#include "global.h"        // global variables


//*************************************************************************************
//*************************************************************************************
//*************************************************************************************
//
// Function: SetLBSelection
//
//     Parameters:      none
//
//     Purpose:
//          
//          This function is used to reset the highlight in the display listbox
//
//
//*************************************************************************************
//*************************************************************************************
//*************************************************************************************


void SetLBSelection (void) 

{
   // *************************************************************************
   //  local variables
   // *************************************************************************
   
   
   DWORD dwNumberListBoxItems;               // number of items in ListBox
   
   
   
   // *************************************************************************
   //  main section of function
   //
   //    Get the number of items (index value) in the ListBox.
   //    Then reset the highlight to the bottom.
   //
   //    Note: the index returned from LB_GETCOUNT is off by one..so decrement
   //          to get the correct offset for LB_SETCURSEL.
   // *************************************************************************
   
   SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR)" ");             // blank line
   
   
   dwNumberListBoxItems = SendMessage(hLBhwnd,LB_GETCOUNT,0,0L);        // recalc

   dwNumberListBoxItems--;                                              // decrement index

   SendMessage(hLBhwnd,LB_SETCURSEL,LOWORD(dwNumberListBoxItems),0);    // set selection 
   

}     



//*************************************************************************************
//*************************************************************************************
//*************************************************************************************
//
// Function:  GetResourceType
//
//     Parameters:      
//          hDlg  - Handle to the current displayed dialog
//
//     Purpose:
//          
//          This function is used to determine the type of resource selected in
//          resource dialog box.
//
//*************************************************************************************
//*************************************************************************************
//*************************************************************************************


int GetResourceType (HANDLE hDlg)

{
   
   // *************************************************************************
   //  local variables
   // *************************************************************************
   
   
   int i;
   
   DWORD temp;   
   
   int  nTypes[]     = {WNTYPE_DRIVE, WNTYPE_PRINTER, WNTYPE_FILE, WNTYPE_COMM, WNTYPE_INVALID};
   char *szTypes[]   = {"WNTYPE_DRIVE", "WNTYPE_PRINTER", "WNTYPE_FILE", "WNTYPE_COMM", "WNTYPE_INVALID"};
   int  nRButtons[]  = {IDB_DRIVE, IDB_PRINTER, IDB_FILE, IDB_COMM, IDB_INVALID};

   
   
   // *************************************************************************
   //  main section of function
   // *************************************************************************
   

   for (i=0; i < NUMBER_RESOURCES; i++)
     {
        szResourceType = szTypes[i];
        
        temp = (int) SendDlgItemMessage(hDlg,nRButtons[i],BM_GETSTATE,0,0L);
        
        if (temp & BFCHECK)
           return (nTypes[i]);

     }

}     



//*************************************************************************************
//*************************************************************************************
//*************************************************************************************
//
// Function:  GetPrinterPort
//
//     Parameters:      
//          hDlg  - Handle to the current displayed dialog
//
//     Purpose:
//          
//          This function is used to determine the printer port selected. 
//
//*************************************************************************************
//*************************************************************************************
//*************************************************************************************


void GetPrinterPort (HANDLE hDlg)

{
   
   // ************************************************************************
   //  local variables
   // *************************************************************************
   
   int i;
   
   DWORD temp;   
   
   char *szPorts[]   = {"LPT1:", "LPT2:", "LPT3:"};
   int  nRButtons[]  = {IDB_LPT1, IDB_LPT2, IDB_LPT3};

   
   
   // *************************************************************************
   //  main section of function
   // *************************************************************************
   
   
   for (i=0; i < NUMBER_PRINTER_PORTS; i++)
     {
        temp = (int) SendDlgItemMessage(hDlg,nRButtons[i],BM_GETSTATE,0,0L);
        
        if (temp & BFCHECK)
          { 
            strcpy(szPrinterPort,szPorts[i]);
            return;
          }
     }
}     




//*************************************************************************************
//*************************************************************************************
//*************************************************************************************
//
// Function:  GetPrinterDriver
//
//     Parameters:      
//          printer_buffer - used to hold the installed printer drivers
//
//     Purpose:
//          
//          This function obtains the printer drivers installed...checks
//          WIN.INI's [PrinterPorts] section.  Use GetProfileString to obtain
//          all the printer drivers installed.  All the printer drivers are put
//          into a buffer (szBuffer).  The function will then parse all the installed
//          printers in szBuffer...and put them into printer_buffer.
//
//*************************************************************************************
//*************************************************************************************
//*************************************************************************************


void GetPrinterDriver (char printer_buffer[][PMAX_LENGTH])

{
   
   // *************************************************************************
   //  local variables
   // *************************************************************************
   
   int nCount;                                  // counter
   int nBytes = 0;                              // number of bytes read
   
   char szbuffer[MAX_PRINTERS * PMAX_LENGTH];   // temp buffer
   char *pszPointer;                            // pointer to tempbuffer

   LPSTR  lpszReturnBuffer;                     // pointer to buffer
   
   
   
   // *************************************************************************
   //  main section of function
   // *************************************************************************
   
   
   // *************************************************************************
   //  init 
   // *************************************************************************
   
   pszPointer = szbuffer;

   lpszReturnBuffer  = szbuffer;
   
   
   for (nCount = 0; nCount < (MAX_PRINTERS * PMAX_LENGTH); nCount++)   // init buffer
     szbuffer[nCount] = STRING_NULL;


   
   
   // ******************************************************************************
   //  get all the printer drivers installed - all printers are put into the buffer
   // ******************************************************************************

   nBytes = GetProfileString("PrinterPorts",NULL,"",lpszReturnBuffer,sizeof(szbuffer));
   

   if (nBytes > 0)                                 // > 0 means we have some printer drivers
     {
       
       for (nCount = 0; nCount < MAX_PRINTERS; nCount++)
         {
            if (*pszPointer != STRING_NULL)
               strcpy(printer_buffer[nCount], pszPointer);
         
         
            while (*pszPointer != STRING_NULL)     // this gets me to next \0
              pszPointer++;

            pszPointer++;                          // gets me to next printer
         
            if (*pszPointer == STRING_NULL)        // no more printers to process
              return;
         
         }
     
     
     }

}     




//*************************************************************************************
//*************************************************************************************
//*************************************************************************************
//
// Function:  TranslateIndex
//
//     Parameters:      
//          wResource:  The resource currently selected
//          wIndex   :  Index of the resource
//
//
//     Purpose:
//          
//          This function is used to translate the drive/printer index into
//          more connection information (drive/port & share).
//
//*************************************************************************************
//*************************************************************************************
//*************************************************************************************


void TranslateIndex (WORD wResource, WORD wIndex)

{
   
   // *************************************************************************
   //  local variables
   // *************************************************************************
   
   
   WORD wRemoteName = MAX_LENGTH;         // number of bytes in remote name
   WORD wReturn;                          // function return

   char szResource[MAX_LENGTH];           // string for resource
   
   char szbuffer[BUFFER_LENGTH];          // output buffer
   char cDriveLetter;                     // drive letter
   
   char *szPorts[]   = {"xxx", "LPT1:", "LPT2:", "LPT3:"};         // printer ports
   

   // *************************************************************************
   //  main section of function
   // *************************************************************************
   
   szResource[0] = STRING_NULL;                                      // init
   szPath[0]     = STRING_NULL;
   
   
   if (wIndex == -1)                      // -1 when no connection information available
     {
        switch (wResource)
          {
            case WNTYPE_PRINTER:
        
               wsprintf(szbuffer,"wPrinterIndex=%d.\tLast connection cannot be determined.", 
                        wIndex);
               break;
             
             
            case WNTYPE_DRIVE:
                
               wsprintf(szbuffer,"wDriveIndex=%d.  \tLast connection cannot be determined.", 
                        wIndex);
               break;
          }
        
        SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR)szbuffer);
     
     }
   else
     {
        switch (wResource)
          {
       
            case WNTYPE_PRINTER:
          
               strcpy(szResource, szPorts[wIndex]);                             // get the port
            
               wReturn = WNetGetConnection(szResource, szPath, &wRemoteName);   // get connection info
          
          
               if (wReturn == WN_NOT_CONNECTED)
                  wsprintf(szbuffer,"wPrinterIndex=%d.\tPort %s is no longer connected!!",
                           wIndex, (LPSTR)szResource);
                
               else
                  wsprintf(szbuffer,"wPrinterIndex=%d.\tPort %s connected to %s.",
                           wIndex, (LPSTR)szResource, (LPSTR)szPath);
            
               break;


       
            case WNTYPE_DRIVE:
          
               cDriveLetter = 'A'+ wIndex;                                // get the drive letter
          
               wsprintf(szResource,"%c:\0", cDriveLetter);

               wReturn = WNetGetConnection(szResource, szPath, &wRemoteName);   // get connection info
          
          
               if (wReturn == WN_NOT_CONNECTED)
                  wsprintf(szbuffer,"wDriveIndex=%d.  \tDrive %s is no longer connected!!",
                           wIndex, (LPSTR)szResource);
            
               else
                  wsprintf(szbuffer,"wDriveIndex=%d.  \tDrive %s connected to %s.",
                           wIndex, (LPSTR)szResource, (LPSTR)szPath);
            
               break;     
          }
     
        SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR)szbuffer);
     }

}     



//*************************************************************************************
//*************************************************************************************
//*************************************************************************************
//
// Function:  Detect_WFW
//
//     Parameters:      none
//
//
//     Purpose:
//          
//          Checks to see if Windows for Workgroups is installed.
//
//          Please refer to WFW SDK docs - Chapter 1 titled "Windows for Workgroups
//          Basics", section 1.3 ("Checking for Windows for Workgroups")
//
//
//          Overview:   1.  Use WNetGetCaps to get the network type
//                      2.  Check to see if multiple-network bit is set
//                      3.  If multiple-network bit is set...then check low byte for WFW bit
//
//
//     Returns:
//
//          WFW_DETECTED:     if Windows for Workgroups is detected
//          WFW_NOTDETECTED:  if Windows for Workgroups is NOT detected
//
//*************************************************************************************
//*************************************************************************************
//*************************************************************************************


int Detect_WFW (void)

{
   
   // *************************************************************************
   //  local variables
   // *************************************************************************
   
   UINT uCaps;      


   // *************************************************************************
   //  main section of function
   // *************************************************************************

    
    uCaps = WNetGetCaps(WNNC_NET_TYPE);                     // WNetGetCaps returns network type
         
    
    if (uCaps & WNNC_NET_MultiNet)                          // check for mutiple-network bit
      {
        
        if ((LOBYTE(uCaps)) & WNNC_SUBNET_WinWorkgroups)    // check low byte for WFW bit
          {
             
             MessageBox(NULL,"Windows for Workgroups is running on this computer!!",
                        "Checking for Windows for Workgroups",MB_ICONEXCLAMATION | MB_OK);
          
             
             return WFW_DETECTED;
          }
        else  
          {  
             
             MessageBox(NULL,"Windows for Workgroups is not running on this computer! \n\nPlease ensure you are running Windows for Workgroups!!",
                        "Checking for Windows for Workgroups",MB_ICONSTOP | MB_OK);                   
             
             return WFW_NOTDETECTED;
          
          }
      }
    else
      {
        MessageBox(NULL,"Windows for Workgroups is not running on this computer! \n\nPlease ensure you are running Windows for Workgroups!!",
                   "Checking for Windows for Workgroups",MB_ICONSTOP | MB_OK);                   
        
        return WFW_NOTDETECTED;
      }
        
    //*************************************************************************************
    //************************* end of check for WFW **************************************        
    //*************************************************************************************
}

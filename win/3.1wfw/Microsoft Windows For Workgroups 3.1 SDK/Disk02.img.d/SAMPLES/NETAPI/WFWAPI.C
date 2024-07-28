//**************************************************************************
//
//  Filename: WFWAPI.C
//
//  Purpose:
//      
//      This is just a sample program to call some of the
//      Window for Workgroups (aka WFW) API's as described in the WFW SDK.
//
//
//  Other C Files:
//
//      a) output.c     - to output messages to the test window
//      b) net.c        - to call some of the network (NET) functions
//      c) multinet.c   - to call some of the multiple network (MNET) functions
//      d) wnet.c       - to call some of the Windows Network (WNET) functions
//      e) utility.c    - general functions
//
//
//  Notes:  3-D controls (ctl3d .h, .dll, .lib) are from the Microsoft Developer
//          Network CD-ROM.  For information on joining the Microsoft Developer 
//          Network:
//
//             call: (800) 227-4697 (6:30am - 5:30 PDT).
//
//          Or if you are interested in only receiving Microsoft's Developer
//          Network News (a bi-monthly publication), send your request along
//          with your Name, Company, and Address to:
//
//             Microsoft Developer Network
//             One Microsoft Way
//             Redmond, WA  98052-6399
//          
//
//          Future versions of Windows will implement all controls in 3-D.           
//          
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

#include "CTL3D.H"          // 3-D control Header 

#include "WFWAPI.H"         // Sample App Header

#include <WFWNET.H>         // Windows for Workgroups Header
#include <WINNET.H>         // Windows Network (WNET) Header



//**************************************************************************
// Global Variables
//**************************************************************************


#include "global.h"         // Global Variables Header



//*************************************************************************************
//*************************************************************************************
//*************************************************************************************
//
// Function:  WinMain
//
//     Parameters:
//         hInstance     - Handle to current Data Segment
//         hPrevInstance - Handle to previous Data Segment (NULL if none)
//         lpszCmdLine   - Long pointer to command line info
//         nCmdShow      - Integer value specifying how to start app.,
//                            (Iconic [7] or Normal [1,5])
//
//     Purpose:
//         Windows main function
//
//*************************************************************************************
//*************************************************************************************
//*************************************************************************************


int PASCAL WinMain (HANDLE hInstance, 
                    HANDLE hPrevInstance, 
                    LPSTR  lpszCmdLine,
                    int    nCmdShow)
{

    //*************************************************************************************
    //  Local Variables
    //*************************************************************************************
    
    
    int  nReturn;

    
    //*************************************************************************************
    //************************* end of check for WFW **************************************        
    //*************************************************************************************
    
    
    
    if (Init(hInstance, hPrevInstance,lpszCmdLine,nCmdShow))
      {
         nReturn = MainLoop(hInstance);                     // process message loop
         
      }
    
    return nReturn;

}





//*************************************************************************************
//*************************************************************************************
//*************************************************************************************
//
// Function: Init
//
//      Parameters:
//          hInstance     - Handle to current Data Segment
//          hPrevInstance - Handle to previous Data Segment (NULL if none)
//          lpszCmdLine   - Long pointer to command line info
//          nCmdShow      - Integer value specifying how to start app.,
//                            (Iconic [7] or Normal [1,5])
//
//      Purpose:          
//          Initialization for the program is done here:
//
//
//*************************************************************************************
//*************************************************************************************
//*************************************************************************************

BOOL Init (HANDLE hInstance,   HANDLE hPrevInstance,
           LPSTR  lpszCmdLine, int    nCmdShow)

{
    
    //*************************************************************************************
    //  Local Variables
    //*************************************************************************************
    
    
    WNDCLASS rClass;             // winclass variable
    
    DWORD dwVersion;             // windows version variable
    
    
    
    //*************************************************************************************
    // Main section of function
    //*************************************************************************************
    

    //*********************************************************************************
    // if not already running.... 
    //*********************************************************************************
    
    if (!hPrevInstance)          
      {                                     
    
    
         //****************************************************************************
         // Check to see if WFW is running
         //****************************************************************************
      
         if (Detect_WFW() == WFW_NOTDETECTED)
            return FALSE;
    
    
         

         //*************************************************************************************
         // Init some of the global variables
         //*************************************************************************************
    
         hInst = hInstance;                 // set Global instance handle to this program's handle

         bWin4x = FALSE;                    // Windows version 4x?
    
    

         //*************************************************************************************
         // Check Windows Version...
         //*************************************************************************************
     
         dwVersion = GetVersion();

    
         if (LOBYTE(LOWORD(dwVersion)) > 3)       // windows version greater than 3.x?
        
            bWin4x = TRUE;
      

    
         //*************************************************************************************
         //  Setup 3D controls - only if not running Windows 4.x
         //  In other words...disable 3D controls if running Windows 4.x or above since
         //    Windows 4.x will implement all controls in 3D.
         //*************************************************************************************
     
         if (!bWin4x)
           {
               Ctl3dRegister(hInst);        // setup for 3-d controls (CTL3D.DLL)
               Ctl3dAutoSubclass(hInst);    // auto subclassing for all controls
           }



      
         
         //****************************************************************************
         // fill in the window class attributes
         //****************************************************************************
         
         
         rClass.style           = 0;
         rClass.lpfnWndProc     = OverlappedWindowProc1;
         rClass.cbClsExtra      = 0; 
         rClass.cbWndExtra      = 0; 
         rClass.hInstance       = hInstance;
         rClass.hIcon           = LoadIcon(hInstance,(LPCSTR)MAKEINTRESOURCE(WFWICON));
         rClass.hCursor         = NULL;
         rClass.hbrBackground   = COLOR_WINDOW+1;
         rClass.lpszMenuName    = "MENU1";
         rClass.lpszClassName   = "WFWAPI";

         if (!RegisterClass (&rClass))         // register the Window Class
           {
              return FALSE;
           }
        
       

         //*********************************************************************************
         //  Create the Window
         //*********************************************************************************


         hWnd1 = CreateWindow ("WFWAPI",                                                        // window class name
                               "Windows for Workgroups API Test Window",                        // window caption 
                               WS_OVERLAPPEDWINDOW,                                             // window style
                               GetPrivateProfileInt("coor","nX",CW_USEDEFAULT,INIFILE),         // initial x position
                               GetPrivateProfileInt("coor","nY",CW_USEDEFAULT,INIFILE),         // initial y position
                               GetPrivateProfileInt("coor","nWidth",CW_USEDEFAULT,INIFILE),     // initial x size
                               GetPrivateProfileInt("coor","nHeight",CW_USEDEFAULT,INIFILE),    // initial y size
                               NULL,                                                            // handle to parent
                               NULL,                                                            // window menu handle
                               hInstance,                                                       // program instance handle
                               NULL);                                                           // creation paramters



         ShowWindow(hWnd1, nCmdShow);

         return hWnd1;
      }
    else
      {
         
         //********************************************************************************
         //  Since this app is already running...get handle of previous instance & make the
         //  first instance of the app the active window.
         //******************************************************************************** 
         
         
         hWnd1 = FindWindow("WFWAPI", "Windows for Workgroups API Test Window");    

         SetActiveWindow(hWnd1);

         return FALSE;
      }
}




//*************************************************************************************
//*************************************************************************************
//*************************************************************************************
//
// Function: MainLoop
//
//      Parameters:
//          hInstance     - Handle to current Data Segment
//
//      Purpose:          
//          This is where the main windows message processing loop resides.
//          This is also where the yield point is for the application
//
//*************************************************************************************
//*************************************************************************************
//*************************************************************************************

int  MainLoop (HANDLE hInstance)

{
    //*************************************************************************************
    //  Local Variables
    //*************************************************************************************
    
    
    MSG msg;                                            // message declaration
    
    
    //*************************************************************************************
    //  message processing loop is here
    //*************************************************************************************
    
    
    while (GetMessage(&msg,NULL,0,0))       // get messages from app/system queue
      {
         TranslateMessage(&msg);            // translate the msg
         DispatchMessage(&msg);             // send the msg back to Win for processing
         
      }
    
    return msg.wParam;

}




//*************************************************************************************
//*************************************************************************************
//*************************************************************************************
//
// Function: OverlappedWindowProc1
//
//      Parameters:
//          hWnd    - Handle to Window which message is delivered to.
//          msgID   - ID number of message
//          wParam  - Additional Information - 16-bit parameter
//          lParam  - Additional Information - 32-bit parameter
//
//
//      Purpose:          
//          Windows Procedure - which messages do I want to process?
//          This procedur is called by Windows...never from my own code
//
//*************************************************************************************
//*************************************************************************************
//*************************************************************************************


long FAR PASCAL OverlappedWindowProc1 (HWND    hWnd,
                                       UINT    wMsgID,
                                       WPARAM  wParam,
                                       LPARAM  lParam)

{
    //*************************************************************************************
    //  Local Variables
    //*************************************************************************************
    
    RECT rc;                            // rectangle struct

    char szbuffer2[BUFFER_LENGTH];      // buffer

    HDC hdc;                            // handle to a DC
    
    int nHeight;                        // font height
   
    static HANDLE hFont;                // handle to a font
   

    //************************************************************************   
    //  main part of window procedure
    //************************************************************************
   
    
    switch (wMsgID)
      {


        case WM_CREATE:

            
            //************************************************************************   
            //  Create instance thunk for dialog procedures
            //************************************************************************
            
            lpfnDlgProc  = MakeProcInstance((FARPROC)DlgProc,hInst);
            lpfnDlgProc2 = MakeProcInstance((FARPROC)DlgProc2,hInst);
            
            
            
            //************************************************************************   
            //  Create a ListBox that covers the current window's client area.
            //  This ListBox will be used to display all the output messages.
            //************************************************************************
            
            GetClientRect( hWnd, &rc );
            
            hLBhwnd  = CreateWindow( "LISTBOX",
                                     "Windows for Workgroups API Test Window",
                                     WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | LBS_USETABSTOPS | LBS_NOINTEGRALHEIGHT,
                                     0,   
                                     0,
                                     rc.right,
                                     rc.bottom,
                                     hWnd,
                                     NULL,
                                     hInst,
                                     NULL );

            //************************************************************************   
            //  Calculate a 9 point font - and select a proportionally spaced font.
            //  Courier New to be used for messages to be displayed in the ListBox
            //************************************************************************


            hdc = CreateIC( "DISPLAY", NULL, NULL, NULL );
            
            nHeight = -(9 * GetDeviceCaps( hdc, LOGPIXELSY )) / 72;
            
            DeleteDC( hdc );

            hFont = CreateFont( nHeight, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Courier New" );

            SendMessage(hLBhwnd, WM_SETFONT, hFont, FALSE);

            break;
            

        
        case WM_SETFOCUS:
         
            SetFocus(hLBhwnd);                    // set focus to the ListBox

            break;


        
        
        case WM_SIZE:

            MoveWindow(hLBhwnd,0,0,LOWORD(lParam),HIWORD(lParam),TRUE);   // move ListBox window to
                                                                          // cover the entire client area
            break;
        
        
        
        
        case WM_COMMAND:
        
          
           switch (wParam)
             {
               
               case IDM_SHARES_DIALOG:               
                  
                  wFunction = IDM_SHARES_DIALOG;

                  wsprintf(szbuffer2,"WNetSharesDialog is being called:");
                  SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
                  
                  wsprintf(szbuffer2,"WNetSharesDialog is NOT IMPLEMENTED!!");
                  SendMessage(hLBhwnd,LB_ADDSTRING,0,(LPARAM)(LPCSTR) szbuffer2);
                  
                  MessageBox(hWnd,"WNetSharesDialog: This function is NOT implemented!!","FUNCTION WARNING!",MB_ICONSTOP | MB_OK);
                  
                  SetLBSelection();
                  break;


               case IDM_SHAREAS_DIALOG:
                  
                  wFunction = IDM_SHAREAS_DIALOG;
                  
                  NetShareAsDialog();                  // this function is in wnet.c
                  
                  SetLBSelection();                    // this function is in utility.c
                  
                  break;

               
               case IDM_SERVER_BROWSE_DIALOG:               
                  
                  wFunction = IDM_SERVER_BROWSE_DIALOG;

                  NetServerBrowseDialog();            // this function is in wnet.c
                  
                  SetLBSelection();
                  
                  break;

               
               case IDM_STOPSHARE_DIALOG:               
                  
                  wFunction = IDM_STOPSHARE_DIALOG;
                  
                  NetStopShareDialog();               // this function is in wnet.c
                  
                  SetLBSelection();
                  
                  break;

               
               case IDM_NETWKSTAGETINFO:
                  
                  wFunction = NO_FUNCTION;
                  
                  WkstaGetInfo();                     // this function is in net.c
                  
                  SetLBSelection();
                  
                  break;

               
               case IDM_NETSERVERENUM2:
                  
                  wFunction = IDM_NETSERVERENUM2;

                  NetEnum2();                         // this function is in net.c
                  
                  SetLBSelection();
                  
                  break;
               
               
               case IDM_MULTINET:
                  
                  wFunction = NO_FUNCTION;
                  
                  Multinet();                         // this function is in multinet.c
                  
                  SetLBSelection();
                  
                  break;
               
               
               case IDM_BROWSE_DIALOG:
                  
                  wFunction = IDM_BROWSE_DIALOG;

                  NetBrowseDialog();                  // this function is in wnet.c
                  
                  SetLBSelection();
                  
                  break;

               
               case IDM_CONNECT_DIALOG:
                  
                  wFunction = IDM_CONNECT_DIALOG;

                  NetConnectDialog();                 // this function is in wnet.c
                  
                  SetLBSelection();
                  
                  break;
               
               
               case IDM_CONNECTION_DIALOG:
                  
                  wFunction = IDM_CONNECTION_DIALOG;

                  NetConnectionDialog();              // this function is in wnet.c
                  
                  SetLBSelection();
                  
                  break;


               case IDM_DISCONNECT_DIALOG:
                  
                  wFunction = IDM_DISCONNECT_DIALOG;

                  NetDisconnectDialog();              // this function is in wnet.c
                  
                  SetLBSelection();
                  
                  break;
               
               

               case IDM_GETLASTCONNECTION:
                  
                  wFunction = IDM_GETLASTCONNECTION;

                  GetLastConnection();                // this function is in wnet.c
                  
                  SetLBSelection();
                  
                  break;

               
               case IDM_GETSHARECOUNT:
                  
                  wFunction = IDM_GETSHARECOUNT;

                  GetShareCount();                    // this function is in wnet.c
                  
                  SetLBSelection();

                  break;


               case IDM_GETSHARENAME:
                  
                  wFunction = IDM_GETSHARENAME;

                  GetShareName();                     // this function is in wnet.c
                  
                  SetLBSelection();
                  
                  break;
               
               
               case IDM_GETSHAREPATH:
                  
                  wFunction = IDM_GETSHAREPATH;

                  GetSharePath();                     // this function is in wnet.c
                  
                  SetLBSelection();
                  
                  break;
               
               
               
               case IDM_EXIT:
                  
                  SendMessage(hWnd,WM_CLOSE,0,0);                  
                  
                  break;
            
            
            
               case IDM_HELP_ABOUT:

                  DialogBox(hInst,MAKEINTRESOURCE(IDM_HELP_ABOUT),hWnd,lpfnDlgProc);
                  
                  break;

            
             }
          
          break;



        
        case WM_DESTROY:
          
          
          //************************************************************************   
          //  Free up items created previously
          //************************************************************************
          
          FreeProcInstance(lpfnDlgProc);    // freeing the instance thunk for dialog proc #1
          FreeProcInstance(lpfnDlgProc2);   // freeing the instance thunk for dialog proc #2
          
          DeleteObject(hFont);              // delete the font (courier new - 9pt)

          
          if (!bWin4x)
             Ctl3dUnregister(hInst);        // unregister the 3D controls
          
          
          //************************************************************************   
          //  Save the current window coordinates into the .ini file - only  if 
          //  app is not iconic
          //************************************************************************
          
          if (! (IsIconic(hWnd1)))
            {
               GetWindowRect(hWnd,&rc);          // get current windows coordinates
          
               wsprintf(szbuffer2, "%d",rc.left);
               WritePrivateProfileString("coor","nX",szbuffer2,INIFILE);
          
               wsprintf(szbuffer2, "%d",rc.top);
               WritePrivateProfileString("coor","nY",szbuffer2,INIFILE);
          
               wsprintf(szbuffer2, "%d",rc.bottom - rc.top);
               WritePrivateProfileString("coor","nHeight",szbuffer2,INIFILE);
          
               wsprintf(szbuffer2, "%d",rc.right  - rc.left);
               WritePrivateProfileString("coor","nWidth",szbuffer2,INIFILE);
            }
          
          
          //************************************************************************   
          //  Post the destroy message
          //************************************************************************
          
          PostQuitMessage(0);               
          
          return 0;
          
        
        
        //************************************************************************   
        //  else pass the message to Default Windows Procedure...
        //************************************************************************
        
        default:
          
          return DefWindowProc(hWnd, wMsgID, wParam, lParam);
      
      }


}


//*************************************************************************************
//*************************************************************************************
//*************************************************************************************
//
// Dialog Procedure DlgProc - 
//
//      Parameters:
//          hWnd    - Handle to Window which message is delivered to.
//          msgID   - ID number of message
//          wParam  - Additional information - 16-bit parameter
//          lParam  - Additional information - 32-bit parameter
//
//
//      Purpose:
//          To process Dialog Box messages.
//
//*************************************************************************************
//*************************************************************************************
//*************************************************************************************

long FAR PASCAL DlgProc(HWND    hWndDlg,
                        UINT    wMsgID,
                        WPARAM  wParam,
                        LPARAM  lParam)

{
   
   switch (wMsgID)
     {

       case WM_INITDIALOG:
        
         SendDlgItemMessage(hWndDlg,IDB_DRIVE, BM_SETCHECK, TRUE, 0L);
         
         SetDlgItemText(hWndDlg, IDD_WORKGROUP, szPath);

         return TRUE;

      
      
      
       case WM_COMMAND:
      
         switch (wParam)
           {
            
             case IDB_OK:
         
                
                switch (wFunction)
                  {
                     case IDM_SHAREAS_DIALOG:
                        GetDlgItemText(hWndDlg,IDD_PATH,szPath,MAX_LENGTH);             
                        break;

                     case IDM_GETSHARENAME:
                        GetDlgItemText(hWndDlg,IDD_PATH_ORDEVICE,szPath,MAX_LENGTH);   
                        break;

                     case IDM_GETSHAREPATH:
                        GetDlgItemText(hWndDlg,IDD_NETWORK_NAME,szPath,MAX_LENGTH);   
                        break;
                      
                     case IDM_NETSERVERENUM2:
                        GetDlgItemText(hWndDlg,IDD_WORKGROUP,szPath,MAX_LENGTH);   
                        break;
                  }

                
                if ((wFunction != IDM_SERVER_BROWSE_DIALOG) && (wFunction != NO_FUNCTION))
                   
                   wResourceType = GetResourceType(hWndDlg);     // this function is in utility.c
                
                
                

                EndDialog(hWndDlg,TRUE);
                
                return TRUE;
                   

             
             case IDB_CANCEL:
         
                EndDialog(hWndDlg,FALSE);
                return TRUE;

           }   
     }

   
   return FALSE;

}     
     


//*************************************************************************************
//*************************************************************************************
//*************************************************************************************
//
// Dialog Procedure DlgProc #2- 
//
//     Parameters:
//         hWnd    - Handle to Window which message is delivered to.
//         msgID   - ID number of message
//         wParam  - Additional information - 16-bit parameter
//         lParam  - Additional information - 32-bit parameter
//
//
//*************************************************************************************
//*************************************************************************************
//*************************************************************************************

long FAR PASCAL DlgProc2(HWND    hWndDlg,
                        UINT    wMsgID,
                        WPARAM  wParam,
                        LPARAM  lParam)

{
    //*************************************************************************************
    //  Local Variables
    //*************************************************************************************
   
    int nCount;
   
    static char szbuffer[MAX_PRINTERS][PMAX_LENGTH];
   
    static int counter;

    
    
    //*************************************************************************************
    // main section of the dialog procedure 
    //*************************************************************************************
   
    
    //*************************************************************************************
    //  init the buffer if 1st time thru
    //*************************************************************************************
   
    
    if (counter == 0)                                       // first time? init the buffer
      {
        for (nCount = 0; nCount < MAX_PRINTERS; nCount++)
          szbuffer[nCount][0] = STRING_NULL;
       
        counter++;                                          // loop counter....

        GetPrinterDriver(szbuffer);                         // get the installed printer drivers
                                                            // this function is in utility.c
      }

   
    switch (wMsgID)
      {

        case WM_INITDIALOG:
        
           SendDlgItemMessage(hWndDlg,IDB_LPT1, BM_SETCHECK, TRUE, 0L);
        
           for (nCount = 0; nCount < MAX_PRINTERS; nCount++)
             {
               if (szbuffer[nCount][0] != STRING_NULL)
                 SendDlgItemMessage(hWndDlg,IDD_PRINTER_NAME, CB_ADDSTRING, 0, (LPARAM)((LPCSTR)szbuffer[nCount]));
             }

           SendDlgItemMessage(hWndDlg,IDD_PRINTER_NAME, CB_ADDSTRING, 0, (LPARAM)((LPCSTR)"[INVALID PRINTER]"));
        
           return TRUE;

      
      
      
        case WM_COMMAND:
      
           switch (wParam)
             {
            
                case IDB_OK:
                
                   GetPrinterPort(hWndDlg);           // this function is in utility.c

                
                   GetDlgItemText(hWndDlg,IDD_PRINTER_NAME,szPath,MAX_LENGTH);
                
                   
                   EndDialog(hWndDlg,TRUE);
                
                   return TRUE;
                   

                case IDB_CANCEL:
         
                   EndDialog(hWndDlg,FALSE);
                
                   return TRUE;
             }   
      }

   
   return FALSE;

}     
     





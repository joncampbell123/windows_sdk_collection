//

//	FILE:    TTY.c

//	PURPOSE: This sample terminal application demonstrates
//          the basic uses of Windows Communications functions. 
//          It also shows the basic structure for a terminal program.

//	FUNCTIONS:
//          WinMain() - Initializes app, calls all other functions.
//          TTYWndProc() - Window procedure for terminal window.
//          About() - Window procedure for About dialog box.
//          AboutInit() - Initialization procedure for About dialog box.
//          SettingDlgProc() - Window procedure for Comm Settings dialog.
//

#include <windows.h>
#include <string.h>
#include "wstdio.h"
#include "tty.h"


//========================================================================\\

// Declarations

//========================================================================\\

#define COM1  "com1"
#define COM2  "com2"
#define CommSettings "com1:96,n,8,1"

#define BufMax	    160     // Size of line buffer used for displaying text
#define cbInBuf    1024    // Size of receive buffer
#define cbOutBuf   128     // size of transmit buffer

DCB CommDCB;               // DCB for comm port
int PortID;                // The comm port id
COMSTAT CommStat;          // COMSTAT info buffer for comm port
char MsgBuff[BufMax + 1];  // Buffer to hold incoming characters
BOOL bConnected;           // Flag to indicate if connected
short nCommErr;            // Storage for communications error data
WORD wCommEvt;             // Storage for communications event data

HWND hTTYWnd;              // Handle to application window

char sTemp[256];

static HANDLE hInst;       // Global instance handle
FARPROC lpprocAbout;       // Pointer to "About" dialog box procedure
FARPROC lpfnOldTTYProc;    // Pointer to TTY proc prior to subclassing

long FAR PASCAL TTYWndProc(HWND, unsigned, WORD, LONG);


//========================================================================\\

// FUNCTION: About(HWND, unsigned, WORD, LONG)

// PURPOSE:  Processes messages for About dialog box.

//========================================================================\\

BOOL FAR PASCAL About( hDlg, message, wParam, lParam )
HWND hDlg;
unsigned message;
WORD wParam;
LONG lParam;
{
    if (message == WM_COMMAND
	|| message == WM_LBUTTONDOWN) {

	// if we click the mouse in the dialog, or press 'Enter', then go away
        EndDialog( hDlg, TRUE );
        return TRUE;
        }
    else if (message == WM_INITDIALOG)
        return TRUE;
    else return FALSE;
}


//========================================================================\\

// FUNCTION: AboutInit(HWND, HANDLE)

// PURPOSE:  About box initialization.

//========================================================================\\

BOOL AboutInit(HWND hWnd, HANDLE hInstance)
{
    HMENU hMenu;

    /* Bind callback function with module instance */
    lpprocAbout = MakeProcInstance( (FARPROC)About, hInstance );

    return TRUE;
}


//========================================================================\\

// FUNCTION: SettingDlgProc(HWND, unsigned, WORD, LONG)

// PURPOSE:  Processes messages for Communications Settings dialog.

//========================================================================\\

BOOL FAR PASCAL SettingDlgProc( hDlg, message, wParam, lParam )
HWND hDlg;
unsigned message;
WORD wParam;
LONG lParam;
{
   int theButton;
   static DCB dlgDCB;

   switch(message) {
      case WM_COMMAND:

         // if the Ok button is pressed the new settings are saved

         if(wParam == IDOK) {
            // save the new settings
            CommDCB = dlgDCB;
            // close the dialog
            EndDialog( hDlg, TRUE );

         } else

         // otherwise, the settings are not saved and changes are discarded
            
            if(wParam == IDCANCEL)
               EndDialog(hDlg,FALSE);
            else
               if(HIWORD(lParam) == BN_CLICKED) {

                  // if a button is clicked and it is a radiobutton,
                  // then we uncheck the current selection and check
                  // the one that was clicked.

                  HWND hStartWnd = GetDlgItem(hDlg,wParam);

                  if(LOWORD(GetWindowLong(hStartWnd,GWL_STYLE))
                     == BS_AUTORADIOBUTTON){
                     HWND hCurrWnd = hStartWnd;
                     do{
                        hCurrWnd = GetNextDlgGroupItem(hDlg,hCurrWnd,1);
                        SendMessage(hCurrWnd, BM_SETCHECK, hCurrWnd == hStartWnd, 0L);
                        } while(hCurrWnd != hStartWnd);
                  }

			// now we set the appropriate value in the DCB for the
			// button that was clicked

			switch (wParam){
                            case RBBAUD_300: dlgDCB.BaudRate = 300; break;
                            case RBBAUD_1200: dlgDCB.BaudRate = 1200; break;
                            case RBBAUD_2400: dlgDCB.BaudRate = 2400; break;
			    case RBBAUD_9600: dlgDCB.BaudRate = 9600; break;

                            case RBDBITS_7: dlgDCB.ByteSize = 7; break;
			    case RBDBITS_8: dlgDCB.ByteSize = 8; break;

                            case RBPARITY_EVEN: dlgDCB.Parity = EVENPARITY; break;
                            case RBPARITY_ODD: dlgDCB.Parity = ODDPARITY; break;
			    case RBPARITY_NONE: dlgDCB.Parity = NOPARITY; break;

                            case RBSBITS_2: dlgDCB.StopBits = TWOSTOPBITS; break;
			    case RBSBITS_1: dlgDCB.StopBits = ONESTOPBIT; break;

			    case CBXONXOFF: dlgDCB.fInX = dlgDCB.fInX?0:1; break;

                            case RBPORT_COM1: dlgDCB.Id = 1; break;
                            case RBPORT_COM2: dlgDCB.Id = 2; break;
                        }
		    } else
                       return FALSE;

            break;

	case WM_INITDIALOG:

		// make a copy of the current DCB
		// we will change this copy, and copy back to the original
		// if we click Ok

		dlgDCB = CommDCB;

		// set buttons as reflected by the current DCB

		// if the current port isn't com2, set com1 button
		theButton = (dlgDCB.Id == 2 ? RBPORT_COM2 : RBPORT_COM1);
		SendDlgItemMessage(hDlg,theButton,BM_SETCHECK,1,0L);

		// set baud button
                switch(dlgDCB.BaudRate){
                    case 300: theButton = RBBAUD_300; break;
                    case 1200: theButton = RBBAUD_1200; break;
                    case 2400: theButton = RBBAUD_2400; break;
                    case 9600: theButton = RBBAUD_9600; break;
                    default: theButton = RBBAUD_300; break;
		}
		SendDlgItemMessage(hDlg,theButton,BM_SETCHECK,1,0L);

		// set data bits button. if it's not 8, then it's 7
                theButton = (dlgDCB.ByteSize == 8 ? RBDBITS_8 : RBDBITS_7);
		SendDlgItemMessage(hDlg,theButton,BM_SETCHECK,1,0L);

		// set parity button
                switch(dlgDCB.Parity){
                    case EVENPARITY: theButton = RBPARITY_EVEN; break;
                    case ODDPARITY: theButton = RBPARITY_ODD; break;
                    case NOPARITY: theButton = RBPARITY_NONE; break;
                    default: theButton = RBPARITY_NONE; break;
                }
		SendDlgItemMessage(hDlg,theButton,BM_SETCHECK,1,0L);

		// set stop bits button. if it's not 2, then it's 1
                theButton = (dlgDCB.StopBits == TWOSTOPBITS ? RBSBITS_2 : RBSBITS_1);
		SendDlgItemMessage(hDlg,theButton,BM_SETCHECK,1,0L);

		// set Xon/Xoff check box to on or off
		SendDlgItemMessage(hDlg,CBXONXOFF,BM_SETCHECK,dlgDCB.fInX,0L);

            break;

        default:
            return FALSE;
    }
    return TRUE;
}


//========================================================================\\

// FUNCTION: WinMain(HANDLE, HANDLE, LPSTR, int)

// PURPOSE:  Main procedure of the application.

//========================================================================\\

int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int cmdShow;
{
    MSG   msg;
    BOOL bMsgAvail;
    short iNumRead,iError;

    hInst = hInstance;

    // initialize the stdio window library
    if(!hPrevInstance)
        if(!stdioInit(hInstance)) return FALSE;

    // create a stdio window
    if(!(hTTYWnd = CreateStdioWindow(
                      "TTY",
                      WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT,
                      CW_USEDEFAULT,
                      CW_USEDEFAULT,
                      CW_USEDEFAULT,
                      NULL,
                      hInstance,
                      TRUE)))
        return FALSE;

    // subclass the stdio window
     
    lpfnOldTTYProc = (FARPROC) SetWindowLong(hTTYWnd,GWL_WNDPROC,
                                          (DWORD) TTYWndProc);

    // add the about box to the system menu
    AboutInit(hTTYWnd,hInstance);

    // add the terminal menu
    SetMenu(hTTYWnd,LoadMenu(hInstance, "TTYMENU"));

    // set the application icon
    SetClassWord(hTTYWnd, GCW_HICON,
                 LoadIcon( hInstance, MAKEINTRESOURCE(TTYICON) ));

    bConnected = FALSE;

    // initialize the DCB to default settings
    if(BuildCommDCB(CommSettings,&CommDCB) != 0) {
        MessageBox(GetFocus(),"Error Building DCB!","",MB_OK);
    }

    CommDCB.CtsTimeout = 100;	       // Set Cts Timeout value
    CommDCB.DsrTimeout = 100;	       // Set Dsr Timeout value
    CommDCB.fOutX = 1;		       // output Xon/Xoff flow control on
    CommDCB.fInX = 1;		       // input Xon/Xoff flow control on
    CommDCB.XonChar = 0x11;	       // specify the Xon character
    CommDCB.XoffChar = 0x13;	       // specify the Xoff character
    CommDCB.fNull = 1;		       // strip null characters
    CommDCB.XonLim = 30;	       // distance from queue empty to Xon
    CommDCB.XoffLim = (cbInBuf/2) + 1; // distance from queue full to Xoff
    CommDCB.fBinary = 0;

    // show the window
    ShowWindow( hTTYWnd, cmdShow );
    UpdateWindow( hTTYWnd );

    // PeekMessage loop to poll the comm port and pull messages from the queue
    // if there is a message available, process it.
    // otherwise, check the port and handle any available characters.

    while(TRUE){
        bMsgAvail = PeekMessage(&msg,NULL,0,0,PM_REMOVE);

        if(bMsgAvail){

            if(msg.message == WM_QUIT) break;

            TranslateMessage((LPMSG)&msg);
            DispatchMessage((LPMSG)&msg);

        } else {

            // check the comm port and process any available
            // characters. you could also use a timer instead, and have
            // the timer case of the wndproc check the port.
            if(bConnected){

                // get the CommStat record and get the # of characters available
		GetCommError(CommDCB.Id,&CommStat);

                // get the number of characters available
                iNumRead = CommStat.cbInQue;

                if(iNumRead > 0) {

                    // get the number of characters rounded to the buffer size
                    if(iNumRead > BufMax) iNumRead = BufMax;

                    // read the characters
                    iNumRead = ReadComm(CommDCB.Id,MsgBuff,
                                        iNumRead);

                    // check for errors
                    if(iNumRead < 0) {
                        iNumRead = -iNumRead;
                        nCommErr = GetCommError(CommDCB.Id,&CommStat);
                        // clear the event mask
			wCommEvt = GetCommEventMask(CommDCB.Id,0xFFFF);
                        // display what the error was
                        LoadString(hInst, nCommErr, sTemp, 20);
                        MessageBox(GetFocus(), sTemp, "Comm Read Error!",MB_OK);
                    }

                    MsgBuff[iNumRead] = '\0';

                    // send the characters to the tty window for processing
                    SendMessage(hTTYWnd,COMM_CHARS,
				iNumRead,(LONG)(LPSTR)MsgBuff);
		    //wputs((LPSTR) MsgBuff);	// could just do this instead
                }


            }
        }
    }

}

//========================================================================\\

// FUNCTION: TTYWndProc(HWND, unsigned, WORD, LONG)

// PURPOSE:  Processes messages for the terminal window.

//========================================================================\\

long FAR PASCAL TTYWndProc( hWnd, message, wParam, lParam )
HWND hWnd;
unsigned message;
WORD wParam;
LONG lParam;
{
    FARPROC   lpSettingDlgProc;
    char      szErr[22];
    unsigned  nErr;

    switch (message)
    {

    case WM_DESTROY:
        PostQuitMessage( 0 );
        break;

    case WM_ENDSESSION:
	if (wParam && bConnected)
	    SendMessage(hWnd, WM_COMMAND, TTYCONNECT, 1L);
	break;

    case WM_CLOSE:
        // disconnect if still connected
        if(bConnected)SendMessage(hWnd,WM_COMMAND,TTYCONNECT,0L);
        // go ahead and close down
        return CallWindowProc(lpfnOldTTYProc,hWnd,
                              message,wParam,lParam);
        break;

    case WM_COMMAND:
        switch(wParam){

        case IDSABOUT:
            DialogBox( hInst, MAKEINTRESOURCE(ABOUTBOX), hWnd, lpprocAbout );
            break;

        case TTYEXIT:
            PostMessage(hWnd,WM_CLOSE,0,0L);
            break;

        case TTYCONNECT:
            // connect to port if not already connected
            if(!bConnected){
		if((PortID = OpenComm((CommDCB.Id == 2?COM2:COM1),cbInBuf,cbOutBuf)) < 0) {
                    MessageBox(hWnd,"Error Opening Comm Port!","",MB_OK);
                    break;
                }

                FlushComm(PortID,0);
                FlushComm(PortID,1);

		CommDCB.Id = PortID;
		if(CommDCB.fInX) {
		    CommDCB.fOutX = 1;	    // enable output Xon/Xoff flow ctl
		    CommDCB.fInX = 1;	    // enable input Xon/Xoff flow ctl
		    CommDCB.fRtsflow = 0;   // disable hardware flow ctl
		    CommDCB.fDtrflow = 0;   // disable hardware flow ctl
		} else {
		    CommDCB.fOutX = 0;	    // disable ouput Xon/Xoff flow ctl
		    CommDCB.fInX = 0;	    // disable input Xon/Xoff flow ctl
		    CommDCB.fRtsflow = 1;   // enable hardware flow ctl
		    CommDCB.fDtrflow = 1;   // enable hardware flow ctl
		}


                if(SetCommState(&CommDCB) !=0 ){
                    MessageBox(hWnd,"Error Setting CommState!","",MB_OK);
                    break;
                }
                bConnected = TRUE;
                CheckMenuItem(GetMenu(hWnd),TTYCONNECT,MF_CHECKED);
                EnableMenuItem(GetMenu(hWnd),TTYSETTINGS,MF_DISABLED | MF_GRAYED);
                MessageBox(hWnd,"Connection was successful.","",MB_OK);

            }else{
                // otherwise disconnect
                FlushComm(CommDCB.Id,0);
                FlushComm(CommDCB.Id,1);
                CloseComm(CommDCB.Id);
		if (!lParam)
                    MessageBox(hWnd,"Connection closed.","",MB_OK);
                bConnected = FALSE;
                CheckMenuItem(GetMenu(hWnd),TTYCONNECT,MF_UNCHECKED);
                EnableMenuItem(GetMenu(hWnd),TTYSETTINGS,MF_ENABLED);

            }
            break;

        case TTYSETTINGS:
            // settings dialog
            lpSettingDlgProc = MakeProcInstance(SettingDlgProc,hInst);
            DialogBox(hInst,"SETTINGSDLG",hWnd,lpSettingDlgProc);
            FreeProcInstance(lpSettingDlgProc);
            break;
        }
        break;

    case WM_CHAR:
	if(!bConnected) break;

	// if we're connected, send any keyboard characters to the port

        nCommErr = WriteComm(CommDCB.Id,(LPSTR) &wParam,1);
        if(nCommErr != 1) {
	    nCommErr = GetCommError(CommDCB.Id,&CommStat);
            if(nCommErr != 0) {
		sTemp[0] = 0;
		for (nErr = 1; nErr != 0; nErr = nErr << 1) {
		    if (nErr & nCommErr) {
			LoadString(hInst, nErr, szErr, 20);
			strcat(sTemp, szErr);
			strcat(sTemp, "\n");
		    }
		}
		MessageBox(hWnd, sTemp, "Comm Write Error!", MB_OK);
            }
	    wCommEvt = GetCommEventMask(CommDCB.Id, 0xFFFF);
        }
        break;

    case COMM_CHARS:
	// display available characters
        if(wParam > 0)
            wputs((LPSTR) lParam);

        break;

    // Pass all other messages to class's window procedure, since window
    // was subclassed
    default:
        return CallWindowProc(lpfnOldTTYProc,hWnd,
                              message,wParam,lParam);
        break;
    }
    return(0L);
}

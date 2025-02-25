/*----------------------------------------------------------------------------*\
|   mcitest.c - A testbed for MCI                                              |
|                                                                              |
|                                                                              |
|   History:                                                                   |
|       01/01/88 toddla     Created                                            |
|       03/01/90 davidle    Modified quick app into MCI testbed                |
|       09/17/90 t-mikemc   Added Notification box with 3 notification types   |
|       11/02/90 w-dougb    Commented & formatted the code to look pretty      |
|                                                                              |
\*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*\
|                                                                              |
|   i n c l u d e   f i l e s                                                  |
|                                                                              |
\*----------------------------------------------------------------------------*/

#include <windows.h>
#include <mmsystem.h>
#include <wincom.h>
#include "mcitest.h"
#include "mcimain.h"
#include "edit.h"


/*----------------------------------------------------------------------------*\
|                                                                              |
|   c o n s t a n t   a n d   m a c r o   d e f i n i t i o n s                |
|                                                                              |
\*----------------------------------------------------------------------------*/

#define BUFFER_LENGTH 128
#define SLASH(c)      ((c) == '/' || (c) == '\\')


/*----------------------------------------------------------------------------*\
|                                                                              |
|   g l o b a l   v a r i a b l e s                                            |
|                                                                              |
\*----------------------------------------------------------------------------*/

static  char        szAppName[]= "MCI Test";         
static  HANDLE      hInstApp;
static  HWND        hwndMainDlg = 0;
static  HWND        hwndEdit = 0;
static  HWND        hwndDevices = 0;
static  char        szMciFile[BUFFER_LENGTH] = "";
static  char        szBuffer[BUFFER_LENGTH];
static  int         nLastNumberOfDevices = 0;


/*----------------------------------------------------------------------------*\
|                                                                              |
|   f u n c t i o n   d e f i n i t i o n s                                    |
|                                                                              |
\*----------------------------------------------------------------------------*/

BOOL FAR PASCAL _export mcitester(HWND hwnd, unsigned msg, WORD wParam,
    long lParam);

BOOL fDialog(int id,HWND hwnd,FARPROC fpfn);
PSTR FileName(PSTR szPath);
void update_device_list(void);
void sendstring(HWND hDlg, PSTR strBuffer);
void execute( HWND hdlg, BOOL fStep);
void OpenMciFile( HWND hdlg, LPSTR szFile);
BOOL AppInit(HANDLE hInst, HANDLE hPrev, WORD sw, LPSTR szCmdLine);


/*----------------------------------------------------------------------------*\
|   AppAbout( hDlg, uiMessage, wParam, lParam )                                |
|                                                                              |
|   Description:                                                               |
|       This function handles messages belonging to the "About" dialog box.    |
|       The only message that it looks for is WM_COMMAND, indicating the user  |
|       has pressed the "OK" button.  When this happens, it takes down         |
|       the dialog box.                                                        |
|                                                                              |
|   Arguments:                                                                 |
|       hDlg            window handle of the about dialog window               |
|       uiMessage       message number                                         |
|       wParam          message-dependent parameter                            |
|       lParam          message-dependent parameter                            |
|                                                                              |
|   Returns:                                                                   |    
|       TRUE if the message has been processed, else FALSE                     |
|                                                                              |
\*----------------------------------------------------------------------------*/

BOOL FAR PASCAL _export AppAbout(hDlg, msg, wParam, lParam)

HWND hDlg;
unsigned msg;
WORD wParam;
long lParam;

{
    switch (msg) {

        case WM_COMMAND:

            if (wParam == IDOK)
                EndDialog(hDlg,TRUE);

        break;

        case WM_INITDIALOG:
            return TRUE;

    }

    return FALSE;
}


/*----------------------------------------------------------------------------*\
|   OpenMciFile( hDlg, szFile )                                                |
|                                                                              |
|   Description:                                                               |
|       This function opens the MCI file specified by <szFile> and updates the |
|       main window caption to display this file name along with the app name. |
|                                                                              |
|   Arguments:                                                                 |
|       hDlg            window handle of the main dialog window                |
|       szFile          pointer to the string containing the filename to be    |
|                        opened                                                |
|   Returns:                                                                   |    
|       void                                                                   |
|                                                                              |
\*----------------------------------------------------------------------------*/

void OpenMciFile(hdlg, szFile)

HWND hdlg;
LPSTR szFile;

{
    if (EditOpenFile(hwndEdit, szFile)) {

        lstrcpy(szMciFile,szFile);
        wsprintf(szBuffer,"%ls - %ls", (LPSTR)szAppName,
            (LPSTR)FileName(szMciFile));
        SetWindowText(hdlg, szBuffer);
    }
}


/*----------------------------------------------------------------------------*\
|   sendstring( hDlg, strBuffer )                                              |
|                                                                              |
|   Description:                                                               |
|       This function sends the string command specified in <strBuffer> to MCI |
|       via the MCI string interface. Any message returned by MCI is displayed |
|       in the 'MCI output' box. Any error which may have occurred is displayed|
|       in the 'Error' box'.                                                   |
|                                                                              |
|   Arguments:                                                                 |
|       hDlg            window handle of the main dialog window                |
|       strBuffer       pointer to the string containing the string command to |
|                        be executed                                           |
|   Returns:                                                                   |    
|       void                                                                   |
|                                                                              |
\*----------------------------------------------------------------------------*/

void sendstring(hDlg, strBuffer)

HWND hDlg;
PSTR strBuffer;

{
    char    strReturn[BUFFER_LENGTH];       /* string containing the message
                                                returned by MCI               */
    DWORD   dwErr;                          /* variable containing the return
                                                code from the MCI command     */

    /* Uncheck the notification buttons */

    CheckDlgButton (hDlg, ID_NOT_SUCCESS, FALSE);
    CheckDlgButton (hDlg, ID_NOT_SUPER, FALSE);
    CheckDlgButton (hDlg, ID_NOT_ABORT, FALSE);

    /* Send the string command to MCI */

    dwErr = mciSendString (strBuffer, strReturn, sizeof(strReturn), hDlg);

    /* Put the text message returned by MCI into the 'MCI Output' box */

    SetDlgItemText (hDlg, ID_OUTPUT, strReturn);

    /*
     * Decode the error # returned by MCI, and display the string in the
     * 'Error' box.
     *
     */

    mciGetErrorString (dwErr, strBuffer, BUFFER_LENGTH);
    SetDlgItemText (hDlg, ID_ERRORCODE, strBuffer);

    /* Update the internal list of currently open devices */

    update_device_list();
}


/*----------------------------------------------------------------------------*\
|   execute( hDlg, fStep )                                                     |
|                                                                              |
|   Description:                                                               |
|       This function executes the MCI command which is currently selected in  |
|       the edit box. If <fStep> is true, then only this one line will be      |
|       executed. Otherwise, every line from the currently selected line to    |
|       the last line in the edit box will be executed sequentially.           |
|                                                                              |
|   Arguments:                                                                 |
|       hDlg            window handle of the main dialog window                |
|       fStep           flag indicating whether or not to work in 'single step'|
|                        mode                                                  |
|   Returns:                                                                   |    
|       void                                                                   |
|                                                                              |
\*----------------------------------------------------------------------------*/

void execute(hdlg, fStep)

HWND hdlg;
BOOL fStep;

{
    int  iLine;             /* line # of the command currently being executed
                                in the edit box                               */
    int  n=0;               /* counter variable                               */

    /*
     * Go through this loop for every line in the edit box from the currently
     * selected line to the last line, or until we break out of the loop.
     *
     */

    for (iLine = EditGetCurLine(hwndEdit);
         EditGetLine(hwndEdit, iLine, szBuffer, sizeof(szBuffer));
         iLine++ )
    {

        /* If we hit a comment line or a blank line, skip to the next line */

        if (*szBuffer == ';' || *szBuffer == 0)
            continue;

        /* Select the line that is about to be processed */

        EditSelectLine(hwndEdit,iLine);

        /*
         * If we're in 'single step' mode and we've already processed one
         * line, then break out of the loop (and exit the routine).
         *
         */

        if (fStep && ++n == 2)
            break;

        /*
         * Send the command on the current line to MCI via the string
         * interface.
         *
         */
  
        sendstring(hdlg, szBuffer);
    }
}


/*----------------------------------------------------------------------------*\
|   devices( hDlg, uiMessage, wParam, lParam )                                 |
|                                                                              |
|   Description:                                                               |
|       This function handles messages belonging to the "List of open devices" |
|       dialog box. The only message that it looks for is WM_COMMAND,          |
|       indicating the user has pressed the "OK" button.  When this happens,   |
|       it takes down the dialog box.                                          |
|                                                                              |
|   Arguments:                                                                 |
|       hDlg            window handle of the Devices dialog window             |
|       uiMessage       message number                                         |
|       wParam          message-dependent parameter                            |
|       lParam          message-dependent parameter                            |
|                                                                              |
|   Returns:                                                                   |    
|       TRUE if the message has been processed, else FALSE                     |
|                                                                              |
\*----------------------------------------------------------------------------*/

BOOL FAR PASCAL _export devices(hdlg, msg, wParam, lParam)

HWND hdlg;
unsigned msg;
WORD wParam;
long lParam;

{
    switch (msg) {

        case WM_COMMAND:

            switch (wParam) {

                case ID_END_DEVICE_LIST:

                    hwndDevices = 0;
                    EndDialog(hdlg,TRUE);

                break;
            }

        break;
    }

    return FALSE;
}


/*----------------------------------------------------------------------------*\
|   get_number_of_devices()                                                    |
|                                                                              |
|   Description:                                                               |
|       This function sends a command to MCI querying it as to how many        |
|       are currently open in the system. It returns the value provided by MCI.|
|                                                                              |
|   Arguments:                                                                 |
|       none                                                                   |
|                                                                              |
|   Returns:                                                                   |    
|       The number of open devices in the system, or 0 if an error occurred.   |
|                                                                              |
\*----------------------------------------------------------------------------*/

int get_number_of_devices(void)

{
    MCI_SYSINFO_PARMS sysinfo;      /* Parameter structure used for getting
                                        information about the devices in the
                                        system                                */
    DWORD n;                        /* variable holding the count of open   
                                        devices                               */

    /*
     * Set things up so that MCI puts the number of open devices directly
     * into <n>.
     *
     */

    sysinfo.lpstrReturn = (LPSTR)(LPDWORD)&n;

    /*
     * Send the MCI a command querying all devices in the system to see if they
     * are open. If the command was successful, return the number provided by
     * MCI. Otherwise, return 0.
     *
     */

    if (mciSendCommand (MCI_ALLINSYSTEM_DEVICE_ID, MCI_SYSINFO,
                        MCI_SYSINFO_OPEN | MCI_SYSINFO_QUANTITY,
                        (DWORD)(LPMCI_SYSINFO_PARMS)&sysinfo) != 0)
        return 0;
    else
        return (int)n;
}


/*----------------------------------------------------------------------------*\
|   update_device_list()                                                       |
|                                                                              |
|   Description:                                                               |
|       This function updates the list of devices displayed in the Devices     |
|       dialog.                                                                |
|                                                                              |
|   Arguments:                                                                 |
|       none                                                                   |
|                                                                              |
|   Returns:                                                                   |    
|       void                                                                   |
|                                                                              |
\*----------------------------------------------------------------------------*/

void update_device_list(void)

{
    MCI_SYSINFO_PARMS sysinfo;      /* Parameter structure used for getting
                                          information about the devices in the
                                          system                              */
    HWND              hwndList;     /* handle to the Devices listbox window   */    
    char              strBuf[256];  /* string used for several things         */
    int               n, i, id;     /* miscellaneous variables                */    

    /* If the Devices dialog is not present, then return */
 
    if (hwndDevices == 0)
        return;

    /* Find out how many devices are currently open in the system */

    n = get_number_of_devices();

    /* Update the dialog caption appropriately */

    wsprintf (strBuf, "Open MCI Devices (count=%d)", n);
    SetWindowText (hwndDevices, strBuf);

    /* Get a handle to the dialog's listbox, and prepare it for updating */

    hwndList = GetDlgItem (hwndDevices, ID_DEVICE_LIST);
    SendMessage (hwndList, LB_RESETCONTENT, 0, 0L);
    SendMessage (hwndList, WM_SETREDRAW, FALSE, 0L);

    /*
     * Get the name of each open device in the system, one device at a time.
     * Add each device's name to the listbox.
     *
     */

    for (i = 1; i <= n; ++i) {

        sysinfo.dwNumber = i;
        sysinfo.lpstrReturn = (LPSTR)&strBuf;
 
        /* If we encounter an error, skip to the next device */

        if (mciSendCommand (MCI_ALLINSYSTEM_DEVICE_ID, MCI_SYSINFO,
                            MCI_SYSINFO_OPEN | MCI_SYSINFO_NAME,
                            (DWORD)(LPMCI_SYSINFO_PARMS)&sysinfo) != 0)
            continue;

        /* Redraw the list when all device names have been added */

        if (i == n)
            SendMessage (hwndList, WM_SETREDRAW, TRUE, 0L);

        /* Add the device name to the listbox */

        SendMessage (hwndList, LB_ADDSTRING, 0, (long)(LPSTR)strBuf);
    }

    /* Remember the number of open devices we found this time */

    nLastNumberOfDevices = n;
}


/*----------------------------------------------------------------------------*\
|   create_device_list()                                                       |
|                                                                              |
|   Description:                                                               |
|       This function creates the Devices dialog box and updates the list of   |
|       open devices displayed in it.                                          |
|                                                                              |
|   Arguments:                                                                 |
|       none                                                                   |
|                                                                              |
|   Returns:                                                                   |    
|       void                                                                   |
|                                                                              |
\*----------------------------------------------------------------------------*/

void create_device_list(void)

{
    FARPROC proc;                       /* used to create an instance of the
                                            Devices dialog proc               */

    /* Create the Devices dialog box */

    proc = MakeProcInstance ((FARPROC)devices, hInstApp);
    hwndDevices = CreateDialog (hInstApp, "devices", hwndMainDlg, proc);
    if (hwndDevices == NULL)
        return;

    /* Update the information displayed in the listbox */

    update_device_list();
}


/*----------------------------------------------------------------------------*\
|   mcitester( hDlg, uiMessage, wParam, lParam )                               |
|                                                                              |
|   Description:                                                               |
|       This function is the main message handler for MCI test. It handles     |
|       messages from the pushbuttons, radio buttons, edit controls, menu      |
|       system, etc. When it receives a WM_EXIT message, this routine tears    |
|       everything down and exits.                                             |
|                                                                              |
|   Arguments:                                                                 |
|       hDlg            window handle of the main dialog window                |
|       uiMessage       message number                                         |
|       wParam          message-dependent parameter                            |
|       lParam          message-dependent parameter                            |
|                                                                              |
|   Returns:                                                                   |    
|       TRUE if the message has been processed, else FALSE                     |
|                                                                              |
\*----------------------------------------------------------------------------*/

BOOL FAR PASCAL _export mcitester(hdlg, msg, wParam, lParam)

HWND hdlg;
unsigned msg;
WORD wParam;
long lParam;

{
    DWORD dw;                       /* return value from various messages     */
    WORD  f;                        /* return value from various messages     */
    BOOL  fSel;                     /* is something currently selected?       */
    WORD  wID;                      /* the type of notification required      */

    switch (msg) {

        case WM_COMMAND:

            switch (wParam) {

                case IDOK:

                    /*
                     * When the OK button gets pressed, insert a CR LF into
                     * the edit control. and execute the current line.
                     *
                     */

                    SetFocus(hwndEdit);
                    f = EditGetCurLine(hwndEdit);
                    execute(hdlg, TRUE);

                    EditSetCurLine(hwndEdit, f);

                    SendMessage(hwndEdit, WM_KEYDOWN, VK_END, 0L);
                    SendMessage(hwndEdit, WM_KEYUP, VK_END, 0L);
                    SendMessage(hwndEdit, EM_REPLACESEL, 0,(LONG)(LPSTR)"\r\n");
                    
                break;

                case ID_GO:

                    /*
                     * When the GO! button gets pressed, execute every line
                     * in the edit box starting with the first one.
                     *
                     */

                    EditSetCurLine(hwndEdit, 0);
                    execute(hdlg, FALSE);
                    
                break;

                case ID_STEP:

                    /*
                     * When the STEP button gets pressed, execute the currently
                     * selected line in the edit box.
                     *
                     */

                    execute(hdlg, TRUE);
                    
                break;

                case MENU_EXIT:
                case ID_EXIT:
                case IDCANCEL:

                    /*
                     * If the user indicates that he/she wishes to exit the
                     * application, then end the main dialog and post a WM_QUIT
                     * message.
                     *
                     */

                    EndDialog(hdlg,TRUE);
                    PostQuitMessage (0);
                    hwndMainDlg = 0;
                    
                break;

                case MENU_ABOUT:

                    /* Show the 'About...' box */

                    fDialog(ABOUTBOX,hdlg,(FARPROC)AppAbout);
                    
                break;

                case WM_CLEAR:
                case WM_CUT:
                case WM_COPY:
                case WM_PASTE:
                case WM_UNDO:

                    /* Pass whatever edit message we receive to the edit box */

                    SendMessage(hwndEdit,wParam,0,0L);
                    
                break;

                case MENU_OPEN:

                    /* Open a standard WINCOM 'File Open' dialog */

                    f = OpenFileDialog(hdlg, "MCI Test", "*.mci",
                        DLGOPEN_OPEN | DLGOPEN_MUSTEXIST | DLGOPEN_SEARCHPATH,
                        szBuffer, sizeof(szBuffer) );

                    /* If the user selected a valid file, then open it */

                    if (f == DLG_OKFILE)
                        OpenMciFile(hdlg, szBuffer);

                break;

                case MENU_SAVE:

                    /*
                     * If a filename exists, then save the contents of the edit
                     * box under that filename.
                     *
                     */

                    if (*szMciFile) {

                        EditSaveFile(hwndEdit, szMciFile);
                        break;
                    }

                break;

                case MENU_SAVEAS:

                    /*
                     * Open a standard WINCOM 'File Open' dialog to allow the
                     * user to spacify a filename to save under.
                     *
                     */

                    f = OpenFileDialog(hdlg, "MciTest", "*.mci",
                        DLGOPEN_SAVE | DLGOPEN_SEARCHPATH, szBuffer,
                        sizeof(szBuffer) );

                    /*
                     * If the user didn't hit Cancel, then he must have set a
                     * filename, so save the contents of the edit box under
                     * that filename.
                     *
                     */

                    if (f != DLG_CANCEL) {

                        EditSaveFile(hwndEdit, szBuffer);
                    }

                break;

                case MENU_DEVICES:

                    /*
                     * If the Devices dialog box doesn't already exist, then
                     * create and display it.
                     *
                     */

                    if (hwndDevices == 0)
                        create_device_list();

                break;
            }
        break;

        case WM_INITDIALOG:

            /* Do general initialization stuff */

            hwndEdit = GetDlgItem(hdlg,ID_INPUT);

            SetMenu (hdlg, LoadMenu(hInstApp,"MCIMENU"));
            SetClassWord (hdlg, GCW_HICON, LoadIcon (hInstApp, "MCITEST"));

            CheckDlgButton (hdlg, ID_NOT_SUCCESS, FALSE);
            CheckDlgButton (hdlg, ID_NOT_SUPER, FALSE);
            CheckDlgButton (hdlg, ID_NOT_ABORT, FALSE);

        return TRUE;

        case WM_DESTROY:

            /* End the dialog and send a WM_QUIT message */

            EndDialog(hdlg,TRUE);
            PostQuitMessage (0);
            hwndMainDlg = 0;
            
        break;

        case MM_MCINOTIFY:

            /*
             * Check the radio button corresponding to the notification
             * received.
             *
             */

            wID = NULL;
            switch (wParam) {

                case MCI_NOTIFY_SUCCESSFUL:

                    wID = ID_NOT_SUCCESS;
                
                break;

                case MCI_NOTIFY_SUPERSEDED:

                    wID = ID_NOT_SUPER;
                
                break;

                case MCI_NOTIFY_ABORTED:

                    wID = ID_NOT_ABORT;
                
                break;

                default:
                break;
            }

            if (wID) {

                CheckDlgButton (hdlg, wID, TRUE);
                SetFocus (GetDlgItem(hdlg, ID_INPUT));
            }

        break;

        case WM_INITMENUPOPUP:                   /* wParam is menu handle */

            /* Enable the 'Save' option if a valid filename exists */

            EnableMenuItem(wParam, MENU_SAVE,
                *szMciFile ? MF_ENABLED : MF_GRAYED);

            /* Find out if something is currently selected in the edit box */

            dw = SendMessage(hwndEdit,EM_GETSEL,0,0L);
            fSel = HIWORD(dw) != LOWORD(dw);

            /* Enable / disable the Edit menu options appropriately */

            EnableMenuItem (wParam, WM_UNDO ,
                SendMessage(hwndEdit,EM_CANUNDO,0,0L) ? MF_ENABLED : MF_GRAYED);
            EnableMenuItem (wParam, WM_CUT  , fSel ? MF_ENABLED : MF_GRAYED);
            EnableMenuItem (wParam, WM_COPY , fSel ? MF_ENABLED : MF_GRAYED);
            EnableMenuItem (wParam, WM_PASTE,
                IsClipboardFormatAvailable(CF_TEXT) ? MF_ENABLED : MF_GRAYED);
            EnableMenuItem (wParam, WM_CLEAR, fSel ? MF_ENABLED : MF_GRAYED);

        return 0L;
    }

    return FALSE;
}


/*----------------------------------------------------------------------------*\
|   AppInit( hInst, hPrev, sw, szCmdLine)                                      |
|                                                                              |
|   Description:                                                               |
|       This is called when the application is first loaded into memory. It    |
|       performs all initialization that doesn't need to be done once per      |
|       instance.                                                              |
|                                                                              |
|   Arguments:                                                                 |
|       hInstance       instance handle of current instance                    |
|       hPrev           instance handle of previous instance                   |
|       sw              not really used at all                                 |
|       szCmdLine       string containing the command line arguments           |
|                                                                              |
|   Returns:                                                                   |
|       TRUE if successful, FALSE if not                                       |
|                                                                              |
\*----------------------------------------------------------------------------*/

BOOL AppInit(hInst, hPrev, sw, szCmdLine)

HANDLE hInst;
HANDLE hPrev;
WORD sw;
LPSTR szCmdLine;

{
    /* Save the app's instance handle for use by the dialog boxes */

    hInstApp = hInst;

    /* Put up the main dialog box */

    hwndMainDlg = CreateDialog (hInst, "mcitester", NULL,
        MakeProcInstance ((FARPROC)mcitester, hInst));

    /* Fix up WIN.INI if this is the first time we are run... */

    if (!GetProfileString("extensions","mci","",szBuffer,sizeof(szBuffer)))
        WriteProfileString("extensions","mci","mcitest.exe ^.mci");

    /*
     * If a command line argument was specified, assume it to be a filename
     * and open that file.
     *
     */

    if (szCmdLine && *szCmdLine)
        OpenMciFile(hwndMainDlg,szCmdLine);

    return TRUE;
}


/*----------------------------------------------------------------------------*\
|   WinMain( hInst, hPrev, lpszCmdLine, sw )                                   |
|                                                                              |
|   Description:                                                               |
|       The main procedure for the app. After initializing, it just goes       |
|       into a message-processing loop until it gets a WM_QUIT message         |
|       (meaning the app was closed).                                          |
|                                                                              |
|   Arguments:                                                                 |
|       hInst           instance handle of this instance of the app            |
|       hPrev           instance handle of previous instance, NULL if first    |
|       szCmdLine       null-terminated command line string                    |
|       sw              specifies how the window is to be initially displayed  |
|                                                                              |
|   Returns:                                                                   |
|       The exit code as specified in the WM_QUIT message.                     |
|                                                                              |
\*----------------------------------------------------------------------------*/

int PASCAL WinMain(hInst, hPrev, szCmdLine, sw)

HANDLE hInst;
HANDLE hPrev;
LPSTR szCmdLine;
WORD sw;

{
    MSG     msg;                    /* Windows message structure */

    /* Call the initialization procedure */

    if (!AppInit(hInst,hPrev,sw,szCmdLine))
        return FALSE;


    /* Poll the event queue for messages */

    while (GetMessage(&msg,NULL,0,0))  {

        /*
         * If the Devices dialog is showing and the number of open devices has
         * changed since we last checked, then update the list of open devices.
         *
         */

        if (hwndDevices != 0 && get_number_of_devices() != nLastNumberOfDevices)
            update_device_list();

        /* Main message processing */

        if (!hwndMainDlg || !IsDialogMessage(hwndMainDlg, &msg)) {

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return msg.wParam;
}


/*----------------------------------------------------------------------------*\
|   fDialog(id,hwnd,fpfn)                                                      |
|                                                                              |
|   Description:                                                               |
|       This function displays a dialog box and returns the exit code.         |
|       The function passed will have a proc instance made for it.             |
|                                                                              |
|   Arguments:                                                                 |
|       id              resource ID of the dialog to be displayed              |
|       hwnd            parent window of the dialog                            |
|       fpfn            dialog's message function                              |
|                                                                              |
|   Returns:                                                                   |
|       exit code of the dialog (what was passed to EndDialog)                 |
|                                                                              |
\*----------------------------------------------------------------------------*/

BOOL fDialog(id, hwnd, fpfn)

int id;
HWND hwnd;
FARPROC fpfn;

{
    BOOL        f;                  /* The value returned by DialogBox        */
    HANDLE      hInst;              /* handle to the window instance          */

    /*
     * Get a handle to the window's instance, and use this to make a proc
     * instance.
     *
     */

    hInst = GetWindowWord(hwnd,GWW_HINSTANCE);
    fpfn  = MakeProcInstance(fpfn,hInst);

    /* Create the dialog box and remember the return code when it exits */

    f = DialogBox(hInst,MAKEINTRESOURCE(id),hwnd,fpfn);

    /* Free up resources used by the dialog, and return the return code */

    FreeProcInstance (fpfn);
    return f;
}


/*----------------------------------------------------------------------------*\
|   FileName(szPath)                                                           |
|                                                                              |
|   Description:                                                               |
|       This function takes the full path\filename string specified in <szPath>|
|       and returns a pointer to the first character of the filename in the    |
|       same string.                                                           |
|                                                                              |
|   Arguments:                                                                 |
|       szPath          pointer to the full path\filename string               |
|                                                                              |
|   Returns:                                                                   |
|       a pointer to the first character of the filename in the same string    |
|                                                                              |
\*----------------------------------------------------------------------------*/

PSTR FileName(szPath)

PSTR szPath;

{
    PSTR   sz;                      /* temporary pointer to the string        */

    /* Scan to the end of the string */

    for (sz=szPath; *sz; sz++)
        ;

    /*
     * Now start scanning back towards the beginning of the string until we hit
     * a slash (\) character, colon, or the start of the string.
     *
     */

    for (; sz>=szPath && !SLASH(*sz) && *sz!=':'; sz--)
        ;

    /* We're now pointing to the char. before the first char. in the filename */

    return ++sz;
}

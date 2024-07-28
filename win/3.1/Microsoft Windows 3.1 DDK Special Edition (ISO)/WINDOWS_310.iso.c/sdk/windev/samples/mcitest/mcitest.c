/* mcitest.c - WinMain(), main dialog box and support code for MCITest.
 *
 * MCITest is a Windows with Multimedia sample application illustrating
 * the use of the Media Control Interface (MCI). MCITest puts up a dialog
 * box allowing you to enter and execute MCI string commands.
 *
 *    (C) Copyright Microsoft Corp. 1991.  All rights reserved.
 *
 *    You have a royalty-free right to use, modify, reproduce and 
 *    distribute the Sample Files (and/or any modified version) in 
 *    any way you find useful, provided that you agree that 
 *    Microsoft has no warranty obligations or liability for any 
 *    Sample Application Files which are modified. 
 */

#include <windows.h>
#include <mmsystem.h>
#include "mcitest.h"
#include "edit.h"
#include "dlgopen.h"


#define BUFFER_LENGTH 128
#define SLASH(c)      ((c) == '/' || (c) == '\\')


/* Global variables.
 */
static  int         nLastNumberOfDevices;
static  HANDLE      hAccTable;
static  HANDLE      hInstApp;
static  HWND        hwndMainDlg;
static  HWND        hwndEdit;
static  HWND        hwndDevices;
static  FARPROC     lpfnDevices;
static  FARPROC     lpfnTester;
static  char        aszMciFile[BUFFER_LENGTH];
static  char        aszBuffer[BUFFER_LENGTH];
static  char        aszExt[] = "*.mcs";
static  char        aszAppName[] = "MCI Test";         
static  char        aszMainTextFormat[] = "%ls - %ls";
static  char        aszDeviceTextFormat[] = "Open MCI Devices(count=%d)";
static  char        aszNULL[] = "";
static  char        aszTRUE[] = "TRUE";
static  char        aszFALSE[] = "FALSE";
static  char        aszEOL[] = "\r\n";
static  char        aszOpenFileTitle[] = "Open MCITest File";
static  char        aszSaveFileTitle[] = "Save MCITest File";
static  char        aszSaveFileControl[] = "Save File &Name";
static  char        aszProfileSection[] = "extensions";
static  char        aszProfileKey[] = "mcs";
static  char        aszProfileSetting[] = "mcitest.exe ^.mcs";

/* AboutDlgProc - Dialog procedure function for ABOUTBOX.
 *
 * Params:  Standard window procedure parameters.
 */
BOOL FAR PASCAL AboutDlgProc(
    HWND    hwndDlg,
    unsigned	wMsg,
    WORD    wParam,
    LONG    lParam)
{
    switch (wMsg) {
        case WM_COMMAND:
            if (wParam == IDOK)
                EndDialog(hwndDlg, TRUE);
            break;
        case WM_INITDIALOG:
            return TRUE;
    }
    return FALSE;
}


/* FileName - Takes the full pathname string and returns a pointer to 
 *   the first character of the filename in the same string.
 *
 * Params:  szPath -  pointer to the full path\filename string
 *
 * Returns: a pointer to the first character of the filename in the string
 */
PSTR    PASCAL NEAR FileName(
    PSTR    szPath)
{
    PSTR   szCurrent;

    /* Scan to the end of the string */

    for (szCurrent = szPath; *szCurrent; szCurrent++)
        ;

    /*
     * Now start scanning back towards the beginning of the string until
     * a slash (\) character, colon, or start of the string is encountered.
     */
    for (; szCurrent >= szPath && !SLASH(*szCurrent) && *szCurrent != ':'; szCurrent--)
        ;

    /* Now pointing to the char before the first char in the filename.
     */
    return ++szCurrent;
}


/* OpenMciFile - Opens the specified MCI file and updates the main window 
 *   caption to display this file name along with the app name.
 *
 * Params:  hWndDlg - window handle of the main dialog window
 *          lszFile - pointer to the string containing the filename of
 *            file to be opened
 *
 * Returns: void
 */
void    PASCAL NEAR OpenMciFile(
    HWND    hwndDlg,
    LPSTR   lszFile)
{
    if (EditOpenFile(hwndEdit, lszFile)) {

        lstrcpy(aszMciFile, lszFile);
        wsprintf(aszBuffer, aszMainTextFormat, (LPSTR)aszAppName,
            (LPSTR)FileName(aszMciFile));
        SetWindowText(hwndDlg, aszBuffer);
    }
}


/* get_number_of_devices - Sends a command to MCI querying it as to how 
 *   many devices are currently open in the system. It returns the value 
 *   provided by MCI.
 *
 * Params:  void
 *
 * Returns: The number of MCI devices currently open.
 *
 */
int PASCAL NEAR get_number_of_devices(
    void)

{
    MCI_SYSINFO_PARMS sysinfo;
    DWORD dwDevices;

    /* Set things up so that MCI puts the number of open devices directly
     * into <nDevices>.
     */
    sysinfo.lpstrReturn = (LPSTR)(LPDWORD)&dwDevices;
    sysinfo.dwRetSize = sizeof(DWORD);

    /* Send MCI a command querying all devices in the system to see if they
     * are open. If the command was successful, return the number provided by
     * MCI. Otherwise, return 0.
     */
    if (mciSendCommand(MCI_ALL_DEVICE_ID, MCI_SYSINFO,
                        MCI_SYSINFO_OPEN | MCI_SYSINFO_QUANTITY,
                        (DWORD)(LPMCI_SYSINFO_PARMS)&sysinfo) != 0)
        return 0;
    else
        return (int)dwDevices;
}


/* update_device_list - Updates the list of devices displayed in the 
 *   Devices dialog.
 *
 * Params:  void
 *
 * Return:  void
 */
void    PASCAL NEAR update_device_list(
    void)

{
    MCI_SYSINFO_PARMS sysinfo;
    HWND              hwndList;
    char              aszBuf[256];
    int               nDevices;
    int               nCurrentDevice;

    /* If the Devices dialog is not present, then return.
     */
    if (hwndDevices == NULL)
        return;

    /* Find out how many devices are currently open in the system.
     */
    nDevices = get_number_of_devices();

    /* Update the dialog caption appropriately.
     */
    wsprintf(aszBuf, aszDeviceTextFormat, nDevices);
    SetWindowText(hwndDevices, aszBuf);

    /* Get a handle to the dialog's listbox, and prepare it for updating.
     */
    hwndList = GetDlgItem(hwndDevices, ID_DEVICE_LIST);
    SendMessage(hwndList, LB_RESETCONTENT, 0, 0L);
    SendMessage(hwndList, WM_SETREDRAW, FALSE, 0L);

    /* Get the name of each open device in the system, one device at a time.
     * Add each device's name to the listbox.
     */
    for (nCurrentDevice = 1; nCurrentDevice <= nDevices; ++nCurrentDevice) {

        sysinfo.dwNumber = nCurrentDevice;
        sysinfo.lpstrReturn = (LPSTR)&aszBuf;
    sysinfo.dwRetSize = sizeof(aszBuf);
 
        /* If an error is encountered, skip to the next device.
         */
        if (mciSendCommand(MCI_ALL_DEVICE_ID, MCI_SYSINFO,
                            MCI_SYSINFO_OPEN | MCI_SYSINFO_NAME,
                            (DWORD)(LPMCI_SYSINFO_PARMS)&sysinfo) != 0)
            continue;

        /* Redraw the list when all device names have been added.
         */
        if (nCurrentDevice == nDevices)
            SendMessage(hwndList, WM_SETREDRAW, TRUE, 0L);

        /* Add the device name to the listbox.
         */
        SendMessage(hwndList, LB_ADDSTRING, 0, (LONG)(LPSTR)aszBuf);
    }

    /* Remember the number of open devices found this time.
     */
    nLastNumberOfDevices = nDevices;
}


/* sendstring - Sends the specified string command to MCI via the MCI 
 *   string interface. Any message returned by MCI is displayed
 *   in the 'MCI output' box. Any error which may have occurred is 
 *   displayed in the 'Error' box'.
 *
 * Params:  hwndDlg - window handle of the main dialog window
 *          szBuffer - pointer to the string containing the string 
 *             command to be executed
 *
 * Returns: MCI error number.
 */
DWORD   PASCAL NEAR sendstring(
    HWND    hwndDlg,
    PSTR    strBuffer)
{
    char    aszReturn[BUFFER_LENGTH];
    DWORD   dwErr;

    /* Uncheck the notification buttons.
     */
    CheckDlgButton(hwndDlg, ID_NOT_SUCCESS, FALSE);
    CheckDlgButton(hwndDlg, ID_NOT_SUPER, FALSE);
    CheckDlgButton(hwndDlg, ID_NOT_ABORT, FALSE);
    CheckDlgButton(hwndDlg, ID_NOT_FAIL, FALSE);

        /* Send the string command to MCI.
         */
        dwErr = mciSendString(strBuffer, aszReturn, sizeof(aszReturn), hwndDlg);

        /* Put the text message returned by MCI into the 'MCI Output' box.
         */
        SetDlgItemText(hwndDlg, ID_OUTPUT, aszReturn);

        /* Decode the error # returned by MCI, and display the string in the
         * 'Error' box.
         */
        mciGetErrorString(dwErr, strBuffer, BUFFER_LENGTH);
        SetDlgItemText(hwndDlg, ID_ERRORCODE, strBuffer);

    /* Update the internal list of currently open devices.
     */
    update_device_list();
    return dwErr;
}


/* NDialog - Displays a dialog box and returns the exit code.
 *   The function passed will have a proc instance made for it.
 *
 * Params:  id - resource ID of the dialog to be displayed
 *          hwnd - parent window of the dialog
 *          fpfn - dialog's message function
 *
 * Returns: exit code of the dialog (what was passed to EndDialog)
 */
int PASCAL NEAR NDialog(
    int     id,
    HWND    hwnd,
    FARPROC lpfnDialog)
{
    int         nDialogReturn;
    HANDLE      hInst;

    /* Get a handle to the window's instance, and use this to make a proc
     * instance.
     */
    hInst = (HANDLE)GetWindowWord(hwnd, GWW_HINSTANCE);
    lpfnDialog = MakeProcInstance(lpfnDialog, hInst);

    /* Create the dialog box and remember the return code when it exits.
     */
    nDialogReturn = DialogBox(hInst, MAKEINTRESOURCE(id), hwnd, (DLGPROC)lpfnDialog);

    /* Free up resources used by the dialog, and return the return code.
     */
    FreeProcInstance(lpfnDialog);
    return nDialogReturn;
}


/* ErrDlgFunc - Callback function for the dialog box which occurs during 
 *  the execution of an error in a loop of MCITEST commands. It displays 
 *  Abort, Continue and Ignore buttons.
 *
 * Params:  standard window procedure parameters.
 */
BOOL FAR PASCAL ErrDlgFunc(
    HWND    hwndDlg,
    WORD    wMsg,
    WORD    wParam,
    LONG    lParam) 
{
    switch(wMsg) {

    case WM_INITDIALOG:
        return TRUE;

    case WM_COMMAND:
        switch(wParam) {            // button pushed
        case IDABORT:
        case IDOK:                      
        case IDIGNORE:
            EndDialog(hwndDlg, wParam); // just return button ID
            break;
        }
        break;
    }
    return FALSE;
}


/* execute - Executes the MCI command which is currently selected in
 *       the edit box. If <fStep> is true, then only this one line will be
 *       executed. Otherwise, every line from the currently selected line to
 *       the last line in the edit box will be executed sequentially.
 *
 * Params:  hwndDlg - window handle of the main dialog window
 *          fSingleStep - flag indicating whether or not to work in 
 *            'single step' mode
 *
 * Returns: void
 */
void    PASCAL NEAR execute(
    HWND    hwndDlg,
    BOOL    fSingleStep)
{
    int  iLine;                                
    int  n = 0;
    int  runcount;
    int  count;
    int  iLineStart;
    BOOL fIgnoreErrors = FALSE;

    runcount = GetDlgItemInt(hwndDlg, ID_RUNCOUNT, NULL, TRUE);

    /* Go through this loop for every line in the edit box from the currently
     * selected line to the last line, or a single line if in single step mode
     */
    iLineStart = EditGetCurLine(hwndEdit);
    for (count = runcount; count--;) {
        for (iLine = iLineStart;
            EditGetLine(hwndEdit, iLine, aszBuffer, sizeof(aszBuffer));
            iLine++) {

            /* If a comment line or a blank line, skip to the next line.
             */
            if (*aszBuffer == ';' || *aszBuffer == 0)
                continue;

            /* Select the line that is about to be processed.
             */
            EditSelectLine(hwndEdit, iLine);

            /* If in 'single step' mode and one line has already been 
             * proccessed, then break out of the loop (and exit the routine).
             */
            if (fSingleStep && (++n == 2))
                break;

            /* Send the command on the current line to MCI via the string
             * interface.
             */
            if (sendstring(hwndDlg, aszBuffer) && !fIgnoreErrors &&
                runcount > 1 && !fSingleStep) {
                int nRet;

                nRet = NDialog(IDD_ERRORDLG, hwndDlg, (FARPROC)ErrDlgFunc);
                if (nRet == IDABORT)
                    goto exit_fn;
                if (nRet == IDIGNORE)
                    fIgnoreErrors = TRUE;
            }
        }
        SetDlgItemInt(hwndDlg, ID_RUNCOUNT, count, TRUE);
        if (fSingleStep)
            break;
    }
exit_fn:
    SetDlgItemInt(hwndDlg, ID_RUNCOUNT, runcount, TRUE);
}


/* devices - This function handles messages belonging to the 
 *   "List of open devices" dialog box. The only message that it looks for 
 *    is WM_COMMAND, indicating the user has pressed the "OK" button.  
 *    When this happens, it takes down the dialog box.
 *
 * Params:  standard window procedure parameters
 *
 * Return:  TRUE if the message has been processed, else FALSE.
 */
BOOL FAR PASCAL devices(
    HWND    hwndDlg,
    WORD    wMsg,
    WORD    wParam,
    LONG    lParam)
{
    switch (wMsg) {

        case WM_COMMAND:

            switch (wParam) {

                case ID_END_DEVICE_LIST:

                    hwndDevices = NULL;
                    EndDialog(hwndDlg, TRUE);

                break;
            }

        break;
    }

    return FALSE;
}


/* create_device_list - Creates the Devices dialog box and updates 
 *   the list of open devices displayed in it.
 *
 * Params:  void
 *
 * Returns: void
 */
void    PASCAL NEAR create_device_list(
    void)

{
    /* Create the Devices dialog box.
     */
    if (lpfnDevices == NULL)
        lpfnDevices = MakeProcInstance((FARPROC)devices, hInstApp);
    hwndDevices = CreateDialog(hInstApp, MAKEINTRESOURCE(IDD_DEVICES),
        hwndMainDlg, (DLGPROC)lpfnDevices);
    if (hwndDevices == NULL)
        return;

    /* Update the information displayed in the listbox.
     */
    update_device_list();
}


/* mcitester - This function is the main message handler for MCI test.
 *  It handles messages from the pushbuttons, radio buttons, edit controls, 
 *  menu system, etc. When it receives a WM_EXIT message, this routine tears
 *  everything down and exits.
 *
 * Params:  standard window procedure paramters
 *
 * Returns: TRUE if the message has been processed, else FALSE
 */
BOOL FAR PASCAL mcitester(
    HWND    hwndDlg,
    WORD    wMsg,
    WORD    wParam,
    LONG    lParam)
{
    DWORD dw;
    WORD  f;
    BOOL  fSel;
    WORD  wID;

    switch (wMsg) {

        case WM_COMMAND:

            switch (wParam) {

                case IDOK:

                    /* When the OK button gets pressed, insert a CR LF into
                     * the edit control. and execute the current line.
                     */
                    SetFocus(hwndEdit);
                    f = EditGetCurLine(hwndEdit);
                    execute(hwndDlg, TRUE);

                    EditSetCurLine(hwndEdit, f);

                    SendMessage(hwndEdit, WM_KEYDOWN, VK_END, 0L);
                    SendMessage(hwndEdit, WM_KEYUP, VK_END, 0L);
                    SendMessage(hwndEdit, EM_REPLACESEL, 0, (LONG)(LPSTR)aszEOL);

                break;

                case ID_GO:

                    /* When the GO! button gets pressed, execute every line
                     * in the edit box starting with the first one.
                     */
                    EditSetCurLine(hwndEdit, 0);
                    execute(hwndDlg, FALSE);
                    
                break;

                case ID_STEP:

                    /* When the STEP button gets pressed, execute the 
                     * currently selected line in the edit box.
                     */
                    execute(hwndDlg, TRUE);
                    
                break;

                case MENU_EXIT:
                case ID_EXIT:
                case IDCANCEL:

                    /* If the user indicates that he/she wishes to exit the
                     * application, then end the main dialog and post a
                     * WM_QUIT message.
                     */
                    EndDialog(hwndDlg, TRUE);
                    PostQuitMessage(0);
                    hwndMainDlg = 0;
                    
                break;

                case MENU_ABOUT:

                    /* Show the 'About...' box.
                     */
                    NDialog(IDD_ABOUTBOX, hwndDlg, (FARPROC)AboutDlgProc);

                break;

                case WM_CLEAR:
                case WM_CUT:
                case WM_COPY:
                case WM_PASTE:
                case WM_UNDO:

                    /* Pass whatever edit message received to the edit box.
                     */
                    SendMessage(hwndEdit, wParam, 0, 0L);
                    
                break;

                case MENU_OPEN:

                    f = OpenFileDialog(hwndDlg, aszOpenFileTitle, aszExt,
                        DLGOPEN_MUSTEXIST | OF_EXIST | OF_READ, NULL,
                        aszBuffer, sizeof(aszBuffer));

                    /* If the user selected a valid file, then open it */

                    if ((int)f >= 0)
                        OpenMciFile(hwndDlg, aszBuffer);

                break;

                case MENU_SAVE:

                    /* If a filename exists, then save the contents of the
                     * edit box under that filename.
                     */
                    if (*aszMciFile) {

                        EditSaveFile(hwndEdit, aszMciFile);
                        break;
                    }

                break;

                case MENU_SAVEAS:

                    /* Open a 'File Open' dialog to allow the
                     * user to spacify a filename to save under.
                     */

                    *aszBuffer = (char)0;
                    f = OpenFileDialog(hwndDlg, aszSaveFileTitle, aszExt,
                        DLGOPEN_SAVE | OF_EXIST, aszSaveFileControl, aszBuffer,
                        sizeof(aszBuffer));

                    /* If the user didn't hit Cancel, then he must have set a
                     * filename, so save the contents of the edit box under
                     * that filename.
                     */
                    if (f != DLGOPEN_CANCEL) {

                        EditSaveFile(hwndEdit, aszBuffer);
                    }

                break;

                case MENU_DEVICES:

                    /* If the Devices dialog box doesn't already exist, then
                     * create and display it.
                     */
                    if (hwndDevices == NULL)
                        create_device_list();

                break;
            }
        break;

        case WM_INITDIALOG:

            /* Do general initialization stuff.
             */
            hwndEdit = GetDlgItem(hwndDlg, ID_INPUT);

            SetMenu(hwndDlg, LoadMenu(hInstApp, MAKEINTRESOURCE(IDM_MCITEST)));

            CheckDlgButton(hwndDlg, ID_NOT_SUCCESS, FALSE);
            CheckDlgButton(hwndDlg, ID_NOT_SUPER, FALSE);
            CheckDlgButton(hwndDlg, ID_NOT_ABORT, FALSE);
            CheckDlgButton(hwndDlg, ID_NOT_FAIL, FALSE);
            SetDlgItemInt(hwndDlg, ID_RUNCOUNT, 1, TRUE);

            hAccTable = LoadAccelerators(hInstApp,
                MAKEINTRESOURCE(IDA_MCITEST));

        return TRUE;

        case WM_DESTROY:

            /* End the dialog and send a WM_QUIT message.
             */
            EndDialog(hwndDlg, TRUE);
            PostQuitMessage(0);
            hwndMainDlg = 0;
            
        break;

        case MM_MCINOTIFY:

            /* Check the radio button corresponding to the notification
             * received.
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

                case MCI_NOTIFY_FAILURE:

                    wID = ID_NOT_FAIL;
                
                break;

                default:
                break;
            }

            if (wID) {

                CheckDlgButton(hwndDlg, wID, TRUE);
                SetFocus(GetDlgItem(hwndDlg, ID_INPUT));
            }

        break;

        case WM_INITMENUPOPUP:

            /* Enable the 'Save' option if a valid filename exists.
             */
            EnableMenuItem((HMENU)wParam, MENU_SAVE,
                *aszMciFile ? MF_ENABLED : MF_GRAYED);

            /* Find out if something is currently selected in the edit box.
             */
            dw = SendMessage(hwndEdit, EM_GETSEL, 0, 0L);
            fSel = HIWORD(dw) != LOWORD(dw);

            /* Enable / disable the Edit menu options appropriately.
             */
            EnableMenuItem((HMENU)wParam, WM_UNDO,
                SendMessage(hwndEdit,EM_CANUNDO,0,0L) ? MF_ENABLED : MF_GRAYED);
            EnableMenuItem((HMENU)wParam, WM_CUT, fSel ? MF_ENABLED : MF_GRAYED);
            EnableMenuItem((HMENU)wParam, WM_COPY, fSel ? MF_ENABLED : MF_GRAYED);
            EnableMenuItem((HMENU)wParam, WM_PASTE,
                IsClipboardFormatAvailable(CF_TEXT) ? MF_ENABLED : MF_GRAYED);
            EnableMenuItem((HMENU)wParam, WM_CLEAR, fSel ? MF_ENABLED : MF_GRAYED);

        return 0L;
    }

    return FALSE;
}


/* AppInit - This is called when the application is first loaded into memory.
 *   It performs all initialization that doesn't need to be done once per
 *   instance.
 *
 * Params:  hInstance - instance handle of current instance
 *          hPrev - instance handle of previous instance
 *          szCmdLine - string containing the command line arguments
 *          nCmdShow - not really used at all
 *
 * Returns: TRUE if successful, FALSE if not
 *
 */
BOOL    PASCAL NEAR AppInit(
    HANDLE  hInst,
    HANDLE  hPrev,
    LPSTR   lszCmdLine,
    WORD    nCmdShow)

{
    /* Save the app's instance handle for use by the dialog boxes.
     */
    hInstApp = hInst;

    /* Put up the main dialog box.
     */
    lpfnTester = MakeProcInstance((FARPROC)mcitester, hInst);
    hwndMainDlg = CreateDialog(hInst, MAKEINTRESOURCE(IDD_MCITEST), NULL,
        (DLGPROC)lpfnTester);

    /* Fix up WIN.INI if this is the first time run... 
     */
    if (!GetProfileString(aszProfileSection, aszProfileKey, aszNULL, aszBuffer, sizeof(aszBuffer)))
        WriteProfileString(aszProfileSection, aszProfileKey, aszProfileSetting);

    /* If a command line argument was specified, assume it to be a filename
     * and open that file.
     */
    if (lszCmdLine && *lszCmdLine)
        OpenMciFile(hwndMainDlg, lszCmdLine);

    return TRUE;
}


/* WinMain - Entry point from Windows
 *
 * Params:  hInstance - instance handle of current instance
 *          hPrev - instance handle of previous instance
 *          szCmdLine - string containing the command line arguments
 *          nCmdShow - not really used at all
 *
 * Returns: The exit code as specified in the WM_QUIT message.
 */
int PASCAL WinMain(
    HINSTANCE  hInst,
    HINSTANCE  hPrev,
    LPSTR   lszCmdLine,
    int     nCmdShow)
{
    MSG     msg;

    /* Call the initialization procedure.
     */
    if (!AppInit(hInst, hPrev, lszCmdLine, nCmdShow))
        return FALSE;


    /* Poll the event queue for messages.
     */
    while (GetMessage(&msg, NULL, 0, 0))  {

        /* If the Devices dialog is showing and the number of open devices has
         * changed since last checked, then update the list of open devices.
         */

        if (hwndDevices != 0 && get_number_of_devices() != nLastNumberOfDevices)
            update_device_list();

        /* Main message processing.
         */
        if (hwndMainDlg &&
            (TranslateAccelerator(hwndMainDlg, hAccTable, &msg) ||
            IsDialogMessage(hwndMainDlg, &msg)))
            continue;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (lpfnDevices != NULL)
        FreeProcInstance(lpfnDevices);
    FreeProcInstance(lpfnTester);
    return msg.wParam;
}

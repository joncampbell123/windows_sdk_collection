// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 1993-1995  Microsoft Corporation.  All Rights Reserved.
//
// MODULE:  OWNCOMBO.C
//
// PURPOSE:  Illustrates the use of functions and messages
//           for comboboxes and ownder-draw control styles
//
// PLATFORMS:  Windows 95, NT, Win32s
//
// FUNCTIONS:
//    WinMain()              - Calls initialization function, processes
//                               message loop
//    OwnComboInit           - Registers the main window class
//    About()                - Process messages for "About" dialog box
//    OwnComboWndProc()      - Processes messages for the main wndProc;
//                               handles menu selections and others
//    DrawEntireItem()       - Handles the drawing of a listbox
//                               or combobox item.
//    HandleSelectionState() - Handles the selecting/deselecting
//                               of a list box or combobox item.
//    HandleFocusState()     - Handles the getting/losing of the
//                               input focus by a list box
//    ListBoxExample()       - Dialog function for the owner-draw listbox
//                               example.
//    ComboBoxExample()      - Dialog function for the text combobox dialog.
//    OwnerComboBoxExample() - Dialog function for the drop-down
//                               ownerdraw comboBox.
//
// SPECIAL INSTRUCTIONS: N/A
//

#include "windows.h"
#include "owncombo.h"

HANDLE  hInst;


//  FUNCTION   : WinMain(HANDLE, HANDLE, LPSTR, int)
//
//  PURPOSE    : Creates the app. window and enters the message loop.
//
//  PARAMETERS :
//      hInstance     - Integer which uniquely identifies this instance
//                        of the application
//      hPrevInstance - Integer identifier of the previous instance of
//                        the application (hPrevInstance == NULL if no
//                        such previous instance exists)
//      lpCmdLine     - Long pointer to command line info
//      nCmdShow      - Integer value specifying how to start application

int APIENTRY WinMain(HANDLE hInstance, HANDLE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow)
{
    HWND  hWnd;
    MSG   msg;

    UNREFERENCED_PARAMETER( lpCmdLine );

    if (!hPrevInstance)
        if (!OwnComboInit (hInstance))
            return (0);

    hInst = hInstance;

    // Create the application's main window
    hWnd = CreateWindow ("owncombo",
                         "Owner-draw & Combo Box Example",
                         WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT,
                         CW_USEDEFAULT,
                         CW_USEDEFAULT,
                         CW_USEDEFAULT,
                         (HWND) NULL,
                         NULL,
                         hInstance,
                         (LPSTR) NULL);

    if (!hWnd)
        return (0);

    ShowWindow (hWnd, nCmdShow);

    while (GetMessage (&msg, NULL, 0, 0)) {
        TranslateMessage (&msg);
        DispatchMessage (&msg);
    }

    return(msg.wParam);
}


//  FUNCTION   : OwnComboInit (hInstance)
//
//  PURPOSE    : Registers the main window class.
//
//  PARAMETERS :
//      hInstance - Integer which uniquely identifies this instance
//                  of the application
//  RETURNS    :
//      TRUE  - if RegisterClass () succeeds.
//      FALSE - if RegisterClass () fails.

BOOL NEAR PASCAL OwnComboInit (HANDLE hInstance)
{
    HANDLE     hMemory;
    PWNDCLASS  pWndClass;
    BOOL       bSuccess;

    // Allocate for, and fill class structure.
    hMemory = LocalAlloc (LPTR, sizeof (WNDCLASS));
    if(!hMemory) {
        MessageBox(NULL, "<OwnComboInit> Not enough memory.", NULL,
                   MB_OK | MB_ICONHAND);
        return(FALSE);
    }

    pWndClass = (PWNDCLASS) LocalLock (hMemory);

    pWndClass->style         = 0;
    pWndClass->lpfnWndProc   = (WNDPROC) OwnComboWndProc;
    pWndClass->hInstance     = hInstance;
    pWndClass->hIcon         = LoadIcon (hInstance, "owncombo");
    pWndClass->hCursor       = LoadCursor (NULL, IDC_ARROW);
    pWndClass->hbrBackground = GetStockObject (WHITE_BRUSH);
    pWndClass->lpszMenuName  = (LPSTR) "OwnComboMenu",
    pWndClass->lpszClassName = (LPSTR) "owncombo";

    bSuccess = RegisterClass (pWndClass);

    LocalUnlock (hMemory);
    LocalFree (hMemory);
    return (bSuccess);
}

//  FUNCTION   : About (hDlg,message, wParam, lParam)
//
//  PURPOSE    : Processes messages for "About" dialog box.
//
//  PARAMETERS :
//      hDlg    - handle of the dialog box window
//      message - message currently to be processed
//      wParam  - additional info associated with the message
//      lParam  - additional DWORD info associated with the message
//

BOOL APIENTRY About (HWND hDlg, UINT message, UINT wParam, LONG lParam)
{

    UNREFERENCED_PARAMETER(lParam);

    switch (message) {
    case WM_INITDIALOG:
        return(TRUE);

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog (hDlg,0);
            return(FALSE);
        }
        break;

    default:
        break;
    }
    return(FALSE);
}


//  FUNCTION   : OwnComboWndProc (hWnd,message, wParam, lParam)
//
//  PURPOSE    : Processes messages for "About" dialog box.
//
//  PARAMETERS :
//      hDlg    - handle of the dialog box window
//      message - message currently to be processed
//      wParam  - additional info associated with the message
//      lParam  - additional DWORD info associated with the message
//
//  RETURNS    :
//      TRUE  -
//      FALSE -

LONG APIENTRY OwnComboWndProc (HWND hWnd, UINT message, UINT wParam,
                               LONG lParam)
{
    FARPROC lpProc;

    switch (message) {
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDM_EXIT:
                    DestroyWindow (hWnd);
                    break;

                case IDM_ABOUT:
                    // Bring up the about box
                    lpProc = MakeProcInstance ((FARPROC)About, hInst);
                    DialogBox (hInst,"AboutBox", hWnd, lpProc);
                    FreeProcInstance (lpProc);
                    break;

                case IDM_LISTBOX:
                    // Bring up the list box example */
                    lpProc = MakeProcInstance ((FARPROC)ListBoxExample,
                                               hInst);
                    DialogBox (hInst,"ListBoxDialog",
                               hWnd, lpProc);
                    FreeProcInstance (lpProc);
                    break;

                case IDM_MULTILISTBOX:
                    // Bring up the multiple selection list box example
                    lpProc = MakeProcInstance ((FARPROC)ListBoxExample, hInst);
                    DialogBox (hInst, "MultiListBoxDialog",
                               hWnd, lpProc);
                    FreeProcInstance (lpProc);
                    break;

                case IDM_COMBOBOX:
                    // Bring up the combo box example
                    lpProc = MakeProcInstance ((FARPROC)ComboBoxExample,
                                               hInst);
                    DialogBox (hInst, "ComboBoxDialog", hWnd, lpProc);
                    FreeProcInstance (lpProc);
                    break;

                case IDM_OWNERCOMBOBOX:
                    // Bring up the owner-draw dropdown list box example
                    lpProc = MakeProcInstance ((FARPROC)OwnerComboBoxExample,
                                               hInst);
                    DialogBox (hInst, "OwnerComboBoxDialog", hWnd, lpProc);
                    FreeProcInstance (lpProc);
                    break;
        }
        break;

        case WM_DESTROY:
            PostQuitMessage (0);
            break;

        default:
            return(DefWindowProc(hWnd, message, wParam, lParam));
    }
    return(0);
}

//  FUNCTION   : HandleSelectionState (LPDRAWITEMSTRUCT, int)
//
//  PURPOSE    : Handles a change in an item selection state. If an item is
//               selected, a black rectangular frame is drawn around that
//               item; if an item is de-selected, the frame is removed.
//
//  PARAMETERS :
//      lpdis  - long pointer to a DRAWITEMSTRUCT
//      inflate- used in the InflateRect() call
//
//  COMMENTS   : The black selection frame is slightly larger than the gray
//               focus frame so they won't paint over each other.

VOID APIENTRY HandleSelectionState (LPDRAWITEMSTRUCT lpdis,int inflate)
{

    RECT rc;
    HBRUSH  hbr;

    // Resize rectangle to place selection frame outside of the focus
    //   frame and the item.

   CopyRect ((LPRECT)&rc, (LPRECT)&lpdis->rcItem);
   InflateRect ((LPRECT)&rc, inflate, inflate);

   if (lpdis->itemState & ODS_SELECTED) {
      hbr = GetStockObject(BLACK_BRUSH);  // selecting item --
                                          // paint a black frame
   }
   else {
      hbr = CreateSolidBrush(GetSysColor(COLOR_WINDOW)); // de-selecting item
                                                         // -- remove frame
   }

   FrameRect(lpdis->hDC, (LPRECT)&rc, hbr);
   DeleteObject (hbr);
}

//  FUNCTION   : HandleSelectionState (LPDRAWITEMSTRUCT, int)
//
//  PURPOSE    : Handle a change in item focus state. If an item gains the
//               input focus, a gray rectangular frame is drawn around that
//               item; if an item loses the input focus, the gray frame is
//               removed.
//  PARAMETERS :
//      lpdis  - long pointer to a DRAWITEMSTRUCT
//      inflate- used in the InflateRect() call
//
//  COMMENT    : The gray focus frame is slightly smaller than the black
//               selection frame so they won't paint over each other.
//
//  RETURNS:
//      None.

VOID APIENTRY HandleFocusState(LPDRAWITEMSTRUCT lpdis, INT inflate)
{
    RECT rc;
    HBRUSH hbr;

    // Resize rectangle to place focus frame between the selection
    // frame and the item.

   CopyRect ((LPRECT)&rc, (LPRECT)&lpdis->rcItem);
   InflateRect ((LPRECT)&rc, inflate, inflate);

   if (lpdis->itemState & ODS_FOCUS) {
      hbr = GetStockObject(GRAY_BRUSH); // gaining input focus --
                                        // paint a gray frame
   }
   else  {
      hbr = CreateSolidBrush(GetSysColor(COLOR_WINDOW)); // losing input focus
                                                  // remove (paint over) frame
   }
   FrameRect(lpdis->hDC, (LPRECT)&rc, hbr);
   DeleteObject (hbr);
}

//  FUNCTION   : DrawEntireItem(LPDRAWITEMSTRUCT, int)
//
//  PURPOSE    : Draws an item and frames it with a selection frame and/or
//               a focus frame when appropriate.
//
//  PARAMETERS :
//      lpdis  - long pointer to a DRAWITEMSTRUCT
//      inflate- used in the InflateRect() call
//
//  RETURNS:
//      None.

VOID APIENTRY DrawEntireItem (LPDRAWITEMSTRUCT lpdis, INT inflate)
{
    RECT rc;
    HBRUSH hbr;

    // Resize rectangle to leave space for frames
    CopyRect ((LPRECT)&rc, (LPRECT)&lpdis->rcItem);
    InflateRect ((LPRECT)&rc, inflate, inflate);

    // Create a brush using the value in the item data field (this value
    //   was initialized when we added the item to the list/combo box using
    //   LB_ADDSTRING/CB_ADDSTRING) and draw the color in the list/combo box.

    hbr = CreateSolidBrush (lpdis->itemData);
    FillRect (lpdis->hDC, (LPRECT)&rc, hbr);
    DeleteObject (hbr);

    // Draw or erase appropriate frames
    HandleSelectionState(lpdis, inflate + 4);
    HandleFocusState(lpdis, inflate + 2);
}

//  FUNCTION   : ListBoxExample (hDlg, message, wParam, lParam)
//
//  PURPOSE    :
//     Dialog function for the owner-draw list box example.
//     It sets up the example dialog with the owner-draw list box,
//     adds the colors to the list box, and handles setting the
//     selection and focus for the items.
//
//  PARAMETERS :
//      hDlg    - handle of the dialog box window
//      message - message currently to be processed
//      wParam  - additional info associated with the message
//      lParam  - additional DWORD info associated with the message
//
//  RETURNS :
//      TRUE    - message was processed,and does not get passed
//                on to the Dialog Manager
//      FALSE   - message was processed, and is passed on to DlgManager
//                for default processing.

BOOL APIENTRY ListBoxExample (HWND hDlg, UINT message, UINT wParam,
                              LONG lParam)
{
    // Vars for WM_DRAWITEM
    LPDRAWITEMSTRUCT lpdis;
    LPMEASUREITEMSTRUCT lpmis;

    switch (message) {
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDOK:
                    EndDialog (hDlg, 0);
                    return (TRUE);
                    break;

                // Clicking any of these buttons adds the corresponding color
                // to the list box. The application-supplied data is the RGB
                // value for the color to be drawn in the listbox.

                case ID_BLACK:
                    SendMessage (GetDlgItem (hDlg, ID_LISTBOX),
                                 LB_ADDSTRING, 0, RGB (0,0,0));
                    return(TRUE);
                    break;

                case ID_RED:
                    SendMessage (GetDlgItem (hDlg, ID_LISTBOX),
                                 LB_ADDSTRING, 0, RGB (255,0,0));
                    return(TRUE);
                    break;

                case ID_BLUE:
                    SendMessage (GetDlgItem (hDlg, ID_LISTBOX),
                                 LB_ADDSTRING, 0, RGB (0,0,255));
                    return(TRUE);
                    break;

                case ID_GREEN:
                    SendMessage (GetDlgItem (hDlg, ID_LISTBOX),
                                 LB_ADDSTRING, 0, RGB (0,255,0));
                    return(TRUE);
                    break;

                default:
                    return(FALSE);
                    break;
            }
        case WM_DRAWITEM:
            // Get pointer to the DRAWITEMSTRUCT
            lpdis = (LPDRAWITEMSTRUCT)lParam;
            if (lpdis->itemID == -1) {

                // -1 indicates no list box items. This is sent
                //   when the user TABS into an empty list box or
                //   an empty list box gets the focus. We have to
                //   indicate (somehow) that this owner-draw list box has
                //   the focus. We do it in response to this message.
                //   Note that lpdis->itemData field would be invalid
                //   in this instance so we can't allow it to fall
                // into our standard routines.

                HandleFocusState(lpdis, -5);
            }
            else {
                switch (lpdis->itemAction) {
                    case ODA_DRAWENTIRE:
                        DrawEntireItem(lpdis, -7);
                        break;

                    case ODA_SELECT:
                        HandleSelectionState(lpdis, -3);
                        break;

                    case ODA_FOCUS:
                        HandleFocusState(lpdis, -5);
                        break;
                }
            }

            return(TRUE);
            break;

        case WM_MEASUREITEM:
            lpmis = (LPMEASUREITEMSTRUCT)lParam;

            // All the items are the same height since the list box
            //   style is LBS_OWNERDRAWFIXED

            lpmis->itemHeight = 30;
            break;

        case WM_CLOSE:
            EndDialog(hDlg, 0);
            return(TRUE);
            break;

        default:
            return(FALSE);
    }
    return(TRUE);
}

//  FUNCTION   : ComboBoxExample (hDlg, message, wParam, lParam)
//
//  PURPOSE    :
//      Dialog function for the text combo dialog. The push buttons
//      send various messages to the combo box and the edit control
//      when selected. They allow the user to vary data sent with
//      each message.
//
//  PARAMETERS :
//      hDlg    - handle of the dialog box window
//      message - message currently to be processed
//      wParam  - additional info associated with the message
//      lParam  - additional DWORD info associated with the message
//
//  RETURNS :
//      TRUE    - message was processed,and does not get passed
//                on to the Dialog Manager
//      FALSE   - message was processed, and is passed on to DlgManager
//                for default processing.

BOOL APIENTRY ComboBoxExample(HWND hDlg, UINT message,
                              UINT wParam, LONG lParam)
{
    HWND hWndCombo;              // Handle to the combo box control
                                 //   in the dialog box window
    HWND hWndCheckBox;           // Handle to the Auto Check Box
    CHAR strSingleEditLine[255]; // Single line edit control input
    INT  wIndex, wCount;

    hWndCombo    = GetDlgItem(hDlg, ID_COMBOBOX);
    hWndCheckBox  = GetDlgItem(hDlg, ID_STEPSBOX);

    switch (message) {
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDOK:
                    EndDialog (hDlg,0);
                    return(TRUE);

                case ID_UNSLBUTTON:
                    // Selecting this button unselects any selection in
                    //   the combobox.

                    SetDlgItemText (hDlg, ID_TEXT1, "");
                    SetDlgItemText (hDlg, ID_TEXT2, "");
                    wIndex = (WORD)SendMessage(hWndCombo,
                                        CB_GETCURSEL, 0, 0L);
                    if (wIndex == CB_ERR)
                        MessageBox (hDlg, (LPSTR)"No Selection", NULL, MB_OK);
                    else
                        SendMessage (hWndCombo, CB_SETCURSEL, (WPARAM)-1, 0L);
                    SetFocus (GetDlgItem (hDlg, ID_SINGLEEDIT));
                    break;

                case ID_NUMSELBUTTON:
                    // An integer value is taken from the edit control
                    //   and an attempt is made to select a combo box entry
                    //   having this index.

                    SetDlgItemText (hDlg, ID_TEXT1, "");
                    SetDlgItemText (hDlg, ID_TEXT2, "");
                    wCount = (WORD) SendMessage (hWndCombo, CB_GETCOUNT, 0, 0L);
                    wIndex = (INT) GetDlgItemInt (hDlg, ID_SINGLEEDIT,
                                                  NULL, TRUE);
                    if (wIndex >= wCount)
                        MessageBox (hDlg, (LPSTR)"Bad Selection", NULL, MB_OK);
                    else
                        SendMessage(hWndCombo, CB_SETCURSEL, wIndex, 0L);
                    SetFocus (GetDlgItem (hDlg, ID_SINGLEEDIT));
                    break;

                case ID_TXTSELBUTTON:
                    // A text string is taken from the edit control and
                    // attempt is made to select a combo box entry having
                    // the string as a prefix.

                    SetDlgItemText (hDlg, ID_TEXT1, "");
                    SetDlgItemText (hDlg, ID_TEXT2, "");
                    GetDlgItemText (hDlg, ID_SINGLEEDIT,
                                   (LPSTR)strSingleEditLine, 255);
                    wIndex = (WORD) SendMessage (hWndCombo, CB_SELECTSTRING,
                                   (WPARAM)-1,(LONG)(LPSTR)strSingleEditLine);
                    if (wIndex == CB_ERR)
                        MessageBox (hDlg, (LPSTR)"Bad Selection", NULL, MB_OK);
                    SetFocus (GetDlgItem (hDlg, ID_SINGLEEDIT));
                    break;

                case ID_FNDSELBUTTON:
                    // Searches for the text specified in the list of combo
                    // entries and returns the index (in combo box) of the
                    // first match. The index is displayed in the "Text1"
                    // field of the dialog.

                    SetDlgItemText (hDlg, ID_TEXT1, "");
                    SetDlgItemText (hDlg, ID_TEXT2, "");
                    GetDlgItemText (hDlg, ID_SINGLEEDIT,
                         (LPSTR)strSingleEditLine, 255);
                    wIndex = (WORD)SendMessage (hWndCombo, CB_FINDSTRING,
                                  (WPARAM)-1,(LONG)(LPSTR)strSingleEditLine);
                    if (wIndex == CB_ERR)
                        MessageBox (hDlg, (LPSTR)"Bad Selection", NULL, MB_OK);
                    else
                        SetDlgItemInt (hDlg, ID_TEXT1, wIndex, FALSE);
                    SetFocus (GetDlgItem (hDlg, ID_SINGLEEDIT));
                    break;

                case ID_CLRBUTTON:
                    // Clears the combo box of all it's entries

                    SetDlgItemText (hDlg, ID_TEXT1, "");
                    SetDlgItemText (hDlg, ID_TEXT2, "");

                    wCount = (WORD) SendMessage (hWndCombo, CB_GETCOUNT, 0, 0L);
                    if (!wCount)
                        MessageBox (hDlg, (LPSTR)"Already clear", NULL, MB_OK);
                    else {
                        SetDlgItemInt (hDlg, ID_TEXT1, wCount, TRUE);
                        SetDlgItemText (hDlg, ID_TEXT2, "Items cleared");
                        SendMessage (hWndCombo,CB_RESETCONTENT, 0, 0L);
                    }
                    SetFocus (GetDlgItem (hDlg,ID_SINGLEEDIT));
                    break;

                case ID_ADDBUTTON:
                    // Takes the string specified in the edit control and
                    // adds it to the combo box.

                    SetDlgItemText (hDlg, ID_TEXT1, "");
                    SetDlgItemText (hDlg, ID_TEXT2, "");
                    GetDlgItemText (hDlg, ID_SINGLEEDIT,
                                    strSingleEditLine, 255);
                    SendMessage (hWndCombo, CB_ADDSTRING, 0,
                                (LONG)(LPSTR) strSingleEditLine);
                    SetFocus (GetDlgItem (hDlg, ID_SINGLEEDIT));
                    break;

                case ID_DELETEBUTTON:
                    // Delete the currently selected item from
                    // the combo box.

                    SetDlgItemText (hDlg, ID_TEXT1, "");
                    SetDlgItemText (hDlg, ID_TEXT2, "");
                    wIndex = (WORD) SendMessage (hWndCombo, CB_GETCURSEL, 0, 0L);
                    if (SendMessage (hWndCombo, CB_DELETESTRING,
                                     wIndex, 0L) == CB_ERR)
                        MessageBox (hDlg, (LPSTR)"No Selection", NULL, MB_OK);
                    else {
                        SetDlgItemText (hDlg, ID_TEXT1, "deleted index #");
                        SetDlgItemInt  (hDlg, ID_TEXT2, wIndex, TRUE);
                    }
                    SetFocus (GetDlgItem (hDlg, ID_SINGLEEDIT));
                    break;

                case ID_CBDIRBUTTON:
                    // Appends a directory listing of the current directory
                    //   to the combo box entries.

                    SetDlgItemText (hDlg, ID_TEXT1, "");
                    SetDlgItemText (hDlg, ID_TEXT2, "");
                    wIndex = (WORD)SendMessage (hWndCombo, CB_DIR,
                                    0x10|0x4000, (LONG)(LPSTR)"*.*");
                    SetFocus (GetDlgItem (hDlg, ID_SINGLEEDIT));
                    break;

                case ID_CPYBUTTON:
                    // Copies the currently selected item in the combo box to
                    //   the edit control.

                    SetDlgItemText (hDlg, ID_TEXT1, "");
                    SetDlgItemText (hDlg, ID_TEXT2, "");

                    wIndex = (WORD) SendMessage (hWndCombo, CB_GETCURSEL,
                                                 0, 0L);
                    if (wIndex == CB_ERR)
                        MessageBox(hDlg, (LPSTR)"No Selection", NULL, MB_OK);
                    else {
                        wCount = SendMessage (hWndCombo, CB_GETLBTEXTLEN,
                                              wIndex, 0L);
                        SendMessage (hWndCombo, CB_GETLBTEXT, wIndex,
                                    (LONG)(LPSTR)strSingleEditLine);
                        SetDlgItemText(hDlg, ID_SINGLEEDIT,
                                      (LPSTR)strSingleEditLine);
                        SetDlgItemText(hDlg, ID_TEXT1, "copied index #");
                        SetDlgItemInt(hDlg, ID_TEXT2, wIndex, TRUE);
                    }
                    SetFocus (GetDlgItem (hDlg, ID_SINGLEEDIT));
                    break;

                // When the combo notification box is checked, a message
                //   box is flashed showing what notification codes
                //   the combo box is returning to the app. in response
                //   to the messages sent by the buttons.

                case ID_COMBOBOX:
                    if (SendMessage (hWndCheckBox, BM_GETCHECK, 0, 0L)){
                        switch (HIWORD(lParam)) {
                            case (WORD)CBN_ERRSPACE:
                                MessageBox (hDlg, (LPSTR)"CB Out of Space",
                                            "CB MSG", MB_OK);
                                break;

                            case CBN_SELCHANGE:
                                MessageBox (hDlg, (LPSTR)"CB Sel Change",
                                            "CB MSG", MB_OK);
                                break;

                            case CBN_DBLCLK:
                                MessageBox(hDlg, (LPSTR)"CB Double Click",
                                           "CB MSG", MB_OK);
                                break;

                            case CBN_SETFOCUS:
                                SetDlgItemText(hDlg, ID_TEXT1, "CB SetFocus");
                                break;

                            case CBN_KILLFOCUS:
                                SetDlgItemText(hDlg, ID_TEXT1, "CB KillFocus");
                                break;
                        }
                    }
                    break;

                default:
                    return(FALSE);
            }
            break;

        case WM_CLOSE:
            EndDialog(hDlg, 0);
            return(TRUE);
            break;

        default:
            return(FALSE);
    }
    return(TRUE);
}


//  FUNCTION   : OwnerComboBoxExample (hDlg, message, wParam, lParam)
//
//  PURPOSE    :
//      Dialog function for the dropdown list combo box with
//      owner-draw.
//
//  PARAMETERS :
//      hDlg    - handle of the dialog box window
//      message - message currently to be processed
//      wParam  - additional info associated with the message
//      lParam  - additional DWORD info associated with the message
//
//  RETURNS :
//      TRUE    - message was processed,and does not get passed
//                on to the Dialog Manager
//      FALSE   - message was processed, and is passed on to DlgManager
//                for default processing.

BOOL APIENTRY OwnerComboBoxExample (HWND hDlg, UINT message,
                                    UINT wParam, LONG lParam)
{
    LPDRAWITEMSTRUCT lpdis;
    LPMEASUREITEMSTRUCT lpmis;

    switch (message) {
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDOK:
                    EndDialog (hDlg, 0);
                    return(TRUE);
                    break;

                // Clicking any of these buttons adds the corresponding
                //   color to the combo box. The application-supplied
                //   data is the RGB value for the color to be drawn
                //   in the listbox.

                case ID_BLACK:
                    SendMessage (GetDlgItem(hDlg, ID_LISTBOX),
                                 CB_ADDSTRING, 0, RGB (0,0,0));
                    return(TRUE);
                    break;

                case ID_RED:
                    SendMessage (GetDlgItem (hDlg, ID_LISTBOX),
                                 CB_ADDSTRING, 0, RGB (255,0,0));
                    return(TRUE);
                    break;

                case ID_BLUE:
                    SendMessage (GetDlgItem(hDlg, ID_LISTBOX),
                                 CB_ADDSTRING, 0, RGB (0,0,255));
                    return(TRUE);
                    break;

                case ID_GREEN:
                    SendMessage (GetDlgItem(hDlg, ID_LISTBOX),
                                 CB_ADDSTRING, 0, RGB (0,255,0));
                    return(TRUE);
                    break;

                default:
                    return(TRUE);
                    break;
            }

        case WM_DRAWITEM:
            // Get pointer to the DRAWITEMSTRUCT
            lpdis = (LPDRAWITEMSTRUCT)lParam;

            if (lpdis->itemID == -1) {

                // -1 indicates no combo box items. This is sent
                //   when the user TABS into an empty combo box or
                //   an empty combo box gets the focus. We have to
                //   indicate (somehow) that this owner-draw combo box
                //   has the focus. We do it in response to this msg.
                //   Note that lpdis->itemData field would be invalid
                //   in this instance so we can't allow it to fall into
                //   our standard routines.

                HandleFocusState(lpdis, -2);
            }
            else {
                switch (lpdis->itemAction) {
                    case ODA_DRAWENTIRE:
                        DrawEntireItem(lpdis, -4);
                        break;

                    case ODA_SELECT:
                        HandleSelectionState(lpdis, 0);
                        break;

                    case ODA_FOCUS:
                        HandleFocusState(lpdis, -2);
                        break;
                }
            }

            return(TRUE);
            break;

        case WM_MEASUREITEM:
            lpmis = (LPMEASUREITEMSTRUCT)lParam;

            // All the items are the same height since the combo box
            // is CBS_OWNERDRAWFIXED

            if (lpmis->itemID == -1) {

                // If -1 for item, then we are setting the height of
                // the always visible static item part of the dropdown
                // combo box.

                lpmis->itemHeight = 25;
                return(TRUE);
            }
            lpmis->itemHeight = 30;
            break;

        case WM_CLOSE:
            EndDialog(hDlg, 0);
            return(TRUE);
            break;

        default:
            return(FALSE);
    }
    return(TRUE);
}


/****************************************************************************
 *									    *
 *  FILE	: OwnCombo.h						    *
 *									    *
 *  DESCRIPTION : Header file for OwnCombo example.			    *
 *									    *
 ****************************************************************************/
/* Defines for menu items */
#define IDM_EXIT    201
#define IDM_ABOUT   202

#define IDM_LISTBOX       300
#define IDM_MULTILISTBOX  301
#define IDM_COMBOBOX      302
#define IDM_OWNERCOMBOBOX 303

#define IDM_HELP	  400

/* Defines for dialog box ownerdraw controls. */
#define ID_LISTBOX  1000
#define ID_BLACK    1003
#define ID_WHITE    1004
#define ID_RED      1005
#define ID_BLUE     1006
#define ID_GREEN    1007

/* Defines for combo box example */
#define ID_COMBOBOX     1000
#define ID_SINGLEEDIT   1001
#define ID_TEXT1        1002
#define ID_TEXT2        1003
#define ID_STEPSBOX     1004
#define ID_UNSLBUTTON   1005
#define ID_NUMSELBUTTON 1006
#define ID_TXTSELBUTTON 1007
#define ID_FNDSELBUTTON 1008
#define ID_CLRBUTTON    1009
#define ID_ADDBUTTON    1010
#define ID_DELETEBUTTON 1011
#define ID_CPYBUTTON    1012
#define ID_CBDIRBUTTON  1013

int PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);
long FAR  PASCAL OwnComboWndProc(HWND, UINT, WPARAM, LPARAM);
BOOL NEAR PASCAL OwnComboInit(HANDLE);
BOOL FAR  PASCAL About(HWND, unsigned, WORD, LONG);
BOOL FAR  PASCAL ListBoxExample(HWND, unsigned, WORD, LONG);
BOOL FAR  PASCAL ComboBoxExample(HWND, unsigned, WORD, LONG);
BOOL FAR  PASCAL OwnerComboBoxExample(HWND, unsigned, WORD, LONG);

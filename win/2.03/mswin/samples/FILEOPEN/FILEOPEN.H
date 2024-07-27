/* file menu items */

#define     IDM_NEW      100
#define     IDM_OPEN     101
#define     IDM_SAVE     102
#define     IDM_SAVEAS   103
#define     IDM_PRINT    104
#define     IDM_EXIT     105
#define     IDM_ABOUT    106

/* edit menu items */

#define     IDM_UNDO     200
#define     IDM_CUT      201
#define     IDM_COPY     202
#define     IDM_PASTE    203
#define     IDM_CLEAR    204

/* Control IDs */

#define     ID_FILENAME  400
#define     ID_EDIT      401
#define     ID_FILES     402
#define     ID_PATH      403
#define     ID_LISTBOX   404

int PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);
BOOL FileOpenInit(HANDLE);
long FAR PASCAL FileOpenWndProc(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL About(HWND, unsigned, WORD, LONG);
HANDLE FAR PASCAL OpenDlg(HWND, unsigned, WORD, LONG);
void UpdateListBox(HWND);
void SeparateFile(HWND, LPSTR, LPSTR, LPSTR);
void AddExt(PSTR, PSTR);
void ChangeDefExt(PSTR, PSTR);
void _lstrcpy(LPSTR, LPSTR);
void _lstrncpy(LPSTR, LPSTR, int);
int  _lstrlen(LPSTR);

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

#define MAXFILESIZE 0x7fff           /* maximum file size that can be loaded */

long FAR PASCAL EditFileWndProc(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL About(HWND, unsigned, WORD, LONG);
BOOL EditFileInit(HANDLE);
HANDLE FAR PASCAL OpenDlg(HWND, unsigned, WORD, LONG);
int FAR PASCAL SaveAsDlg(HWND, unsigned, WORD, LONG);
BOOL CheckFileName(HWND, PSTR, PSTR);
BOOL SaveFile(HWND);
BOOL QuerySaveFile(HWND);
void _lstrcpy(LPSTR, LPSTR);
void _lstrncpy(LPSTR, LPSTR, int);
int  _lstrlen(LPSTR);
void SeparateFile(HWND, LPSTR, LPSTR, LPSTR);
void UpdateListBox(HWND);
void SetNewBuffer(HWND, HANDLE, PSTR);
void AddExt(PSTR, PSTR);
void ChangeDefExt(PSTR, PSTR);

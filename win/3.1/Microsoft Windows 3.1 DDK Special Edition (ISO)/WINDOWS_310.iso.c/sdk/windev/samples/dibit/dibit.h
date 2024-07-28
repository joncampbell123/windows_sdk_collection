#define IDM_ABOUT	100
#define IDM_OPEN	101
#define IDM_SAVE	102

/* Options menu items */

#define IDM_SETDIB	200
#define IDM_TODEV	201
#define IDM_STRETCH	202

/* Palette menu items */
#define	IDM_PALRGB	300
#define	IDM_PALIND	301

/* Control IDs */

#define     IDC_FILENAME  400
#define     IDC_EDIT      401
#define     IDC_FILES     402
#define     IDC_PATH      403
#define     IDC_LISTBOX   404

/* flags for _lseek */
#define  SEEK_CUR 1
#define  SEEK_END 2
#define  SEEK_SET 0

#define MAXREAD  32768		       /* Number of bytes to be read during */
				       /* each read operation.		    */
int PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);
BOOL InitApplication(HANDLE);
BOOL InitInstance(HANDLE, int);
long FAR PASCAL MainWndProc(HWND, UINT, WPARAM, LPARAM);
BOOL FAR PASCAL About(HWND, unsigned, WORD, LONG);

/* OpenFile functions */
HANDLE FAR PASCAL OpenDlg(HWND, unsigned, WORD, LONG);
void SeparateFile(HWND, LPSTR, LPSTR, LPSTR);
void UpdateListBox(HWND);
void AddExt(PSTR, PSTR);
void ChangeDefExt(PSTR, PSTR);

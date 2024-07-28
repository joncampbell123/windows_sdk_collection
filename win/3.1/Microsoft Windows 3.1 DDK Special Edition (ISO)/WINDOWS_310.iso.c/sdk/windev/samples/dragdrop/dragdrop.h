//********************** Defines for menu

#define IDM_ABOUT           100
#define IDM_INFO            110

//********************** Defines for Dialog IDs

#define IDD_NUMFILES        200
#define IDD_XCORD           210
#define IDD_YCORD           220


//********************** Defines for the main window's size

#define MAIN_WIDTH          400
#define MAIN_HEIGHT         200


//********************** Misc. defines

#define FILE_NAME_LENGTH    45
#define STRING_LEN          10


//********************** Function prototypes

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpCmdLine,  int nCmdShow);

long FAR PASCAL MainWndProc (HWND hWnd,   UINT message,
			     WPARAM wParam, LPARAM lParam);

BOOL FAR PASCAL About (HWND hDlg,   unsigned message,
                       WORD wParam, LONG lParam);

BOOL FAR PASCAL Info (HWND hDlg,   unsigned message,
                      WORD wParam, LONG lParam);


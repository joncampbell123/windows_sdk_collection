HWND WINAPI CreateTrackbar(HWND hWndParent,
						   DWORD dwStyle,
                           int nMin, 
                           int nMax);


HWND CreateLabel(HWND hwndParent, 
                 LPTSTR szText);

LRESULT MsgHScroll(HWND hWnd, 
				   UINT uMessage, 
				   WPARAM wParam, 
				   LPARAM lParam);

LRESULT MsgVScroll(HWND hWnd, 
				   UINT uMessage, 
				   WPARAM wParam, 
				   LPARAM lParam);


// Constants
#define TB_MIN 0
#define TB_MAX 20
#define MAX_CHARS 5
#define PAGE_SIZE 4
#define TB_WIDTH  200
#define TB_HEIGHT 30
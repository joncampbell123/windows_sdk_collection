


#define FAR far
#define NEAR	near
#define PASCAL pascal

typedef struct {
	int x;
	int y;
} POINT;
typedef POINT FAR *LPPOINT;

typedef struct {
	int left;
	int top;
	int right;
	int bottom;
} RECT;
typedef RECT FAR *LPRECT;

typedef unsigned char	BYTE;
typedef unsigned short	WORD;
typedef long LONG;
typedef unsigned long  DWORD;
typedef WORD HANDLE;
typedef HANDLE HDC;
typedef HANDLE FAR * LPHANDLE;
typedef int BOOL;
typedef int FAR *LPBOOL;
typedef short FAR *LPSHORT;
typedef char FAR *LPSTR;
typedef int (FAR PASCAL *FARPROC)();

typedef WORD HWND;
typedef WORD HINST;
#define MAKELONG(a, b)	((long)(((unsigned)a) | ((unsigned long)((unsigned)b)) << 16))
#define LOWORD(l)	((WORD)(l))
#define HIWORD(l)	((WORD)(((DWORD)(l) >> 16) & 0xffff))


/* Window styles */

#define WS_BORDER	0x00800000L
#define WS_VSCROLL	0x00200000L

/* listbox style bits */
#define LBS_SORT	  0x0002L
#define LBS_NOTIFY	  0x0001L
#define LBS_STANDARD	  (LBS_NOTIFY | LBS_SORT | WS_VSCROLL | WS_BORDER)

#define LBN_SELCHANGE   1
#define LBN_DBLCLK      2

#define LB_SETCURSEL	 (7+WM_USER)
#define LB_GETCURSEL	 (9+WM_USER)
#define LB_INSERTSTRING  (2+WM_USER)


HANDLE	    FAR PASCAL FindResource( HANDLE, LPSTR, LPSTR );
int	    FAR PASCAL AccessResource( HANDLE, HANDLE );
HANDLE	    FAR PASCAL LoadResource( HANDLE, HANDLE );
BOOL	    FAR PASCAL FreeResource( HANDLE );
LPSTR  FAR PASCAL LockResource( HANDLE );
HANDLE	    FAR PASCAL GetModuleHandle( LPSTR );
WORD	    FAR PASCAL SizeofResource( HANDLE, HANDLE );


void	    FAR PASCAL FatalExit( int );
HANDLE	    FAR PASCAL FindResource( HANDLE, LPSTR, LPSTR );
HANDLE	    FAR PASCAL LoadResource( HANDLE, HANDLE );
BOOL	    FAR PASCAL FreeResource( HANDLE );
LPSTR  FAR PASCAL LockResource( HANDLE );
WORD	    FAR PASCAL SizeofResource( HANDLE, HANDLE );

HANDLE	    FAR PASCAL GlobalAlloc( WORD, DWORD );
HANDLE	    FAR PASCAL GlobalFree( HANDLE );
LPSTR	    FAR PASCAL GlobalLock( HANDLE );
BOOL	    FAR PASCAL GlobalUnlock( HANDLE );
DWORD	    FAR PASCAL GlobalCompact(DWORD);

HANDLE	FAR PASCAL LocalAlloc( WORD, WORD );
HANDLE	FAR PASCAL LocalFree( HANDLE );
BOOL	FAR PASCAL LocalUnlock( HANDLE );
char NEAR * FAR PASCAL LocalLock( HANDLE );


DWORD	    FAR PASCAL GlobalCompact( DWORD );
#define GlobalDiscard( h ) GlobalReAlloc( h, 0L, GMEM_MOVEABLE )
DWORD	    FAR PASCAL GlobalHandle( WORD );
HANDLE	    FAR PASCAL GlobalReAlloc( HANDLE, DWORD, WORD );
DWORD	    FAR PASCAL GlobalSize( HANDLE );
WORD	    FAR PASCAL GlobalFlags( HANDLE );


int	    FAR PASCAL SetRectEmpty(LPRECT);

int	    far PASCAL _lopen( LPSTR, int );
void	    far PASCAL _lclose( int );
int	    far PASCAL _lcreat( LPSTR, int );
BOOL	    far PASCAL _ldelete( LPSTR );
WORD	    far PASCAL _lwrite( int, LPSTR, int );
WORD	    far PASCAL _lread( int, LPSTR, int );
LONG	    far PASCAL _llseek( int, long, int );

/* Interface to global memory manager */
#define GMEM_FIXED	    0x0000
#define GMEM_MOVEABLE	    0x0002
#define GMEM_NOCOMPACT	    0x0010
#define GMEM_NODISCARD	    0x0020
#define GMEM_ZEROINIT	    0x0040
#define GMEM_MODIFY	    0x0080
#define GMEM_DISCARDABLE    0x0F00
#define GHND	(GMEM_MOVEABLE | GMEM_ZEROINIT)
#define GPTR	(GMEM_FIXED    | GMEM_ZEROINIT)
#define GMEM_SHARE          0x2000
#define GMEM_DDESHARE       0x2000
#define GMEM_LOWER          0x1000
#define GMEM_NOTIFY         0x4000



#define READ	    0	/* Flags for _lopen */
#define WRITE	    1
#define READ_WRITE  2

#define WM_DEVMODECHANGE    0x001b
#define WM_INITDIALOG	    0x0110
#define WM_COMMAND	    0x0111

#define ES_AUTOHSCROLL	  128L
#define WM_USER 	 0x0400
#define EM_LIMITTEXT	 WM_USER+21

int	    FAR PASCAL LoadString( HANDLE, unsigned, LPSTR, int );
void	    FAR PASCAL SetDlgItemInt(HWND, int, unsigned, BOOL);
long	    FAR PASCAL SendDlgItemMessage(HWND, int, unsigned, WORD, LONG);
long	    FAR PASCAL SendMessage(HWND, unsigned, WORD, LONG);
short	    FAR PASCAL SetEnvironment(LPSTR, LPSTR, WORD);
short	    FAR PASCAL GetEnvironment(LPSTR, LPSTR, WORD);
void	    FAR PASCAL CheckDlgButton(HWND, int, WORD);
void	    FAR PASCAL CheckRadioButton(HWND, int, int, int);
int	    FAR PASCAL DialogBox(HANDLE, LPSTR, HWND, FARPROC);
void	    FAR PASCAL EndDialog(HWND, int);
unsigned    FAR PASCAL GetDlgItemInt(HWND, int, BOOL FAR *, BOOL);
HWND	    FAR PASCAL GetDlgItem(HWND, int);
HWND	    FAR PASCAL SetFocus(HWND);

int	    FAR PASCAL GetProfileInt( LPSTR, LPSTR, int );
int	    FAR PASCAL GetProfileString( LPSTR, LPSTR, LPSTR, LPSTR, int );
BOOL	    FAR PASCAL WriteProfileString( LPSTR, LPSTR, LPSTR );
FARPROC     FAR PASCAL MakeProcInstance(FARPROC, HANDLE);
void	    FAR PASCAL FreeProcInstance(FARPROC);

/* added 87-1-23 by sec:
 */
#define SS_LEFT       0L
#define SS_RIGHT      2L
#define ES_LEFT           0L
#define BS_PUSHBUTTON    0L
#define BS_DEFPUSHBUTTON 1L
#define BS_RADIOBUTTON   4L

/* notification codes */
#define EN_SETFOCUS   0x0100
#define EN_KILLFOCUS  0x0200
#define EN_CHANGE     0x0300
#define EN_ERRSPACE   0x0500
#define EN_HSCROLL    0x0601
#define EN_VSCROLL    0x0602


#define MB_OK			    0x0000
#define MB_ICONEXCLAMATION  0x0030

int  FAR PASCAL MessageBox(HWND, LPSTR, LPSTR, WORD);

/************************************************************

	Header file for HFORM.C


************************************************************/

#define WIN31
#define NOCOMM

/* Constants */

#define	cchTitleMax		32
#define	cbSzDlgMax		80

#define chNextField		1
#define chPrecField		2

/* Typedefs */

typedef struct
	{
	char	szTitle[cchTitleMax];
	int	xStatic;
	int	yStatic;
	int	xEdit;
	int	yEdit;
	int	cxEdit;
	int	cyEdit;
	DWORD	dwEditStyle;
	ALC	alc;
	WORD wFieldType;
	HWND	hwnd;
	}
	FIELD, *PFIELD, FAR *LPFIELD;		/* field */

#define FIELDEDIT 0
#define FIELDBEDIT 1
#define FIELDPIC 2

/*	Prototypes */

int				PASCAL	WinMain(HANDLE hInstance, HANDLE hPrevInstance, LPSTR lpszCommandLine, int cmdShow);
HANDLE	FAR	PASCAL	SampleDlgProc(HWND hdlg, WORD message, WORD wParam, LONG lParam);
LONG		FAR	PASCAL	HformWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) ;
BOOL		NEAR	PASCAL	FInitApp(HANDLE hInstance);
BOOL		NEAR	PASCAL	FInitInstance(HANDLE hInstance, HANDLE hPrevInstance, int cmdShow);
BOOL		NEAR	PASCAL	FCreateForm(HWND hwndParent);
VOID		NEAR	PASCAL	SampleDialog(HWND hinstance);
BOOL 		NEAR	PASCAL	FPenOrStubPresent(VOID);
int 		NEAR 	PASCAL 	IFromHwnd(HWND hwnd);
BOOL		NEAR	PASCAL	ProcessFieldChange(HWND hwndFocusField, WORD wParam);

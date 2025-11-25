#define CPCAPTION      1

#define RUN_ICON     100
#define RUN_NAME     101
#define RUN_DESC     102
#define RUN_DLG      103

// function prototypes
LONG CALLBACK CPlApplet(HWND, UINT, LONG, LONG);
BOOL APIENTRY RunDlgProc(HWND, UINT, UINT, LONG);
BOOL APIENTRY FileDlgProc(HWND, UINT, UINT, LONG);
BOOL Browse (HWND IN, CHAR * IN OUT, CHAR * OUT, BOOL IN);
BOOL PageCreate (BOOL bMode, 	CHAR * szFileName, CHAR * ListBox);


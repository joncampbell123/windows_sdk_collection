/*
 * Declare.h	declarations of all procedures in dlgsave.c, dlgopen.c,
 *		print.c, sample.c, and tools.asm.
 *------------------------------------------------------------------------*/

/*------ dlgsave.c ------*/
extern int FAR DlgSaveAs (HANDLE, HWND, int, OFSTRUCT *, int *, char *, char *, char *);
extern BOOL FAR PASCAL DlgFnSaveAs (HWND, unsigned, WORD, LONG);
extern VOID DlgInitSaveAs (HWND, OFSTRUCT *);
extern char * FAR DlgPFileInPath (char *);
extern DlgAddExt (char *);
extern BOOL FAR DlgMergeStrings (char *, char *, char *);


/*------ dlgopen.c -------*/
extern int FAR DlgOpen (HANDLE, HWND, int, OFSTRUCT *, int *, char *, char *, char *);
extern int FAR PASCAL DlgFnOpen (HWND, unsigned, WORD, LONG);
extern VOID FAR DlgCheckOkEnable (HWND, int, unsigned);
extern DlgAddCorrectExtension (char *, BOOL);
extern BOOL FAR DlgCheckFilename (char *);
extern BOOL DlgSearchSpec (char *);

/*----- print.c -----*/
extern int FAR	PASCAL PrintFile(HANDLE, HWND, int, OFSTRUCT, char *, char *);
extern int FAR PASCAL AbortTest(HDC, int);
extern int FAR PASCAL DlgFnAbort(HWND, unsigned, WORD, LONG);
extern HDC GetPrinterDC();
extern PrintErr(HWND, int);
extern CenterPopup(HWND);
extern GetScreenCenter(HWND, LPPOINT, int, int);

/*------ sample.c -------*/
extern LONG FAR PASCAL SampleWndProc (HWND, unsigned, WORD, LONG);
extern SampleCommand (HWND, WORD);
extern BOOL FAR PASCAL SampleAboutDlg (HWND, unsigned, WORD, LONG);
extern int PASCAL WinMain (HANDLE, HANDLE, LPSTR, int);
extern BOOL SampleInit (HANDLE);
extern SampleCheckSave (HWND);
extern SampleUpdateTitle (HWND);


/*------ tools.asm -------*/
extern LPSTR FAR PASCAL lstrcpy( LPSTR, LPSTR );
extern LPSTR FAR PASCAL lstrcat( LPSTR, LPSTR );
extern int   FAR PASCAL lstrlen( LPSTR );
extern VOID  FAR PASCAL _lclose( int );

/****************************************************************************
 *
 *	WINCOM.H
 *
 * These two dialogs are used by the sample code for this release only.
 * They will not be used in future sample code.  We do not recommend that
 * either of these dialogs be used for development of applications, since
 * they may not be supported in future releases of the system software.
 *
 ****************************************************************************/

/*
 *	 OPEN FILE DIALOG BOX
 */

int FAR PASCAL OpenFileDialog(HWND hwndParent, LPSTR lpszTitle,
				LPSTR lpszExtension, WORD wFlags,
				LPSTR lpszFileBuf, WORD wFileLen );

/*  Flags for OpenFileDialog  */
#define DLGOPEN_MUSTEXIST	0x0001
#define DLGOPEN_NOSHOWSPEC	0x0002
#define DLGOPEN_SAVE		0x0004
#define DLGOPEN_OPEN		0x0008
#define DLGOPEN_SHOWDEFAULT	0x0020
#define DLGOPEN_NOBEEPS		0x0040
#define DLGOPEN_SEARCHPATH	0x0080

/*  Return codes  */
#define DLG_CANCEL	0
#define DLG_MISSINGFILE	-1
#define DLG_OKFILE	1

		
/*
 *  ERROR MESSAGE REPORTING BOX
 */

short FAR cdecl ErrorResBox(	HWND	hwnd,
				HANDLE	hInst,
				WORD	flags,
				WORD	idAppName,
				WORD	idErrorStr, ...);


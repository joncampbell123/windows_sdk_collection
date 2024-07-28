/**[f******************************************************************
 * atprocs.h - 
 *
 * Copyright (C) 1988 Aldus Corporation.  All rights reserved.
 * Company confidential.
 *
 **f]*****************************************************************/

/*
 * this module defines macros to access the DLL apple talk functions
 *
 * AT functions that are statically linked with the driver are found in
 * atstuff.h
 *
 */

extern HWND ghATModule;

extern	BOOL	(FAR PASCAL *lpfnATSelect)(HWND);
extern	void	(FAR PASCAL *lpfnATMessage)(short,WORD,LPSTR);
extern	BOOL	(FAR PASCAL *lpfnATOpenStatusWnd)(HWND);
extern	void	(FAR PASCAL *lpfnATCloseStatusWnd)(void);
extern	short	(FAR PASCAL *lpfnATWrite)(LPSTR, short);
extern	short	(FAR PASCAL *lpfnATClose)(void);
extern	short	(FAR PASCAL *lpfnATSendEOF)(void);
extern	BOOL	(FAR PASCAL *lpfnATChooser)(HWND);
extern	HANDLE	(FAR PASCAL *lpfnATOpen)(LPSTR, LPSTR, HDC);


#define ATSelect(a)		((*lpfnATSelect)(a))
#define ATMessage(a,b,c)	((*lpfnATMessage)(a,b,c))
#define ATOpenStatusWnd(a)	((*lpfnATOpenStatusWnd)(a))
#define ATCloseStatusWnd()	((*lpfnATCloseStatusWnd)())
#define ATWrite(a,b)		((*lpfnATWrite)(a,b))
#define ATClose()		((*lpfnATClose)())
#define ATSendEOF()		((*lpfnATSendEOF)())
#define ATChooser(a)		((*lpfnATChooser)(a))
#define ATOpen(a,b,c)		((*lpfnATOpen)(a,b,c))

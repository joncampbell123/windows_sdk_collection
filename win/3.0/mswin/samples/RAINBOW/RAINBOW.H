/*
 * WINDOWS RAINBOW CONTROL - INCLUDE FILE
 *
 */
 
/* rainbow style dialog definitions */
#define	IDTEXT 				0x0100
#define	IDVALUE				0x0101

/* rainbow message definitions */
#define	RM_SETSEL			(WM_USER+1)
#define 	RM_GETSEL			(WM_USER+2)
#define	RM_SETCOLORS		(WM_USER+3)
#define	RM_GETCOLORS		(WM_USER+4)


HANDLE FAR PASCAL RainbowInfo();
BOOL FAR PASCAL RainbowStyle( HWND, HANDLE, LPFNSTRTOID, LPFNIDTOSTR );
WORD FAR PASCAL RainbowFlags( WORD, LPSTR, WORD );




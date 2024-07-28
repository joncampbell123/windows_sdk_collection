/*
 *
 * fonts.h
 *
 */

/* offsets from base pointer to useful things in font directory:
 * all of these are # WORDS to go back from base
 */
#define FDIR_TTDF       -1
#define FDIR_CSOFTFONTS -2

BOOL	FAR PASCAL LoadFontDir(int, LPSTR);
LPSTR	FAR PASCAL LockFontDir(int);
void	FAR PASCAL UnlockFontDir(int);
void	FAR PASCAL DeleteFontDir(int);

LPFX	FAR PASCAL LockFont(LPFONTINFO);	/* from charwdth.c */


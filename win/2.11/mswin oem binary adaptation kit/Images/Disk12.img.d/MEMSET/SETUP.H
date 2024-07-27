/*
** Header-File for SETUP
*/

/*  History
**
**   ......
** 20 may 87    plb     created SUINF.H.
** 21 may 87    plb     added 'struct keyboard' and 'struct machine' to
*                       'union dfile'.
** 22 may 87    plb     added SW_DFHD,SW_DFPRINTFON record selector.
** 27 may 87    plb     changed the file pointer list to an array in
**                      dfile.machine
**                      define S_KERNEL, etc., moved BOOL etc. here.
** 29 may 87    plb     Add [harddisk] size defs
**  9 jun 87    plb     Remove [pif] section
** 14 jun 87    plb     Combine SUSCR.H, SUINF.H, SUAPPDAT.H.
**                      Added function and data extern declarations.
**                      Added S_APPS.
** 15 jun 87    plb     Added defs for InsertMenu[], InsertDisk().
**                      Added StoreStandardItem(), ClearStatusLine()
**                      smLoadErr, smWriteErr.
** 16 jun 87    plb     added sOemDisplayLoaded[], etc.
** 20 jun 87    mp      DefaultPath and WindowsPath moved to SUPATH and SUDEST
**                      Moved RecNPtr(int) decl. from external declaration section
** 18 jun 87    mp      change char to CHAR (defined as unsigned char)
**                      include of setup2.h in the middle of setup.h
** 15 jul 87    mp      join setup2.h and setup.h to setup.h, remove #ifdef's
** 18 jul 87    mp      Added S_386 and S_3EX
** 21 jul 87    mp      change or remove entries from SUAPPDAT.C
**  3 aug 87    mp      aliasing for 386 files
*/

#define LINT_ARGS

#include <malloc.h>
#include <memory.h>

#define CHAR unsigned char

/* some common #define's for NULL and BOOL, etc. */
#define TRUE 1
#define FALSE 0

#ifndef NULL
#define NULL (char *) 0
#endif

/* the following are borrowed from CONFIG.H from old setup */

#define FILEMAX 13              /* max length filename with extension */
#define PATHMAX 60              /* path length max */
#define DESCMAX 129             /* max description in newexe header */

#define MAX(x, y) ((x) < (y) ? (y) : (x))    /* maximum of x and y */
#define MIN(x, y) ((x) > (y) ? (y) : (x))    /* minimum of x and y */



/* define type 'textline' for a row of a screen */

typedef struct txline
	{
	unsigned char   row;
	unsigned char   col;
	unsigned char   * line;
	} textline;


/* size definition for menu array -- no. of files which can be selected
** with ScrSelectList
*/

#define MENUSIZE 200

/* line numbers for display of messages
*/
#define MAXWINBOT 22    /* lowest line used for menu scrolling window */
#define ERRLINE   23
#define STATLINE  24    /* status line (1-based) */



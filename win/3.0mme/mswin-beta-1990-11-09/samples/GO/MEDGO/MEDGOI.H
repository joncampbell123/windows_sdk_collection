/* medgoi.h
 *
 * Internal definitions for the MEDGO handler DLL.
 * This file requires "windows.h", "mediaman.h"
 */

#ifndef _MEDGOI_H_
#define _MEDGOI_H_

#include "medgo.h"


/* DLL global variables */
extern	HANDLE		ghInst;		// DLL instance handle


/* declaration of the handlers */
MedHandler	GOHandler;
MedHandler	GOTXHandler;


/* internal ids for string resources */
#define IDS_GONAME		10
#define IDS_GOTXNAME		11
#define IDS_LOADINGFILE		12
#define IDS_DONELOADINGFILE	13
#define IDS_SAVINGFILE		14
#define IDS_DONESAVINGFILE	15


#endif  /* _MEDGOI_H_  */

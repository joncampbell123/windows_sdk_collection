/*
 * COMMON.H
 *
 * Structures and definitions applicable to all OLE 2.0 UI dialogs.
 *
 * Copyright (c)1992-1994 Microsoft Corporation, All Right Reserved
 */


#ifndef _COMMON_H_
#define _COMMON_H_


#include <lists.h>

short SetUpOLEUIResFile(void);
void ClearOLEUIResFile(short hostResNum);
extern void* SetLpqdFromA5(void);
extern void RestoreLpqd(void*);

// macros used in the UI dialogs
#ifndef MAX
#define MAX(x, y)	((x) > (y) ? (x) : (y))
#endif

#ifndef MIN
#define MIN(x, y) 	((x) < (y) ? (x) : (y))
#endif


/*
 * Standard structure for all dialogs.  This commonality lets us make
 * a single piece of code that will validate this entire structure and
 * perform any necessary initialization.
 */

typedef struct tagOLEUISTANDARD
{
    //These IN fields are standard across all OLEUI dialog functions.
    unsigned long   cbStruct;       //Structure Size
    unsigned long   dwFlags;        //IN-OUT:  Flags
    char           *lpszCaption;    //Dialog caption bar contents
    Point			ptPosition;		//Dialog position, {0,0} = center
    LPFNOLEUIHOOK   lpfnHook;       //Hook callback
    long            lCustData;      //Custom data to pass to hook
} OLEUISTANDARD, *POLEUISTANDARD, *LPOLEUISTANDARD;



//Function prototypes
//COMMON.C
unsigned int	UStandardValidation(LPOLEUISTANDARD, unsigned int);
unsigned int	UStandardInvocation(LPOLEUISTANDARD, DialogPtr *, int);

void			*PvStandardInit(DialogPtr, unsigned int);
Boolean			FStandardHook(LPOLEUISTANDARD, DialogPtr, EventRecord *, short *, long);

void			CheckRadioButton(DialogPtr, int, int, int);
Boolean			FLGetSelectedCell(ListHandle, Cell *);
Boolean			FLSetSelection(ListHandle, Ptr, short);

char			*Ole2UIPathNameFromWD(long, char *);
char			*Ole2UIPathNameFromDirID(long, int, char *);

void			DrawDefaultBorder(DialogPtr, short, Boolean);
void			FlashButton(DialogPtr pDialog, short iitem);


#endif //_COMMON_H_

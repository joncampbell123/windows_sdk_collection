/************************************************************


	Resource header file for EXPENSE.C


************************************************************/

#define menuMain		1
#define miFile 		100
#define miExit			101
#define miClearAll	102
#define miClearSig	103
#define miAbout		104

/*
	miNextField and miPrecField are used as menu accelerators
	to move from one field to another using the TAB and SHIFT-TAB
	keystrokes.
*/

#define miNextField	105
#define miPrecField	106

#define iconExpense	100
#ifdef RC_INVOKED
#define ID(id) id
#else
#define ID(id) MAKEINTRESOURCE(id)
#endif

/* resource ID's */
#define IDEXPENSE  ID(1)

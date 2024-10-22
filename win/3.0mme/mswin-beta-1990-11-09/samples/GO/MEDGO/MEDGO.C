/* medgo.c
 *
 * ResInit (called by MediaMan when MedGo is initalized) calls MediaMan
 * to register the Go media element handlers.
 */

#include <windows.h>
#include <mediaman.h>
#include "medgoi.h"


/* global variables */
HANDLE		ghInst;			// library instance handle


/* LibMain(hModule, cbHeap, lpchCmdLine)
 *
 * DLL main entry point.
 */
BOOL FAR PASCAL LibMain(HANDLE hModule, int cbHeap, LPSTR lpchCmdLine)
{
	/* stash away the DLL instance handle */
	ghInst = hModule;

	/* return TRUE on success */
	return TRUE;
}


/* WEP()
 * 
 * This procedure is called when the library is unloaded by Windows.
 * Note that ResTerminate() will already have been called by MediaMan.
 * 
 * Note: you cannot call any dependent DLLs from this procedure because
 * Windows unloads things from the bottom of the dependency tree upwards.
 * In addition, you cannot call any of the LoadModule/FreeModule/WinExec/
 * LoadLibary/FreeLibrary routines here, since these procedures are
 * non-reentrant.  Global memory operations are a no-no as well.
 */
WORD FAR PASCAL WEP(WORD wBogusParam)
{
	return NULL;
}


/* MedInit()
 * 
 * Main MEDIAMAN entry point.  Called when the DLL is loaded by the
 * Media Element Manager.  Registers the element types that this DLL
 * includes.
 */
BOOL FAR PASCAL MedInit()
{
	/* register GO as a primary and secondary handler */
	medRegisterType(medtypeGO, GOHandler,
		MEDTYPE_LOGICAL | MEDTYPE_PHYSICAL);

	/* register GOTX secondary handler */
	medRegisterType(medtypeGOTX, GOTXHandler, MEDTYPE_PHYSICAL);

	return TRUE;
}


/* MedTerminate
 * 
 * Entry point called by Media Element Manager when DLL is being unloaded.
 * Unregister all element types.
 */
BOOL FAR PASCAL MedTerminate()
{
	/* unregister types */
	medUnregisterType(medtypeGO);
	medUnregisterType(medtypeGOTX);

	return TRUE;
}

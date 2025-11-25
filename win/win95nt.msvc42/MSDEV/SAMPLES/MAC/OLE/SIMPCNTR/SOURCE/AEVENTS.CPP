#include "hdrs.h"

extern CApplication *	gApp;


PASCAL_CALL(OSErr) HandleOpenApplication(AEDescList * /* aevt */, AEDescList * /* reply */, long /* refCon */)
{
	return noErr;
}


PASCAL_CALL(OSErr) HandleOpenDocument(AEDescList * /* aevt */, AEDescList * /* reply */, long /* refCon */)
{
	return noErr;
}


PASCAL_CALL(OSErr) HandlePrintDocument(AEDescList * /* aevt */, AEDescList * /* reply */, long /* refCon */)
{
	return noErr;
}


PASCAL_CALL(OSErr) HandleQuitApplication(AppleEvent * /* aevt */, AEDescList * /* reply */, long /* refCon */)
{
	gApp->m_fDone = true;
	return noErr;
}

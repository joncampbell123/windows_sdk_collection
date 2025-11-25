#include "hdrs.h"

extern CApplication *	gApp;

ASSERTDATA;


PASCAL_CALL(OSErr) HandleOpenApplication(AEDescList * /* aevt */, AEDescList * /* reply */, CApplication * theApp)
{
	theApp->CreateNewDocument(true);

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
	gApp->DoQuit();

	return (gApp->ReadyToQuit() ? (OSErr)noErr : userCanceledErr);
}


PASCAL_CALL(OSErr) HandleOleEmbedding(AppleEvent * /* aevt */, AppleEvent* /* reply */, CApplication * theApp)
{
	ASSERT(theApp->m_fUserInControl = true);

	theApp->m_fUserInControl = false;

	theApp->OleLockApplication(false, false);

	return noErr;
}

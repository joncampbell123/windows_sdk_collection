#ifndef _APPLE_EVENTS_H_
#define _APPLE_EVENTS_H_

#include <AppleEvents.h>

class CApplication;

PASCAL_CALL(OSErr)  HandleOpenApplication(AEDescList *aevt, AEDescList *reply, CApplication *theApp);
PASCAL_CALL(OSErr)  HandleOpenDocument(AEDescList *aevt, AEDescList *reply, long refCon);
PASCAL_CALL(OSErr)  HandlePrintDocument(AEDescList *aevt, AEDescList *reply, long refCon);
PASCAL_CALL(OSErr)  HandleQuitApplication(AEDescList *aevt, AEDescList *reply, long refCon);
PASCAL_CALL(OSErr)  HandleOleEmbedding(AppleEvent *aevt, AppleEvent *reply, CApplication *theApp);

#endif // _APPLE_EVENTS_H_

#ifndef _MSC_VER
#pragma once
#endif


#ifndef _APPLE_EVENTS_H_
#define _APPLE_EVENTS_H_

PASCAL_CALL(OSErr) HandleOpenApplication(AEDescList *aevt, AEDescList *reply, long refCon);
PASCAL_CALL(OSErr) HandleOpenDocument(AEDescList *aevt, AEDescList *reply, long refCon);
PASCAL_CALL(OSErr) HandlePrintDocument(AEDescList *aevt, AEDescList *reply, long refCon);
PASCAL_CALL(OSErr) HandleQuitApplication(AEDescList *aevt, AEDescList *reply, long refCon);

#endif // _APPLE_EVENTS_H_

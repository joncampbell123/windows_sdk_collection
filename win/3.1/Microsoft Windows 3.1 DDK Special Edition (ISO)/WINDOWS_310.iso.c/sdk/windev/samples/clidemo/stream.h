/*
 * stream.h - OLE stream I/O headers.
 *
 * Created by Microsoft Corporation.
 * (c) Copyright Microsoft Corp. 1990 - 1992  All Rights Reserved
 */

//--- CONSTANTS ----

#define MAXREAD   ((LONG)  (60L * 1024L))

//--- GLOBALS ---

extern DWORD    vcbObject;

//--- PROTOTYPES ---

//- OLE callbacks

DWORD FAR PASCAL ReadStream(LPAPPSTREAM, LPSTR, DWORD);
DWORD FAR PASCAL WriteStream(LPAPPSTREAM, LPSTR, DWORD);

//- Far

BOOL FAR          WriteToFile(LPAPPSTREAM);
BOOL FAR          ObjWrite(LPAPPSTREAM, APPITEMPTR);
BOOL FAR          ReadFromFile(LPAPPSTREAM, LHCLIENTDOC, LPOLECLIENT);
BOOL FAR          ObjRead(LPAPPSTREAM, LHCLIENTDOC, LPOLECLIENT);

//- Local

DWORD PASCAL      lread(HANDLE, void FAR *, DWORD);
DWORD PASCAL      lwrite(HANDLE, void FAR *, DWORD);
static void       UpdateLinks(LHCLIENTDOC);
static void       UpdateFromOpenServers(void);

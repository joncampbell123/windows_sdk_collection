/*
 * object.h 
 *
 * Created by Microsoft Corporation.
 * (c) Copyright Microsoft Corp. 1990 - 1992  All Rights Reserved
 */

//--- PROTOTYPES ---

//- OLE Callbacks

int FAR PASCAL CallBack(LPOLECLIENT, OLE_NOTIFICATION, LPOLEOBJECT);

//- Exported Windows procedures

long FAR PASCAL ItemWndProc(HWND, UINT, WPARAM, LPARAM);

//- Far
void FAR       ObjDelete(APPITEMPTR, BOOL);
void FAR       ConvertToClient(LPRECT);
OLESTATUS FAR  Error(OLESTATUS);
APPITEMPTR FAR PreItemCreate(LPOLECLIENT, BOOL, LHCLIENTDOC);
BOOL FAR       PostItemCreate(LPOLEOBJECT, LONG, LPRECT, APPITEMPTR);
void FAR       ObjPaste(BOOL, LHCLIENTDOC, LPOLECLIENT);
BOOL FAR       ObjCopy(APPITEMPTR);
BOOL FAR       ObjGetData (APPITEMPTR, LPSTR);
void FAR       ObjChangeLinkData(APPITEMPTR, LPSTR);
void FAR       ObjSaveUndo(APPITEMPTR);
void FAR       ObjDelUndo(APPITEMPTR); 
void FAR       ObjUndo(APPITEMPTR);
void FAR       ObjFreeze(APPITEMPTR);
void FAR       ObjInsert(LHCLIENTDOC, LPOLECLIENT);
void FAR       ObjCreateFromTemplate(LHCLIENTDOC, LPOLECLIENT);
void FAR       ObjCreateWrap(HANDLE, LHCLIENTDOC, LPOLECLIENT);
void FAR       UpdateObjectMenuItem(HMENU);
void FAR       ExecuteVerb(int, APPITEMPTR);

//- Local
static void    Release(APPITEMPTR);
BOOL FAR       ObjSetBounds(APPITEMPTR);

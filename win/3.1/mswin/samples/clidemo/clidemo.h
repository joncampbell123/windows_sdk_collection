/*
 * clidemo.h 
 *
 * Created by Microsoft Corporation.
 * (c) Copyright Microsoft Corp. 1990 - 1992  All Rights Reserved
 */

//--- CONSTANTS ---

#define CXDEFAULT       400     //- Default object size:  400 x 300 
#define CYDEFAULT       300
#define COBJECTSMAX     50      //- max number of objects in our app 

//--- PROTOTYPES ---

//--- Exported window procedures 

long FAR PASCAL      FrameWndProc(HWND, UINT, WPARAM, LPARAM);

//--- FAR 

void FAR             FixObjectBounds(LPRECT lprc);

//--- Local

static LPOLECLIENT   InitClient(HANDLE);
static void          EndClient(LPOLECLIENT);
static LPAPPSTREAM   InitStream(HANDLE);
static void          EndStream(LPAPPSTREAM);
static void          ProcessCmdLine(LPSTR);
static BOOL          InitApplication(HANDLE); 
static BOOL          InitInstance(HANDLE);
static void          SetTitle(PSTR);
static void          MyOpenFile(PSTR,LHCLIENTDOC *, LPOLECLIENT, LPAPPSTREAM);
static void          NewFile(PSTR,LHCLIENTDOC *, LPAPPSTREAM); 
static BOOL          SaveFile(PSTR, LHCLIENTDOC, LPAPPSTREAM);
static void          SaveasFile(PSTR, LHCLIENTDOC, LPAPPSTREAM);
static BOOL          LoadFile(PSTR, LHCLIENTDOC, LPOLECLIENT, LPAPPSTREAM); 
static void          ClearAll(LHCLIENTDOC, BOOL);
static void          EndInstance(void);
static BOOL          SaveAsNeeded(PSTR,LHCLIENTDOC,LPAPPSTREAM);
static void          UpdateMenu(HMENU);
static BOOL          RegDoc(PSTR, LHCLIENTDOC *);
static void          DeregDoc(LHCLIENTDOC);
static BOOL          InitAsOleClient(HANDLE, HWND, PSTR, LHCLIENTDOC *, LPOLECLIENT *,  LPAPPSTREAM *);
void FAR             ClearItem(APPITEMPTR);
static long          QueryEndSession(PSTR, LHCLIENTDOC, LPAPPSTREAM);

//--- MACROS ---

/*
 * ANY_OBJECT_BUSY
 * checks to see if any object in the document is busy. This prevents 
 * a new document from being saved to file if there are objects in 
 * asynchronous states.
 */

#define ANY_OBJECT_BUSY  {\
    if (ObjectsBusy()) \
         break; \
}
   

/*
 * LINKS.H
 *
 * Internal definitions, structures, and function prototypes for the
 * OLE 2.0 UI Edit Links dialog.
 *
 * Copyright (c)1992-1994 Microsoft Corporation, All Right Reserved
 */


#ifndef _LINKS_H_
#define _LINKS_H_


#define DIDEditLinks		  10002
#define DIDChangeSource       10010
#define IDListDef			  10001
#define kLinkPathMenu		  10000

#define OLEUI_SZMAX		256
#define LINKTYPELEN		9
#define szNULL			"\0"


//Internally used structure

typedef struct tagLINKINFO
{
	unsigned long		dwLink;				// app specific identifier of a link
	char				*lpszDisplayName;	// file based part of name
	char				szChoppedName[OLEUI_SZMAX];	// name after being chopped
	char				*lpszChoppedName;			// ptr to name after being chopped
	char				szChoppedLink[OLEUI_SZMAX];	// link after being chopped
	char				*lpszChoppedLink;			// ptr to link after being chopped
	char				*lpszItemName;		// object part of name
	char				*lpszShortFileName;	// filename without path
	char				*lpszShortLinkType;	// Short link type - progID
	char				*lpszFullLinkType;	// Full link type - user friendly name
	char				*lpszAMX;			// Is the link auto (A) man (M) or dead (X)
	unsigned long		clenFileName;		// count of file part of mon.
	Boolean				fSourceAvailable;	// bound or not - on boot assume yes??
	Boolean				fIsAuto;			// 1 = automatic, 0 = manual update
	Boolean				fIsMarked;			// 1 = marked, 0 = not
	Boolean				fDontFree;			// Don't free this data since it's being reused
	Boolean				fIsSelected;		// item selected or to be selected
	short				nColPos[3];			// Indent position for each column
} LINKINFO, *LPLINKINFO;


/*
 * What we store extra in this structure besides the original caller's
 * pointer are those fields that we need to modify during the life of
 * the dialog but that we don't want to change in the original structure
 * until the user presses OK.
 */

typedef struct tagEDITLINKS
{
	//Keep this item first as the Standard* functions depend on it here.

	LPOLEUIEDITLINKS	lpOEL;				// Original structure passed.

	Boolean				fClose;				// Does the button read cancel or close?
	Boolean             fItemSelected;
	char				szLinkSource[OLEUI_CCHKEYMAX];
	char				szLinkType[OLEUI_CCHKEYMAX];
	short				nColPos[3];
	ListHandle			hListLinks;
	MenuHandle			hMenuSource;
	short				nPopupWidth;
} EDITLINKS, *PEDITLINKS, *LPEDITLINKS;


typedef enum { NOTHING, SET_SELECTION, CHECK_SELECTION } ACTION;


// Data to and from the ChangeSource dialog hook
typedef struct tagCHANGESOURCEHOOKDATA
{
    LPLINKINFO			lpLI;
    LPEDITLINKS			lpEL;
    Boolean				fValidLink,
    					fFirstCall,
    					fEditActive,
    					fFileSelected,
    					fTabKey,
    					fHitOK,
    					fFlashOK;
	short				nFileLength;
	char				szFileName[OLEUI_CCHPATHMAX];
	char				szItemName[OLEUI_CCHPATHMAX];
	Boolean				bFileNameStored;
	Boolean				bItemNameStored;
	char				szEdit[OLEUI_CCHPATHMAX];
	short				nEditLength;
    ACTION				Action;
    FSSpec				fssFile;
    char				*lpszFrom;			// string containing prefix of source
											// changed from
    char				*lpszTo;			// string containing prefix of source
											// source changed to
	StandardFileReply	sfReply;
} CHANGESOURCEHOOKDATA, *PCHANGESOURCEHOOKDATA, *LPCHANGESOURCEHOOKDATA;


//Change Source Dialog identifiers
#define CS_BTN_OK						 10
#define CS_BTN_HELP						 11
#define CS_EDIT_SOURCE					 12
#define CS_UITEM_OKOUTLINE				 13


//Internal function prototypes
//LINKS.C
unsigned int	UEditLinksInit(DialogPtr, LPOLEUIEDITLINKS);
unsigned int	ELUInitUserItems(DialogPtr);

int				LoadLinkLB(ListHandle hListBox, LPOLEUILINKCONTAINER lpOleUILinkCntr, PEDITLINKS pEL);
int				AddLinkLBItem(ListHandle, LPOLEUILINKCONTAINER, LPLINKINFO, Boolean);
void			RefreshLinkLB(ListHandle, LPOLEUILINKCONTAINER);
void			UpdateLinkLBItem(ListHandle, int, LPEDITLINKS, Boolean);
void			ChangeAllLinks(ListHandle, LPOLEUILINKCONTAINER, char *, char *);

void			BreakString(LPLINKINFO);

#ifndef _MSC_VER
pascal Boolean	EditLinksDialogProc(DialogPtr, EventRecord *, short *);
pascal void		ELUserItemProc(DialogPtr, short);
#else
Boolean __pascal EditLinksDialogProc(DialogPtr, EventRecord *, short *);
void __pascal	 ELUserItemProc(DialogPtr, short);
#endif

void			ELUpdateOneControl(DialogPtr pDialog, short iitem, Boolean fAvail);
void			ELUpdateAllControls(DialogPtr, PEDITLINKS);

HRESULT			Container_UpdateNow(DialogPtr, LPEDITLINKS);
HRESULT			Container_OpenSource(DialogPtr, LPEDITLINKS);
Boolean			Container_ChangeSource(DialogPtr, LPEDITLINKS);
HRESULT			Container_BreakLink(DialogPtr, LPEDITLINKS);
HRESULT			Container_AutomaticManual(DialogPtr, Boolean, LPEDITLINKS);

Boolean			ChangeSource(DialogPtr, LPCHANGESOURCEHOOKDATA);

#ifndef _MSC_VER
pascal short	ChangeSourceDialogProc(short iitem, DialogPtr pDialog, LPCHANGESOURCEHOOKDATA lpCshData);
pascal Boolean	ChangeSourceModalProc(DialogPtr pDialog, EventRecord *pEvent, short *pItem, LPCHANGESOURCEHOOKDATA lpCshData);
pascal void		ChangeSourceActivateProc(DialogPtr pDialog, short iitem, Boolean fActivating, LPCHANGESOURCEHOOKDATA lpCshData);
pascal void		CSUserItemProc(DialogPtr pDialog, short iitem);
#else
short __pascal	 ChangeSourceDialogProc(short iitem, DialogPtr pDialog, LPCHANGESOURCEHOOKDATA lpCshData);
Boolean __pascal ChangeSourceModalProc(DialogPtr pDialog, EventRecord *pEvent, short *pItem, LPCHANGESOURCEHOOKDATA lpCshData);
void __pascal	 ChangeSourceActivateProc(DialogPtr pDialog, short iitem, Boolean fActivating, LPCHANGESOURCEHOOKDATA lpCshData);
void __pascal	 CSUserItemProc(DialogPtr pDialog, short iitem);
#endif

void			DiffPrefix(char *lpsz1, char *lpsz2, char **lplpszPrefix1, char **lplpszPrefix2);

void			EditLinksCleanup(DialogPtr);


#endif // __LINKS_H__

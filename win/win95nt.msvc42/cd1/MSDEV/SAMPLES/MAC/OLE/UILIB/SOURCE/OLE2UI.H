/*
 * OLE2UI.H
 *
 * Public definitions, structures, types, and function prototypes for the
 * OLE 2.0 User Interface support library.
 *
 * Copyright (c)1992-1994 Microsoft Corporation, All Right Reserved
 */


#ifndef _OLE2UI_H_
#define _OLE2UI_H_


#if defined (__cplusplus)  && !defined(__MWERKS__)
extern "C" {
#endif


#ifndef THINK_C
#include <Dialogs.h>
#include <Events.h>
#endif

#include <ole2.h>
#include "olestd.h"


#ifndef TRUE
#define FALSE				 0
#define TRUE				 1
#endif

#define ENTERKEY			 3
#define TABKEY				 9
#define RETURNKEY			13
#define ESCAPEKEY			27
#define UPKEY				30
#define DOWNKEY				31
#define PERIODKEY			46
#define CKEY				99
#define VKEY				118
#define XKEY				120


//Standard error definitions
#define OLEUI_FALSE                       0
#define OLEUI_SUCCESS                     1     //No error, same as OLEUI_OK
#define OLEUI_OK                          1     //OK button pressed
#define OLEUI_CANCEL                      2     //Cancel button pressed

#define OLEUI_ERR_STANDARDMIN           100
#define OLEUI_ERR_STRUCTURENULL         101   //Standard field validation
#define OLEUI_ERR_STRUCTUREINVALID      102
#define OLEUI_ERR_CBSTRUCTINCORRECT     103
#define OLEUI_ERR_DIALOGRES				104
#define OLEUI_ERR_LPSZCAPTIONINVALID    105
#define OLEUI_ERR_LPFNHOOKINVALID       106
#define OLEUI_ERR_HINSTANCEINVALID      107
#define OLEUI_ERR_LPSZTEMPLATEINVALID   108
#define OLEUI_ERR_HRESOURCEINVALID      109

#define OLEUI_ERR_FINDTEMPLATEFAILURE   110   //Initialization errors
#define OLEUI_ERR_LOADTEMPLATEFAILURE   111
#define OLEUI_ERR_DIALOGFAILURE         112
#define OLEUI_ERR_LOCALMEMALLOC         113
#define OLEUI_ERR_GLOBALMEMALLOC        114
#define OLEUI_ERR_LOADSTRING            115

#define OLEUI_ERR_STANDARDMAX           116   //Start here for specific errors.


//Result Image IDs
#define RESULTIMAGE_NONE			 0xFFFF
#define RESULTIMAGE_PASTE				  0
#define RESULTIMAGE_EMBED				  1
#define RESULTIMAGE_EMBEDICON			  2
#define RESULTIMAGE_LINK				  3
#define RESULTIMAGE_LINKICON			  4
#define RESULTIMAGE_LINKTOLINK			  5
#define RESULTIMAGE_LINKTOLINKICON		  6


//Maximum size of file path (how big should this be?)
#define OLEUI_CCHPATHMAX                256

//Maximum verb length and length of Object menu
#define OLEUI_OBJECTMENUMAX				 32

//Maximum key size we read from the RegDB.
#define OLEUI_CCHKEYMAX                 256

//Icon label length
#define OLEUI_CCHLABELMAX                40

//OLE1 ProgID length
#define OLEUI_OLE1PROGIDMAX				40

//Resource ID of picture used in Result area
#define OLEUI_PICTID				  10000



/*************************************************************************
** OLE OBJECT FEEDBACK EFFECTS
*************************************************************************/

#define OLEUI_HANDLES_USEINVERSE    0x00000001L
#define OLEUI_HANDLES_NOBORDER      0x00000002L
#define OLEUI_HANDLES_INSIDE        0x00000004L
#define OLEUI_HANDLES_OUTSIDE       0x00000008L


#define OLEUI_SHADE_FULLRECT        1
#define OLEUI_SHADE_BORDERIN        2
#define OLEUI_SHADE_BORDEROUT       3

/* objfdbk.c function prototypes */
STDAPI_(void) OleUIDrawShading(Rect* pRect, GrafPtr drawPort, unsigned long dwFlags, short cWidth);
STDAPI_(void) OleUIDrawHandles(
	Rect*  			pRect,
	GrafPtr    		drawPort,
	unsigned long   dwFlags,
	short    		cSize
);
STDAPI_(void) OleUIShowObject(Rect* pRect, GrafPtr drawPort, Boolean fIsLink);



/*************************************************************************
** INSERT OBJECT DIALOG
*************************************************************************/


typedef struct tagOLEUIINSERTOBJECT
{
    //These IN fields are standard across all OLEUI dialog functions.
    unsigned long   cbStruct;       //Structure Size
    unsigned long   dwFlags;        //IN-OUT:  Flags
    char           *lpszCaption;    //Dialog caption bar contents
    Point			ptPosition;		//Dialog position, {0,0} = center
    LPFNOLEUIHOOK   lpfnHook;       //Hook callback
    long            lCustData;      //Custom data to pass to hook

    //Specifics for OLEUIINSERTOBJECT.  All are IN-OUT unless otherwise spec.
    CLSID           clsid;            //Return space for class ID
    FSSpec         *pfssFile;         //FSSpec for inserts or links
    char           *pszFile;          //Filename for inserts or links
    unsigned int    cchFile;          //Size of pszFile buffer: OLEUI_CCHPATHMAX
    unsigned int    cClsidExclude;    //IN only:  CLSIDs in lpClsidExclude
    LPCLSID         lpClsidExclude;   //List of CLSIDs to exclude from listing.

    //Specific to create objects if flags say so
    IID             iid;              //Requested interface on creation.
    unsigned long   oleRender;        //Rendering option
    LPFORMATETC     lpFormatEtc;      //Desired format
    LPOLECLIENTSITE lpIOleClientSite; //Site to be use for the object.
    LPSTORAGE       lpIStorage;       //Storage used for the object
    void          **ppvObj;           //Where the object is returned.
    SCODE           sc;               //Result of creation calls.
	PicHandle       hMetaPict;        	  //OUT: PICT containing iconic aspect.
} OLEUIINSERTOBJECT, *POLEUIINSERTOBJECT, *LPOLEUIINSERTOBJECT;


//API to bring up InsertObject dialog
STDAPI_(unsigned int) OleUIInsertObject(LPOLEUIINSERTOBJECT);


//Insert Object flags
#define IOF_SHOWHELP                	0x00000001L
#define IOF_SELECTCREATENEW         	0x00000002L
#define IOF_SELECTCREATEFROMFILE    	0x00000004L
#define IOF_CHECKLINK               	0x00000008L
#define IOF_CHECKDISPLAYASICON      	0x00000010L
#define IOF_CREATENEWOBJECT         	0x00000020L
#define IOF_CREATEFILEOBJECT        	0x00000040L
#define IOF_CREATELINKOBJECT        	0x00000080L
#define IOF_DISABLELINK             	0x00000100L
#define IOF_VERIFYSERVERSEXIST      	0x00000200L
#define IOF_SELECTDEFAULTFILE			0x00000400L


//Insert Object specific error codes
#define OLEUI_IOERR_PSZFILEINVALID          (OLEUI_ERR_STANDARDMAX+0)
#define OLEUI_IOERR_LPSZLABELINVALID        (OLEUI_ERR_STANDARDMAX+1)
#define OLEUI_IOERR_HICONINVALID            (OLEUI_ERR_STANDARDMAX+2)
#define OLEUI_IOERR_LPFORMATETCINVALID      (OLEUI_ERR_STANDARDMAX+3)
#define OLEUI_IOERR_PPVOBJINVALID           (OLEUI_ERR_STANDARDMAX+4)
#define OLEUI_IOERR_LPIOLECLIENTSITEINVALID (OLEUI_ERR_STANDARDMAX+5)
#define OLEUI_IOERR_LPISTORAGEINVALID       (OLEUI_ERR_STANDARDMAX+6)
#define OLEUI_IOERR_SCODEHASERROR           (OLEUI_ERR_STANDARDMAX+7)
#define OLEUI_IOERR_LPCLSIDEXCLUDEINVALID   (OLEUI_ERR_STANDARDMAX+8)
#define OLEUI_IOERR_CCHFILEINVALID          (OLEUI_ERR_STANDARDMAX+9)


//Insert Object Dialog identifiers
#define IO_BTN_OPEN						  1
#define IO_BTN_CANCEL					  2
#define IO_UITEM_VOLUME					  4
#define IO_BTN_EJECT					  5
#define IO_BTN_DRIVE					  6
#define IO_UITEM_FILES					  7
#define IO_UITEM_POPUP					  8
#define IO_UITEM_LINE					  9
#define IO_BTN_OK						 10
#define IO_BTN_HELP						 11
#define IO_CBOX_DISPASICON				 12
#define IO_RBTN_NEW						 13
#define IO_RBTN_FROMFILE				 14
#define IO_UITEM_RESULT					 15
#define IO_UITEM_LBOXNEW				 16
#define IO_UITEM_ICON					 17
#define IO_UITEM_ICONTXT				 18
#define IO_UITEM_OKOUTLINE				 19
#define IO_CBOX_LINK					 20
#define IO_TEXT_TYPE					 21
#define IO_TEXT_RESULT					 22
#define IO_TEXT_CREATE					 23
#define IO_TE_LABELEDIT					 24

#define IO_UITEM_FIRST				IO_UITEM_RESULT
#define IO_UITEM_LAST				IO_UITEM_OKOUTLINE


// Insert Object String IDs
#define IDS_IORESULTNEW					  1
#define IDS_IORESULTNEWICON				  2
#define IDS_IORESULTFROMFILE			  3
#define IDS_IORESULTFROMFILEICON		  4
#define IDS_IORESULTLINKFILE			  5
#define IDS_IORESULTLINKFILEICON		  6



/*************************************************************************
** PASTE SPECIAL DIALOG
*************************************************************************/


// Maximum number of link types
#define     PS_MAXLINKTYPES  8


typedef struct tagOLEUIPASTESPECIAL
{
    //These IN fields are standard across all OLEUI dialog functions.
    unsigned long   cbStruct;       //Structure Size
    unsigned long   dwFlags;        //IN-OUT:  Flags
    char           *lpszCaption;    //Dialog caption bar contents
    Point			ptPosition;		//Dialog position, {0,0} = center
    LPFNOLEUIHOOK   lpfnHook;       //Hook callback
    long            lCustData;      //Custom data to pass to hook

    //Specifics for OLEUIPASTESPECIAL.

    //IN  fields
    LPDATAOBJECT    lpSrcDataObj;   //Source IDataObject* (on the clipboard) for data to paste

    POLEUIPASTEENTRY arrPasteEntries; //OLEUIPASTEENTRY array which specifies acceptable formats. See
                                    //  OLEUIPASTEENTRY for more information.
    int             cPasteEntries;  //Number of OLEUIPASTEENTRY array entries

    unsigned int    *arrLinkTypes;  //List of link types that are acceptable. Link types are referred
                                    //  to using OLEUIPASTEFLAGS in arrPasteEntries
    int             cLinkTypes;     //Number of link types

    //OUT fields
    int             nSelectedIndex; //Index of arrPasteEntries[] that the user selected
    Boolean         fLink;          //Indicates if Paste or Paste Link was selected by the user
	PicHandle		hMetaPict;			//Handle to Metafile containing icon
									// and icon title selected by the user
									// Use the Metafile utility functions
									// defined in this header to
									// manipulate hMetaPict
} OLEUIPASTESPECIAL, *POLEUIPASTESPECIAL, *LPOLEUIPASTESPECIAL;


//API to bring up InsertObject dialog
STDAPI_(unsigned int) OleUIPasteSpecial(LPOLEUIPASTESPECIAL);


//Paste Special flags
#define PSF_SHOWHELP					0x00000001L
#define PSF_SELECTPASTE					0x00000002L
#define PSF_SELECTPASTELINK				0x00000004L
#define PSF_CHECKDISPLAYASICON			0x00000008L

// ****************
#define PSF_DISABLEDISPLAYASICON		0x00000010L
// ****************


//Paste Special specific error codes
#define OLEUI_IOERR_SRCDATAOBJECTINVALID      (OLEUI_ERR_STANDARDMAX+0)
#define OLEUI_IOERR_ARRPASTEENTRIESINVALID    (OLEUI_ERR_STANDARDMAX+1)
#define OLEUI_IOERR_ARRLINKTYPESINVALID       (OLEUI_ERR_STANDARDMAX+2)
#define OLEUI_PSERR_CLIPBOARDCHANGED          (OLEUI_ERR_STANDARDMAX+3)


//Paste Special Dialog identifiers
#define PS_BTN_OK						  1
#define PS_BTN_CANCEL					  2
#define PS_BTN_HELP						  3
#define PS_CBOX_DISPASICON				  4
#define PS_RBTN_PASTE					  5
#define PS_RBTN_PASTELINK				  6
#define PS_UITEM_RESULT					  7
#define PS_UITEM_LBOX					  8
#define PS_UITEM_SRCTEXT				  9
#define PS_UITEM_ICON					 10
#define PS_UITEM_ICONTXT				 11
#define PS_UITEM_OKOUTLINE				 12
#define PS_TEXT_SOURCE					 13
#define PS_TEXT_AS						 14
#define PS_TEXT_RESULT					 15
#define PS_TE_LABELEDIT					 16

#define PS_UITEM_FIRST				PS_UITEM_RESULT
#define PS_UITEM_LAST				PS_UITEM_OKOUTLINE

#define PS_ITEM_MAX					PS_TE_LABELEDIT


// Paste Special String IDs
#define IDS_PSPASTEDATA					  1
#define IDS_PSPASTEOBJECT				  2
#define IDS_PSPASTEOBJECTASICON			  3
#define IDS_PSPASTELINKDATA				  4
#define IDS_PSPASTELINKOBJECT			  5
#define IDS_PSPASTELINKOBJECTASICON		  6
#define IDS_PSNONOLE					  7
#define IDS_PSUNKNOWNTYPE				  8
#define IDS_PSUNKNOWNSRC				  9
#define IDS_PSUNKNOWNAPP				 10



/*************************************************************************
** EDIT LINKS DIALOG
*************************************************************************/


/* IOleUILinkContainer Interface
** -----------------------------
**    This interface must be implemented by container applications that
**    want to use the EditLinks dialog. the EditLinks dialog calls back
**    to the container app to perform the OLE functions to manipulate
**    the links within the container.
*/

#define LPOLEUILINKCONTAINER     IOleUILinkContainer *

#undef  INTERFACE
#define INTERFACE   IOleUILinkContainer

DECLARE_INTERFACE_(IOleUILinkContainer, IUnknown)
{
    BEGIN_INTERFACE
    //*** IUnknown methods ***/
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void * * ppvObj) PURE;
    STDMETHOD_(unsigned long,AddRef) (THIS) PURE;
    STDMETHOD_(unsigned long,Release) (THIS) PURE;

    STDMETHOD_(unsigned long,GetNextLink) (THIS_ unsigned long dwLink) PURE;
    STDMETHOD(SetLinkUpdateOptions) (THIS_ unsigned long dwLink, unsigned long dwUpdateOpt) PURE;
    STDMETHOD(GetLinkUpdateOptions) (THIS_ unsigned long dwLink, unsigned long * lpdwUpdateOpt) PURE;
    STDMETHOD(SetLinkSource) (THIS_
            unsigned long    dwLink,
            char *           lpszDisplayName,
            unsigned long    lenFileName,
            unsigned long *  pchEaten,
            Boolean          fValidateSource) PURE;
    STDMETHOD(GetLinkSource) (THIS_
            unsigned long    dwLink,
            char * *         lplpszDisplayName,
            unsigned long *  lplenFileName,
            char * *         lplpszFullLinkType,
            char * *         lplpszShortLinkType,
            Boolean *        lpfSourceAvailable,
            Boolean *        lpfIsSelected) PURE;
    STDMETHOD(OpenLinkSource) (THIS_ unsigned long dwLink) PURE;
    STDMETHOD(UpdateLink) (THIS_
            unsigned long    dwLink,
            Boolean          fErrorMessage,
            Boolean          fErrorAction) PURE;
    STDMETHOD(CancelLink) (THIS_ unsigned long dwLink) PURE;
};


typedef struct tagOLEUIEDITLINKS
{
    //These IN fields are standard across all OLEUI dialog functions.
    unsigned long   cbStruct;       //Structure Size
    unsigned long   dwFlags;        //IN-OUT:  Flags
    char           *lpszCaption;    //Dialog caption bar contents
    Point			ptPosition;		//Dialog position, {0,0} = center
    LPFNOLEUIHOOK   lpfnHook;       //Hook callback
    long            lCustData;      //Custom data to pass to hook

    //Specifics for OLEUIEDITLINKS.  All are IN-OUT unless otherwise spec.

	LPOLEUILINKCONTAINER lpOleUILinkContainer;	//IN: Interface to manipulate
												//links in the container
} OLEUIEDITLINKS, *POLEUIEDITLINKS, *LPOLEUIEDITLINKS;


//API Prototypes
STDAPI_(unsigned int) OleUIEditLinks(LPOLEUIEDITLINKS);


//Edit Links flags
#define ELF_SHOWHELP                	0x00000001L

// ****************
#define ELF_DISABLEUPDATENOW			0x00000002L
#define ELF_DISABLEOPENSOURCE			0x00000004L
#define ELF_DISABLECHANGESOURCE			0x00000008L
#define ELF_DISABLECANCELLINK			0x00000010L
// ****************


//Edit Links Dialog identifiers
#define EL_BTN_CLOSE					  1
#define EL_BTN_UPDATENOW				  2
#define EL_BTN_OPENSOURCE				  3
#define EL_BTN_CHANGESOURCE				  4
#define EL_BTN_BREAKLINK				  5
#define EL_BTN_HELP						  6
#define EL_RBTN_AUTOMATIC				  7
#define EL_RBTN_MANUAL					  8
#define EL_UITEM_LBOX					  9
#define EL_UITEM_LINKSOURCE				 10
#define EL_UITEM_LINKTYPE				 11
#define EL_UITEM_OKOUTLINE				 12
#define EL_TEXT_LINKS					 13
#define EL_TEXT_TYPE					 14
#define EL_TEXT_UPDATE					 15

#define EL_UITEM_FIRST				EL_UITEM_LBOX
#define EL_UITEM_LAST				EL_UITEM_OKOUTLINE

#define EL_ITEM_MAX					EL_UITEM_OKOUTLINE


// String IDs for Edit Links dialog
#define IDS_LINK_AUTO					  1
#define IDS_LINK_MANUAL					  2
#define IDS_LINK_UNKNOWN				  3
#define IDS_LINKS						  4
#define IDS_FAILED						  5
#define IDS_CHANGESOURCE				  6
#define IDS_INVALIDSOURCE				  7
#define IDS_CHANGEADDITIONALLINKS		  8
#define IDS_ERR_GETLINKSOURCE			  9
#define IDS_ERR_GETLINKUPDATEOPTIONS	 10
#define IDS_ERR_ADDSTRING				 11
#define IDS_BTN_OK                       12
#define IDS_BTN_CANCEL                   13
#define IDS_BTN_CLOSE                    14
#define IDS_BTN_OPEN                     15



/*************************************************************************
** CONVERT DIALOG
*************************************************************************/
typedef union tagOLEUIOLEFORMAT {
    unsigned long   dwOle2Format;		//OLE2 data format
	char		   *lpszOle1Format;		//OLE1 data format
} OLEUIOLEFORMAT;

typedef struct tagOLEUICONVERT
{
    //These IN fields are standard across all OLEUI dialog functions.
    unsigned long   cbStruct;       //Structure Size
    unsigned long   dwFlags;        //IN-OUT:  Flags
    char           *lpszCaption;    //Dialog caption bar contents
    Point			ptPosition;		//Dialog position, {0,0} = center
    LPFNOLEUIHOOK   lpfnHook;       //Hook callback
    long            lCustData;      //Custom data to pass to hook

    //Specifics for OLEUICONVERT.  All are IN-OUT unless otherwise spec.
    CLSID           clsid;            //Class ID sent in to dialog: IN only
    CLSID           clsidConvertDefault;  //Class ID to use as convert default: IN only
    CLSID           clsidActivateDefault;  //Class ID to use as activate default: IN only

    CLSID           clsidNew;         //Selected Class ID: OUT only
    unsigned long   dvAspect;         //IN-OUT, either DVASPECT_CONTENT or
                                      //DVASPECT_ICON

	Boolean			fIsOle1Object;    //IN only; true if object is OLE 1.0 object
									  //  TRUE:  use lpszOle1Format field in following union
									  //  FALSE: use dwOle2Format field in following union

	OLEUIOLEFORMAT	fmt;

    Boolean         fIsLinkedObject;  //IN only; true if object is linked
    char           *lpszUserType;     //IN: user type name of original class. We'll do lookup if it's NULL.
                                      //This gets freed on exit.
    Boolean         fObjectsIconChanged;  // OUT; TRUE if ChangeIcon was called (and not cancelled)

	PicHandle       hMetaPict;        //IN-OUT: METAFILEPICT containing iconic aspect.
	char			*pszDefLabel;     //IN-OUT: default label to use for icon.
									  //  if NULL, the short user type name
									  //  will be used. if the object is a
									  //  link, the caller should pass the
									  //  DisplayName of the link source
									  //  This gets freed on exit.


} OLEUICONVERT, *POLEUICONVERT, *LPOLEUICONVERT;


//API prototype
STDAPI_(unsigned int) OleUIConvert(LPOLEUICONVERT);
STDAPI_(Boolean) OleUICanConvertOrActivateAs(
		REFCLSID    	rClsid,
		Boolean        	fIsLinkedObject,
		Boolean			fIsOle1Object,
		OLEUIOLEFORMAT 	Format
);

//Convert Dialog flags

// IN only: Shows "HELP" button
#define CF_SHOWHELP						0x00000001L

// IN only: lets you set the convert default object - the one that is
// selected as default in the convert listbox.
#define CF_SETCONVERTDEFAULT			0x00000002L


// IN only: lets you set the activate default object - the one that is
// selected as default in the activate listbox.

#define CF_SETACTIVATEDEFAULT			0x00000004L


// IN/OUT: Selects the "Convert To" radio button, is set on exit if
// this button was selected
#define CF_SELECTCONVERTTO				0x00000008L

// IN/OUT: Selects the "Activate As" radio button, is set on exit if
// this button was selected
#define CF_SELECTACTIVATEAS				0x00000010L

// ****************
#define CF_DISABLEDISPLAYASICON			0x00000020L
#define CF_DISABLEACTIVATEAS			0x00000040L
// ****************


//Convert specific error codes
#define OLEUI_CTERR_CLASSIDINVALID      (OLEUI_ERR_STANDARDMAX+1)
#define OLEUI_CTERR_DVASPECTINVALID     (OLEUI_ERR_STANDARDMAX+2)
#define OLEUI_CTERR_CBFORMATINVALID     (OLEUI_ERR_STANDARDMAX+3)
#define OLEUI_CTERR_HMETAPICTINVALID    (OLEUI_ERR_STANDARDMAX+4)
#define OLEUI_CTERR_STRINGINVALID       (OLEUI_ERR_STANDARDMAX+5)


//Convert Dialog identifiers
#define CV_BTN_OK						  1
#define CV_BTN_CANCEL					  2
#define CV_BTN_HELP						  3
#define CV_CBOX_DISPASICON				  4
#define CV_RBTN_CONVERT					  5
#define CV_RBTN_ACTIVATE				  6
#define CV_UITEM_RESULT					  7
#define CV_UITEM_LBOX					  8
#define CV_UITEM_SRCTEXT				  9
#define CV_UITEM_ICON					 10
#define CV_UITEM_ICONTXT				 11
#define CV_UITEM_OKOUTLINE				 12
#define CV_TEXT_SOURCE					 13
#define CV_TEXT_AS						 14
#define CV_TEXT_RESULT					 15
#define CV_TE_LABELEDIT					 16

#define CV_UITEM_FIRST				CV_UITEM_RESULT
#define CV_UITEM_LAST				CV_UITEM_OKOUTLINE

#define CV_ITEM_MAX					CV_TE_LABELEDIT


// String IDs for Convert dialog
#define IDS_CVRESULTCONVERTLINK			  1
#define IDS_CVRESULTCONVERTTO			  2
#define IDS_CVRESULTNOCHANGE			  3
#define IDS_CVRESULTDISPLAYASICON		  4
#define IDS_CVRESULTACTIVATEAS			  5
#define IDS_CVRESULTACTIVATEDIFF		  6



/*************************************************************************
** Update Links DIALOG
*************************************************************************/

#define UL_BTN_STOP						  1
#define UL_UITEM_METER					  2
#define UL_TEXT_PERCENT					  3

STDAPI_(Boolean) OleUIUpdateLinks(LPOLEUILINKCONTAINER lpOleUILinkCntr, int cLinks);



/*************************************************************************
** Miscellaneous utility functions
*************************************************************************/

typedef struct VerbRec {
	short 	menuID;
	short 	menuItem;
	long	verb;
} VerbRec;

typedef struct VerbMenuRec {
	short 	ConvertMenuID;
	short 	ConvertMenuItem;
	short	verbCount;
	VerbRec	*verbRec;
} VerbMenuRec;
	
//API prototype
STDAPI_(Boolean) OleUIAddVerbMenu(
		LPOLEOBJECT 	lpOleObj,
		char* 			lpszShortType,
		MenuHandle 		hEditMenu,
		short			idEditMenu,
		MenuHandle		hVerbMenu,
		short			idVerbMenu,
		unsigned int	uVerbPos,
		Boolean 		bAddConvert,
		VerbMenuRec*	pVerbMenuRec
);

STDAPI_(Boolean) OleUIFindVerbConvert(
		VerbMenuRec* 	pVerbMenuRec,
		short 			menuID,
		short 			menuItem,
		Boolean*		pConvert,
		long* 			pVerb
);

// String IDs
#define IDS_OLE2UIUNKNOWN       		11000
#define IDS_OLE2UILINK          		11001
#define IDS_OLE2UIOBJECT        		11002
#define IDS_OLE2UIEDIT          		11003
#define IDS_OLE2UICONVERT       		11004
#define IDS_OLE2UIEDITLINKCMD_1VERB     11005
#define IDS_OLE2UIEDITOBJECTCMD_1VERB   11006
#define IDS_OLE2UIEDITLINKCMD_NVERB     11007
#define IDS_OLE2UIEDITOBJECTCMD_NVERB   11008
#define IDS_OLE2UIEDITNOOBJCMD  		11009



/*************************************************************************
** BUSY DIALOG
*************************************************************************/


typedef struct tagOLEUIBUSY {

    //These IN fields are standard across all OLEUI dialog functions.
    unsigned long   	cbStruct;         //Structure Size
    unsigned long       dwFlags;          //IN-OUT:  Flags ** NOTE ** this dialog has no flags
    char				*lpszCaption;      //Dialog caption bar contents
    Point				ptPosition;		//Dialog position, {0,0} = center
    LPFNOLEUIHOOK   	lpfnHook;         //Hook callback
    long          		lCustData;        //Custom data to pass to hook

    //Specifics for OLEUIBUSY.
    ProcessSerialNumberPtr      pPSN;            //IN: HTask which is blocking
} OLEUIBUSY, *POLEUIBUSY, *LPOLEUIBUSY;

//API prototype
STDAPI_(unsigned int) OleUIBusy(LPOLEUIBUSY);

// Flags for this dialog

// IN only: Disables "Cancel" button
#define BZ_DISABLECANCELBUTTON          0x00000001L

// IN only: Disables "Switch To..." button
#define BZ_DISABLESWITCHTOBUTTON        0x00000002L

// IN only: Disables "Retry" button
#define BZ_DISABLERETRYBUTTON           0x00000004L

// IN only: Generates a "Not Responding" dialog as opposed to the
// "Busy" dialog.  The wording in the text is slightly different, and
// the "Cancel" button is grayed out if you set this flag.
#define BZ_NOTRESPONDINGDIALOG          0x00000008L

// Busy specific error/return codes
#define OLEUI_BZERR_HTASKINVALID     (OLEUI_ERR_STANDARDMAX+0)

// SWITCHTOSELECTED is returned when user hit "switch to"
#define OLEUI_BZ_SWITCHTOSELECTED    (OLEUI_ERR_STANDARDMAX+1)

// RETRYSELECTED is returned when user hit "retry"
#define OLEUI_BZ_RETRYSELECTED       (OLEUI_ERR_STANDARDMAX+2)

// CALLUNBLOCKED is returned when call has been unblocked
#define OLEUI_BZ_CALLUNBLOCKED       (OLEUI_ERR_STANDARDMAX+3)

// String IDs
#define IDS_BZRESULTTEXTNOTRESPONDING		11010
#define IDS_BZRESULTTEXTBUSY				11011
#define IDS_APPBLOCKED						11012

#define OLEUI_ICONFRAME 				3
#define OLEUI_LBOXFRAME 				3



/*************************************************************************
** PROMPT USER DIALOGS
*************************************************************************/

// The following Dialogs are message dialogs used by OleUIPromptUser API
#define IDD_INVALIDSOURCE			10020
#define IDD_LINKSOURCEUNAVAILABLE   10021
#define IDD_SERVERNOTREG        	10022
#define IDD_LINKTYPECHANGED     	10023
#define IDD_SERVERNOTFOUND      	10024
#define IDD_OUTOFMEMORY         	10025

#define PU_TEXT_MESSAGE				1
#define PU_BTN_LINKS				2
#define PU_BTN_TEXT					3
#define PU_BTN_CONVERT				4
#define PU_BTN_BROWSE				5
#define PU_BTN_YES					6
#define PU_BTN_NO					7

#define PU_BTN_FIRST				2

/* inside ole2ui.c */

#ifdef _MSC_VER
short _cdecl OleUIPromptUser(int nTemplate, ...);
#else
short OleUIPromptUser(int nTemplate, ...);
#endif



/*************************************************************************
** MESSAGE FILTER ROUTINES
*************************************************************************/

/* a significant message is consider a mouse click or keyboard input. */
#define UserInputMask		mDownMask | mUpMask | keyDownMask | keyUpMask | autoKeyMask
#define UrgentEvtMask		updateMask | activMask | osMask

// Message Pending callback procedure
typedef Boolean (*MSGPENDINGPROC)(long userData);

// HandleInComingCall callback procedure
typedef unsigned long (*HANDLEINCOMINGCALLBACKPROC)
    (
        unsigned long       	 dwCallType,
        ProcessSerialNumberPtr   htaskCaller,
        unsigned long       	 dwTickCount,
        LPINTERFACEINFO     	 lpInterfaceInfo
    );

STDAPI_(void) OleStdMessageFilterInitInterfaces(void);

STDAPI_(LPMESSAGEFILTER) OleStdMsgFilter_Create(
        char* 			szAppName,
        MSGPENDINGPROC lpfnCallback,
        LPFNOLEUIHOOK  lpfnOleUIHook,         // Busy dialog hook callback
        long			userData);

STDAPI_(void) OleStdMsgFilter_SetInComingCallStatus(
        LPMESSAGEFILTER lpThis, unsigned long dwInComingCallStatus);

STDAPI_(unsigned long) OleStdMsgFilter_GetInComingCallStatus(
        LPMESSAGEFILTER lpThis);

STDAPI_(HANDLEINCOMINGCALLBACKPROC)
    OleStdMsgFilter_SetHandleInComingCallbackProc(
        LPMESSAGEFILTER             lpThis,
        HANDLEINCOMINGCALLBACKPROC  lpfnHandleInComingCallback);

STDAPI_(Boolean) OleStdMsgFilter_EnableBusyDialog(
        LPMESSAGEFILTER lpThis, Boolean fEnable);

STDAPI_(Boolean) OleStdMsgFilter_EnableNotRespondingDialog(
        LPMESSAGEFILTER lpThis, Boolean fEnable);



#if defined(__cplusplus) && !defined(__MWERKS__)
}
#endif


#endif //_OLE2UI_H_

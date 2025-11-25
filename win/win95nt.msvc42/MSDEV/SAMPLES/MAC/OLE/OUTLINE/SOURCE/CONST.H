/*****************************************************************************\
*                                                                             *
*    ApplicationConsts.h                                                      *
*                                                                             *
*    OLE Version 2.0 Sample Code                                              *
*                                                                             *
*    Copyright (c) 1992-1994, Microsoft Corp. All rights reserved.            *
*                                                                             *
\*****************************************************************************/

#ifndef __CONST_H__
#define __CONST_H__

#if !defined(rez) && qOle
#include "Ole2.h"
#endif


// OLDNAME: ApplicationConsts.h

#define kStackNeeded				20			// stack space needed in K

#define kAEOutlineEvent				'OUTL'
#define kAEDoCmd					'Cmd '

#define kPrefFileName				"\pOutline Preferences"

#define kToolbar_WIND				1024
#define kToolbar_WDEF				1024
#define kToolbar_PICT				1024
#define kFormulabar_PICT			1025

#define kMenuItemToCmdResType		'CMD '
#define kMenuItemToCmdResID			256

#define kAppBuzzword_STRs			256
	#define kQuiting_AppBuzzword	1
	#define kClosing_AppBuzzword	2

#define kNewDocumentName_STR		256

#define kSave_DLOG					256

#if defined(rez) || defined(derez)
	type kMenuItemToCmdResType {

		integer = $$CountOf(CmdsArray);

		wide array CmdsArray {
			integer;
			integer;
			longint;
		};
	};
#else
	struct MenuCommand {
		short	menuID;
		short	menuItem;
		long	menuCmd;
	};

	typedef struct MenuCommand MenuCommand, *MenuCommandPtr, **MenuCommandHandle;
#endif

// OLDNAME: Commands.h
#define kApple_MENU			256

#define cmdNull				0

#define	cmdQuit				1
#define	cmdNew				2
#define	cmdOpen				3
#define	cmdClose			4
#define	cmdSave				5
#define	cmdSaveAs			6
#define	cmdRevert			7
#define	cmdPageSetup		8
#define	cmdPrint			9

#define	cmdUndo				16
#define	cmdCut				18
#define	cmdCopy				19
#define	cmdPaste			20
#define	cmdClear			21
#define cmdSelectAll		23

#define cmdToolbar			32

#define cmdToolbarAtTop			33
#define cmdToolbarAtBottom		34
#define cmdToolbarFloating		35
#define cmdToolbarHidden		36

#define cmdFormulaBar			48
#define cmdFormulaBarAtTop		49
#define cmdFormulaBarAtBottom	50
#define cmdFormulaBarFloating	51

#define cmdHeadings				64
#define cmdHeadingsShow			65
#define cmdHeadingsHide			66

#if qDebug
#define cmdDebug				128
#define cmdAsserts				129
#define cmdRejectMessages		130
#endif

#define	cmdAbout			256

// OLDNAME: OleConsts.h
/* CLASS ID CONSTANTS (GUID's)
**	  OLE2NOTE: these class id values are allocated out of a private pool
**	  of GUID's allocated to the OLE 2.0 development team. GUID's of
**	  the following range have been allocated to OLE 2.0 sample code:
**		   00000400-0000-0000-C000-000000000046
**		   000004FF-0000-0000-C000-000000000046
**
**	  values reserverd thus far:
**			00000400				-- OLE 2.0 Server Sample Outline
**			00000401				-- OLE 2.0 Container Sample Outline
**			00000402				-- OLE 2.0 In-Place Server Outline
**			00000403				-- OLE 2.0 In-Place Container Outline
**			00000404 : 0000040F		-- reserved for Outline Series
**			000004F0 : 000004FF		-- Developer Relations Series
*/
#if !defined(rez) && qOle

DEFINE_OLEGUID(CLSID_OutlineServer,				0x00000400L, 0, 0);
DEFINE_OLEGUID(CLSID_OutlineContainer,			0x00000401L, 0, 0);
DEFINE_OLEGUID(CLSID_OutlineInPlaceServer,		0x00000402L, 0, 0);
DEFINE_OLEGUID(CLSID_OutlineInPlaceContainer,	0x00000403L, 0, 0);

#if qOleContainerApp
DEFINE_OLEGUID(IID_IOleUILinkContainer,			0x000004FFL, 0, 0);
#endif

#endif // !defined(rez) && qOle

#if !qOleInPlace
	#define kOutlineServerType						'OUTS'
	#define kOutlineServerTypeStr					"OUTS"
	#define kOutlineServerFullUserTypeName			"OLE 2.0 Server Sample Outline"
	#define kOutlineServerShortUserTypeName			"Outline"
	#define kOutlineServerAppName					"OLE 2.0 Outline Server"
	#define kOutlineServerProgID					"OLE2SvrOutl"
	#define kOutlineServerCLSID						"{00000400-0000-0000-C000-000000000046}"
	
	#define kOutlineContainerType					'OUTC'
	#define kOutlineContainerTypeStr				"OUTC"
	#define kOutlineContainerFullUserTypeName		"OLE 2.0 Container Sample Outline"
	#define kOutlineContainerShortUserTypeName		"Outline"
	#define kOutlineContainerAppName				"OLE 2.0 Outline Container"
	#define kOutlineContainerProgID					"OLE2CntrOutl"
	#define kOutlineContainerCLSID					"{00000401-0000-0000-C000-000000000046}"
#endif

#define kOutlineInPlaceServerType					'OTIS'
#define kOutlineInPlaceServerTypeStr				"OTIS"
#define kOutlineInPlaceServerFullUserTypeName		"OLE 2.0 InPlace Server Sample Outline"
#define kOutlineInPlaceServerShortUserTypeName		"Outline"
#define kOutlineInPlaceServerAppName				"OLE 2.0 In-Place Outline Server"
#define kOutlineInPlaceServerProgID					"OLE2ISvrOutl"
#define kOutlineInPlaceServerCLSID					"{00000402-0000-0000-C000-000000000046}"

#define kOutlineInPlaceContainerType				'OTIC'
#define kOutlineInPlaceContainerTypeStr				"OTIC"
#define kOutlineInPlaceContainerFullUserTypeName	"OLE 2.0 InPlace Container Sample Outline"
#define kOutlineInPlaceContainerShortUserTypeName	"Outline"
#define kOutlineInPlaceContainerAppName				"OLE 2.0 In-Place Outline Container"
#define kOutlineInPlaceContainerProgID				"OLE2ICntrOutl"
#define kOutlineInPlaceContainerCLSID				"{00000403-0000-0000-C000-000000000046}"

// if we are inplace remap some of the application constants
#if qOleInPlace
	#define kOutlineServerType						kOutlineInPlaceServerType
	#define kOutlineServerTypeStr					kOutlineInPlaceServerTypeStr
	#define kOutlineServerFullUserTypeName			kOutlineInPlaceServerFullUserTypeName
	#define kOutlineServerShortUserTypeName			kOutlineInPlaceServerShortUserTypeName
	#define kOutlineServerAppName					kOutlineInPlaceServerAppName
	#define kOutlineServerProgID					kOutlineInPlaceServerProgID
	#define kOutlineServerCLSID						kOutlineInPlaceServerCLSID
	
	#define kOutlineContainerType					kOutlineInPlaceContainerType
	#define kOutlineContainerTypeStr				kOutlineInPlaceContainerTypeStr
	#define kOutlineContainerFullUserTypeName		kOutlineInPlaceContainerFullUserTypeName
	#define kOutlineContainerShortUserTypeName		kOutlineInPlaceContainerShortUserTypeName
	#define kOutlineContainerAppName				kOutlineInPlaceContainerAppName
	#define kOutlineContainerProgID					kOutlineInPlaceContainerProgID
	#define kOutlineContainerCLSID					kOutlineInPlaceContainerCLSID
#endif

#define kOutlineType								'OUTL'
#define kOutlineTypeStr								"OUTL"

#define kOutlineDocumentFileType					'OUTL'				// document storage type
#define kOutlineDocumentFile						"OUTL"

#define kOutlineContainerDocumentFileType			'OUTC'				// container document storage type
#define kOutlineContainerDocumentFile				"OUTC"

#if qOleInPlace
#if qOleServerApp
		#define CLSID_Application		CLSID_OutlineInPlaceServer
#elif qOleContainerApp
		#define CLSID_Application		CLSID_OutlineInPlaceContainer
#endif
#else
#if qOleServerApp
		#define CLSID_Application		CLSID_OutlineServer
#elif qOleContainerApp
		#define CLSID_Application		CLSID_OutlineContainer
#endif
#endif

// Codes for CallBack events
#if !defined(rez)

enum OLE_NOTIFICATION
{
	OLE_ONDATACHANGE,
	OLE_ONSAVE,
	OLE_ONRENAME,
	OLE_ONCLOSE
};

typedef enum OLE_NOTIFICATION OLE_NOTIFICATION;

/* Codes to indicate mode of storage for an object.
**    Mode of the storage is modified by the IPersistStorage methods:
**		Save, HandsOffStorage, and SaveCompleted.
*/
enum STGMODE {
	STGMODE_NORMAL		= 0,
	STGMODE_NOSCRIBBLE	= 1,
	STGMODE_HANDSOFF	= 2
};

typedef enum STGMODE STGMODE;
#endif

#define kDefaultObjectHeight			60			// default object height
#define kDefaultObjectWidth				100			// default object width

#define kHandleSize						5			// size of object resize handle

#define	kHitTestDelta					4			// border threshold to start drag
#define kDragDelay						500			// time delay (in msec) before drag should start
#define kDragMinDist     				3			// min. distance (radius) before drag should start
#define kScrollDelay     				50			// time delay (in msec) before scroll should start
#define kScrollInset     				11			// Border inset distance to start drag scroll
#define kScrollInterval  				50			// scroll interval time (in msec)

#if !defined(rez)

/* Flags to control direction for drag scrolling */
typedef enum tagSCROLLDIR {
    SCROLLDIR_NULL          = 0,
    SCROLLDIR_UP            = 1,
    SCROLLDIR_DOWN          = 2,
    SCROLLDIR_RIGHT         = 3,    // currently not used
    SCROLLDIR_LEFT          = 4     // currently not used
} SCROLLDIR;

// Document initialization type
enum OleDocumentType {
	kUnknownDocumentType,			// new doc created but not yet initialized
	kNewDocumentType,				// init from scratch (new doc)
	kFromFileDocumentType,			// init from a file (open doc)
	kEmbeddedDocumentType,			// init from an IStorage* of an embedded obj
	kFromStorageDocumentType		// init from an IStorage* with doc bit set
};

typedef enum OleDocumentType OleDocumentType;

// YNCResult is a type used to indicate a result from
// a user operation. YNC means yes/no/cancel, but yes
// and no can also have synonyms like Save and Discard.
enum { noResult, yesResult, cancelResult };


typedef short YNCResult;
#endif

// OLDNAME: OleServerDocumentConst.h

#define kServerMenu_STRs		2048
#define kServerMenu_STRs_Close	1
#define kServerMenu_STRs_Save	2
#define	kServerMenu_STRs_SaveAs	3
#define kServerMenu_STRs_Quit	4

// OLDNAME: OutlineConst.h
#define kPrefSize				512			// preferred memory size in K
#define kMinSize				512			// minimum memory size in K

#define kOutline_WIND			1025

#define kMenuBar_MBAR			1024

// kApple_MENU
#define kFile_MENU				1025
#define kEdit_MENU				1026
#define kLines_MENU				1027
#define kName_MENU				1028
#define kOptions_MENU			1029
#define kWindows_MENU				1031

#if qDebug

#define kDebug_MENU					1030

#if !qOle
#define kDebugMenuName				"Debug"
#endif

#if qOleContainerApp && !qOleInPlace
#define kDebugMenuName				"DbgCntr"
#endif

#if qOleContainerApp && qOleInPlace
#define kDebugMenuName				"DbgICntr"
#endif

#if qOleServerApp && !qOleInPlace
#define kDebugMenuName				"DbgSvr"
#endif

#if qOleServerApp && qOleInPlace
#define kDebugMenuName				"DbgISvr"
#endif

#endif	// qDebug

#define kToolbar_MENU			128
#define kToolbar_MENU_Char		"\0x80"

#define kFormulaBar_MENU			129
#define kFormulaBar_MENU_Char		"\0x81"

#define kHeadings_MENU				130
#define kHeadings_MENU_Char			"\0x82"

#define kObject_MENU				131
#define kObject_MENU_Char			"\0x83"
#define kTemporaryFileNamePrefix_STR	1024

#define kOutlineFormat_CSTR				1024
#define kOutlineResult_CSTR				1025
#define kTextFormat_CSTR				1026
#define kTextResult_CSTR				1027
#define	kObjectNamePrefix_CSTR			1028
#define kEmbeddedObjectFormat_CSTR		1029
#define kEmbeddedObjectResult_CSTR		1030
#define kPictFormat_CSTR				1031
#define kPictResult_CSTR				1032
#define kDIBFormat_CSTR					1033
#define kDIBResult_CSTR					1034
#define kBitmapFormat_CSTR				1035
#define kBitmapResult_CSTR				1036
#define kLinkSourceFormat_CSTR			1037
#define kLinkSourceResult_CSTR			1038
#define kNewNamePrefix_CSTR				1039

#define cmdNewLine						1024
#define cmdIndentLine					1025
#define cmdUnIndentLine					1026
#define cmdDoubleClick					1027
#define cmdPasteLink					1028
#define cmdPasteSpecial					1029
#define cmdInsertObject					1030
#define cmdLinks						1031
#define cmdObject						1032
#define cmdShowName						1033
#define cmdOpenVerb						1034

// cursor resource
#define kCursorBackwardResize			101
#define kCursorForwardResize			102
#define kCursorHorizResize				103
#define kCursorVertResize				104
#define kCursorDragNone					501
#define kCursorDragCopy					502
#define kCursorDragMove					503
#define kCursorDragLink					504
#define kCursorDragScroll				505

// dialog resource
#define kName_DLOG						600
#define kAbout_DLOG						601

// dialog items constants
#define ID_BTN_CLOSE					1
#define ID_BTN_DEFINE					2
#define ID_BTN_DELETE					3
#define	ID_BTN_GOTO						4
#define ID_UITEM_NAMELIST				5
#define ID_TE_NAME						6
#define ID_TEXT_LINERANGE				7
#define ID_TE_FROM						8
#define ID_TEXT_TO						9
#define ID_TE_TO						10




#endif

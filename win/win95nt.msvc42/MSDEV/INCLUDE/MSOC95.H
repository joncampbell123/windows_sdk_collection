#ifdef __cplusplus		// that's two underscores...
   extern "C" {
#endif

// Beta Version 0.04
//////////////////////////////////////////////////////////////////////////
// MSOC95.H
// -----------------------------------------------------------------------
// Copyright (c) 1985-1995, Microsoft Corporation
//
// Abstract:
//
//   APIs for interfacing with Microsoft Office95.
// 
//////////////////////////////////////////////////////////////////////////
//

// Error return codes:

// If there is a problem loading the DLL:
#define MSOC_INVALID_DLL			0xE0000001L

// If the HMSOC value passed in cannot be locked down:
#define MSOC_INVALID_HANDLE			0xE0000002L

// If the one of the data values passed in is invalid
// (Usually the pointer to data member to recieve the return information)
#define MSOC_INVALID_DATA			0xE0000003L

// Unable to locate a procedure in OFFICE.DLL during load time
// You may have an out-of-date DLL
#define MSOC_INVALID_PROCEDURE		0xE0000004L

// Failed DLL initialization
#define MSOC_FAILED_INIT			0xE0000005L

// msocLoad failed to read in values from storage
#define MSOC_FAILED_LOAD			0xE0000006L

// msocStore failed to write values out to storage
#define MSOC_FAILED_STORE			0xE0000007L

// invalid window passed in to msocShowDocPropsDialog
#define MSOC_INVALID_WINDOW         0xE0000008L

// msocShowDocPropsDialog failed to bring up dialog
#define MSOC_FAILED_DIALOG			0xE0000009L

// Invalid storage passed in to msocLoad or msocStore
#define MSOC_INVALID_STORAGE        0xE000000AL

// Unbalanced msocGetThumbnail/msocReleasThumbnail
#define MSOC_ERROR_THUMBNAILS_REMAIN	0xE000000BL

// Unbalanced msocCreate/msocDelete
#define MSOC_ERROR_OBJECTS_REMAIN		0xE000000CL

// OFFICE.DLL failed to create objects
#define MSOC_FAILED_CREATE				0xE000000DL

// msocInit was called a second time. This is not an error.
#define MSOC_ALREADY_INITIALIZED		0xE000000EL

// Unmatched, or improper calls to msocAllocateCustomList and msocFreeCustomList
#define MSOC_ERROR_LIST_ITEMS_REMAIN	0xE000000FL

// Office was not installed properly...
#define MSOC_OFFICE_NOT_INSTALLED		0xE0000010L

/////////////////////////////////////////////////////////////////////////////
// Type Declaration
//

// HMSOC - Core object. Created via msocCreate. Deleted via msocDelete
typedef HANDLE HMSOC;

#if defined(__offcapi_h__)
#define OFFICEAPI DLLEXPORT
#else

// Note about tagSINAIL:
//
// if cftag is
//   0 - pFMTID is NULL i.e. no format name
//  -1 - Windows built-in Clipboard format. pFMTID points to a DWORD
//  -2 - Macintosh Format Value.            pFMTID points to a DWORD
//  -3 - FMTID.                             pFMTID points to 16 bytes
//  >0 - Length of string.                  pFMTID points to cftag bytes
//
// If this structure is filled in by msocGetThumbnail, pbFMTID and
// pbData were allocated by Office.DLL, and must be deallocated by
// calling msocReleaseThumbnail.
//
typedef struct tagSINAIL
{
   DWORD cbData;     // size of *pdata
   DWORD cftag;      // either 0,-1,-2,-3, or positive. This decides the size of pFMTID.
   BYTE *pbFMTID;    // bytes representing the FMTID
   BYTE *pbData;     // bytes representing the data
} SINAIL;
typedef SINAIL FAR * LPSINAIL;

#define OFFICEAPI extern
#define OIO_ANSI                0x0001 // The storage is an ANSI storage (UNICODE is the default)
#define OIO_SAVESIMPLEDOCFILE   0x0004 // The storage is a simple DOC file.

#endif

////
//////////////////////////////////////////////////////////////////////////////
////	File initialization and processing functions
//////////////////////////////////////////////////////////////////////////////
////

//////////////////////////////////////////////////////////////////////////////
// msocInit
//
//  Should be the first call made into this DLL. It initializes the system,
//  and sets up the language to use. It should only be called once. If it
//  is called a second time, it will return FALSE, and GetLastError()
//  will return MSOC_ALREADY_INITIALIZED.
//
//	hinst - Instance handle to current application.
//
//	lcid - Language identifier.
//

OFFICEAPI BOOL msocInit (HINSTANCE hinst, LCID lcid);

//////////////////////////////////////////////////////////////////////////////
// msocCreate
//
//  Creates base HMSOC object to be used with all other functions. You 
//  must delete these handles via msocDelete.
//
//	prglpfn - array of function pointers:
//		[0] = Code Page Conversion
//		[1] = Convert an sz to a double
//		[2] = Convert a double to an sz
//
// If prglpfn is NULL, then adaquate default functions will be used. If
// prglpfn is non NULL, then you -must- provide a Code Page Converter.
// If you don't provide the floating point converters, then the 'Number'
// data type of the 'Custom' property sheet will not allow floating
// point values. If you provide one floating point converter, you must
// provide both.
//
//------------------------------------------------------------------------
//  Code Page Conversion
//
//  rglpfn[0] = (BOOL) (OFC_CALLBACK *lpfnFCPConvert) (
//			LPSTR lpsz,
//			DWORD dwFrom,
//			DWORD dwTo,
//			BOOL fMacintosh)
//
//    lpsz is a 0 terminated C string, dwFrom is the code page
//    lpsz is currently stored as, dwTo is the code page it should
//    be converted to, fMacintosh indicates whether dwFrom is a Mac
//    or Windows code page identifier.
//------------------------------------------------------------------------
//  Convert an sz to a double
//
//  rglpfn[1] = (BOOL) (OFC_CALLBACK *lpfnFSzToNum)(
//			double *lpdbl,
//			LPSTR lpszNum)
//
//   lpdbl - pointer to a double, this is set by the app
//   lpszNum - zero-terminated string representing the number
//------------------------------------------------------------------------
//  Convert a double to an sz
//
//  rglpfn[2] = (BOOL) (OFC_CALLBACK *lpfnFNumToSz)(
//				double *lpdbl,
//				LPSTR lpszNum,
//				DWORD cbMax)
//
//   lpdbl   - pointer to a double
//   lpszNum - on return a zero-terminated string representing the number
//   cbMax   - Max number of bytes in lpszNum
//------------------------------------------------------------------------
//

OFFICEAPI HMSOC msocCreate (void *prglpfn[]);

//////////////////////////////////////////////////////////////////////////////
// msocLoad
//
//  Loads document property values from the specified storage.
//
//	hmsoc - handle returned by msocCreate
//
//	lpStorage - an LPSTORAGE that has already been opened
//
//	dwFlags - flags that specify the format of the storage to be
//		loaded. Default is UniCode and full structured storage. For
//		compatibility with older document files, you may supply one
//		or more of the following values:
//			OIO_ANSI
//			OIO_SAVESIMPLEDOCFILE
//

OFFICEAPI BOOL msocLoad (HMSOC hmsoc, LPSTORAGE lpStorage, DWORD dwFlags);

//////////////////////////////////////////////////////////////////////////////
// msocStore
//
//  Saves document property values into the specified storage
//
//	hmsoc - handle returned by msocCreate
//
//	lpStorage - an LPSTORAGE that has already been opened
//
//	dwFlags - flags that specify the format of the storage to be
//		loaded. Default is UniCode and full structured storage. For
//		compatibility with older document files, you may supply one
//		or more of the following values:
//			OIO_ANSI
//			OIO_SAVESIMPLEDOCFILE
//

OFFICEAPI BOOL msocStore (HMSOC hmsoc, LPSTORAGE lpStorage, DWORD dwFlags);

//////////////////////////////////////////////////////////////////////////////
// msocDelete
//
//  Delete the specified HMSOC from the system and release resources.
//
//	hmsoc - handle returned by msocCreate
//

OFFICEAPI BOOL msocDelete (HMSOC hmsoc);

////
//////////////////////////////////////////////////////////////////////////////
////	Value Query/Set functions
//////////////////////////////////////////////////////////////////////////////
////

// String Values
#define MSOC_STRING_FIRST		0
#define MSOC_INFO_TITLE			0
#define MSOC_INFO_SUBJECT		1
#define MSOC_INFO_AUTHOR		2
#define MSOC_INFO_KEYWORDS		3
#define MSOC_INFO_COMMENTS		4
#define MSOC_INFO_TEMPLATE		5
#define MSOC_INFO_LASTAUTH		6
#define MSOC_INFO_REVISION		7
#define MSOC_INFO_APPNAME		8

#define MSOC_INFO_CATEGORY		9
#define MSOC_INFO_FORMAT		10
#define MSOC_INFO_MANAGER		11
#define MSOC_INFO_COMPANY		12
#define MSOC_STRING_LAST		12

// Time Values
#define MSOC_TIME_FIRST			20
#define MSOC_INFO_TOTALEDIT		20
#define MSOC_INFO_LASTPRINT		21
#define MSOC_INFO_CREATION		22
#define MSOC_INFO_LASTSAVE		23
#define MSOC_TIME_LAST			23

// Integer Values
#define MSOC_INT_FIRST			30
#define MSOC_INFO_PAGES			30
#define MSOC_INFO_WORDS			31
#define MSOC_INFO_CHARS			32
#define MSOC_INFO_SECURITY		33

#define MSOC_INFO_BYTES			34
#define MSOC_INFO_LINES			35
#define MSOC_INFO_PARAS			36
#define MSOC_INFO_SLIDES		37
#define MSOC_INFO_NOTES			38
#define MSOC_INFO_HIDDENSLIDES	39
#define MSOC_INFO_MMCLIPS		40
#define MSOC_INT_LAST			40

// Special Time Value:
#define MSOC_INFO_TOTALEDIT_MINUTES	50

//////////////////////////////////////////////////////////////////////////////
// msocGet
//
// Gets a property from the document. 'item' specifies
// the value to get (see the MSOC_INFO_... values above), lpItem points to
// the storage location, and cbItem is the size of the item.
//
//	hmsoc - Handle returned from msocCreate
//
//	item - index to item type to retrieve
//
//	lpItem - Pointer to storage for retrieved data
//
//	pcbItem - Pointer to UINT that specifies size of storage pointed to
//		by 'lpItem'. If lpItem is NULL, then this will return the size
//		needed by the item index indicated.
//

OFFICEAPI INT msocGet (HMSOC hmsoc, UINT item, LPVOID lpItem, UINT *pcbItem);

//////////////////////////////////////////////////////////////////////////////
// msocSet
//
// Sets a property in the document. 'item' specifies
// the value to set (see the MSOC_INFO_... values above). lpItem points
// to the item.
//
//	hmsoc - Handle returned from msocCreate
//
//	item - index to item type to retrieve
//
//	lpItem - Pointer to item data to use.
//

OFFICEAPI BOOL msocSet (HMSOC hmsoc, UINT item, LPVOID lpItem);


////
//////////////////////////////////////////////////////////////////////////////
////	Thumbnail Functions
//////////////////////////////////////////////////////////////////////////////
////

//////////////////////////////////////////////////////////////////////////////
// msocGetThumbnail
//
// Retrieve thumbnail data/information for the specified object.
// The LPSINAIL structure contains some data pointers within it, that Office
// will allocate and set appropriately. Because of this, you need to call
// msocReleaseThumbnail once you are done with the data that was returned.
//
//	hmsoc - Handle returned from msocCreate
//
//	lpSiNail - Pointer to SINAIL structure, may be NULL
//
//	pfScale - Pointer to BOOL to retrieve 'scaled' indication
//		(FALSE indicates 'cropped'), may be NULL
//

OFFICEAPI BOOL msocGetThumbnail (HMSOC hmsoc, LPSINAIL lpSiNail, BOOL *pfScale);

//////////////////////////////////////////////////////////////////////////////
// msocSetThumbnail
//
// Assign thumbnail data for the specified object.
//
//	hmsoc - Handle returned from msocCreate
//
//	lpSiNail - Pointer to SINAIL structure. May be NULL to indicate no
//		change
//
//	pfScale - Pointer to BOOL to indicate scaling. May be NULL.
//

OFFICEAPI BOOL msocSetThumbnail (HMSOC hmsoc, LPSINAIL lpSiNail, BOOL *pfScale);

//////////////////////////////////////////////////////////////////////////////
// msocReleaseThumbnail
//
// This function will allow Office to deallocate some of the data associated
// with the LPSINAIL structure that it allocated in the msocGetThumbnail call.
//
//	lpSiNail - Pointer to SiNail structure that was filled in by a call
//		to msocGetThumbnail
//

OFFICEAPI BOOL msocReleaseThumbnail (LPSINAIL lpSiNail);
 
////
//////////////////////////////////////////////////////////////////////////////
////	Property Dialog Functions
//////////////////////////////////////////////////////////////////////////////
////

//////////////////////////////////////////////////////////////////////////////
// msocShowDocPropsDialog
//
// Brings up a property dialog for the specified file.
//
// HWND hwndParent
//		Window handle to use for 'parent' of dialog.
//
// LPSTR szFileName
//		Fully qualified name of the storage that should be associated with
//		the properties to be displayed. If not NULL, the files creation date
//		size, etc. will be displayed in the dialog.
//
// LPPOINT pptCtr
//		Coordinates for the center of the dialog. if pptCtr->x == -1, then
//		the default dialog positions will be used.
//
//		The values will be set to the actual dialog location upon return.
//
//		The values should be in 'client' coordinates.
//
// LPSTR lpszCaption
//		Caption to be used for the dialog. The string " Properties" will be
//		appended to whatever value is supplied.
//

OFFICEAPI BOOL msocShowDocPropsDialog(HMSOC hmsoc, HWND hwndParent, LPSTR szFileName, LPPOINT pptCtr, LPSTR lpszCaption);

////
//////////////////////////////////////////////////////////////////////////////
////	Heading/DocPart Pair Functions:
//////////////////////////////////////////////////////////////////////////////
////

//////////////////////////////////////////////////////////////////////////
// ...a short description of Headings and DocParts...
//
//  + "Heading #1"
//  |     |
//  |     + "DocPart #1"
//  |     + "DocPart #2"
//  |     + "DocPart #3"
//  |
//  + "Heading #2"
//  |     |
//  |     + "DocPart #1"
//  |     + "DocPart #2"
//  |
//  + "Heading #3"      
//        |
//        + "DocPart #1"
//
// Heading/DocPart pairs represent informational only data that can
// be added to a document. The actual purpose and content of this
// information is under complete control of the application. It can
// represent the internal layout of the document it is associated with,
// or it can represent external conditions associated with the document.
// These Heading/DocPart pairs are displayed on the 'Contents' page of
// the document property sheet.
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// msocGetHeading
//
// Get the specified heading, and the number of DocParts it contains.
//
//	hmsoc - Handle returned from msocCreate
//
//	heading - 1 based index of the heading to retrieve
//
//	szHeading - String storage to hold the retrieved data.
//
//	lpcbHeading - Maximum number of characters to copy
//
//	lpcbDocParts - Pointer to UINT that will hold the number of DocParts
//		in this heading
//
// If heading is '0' the total number of headings will be returned in
// lpcbHeading.
//
// If heading is valid, but szHeading is NULL, the size of the heading
// string will be returned in lpcbHeading.
//

OFFICEAPI BOOL msocGetHeading (HMSOC hmsoc, UINT heading, LPSTR szHeading, UINT *lpcbHeading, UINT *lpcbDocParts);

//////////////////////////////////////////////////////////////////////////////
// Macros for easier access:
//
// msocGetHeadingCount - Return the total number of headers
#define msocGetHeadingCount(hmsoc,lpCount) msocGetHeading(hmsoc,0,NULL,lpCount,NULL)
// msocGetHeadingSize - Return the size of the heading string
#define msocGetHeadingSize(hmsoc,heading,lpSize) msocGetHeading(hmsoc,heading,NULL,lpSize,NULL)

//////////////////////////////////////////////////////////////////////////////
// msocSetHeading
//
// Set the specified heading value.
//
// heading is the index of the heading to set.
// szHeading is the value to assign to this heading.
//
// If heading is negative, szHeading will be 'inserted' just prior to the
// heading index that is indicated by the absolute value of heading. Thus
// to insert a heading in the front of the list, use an index of '-1'
// The return value will be the index location of this item (absolute value)
//
// If heading is '0', szHeading will be appended to the end of the list.
// The return value will be the index location of this item.
//
// If szHeading is NULL, then the specified heading will be deleted. This
// will also delete any 'DocParts' associated with this heading
//

OFFICEAPI UINT msocSetHeading (HMSOC hmsoc, INT heading, LPSTR szHeading);

//////////////////////////////////////////////////////////////////////////////
// Macros for easier access:
//
// msocInsertHeading - Insert a heading at the specified index
#define msocInsertHeading(hmsoc,index,szHeading) msocSetHeading(hmsoc,-(index),szHeading)
// msocAppendHeading - Append a heading
#define msocAppendHeading(hmsoc,szHeading) msocSetHeading(hmsoc,0,szHeading)
// msocDeleteHeading - Delete the heading at the specified index
#define msocDeleteHeading(hmsoc,index) msocSetHeading(hmsoc,index,NULL)

//////////////////////////////////////////////////////////////////////////////
// msocGetDocPart
//
// Get the specified Docpart
//
// heading is the heading index to get a DocPart of
// docpart is the docpart index.
// szDocpart is the string to hold the result
// lpcbDocPart is the maximum number of characters to copy.
//
// if heading is 0 the total number of DocParts for all headings will be returned in lpcbDocPart
//
// if heading is valid, and docpart is 0, the number of docparts for this heading
// will be returned in lpdbDocPart.
//
// if heading and docpart are valid, and  szDocPart is NULL, then the number of
// characters in the docpart name will be returned in lpcbDocPart
//

OFFICEAPI BOOL msocGetDocPart (HMSOC hmsoc, UINT heading, UINT docpart, LPSTR szDocPart, UINT *lpcbDocPart);

//////////////////////////////////////////////////////////////////////////////
// Macros for easier access:
//
// msocGetDocPartCountAll - Gets a count of ALL docparts in all headings
#define msocGetDocPartCountAll(hmsoc,lpCount) msocGetDocPart(hmsoc,0,0,NULL,lpCount)
// msocGetDocPartCount - Gets a count of the docparts for a heading
#define msocGetDocPartCount(hmsoc,heading,lpCount) msocGetDocPart(hmsoc,heading,0,NULL,lpCount)
// msocGeDocPartSize - Get the size of the string for the DocPart
#define msocGetDocPartSize(hmsoc,heading,docpart,lpSize) msocGetDocPart(hmsoc,heading,docpart,NULL,lpSize)

//////////////////////////////////////////////////////////////////////////////
// msocSetDocPart
//
// Set the specified DocPart
//
// heading is the heading index
// docpart is the docpart index
// szDocPart is the string to assign
//
// heading must be a valid heading index
//
// if docpart is negative, szDocPart will be inserted just prior to the docpart
// referenced by the absolute value of docpart. So to insert a DocPart in the
// front of the list, use a docpart index of '-1'
// The return value will be the index of this item (absolute value)
//
// if docpart is '0', szDocPart will be appended to the end of the list.
// The return value will be the index of this item
//
// if docpart is valid, and szDocPart is NULL, then the specified DocPart will be
// deleted.
//

OFFICEAPI UINT msocSetDocPart (HMSOC hmsoc, UINT heading, INT docpart, LPSTR szDocPart);

//////////////////////////////////////////////////////////////////////////////
// Macros for easier access:
//
// msocInsertDocPart - Insert a DocPart in the heading at the index
#define msocInsertDocPart(hmsoc,heading,index,szDocPart) msocSetDocPart (hmsoc,heading,-(index),szDocPart)
// msocAppendDocPart - Append a Docpart to the heading 
#define msocAppendDocPart(hmsoc,heading,szDocPart) msocSetDocPart (hmsoc,heading,0,szDocPart)
// msocDeleteDocPart - Delete the specified DocPart
#define msocDeleteDocPart(hmsoc,heading,index) msocSetDocPart (hmsoc,heading,index,NULL)

#ifdef __cplusplus
   }
#endif

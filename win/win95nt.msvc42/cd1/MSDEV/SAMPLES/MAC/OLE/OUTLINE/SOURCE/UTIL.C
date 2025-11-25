/*****************************************************************************\
*                                                                             *
*    KeyUtilities.c                                                           *
*                                                                             *
*    OLE Version 2.0 Sample Code                                              *
*                                                                             *
*    Copyright (c) 1992-1994, Microsoft Corp. All rights reserved.            *
*                                                                             *
\*****************************************************************************/

#if !defined(_MSC_VER) && !defined(THINK_C)
#include "OLine.h"
#endif

#include "Types.h"

#if defined(USEHEADER)
#include "OleHdrs.h"
#endif
#include "Debug.h"
#include "Util.h"
#include <Traps.h>
#include <GestaltEqu.h>
#include "Layers.h"
#include "Folders.h"
#include "OleXcept.h"
#include <TextUtils.h>
#include "stdio.h"
#include <Traps.h>
#include <SegLoad.h>

#include "App.h"
#include "Window.h"
#if qFrameTools
	#include "Toolbar.h"
#endif
#include "Doc.h"
#include "Line.h"
#include "errors.h"

#ifdef __PPCC__
#include "Layers.h"
#include "stdio.h"
#endif

#include "ole2ui.h"


// OLDNAME: KeyUtilities.c

Boolean IsThisKeyDown(const char theKey)
{
	union
	{
		KeyMap asMap;
		Byte asBytes[16];
	} u;

	GetKeys(u.asMap);
	return u.asBytes[theKey >> 3] & (1 << (theKey & 0x07)) ? TRUE : FALSE;
}

Boolean IsCommandKeyDown()

{
	const short kCommandKey = 55;
	return IsThisKeyDown((const char)kCommandKey);
}

Boolean IsControlKeyDown()

{
	const short kCtlKey = 0x3B;
	return IsThisKeyDown((const char)kCtlKey);
}

Boolean IsOptionKeyDown()

{
	const short kOptionKey = 58;
	return IsThisKeyDown((const char)kOptionKey);
}

Boolean IsShiftKeyDown()

{
	const short kShiftKey = 56;
	return IsThisKeyDown((const char)kShiftKey);
}

// OLDNAME: Utilities.c

#if defined(_MSC_VER) || defined(__powerc)

OSErr __pascal NewLayer(WindowPtr* layerRef, Boolean visible, Boolean neverActive, WindowPtr behind, long refCon)
{
   return noErr;
}

	
WindowPtr __pascal GetLayer(void)
{
   return NULL;
}

OSErr __pascal SetLayer(WindowPtr layer)
{
   return noErr;
}

WindowPtr __pascal SwapLayer(WindowPtr layer)
{
   return NULL;
}

#endif


#if defined(_MSC_VER) || defined(__powerc) || defined (__MWERKS__)

void bzero(void *p, short bytes)
{
	char *pc = (char *)p;

	while (bytes--)
		*pc++ = 0;
}

void bcopy(void *pSrc, void *pDest, short bytes)
{
	BlockMove(pSrc, pDest, bytes);
}

StringPtr pStrCopy(StringPtr pDest, StringPtr pSrc)
{
	BlockMove(pSrc, pDest, pSrc[0]+1);
	return pDest;
}

void LocalToGlobalRect(Rect *r)
{
	LocalToGlobal(&topLeft(*r));
	LocalToGlobal(&botRight(*r));
}

void GlobalToLocalRect(Rect *r)
{
	GlobalToLocal(&topLeft(*r));
	GlobalToLocal(&botRight(*r));
}

unsigned short SwapWord(unsigned short oldVal)
{
	return (((oldVal & 0xff) << 8) | (oldVal >> 8));
}

unsigned long SwapLong(unsigned long oldVal)
{
	return (((unsigned long)(SwapWord((unsigned short)(oldVal & 0xffff))) << 16) |
		((unsigned long)SwapWord((unsigned short)(oldVal >> 16))));
}

#endif

void pStrCat(StringPtr pBase, StringPtr pStr)
{
	ASSERTCOND(pBase[0] + pStr[0] <= 255);

	bcopy(&pStr[1], &pBase[pBase[0]+1], pStr[0]);
	pBase[0] += pStr[0];
}

void strinsert(char* dst, char* src)
{
	short	len;
	
	len = strlen(src);
	
	BlockMove(dst, dst + len, strlen(dst) + 1);		// can overwrite itself
	bcopy(src, dst, len);
}

#ifndef __PPCC__
Boolean TrapAvailable(short tNumber,TrapType tType)
{
	// Check and see if the trap exists. On 64K ROM machines, tType will be ignored.
    return (Boolean) (NGetTrapAddress( tNumber, tType) != GetToolTrapAddress( (short) _Unimplemented));
}
#endif

void DisableAllItems(MenuHandle theMenu)
{
	short	i;

	ASSERTCOND(theMenu != nil);

	for (i = CountMItems(theMenu); i>0; i--)
	{
		DisableItem(theMenu, i);
		CheckItem(theMenu, i, false);
	}
}

Boolean AppleEventsInstalled(void)
{
	OSErr	err;
	long	result;

	err = Gestalt(gestaltAppleEventsAttr, &result);

	return !err && ((result >> gestaltAppleEventsPresent) & 0x1);
}

short QuickdrawVersion(void)
{
	OSErr	err;
	long	result;

	err = Gestalt(gestaltQuickdrawVersion, &result);

	return (short)(err ? 0 : (result >> 8));
}

Boolean WaitNextEventAvailable(void)
{
	return TrapAvailable((short)_WaitNextEvent, ToolTrap);
}

Boolean haveAUX()
{
	OSErr	err;
	long	result;

	err = Gestalt(gestaltAUXVersion, &result);

	return !err;
}

OSErr MyGotRequiredParams(AppleEvent* theAppleEvent)
{
	DescType	returnedSize;
	Size		actualSize;
	OSErr		err;

	err = AEGetAttributePtr(theAppleEvent, keyMissedKeywordAttr, typeWildCard, &returnedSize, nil, 0, &actualSize);

	if (err == errAEDescNotFound)
		return noErr;
	else if (err == noErr)
		return errAEParamMissed;

	return err;
}

void PathNameFromDirID(long dirID, short vrefNum, char *s)
/* routine copied from Apple Tech Note #238 */
{
	CInfoPBRec	iob;
	Str255		stDir;

	*s = '\0';

	iob.dirInfo.ioNamePtr = stDir;
	iob.dirInfo.ioDrParID = dirID;

	do {
		iob.dirInfo.ioVRefNum = vrefNum;
		iob.dirInfo.ioFDirIndex = -1;
		iob.dirInfo.ioDrDirID = iob.dirInfo.ioDrParID;

		if (PBGetCatInfoSync(&iob) != noErr)
			break;

		if (haveAUX())
		{
			// check if we are at the root
			if (stDir[1] != '/')
				pStrCat(stDir, "\p/");
		}
		else
			pStrCat(stDir, "\p:");
		
		strinsert(s, (char*)p2cstr(stDir));

	} while (iob.dirInfo.ioDrDirID != 2);
}

void PathNameFromFSSpec(FSSpec *fs, char *s)
{	
	PathNameFromDirID(fs->parID, fs->vRefNum, s);	// get path name
	strcat(s, (char*)p2cstr(fs->name));	// add filename to path name
	c2pstr((char *)fs->name);
}

Boolean FSSpecCompare(FSSpecPtr pSrc, FSSpecPtr pDest)
{
	ASSERTCOND(pSrc != nil && pDest != nil);
	
	return ((pSrc->vRefNum == pDest->vRefNum) &&
			(pSrc->parID == pDest->parID) &&
			EqualString(pSrc->name, pDest->name, false, true));
}

void CreateTemporaryFSSpec(StringPtr pBase, FSSpecPtr pSpec)
{
	OSErr			err;
	short			foundVRefNum;
	long			foundDirID;
	unsigned long	secs;
	
	err = FindFolder(kOnSystemDisk, kTemporaryFolderType, kCreateFolder, &foundVRefNum, &foundDirID);
	ASSERTCOND(err == noErr);
	
	pSpec->vRefNum = foundVRefNum;
	pSpec->parID = foundDirID;
	
	do
	{
		FInfo	fndrInfo;
		Str32	str;
		
		GetDateTime(&secs);
		NumToString(secs, str);
		
		pSpec->name[0] = 0;
		if (pBase != nil)
			pStrCat(pSpec->name, pBase);
			
		pStrCat(pSpec->name, str);
		
		err = FSpGetFInfo(pSpec, &fndrInfo);
		ASSERTCOND(err == fnfErr || err == noErr);
	}
	while (err == noErr);
}

void GetUnionWindowStrucRgns(RgnHandle rgn)
{
	WindowPeek	p;
	
	ASSERTCOND(rgn != nil);
	
	SetEmptyRgn(rgn);
	
	p = (WindowPeek)FrontWindow();
	
	while(p)
	{
		UnionRgn(rgn, p->strucRgn, rgn);
		p = p->nextWindow;
	}
}

short GetWindowMinDepth(WindowPtr pWindow)
{
	GrafPtr		gpSaved;
	Rect		rGlobal,
				rIgnore;
	GDHandle	hGDevice;
	short		minDepth;

	ASSERTCOND(pWindow != NULL);

	if (QuickdrawVersion() == kOriginalQD)
		return 1;

	GetPort(&gpSaved);
	SetPort(pWindow);
	rGlobal = pWindow->portRect;
	LocalToGlobalRect(&rGlobal);
	SetPort(gpSaved);

	minDepth = 32767;

	for (hGDevice = GetDeviceList(); hGDevice; hGDevice = GetNextDevice(hGDevice))
	{
		if (TestDeviceAttribute(hGDevice, screenDevice) &&
			TestDeviceAttribute(hGDevice, screenActive) &&
			SectRect(&rGlobal, &((*hGDevice)->gdRect), &rIgnore) &&
			(**(**hGDevice).gdPMap).pixelSize < minDepth)
				minDepth = (**(**hGDevice).gdPMap).pixelSize;
	}

	return minDepth;
}

void FlashItem(DialogPtr theDialog, short item, short delay)
{
	short			type;
	Handle			itemHandle;
	Rect			box;
 	long			temp;

  	GetDItem(theDialog, item, &type, &itemHandle, &box);

	if (type == ctrlItem)
	{
		HiliteControl((ControlHandle)itemHandle, inButton);
		Delay(delay, &temp);
		HiliteControl((ControlHandle)itemHandle,0);
	}
	else
	{
		GrafPtr		poldPort;
  	  	
		GetPort(&poldPort);
		SetPort(theDialog);
		
		InvertRect(&box);
		Delay(delay, &temp);
		InvertRect(&box);
		
		SetPort(poldPort);
	}
}

Boolean AmIFrontProcess(void)
{
	ProcessSerialNumber		currentProcess;
	ProcessSerialNumber		frontProcess;
	Boolean					fSame;

	GetCurrentProcess(&currentProcess);
	GetFrontProcess(&frontProcess);

	SameProcess(&currentProcess, &frontProcess, &fSame);

	return fSame;
}

Boolean IsAppFrontProcess(OSType appSig)
{
	ProcessSerialNumber		frontProcess;
	ProcessInfoRec			processInfo;

	GetFrontProcess(&frontProcess);

	processInfo.processInfoLength = sizeof(processInfo);
	processInfo.processName = nil;
	processInfo.processAppSpec = nil;

	GetProcessInformation(&frontProcess, &processInfo);

	return (processInfo.processSignature == appSig);
}

void DrawTextCentered(Ptr textBuf, short byteCount, Rect *r)
{
	short		width;
	short		height;
	FontInfo	info;
	
	GetFontInfo(&info);
	
	width = TextWidth(textBuf, 0, byteCount);
	height = info.ascent + info.descent;
	
	MoveTo(
		(short)(r->left + ((r->right - r->left - width) >> 1)),
		(short)(r->top + ((r->bottom - r->top - height) >> 1) + info.ascent));
	
	DrawText(textBuf, 0, byteCount);
}

OSErr FindPrefFolder(short *foundVRefNum, long *foundDirID)
{
	long			gesResponse;
	SysEnvRec		envRec;
	WDPBRec			myWDPB;
	unsigned char	volName[34];
	OSErr			result;

	ASSERTCOND(foundVRefNum != NULL);
	ASSERTCOND(foundDirID != NULL);

	*foundVRefNum = 0;
	*foundDirID = 0;

	if (!Gestalt(gestaltFindFolderAttr, &gesResponse) &&
		gesResponse & (1 << gestaltFindFolderPresent))			// Does Folder Manager exist?
	{
		result = FindFolder(kOnSystemDisk, kPreferencesFolderType, kDontCreateFolder,
							foundVRefNum, foundDirID);
	}
	else
	{
		// Gestalt can't give us the answer, so we resort to SysEnvirons
		if (!(result = SysEnvirons(curSysEnvVers, &envRec)))
		{
			myWDPB.ioVRefNum = envRec.sysVRefNum;
			volName[0] = '\000';								// Zero volume name
			myWDPB.ioNamePtr = volName;
			myWDPB.ioWDIndex = 0;
			myWDPB.ioWDProcID = 0;
			if (!(result = PBGetWDInfo(&myWDPB, 0)))
			{
				*foundVRefNum = myWDPB.ioWDVRefNum;
				*foundDirID = myWDPB.ioWDDirID;
			}
		}
	}

	return result;
}

void PrintHResult(HRESULT hr)
{
	SCODE 		sc = GetScode(hr);
	char		szOut[256];
	
	if (hr == NOERROR)
		return;
	
	*szOut = sprintf(&szOut[1], "%s", szFromScode(sc));
	DebugStr((StringPtr)szOut);
}	

#define DISP_ERROR(X) MAKE_SCODE(SEVERITY_ERROR, FACILITY_DISPATCH, X)

#define DISP_E_UNKNOWNINTERFACE			DISP_ERROR(1)
#define DISP_E_MEMBERNOTFOUND			DISP_ERROR(3)
#define DISP_E_PARAMNOTFOUND			DISP_ERROR(4)
#define DISP_E_TYPEMISMATCH				DISP_ERROR(5)
#define DISP_E_UNKNOWNNAME				DISP_ERROR(6)
#define DISP_E_NONAMEDARGS				DISP_ERROR(7)
#define DISP_E_BADVARTYPE				DISP_ERROR(8)
#define DISP_E_EXCEPTION				DISP_ERROR(9)
#define DISP_E_OVERFLOW					DISP_ERROR(10)
#define DISP_E_BADINDEX					DISP_ERROR(11)
#define DISP_E_UNKNOWNLCID				DISP_ERROR(12)
#define DISP_E_ARRAYISLOCKED			DISP_ERROR(13)
#define DISP_E_BADPARAMCOUNT			DISP_ERROR(14)
#define DISP_E_PARAMNOTOPTIONAL			DISP_ERROR(15)
#define DISP_E_BADCALLEE				DISP_ERROR(16)
#define DISP_E_NOTACOLLECTION			DISP_ERROR(17)

#define TYPE_ERROR(X) MAKE_SCODE(SEVERITY_ERROR, FACILITY_DISPATCH, X)

#define TYPE_E_BUFFERTOOSMALL			TYPE_ERROR(32790)
#define TYPE_E_INVDATAREAD				TYPE_ERROR(32792)
#define TYPE_E_UNSUPFORMAT				TYPE_ERROR(32793)
#define TYPE_E_REGISTRYACCESS			TYPE_ERROR(32796)
#define TYPE_E_LIBNOTREGISTERED 		TYPE_ERROR(32797)
#define TYPE_E_UNDEFINEDTYPE			TYPE_ERROR(32807)
#define TYPE_E_QUALIFIEDNAMEDISALLOWED	TYPE_ERROR(32808)
#define TYPE_E_INVALIDSTATE				TYPE_ERROR(32809)
#define TYPE_E_WRONGTYPEKIND			TYPE_ERROR(32810)
#define TYPE_E_ELEMENTNOTFOUND			TYPE_ERROR(32811)
#define TYPE_E_AMBIGUOUSNAME			TYPE_ERROR(32812)
#define TYPE_E_NAMECONFLICT				TYPE_ERROR(32813)
#define TYPE_E_UNKNOWNLCID				TYPE_ERROR(32814)
#define TYPE_E_DLLFUNCTIONNOTFOUND		TYPE_ERROR(32815)
#define TYPE_E_BADMODULEKIND			TYPE_ERROR(35005)
#define TYPE_E_SIZETOOBIG				TYPE_ERROR(35013)
#define TYPE_E_DUPLICATEID				TYPE_ERROR(35014)
#define TYPE_E_TYPEMISMATCH				TYPE_ERROR(36000)
#define TYPE_E_OUTOFBOUNDS				TYPE_ERROR(36001)
#define TYPE_E_IOERROR					TYPE_ERROR(36002)
#define TYPE_E_CANTCREATETMPFILE		TYPE_ERROR(36003)
#define TYPE_E_CANTLOADLIBRARY			TYPE_ERROR(40010)
#define TYPE_E_INCONSISTENTPROPFUNCS	TYPE_ERROR(40067)
#define TYPE_E_CIRCULARTYPE				TYPE_ERROR(40068)

#define CASE_SCODE(sc)  \
        case sc: \
			return #sc; \
			break;
//BUGBUG Until fixed for PPC compiler
#ifndef _PPCMAC
char* szFromScode(SCODE sc)
{
	switch (sc)
	{
		/* SCODE's defined in SCODE.H */

		CASE_SCODE(S_OK)
		CASE_SCODE(S_FALSE)
		CASE_SCODE(E_UNEXPECTED)
		CASE_SCODE(E_OUTOFMEMORY)
		CASE_SCODE(E_INVALIDARG)
		CASE_SCODE(E_NOINTERFACE)
		CASE_SCODE(E_POINTER)
		CASE_SCODE(E_HANDLE)
		CASE_SCODE(E_ABORT)
		CASE_SCODE(E_FAIL)
		CASE_SCODE(E_ACCESSDENIED)

		/* SCODE's defined in OLE2.H */

		CASE_SCODE(OLE_E_OLEVERB)
		CASE_SCODE(OLE_E_ADVF)
		CASE_SCODE(OLE_E_ENUM_NOMORE)
		CASE_SCODE(OLE_E_ADVISENOTSUPPORTED)
		CASE_SCODE(OLE_E_NOCONNECTION)
		CASE_SCODE(OLE_E_NOTRUNNING)
		CASE_SCODE(OLE_E_NOCACHE)
		CASE_SCODE(OLE_E_BLANK)
		CASE_SCODE(OLE_E_CLASSDIFF)
		CASE_SCODE(OLE_E_CANT_GETMONIKER)
		CASE_SCODE(OLE_E_CANT_BINDTOSOURCE)
		CASE_SCODE(OLE_E_STATIC)
		CASE_SCODE(OLE_E_PROMPTSAVECANCELLED)
		CASE_SCODE(OLE_E_INVALIDRECT)
		CASE_SCODE(OLE_E_WRONGCOMPOBJ)
		CASE_SCODE(OLE_E_INVALIDHWND)
		CASE_SCODE(OLE_E_NOT_INPLACEACTIVE)
		CASE_SCODE(OLE_E_CANTCONVERT)
//		CASE_SCODE(OLE_E_NOSTORAGE)

		CASE_SCODE(DV_E_FORMATETC)
		CASE_SCODE(DV_E_DVTARGETDEVICE)
		CASE_SCODE(DV_E_STGMEDIUM)
		CASE_SCODE(DV_E_STATDATA)
		CASE_SCODE(DV_E_LINDEX)
		CASE_SCODE(DV_E_TYMED)
		CASE_SCODE(DV_E_CLIPFORMAT)
		CASE_SCODE(DV_E_DVASPECT)
		CASE_SCODE(DV_E_DVTARGETDEVICE_SIZE)
		CASE_SCODE(DV_E_NOIVIEWOBJECT)

		CASE_SCODE(OLE_S_USEREG)
		CASE_SCODE(OLE_S_STATIC)
		CASE_SCODE(OLE_S_MAC_CLIPFORMAT)

		CASE_SCODE(CONVERT10_E_OLESTREAM_GET)
		CASE_SCODE(CONVERT10_E_OLESTREAM_PUT)
		CASE_SCODE(CONVERT10_E_OLESTREAM_FMT)
		CASE_SCODE(CONVERT10_E_OLESTREAM_BITMAP_TO_DIB)
		CASE_SCODE(CONVERT10_E_STG_FMT)
		CASE_SCODE(CONVERT10_E_STG_NO_STD_STREAM)
		CASE_SCODE(CONVERT10_E_STG_DIB_TO_BITMAP)
		CASE_SCODE(CONVERT10_S_NO_PRESENTATION)

		CASE_SCODE(CLIPBRD_E_CANT_OPEN)
		CASE_SCODE(CLIPBRD_E_CANT_EMPTY)
		CASE_SCODE(CLIPBRD_E_CANT_SET)
		CASE_SCODE(CLIPBRD_E_BAD_DATA)
		CASE_SCODE(CLIPBRD_E_CANT_CLOSE)

		CASE_SCODE(DRAGDROP_E_NOTREGISTERED)
		CASE_SCODE(DRAGDROP_E_ALREADYREGISTERED)
		CASE_SCODE(DRAGDROP_E_INVALIDHWND)
		CASE_SCODE(DRAGDROP_S_DROP)
		CASE_SCODE(DRAGDROP_S_CANCEL)
		CASE_SCODE(DRAGDROP_S_USEDEFAULTCURSORS)

		CASE_SCODE(OLEOBJ_E_NOVERBS)
		CASE_SCODE(OLEOBJ_E_INVALIDVERB)
		CASE_SCODE(OLEOBJ_S_INVALIDVERB)
		CASE_SCODE(OLEOBJ_S_CANNOT_DOVERB_NOW)
		CASE_SCODE(OLEOBJ_S_INVALIDHWND)
		CASE_SCODE(INPLACE_E_NOTUNDOABLE)
		CASE_SCODE(INPLACE_E_NOTOOLSPACE)
		CASE_SCODE(INPLACE_S_TRUNCATED)

		/* SCODE's defined in COMPOBJ.H */

		CASE_SCODE(CO_E_NOTINITIALIZED)
		CASE_SCODE(CO_E_ALREADYINITIALIZED)
		CASE_SCODE(CO_E_CANTDETERMINECLASS)
		CASE_SCODE(CO_E_CLASSSTRING)
		CASE_SCODE(CO_E_IIDSTRING)
		CASE_SCODE(CO_E_APPNOTFOUND)
		CASE_SCODE(CO_E_APPSINGLEUSE)
		CASE_SCODE(CO_E_ERRORINAPP)
		CASE_SCODE(CO_E_DLLNOTFOUND)
		CASE_SCODE(CO_E_ERRORINDLL)
		CASE_SCODE(CO_E_WRONGOSFORAPP)
		CASE_SCODE(CO_E_OBJNOTREG)
		CASE_SCODE(CO_E_OBJISREG)
		CASE_SCODE(CO_E_OBJNOTCONNECTED)
		CASE_SCODE(CO_E_APPDIDNTREG)
		CASE_SCODE(CLASS_E_NOAGGREGATION)
//		CASE_SCODE(CLASS_E_CLASSNOTAVAILABLE)
		CASE_SCODE(REGDB_E_READREGDB)
		CASE_SCODE(REGDB_E_WRITEREGDB)
		CASE_SCODE(REGDB_E_KEYMISSING)
		CASE_SCODE(REGDB_E_INVALIDVALUE)
		CASE_SCODE(REGDB_E_CLASSNOTREG)
		CASE_SCODE(REGDB_E_IIDNOTREG)
		CASE_SCODE(RPC_E_CALL_REJECTED)
		CASE_SCODE(RPC_E_CALL_CANCELED)
		CASE_SCODE(RPC_E_CANTPOST_INSENDCALL)
		CASE_SCODE(RPC_E_CANTCALLOUT_INASYNCCALL)
		CASE_SCODE(RPC_E_CANTCALLOUT_INEXTERNALCALL)
		CASE_SCODE(RPC_E_CONNECTION_TERMINATED)
		CASE_SCODE(RPC_E_SERVER_DIED)
		CASE_SCODE(RPC_E_CLIENT_DIED)
		CASE_SCODE(RPC_E_INVALID_DATAPACKET)
		CASE_SCODE(RPC_E_CANTTRANSMIT_CALL)
		CASE_SCODE(RPC_E_CLIENT_CANTMARSHAL_DATA)
		CASE_SCODE(RPC_E_CLIENT_CANTUNMARSHAL_DATA)
		CASE_SCODE(RPC_E_SERVER_CANTMARSHAL_DATA)
		CASE_SCODE(RPC_E_SERVER_CANTUNMARSHAL_DATA)
		CASE_SCODE(RPC_E_INVALID_DATA)
		CASE_SCODE(RPC_E_INVALID_PARAMETER)
		CASE_SCODE(RPC_E_CANTCALLOUT_AGAIN)
		CASE_SCODE(RPC_E_UNEXPECTED)

		/* SCODE's defined in DVOBJ.H */

		CASE_SCODE(DATA_S_SAMEFORMATETC)
		CASE_SCODE(VIEW_E_DRAW)
		CASE_SCODE(VIEW_S_ALREADY_FROZEN)
		CASE_SCODE(CACHE_E_NOCACHE_UPDATED)
		CASE_SCODE(CACHE_S_FORMATETC_NOTSUPPORTED)
		CASE_SCODE(CACHE_S_SAMECACHE)
		CASE_SCODE(CACHE_S_SOMECACHES_NOTUPDATED)

		/* SCODE's defined in STORAGE.H */

		CASE_SCODE(STG_E_INVALIDFUNCTION)
		CASE_SCODE(STG_E_FILENOTFOUND)
		CASE_SCODE(STG_E_PATHNOTFOUND)
		CASE_SCODE(STG_E_TOOMANYOPENFILES)
		CASE_SCODE(STG_E_ACCESSDENIED)
		CASE_SCODE(STG_E_INVALIDHANDLE)
		CASE_SCODE(STG_E_INSUFFICIENTMEMORY)
		CASE_SCODE(STG_E_INVALIDPOINTER)
		CASE_SCODE(STG_E_NOMOREFILES)
		CASE_SCODE(STG_E_DISKISWRITEPROTECTED)
		CASE_SCODE(STG_E_SEEKERROR)
		CASE_SCODE(STG_E_WRITEFAULT)
		CASE_SCODE(STG_E_READFAULT)
		CASE_SCODE(STG_E_SHAREVIOLATION)
		CASE_SCODE(STG_E_LOCKVIOLATION)
		CASE_SCODE(STG_E_FILEALREADYEXISTS)
		CASE_SCODE(STG_E_INVALIDPARAMETER)
		CASE_SCODE(STG_E_MEDIUMFULL)
		CASE_SCODE(STG_E_ABNORMALAPIEXIT)
		CASE_SCODE(STG_E_INVALIDHEADER)
		CASE_SCODE(STG_E_INVALIDNAME)
		CASE_SCODE(STG_E_UNKNOWN)
		CASE_SCODE(STG_E_UNIMPLEMENTEDFUNCTION)
		CASE_SCODE(STG_E_INVALIDFLAG)
		CASE_SCODE(STG_E_INUSE)
		CASE_SCODE(STG_E_NOTCURRENT)
		CASE_SCODE(STG_E_REVERTED)
		CASE_SCODE(STG_E_CANTSAVE)
		CASE_SCODE(STG_E_OLDFORMAT)
		CASE_SCODE(STG_E_OLDDLL)
		CASE_SCODE(STG_E_SHAREREQUIRED)
		CASE_SCODE(STG_E_NOTFILEBASEDSTORAGE)
		CASE_SCODE(STG_E_EXTANTMARSHALLINGS)
		CASE_SCODE(STG_S_CONVERTED)

		/* SCODE's defined in STORAGE.H */

		CASE_SCODE(MK_E_CONNECTMANUALLY)
		CASE_SCODE(MK_E_EXCEEDEDDEADLINE)
		CASE_SCODE(MK_E_NEEDGENERIC)
		CASE_SCODE(MK_E_UNAVAILABLE)
		CASE_SCODE(MK_E_SYNTAX)
		CASE_SCODE(MK_E_NOOBJECT)
		CASE_SCODE(MK_E_INVALIDEXTENSION)
		CASE_SCODE(MK_E_INTERMEDIATEINTERFACENOTSUPPORTED)
		CASE_SCODE(MK_E_NOTBINDABLE)
		CASE_SCODE(MK_E_NOTBOUND)
		CASE_SCODE(MK_E_CANTOPENFILE)
		CASE_SCODE(MK_E_MUSTBOTHERUSER)
		CASE_SCODE(MK_E_NOINVERSE)
		CASE_SCODE(MK_E_NOSTORAGE)
		CASE_SCODE(MK_E_NOPREFIX)
		CASE_SCODE(MK_S_REDUCED_TO_SELF)
		CASE_SCODE(MK_S_ME)
		CASE_SCODE(MK_S_HIM)
		CASE_SCODE(MK_S_US)
		CASE_SCODE(MK_S_MONIKERALREADYREGISTERED)
		
		/* SCODE's defined in DISPATCH.H */
		
		CASE_SCODE(DISP_E_UNKNOWNINTERFACE)
		CASE_SCODE(DISP_E_MEMBERNOTFOUND)
		CASE_SCODE(DISP_E_PARAMNOTFOUND)
		CASE_SCODE(DISP_E_TYPEMISMATCH)
		CASE_SCODE(DISP_E_UNKNOWNNAME)
		CASE_SCODE(DISP_E_NONAMEDARGS)
		CASE_SCODE(DISP_E_BADVARTYPE)
		CASE_SCODE(DISP_E_EXCEPTION)
		CASE_SCODE(DISP_E_OVERFLOW)
		CASE_SCODE(DISP_E_BADINDEX)
		CASE_SCODE(DISP_E_UNKNOWNLCID)
		CASE_SCODE(DISP_E_ARRAYISLOCKED)
		CASE_SCODE(DISP_E_BADPARAMCOUNT)
		CASE_SCODE(DISP_E_PARAMNOTOPTIONAL)
		CASE_SCODE(DISP_E_BADCALLEE)
		CASE_SCODE(DISP_E_NOTACOLLECTION)
		
		CASE_SCODE(TYPE_E_BUFFERTOOSMALL)
		CASE_SCODE(TYPE_E_INVDATAREAD)
		CASE_SCODE(TYPE_E_UNSUPFORMAT)
		CASE_SCODE(TYPE_E_REGISTRYACCESS)
		CASE_SCODE(TYPE_E_LIBNOTREGISTERED)
		CASE_SCODE(TYPE_E_UNDEFINEDTYPE)
		CASE_SCODE(TYPE_E_QUALIFIEDNAMEDISALLOWED)
		CASE_SCODE(TYPE_E_INVALIDSTATE)
		CASE_SCODE(TYPE_E_WRONGTYPEKIND)
		CASE_SCODE(TYPE_E_ELEMENTNOTFOUND)
		CASE_SCODE(TYPE_E_AMBIGUOUSNAME)
		CASE_SCODE(TYPE_E_NAMECONFLICT)
		CASE_SCODE(TYPE_E_UNKNOWNLCID)
		CASE_SCODE(TYPE_E_DLLFUNCTIONNOTFOUND)
		CASE_SCODE(TYPE_E_BADMODULEKIND)
		CASE_SCODE(TYPE_E_SIZETOOBIG)
		CASE_SCODE(TYPE_E_DUPLICATEID)
		CASE_SCODE(TYPE_E_TYPEMISMATCH)
		CASE_SCODE(TYPE_E_OUTOFBOUNDS)
		CASE_SCODE(TYPE_E_IOERROR)
		CASE_SCODE(TYPE_E_CANTCREATETMPFILE)
		CASE_SCODE(TYPE_E_CANTLOADLIBRARY)
		CASE_SCODE(TYPE_E_INCONSISTENTPROPFUNCS)
		CASE_SCODE(TYPE_E_CIRCULARTYPE)

		default:
			return "Unknown SCODE";
			break;
	}

	return NULL;
}
#else
char* szFromScode(SCODE sc)
{
	switch (sc)
	{
	   CASE_SCODE(S_OK)

	   default:
		   return "Unknown SCODE";
		   break;
	}

	return NULL;

}
#endif // _PPCMAC



// OLDNAME: Vtbls.c

//#pragma segment VtblInitSeg
void InitVtbls(void)
{
	TRY
	{
		AppInitVtbl();
		WinInitVtbl();
#if qFrameTools
		ToolbarInitVtbl();
#endif
		DocInitVtbl();
		OutlineInitVtbl();
#if qOle
		OleOutlineInitVtbl();
#endif
		OutlineDocInitVtbl();
#if qOle
		OleOutlineDocInitVtbl();
#endif
		LineInitVtbl();
		TextLineInitVtbl();
#if qOleContainerApp
		OleContainerLineInitVtbl();
#endif
	}
	CATCH
	{
		ExitToShell();
	}
	ENDTRY
}

//#pragma segment VtblDisposeSeg
void DisposeVtbls(void)
{
	AppDisposeVtbl();
	DocDisposeVtbl();
	OutlineDisposeVtbl();
#if qOle
	OleOutlineDisposeVtbl();
#endif
	OutlineDocDisposeVtbl();
#if qOle
	OleOutlineDocDisposeVtbl();
#endif
	LineDisposeVtbl();
	TextLineDisposeVtbl();
#if qOleContainerApp
	OleContainerLineDisposeVtbl();
#endif
}


// OLDNAME: VtblUtilities.c
void InheritFromVtbl(void* pSuperClass, void* pBaseClass)
{	
	long	sizeBaseClass;
	
	sizeBaseClass = GetPtrSize(pBaseClass);
	
	ASSERTCOND(ValidVtbl(pBaseClass, sizeBaseClass));
	ASSERTCOND(pSuperClass != nil);
	ASSERTCOND(GetPtrSize(pSuperClass) >= sizeBaseClass);
	
	bcopy(pBaseClass, pSuperClass, (short)(sizeBaseClass));
}


Boolean ValidVtbl(void* p, long size)
{
	if (p == nil || size == 0)
		return false;
		
	// should be a multiple of 4
	if (size % 4)
		return false;

	for(; size>0; size -= 4)
	{
		// every entry should have a function
		if (*(*(ProcPtr**)&p)++ == nil)
			return false;
	}
	
	return true;
}


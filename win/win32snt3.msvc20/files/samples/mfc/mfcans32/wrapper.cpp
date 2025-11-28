//+--------------------------------------------------------------------------
//
//  Copyright (C) 1994, Microsoft Corporation.  All Rights Reserved.
//
//  File:       thunks.cpp
//
//  Contents:   Wrap an Unicode with an ANSI object or an ANSI object with an
//              Unicode object.
//
//  Functions:  WrapIEnumStringA
//              WrapIEnumSTATSTGA
//              WrapILockBytesA
//              WrapIStreamA
//              WrapIStorageA
//              WrapIRootStorageA
//              WrapIBindCtxA
//              WrapIMonikerA
//              WrapIRunningObjectTableA
//              WrapIEnumMonikerA
//              WrapIEnumSTATDATAA
//              WrapIDataObjectA
//              WrapIViewObject2A
//              WrapIAdviseSink2A
//              WrapIDataAdviseHolderA
//              WrapIOleCache2A
//              WrapIOleCacheControlA
//              WrapIDropTargetA
//              WrapIPersistA
//              WrapIPersistStorageA
//              WrapIPersistStreamA
//              WrapIPersistFileA
//              WrapIEnumOLEVERBA
//              WrapIOleObjectA
//              WrapIOleClientSiteA
//              WrapIRunnableObjectA
//              WrapIParseDisplayNameA
//              WrapIOleContainerA
//              WrapIOleItemContainerA
//              WrapIOleAdviseHolderA
//              WrapIOleLinkA
//              WrapIOleInPlaceObjectA
//              WrapIOleInPlaceActiveObjectA
//              WrapIOleInPlaceUIWindowA
//              WrapIOleInPlaceFrameA
//              WrapIOleInPlaceSiteA
//
//  History:    19-Jan-94   v-kentc     Created.
//              05-Apr-94   v-kentc     Add multi-threaded support.
//
//---------------------------------------------------------------------------

#include "Ole2Ansi.h"

#pragma warning(disable: 4243)


//
//  Support multi-threaded applications.
//
CRITICAL_SECTION CriticalSection;

//
//  Keep track of ANSI and Unicode wrappers in a linked list.
//
typedef struct tagWRAPPER
{
	struct tagWRAPPER * pNext;
	IDINTERFACE                idRef;
	LPUNKNOWN           Wrappee;
	LPUNKNOWN           Wrapper;
} WRAPPER, * PWRAPPER;

static int nInitCount;

// per-thread data is contained in this TLS structure
struct WRAP_DATA
{
	//
	// Linked list of wrappers (much slower than below)
	//
	PWRAPPER pAnsiWrappers;
	PWRAPPER pWideWrappers;

	//
	//  Generally a small set of interfaces are used at a time.  Keep a table of
	//  the last wrapper accessed per interface.  If an interface is known to be
	//  a wrapper (ie, in the table), then the wrappee can be pulled directly
	//  from the wrapper.
	//
	LPUNKNOWN aAnsiWrapperCache[ID_SIZE];
	LPUNKNOWN aWideWrapperCache[ID_SIZE];
};
// and referenced by this TLS index
static DWORD tlsIndex = (DWORD)-1;      // for Win32s
static WRAP_DATA wrapData;      // for Win32


HRESULT WrapAddAnsiWrapper(IDINTERFACE, LPUNKNOWN, LPUNKNOWN);
HRESULT WrapAddWideWrapper(IDINTERFACE, LPUNKNOWN, LPUNKNOWN);

int IsValidDispInterface(REFIID riid);
STDAPI_(int) StringFromGUID2A(REFGUID rguid, LPSTR szGuid, int cbMax);



//+--------------------------------------------------------------------------
//
//  Routine:    GetWrapData
//
//  Synopsis:   retrieve thread local storage on Win32s
//
//  Returns:    pointer to WRAP_DATA ("global" data)
//
//---------------------------------------------------------------------------
static WRAP_DATA* GetWrapData()
{
	WRAP_DATA* pData;
	if (tlsIndex == (DWORD)-1)
		pData = &wrapData;
	else
		pData = (WRAP_DATA*)TlsGetValue(tlsIndex);
	return pData;
}

//+--------------------------------------------------------------------------
//
//  Routine:    Ole2AnsiAFromW
//
//  Synopsis:
//
//  Returns:
//
//---------------------------------------------------------------------------
STDAPI Ole2AnsiAFromW(REFIID riid, LPUNKNOWN pWrappee, LPUNKNOWN * ppWrapper)
{
	return WrapInterfaceAFromW(WrapTranslateIID(riid), pWrappee, ppWrapper);
}


//+--------------------------------------------------------------------------
//
//  Routine:    Ole2AnsiWFromA
//
//  Synopsis:
//
//  Returns:
//
//---------------------------------------------------------------------------
STDAPI Ole2AnsiWFromA(REFIID riid, LPUNKNOWN pWrappee, LPUNKNOWN * ppWrapper)
{
	return WrapInterfaceWFromA(WrapTranslateIID(riid), pWrappee, ppWrapper);
}


//+--------------------------------------------------------------------------
//
//  Routine:    WrapAnyAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
IDINTERFACE WrapTranslateIID(REFIID riid)
{
	IDINTERFACE idRef;


	//
	//  The following is a performance hack.  If the last 12 bytes of a GUID
	//  is zero then it could be a system GUID.   Do a fast switch() on the
	//  first 4 bytes to determine which GUID it is.
	//
	if (memcmp(((PULONG)&riid) + 1, &IID_IUnknown, sizeof(12)) == 0)
	{
		//
		//  The last 12 bytes are zero, select the ID based on the
		//  first 4 bytes.
		switch (*(PULONG)&riid)
		{
		case 0x00000000:            // IID_IUnknown
			return ID_IUnknown;

		case 0x00000001:            // IID_IClassFactory
			return ID_IClassFactory;

		case 0x00000003:            // IID_IMarshal
			return ID_IMarshal;

		case 0x0000000a:            // IID_ILockBytes
			return ID_ILockBytes;

		case 0x0000000b:            // IID_IStorage
			return ID_IStorage;

		case 0x0000000c:            // IID_IStream
			return ID_IStream;

		case 0x0000000d:            // IID_IEnumSTATSTG
			return ID_IEnumSTATSTG;

		case 0x0000000e:            // IID_IBindCtx
			return ID_IBindCtx;

		case 0x0000000f:                        // IID_IMoniker
			return ID_IMoniker;

		case 0x00000109:                        // IID_IPersistStream
			return ID_IPersistStream;

		case 0x0000010c:            // IID_IPersist
			return ID_IMoniker;

		case 0x00000010:            // IID_RunningObjectTable
			return ID_IRunningObjectTable;

		case 0x00000012:            // IID_RootStorage
			return ID_IRootStorage;

		case 0x00000018:            // IID_IStdMarshalInfo
			return ID_IStdMarshalInfo;

		case 0x00000101:            // IID_IEnumString
			return ID_IEnumString;

		case 0x00000102:            // IID_IEnumMoniker
			return ID_IEnumMoniker;

		case 0x00000103:            // IID_IEnumFORMATETC
			return ID_IEnumFORMATETC;

		case 0x00000104:            // IID_IEnumOLEVERB
			return ID_IEnumOLEVERB;

		case 0x00000105:            // IID_ISTATDATA
			return ID_IEnumSTATDATA;

		case 0x0000010a:            // IID_IPersistStorage
			return ID_IPersistStorage;

		case 0x0000010b:            // IID_IPersistFile
			return ID_IPersistFile;

		case 0x0000010e:            // IID_IDataObject
			return ID_IDataObject;

		case 0x00000110:            // IID_IDataAdviseHolder
			return ID_IDataAdviseHolder;

		case 0x00000111:            // IID_IOleAdviseHolder
			return ID_IOleAdviseHolder;

		case 0x00000112:            // IID_IOleObject
			return ID_IOleObject;

		case 0x00000113:            // IID_IOleInPlaceObject
			return ID_IOleInPlaceObject;

		case 0x00000114:                        // IID_IOleWindow
			return ID_IOleWindow;

		case 0x00000115:                        // IID_IOleInPlaceUIWindow
			return ID_IOleInPlaceUIWindow;

		case 0x00000116:            // IID_IOleInPlaceFrame
			return ID_IOleInPlaceFrame;

		case 0x00000117:            // IID_IOleInPlaceActiveObject
			return ID_IOleInPlaceActiveObject;

		case 0x00000118:            // IID_IOleClientSite
			return ID_IOleClientSite;

		case 0x00000119:            // IID_IOleInPlaceSite
			return ID_IOleInPlaceSite;

		case 0x0000011a:                        // IID_IParseDisplayName
			return ID_IParseDisplayName;

		case 0x0000011b:                        // IID_IOleContainer
			return ID_IOleContainer;

		case 0x0000011c:            // IID_IOleItemContainer
			return ID_IOleItemContainer;

		case 0x0000011d:            // IID_IOleLink
			return ID_IOleLink;

		case 0x00000122:            // IID_DropTarget
			return ID_IDropTarget;

		case 0x0000010f:            // IID_IAdviseSink
			return ID_IAdviseSink;

		case 0x00000125:            // IID_IAdviseSink2
			return ID_IAdviseSink2;

		case 0x00000126:            // IID_IRunnableObject
			return ID_IRunnableObject;

		case 0x0000010d:            // IID_IViewObject
			return ID_IViewObject;

		case 0x00000127:            // IID_IViewObject2
			return ID_IViewObject2;

		case 0x0000011e:            // IID_IOleCache
			return ID_IOleCache;

		case 0x00000128:            // IID_IOleCache2
			return ID_IOleCache2;

		case 0x00000129:            // IID_IOleCacheControl
			return ID_IOleCacheControl;

		case 0x00020400:
			return ID_IDispatch;

		case 0x00020404:
			return ID_IEnumVARIANT;

		case 0x00020401:
			return ID_ITypeInfo;

		case 0x00020402:
			return ID_ITypeLib;

		case 0x00020403:
			return ID_ITypeComp;

		case 0x00020405:
			return ID_ICreateTypeInfo;

		case 0x00020406:
			return ID_ICreateTypeLib;

		default:
			break;
		} // switch
	} //if
	else if ( ID_NULL != ( idRef = WrapTranslateControlIID(riid) ))
		return idRef;

#ifndef NOERRORINFO
	// Check for GUIDs that dont fit into the canonical system pattern
	if (riid == IID_IErrorInfo)
		return ID_IErrorInfo;

	if (riid == IID_ICreateErrorInfo)
		return ID_ICreateErrorInfo;
#endif //!NOERRORINFO

	// Blechh -- As a last resort, do really slow registry checks for
	// user-defined dispintefaces.
	if (IsValidDispInterface(riid))
		return ID_IDispatch;

#ifdef _DEBUG
	{
		char buf[256];
		wsprintf(buf, "? Interface %x", *(PULONG)&riid);
		TraceNotify(buf);
	}
#endif
	return ID_NULL;
}

// CONSIDER: make this faster, by doing as much of the string construction
// CONSIDER: as possible up-front.
int IsValidDispInterface(REFIID riid)
{
	static char szPSDispatch[] = "{00020420-0000-0000-C000-000000000046}";
	static char szPSAutomation[] = "{00020424-0000-0000-C000-000000000046}";

	long cb;
	const int bufSize = 256;
	char szKey[128], szValue[bufSize];

	// Construct ProxyStubClsid key for dispinterface
	lstrcpy(szKey, "Interface\\");
	StringFromGUID2A(riid, szKey+sizeof("Interface\\")-1, 40);
	lstrcat(szKey, "\\ProxyStubClsid32");

	// Check if valid dispinterface
	cb = bufSize;
	if (RegQueryValue(HKEY_CLASSES_ROOT, szKey, szValue, &cb) != ERROR_SUCCESS)
		return 0;

	if (!lstrcmpi(szValue, szPSDispatch) || !lstrcmpi(szValue, szPSAutomation))
	{
		// ok: found it
		return 1;
	}
	return 0;
}


//+--------------------------------------------------------------------------
//
//  Routine:    WrapAnyAFromW
//
//  Synopsis:   Wrap an Unnicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
LPUNKNOWN WrapAnyAFromW(LPUNKNOWN pObjOuter, IDINTERFACE idRef, LPUNKNOWN pObj)
{
	CInterface* pInterface;

	switch (idRef)
	{
	case ID_IClassFactory:
		pInterface = (CInterface*)new CClassFactoryA(pObjOuter, (IClassFactory *)pObj);
		break;
	case ID_IMarshal:
		pInterface = (CInterface*)new CMarshalA(pObjOuter, (IMarshal *)pObj);
		break;
	case ID_IEnumString:
		pInterface = (CInterface*)new CEnumStringA(pObjOuter, (IEnumString *)pObj);
		break;
	case ID_IEnumFORMATETC:
		pInterface = (CInterface*)new CEnumFORMATETCA(pObjOuter, (IEnumFORMATETC *)pObj);
		break;
	case ID_IEnumSTATSTG:
		pInterface = (CInterface*)new CEnumSTATSTGA(pObjOuter, (IEnumSTATSTG *)pObj);
		break;
	case ID_ILockBytes:
		pInterface = (CInterface*)new CLockBytesA(pObjOuter, (ILockBytes *)pObj);
		break;
	case ID_IStream:
		pInterface = (CInterface*)new CStreamA(pObjOuter, (IStream *)pObj);
		break;
	case ID_IStorage:
		pInterface = (CInterface*)new CStorageA(pObjOuter, (IStorage *)pObj);
		break;
	case ID_IRootStorage:
		pInterface = (CInterface*)new CRootStorageA(pObjOuter, (IRootStorage *)pObj);
		break;
	case ID_IStdMarshalInfo:
		pInterface = (CInterface*)new CStdMarshalInfoA(pObjOuter, (IStdMarshalInfo *)pObj);
		break;
#ifndef NOERRORINFO
	case ID_IErrorInfo:
		pInterface = (CInterface*)new CErrorInfoW(pObjOuter, (IErrorInfoA *)pObj);
		break;
	case ID_ICreateErrorInfo:
		pInterface = (CInterface*)new CCreateErrorInfoW(pObjOuter, (ICreateErrorInfoA *)pObj);
		break;
#endif //!NOERRORINFO
	case ID_IDropTarget:
		pInterface = (CInterface*)new CDropTargetA(pObjOuter, (IDropTarget *)pObj);
		break;
	case ID_IPersistStorage:
		pInterface = (CInterface*)new CPersistStorageA(pObjOuter, (IPersistStorage *)pObj);
		break;
	case ID_IPersistFile:
		pInterface = (CInterface*)new CPersistFileA(pObjOuter, (IPersistFile *)pObj);
		break;
	case ID_IBindCtx:
		pInterface = (CInterface*)new CBindCtxA(pObjOuter, (IBindCtx *)pObj);
		break;
	case ID_IPersistStream:
	case ID_IPersist:
	case ID_IMoniker:
		pInterface = (CInterface*)new CMonikerA(pObjOuter, (IMoniker *)pObj);
		break;
	case ID_IRunningObjectTable:
		pInterface = (CInterface*)new CRunningObjectTableA(pObjOuter, (IRunningObjectTable *)pObj);
		break;
	case ID_IEnumMoniker:
		pInterface = (CInterface*)new CEnumMonikerA(pObjOuter, (IEnumMoniker *)pObj);
		break;
	case ID_IEnumSTATDATA:
		pInterface = (CInterface*)new CEnumSTATDATAA(pObjOuter, (IEnumSTATDATA *)pObj);
		break;
	case ID_IDataObject:
		pInterface = (CInterface*)new CDataObjectA(pObjOuter, (IDataObject *)pObj);
		break;
	case ID_IViewObject:
	case ID_IViewObject2:
		pInterface = (CInterface*)new CViewObject2A(pObjOuter, (IViewObject2 *)pObj);
		break;
	case ID_IAdviseSink:
	case ID_IAdviseSink2:
		pInterface = (CInterface*)new CAdviseSink2A(pObjOuter, (IAdviseSink2 *)pObj);
		break;
	case ID_IDataAdviseHolder:
		pInterface = (CInterface*)new CDataAdviseHolderA(pObjOuter, (IDataAdviseHolder *)pObj);
		break;
		case ID_IOleCache:
	case ID_IOleCache2:
		pInterface = (CInterface*)new COleCache2A(pObjOuter, (IOleCache2 *)pObj);
		break;
	case ID_IOleCacheControl:
		pInterface = (CInterface*)new COleCacheControlA(pObjOuter, (IOleCacheControl *)pObj);
		break;
	case ID_IEnumOLEVERB:
		pInterface = (CInterface*)new CEnumOLEVERBA(pObjOuter, (IEnumOLEVERB *)pObj);
		break;
	case ID_IOleObject:
		pInterface = (CInterface*)new COleObjectA(pObjOuter, (IOleObject *)pObj);
		break;
	case ID_IOleClientSite:
		pInterface = (CInterface*)new COleClientSiteA(pObjOuter, (IOleClientSite *)pObj);
		break;
	case ID_IRunnableObject:
		pInterface = (CInterface*)new CRunnableObjectA(pObjOuter, (IRunnableObject *)pObj);
		break;
	case ID_IParseDisplayName:
	case ID_IOleContainer:
	case ID_IOleItemContainer:
		pInterface = (CInterface*)new COleItemContainerA(pObjOuter, (IOleItemContainer *)pObj);
		break;
	case ID_IOleAdviseHolder:
		pInterface = (CInterface*)new COleAdviseHolderA(pObjOuter, (IOleAdviseHolder *)pObj);
		break;
	case ID_IOleLink:
		pInterface = (CInterface*)new COleLinkA(pObjOuter, (IOleLink *)pObj);
		break;
	case ID_IOleInPlaceObject:
		pInterface = (CInterface*)new COleInPlaceObjectA(pObjOuter, (IOleInPlaceObject *)pObj);
		break;
	case ID_IOleInPlaceActiveObject:
		pInterface = (CInterface*)new COleInPlaceActiveObjectA(pObjOuter, (IOleInPlaceActiveObject *)pObj);
		break;
	case ID_IOleWindow:
	case ID_IOleInPlaceUIWindow:
	case ID_IOleInPlaceFrame:
		pInterface = (CInterface*)new COleInPlaceFrameA(pObjOuter, (IOleInPlaceFrame *)pObj);
		break;
	case ID_IOleInPlaceSite:
		pInterface = (CInterface*)new COleInPlaceSiteA(pObjOuter, (IOleInPlaceSite *)pObj);
		break;
	case ID_ITypeLib:
		pInterface = (CInterface*)new CTypeLibA(pObjOuter, (ITypeLib *)pObj);
		break;
	case ID_ITypeInfo:
		pInterface = (CInterface*)new CTypeInfoA(pObjOuter, (ITypeInfo *)pObj);
		break;
	case ID_ITypeComp:
		pInterface = (CInterface*)new CTypeCompA(pObjOuter, (ITypeComp *)pObj);
		break;
	case ID_ICreateTypeLib:
		pInterface = (CInterface*)new CCreateTypeLibA(pObjOuter, (ICreateTypeLib *)pObj);
		break;
	case ID_ICreateTypeInfo:
		pInterface = (CInterface*)new CCreateTypeInfoA(pObjOuter, (ICreateTypeInfo *)pObj);
		break;
	case ID_IEnumVARIANT:
		pInterface = (CInterface*)new CEnumVARIANTA(pObjOuter, (IEnumVARIANT *)pObj);
		break;
	case ID_IDispatch:
		pInterface = (CInterface*)new CDispatchA(pObjOuter, (IDispatch *)pObj);
		break;
	default:
		pInterface = WrapAnyControlAFromW(pObjOuter, idRef, pObj);
	}

	if (pInterface == NULL)
		TraceFatalError("WrapAnyAFromW ID_XXX entry mismatch");

	Assert(pInterface->IsANSI());
	pInterface->SetInterfaceId(idRef);
	WrapAddAnsiWrapper(idRef, pObj, pInterface);

	return pInterface;
}


//+--------------------------------------------------------------------------
//
//  Routine:    WrapAnyWFromA
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
LPUNKNOWN WrapAnyWFromA(LPUNKNOWN pObjOuter, IDINTERFACE idRef, LPUNKNOWN pObj)
{
		CInterface* pInterface;

	switch (idRef)
	{
	case ID_IClassFactory:
		pInterface = (CInterface*)new CClassFactoryW(pObjOuter, (IClassFactory *)pObj);
		break;
	case ID_IMarshal:
		pInterface = (CInterface*)new CMarshalW(pObjOuter, (IMarshalA *)pObj);
		break;
	case ID_IEnumString:
		pInterface = (CInterface*)new CEnumStringW(pObjOuter, (IEnumStringA *)pObj);
		break;
	case ID_IEnumFORMATETC:
		pInterface = (CInterface*)new CEnumFORMATETCW(pObjOuter, (IEnumFORMATETCA *)pObj);
		break;
	case ID_IEnumSTATSTG:
		pInterface = (CInterface*)new CEnumSTATSTGW(pObjOuter, (IEnumSTATSTGA *)pObj);
		break;
	case ID_ILockBytes:
		pInterface = (CInterface*)new CLockBytesW(pObjOuter, (ILockBytesA *)pObj);
		break;
	case ID_IStream:
		pInterface = (CInterface*)new CStreamW(pObjOuter, (IStreamA *)pObj);
		break;
	case ID_IStorage:
		pInterface = (CInterface*)new CStorageW(pObjOuter, (IStorageA *)pObj);
		break;
	case ID_IRootStorage:
		pInterface = (CInterface*)new CRootStorageW(pObjOuter, (IRootStorageA *)pObj);
		break;
	case ID_IStdMarshalInfo:
		pInterface = (CInterface*)new CStdMarshalInfoW(pObjOuter, (IStdMarshalInfoA *)pObj);
		break;
#ifndef NOERRORINFO
	case ID_IErrorInfo:
		pInterface = (CInterface*)new CErrorInfoA(pObjOuter, (IErrorInfo *)pObj);
		break;
	case ID_ICreateErrorInfo:
		pInterface = (CInterface*)new CCreateErrorInfoA(pObjOuter, (ICreateErrorInfo *)pObj);
		break;
#endif //!NOERRORINFO
	case ID_IDropTarget:
		pInterface = (CInterface*)new CDropTargetW(pObjOuter, (IDropTargetA *)pObj);
		break;
	case ID_IPersistStorage:
		pInterface = (CInterface*)new CPersistStorageW(pObjOuter, (IPersistStorageA *)pObj);
		break;
	case ID_IPersistFile:
		pInterface = (CInterface*)new CPersistFileW(pObjOuter, (IPersistFileA *)pObj);
		break;
	case ID_IBindCtx:
		pInterface = (CInterface*)new CBindCtxW(pObjOuter, (IBindCtxA *)pObj);
		break;
	case ID_IPersistStream:
	case ID_IPersist:
	case ID_IMoniker:
		pInterface = (CInterface*)new CMonikerW(pObjOuter, (IMonikerA *)pObj);
		break;
	case ID_IRunningObjectTable:
		pInterface = (CInterface*)new CRunningObjectTableW(pObjOuter, (IRunningObjectTableA *)pObj);
		break;
	case ID_IEnumMoniker:
		pInterface = (CInterface*)new CEnumMonikerW(pObjOuter, (IEnumMonikerA *)pObj);
		break;
	case ID_IEnumSTATDATA:
		pInterface = (CInterface*)new CEnumSTATDATAW(pObjOuter, (IEnumSTATDATAA *)pObj);
		break;
	case ID_IDataObject:
		pInterface = (CInterface*)new CDataObjectW(pObjOuter, (IDataObjectA *)pObj);
		break;
	case ID_IViewObject:
	case ID_IViewObject2:
		pInterface = (CInterface*)new CViewObject2W(pObjOuter, (IViewObject2A *)pObj);
		break;
	case ID_IAdviseSink:
	case ID_IAdviseSink2:
		pInterface = (CInterface*)new CAdviseSink2W(pObjOuter, (IAdviseSink2A *)pObj);
		break;
	case ID_IDataAdviseHolder:
		pInterface = (CInterface*)new CDataAdviseHolderW(pObjOuter, (IDataAdviseHolderA *)pObj);
		break;
		case ID_IOleCache:
	case ID_IOleCache2:
		pInterface = (CInterface*)new COleCache2W(pObjOuter, (IOleCache2A *)pObj);
		break;
	case ID_IOleCacheControl:
		pInterface = (CInterface*)new COleCacheControlW(pObjOuter, (IOleCacheControlA *)pObj);
		break;
	case ID_IEnumOLEVERB:
		pInterface = (CInterface*)new CEnumOLEVERBW(pObjOuter, (IEnumOLEVERBA *)pObj);
		break;
	case ID_IOleObject:
		pInterface = (CInterface*)new COleObjectW(pObjOuter, (IOleObjectA *)pObj);
		break;
	case ID_IOleClientSite:
		pInterface = (CInterface*)new COleClientSiteW(pObjOuter, (IOleClientSiteA *)pObj);
		break;
	case ID_IRunnableObject:
		pInterface = (CInterface*)new CRunnableObjectW(pObjOuter, (IRunnableObjectA *)pObj);
		break;
	case ID_IParseDisplayName:
	case ID_IOleContainer:
	case ID_IOleItemContainer:
		pInterface = (CInterface*)new COleItemContainerW(pObjOuter, (IOleItemContainerA *)pObj);
		break;
	case ID_IOleAdviseHolder:
		pInterface = (CInterface*)new COleAdviseHolderW(pObjOuter, (IOleAdviseHolderA *)pObj);
		break;
	case ID_IOleLink:
		pInterface = (CInterface*)new COleLinkW(pObjOuter, (IOleLinkA *)pObj);
		break;
	case ID_IOleInPlaceObject:
		pInterface = (CInterface*)new COleInPlaceObjectW(pObjOuter, (IOleInPlaceObjectA *)pObj);
		break;
	case ID_IOleInPlaceActiveObject:
		pInterface = (CInterface*)new COleInPlaceActiveObjectW(pObjOuter, (IOleInPlaceActiveObjectA *)pObj);
		break;
	case ID_IOleWindow:
	case ID_IOleInPlaceUIWindow:
	case ID_IOleInPlaceFrame:
		pInterface = (CInterface*)new COleInPlaceFrameW(pObjOuter, (IOleInPlaceFrameA *)pObj);
		break;
	case ID_IOleInPlaceSite:
		pInterface = (CInterface*)new COleInPlaceSiteW(pObjOuter, (IOleInPlaceSiteA *)pObj);
		break;
	case ID_ITypeLib:
		pInterface = (CInterface*)new CTypeLibW(pObjOuter, (ITypeLibA *)pObj);
		break;
	case ID_ITypeInfo:
		pInterface = (CInterface*)new CTypeInfoW(pObjOuter, (ITypeInfoA *)pObj);
		break;
	case ID_ITypeComp:
		pInterface = (CInterface*)new CTypeCompW(pObjOuter, (ITypeCompA *)pObj);
		break;
	case ID_ICreateTypeLib:
		pInterface = (CInterface*)new CCreateTypeLibW(pObjOuter, (ICreateTypeLibA *)pObj);
		break;
	case ID_ICreateTypeInfo:
		pInterface = (CInterface*)new CCreateTypeInfoW(pObjOuter, (ICreateTypeInfoA *)pObj);
		break;
	case ID_IEnumVARIANT:
		pInterface = (CInterface*)new CEnumVARIANTW(pObjOuter, (IEnumVARIANTA *)pObj);
		break;
	case ID_IDispatch:
		pInterface = (CInterface*)new CDispatchW(pObjOuter, (IDispatchA *)pObj);
		break;
	default:
		pInterface = WrapAnyControlWFromA(pObjOuter, idRef, pObj);
	}

	if (pInterface == NULL)
		TraceFatalError("WrapAnyWFromA ID_XXX entry mismatch");

	Assert(!pInterface->IsANSI());
	pInterface->SetInterfaceId(idRef);
	WrapAddWideWrapper(idRef, pObj, pInterface);

	return pInterface;
}


//+--------------------------------------------------------------------------
//
//  Routine:    IUnknownAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
STDAPI WrapIUnknownAFromW(LPUNKNOWN pobj, LPUNKNOWN * ppobjA)
{
	return WrapInterfaceAFromW(ID_IUnknown, pobj, ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IUnknownWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
STDAPI WrapIUnknownWFromA(LPUNKNOWN pobjA, LPUNKNOWN * ppobj)
{
	return WrapInterfaceWFromA(ID_IUnknown, pobjA, ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IClassFactoryAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIClassFactoryAFromW(LPCLASSFACTORY pobj, LPCLASSFACTORYA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IClassFactory, pobj, (LPUNKNOWN *)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IClassFactoryWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIClassFactoryWFromA(LPCLASSFACTORYA pobjA, LPCLASSFACTORY * ppobj)
{
	return WrapInterfaceWFromA(ID_IClassFactory, pobjA, (LPUNKNOWN *)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IMarshalAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIMarshalAFromW(LPMARSHAL pobj, LPMARSHALA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IMarshal, pobj, (LPUNKNOWN *)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IMarshalWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIMarshalWFromA(LPMARSHALA pobjA, LPMARSHAL * ppobj)
{
	return WrapInterfaceWFromA(ID_IMarshal, pobjA, (LPUNKNOWN *)ppobj);
}


#ifndef NOERRORINFO
//+--------------------------------------------------------------------------
//
//  Routine:    WrapIErrorInfoWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIErrorInfoWFromA(IErrorInfoA *pobjA, IErrorInfo ** ppobj)
{
	return WrapInterfaceWFromA(ID_IErrorInfo, pobjA, (LPUNKNOWN *)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    WrapIErrorInfoAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIErrorInfoAFromW(IErrorInfo *pobj, IErrorInfoA ** ppobjA)
{
	return WrapInterfaceAFromW(ID_IErrorInfo, pobj, (LPUNKNOWN *)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    WrapICreateErrorInfoWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapICreateErrorInfoWFromA(ICreateErrorInfoA *pobjA, ICreateErrorInfo ** ppobj)
{
	return WrapInterfaceWFromA(ID_ICreateErrorInfo, pobjA, (LPUNKNOWN *)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    WrapICreateErrorInfoAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapICreateErrorInfoAFromW(ICreateErrorInfo *pobj, ICreateErrorInfoA ** ppobjA)
{
	return WrapInterfaceAFromW(ID_ICreateErrorInfo, pobj, (LPUNKNOWN *)ppobjA);
}
#endif //!NOERRORINFO

//+--------------------------------------------------------------------------
//
//  Routine:    IEnumStringAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIEnumStringAFromW(LPENUMSTRING pobj, LPENUMSTRINGA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IEnumString, pobj, (LPUNKNOWN *)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IEnumStringWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIEnumStringWFromA(LPENUMSTRINGA pobjA, LPENUMSTRING * ppobj)
{
	return WrapInterfaceWFromA(ID_IEnumString, pobjA, (LPUNKNOWN *)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IEnumFORMATETCAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIEnumFORMATETCAFromW(LPENUMFORMATETC pobj, LPENUMFORMATETCA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IEnumFORMATETC, pobj, (LPUNKNOWN *)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IEnumFORMATETCWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIEnumFORMATETCWFromA(LPENUMFORMATETCA pobjA, LPENUMFORMATETC * ppobj)
{
	return WrapInterfaceWFromA(ID_IEnumFORMATETC, pobjA, (LPUNKNOWN *)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IEnumSTATSTGAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIEnumSTATSTGAFromW(LPENUMSTATSTG pobj, LPENUMSTATSTGA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IEnumSTATSTG, pobj, (LPUNKNOWN *)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IEnumSTATSTGWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIEnumSTATSTGWFromA(LPENUMSTATSTGA pobjA, LPENUMSTATSTG * ppobj)
{
	return WrapInterfaceWFromA(ID_IEnumSTATSTG, pobjA, (LPUNKNOWN *)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ILockBytesAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapILockBytesAFromW(LPLOCKBYTES pobj, LPLOCKBYTESA * ppobjA)
{
	return WrapInterfaceAFromW(ID_ILockBytes, pobj, (LPUNKNOWN *)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ILockBytesWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapILockBytesWFromA(LPLOCKBYTESA pobjA, LPLOCKBYTES * ppobj)
{
	return WrapInterfaceWFromA(ID_ILockBytes, pobjA, (LPUNKNOWN *)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IStreamAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIStreamAFromW(LPSTREAM pobj, LPSTREAMA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IStream, pobj, (LPUNKNOWN *)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IStreamWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIStreamWFromA(LPSTREAMA pobjA, LPSTREAM * ppobj)
{
	return WrapInterfaceWFromA(ID_IStream, pobjA, (LPUNKNOWN *)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IStorageAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
STDAPI WrapIStorageAFromW(LPSTORAGE pobj, LPSTORAGEA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IStorage, pobj, (LPUNKNOWN *)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IStorageWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
STDAPI WrapIStorageWFromA(LPSTORAGEA pobjA, LPSTORAGE * ppobj)
{
	return WrapInterfaceWFromA(ID_IStorage, pobjA, (LPUNKNOWN *)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IRootStorageAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIRootStorageAFromW(LPROOTSTORAGE pobj, LPROOTSTORAGEA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IRootStorage, pobj, (LPUNKNOWN *)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IRootStorageWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIRootStorageWFromA(LPROOTSTORAGEA pobjA, LPROOTSTORAGE * ppobj)
{
	return WrapInterfaceWFromA(ID_IRootStorage, pobjA, (LPUNKNOWN *)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IBindCtxAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIBindCtxAFromW(LPBINDCTX pobj, LPBINDCTXA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IBindCtx, pobj, (LPUNKNOWN *)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IBindCtxWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIBindCtxWFromA(LPBINDCTXA pobjA, LPBINDCTX * ppobj)
{
	return WrapInterfaceWFromA(ID_IBindCtx, pobjA, (LPUNKNOWN *)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IMonikerAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIMonikerAFromW(LPMONIKER pobj, LPMONIKERA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IMoniker, pobj, (LPUNKNOWN *)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IMonikerWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIMonikerWFromA(LPMONIKERA pobjA, LPMONIKER * ppobj)
{
	return WrapInterfaceWFromA(ID_IMoniker, pobjA, (LPUNKNOWN *)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IRunningObjectTableAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIRunningObjectTableAFromW(LPRUNNINGOBJECTTABLE pobj, LPRUNNINGOBJECTTABLEA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IRunningObjectTable, pobj, (LPUNKNOWN *)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IRunningObjectTableWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIRunningObjectTableWFromA(LPRUNNINGOBJECTTABLEA pobjA, LPRUNNINGOBJECTTABLE * ppobj)
{
	return WrapInterfaceWFromA(ID_IRunningObjectTable, pobjA, (LPUNKNOWN *)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IEnumMonikerAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIEnumMonikerAFromW(LPENUMMONIKER pobj, LPENUMMONIKERA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IEnumMoniker, pobj, (LPUNKNOWN *)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IEnumMonikerWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIEnumMonikerWFromA(LPENUMMONIKERA pobjA, LPENUMMONIKER * ppobj)
{
	return WrapInterfaceWFromA(ID_IEnumMoniker, pobjA, (LPUNKNOWN *)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IEnumSTATDATAAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIEnumSTATDATAAFromW(LPENUMSTATDATA pobj, LPENUMSTATDATAA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IEnumSTATDATA, pobj, (LPUNKNOWN *)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IEnumSTATDATAWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIEnumSTATDATAWFromA(LPENUMSTATDATAA pobjA, LPENUMSTATDATA * ppobj)
{
	return WrapInterfaceWFromA(ID_IEnumSTATDATA, pobjA, (LPUNKNOWN *)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IDataObjectAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIDataObjectAFromW(LPDATAOBJECT pobj, LPDATAOBJECTA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IDataObject, pobj, (LPUNKNOWN *)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IDataObjectWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIDataObjectWFromA(LPDATAOBJECTA pobjA, LPDATAOBJECT * ppobj)
{
	return WrapInterfaceWFromA(ID_IDataObject, pobjA, (LPUNKNOWN *)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IViewObjectAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIViewObjectAFromW(LPVIEWOBJECT pobj, LPVIEWOBJECTA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IViewObject, pobj, (LPUNKNOWN*)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IViewObjectWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIViewObjectWFromA(LPVIEWOBJECTA pobjA, LPVIEWOBJECT * ppobj)
{
	return WrapInterfaceWFromA(ID_IViewObject, pobjA, (LPUNKNOWN*)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IViewObject2AFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIViewObject2AFromW(LPVIEWOBJECT2 pobj, LPVIEWOBJECT2A * ppobjA)
{
	return WrapInterfaceAFromW(ID_IViewObject2, pobj, (LPUNKNOWN *)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IViewObject2WFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIViewObject2WFromA(LPVIEWOBJECT2A pobjA, LPVIEWOBJECT2 * ppobj)
{
	return WrapInterfaceWFromA(ID_IViewObject2, pobjA, (LPUNKNOWN *)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IAdviseSinkAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIAdviseSinkAFromW(LPADVISESINK pobj, LPADVISESINKA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IAdviseSink, pobj, (LPUNKNOWN*)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IAdviseSinkWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIAdviseSinkWFromA(LPADVISESINKA pobjA, LPADVISESINK * ppobj)
{
	return WrapInterfaceWFromA(ID_IAdviseSink, pobjA, (LPUNKNOWN*)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IAdviseSink2AFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIAdviseSink2AFromW(LPADVISESINK2 pobj, LPADVISESINK2A * ppobjA)
{
	return WrapInterfaceAFromW(ID_IAdviseSink2, pobj, (LPUNKNOWN *)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IAdviseSink2WFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIAdviseSink2WFromA(LPADVISESINK2A pobjA, LPADVISESINK2 * ppobj)
{
	return WrapInterfaceWFromA(ID_IAdviseSink2, pobjA, (LPUNKNOWN *)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IDataAdviseHolderAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIDataAdviseHolderAFromW(LPDATAADVISEHOLDER pobj, LPDATAADVISEHOLDERA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IDataAdviseHolder, pobj, (LPUNKNOWN *)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IDataAdviseHolderWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIDataAdviseHolderWFromA(LPDATAADVISEHOLDERA pobjA, LPDATAADVISEHOLDER * ppobj)
{
	return WrapInterfaceWFromA(ID_IDataAdviseHolder, pobjA, (LPUNKNOWN *)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IOleCacheAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIOleCacheAFromW(LPOLECACHE pobj, LPOLECACHEA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IOleCache, pobj, (LPUNKNOWN*)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IOleCacheWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIOleCacheWFromA(LPOLECACHEA pobjA, LPOLECACHE * ppobj)
{
	return WrapInterfaceWFromA(ID_IOleCache, pobjA, (LPUNKNOWN*)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IOleCache2AFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIOleCache2AFromW(LPOLECACHE2 pobj, LPOLECACHE2A * ppobjA)
{
	return WrapInterfaceAFromW(ID_IOleCache2, pobj, (LPUNKNOWN *)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IOleCache2WFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIOleCache2WFromA(LPOLECACHE2A pobjA, LPOLECACHE2 * ppobj)
{
	return WrapInterfaceWFromA(ID_IOleCache2, pobjA, (LPUNKNOWN *)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IOleCacheControlAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIOleCacheControlAFromW(LPOLECACHECONTROL pobj, LPOLECACHECONTROLA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IOleCacheControl, pobj, (LPUNKNOWN *)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IOleCacheControlWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIOleCacheControlWFromA(LPOLECACHECONTROLA pobjA, LPOLECACHECONTROL * ppobj)
{
	return WrapInterfaceWFromA(ID_IOleCacheControl, pobjA, (LPUNKNOWN *)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IDropTargetAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIDropTargetAFromW(LPDROPTARGET pobj, LPDROPTARGETA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IDropTarget, pobj, (LPUNKNOWN *)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IDropTargetWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIDropTargetWFromA(LPDROPTARGETA pobjA, LPDROPTARGET * ppobj)
{
	return WrapInterfaceWFromA(ID_IDropTarget, pobjA, (LPUNKNOWN *)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IPersistStreamAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIPersistStreamAFromW(LPPERSISTSTREAM pobj, LPPERSISTSTREAMA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IMoniker, pobj, (LPUNKNOWN *)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IPersistStreamWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIPersistStreamWFromA(LPPERSISTSTREAMA pobjA, LPPERSISTSTREAM * ppobj)
{
	return WrapInterfaceWFromA(ID_IMoniker, pobjA, (LPUNKNOWN *)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IPersistAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIPersistAFromW(LPPERSIST pobj, LPPERSIST * ppobjA)
{
	return WrapInterfaceAFromW(ID_IMoniker, pobj, (LPUNKNOWN *)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IPersistWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIPersistWFromA(LPPERSIST pobjA, LPPERSIST * ppobj)
{
	return WrapInterfaceWFromA(ID_IPersist, pobjA, (LPUNKNOWN *)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IPersistStorageAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIPersistStorageAFromW(LPPERSISTSTORAGE pobj, LPPERSISTSTORAGEA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IPersistStorage, pobj, (LPUNKNOWN *)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IPersistStorageWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIPersistStorageWFromA(LPPERSISTSTORAGEA pobjA, LPPERSISTSTORAGE * ppobj)
{
	return WrapInterfaceWFromA(ID_IPersistStorage, pobjA, (LPUNKNOWN *)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IPersistFileAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIPersistFileAFromW(LPPERSISTFILE pobj, LPPERSISTFILEA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IPersistFile, pobj, (LPUNKNOWN *)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IPersistFileWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIPersistFileWFromA(LPPERSISTFILEA pobjA, LPPERSISTFILE * ppobj)
{
	return WrapInterfaceWFromA(ID_IPersistFile, pobjA, (LPUNKNOWN *)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IEnumOLEVERBAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIEnumOLEVERBAFromW(LPENUMOLEVERB pobj, LPENUMOLEVERBA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IEnumOLEVERB, pobj, (LPUNKNOWN *)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IEnumOLEVERBWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIEnumOLEVERBWFromA(LPENUMOLEVERBA pobjA, LPENUMOLEVERB * ppobj)
{
	return WrapInterfaceWFromA(ID_IEnumOLEVERB, pobjA, (LPUNKNOWN *)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IOleObjectAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIOleObjectAFromW(LPOLEOBJECT pobj, LPOLEOBJECTA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IOleObject, pobj, (LPUNKNOWN *)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IOleObjectWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIOleObjectWFromA(LPOLEOBJECTA pobjA, LPOLEOBJECT * ppobj)
{
	return WrapInterfaceWFromA(ID_IOleObject, pobjA, (LPUNKNOWN *)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IOleClientSiteAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIOleClientSiteAFromW(LPOLECLIENTSITE pobj, LPOLECLIENTSITEA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IOleClientSite, pobj, (LPUNKNOWN *)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IOleClientSiteWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIOleClientSiteWFromA(LPOLECLIENTSITEA pobjA, LPOLECLIENTSITE * ppobj)
{
	return WrapInterfaceWFromA(ID_IOleClientSite, pobjA, (LPUNKNOWN *)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IRunnableObjectAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIRunnableObjectAFromW(LPRUNNABLEOBJECT pobj, LPRUNNABLEOBJECTA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IRunnableObject, pobj, (LPUNKNOWN *)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IRunnableObjectWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIRunnableObjectWFromA(LPRUNNABLEOBJECTA pobjA, LPRUNNABLEOBJECT * ppobj)
{
	return WrapInterfaceWFromA(ID_IRunnableObject, pobjA, (LPUNKNOWN *)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IParseDisplayNameAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIParseDisplayNameAFromW(LPPARSEDISPLAYNAME pobj, LPPARSEDISPLAYNAMEA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IParseDisplayName, pobj, (LPUNKNOWN *)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IParseDisplayNameWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIParseDisplayNameWFromA(LPPARSEDISPLAYNAMEA pobjA, LPPARSEDISPLAYNAME * ppobj)
{
	return WrapInterfaceWFromA(ID_IParseDisplayName, pobjA, (LPUNKNOWN *)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IOleContainerAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIOleContainerAFromW(LPOLECONTAINER pobj, LPOLECONTAINERA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IOleItemContainer, pobj, (LPUNKNOWN *)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IOleContainerWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIOleContainerWFromA(LPOLECONTAINERA pobjA, LPOLECONTAINER * ppobj)
{
	return WrapInterfaceWFromA(ID_IOleItemContainer, pobjA, (LPUNKNOWN *)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IOleItemContainerAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIOleItemContainerAFromW(LPOLEITEMCONTAINER pobj, LPOLEITEMCONTAINERA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IOleItemContainer, pobj, (LPUNKNOWN *)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IOleItemContainerWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIOleItemContainerWFromA(LPOLEITEMCONTAINERA pobjA, LPOLEITEMCONTAINER * ppobj)
{
	return WrapInterfaceWFromA(ID_IOleItemContainer, pobjA, (LPUNKNOWN *)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IOleAdviseHolderAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIOleAdviseHolderAFromW(LPOLEADVISEHOLDER pobj, LPOLEADVISEHOLDERA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IOleAdviseHolder, pobj, (LPUNKNOWN *)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IOleAdviseHolderWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIOleAdviseHolderWFromA(LPOLEADVISEHOLDERA pobjA, LPOLEADVISEHOLDER * ppobj)
{
	return WrapInterfaceWFromA(ID_IOleAdviseHolder, pobjA, (LPUNKNOWN *)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IOleLinkAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIOleLinkAFromW(LPOLELINK pobj, LPOLELINKA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IOleLink, pobj, (LPUNKNOWN *)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IOleLinkWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIOleLinkWFromA(LPOLELINKA pobjA, LPOLELINK * ppobj)
{
	return WrapInterfaceWFromA(ID_IOleLink, pobjA, (LPUNKNOWN *)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IOleInPlaceObjectAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIOleInPlaceObjectAFromW(LPOLEINPLACEOBJECT pobj, LPOLEINPLACEOBJECTA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IOleInPlaceObject, pobj, (LPUNKNOWN *)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IOleInPlaceObjectWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIOleInPlaceObjectWFromA(LPOLEINPLACEOBJECTA pobjA, LPOLEINPLACEOBJECT * ppobj)
{
	return WrapInterfaceWFromA(ID_IOleInPlaceObject, pobjA, (LPUNKNOWN *)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IOleInPlaceActiveObjectAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIOleInPlaceActiveObjectAFromW(LPOLEINPLACEACTIVEOBJECT pobj, LPOLEINPLACEACTIVEOBJECTA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IOleInPlaceActiveObject, pobj, (LPUNKNOWN *)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IOleInPlaceActiveObjectWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIOleInPlaceActiveObjectWFromA(LPOLEINPLACEACTIVEOBJECTA pobjA, LPOLEINPLACEACTIVEOBJECT * ppobj)
{
	return WrapInterfaceWFromA(ID_IOleInPlaceActiveObject, pobjA, (LPUNKNOWN *)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IOleInPlaceUIWindowAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIOleInPlaceUIWindowAFromW(LPOLEINPLACEUIWINDOW pobj, LPOLEINPLACEUIWINDOWA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IOleInPlaceUIWindow, pobj, (LPUNKNOWN *)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IOleInPlaceUIWindowWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIOleInPlaceUIWindowWFromA(LPOLEINPLACEUIWINDOWA pobjA, LPOLEINPLACEUIWINDOW * ppobj)
{
	return WrapInterfaceWFromA(ID_IOleInPlaceUIWindow, pobjA, (LPUNKNOWN *)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IOleInPlaceFrameAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIOleInPlaceFrameAFromW(LPOLEINPLACEFRAME pobj, LPOLEINPLACEFRAMEA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IOleInPlaceFrame, pobj, (LPUNKNOWN *)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IOleInPlaceFrameWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIOleInPlaceFrameWFromA(LPOLEINPLACEFRAMEA pobjA, LPOLEINPLACEFRAME * ppobj)
{
	return WrapInterfaceWFromA(ID_IOleInPlaceFrame, pobjA, (LPUNKNOWN *)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IOleInPlaceSiteAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIOleInPlaceSiteAFromW(LPOLEINPLACESITE pobj, LPOLEINPLACESITEA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IOleInPlaceSite, pobj, (LPUNKNOWN *)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IOleInPlaceSiteWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIOleInPlaceSiteWFromA(LPOLEINPLACESITEA pobjA, LPOLEINPLACESITE * ppobj)
{
	return WrapInterfaceWFromA(ID_IOleInPlaceSite, pobjA, (LPUNKNOWN *)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ITypeLibAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapITypeLibAFromW(LPTYPELIB pobj, LPTYPELIBA * ppobjA)
{
	return WrapInterfaceAFromW(ID_ITypeLib, pobj, (LPUNKNOWN *)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ITypeLibWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapITypeLibWFromA(LPTYPELIBA pobjA, LPTYPELIB * ppobj)
{
	return WrapInterfaceWFromA(ID_ITypeLib, pobjA, (LPUNKNOWN *)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ITypeInfoAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapITypeInfoAFromW(LPTYPEINFO pobj, LPTYPEINFOA * ppobjA)
{
	return WrapInterfaceAFromW(ID_ITypeInfo, pobj, (LPUNKNOWN *)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ITypeInfoWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapITypeInfoWFromA(LPTYPEINFOA pobjA, LPTYPEINFO * ppobj)
{
	return WrapInterfaceWFromA(ID_ITypeInfo, pobjA, (LPUNKNOWN *)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ITypeCompAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapITypeCompAFromW(LPTYPECOMP pobj, LPTYPECOMPA * ppobjA)
{
	return WrapInterfaceAFromW(ID_ITypeComp, pobj, (LPUNKNOWN *)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ITypeCompWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapITypeCompWFromA(LPTYPECOMPA pobjA, LPTYPECOMP * ppobj)
{
	return WrapInterfaceWFromA(ID_ITypeComp, pobjA, (LPUNKNOWN *)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ICreateTypeLibAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapICreateTypeLibAFromW(LPCREATETYPELIB pobj, LPCREATETYPELIBA * ppobjA)
{
	return WrapInterfaceAFromW(ID_ICreateTypeLib, pobj, (LPUNKNOWN *)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ICreateTypeLibWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapICreateTypeLibWFromA(LPCREATETYPELIBA pobjA, LPCREATETYPELIB * ppobj)
{
	return WrapInterfaceWFromA(ID_ICreateTypeLib, pobjA, (LPUNKNOWN *)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ICreateTypeInfoAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapICreateTypeInfoAFromW(LPCREATETYPEINFO pobj, LPCREATETYPEINFOA * ppobjA)
{
	return WrapInterfaceAFromW(ID_ICreateTypeInfo, pobj, (LPUNKNOWN *)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ICreateTypeInfoWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapICreateTypeInfoWFromA(LPCREATETYPEINFOA pobjA, LPCREATETYPEINFO * ppobj)
{
	return WrapInterfaceWFromA(ID_ICreateTypeInfo, pobjA, (LPUNKNOWN *)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IEnumVARIANTAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIEnumVARIANTAFromW(LPENUMVARIANT pobj, LPENUMVARIANTA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IEnumVARIANT, pobj, (LPUNKNOWN *)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IEnumVARIANTWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapIEnumVARIANTWFromA(LPENUMVARIANTA pobjA, LPENUMVARIANT * ppobj)
{
	return WrapInterfaceWFromA(ID_IEnumVARIANT, pobjA, (LPUNKNOWN *)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IDispatchAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
STDAPI WrapIDispatchAFromW(LPDISPATCH pobj, LPDISPATCHA * ppobjA)
{
	return WrapInterfaceAFromW(ID_IDispatch, pobj, (LPUNKNOWN *)ppobjA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    IDispatchWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
STDAPI WrapIDispatchWFromA(LPDISPATCHA pobjA, LPDISPATCH * ppobj)
{
	return WrapInterfaceWFromA(ID_IDispatch, pobjA, (LPUNKNOWN *)ppobj);
}


//+--------------------------------------------------------------------------
//
//  Routine:    InterfaceAFromW
//
//  Synopsis:   Wrap an Unicode object with an ANSI thunking object.
//
//  Returns:    Address of the ANSI thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapInterfaceAFromW(IDINTERFACE idRef, LPUNKNOWN pobj, LPUNKNOWN * ppobjA)
{
	PWRAPPER  pWrapper;
	LPUNKNOWN pUnk = NULL;
	HRESULT   hResult;


	TraceWrapper("WrapInterfaceAFromW");

#ifdef _DEBUG
	{
		char buf[256];
			wsprintf(buf, "\tid %d, Ansi %x", idRef, pobj);
		TraceNotify(buf);
	}
#endif

	//
	//
	//
	if (idRef == ID_NULL || pobj == NULL)
		{
				if (pobj != NULL)
						pobj->AddRef();
		*ppobjA = pobj;
		return ResultFromScode(S_OK);
	}

	EnterCriticalSection(&CriticalSection);
	WRAP_DATA* pData = GetWrapData();

	//
	//
	//
	*ppobjA = NULL;

	//
	//  If the Wide Interface is contained in the cache, then it is a wrapper.
	//  Given a known wrapper, the wrappee can be received directly without
	//  the overhead of scanning the wrapper table.
	//
	if (pobj == pData->aWideWrapperCache[idRef])
	{
		*ppobjA = ((CUnknownW *)pobj)->GetWrappee();
		TraceAddRef("Cache hit", *ppobjA, (*ppobjA)->AddRef());
		goto Return;
	}

	//
	//  Determine if the Unicode interface that needs to be wrapped is
	//  actually a wrapper of an ANSI interface.  If so, just grab the
	//  ANSI interface and return it.
	//
	if (idRef != ID_IUnknown)
	{
		pWrapper = pData->pWideWrappers;
		while (pWrapper)
		{
			if (pWrapper->Wrapper == pobj)
			{
				if (pWrapper->idRef == idRef)
				{
					*ppobjA = pWrapper->Wrappee;
					TraceAddRef("Unwrap native from wrapper", *ppobjA, (*ppobjA)->AddRef());
					pData->aWideWrapperCache[idRef] = pobj;
					goto Return;
				}
			}

			if (pWrapper->Wrappee == pobj)
				TraceFatalError("Internal wrapper mismatch 2");

		pWrapper = pWrapper->pNext;
		}
	}

	//
	//  Search the Ansi table to determine if this interface has already been
	//  wrapped.  If is has, AddRef the current wrapper and return it.
	//
	pWrapper = pData->pAnsiWrappers;
	while (pWrapper)
	{
		if (pWrapper->Wrappee == pobj)
		{
			*ppobjA = pWrapper->Wrapper;
			if (pWrapper->idRef == idRef)
			{
				TraceAddRef("Unicode interface already wrapped", *ppobjA, (*ppobjA)->AddRef());
				goto Done;
			}
		}

		if (pWrapper->Wrapper == pobj)
			TraceFatalError("Internal wrapper mismatch 1");

		pWrapper = pWrapper->pNext;
	}

	//
	//  The Unicode interface is already wrapped by another interface, so just
	//  wrap it and merge into the current wrapper object.
	//
	if (*ppobjA != NULL)
	{
		(*ppobjA)->QueryInterface(IID_IUnknown, (LPVOID *)&pUnk);
	}

	//
	//  If no wrapper Unknown yet, then create one now.
	//
	if (pUnk == NULL)
	{
		LPUNKNOWN pUnkW;

		hResult = pobj->QueryInterface(IID_IUnknown, (LPVOID *)&pUnkW);
		if (FAILED(hResult))
			goto Error;

		pWrapper = pData->pAnsiWrappers;
		while (pWrapper)
		{
			if ((pWrapper->Wrappee == pUnkW) && pWrapper->idRef == ID_IUnknown)
			{
				pUnk = pWrapper->Wrapper;
				pUnk->AddRef();
				break;
			}

		pWrapper = pWrapper->pNext;
		}

		if (pUnk == NULL)
		{
			pUnk = new CUnknownA(pUnkW, idRef);
			if (pUnk == NULL)
			{
				hResult = ResultFromScode(E_OUTOFMEMORY);
				goto Error;
			}

			WrapAddAnsiWrapper(ID_IUnknown, pUnkW, pUnk);
		}

		pUnkW->Release();
	}

	//
	//  If all we wanted was an Unknown then we're done.
	//
	if (idRef == ID_IUnknown)
	{
		*ppobjA = pUnk;
		goto Done;
	}

	*ppobjA = WrapAnyAFromW(pUnk, idRef, pobj);
	if (*ppobjA == NULL)
	{
		hResult = ResultFromScode(E_OUTOFMEMORY);
		goto Error;
	}

	pobj->AddRef();

	((CUnknownA *)pUnk)->AddInterface(idRef, *ppobjA);

Done:
	pData->aAnsiWrapperCache[idRef] = *ppobjA;

#ifdef _DEBUG
	{
		char buf[256];
		wsprintf(buf, "\t%d Wrappee %x, Wrapper %x", idRef, pobj, *ppobjA);
		TraceNotify(buf);
	}
#endif

Return:
	hResult = ResultFromScode(S_OK);

Error:
	LeaveCriticalSection(&CriticalSection);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    InterfaceWFromA
//
//  Synopsis:   Wrap an ANSI object with an Unicode thunking object.
//
//  Returns:    Address of the Unicode thunking object.
//
//---------------------------------------------------------------------------
HRESULT WrapInterfaceWFromA(IDINTERFACE idRef, LPUNKNOWN pobjA, LPUNKNOWN * ppobj)
{
	PWRAPPER  pWrapper;
	LPUNKNOWN pUnk = NULL;
	HRESULT   hResult;


	TraceWrapper("WrapInterfaceWFromA");

#ifdef _DEBUG
	{
		char buf[256];
		wsprintf(buf, "\tid %d, Wide %x", idRef, pobjA);
		TraceNotify(buf);
	}
#endif

	//
	//
	//
	if (idRef == ID_NULL || pobjA == NULL)
		{
				if (pobjA != NULL)
						pobjA->AddRef();
		*ppobj = pobjA;
		return ResultFromScode(S_OK);
	}

	EnterCriticalSection(&CriticalSection);
	WRAP_DATA* pData = GetWrapData();

	//
	//
	//
	*ppobj = NULL;

	//
	//  If the ANSI Interface is contained in the cache, then it is a wrapper.
	//  Given a known wrapper, the wrappee can be received directly without
	//  the overhead of scanning the wrapper table.
	//
	if (pobjA == pData->aAnsiWrapperCache[idRef])
	{
		*ppobj = ((CUnknownA *)pobjA)->GetWrappee();
		TraceAddRef("Cache hit", *ppobj, (*ppobj)->AddRef());
		goto Return;
	}

	//
	//  Determine if the Ansi interface that needs to be wrapped is
	//  actually a wrapper of an Unicode interface.  If so, just grab the
	//  Unicode interface and return it.
	//
	if (idRef != ID_IUnknown)
	{
		pWrapper = pData->pAnsiWrappers;
		while (pWrapper)
		{
			if (pWrapper->Wrapper == pobjA)
			{
				if (pWrapper->idRef == idRef)
				{
					*ppobj = pWrapper->Wrappee;
					TraceAddRef("Unwrap native from wrapper", *ppobj, (*ppobj)->AddRef());
					pData->aAnsiWrapperCache[idRef] = pobjA;
					goto Return;
				}
			}

			if (pWrapper->Wrappee == pobjA)
				TraceFatalError("Internal wrapper mismatch 3");

			pWrapper = pWrapper->pNext;
		}
	}

	//
	//  Search the Unicode table to determine if this interface has already been
	//  wrapped.  If is has, AddRef the current wrapper and return it.
	//
	pWrapper = pData->pWideWrappers;
	while (pWrapper)
	{
		if (pWrapper->Wrappee == pobjA)
		{
			*ppobj = pWrapper->Wrapper;
			if (pWrapper->idRef == idRef)
			{
				TraceAddRef("Ansi interface already wrapped", *ppobj, (*ppobj)->AddRef());
				goto Done;
			}
		}

		if (pWrapper->Wrapper == pobjA)
			TraceFatalError("Internal wrapper mismatch 4");

		pWrapper = pWrapper->pNext;
	}

	//
	//  The Unicode interface is already wrapped by another interface, so just
	//  wrap it and merge into the current wrapper object.
	//
	if (*ppobj != NULL)
	{
		(*ppobj)->QueryInterface(IID_IUnknown, (LPVOID *)&pUnk);
	}

	//
	//  If no wrapper Unknown yet, then create one now.
	//
	if (pUnk == NULL)
	{
		LPUNKNOWN pUnkA;

		hResult = pobjA->QueryInterface(IID_IUnknown, (LPVOID *)&pUnkA);
		if (FAILED(hResult))
			goto Error;

		pWrapper = pData->pWideWrappers;
		while (pWrapper)
		{
			if ((pWrapper->Wrappee == pUnkA) && pWrapper->idRef == ID_IUnknown)
			{
				pUnk = pWrapper->Wrapper;
				pUnk->AddRef();
				break;
			}

			pWrapper = pWrapper->pNext;
		}

		if (pUnk == NULL)
		{
			pUnk = new CUnknownW(pUnkA, idRef);
			if (pUnk == NULL)
			{
				hResult = ResultFromScode(E_OUTOFMEMORY);
				goto Error;
			}

			WrapAddWideWrapper(ID_IUnknown, pUnkA, pUnk);
		}

		pUnkA->Release();
	}

	//
	//  If all we wanted was an Unknown then we're done.
	//
	if (idRef == ID_IUnknown)
	{
		*ppobj = pUnk;
		goto Done;
	}

	*ppobj = WrapAnyWFromA(pUnk, idRef, pobjA);
	if (*ppobj == NULL)
		hResult = ResultFromScode(E_OUTOFMEMORY);

	pobjA->AddRef();

	((CUnknownW *)pUnk)->AddInterface(idRef, *ppobj);

Done:
	pData->aWideWrapperCache[idRef] = *ppobj;

#ifdef _DEBUG
	{
		char buf[256];
		wsprintf(buf, "\t%d Wrappee %x, Wrapper %x", idRef, pobjA, *ppobj);
		TraceNotify(buf);
	}
#endif

Return:
	hResult = ResultFromScode(S_OK);

Error:
	LeaveCriticalSection(&CriticalSection);

	return hResult;
}


HRESULT WrapAddAnsiWrapper(IDINTERFACE idRef, LPUNKNOWN pobj, LPUNKNOWN pobjA)
{
	PWRAPPER pWrapper;


	pWrapper = new WRAPPER;
	if (pWrapper == NULL)
		return ResultFromScode(E_OUTOFMEMORY);

	EnterCriticalSection(&CriticalSection);
	WRAP_DATA* pData = GetWrapData();

	pWrapper->idRef   = idRef;
	pWrapper->Wrappee = pobj;
	pWrapper->Wrapper = pobjA;

	pWrapper->pNext = pData->pAnsiWrappers;
	pData->pAnsiWrappers = pWrapper;

#ifdef _DEBUG
	{
		char buf[256];
		wsprintf(buf, "WrapAddAnsiWrapper %d Wrappee %x, Wrapper %x", idRef, pobj, pobjA);
		TraceNotify(buf);
	}
#endif

	LeaveCriticalSection(&CriticalSection);

	return ResultFromScode(S_OK);
}


VOID WrapDeleteWrapper(LPUNKNOWN pObj)
{
	PWRAPPER pCurr;
	PWRAPPER pPrev;

	EnterCriticalSection(&CriticalSection);
	WRAP_DATA* pData = GetWrapData();

	if (((CInterface *)pObj)->IsANSI())
	{
		pCurr = pData->pAnsiWrappers;
		if (pCurr)
		{
			if (pCurr->Wrapper == pObj)
			{
				pData->pAnsiWrappers = pCurr->pNext;
			}
			else
			{
				pPrev = pCurr;
				pCurr = pPrev->pNext;
				while (pCurr)
				{
					if (pCurr->Wrapper == pObj)
					{
						pPrev->pNext = pCurr->pNext;
						break;
					}

					pPrev = pCurr;
					pCurr = pCurr->pNext;
				}
			}

			if (pCurr && pData->aAnsiWrapperCache[pCurr->idRef] == pCurr->Wrapper)
				pData->aAnsiWrapperCache[pCurr->idRef] = NULL;
		}
	}
	else
	{
		pCurr = pData->pWideWrappers;
		if (pCurr)
		{
			if (pCurr->Wrapper == pObj)
			{
				pData->pWideWrappers = pCurr->pNext;
			}
			else
			{
				pPrev = pCurr;
				pCurr = pPrev->pNext;
				while (pCurr)
				{
					if (pCurr->Wrapper == pObj)
					{
						pPrev->pNext = pCurr->pNext;
						break;
					}

					pPrev = pCurr;
					pCurr = pCurr->pNext;
				}
			}

			if (pCurr && pData->aWideWrapperCache[pCurr->idRef] == pCurr->Wrapper)
				pData->aWideWrapperCache[pCurr->idRef] = NULL;
		}
	}

#ifdef _DEBUG
	if (pCurr == 0)
		DebugBreak();
#endif

	delete pCurr;
	delete pObj;

	LeaveCriticalSection(&CriticalSection);
}



HRESULT WrapAddWideWrapper(IDINTERFACE idRef, LPUNKNOWN pobj, LPUNKNOWN pobjA)
{
	PWRAPPER pWrapper;


	pWrapper = new WRAPPER;
	if (pWrapper == NULL)
		return ResultFromScode(E_OUTOFMEMORY);

	EnterCriticalSection(&CriticalSection);
	WRAP_DATA* pData = GetWrapData();

	pWrapper->idRef   = idRef;
	pWrapper->Wrappee = pobj;
	pWrapper->Wrapper = pobjA;

	pWrapper->pNext = pData->pWideWrappers;
	pData->pWideWrappers = pWrapper;

#ifdef _DEBUG
	{
		char buf[256];
		wsprintf(buf, "WrapAddAnsiWrapper %d Wrappee %x, Wrapper %x", idRef, pobj, pobjA);
		TraceNotify(buf);
	}
#endif

	LeaveCriticalSection(&CriticalSection);

	return ResultFromScode(S_OK);
}

#pragma code_seg(".text$initseg")

DWORD WINAPI _AfxTlsAlloc()
{
	DWORD dwResult = TlsAlloc();
	DWORD dwVersion = GetVersion();
	if ((dwVersion & 0x80000000) && (BYTE)dwVersion <= 3)
	{
		while (dwResult >= 0 && dwResult <= 2)
			dwResult = TlsAlloc();
	}
	return dwResult;
}

BOOL WrapInit(VOID)
{
	//
	//  Initialize the global resources.
	//
	if (!GlobalInit())
		return FALSE;

	if (nInitCount == 0)
	{
		// allocate thread local storage index for Win32s
		DWORD dwVersion = GetVersion();
		if ((dwVersion & 0x80000000) && (BYTE)dwVersion < 4)
		{
			tlsIndex = _AfxTlsAlloc();
			if (tlsIndex == (DWORD)-1)
				return FALSE;
		}
		InitializeCriticalSection(&CriticalSection);
	}

	if (tlsIndex != (DWORD)-1)
	{
		void* pData = LocalAlloc(LPTR, sizeof(WRAP_DATA));
		if (pData == NULL)
		{
			WrapCleanup();
			return FALSE;
		}
		TlsSetValue(tlsIndex, pData);
	}
	++nInitCount;

	return TRUE;
}

#pragma code_seg(".orpc")

void WrapCleanup(VOID)
{
#ifdef _DEBUG
	PWRAPPER pWrapper;
	char buf[MAX_STRING];

	WRAP_DATA* pData = GetWrapData();

	pWrapper = pData->pAnsiWrappers;
	while (pWrapper)
	{
		wsprintf(buf, "Ansi Wrapper not deleted %x, Id %d, Wrappee %x, Wrapper %x",
				pWrapper, pWrapper->idRef, pWrapper->Wrappee, pWrapper->Wrapper);
		TraceWarning(buf);
		pWrapper = pWrapper->pNext;
	}

	pWrapper = pData->pWideWrappers;
	while (pWrapper)
	{
		wsprintf(buf, "Wide Wrapper not deleted %x, Id %d, Wrappee %x, Wrapper %x",
				pWrapper, pWrapper->idRef, pWrapper->Wrappee, pWrapper->Wrapper);
		TraceWarning(buf);
		pWrapper = pWrapper->pNext;
	}
#endif

	// cleanup thread local storage
	if (tlsIndex != (DWORD)-1)
	{
		void* pData = TlsGetValue(tlsIndex);
		TlsSetValue(tlsIndex, NULL);
		if (pData != NULL)
			LocalFree(pData);
	}

	// last chance cleanup
	if (nInitCount == 1)
	{
		// cleanup thread local storage
		if (tlsIndex != (DWORD)-1)
		{
			TlsFree(tlsIndex);
			tlsIndex = (DWORD)-1;
		}
		DeleteCriticalSection(&CriticalSection);
	}
	--nInitCount;
}

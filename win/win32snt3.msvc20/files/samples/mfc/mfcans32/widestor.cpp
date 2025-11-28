//+--------------------------------------------------------------------------
//
//  Copyright (C) 1994, Microsoft Corporation.  All Rights Reserved.
//
//  File:       ansistor.cpp
//
//  Contents:   ANSI Wrappers for Unicode Storage Interfaces and APIs.
//
//  Classes:    CEnumSTATSTGW - Unicode wrapper object for IEnumSTATSTG.
//              CLockBytesW   - Unicode wrapper object for ILockBytes.
//              CStreamW      - Unicode wrapper object for IStream.
//              CStorageW     - Unicode wrapper object for IStorage.
//              CRootStorageW - Unicode wrapper object for IRootStorage.
//
//  History:    01-Nov-93   v-kentc     Created.
//
//---------------------------------------------------------------------------

#include "Ole2Ansi.h"



//***************************************************************************
//
//                   IEnumSTATSTGW Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CEnumSTATSTGW::Next, public
//
//  Synopsis:   Thunks Next to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumSTATSTGW::Next(
		ULONG celt,
		STATSTG * rgelt,
		ULONG * pceltFetched)
{
	TraceMethodEnter("CEnumSTATSTGW::Next", this);

	ULONG   celtFetched;
	HRESULT hReturn;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IEnumSTATSTGA, Next));

	if (pceltFetched == NULL)
		pceltFetched = &celtFetched;

	hReturn = GetANSI()->Next(celt, (STATSTGA *)rgelt, pceltFetched);
	if (FAILED(hReturn))
		return (hReturn);

	hResult = ConvertSTATSTGArrayToW(rgelt, *pceltFetched);
	if (FAILED(hResult))
		return (hResult);

	return hReturn;
}


//+--------------------------------------------------------------------------
//
//  Member:     CEnumSTATSTGW::Skip, public
//
//  Synopsis:   Thunks Skip to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumSTATSTGW::Skip(ULONG celt)
{
	_DebugHook(GetANSI(), MEMBER_PTR(IEnumSTATSTGA, Skip));

	return GetANSI()->Skip(celt);
}


//+--------------------------------------------------------------------------
//
//  Member:     CEnumSTATSTGW::Reset, public
//
//  Synopsis:   Thunks Reset to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumSTATSTGW::Reset(VOID)
{
	_DebugHook(GetANSI(), MEMBER_PTR(IEnumSTATSTGA, Reset));

	return GetANSI()->Reset();
}


//+--------------------------------------------------------------------------
//
//  Member:     CEnumSTATSTGW::Clone, public
//
//  Synopsis:   Thunks Clone to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumSTATSTGW::Clone(IEnumSTATSTG * * ppstat)
{
	TraceMethodEnter("CEnumSTATSTGW::Clone", this);

	IEnumSTATSTGA * pstatA;


	_DebugHook(GetANSI(), MEMBER_PTR(IEnumSTATSTGA, Clone));

	*ppstat = NULL;

	HRESULT hResult = GetANSI()->Clone(&pstatA);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapIEnumSTATSTGWFromA(pstatA, ppstat);

	if (pstatA)
		pstatA->Release();

	return hResult;
}


//***************************************************************************
//
//                   ILockBytesW Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CLockBytesW::ReadAt, public
//
//  Synopsis:   Thunks ReadAt to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CLockBytesW::ReadAt(
		ULARGE_INTEGER ulOffset,
		VOID HUGEP *pv,
		ULONG cb,
		ULONG *pcbRead)
{
	_DebugHook(GetANSI(), MEMBER_PTR(ILockBytesA, ReadAt));

	return GetANSI()->ReadAt(ulOffset, pv, cb, pcbRead);
}


//+--------------------------------------------------------------------------
//
//  Member:     CLockBytesW::WriteAt, public
//
//  Synopsis:   Thunks WriteAt to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CLockBytesW::WriteAt(
		ULARGE_INTEGER ulOffset,
		VOID const HUGEP *pv,
		ULONG cb,
		ULONG *pcbWritten)
{
	_DebugHook(GetANSI(), MEMBER_PTR(ILockBytesA, WriteAt));

	return GetANSI()->WriteAt(ulOffset, pv, cb, pcbWritten);
}


//+--------------------------------------------------------------------------
//
//  Member:     CLockBytesW::Flush, public
//
//  Synopsis:   Thunks Flush to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CLockBytesW::Flush(VOID)
{
	_DebugHook(GetANSI(), MEMBER_PTR(ILockBytesA, Flush));

	return GetANSI()->Flush();
}


//+--------------------------------------------------------------------------
//
//  Member:     CLockBytesW::SetSize, public
//
//  Synopsis:   Thunks SetSize to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CLockBytesW::SetSize(ULARGE_INTEGER cb)
{
	_DebugHook(GetANSI(), MEMBER_PTR(ILockBytesA, SetSize));

	return GetANSI()->SetSize(cb);
}


//+--------------------------------------------------------------------------
//
//  Member:     CLockBytesW::LockRegion, public
//
//  Synopsis:   Thunks LockRegion to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CLockBytesW::LockRegion(
		ULARGE_INTEGER libOffset,
		ULARGE_INTEGER cb,
		DWORD dwLockType)
{
	_DebugHook(GetANSI(), MEMBER_PTR(ILockBytesA, LockRegion));

	return GetANSI()->LockRegion(libOffset, cb, dwLockType);
}


//+--------------------------------------------------------------------------
//
//  Member:     CLockBytesW::UnlockRegion, public
//
//  Synopsis:   Thunks UnlockRegion to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CLockBytesW::UnlockRegion(
		ULARGE_INTEGER libOffset,
		ULARGE_INTEGER cb,
		DWORD dwLockType)
{
	_DebugHook(GetANSI(), MEMBER_PTR(ILockBytesA, UnlockRegion));

	return GetANSI()->UnlockRegion(libOffset, cb, dwLockType);
}


//+--------------------------------------------------------------------------
//
//  Member:     CLockBytesW::Stat, public
//
//  Synopsis:   Thunks Stat to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CLockBytesW::Stat(STATSTG *pstatstg, DWORD grfStatFlag)
{
	TraceMethodEnter("CLockBytesW::Stat", this);

	_DebugHook(GetANSI(), MEMBER_PTR(ILockBytesA, Stat));

	HRESULT hResult = GetANSI()->Stat((STATSTGA *)pstatstg, grfStatFlag);
	if (FAILED(hResult))
		return (hResult);

	return ConvertSTATSTGToW(pstatstg);
}



//***************************************************************************
//
//                   IStreamW Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CStreamW::Read, public
//
//  Synopsis:   Thunks Read to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStreamW::Read(
		VOID HUGEP *pv,
		ULONG cb,
		ULONG *pcbRead)
{
	_DebugHook(GetANSI(), MEMBER_PTR(IStreamA, Read));

	return GetANSI()->Read(pv, cb, pcbRead);
}


//+--------------------------------------------------------------------------
//
//  Member:     CStreamW::Write, public
//
//  Synopsis:   Thunks Write to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStreamW::Write(
		VOID const HUGEP *pv,
		ULONG cb,
		ULONG *pcbWritten)
{
	_DebugHook(GetANSI(), MEMBER_PTR(IStreamA, Write));

	return GetANSI()->Write(pv, cb, pcbWritten);
}


//+--------------------------------------------------------------------------
//
//  Member:     CStreamW::Seek, public
//
//  Synopsis:   Thunks Seek to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStreamW::Seek(
		LARGE_INTEGER dlibMove,
		DWORD dwOrigin,
		ULARGE_INTEGER *plibNewPosition)
{
	_DebugHook(GetANSI(), MEMBER_PTR(IStreamA, Seek));

	return GetANSI()->Seek(dlibMove, dwOrigin, plibNewPosition);
}


//+--------------------------------------------------------------------------
//
//  Member:     CStreamW::SetSize, public
//
//  Synopsis:   Thunks SetSize to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStreamW::SetSize(ULARGE_INTEGER libNewSize)
{
	_DebugHook(GetANSI(), MEMBER_PTR(IStreamA, SetSize));

	return GetANSI()->SetSize(libNewSize);
}


//+--------------------------------------------------------------------------
//
//  Member:     CStreamW::CopyTo, public
//
//  Synopsis:   Thunks CopyTo to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStreamW::CopyTo(
		IStream *pstrm,
		ULARGE_INTEGER cb,
		ULARGE_INTEGER *pcbRead,
		ULARGE_INTEGER *pcbWritten)
{
	TraceMethodEnter("CStreamW::CopyTo", this);

	IStreamA * pstrmA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IStreamA, CopyTo));

	hResult = WrapIStreamAFromW(pstrm, &pstrmA);
	if (FAILED(hResult))
		return (hResult);

	hResult = GetANSI()->CopyTo(pstrmA, cb, pcbRead, pcbWritten);

	if (pstrmA)
		pstrmA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CStreamW::Commit, public
//
//  Synopsis:   Thunks Commit to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStreamW::Commit(DWORD grfCommitFlags)
{
	_DebugHook(GetANSI(), MEMBER_PTR(IStreamA, Commit));

	return GetANSI()->Commit(grfCommitFlags);
}


//+--------------------------------------------------------------------------
//
//  Member:     CStreamW::Revert, public
//
//  Synopsis:   Thunks Revert to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStreamW::Revert(VOID)
{
	_DebugHook(GetANSI(), MEMBER_PTR(IStreamA, Revert));

	return GetANSI()->Revert();
}


//+--------------------------------------------------------------------------
//
//  Member:     CStreamW::LockRegion, public
//
//  Synopsis:   Thunks LockRegion to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStreamW::LockRegion(
		ULARGE_INTEGER libOffset,
		ULARGE_INTEGER cb,
		DWORD dwLockType)
{
	_DebugHook(GetANSI(), MEMBER_PTR(IStreamA, LockRegion));

	return GetANSI()->LockRegion(libOffset, cb, dwLockType);
}


//+--------------------------------------------------------------------------
//
//  Member:     CStreamW::UnlockRegion, public
//
//  Synopsis:   Thunks UnlockRegion to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStreamW::UnlockRegion(
		ULARGE_INTEGER libOffset,
		ULARGE_INTEGER cb,
		DWORD dwLockType)
{
	_DebugHook(GetANSI(), MEMBER_PTR(IStreamA, UnlockRegion));

	return GetANSI()->UnlockRegion(libOffset, cb, dwLockType);
}


//+--------------------------------------------------------------------------
//
//  Member:     CStreamW::Stat, public
//
//  Synopsis:   Thunks Stat to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStreamW::Stat(STATSTG *pstatstg, DWORD grfStatFlag)
{
	TraceMethodEnter("CStreamW::Stat", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IStreamA, Stat));

	HRESULT hResult = GetANSI()->Stat((STATSTGA *)pstatstg, grfStatFlag);
	if (FAILED(hResult))
		return (hResult);

	return ConvertSTATSTGToW(pstatstg);
}


//+--------------------------------------------------------------------------
//
//  Member:     CStreamW::Clone, public
//
//  Synopsis:   Thunks Clone to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStreamW::Clone(IStream * * ppstrm)
{
	TraceMethodEnter("CStreamW::Clone", this);

	IStreamA * pstrmA;


	_DebugHook(GetANSI(), MEMBER_PTR(IStreamA, Clone));

	*ppstrm = NULL;

	HRESULT hResult = GetANSI()->Clone(&pstrmA);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapIStreamWFromA(pstrmA, ppstrm);

	if (pstrmA)
		pstrmA->Release();

	return hResult;
}



//***************************************************************************
//
//                   IStorageW Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CStorageW::CreateStream, public
//
//  Synopsis:   Thunks CreateStream to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStorageW::CreateStream(
		LPCOLESTR pstrName,
		DWORD grfMode,
		DWORD reserved1,
		DWORD reserved2,
		IStream **ppstrm)
{
	TraceMethodEnter("CStorageW::CreateStream", this);

	CHAR strNameA[MAX_PATH];
	IStreamA * pstrmA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IStorageA, CreateStream));

	*ppstrm = NULL;

	ConvertStringToA(pstrName, strNameA);

	hResult = GetANSI()->CreateStream(strNameA, grfMode, reserved1, reserved2, &pstrmA);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapIStreamWFromA(pstrmA, ppstrm);

	if (pstrmA)
		pstrmA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CStorageW::OpenStream, public
//
//  Synopsis:   Thunks OpenStream to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStorageW::OpenStream(
		LPCOLESTR pstrName,
		void *reserved1,
		DWORD grfMode,
		DWORD reserved2,
		IStream **ppstrm)
{
	TraceMethodEnter("CStorageW::OpenStream", this);

	CHAR strNameA[MAX_PATH];
	IStreamA * pstrmA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IStorageA, OpenStream));

	*ppstrm = NULL;

	ConvertStringToA(pstrName, strNameA);

	hResult = GetANSI()->OpenStream(strNameA, reserved1, grfMode, reserved2, &pstrmA);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapIStreamWFromA(pstrmA, ppstrm);

	if (pstrmA)
		pstrmA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CStorageW::CreateStorage, public
//
//  Synopsis:   Thunks CreateStorage to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStorageW::CreateStorage(
		LPCOLESTR pstrName,
		DWORD grfMode,
		DWORD reserved1,
		DWORD reserved2,
		IStorage **ppstg)
{
	TraceMethodEnter("CStorageW::CreateStorage", this);

	CHAR strNameA[MAX_PATH];
	IStorageA * pstgA;
	HRESULT hResult;
	HRESULT hReturn;


	_DebugHook(GetANSI(), MEMBER_PTR(IStorageA, CreateStorage));

	*ppstg = NULL;

	ConvertStringToA(pstrName, strNameA);

	hReturn = GetANSI()->CreateStorage(strNameA, grfMode, reserved1, reserved2, &pstgA);
	if (FAILED(hReturn))
		return (hReturn);

	hResult = WrapIStorageWFromA(pstgA, ppstg);
	if (SUCCEEDED(hResult))
		hResult = hReturn;

	if (pstgA)
		pstgA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CStorageW::OpenStorage, public
//
//  Synopsis:   Thunks OpenStorage to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStorageW::OpenStorage(
		LPCOLESTR pstrName,
		IStorage *pstgPriority,
		DWORD grfMode,
		SNB snbExclude,
		DWORD reserved,
		IStorage **ppstg)
{
	TraceMethodEnter("CStorageW::OpenStorage", this);

	CHAR strNameA[MAX_PATH];
	SNBA  snbExcludeA;
	IStorageA * pstgPriorityA;
	IStorageA * pstgA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IStorageA, OpenStorage));

	*ppstg = NULL;

	ConvertStringToA(pstrName, strNameA);

	hResult = ConvertSNBToA(snbExclude, &snbExcludeA);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIStorageAFromW(pstgPriority, &pstgPriorityA);
	if (FAILED(hResult))
		goto Error;

	hResult = GetANSI()->OpenStorage(strNameA, pstgPriorityA, grfMode, snbExcludeA, reserved, &pstgA);
	if (FAILED(hResult))
		goto Error1;

	hResult = WrapIStorageWFromA(pstgA, ppstg);

	if (pstgA)
		pstgA->Release();

Error1:
	if (pstgPriorityA)
		pstgPriorityA->Release();
Error:
	ConvertSNBFree(snbExcludeA);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CStorageW::CopyTo, public
//
//  Synopsis:   Thunks CopyTo to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStorageW::CopyTo(
		DWORD ciidExclude,
		IID const *rgiidExclude,
		SNB snbExclude,
		IStorage *pstgDest)
{
	TraceMethodEnter("CStorageW::CopyTo", this);

	SNBA snbExcludeA;
	IStorageA * pstgDestA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IStorageA, CopyTo));

	hResult = ConvertSNBToA(snbExclude, &snbExcludeA);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapIStorageAFromW(pstgDest, &pstgDestA);
	if (FAILED(hResult))
		goto Error;

	hResult = GetANSI()->CopyTo(ciidExclude, rgiidExclude, snbExcludeA, pstgDestA);

	if (pstgDestA)
		pstgDestA->Release();

Error:
	ConvertSNBFree(snbExcludeA);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CStorageW::MoveElementTo, public
//
//  Synopsis:   Thunks MoveElementTo to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStorageW::MoveElementTo(
		LPCOLESTR pstrName,
		IStorage *pstgDest,
		LPCOLESTR pstrNewName,
		DWORD grfFlags)
{
	TraceMethodEnter("CStorageW::MoveElementTo", this);

	CHAR strNameA[MAX_PATH];
	CHAR strNewNameA[MAX_PATH];
	IStorageA * pstgDestA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IStorageA, MoveElementTo));

	ConvertStringToA(pstrName, strNameA);

	ConvertStringToA(pstrNewName, strNewNameA);

	hResult = WrapIStorageAFromW(pstgDest, &pstgDestA);
	if (FAILED(hResult))
		return hResult;

	hResult = GetANSI()->MoveElementTo(strNameA, pstgDestA, strNewNameA, grfFlags);

	if (pstgDestA)
		pstgDestA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CStorageW::Commit, public
//
//  Synopsis:   Thunks Commit to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStorageW::Commit(DWORD grfCommitFlags)
{
	_DebugHook(GetANSI(), MEMBER_PTR(IStorageA, Commit));

	return GetANSI()->Commit(grfCommitFlags);
}


//+--------------------------------------------------------------------------
//
//  Member:     CStorageW::Revert, public
//
//  Synopsis:   Thunks Revert to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStorageW::Revert(VOID)
{
	_DebugHook(GetANSI(), MEMBER_PTR(IStorageA, Revert));

	return GetANSI()->Revert();
}


//+--------------------------------------------------------------------------
//
//  Member:     CStorageW::EnumElements, public
//
//  Synopsis:   Thunks EnumElements to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStorageW::EnumElements(
		DWORD reserved1,
		void *reserved2,
		DWORD reserved3,
		IEnumSTATSTG **ppenm)
{
	TraceMethodEnter("CStorageW::EnumElements", this);

	IEnumSTATSTGA * penmA = NULL;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IStorageA, EnumElements));

	*ppenm = NULL;

	hResult = GetANSI()->EnumElements(reserved1, reserved2, reserved3, &penmA);
	if (FAILED(hResult))
		return (hResult);

	if (penmA)
	{
		hResult = WrapIEnumSTATSTGWFromA(penmA, ppenm);
		penmA->Release();
	}
	else
		*ppenm = NULL;

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CStorageW::DestroyElement, public
//
//  Synopsis:   Thunks DestroyElement to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStorageW::DestroyElement(LPCOLESTR pstrName)
{
	TraceMethodEnter("CStorageW::DestroyElement", this);

	CHAR strNameA[MAX_PATH];


	_DebugHook(GetANSI(), MEMBER_PTR(IStorageA, DestroyElement));

	ConvertStringToA(pstrName, strNameA);

	return GetANSI()->DestroyElement(strNameA);
}


//+--------------------------------------------------------------------------
//
//  Member:     CStorageW::RenameElement, public
//
//  Synopsis:   Thunks RenameElement to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStorageW::RenameElement(
		LPCOLESTR pstrOldName,
		LPCOLESTR pstrNewName)
{
	TraceMethodEnter("CStorageW::RenameElement", this);

	CHAR strOldNameA[MAX_PATH];
	CHAR strNewNameA[MAX_PATH];


	_DebugHook(GetANSI(), MEMBER_PTR(IStorageA, RenameElement));

	ConvertStringToA(pstrOldName, strOldNameA);

	ConvertStringToA(pstrNewName, strNewNameA);

	return GetANSI()->RenameElement(strOldNameA, strNewNameA);
}


//+--------------------------------------------------------------------------
//
//  Member:     CStorageW::SetElementTimes, public
//
//  Synopsis:   Thunks SetElementTimes to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStorageW::SetElementTimes(
		LPCOLESTR pstrName,
		FILETIME const *pctime,
		FILETIME const *patime,
		FILETIME const *pmtime)
{
	TraceMethodEnter("CStorageW::SetElementTimes", this);

	CHAR strNameA[MAX_PATH];


	_DebugHook(GetANSI(), MEMBER_PTR(IStorageA, SetElementTimes));

	ConvertStringToA(pstrName, strNameA);

	return GetANSI()->SetElementTimes(strNameA, pctime, patime, pmtime);
}


//+--------------------------------------------------------------------------
//
//  Member:     CStorageW::SetClass, public
//
//  Synopsis:   Thunks SetClass to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStorageW::SetClass(REFCLSID clsid)
{
	_DebugHook(GetANSI(), MEMBER_PTR(IStorageA, SetClass));

	return GetANSI()->SetClass(clsid);
}


//+--------------------------------------------------------------------------
//
//  Member:     CStorageW::SetStateBits, public
//
//  Synopsis:   Thunks SetStateBits to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStorageW::SetStateBits(DWORD grfCommitFlags, DWORD grfMask)
{
	_DebugHook(GetANSI(), MEMBER_PTR(IStorageA, SetStateBits));

	return GetANSI()->SetStateBits(grfCommitFlags, grfMask);
}


//+--------------------------------------------------------------------------
//
//  Member:     CStorageW::Stat, public
//
//  Synopsis:   Thunks Stat to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStorageW::Stat(STATSTG *pstatstg, DWORD grfStatFlag)
{
	TraceMethodEnter("CStorageW::Stat", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IStorageA, Stat));

	HRESULT hResult = GetANSI()->Stat((STATSTGA *)pstatstg, grfStatFlag);
	if (FAILED(hResult))
		return (hResult);

	return ConvertSTATSTGToW(pstatstg);
}


//***************************************************************************
//
//                   IRootStorageW Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CRootStorageW::SwitchToFile, public
//
//  Synopsis:   Thunks SwitchToFile to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CRootStorageW::SwitchToFile(LPOLESTR pstrFile)
{
	TraceMethodEnter("CRootStorageW::SwitchToFile", this);

	CHAR strFileA[MAX_STRING];


	_DebugHook(GetANSI(), MEMBER_PTR(IRootStorageA, SwitchToFile));

	ConvertStringToA(pstrFile, strFileA);

	return GetANSI()->SwitchToFile(strFileA);
}

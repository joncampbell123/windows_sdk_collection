//+--------------------------------------------------------------------------
//
//  Copyright (C) 1994, Microsoft Corporation.  All Rights Reserved.
//
//  File:       ansistor.cpp
//
//  Contents:   ANSI Wrappers for Unicode Storage Interfaces and APIs.
//
//  Classes:    CEnumSTATSTGA - ANSI wrapper object for IEnumSTATSTG.
//              CLockBytesA   - ANSI wrapper object for ILockBytes.
//              CStreamA      - ANSI wrapper object for IStream.
//              CStorageA     - ANSI wrapper object for IStorage.
//              CRootStorageA - ANSI wrapper object for IRootStorage.
//
//  Functions:  StgCreateDocfileA
//              StgCreateDocfileOnILockBytesA
//              StgOpenStorageA
//              StgOpenStorageOnILockBytesA
//              StgIsStorageFileA
//              StgIsStorageILockBytesA
//              StgSetTimesA
//
//  History:    01-Nov-93   v-kentc     Created.
//
//---------------------------------------------------------------------------

#include "Ole2Ansi.h"



//***************************************************************************
//
//                   IEnumSTATSTGA Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CEnumSTATSTGA::Next, public
//
//  Synopsis:   Thunks Next to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumSTATSTGA::Next(
		ULONG celt,
		STATSTGA * rgelt,
		ULONG * pceltFetched)
{
	TraceMethodEnter("CEnumSTATSTGA::Next", this);

	ULONG   celtFetched;
	HRESULT hReturn;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IEnumSTATSTG, Next));

	if (pceltFetched == NULL)
		pceltFetched = &celtFetched;

	hReturn = GetWide()->Next(celt, (STATSTG *)rgelt, pceltFetched);
	if (FAILED(hReturn))
		return (hReturn);

	hResult = ConvertSTATSTGArrayToA(rgelt, *pceltFetched);
	if (FAILED(hResult))
		return (hResult);

	return hReturn;
}


//+--------------------------------------------------------------------------
//
//  Member:     CEnumSTATSTGA::Skip, public
//
//  Synopsis:   Thunks Skip to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumSTATSTGA::Skip(ULONG celt)
{
	_DebugHook(GetWide(), MEMBER_PTR(IEnumSTATSTG, Skip));

	return GetWide()->Skip(celt);
}


//+--------------------------------------------------------------------------
//
//  Member:     CEnumSTATSTGA::Reset, public
//
//  Synopsis:   Thunks Reset to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumSTATSTGA::Reset(VOID)
{
	_DebugHook(GetWide(), MEMBER_PTR(IEnumSTATSTG, Reset));

	return GetWide()->Reset();
}


//+--------------------------------------------------------------------------
//
//  Member:     CEnumSTATSTGA::Clone, public
//
//  Synopsis:   Thunks Clone to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumSTATSTGA::Clone(IEnumSTATSTGA * * ppstatA)
{
	TraceMethodEnter("CEnumSTATSTGA::Clone", this);

	IEnumSTATSTG * pstat;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IEnumSTATSTG, Clone));

	*ppstatA = NULL;

	hResult = GetWide()->Clone(&pstat);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapIEnumSTATSTGAFromW(pstat, ppstatA);

	if (pstat)
		pstat->Release();

	return hResult;
}


//***************************************************************************
//
//                   ILockBytesA Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CLockBytesA::ReadAt, public
//
//  Synopsis:   Thunks ReadAt to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CLockBytesA::ReadAt(
		ULARGE_INTEGER ulOffset,
		VOID HUGEP *pv,
		ULONG cb,
		ULONG *pcbRead)
{
	_DebugHook(GetWide(), MEMBER_PTR(ILockBytes, ReadAt));

	return GetWide()->ReadAt(ulOffset, pv, cb, pcbRead);
}


//+--------------------------------------------------------------------------
//
//  Member:     CLockBytesA::WriteAt, public
//
//  Synopsis:   Thunks WriteAt to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CLockBytesA::WriteAt(
		ULARGE_INTEGER ulOffset,
		VOID const HUGEP *pv,
		ULONG cb,
		ULONG *pcbWritten)
{
	_DebugHook(GetWide(), MEMBER_PTR(ILockBytes, WriteAt));

	return GetWide()->WriteAt(ulOffset, pv, cb, pcbWritten);
}


//+--------------------------------------------------------------------------
//
//  Member:     CLockBytesA::Flush, public
//
//  Synopsis:   Thunks Flush to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CLockBytesA::Flush(VOID)
{
	_DebugHook(GetWide(), MEMBER_PTR(ILockBytes, Flush));

	return GetWide()->Flush();
}


//+--------------------------------------------------------------------------
//
//  Member:     CLockBytesA::SetSize, public
//
//  Synopsis:   Thunks SetSize to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CLockBytesA::SetSize(ULARGE_INTEGER cb)
{
	_DebugHook(GetWide(), MEMBER_PTR(ILockBytes, SetSize));

	return GetWide()->SetSize(cb);
}


//+--------------------------------------------------------------------------
//
//  Member:     CLockBytesA::LockRegion, public
//
//  Synopsis:   Thunks LockRegion to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CLockBytesA::LockRegion(
		ULARGE_INTEGER libOffset,
		ULARGE_INTEGER cb,
		DWORD dwLockType)
{
	_DebugHook(GetWide(), MEMBER_PTR(ILockBytes, LockRegion));

	return GetWide()->LockRegion(libOffset, cb, dwLockType);
}


//+--------------------------------------------------------------------------
//
//  Member:     CLockBytesA::UnlockRegion, public
//
//  Synopsis:   Thunks UnlockRegion to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CLockBytesA::UnlockRegion(
		ULARGE_INTEGER libOffset,
		ULARGE_INTEGER cb,
		DWORD dwLockType)
{
	_DebugHook(GetWide(), MEMBER_PTR(ILockBytes, UnlockRegion));

	return GetWide()->UnlockRegion(libOffset, cb, dwLockType);
}


//+--------------------------------------------------------------------------
//
//  Member:     CLockBytesA::Stat, public
//
//  Synopsis:   Thunks Stat to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CLockBytesA::Stat(STATSTGA *pstatstg, DWORD grfStatFlag)
{
	TraceMethodEnter("CLockBytesA::Stat", this);

	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(ILockBytes, Stat));

	hResult = GetWide()->Stat((STATSTG *)pstatstg, grfStatFlag);
	if (FAILED(hResult))
		return (hResult);

	return ConvertSTATSTGToA(pstatstg);
}



//***************************************************************************
//
//                   IStreamA Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CStreamA::Read, public
//
//  Synopsis:   Thunks Read to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStreamA::Read(
		VOID HUGEP *pv,
		ULONG cb,
		ULONG *pcbRead)
{
	_DebugHook(GetWide(), MEMBER_PTR(IStream, Read));

	return GetWide()->Read(pv, cb, pcbRead);
}


//+--------------------------------------------------------------------------
//
//  Member:     CStreamA::Write, public
//
//  Synopsis:   Thunks Write to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStreamA::Write(
		VOID const HUGEP *pv,
		ULONG cb,
		ULONG *pcbWritten)
{
	_DebugHook(GetWide(), MEMBER_PTR(IStream, Write));

	return GetWide()->Write(pv, cb, pcbWritten);
}


//+--------------------------------------------------------------------------
//
//  Member:     CStreamA::Seek, public
//
//  Synopsis:   Thunks Seek to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStreamA::Seek(
		LARGE_INTEGER dlibMove,
		DWORD dwOrigin,
		ULARGE_INTEGER *plibNewPosition)
{
	_DebugHook(GetWide(), MEMBER_PTR(IStream, Seek));

	return GetWide()->Seek(dlibMove, dwOrigin, plibNewPosition);
}


//+--------------------------------------------------------------------------
//
//  Member:     CStreamA::SetSize, public
//
//  Synopsis:   Thunks SetSize to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStreamA::SetSize(ULARGE_INTEGER libNewSize)
{
	_DebugHook(GetWide(), MEMBER_PTR(IStream, SetSize));

	return GetWide()->SetSize(libNewSize);
}


//+--------------------------------------------------------------------------
//
//  Member:     CStreamA::CopyTo, public
//
//  Synopsis:   Thunks CopyTo to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStreamA::CopyTo(
		IStreamA *pstrmA,
		ULARGE_INTEGER cb,
		ULARGE_INTEGER *pcbRead,
		ULARGE_INTEGER *pcbWritten)
{
	TraceMethodEnter("CStreamA::CopyTo", this);

	IStream * pstrm;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IStream, CopyTo));

	hResult = WrapIStreamWFromA(pstrmA, &pstrm);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->CopyTo(pstrm, cb, pcbRead, pcbWritten);

	if (pstrm)
		pstrm->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CStreamA::Commit, public
//
//  Synopsis:   Thunks Commit to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStreamA::Commit(DWORD grfCommitFlags)
{
	_DebugHook(GetWide(), MEMBER_PTR(IStream, Commit));

	return GetWide()->Commit(grfCommitFlags);
}


//+--------------------------------------------------------------------------
//
//  Member:     CStreamA::Revert, public
//
//  Synopsis:   Thunks Revert to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStreamA::Revert(VOID)
{
	_DebugHook(GetWide(), MEMBER_PTR(IStream, Revert));

	return GetWide()->Revert();
}


//+--------------------------------------------------------------------------
//
//  Member:     CStreamA::LockRegion, public
//
//  Synopsis:   Thunks LockRegion to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStreamA::LockRegion(
		ULARGE_INTEGER libOffset,
		ULARGE_INTEGER cb,
		DWORD dwLockType)
{
	_DebugHook(GetWide(), MEMBER_PTR(IStream, LockRegion));

	return GetWide()->LockRegion(libOffset, cb, dwLockType);
}


//+--------------------------------------------------------------------------
//
//  Member:     CStreamA::UnlockRegion, public
//
//  Synopsis:   Thunks UnlockRegion to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStreamA::UnlockRegion(
		ULARGE_INTEGER libOffset,
		ULARGE_INTEGER cb,
		DWORD dwLockType)
{
	_DebugHook(GetWide(), MEMBER_PTR(IStream, UnlockRegion));

	return GetWide()->UnlockRegion(libOffset, cb, dwLockType);
}


//+--------------------------------------------------------------------------
//
//  Member:     CStreamA::Stat, public
//
//  Synopsis:   Thunks Stat to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStreamA::Stat(STATSTGA *pstatstg, DWORD grfStatFlag)
{
	TraceMethodEnter("CStreamA::Stat", this);

	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IStream, Stat));

	hResult = GetWide()->Stat((STATSTG *)pstatstg, grfStatFlag);
	if (FAILED(hResult))
		return (hResult);

	return ConvertSTATSTGToA(pstatstg);
}


//+--------------------------------------------------------------------------
//
//  Member:     CStreamA::Clone, public
//
//  Synopsis:   Thunks Clone to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStreamA::Clone(IStreamA * * ppstrmA)
{
	TraceMethodEnter("CStreamA::Clone", this);

	IStream * pstrm;


	_DebugHook(GetWide(), MEMBER_PTR(IStream, Clone));

	*ppstrmA = NULL;

	HRESULT hResult = GetWide()->Clone(&pstrm);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapIStreamAFromW(pstrm, ppstrmA);

	if (pstrm)
		pstrm->Release();

	return hResult;
}



//***************************************************************************
//
//                   IStorageA Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CStorageA::CreateStream, public
//
//  Synopsis:   Thunks CreateStream to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStorageA::CreateStream(
		LPCSTR pstrNameA,
		DWORD grfMode,
		DWORD reserved1,
		DWORD reserved2,
		IStreamA **ppstrmA)
{
	TraceMethodEnter("CStorageA::CreateStream", this);

	OLECHAR strName[MAX_PATH];
	IStream * pstrm;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IStorage, CreateStream));

	*ppstrmA = NULL;

	ConvertStringToW(pstrNameA, strName);

	hResult = GetWide()->CreateStream(strName, grfMode, reserved1, reserved2, &pstrm);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapIStreamAFromW(pstrm, ppstrmA);

	if (pstrm)
		pstrm->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CStorageA::OpenStream, public
//
//  Synopsis:   Thunks OpenStream to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStorageA::OpenStream(
		LPCSTR pstrNameA,
		void *reserved1,
		DWORD grfMode,
		DWORD reserved2,
		IStreamA **ppstrmA)
{
	TraceMethodEnter("CStorageA::OpenStream", this);

	OLECHAR strName[MAX_PATH];
	LPSTREAM pstrm;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IStorage, OpenStream));

	*ppstrmA = NULL;

	ConvertStringToW(pstrNameA, strName);

	hResult = GetWide()->OpenStream(strName, reserved1, grfMode, reserved2, &pstrm);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapIStreamAFromW(pstrm, ppstrmA);

	if (pstrm)
		pstrm->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CStorageA::CreateStorage, public
//
//  Synopsis:   Thunks CreateStorage to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStorageA::CreateStorage(
		LPCSTR pstrNameA,
		DWORD grfMode,
		DWORD reserved1,
		DWORD reserved2,
		IStorageA **ppstgA)
{
	TraceMethodEnter("CStorageA::CreateStorage", this);

	OLECHAR strName[MAX_PATH];
	IStorage * pstg;
	HRESULT hResult;
	HRESULT hReturn;


	_DebugHook(GetWide(), MEMBER_PTR(IStorage, CreateStorage));

	*ppstgA = NULL;

	ConvertStringToW(pstrNameA, strName);

	hReturn = GetWide()->CreateStorage(strName, grfMode, reserved1, reserved2, &pstg);
	if (FAILED(hReturn))
		return (hReturn);

	hResult = WrapIStorageAFromW(pstg, ppstgA);
	if (SUCCEEDED(hResult))
		hResult = hReturn;

	if (pstg)
		pstg->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CStorageA::OpenStorage, public
//
//  Synopsis:   Thunks OpenStorage to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStorageA::OpenStorage(
		LPCSTR pstrNameA,
		IStorageA *pstgPriorityA,
		DWORD grfMode,
		SNBA snbExcludeA,
		DWORD reserved,
		IStorageA **ppstgA)
{
	TraceMethodEnter("CStorageA::OpenStorage", this);

	OLECHAR strName[MAX_PATH];
	SNB      snbExclude;
	IStorage * pstgPriority;
	IStorage * pstg;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IStorage, OpenStorage));

	*ppstgA = NULL;

	ConvertStringToW(pstrNameA, strName);

	hResult = ConvertSNBToW(snbExcludeA, &snbExclude);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIStorageWFromA(pstgPriorityA, &pstgPriority);
	if (FAILED(hResult))
		goto Error;

	hResult = GetWide()->OpenStorage(strName, pstgPriority, grfMode, snbExclude, reserved, &pstg);
	if (FAILED(hResult))
		goto Error1;

	hResult = WrapIStorageAFromW(pstg, ppstgA);

	if (pstg)
		pstg->Release();

Error1:
	if (pstgPriority)
		pstgPriority->Release();
Error:
	ConvertSNBFree(snbExclude);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CStorageA::CopyTo, public
//
//  Synopsis:   Thunks CopyTo to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStorageA::CopyTo(
		DWORD ciidExclude,
		IID const *rgiidExclude,
		SNBA snbExcludeA,
		IStorageA *pstgDestA)
{
	TraceMethodEnter("CStorageA::CopyTo", this);

	SNB      snbExclude;
	IStorage * pstgDest;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IStorage, CopyTo));

	hResult = ConvertSNBToW(snbExcludeA, &snbExclude);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapIStorageWFromA(pstgDestA, &pstgDest);
	if (FAILED(hResult))
		goto Error;

	hResult = GetWide()->CopyTo(ciidExclude, rgiidExclude, snbExclude, pstgDest);

	if (pstgDest)
		pstgDest->Release();

Error:
	ConvertSNBFree(snbExclude);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CStorageA::MoveElementTo, public
//
//  Synopsis:   Thunks MoveElementTo to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStorageA::MoveElementTo(
		LPCSTR pstrNameA,
		IStorageA *pstgDestA,
		LPCSTR pstrNewNameA,
		DWORD grfFlags)
{
	TraceMethodEnter("CStorageA::MoveElementTo", this);

	OLECHAR strName[MAX_PATH];
	OLECHAR strNewName[MAX_PATH];
	IStorage * pstgDest;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IStorage, MoveElementTo));

	ConvertStringToW(pstrNameA, strName);

	ConvertStringToW(pstrNewNameA, strNewName);

	hResult = WrapIStorageWFromA(pstgDestA, &pstgDest);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->MoveElementTo(strName, pstgDest, strNewName, grfFlags);

	if (pstgDest)
		pstgDest->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CStorageA::Commit, public
//
//  Synopsis:   Thunks Commit to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStorageA::Commit(DWORD grfCommitFlags)
{
	_DebugHook(GetWide(), MEMBER_PTR(IStorage, Commit));

	return GetWide()->Commit(grfCommitFlags);
}


//+--------------------------------------------------------------------------
//
//  Member:     CStorageA::Revert, public
//
//  Synopsis:   Thunks Revert to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStorageA::Revert(VOID)
{
	_DebugHook(GetWide(), MEMBER_PTR(IStorage, Revert));

	return GetWide()->Revert();
}


//+--------------------------------------------------------------------------
//
//  Member:     CStorageA::EnumElements, public
//
//  Synopsis:   Thunks EnumElements to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStorageA::EnumElements(
		DWORD reserved1,
		void *reserved2,
		DWORD reserved3,
		IEnumSTATSTGA **ppenmA)
{
	TraceMethodEnter("CStorageA::EnumElements", this);

	IEnumSTATSTG * penm = NULL;
	HRESULT hResult;



	_DebugHook(GetWide(), MEMBER_PTR(IStorage, EnumElements));

	hResult = GetWide()->EnumElements(reserved1, reserved2, reserved3, &penm);
	if (FAILED(hResult))
		return (hResult);

	if (penm)
	{
		hResult = WrapIEnumSTATSTGAFromW(penm, ppenmA);
		penm->Release();
	}
	else
		*ppenmA = NULL;

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CStorageA::DestroyElement, public
//
//  Synopsis:   Thunks DestroyElement to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStorageA::DestroyElement(LPCSTR pstrNameA)
{
	TraceMethodEnter("CStorageA::DestroyElement", this);

	OLECHAR strName[MAX_PATH];


	_DebugHook(GetWide(), MEMBER_PTR(IStorage, DestroyElement));

	ConvertStringToW(pstrNameA, strName);

	return GetWide()->DestroyElement(strName);
}


//+--------------------------------------------------------------------------
//
//  Member:     CStorageA::RenameElement, public
//
//  Synopsis:   Thunks RenameElement to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStorageA::RenameElement(
		LPCSTR pstrOldNameA,
		LPCSTR pstrNewNameA)
{
	TraceMethodEnter("CStorageA::RenameElement", this);

	OLECHAR strOldName[MAX_PATH];
	OLECHAR strNewName[MAX_PATH];


	_DebugHook(GetWide(), MEMBER_PTR(IStorage, RenameElement));

	ConvertStringToW(pstrOldNameA, strOldName);

	ConvertStringToW(pstrNewNameA, strNewName);

	return GetWide()->RenameElement(strOldName, strNewName);
}


//+--------------------------------------------------------------------------
//
//  Member:     CStorageA::SetElementTimes, public
//
//  Synopsis:   Thunks SetElementTimes to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStorageA::SetElementTimes(
		LPCSTR pstrNameA,
		FILETIME const *pctime,
		FILETIME const *patime,
		FILETIME const *pmtime)
{
	TraceMethodEnter("CStorageA::SetElementTimes", this);

	OLECHAR strName[MAX_PATH];


	_DebugHook(GetWide(), MEMBER_PTR(IStorage, SetElementTimes));

	ConvertStringToW(pstrNameA, strName);

	return GetWide()->SetElementTimes(strName, pctime, patime, pmtime);
}


//+--------------------------------------------------------------------------
//
//  Member:     CStorageA::SetClass, public
//
//  Synopsis:   Thunks SetClass to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStorageA::SetClass(REFCLSID clsid)
{
	_DebugHook(GetWide(), MEMBER_PTR(IStorage, SetClass));

	return GetWide()->SetClass(clsid);
}


//+--------------------------------------------------------------------------
//
//  Member:     CStorageA::SetStateBits, public
//
//  Synopsis:   Thunks SetStateBits to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStorageA::SetStateBits(DWORD grfCommitFlags, DWORD grfMask)
{
	_DebugHook(GetWide(), MEMBER_PTR(IStorage, SetStateBits));

	return GetWide()->SetStateBits(grfCommitFlags, grfMask);
}


//+--------------------------------------------------------------------------
//
//  Member:     CStorageA::Stat, public
//
//  Synopsis:   Thunks Stat to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStorageA::Stat(STATSTGA *pstatstg, DWORD grfStatFlag)
{
	TraceMethodEnter("CStorageA::Stat", this);

	_DebugHook(GetWide(), MEMBER_PTR(IStorage, Stat));

	HRESULT hResult = GetWide()->Stat((STATSTG *)pstatstg, grfStatFlag);
	if (FAILED(hResult))
		return (hResult);

	return ConvertSTATSTGToA(pstatstg);
}


//***************************************************************************
//
//                   IRootStorageA Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CRootStorageA::SwitchToFile, public
//
//  Synopsis:   Thunks SwitchToFile to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CRootStorageA::SwitchToFile(LPSTR pstrFileA)
{
	TraceMethodEnter("CRootStorageA::SwitchToFile", this);

	OLECHAR strFile[MAX_PATH];


	_DebugHook(GetWide(), MEMBER_PTR(IRootStorage, SwitchToFile));

	ConvertStringToW(pstrFileA, strFile);

	return GetWide()->SwitchToFile(strFile);
}



//***************************************************************************
//
//                          Storage API Thunks.
//
//***************************************************************************


//+--------------------------------------------------------------------------
//
//  Routine:    StgCreateDocfileA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI StgCreateDocfileA(LPCSTR pcsNameA,
			DWORD grfMode,
			DWORD reserved,
			IStorageA * *ppstgOpenA)
{
	TraceSTDAPIEnter("StgCreateDocfileA");

	OLECHAR  csName[MAX_PATH];
	LPOLESTR pcsName;
	IStorage * pstg;
	HRESULT  hResult;
	HRESULT  hReturn;


	*ppstgOpenA = NULL;

	if (pcsNameA)
	{
		ConvertStringToW(pcsNameA, csName);
		pcsName = csName;
	}
	else
		pcsName = NULL;

	hReturn = StgCreateDocfile(pcsName, grfMode, reserved, &pstg);
	if (FAILED(hReturn))
		return (hReturn);

	hResult = WrapIStorageAFromW(pstg, ppstgOpenA);
	if (SUCCEEDED(hResult))
		hResult = hReturn;

	if (pstg)
		pstg->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    StgCreateDocfileOnILockBytesA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI StgCreateDocfileOnILockBytesA(ILockBytesA *plckbytA,
					DWORD grfMode,
					DWORD reserved,
					IStorageA * *ppstgOpenA)
{
	TraceSTDAPIEnter("StgCreateDocfileOnILockBytesA");
	ILockBytes * plckbyt;
	IStorage * pstg;
	HRESULT hResult;
	HRESULT hReturn;


	*ppstgOpenA = NULL;

	hReturn = WrapILockBytesWFromA(plckbytA, &plckbyt);
	if (FAILED(hReturn))
		return (hReturn);

	hReturn = StgCreateDocfileOnILockBytes(plckbyt, grfMode, reserved, &pstg);
	if (FAILED(hReturn))
		goto Error;

	hResult = WrapIStorageAFromW(pstg, ppstgOpenA);
	if (FAILED(hResult))
		hReturn = hResult;

	if (pstg)
		pstg->Release();

Error:
	if (plckbyt)
		plckbyt->Release();

	return hReturn;
}


//+--------------------------------------------------------------------------
//
//  Routine:    StgOpenStorageA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI StgOpenStorageA(LPCSTR pcsNameA,
			  IStorageA *pstgPriorityA,
			  DWORD grfMode,
			  SNBA snbExcludeA,
			  DWORD reserved,
			  IStorageA * *ppstgOpenA)
{
	TraceSTDAPIEnter("StgOpenStorageA");

	OLECHAR   csName[MAX_PATH];
	LPOLESTR  pcsName;
	LPSTORAGE pstgPriority;
	SNB       snbExclude;
	LPSTORAGE pstg;
	HRESULT   hResult;


	*ppstgOpenA = NULL;

	if (pcsNameA)
	{
		ConvertStringToW(pcsNameA, csName);
		pcsName= csName;
	}
	else
		pcsName = NULL;

	hResult = WrapIStorageWFromA(pstgPriorityA, &pstgPriority);
	if (FAILED(hResult))
		return hResult;

	hResult = ConvertSNBToW(snbExcludeA, &snbExclude);
	if (FAILED(hResult))
		goto Error;

	hResult = StgOpenStorage(pcsName, pstgPriority, grfMode, snbExclude, reserved, &pstg);
	if (FAILED(hResult))
		goto Error1;

	hResult = WrapIStorageAFromW(pstg, ppstgOpenA);

	if (pstg)
		pstg->Release();

Error1:
	ConvertSNBFree(snbExclude);

Error:
	if (pstgPriority)
		pstgPriority->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    StgOpenStorageOnILockBytesA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI StgOpenStorageOnILockBytesA(ILockBytesA *plckbytA,
				  IStorageA *pstgPriorityA,
				  DWORD grfMode,
				  SNBA snbExcludeA,
				  DWORD reserved,
				  IStorageA * *ppstgOpenA)
{
	TraceSTDAPIEnter("StgOpenStorageOnILockBytesA");
	ILockBytes * plckbyt;
	IStorage * pstgPriority;
	IStorage * pstg;
	SNB        snbExclude;
	HRESULT hResult;


	*ppstgOpenA = NULL;

	hResult = WrapILockBytesWFromA(plckbytA, &plckbyt);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapIStorageWFromA(pstgPriorityA, &pstgPriority);
	if (FAILED(hResult))
		goto Error;

	hResult = ConvertSNBToW(snbExcludeA, &snbExclude);
	if (FAILED(hResult))
		goto Error1;

	hResult = StgOpenStorageOnILockBytes(plckbyt, pstgPriority, grfMode, snbExclude, reserved, &pstg);
	if (FAILED(hResult))
		goto Error2;

	hResult = WrapIStorageAFromW(pstg, ppstgOpenA);

	if (pstg)
		pstg->Release();

Error2:
	ConvertSNBFree(snbExclude);

Error1:
	if (pstgPriority)
		pstgPriority->Release();

Error:
	if (plckbyt)
		plckbyt->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    StgIsStorageFileA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI StgIsStorageFileA(LPCSTR pcsNameA)
{
	TraceSTDAPIEnter("StgIsStorageFileA");

	OLECHAR csName[MAX_PATH];


	ConvertStringToW(pcsNameA, csName);

	return StgIsStorageFile(csName);
}


//+--------------------------------------------------------------------------
//
//  Routine:    StgIsStorageILockBytesA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI StgIsStorageILockBytesA(ILockBytesA * plckbytA)
{
	TraceSTDAPIEnter("StgIsStorageILockBytesA");
	ILockBytes * plckbyt;
	HRESULT hResult;


	hResult = WrapILockBytesWFromA(plckbytA, &plckbyt);
	if (FAILED(hResult))
		return (hResult);

	hResult = StgIsStorageILockBytes(plckbyt);

	if (plckbyt)
		plckbyt->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    StgSetTimesA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI StgSetTimesA(LPCSTR lpszNameA,
		  FILETIME const FAR* pctime,
				  FILETIME const FAR* patime,
				  FILETIME const FAR* pmtime)
{
	TraceSTDAPIEnter("StgSetTimesA");

	OLECHAR szName[MAX_PATH];


	ConvertStringToW(lpszNameA, szName);

	return StgSetTimes(szName, pctime, patime, pmtime);
}

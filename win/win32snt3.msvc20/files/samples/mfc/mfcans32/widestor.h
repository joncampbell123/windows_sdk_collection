//+--------------------------------------------------------------------------
//
//  Copyright (C) 1994, Microsoft Corporation.  All Rights Reserved.
//
//  File:   widestor.h
//
//  Contents:   Unicode Wrappers for ANSI Storage Interfaces.
//
//  Classes:    CEnumSTATSTGW - ANSI wrapper object for IEnumSTATSTG.
//      CLockBytesW   - ANSI wrapper object for ILockBytes.
//      CStreamW      - ANSI wrapper object for IStream.
//      CStorageW     - ANSI wrapper object for IStorage.
//      CRootStorageW - ANSI wrapper object for IRootStorage.
//
//  History:    01-Nov-93   v-kentc     Created.
//
//---------------------------------------------------------------------------


//
//  Forward declarations
//
class CEnumSTATSTGW;
class CLockBytesW;
class CStreamW;
class CStorageW;
class CRootStorageW;


//+--------------------------------------------------------------------------
//
//  Class:  CEnumSTATSTGW
//
//  Synopsis:   Class definition of IEnumSTATSTGW
//
//---------------------------------------------------------------------------
class CEnumSTATSTGW : CWideInterface
{
public:
	// *** IEnumSTATSTGW methods ***
	STDMETHOD(Next) (ULONG celt,
			   STATSTG * rgelt,
					   ULONG * pceltFetched);
	STDMETHOD(Skip) (ULONG celt);
	STDMETHOD(Reset) (VOID);
	STDMETHOD(Clone) (IEnumSTATSTG * * ppenm);

	inline CEnumSTATSTGW(LPUNKNOWN pUnk, IEnumSTATSTGA * pANSI) :
			CWideInterface(ID_IEnumSTATSTG, pUnk, (LPUNKNOWN)pANSI) {};

	inline IEnumSTATSTGA * GetANSI() const
		{ return (IEnumSTATSTGA *)m_pObj; };
};


//+--------------------------------------------------------------------------
//
//  Class:  CLockBytesW
//
//  Synopsis:   Class definition of ILockBytesW
//
//---------------------------------------------------------------------------

class CLockBytesW : CWideInterface
{
public:
	// *** ILockBytes methods ***
	STDMETHOD(ReadAt) (ULARGE_INTEGER ulOffset,
			 VOID HUGEP *pv,
			 ULONG cb,
			 ULONG *pcbRead);
	STDMETHOD(WriteAt) (ULARGE_INTEGER ulOffset,
			  VOID const HUGEP *pv,
			  ULONG cb,
			  ULONG *pcbWritten);
	STDMETHOD(Flush) (VOID);
	STDMETHOD(SetSize) (ULARGE_INTEGER cb);
	STDMETHOD(LockRegion) (ULARGE_INTEGER libOffset,
				 ULARGE_INTEGER cb,
				 DWORD dwLockType);
	STDMETHOD(UnlockRegion) (ULARGE_INTEGER libOffset,
				   ULARGE_INTEGER cb,
				 DWORD dwLockType);
	STDMETHOD(Stat) (STATSTG *pstatstg, DWORD grfStatFlag);

	inline CLockBytesW(LPUNKNOWN pUnk, ILockBytesA * pANSI) :
			CWideInterface(ID_ILockBytes, pUnk, (LPUNKNOWN)pANSI) {};

	inline ILockBytesA * GetANSI() const
		{ return (ILockBytesA *)m_pObj; };
};


//+--------------------------------------------------------------------------
//
//  Class:  CStreamW
//
//  Synopsis:   Class definition of IStreamW
//
//---------------------------------------------------------------------------

class CStreamW : CWideInterface
{
public:
	// *** IStream methods ***
	STDMETHOD(Read) (VOID HUGEP *pv,
			 ULONG cb, ULONG *pcbRead);
	STDMETHOD(Write) (VOID const HUGEP *pv,
			ULONG cb,
			ULONG *pcbWritten);
	STDMETHOD(Seek) (LARGE_INTEGER dlibMove,
			   DWORD dwOrigin,
			   ULARGE_INTEGER *plibNewPosition);
	STDMETHOD(SetSize) (ULARGE_INTEGER libNewSize);
	STDMETHOD(CopyTo) (IStream *pstm,
			 ULARGE_INTEGER cb,
			 ULARGE_INTEGER *pcbRead,
			 ULARGE_INTEGER *pcbWritten);
	STDMETHOD(Commit) (DWORD grfCommitFlags);
	STDMETHOD(Revert) (VOID);
	STDMETHOD(LockRegion) (ULARGE_INTEGER libOffset,
				 ULARGE_INTEGER cb,
				 DWORD dwLockType);
	STDMETHOD(UnlockRegion) (ULARGE_INTEGER libOffset,
				 ULARGE_INTEGER cb,
				 DWORD dwLockType);
	STDMETHOD(Stat) (STATSTG *pstatstg, DWORD grfStatFlag);
	STDMETHOD(Clone)(IStream * *ppstm);

	inline CStreamW(LPUNKNOWN pUnk, IStreamA * pANSI) :
			CWideInterface(ID_IStream, pUnk, (LPUNKNOWN)pANSI) {};

	inline IStreamA * GetANSI() const
		{ return (IStreamA *)m_pObj; };
};


//+--------------------------------------------------------------------------
//
//  Class:  CStorageW
//
//  Synopsis:   Class definition of IStorageW
//
//---------------------------------------------------------------------------

class CStorageW : CWideInterface
{
public:
	// *** IStorage methods ***
	STDMETHOD(CreateStream) (LPCOLESTR psName,
				   DWORD grfMode,
				   DWORD reserved1,
				   DWORD reserved2,
		   IStream **ppstm);
	STDMETHOD(OpenStream) (LPCOLESTR psName,
		 void *reserved1,
				 DWORD grfMode,
				 DWORD reserved2,
		 IStream **ppstm);
	STDMETHOD(CreateStorage) (LPCOLESTR psName,
				DWORD grfMode,
				DWORD reserved1,
				DWORD reserved2,
		IStorage **ppstg);
	STDMETHOD(OpenStorage) (LPCOLESTR psName,
		  IStorage *pstgPriority,
				  DWORD grfMode,
				  SNB snbExclude,
				  DWORD reserved,
		  IStorage **ppstg);
	STDMETHOD(CopyTo) (DWORD ciidExclude,
			   IID const *rgiidExclude,
			   SNB snbExclude,
		   IStorage *pstgDest);
	STDMETHOD(MoveElementTo) (LPCOLESTR lpszName,
			  IStorage *pstgDest,
							  LPCOLESTR lpszNewName,
							  DWORD grfFlags);
	STDMETHOD(Commit) (DWORD grfCommitFlags);
	STDMETHOD(Revert) (VOID);
	STDMETHOD(EnumElements) (DWORD reserved1,
				 void *reserved2,
				 DWORD reserved3,
		 IEnumSTATSTG **ppenm);
	STDMETHOD(DestroyElement) (LPCOLESTR psName);
	STDMETHOD(RenameElement) (LPCOLESTR pcsOldName,
				LPCOLESTR pcsNewName);
	STDMETHOD(SetElementTimes) (LPCOLESTR lpszName,
						FILETIME const *pctime,
								FILETIME const *patime,
								FILETIME const *pmtime);
	STDMETHOD(SetClass) (REFCLSID clsid);
	STDMETHOD(SetStateBits) (DWORD grfStateBits, DWORD grfMask);
	STDMETHOD(Stat) (STATSTG *pstatstg, DWORD grfStatFlag);

	inline CStorageW(LPUNKNOWN pUnk, IStorageA * pANSI) :
			CWideInterface(ID_IStorage, pUnk, (LPUNKNOWN)pANSI) {};

	inline IStorageA * GetANSI() const
		{ return (IStorageA *)m_pObj; };
};


//+--------------------------------------------------------------------------
//
//  Class:  CRootStorageW
//
//  Synopsis:   Class definition of IRootStorageW
//
//---------------------------------------------------------------------------

class CRootStorageW : CWideInterface
{
public:
	// *** IRootStorage methods ***
	STDMETHOD(SwitchToFile) (LPOLESTR lpstrFile);

	inline CRootStorageW(LPUNKNOWN pUnk, IRootStorageA * pANSI) :
			CWideInterface(ID_IRootStorage, pUnk, (LPUNKNOWN)pANSI) {};

	inline IRootStorageA * GetANSI() const
		{ return (IRootStorageA *)m_pObj; };
};

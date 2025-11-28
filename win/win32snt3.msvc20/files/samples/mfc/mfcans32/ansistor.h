//+--------------------------------------------------------------------------
//
//  Copyright (C) 1994, Microsoft Corporation.  All Rights Reserved.
//
//  File:       ansistor.h
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


//
//  Forward declarations
//
class CEnumSTATSTGA;
class CLockBytesA;
class CStreamA;
class CStorageA;
class CRootStorageA;


typedef LPSTR * SNBA;

typedef struct tagSTATSTGA
{
	LPSTR pwcsName;
	DWORD type;
	ULARGE_INTEGER cbSize;
	FILETIME mtime;
	FILETIME ctime;
	FILETIME atime;
	DWORD grfMode;
	DWORD grfLocksSupported;
	CLSID clsid;
	DWORD grfStateBits;
	DWORD reserved;
} STATSTGA;


/*---------------------------------------------------------------------*/
/*                            IEnumSTATSTGA                            */
/*---------------------------------------------------------------------*/

#undef  INTERFACE
#define INTERFACE   IEnumSTATSTGA

DECLARE_INTERFACE_(IEnumSTATSTGA, IUnknown)
{
	// *** IUnknown methods ***
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
	STDMETHOD_(ULONG,Release) (THIS) PURE;

	// *** IENUMSTATSTGA methods ***
	STDMETHOD(Next) (THIS_ ULONG celt, STATSTGA * rgelt, ULONG *pceltFetched) PURE;
	STDMETHOD(Skip) (THIS_ ULONG celt) PURE;
	STDMETHOD(Reset) (THIS) PURE;
	STDMETHOD(Clone) (THIS_ IEnumSTATSTGA **ppenm) PURE;
};

typedef IEnumSTATSTGA * LPENUMSTATSTGA;


/*---------------------------------------------------------------------*/
/*                             ILockBytesA                             */
/*---------------------------------------------------------------------*/

#undef  INTERFACE
#define INTERFACE   ILockBytesA

DECLARE_INTERFACE_(ILockBytesA, IUnknown)
{
	// *** IUnknown methods ***
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
	STDMETHOD_(ULONG,Release) (THIS) PURE;

	// *** ILockBytes methods ***
	STDMETHOD(ReadAt) (THIS_ ULARGE_INTEGER ulOffset,
			 VOID HUGEP *pv,
			 ULONG cb,
			 ULONG *pcbRead) PURE;
	STDMETHOD(WriteAt) (THIS_ ULARGE_INTEGER ulOffset,
			  VOID const HUGEP *pv,
			  ULONG cb,
			  ULONG *pcbWritten) PURE;
	STDMETHOD(Flush) (THIS) PURE;
	STDMETHOD(SetSize) (THIS_ ULARGE_INTEGER cb) PURE;
	STDMETHOD(LockRegion) (THIS_ ULARGE_INTEGER libOffset,
				 ULARGE_INTEGER cb,
				 DWORD dwLockType) PURE;
	STDMETHOD(UnlockRegion) (THIS_ ULARGE_INTEGER libOffset,
				   ULARGE_INTEGER cb,
				 DWORD dwLockType) PURE;
	STDMETHOD(Stat) (THIS_ STATSTGA *pstatstg, DWORD grfStatFlag) PURE;
};

typedef ILockBytesA * LPLOCKBYTESA;


/*---------------------------------------------------------------------*/
/*                              IStreamA                               */
/*---------------------------------------------------------------------*/

#undef  INTERFACE
#define INTERFACE   IStreamA

DECLARE_INTERFACE_(IStreamA, IUnknown)
{
	// *** IUnknown methods ***
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
	STDMETHOD_(ULONG,Release) (THIS) PURE;

	// *** IStreamA methods ***
	STDMETHOD(Read) (THIS_ VOID HUGEP *pv,
			 ULONG cb, ULONG *pcbRead) PURE;
	STDMETHOD(Write) (THIS_ VOID const HUGEP *pv,
			  ULONG cb,
			  ULONG *pcbWritten) PURE;
	STDMETHOD(Seek) (THIS_ LARGE_INTEGER dlibMove,
			   DWORD dwOrigin,
			   ULARGE_INTEGER *plibNewPosition) PURE;
	STDMETHOD(SetSize) (THIS_ ULARGE_INTEGER libNewSize) PURE;
	STDMETHOD(CopyTo) (THIS_ IStreamA *pstm,
			 ULARGE_INTEGER cb,
			 ULARGE_INTEGER *pcbRead,
			 ULARGE_INTEGER *pcbWritten) PURE;
	STDMETHOD(Commit) (THIS_ DWORD grfCommitFlags) PURE;
	STDMETHOD(Revert) (THIS) PURE;
	STDMETHOD(LockRegion) (THIS_ ULARGE_INTEGER libOffset,
				 ULARGE_INTEGER cb,
				 DWORD dwLockType) PURE;
	STDMETHOD(UnlockRegion) (THIS_ ULARGE_INTEGER libOffset,
				 ULARGE_INTEGER cb,
				 DWORD dwLockType) PURE;
	STDMETHOD(Stat) (THIS_ STATSTGA *pstatstg, DWORD grfStatFlag) PURE;
	STDMETHOD(Clone)(THIS_ IStreamA * *ppstm) PURE;
};

typedef IStreamA * LPSTREAMA;


/*---------------------------------------------------------------------*/
/*                              IStorageA                              */
/*---------------------------------------------------------------------*/

#undef  INTERFACE
#define INTERFACE   IStorageA

DECLARE_INTERFACE_(IStorageA, IUnknown)
{
	// *** IUnknown methods ***
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
	STDMETHOD_(ULONG,Release) (THIS) PURE;

	// *** IStorage methods ***
	STDMETHOD(CreateStream) (THIS_ LPCSTR pwcsName,
				   DWORD grfMode,
				   DWORD reserved1,
				   DWORD reserved2,
				   IStreamA **ppstm) PURE;
	STDMETHOD(OpenStream) (THIS_ LPCSTR pwcsName,
		 void *reserved1,
				 DWORD grfMode,
				 DWORD reserved2,
				 IStreamA **ppstm) PURE;
	STDMETHOD(CreateStorage) (THIS_ LPCSTR pwcsName,
				DWORD grfMode,
				DWORD reserved1,
				DWORD reserved2,
				IStorageA **ppstg) PURE;
	STDMETHOD(OpenStorage) (THIS_ LPCSTR pwcsName,
				  IStorageA *pstgPriority,
				  DWORD grfMode,
				  SNBA snbExclude,
				  DWORD reserved,
				  IStorageA **ppstg) PURE;
	STDMETHOD(CopyTo) (THIS_ DWORD ciidExclude,
			   IID const *rgiidExclude,
			   SNBA snbExclude,
			   IStorageA *pstgDest) PURE;
	STDMETHOD(MoveElementTo) (THIS_ LPCSTR lpszName,
					  IStorageA *pstgDest,
							  LPCSTR lpszNewName,
							  DWORD grfFlags) PURE;
	STDMETHOD(Commit) (THIS_ DWORD grfCommitFlags) PURE;
	STDMETHOD(Revert) (THIS) PURE;
	STDMETHOD(EnumElements) (THIS_ DWORD reserved1,
				 void *reserved2,
				 DWORD reserved3,
				 IEnumSTATSTGA **ppenm) PURE;
	STDMETHOD(DestroyElement) (THIS_ LPCSTR pwcsName) PURE;
	STDMETHOD(RenameElement) (THIS_ LPCSTR pcsOldName,
				LPCSTR pcsNewName) PURE;
	STDMETHOD(SetElementTimes) (THIS_ LPCSTR lpszName,
						FILETIME const *pctime,
								FILETIME const *patime,
								FILETIME const *pmtime) PURE;
	STDMETHOD(SetClass) (THIS_ REFCLSID clsid) PURE;
	STDMETHOD(SetStateBits) (THIS_ DWORD grfStateBits, DWORD grfMask) PURE;
	STDMETHOD(Stat) (THIS_ STATSTGA *pstatstgA, DWORD grfStatFlag) PURE;
};

typedef IStorageA * LPSTORAGEA;


/*---------------------------------------------------------------------*/
/*                            IRootStorageA                            */
/*---------------------------------------------------------------------*/

#undef  INTERFACE
#define INTERFACE   IRootStorageA

DECLARE_INTERFACE_(IRootStorageA, IUnknown)
{
	// *** IUnknown methods ***
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
	STDMETHOD_(ULONG,Release) (THIS) PURE;

	// *** IRootStorage methods ***
	STDMETHOD(SwitchToFile) (THIS_ LPSTR lpstrFile) PURE;
};

typedef IRootStorageA * LPROOTSTORAGEA;


/*---------------------------------------------------------------------*/
/*                         ANSI Storage API's                          */
/*---------------------------------------------------------------------*/

STDAPI StgCreateDocfileA(LPCSTR pwcsName,
			DWORD grfMode,
			DWORD reserved,
			IStorageA * *ppstgOpenA);
STDAPI StgCreateDocfileOnILockBytesA(ILockBytesA *plkbyt,
					DWORD grfMode,
					DWORD reserved,
					IStorageA * *ppstgOpen);
STDAPI StgOpenStorageA(LPCSTR pwcsName,
			  IStorageA *pstgPriority,
			  DWORD grfMode,
			  SNBA snbExclude,
			  DWORD reserved,
			  IStorageA * *ppstgOpenA);
STDAPI StgOpenStorageOnILockBytesA(ILockBytesA *plkbyt,
				  IStorageA *pstgPriority,
				  DWORD grfMode,
				  SNBA snbExclude,
				  DWORD reserved,
				  IStorageA * *ppstgOpen);
STDAPI StgIsStorageFileA(LPCSTR pwcsName);
STDAPI StgIsStorageILockBytesA(ILockBytesA * plkbyt);
STDAPI StgSetTimesA(LPCSTR lpszName,
		  FILETIME const * pctime,
				  FILETIME const * patime,
				  FILETIME const * pmtime);


//+--------------------------------------------------------------------------
//
//  Class:      CEnumSTATSTGA
//
//  Synopsis:   Class definition of IEnumSTATSTGA
//
//---------------------------------------------------------------------------
class CEnumSTATSTGA : CAnsiInterface
{
public:
	// *** IEnumSTATSTGA methods ***
	STDMETHOD(Next) (ULONG celt,
					   STATSTGA * rgelt,
					   ULONG * pceltFetched);
	STDMETHOD(Skip) (ULONG celt);
	STDMETHOD(Reset) (VOID);
	STDMETHOD(Clone) (IEnumSTATSTGA * * ppenm);

	inline CEnumSTATSTGA(LPUNKNOWN pUnk, IEnumSTATSTG * pObj) :
			CAnsiInterface(ID_IEnumSTATSTG, pUnk, (LPUNKNOWN)pObj) {};

	inline IEnumSTATSTG * GetWide() const
			{ return (IEnumSTATSTG *)m_pObj; };
};


//+--------------------------------------------------------------------------
//
//  Class:      CLockBytesA
//
//  Synopsis:   Class definition of ILockBytesA
//
//---------------------------------------------------------------------------

class CLockBytesA : CAnsiInterface
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
	STDMETHOD(Stat) (STATSTGA *pstatstg, DWORD grfStatFlag);

	inline CLockBytesA(LPUNKNOWN pUnk, ILockBytes * pObj) :
			CAnsiInterface(ID_ILockBytes, pUnk, (LPUNKNOWN)pObj) {};

	inline ILockBytes * GetWide() const
			{ return (ILockBytes *)m_pObj; };
};


//+--------------------------------------------------------------------------
//
//  Class:      CStreamA
//
//  Synopsis:   Class definition of IStreamA
//
//---------------------------------------------------------------------------

class CStreamA : public CAnsiInterface
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
	STDMETHOD(CopyTo) (IStreamA *pstm,
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
	STDMETHOD(Stat) (STATSTGA *pstatstg, DWORD grfStatFlag);
	STDMETHOD(Clone)(IStreamA * *ppstm);

	inline CStreamA(LPUNKNOWN pUnk, IStream * pObj) :
			CAnsiInterface(ID_IStream, pUnk, (LPUNKNOWN)pObj) {};

	inline IStream * GetWide() const
			{ return (IStream *)m_pObj; };
};


//+--------------------------------------------------------------------------
//
//  Class:      CStorageA
//
//  Synopsis:   Class definition of IStorageA
//
//---------------------------------------------------------------------------

class CStorageA : public CAnsiInterface
{
public:
	// *** IStorage methods ***
	STDMETHOD(CreateStream) (LPCSTR psName,
				   DWORD grfMode,
				   DWORD reserved1,
				   DWORD reserved2,
				   IStreamA **ppstm);
	STDMETHOD(OpenStream) (LPCSTR psName,
		 void *reserved1,
				 DWORD grfMode,
				 DWORD reserved2,
				 IStreamA **ppstm);
	STDMETHOD(CreateStorage) (LPCSTR psName,
				DWORD grfMode,
				DWORD reserved1,
				DWORD reserved2,
				IStorageA **ppstg);
	STDMETHOD(OpenStorage) (LPCSTR psName,
				  IStorageA *pstgPriority,
				  DWORD grfMode,
				  SNBA snbExclude,
				  DWORD reserved,
				  IStorageA **ppstg);
	STDMETHOD(CopyTo) (DWORD ciidExclude,
			   IID const *rgiidExclude,
			   SNBA snbExclude,
			   IStorageA *pstgDest);
	STDMETHOD(MoveElementTo) (LPCSTR lpszName,
					  IStorageA *pstgDest,
							  LPCSTR lpszNewName,
							  DWORD grfFlags);
	STDMETHOD(Commit) (DWORD grfCommitFlags);
	STDMETHOD(Revert) (VOID);
	STDMETHOD(EnumElements) (DWORD reserved1,
				 void *reserved2,
				 DWORD reserved3,
				 IEnumSTATSTGA **ppenm);
	STDMETHOD(DestroyElement) (LPCSTR psName);
	STDMETHOD(RenameElement) (LPCSTR pcsOldName,
				LPCSTR pcsNewName);
	STDMETHOD(SetElementTimes) (LPCSTR lpszName,
						FILETIME const *pctime,
								FILETIME const *patime,
								FILETIME const *pmtime);
	STDMETHOD(SetClass) (REFCLSID clsid);
	STDMETHOD(SetStateBits) (DWORD grfStateBits, DWORD grfMask);
	STDMETHOD(Stat) (STATSTGA *pstatstg, DWORD grfStatFlag);

	inline CStorageA(LPUNKNOWN pUnk, IStorage * pObj) :
			CAnsiInterface(ID_IStorage, pUnk, (LPUNKNOWN)pObj) {};

	inline IStorage * GetWide() const
			{ return (IStorage *)m_pObj; };
};


//+--------------------------------------------------------------------------
//
//  Class:      CRootStorageA
//
//  Synopsis:   Class definition of IRootStorageA
//
//---------------------------------------------------------------------------

class CRootStorageA : public CAnsiInterface
{
public:
	// *** IRootStorage methods ***
	STDMETHOD(SwitchToFile) (LPSTR lpstrFile);

	inline CRootStorageA(LPUNKNOWN pUnk, IRootStorage * pObj) :
			CAnsiInterface(ID_IRootStorage, pUnk, (LPUNKNOWN)pObj) {};

	inline IRootStorage * GetWide() const
			{ return (IRootStorage *)m_pObj; };
};

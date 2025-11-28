//+--------------------------------------------------------------------------
//
//  Copyright (C) 1994, Microsoft Corporation.  All Rights Reserved.
//
//  File:       ansidvob.h
//
//  Contents:   ANSI Wrappers for Unicode DvObj Interfaces and APIs.
//
//  Classes:    CEnumSTATDATAA
//              CDataObjectA
//              CViewObject2A
//              CAdviseSink2A
//              CDataAdviseHolder
//              COleCache2
//              COleCacheControl
//
//  Functions:  None.
//
//  History:    01-Nov-93   v-kentc     Created.
//
//---------------------------------------------------------------------------


#if defined(__cplusplus)
interface IAdviseSink2A;
#else
typedef interface IAdviseSink2A IAdviseSink2A;
#endif

typedef IAdviseSink2A * LPADVISESINKA;

// Data/View target device; determines the device for drawing or gettting data
typedef struct FARSTRUCT tagDVTARGETDEVICEA
{
	DWORD tdSize;
	WORD tdDeviceNameOffset;
	WORD tdDriverNameOffset;
	WORD tdPortNameOffset;
	WORD tdExtDevmodeOffset;
	BYTE tdData[1];
} DVTARGETDEVICEA, * LPDVTARGETDEVICEA;

typedef DVTARGETDEVICE * LPDVTARGETDEVICE;

// Format, etc.; completely specifices the kind of data desired, including tymed
typedef struct FARSTRUCT tagFORMATETCA
{
	CLIPFORMAT          cfFormat;
	DVTARGETDEVICEA FAR* ptd;
	DWORD               dwAspect;
	LONG                lindex;
	DWORD               tymed;
} FORMATETCA, FAR* LPFORMATETCA;

typedef struct tagSTGMEDIUMA
{
	DWORD   tymed;
	union
	{
		HANDLE  hGlobal;
	LPSTR   lpszFileName;
	IStreamA * pstm;
	IStorageA * pstg;
	}
#ifndef __cplusplus
#ifdef NONAMELESSUNION
	u       // add a tag when name less unions not supported
#endif
#endif
	;
	IUnknown * pUnkForRelease;
} STGMEDIUMA, * LPSTGMEDIUMA;

typedef struct tagSTATDATAA
{                                   // field used by:
	FORMATETCA formatetc;           // EnumAdvise, EnumData (cache), EnumFormats
	DWORD advf;                     // EnumAdvise, EnumData (cache)
	LPADVISESINKA pAdvSink;         // EnumAdvise
	DWORD dwConnection;             // EnumAdvise
} STATDATAA;

typedef  STATDATAA * LPSTATDATAA;


/*---------------------------------------------------------------------*/
/*                IEnumSTATDATAA                                       */
/*---------------------------------------------------------------------*/

#undef  INTERFACE
#define INTERFACE   IEnumSTATDATAA

DECLARE_INTERFACE_(IEnumSTATDATAA, IUnknown)
{
	// *** IUnknown methods ***
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppv) PURE;
	STDMETHOD_(ULONG, AddRef) (THIS)  PURE;
	STDMETHOD_(ULONG, Release) (THIS) PURE;

	// *** IEnumSTATDATA methods ***
	STDMETHOD(Next) (THIS_ ULONG celt, STATDATAA  * rgelt, ULONG * pceltFetched) PURE;
	STDMETHOD(Skip) (THIS_ ULONG celt) PURE;
	STDMETHOD(Reset) (THIS) PURE;
	STDMETHOD(Clone) (THIS_ IEnumSTATDATAA * * ppenum) PURE;
};
typedef        IEnumSTATDATAA * LPENUMSTATDATAA;


/*---------------------------------------------------------------------*/
/*                            IEnumFORMATETCA                          */
/*---------------------------------------------------------------------*/

#undef  INTERFACE
#define INTERFACE   IEnumFORMATETCA

DECLARE_INTERFACE_(IEnumFORMATETCA, IUnknown)
{
	// *** IUnknown methods ***
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
	STDMETHOD_(ULONG,Release) (THIS) PURE;

	// *** IEnumFORMATETC methods ***
	STDMETHOD(Next) (THIS_ ULONG celt,
					   FORMATETCA * rgelt,
					   ULONG * pceltFetched) PURE;
	STDMETHOD(Skip) (THIS_ ULONG celt) PURE;
	STDMETHOD(Reset) (THIS) PURE;
	STDMETHOD(Clone) (THIS_ IEnumFORMATETCA * * ppenm) PURE;
};
typedef IEnumFORMATETCA * LPENUMFORMATETCA;


/*---------------------------------------------------------------------*/
/*                IDataObjectA                                         */
/*---------------------------------------------------------------------*/

#undef  INTERFACE
#define INTERFACE   IDataObjectA

DECLARE_INTERFACE_(IDataObjectA, IUnknown)
{
	// *** IUnknown methods ***
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
	STDMETHOD_(ULONG, Release) (THIS) PURE;

	// *** IDataObject methods ***
	STDMETHOD(GetData) (THIS_ LPFORMATETCA pformatetcIn,
				LPSTGMEDIUMA pmediumA ) PURE;
	STDMETHOD(GetDataHere) (THIS_ LPFORMATETCA pformatetc,
				LPSTGMEDIUMA pmediumA ) PURE;
	STDMETHOD(QueryGetData) (THIS_ LPFORMATETCA pformatetc ) PURE;
	STDMETHOD(GetCanonicalFormatEtc) (THIS_ LPFORMATETCA pformatetc,
							LPFORMATETCA pformatetcOut) PURE;
	STDMETHOD(SetData) (THIS_ LPFORMATETCA pformatetc, STGMEDIUMA  * pmediumA,
							BOOL fRelease) PURE;
	STDMETHOD(EnumFormatEtc) (THIS_ DWORD dwDirection,
							LPENUMFORMATETCA * ppenumFormatEtc) PURE;

	STDMETHOD(DAdvise) (THIS_ FORMATETCA * pFormatetc, DWORD advf,
			LPADVISESINKA pAdvSinkA, DWORD * pdwConnection) PURE;
	STDMETHOD(DUnadvise) (THIS_ DWORD dwConnection) PURE;
	STDMETHOD(EnumDAdvise) (THIS_ LPENUMSTATDATAA * ppenumAdviseA) PURE;
};
typedef      IDataObjectA * LPDATAOBJECTA;


/*---------------------------------------------------------------------*/
/*                             IViewObject2A                           */
/*---------------------------------------------------------------------*/

#undef  INTERFACE
#define INTERFACE   IViewObject2A

DECLARE_INTERFACE_(IViewObject2A, IUnknown)
{
	// *** IUnknown methods ***
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
	STDMETHOD_(ULONG,Release) (THIS) PURE;

	// *** IViewObject methods ***
	STDMETHOD(Draw) (THIS_ DWORD dwDrawAspect, LONG lindex,
					void * pvAspect, DVTARGETDEVICEA  * ptd,
					HDC hicTargetDev,
					HDC hdcDraw,
					LPCRECTL lprcBounds,
					LPCRECTL lprcWBounds,
					BOOL (CALLBACK * pfnContinue) (DWORD),
					DWORD dwContinue) PURE;

	STDMETHOD(GetColorSet) (THIS_ DWORD dwDrawAspect, LONG lindex,
					void * pvAspect, DVTARGETDEVICEA  * ptd,
					HDC hicTargetDev,
					LPLOGPALETTE * ppColorSet) PURE;

	STDMETHOD(Freeze)(THIS_ DWORD dwDrawAspect, LONG lindex,
					void * pvAspect,
					DWORD * pdwFreeze) PURE;
	STDMETHOD(Unfreeze) (THIS_ DWORD dwFreeze) PURE;
	STDMETHOD(SetAdvise) (THIS_ DWORD aspects, DWORD advf,
			LPADVISESINKA pAdvSinkA) PURE;
	STDMETHOD(GetAdvise) (THIS_ DWORD * pAspects, DWORD * pAdvf,
			LPADVISESINKA * ppAdvSinkA) PURE;

	// *** IViewObject2 methods ***
	STDMETHOD(GetExtent) (THIS_ DWORD dwDrawAspect, LONG lindex,
					DVTARGETDEVICEA  * ptd, LPSIZEL lpsizel) PURE;

};
typedef      IViewObject2A * LPVIEWOBJECTA;
typedef      IViewObject2A * LPVIEWOBJECT2A;


/*---------------------------------------------------------------------*/
/*                           IAdviseSink2A                             */
/*---------------------------------------------------------------------*/

#undef  INTERFACE
#define INTERFACE   IAdviseSink2A

DECLARE_INTERFACE_(IAdviseSink2A, IUnknown)
{
	// *** IUnknown methods ***
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppv) PURE;
	STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
	STDMETHOD_(ULONG,Release) (THIS) PURE;

	// *** IAdviseSink methods ***
	STDMETHOD_(void,OnDataChange)(THIS_ FORMATETCA * pFormatetc,
				STGMEDIUMA * pStgmedA) PURE;
	STDMETHOD_(void,OnViewChange)(THIS_ DWORD dwAspect, LONG lindex) PURE;
	STDMETHOD_(void,OnRename)(THIS_ LPMONIKERA pmkA) PURE;
	STDMETHOD_(void,OnSave)(THIS) PURE;
	STDMETHOD_(void,OnClose)(THIS) PURE;

	// *** IAdviseSink2 methods ***
	STDMETHOD_(void,OnLinkSrcChange)(THIS_ LPMONIKERA pmkA) PURE;
};
typedef      IAdviseSink2A * LPADVISESINK2A;


/*---------------------------------------------------------------------*/
/*                IDataAdviseHolderA               */
/*---------------------------------------------------------------------*/

#undef  INTERFACE
#define INTERFACE   IDataAdviseHolderA

DECLARE_INTERFACE_(IDataAdviseHolderA, IUnknown)
{
	// *** IUnknown methods ***
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppv) PURE;
	STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
	STDMETHOD_(ULONG,Release) (THIS) PURE;

	// *** IDataAdviseHolder methods ***
	STDMETHOD(Advise)(THIS_ LPDATAOBJECTA pDataObjectA, FORMATETCA * pFetc,
		DWORD advf, LPADVISESINKA pAdviseA, DWORD * pdwConnection) PURE;
	STDMETHOD(Unadvise)(THIS_ DWORD dwConnection) PURE;
	STDMETHOD(EnumAdvise)(THIS_ LPENUMSTATDATAA * ppenumAdviseA) PURE;

	STDMETHOD(SendOnDataChange)(THIS_ LPDATAOBJECTA pDataObjectA, DWORD dwReserved, DWORD advf) PURE;
};
typedef      IDataAdviseHolderA * LPDATAADVISEHOLDERA;


/*---------------------------------------------------------------------*/
/*                IOleCache2A                  */
/*---------------------------------------------------------------------*/

#undef  INTERFACE
#define INTERFACE   IOleCache2A

DECLARE_INTERFACE_(IOleCache2A, IUnknown)
{
	// *** IUnknown methods ***
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
	STDMETHOD_(ULONG, Release) (THIS) PURE;

	// *** IOleCache methods ***
	STDMETHOD(Cache) (THIS_ LPFORMATETCA lpFormatetc, DWORD advf, LPDWORD lpdwConnection) PURE;
	STDMETHOD(Uncache) (THIS_ DWORD dwConnection) PURE;
	STDMETHOD(EnumCache) (THIS_ LPENUMSTATDATAA * ppenumStatDataA) PURE;
	STDMETHOD(InitCache) (THIS_ LPDATAOBJECTA pDataObjectA) PURE;
	STDMETHOD(SetData) (THIS_ LPFORMATETCA pformatetc, STGMEDIUMA  * pmediumA,
							BOOL fRelease) PURE;

	// *** IOleCache2 methods ***
	STDMETHOD(UpdateCache) (THIS_ LPDATAOBJECTA pDataObjectA, DWORD grfUpdf,
							LPVOID pReserved) PURE;
	STDMETHOD(DiscardCache) (THIS_ DWORD dwDiscardOptions) PURE;

};
typedef      IOleCache2A * LPOLECACHEA;
typedef      IOleCache2A * LPOLECACHE2A;


/*---------------------------------------------------------------------*/
/*                IOleCacheControlA                */
/*---------------------------------------------------------------------*/

#undef  INTERFACE
#define INTERFACE   IOleCacheControlA

DECLARE_INTERFACE_(IOleCacheControlA, IUnknown)
{
	// *** IUnknown methods ***
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
	STDMETHOD_(ULONG, Release) (THIS) PURE;

	// *** IDataObject methods ***
	STDMETHOD(OnRun) (THIS_ LPDATAOBJECTA pDataObjectA) PURE;
	STDMETHOD(OnStop) (THIS) PURE;
};
typedef      IOleCacheControlA * LPOLECACHECONTROLA;



//
//  Forward declarations
//
class CEnumSTATDATAA;
class CDataObjectA;
class CViewObject2A;
class CAdviseSink2A;
class COleCache2A;
class COleCacheControlA;
class CDataAdviseHolderA;



//+--------------------------------------------------------------------------
//
//  Class:      CEnumSTATDATAA
//
//  Synopsis:   Class definition of IEnumSTATDATAA
//
//---------------------------------------------------------------------------
class CEnumSTATDATAA : CAnsiInterface
{
public:
	// *** IEnumSTATDATAA methods ***
	STDMETHOD(Next) (ULONG celt,
					   STATDATAA * rgelt,
					   ULONG * pceltFetched);
	STDMETHOD(Skip) (ULONG celt);
	STDMETHOD(Reset) (VOID);
	STDMETHOD(Clone) (IEnumSTATDATAA * * ppenm);

	inline CEnumSTATDATAA(LPUNKNOWN pUnk, IEnumSTATDATA * pWide) :
			CAnsiInterface(ID_IEnumSTATDATA, pUnk, (LPUNKNOWN)pWide) {};

	inline IEnumSTATDATA * GetWide() const
			{ return (IEnumSTATDATA *)m_pObj; };
};



//+--------------------------------------------------------------------------
//
//  Class:      CEnumFORMATETCA
//
//  Synopsis:   Class definition of IEnumFORMATETCA
//
//---------------------------------------------------------------------------
class CEnumFORMATETCA : CAnsiInterface
{
public:
	// *** IEnumFORMATETCA methods ***
	STDMETHOD(Next) (ULONG celt,
					   FORMATETCA* rgelt,
					   ULONG FAR* pceltFetched);
	STDMETHOD(Skip) (ULONG celt);
	STDMETHOD(Reset) (VOID);
	STDMETHOD(Clone) (IEnumFORMATETCA * * ppenm);

	inline CEnumFORMATETCA(LPUNKNOWN pUnk, IEnumFORMATETC * pObj) :
			CAnsiInterface(ID_IEnumFORMATETC, pUnk, (LPUNKNOWN)pObj) {};

	inline IEnumFORMATETC * GetWide() const
			{ return (IEnumFORMATETC *)m_pObj; };
};



//+--------------------------------------------------------------------------
//
//  Class:      CDataObjectA
//
//  Synopsis:   Class definition of IDataObjectA
//
//---------------------------------------------------------------------------
class CDataObjectA : CAnsiInterface
{
public:
	// *** IDataObject methods ***
	STDMETHOD(GetData) (LPFORMATETCA pformatetcIn,
				LPSTGMEDIUMA pmediumA );
	STDMETHOD(GetDataHere) (LPFORMATETCA pformatetc,
				LPSTGMEDIUMA pmediumA );
	STDMETHOD(QueryGetData) (LPFORMATETCA pformatetc );
	STDMETHOD(GetCanonicalFormatEtc) (LPFORMATETCA pformatetc,
							LPFORMATETCA pformatetcOut);
	STDMETHOD(SetData) (LPFORMATETCA pformatetc, STGMEDIUMA * pmediumA,
							BOOL fRelease);
	STDMETHOD(EnumFormatEtc) (DWORD dwDirection,
							LPENUMFORMATETCA * ppenumFormatEtc);

	STDMETHOD(DAdvise) (FORMATETCA * pFormatetc, DWORD advf,
			LPADVISESINKA pAdvSinkA, DWORD * pdwConnection);
	STDMETHOD(DUnadvise) (DWORD dwConnection);
	STDMETHOD(EnumDAdvise) (LPENUMSTATDATAA * ppenumAdviseA);

	inline CDataObjectA(LPUNKNOWN pUnk, IDataObject * pWide) :
			CAnsiInterface(ID_IDataObject, pUnk, (LPUNKNOWN)pWide) {};

	inline IDataObject * GetWide() const
			{ return (IDataObject *)m_pObj; };
};



//+--------------------------------------------------------------------------
//
//  Class:      CViewObject2A
//
//  Synopsis:   Class definition of IViewObject2A
//
//---------------------------------------------------------------------------
class CViewObject2A : CAnsiInterface
{
public:
	// *** IViewObject methods ***
	STDMETHOD(Draw) (DWORD dwDrawAspect, LONG lindex,
					void * pvAspect, DVTARGETDEVICEA * ptd,
					HDC hicTargetDev,
					HDC hdcDraw,
					LPCRECTL lprcBounds,
					LPCRECTL lprcWBounds,
					BOOL (CALLBACK * pfnContinue) (DWORD),
					DWORD dwContinue);

	STDMETHOD(GetColorSet) (DWORD dwDrawAspect, LONG lindex,
					void * pvAspect, DVTARGETDEVICEA * ptd,
					HDC hicTargetDev,
					LPLOGPALETTE * ppColorSet);

	STDMETHOD(Freeze)(DWORD dwDrawAspect, LONG lindex,
					void * pvAspect,
					DWORD * pdwFreeze);
	STDMETHOD(Unfreeze) (DWORD dwFreeze);
	STDMETHOD(SetAdvise) (DWORD aspects, DWORD advf,
			LPADVISESINKA pAdvSinkA);
	STDMETHOD(GetAdvise) (DWORD * pAspects, DWORD * pAdvf,
			LPADVISESINKA * ppAdvSinkA);

	// *** IViewObject2 methods ***
	STDMETHOD(GetExtent) (DWORD dwDrawAspect, LONG lindex,
					DVTARGETDEVICEA * ptd, LPSIZEL lpsizel);


	inline CViewObject2A(LPUNKNOWN pUnk, IViewObject2 * pWide) :
			CAnsiInterface(ID_IViewObject2, pUnk, (LPUNKNOWN)pWide) {};

	inline IViewObject2 * GetWide() const
			{ return (IViewObject2 *)m_pObj; };
};



//+--------------------------------------------------------------------------
//
//  Class:      CAdviseSink2A
//
//  Synopsis:   Class definition of IAdviseSink2A
//
//---------------------------------------------------------------------------
class CAdviseSink2A : CAnsiInterface
{
public:
	// *** IAdviseSink methods ***
	STDMETHOD_(void,OnDataChange)(FORMATETCA * pFormatetc,
				STGMEDIUMA * pStgmedA);
	STDMETHOD_(void,OnViewChange)(DWORD dwAspect, LONG lindex);
	STDMETHOD_(void,OnRename)(LPMONIKERA pmkA);
	STDMETHOD_(void,OnSave)(VOID);
	STDMETHOD_(void,OnClose)(VOID);

	// *** IAdviseSink2 methods ***
	STDMETHOD_(void,OnLinkSrcChange)(LPMONIKERA pmkA);

	inline CAdviseSink2A(LPUNKNOWN pUnk, IAdviseSink2 * pWide) :
			CAnsiInterface(ID_IAdviseSink2, pUnk, (LPUNKNOWN)pWide) {};

	inline IAdviseSink2 * GetWide() const
			{ return (IAdviseSink2 *)m_pObj; };
};



//+--------------------------------------------------------------------------
//
//  Class:      CDataAdviseHolderA
//
//  Synopsis:   Class definition of IDataAdviseHolderA
//
//---------------------------------------------------------------------------
class CDataAdviseHolderA : CAnsiInterface
{
public:
	// *** IDataAdviseHolder methods ***
	STDMETHOD(Advise)(LPDATAOBJECTA pDataObjectA, FORMATETCA * pFetc,
		DWORD advf, LPADVISESINKA pAdviseA, DWORD * pdwConnection);
	STDMETHOD(Unadvise)(DWORD dwConnection);
	STDMETHOD(EnumAdvise)(LPENUMSTATDATAA * ppenumAdviseA);

	STDMETHOD(SendOnDataChange)(LPDATAOBJECTA pDataObjectA, DWORD dwReserved, DWORD advf);

	inline CDataAdviseHolderA(LPUNKNOWN pUnk, IDataAdviseHolder * pWide) :
			CAnsiInterface(ID_IDataAdviseHolder, pUnk, (LPUNKNOWN)pWide) {};

	inline IDataAdviseHolder * GetWide() const
			{ return (IDataAdviseHolder *)m_pObj; };
};



//+--------------------------------------------------------------------------
//
//  Class:      COleCache2A
//
//  Synopsis:   Class definition of IOleCache2A
//
//---------------------------------------------------------------------------
class COleCache2A : CAnsiInterface
{
public:
	// *** IOleCache methods ***
	STDMETHOD(Cache) (LPFORMATETCA lpFormatetc, DWORD advf, LPDWORD lpdwConnection);
	STDMETHOD(Uncache) (DWORD dwConnection);
	STDMETHOD(EnumCache) (LPENUMSTATDATAA * ppenumStatDataA);
	STDMETHOD(InitCache) (LPDATAOBJECTA pDataObjectA);
	STDMETHOD(SetData) (LPFORMATETCA pformatetc, STGMEDIUMA * pmediumA,
							BOOL fRelease);

	// *** IOleCache2 methods ***
	STDMETHOD(UpdateCache) (LPDATAOBJECTA pDataObjectA, DWORD grfUpdf,
							LPVOID pReserved);
	STDMETHOD(DiscardCache) (DWORD dwDiscardOptions);


	inline COleCache2A(LPUNKNOWN pUnk, IOleCache2 * pWide) :
			CAnsiInterface(ID_IOleCache2, pUnk, (LPUNKNOWN)pWide) {};

	inline IOleCache2 * GetWide() const
			{ return (IOleCache2 *)m_pObj; };
};



//+--------------------------------------------------------------------------
//
//  Class:      COleCacheControlA
//
//  Synopsis:   Class definition of IOleCacheControlA
//
//---------------------------------------------------------------------------
class COleCacheControlA : CAnsiInterface
{
public:
	// *** IDataObject methods ***
	STDMETHOD(OnRun) (LPDATAOBJECTA pDataObjectA);
	STDMETHOD(OnStop) (VOID);

	inline COleCacheControlA(LPUNKNOWN pUnk, IOleCacheControl * pWide) :
			CAnsiInterface(ID_IOleCacheControl, pUnk, (LPUNKNOWN)pWide) {};

	inline IOleCacheControl * GetWide() const
			{ return (IOleCacheControl *)m_pObj; };
};

//+--------------------------------------------------------------------------
//
//  Copyright (C) 1994, Microsoft Corporation.  All Rights Reserved.
//
//  File:       widedvob.h
//
//  Contents:   Unicode Wrappers for ANSI DvObj Interfaces and APIs.
//
//  Classes:    CEnumSTATDATAW
//              CDataObjectW
//              CViewObjectW
//              CViewObject2W
//              CAdviseSinkW
//              CAdviseSink2W
//              CDataAdviseHolderW
//              COleCacheW
//              COleCache2W
//              COleCacheControlW
//
//  Functions:  IEnumSTATDATAWFromA
//              IDataObjectWFromA
//              IViewObjectWFromA
//              IViewObject2WFromA
//              IAdviseSinkWFromA
//              IAdviseSink2WFromA
//              IDataAdviseHolderWFromA
//              IOleCacheWFromA
//              IOleCache2WFromA
//              IOleCacheControlWFromA
//
//  History:    01-Nov-93   v-kentc     Created.
//
//---------------------------------------------------------------------------


//
//  Forward declarations
//
class CEnumSTATDATAW;
class CDataObjectW;
class CViewObject2W;
class CAdviseSink2W;
class COleCache2W;
class COleCacheControlW;
class CDataAdviseHolderW;



//+--------------------------------------------------------------------------
//
//  Class:      CEnumSTATDATAW
//
//  Synopsis:   Class definition of IEnumSTATDATAW
//
//---------------------------------------------------------------------------
class CEnumSTATDATAW : CWideInterface
{
public:
	// *** IEnumSTATDATAA methods ***
	STDMETHOD(Next) (ULONG celt,
					   STATDATA * rgelt,
					   ULONG * pceltFetched);
	STDMETHOD(Skip) (ULONG celt);
	STDMETHOD(Reset) (VOID);
	STDMETHOD(Clone) (IEnumSTATDATA * * ppenm);

	inline CEnumSTATDATAW(LPUNKNOWN pUnk, IEnumSTATDATAA * pANSI) :
			CWideInterface(ID_IEnumSTATDATA, pUnk, (LPUNKNOWN)pANSI) {};

	inline IEnumSTATDATAA * GetANSI() const
		{ return (IEnumSTATDATAA *)m_pObj; };
};



//+--------------------------------------------------------------------------
//
//  Class:      CEnumFORMATETCW
//
//  Synopsis:   Class definition of IEnumFORMATETCA
//
//---------------------------------------------------------------------------
class CEnumFORMATETCW : CWideInterface
{
public:
	// *** IEnumFORMATETCA methods ***
	STDMETHOD(Next) (ULONG celt,
					   FORMATETC * rgelt,
					   ULONG FAR* pceltFetched);
	STDMETHOD(Skip) (ULONG celt);
	STDMETHOD(Reset) (VOID);
	STDMETHOD(Clone) (IEnumFORMATETC * * ppenm);

	inline CEnumFORMATETCW(LPUNKNOWN pUnk, IEnumFORMATETCA * pANSI) :
			CWideInterface(ID_IEnumFORMATETC, pUnk, (LPUNKNOWN)pANSI) {};

	inline IEnumFORMATETCA * GetANSI() const
		{ return (IEnumFORMATETCA *)m_pObj; };
};



//+--------------------------------------------------------------------------
//
//  Class:      CDataObjectW
//
//  Synopsis:   Class definition of IDataObjectW
//
//---------------------------------------------------------------------------
class CDataObjectW : CWideInterface
{
public:
	// *** IDataObject methods ***
	STDMETHOD(GetData) (LPFORMATETC pformatetcIn,
							LPSTGMEDIUM pmedium );
	STDMETHOD(GetDataHere) (LPFORMATETC pformatetc,
							LPSTGMEDIUM pmedium );
	STDMETHOD(QueryGetData) (LPFORMATETC pformatetc );
	STDMETHOD(GetCanonicalFormatEtc) (LPFORMATETC pformatetc,
							LPFORMATETC pformatetcOut);
	STDMETHOD(SetData) (LPFORMATETC pformatetc, STGMEDIUM FAR * pmedium,
							BOOL fRelease);
	STDMETHOD(EnumFormatEtc) (DWORD dwDirection,
							LPENUMFORMATETC FAR* ppenumFormatEtc);

	STDMETHOD(DAdvise) (FORMATETC FAR* pFormatetc, DWORD advf,
					LPADVISESINK pAdvSink, DWORD FAR* pdwConnection);
	STDMETHOD(DUnadvise) (DWORD dwConnection);
	STDMETHOD(EnumDAdvise) (LPENUMSTATDATA FAR* ppenumAdvise);

	inline CDataObjectW(LPUNKNOWN pUnk, IDataObjectA * pANSI) :
			CWideInterface(ID_IDataObject, pUnk, (LPUNKNOWN)pANSI) {};

	inline IDataObjectA * GetANSI() const
		{ return (IDataObjectA *)m_pObj; };
};



//+--------------------------------------------------------------------------
//
//  Class:      CViewObject2W
//
//  Synopsis:   Class definition of IViewObject2W
//
//---------------------------------------------------------------------------
class CViewObject2W : CWideInterface
{
public:
	// *** IViewObject methods ***
	STDMETHOD(Draw) (DWORD dwDrawAspect, LONG lindex,
					void FAR* pvAspect, DVTARGETDEVICE FAR * ptd,
					HDC hicTargetDev,
					HDC hdcDraw,
					LPCRECTL lprcBounds,
					LPCRECTL lprcWBounds,
					BOOL (CALLBACK * pfnContinue) (DWORD),
					DWORD dwContinue);

	STDMETHOD(GetColorSet) (DWORD dwDrawAspect, LONG lindex,
					void FAR* pvAspect, DVTARGETDEVICE FAR * ptd,
					HDC hicTargetDev,
					LPLOGPALETTE FAR* ppColorSet);

	STDMETHOD(Freeze)(DWORD dwDrawAspect, LONG lindex,
					void FAR* pvAspect,
					DWORD FAR* pdwFreeze);
	STDMETHOD(Unfreeze) (DWORD dwFreeze);
	STDMETHOD(SetAdvise) (DWORD aspects, DWORD advf,
					LPADVISESINK pAdvSink);
	STDMETHOD(GetAdvise) (DWORD FAR* pAspects, DWORD FAR* pAdvf,
					LPADVISESINK FAR* ppAdvSink);

	// *** IViewObject2 methods ***
	STDMETHOD(GetExtent) (DWORD dwDrawAspect, LONG lindex,
					DVTARGETDEVICE FAR * ptd, LPSIZEL lpsizel);

	inline CViewObject2W(LPUNKNOWN pUnk, IViewObject2A * pANSI) :
			CWideInterface(ID_IViewObject2, pUnk, (LPUNKNOWN)pANSI) {};

	inline IViewObject2A * GetANSI() const
		{ return (IViewObject2A *)m_pObj; };
};



//+--------------------------------------------------------------------------
//
//  Class:      CAdviseSink2W
//
//  Synopsis:   Class definition of IAdviseSink2W
//
//---------------------------------------------------------------------------
class CAdviseSink2W : CWideInterface
{
public:
	// *** IAdviseSink methods ***
	STDMETHOD_(void,OnDataChange)(FORMATETC * pFormatetc,
				STGMEDIUM * pStgmed);
	STDMETHOD_(void,OnViewChange)(DWORD dwAspect, LONG lindex);
	STDMETHOD_(void,OnRename)(LPMONIKER pmk);
	STDMETHOD_(void,OnSave)(VOID);
	STDMETHOD_(void,OnClose)(VOID);

	// *** IAdviseSink2 methods ***
	STDMETHOD_(void,OnLinkSrcChange)(LPMONIKER pmk);

	inline CAdviseSink2W(LPUNKNOWN pUnk, IAdviseSink2A * pANSI) :
			CWideInterface(ID_IAdviseSink2, pUnk, (LPUNKNOWN)pANSI) {};

	inline IAdviseSink2A * GetANSI() const
		{ return (IAdviseSink2A *)m_pObj; };
};



//+--------------------------------------------------------------------------
//
//  Class:      CDataAdviseHolderW
//
//  Synopsis:   Class definition of IDataAdviseHolderW
//
//---------------------------------------------------------------------------
class CDataAdviseHolderW : CWideInterface
{
public:
	// *** IDataAdviseHolder methods ***
	STDMETHOD(Advise)(LPDATAOBJECT pDataObject, FORMATETC * pFetc,
		DWORD advf, LPADVISESINK pAdvise, DWORD * pdwConnection);
	STDMETHOD(Unadvise)(DWORD dwConnection);
	STDMETHOD(EnumAdvise)(LPENUMSTATDATA * ppenumAdvise);

	STDMETHOD(SendOnDataChange)(LPDATAOBJECT pDataObject, DWORD dwReserved, DWORD advf);

	inline CDataAdviseHolderW(LPUNKNOWN pUnk, IDataAdviseHolderA * pANSI) :
			CWideInterface(ID_IDataAdviseHolder, pUnk, (LPUNKNOWN)pANSI) {};

	inline IDataAdviseHolderA * GetANSI() const
		{ return (IDataAdviseHolderA *)m_pObj; };
};



//+--------------------------------------------------------------------------
//
//  Class:      COleCache2W
//
//  Synopsis:   Class definition of IOleCache2W
//
//---------------------------------------------------------------------------
class COleCache2W : CWideInterface
{
public:
	// *** IOleCache methods ***
	STDMETHOD(Cache) (LPFORMATETC lpFormatetc, DWORD advf, LPDWORD lpdwConnection);
	STDMETHOD(Uncache) (DWORD dwConnection);
	STDMETHOD(EnumCache) (LPENUMSTATDATA FAR* ppenumStatData);
	STDMETHOD(InitCache) (LPDATAOBJECT pDataObject);
	STDMETHOD(SetData) (LPFORMATETC pformatetc, STGMEDIUM FAR * pmedium,
							BOOL fRelease);

	// *** IOleCache2 methods ***
	STDMETHOD(UpdateCache) (LPDATAOBJECT pDataObject, DWORD grfUpdf,
							LPVOID pReserved);
	STDMETHOD(DiscardCache) (DWORD dwDiscardOptions);

	inline COleCache2W(LPUNKNOWN pUnk, IOleCache2A * pANSI) :
			CWideInterface(ID_IOleCache2, pUnk, (LPUNKNOWN)pANSI) {};

	inline IOleCache2A * GetANSI() const
		{ return (IOleCache2A *)m_pObj; };
};



//+--------------------------------------------------------------------------
//
//  Class:      COleCacheControlW
//
//  Synopsis:   Class definition of IOleCacheControlW
//
//---------------------------------------------------------------------------
class COleCacheControlW : CWideInterface
{
public:
	// *** IDataObject methods ***
	STDMETHOD(OnRun) (LPDATAOBJECT pDataObject);
	STDMETHOD(OnStop) (VOID);

	inline COleCacheControlW(LPUNKNOWN pUnk, IOleCacheControlA * pANSI) :
			CWideInterface(ID_IOleCacheControl, pUnk, (LPUNKNOWN)pANSI) {};

	inline IOleCacheControlA * GetANSI() const
		{ return (IOleCacheControlA *)m_pObj; };
};

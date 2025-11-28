// FoxtlibCtl.cpp : Implementation of the CFoxtlibCtrl OLE control class.

#include "stdafx.h"
#include "foxtlib.h"
#include "FoxtlibCtl.h"
#include "FoxtlibPpg.h"
#include "pro_ext.h"
#include "malloc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAXPARMS 20	//max # of names for SetFuncAndParamNames (1 + # of parms)

#define VALUE_N_I(val)  (val.ev_type =='I' ? val.ev_long : (long)val.ev_real)

IMPLEMENT_DYNCREATE(CFoxtlibCtrl, COleControl)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CFoxtlibCtrl, COleControl)
	//{{AFX_MSG_MAP(CFoxtlibCtrl)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_EDIT, OnEdit)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CFoxtlibCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CFoxtlibCtrl)
	DISP_FUNCTION(CFoxtlibCtrl, "TLLoadTypeLib", TLLoadTypeLib, VT_I4, VTS_BSTR)
	DISP_FUNCTION(CFoxtlibCtrl, "TLRelease", TLRelease, VT_I4, VTS_I4)
	DISP_FUNCTION(CFoxtlibCtrl, "TLGetTypeInfoCount", TLGetTypeInfoCount, VT_I4, VTS_I4)
	DISP_FUNCTION(CFoxtlibCtrl, "TLGetTypeAttr", TLGetTypeAttr, VT_I4, VTS_I4 VTS_BSTR)
	DISP_FUNCTION(CFoxtlibCtrl, "TLGetTypeInfo", TLGetTypeInfo, VT_I4, VTS_I4 VTS_I4)
	DISP_FUNCTION(CFoxtlibCtrl, "TLGetDocumentation", TLGetDocumentation, VT_I4, VTS_I4 VTS_BSTR VTS_I4 VTS_I4)
	DISP_FUNCTION(CFoxtlibCtrl, "TIGetNames", TIGetNames, VT_I4, VTS_I4 VTS_BSTR VTS_I4)
	DISP_FUNCTION(CFoxtlibCtrl, "TIGetFuncDesc", TIGetFuncDesc, VT_I4, VTS_I4 VTS_BSTR VTS_I4 VTS_BSTR)
	DISP_FUNCTION(CFoxtlibCtrl, "TLWCreateTypeLib", TLWCreateTypeLib, VT_I4, VTS_BSTR VTS_PI4)
	DISP_FUNCTION(CFoxtlibCtrl, "TIGetVarDesc", TIGetVarDesc, VT_I4, VTS_I4 VTS_BSTR VTS_I4)
	DISP_FUNCTION(CFoxtlibCtrl, "TLWSaveAllChanges", TLWSaveAllChanges, VT_I4, VTS_I4)
	DISP_FUNCTION(CFoxtlibCtrl, "TIWCreateTypeInfo", TIWCreateTypeInfo, VT_I4, VTS_I4 VTS_I4 VTS_PI4 VTS_BSTR VTS_BSTR VTS_PI4)
	DISP_FUNCTION(CFoxtlibCtrl, "TLIWWriteDocumentation", TLIWWriteDocumentation, VT_I4, VTS_I4 VTS_BSTR VTS_BSTR VTS_I4 VTS_BSTR VTS_I4)
	DISP_FUNCTION(CFoxtlibCtrl, "TILayout", TILayout, VT_I4, VTS_I4)
	DISP_FUNCTION(CFoxtlibCtrl, "TIRelease", TIRelease, VT_I4, VTS_I4)
	DISP_FUNCTION(CFoxtlibCtrl, "TIAddFuncDesc", TIAddFuncDesc, VT_I4, VTS_I4 VTS_I4 VTS_BSTR VTS_BSTR VTS_BSTR VTS_I4)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CFoxtlibCtrl, COleControl)
	//{{AFX_EVENT_MAP(CFoxtlibCtrl)
	// NOTE - ClassWizard will add and remove event map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CFoxtlibCtrl, 1)
	PROPPAGEID(CFoxtlibPropPage::guid)
END_PROPPAGEIDS(CFoxtlibCtrl)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CFoxtlibCtrl, "FOXTLIB.FoxtlibCtrl.1",
	0x22852ee3, 0xb01b, 0x11cf, 0xb8, 0x26, 0, 0xa0, 0xc9, 0x5, 0x5d, 0x9e)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CFoxtlibCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DFoxtlib =
		{ 0x22852ee9, 0xb01b, 0x11cf, { 0xb8, 0x26, 0, 0xa0, 0xc9, 0x5, 0x5d, 0x9e } };
const IID BASED_CODE IID_DFoxtlibEvents =
		{ 0x22852eea, 0xb01b, 0x11cf, { 0xb8, 0x26, 0, 0xa0, 0xc9, 0x5, 0x5d, 0x9e } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwFoxtlibOleMisc =
	OLEMISC_INVISIBLEATRUNTIME |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CFoxtlibCtrl, IDS_FOXTLIB, _dwFoxtlibOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CFoxtlibCtrl::CFoxtlibCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CFoxtlibCtrl

BOOL CFoxtlibCtrl::CFoxtlibCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegInsertable | afxRegApartmentThreading to afxRegInsertable.

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_FOXTLIB,
			IDB_FOXTLIB,
			afxRegInsertable | afxRegApartmentThreading,
			_dwFoxtlibOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CFoxtlibCtrl::CFoxtlibCtrl - Constructor

CFoxtlibCtrl::CFoxtlibCtrl()
{
	InitializeIIDs(&IID_DFoxtlib, &IID_DFoxtlibEvents);
	if (!_OCXAPI(AfxGetInstanceHandle(),DLL_PROCESS_ATTACH))
	{
		::MessageBox(0,"This OCX can only be hosted by Visual Foxpro","",0);
		//Here you can do whatever you want when the host isn't VFP:
		// you might want to reject loading or you might want to set a property
		//saying that the host isn't VFP and the control will use other means
		// to achieve it's purpose.
	}
}


/////////////////////////////////////////////////////////////////////////////
// CFoxtlibCtrl::~CFoxtlibCtrl - Destructor

CFoxtlibCtrl::~CFoxtlibCtrl()
{
	_OCXAPI(AfxGetInstanceHandle(),DLL_PROCESS_DETACH);
}


/////////////////////////////////////////////////////////////////////////////
// CFoxtlibCtrl::OnDraw - Drawing function

void CFoxtlibCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	// TODO: Replace the following code with your own drawing code.
	pdc->FillRect(rcBounds, CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));
	pdc->Ellipse(rcBounds);
}


/////////////////////////////////////////////////////////////////////////////
// CFoxtlibCtrl::DoPropExchange - Persistence support

void CFoxtlibCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.

}


/////////////////////////////////////////////////////////////////////////////
// CFoxtlibCtrl::OnResetState - Reset control to default state

void CFoxtlibCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}


/////



#include "winnls.h"

void OLEFreeString(void **ppsz)
{
	HRESULT 	hr;
	IMalloc    	*pIMalloc;

	if (NULL != ppsz && NULL != *ppsz)
	{
		hr = CoGetMalloc(MEMCTX_TASK, &pIMalloc);

		if (FAILED(hr))
	    	return;
		
		pIMalloc->Free(*ppsz);

		pIMalloc->Release();
		
		*ppsz = NULL;
	}
	
	return;
}

HRESULT OLECopyAnsiToOle(const TEXT *pszA, OLECHAR *pszW, int cbW)
{
	ULONG	cchW;
	HRESULT	hr;


	hr = NOERROR;
	
	cchW = cbW / sizeof(OLECHAR);	// do this here in case cbW == 1 which is invalid
	
	if (!pszW || !cchW)
	{
		hr = ResultFromScode(E_INVALIDARG);
	}
	else
	{
		*pszW = '\0';
		
		if (pszA && *pszA)
		{
#if WIN32
			if (MultiByteToWideChar(CP_ACP, 0, (LPSTR)pszA, -1, pszW, cchW) == 0)
			{
				*pszW = '\0';
				hr = ResultFromScode(E_FAIL);
			}
#elif MAC_OS
			// NO MAC UNICODE YET
			strncpy(pszW, (char *) pszA, cchW);
#endif
		}
	}

	return (hr);
}


HRESULT OLEConvertStringAlloc(ULONG ulSize, void **ppv)
{
	HRESULT     hr;
	IMalloc    *pIMalloc;

	// Reject zero-length strings, because a valid, but empty, ptr. would get
	// allocated, which might confuse the caller.
	if ((ulSize == 0) || (ppv == NULL))
	{
		return (ResultFromScode(E_INVALIDARG));
	}

	*ppv = NULL;
	
	hr = CoGetMalloc(MEMCTX_TASK, &pIMalloc);

	if (!SUCCEEDED(hr))
	    return (hr);

	*ppv = pIMalloc->Alloc(ulSize);
	pIMalloc->Release();

	if (*ppv != NULL)
		memset(*ppv, 0, ulSize);

	return ((*ppv == NULL) ? ResultFromScode(E_OUTOFMEMORY) : ResultFromScode(NOERROR));
}



HRESULT OLEAnsiToOleString(const TEXT *pszA, OLECHAR **ppszW)
{
	ULONG   cch, 	// character count
			cb;		// byte count
	HRESULT hr;


	if (!ppszW)
		return (ResultFromScode(E_INVALIDARG));

	*ppszW = NULL;
	hr = NOERROR;

	if (pszA)
	{
#if WIN32
		cch = MultiByteToWideChar(CP_ACP, 0, (LPSTR)pszA, -1, NULL, 0);
#elif MAC_OS
		// NO MAC UNICODE YET
		cch = strlen((char *) pszA) + 1;
#endif

		cb = cch * sizeof(OLECHAR);
	}
		
	if (SUCCEEDED(hr = OLEConvertStringAlloc(cb, (void **)ppszW)))
	{
		hr = OLECopyAnsiToOle(pszA, *ppszW, cb);
		if (FAILED(hr))
			OLEFreeString((void **)ppszW);	// This will set *ppszW = NULL.
	}
	return (hr);
}


HRESULT OLECopyOleToAnsi(OLECHAR *pszW, TEXT *pszA, int cbA)
{
	HRESULT	hr = NOERROR;
	
	if (!pszA || !cbA)
	{
		hr = ResultFromScode(E_INVALIDARG);
	}
	else
	{
		*pszA = '\0';
		
		if (pszW && *pszW)
		{
#if WIN32
			if (WideCharToMultiByte(CP_ACP, 0, pszW, -1, (LPSTR)pszA, cbA, NULL, NULL) == 0)
			{
				*pszA = '\0';
				hr = ResultFromScode(E_FAIL);
			}
#elif MAC_OS
			// NO MAC UNICODE YET
			strncpy((char *) pszA, pszW, cbA);
#endif
		}
	}

	return (hr);
}

HRESULT OLEOleToAnsiString(OLECHAR *pszW, TEXT **ppszA)
{
	ULONG   cch, 	// character count
			cb;		// byte count
	HRESULT hr;


	if (!ppszA)
		return (ResultFromScode(E_INVALIDARG));

	*ppszA = NULL;
	hr = NOERROR;

	if (pszW)
	{
#if WIN32
		cch = WideCharToMultiByte(CP_ACP, 0, pszW, -1, NULL, 0, NULL, NULL);
#elif MAC_OS
		// NO MAC UNICODE YET
		cch = strlen(pszW) + 1;
#endif

		cb = cch;
		
		if (SUCCEEDED(hr = OLEConvertStringAlloc(cb, (void **)ppszA)))
		{
			hr = OLECopyOleToAnsi(pszW, *ppszA, cb);
			if (FAILED(hr))
				OLEFreeString((void **)ppszA);	// This will set *ppszA = NULL.
		}
	}
	
	return (hr);
}



void StoreBstr(Locator *ploc,int index, BSTR *lpBstr) {
	char *szBuff;
	Value val;
	OLEOleToAnsiString(*lpBstr,&szBuff);
	SysFreeString(*lpBstr);
	val.ev_type = 'C';
	ploc->l_sub1 = index;
	if (szBuff) {
		val.ev_length = strlen(szBuff);
		val.ev_handle = _AllocHand(val.ev_length);
		_HLock(val.ev_handle);
		_MemMove(_HandToPtr(val.ev_handle),szBuff,val.ev_length);
		_Store(ploc,&val);
		_HUnLock(val.ev_handle);
		_FreeHand(val.ev_handle);
	} else {
		val.ev_length = val.ev_handle = 0;
		_Store(ploc,&val);
	}
}



//Wrapper to work around a buffer allocation problem
NTI MyNameTableIndex(char *name) {
	char szBuff[256];
	strcpy(szBuff,name);
	return _NameTableIndex(szBuff);
}


/////////////////////////////////////////////////////////////////////////////
// CFoxtlibCtrl message handlers



long CFoxtlibCtrl::TLLoadTypeLib(LPCTSTR szFileName) 
{
	Value val;
	_Evaluate(&val,"recno()+5");
	_Execute("jj=eval('recno()')");
	OLECHAR *OFileName;
	HRESULT hr;
	ITypeLib * lptlib;
	OLEAnsiToOleString((TEXT *)szFileName,&OFileName);
	hr =  ::LoadTypeLib(OFileName, &lptlib);
	OLEFreeString((void **)&OFileName);
	if (SUCCEEDED(hr))
		return (long)lptlib;

	return 0;
}

long CFoxtlibCtrl::TLRelease(long pTypeInfo) 
{
	int nResult;
	__try {
		nResult = ((ITypeLib *)pTypeInfo)->Release();
	} __except  (EXCEPTION_EXECUTE_HANDLER) {

		nResult = 0;
	}
	return nResult;
}

long CFoxtlibCtrl::TLGetTypeInfoCount(long pTypeInfo) 
{
	int nResult;
	__try {
		nResult = ((ITypeLib *)pTypeInfo)->GetTypeInfoCount();
	} __except  (EXCEPTION_EXECUTE_HANDLER) {

		nResult = 0;
	}
	return nResult;
}

long CFoxtlibCtrl::TLGetTypeAttr(long pTypeInfo, LPCTSTR szArrName) 
{
	int nResult = 1;
	TYPEATTR *lpTypeAttr;
	Locator loc;
	Value val;
	OLECHAR szGuid[128];
	char *szBuff;
	__try {
		if (_FindVar(MyNameTableIndex((char *)szArrName),-1,&loc)) {
			((ITypeInfo *)pTypeInfo)->GetTypeAttr(&lpTypeAttr);
			if (_ALen(loc.l_NTI, AL_ELEMENTS) < 16)
			{
				throw(631); //"Array argument not of proper size.");
			}

			//1 = Guid
			StringFromGUID2(lpTypeAttr->guid,(LPOLESTR )&szGuid,sizeof(szGuid));
			OLEOleToAnsiString(szGuid,&szBuff);
			val.ev_type = 'C';
			val.ev_length = strlen(szBuff);
			val.ev_handle = _AllocHand(val.ev_length);
			_HLock(val.ev_handle);
			_MemMove((char *)_HandToPtr(val.ev_handle),szBuff,val.ev_length);
			OLEFreeString((void **)&szBuff);
			_HUnLock(val.ev_handle);

			loc.l_sub1 = 1;
			_Store(&loc,&val);
			_FreeHand(val.ev_handle);

			//2 = LCID
			loc.l_sub1 = 2;
			val.ev_type = 'I';
			val.ev_long = lpTypeAttr->lcid;
			_Store(&loc,&val);

			//3 dwReserved
			loc.l_sub1 = 3;
			val.ev_type = 'I';
			val.ev_long = lpTypeAttr->dwReserved;
			_Store(&loc,&val);

			//4 Constructor
			loc.l_sub1 = 4;
			val.ev_type = 'I';
			val.ev_long = lpTypeAttr->memidConstructor;
			_Store(&loc,&val);

			//5 Destructor
			loc.l_sub1 = 5;
			val.ev_type = 'I';
			val.ev_long = lpTypeAttr->memidDestructor;
			_Store(&loc,&val);

			//6 lpstrSchema reserved
			loc.l_sub1 = 6;
			val.ev_type = 'I';
			val.ev_long = (int)lpTypeAttr->lpstrSchema;
			_Store(&loc,&val);

			//7 size Instance
			loc.l_sub1 = 7;
			val.ev_type = 'I';
			val.ev_long = lpTypeAttr->cbSizeInstance;
			_Store(&loc,&val);

			//8 TypeKind
			loc.l_sub1 = 8;
			val.ev_type = 'I';
			val.ev_long = lpTypeAttr->typekind;
			_Store(&loc,&val);

			//9 cFuncs
			loc.l_sub1 = 9;
			val.ev_type = 'I';
			val.ev_long = lpTypeAttr->cFuncs;
			_Store(&loc,&val);

			//10 cVars
			loc.l_sub1 = 10;
			val.ev_type = 'I';
			val.ev_long = lpTypeAttr->cVars;
			_Store(&loc,&val);

			//11 cImplTypes
			loc.l_sub1 = 11;
			val.ev_type = 'I';
			val.ev_long = lpTypeAttr->cImplTypes;
			_Store(&loc,&val);

			//12 cbSizeVft
			loc.l_sub1 = 12;
			val.ev_type = 'I';
			val.ev_long = lpTypeAttr->cbSizeVft;
			_Store(&loc,&val);

			//13 cbAlignment
			loc.l_sub1 = 13;
			val.ev_type = 'I';
			val.ev_long = lpTypeAttr->cbAlignment;
			_Store(&loc,&val);

			//14 wTypeFlags
			loc.l_sub1 = 14;
			val.ev_type = 'I';
			val.ev_long = lpTypeAttr->wTypeFlags;
			_Store(&loc,&val);

			//15 wMajorVerNum
			loc.l_sub1 = 15;
			val.ev_type = 'I';
			val.ev_long = lpTypeAttr->wMajorVerNum;
			_Store(&loc,&val);

			//16 wMinorVerNum
			loc.l_sub1 = 16;
			val.ev_type = 'I';
			val.ev_long = lpTypeAttr->wMinorVerNum;
			_Store(&loc,&val);

			((ITypeInfo *)pTypeInfo)->ReleaseTypeAttr(lpTypeAttr);

		}
	} __except  (EXCEPTION_EXECUTE_HANDLER) {
		nResult = 0;
	}
	return nResult;
}

long CFoxtlibCtrl::TLGetTypeInfo(long pTypeInfo,long nIndex) 
{
	ITypeInfo * lpTypeInfo;
	int nResult;
	__try {
		nResult = ((ITypeLib *)pTypeInfo)->GetTypeInfo(nIndex,&lpTypeInfo);
		if (SUCCEEDED(nResult))
			nResult = (int) lpTypeInfo;

	} __except  (EXCEPTION_EXECUTE_HANDLER) {
		nResult = 0;
	}
	return nResult;
}
//				=GetDocumentation(m.ITypeInfo,@ta,m.j)

long CFoxtlibCtrl::TLGetDocumentation(long pTypeInfo, LPCTSTR szArrName, long nIndex,long nKind) 
{
	//Kind = 0 for TypeLib, 1 for TypeInfo
	int nResult = 1;
	BSTR bstrName;
	BSTR bstrDocString;
	unsigned long dwHelpContext;
	BSTR bstrHelpFile;
	Locator loc;
	Value val;
	__try {
		if (_FindVar(MyNameTableIndex((char *)szArrName),-1,&loc)) {
			if (nKind)
			{
				((ITypeInfo *)pTypeInfo)->GetDocumentation(nIndex ,&bstrName,&bstrDocString,
					&dwHelpContext,&bstrHelpFile);
			} else {
				((ITypeLib *)pTypeInfo)->GetDocumentation(nIndex ,&bstrName,&bstrDocString,
					&dwHelpContext,&bstrHelpFile);
			}
			if (_ALen(loc.l_NTI, AL_ELEMENTS) < 4)
			{
				throw(631); //"Array argument not of proper size.");
			}

			StoreBstr(&loc,1,&bstrName);
			StoreBstr(&loc,2,&bstrDocString);
			StoreBstr(&loc,3,&bstrHelpFile);

			loc.l_sub1 = 4;
			val.ev_type = 'I';
			val.ev_long = dwHelpContext;
			_Store(&loc,&val);
		}

	} __except  (EXCEPTION_EXECUTE_HANDLER) {
		nResult = 0;
	}
	return nResult;
}

long CFoxtlibCtrl::TIGetNames(long pTypeInfo, LPCTSTR szArrName, long nMemId) 
{
	int nResult = 1;
	Locator loc;
	__try {
		if (_FindVar(MyNameTableIndex((char *)szArrName),-1,&loc)) {
			BSTR rgbstrNames[MAXPARMS];
			int cNames = 0;

			((ITypeInfo *)pTypeInfo)->GetNames(nMemId,rgbstrNames,MAXPARMS,(UINT *)&cNames);
			if (cNames)
			{

				if (_ALen(loc.l_NTI, AL_ELEMENTS) < cNames)
				{
					throw(631); //"Array argument not of proper size.");
				}
				for (int i=1 ; i <= cNames ; i++)	{	//index through Fox array
					StoreBstr(&loc, i, &rgbstrNames[i-1]);
				}
			}
			nResult = cNames;
		}

	} __except  (EXCEPTION_EXECUTE_HANDLER) {
		nResult = 0;
	}
	return nResult;
}

long CFoxtlibCtrl::TIGetFuncDesc(long pTypeInfo, LPCTSTR szArrName, long nIndex, LPCTSTR szParmsArr) 
{
	int nResult = 1;
	Locator loc,ploc;
	Value val;
	__try {
		if (_FindVar(MyNameTableIndex((char *)szArrName),-1,&loc)) {
			FUNCDESC * lpFuncDesc;

			if (_ALen(loc.l_NTI, AL_ELEMENTS) < 5)
			{
				throw(631); //"Array argument not of proper size.");
			}
			((ITypeInfo *)pTypeInfo)->GetFuncDesc(nIndex,&lpFuncDesc);

			//1 = memid
			loc.l_sub1 = 1;
			val.ev_type = 'I';
			val.ev_long = lpFuncDesc->memid;
			_Store(&loc,&val);

			if (_FindVar(MyNameTableIndex((char *)szParmsArr),-1,&ploc)) {
				int nelem = lpFuncDesc->cParams + lpFuncDesc->cParamsOpt;
				if (_ALen(loc.l_NTI, AL_ELEMENTS) < nelem)
				{
					throw(631); //"Array argument not of proper size.");
				}
				ploc.l_sub1 = 1;
				val.ev_type = 'I';
				val.ev_long = (long) lpFuncDesc->elemdescFunc.tdesc.vt;
				_Store(&ploc,&val);
				for (int i = 0 ; i < nelem ; i++) {
					ploc.l_sub1 = 2 + i;		//start parms at xbase array element #2
					val.ev_type = 'I';
					val.ev_long = (long)lpFuncDesc->lprgelemdescParam[i].tdesc.vt;
					_Store(&ploc,&val);

				}


			}

			//2 = FUNCKIND
			loc.l_sub1 = 2;
			val.ev_type = 'I';
			val.ev_long = lpFuncDesc->funckind;
			_Store(&loc,&val);

			//3 = INVOKEKIND
			loc.l_sub1 = 3;
			val.ev_type = 'I';
			val.ev_long = lpFuncDesc->invkind;
			_Store(&loc,&val);


			//4 = CALLCONV
			loc.l_sub1 = 4;
			val.ev_type = 'I';
			val.ev_long = lpFuncDesc->callconv;
			_Store(&loc,&val);

			//5 = cParams
			loc.l_sub1 = 5;
			val.ev_type = 'I';
			val.ev_long = lpFuncDesc->cParams;
			_Store(&loc,&val);

			//6 = cParamsOpt
			loc.l_sub1 = 6;
			val.ev_type = 'I';
			val.ev_long = lpFuncDesc->cParamsOpt;
			_Store(&loc,&val);

			//7 = oVft;
			loc.l_sub1 = 7;
			val.ev_type = 'I';
			val.ev_long = lpFuncDesc->oVft;
			_Store(&loc,&val);

			//8 = cScodes
			loc.l_sub1 = 8;
			val.ev_type = 'I';
			val.ev_long = lpFuncDesc->cScodes;
			_Store(&loc,&val);


			//9 = wFuncFlags
			loc.l_sub1 = 9;
			val.ev_type = 'I';
			val.ev_long = lpFuncDesc->wFuncFlags;
			_Store(&loc,&val);


			((ITypeInfo *)pTypeInfo)->ReleaseFuncDesc(lpFuncDesc);
		}

	} __except  (EXCEPTION_EXECUTE_HANDLER) {
		nResult = 0;
	}
	return nResult;
}

long ProcDesc(TYPEDESC *td) {
	long lVal = 0;
	switch(td->vt) {
	case VT_PTR:
	case VT_SAFEARRAY:
		lVal = ProcDesc(td->lptdesc);
		break;
	case VT_CARRAY:
		break;
	case VT_USERDEFINED:
		lVal= td->hreftype;
		break;
	default:
		break;
	}
	return lVal;
}

long CFoxtlibCtrl::TIGetVarDesc(long pTypeInfo, LPCTSTR szArrName, long nIndex) 
{
	int nResult = 1;
	Locator loc;
	Value val;
	__try {
		if (_FindVar(MyNameTableIndex((char *)szArrName),-1,&loc)) {
			VARDESC * lpVarDesc;

			if (_ALen(loc.l_NTI, AL_ELEMENTS) < 5)
			{
				throw(631); //"Array argument not of proper size.");
			}
			((ITypeInfo *)pTypeInfo)->GetVarDesc(nIndex,&lpVarDesc);

			//1 = memid
			loc.l_sub1 = 1;
			val.ev_type = 'I';
			val.ev_long = lpVarDesc->memid;
			_Store(&loc,&val);

			//2 = wVarFlags
			loc.l_sub1 = 2;
			val.ev_type = 'I';
			val.ev_long = (long) lpVarDesc->wVarFlags;
			_Store(&loc,&val);

			//3 = Var kind
			loc.l_sub1 = 3;
			val.ev_type = 'I';
			val.ev_long = (long) lpVarDesc->varkind;
			_Store(&loc,&val);

			//4 = Var type
			loc.l_sub1 = 4;
			val.ev_type = 'I';
			val.ev_long = (long) lpVarDesc->elemdescVar.tdesc.vt;
			_Store(&loc,&val);

			//5 = wIDLFlags
			loc.l_sub1 = 5;
			val.ev_type = 'I';
			val.ev_long = (long) lpVarDesc->elemdescVar.idldesc.wIDLFlags;
			_Store(&loc,&val);

			//6 = dwReserved
			loc.l_sub1 = 6;
			val.ev_type = 'I';
			val.ev_long = (long) lpVarDesc->elemdescVar.idldesc.dwReserved;
			_Store(&loc,&val);

			//7,8 = mixture
			loc.l_sub1 = 7;
			val.ev_type = 'I';

			switch(lpVarDesc->varkind) {
			case VAR_CONST:
				switch(lpVarDesc->lpvarValue->vt) {
				case VT_I4:
					val.ev_long = (long) lpVarDesc->lpvarValue->lVal;
					break;
				case VT_BOOL:
					val.ev_long = (long) lpVarDesc->lpvarValue->boolVal;
					break;
//				case VT_BSTR:
//bugbug					StoreBstr(&loc,7,&lpVarDesc->lpvarValue->bstrVal);
//					break;
				}
				break;
			case VAR_PERINSTANCE:
				val.ev_long = (long) lpVarDesc->oInst;
				break;
			default:
				val.ev_long = lpVarDesc->elemdescVar.tdesc.lptdesc->vt;
				_Store(&loc,&val);

				loc.l_sub1 = 8;
				val.ev_type = 'I';
				val.ev_long = ProcDesc(&lpVarDesc->elemdescVar.tdesc);
				break;
			}
			_Store(&loc,&val);

			((ITypeInfo *)pTypeInfo)->ReleaseVarDesc(lpVarDesc);
		}

	} __except  (EXCEPTION_EXECUTE_HANDLER) {
		nResult = 0;
	}
	return nResult;
}

//*********************************************
//*********************************************
//*********************************************
//Create Typelib stuff
//*********************************************
//*********************************************
//*********************************************
long CFoxtlibCtrl::TLWCreateTypeLib(LPCTSTR szTLBName,long *res)
{
	HRESULT hr;
	LPCREATETYPELIB lpCreateTypeLib;
	OLECHAR *wTLBName;
	__try {
		*res = 0;
		OLEAnsiToOleString(szTLBName,&wTLBName);
		if ((hr = CreateTypeLib(SYS_WIN32,wTLBName,&lpCreateTypeLib)) == S_OK) {
			*res = (long)lpCreateTypeLib;
		}
		OLEFreeString((void **)&wTLBName);
	} __except  (EXCEPTION_EXECUTE_HANDLER) {
		hr = E_FAIL;
	}
	return hr;
}


long CFoxtlibCtrl::TLWSaveAllChanges(long lpCreateTypeLib) //returns 0 on success
{
	HRESULT hr = E_FAIL;
	__try {
		hr = ((ICreateTypeLib *)lpCreateTypeLib)->SaveAllChanges();
		((ICreateTypeLib *)lpCreateTypeLib)->Release();
	} __except  (EXCEPTION_EXECUTE_HANDLER) {
		hr = E_FAIL;
	}
	return hr;
}

/*HRESULT ICreateTypeInfo::AddRefTypeInfo(lptinfo, lphreftype)
ITypeInfo FAR*  lptinfo
HREFTYPE FAR*  lphreftype

The second parameter returns a pointer to the handle of the added type information. 
If AddRefTypeInfo has been called previously for the same type information, the index 
that was returned by the previous call is returned in lphreftype. If the referenced type 
description is in the type library being created, its type information can be obtained by
 calling IUnknown::QueryInterface(IID_ITypeInfo, ...) on the ICreateTypeInfo interface of that type description.
  
*/
HRESULT AddImplType(LPCREATETYPEINFO lpCreateTypeInfo, REFIID riid,LPTYPEINFO lpTypeInfo)
{
	HRESULT		hresult = E_FAIL;
	char		szFile[MAX_PATH];
	OLECHAR		*pOle;
	LPTYPELIB	pITypeLib;
	LPTYPEINFO	lpIDispatchTypeInfo;
	HREFTYPE	hreftype;


	if ((hresult = lpCreateTypeInfo->AddRefTypeInfo(lpTypeInfo, &hreftype)) == NOERROR)
	{
		hresult = lpCreateTypeInfo->AddImplType(0, hreftype);
	}
	lpTypeInfo->Release();
	return hresult;




	if ((riid == IID_IDispatch || riid == IID_IUnknown ) && GetSystemDirectory(szFile,sizeof(szFile)) > 0)
	{
	 	strcat(szFile, "\\stdole32.tlb");
	}
	else
	{
	 	ASSERT(0);
	 	return 1;
	}

	if ((hresult = OLEAnsiToOleString(szFile, &pOle)) == NOERROR)
	{
		if ((hresult = LoadTypeLib(pOle, &pITypeLib)) == NOERROR)
		{
			hresult = pITypeLib->GetTypeInfoOfGuid(riid, &lpIDispatchTypeInfo);
			pITypeLib->Release();
		}
		OLEFreeString((void **)&pOle);

		if (hresult == NOERROR)
		{
			if ((hresult = lpCreateTypeInfo->AddRefTypeInfo(lpIDispatchTypeInfo, &hreftype)) == NOERROR)
			{
				hresult = lpCreateTypeInfo->AddImplType(0, hreftype);
			}
			lpIDispatchTypeInfo->Release();
		}
	}

	return hresult;

}


long CFoxtlibCtrl::TIWCreateTypeInfo(long lpCreateTypeLib, long TypeKind,long * res, LPCTSTR szArrName,LPCTSTR szTypeInfoName,long *lpTypeInfo) 
{
	LPCREATETYPEINFO lpCreateTypeInfo;
	int nResult = 1;
	Locator loc;
	Value val;
	HRESULT hr;
	OLECHAR *pOle;
	char szBuff[500];
	GUID myguid;
	__try {
		if (_FindVar(MyNameTableIndex((char *)szArrName),-1,&loc)) {
			if (_ALen(loc.l_NTI, AL_ELEMENTS) < 16)
			{
				throw(631); //"Array argument not of proper size.");
			}
			OLEAnsiToOleString(szTypeInfoName,&pOle);
			hr = ((ICreateTypeLib *)lpCreateTypeLib)->CreateTypeInfo(pOle,(TYPEKIND)TypeKind, (ICreateTypeInfo **)&lpCreateTypeInfo);
			OLEFreeString((void **)&pOle);
			if (hr == TYPE_E_NAMECONFLICT) 
				return hr;
			ASSERT(lpCreateTypeInfo);
			*res = (long) lpCreateTypeInfo;


/*
The second parameter returns a pointer to the handle of the added type information. 
If AddRefTypeInfo has been called previously for the same type information, the index 
that was returned by the previous call is returned in lphreftype. If the referenced type 
description is in the type library being created, its type information can be obtained by
 calling IUnknown::QueryInterface(IID_ITypeInfo, ...) on the ICreateTypeInfo interface of that type description.
*/
			if (TKIND_DISPATCH == TypeKind) {
				LPDISPATCH lDisp;
				((ICreateTypeInfo *)lpCreateTypeInfo)->QueryInterface(IID_ITypeInfo,(void **)&lDisp);
				*lpTypeInfo = (long)lDisp;
			}


			//17 = Guid
			loc.l_sub1 = 17;
			_Load(&loc,&val);
			ASSERT(val.ev_type == 'C');
			_HLock(val.ev_handle);
			_MemMove(szBuff,(char *)_HandToPtr(val.ev_handle),val.ev_length);
			szBuff[val.ev_length] = '\0';
			_HUnLock(val.ev_handle);
			_FreeHand(val.ev_handle);
			OLEAnsiToOleString(szBuff,&pOle);
			CLSIDFromString(pOle,&myguid);
			OLEFreeString((void **)&pOle);
			((ICreateTypeInfo *)lpCreateTypeInfo)->SetGuid(myguid);

			//13 cbAlignment
			loc.l_sub1 = 13;
			_Load(&loc,&val);
			ASSERT(val.ev_type == 'I' || val.ev_type == 'N');
			((ICreateTypeInfo *)lpCreateTypeInfo)->SetAlignment((unsigned short)VALUE_N_I(val));

			//14 wTypeFlags
			loc.l_sub1 = 14;
			_Load(&loc,&val);
			ASSERT(val.ev_type == 'I' || val.ev_type == 'N');
			((ICreateTypeInfo *)lpCreateTypeInfo)->SetTypeFlags(VALUE_N_I(val));
			if (TypeKind == TKIND_COCLASS) {
//			if (VALUE_N_I(val) & TYPEFLAG_FCANCREATE) {
				AddImplType((ICreateTypeInfo *)lpCreateTypeInfo,IID_IDispatch,(LPTYPEINFO)*lpTypeInfo);
			}




			//15 wMajorVerNum
			loc.l_sub1 = 15;
			_Load(&loc,&val);
			ASSERT(val.ev_type == 'I' || val.ev_type == 'N');
			int wMajorVerNum =(int)VALUE_N_I(val);

			//16 wMinorVerNum
			loc.l_sub1 = 16;
			_Load(&loc,&val);
			ASSERT(val.ev_type == 'I' || val.ev_type == 'N');
			((ICreateTypeInfo *)lpCreateTypeInfo)->SetVersion(wMajorVerNum,(unsigned short)VALUE_N_I(val));


/*
			//2 = LCID
			loc.l_sub1 = 2;
			((ICreateTypeInfo *)lpTypeInfo)->SetGuid(myguid);
			val.ev_long = lpTypeAttr->lcid;

			//3 dwReserved
			loc.l_sub1 = 3;
			val.ev_type = 'I';
			val.ev_long = lpTypeAttr->dwReserved;
			_Store(&loc,&val);

			//4 Constructor
			loc.l_sub1 = 4;
			val.ev_type = 'I';
			val.ev_long = lpTypeAttr->memidConstructor;
			_Store(&loc,&val);

			//5 Destructor
			loc.l_sub1 = 5;
			val.ev_type = 'I';
			val.ev_long = lpTypeAttr->memidDestructor;
			_Store(&loc,&val);

			//6 lpstrSchema reserved
			loc.l_sub1 = 6;
			val.ev_type = 'I';
			val.ev_long = (int)lpTypeAttr->lpstrSchema;
			_Store(&loc,&val);

			//7 size Instance
			loc.l_sub1 = 7;
			val.ev_type = 'I';
			val.ev_long = lpTypeAttr->cbSizeInstance;
			_Store(&loc,&val);

			//8 TypeKind
			loc.l_sub1 = 8;
			val.ev_type = 'I';
			val.ev_long = lpTypeAttr->typekind;
			_Store(&loc,&val);

			//9 cFuncs
			loc.l_sub1 = 9;
			val.ev_type = 'I';
			val.ev_long = lpTypeAttr->cFuncs;
			_Store(&loc,&val);

			//10 cVars
			loc.l_sub1 = 10;
			val.ev_type = 'I';
			val.ev_long = lpTypeAttr->cVars;
			_Store(&loc,&val);

			//11 cImplTypes
			loc.l_sub1 = 11;
			val.ev_type = 'I';
			val.ev_long = lpTypeAttr->cImplTypes;
			_Store(&loc,&val);

			//12 cbSizeVft
			loc.l_sub1 = 12;
			val.ev_type = 'I';
			val.ev_long = lpTypeAttr->cbSizeVft;
			_Store(&loc,&val);

*/
		}
	} __except  (EXCEPTION_EXECUTE_HANDLER) {
		nResult = 0;
	}
	return nResult;
}


//This func has 2 modes:
//If name is not empty, then pInterface is LPCREATETYPELIB
// else   pInterface is LPCREATETYPEINFO
long CFoxtlibCtrl::TLIWWriteDocumentation(long pInterface, LPCTSTR Name, LPCTSTR DocString, long HelpContext, LPCTSTR HelpFile, long nIndex) 
{
	HRESULT hr = E_FAIL;
	OLECHAR *pOle;
	__try {
		if (*Name) {
			if (*Name == '@') {
				OLEAnsiToOleString(DocString,&pOle);
				((LPCREATETYPEINFO)pInterface)->SetFuncDocString(nIndex,pOle);
				OLEFreeString((void **)&pOle);

				((LPCREATETYPEINFO)pInterface)->SetFuncHelpContext(nIndex,HelpContext);

			} else {
				OLEAnsiToOleString(Name,&pOle);
				((LPCREATETYPELIB)pInterface)->SetName(pOle);
				OLEFreeString((void **)&pOle);

				OLEAnsiToOleString(DocString,&pOle);
				((LPCREATETYPELIB)pInterface)->SetDocString(pOle);
				OLEFreeString((void **)&pOle);

				((LPCREATETYPELIB)pInterface)->SetHelpContext(HelpContext);

			}
		} else {
			OLEAnsiToOleString(DocString,&pOle);
			((LPCREATETYPEINFO)pInterface)->SetDocString(pOle);
			OLEFreeString((void **)&pOle);
		}
	} __except  (EXCEPTION_EXECUTE_HANDLER) {
		hr = E_FAIL;
	}
	return hr;
	

}

long CFoxtlibCtrl::TILayout(long lpCreateTypeInfo)	// must be called to finish up writing a typeinfo
{
	HRESULT hr;
	__try {
		hr = ((LPCREATETYPEINFO)lpCreateTypeInfo)->LayOut();
	} __except  (EXCEPTION_EXECUTE_HANDLER) {
		hr = E_FAIL;
	}
	return hr;
}

long CFoxtlibCtrl::TIRelease(long lpTypeInfo) 
{
	HRESULT hr;
	__try {
		hr = ((LPTYPEINFO)lpTypeInfo)->Release();
	} __except  (EXCEPTION_EXECUTE_HANDLER) {
		hr = E_FAIL;
	}
	return hr;
}

long CFoxtlibCtrl::TIAddFuncDesc(long lpCreateTypeInfo, long nIndex, LPCTSTR szArrName, LPCTSTR szParmsArr, LPCTSTR szNamesArr,long nNames) 
{
	HRESULT hr;
	int i;
	FUNCDESC FuncDesc;
	ELEMDESC	edesc[1];
	Locator loc,locNames,locParms;
	Value val;
	int nParams;
	char szBuff[255];
	OLECHAR * pOleArr[MAXPARMS];
	__try {
		if (_FindVar(MyNameTableIndex((char *)szArrName),-1,&loc)) {
			if (_ALen(loc.l_NTI, AL_ELEMENTS) < 16)
			{
				throw(631); //"Array argument not of proper size.");
			}
			memset(&FuncDesc,0,sizeof(FuncDesc));

			//1 = Memid
			loc.l_sub1 = 1;
			_Load(&loc,&val);
			ASSERT(val.ev_type == 'I' || val.ev_type == 'N');
			FuncDesc.memid = (int)VALUE_N_I(val);

			//2 = FUNCKIND
			loc.l_sub1 = 2;
			_Load(&loc,&val);
			ASSERT(val.ev_type == 'I' || val.ev_type == 'N');
			FuncDesc.funckind = (FUNCKIND)VALUE_N_I(val);

			//3 = INVOKEKIND
			loc.l_sub1 = 3;
			_Load(&loc,&val);
			ASSERT(val.ev_type == 'I' || val.ev_type == 'N');
			FuncDesc.invkind = (INVOKEKIND)VALUE_N_I(val);


			//4 = CALLCONV
			loc.l_sub1 = 4;
			_Load(&loc,&val);
			ASSERT(val.ev_type == 'I' || val.ev_type == 'N');
			FuncDesc.callconv = (CALLCONV)VALUE_N_I(val);

			//5 = cParams
			loc.l_sub1 = 5;
			_Load(&loc,&val);
			ASSERT(val.ev_type == 'I' || val.ev_type == 'N');
			FuncDesc.cParams = (short) VALUE_N_I(val);

			//6 = cParamsOpt
			loc.l_sub1 = 6;
			_Load(&loc,&val);
			ASSERT(val.ev_type == 'I' || val.ev_type == 'N');
			FuncDesc.cParamsOpt = (short)VALUE_N_I(val);
			nParams = FuncDesc.cParams + FuncDesc.cParamsOpt;

			//8 = cScodes
			loc.l_sub1 = 8;
			_Load(&loc,&val);
			ASSERT(val.ev_type == 'I' || val.ev_type == 'N');
			FuncDesc.cScodes = (SHORT)VALUE_N_I(val);

			//9 = wFuncFlags
			loc.l_sub1 = 9;
			_Load(&loc,&val);
			FuncDesc.wFuncFlags = (WORD) VALUE_N_I(val);

			if (!_FindVar(MyNameTableIndex((char *)szParmsArr),-1,&locParms)) {
				throw 631;
			}

			//get the type of the return value
			locParms.l_sub1 = 1;
			_Load(&locParms,&val);
			ASSERT(val.ev_type == 'I' || val.ev_type == 'N');
			switch (FuncDesc.invkind)
			{
			case INVOKE_PROPERTYGET:
				// return type
				FuncDesc.elemdescFunc.tdesc.vt = (VARTYPE) VALUE_N_I(val);
				break;
				
			case INVOKE_FUNC:
			case INVOKE_PROPERTYPUT:
				// 'Parameter info'
				edesc[0].tdesc.vt = (VARTYPE) VALUE_N_I(val);
				FuncDesc.lprgelemdescParam = (ELEMDESC *)edesc;

				// return type
				FuncDesc.elemdescFunc.tdesc.vt = VT_VOID;
				break;
				

			case INVOKE_PROPERTYPUTREF:
				break;
			default:
				ASSERT(0);
			}
			if (nParams) {
				FuncDesc.lprgelemdescParam = (LPELEMDESC)_alloca((FuncDesc.cParams +1)* sizeof(ELEMDESC));
				memset(FuncDesc.lprgelemdescParam,0,FuncDesc.cParams * sizeof(FuncDesc));
				for (int i =0 ; i< nParams ; i++) {
					locParms.l_sub1 = i + 2;
					_Load(&locParms,&val);
					ASSERT(val.ev_type == 'I' || val.ev_type == 'N');
					FuncDesc.lprgelemdescParam[i].tdesc.vt = (VARTYPE)VALUE_N_I(val);
				}
			}
			hr = ((LPCREATETYPEINFO)lpCreateTypeInfo)->AddFuncDesc(nIndex,&FuncDesc);


			if (!_FindVar(MyNameTableIndex((char *)szNamesArr),-1,&locNames)) {
				throw(631);
			}
			if (_ALen(locNames.l_NTI, AL_ELEMENTS) < MAXPARMS)
			{
				throw(631); //"Array argument not of proper size.");
			}
			for (i = 0 ; i < nNames ; i++) {
				locNames.l_sub1 = i+1;
				_Load(&locNames,&val);
				ASSERT(val.ev_type == 'C');
				_HLock(val.ev_handle);
				_MemMove(szBuff,_HandToPtr(val.ev_handle),val.ev_length);
				_HUnLock(val.ev_handle);
				_FreeHand(val.ev_handle);
				szBuff[val.ev_length] = '\0';

				OLEAnsiToOleString(szBuff,&pOleArr[i]);
			}



			hr = ((LPCREATETYPEINFO)lpCreateTypeInfo)->SetFuncAndParamNames(nIndex,pOleArr,nNames);

			for (i = 0 ; i < nNames ; i++) {
				OLEFreeString((void **)&pOleArr[i]);
			}



/*

			if (_FindVar(MyNameTableIndex((char *)szParmsArr),-1,&ploc)) {
				int nelem = lpFuncDesc->cParams + lpFuncDesc->cParamsOpt;
				if (_ALen(loc.l_NTI, AL_ELEMENTS) < nelem)
				{
					throw(631); //"Array argument not of proper size.");
				}
				ploc.l_sub1 = 1;
				val.ev_type = 'I';
				val.ev_long = (long) lpFuncDesc->elemdescFunc.tdesc.vt;
				_Store(&ploc,&val);
				for (int i = 0 ; i < nelem ; i++) {
					ploc.l_sub1 = 2 + i;		//start parms at xbase array element #2
					val.ev_type = 'I';
					val.ev_long = (long)lpFuncDesc->lprgelemdescParam[i].tdesc.vt;
					_Store(&ploc,&val);

				}


			}


			//7 = oVft;
			loc.l_sub1 = 7;
			val.ev_type = 'I';
			val.ev_long = lpFuncDesc->oVft;
			_Store(&loc,&val);

			//8 = cScodes
			loc.l_sub1 = 8;
			val.ev_type = 'I';
			val.ev_long = lpFuncDesc->cScodes;
			_Store(&loc,&val);


			//9 = wFuncFlags
			loc.l_sub1 = 9;
			val.ev_type = 'I';
			val.ev_long = lpFuncDesc->wFuncFlags;
			_Store(&loc,&val);


			((ITypeInfo *)pTypeInfo)->ReleaseFuncDesc(lpFuncDesc);
*/
		
		}
	} __except  (EXCEPTION_EXECUTE_HANDLER) {
		hr = E_FAIL;
	}
	return hr;
}

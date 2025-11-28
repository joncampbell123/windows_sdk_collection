// FoxtlibCtl.h : Declaration of the CFoxtlibCtrl OLE control class.

/////////////////////////////////////////////////////////////////////////////
// CFoxtlibCtrl : See FoxtlibCtl.cpp for implementation.

class CFoxtlibCtrl : public COleControl
{
	DECLARE_DYNCREATE(CFoxtlibCtrl)

// Constructor
public:
	CFoxtlibCtrl();

// Overrides

	// Drawing function
	virtual void OnDraw(
				CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);

	// Persistence
	virtual void DoPropExchange(CPropExchange* pPX);

	// Reset control state
	virtual void OnResetState();

// Implementation
protected:
	~CFoxtlibCtrl();

	DECLARE_OLECREATE_EX(CFoxtlibCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CFoxtlibCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CFoxtlibCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CFoxtlibCtrl)		// Type name and misc status

// Message maps
	//{{AFX_MSG(CFoxtlibCtrl)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CFoxtlibCtrl)
	afx_msg long TLLoadTypeLib(LPCTSTR szFilename);
	afx_msg long TLRelease(long pTypeInfo);
	afx_msg long TLGetTypeInfoCount(long pTypeInfo);
	afx_msg long TLGetTypeAttr(long pTypeInfo, LPCTSTR szArrName);
	afx_msg long TLGetTypeInfo(long pTypeInfo, long nIndex);
	afx_msg long TLGetDocumentation(long pTypeInfo, LPCTSTR szArrName, long nIndex, long nKind);
	afx_msg long TIGetNames(long pTypeInfo, LPCTSTR szArrName, long nMemId);
	afx_msg long TIGetFuncDesc(long pTypeInfo, LPCTSTR szArrName, long nIndex, LPCTSTR szParmsArr);
	afx_msg long test(VARIANT FAR* p1);
	afx_msg long TIGetVarDesc(long pTypeInfo, LPCTSTR szArrName, long nIndex);
	afx_msg long TLWCreateTypeLib(LPCTSTR szTLBName,long *res);
	afx_msg long TLWSaveAllChanges(long lpCreateTypeLib);
	afx_msg long TIWCreateTypeInfo(long lpCreateTypeLib, long TypeKind,long * res, LPCTSTR szArrName,LPCTSTR szTypeInfoName,long *lpTypeInfo);
	afx_msg long TLIWWriteDocumentation(long pInterface, LPCTSTR Name, LPCTSTR DocString, long HelpContext, LPCTSTR HelpFile, long nIndex);
	afx_msg long TILayout(long lpCreateTypeInfo);
	afx_msg long TIRelease(long lpTypeInfo);
	afx_msg long TIAddFuncDesc(long lpCreateTypeInfo, long nIndex, LPCTSTR szArrName, LPCTSTR szParmsArr, LPCTSTR szNamesArr,long nNames);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

// Event maps
	//{{AFX_EVENT(CFoxtlibCtrl)
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
	//{{AFX_DISP_ID(CFoxtlibCtrl)
	dispidTLLoadTypeLib = 1L,
	dispidTLRelease = 2L,
	dispidTLGetTypeInfoCount = 3L,
	dispidTLGetTypeAttr = 4L,
	dispidTLGetTypeInfo = 5L,
	dispidTLGetDocumentation = 6L,
	dispidTIGetNames = 7L,
	dispidTIGetFuncDesc = 8L,
	dispidTest = 9L,
	dispidTIGetVarDesc = 10L,
	//}}AFX_DISP_ID
	};
};

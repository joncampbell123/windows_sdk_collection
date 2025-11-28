//////////////////////////////////////////////////////////////////////////
//
//  controlhost.h
//
//  This file contains our CControlHost specification. It is used
//  to implement a generic ActiveX Control container.
//
//  (C) Copyright 1997 by Microsoft Corporation. All rights reserved.
//
//////////////////////////////////////////////////////////////////////////

#ifndef _CONTROLHOST_H_
#define _CONTROLHOST_H_

#include <ocidl.h>

#define BASE_EXTENDED_PROPERTY      0x80010000
#define DISPID_NAME                 (BASE_EXTENDED_PROPERTY | 0x00)
#define DISPID_ALIGN                (BASE_EXTENDED_PROPERTY | 0x01)
#define DISPID_BASEHREF             (BASE_EXTENDED_PROPERTY | 0x02)

class CControlHost : public IDispatch,
                     public IOleInPlaceSiteWindowless,
                     public IOleInPlaceFrame,
                     public IOleControlSite,
                     public IOleClientSite
{
    protected:
        HWND                _hwnd;              // container window handle
        HWND                _hwndControl;       // Control's window handle
        UINT                _cRef;              // IUnknown ref count
        BOOL                _bCapture;          // mouse capture flag
        RECT                _rcPos;
        DWORD               _dwTBSink;

        IUnknown           *_punk;              // copy unknown from instance structure
        void               *_punkOuter;         // parent container
        CDAViewerCtl       *_pvc;               // DAViewerControl

    public:
        void NotifyButton(long buttonId, long buttonState)
        {
            if(buttonId == 0)
                buttonId++;
        }
        void NotifyTrackBar(long position)
        {
            if(position == 0)
                position++;
        }
        void NotifyStatusBar(long position, BSTR* pstrText)
        {
            if(position == 0)
                position++;
        }

        ~CControlHost();
        CControlHost(IUnknown *);

        // *** IUnknown Methods ***
        STDMETHODIMP QueryInterface(REFIID riid, LPVOID * ppvObj);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        // *** IDispatch Methods ***
        STDMETHOD (GetIDsOfNames)(REFIID riid, OLECHAR FAR* FAR* rgszNames,	unsigned int cNames, LCID lcid,	DISPID FAR* rgdispid);
        STDMETHOD (GetTypeInfo)(unsigned int itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo);
        STDMETHOD (GetTypeInfoCount)(unsigned int FAR * pctinfo);
        STDMETHOD (Invoke)(DISPID dispid, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS FAR *pdispparams, VARIANT FAR *pvarResult, EXCEPINFO FAR * pexecinfo, unsigned int FAR *puArgErr);

        // *** IOleClientSite methods ***
        STDMETHOD (SaveObject)();
        STDMETHOD (GetMoniker)(DWORD, DWORD, LPMONIKER *);
        STDMETHOD (GetContainer)(LPOLECONTAINER *);
        STDMETHOD (ShowObject)();
        STDMETHOD (OnShowWindow)(BOOL);
        STDMETHOD (RequestNewObjectLayout)();

        // *** IOleWindow Methods ***
        STDMETHOD (GetWindow) (HWND * phwnd);
        STDMETHOD (ContextSensitiveHelp) (BOOL fEnterMode);

        // *** IOleInPlaceSite Methods ***
        STDMETHOD (CanInPlaceActivate) (void);
        STDMETHOD (OnInPlaceActivate) (void);
        STDMETHOD (OnUIActivate) (void);
        STDMETHOD (GetWindowContext) (IOleInPlaceFrame ** ppFrame, IOleInPlaceUIWindow ** ppDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo);
        STDMETHOD (Scroll) (SIZE scrollExtent);
        STDMETHOD (OnUIDeactivate) (BOOL fUndoable);
        STDMETHOD (OnInPlaceDeactivate) (void);
        STDMETHOD (DiscardUndoState) (void);
        STDMETHOD (DeactivateAndUndo) (void);
        STDMETHOD (OnPosRectChange) (LPCRECT lprcPosRect);

        // *** IOleInPlaceSiteEx Methods ***
        STDMETHOD (OnInPlaceActivateEx) (BOOL *pfNoRedraw, DWORD dwFlags);
        STDMETHOD (OnInPlaceDeactivateEx) (BOOL fNoRedraw);
        STDMETHOD (RequestUIActivate) (void);

        // *** IOleInPlaceSiteWindowless Methods ***
        STDMETHOD (CanWindowlessActivate) (void);
        STDMETHOD (GetCapture) (void);
        STDMETHOD (SetCapture) (BOOL fCapture);
        STDMETHOD (GetFocus) (void);
        STDMETHOD (SetFocus) (BOOL fFocus);
        STDMETHOD (GetDC) (LPCRECT pRect, DWORD grfFlags, HDC *phDC);
        STDMETHOD (ReleaseDC) (HDC hDC);
        STDMETHOD (InvalidateRect) (LPCRECT pRect, BOOL fErase);
        STDMETHOD (InvalidateRgn) (HRGN hRGN, BOOL fErase);
        STDMETHOD (ScrollRect) (INT dx, INT dy, LPCRECT pRectScroll, LPCRECT pRectClip);
        STDMETHOD (AdjustRect) (LPRECT prc);
        STDMETHOD (OnDefWindowMessage) (UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *plResult);

        // *** IOleInPlaceUIWindow Methods ***
        STDMETHOD (GetBorder)(LPRECT lprectBorder);
        STDMETHOD (RequestBorderSpace)(LPCBORDERWIDTHS lpborderwidths);
        STDMETHOD (SetBorderSpace)(LPCBORDERWIDTHS lpborderwidths);
        STDMETHOD (SetActiveObject)(IOleInPlaceActiveObject * pActiveObject,
                                    LPCOLESTR lpszObjName);

        // *** IOleInPlaceFrame Methods ***
        STDMETHOD (InsertMenus)(HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths);
        STDMETHOD (SetMenu)(HMENU hmenuShared, HOLEMENU holemenu, HWND hwndActiveObject);
        STDMETHOD (RemoveMenus)(HMENU hmenuShared);
        STDMETHOD (SetStatusText)(LPCOLESTR pszStatusText);
        STDMETHOD (EnableModeless)(BOOL fEnable);
        STDMETHOD (TranslateAccelerator)(LPMSG lpmsg, WORD wID);

        // *** IOleControlSite Methods ***
        STDMETHOD (OnControlInfoChanged)(void);
        STDMETHOD (LockInPlaceActive)(BOOL fLock);
        STDMETHOD (GetExtendedControl)(IDispatch **ppDisp);
        STDMETHOD (TransformCoords)(POINTL *pptlHimetric, POINTF *pptfContainer, DWORD dwFlags);
        STDMETHOD (TranslateAccelerator)(LPMSG pMsg, DWORD grfModifiers);
        STDMETHOD (OnFocus)(BOOL fGotFocus);
        STDMETHOD (ShowPropertyFrame)(void);

        // *** External Helper API ***
        HRESULT SetHwnd(HWND hwnd);

        HRESULT CreateControl();
        HRESULT DeleteControl();
        HRESULT QueryObject(REFIID riid, void **ppvObject);
};

#endif

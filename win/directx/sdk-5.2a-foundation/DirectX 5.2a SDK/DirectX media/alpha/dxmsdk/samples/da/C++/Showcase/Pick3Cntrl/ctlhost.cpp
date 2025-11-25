//////////////////////////////////////////////////////////////////////////
//
//  controlhost.cpp
//
//  This file contains the implementation of the CControlHost class.
//  This class is used to host ActiveX Controls.
//
//  (C) Copyright 1997 by Microsoft Corporation. All rights reserved.
//
//////////////////////////////////////////////////////////////////////////

#define INITGUID
#include <exdispid.h>
#include <olectl.h>
#include "dactl.h"
#include "ctlhost.h"
#include <comdef.h>

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::CControlHost                      [CONSTRUCTOR]
//
CControlHost::CControlHost(IUnknown *pUnk)
    :_hwnd(NULL), _punkOuter(NULL),
    _cRef(0), _punk(NULL), _pvc(NULL),
    _dwTBSink(0)
{
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::~CControlHost                     [DESTRUCTOR]
//
CControlHost::~CControlHost()
{
    DeleteControl();    // insurance
}


//////////////////////////////////////////////////////////////////////////
//
//
//
HRESULT CControlHost::SetHwnd(HWND hwnd)
{
    _hwnd = hwnd;
    if (!GetClientRect(_hwnd, &_rcPos))
        SetRectEmpty(&_rcPos);

    return S_OK;
}
//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::QueryInterface                    [IUnknown]
//
HRESULT CControlHost::QueryInterface(REFIID riid, LPVOID * ppvObj)
{
    if (ppvObj == NULL)
        return E_INVALIDARG;

    *ppvObj = NULL;

    if (IsEqualIID(riid, IID_IOleInPlaceSite))
        *ppvObj = (IOleInPlaceSite *)this;
    else if (IsEqualIID(riid, IID_IOleControlSite))
        *ppvObj = (IOleControlSite *)this;
    else if (IsEqualIID(riid, IID_IOleInPlaceSiteWindowless))
        *ppvObj = (IOleInPlaceSiteWindowless *)this;
    else if (IsEqualIID(riid, IID_IDispatch))
        *ppvObj = (IDispatch *)this;
    else
        return (E_NOINTERFACE);

    if(*ppvObj)
        AddRef();

    return (S_OK);
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::AddRef                            [IUnknown]
//
ULONG CControlHost::AddRef()
{
    return (_cRef++);
}

ULONG CControlHost::Release()
{
    _cRef--;

    if (_cRef > 0)
        return (_cRef);

    _cRef = 0;
    delete this;

    return (0);
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::SaveObject                        [IOleClientSite]
//
HRESULT CControlHost::SaveObject()
{
    return (E_NOTIMPL);
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::GetMoniker                        [IOleClientSite]
//
HRESULT CControlHost::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, LPMONIKER * ppMk)
{
    return (E_NOTIMPL);
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::GetContainer                      [IOleClientSite]
//
HRESULT CControlHost::GetContainer(LPOLECONTAINER * ppContainer )
{
    return (E_NOTIMPL);
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::ShowObject                        [IOleClientSite]
//
HRESULT CControlHost::ShowObject()
{
    return S_OK;
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::OnShowWindow                      [IOleClientSite]
//
HRESULT CControlHost::OnShowWindow(BOOL fShow)
{
    return (S_OK);
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::RequestNewObjectLayout            [IOleClientSite]
//
HRESULT CControlHost::RequestNewObjectLayout()
{
    return (E_NOTIMPL);
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::GetWindow                         [IOleWindow]
//
HRESULT CControlHost::GetWindow(HWND * lphwnd)
{
    if (_hwnd == NULL)
        return (S_FALSE);

    *lphwnd = _hwnd;
    return (S_OK);
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::ContextSensitiveHelp              [IOleWindow]
//
HRESULT CControlHost::ContextSensitiveHelp(BOOL fEnterMode)
{
    return (E_NOTIMPL);
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::CanInPlaceActivate                [IOleInPlaceSite]
//
HRESULT CControlHost::CanInPlaceActivate(void)
{
    return (S_OK);
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::OnInPlaceActivate                 [IOleInPlaceSite]
//
HRESULT CControlHost::OnInPlaceActivate(void)
{
    return (S_OK);
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::OnUIActivate                      [IOleInPlaceSite]
//
HRESULT CControlHost::OnUIActivate(void)
{
    return (E_NOTIMPL);
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::GetWindowContext                  [IOleInPlaceSite]
//
HRESULT CControlHost::GetWindowContext (IOleInPlaceFrame ** ppFrame, IOleInPlaceUIWindow ** ppIIPUIWin,
                                  LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
    *ppFrame = this;
    AddRef();

    *ppIIPUIWin = NULL;

    CopyRect(lprcPosRect, &_rcPos);
    CopyRect(lprcClipRect, &_rcPos);

    lpFrameInfo->cb             = sizeof(OLEINPLACEFRAMEINFO);
    lpFrameInfo->fMDIApp        = FALSE;
    lpFrameInfo->hwndFrame      = _hwnd;
    lpFrameInfo->haccel         = 0;
    lpFrameInfo->cAccelEntries  = 0;

    return (S_OK);
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::Scroll                            [IOleInPlaceSite]
//
HRESULT CControlHost::Scroll(SIZE scrollExtent)
{
    return (E_NOTIMPL);
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::OnUIDeactivate                    [IOleInPlaceSite]
//
HRESULT CControlHost::OnUIDeactivate(BOOL fUndoable)
{
    return (E_NOTIMPL);
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::OnInPlaceDeactivate               [IOleInPlaceSite]
//
HRESULT CControlHost::OnInPlaceDeactivate(void)
{
    return (S_OK);
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::DiscardUndoState                  [IOleInPlaceSite]
//
HRESULT CControlHost::DiscardUndoState(void)
{
    return (E_NOTIMPL);
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::DeactivateAndUndo                 [IOleInPlaceSite]
//
HRESULT CControlHost::DeactivateAndUndo(void)
{
    return (E_NOTIMPL);
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::OnPosRectChange                   [IOleInPlaceSite]
//
HRESULT CControlHost::OnPosRectChange(LPCRECT lprcPosRect)
{
    RECT rcPos, rcClient;
    HWND hwnd;
    GetWindow(&hwnd);
    GetClientRect(hwnd, &rcClient);
    GetWindowRect(hwnd, &rcPos);

    if(rcClient.bottom < lprcPosRect->bottom)
        rcPos.bottom += lprcPosRect->bottom - rcClient.bottom;
    else
        rcPos.bottom -= rcClient.bottom - lprcPosRect->bottom;

    if(rcClient.right < lprcPosRect->right)
        rcPos.right += lprcPosRect->right - rcClient.right; 
    else
        rcPos.right -= rcClient.right - lprcPosRect->right; 

    MoveWindow( hwnd, 
                rcPos.left, 
                rcPos.top, 
                rcPos.right - rcPos.left, 
                rcPos.bottom - rcPos.top,
                TRUE);

    return (S_OK);
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::OnInPlaceActivateEx               [IOleInPlaceSiteEx]
//
HRESULT CControlHost::OnInPlaceActivateEx(BOOL *pfNoRedraw, DWORD dwFlags)
{
    OleLockRunning(_punk, TRUE, FALSE);
    HRESULT hr = E_FAIL;

    return S_OK;
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::OnInPlaceDeactivateEx             [IOleInPlaceSiteEx]
//
HRESULT CControlHost::OnInPlaceDeactivateEx(BOOL fNoRedraw)
{
    OleLockRunning(_punk, FALSE, FALSE);
    return S_OK;
}


//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::RequestUIActivate                 [IOleInPlaceSiteEx]
//
HRESULT CControlHost::RequestUIActivate(void)
{
    return S_OK;
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::CanWindowlessActivate             [IOleInPlaceSiteWindowless]
//
HRESULT CControlHost::CanWindowlessActivate(void)
{
    return (TRUE); // m_bCanWindowlessActivate);
}
//////////////////////////////////////////////////////////////////////////
//                                                  [IOleInPlaceSiteWindowless]
//  CControlHost::GetCapture
//
HRESULT CControlHost::GetCapture(void)
{
    return (E_NOTIMPL);
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::SetCapture                        [IOleInPlaceSiteWindowless]
//
HRESULT CControlHost::SetCapture(BOOL fCapture)
{

    if (fCapture)
    {
        ::SetCapture(_hwnd);
        _bCapture = TRUE;
    }
    else
    {
        ::ReleaseCapture();
        _bCapture = FALSE;
    }
    return S_OK;
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::GetFocus                          [IOleInPlaceSiteWindowless]
//
HRESULT CControlHost::GetFocus(void)
{
    return (S_OK);
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::SetFocus                          [IOleInPlaceSiteWindowless]
//
HRESULT CControlHost::SetFocus(BOOL fFocus)
{
    return (S_OK);
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::GetDC                             [IOleInPlaceSiteWindowless]
//
HRESULT CControlHost::GetDC(LPCRECT pRect, DWORD grfFlags, HDC *phDC)
{
    if (!phDC)
        return E_POINTER;

    *phDC = ::GetDC(_hwnd);
    return S_OK;
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::ReleaseDC                         [IOleInPlaceSiteWindowless]
//
HRESULT CControlHost::ReleaseDC(HDC hDC)
{
    ::ReleaseDC(_hwnd, hDC);
    return S_OK;
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::InvalidateRect                    [IOleInPlaceSiteWindowless]
//
HRESULT CControlHost::InvalidateRect(LPCRECT pRect, BOOL fErase)
{
    ::InvalidateRect(_hwnd, pRect, fErase);
    return S_OK;
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::InvalidateRgn                     [IOleInPlaceSiteWindowless]
//
HRESULT CControlHost::InvalidateRgn(HRGN hRGN, BOOL fErase)
{
    ::InvalidateRgn(_hwnd, hRGN, fErase);
    return S_OK;
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::ScrollRect                        [IOleInPlaceSiteWindowless]
//
HRESULT CControlHost::ScrollRect(INT dx, INT dy, LPCRECT pRectScroll, LPCRECT pRectClip)
{
    return (S_OK);
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::AdjustRect                        [IOleInPlaceSiteWindowless]
//
HRESULT CControlHost::AdjustRect(LPRECT prc)
{
    return (S_OK);
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::OnDefWindowMessage                [IOleInPlaceSiteWindowless]
//
HRESULT CControlHost::OnDefWindowMessage(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *plResult)
{
    *plResult = ::DefWindowProc(_hwnd, msg, wParam, lParam);
    return S_OK;
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::GetBorder                         [IOleInPlaceUIWindow]
//
HRESULT CControlHost::GetBorder(LPRECT lprectBorder)
{
    return (E_NOTIMPL);
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::RequestBorderSpace                [IOleInPlaceUIWindow]
//
HRESULT CControlHost::RequestBorderSpace(LPCBORDERWIDTHS lpborderwidths)
{
    return (E_NOTIMPL);
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::SetBorderSpace                    [IOleInPlaceUIWindow]
//
HRESULT CControlHost::SetBorderSpace(LPCBORDERWIDTHS lpborderwidths)
{
    if(_punkOuter)
        return ((CControlHost*)_punkOuter)->SetBorderSpace(lpborderwidths);

    return (E_NOTIMPL);
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::SetActiveObject                   [IOleInPlaceUIWindow]
//
HRESULT CControlHost::SetActiveObject(IOleInPlaceActiveObject * pActiveObject, LPCOLESTR lpszObjName)
{
    return (E_NOTIMPL);
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::InsertMenus                       [IOleInPlaceFrame]
//
HRESULT CControlHost::InsertMenus(HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths)
{
    return (E_NOTIMPL);
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::SetMenu                           [IOleInPlaceFrame]
//
HRESULT CControlHost::SetMenu(HMENU hmenuShared, HOLEMENU holemenu, HWND hwndActiveObject)
{
    return (E_NOTIMPL);
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::RemoveMenus                       [IOleInPlaceFrame]
//
HRESULT CControlHost::RemoveMenus(HMENU hmenuShared)
{
    return (E_NOTIMPL);
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::SetStatusText                     [IOleInPlaceFrame]
//
HRESULT CControlHost::SetStatusText(LPCOLESTR pszStatusText)
{
    return (E_NOTIMPL);
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::EnableModeless                    [IOleInPlaceFrame]
//
HRESULT CControlHost::EnableModeless(BOOL fEnable)
{
    return (E_NOTIMPL);
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::TranslateAccelerator              [IOleInPlaceFrame]
//
HRESULT CControlHost::TranslateAccelerator(LPMSG lpmsg, WORD wID)
{
    return (E_NOTIMPL);
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::OnControlInfoChanged              [IOleControlSite]
//
HRESULT CControlHost::OnControlInfoChanged()
{
    return (E_NOTIMPL);
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::LockInPlaceActive                 [IOleControlSite]
//
HRESULT CControlHost::LockInPlaceActive(BOOL fLock)
{
    return (E_NOTIMPL);
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::GetExtendedControl                [IOleControlSite]
//
HRESULT CControlHost::GetExtendedControl(IDispatch **ppDisp)
{
    if (ppDisp == NULL)
        return (E_INVALIDARG);

    *ppDisp = (IDispatch *)this;
    AddRef();

    return (S_OK);
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::TransformCoords                   [IOleControlSite]
//
HRESULT CControlHost::TransformCoords(POINTL *pptlHimetric, POINTF *pptfContainer, DWORD dwFlags)
{
    return (E_NOTIMPL);
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::TranslateAccelerator              [IOleControlSite]
//
HRESULT CControlHost::TranslateAccelerator(LPMSG pMsg, DWORD grfModifiers)
{
    return (S_FALSE);
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::OnFocus                           [IOleControlSite]
//
HRESULT CControlHost::OnFocus(BOOL fGotFocus)
{
    return (E_NOTIMPL);
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::ShowPropertyFrame                 [IOleControlSite]
//
HRESULT CControlHost::ShowPropertyFrame(void)
{
    return (E_NOTIMPL);
}


//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::GetIDsOfNames                     [IDispatch]
//
HRESULT CControlHost::GetIDsOfNames(REFIID riid, OLECHAR FAR* FAR* rgszNames, unsigned int cNames, LCID lcid, DISPID FAR* rgdispid)
{
    HRESULT     hr;                     // standard ole return code
    LPOLESTR    pName;
    DISPID      *pdispid;

    hr      = S_OK;
    pName   = *rgszNames;
    pdispid = rgdispid;

    for (UINT i=0; i<cNames; i++)
    {
        if (pName != NULL)
        {
            if (_wcsicmp(pName, L"basehref") == 0)
                *pdispid = DISPID_BASEHREF;
            else if (_wcsicmp(pName, L"align") == 0)
                *pdispid = DISPID_ALIGN;
            else
            {
                *pdispid = DISPID_UNKNOWN;
                hr       = DISP_E_UNKNOWNNAME;
            }
        }

        pdispid++;
        pName++;
    }

    return (hr);
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::GetTypeInfo                       [IDispatch]
//
HRESULT CControlHost::GetTypeInfo(unsigned int itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo)
{
    return (E_NOTIMPL);
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::GetTypeInfoCount                  [IDispatch]
//
HRESULT CControlHost::GetTypeInfoCount(unsigned int FAR * pctinfo)
{
    return (E_NOTIMPL);
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::Invoke                            [IDispatch]
//
HRESULT CControlHost::Invoke(DISPID dispid, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS FAR *pdispparams, VARIANT FAR *pvarResult, EXCEPINFO FAR * pexecinfo, unsigned int FAR *puArgErr)
{
    HRESULT     hr = S_OK;  // standard ole return code

    // The following code are the DISPIDs for the ambient properties
    // that we will support.
    if ((pvarResult == NULL) && (wFlags == DISPATCH_PROPERTYGET))
        return (E_INVALIDARG);
    
    switch (dispid)
    {
        case DISPID_NAVIGATECOMPLETE:
            hr = S_OK;

        case DISPID_AMBIENT_DISPLAYNAME:
            if (!pvarResult->bstrVal)
                pvarResult->bstrVal = ::SysAllocString(L"");

            hr = S_OK;

            // If we STILL don't have a bstrVal.  Clean up and return an empty variant.
            if (!pvarResult->bstrVal) 
            {
                VariantInit(pvarResult);
                hr = E_FAIL;
            }

            break;

        case DISPID_AMBIENT_USERMODE:
        case DISPID_AMBIENT_MESSAGEREFLECT:
            pvarResult->vt      = VT_BOOL;
            pvarResult->boolVal = TRUE;
            hr                  = S_OK;
            break;

        case DISPID_AMBIENT_SHOWHATCHING:
        case DISPID_AMBIENT_SHOWGRABHANDLES:
        case DISPID_AMBIENT_SUPPORTSMNEMONICS:
            pvarResult->vt      = VT_BOOL;
            pvarResult->boolVal = FALSE;
            hr                  = S_OK;
            break;

        // Not yet implemented!
        case DISPID_AMBIENT_BACKCOLOR:
        case DISPID_AMBIENT_FORECOLOR:
        case DISPID_AMBIENT_UIDEAD:
        case DISPID_AMBIENT_AUTOCLIP:
            hr = S_OK;
            break;

        // Extender Properties
        case DISPID_BASEHREF:
        case DISPID_ALIGN:
            hr = DISP_E_MEMBERNOTFOUND;
            break;

        default:
            hr = DISP_E_MEMBERNOTFOUND;
    }

    return (hr);
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::CreateControl                     [HELPER]
//
HRESULT CControlHost::CreateControl()
{
    HRESULT             hr;             // standard ole return code
    IOleObject          *pIOleObject;   // IOleObject interface pointer
    IPersistStreamInit  *pps;           // IPersistStreamInit interface pointer
    
    // Create a DAViewerControl.
    _pvc = new CDAViewerCtl();

    // Check if the creation succeeded.
    if (!_pvc)
        return E_OUTOFMEMORY;

    hr = _pvc->GetIUnknown(&_punk);
    if (FAILED(hr)) {
        return (hr);
    }

    AddRef();  // addref the container

    hr = _punk->QueryInterface(IID_IOleObject, (LPVOID *)&pIOleObject);

    if (SUCCEEDED(hr))
    {
        pIOleObject->SetClientSite(this);

        // Controls like to know they have been initialized by getting
        // called on their InitNew method.
        hr = _punk->QueryInterface(IID_IPersistStreamInit, (LPVOID *)&pps);
        if (SUCCEEDED(hr))
        {
            pps->InitNew();
            pps->Release();
        }

        // The control creation succeeded, lets create the model.
        _pvc->CreateModel();

        // Tell the control to activate and show itself.
        pIOleObject->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL, this, 0, _hwnd, &_rcPos);
        pIOleObject->DoVerb(OLEIVERB_SHOW, NULL, this, 0, _hwnd, &_rcPos);
        pIOleObject->Release();

        // cache some frequently needed interface pointers    
        // cache the control's window handle
        _hwndControl = NULL;
        IOleInPlaceActiveObject *pipao = NULL;
        hr = _punk->QueryInterface(IID_IOleInPlaceActiveObject, (void**) &pipao);
        if (SUCCEEDED(hr))
        {
            pipao->GetWindow(&_hwndControl);
            pipao->Release();
        }
    }
    return (hr);
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::DeleteControl                     [HELPER]
//
HRESULT CControlHost::DeleteControl()
{
    HRESULT     hr;                     // standard ole return code
    IOleObject  *pIOleObject;           // IOleObject interface pointer
            
    if (_pvc != NULL) 
    {
        delete _pvc;
        _pvc = NULL;
    }

    // Close the Control and release the cached pointers.
    if (_punk != NULL)
    {
        hr = _punk->QueryInterface(IID_IOleObject, (LPVOID *)&pIOleObject);
        if (SUCCEEDED(hr))
        {
            pIOleObject->DoVerb(OLEIVERB_HIDE, NULL, this, 0, _hwnd, NULL);
            pIOleObject->Close(OLECLOSE_NOSAVE);
            pIOleObject->SetClientSite(NULL);
            pIOleObject->Release();
          
            _punk->Release();
            _punk = NULL;
        }

        Release();  // Release our hold on the container
    }

    return (S_OK);
}

//////////////////////////////////////////////////////////////////////////
//
//  CControlHost::QueryObject                       [HELPER]
//
HRESULT CControlHost::QueryObject(REFIID riid, void **ppvObject)
{
    HRESULT hr = E_POINTER;
    if (ppvObject)
    {
        if (_punk)
        {
            hr = _punk->QueryInterface(riid, ppvObject);
        }
        else
        {
            *ppvObject = NULL;
            hr = OLE_E_NOCONNECTION;
        }
    }
    return hr;
}

/*
 * IDROPSRC.CPP
 *
 * Template implementation of a DropSource object.  There is
 * actually a full standard implementation of this interface here.
 *
 * Copyright (c)1993-1996 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#include "idropsrc.h"



/*
 * CDropSource::CDropSource
 * CDropSource::~CDropSource
 *
 * Constructor Parameters:
 *  pObj            LPVOID back pointer to whatever we live with.
 */

CDropSource::CDropSource(LPVOID pBack)
    {
    m_cRef=0;
    m_pBack=pBack;
    return;
    }


CDropSource::~CDropSource(void)
    {
    return;
    }




/*
 * CDropSource::QueryInterface
 * CDropSource::AddRef
 * CDropSource::Release
 *
 * Purpose:
 *  Non-delegating IUnknown members for CDropSource.
 */

STDMETHODIMP CDropSource::QueryInterface(REFIID riid
    , LPVOID *ppv)
    {
    *ppv=NULL;

    //Any interface on this object is the object pointer.
    if (IsEqualIID(riid, IID_IUnknown)
        || IsEqualIID(riid, IID_IDropSource))
        *ppv=(LPVOID)this;

    /*
     * If we actually assign an interface to ppv we need to AddRef
     * it since we're returning a new pointer.
     */
    if (NULL!=*ppv)
        {
        ((LPUNKNOWN)*ppv)->AddRef();
        return NOERROR;
        }

    return ResultFromScode(E_NOINTERFACE);
    }


STDMETHODIMP_(ULONG) CDropSource::AddRef(void)
    {
    return ++m_cRef;
    }

STDMETHODIMP_(ULONG) CDropSource::Release(void)
    {
    ULONG   cRefT;

    cRefT=--m_cRef;

    if (0L==m_cRef)
        delete this;

    return cRefT;
    }





/*
 * CDropSource::QueryDragContinue
 *
 * Purpose:
 *  Determines whether to continue a drag operation or cancel it.
 *
 * Parameters:
 *  fEsc            BOOL indicating that the ESC key was pressed.
 *  grfKeyState     DWORD providing states of keys and mouse buttons
 *
 * Return Value:
 *  HRESULT         DRAGDROP_S_CANCEL to stop the drag,
 *                  DRAGDROP_S_DROP to drop the data where it is,
 *                  or NOERROR to continue.
 */

STDMETHODIMP CDropSource::QueryContinueDrag(BOOL fEsc
    , DWORD grfKeyState)
    {
    if (fEsc)
        return ResultFromScode(DRAGDROP_S_CANCEL);

    if (!(grfKeyState & MK_LBUTTON))
        return ResultFromScode(DRAGDROP_S_DROP);

    return NOERROR;
    }






/*
 * CDropSource::GiveFeedback
 *
 * Purpose:
 *  Provides cursor feedback to the user since the source task
 *  always has the mouse capture.  We can also provide any other
 *  type of feedback above cursors if we so desire.
 *
 * Parameters:
 *  dwEffect        DWORD effect flags returned from the last target
 *
 * Return Value:
 *  HRESULT         NOERROR if you set a cursor yourself or
 *                  DRAGDROP_S_USEDEFAULTCURSORS to let OLE do
 *                  the work.
 */

STDMETHODIMP CDropSource::GiveFeedback(DWORD dwEffect)
    {
    return ResultFromScode(DRAGDROP_S_USEDEFAULTCURSORS);
    }

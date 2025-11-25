#include "dadrag.h"


/***************************************
 * CDADrag
 ***************************************/

STDMETHODIMP_(ULONG) CDADrag::AddRef()  { 
	return InterlockedIncrement(&_cRefs); 
}
	
STDMETHODIMP_(ULONG) CDADrag::Release()  {
	ULONG refCount = InterlockedDecrement(&_cRefs);
	if (!refCount) {
		delete this;
		return refCount;
	}
	return _cRefs;
}

STDMETHODIMP CDADrag::QueryInterface(REFIID riid, void **ppv)  {
	if (!ppv)
		return E_POINTER;
	
	*ppv = NULL;
	if (riid == IID_IUnknown)  {
		*ppv = (void *)(IUnknown *)this;
	} else if (riid == IID_IDABvrHook)  {
		*ppv = (void *)(IDAUntilNotifier *)this;
	}
	
	if (*ppv)  {
		((IUnknown *)*ppv)->AddRef();
		return S_OK;
	}
	
	return E_NOINTERFACE;
}

STDMETHODIMP CDADrag::initNotify(IDAImagePtr im, IDAPoint2Ptr pt, IDAStaticsPtr e)  {
   _e  = e;
  IDAPickableResultPtr pim = im->Pickable();
  _pickEv = _e->AndEvent(e->LeftButtonDown, pim->PickEvent);

  // construct a handler for release events,
  // upon pick events it passes the puck back to this object.
  _releaseHdlr = new CDARelease();
  _releaseHdlr->initNotify(_pickEv, this, e);  
  _draggablePt = (IDAPoint2Ptr)_e->UntilNotify(pt, _pickEv, this)->RunOnce(); 
  _releaseEv = _e->LeftButtonUp->Snapshot(_draggablePt);

  _draggableImg = pim->Image->Transform(_e->Translate2Point(_draggablePt));

  return S_OK;
}

STDMETHODIMP CDADrag::raw_Notify(IDABehavior* eventData,
					IDABehavior* curRunningBvr,
					IDAView* curView,
					IDABehavior** ppBvr)  {
	IDAPoint2Ptr currPt = (IDAPoint2Ptr)curRunningBvr;

  // pull apart the pair that comes from andEvent and from the
	// pick event pair, ultimately to get at the local mouse 
	IDAPairPtr andEventPair = (IDAPairPtr)eventData;
        
  IDAPairPtr    pickPair   = (IDAPairPtr)(andEventPair->Second);

	IDAVector2Ptr localMouse = (IDAVector2Ptr)(pickPair->Second);

	IDAPoint2Ptr draggingPt = currPt->Transform(_e->Translate2Vector(localMouse));

  IDAPoint2Ptr finalPt = (IDAPoint2Ptr)_e->UntilNotify(draggingPt, _releaseEv, _releaseHdlr);

	*ppBvr = (IDABehavior*)finalPt;

	finalPt->AddRef();

	return S_OK;
}

/***************************************
 * CDARelease
 ***************************************/

STDMETHODIMP_(ULONG) CDARelease::AddRef()  { 
	return InterlockedIncrement(&_cRefs); 
}
	
STDMETHODIMP_(ULONG) CDARelease::Release()  {
	ULONG refCount = InterlockedDecrement(&_cRefs);
	if (!refCount) {
		delete this;
		return refCount;
	}
	return _cRefs;
}

STDMETHODIMP CDARelease::QueryInterface(REFIID riid, void **ppv)  {
	if (!ppv)
		return E_POINTER;
	
	*ppv = NULL;
	if (riid == IID_IUnknown)  {
		*ppv = (void *)(IUnknown *)this;
	} else if (riid == IID_IDABvrHook)  {
		*ppv = (void *)(IDAUntilNotifier *)this;
	}
	
	if (*ppv)  {
		((IUnknown *)*ppv)->AddRef();
		return S_OK;
	}
	
	return E_NOINTERFACE;
}

STDMETHODIMP CDARelease::initNotify(IDAEventPtr pickEv, CDADrag* grabHdlr, IDAStaticsPtr e)  {
   _e  = e;
  _pickEv = pickEv;
  _grabHdlr = grabHdlr;
  return S_OK;
}

STDMETHODIMP CDARelease::raw_Notify(IDABehavior* eventData,
					IDABehavior* curRunningBvr,
					IDAView* curView,
					IDABehavior** ppBvr)  {

  // Releasing.  Freeze the image where it is, and go 
  // back to waiting for a pick event.
  IDAPoint2Ptr snappedPt = (IDAPoint2Ptr)eventData;
  IDAPoint2Ptr releasePt = (IDAPoint2Ptr)_e->UntilNotify(snappedPt,
    _pickEv, _grabHdlr);

  *ppBvr = (IDABehavior*)releasePt;

	releasePt->AddRef();

	return S_OK;
}



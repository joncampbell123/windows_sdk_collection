#include <atlbase.h>
#include <comdef.h>
#include <danim.h>

#include <stdio.h>
#include <sys\timeb.h>
#include <tchar.h>

#define WINDOW_WIDTH    300
#define WINDOW_HEIGHT   300

// Create the DirectAnimation class that will render to the primary DirectDraw
// surface.
class DXA  {
  
  public:
	DXA();
	BOOL initDXAViewObj(HWND hwnd, BOOL is8Bit, IUnknown *ddsurf);
	BOOL resetDXASurfaces(IUnknown *ddsurf);
	void tick();
	void cleanupDXA();
  protected:
	HRESULT hr;
	static IDAView* _view;
	IDAStatics* e;
	IDAImage* pFinalImg;
	IDAImage* pBackImg;
	IDAImage* pFrontImg;
	IDASound* pFinalSound;
	double startTime;
};

IDAView* DXA::_view = NULL;

/*
 * StartOle
 *
 * Start the Ole services.
 */
struct StartOle {
    StartOle() { CoInitialize( NULL ); }
    ~StartOle() { CoUninitialize(); }
} _inst_StartOle;

/*
 * dump_com_error
 *
 * Dump the com error should an exception occurs in InitDXAViewObj.
 * This is mainly for debugging purpose.
 */
void dump_com_error( _com_error &e )
{
    char buf[2048];

    sprintf(buf, _T( "Oops - hit an error!\n\tCode = %08lx\n\tCode meaning = %s\n" ),
            e.Error(), e.ErrorMessage());
    OutputDebugString(buf);
}

DXA::DXA()  {
  _view = NULL;
  e = NULL;
  pFinalImg = NULL;
  pBackImg = NULL;
  pFrontImg = NULL;
  pFinalSound = NULL;
}

/*
 * initDXAViewObj
 *
 * Create the DAView object, construct the model, then start the model.
 *
 */
BOOL DXA::initDXAViewObj(HWND hwnd, BOOL is8Bit, IUnknown *ddsurf)
{
	  hr = CoCreateInstance(CLSID_DAView, 
	    NULL, CLSCTX_INPROC, IID_IDAView, (void**) &_view);
	  if(!SUCCEEDED(hr))  {
	    cleanupDXA();
		return FALSE;
	  }

	  hr = CoCreateInstance(CLSID_DAStatics, 
	    NULL, CLSCTX_INPROC, IID_IDAStatics, (void**) &e);
	  if(!SUCCEEDED(hr))  {
	    cleanupDXA();
		return FALSE;
	  }
	  
	  hr = e->ImportImage(_bstr_t("myimage1.gif"), &pBackImg);
	  if(!SUCCEEDED(hr))  {
	    cleanupDXA();
		return FALSE;
	  }

	  hr = e->ImportImage(_bstr_t("myimage2.gif"), &pFrontImg);
	  if(!SUCCEEDED(hr))  {
	    cleanupDXA();
		return FALSE;
	  }

	  hr = e->Overlay(pFrontImg, pBackImg, &pFinalImg);
	  if(!SUCCEEDED(hr))  {
	    cleanupDXA();
		return FALSE;
	  }

	  hr = _view->put_IDirectDrawSurface(ddsurf);
	  if(!SUCCEEDED(hr))  {
	    cleanupDXA();
		return FALSE;
	  }

	  hr = _view->put_CompositeDirectlyToTarget(TRUE);
	  if(!SUCCEEDED(hr))  {
	    cleanupDXA();
		return FALSE;
	  }

	  hr = e->get_Silence(&pFinalSound);
	  if(!SUCCEEDED(hr))  {
	    cleanupDXA();
		return FALSE;
	  }

	  struct _timeb timebuffer;
      _ftime( &timebuffer );
      startTime = timebuffer.time + ( timebuffer.millitm / 1000.0 );

	  hr = _view->StartModel(pFrontImg, pFinalSound, 0);
	  if(!SUCCEEDED(hr))  {
	    cleanupDXA();
		return FALSE;
	  }

    return TRUE;
}

BOOL DXA::resetDXASurfaces(IUnknown *ddsurf)
{
    try {
	  hr = _view->put_IDirectDrawSurface(ddsurf);
	  if(!SUCCEEDED(hr))  {
	    cleanupDXA();
		return FALSE;
	  }
    } catch (_com_error &e) {
        dump_com_error( e );
		cleanupDXA();
        return FALSE;
    }
    return TRUE;
}

void DXA::cleanupDXA()  {
  if(_view != NULL)  {
    _view->Release();
	_view = NULL;
  }
  if(e != NULL)  {
	e->Release();
	e = NULL;
  }
  if(pBackImg != NULL)  {
	pBackImg->Release();
	pBackImg = NULL;
  }
  if(pFrontImg != NULL)  {
	pFrontImg->Release();
	pFrontImg = NULL;
  }

  if(pBackImg != NULL)  {
	pBackImg->Release();
	pBackImg = NULL;
  }

  if(pFinalSound != NULL)  {
	pFinalSound->Release();
	pFinalSound = NULL;
  }
}


/*
 * tick
 * Ask DA to sample and display the model.
 */
void DXA::tick() {
  if (_view != NULL) {
    struct _timeb timebuffer;

    _ftime( &timebuffer );
    double thisTime = timebuffer.time + ( timebuffer.millitm / 1000.0 );

    thisTime -= startTime;

    BOOL bRender;
    hr = _view->Tick(thisTime, (short*)&bRender);

    if(!SUCCEEDED(hr))  {
	  cleanupDXA();
	  return;
	}

    if(bRender)  {
      hr = _view->Render();
	  if(!SUCCEEDED(hr))  {
	    cleanupDXA();
	    return;
	  }
	}
  }
}



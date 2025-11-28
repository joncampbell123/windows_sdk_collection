//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1992 - 1997  Microsoft Corporation.  All Rights Reserved.
//
//--------------------------------------------------------------------------;

#include <streams.h>
#include "Ext_Enum.h"

class CAMovie : public IAMovie, public CUnknown
{
private:
    IGraphBuilder   *   pGB;
    IMediaControl   *   pMC;
    IMediaEvent     *   pME;
    IMediaPosition  *   pMP;

protected:
    ~CAMovie();
    CAMovie(TCHAR *pName, LPUNKNOWN pUnk, HRESULT *phr);

    // Disconnect all pins.  If a filter has no pins, remove it completely
    // This is to part-destroy a filter graph, leaving only those filters
    // that may be usefult for rendering a file or pin after this function
    // has been called.
    HRESULT DisconnectAllAndRemoveIsolates();

    // For all filters in graph, if the filter has pins, and all are unconnected,
    // then remove the filter from the graph.
    HRESULT RemoveAllUnconnected();

public:
    DECLARE_IUNKNOWN

    static CUnknown * WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT *phr);

    STDMETHODIMP NonDelegatingQueryInterface(REFIID iid, void ** ppv);

    // IFilterGraph

    // Add a filter to the graph and name it with *pName.
    // If the name is not unique, The request will fail.
    // The Filter graph will call the JoinFilterGraph
    // member function of the filter to inform it.
    // This must be called before attempting Connect, ConnectDirect or Render
    // for pins of the filter.

    STDMETHODIMP AddFilter
        ( IBaseFilter * pFilter,
          LPCWSTR pName
        );

    // Remove a filter from the graph. The filter graph implementation
    // will inform the filter that it is being removed.

    STDMETHODIMP RemoveFilter
        ( IBaseFilter * pFilter
        );

    // Set *ppEnum to be an enumerator for all filters in the graph.

    STDMETHODIMP EnumFilters
        ( IEnumFilters **ppEnum
        );

    // Set *ppFilter to be the filter which was added with the name *pName
    // Will fail and set *ppFilter to NULL if the name is not in this graph.

    STDMETHODIMP FindFilterByName
        ( LPCWSTR pName,
          IBaseFilter ** ppFilter
        );

    //==========================================================================
    // Low level connection functions
    //==========================================================================

    // Connect these two pins directly (i.e. without intervening filters)
    // the media type is optional, and may be partially specified (that is
    // the subtype and/or format type may be GUID_NULL). See IPin::Connect
    // for details of the media type parameter.
    STDMETHODIMP ConnectDirect
        ( IPin * ppinOut,              // the output pin
          IPin * ppinIn,               // the input pin
          const AM_MEDIA_TYPE* pmt     // optional mediatype
        );

    // On a separate thread (which will not hold any relevant locks)
    // Break the connection that this pin has and reconnect it to the
    // same other pin.

    STDMETHODIMP Reconnect
        ( IPin * ppin        // the pin to disconnect and reconnect
        );

    // Disconnect this pin, if connected.  Successful no-op if not connected.

    STDMETHODIMP Disconnect
        ( IPin * ppin
        );

    //==========================================================================
    // intelligent connectivity - now in IGraphBuilder, axextend.idl
    //==========================================================================

    //==========================================================================
    // Whole graph functions
    //==========================================================================

    // Once a graph is built, it can behave as a (composite) filter.
    // To control this filter, QueryInterface for IMediaFilter.

    // The filtergraph will by default ensure that the graph has a sync source
    // when it is made to Run.  SetSyncSource(NULL) will prevent that and allow
    // all the filters to run unsynchronised until further notice.
    // SetDefaultSyncSource will set the default sync source (the same as would
    // have been set by default on the first call to Run).
    STDMETHODIMP SetDefaultSyncSource(void);



    // IGraphBuilder

    // Connect these two pins directly or indirectly, using transform filters
    // if necessary.

    STDMETHODIMP Connect
    ( IPin * ppinOut,    // the output pin
      IPin * ppinIn      // the input pin
    );

    // Connect this output pin directly or indirectly, using transform filters
    // if necessary to something that will render it.

    STDMETHODIMP Render
    ( IPin * ppinOut     // the output pin
    );

    // This pair of methods are a cross between the IGraphBuilder ones and the IMediaControl ones,
    // unfortunate perhaps, but they do make the nicest C / C++ interfaces .

    // adds and connects filters needed to play the specified file
    STDMETHODIMP RenderFile( LPCWSTR strFilename );

    // adds to the graph the source filter that can read this file,
    // and returns an IFilterInfo object for it
    STDMETHODIMP AddSourceFilter( LPCWSTR strFilename, IBaseFilter ** ppUnk);


    // IMediaControl methods
    STDMETHODIMP Run();
    STDMETHODIMP Pause();
    STDMETHODIMP Stop();

    //returns the state. same semantics as IMediaFilter::GetState
    STDMETHODIMP GetState(
                LONG msTimeout,
                FILTER_STATE* pfs);

    // IMediaEvent

    // get back the event handle. This is manual-reset
    // (don't - it's reset by the event mechanism) and remains set
    // when events are queued, and reset when the queue is empty.
    STDMETHODIMP GetEventHandle(
                    HEVENT * hEvent);

    // remove the next event notification from the head of the queue and
    // return it. Waits up to msTimeout millisecs if there are no events.
    // if a timeout occurs without any events, this method will return
    // E_ABORT, and the value of the event code and other parameters
    // is undefined.
    STDMETHODIMP GetEvent(
                    long * lEventCode,
                    long * lParam1,
                    long * lParam2,
                    long msTimeout
                    );
    // Free any resources associated with the parameters to an event.
    // Event parameters may be LONGs, IUnknown* or BSTR. No action
    // is taken with LONGs. IUnknown are passed addrefed and need a
    // Release call. BSTR are allocated by the task allocator and will be
    // freed by calling the task allocator.
    STDMETHODIMP FreeEventParams(
		    long lEvCode,
		    long lParam1,
		    long lParam2);

    // Calls GetEvent repeatedly discarding events until it finds a
    // completion event (EC_COMPLETE, EC_ERRORABORT, or EC_USERABORT).
    // The completion event is removed from the queue and returned
    // in pEvCode. Note that the object is still in running mode until
    // a Pause or Stop call is made.
    // If the timeout occurs, *pEvCode will be 0 and E_ABORT will be
    // returned.
    STDMETHODIMP WaitForCompletion(
                    long msTimeout,
                    long * pEvCode);

    // cancels any system handling of the specified event code
    // and ensures that the events are passed straight to the application
    // (via GetEvent) and not handled. A good example of this is
    // EC_REPAINT: default handling for this ensures the painting of the
    // window and does not get posted to the app.
    STDMETHODIMP CancelDefaultHandling(
                    long lEvCode);

    // restore the normal system default handling that may have been
    // cancelled by CancelDefaultHandling().
    STDMETHODIMP RestoreDefaultHandling( long lEvCode);


    // IMediaPosition
    // properties
    STDMETHODIMP get_Duration(
                REFTIME* plength);

    STDMETHODIMP put_CurrentPosition(
                REFTIME llTime);

    STDMETHODIMP get_CurrentPosition(
                REFTIME* pllTime);

    STDMETHODIMP get_StopTime(
                REFTIME* pllTime);

    STDMETHODIMP put_StopTime(
                REFTIME llTime);

    STDMETHODIMP get_PrerollTime(
                REFTIME* pllTime);
    STDMETHODIMP put_PrerollTime(
                REFTIME llTime);

    STDMETHODIMP put_Rate(
                double dRate);
    STDMETHODIMP get_Rate(
                double * pdRate);

	/* New methods */

    // Remove all filters
    STDMETHODIMP  RemoveAllFilters();

    // If the filter graph can be Run to completion, run it and wait for it to complete
	STDMETHODIMP  Play();
    // As above, but RenderNewFile first
    STDMETHODIMP  PlayFile( LPCWSTR strFilename );

    // Render all unconnected output pins
    STDMETHODIMP  RenderAll();
    // Render supplied files "as if" the filter graoh were first emptied
    STDMETHODIMP  RenderNewFile( LPCWSTR strFilename );

    STDMETHODIMP  EnumFiltersByInterface( REFIID iid, IEnumFilters ** pEnumFilters );
    // Enum pins over the entire graph
    STDMETHODIMP  EnumPins( IEnumPins ** ppEnum );
    STDMETHODIMP  EnumPinsIn( IEnumPins ** ppEnum );
    STDMETHODIMP  EnumPinsOut( IEnumPins ** ppEnum );
};



CFactoryTemplate g_Templates[]=
{   {L"ActiveMovie Sample PID", &CLSID_AMovie,   CAMovie::CreateInstance},
};
int g_cTemplates = sizeof(g_Templates)/sizeof(g_Templates[0]);


CAMovie::~CAMovie()
{ /* nothing to do */ }

CAMovie::CAMovie(TCHAR *pName, LPUNKNOWN pUnk, HRESULT *phr)
: CUnknown( pName, pUnk )
, pGB(0), pMC(0), pME(0), pMP(0)
{
    if (!pUnk) *phr = VFW_E_NEED_OWNER;
    else if SUCCEEDED(*phr)
    {
        *phr = pUnk->QueryInterface( IID_IGraphBuilder, reinterpret_cast<void**>(&pGB) );
        if SUCCEEDED(*phr)
        {
            pGB->Release();
            *phr = pUnk->QueryInterface( IID_IMediaControl, reinterpret_cast<void**>(&pMC) );
            if SUCCEEDED(*phr)
            {
                pMC->Release();
                *phr = pUnk->QueryInterface( IID_IMediaEvent, reinterpret_cast<void**>(&pME) );
                if SUCCEEDED(*phr)
                {
                    pME->Release();
                    *phr = pUnk->QueryInterface( IID_IMediaPosition, reinterpret_cast<void**>(&pMP) );
                    if SUCCEEDED(*phr) pMP->Release();
                }
            }
        }
    }
}

CUnknown * WINAPI CAMovie::CreateInstance(LPUNKNOWN pUnk, HRESULT *phr)
{
    CUnknown * result = 0;
    result = new CAMovie( NAME("IAMovie Class"), pUnk, phr );
    if ( !result ) *phr = E_OUTOFMEMORY;
    return result;
}

STDMETHODIMP CAMovie::NonDelegatingQueryInterface(REFIID iid, void ** ppv)
{
    CheckPointer(ppv,E_POINTER);

    if ( iid == IID_IAMovie )
    {
        return GetInterface(static_cast<IAMovie *>(this), ppv );
    }
    else
    {
	return CUnknown::NonDelegatingQueryInterface(iid, ppv);
    }
}


// IFilterGraph (pGB)
STDMETHODIMP CAMovie::AddFilter( IBaseFilter * pFilter, LPCWSTR pName )
{ return pGB->AddFilter( pFilter, pName ); }
STDMETHODIMP CAMovie::RemoveFilter( IBaseFilter * pFilter )
{ return pGB->RemoveFilter( pFilter ); }
STDMETHODIMP CAMovie::EnumFilters( IEnumFilters **ppEnum )
{ return pGB->EnumFilters( ppEnum ); }
STDMETHODIMP CAMovie::FindFilterByName( LPCWSTR pName, IBaseFilter ** ppFilter )
{ return pGB->FindFilterByName( pName, ppFilter ); }
STDMETHODIMP CAMovie::ConnectDirect( IPin * ppinOut, IPin * ppinIn, const AM_MEDIA_TYPE* pmt )
{ return pGB->ConnectDirect( ppinOut, ppinIn, pmt ); }
STDMETHODIMP CAMovie::Reconnect( IPin * ppin )
{ return pGB->Reconnect( ppin ); }
STDMETHODIMP CAMovie::Disconnect( IPin * ppin )
{ return pGB->Disconnect( ppin ); }
STDMETHODIMP CAMovie::SetDefaultSyncSource( )
{ return pGB->SetDefaultSyncSource( ); }


// IGraphBuilder (pGB)
STDMETHODIMP CAMovie::Connect( IPin * ppinOut, IPin * ppinIn )
{ return pGB->Connect( ppinOut, ppinIn ); }
STDMETHODIMP CAMovie::Render( IPin * ppinOut )
{ return pGB->Render( ppinOut ); }


// This pair of methods are a cross between the IGraphBuilder ones and the IMediaControl ones,
// unfortunate perhaps, but they do make the nicest C / C++ interfaces .
STDMETHODIMP CAMovie::AddSourceFilter( LPCWSTR strFilename, IBaseFilter ** ppUnk )
{
    const LPWSTR fn = const_cast<LPWSTR>(strFilename);
    return pGB->AddSourceFilter( fn, fn, ppUnk );
}

STDMETHODIMP CAMovie::RenderFile( LPCWSTR strFilename )
{
    const LPWSTR fn = const_cast<LPWSTR>(strFilename);
    return pGB->RenderFile( fn, fn );
}


// IMediaControl (pMC)
STDMETHODIMP CAMovie::Run( )
{ return pMC->Run( ); }
STDMETHODIMP CAMovie::Pause( )
{ return pMC->Pause( ); }
STDMETHODIMP CAMovie::Stop( )
{ return pMC->Stop( ); }
STDMETHODIMP CAMovie::GetState( LONG msTimeout, FILTER_STATE* pfs )
{ return pMC->GetState( msTimeout, (long *)pfs ); }



// IMediaEvent (pME)
STDMETHODIMP CAMovie::GetEventHandle( HEVENT * hEvent )
{ return pME->GetEventHandle( (long *)hEvent ); }
STDMETHODIMP CAMovie::GetEvent( long * lEventCode, long * lParam1, long * lParam2, long msTimeout )
{ return pME->GetEvent( lEventCode, lParam1, lParam2, msTimeout ); }
STDMETHODIMP CAMovie::FreeEventParams( long lEventCode, long lParam1, long lParam2 )
{ return pME->FreeEventParams( lEventCode, lParam1, lParam2 ); }
STDMETHODIMP CAMovie::WaitForCompletion( long msTimeout, long * pEvCode )
{ return pME->WaitForCompletion( msTimeout, pEvCode ); }
STDMETHODIMP CAMovie::CancelDefaultHandling( long lEvCode )
{ return pME->CancelDefaultHandling( lEvCode ); }
STDMETHODIMP CAMovie::RestoreDefaultHandling( long lEvCode )
{ return pME->RestoreDefaultHandling( lEvCode ); }


// IMediaPosition (pMP )
STDMETHODIMP CAMovie::get_Duration( REFTIME* plength )
{ return pMP->get_Duration( plength ); }
STDMETHODIMP CAMovie::put_CurrentPosition( REFTIME llTime )
{ return pMP->put_CurrentPosition( llTime ); }
STDMETHODIMP CAMovie::get_CurrentPosition( REFTIME* pllTime )
{ return pMP->get_CurrentPosition( pllTime ); }
STDMETHODIMP CAMovie::get_StopTime( REFTIME* pllTime )
{ return pMP->get_StopTime( pllTime ); }
STDMETHODIMP CAMovie::put_StopTime( REFTIME llTime )
{ return pMP->put_StopTime( llTime ); }
STDMETHODIMP CAMovie::get_PrerollTime( REFTIME* pllTime )
{ return pMP->get_PrerollTime( pllTime ); }
STDMETHODIMP CAMovie::put_PrerollTime( REFTIME llTime )
{ return pMP->put_PrerollTime( llTime ); }
STDMETHODIMP CAMovie::put_Rate( double dRate )
{ return pMP->put_Rate( dRate ); }
STDMETHODIMP CAMovie::get_Rate( double * pdRate )
{ return pMP->get_Rate( pdRate ); }



// New methods

// Remove all filters
STDMETHODIMP CAMovie::RemoveAllFilters()
{
    HRESULT  hr;

    IEnumFilters * piEnumFilters;
    hr = EnumFilters( &piEnumFilters );
    if SUCCEEDED(hr)
    {
        CGenericList<IBaseFilter> FilterList("List of filters to be removed");

        IBaseFilter * piFilter;
        while ( piEnumFilters->Next(1, &piFilter, 0) == NOERROR )
        {
            FilterList.AddTail( piFilter );
        }
        piEnumFilters->Release();
        while ( piFilter = FilterList.RemoveHead() )
        {
            hr = RemoveFilter( piFilter );
            piFilter->Release();
        }
    }
    return hr;
}

// If the filter graph can be Run to completion, run it and wait for it to complete
STDMETHODIMP  CAMovie::Play()
{
    HRESULT hr;
    REFTIME duration;
    hr = get_Duration( &duration );
    if ( SUCCEEDED( hr ) && duration > 0 )
    {
        hr = Run();
        if SUCCEEDED(hr)
        {
            long evCode;
            WaitForCompletion( INFINITE, &evCode );
        }
        Stop();
    }
    return hr;
}

STDMETHODIMP  CAMovie::PlayFile( LPCWSTR strFilename )
{
    HRESULT hr;
    hr = RenderNewFile( strFilename );
    if SUCCEEDED(hr)
    {
        hr = Play();
    }
    return hr;
}

STDMETHODIMP CAMovie::RenderAll( )
{
    HRESULT hr = NOERROR;
    CImpIEnumPinsOutOverGraph EnumOutPins( this, &hr );
    if SUCCEEDED(hr)
    {
        IPin * piPin;

        while ( EnumOutPins.Next( 1, &piPin, 0 ) == NOERROR )
        {
            Render( piPin );
        }
    }
    return hr;
}

STDMETHODIMP CAMovie::RenderNewFile( LPCWSTR strFilename )
{
    HRESULT hr;
    DisconnectAllAndRemoveIsolates();
    hr = RenderFile( strFilename );
    RemoveAllUnconnected();
    return hr;
}

STDMETHODIMP CAMovie::EnumFiltersByInterface( REFIID iid, IEnumFilters ** ppEnum )
{
    HRESULT hr = NOERROR;
    *ppEnum = new CImpIEnumFilters( pGB, iid );
    if (!*ppEnum) hr = E_OUTOFMEMORY;
    else if FAILED(hr)
    {
        delete *ppEnum;
        *ppEnum = 0;
    }
    return hr;
}

STDMETHODIMP CAMovie::EnumPins( IEnumPins ** ppEnum )
{
    HRESULT hr = NOERROR;
    *ppEnum = new CImpIEnumPinsOverGraph( pGB, &hr );
    if (!*ppEnum) hr = E_OUTOFMEMORY;
    else if FAILED(hr)
    {
        delete *ppEnum;
        *ppEnum = 0;
    }
    return hr;
}

STDMETHODIMP CAMovie::EnumPinsIn( IEnumPins ** ppEnum )
{
    HRESULT hr = NOERROR;
    *ppEnum = new CImpIEnumPinsInOverGraph( pGB, &hr );
    if (!*ppEnum) hr = E_OUTOFMEMORY;
    else if FAILED(hr)
    {
        delete *ppEnum;
        *ppEnum = 0;
    }
    return hr;
}

STDMETHODIMP CAMovie::EnumPinsOut( IEnumPins ** ppEnum )
{
    HRESULT hr = NOERROR;
    *ppEnum = new CImpIEnumPinsOutOverGraph( pGB, &hr );
    if (!*ppEnum) hr = E_OUTOFMEMORY;
    else if FAILED(hr)
    {
        delete *ppEnum;
        *ppEnum = 0;
    }
    return hr;
}


// Protected methods

// #include <iostream.h>
// #include <SampIOS.h>

// Disconnect all pins.  If a filter has no pins, remove it completely
// This is to part-destroy a filter graph, leaving only those filters
// that may be usefult for rendering a file or pin after this function
// has been called.
HRESULT CAMovie::DisconnectAllAndRemoveIsolates()
{
    // cout << "Pre DisconnectAllAndRemoveIsolates()\n" << pGB << endl;
    HRESULT  hr;

    IEnumFilters * piEnumFilters;
    hr = EnumFilters( &piEnumFilters );
    if SUCCEEDED(hr)
    {
        CGenericList<IPin>        PinList("List of pins to be disconnected");
        CGenericList<IBaseFilter> FilterList("List of filters to be removed");
        IBaseFilter * piFilter;
        IPin * piPin;
        while ( piEnumFilters->Next(1, &piFilter, 0) == NOERROR )
        {
            BOOL connected = FALSE, connectable = FALSE;
            IEnumPins * piEnumPins;
            hr = piFilter->EnumPins( &piEnumPins );
            if SUCCEEDED(hr)
            {
                while ( piEnumPins->Next(1, &piPin, 0) == NOERROR )
                {
                    connectable = TRUE;
                    IPin * piPinConnectedTo;
                    piPin->ConnectedTo( &piPinConnectedTo );
                    if (piPinConnectedTo)
                    {
                        PinList.AddTail( piPin );
                        piPinConnectedTo->Release();
                    }
                    else piPin->Release();
                }
                piEnumPins->Release();
            }
            if (!connectable)   FilterList.AddTail( piFilter );
            else                piFilter->Release();
        }
        piEnumFilters->Release();
        while ( piPin = PinList.RemoveHead() )
        {
            hr = Disconnect( piPin );
            piPin->Release();
        }
        while ( piFilter = FilterList.RemoveHead() )
        {
            hr = RemoveFilter( piFilter );
            piFilter->Release();
        }
    }
    // cout << "Post DisconnectAllAndRemoveIsolates()\n" << pGB << endl;
    return hr;
}

// For all filters in graph, if the filter has pins, and all are unconnected,
// then remove the filter from the graph.
HRESULT CAMovie::RemoveAllUnconnected()
{
    // cout << "Pre RemoveAllUnconnected()\n" << pGB << endl;
    HRESULT  hr;

    IEnumFilters * piEnumFilters;
    hr = EnumFilters( &piEnumFilters );
    if SUCCEEDED(hr)
    {
        CGenericList<IBaseFilter> FilterList("List of filters to be removed");
        IBaseFilter * piFilter;
        while ( piEnumFilters->Next(1, &piFilter, 0) == NOERROR )
        {
            BOOL connected = FALSE, connectable = FALSE;
            IEnumPins * piEnumPins;
            hr = piFilter->EnumPins( &piEnumPins );
            if SUCCEEDED(hr)
            {
                IPin * piPin;
                while ( !connected && piEnumPins->Next(1, &piPin, 0) == NOERROR )
                {
                    connectable = TRUE;
                    IPin * piPinConnectedTo;
                    piPin->ConnectedTo( &piPinConnectedTo );
                    if (piPinConnectedTo)
                    {
                        connected = TRUE;
                        piPinConnectedTo->Release();
                    }
                    piPin->Release();
                }
                piEnumPins->Release();
            }
            if (connectable && !connected)  FilterList.AddTail( piFilter );
            else                            piFilter->Release();
        }
        piEnumFilters->Release();
        while ( piFilter = FilterList.RemoveHead() )
        {
            hr = RemoveFilter( piFilter );
            piFilter->Release();
        }
    }
    // cout << "Post RemoveAllUnconnected()\n" << pGB << endl;
    return hr;
}


//---------------------------------------------------------------------------
//
//  AMovieRegisterInterface()
//
//
//
//---------------------------------------------------------------------------

HRESULT
AMovieRegisterInterface( CLSID  ClsidInterface
                       , LPTSTR strName
                       , CLSID  ClsidDistributor )
{
  TCHAR   achKey[MAX_PATH];
  TCHAR   achClsidInterface[CHARS_IN_GUID];
  TCHAR   achClsidDistributor[CHARS_IN_GUID];
  OLECHAR achTemp[CHARS_IN_GUID];
  HKEY    hkey, hsubkey;
  LONG    lReturn;
  HRESULT hr;

  hr = StringFromGUID2( ClsidInterface
                      , achTemp
                      , CHARS_IN_GUID );
  if( SUCCEEDED(hr) )
  {
    wsprintf( achClsidInterface, "%ls", achTemp );

    hr = StringFromGUID2( ClsidDistributor
                        , achTemp
                        , CHARS_IN_GUID );
  }

  if( FAILED(hr) ) return hr;

  wsprintf( achClsidDistributor, "%ls", achTemp );

  wsprintf( achKey, "Interface\\%s", achClsidInterface );
  lReturn = RegCreateKey( HKEY_CLASSES_ROOT
                        , achKey
                        , &hkey );
  if( ERROR_SUCCESS == lReturn )
  {
    if( NULL != strName )
    {
      RegSetValue( hkey
                 , NULL
                 , REG_SZ
                 , strName
                 , lstrlen(strName)+1 );
    }

    lReturn = RegCreateKey( hkey
                          , "Distributor"
                          , &hsubkey );
    if( ERROR_SUCCESS == lReturn )
    {

      RegSetValue( hsubkey
                 , NULL
                 , REG_SZ
                 , achClsidDistributor
                 , lstrlen(achClsidDistributor)+1 );

      RegCloseKey( hsubkey );
    }

    RegCloseKey( hkey );
  }

  return HRESULT_FROM_WIN32(lReturn);

}


//---------------------------------------------------------------------------
//
// AMovieUnregisterInterface()
//
//
//
//---------------------------------------------------------------------------

HRESULT
AMovieUnregisterInterface( REFCLSID ClsidInterface )
{
  HKEY    hkey;
  LONG    lReturn;
  HRESULT hr;
  TCHAR   achClsidInterface[CHARS_IN_GUID];
  OLECHAR achTemp[CHARS_IN_GUID];

  // comvert CLSID to OLE string
  //
  hr  = StringFromGUID2( ClsidInterface, achTemp, CHARS_IN_GUID );
  if( FAILED(hr) ) return hr;

  // convert OLE string to ANSI (as that's
  // all that WIN95 Reg* calls can handle
  //
  wsprintf( achClsidInterface, "%ls", achTemp );

  // open Interface key
  //
  lReturn = RegOpenKeyEx( HKEY_CLASSES_ROOT
                        , TEXT("Interface")
                        , 0
                        , MAXIMUM_ALLOWED
                        , &hkey );
  if( ERROR_SUCCESS == lReturn )
  {
    // if successful, eliminate subkey and close
    // Interface key
    //
    EliminateSubKey( hkey, achClsidInterface );
    RegCloseKey( hkey );
  }

  return HRESULT_FROM_WIN32( lReturn );
}

//
// exported entry points for registration and
// unregistration.
//
// In this case they (un)register out one and
// only interface and then call through to the
// default implementation.
//
// UUID Interface   {359ace10-7688-11cf-8b23-00805f6cef60}
// Interface name   IAMovie
// UUID Distributor {5f2759c0-7685-11cf-8b23-00805f6cef60}
//
STDAPI DllRegisterServer()
{
  HRESULT hr = NOERROR;

  // Register interface so ActiveMovie can
  // find me from my IID
  //
  hr = AMovieRegisterInterface( IID_IAMovie
                              , TEXT("IAMovie")
                              , CLSID_AMovie  );

  if( SUCCEEDED(hr) )
    hr = AMovieDllRegisterServer2( TRUE );

  return hr;
}

STDAPI DllUnregisterServer()
{
  HRESULT hr = NOERROR;

  hr = AMovieUnregisterInterface( IID_IAMovie );

  if( SUCCEEDED(hr) )
    hr = AMovieDllRegisterServer2( FALSE );

  return hr;
}

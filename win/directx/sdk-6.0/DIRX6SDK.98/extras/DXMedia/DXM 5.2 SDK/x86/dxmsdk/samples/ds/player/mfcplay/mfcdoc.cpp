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
//
// playdoc.cpp : implementation of the CPlayerDoc class
//

#include "stdafx.h"
#include <reftime.h>
#include "mfcplay.h"

#include "mfcdoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPlayerDoc

IMPLEMENT_DYNCREATE(CPlayerDoc, CDocument)

BEGIN_MESSAGE_MAP(CPlayerDoc, CDocument)
	//{{AFX_MSG_MAP(CPlayerDoc)
	ON_UPDATE_COMMAND_UI(ID_MEDIA_PLAY, OnUpdateMediaPlay)
	ON_UPDATE_COMMAND_UI(ID_MEDIA_PAUSE, OnUpdateMediaPause)
	ON_UPDATE_COMMAND_UI(ID_MEDIA_STOP, OnUpdateMediaStop)
	ON_COMMAND(ID_MEDIA_PLAY, OnMediaPlay)
	ON_COMMAND(ID_MEDIA_PAUSE, OnMediaPause)
	ON_COMMAND(ID_MEDIA_STOP, OnMediaStop)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlayerDoc construction/destruction

CPlayerDoc::CPlayerDoc()
{
	// TODO: add one-time construction code here
    m_pGraph = NULL;
    m_State = Uninitialized;
    m_hGraphNotifyEvent = NULL;

    ((CPlayerApp *) AfxGetApp())->OnDocumentCreated( this );
}

BOOL CPlayerDoc::CreateFilterGraph(void) {
    HRESULT hr;	// return code

    ASSERT(m_pGraph == NULL);

    hr = CoCreateInstance(CLSID_FilterGraph, 		// get this documents graph object
                          NULL,
                          CLSCTX_INPROC_SERVER,
                          IID_IGraphBuilder,
                          (void **) &m_pGraph);
    if (FAILED(hr)){
    	m_pGraph = NULL;
        return FALSE;
    }

    // get hold of the event notification handle so we can wait for
    // completion
    IMediaEvent *pME;
    hr = m_pGraph->QueryInterface(IID_IMediaEvent, (void **) &pME);
    if (FAILED(hr)) {
        DeleteContents();
        return FALSE;
    }

    hr = pME->GetEventHandle((OAEVENT*) &m_hGraphNotifyEvent);

    pME->Release();

    if (FAILED(hr)) {
        DeleteContents();
        return FALSE;
    }

    return TRUE;
}

void CPlayerDoc::DeleteContents( void ){

    if (m_pGraph != NULL) {
        m_pGraph->Release();
        m_pGraph = NULL;
    }


    // this event is owned by the filtergraph and thus is no longer valid
    m_hGraphNotifyEvent = NULL;

    m_State = Uninitialized;
}

CPlayerDoc::~CPlayerDoc()
{
    DeleteContents();

    ((CPlayerApp *) AfxGetApp())->OnDocumentDestroyed( this );
}


/////////////////////////////////////////////////////////////////////////////
// CPlayerDoc diagnostics

#ifdef _DEBUG
void CPlayerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPlayerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPlayerDoc commands

void CPlayerDoc::OnUpdateMediaPlay(CCmdUI* pCmdUI)
{
    pCmdUI->Enable( CanPlay() );
	
}

void CPlayerDoc::OnUpdateMediaPause(CCmdUI* pCmdUI)
{
    pCmdUI->Enable( CanPause() );
	
}

void CPlayerDoc::OnUpdateMediaStop(CCmdUI* pCmdUI)
{
    pCmdUI->Enable( CanStop() );
	
}

void CPlayerDoc::OnMediaPlay()
{
    if( CanPlay() ){
        HRESULT	hr;
        IMediaControl *pMC;

        // Obtain the interface to our filter graph
        hr = m_pGraph->QueryInterface(IID_IMediaControl, (void **) &pMC);

        if( SUCCEEDED(hr) ){
            // Ask the filter graph to play and release the interface

            // default behaviour is to carry on from where we stopped last
            // time.
            //
            // if you want it to do this, but rewind at the end then
            // define REWIND.
            // Otherwise you probably want to always start from the
            // beginning -> define FROM_START (in OnMediaStop)
#undef REWIND
#define FROM_START

#ifdef REWIND
            IMediaPosition * pMP;
            hr = m_pGraph->QueryInterface(IID_IMediaPosition, (void**) &pMP);
            if (SUCCEEDED(hr)) {
                // start from last position, but rewind if near the
                // end
                REFTIME tCurrent, tLength;
                hr = pMP->get_Duration(&tLength);
                if (SUCCEEDED(hr)) {
                    hr = pMP->get_CurrentPosition(&tCurrent);
                    if (SUCCEEDED(hr)) {
                        // within 1sec of end? (or past end?)
                        if ((tLength - tCurrent) < 1) {
                            pMP->put_CurrentPosition(0);
                        }
                    }
                }
                pMP->Release();
            }
#endif


            hr = pMC->Run();
            pMC->Release();

            if( SUCCEEDED(hr) ){
                m_State=Playing;
                return;
            }
        }

        // Inform the user that an error occurred
        AfxMessageBox(IDS_CANT_PLAY);

    }	
}

void CPlayerDoc::OnMediaPause()
{
	if( CanPause() ){
        HRESULT	hr;
        IMediaControl *pMC;

        // Obtain the interface to our filter graph
        hr = m_pGraph->QueryInterface(IID_IMediaControl, (void **) &pMC);

        if( SUCCEEDED(hr) ){
            // Ask the filter graph to pause and release the interface
            hr = pMC->Pause();
            pMC->Release();

            if( SUCCEEDED(hr) ){
                m_State = Paused;
                return;
            }
        }

        // Inform the user that an error occurred
        AfxMessageBox(IDS_CANT_PAUSE);
    }
	
}

// stop the graph without rewinding
void CPlayerDoc::OnAbortStop()
{
	if( CanStop() ){
        HRESULT	hr;
        IMediaControl *pMC;

        // Obtain the interface to our filter graph
        hr = m_pGraph->QueryInterface(IID_IMediaControl, (void **) &pMC);

        if( SUCCEEDED(hr) ){
            // Ask the filter graph to stop and release the interface
            hr = pMC->Stop();
#ifdef FROM_START
            // if we want always to play from the beginning
            // then we should seek back to the start here
            // (on app or user stop request, and also after EC_COMPLETE).
            IMediaPosition * pMP;
            hr = m_pGraph->QueryInterface(IID_IMediaPosition, (void**) &pMP);
            if (SUCCEEDED(hr)) {
                pMP->put_CurrentPosition(0);
                pMP->Release();
            }

            // no visible rewind or we will re-show the window!

#endif

            pMC->Release();

            if( SUCCEEDED(hr) ){
                m_State = Stopped;
                return;
            }
        }

        // Inform the user that an error occurred
        AfxMessageBox(IDS_CANT_STOP);
	}
}

// There are two different ways to stop a graph. We can stop and then when we
// are later paused or run continue from the same position. Alternatively the
// graph can be set back to the start of the media when it is stopped to have
// a more CDPLAYER style interface. These are both offered here conditionally
// compiled using the FROM_START definition. The main difference is that in
// the latter case we put the current position to zero while we change states

void CPlayerDoc::OnMediaStop()
{
    if( CanStop() ){
        HRESULT	hr;
        IMediaControl *pMC;

        // Obtain the interface to our filter graph
        hr = m_pGraph->QueryInterface(IID_IMediaControl, (void **) &pMC);
        if( SUCCEEDED(hr) ){

#ifdef FROM_START
            IMediaPosition * pMP;
            OAFilterState state;

            // Ask the filter graph to pause
            hr = pMC->Pause();

            // if we want always to play from the beginning
            // then we should seek back to the start here
            // (on app or user stop request, and also after EC_COMPLETE).
            hr = m_pGraph->QueryInterface(IID_IMediaPosition,
                                          (void**) &pMP);
            if (SUCCEEDED(hr)) {
                pMP->put_CurrentPosition(0);
                pMP->Release();
            }

            // wait for pause to complete
            pMC->GetState(INFINITE, &state);
#endif
            // now really do the stop
            pMC->Stop();
            pMC->Release();
            m_State = Stopped;
            return;
        }
        // Inform the user that an error occurred
        AfxMessageBox(IDS_CANT_STOP);
    }
}


BOOL CPlayerDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
    WCHAR	wPath[MAX_PATH];

    DeleteContents();

    if ( !CreateFilterGraph() ) {
        AfxMessageBox(IDS_CANT_INIT_QUARTZ);
        return FALSE;
    }

    MultiByteToWideChar( CP_ACP, 0, lpszPathName,
                         -1, wPath, MAX_PATH );


    if (FAILED( m_pGraph->RenderFile(wPath, NULL) )) {
        AfxMessageBox(IDS_CANT_RENDER_FILE);
        return FALSE;
    }

    m_State = Stopped;

    return TRUE;
}

//
// If the event handle is valid, ask the graph
// if anything has happened. eg the graph has stopped...
void CPlayerDoc::OnGraphNotify(void) {
    IMediaEvent *pME;
    long lEventCode, lParam1, lParam2;

    ASSERT( m_hGraphNotifyEvent != NULL );

    if( SUCCEEDED(m_pGraph->QueryInterface(IID_IMediaEvent, (void **) &pME))) {
        if( SUCCEEDED(pME->GetEvent(&lEventCode, &lParam1, &lParam2, 0))) {

            // if this is a normal stop, then we do a rewind as
            // we would if the user pressed stop. For an abort,
            // we don't touch this.
            if (lEventCode == EC_COMPLETE) {
                OnMediaStop();
            } else if ((lEventCode == EC_ERRORABORT) ||
                       (lEventCode == EC_USERABORT)) {

                // put the graph into stop mode but don't mess with it.
                OnAbortStop();
            }
        }
    	pME->Release();
	}

    // The toolbar will only be updated after a message has been received,
    // so send a dummy message
    PostMessage( AfxGetMainWnd()->m_hWnd, WM_WAKEUP, 0, 0 );

}


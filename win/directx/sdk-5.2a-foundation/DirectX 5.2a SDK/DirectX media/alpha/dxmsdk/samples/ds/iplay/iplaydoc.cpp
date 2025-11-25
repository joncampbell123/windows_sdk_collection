// This code and information is provided "as is" without warranty of
// any kind, either expressed or implied, including but not limited to
// the implied warranties of merchantability and/or fitness for a
// particular purpose.

// Copyright (C) 1996 - 1997 Intel corporation.  All rights reserved.

// IPlaydoc.cpp : implementation of the CIPlayDoc class
//

#include "stdafx.h"
#include "IPlay.h"
#include "IPlaydoc.h"

#include <initguid.h>       // OLE (Quartz) initialization

#include "ax_spec.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIPlayDoc

IMPLEMENT_DYNCREATE(CIPlayDoc, CDocument)

#define new DEBUG_NEW //Checking for mem leaks in debug mode

BEGIN_MESSAGE_MAP(CIPlayDoc, CDocument)
	//{{AFX_MSG_MAP(CIPlayDoc)
	ON_COMMAND(ID_MEDIA_LOOP, OnMediaLoop)
	ON_COMMAND(ID_MEDIA_PAUSE, OnMediaPause)
	ON_COMMAND(ID_MEDIA_ZOOMX2, OnMediaZoomx2)
	ON_UPDATE_COMMAND_UI(ID_MEDIA_PAUSE, OnUpdateMediaPause)
	ON_UPDATE_COMMAND_UI(ID_MEDIA_PLAY, OnUpdateMediaPlay)
	ON_UPDATE_COMMAND_UI(ID_MEDIA_STOP, OnUpdateMediaStop)
	ON_UPDATE_COMMAND_UI(ID_MEDIA_ZOOMX2, OnUpdateMediaZoomx2)
	ON_UPDATE_COMMAND_UI(ID_MEDIA_LOOP, OnUpdateMediaLoop)
	ON_COMMAND(ID_MEDIA_PLAY, OnMediaPlay)
	ON_COMMAND(ID_MEDIA_STOP, OnMediaStop)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

R4_DEC_SEQ_DATA   m_r4SeqData;  // Indeo data structures
R4_DEC_FRAME_DATA m_r4FrameData;

/////////////////////////////////////////////////////////////////////////////
// CIPlayDoc construction/destruction

CIPlayDoc::CIPlayDoc()
{
	m_pGraph      = NULL;
	m_hGraphEvent = NULL;
	m_pIndeo      = NULL;
	m_lpstrPath[0] = '\0';
	m_bLoop       = FALSE;
	m_bZoom		  = FALSE;
	m_lWidth	  = 0;
	m_lHeight	  = 0;
    m_State       = Uninitialized;

	// Initialize the headers for the Indeo data structures
	// This stuff never changes (except for dwFlags)
	m_r4FrameData.dwSize = sizeof(R4_DEC_FRAME_DATA);
	m_r4FrameData.dwFourCC = FOURCC_IV41;
	m_r4FrameData.dwVersion = SPECIFIC_INTERFACE_VERSION;
	m_r4FrameData.oeEnvironment = OE_32;
	m_r4FrameData.dwFlags = 0;

	m_r4SeqData.dwSize = sizeof(R4_DEC_SEQ_DATA);
	m_r4SeqData.dwFourCC = FOURCC_IV41;
	m_r4SeqData.dwVersion = SPECIFIC_INTERFACE_VERSION;
	m_r4SeqData.oeEnvironment = OE_32;
	m_r4SeqData.dwFlags = 0;

    ((CIPlayApp *) AfxGetApp())->OnDocumentCreated( this );
}

CIPlayDoc::~CIPlayDoc()
{
	DeleteContents();

	((CIPlayApp *) AfxGetApp())->OnDocumentDestroyed( this );

}


/////////////////////////////////////////////////////////////////////////////
// CIPlayDoc diagnostics

#ifdef _DEBUG
void CIPlayDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CIPlayDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CIPlayDoc Message Handlers

BOOL CIPlayDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
    WCHAR	     wPath[MAX_PATH];

	char szDrive[ _MAX_DRIVE ] ;	
	char szDir[ _MAX_DIR ];			
	char szFname[ _MAX_FNAME ];		
	char szExt[ _MAX_EXT ];		

	IBaseFilter  *pFilter;  // EnumFilters vars
	IEnumFilters *pEnum;
	ULONG        pcFetched;
	HRESULT      hr;
//	HWND       	 hVidWin;

	void *pIF;	  // generic interface to graph object

	// Get rid of any previously opened graph.
    DeleteContents();

	// Create a new filter graph object.
    if ( !CreateFilterGraph() ) {
        AfxMessageBox(IDS_CANT_INIT_QUARTZ);
        return FALSE;
    }

	// Render the file using the new filter graph.
	// This will add the necessary filters to the
	// graph (which up til now was empty).
	strcpy(m_lpstrPath, lpszPathName);
    MultiByteToWideChar( CP_ACP, 0, lpszPathName,
                         -1, wPath, MAX_PATH );

    hr = m_pGraph->QueryInterface(IID_IGraphBuilder, &pIF);

    if (FAILED( ((IGraphBuilder *)pIF)->RenderFile(wPath, NULL) )) {
        AfxMessageBox(IDS_CANT_RENDER_FILE);
        return FALSE;
    }
	((IGraphBuilder *)pIF)->Release();

	
	// Position and set the title of the video window.
	_splitpath( lpszPathName, szDrive, szDir, szFname, szExt );		
	MultiByteToWideChar( CP_ACP, 0, CString(szFname) + szExt,-1,	
						 wPath, _MAX_FNAME + _MAX_EXT);
	
	
	SetWindow((BSTR)wPath);
	
	// Get and save the native video size -- used for 
	// zoom by 2.
	hr = m_pGraph->QueryInterface(IID_IBasicVideo, &pIF);

    if( SUCCEEDED(hr) ){
		((IBasicVideo *)pIF)->GetVideoSize(&m_lWidth, &m_lHeight);
		((IBasicVideo *)pIF)->Release();
	}

	// Call the stop function to queue up the first
	// frame of the clip.  Have to set the state first
	// to Paused so we can transistion to Stopped.
	m_State = Paused;
	OnMediaStop();

	// See if the Indeo codec filter is in the graph.
	hr = m_pGraph->EnumFilters(&pEnum);

	if (FAILED(hr))
		return TRUE;

	while ((hr = pEnum->Next(1, &pFilter, &pcFetched)) == S_OK)
	{
		hr = pFilter->QueryInterface(IID_IIndeoDecode, &m_pIndeo);
		pFilter->Release();
		if (hr == S_OK)
			break;

	}
	// This interface needs to be released, even though
	// it was not obtained in the usual way with QueryInterface.
	pEnum->Release(); 
	
	// Someday, the following line can replace the above section of code
	// that enumerates the filters in the graph to find the 
	// IIndeoDecode interface, but apparently that someday is not today.

	//hr = m_pGraph->QueryInterface(IID_IIndeoDecode, &m_pIndeo);

	return TRUE;
}

void CIPlayDoc::OnGraphNotify(void) {
    IMediaEvent *pME;
    long lEventCode, lParam1, lParam2;

    ASSERT( m_hGraphEvent != NULL );

    if( SUCCEEDED(m_pGraph->QueryInterface(IID_IMediaEvent, (void **) &pME))){
        if( SUCCEEDED(pME->GetEvent(&lEventCode, &lParam1, &lParam2, 0)) )  {

			switch (lEventCode) {

            	case EC_COMPLETE:
          		  	OnMediaStop();
					if (m_bLoop) OnMediaPlay();
					break;
            	case EC_USERABORT:
            	case EC_ERRORABORT:
            	    OnMediaStop();
        	}
		}

    	pME->Release();
	}

    // Send a message so toolbar gets updated
    PostMessage( AfxGetMainWnd()->m_hWnd, WM_USER, 0, 0 );

}

void CIPlayDoc::OnMediaLoop() 
{
	m_bLoop = !m_bLoop;
	
}

void CIPlayDoc::OnMediaPause() 
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
	}
	
}

void CIPlayDoc::OnMediaPlay() 
{
    if( CanPlay() ){
        HRESULT	hr;
        IMediaControl *pMC;

        // Obtain the interface to our filter graph
        hr = m_pGraph->QueryInterface(IID_IMediaControl, (void **) &pMC);

        if( SUCCEEDED(hr) ){
            // Ask the filter graph to play and release the interface
            hr = pMC->Run();
            pMC->Release();

            if( SUCCEEDED(hr) ){
                m_State=Playing;
				// Let Indeo view object know the state changed
				UpdateAllViews(NULL);
                return;
            }
        }
	}
	
}

void CIPlayDoc::OnMediaStop() 
{
	if( CanStop() ){
        HRESULT	hr;
        IMediaControl *pMC;
		IMediaPosition *pMP;
		OAFilterState fstate;

        // Obtain the interface to our filter graph
        hr = m_pGraph->QueryInterface(IID_IMediaControl, (void **) &pMC);

        if( SUCCEEDED(hr) ){
            // Ask the filter graph to stop and release the interface

			// put graph in stopped state
            hr = pMC->Stop();  

			// Rewind the stream
			hr = m_pGraph->QueryInterface(IID_IMediaPosition, (void **) &pMP);
			hr = pMP->put_CurrentPosition(0);
			hr = pMP->Release();

			// Flush the graph
			hr = pMC->Pause();

			// Wait for state transition to complete
			hr = pMC->GetState(5000, &fstate);

			// Now back to stopped state
			hr = pMC->Stop();

            pMC->Release();

            if( SUCCEEDED(hr) ){
                m_State = Stopped;
				// Let Indeo view object know the state changed
				UpdateAllViews(NULL);
                return;
            }
        }
	}
	
}

void CIPlayDoc::OnMediaZoomx2() 
{
	// This function doubles the width & height of the original video
	// size -- depending on the hardware, this may result in a
	// better quality video than a random stretch of the video window.
	// Subsequent calls toggle the video size between the original size 
	// and the "x2" size, ignoring the fact that the user may have 
	// resized the window using the window frame.  The function attempts to 
	// keep the video window's center in place (zooms/unzooms from top,
	// bottom, right and left).

	HRESULT hr;
	void *pIF;
	long x,y,width,height,width2,height2;

	m_bZoom = !m_bZoom;
	
	hr = m_pGraph->QueryInterface(IID_IVideoWindow, &pIF);

    if( SUCCEEDED(hr) ){
		if (m_bZoom) {
			// Set the width & height to twice the native video size
			// Allow for the window frame -- we really want to just
			// double the size of the client area.
			width2 = (m_lWidth * 2) + (m_lWinWidth - m_lWidth);
			height2 = (m_lHeight * 2) + (m_lWinHeight - m_lHeight);
			((IVideoWindow *)pIF)->GetWindowPosition(&x, &y, &width, &height);
			((IVideoWindow *)pIF)->SetWindowPosition(x + (width - width2)/2,
				y + (height - height2)/2, width2, height2);
		}
		else {
			// Restore original size
			((IVideoWindow *)pIF)->GetWindowPosition(&x, &y, &width, &height);
			((IVideoWindow *)pIF)->SetWindowPosition(x +
				(width - m_lWinWidth)/2, y + (height - m_lWinHeight)/2,
				m_lWinWidth, m_lWinHeight);
		} 

		((IVideoWindow *)pIF)->Release();

	}
}

void CIPlayDoc::OnUpdateMediaPause(CCmdUI* pCmdUI) 
{
    pCmdUI->Enable( CanPause() );
	
}

void CIPlayDoc::OnUpdateMediaPlay(CCmdUI* pCmdUI) 
{
    pCmdUI->Enable( CanPlay() );
	
}

void CIPlayDoc::OnUpdateMediaStop(CCmdUI* pCmdUI) 
{
    pCmdUI->Enable( CanStop() );
	
}

void CIPlayDoc::OnUpdateMediaLoop(CCmdUI* pCmdUI) 
{
    pCmdUI->Enable( IsInitialized() );
	pCmdUI->SetCheck( m_bLoop );
	
}

void CIPlayDoc::OnUpdateMediaZoomx2(CCmdUI* pCmdUI) 
{
    pCmdUI->Enable( IsInitialized() );
	pCmdUI->SetCheck( m_bZoom );
	
}

/////////////////////////////////////////////////////////////////////////////
// CIPlayDoc Commands

BOOL CIPlayDoc::IsIndeo()
{
	return (m_pIndeo != NULL);
}

void CIPlayDoc::GetTransFillRGB(int& red, int& green, int& blue)
{
	HRESULT hr;

	m_r4FrameData.mtType = MT_DECODE_FRAME_VALUE;
	
	m_r4FrameData.dwFlags = DECFRAME_FILL_COLOR |
							DECFRAME_VALID;
	
	m_r4FrameData.dwFillColor = DECFRAME_FILL_UNDEFINED;					

	hr = ((IIndeoDecode *)m_pIndeo)->get_DecodeFrame(&m_r4FrameData);

	if (FAILED( hr )) {
        AfxMessageBox(IDS_CANT_GET_FRAME_DATA);
		return;
	}
	
	if (m_r4FrameData.dwFillColor & DECFRAME_FILL_UNDEFINED) {
		red   = 0;
		green =	0;
		blue  =	0;
	} else { // Fill color is valid
		red   = (m_r4FrameData.dwFillColor&0x00FF0000)>>16;
		green =	(m_r4FrameData.dwFillColor&0x0000FF00)>>8;
		blue  =	(m_r4FrameData.dwFillColor&0x000000FF);
	}
	
	return;
}


void CIPlayDoc::GetBCS(int& b, int& c, int& s)
{
	HRESULT hr;

	m_r4FrameData.mtType = MT_DECODE_FRAME_VALUE;
	m_r4FrameData.dwFlags = DECFRAME_BRIGHTNESS |
							DECFRAME_CONTRAST |
							DECFRAME_SATURATION |
							DECFRAME_VALID;

	hr = ((IIndeoDecode *)m_pIndeo)->get_DecodeFrame(&m_r4FrameData);

	if (FAILED( hr )) {
        AfxMessageBox(IDS_CANT_GET_FRAME_DATA);
		return;
	}

	b = LOWORD(m_r4FrameData.lBrightness);
	c = LOWORD(m_r4FrameData.lContrast);
	s = LOWORD(m_r4FrameData.lSaturation);

	return;
}

void CIPlayDoc::GetDecodeTime(DWORD& time)
{
	HRESULT hr;

	m_r4FrameData.mtType = MT_DECODE_FRAME_VALUE;
	m_r4FrameData.dwFlags = DECFRAME_TIME_LIMIT | DECFRAME_VALID;

	hr = ((IIndeoDecode *)m_pIndeo)->get_DecodeFrame(&m_r4FrameData);

	if (FAILED( hr )) {
        AfxMessageBox(IDS_CANT_SET_FRAME_DATA);
		return;
	}

	time = m_r4FrameData.dwTimeLimit;

	return;
}


void CIPlayDoc::GetDecodeRect(DWORD& x, DWORD& y, DWORD& width, DWORD& height)
{
	HRESULT hr;

	m_r4FrameData.mtType = MT_DECODE_FRAME_VALUE;
	m_r4FrameData.dwFlags = DECFRAME_DECODE_RECT | DECFRAME_VALID;

	hr = ((IIndeoDecode *)m_pIndeo)->get_DecodeFrame(&m_r4FrameData);

	if (FAILED( hr )) {
        AfxMessageBox(IDS_CANT_GET_FRAME_DATA);
		return;
	}

	x = m_r4FrameData.rDecodeRect.dwX;
	y = m_r4FrameData.rDecodeRect.dwY;
	width = m_r4FrameData.rDecodeRect.dwWidth;
	height = m_r4FrameData.rDecodeRect.dwHeight;

	return;
}

void CIPlayDoc::GetViewRect(DWORD& x, DWORD& y, DWORD& width, DWORD& height)
{
	HRESULT hr;

	m_r4FrameData.mtType = MT_DECODE_FRAME_VALUE;
	m_r4FrameData.dwFlags = DECFRAME_VIEW_RECT | DECFRAME_VALID;

	hr = ((IIndeoDecode *)m_pIndeo)->get_DecodeFrame(&m_r4FrameData);

	if (FAILED( hr )) {
        AfxMessageBox(IDS_CANT_GET_FRAME_DATA);
		return;
	}

	x = m_r4FrameData.rViewRect.dwX;
	y = m_r4FrameData.rViewRect.dwY;
	width = m_r4FrameData.rViewRect.dwWidth;
	height = m_r4FrameData.rViewRect.dwHeight;

	return;
}

void CIPlayDoc::GetSequenceOptions(BOOL& altline, BOOL& ddframes, BOOL& ddquality, BOOL& trans, BOOL& usekey,
	DWORD& key)
{
	HRESULT hr;

	m_r4SeqData.mtType = MT_DECODE_SEQ_VALUE;
	m_r4SeqData.dwFlags =	DECSEQ_KEY |
							DECSEQ_SCALABILITY |
							DECSEQ_FILL_TRANSPARENT |
							DECSEQ_ALT_LINE |
							DECSEQ_VALID;

	hr = ((IIndeoDecode *)m_pIndeo)->get_DecodeSequence(&m_r4SeqData);

	if (FAILED( hr )) {
        AfxMessageBox(IDS_CANT_GET_SEQ_DATA);
		return;
	}

	altline = m_r4SeqData.fAltLine;
    
	switch (m_r4SeqData.eScalability)
	{
		case SC_ON:
			ddframes = ddquality = FALSE;
			break;
		case SC_OFF:
			ddframes = ddquality = TRUE;
			break;
		case SC_DONT_DROP_FRAMES:
			ddframes = TRUE;
			ddquality = FALSE;
			break;
		case SC_DONT_DROP_QUALITY:
			ddframes = FALSE;
			ddquality = TRUE;
			break;
	}

	trans = m_r4SeqData.fFillTransparentPixels;
	usekey = m_r4SeqData.fEnabledKey;
	key = m_r4SeqData.dwKey;

	return;
}

void CIPlayDoc::SetTransFillRGB(DWORD rgb)
{
	HRESULT hr;

	m_r4FrameData.mtType = MT_DECODE_FRAME_VALUE;
	m_r4FrameData.dwFlags = DECFRAME_VALID | DECFRAME_FILL_COLOR;
	
	m_r4FrameData.dwFillColor = rgb;

	hr = ((IIndeoDecode *)m_pIndeo)->set_DecodeFrame(&m_r4FrameData);

	if (FAILED( hr )) 
        AfxMessageBox(IDS_CANT_SET_FRAME_DATA);
	
	return;
}

void CIPlayDoc::SetBCS(int b, int c, int s)
{
	HRESULT hr;

	m_r4FrameData.mtType = MT_DECODE_FRAME_VALUE;
	m_r4FrameData.dwFlags = DECFRAME_BRIGHTNESS |
							DECFRAME_CONTRAST |
							DECFRAME_SATURATION |
							DECFRAME_VALID;

	m_r4FrameData.lBrightness = b;
	m_r4FrameData.lContrast = c;
	m_r4FrameData.lSaturation = s;

	hr = ((IIndeoDecode *)m_pIndeo)->set_DecodeFrame(&m_r4FrameData);

	if (FAILED( hr )) 
        AfxMessageBox(IDS_CANT_SET_FRAME_DATA);

	return;
}


void CIPlayDoc::SetDecodeTime(DWORD time)
{
	HRESULT hr;

	m_r4FrameData.mtType = MT_DECODE_FRAME_VALUE;
	m_r4FrameData.dwFlags = DECFRAME_TIME_LIMIT | DECFRAME_VALID;

	m_r4FrameData.dwTimeLimit = time;

	hr = ((IIndeoDecode *)m_pIndeo)->set_DecodeFrame(&m_r4FrameData);

	if (FAILED( hr )) 
        AfxMessageBox(IDS_CANT_SET_FRAME_DATA);

	return;
}

void CIPlayDoc::SetDecodeRect(DWORD x, DWORD y, DWORD width, DWORD height)
{
	HRESULT hr;
	
	m_r4FrameData.mtType = MT_DECODE_FRAME_VALUE;
	
	m_r4FrameData.dwFlags = DECFRAME_DECODE_RECT |	 
							DECFRAME_VALID;
		
	m_r4FrameData.rDecodeRect.dwX = x;
	m_r4FrameData.rDecodeRect.dwY = y;
	m_r4FrameData.rDecodeRect.dwWidth = width;
	m_r4FrameData.rDecodeRect.dwHeight = height;

	m_r4FrameData.rDecodeRect.dwX = x;
	m_r4FrameData.rDecodeRect.dwY = y;
	m_r4FrameData.rDecodeRect.dwWidth = width;
	m_r4FrameData.rDecodeRect.dwHeight = height;
		
	hr = ((IIndeoDecode *)m_pIndeo)->set_DecodeFrame(&m_r4FrameData);

	if (FAILED( hr )) 
        AfxMessageBox(IDS_CANT_SET_FRAME_DATA);

	return;
}

void CIPlayDoc::SetViewRect(BOOL view ,DWORD x, DWORD y, DWORD width, DWORD height)
{
	HRESULT hr;

	m_r4FrameData.mtType = MT_DECODE_FRAME_VALUE;
	m_r4FrameData.dwFlags = DECFRAME_VIEW_RECT | 
							DECFRAME_VIEW_RECT_ORIGIN |
							DECFRAME_VALID;
	
	m_r4FrameData.fViewRectOrigin = view;
	
	m_r4FrameData.rViewRect.dwX = x;
	m_r4FrameData.rViewRect.dwY = y;
	m_r4FrameData.rViewRect.dwWidth = width;
	m_r4FrameData.rViewRect.dwHeight = height;

	hr = ((IIndeoDecode *)m_pIndeo)->set_DecodeFrame(&m_r4FrameData);

	if (FAILED( hr )) 
        AfxMessageBox(IDS_CANT_SET_FRAME_DATA);

	return;
}

void CIPlayDoc::SetSequenceOptions(BOOL altline, LONG scal, BOOL fill, BOOL usekey, DWORD key)
{
	HRESULT hr;

	m_r4SeqData.mtType = MT_DECODE_SEQ_VALUE;
	m_r4SeqData.dwFlags =	DECSEQ_KEY |
							DECSEQ_SCALABILITY |
							DECSEQ_FILL_TRANSPARENT |
							DECSEQ_ALT_LINE |
							DECSEQ_VALID;

	m_r4SeqData.fAltLine = altline;
	m_r4SeqData.eScalability = scal;
	m_r4SeqData.fFillTransparentPixels = fill;
	m_r4SeqData.fEnabledKey = usekey;
	m_r4SeqData.dwKey = key;

	hr = ((IIndeoDecode *)m_pIndeo)->set_DecodeSequence(&m_r4SeqData);

	if (FAILED( hr )) 
        AfxMessageBox(IDS_CANT_SET_SEQ_DATA);

	return;
}

void CIPlayDoc::GetFrameDefaults(int& b, int& c, int& s, DWORD& time,
					  DWORD& dx, DWORD& dy, DWORD& dWidth, DWORD& dHeight,
					  DWORD& vx, DWORD& vy, DWORD& vWidth, DWORD& vHeight)
{
	HRESULT hr;

	m_r4FrameData.mtType = MT_DECODE_FRAME_DEFAULT;
	m_r4FrameData.dwFlags = DECFRAME_BRIGHTNESS |
							DECFRAME_CONTRAST |
							DECFRAME_SATURATION | 
							DECFRAME_TIME_LIMIT |
							DECFRAME_DECODE_RECT |
							DECFRAME_VIEW_RECT |
							DECFRAME_VIEW_RECT_ORIGIN |
							DECFRAME_VALID;

	hr = ((IIndeoDecode *)m_pIndeo)->get_DecodeFrame(&m_r4FrameData);	
	
	if (FAILED( hr )) {
        AfxMessageBox(IDS_CANT_GET_FRAME_DATA);
		return;
	}

	b = LOWORD(m_r4FrameData.lBrightness);
	c = LOWORD(m_r4FrameData.lContrast);
	s = LOWORD(m_r4FrameData.lSaturation);
	
	
	time = m_r4FrameData.dwTimeLimit;

	dx = m_r4FrameData.rDecodeRect.dwX;
	dy = m_r4FrameData.rDecodeRect.dwY;
	dWidth = m_r4FrameData.rDecodeRect.dwWidth;
	dHeight = m_r4FrameData.rDecodeRect.dwHeight;
	
	vx = m_r4FrameData.rViewRect.dwX;
	vy = m_r4FrameData.rViewRect.dwY;
	vWidth = m_r4FrameData.rViewRect.dwWidth;
	vHeight = m_r4FrameData.rViewRect.dwHeight;

	return;
}

void CIPlayDoc::GetSeqDefaults(BOOL& altline, BOOL& ddframes, BOOL& ddquality, BOOL& trans, BOOL& usekey,
	DWORD& key)
{
	HRESULT hr;

	m_r4SeqData.mtType = MT_DECODE_SEQ_DEFAULT;
	m_r4SeqData.dwFlags =	DECSEQ_KEY |
							DECSEQ_SCALABILITY |
							DECSEQ_FILL_TRANSPARENT |
							DECSEQ_ALT_LINE |
							DECSEQ_VALID;

	hr = ((IIndeoDecode *)m_pIndeo)->get_DecodeSequence(&m_r4SeqData);

	if (FAILED( hr )) {
        AfxMessageBox(IDS_CANT_GET_SEQ_DATA);
		return;
	}

	altline = m_r4SeqData.fAltLine;
    
	switch (m_r4SeqData.eScalability)
	{
		case SC_ON:
			ddframes = ddquality = FALSE;
			break;
		case SC_OFF:
			ddframes = ddquality = TRUE;
			break;
		case SC_DONT_DROP_FRAMES:
			ddframes = TRUE;
			ddquality = FALSE;
			break;
		case SC_DONT_DROP_QUALITY:
			ddframes = FALSE;
			ddquality = TRUE;
			break;
	}
	
	trans = m_r4SeqData.fFillTransparentPixels;
	usekey = m_r4SeqData.fEnabledKey;
	key = m_r4SeqData.dwKey;
}

ULONG CIPlayDoc::VideoWidth()
{
	HRESULT hr;
	IBasicVideo *pIF;
	long width;

    hr = m_pGraph->QueryInterface(IID_IBasicVideo, (void **)&pIF);

    if( FAILED(hr) )	 {
		AfxMessageBox(IDS_CANT_GET_VIDEO_INTERFACE);
		return 0;
	}

	pIF->get_DestinationWidth(&width);
	pIF->Release();

	return width;

}

ULONG CIPlayDoc::VideoHeight()
{
	HRESULT hr;
	IBasicVideo *pIF;
	long height;

    hr = m_pGraph->QueryInterface(IID_IBasicVideo, (void **)&pIF);

    if( FAILED(hr) )	 {
		AfxMessageBox(IDS_CANT_GET_VIDEO_INTERFACE);
		return 0;
	}

	pIF->get_DestinationHeight(&height);
	pIF->Release();

	return height;

}

/////////////////////////////////////////////////////////////////////////////
// CIPlayDoc protected funtions

BOOL CIPlayDoc::CreateFilterGraph(void) {
    HRESULT hr;	// return code


    ASSERT(m_pGraph == NULL);

	// Create this document's filter graph object
    hr = CoCreateInstance(CLSID_FilterGraph, 	
                          NULL,
                          CLSCTX_INPROC_SERVER,
                          IID_IFilterGraph,
                          (void **) &m_pGraph);


    if (FAILED(hr)){
    	m_pGraph = NULL;
        return FALSE;
    }

    // get media event handle 
    IMediaEvent *pME;
    hr = m_pGraph->QueryInterface(IID_IMediaEvent, (void **) &pME);
    if (FAILED(hr)) {
        DeleteContents();
        return FALSE;
    }

    hr = pME->GetEventHandle((OAEVENT*) &m_hGraphEvent);

    pME->Release();

    if (FAILED(hr)) {
        DeleteContents();
        return FALSE;
    }
    return TRUE;
}

void CIPlayDoc::DeleteContents() 
{
	// Release all interfaces to graph
	// and filters.

	if (m_pIndeo != NULL) {
	 	((IIndeoDecode *)m_pIndeo)->Release();
		m_pIndeo = NULL;
	}

    if (m_pGraph != NULL) {
        m_pGraph->Release();
        m_pGraph = NULL;
	}

	m_hGraphEvent = NULL;

    m_State = Uninitialized;
}

void CIPlayDoc::SetWindow(BSTR wPath)
{
	HRESULT hr;
	IVideoWindow *pVidWin;
	RECT rDeskTop;
	int x, y;

    hr = m_pGraph->QueryInterface(IID_IVideoWindow, (void **)&pVidWin);

    if( FAILED(hr) )	 {
		AfxMessageBox(IDS_CANT_GET_WINDOW_INTERFACE);
		return;
	}

	// Set the caption of the video window
	pVidWin->put_Caption(wPath);

	// Save the original size for zoom x 2
	pVidWin->get_Width(&m_lWinWidth);
	pVidWin->get_Height(&m_lWinHeight);

	// Center the window on the screen
   	GetWindowRect(GetDesktopWindow(), &rDeskTop);

    if (  m_lWinWidth < (rDeskTop.right - rDeskTop.left) ) 
   		x = ((rDeskTop.right - rDeskTop.left) - m_lWinWidth) / 2;
	else
		x = rDeskTop.left;

	if ( m_lWinHeight < (rDeskTop.bottom - rDeskTop.top) ) 
		y = ((rDeskTop.bottom - rDeskTop.top) - m_lWinHeight) / 2;
	else
		y = rDeskTop.top;

	pVidWin->SetWindowPosition(x, y, m_lWinWidth, m_lWinHeight);

	pVidWin->Release();
}

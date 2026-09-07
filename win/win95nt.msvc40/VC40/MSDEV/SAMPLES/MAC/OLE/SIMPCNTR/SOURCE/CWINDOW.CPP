#include "hdrs.h"
#include "mycoguid.h"

#define 	WND_OFFSET		7

ASSERTDATA


CWindow::CWindow(void)
{

	m_pWindow 	    = nil;
	m_pOleClientSite = NULL;
	m_pObjectSite	= NULL;
	m_Click			= TickCount();
}


CWindow::~CWindow(void)
{		
}


OSErr CWindow::InitWindow(void)
{
	Str63		strTitle = "\pSimpleContainer ";
#ifdef _MSC_VER
    Str255      strCount;
#else
	Str32		strCount;
#endif
	
	m_pWindow = GetNewCWindow(kObjectWindow, nil, (WindowPtr)-1);
	if (!m_pWindow)
	{
		WarningAlert("\pError creating window.");
		return memFullErr;
	} 

	SetWRefCon(m_pWindow, (long)this);
	
	NumToString(++gApp->m_nWndCount, strCount);
	
	strCount[strCount[0]+1] = 0;
	
	BlockMove(&strCount[1], &strTitle[strTitle[0]+1], strlen((char *)&strCount[1]));
	
	strTitle[0] += strlen((char *)&strCount[1]);
	
	short	x = gApp->m_nWndCount * WND_OFFSET,
			y = x + 35;
			
	if ( y > (qd.screenBits.bounds.bottom - 5)) // window tiling algorithm
	{	
		static	short	n = gApp->m_nWndCount;
		
		x -= (WND_OFFSET * (n - 2));
		y -= (qd.screenBits.bounds.bottom - 35);
	}
	
	SetWTitle(m_pWindow, strTitle);
	MoveWindow(m_pWindow, x, y, true);
	
	ShowWindow(m_pWindow);
				
	DrawContents();

	return noErr;
}


void CWindow::HandleMouseUp(void)
{
	m_Click = TickCount();
}


void CWindow::HandleKeyDown(EventRecord event)
{
	unsigned char theKey;
	
	theKey = (unsigned char)event.message & charCodeMask;
	if (event.modifiers & cmdKey)
		gApp->HandleMenuCommand(MenuKey(theKey)) ;
}


void CWindow::HandleActivate(Boolean fActivating)
{
	if (fActivating) 
	{	
		AdjustCursor();
		gApp->m_window = (CWindow *)GetWRefCon(m_pWindow);
		gApp->m_window->m_pWindow = m_pWindow;
	}

	InitCursor();				
}


void CWindow::HandleUpdate(void)
{
	BeginUpdate(m_pWindow);
	DrawContents();
	EndUpdate(m_pWindow);
}


void CWindow::HandleZoom(short part)
{
	GrafPtr savePort;
	
	GetPort(&savePort);
	SetPort(m_pWindow);
		
	ZoomWindow(m_pWindow, part, m_pWindow==FrontWindow());
	
	SetPort(savePort);
}


void CWindow::HandleContentClick(void)
{	
	unsigned long	click,
					dblclick;
	HRESULT			hr;
	Rect			rect;
	CObjectSite	*	pOS;
	CursHandle		watch;
	
	click = TickCount();
	dblclick = GetDblTime();
	
	if ( (click - m_Click) <= dblclick)
	{
		if (m_pObjectSite->m_pObject)
		{
			pOS = m_pObjectSite;
			
			pOS->m_pObject->GetExtent(DVASPECT_CONTENT, &pOS->m_ObjSizeL);
		    SetRect(&rect, 0, 0, (short)pOS->m_ObjSizeL.cx, (short)pOS->m_ObjSizeL.cy);
			
			watch = GetCursor(watchCursor);
			if (watch) SetCursor(*watch);
			
			if (!OleIsRunning(pOS->m_pObject))
			{
				hr = OleRun((LPUNKNOWN)pOS->m_pObject);
				FailScode("\pOleRun()", GetScode(hr));
				
				if (GetScode(hr) == S_OK)
				{
					hr = pOS->m_pObject->DoVerb(OLEIVERB_PRIMARY, 
    									 				 NULL, 
    									 				 (LPOLECLIENTSITE)pOS->m_pOleClientSite, 
    									 				 -1, 
    									 				 m_pWindow, 
    									 				 &rect);
    				FailScode("\pDoVerb(OLEIVERB_PRIMARY)", GetScode(hr));
    				InitCursor();
    				return;
    			}
			}
			else
			{
				hr = pOS->m_pObject->DoVerb(OLEIVERB_PRIMARY, 
    									 				 NULL, 
    									 				 (LPOLECLIENTSITE)pOS->m_pOleClientSite, 
    									 				 -1, 
    									 				 m_pWindow, 
    									 				 &rect);
    			FailScode("\pDoVerb(OLEIVERB_PRIMARY)", GetScode(hr));
    			InitCursor();	
			}
		}	
		
	}
}


void CWindow::HandleGrow(EventRecord event)
{
	long	growResult;
	Rect	tempRect;
	short	xOld, 
			yOld;

	SetPort(m_pWindow);
	
	xOld = m_pWindow->portRect.right;
	yOld = m_pWindow->portRect.bottom;
		
	SetRect(&tempRect, MIN_WINDOW_X, MIN_WINDOW_Y, 32767, 32767);

	growResult = GrowWindow(m_pWindow, event.where, &tempRect);
	
	if (growResult != 0) 
	{
		SizeWindow(m_pWindow, LoWord(growResult), HiWord(growResult), true);

		DrawGrowIcon(m_pWindow);
	}
	
	InvalRect(&m_pWindow->portRect);
}


void CWindow::ClipContents(Boolean fClip)
{
	Rect	rc;

	if (fClip)
	{
		rc = m_pWindow->portRect;
		rc.right  -= 15;
		rc.bottom -= 15;
	}
	else
	{
		rc.top = rc.left = -32767;
		rc.bottom = rc.right = 32767;
	}

	ClipRect(&rc);
}


void CWindow::AdjustCursor(void)
{
}


void CWindow::DrawContents(void)
{
	WindowPtr 	save_port;
	
	GetPort(&save_port);
	SetPort(m_pWindow);

	EraseRect(&m_pWindow->portRect);

	if (m_pObjectSite && m_pObjectSite->m_pObject)
	{
		Rect			rect;
		CObjectSite *	pOS;
		
		ClipContents(true);

		pOS = m_pObjectSite;
		
    	m_pObjectSite->m_pObject->GetExtent(DVASPECT_CONTENT, &m_pObjectSite->m_ObjSizeL);

	    SetRect(&rect, 0, 0, (short)m_pObjectSite->m_ObjSizeL.cx, (short)m_pObjectSite->m_ObjSizeL.cy);

		OleDraw((LPUNKNOWN)m_pObjectSite->m_pObject, 
				DVASPECT_CONTENT, 
				m_pWindow, &rect);
	    		
	    PenNormal();
	    	    
	    if (m_pObjectSite->m_fObjectOpen)
        	OleUIDrawShading(&rect, m_pWindow, OLEUI_SHADE_FULLRECT, 0);
	    
    	FrameRect(&rect);

		ClipContents(false);
	}
	
	DrawGrowIcon(m_pWindow);
	
	SetPort(save_port);
}


void CWindow::ShowInsertObjectDlg(void)
{
	OLEUIINSERTOBJECT	*pioDialog;
	FSSpec				fssFile;
	HRESULT				hr;
	unsigned int		result;
	Rect				rect;
	LPVIEWOBJECT		lpViewObject;
	char				szFile[255];
	
	if (m_pObjectSite && m_pObjectSite->m_pObject)
	{
		WindowPtr	save_port;
		
		GetPort(&save_port);
		SetPort(m_pWindow);
		
		EraseRect(&m_pWindow->portRect);
		ValidRect(&m_pWindow->portRect);
		
		m_pObjectSite->DeleteObject();
		
		SetPort(save_port);
	}

	pioDialog = (OLEUIINSERTOBJECT *)NewPtrClear(sizeof(OLEUIINSERTOBJECT));
	
	if (!pioDialog)
	{
		WarningAlert("\pCould not allocate memory for Insert Object Dialog");
		return;
	}
			
    pioDialog->cbStruct = sizeof(*pioDialog);
    pioDialog->cbStruct = sizeof(OLEUIINSERTOBJECT);
    pioDialog->dwFlags  = IOF_SELECTCREATENEW | IOF_DISABLELINK;
	pioDialog->pfssFile = &fssFile;
	pioDialog->pszFile  = szFile;
	
	result=OleUIInsertObject(pioDialog);
	
	if (result == OLEUI_OK)
	{	
		CObjectSite		*pOS;
		CursHandle		watch;
		
		ASSERT(m_pObjectSite);
			
		watch = GetCursor(watchCursor);
		if (watch) SetCursor(*watch);
	
			
		pOS = m_pObjectSite;
							
		hr = StgCreateDocfile(	NULL, 
								STGM_CREATE | STGM_DELETEONRELEASE | STGM_READWRITE | 
								STGM_TRANSACTED | STGM_SHARE_EXCLUSIVE,
								0,
								&m_pObjectSite->m_pRootStg);								
		
		FailScode("\pStgCreateDocFile()", GetScode(hr));
		
			
		if (GetScode(hr) == S_OK)
		{				
			hr=pOS->m_pRootStg->CreateStorage("Object1", 
												STGM_READWRITE |  STGM_TRANSACTED | 
												STGM_SHARE_EXCLUSIVE, 
												0, 
												0, 
												&pOS->m_pObjectStg);
												
			FailScode("\pCreateStorage()", GetScode(hr));
		}
		
		if (GetScode(hr) == S_OK)
		{
			if (pioDialog->dwFlags & IOF_SELECTCREATENEW)
			{	
				hr = OleCreate(	pioDialog->clsid, 
								IID_IOleObject, 
								OLERENDER_DRAW,
								NULL, 
								(LPOLECLIENTSITE)pOS->m_pOleClientSite, 
								pOS->m_pObjectStg, 
								(void * *)&pOS->m_pObject);
				
				FailScode("\pOleCreate()", GetScode(hr));	
			}
			
			if (pioDialog->dwFlags & IOF_SELECTCREATEFROMFILE)
			{	
				// this function exposed from common.c
							
				hr = OleCreateFromFile(	CLSID_NULL, 
										pioDialog->pszFile, 
										IID_IOleObject,
										OLERENDER_DRAW, 
										NULL, 
										(LPOLECLIENTSITE)pOS->m_pOleClientSite, 
										pOS->m_pObjectStg, 
										(void * *)&pOS->m_pObject);
										
				FailScode("\pOleCreateFromFile()", GetScode(hr));	
			}
		}   
		
	    							
		if (GetScode(hr) == S_OK)	// SetAdvise on the object
		{
	    	hr=pOS->m_pObject->Advise((LPADVISESINK)pOS->m_pAdviseSink, &pOS->m_dwConnection);
	    	
	    	FailScode("\pm_pObject->Advise()", GetScode(hr));
	    }             
	    
	    if (GetScode(hr) == S_OK) 	// Set a View Advise
	    {
		    hr=pOS->m_pObject->QueryInterface(IID_IViewObject,(void * *)&lpViewObject);
		    
		    FailScode("\pm_pObject->QueryInterface()", GetScode(hr));
		}
		    
	    if (GetScode(hr) == S_OK)
	    {
    		hr=lpViewObject->SetAdvise( DVASPECT_CONTENT, 
	    								ADVF_PRIMEFIRST, 
	    								pOS->m_pAdviseSink);
	    								
	    	FailScode("\pSetAdvise()", GetScode(hr));
	    }
	    
	    if (GetScode(hr) == S_OK)
	    {									  
	    	lpViewObject->Release();
	    	
	    	pOS->m_pObject->GetExtent(DVASPECT_CONTENT, &pOS->m_ObjSizeL);
		    SetRect(&rect, 0, 0, (short)pOS->m_ObjSizeL.cx, (short)pOS->m_ObjSizeL.cy);
	    
	    	hr=pOS->m_pObject->SetHostNames("Object Name","SimpleContainer");
	    	
	    	FailScode("\pSetHostNames()", GetScode(hr));
	    }
	    
	    if (GetScode(hr) == S_OK)
	    {
			if (pioDialog->dwFlags & IOF_SELECTCREATENEW)
			{
				hr = OleRun(pOS->m_pObject);
				FailScode("\pOleRun()", GetScode(hr));
				
				pOS->m_pOleClientSite->SaveObject();
				
	    		hr=pOS->m_pObject->DoVerb(OLEIVERB_SHOW, 
	    											NULL, 
	    											pOS->m_pOleClientSite, 
	    											-1, 
	    											m_pWindow, 
	    											&rect);
	    									
	    		FailScode("\pDoVerb()", GetScode(hr)); 
	    	} 
	    }                                                   
	}
	
	InitCursor();	
	DisposePtr((Ptr)pioDialog);
}


void CWindow::HandleClose(void)
{
	ASSERT(m_pWindow);
	ASSERT(m_pObjectSite);
	
	m_pObjectSite->Delete();
	
	DisposeWindow(m_pWindow);
		
	gApp->m_window = NULL;
	
	delete this;
	
	if (!FrontWindow())
		gApp->AdjustMenus();
}

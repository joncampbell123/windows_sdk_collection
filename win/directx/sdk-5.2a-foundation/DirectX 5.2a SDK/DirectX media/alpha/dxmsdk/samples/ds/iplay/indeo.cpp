// This code and information is provided "as is" without warranty of
// any kind, either expressed or implied, including but not limited to
// the implied warranties of merchantability and/or fitness for a
// particular purpose.

// Copyright (C) 1996 - 1997 Intel corporation.  All rights reserved.

// Indeo.cpp : implementation file
//

#include "stdafx.h"
#include "IPlay.h"
#include "Indeo.h"
#include "IPlayDoc.h"
#include "Ax_Spec.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

const int BCS_MAX = 128;		// used by trackbar controls
const int BCS_TICS = 16;        

/////////////////////////////////////////////////////////////////////////////
// CIndeo

IMPLEMENT_DYNCREATE(CIndeo, CFormView)

CIndeo::CIndeo()
	: CFormView(CIndeo::IDD)
{
	//{{AFX_DATA_INIT(CIndeo)
	m_bAccessKey = FALSE;
	m_dwDecodeHeight = 0;
	m_dwDecodeWidth = 0;
	m_dwDecodeX = 0;
	m_dwDecodeY = 0;
	m_dwKeyValue = 0;
	m_dwViewHeight = 0;
	m_dwViewWidth = 0;
	m_dwViewX = 0;
	m_dwViewY = 0;
	m_intBrightness = 0;
	m_intContrast = 0;
	m_intSaturation = 0;
	m_dwDecodeTime = 0;
	m_bAltLine = FALSE;
	m_intGreen = 0;
	m_intRed = 0;
	m_intBlue = 0;
	m_bDontDropFrames = FALSE;
	m_bDontDropQuality = FALSE;
	m_bViewOrigin = -1;
	m_bTransFill = -1;
	//}}AFX_DATA_INIT

	m_dwInitTransRGBVal = 0;
	m_bViewOrigin = 0;
}

CIndeo::~CIndeo()
{
}

void CIndeo::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIndeo)
	DDX_Control(pDX, IDC_CUSTOM_COLORS, m_btnCustomColors);
	DDX_Control(pDX, IDC_DONT_DROP_QUALITY, m_checkDontDropQuality);
	DDX_Control(pDX, IDC_DONT_DROP_FRAMES, m_checkDontDropFrames);
	DDX_Control(pDX, IDC_SATURATION_TEXT, m_editSaturation);
	DDX_Control(pDX, IDC_CONTRAST_TEXT, m_editContrast);
	DDX_Control(pDX, IDC_BRIGHTNESS_TEXT, m_editBrightness);
	DDX_Control(pDX, IDC_GREEN_EDIT, m_editTransFillGreen);
	DDX_Control(pDX, IDC_BLUE_EDIT, m_editTransFillBlue);
	DDX_Control(pDX, IDC_RED_EDIT, m_editTransFillRed);
	DDX_Control(pDX, IDC_ALTLINE, m_checkAltLine);
	DDX_Control(pDX, IDC_DECODE_TIME, m_editDecodeTime);
	DDX_Control(pDX, IDC_VIEW_WIDTH, m_editViewWidth);
	DDX_Control(pDX, IDC_VIEW_Y, m_editViewY);
	DDX_Control(pDX, IDC_VIEW_X, m_editViewX);
	DDX_Control(pDX, IDC_VIEW_HEIGHT, m_editViewHeight);
	DDX_Control(pDX, IDC_KEY_VALUE, m_editKeyValue);
	DDX_Control(pDX, IDC_DECODE_Y, m_editDecodeY);
	DDX_Control(pDX, IDC_DECODE_X, m_editDecodeX);
	DDX_Control(pDX, IDC_DECODE_WIDTH, m_editDecodeWidth);
	DDX_Control(pDX, IDC_ACCESSKEY, m_checkAccessKey);
	DDX_Control(pDX, ID_INDEO_DEFAULTS, m_btnDefaults);
	DDX_Control(pDX, ID_INDEO_APPLY, m_btnApply);
	DDX_Control(pDX, IDC_DECODE_HEIGHT, m_editDecodeHeight);
	DDX_Control(pDX, IDC_TB_COLORCONTROL_SATURATION, m_tbSaturation);
	DDX_Control(pDX, IDC_TB_COLORCONTROL_CONTRAST, m_tbContrast);
	DDX_Control(pDX, IDC_TB_COLORCONTROL_BRIGHTNESS, m_tbBrightness);
	DDX_Check(pDX, IDC_ACCESSKEY, m_bAccessKey);
	DDX_Text(pDX, IDC_DECODE_HEIGHT, m_dwDecodeHeight);
	DDX_Text(pDX, IDC_DECODE_WIDTH, m_dwDecodeWidth);
	DDX_Text(pDX, IDC_DECODE_X, m_dwDecodeX);
	DDX_Text(pDX, IDC_DECODE_Y, m_dwDecodeY);
	DDX_Text(pDX, IDC_KEY_VALUE, m_dwKeyValue);
	DDX_Text(pDX, IDC_VIEW_HEIGHT, m_dwViewHeight);
	DDX_Text(pDX, IDC_VIEW_WIDTH, m_dwViewWidth);
	DDX_Text(pDX, IDC_VIEW_X, m_dwViewX);
	DDX_Text(pDX, IDC_VIEW_Y, m_dwViewY);
	DDX_Text(pDX, IDC_BRIGHTNESS_TEXT, m_intBrightness);
	DDV_MinMaxInt(pDX, m_intBrightness, -128, 128);
	DDX_Text(pDX, IDC_CONTRAST_TEXT, m_intContrast);
	DDV_MinMaxInt(pDX, m_intContrast, -128, 128);
	DDX_Text(pDX, IDC_SATURATION_TEXT, m_intSaturation);
	DDV_MinMaxInt(pDX, m_intSaturation, -128, 128);
	DDX_Text(pDX, IDC_DECODE_TIME, m_dwDecodeTime);
	DDX_Control(pDX, IDC_TRANS_FILL, m_radTransFill);
	DDX_Control(pDX, IDC_TRANS_NOFILL, m_radTransNoFill);
	DDX_Check(pDX, IDC_ALTLINE, m_bAltLine);
	DDX_Text(pDX, IDC_GREEN_EDIT, m_intGreen);
	DDV_MinMaxInt(pDX, m_intGreen, 0, 255);
	DDX_Text(pDX, IDC_RED_EDIT, m_intRed);
	DDV_MinMaxInt(pDX, m_intRed, 0, 255);
	DDX_Text(pDX, IDC_BLUE_EDIT, m_intBlue);
	DDV_MinMaxInt(pDX, m_intBlue, 0, 255);
	DDX_Check(pDX, IDC_DONT_DROP_FRAMES, m_bDontDropFrames);
	DDX_Check(pDX, IDC_DONT_DROP_QUALITY, m_bDontDropQuality);
	DDX_Control(pDX, IDC_FRAME_ORIGIN, m_radFrameOrigin);
	DDX_Control(pDX, IDC_VIEW_ORIGIN, m_radViewOrigin);
	DDX_Radio(pDX, IDC_FRAME_ORIGIN, m_bViewOrigin);
	DDX_Radio(pDX, IDC_TRANS_NOFILL, m_bTransFill);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIndeo, CFormView)
	//{{AFX_MSG_MAP(CIndeo)
	ON_BN_CLICKED(ID_INDEO_APPLY, OnIndeoApply)
	ON_BN_CLICKED(ID_INDEO_DEFAULTS, OnIndeoDefaults)
	ON_EN_CHANGE(IDC_DECODE_HEIGHT, OnChangeDecodeHeight)
	ON_EN_CHANGE(IDC_DECODE_WIDTH, OnChangeDecodeWidth)
	ON_EN_CHANGE(IDC_DECODE_X, OnChangeDecodeX)
	ON_EN_CHANGE(IDC_DECODE_Y, OnChangeDecodeY)
	ON_EN_CHANGE(IDC_VIEW_HEIGHT, OnChangeViewHeight)
	ON_EN_CHANGE(IDC_VIEW_WIDTH, OnChangeViewWidth)
	ON_EN_CHANGE(IDC_VIEW_X, OnChangeViewX)
	ON_EN_CHANGE(IDC_VIEW_Y, OnChangeViewY)
	ON_BN_CLICKED(IDC_ACCESSKEY, OnAccesskey)
	ON_EN_CHANGE(IDC_DECODE_TIME, OnChangeDecodeTime)
	ON_EN_CHANGE(IDC_KEY_VALUE, OnChangeKeyValue)
	ON_BN_CLICKED(IDC_TRANS_FILL, OnTransFill)
	ON_BN_CLICKED(IDC_TRANS_NOFILL, OnTransNofill)
	ON_BN_CLICKED(IDC_ALTLINE, OnAltline)
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_GREEN_EDIT, OnChangeGreenEdit)
	ON_EN_CHANGE(IDC_RED_EDIT, OnChangeRedEdit)
	ON_EN_CHANGE(IDC_BLUE_EDIT, OnChangeBlueEdit)
	ON_BN_CLICKED(IDC_DONT_DROP_FRAMES, OnDontDropFrames)
	ON_BN_CLICKED(IDC_DONT_DROP_QUALITY, OnDontDropQuality)
	ON_BN_CLICKED(IDC_VIEW_ORIGIN, OnViewOrigin)
	ON_BN_CLICKED(IDC_FRAME_ORIGIN, OnFrameOrigin)
	ON_BN_CLICKED(IDC_CUSTOM_COLORS, OnCustomColors)
	ON_EN_UPDATE(IDC_BRIGHTNESS_TEXT, OnUpdateBSCText)
	ON_WM_TIMER()
	ON_EN_UPDATE(IDC_CONTRAST_TEXT, OnUpdateBSCText)
	ON_EN_UPDATE(IDC_SATURATION_TEXT, OnUpdateBSCText)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CIndeo diagnostics

#ifdef _DEBUG
void CIndeo::AssertValid() const
{
	CFormView::AssertValid();
}

void CIndeo::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CIndeo message handlers

void CIndeo::OnInitialUpdate() 
{
	CIPlayDoc *pDoc;

	CFormView::OnInitialUpdate();

	// Initialize trackbars
	m_tbBrightness.SetRange(-BCS_MAX, BCS_MAX);
	m_tbBrightness.SetTicFreq(BCS_TICS);

	m_tbContrast.SetRange(-BCS_MAX, BCS_MAX);
	m_tbContrast.SetTicFreq(BCS_TICS);

	m_tbSaturation.SetRange(-BCS_MAX, BCS_MAX);
	m_tbSaturation.SetTicFreq(BCS_TICS);

	// Is the movie an Indeo movie?
	pDoc = (CIPlayDoc *)GetDocument();
	if ( pDoc->IsIndeo() )	{
		EnableAllOptions();

		pDoc->GetBCS(m_intBrightness, m_intContrast, m_intSaturation);
		m_tbBrightness.SetPos(m_intBrightness);
		m_tbContrast.SetPos(m_intContrast);
		m_tbSaturation.SetPos(m_intSaturation);
		
		pDoc->GetTransFillRGB(m_intRed, m_intGreen, m_intBlue);
		
		m_dwInitTransRGBVal = RGB(m_intRed, m_intGreen, m_intBlue);
		
		pDoc->GetDecodeTime(m_dwDecodeTime);
		pDoc->GetDecodeRect(m_dwDecodeX, m_dwDecodeY, m_dwDecodeWidth,
			m_dwDecodeHeight);
		pDoc->GetViewRect(m_dwViewX, m_dwViewY, m_dwViewWidth, m_dwViewHeight);
		
		pDoc->GetSequenceOptions(m_bAltLine, m_bDontDropFrames, m_bDontDropQuality, m_bTransFill, m_bAccessKey,
			m_dwKeyValue);

		// Update dialog controls from member variables
		UpdateData(FALSE);
	}
	else {
		if (pDoc->IsInitialized()) 
			MessageBox("This is not an IV41 movie so Indeo options will not be available.",
			"Not an Indeo Movie", MB_OK);
		DisableAllOptions();	
	}       

}

void CIndeo::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	CIPlayDoc *pDoc;

	pDoc = (CIPlayDoc *)GetDocument();

	// Sequence options can only be changed when the movie
	// is stopped (they can actually be changed any time, but
	// will not take effect until the movie is stoppped and 
	// restarted).
	if (pDoc->IsIndeo()) {
		if (pDoc->m_State == pDoc->Stopped)
			EnableSeqOptions();
		else
			DisableSeqOptions();
	}
}

void CIndeo::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	int origBrightness = m_intBrightness;
	int origContrast = m_intContrast;
	int origSaturation = m_intSaturation;

	if ( ! pScrollBar){
		CFormView::OnHScroll( nSBCode, nPos, pScrollBar );
		return;
	}

	switch (nSBCode) {
		case TB_LINEUP:

 			if (pScrollBar->m_hWnd == m_tbBrightness.m_hWnd) { 
				if (m_intBrightness > -128)
					m_intBrightness--;
			}
			else if (pScrollBar->m_hWnd == m_tbContrast.m_hWnd) {
				if (m_intContrast > -128)
					m_intContrast--;
			}
			else if (pScrollBar->m_hWnd == m_tbSaturation.m_hWnd) {
				if (m_intSaturation > -128)
					m_intSaturation--;
			}
			break;

		case TB_LINEDOWN:

 			if (pScrollBar->m_hWnd == m_tbBrightness.m_hWnd) { 
				if (m_intBrightness < 128)
					m_intBrightness++;
			}
			else if (pScrollBar->m_hWnd == m_tbContrast.m_hWnd)	{
				if (m_intContrast < 128)
					m_intContrast++;
			}
			else if (pScrollBar->m_hWnd == m_tbSaturation.m_hWnd) {
				if (m_intSaturation < 128)
					m_intSaturation++;
			}
			break;

		case TB_THUMBPOSITION: 
		case TB_THUMBTRACK:
			
			if (pScrollBar->m_hWnd == m_tbBrightness.m_hWnd) 
				m_intBrightness = (int)nPos;

			else if (pScrollBar->m_hWnd == m_tbContrast.m_hWnd)
				m_intContrast = (int)nPos;

			else if (pScrollBar->m_hWnd == m_tbSaturation.m_hWnd)
				m_intSaturation = (int)nPos;
			break;

		case TB_PAGEUP:
		case TB_PAGEDOWN:

			if (pScrollBar->m_hWnd == m_tbBrightness.m_hWnd) 
				m_intBrightness = (int)m_tbBrightness.GetPos();

			else if (pScrollBar->m_hWnd == m_tbContrast.m_hWnd)
				m_intContrast = (int)m_tbContrast.GetPos();

			else if (pScrollBar->m_hWnd == m_tbSaturation.m_hWnd)
				m_intSaturation = (int)m_tbSaturation.GetPos();
			break;
	}
	if ((origBrightness != m_intBrightness)
		|| (origContrast != m_intContrast)
		|| (origSaturation != m_intSaturation))
	   	m_btnApply.EnableWindow(TRUE);	

	UpdateData(FALSE);
		
	return;	
		
}

void CIndeo::OnIndeoApply() 
{
	CIPlayDoc *pDoc;
	CRect rd, rv;
	
	LONG lScalTemp;

	// Update member variables from dialog controls
	UpdateData(TRUE);
	
	pDoc = (CIPlayDoc *)GetDocument();

	//Make sure the decode rect is within the video rect
	if ( m_dwDecodeX + m_dwDecodeWidth > pDoc->VideoWidth() ) { 
		AfxMessageBox(IDS_DECODERECT_TOO_BIG, MB_ICONEXCLAMATION | MB_OK);
		m_editDecodeWidth.SetFocus();
		m_editDecodeWidth.SetSel((DWORD)((-1 << 16) & 0xFFFF0000)); 
		return;
	}

	if ( m_dwDecodeY + m_dwDecodeHeight > pDoc->VideoHeight() ) {
		AfxMessageBox(IDS_DECODERECT_TOO_BIG, MB_ICONEXCLAMATION | MB_OK);
		m_editDecodeHeight.SetFocus();
		m_editDecodeHeight.SetSel((DWORD)((-1 << 16) & 0xFFFF0000)); 
		return;
	}

	// Make sure view rect is in decode rect and video rect; if not, return
	rd = CRect(m_dwDecodeX, 
	          m_dwDecodeY, 
	          m_dwDecodeX + m_dwDecodeWidth, 
	          m_dwDecodeY + m_dwDecodeHeight);

	rv = CRect(m_dwViewX,
	           m_dwViewY,
			   m_dwViewX + m_dwViewWidth,
			   m_dwViewY + m_dwViewHeight);

	if ( rv != CRect(0,0,0,0) ) {
	    
		if ( rd == CRect(0,0,0,0) ) {
			if ( (m_dwViewX + m_dwViewWidth > pDoc->VideoWidth()) ||
		         (m_dwViewY + m_dwViewHeight > pDoc->VideoHeight()) ) {
				AfxMessageBox(IDS_VIEWRECT_TOO_BIG, MB_ICONEXCLAMATION | MB_OK);
				m_editViewX.SetFocus();
				m_editViewX.SetSel((DWORD)((-1 << 16) & 0xFFFF0000)); 
				return;
			}
		}
	/*	else if ( rd & rv != rv ) {
			AfxMessageBox(IDS_VIEWRECT_TOO_BIG, MB_ICONEXCLAMATION | MB_OK);
			m_editViewX.SetFocus();
			m_editViewX.SetSel((DWORD)((-1 << 16) & 0xFFFF0000)); 
			return;
		} */

	}     
	
	
	pDoc->SetTransFillRGB( RGB(m_intBlue, m_intGreen, m_intRed) );
	
	pDoc->SetDecodeTime(m_dwDecodeTime);
	
	pDoc->SetDecodeRect(m_dwDecodeX, m_dwDecodeY, m_dwDecodeWidth,
			            m_dwDecodeHeight);

	pDoc->SetViewRect(m_bViewOrigin, m_dwViewX, m_dwViewY, m_dwViewWidth, m_dwViewHeight);

	pDoc->SetBCS(m_intBrightness, m_intContrast, m_intSaturation);

	if (!pDoc->CanStop()) // movie is Stopped
	{	
		if (m_bDontDropFrames && ! m_bDontDropQuality)
			lScalTemp = SC_DONT_DROP_FRAMES;
		else if (m_bDontDropQuality && ! m_bDontDropFrames) 	
			lScalTemp = SC_DONT_DROP_QUALITY;
		else if (m_bDontDropFrames && m_bDontDropQuality)
			lScalTemp = SC_OFF;
		else
			lScalTemp = SC_ON;

		pDoc->SetSequenceOptions(m_bAltLine, lScalTemp, m_bTransFill, m_bAccessKey,
			m_dwKeyValue);	
	}
	
	// Disable Apply button until there is something to apply
   	m_btnApply.EnableWindow(FALSE);
}

void CIndeo::OnIndeoDefaults() 
{
	CIPlayDoc *pDoc;

	pDoc = (CIPlayDoc *)GetDocument();

	pDoc->GetFrameDefaults(m_intBrightness, m_intContrast, m_intSaturation, m_dwDecodeTime, m_dwDecodeX, 
						   m_dwDecodeY, m_dwDecodeWidth, m_dwDecodeHeight, m_dwViewX, m_dwViewY, m_dwViewWidth, 
						   m_dwViewHeight);

	//Get the Initial color of the Transparency Fill Color
	m_intRed   = (m_dwInitTransRGBVal&0x000000FF);
	m_intGreen = (m_dwInitTransRGBVal&0x0000FF00)>>8;
	m_intBlue  = (m_dwInitTransRGBVal&0x00FF0000)>>16;
	
	m_bViewOrigin = FALSE;
	
	if (!pDoc->CanStop())  // movie is Stopped
		pDoc->GetSeqDefaults(m_bAltLine, m_bDontDropFrames, m_bDontDropQuality, m_bTransFill, m_bAccessKey,
			m_dwKeyValue);

	// Update trackbars
	pDoc->SetBCS(m_intBrightness, m_intContrast, m_intSaturation);
	m_tbBrightness.SetPos(m_intBrightness);
	m_tbContrast.SetPos(m_intContrast);
	m_tbSaturation.SetPos(m_intSaturation);

	// Update visiblity of Access Key value
	if (m_bAccessKey)
		m_editKeyValue.ShowWindow(SW_SHOW);
	else
		m_editKeyValue.ShowWindow(SW_HIDE);

	// Update dialog controls from member variables
	UpdateData(FALSE);

	OnIndeoApply();
}

void CIndeo::OnAccesskey() 
{

	UpdateData(TRUE);

	if (m_bAccessKey) {
		m_editKeyValue.ShowWindow(SW_SHOW);
		m_editKeyValue.SetFocus();
		m_editKeyValue.SetSel((DWORD)((-1 << 16) & 0xFFFF0000));
	}
	else
		m_editKeyValue.ShowWindow(SW_HIDE);

	m_btnApply.EnableWindow(TRUE);
		
}

/////////////////////////////////////////////////////////////////////////////
// CIndeo helper functions

void CIndeo::EnableAllOptions()
{
	m_editBrightness.EnableWindow(TRUE);
	m_editContrast.EnableWindow(TRUE);
	m_editSaturation.EnableWindow(TRUE);

	m_tbBrightness.EnableWindow(TRUE);
	m_tbContrast.EnableWindow(TRUE);
	m_tbSaturation.EnableWindow(TRUE);
		
	m_editDecodeTime.EnableWindow(TRUE);
	m_editDecodeX.EnableWindow(TRUE);
	m_editDecodeY.EnableWindow(TRUE);
	m_editDecodeHeight.EnableWindow(TRUE);
	m_editDecodeWidth.EnableWindow(TRUE);

	m_editViewX.EnableWindow(TRUE);
	m_editViewY.EnableWindow(TRUE);
	m_editViewHeight.EnableWindow(TRUE);
	m_editViewWidth.EnableWindow(TRUE);

	m_editTransFillRed.EnableWindow(TRUE);
	m_editTransFillGreen.EnableWindow(TRUE);
	m_editTransFillBlue.EnableWindow(TRUE);
	m_btnCustomColors.EnableWindow(TRUE);

	m_radFrameOrigin.EnableWindow(TRUE);
	m_radViewOrigin.EnableWindow(TRUE);

	m_btnDefaults.EnableWindow(TRUE);

	// Apply button will not be enabled until something changes

	EnableSeqOptions();
}

void CIndeo::DisableAllOptions()
{

	m_editBrightness.EnableWindow(FALSE);
	m_editContrast.EnableWindow(FALSE);
	m_editSaturation.EnableWindow(FALSE);

	m_tbBrightness.EnableWindow(FALSE);
	m_tbContrast.EnableWindow(FALSE);
	m_tbSaturation.EnableWindow(FALSE);

	m_editDecodeTime.EnableWindow(FALSE);
	m_editDecodeX.EnableWindow(FALSE);
	m_editDecodeY.EnableWindow(FALSE);
	m_editDecodeHeight.EnableWindow(FALSE);
	m_editDecodeWidth.EnableWindow(FALSE);

	m_editViewX.EnableWindow(FALSE);
	m_editViewY.EnableWindow(FALSE);
	m_editViewHeight.EnableWindow(FALSE);
	m_editViewWidth.EnableWindow(FALSE);

	m_editTransFillRed.EnableWindow(FALSE);
	m_editTransFillGreen.EnableWindow(FALSE);
	m_editTransFillBlue.EnableWindow(FALSE);
	m_btnCustomColors.EnableWindow(FALSE);

	m_radFrameOrigin.EnableWindow(FALSE);
	m_radViewOrigin.EnableWindow(FALSE);
	
	m_btnDefaults.EnableWindow(FALSE);
	m_btnApply.EnableWindow(FALSE);

	DisableSeqOptions();
}

void CIndeo::EnableSeqOptions()
{
	m_checkAltLine.EnableWindow(TRUE);
	
	m_checkDontDropFrames.EnableWindow(TRUE);
	m_checkDontDropQuality.EnableWindow(TRUE);

	m_checkAccessKey.EnableWindow(TRUE);
	m_editKeyValue.EnableWindow(TRUE);
	
	m_radTransFill.EnableWindow(TRUE);
	m_radTransNoFill.EnableWindow(TRUE);

	if (m_bAccessKey)
		m_editKeyValue.ShowWindow(SW_SHOW);
	else
		m_editKeyValue.ShowWindow(SW_HIDE);
}

void CIndeo::DisableSeqOptions()
{
	m_checkAltLine.EnableWindow(FALSE);
	
	m_checkDontDropFrames.EnableWindow(FALSE);
	m_checkDontDropQuality.EnableWindow(FALSE);

	m_checkAccessKey.EnableWindow(FALSE);
	m_editKeyValue.EnableWindow(FALSE);
	m_radTransFill.EnableWindow(FALSE);
	m_radTransNoFill.EnableWindow(FALSE);
	m_editKeyValue.EnableWindow(FALSE);

	if (m_bAccessKey)
		m_editKeyValue.ShowWindow(SW_SHOW);
	else
		m_editKeyValue.ShowWindow(SW_HIDE);
}


/////////////////////////////////////////////////////////////////////
// The following message handlers simply enable the Apply button when
// something changes.

void CIndeo::OnChangeDecodeHeight() 
{
   	m_btnApply.EnableWindow(TRUE);	
}

void CIndeo::OnChangeDecodeWidth() 
{
   	m_btnApply.EnableWindow(TRUE);	
}

void CIndeo::OnChangeDecodeX() 
{
   		m_btnApply.EnableWindow(TRUE);
}

void CIndeo::OnChangeDecodeY() 
{
   	m_btnApply.EnableWindow(TRUE);	
}

void CIndeo::OnChangeViewHeight() 
{
   	m_btnApply.EnableWindow(TRUE);	
}

void CIndeo::OnChangeViewWidth() 
{
   	m_btnApply.EnableWindow(TRUE);	
}

void CIndeo::OnChangeViewX() 
{
   	m_btnApply.EnableWindow(TRUE);	
}

void CIndeo::OnChangeViewY() 
{
   	m_btnApply.EnableWindow(TRUE);	
}

void CIndeo::OnChangeDecodeTime() 
{
   	m_btnApply.EnableWindow(TRUE);
}

void CIndeo::OnChangeKeyValue() 
{
	m_btnApply.EnableWindow(TRUE);	
}

void CIndeo::OnUpdateBSCText() 
{
	UpdateData(TRUE);
	m_btnApply.EnableWindow(TRUE);	
	m_tbBrightness.SetPos(m_intBrightness);
	m_tbContrast.SetPos(m_intContrast);
	m_tbSaturation.SetPos(m_intSaturation);
}

void CIndeo::OnTransFill() 
{
	m_btnApply.EnableWindow(TRUE);	
}

void CIndeo::OnTransNofill() 
{
	m_btnApply.EnableWindow(TRUE);	
}


void CIndeo::OnAltline() 
{
	m_btnApply.EnableWindow(TRUE);	
}


void CIndeo::OnChangeRedEdit() 
{	
	m_btnApply.EnableWindow(TRUE);
}


void CIndeo::OnChangeGreenEdit() 
{
	m_btnApply.EnableWindow(TRUE);
}


void CIndeo::OnChangeBlueEdit() 
{
	m_btnApply.EnableWindow(TRUE);	
}

void CIndeo::OnDontDropFrames() 
{	
	m_btnApply.EnableWindow(TRUE);	
}

void CIndeo::OnDontDropQuality() 
{
	m_btnApply.EnableWindow(TRUE);
}

void CIndeo::OnViewOrigin() 
{
	m_btnApply.EnableWindow(TRUE);	
	OnIndeoApply();
}

void CIndeo::OnFrameOrigin() 
{
	m_btnApply.EnableWindow(TRUE);
	OnIndeoApply();	
}

void CIndeo::OnCustomColors() 
{
		
	CHOOSECOLOR TransCC;     // common dialog box structure
	
	TransCC.lStructSize = sizeof(CHOOSECOLOR); 
	TransCC.hwndOwner = NULL;
	TransCC.lpCustColors = (LPDWORD) m_clrefUserCustClrs;
	TransCC.rgbResult = RGB(m_intRed, m_intGreen, m_intBlue);
	TransCC.Flags = CC_RGBINIT | CC_FULLOPEN; 
		
	if (ChooseColor(&TransCC)) 	// pointer to structure with initialization data	
	{
		SetDlgItemInt( IDC_RED_EDIT, (TransCC.rgbResult&0x000000FF), FALSE );	
		SetDlgItemInt( IDC_GREEN_EDIT, (TransCC.rgbResult&0x0000FF00)>>8, FALSE );
		SetDlgItemInt( IDC_BLUE_EDIT, (TransCC.rgbResult&0x00FF0000)>>16, FALSE );
		
		OnIndeoApply();
	}
	
}

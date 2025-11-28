//==========================================================================
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1992 - 1997  Microsoft Corporation.  All Rights Reserved.
//
//--------------------------------------------------------------------------
//
// trprop.cpp - IAMExtTransport Property Page for External Device (VCR) Filter
//

#include <streams.h>

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <olectl.h>
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>

#include "ctimecod.h"
#include "cdevcom.h"
#include "cvcrutil.h"
#include "vcruids.h"

#include "resource.h"
#include "fvcrctrl.h"
#include "trprop.h"

// see vcrprop.cpp for a description of how these tables work
typedef struct tagPARMVALUEINFO{
	int ValueType;		// ED_STR || ED_NUM
	int StringID;
	int DevCap;
} PARMVALUEINFO;

#define ED_STR 1
#define ED_NUM 2
#define ED_VAL 3
#define ED_DBL 4

static PARMVALUEINFO ModeTable[] = {
	{ ED_STR, IDS_EDSTAT_MODE, ED_MODE }
};
const int MaxModeParm = sizeof(ModeTable) / sizeof(ModeTable[0]);

// this table is for matching modes to the list box.  We comment
// out the modes not supported by the sample machine
static long ModeValueTable[][2] = {
	//{ IDS_MODE_EDIT_CUE, ED_MODE_EDIT_CUE },
	{ IDS_MODE_FF, ED_MODE_FF }, 
	{ IDS_MODE_FREEZE, ED_MODE_FREEZE }, 
	{ IDS_MODE_PLAY, ED_MODE_PLAY },
	{ IDS_MODE_RECORD, ED_MODE_RECORD }, 
	//{ IDS_MODE_RECORD_STROBE, ED_MODE_RECORD_STROBE }, 
	{ IDS_MODE_REW, ED_MODE_REW },
	//{ IDS_MODE_SHUTTLE, ED_MODE_SHUTTLE },
	//{ IDS_MODE_STEP, ED_MODE_STEP }, 
	{ IDS_MODE_STOP, ED_MODE_STOP }, 
	{ IDS_MODE_THAW, ED_MODE_THAW }
};
const int MaxModeValue = sizeof(ModeValueTable)/(sizeof(ModeValueTable[0][0]) +
				   sizeof(ModeValueTable[0][1]) ); 

// *
// * CExtTranProperties - A few selected properties in IAMExtTransport
// *

//---------------------------------------------------------
//
// CreateInstance
//
//---------------------------------------------------------
CUnknown *CExtTransProperties::CreateInstance(LPUNKNOWN lpunk, HRESULT *phr)
{
    CUnknown *punk = new CExtTransProperties(lpunk, phr);
    if (punk == NULL) {
	*phr = E_OUTOFMEMORY;
    }

    return punk;
}

//---------------------------------------------------------
//
// CExtTransProperties::Constructor
//
//---------------------------------------------------------
CExtTransProperties::CExtTransProperties(LPUNKNOWN lpunk, HRESULT *phr)
    : CBasePropertyPage(NAME("VCR Transport Property Page"),lpunk,
        IDD_DIALOG2, IDS_PROP_TRANS),
	m_pExtDevice(NULL),
	m_pExtTransport(NULL),
    m_bIsInitialized(FALSE),
	m_bSetHourGlass(FALSE)
{

}
//---------------------------------------------------------
//
// Message handler
//
//---------------------------------------------------------
BOOL CExtTransProperties::OnReceiveMessage(HWND hwnd,
                                        UINT uMsg,
                                        WPARAM wParam,
                                        LPARAM lParam)
{
	// need a pointer to this object
	CExtTransProperties *pThis = 
		(CExtTransProperties *) GetWindowLong(m_Dlg, DWL_USER);

    switch (uMsg)
    {
        case WM_INITDIALOG:
        {
			OnInitDialog();
			break;
        }

        case WM_COMMAND:
        {
            if (m_bIsInitialized) {
				if (HIWORD(wParam) == LBN_SELCHANGE)
	                SetDirty();
  				OnCommand( (int) LOWORD( wParam ), (int) HIWORD( wParam ) );
			}
			break;
        }
	}
    return CBasePropertyPage::OnReceiveMessage(hwnd,uMsg,wParam,lParam);
}
//---------------------------------------------------------
//
// Property page initialization
//
//---------------------------------------------------------
HRESULT CExtTransProperties::OnConnect(IUnknown *pUnknown)
{
	// Get the objects on which properties we want to set
    ASSERT(m_pExtDevice == NULL);

	HRESULT hr = pUnknown->QueryInterface( IID_IAMExtDevice,
						(void **) &m_pExtDevice );
	if( FAILED( hr ) )
	    return hr;
	ASSERT( m_pExtDevice != NULL );
	ASSERT(m_pExtTransport == NULL);
	hr = pUnknown->QueryInterface( IID_IAMExtTransport, 
						(void **) &m_pExtTransport );
	if( FAILED( hr ) )
	    return hr;
	ASSERT( m_pExtTransport != NULL );
    m_bIsInitialized = FALSE ;
    return NOERROR;
}
//---------------------------------------------------------
//
// Property page shutdown
//
//---------------------------------------------------------
HRESULT CExtTransProperties::OnDisconnect()
{
    if (m_pExtDevice == NULL){
        return E_UNEXPECTED;
    }
	m_pExtDevice->Release();
	m_pExtDevice = NULL;

	if (m_pExtTransport == NULL){
        return E_UNEXPECTED;
    }

	m_pExtTransport->Release();
	m_pExtTransport = NULL;

    return NOERROR;
}
//---------------------------------------------------------
//
// Property page activation
//
//---------------------------------------------------------
HRESULT CExtTransProperties::OnActivate()
{
    m_bIsInitialized = TRUE;
    return NOERROR;
}
//---------------------------------------------------------
//
// The real initialization work is done here
//
//---------------------------------------------------------
BOOL CExtTransProperties::OnInitDialog( void )
{
	int i;
	TCHAR buf[64];
	
    InitCommonControls();
    
	// get the window handle
	m_hwndModeList = GetDlgItem(m_Dlg, IDC_TRMODE_VALUE);
	ASSERT(m_hwndModeList);
	
	SendMessage(m_hwndModeList, WM_SETREDRAW, TRUE, 0L);
	// fill the mode list box
	for (i = 0; i < MaxModeValue; i++) {
		LoadString(g_hInst, ModeValueTable[i][0], (LPSTR)buf, 64 );
    	SendMessage(m_hwndModeList, LB_ADDSTRING, 0, (LPARAM)(LPSTR)buf);
    }

	// set media state and local control radio buttons, and mode listbox
	UpdateControls();

	m_hHourGlass = LoadCursor(NULL, IDC_WAIT);
	m_hCurrentCursor = GetCursor();
    return (LRESULT) 1;
}
//---------------------------------------------------------
//
// Handle the property page commands
//
//---------------------------------------------------------
BOOL CExtTransProperties::OnCommand( int iButton, int iNotify )
{
	CExtTransProperties *pThis = 
			(CExtTransProperties *) GetWindowLong(m_Dlg, DWL_USER);
    ASSERT(pThis);
    switch( iButton ){
		case IDC_MS_SPINUP:
			pThis->m_CurMediaState = ED_MEDIA_SPIN_UP;
			break;
		case IDC_MS_SD:
			pThis->m_CurMediaState = ED_MEDIA_SPIN_DOWN;
			break;
		case IDC_MS_UL:
			pThis->m_CurMediaState = ED_MEDIA_UNLOAD;
			pThis->m_bSetHourGlass = TRUE;
			break;
		case IDC_TRLOC_ON:
			pThis->m_CurLocalState = OATRUE;
			break;
		case IDC_TRLOCAL_OFF:
			pThis->m_CurLocalState = OAFALSE;
			break;
	}

    SetDirty();
    return (LRESULT) 1;
}
//---------------------------------------------------------
//
//	Apply user-requested changes
//
//---------------------------------------------------------
HRESULT CExtTransProperties::OnApplyChanges()
{
	int temp;
	
	// we might want to set an hourglass cursor here
	
	if (m_bSetHourGlass) {
		SetCursor(m_hHourGlass);
		m_pExtTransport->put_MediaState(m_CurMediaState);
		SetCursor(m_hCurrentCursor);
	}
	m_pExtTransport->put_LocalControl(m_CurLocalState);
	
	temp = SendMessage(m_hwndModeList, LB_GETCURSEL, 0, 0L);
	if ( temp != m_CurModeSel ) {
		m_CurModeSel = temp;
		m_pExtTransport->put_Mode(ModeValueTable[m_CurModeSel][1]);
	}
	
	UpdateControls();
	return(NOERROR);

}

//---------------------------------------------------------
//
// Sets m_bDirty and notifies the property page site of the change
//
//---------------------------------------------------------
void CExtTransProperties::SetDirty()
{
    m_bDirty = TRUE;
    if (m_pPageSite)
    {
        m_pPageSite->OnStatusChange(PROPPAGESTATUS_DIRTY);
    }
}

//---------------------------------------------------------
//
// UpdateControls - sets non-listbox controls to current values
//
//---------------------------------------------------------
void CExtTransProperties::UpdateControls()
{
	int temp;

	m_pExtTransport->get_MediaState(&m_CurMediaState);
	switch (m_CurMediaState) {
		case ED_MEDIA_SPIN_UP:
			temp = IDC_MS_SPINUP;
			break;
		case ED_MEDIA_SPIN_DOWN:
			temp = IDC_MS_SD;
			break;
		case ED_MEDIA_UNLOAD:
			temp = IDC_MS_UL;
			break;
	};
	CheckRadioButton(m_Dlg, IDC_MS_SPINUP, IDC_MS_UL, temp);
		
	m_pExtTransport->get_LocalControl(&m_CurLocalState); 
	switch (m_CurLocalState) {
		case OATRUE:
			temp = IDC_TRLOC_ON;
			break;
		case OAFALSE:
			temp = IDC_TRLOCAL_OFF;
			break;
	}
	CheckRadioButton(m_Dlg, IDC_TRLOC_ON, IDC_TRLOCAL_OFF, temp);

	m_pExtTransport->get_Mode(&m_CurMode);
	// convert to listbox index
	m_CurModeSel = FindModeIndex(m_CurMode);
	SendMessage(m_hwndModeList, LB_SETCURSEL, m_CurModeSel, 0L);
	
	return;
}

//---------------------------------------------------------
//
// FindModeIndex - given a mode, get its index into the 
// string/mode table
//
//---------------------------------------------------------
int	CExtTransProperties::FindModeIndex(long Value){

	int i;
	
	for (i = 0; i <	MaxModeValue; i++) {
		if ( ModeValueTable[i][1] == Value )
			return i;	// found it
	}
	return -1;
}

// eof trprop.cpp

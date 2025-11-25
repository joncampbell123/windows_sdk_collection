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
// vcrprop.cpp - IAMExtDevice Property Page for VCR Control Filter
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
#include "vcrprop.h"

// initialize some data for device capabilities.  Each entry has 3 data items
//	the first item describes whether the capability value is text or numerical,
//  (we know they are all returned in "Value" parm of the capability method).
//	The 2nd is the string ID, and the last is the parameter itself
typedef struct tagDEVCAPINFO{
	int ValueType;		// ED_STR || ED_NUM
	int StringID;
	int DevCap;
} DEVCAPINFO;

#define ED_STR 1
#define ED_NUM 2
#define ED_VAL 3
#define ED_DBL 4

static DEVCAPINFO DevCapTable[] = {
	{ ED_STR, IDS_DEVCAP_CAN_RECORD, ED_DEVCAP_CAN_RECORD },
	{ ED_STR, IDS_DEVCAP_CAN_RECORD_STROBE, ED_DEVCAP_CAN_RECORD_STROBE },
	{ ED_STR, IDS_DEVCAP_HAS_AUDIO, ED_DEVCAP_HAS_AUDIO },
	{ ED_STR, IDS_DEVCAP_HAS_VIDEO, ED_DEVCAP_HAS_VIDEO },
	{ ED_STR, IDS_DEVCAP_USES_FILES, ED_DEVCAP_USES_FILES },         
	{ ED_STR, IDS_DEVCAP_CAN_SAVE, ED_DEVCAP_CAN_SAVE },           
	{ ED_STR, IDS_DEVCAP_DEVICE_TYPE, ED_DEVCAP_DEVICE_TYPE },        
	{ ED_STR, IDS_DEVCAP_TIMECODE_READ, ED_DEVCAP_TIMECODE_READ },      
	{ ED_STR, IDS_DEVCAP_TIMECODE_WRITE, ED_DEVCAP_TIMECODE_WRITE },     
	{ ED_STR, IDS_DEVCAP_CTLTRK_READ, ED_DEVCAP_CTLTRK_READ },        
	{ ED_STR, IDS_DEVCAP_INDEX_READ, ED_DEVCAP_INDEX_READ },         
	{ ED_NUM, IDS_DEVCAP_PREROLL, ED_DEVCAP_PREROLL },            
	{ ED_NUM, IDS_DEVCAP_POSTROLL, ED_DEVCAP_POSTROLL },           
	{ ED_STR, IDS_DEVCAP_SYNCACC, ED_DEVCAP_SYNC_ACCURACY },            
	{ ED_STR, IDS_DEVCAP_NORMAL_RATE, ED_DEVCAP_NORMAL_RATE },        
	{ ED_STR, IDS_DEVCAP_CAN_PREVIEW, ED_DEVCAP_CAN_PREVIEW },        
	{ ED_STR, IDS_DEVCAP_CAN_MONITOR_SOURCES, ED_DEVCAP_CAN_MONITOR_SOURCES },
	{ ED_STR, IDS_DEVCAP_CAN_TEST, ED_DEVCAP_CAN_TEST },                  
	{ ED_STR, IDS_DEVCAP_VIDEO_INPUTS, ED_DEVCAP_VIDEO_INPUTS },       
	{ ED_STR, IDS_DEVCAP_AUDIO_INPUTS, ED_DEVCAP_AUDIO_INPUTS },      
	{ ED_STR, IDS_DEVCAP_NEEDS_CALIBRATING, ED_DEVCAP_NEEDS_CALIBRATING },  
	{ ED_STR, IDS_DEVCAP_SEEK_TYPE, ED_DEVCAP_SEEK_TYPE },   
};
const int MaxDevCaps = sizeof(DevCapTable) / sizeof(DevCapTable[0]);

// this table is for getting the string ID's of the responses
static long DevValueTable[][2] = {
	{ IDS_DEVTYPE_VCR,         			ED_DEVTYPE_VCR },
	{ IDS_DEVTYPE_LASERDISK,   			ED_DEVTYPE_LASERDISK },
	{ IDS_DEVTYPE_ATR,         			ED_DEVTYPE_ATR },
	{ IDS_DEVTYPE_DDR,         			ED_DEVTYPE_DDR },
	{ IDS_DEVTYPE_ROUTER,      			ED_DEVTYPE_ROUTER },
	{ IDS_DEVTYPE_KEYER,       			ED_DEVTYPE_KEYER },
	{ IDS_DEVTYPE_MIXER_VIDEO, 			ED_DEVTYPE_MIXER_VIDEO },
	{ IDS_DEVTYPE_DVE,         			ED_DEVTYPE_DVE },
	{ IDS_DEVTYPE_WIPEGEN,     			ED_DEVTYPE_WIPEGEN },
	{ IDS_DEVTYPE_MIXER_AUDIO, 			ED_DEVTYPE_MIXER_AUDIO },
	{ IDS_DEVTYPE_CG,          			ED_DEVTYPE_CG },
	{ IDS_DEVTYPE_TBC,         			ED_DEVTYPE_TBC },
	{ IDS_DEVTYPE_TCG,         			ED_DEVTYPE_TCG },
	{ IDS_DEVTYPE_GPI,         			ED_DEVTYPE_GPI },
	{ IDS_DEVTYPE_JOYSTICK,    			ED_DEVTYPE_JOYSTICK },
	{ IDS_DEVTYPE_KEYBOARD,    			ED_DEVTYPE_KEYBOARD	},
	{ IDS_SYNCACC_PRECISE, 				ED_SYNCACC_PRECISE	},	
	{ IDS_SYNCACC_FRAME,   				ED_SYNCACC_FRAME },	
	{ IDS_SYNCACC_ROUGH,   				ED_SYNCACC_ROUGH },	
	{ IDS_RATE_24,         				ED_RATE_24 },
	{ IDS_RATE_25,         				ED_RATE_25 },
	{ IDS_RATE_2997,       				ED_RATE_2997 },
	{ IDS_RATE_30,         				ED_RATE_30 },
	{ IDS_SEEK_PERFECT,    				ED_SEEK_PERFECT	},
	{ IDS_SEEK_FAST,       				ED_SEEK_FAST },
	{ IDS_SEEK_SLOW,       				ED_SEEK_SLOW },
	{ IDS_ON,              				ED_POWER_ON	},
	{ IDS_OFF,             				ED_POWER_OFF },
	{ IDS_STANDBY,         				ED_POWER_STANDBY },
	{ IDS_ACTIVE,          				ED_ACTIVE },
	{ IDS_INACTIVE,        				ED_INACTIVE	},
	{ IDS_ALL,             				ED_ALL },
	{ IDS_TRUE,							OATRUE },
	{ IDS_FALSE,						OAFALSE }
};
const int MaxDevValue = sizeof(DevValueTable) / (sizeof(DevValueTable[0][0]) +
												 sizeof(DevValueTable[0][1]));

static UINT CBasePropertyPages = 1;
static const CLSID *pPropertyPageClsids[] =
{
    &CLSID_VCRControlPropertyPage
};
static const dwTimeout = 1000L;

// *
// * CVcrProperties - Properties in IAMExtDevice
// *

//---------------------------------------------------------
//
// CreateInstance
//
//---------------------------------------------------------
CUnknown *CVcrProperties::CreateInstance( LPUNKNOWN punk, HRESULT *phr )
{
    CVcrProperties *pNewObject
        = new CVcrProperties( punk, phr);

    if( pNewObject == NULL )
        *phr = E_OUTOFMEMORY;

    return pNewObject;
}
//---------------------------------------------------------
//
// CVcrProperties::Constructor
//
//---------------------------------------------------------
CVcrProperties::CVcrProperties( LPUNKNOWN pUnk, HRESULT *phr)
    : CBasePropertyPage(NAME("VCR Property Page"),pUnk,
        IDD_DIALOG1, IDS_PROP_GENERAL),
	m_pExtDevice(NULL),
	m_pExtTransport(NULL),
    m_bIsInitialized(FALSE),
	m_hwndCapList(NULL),
	m_hwndCapValue(NULL),
	m_hwndVidInList(NULL),
	m_hwndAudInList(NULL),
	m_hPortButton1(NULL),
	m_hPortButton2(NULL),
	m_hPortButton3(NULL),
	m_hPortButton4(NULL),
	m_hPortButton5(NULL)
{

}

//---------------------------------------------------------
//
// Message handler
//
//---------------------------------------------------------
BOOL CVcrProperties::OnReceiveMessage(HWND hwnd,
                                        UINT uMsg,
                                        WPARAM wParam,
                                        LPARAM lParam)
{
	HRESULT hr;
	FILTER_STATE fsState;

	// need a pointer to this object
	CVcrProperties *pThis = (CVcrProperties *) GetWindowLong(m_Dlg, DWL_USER);

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
				OnCommand( (int) LOWORD( wParam ), (int) HIWORD( wParam ) );
				break;
			}
        }
		default:
			if (m_bIsInitialized) {
				OnCapListNotification();
				// test graph state and enable/disable port selection
				//	controls as appropriate
				hr = m_pFilter->GetState(dwTimeout, &fsState);
				if ( fsState != State_Stopped)
					EnablePortSelection(FALSE);
				else
					EnablePortSelection(TRUE);
			}
    }
    return CBasePropertyPage::OnReceiveMessage(hwnd,uMsg,wParam,lParam);
}

//---------------------------------------------------------
//
// Property page initialization
//
//---------------------------------------------------------
HRESULT CVcrProperties::OnConnect(IUnknown *pUnknown)
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

	hr = pUnknown->QueryInterface( IID_IBaseFilter, (void **) &m_pFilter );
	if( FAILED( hr ) )
	    return hr;
	ASSERT( m_pFilter != NULL );

	m_bIsInitialized = FALSE ;
    return NOERROR;
}
//---------------------------------------------------------
//
// Property page shutdown
//
//---------------------------------------------------------
HRESULT CVcrProperties::OnDisconnect()
{
    if (m_pExtDevice == NULL)
		return E_UNEXPECTED;
    
	m_pExtDevice->Release();
	m_pExtDevice = NULL;

	if (m_pExtTransport == NULL)
        return E_UNEXPECTED;

	m_pExtTransport->Release();
	m_pExtTransport = NULL;

	if (m_pFilter == NULL)
        return E_UNEXPECTED;

	m_pFilter->Release();
	m_pFilter = NULL;
    return NOERROR;
}
//---------------------------------------------------------
//
// Property page activation
//
//---------------------------------------------------------
HRESULT CVcrProperties::OnActivate()
{
    m_bIsInitialized = TRUE;
    return NOERROR;
}
//---------------------------------------------------------
//
// The real initialization work is done here
//
//---------------------------------------------------------
BOOL CVcrProperties::OnInitDialog( void )
{
	int i;
	TCHAR buf[64];
	int temp;
	IPin *pPin;
	IAMPhysicalPinInfo *pIAMPhys;
	IEnumPins *pEnumPins;
	ULONG cFetched;
	LPOLESTR pName = NULL;
	PIN_INFO PinInfo;
	long Type;
	HRESULT hr;
	DWORD dwTimeout;
	FILTER_STATE fsState;

	
    InitCommonControls();
    
	// get the window and control handles
	m_hwndCapList = GetDlgItem(m_Dlg, IDC_DEVCAPS);
	ASSERT(m_hwndCapList);
	m_hwndCapValue = GetDlgItem(m_Dlg, IDC_DCTEXT);
	ASSERT(m_hwndCapValue);
	m_hwndVidInList = GetDlgItem(m_Dlg, IDC_VIDSEL);
	ASSERT(m_hwndVidInList);
	m_hwndAudInList = GetDlgItem(m_Dlg, IDC_AUDSEL);
	ASSERT(m_hwndAudInList);
	m_hPortButton1 = GetDlgItem(m_Dlg, IDC_COM1);
	ASSERT(m_hPortButton1);
	m_hPortButton2 = GetDlgItem(m_Dlg, IDC_COM2);
	ASSERT(m_hPortButton2);
	m_hPortButton3 = GetDlgItem(m_Dlg, IDC_COM3);
	ASSERT(m_hPortButton3);
	m_hPortButton4 = GetDlgItem(m_Dlg, IDC_COM4);
	ASSERT(m_hPortButton4);
	m_hPortButton5 = GetDlgItem(m_Dlg, IDC_COMSIM);
	ASSERT(m_hPortButton5);

	// Decide if Device Port selection should be enabled or not
	dwTimeout = 1000L;
	hr = m_pFilter->GetState(dwTimeout, &fsState);
	if ( fsState != State_Stopped)
		EnablePortSelection(FALSE);
	else
		EnablePortSelection(TRUE);

	temp = SendMessage(m_hwndCapList, WM_SETREDRAW, TRUE, 0L);
	// fill the device cap list box
	for (i = 0; i < MaxDevCaps; i++) {
		temp = LoadString(g_hInst, DevCapTable[i].StringID, (LPSTR)buf, 64 );
    	temp = SendMessage(m_hwndCapList, LB_ADDSTRING, 0, (LPARAM)(LPSTR)buf);
    }

	// put the displayed capability value up
	m_CurCapSel = SendMessage(m_hwndCapList, LB_GETTOPINDEX, 0, 0L);
	ShowCapabilityValue(m_CurCapSel, m_hwndCapValue);

	//put up the device ID
	m_pExtDevice->get_ExternalDeviceID( &pName );
	temp = SetDlgItemText(m_Dlg, IDC_DCDEVID, (LPCTSTR)pName);
	QzTaskMemFree(pName);
	pName = NULL;

	//put up the device version
	m_pExtDevice->get_ExternalDeviceVersion( &pName );
	temp = SetDlgItemText(m_Dlg, IDC_DCDV, (LPCTSTR)pName);
	pName = NULL;

	// are we linked to the graph's controls or not?
	m_pExtTransport->GetStatus(ED_LINK_MODE, &m_bLink);

	// set power status, comm port radio buttons, etc.
	UpdateControls();

	// do the video input list
	temp = SendMessage(m_hwndVidInList, WM_SETREDRAW, TRUE, 0L);
	temp = SendMessage(m_hwndAudInList, WM_SETREDRAW, TRUE, 0L);
	// use the filter's pin enumeration interface for this
	
	m_pFilter->EnumPins( &pEnumPins );
	
	do {// do 1 pin at a time
		pEnumPins->Next( 1, &pPin, &cFetched );
		if ( cFetched > 0 ) {
			// we only deal with input pins here
			pPin->QueryPinInfo( &PinInfo );
			if ( PinInfo.dir == PINDIR_INPUT ) {
				// get our IAMPhysicalPinInfo
				hr = pPin->QueryInterface( IID_IAMPhysicalPinInfo,
					(void **) &pIAMPhys );
				if( FAILED( hr ) )
					return 0;
				ASSERT( pIAMPhys != NULL );
				// is it video pin?
				pIAMPhys->GetPhysicalType( &Type, &pName );
				if (Type < PhysConn_Audio_Tuner )
					// yup - put up the name in the video input list box
					temp = SendMessage(m_hwndVidInList, LB_ADDSTRING, 0,
						(LPARAM)(LPSTR)pName);
				else
					temp = SendMessage(m_hwndAudInList, LB_ADDSTRING, 0,
						(LPARAM)(LPSTR)pName);
				QzTaskMemFree(pName);
				pName = NULL;
			}
			// release the pin whose info we just requested and the
			// pin we just enumerated
			QueryPinInfoReleaseFilter(PinInfo);
			pPin->Release();
		}
		else	// no more pins
			break;
	} while (TRUE);
	
	// lose the enumerator
	pEnumPins->Release();
	
    return (LRESULT) 1;
}
//---------------------------------------------------------
//
// Handle the property page commands
//
//---------------------------------------------------------
BOOL CVcrProperties::OnCommand( int iButton, int iNotify )
{
	CVcrProperties *pThis = (CVcrProperties *) GetWindowLong(m_Dlg, DWL_USER);
    ASSERT(pThis);
    switch( iButton ){
		case IDC_COM1:
			pThis->m_CurDevPort = DEV_PORT_COM1;
			break;
		case IDC_COM2:
			pThis->m_CurDevPort = DEV_PORT_COM2;
			break;
		case IDC_COM3:
			pThis->m_CurDevPort = DEV_PORT_COM3;			
			break;
		case IDC_COM4:
			pThis->m_CurDevPort = DEV_PORT_COM4;
			break;
		case IDC_COMSIM:
			pThis->m_CurDevPort = DEV_PORT_SIM;
			break;
		case IDC_DCPWR_ON:
			pThis->m_CurPowerMode = ED_POWER_ON;
			break;
		case IDC_DCPWR_OFF:
			pThis->m_CurPowerMode = ED_POWER_OFF;
			break;
		case IDC_DCPWR_STBY:
			pThis->m_CurPowerMode = ED_POWER_STANDBY;
			break;
		case IDC_LINK:
			if (IsDlgButtonChecked(m_Dlg, IDC_LINK))
				pThis->m_bLink = OATRUE;
			else
				pThis->m_bLink = OAFALSE;
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
HRESULT CVcrProperties::OnApplyChanges()
{
	if (m_bDevPortEnabled)
		m_pExtDevice->put_DevicePort(m_CurDevPort);
	m_pExtDevice->put_DevicePower(m_CurPowerMode);
	m_pExtTransport->SetTransportVideoParameters(ED_TRANSVIDEO_SET_SOURCE,
							m_CurVidInSel);
    m_pExtTransport->SetTransportAudioParameters(ED_TRANSAUDIO_SET_SOURCE,
							m_CurAudInSel);
	UpdateControls();
	return(NOERROR);

}
//---------------------------------------------------------
// 
//
// Sets m_bDirty and notifies the property page site of the change
//
//---------------------------------------------------------
void CVcrProperties::SetDirty()
{
    m_bDirty = TRUE;
    if (m_pPageSite)
        m_pPageSite->OnStatusChange(PROPPAGESTATUS_DIRTY);
}

//---------------------------------------------------------
//
// UpdateControls - sets non-listbox controls to current values
//
//---------------------------------------------------------
void CVcrProperties::UpdateControls()
{
	int temp;
	LPOLESTR pName = NULL;

	m_pExtDevice->get_DevicePower(&m_CurPowerMode);
	switch (m_CurPowerMode) {
		case ED_POWER_ON:
			temp = IDC_DCPWR_ON;
			break;
		case ED_POWER_OFF:
			temp = IDC_DCPWR_OFF;
			break;
		case ED_POWER_STANDBY:
			temp = IDC_DCPWR_STBY;
			break;
	};
	CheckRadioButton(m_Dlg, IDC_DCPWR_ON, IDC_DCPWR_STBY, temp);
		m_pExtDevice->get_DevicePort(&m_CurDevPort); 
	switch (m_CurDevPort) {
		case DEV_PORT_COM1:
			temp = IDC_COM1;
			break;
		case DEV_PORT_COM2:
			temp = IDC_COM2;
			break;
		case DEV_PORT_COM3:
			temp = IDC_COM3;
			break;
		case DEV_PORT_COM4:
			temp = IDC_COM4;
			break;
		case DEV_PORT_SIM:
			temp = IDC_COMSIM;
			break;
		default:
			// can add support for ARTI or VLAN here
			temp = NULL;
	}
	CheckRadioButton(m_Dlg, IDC_COM1, IDC_COMSIM, temp);
		
	if (m_bLink == OATRUE) {
		m_pExtTransport->put_Mode(ED_MODE_LINK_ON);
		CheckDlgButton(m_Dlg, IDC_LINK, TRUE);
	}
	else {
		m_pExtTransport->put_Mode(ED_MODE_LINK_OFF);
		CheckDlgButton(m_Dlg, IDC_LINK, FALSE);
	}

	//put up the device ID
	m_pExtDevice->get_ExternalDeviceID( &pName );
	temp = SetDlgItemText(m_Dlg, IDC_DCDEVID, (LPCTSTR)pName);
	QzTaskMemFree(pName);
	pName = NULL;

}
//---------------------------------------------------------
//
// OnCapListNotification
//
// Handle the notification messages from the Capabilities control
//
//---------------------------------------------------------
void CVcrProperties::OnCapListNotification()
{
	long temp;
	// fake-out because can't get scroll messages from listbox
	// update the value control if the selection changed
		temp = SendMessage(m_hwndCapList, LB_GETTOPINDEX, 0, 0L);
		if ( temp != m_CurCapSel ) {
			m_CurCapSel = temp;
			ShowCapabilityValue(m_CurCapSel, m_hwndCapValue);
		}
}
//---------------------------------------------------------
//
// FindStringID - given a Capability Value that is known to be a 
//	string, pluck the StringID from the local table
//
//---------------------------------------------------------
int	CVcrProperties::FindStringID(long Value)
{
	int i;
	
	for (i = 0; i <	MaxDevValue; i++) {
		if ( DevValueTable[i][1] == Value )
			return DevValueTable[i][0];	// found it
	}
	return -1;
}
//---------------------------------------------------------
//
// ShowCapabilityValue - uses index into DevCapTable to put this up
//
//---------------------------------------------------------
void CVcrProperties::ShowCapabilityValue(int index, HWND hwndValue)
{
	long Value;
	double dblValue;
	int temp;
	int StringID;
	TCHAR buf[64];

	m_pExtDevice->GetCapability(DevCapTable[index].DevCap, &Value, &dblValue);
	// figure out how to display it
	if ( DevCapTable[index].ValueType == ED_STR ) {
		StringID = FindStringID(Value);
		temp = 	LoadString(g_hInst, StringID, (LPSTR)buf, 64 );
	}
	else 	// it's a number
		_ultoa( (unsigned long) Value, (LPSTR)buf, 10 );
	// put it up
	temp = SetWindowText( hwndValue,(LPSTR)buf);	
}
//---------------------------------------------------------
//
// EnablePortSelection - enables/disables the radio buttons to
//	keep the machine from blowing up
//
//---------------------------------------------------------
void CVcrProperties::EnablePortSelection(BOOL bEnable)
{
	if (bEnable) {
		EnableWindow(m_hPortButton1, TRUE);
		EnableWindow(m_hPortButton2, TRUE);
		EnableWindow(m_hPortButton3, TRUE);
		EnableWindow(m_hPortButton4, TRUE);
		EnableWindow(m_hPortButton5, TRUE);
		m_bDevPortEnabled = TRUE;
	}
	else {
		EnableWindow(m_hPortButton1, FALSE);
		EnableWindow(m_hPortButton2, FALSE);
		EnableWindow(m_hPortButton3, FALSE);
		EnableWindow(m_hPortButton4, FALSE);
		EnableWindow(m_hPortButton5, FALSE);
		m_bDevPortEnabled = FALSE;
	}
}
// eof vcrprop.cpp

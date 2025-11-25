//===========================================================================
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
// KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
// PURPOSE.
//
// Copyright (c) 1992 - 1997  Microsoft Corporation.  All Rights Reserved.
//
//===========================================================================
//
//	filename: cvcrutil.cpp
//
// Base classes implementing external device control (VCR) interfaces.
// Derive from these and implement just the custom method and property
// methods.
//

#include <windows.h>
#include <streams.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <vfw.h>

#include "vcruids.h"
#include <initguid.h>
#include <olectl.h>

#include "ctimecod.h"
#include "cdevcom.h"
#include "cvcrutil.h"

#define DbgFunc(a) DbgLog(( LOG_TRACE, \
                            2, \
                            TEXT("VCRTC::%16hu"), \
                            TEXT(a) \
                         ));

const long ExtVidConnType[] = {
		PhysConn_Video_Tuner,
		PhysConn_Video_Composite,
		PhysConn_Video_SVideo,
		PhysConn_Video_RGB,
		PhysConn_Video_YRYBY,
		PhysConn_Video_SerialDigital,
		PhysConn_Video_ParallelDigital,
		PhysConn_Video_SCSI,
		PhysConn_Video_AUX,
		PhysConn_Video_1394,
		PhysConn_Video_USB
};
const int ExtVidConnCnt = sizeof(ExtVidConnType);

const TCHAR *strExtVidConnType[] = {
		TEXT("Tuner"),
		TEXT("Composite"),
		TEXT("S-Video"),
		TEXT("RGB"),
		TEXT("Y//R-Y//B-Y"),
		TEXT("Serial Digital"),
		TEXT("Parallel Digital"),
		TEXT("SCSI"),
		TEXT("AUX"),
		TEXT("1394"),
		TEXT("USB")
};

const long ExtAudConnType[] = {
		PhysConn_Audio_Tuner,
		PhysConn_Audio_Line,
		PhysConn_Audio_Mic,
		PhysConn_Audio_AESDigital,
		PhysConn_Audio_SPDIFDigital,
		PhysConn_Audio_SCSI,
		PhysConn_Audio_AUX,
		PhysConn_Audio_1394,
		PhysConn_Audio_USB
};
const int ExtAudConnCnt = sizeof(ExtAudConnType);

const TCHAR *strExtAudConnType[] = {
		TEXT("Tuner"),
		TEXT("Line"),
		TEXT("Mic"),
		TEXT("AES Digital"),
		TEXT("SPDIF Digital"),
		TEXT("SCSI"),
		TEXT("AUX"),
		TEXT("1394"),
		TEXT("USB")
};
static TCHAR buf[32];
static TCHAR buf1[32];
const TCHAR strDummyDeviceVersion[] = TEXT("1.0");

//------------------------------------------
//
//	CAMPhysicalPinInfo Implementation
//
//------------------------------------------
CAMPhysicalPinInfo::CAMPhysicalPinInfo(HRESULT * phr, TCHAR *tszName) :
	CUnknown( tszName, NULL )
{
	
}

//------------------------------------------
//
//	Destructor
//
//------------------------------------------
CAMPhysicalPinInfo::~CAMPhysicalPinInfo()
{
 	
}

//------------------------------------------
//
// NonDelegatingQueryInterface
//
// Reveal our Interface
//------------------------------------------
STDMETHODIMP
CAMPhysicalPinInfo::NonDelegatingQueryInterface(REFIID riid, void **ppv) {

	CAutoLock cObjectLock(&m_StateLock);

    if (riid == IID_IAMPhysicalPinInfo)
        return GetInterface((IAMPhysicalPinInfo *) this, ppv);
    else
        return CUnknown::NonDelegatingQueryInterface(riid, ppv);
} 

//------------------------------------------
//
// CAMExtDevice implementation
//
//------------------------------------------
CAMExtDevice::CAMExtDevice(HRESULT * phr, TCHAR *tszName) :
	CUnknown( tszName, NULL ),
	m_CurDevicePort(NULL)
{
	HRESULT Result;
	// must have a way to talk to a machine
	m_devcom = new CDevCom((CBaseFilter *)this, &Result);
    if (m_devcom == NULL || Result) {
        *phr = E_OUTOFMEMORY;
        return;
	}

	// initialize the capabilities structures - we default to a test VCR,
	// in this case a simple SVHS recorder such as the SVO-5800 (RS-422)
	// or other machine with an SVBK-10 RS-232 interface
	m_DevCaps.CanRecord = OATRUE;
	m_DevCaps.CanRecordStrobe = OAFALSE;
	m_DevCaps.HasAudio = OATRUE;
	m_DevCaps.HasVideo = OATRUE;          
	m_DevCaps.UsesFiles = OAFALSE;         
	m_DevCaps.CanSave = OAFALSE;           
	m_DevCaps.DeviceType = ED_DEVTYPE_VCR;        
	m_DevCaps.TCRead = OATRUE;            
	m_DevCaps.TCWrite = OAFALSE;           
	m_DevCaps.CTLRead = OATRUE;           
	m_DevCaps.IndexRead = OAFALSE;         
	m_DevCaps.Preroll = 150L;           
	m_DevCaps.Postroll = 30L;;          
	m_DevCaps.SyncAcc = ED_SYNCACC_PRECISE;           
	m_DevCaps.NormRate = ED_RATE_2997;          
	m_DevCaps.CanPreview = OATRUE;        
	m_DevCaps.CanMonitorSrc = OATRUE;    
	m_DevCaps.CanTest = OATRUE;           
	m_DevCaps.VideoIn = OATRUE;           
	m_DevCaps.AudioIn = OATRUE;           
	m_DevCaps.Calibrate = OAFALSE;
	m_DevCaps.SeekType = ED_SEEK_SLOW;          
			
	// default comm port and power state
	put_DevicePort(DEV_PORT_COM2);
	put_DevicePower(ED_POWER_ON);
}

//------------------------------------------
//
//	Destructor
//
//------------------------------------------
CAMExtDevice::~CAMExtDevice()
{
 	m_devcom->CloseDevPort();
}

//------------------------------------------
//
// NonDelegatingQueryInterface
//
// Reveal our Interface
//------------------------------------------
STDMETHODIMP
CAMExtDevice::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	CAutoLock cObjectLock(&m_StateLock);

    if (riid == IID_IAMExtDevice)
        return GetInterface((IAMExtDevice *) this, ppv);
    else
        return CUnknown::NonDelegatingQueryInterface(riid, ppv);
} 

//------------------------------------------
//
//	GetCapability
//
// General device capabilities property
//------------------------------------------
HRESULT 
CAMExtDevice::GetCapability(long Capability, long *pValue, double *pdblValue )
{
	HRESULT result = S_OK;
	
	switch (Capability){
		 
		case ED_DEVCAP_CAN_RECORD:
			*pValue = m_DevCaps.CanRecord;
			break;
		case ED_DEVCAP_CAN_RECORD_STROBE:
			*pValue = m_DevCaps.CanRecordStrobe;
			break;
		case ED_DEVCAP_HAS_AUDIO:
			*pValue = m_DevCaps.HasAudio;
			break;
		case ED_DEVCAP_HAS_VIDEO:		
			*pValue = m_DevCaps.HasVideo;
			break;
		case ED_DEVCAP_USES_FILES:	
			*pValue = m_DevCaps.UsesFiles;
			break;
		case ED_DEVCAP_CAN_SAVE:		
			*pValue = m_DevCaps.CanSave;
			break;
		case ED_DEVCAP_DEVICE_TYPE:	
			*pValue = m_DevCaps.DeviceType;
			break;
		case ED_DEVCAP_TIMECODE_READ:			
			*pValue = m_DevCaps.TCRead;
			break;
		case ED_DEVCAP_TIMECODE_WRITE:	
			*pValue = m_DevCaps.TCWrite;
			break;
		case ED_DEVCAP_CTLTRK_READ: 			
			*pValue = m_DevCaps.CTLRead;
			break;
		case ED_DEVCAP_INDEX_READ:			
			*pValue = m_DevCaps.IndexRead;
			break;
		case ED_DEVCAP_PREROLL: 		
			*pValue = m_DevCaps.Preroll;
			break;
		case ED_DEVCAP_POSTROLL: 		
			*pValue = m_DevCaps.Postroll;
			break;
		case ED_DEVCAP_SYNC_ACCURACY: 	
			*pValue = m_DevCaps.SyncAcc;
			break;
		case ED_DEVCAP_NORMAL_RATE:
			*pValue = m_DevCaps.NormRate;
			break;
		case ED_DEVCAP_CAN_PREVIEW: 		
			*pValue = m_DevCaps.CanPreview;
			break;
		case ED_DEVCAP_CAN_MONITOR_SOURCES: 
			*pValue = m_DevCaps.CanMonitorSrc;
			break;
		case ED_DEVCAP_CAN_TEST:
			*pValue = m_DevCaps.CanTest;
			break;
		case ED_DEVCAP_VIDEO_INPUTS: 			
			*pValue = m_DevCaps.VideoIn;
			break;
		case ED_DEVCAP_AUDIO_INPUTS: 			
			*pValue = m_DevCaps.AudioIn;
			break;
		case ED_DEVCAP_NEEDS_CALIBRATING:
			*pValue = m_DevCaps.Calibrate;
			break;
		case ED_DEVCAP_SEEK_TYPE:
			*pValue = m_DevCaps.SeekType;
			break;
		default:
			result = VFW_E_NOT_FOUND;
	}	                           
	return S_OK;
}        

//------------------------------------------
//
//	get_ExternalDeviceID
//
// Get external device identification string
//------------------------------------------
HRESULT
CAMExtDevice::get_ExternalDeviceID(LPOLESTR * ppszData)
{
	int Cookie;
	
	CheckPointer(ppszData, E_POINTER);
	*ppszData = NULL;
	// query the device
	if (m_devcom->SendDevCmd(DEVICE_TYPE_REQUEST, &Cookie) )
		return E_FAIL;
	if (m_devcom->Wait4DevResponse(buf, Cookie))
		return E_FAIL;
	m_devcom->ProcessDeviceTypeResponse(buf, buf1);
	if (buf1 !=NULL) {
		*ppszData = (LPOLESTR)
			QzTaskMemAlloc(sizeof(WCHAR) * (1+lstrlenW((LPOLESTR)buf1)));
		if (*ppszData !=NULL)
			lstrcpyW(*ppszData, (LPOLESTR)buf1);
	}
	return S_OK;
}

//------------------------------------------
//
//	get_ExternalDeviceVersion
//
//------------------------------------------
HRESULT
CAMExtDevice::get_ExternalDeviceVersion(LPOLESTR * ppszData)
{
	// The sample machine doesn't support this, so send
	// a dummy response
	CheckPointer(ppszData, E_POINTER);
	*ppszData = NULL;
	// query the device
	*ppszData = (LPOLESTR)
			QzTaskMemAlloc(sizeof(WCHAR) * \
			(1+lstrlenW((LPOLESTR)strDummyDeviceVersion)));
	if (*ppszData !=NULL)
			lstrcpyW(*ppszData, (LPOLESTR)strDummyDeviceVersion);

	return S_OK;
}

//------------------------------------------
//
//	put_DevicePower	
//
// Controls the external device's power mode.
// Unfortunately, the sample machine doesn't support this
//------------------------------------------
HRESULT
CAMExtDevice::put_DevicePower(long PowerMode)
{
	TESTFLAG(PowerMode, OAFALSE);
	return E_NOTIMPL;
}

//------------------------------------------
//
//	get_DevicePower
//
//------------------------------------------
HRESULT 
CAMExtDevice::get_DevicePower(long *pPowerMode)
{
	// if the device port is being simulated, assume power off
	if (m_CurDevicePort == DEV_PORT_SIM)
		m_PowerMode = ED_POWER_OFF;
	else
		m_PowerMode = ED_POWER_ON;
	*pPowerMode = m_PowerMode;
	return S_OK;
}

//------------------------------------------
//
//	Calibrate
//
//------------------------------------------		
HRESULT
CAMExtDevice::Calibrate(HEVENT hEvent, long Mode, long *pStatus)
{
	TESTFLAG( Mode , OAFALSE);
	return E_NOTIMPL;
}

//------------------------------------------
//
//	get_DevicePort
//
//------------------------------------------
HRESULT
CAMExtDevice::get_DevicePort(long *pDevicePort)
{
	*pDevicePort = m_CurDevicePort;
	return S_OK;
}

//------------------------------------------
//
//	put_DevicePort
//
//------------------------------------------
HRESULT
CAMExtDevice::put_DevicePort(long DevicePort)
{
	TESTFLAG( DevicePort, OATRUE);

	if ( DevicePort != m_CurDevicePort ) {
		// range check
		if ((DevicePort < DEV_PORT_MIN) ||
			(DevicePort > DEV_PORT_MAX))
				return VFW_E_NOT_FOUND;	// find correct error code
		if ( (m_CurDevicePort != NULL) & (m_CurDevicePort !=DEV_PORT_SIM) )
			m_devcom->CloseDevPort();
		if (m_devcom->OpenDevPort(DevicePort))
			m_CurDevicePort = DEV_PORT_SIM;
		else
			m_CurDevicePort = DevicePort;
	}
	return S_OK;
}

//------------------------------------------
//
// CAMExtTransport implementation
//
//------------------------------------------
CAMExtTransport::CAMExtTransport(HRESULT * phr, TCHAR *tszName) :
	CUnknown( tszName, NULL )
{
	// initialize the video parms
	m_TranVidParms.OutputMode = ED_PLAYBACK;
	m_TranVidParms.Input = 0;		// use the first (zeroth) input as the default

	m_TranAudParms.EnableOutput = ED_AUDIO_ALL;
	m_TranAudParms.EnableRecord = 0L;
	m_TranAudParms.Input = 0;
	m_TranAudParms.MonitorSource = 0;

	m_TranStatus.Mode = ED_MODE_STOP;
	m_TranStatus.LastError = 0L;
	m_TranStatus.RecordInhibit = OAFALSE;
	m_TranStatus.ServoLock = OAFALSE;
	m_TranStatus.MediaPresent = OAFALSE;
	m_TranStatus.MediaLength = 
	m_TranStatus.MediaSize = 
	m_TranStatus.MediaTrackCount = 
	m_TranStatus.MediaTrackLength =
	m_TranStatus.MediaTrackSide =
	m_TranStatus.MediaType = 0L;
	m_TranStatus.LinkMode = OATRUE;	// default to "linked"

	m_localcontrol = FALSE;
}

//------------------------------------------
//
//	Destructor
//
//------------------------------------------
CAMExtTransport::~CAMExtTransport()
{

}

//------------------------------------------
//
// NonDelegatingQueryInterface
//
// Reveal our Interface
//------------------------------------------
STDMETHODIMP
CAMExtTransport::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	CAutoLock cObjectLock(&m_StateLock);

    if (riid == IID_IAMExtTransport)
        return GetInterface((IAMExtTransport *) this, ppv);
    else
        return CUnknown::NonDelegatingQueryInterface(riid, ppv);
} 

//------------------------------------------
//
//	SetCommunicationObject
//
//------------------------------------------
HRESULT 
CAMExtTransport::SetCommunicationObject(CDevCom *pCDevCom)
{
	m_pTDevCom = pCDevCom;
	return S_OK;
}

//------------------------------------------
//
//	GetCapability
//
// Transport capabilities property
//------------------------------------------
HRESULT 
CAMExtTransport::GetCapability(long Capability, long *pValue,
							   double *pdblValue )
{
	return E_NOTIMPL;
}

//------------------------------------------
//
//	put_MediaState
//
//------------------------------------------
HRESULT
CAMExtTransport::put_MediaState(long State)
{
	TESTFLAG(State, OATRUE);
	int Cookie;
	// the only thing we'll do here is eject - all others imply
	// a bigger state change (like STOP->FREEZE) that we'd like to 
	// do here
	if (State == ED_MEDIA_UNLOAD) {
		if (m_TranStatus.Mode != ED_MODE_STOP)
			CAMExtTransport::put_Mode(ED_MODE_STOP);
		if (m_pTDevCom->SendDevCmd(EJECT, &Cookie))
			return E_FAIL;
		if (m_pTDevCom->Wait4DevResponse(buf, Cookie))
			return E_FAIL;
		Sleep(4000);	// wait a bit until the machine completes
		m_mediastate = State;
		return S_OK;
	}
	return E_NOTIMPL;
}

//------------------------------------------
//
//	get_MediaState
//
//------------------------------------------
HRESULT
CAMExtTransport::get_MediaState(long FAR* pState)
{
	int Cookie;
	BOOL bStatusRet;

	// This might be different for a device other than a VCR, but:
	// ED_MEDIA_SPIN_UP: media inserted and not stopped
	// ED_MEDIA_SPIN_DOWN: media inserted and stopped
	// ED_MEDIA_UNLOAD: media ejected
	if (m_TranStatus.Mode == ED_MODE_STOP) {
		// check device for presence of media
		do {
			if (m_pTDevCom->SendDevCmd(REQUEST_STATUS, &Cookie))
				return E_FAIL;
			if (m_pTDevCom->Wait4DevResponse(buf, Cookie))
				return E_FAIL;
			bStatusRet = m_pTDevCom->ProcessDeviceStatusResponse(buf,
										&m_VcrStatus);
		} while (bStatusRet);
		if (m_VcrStatus.bCassetteOut == OATRUE)
			*pState = ED_MEDIA_UNLOAD;
		else
			*pState = ED_MEDIA_SPIN_DOWN;
	}
	else
		*pState = ED_MEDIA_SPIN_UP;

	m_mediastate = *pState;
	return S_OK;
}

//------------------------------------------
//
//	put_LocalControl
//
//------------------------------------------
HRESULT
CAMExtTransport::put_LocalControl(long State)
{
	TESTFLAG(State, OATRUE);
	long temp;
	int Cookie;
	DWORD dwResult;

	if (State == OATRUE)
		temp = LOCAL_ON;
	else
		temp = LOCAL_OFF;
	dwResult = m_pTDevCom->SendDevCmd(temp, &Cookie);
	if (dwResult == DEV_COMM_ERR_COMMAND_NOT_SUPPORTED)
		return E_NOTIMPL;
	if (dwResult)
		return E_FAIL;
	if (m_pTDevCom->Wait4DevResponse(buf, Cookie))
		return E_FAIL;
	return S_OK;
}

//------------------------------------------
//
//	get_LocalControl
//
//------------------------------------------
HRESULT
CAMExtTransport::get_LocalControl(long FAR* pState)
{
	int Cookie;

	if (m_pTDevCom->SendDevCmd(REQUEST_STATUS, &Cookie))
		return E_FAIL;
	if (m_pTDevCom->Wait4DevResponse(buf, Cookie))
		return E_FAIL;
	m_pTDevCom->ProcessDeviceStatusResponse(buf, &m_VcrStatus);
	*pState = m_VcrStatus.bLocal;
	return S_OK;
}

//------------------------------------------
//
//	GetStatus
//
//------------------------------------------
HRESULT
CAMExtTransport::GetStatus(long StatusItem, long FAR* pValue)
{
	switch (StatusItem) {
		case ED_LINK_MODE:
			*pValue = m_TranStatus.LinkMode;
			return S_OK;
		default:
			return E_NOTIMPL;
	}
}

//------------------------------------------
//
//	GetTransportBasicParameters
//
//------------------------------------------
HRESULT
CAMExtTransport::GetTransportBasicParameters(long Param, long FAR* pValue,
											 LPOLESTR * ppszData)
{
	return E_NOTIMPL;
}

//------------------------------------------
//
//	SetTransportBasicParameters
//
//------------------------------------------
HRESULT
CAMExtTransport::SetTransportBasicParameters(long Param, long Value,
											 LPCOLESTR pszData)
{
	// we test false because this is not implemented
	TESTFLAG(Param, OAFALSE);
	return E_NOTIMPL;
}

//------------------------------------------
//
//	GetTransportVideoParameters
//
//------------------------------------------
HRESULT
CAMExtTransport::GetTransportVideoParameters(long Param, long FAR* pValue)
{
	HRESULT hr = S_OK;

	switch (Param) {
		case ED_TRANSVIDEO_SET_OUTPUT:
			*pValue = m_TranVidParms.OutputMode;
			break;
		case ED_TRANSVIDEO_SET_SOURCE:
			*pValue = m_TranVidParms.Input;
			break;
		default:
			hr = VFW_E_NOT_FOUND;
	}
	return hr;
}

//------------------------------------------
//
//	SetTransportVideoParameters
//
//------------------------------------------
HRESULT
CAMExtTransport::SetTransportVideoParameters(long Param, long Value)
{
	// we test false because this is not implemented
	TESTFLAG(Param, OAFALSE);
	return E_NOTIMPL;	
}

//------------------------------------------
//
//	GetTransportAudioParameters
//
//------------------------------------------
HRESULT
CAMExtTransport::GetTransportAudioParameters(long Param, long FAR* pValue)
{
	HRESULT hr = S_OK;

	switch (Param) {
		case ED_TRANSAUDIO_ENABLE_OUTPUT:
			*pValue = m_TranAudParms.EnableOutput;
			break;
		case ED_TRANSAUDIO_ENABLE_RECORD:
			*pValue = m_TranAudParms.EnableRecord;
			break;
		case ED_TRANSAUDIO_SET_SOURCE:
			*pValue = m_TranAudParms.Input;
			break;
		case ED_TRANSAUDIO_SET_MONITOR:
			*pValue = m_TranAudParms.MonitorSource;
			break;
		default:
			hr = VFW_E_NOT_FOUND;
	}
	return hr;
}

//------------------------------------------
//
//	SetTransportAudioParameters
//
//------------------------------------------
HRESULT
CAMExtTransport::SetTransportAudioParameters(long Param, long Value)
{
	// we test false because this is not implemented
	TESTFLAG(Param, OAFALSE);
	return E_NOTIMPL;	
}

//------------------------------------------
//
//	put_Mode
//
//------------------------------------------
HRESULT
CAMExtTransport::put_Mode(long Mode)
{
	HRESULT result = S_OK;
	DWORD cmdresult;
	static TCHAR buf[32];
	int Cookie;
	
	TESTFLAG(Mode, OATRUE);
	if ( Mode == m_TranStatus.Mode )
			return S_OK;	// don't repeat commands
	if ( m_localcontrol == TRUE )
		return ED_ERR_DEVICE_NOT_READY;

	switch (Mode) {
		case ED_MODE_PLAY:
			cmdresult = m_pTDevCom->SendDevCmd(PLAY, &Cookie);
			ASSERT(cmdresult == 0L);
			if (cmdresult == 0L)
				if (m_pTDevCom->Wait4DevResponse(buf, Cookie))
					return E_FAIL;
			m_lastmode = m_TranStatus.Mode;
			m_TranStatus.Mode = ED_MODE_PLAY;
			break;
		case ED_MODE_STOP:
			cmdresult = m_pTDevCom->SendDevCmd(STOP, &Cookie);		
			ASSERT(cmdresult == 0L);
			if (m_pTDevCom->Wait4DevResponse(buf, Cookie))
				return E_FAIL;
			cmdresult = m_pTDevCom->SendDevCmd(STANDBY_OFF, &Cookie);
			// we know that some machines don't support STANDBY_OFF
			if (cmdresult != 0 &&
				cmdresult != DEV_COMM_ERR_COMMAND_NOT_SUPPORTED )
				ASSERT(cmdresult == 0L);
			if (cmdresult == 0L)
				if (m_pTDevCom->Wait4DevResponse(buf, Cookie))
					return E_FAIL;
			m_lastmode = m_TranStatus.Mode;
			m_TranStatus.Mode = ED_MODE_STOP;
			break;
		case ED_MODE_FREEZE:
			if (m_TranStatus.Mode == ED_MODE_STOP) {
				return result;
			}
			cmdresult = m_pTDevCom->SendDevCmd(FREEZE_ON, &Cookie);
			ASSERT(cmdresult == 0L);
			if (cmdresult == 0L)
				if (m_pTDevCom->Wait4DevResponse(buf, Cookie))
					return E_FAIL;
			m_lastmode = m_TranStatus.Mode;
			m_TranStatus.Mode = ED_MODE_FREEZE;
			break;
		case ED_MODE_THAW:
			cmdresult = m_pTDevCom->SendDevCmd(FREEZE_OFF, &Cookie);
			ASSERT(cmdresult == 0L);
			if (cmdresult == 0L)
				if (m_pTDevCom->Wait4DevResponse(buf, Cookie))
					return E_FAIL;
			m_TranStatus.Mode = m_lastmode;
			break;

		case ED_MODE_FF:			
			cmdresult = m_pTDevCom->SendDevCmd(FFWD, &Cookie);
			ASSERT(cmdresult == 0L);
			if (cmdresult == 0L)
				if (m_pTDevCom->Wait4DevResponse(buf, Cookie))
					return E_FAIL;
			m_lastmode = m_TranStatus.Mode;
			m_TranStatus.Mode = ED_MODE_FF;
			break;
		case ED_MODE_REW:
			cmdresult = m_pTDevCom->SendDevCmd(REWIND, &Cookie);
			ASSERT(cmdresult == 0L);
			if (cmdresult == 0L)
				if (m_pTDevCom->Wait4DevResponse(buf, Cookie))
					return E_FAIL;
			m_lastmode = m_TranStatus.Mode;
			m_TranStatus.Mode = ED_MODE_REW;
			break;
		case ED_MODE_RECORD:
			cmdresult = m_pTDevCom->SendDevCmd(RECORD, &Cookie);
			ASSERT(cmdresult == 0L);
			if (cmdresult == 0L)
				if (m_pTDevCom->Wait4DevResponse(buf, Cookie))
					return E_FAIL;
			m_lastmode = m_TranStatus.Mode;
			m_TranStatus.Mode = ED_MODE_RECORD;
			break;
		case ED_MODE_RECORD_STROBE:
			result = E_NOTIMPL;
			break;
		case ED_MODE_STEP:
			result = E_NOTIMPL;
			break;
		case ED_MODE_SHUTTLE:
			result = E_NOTIMPL;
			break;
		case ED_MODE_LINK_ON:
			m_TranStatus.LinkMode = OATRUE;
			break;
		case ED_MODE_LINK_OFF:
			m_TranStatus.LinkMode = OAFALSE;
			break;
		default:
			result = E_NOTIMPL;
	}
	// clean up if things went well
	return result;
}

//------------------------------------------
//
//	get_Mode
//
//------------------------------------------
HRESULT
CAMExtTransport::get_Mode(long FAR* pMode)
{
	*pMode = m_TranStatus.Mode;
	return S_OK;
}

//------------------------------------------
//
//	put_Rate
//
//------------------------------------------
HRESULT
CAMExtTransport::put_Rate(double dblRate)
{
	// add Test flag support
	return E_NOTIMPL;
}

//------------------------------------------
//
//	get_Rate
//
//------------------------------------------
HRESULT
CAMExtTransport::get_Rate(double FAR* pdblRate)
{
	return E_NOTIMPL;
}

//------------------------------------------
//
//	GetChase
//
//------------------------------------------
HRESULT
CAMExtTransport::GetChase(long FAR* pEnabled, long FAR* pOffset,
						  HEVENT FAR* phEvent)
{
	return E_NOTIMPL;
}

//------------------------------------------
//
//	SetChase
//
//------------------------------------------
HRESULT
CAMExtTransport::SetChase(long Enable, long Offset, HEVENT hEvent)
{
	return E_NOTIMPL;
}

//------------------------------------------
//
//	GetBump
//
//------------------------------------------
HRESULT
CAMExtTransport::GetBump(long FAR* pSpeed, long FAR* pDuration)
{
	return E_NOTIMPL;
}

//------------------------------------------
//
//	SetBump
//
//------------------------------------------
HRESULT
CAMExtTransport::SetBump(long Speed, long Duration)
{
	return E_NOTIMPL;
}

//------------------------------------------
//
//	get_AntiClogControl
//
//------------------------------------------
HRESULT
CAMExtTransport::get_AntiClogControl(long FAR* pEnabled)
{
	return E_NOTIMPL;
}

//------------------------------------------
//
//	put_AntiClogControl
//
//------------------------------------------
HRESULT
CAMExtTransport::put_AntiClogControl(long Enable)
{
	TESTFLAG(Enable, OAFALSE);
	return E_NOTIMPL;
}

//------------------------------------------
//
//	GetEditPropertySet
//
//------------------------------------------
HRESULT
CAMExtTransport::GetEditPropertySet(long EditID, long FAR* pState)
{
	return E_NOTIMPL;
}

//------------------------------------------
//
//	SetEditPropertySet
//
//------------------------------------------
HRESULT
CAMExtTransport::SetEditPropertySet(long FAR* pEditID, long State)
{
	TESTFLAG(State, OAFALSE);
	return E_NOTIMPL;
}

//------------------------------------------
//
//	GetEditProperty
//
//------------------------------------------
HRESULT
CAMExtTransport::GetEditProperty(long EditID, long Param, long FAR* pValue)
{
	return E_NOTIMPL;
}

//------------------------------------------
//
//	SetEditProperty
//
//------------------------------------------
HRESULT
CAMExtTransport::SetEditProperty(long EditID, long Param, long Value)
{
	return E_NOTIMPL;
}

//------------------------------------------
//
//	get_EditStart
//
//------------------------------------------
HRESULT
CAMExtTransport::get_EditStart(long FAR* pValue)
{
	return E_NOTIMPL;
}

//------------------------------------------
//
//	put_EditStart
//
//------------------------------------------
HRESULT
CAMExtTransport::put_EditStart(long Value)
{
	TESTFLAG(Value, OAFALSE);
	return E_NOTIMPL;
}

//------------------------------------------
//
// CAMTcr implementation
//
//------------------------------------------
CAMTcr::CAMTcr(HRESULT * phr, TCHAR *tszName) :
	CUnknown( tszName, NULL )
{
	// need some help with timecode
	m_timecode = new CTimecode();
}

//------------------------------------------
//
//	Destructor
//
//------------------------------------------
CAMTcr::~CAMTcr()
{

}
//------------------------------------------
//
// NonDelegatingQueryInterface
//
// Reveal our Interface
//------------------------------------------
STDMETHODIMP
CAMTcr::NonDelegatingQueryInterface(REFIID riid, void **ppv) {

	CAutoLock cObjectLock(&m_StateLock);

    if (riid == IID_IAMTimecodeReader)
        return GetInterface((IAMTimecodeReader *) this, ppv);
    else
        return CUnknown::NonDelegatingQueryInterface(riid, ppv);
} 

//------------------------------------------
//
//	SetCommunicationObject
//
//------------------------------------------
HRESULT 
CAMTcr::SetCommunicationObject(CDevCom *pCDevCom)
{
	m_pTCDevCom = pCDevCom;
	return S_OK;
}

//------------------------------------------
//
//	GetTCRMode
//
//------------------------------------------
HRESULT 
CAMTcr::GetTCRMode(long Param, long FAR* pValue)
{
	return E_NOTIMPL;
}

//------------------------------------------
//
//	get_VITCLine
//
//------------------------------------------
HRESULT 
CAMTcr::get_VITCLine(long * pLine)
{
	return E_NOTIMPL;
}

//------------------------------------------
//
//	put_VITCLine
//
//------------------------------------------
HRESULT 
CAMTcr::put_VITCLine(long Line)
{
	return E_NOTIMPL;
}

//------------------------------------------
//
//	SetTCRMode
//
//------------------------------------------
HRESULT 
CAMTcr::SetTCRMode(long Param, long Value)
{
	TCHAR buf[20];
	int Cookie;
	DWORD dwResult;

	TESTFLAG(Param, OATRUE);
	// the real implementation will use a table instead
	//	of a switch.  This seriously partial implementation
	//	is just to get basic timecode reading right
	switch (Param)
	{
		case ED_TCR_SOURCE:
			if ( Value == ED_TCR_LTC ) {
				dwResult = m_pTCDevCom->SendDevCmd(TIMER_MODE_SELECT_LTC,
										&Cookie);
				// we know that some machines don't support this command
				// and don't even ask for an answer
				if ( dwResult == DEV_COMM_ERR_COMMAND_NOT_SUPPORTED)
					return E_NOTIMPL;
				if ( dwResult )	// fail on all other result codes
					return E_FAIL;
				if (m_pTCDevCom->Wait4DevResponse(buf, Cookie))
					return E_FAIL;
			}
			return S_OK;
		default:
			return E_NOTIMPL;
	}
	return E_NOTIMPL;
}

//------------------------------------------
//
//	GetTimecode
//
//------------------------------------------
HRESULT 
CAMTcr::GetTimecode( PTIMECODE_SAMPLE pTimecodeSample)
{
	DWORD cmdresult;
	static TCHAR buf[32];
	DWORD fcm;
	int Cookie;
       LONG lFrame, lUser ;

	cmdresult = m_pTCDevCom->SendDevCmd(READ_TC, &Cookie);
	if ( cmdresult == 0L )
	{	// commands might get refused
		if ( (cmdresult = m_pTCDevCom->Wait4DevResponse(buf, Cookie)) )
			return cmdresult;//E_FAIL;
		DbgLog((LOG_TRACE, 1, buf));
		m_pTCDevCom->ProcessVcrTC(buf, &fcm, &lFrame, &lUser);
		pTimecodeSample->timecode.wFrameRate = (WORD)fcm;
		pTimecodeSample->timecode.dwFrames = (DWORD) lFrame ;
		pTimecodeSample->dwUser = (DWORD) lUser ;
		pTimecodeSample->dwFlags = 0L;
		return S_OK;
	}
	// leave the reference time alone for now 
	// (to be filled in by calling filter)
	return S_FALSE;
}

//eof  cvcrutil.cpp

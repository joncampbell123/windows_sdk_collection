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
// filename: cvcrutil.h
//
// base abstract classes implementing the device control interfaces
// derive your filter (or an embedded class) from one of these and implement
// the pure virtual property and method functions
//

#ifndef __CVCRUTIL__
#define __CVCRUTIL__


// a macro to screen for the "Test" flag
#define TESTFLAG(a,b) if( a & 0x8000 ) return b


//---------------------------------------------------------
//
// Structures for this implementation
//
//---------------------------------------------------------

// someplace to put our properties
typedef struct tagDEVCAPS{
	long CanRecord;
	long CanRecordStrobe;
	long HasAudio;
	long HasVideo;
	long UsesFiles;
	long CanSave;
	long DeviceType;
	long TCRead;
	long TCWrite;
	long CTLRead;
	long IndexRead;
	long Preroll;
	long Postroll;
	long SyncAcc;
	long NormRate;
	long CanPreview;
	long CanMonitorSrc;
	long CanTest;
	long VideoIn;
	long AudioIn;
	long Calibrate;
	long SeekType;
	long SimulatedHardware;		// private
} DEVCAPS;

typedef DEVCAPS far *PDEVCAPS;

// transport status
typedef struct tagTRANSPORTSTATUS{
	long Mode;
	long LastError;
	long RecordInhibit;
	long ServoLock;
	long MediaPresent;
	long MediaLength;
	long MediaSize;
	long MediaTrackCount;
	long MediaTrackLength;
	long MediaTrackSide;
	long MediaType;
	long LinkMode;
} TRANSPORTSTATUS;
typedef TRANSPORTSTATUS far *PTRANSPORTSTATUS;

// transport basic parameters
typedef struct tagTRANSPORTBASICPARMS{
	long TimeFormat;
	long TimeReference;
	long Superimpose;
	long EndStopAction;
	long RecordFormat;
	long StepFrames;
	long SetpField;
	long Preroll;
	long RecPreroll;
	long Postroll;
	long EditDelay;
	long PlayTCDelay;
	long RecTCDelay;
	long EditField;
	long FrameServo;
	long ColorFrameServo;
	long ServoRef;
	long WarnGenlock;
	long SetTracking;
	TCHAR VolumeName[40];
	long Ballistic[20];
	long Speed;
	long CounterFormat;
	long TunerChannel;
	long TunerNumber;
	long TimerEvent;
	long TimerStartDay;
	long TimerStartTime;
	long TimerStopDay;
	long TimerStopTime;
} TRANSPORTBASICPARMS;
typedef TRANSPORTBASICPARMS far *PTRANSPORTBASICPARMS;

// transport video parameters
typedef struct tagTRANSPORTVIDEOPARMS{
	long OutputMode;
	long Input;
} TRANSPORTVIDEOPARMS;
typedef TRANSPORTVIDEOPARMS far *PTRANSPORTVIDEOPARMS;

// transport audio parameters
typedef struct tagTRANSPORTAUDIOPARMS{
	long EnableOutput;
	long EnableRecord;
	long EnableSelsync;
	long Input;
	long MonitorSource;
} TRANSPORTAUDIOPARMS;
typedef TRANSPORTAUDIOPARMS far *PTRANSPORTAUDIOPARMS;


//---------------------------------------------------------
//
// Here's the temporary CAMPhysicalPinInfo class to be used with the input pins
//
//---------------------------------------------------------
class CAMPhysicalPinInfo : public CUnknown, public IAMPhysicalPinInfo
{
public:

	CAMPhysicalPinInfo(HRESULT * phr, TCHAR *tszName);
    virtual ~CAMPhysicalPinInfo();
	
	DECLARE_IUNKNOWN
	// override this to say what interfaces we support where
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

	/* IAMPhysicalPinInfo method  - to be implemented by derived class */
	virtual STDMETHODIMP GetPhysicalType(long *pType, LPOLESTR * ppszType) PURE;

protected:
	CCritSec m_StateLock;	// Lock this to serialize function accesses to the 
							//  filter state

private:
	CCritSec  *m_pCritSec;	// Object we use for locking

};

//---------------------------------------------------------
//
//	IAMExtDevice Implementation
//
//---------------------------------------------------------

class CAMExtDevice : public CUnknown, public IAMExtDevice
{
public:

	CAMExtDevice(HRESULT * phr, TCHAR *tszName);
    virtual ~CAMExtDevice();

	DECLARE_IUNKNOWN
	// override this to say what interfaces we support where
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);    
	
	/* IAMExtDevice methods */
	STDMETHODIMP GetCapability (long Capability, long FAR* pValue,
								double FAR* pdblValue);
    STDMETHODIMP get_ExternalDeviceID(LPOLESTR * ppszData);
    STDMETHODIMP get_ExternalDeviceVersion(LPOLESTR * ppszData);
    STDMETHODIMP put_DevicePower(long PowerMode);
    STDMETHODIMP get_DevicePower(long FAR* pPowerMode);
    STDMETHODIMP Calibrate(HEVENT hEvent, long Mode, long FAR* pStatus);
	STDMETHODIMP get_DevicePort(long FAR * pDevicePort);
	STDMETHODIMP put_DevicePort(long DevicePort);

protected:
	CDevCom *m_devcom;		// how to talk to the machine
	CCritSec m_StateLock;	// Lock this to serialize function accesses 
							//  to the filter state

private:
	CCritSec  *m_pCritSec;	// Object we use for locking
	DEVCAPS m_DevCaps;		// external device capabilities
	
	int m_CurDevicePort;	// how we are talking to the machine
	int m_PowerMode;		// on, off or standby
};

//---------------------------------------------------------
//
// IAMExtTransport implementation
//
// Transport Class - VCRs, ATRs, DDRs, etc.
//
//---------------------------------------------------------
class CAMExtTransport : public CUnknown, public IAMExtTransport
{
public:

	CAMExtTransport(HRESULT * phr, TCHAR *tszName);
    virtual ~CAMExtTransport();

   	DECLARE_IUNKNOWN
    // override this to say what interfaces we support where
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

	/* IAMExtTransport methods */
	STDMETHODIMP GetCapability (long Capability, long FAR* pValue,
								double FAR* pdblValue);
    STDMETHODIMP put_MediaState(long State);
    STDMETHODIMP get_MediaState(long FAR* pState);
    STDMETHODIMP put_LocalControl(long State);
    STDMETHODIMP get_LocalControl(long FAR* pState);
    STDMETHODIMP GetStatus(long StatusItem, long FAR* pValue);
    STDMETHODIMP GetTransportBasicParameters(long Param, long FAR* pValue,
											LPOLESTR * ppszData);
    STDMETHODIMP SetTransportBasicParameters(long Param, long Value,
											LPCOLESTR pszData);
    STDMETHODIMP GetTransportVideoParameters(long Param, long FAR* pValue);
    STDMETHODIMP SetTransportVideoParameters(long Param, long Value);
    STDMETHODIMP GetTransportAudioParameters(long Param, long FAR* pValue);
    STDMETHODIMP SetTransportAudioParameters(long Param, long Value);
    STDMETHODIMP put_Mode(long Mode);
    STDMETHODIMP get_Mode(long FAR* pMode);
    STDMETHODIMP put_Rate(double dblRate);
    STDMETHODIMP get_Rate(double FAR* pdblRate);
    STDMETHODIMP GetChase(long FAR* pEnabled, long FAR* pOffset,
							HEVENT FAR* phEvent);
    STDMETHODIMP SetChase(long Enable, long Offset, HEVENT hEvent);
    STDMETHODIMP GetBump(long FAR* pSpeed, long FAR* pDuration);
    STDMETHODIMP SetBump(long Speed, long Duration);
    STDMETHODIMP get_AntiClogControl(long FAR* pEnabled);
    STDMETHODIMP put_AntiClogControl(long Enable);
    STDMETHODIMP GetEditPropertySet(long EditID, long FAR* pState);
    STDMETHODIMP SetEditPropertySet(long FAR* pEditID, long State);
    STDMETHODIMP GetEditProperty(long EditID, long Param, long FAR* pValue);
    STDMETHODIMP SetEditProperty(long EditID, long Param, long Value);
    STDMETHODIMP get_EditStart(long FAR* pValue);
    STDMETHODIMP put_EditStart(long Value);

	// The communications object can change depending on the low-level
	// protocol and/or hardware connection
	HRESULT SetCommunicationObject(CDevCom *);

protected:
	CCritSec m_StateLock;		// Lock this to serialize function accesses 
								//  to the filter state

private:
	CDevCom *m_pTDevCom;				// how to talk to the machine
	TRANSPORTSTATUS	m_TranStatus;		// current status
	TRANSPORTVIDEOPARMS m_TranVidParms;	// keep all capabilities, etc. here
	TRANSPORTAUDIOPARMS m_TranAudParms;
	TRANSPORTBASICPARMS m_TranBasicParms;
		
	// a bunch of properties we want to remember
	long m_lastmode;
	long m_mediastate;
	long m_localcontrol;
	VCRSTATUS m_VcrStatus;		// raw status from VCR
	
};

//---------------------------------------------------------
//
// IAMTimecodeReader implementation
//
//---------------------------------------------------------

// Timecode Reader class
class CAMTcr : public CUnknown, public IAMTimecodeReader
{
public:

	CAMTcr(HRESULT * phr, TCHAR *tszName);
    virtual ~CAMTcr();

   	DECLARE_IUNKNOWN
	// override this to say what interfaces we support where
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

    /* IAMTimecodeReader methods */
    STDMETHODIMP GetTCRMode( long Param, long FAR* pValue);
    STDMETHODIMP SetTCRMode( long Param, long Value);
    STDMETHODIMP put_VITCLine( long Line);
    STDMETHODIMP get_VITCLine( long FAR* pLine);
    STDMETHODIMP GetTimecode( PTIMECODE_SAMPLE pTimecodeSample);

	// helper to connect to the communications object.  This
	// is an implementation-specific method
	HRESULT SetCommunicationObject(CDevCom *);

protected:
	CCritSec m_StateLock;		// Lock this to serialize function accesses
								//  to the filter state

private:
	CDevCom *m_pTCDevCom;		// how to talk to the device
	CTimecode *m_timecode;
};

#endif // __CVCRUTIL__

// eof cvcrutil.h

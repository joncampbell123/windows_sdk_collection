//=============================================================================
//  Microsoft (R) Network Monitor (tm). 
//  Copyright (C) 1991-1999. All rights reserved.
//
//  MODULE: netmon.h
//
//  This is the top-level include file for all Network Monitor components.
//=============================================================================

#if !defined(_NM_)

#define _NM_

#include "npptypes.h"               //... Network types.
#include "nmsupp.h"                 //... Supplemental API's.
#include "bhtypes.h"                //... Type definitions for BH.
#include "nmerr.h"                  //... Error code definitions for BH.
#include "bhfilter.h"               //... Filter definitions.
#include "parser.h"                 //... Parser helper functions and structures.
#include "inilib.h"
#include "nmevent.h"
#include "nmexpert.h"

#pragma pack(1)

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
//  A frame with no number contains this value as its frame number.
//=============================================================================

#define INVALID_FRAME_NUMBER            ((DWORD) -1)

//=============================================================================
//  Capture file flags.
//=============================================================================

#define CAPTUREFILE_OPEN                OPEN_EXISTING
#define CAPTUREFILE_CREATE              CREATE_NEW

//=============================================================================
//  CAPTURE CONTEXT API's.
//=============================================================================

extern LPSYSTEMTIME         WINAPI GetCaptureTimeStamp(HCAPTURE hCapture);

extern DWORD                WINAPI GetCaptureMacType(HCAPTURE hCapture);

extern DWORD                WINAPI GetCaptureTotalFrames(HCAPTURE hCapture);

extern LPSTR                WINAPI GetCaptureComment(HCAPTURE hCapture);

//=============================================================================
//  FRAME HELP API's.
//=============================================================================

extern DWORD                WINAPI MacTypeToAddressType(DWORD MacType);

extern DWORD                WINAPI AddressTypeToMacType(DWORD AddressType);

extern DWORD                WINAPI GetFrameDstAddressOffset(HFRAME hFrame, DWORD AddressType, LPDWORD AddressLength);

extern DWORD                WINAPI GetFrameSrcAddressOffset(HFRAME hFrame, DWORD AddressType, LPDWORD AddressLength);

extern HCAPTURE             WINAPI GetFrameCaptureHandle(HFRAME hFrame);


extern DWORD                WINAPI GetFrameDestAddress(HFRAME       hFrame,
                                                       LPADDRESS    lpAddress,
                                                       DWORD        AddressType,
                                                       DWORD        Flags);

extern DWORD                WINAPI GetFrameSourceAddress(HFRAME     hFrame,
                                                         LPADDRESS  lpAddress,
                                                         DWORD      AddressType,
                                                         DWORD      Flags);

extern DWORD                WINAPI GetFrameMacHeaderLength(HFRAME hFrame);

extern BOOL                 WINAPI CompareFrameDestAddress(HFRAME hFrame, LPADDRESS lpAddress);

extern BOOL                 WINAPI CompareFrameSourceAddress(HFRAME hFrame, LPADDRESS lpAddress);

extern DWORD                WINAPI GetFrameLength(HFRAME hFrame);

extern DWORD                WINAPI GetFrameStoredLength(HFRAME hFrame);

extern DWORD                WINAPI GetFrameMacType(HFRAME hFrame);

extern DWORD                WINAPI GetFrameMacHeaderLength(HFRAME hFrame);

extern DWORD                WINAPI GetFrameNumber(HFRAME hFrame);

extern __int64              WINAPI GetFrameTimeStamp(HFRAME hFrame);

extern ULPFRAME             WINAPI GetFrameFromFrameHandle(HFRAME hFrame);

//=============================================================================
//  FRAME API's.
//=============================================================================

extern HFRAME               WINAPI ModifyFrame(HCAPTURE hCapture,
                                               DWORD    FrameNumber,
                                               LPBYTE   FrameData,
                                               DWORD    FrameLength,
                                               __int64  TimeStamp);

extern HFRAME               WINAPI FindNextFrame(HFRAME hCurrentFrame,
                                                LPSTR ProtocolName,
                                                LPADDRESS lpDesstAddress,
                                                LPADDRESS lpSrcAddress,
                                                LPWORD ProtocolOffset,
                                                DWORD  OriginalFrameNumber,
                                                DWORD  nHighestFrame);

extern HFRAME               WINAPI FindPreviousFrame(HFRAME hCurrentFrame,
                                                    LPSTR ProtocolName,
                                                    LPADDRESS lpDstAddress,
                                                    LPADDRESS lpSrcAddress,
                                                    LPWORD ProtocolOffset,
                                                    DWORD  OriginalFrameNumber,
                                                    DWORD  nLowestFrame );

extern HCAPTURE             WINAPI GetFrameCaptureHandle(HFRAME);

extern HFRAME               WINAPI GetFrame(HCAPTURE hCapture, DWORD FrameNumber);

extern LPRECOGNIZEDATATABLE WINAPI GetFrameRecognizeData(HFRAME hFrame);

//=============================================================================
//  Protocol API's.
//=============================================================================

extern HPROTOCOL            WINAPI CreateProtocol(LPSTR ProtocolName,
                                                  LPENTRYPOINTS lpEntryPoints,
                                                  DWORD cbEntryPoints);

extern VOID                 WINAPI DestroyProtocol(HPROTOCOL hProtocol);

extern LPPROTOCOLINFO       WINAPI GetProtocolInfo(HPROTOCOL hProtocol);

extern HPROPERTY            WINAPI GetProperty(HPROTOCOL hProtocol, LPSTR PropertyName);

extern HPROTOCOL            WINAPI GetProtocolFromName(LPSTR ProtocolName);

extern DWORD                WINAPI GetProtocolStartOffset(HFRAME hFrame, LPSTR ProtocolName);

extern DWORD                WINAPI GetProtocolStartOffsetHandle(HFRAME hFrame, HPROTOCOL hProtocol);

extern DWORD                WINAPI GetPreviousProtocolOffsetByName(HFRAME hFrame,
                                                                   DWORD  dwStartOffset,
                                                                   LPSTR  szProtocolName,
                                                                   DWORD* pdwPreviousOffset);

extern LPPROTOCOLTABLE      WINAPI GetEnabledProtocols(HCAPTURE hCapture);

//=============================================================================
//  Property API's.
//=============================================================================

extern DWORD                WINAPI CreatePropertyDatabase(HPROTOCOL hProtocol, DWORD nProperties);

extern DWORD                WINAPI DestroyPropertyDatabase(HPROTOCOL hProtocol);

extern HPROPERTY            WINAPI AddProperty(HPROTOCOL hProtocol, LPPROPERTYINFO PropertyInfo);

extern BOOL                 WINAPI AttachPropertyInstance(HFRAME    hFrame,
                                                          HPROPERTY hProperty,
                                                          DWORD     Length,
                                                          ULPVOID   lpData,
                                                          DWORD     HelpID,
                                                          DWORD     Level,
                                                          DWORD     IFlags);

extern BOOL                 WINAPI AttachPropertyInstanceEx(HFRAME      hFrame,
                                                            HPROPERTY   hProperty,
                                                            DWORD       Length,
                                                            ULPVOID     lpData,
                                                            DWORD       ExLength,
                                                            ULPVOID     lpExData,
                                                            DWORD       HelpID,
                                                            DWORD       Level,
                                                            DWORD       IFlags);

extern LPPROPERTYINST       WINAPI FindPropertyInstance(HFRAME hFrame, HPROPERTY hProperty);

extern LPPROPERTYINST       WINAPI FindPropertyInstanceRestart (HFRAME      hFrame, 
                                                                HPROPERTY   hProperty, 
                                                                LPPROPERTYINST *lpRestartKey, 
                                                                BOOL        DirForward );

extern LPPROPERTYINFO       WINAPI GetPropertyInfo(HPROPERTY hProperty);

extern LPSTR                WINAPI GetPropertyText(HFRAME hFrame, LPPROPERTYINST lpPI, LPSTR szBuffer, DWORD BufferSize);

//=============================================================================
//  MISC. API's.
//=============================================================================

extern DWORD                WINAPI GetCaptureCommentFromFilename(LPSTR lpFilename, LPSTR lpComment, DWORD BufferSize);

extern int                  WINAPI CompareAddresses(LPADDRESS lpAddress1, LPADDRESS lpAddress2);

extern DWORD                WINAPIV FormatPropertyInstance(LPPROPERTYINST lpPropertyInst, ...);

extern SYSTEMTIME *         WINAPI AdjustSystemTime(SYSTEMTIME *SystemTime, __int64 TimeDelta);

//=============================================================================
//  EXPERT API's for use by Experts
//=============================================================================

DWORD WINAPI ExpertGetFrame( IN HEXPERTKEY hExpertKey,
                             IN DWORD Direction,
                             IN DWORD RequestFlags,
                             IN DWORD RequestedFrameNumber,
                             IN HFILTER hFilter,
                             OUT LPEXPERTFRAMEDESCRIPTOR pEFrameDescriptor);

LPVOID WINAPI ExpertAllocMemory( IN  HEXPERTKEY hExpertKey,
                                 IN  SIZE_T nBytes,
                                 OUT DWORD* pError);

LPVOID WINAPI ExpertReallocMemory( IN  HEXPERTKEY hExpertKey,
                                   IN  LPVOID pOriginalMemory,
                                   IN  SIZE_T nBytes,
                                   OUT DWORD* pError);

DWORD WINAPI ExpertFreeMemory( IN HEXPERTKEY hExpertKey,
                               IN LPVOID pOriginalMemory);

SIZE_T WINAPI ExpertMemorySize( IN HEXPERTKEY hExpertKey,
                                IN LPVOID pOriginalMemory);

DWORD WINAPI ExpertIndicateStatus( IN HEXPERTKEY              hExpertKey, 
                                   IN EXPERTSTATUSENUMERATION Status,
                                   IN DWORD                   SubStatus,
                                   IN const char *            szText,
                                   IN LONG                    PercentDone);

DWORD WINAPI ExpertSubmitEvent( IN HEXPERTKEY   hExpertKey,
                                IN PNMEVENTDATA pExpertEvent);

DWORD WINAPI ExpertGetStartupInfo( IN  HEXPERTKEY hExpertKey,
                                   OUT PEXPERTSTARTUPINFO pExpertStartupInfo);

//=============================================================================
//  MISC. API's.
//=============================================================================

extern DWORD                WINAPI BHGetLastError(VOID);

//=============================================================================
//  DEBUG API's.
//=============================================================================

#ifdef DEBUG

//=============================================================================
//  BreakPoint() macro.
//=============================================================================

// We do not want breakpoints in our code any more...
// so we are defining DebugBreak(), usually a system call, to be
// just a dprintf. BreakPoint() is still defined as DebugBreak().


#ifdef DebugBreak
#undef DebugBreak
#endif

#define DebugBreak()    dprintf("DebugBreak Called at %s:%s", __FILE__, __LINE__);
#define BreakPoint()    DebugBreak()

#endif

#ifdef __cplusplus
}
#endif

#pragma pack()
#endif

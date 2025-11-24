;****************************************************************************
;                                                                           *
; THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
; KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
; IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
; PURPOSE.                                                                  *
;                                                                           *
; Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
;                                                                           *
;****************************************************************************

Begin_Win32_Services VCOMM

VCOMM_Win32_Service	_VCOMMGetVersion
VCOMM_Win32_Service	_VCOMMOpenComm
VCOMM_Win32_Service	_VCOMMSetupComm
VCOMM_Win32_Service	_VCOMMEscapeCommFunction
VCOMM_Win32_Service	_VCOMMGetCommMask
VCOMM_Win32_Service	_VCOMMGetCommProp
VCOMM_Win32_Service	_VCOMMGetCommState
VCOMM_Win32_Service	_VCOMMGetCommTimeouts
VCOMM_Win32_Service	_VCOMMPurgeComm
VCOMM_Win32_Service	_VCOMMSetCommMask
VCOMM_Win32_Service	_VCOMMSetCommState
VCOMM_Win32_Service	_VCOMMSetCommTimeouts
VCOMM_Win32_Service	_VCOMMTransmitCommChar
VCOMM_Win32_Service	_VCOMMWaitCommEvent
VCOMM_Win32_Service	_VCOMMGetCommModemStatus
VCOMM_Win32_Service	_VCOMMWriteComm
VCOMM_Win32_Service	_VCOMMReadComm
VCOMM_Win32_Service	_VCOMMClearCommError
VCOMM_Win32_Service	_VCOMMCloseComm
VCOMM_Win32_Service	_VCOMMGetLastError
VCOMM_Win32_Service	_VCOMMDequeueRequest
VCOMM_Win32_Service	_VCOMMQueryFriendlyName
VCOMM_Win32_Service	_VCOMMGetCommConfig
VCOMM_Win32_Service	_VCOMMSetCommConfig
VCOMM_Win32_Service	_VCOMMGetWin32Error
VCOMM_Win32_Service	_VCOMMFlushFileBuffers
VCOMM_Win32_Service	_VCOMMDeviceIOControl

End_Win32_Services VCOMM

_Win32DCB	STRUC

W32DCBLength	dd	?	; size of DCB
W32BaudRate	dd	?	; baud rate at which port is working
W32BitMask	dd	?	; misc flags
W32Reserved	dw	?	; reserved stuff
W32XonLim	dw	?	; Xmit X-ON threshold
W32XoffLim	dw	?	; Xmit X-OFF threshold
W32ByteSize	db	?	; size of byte
W32Parity	db	?	; parity
W32StopBits	db	?
W32XonChar	db	?
W32XoffChar	db	?
W32ErrorChar	db	?
W32EofChar	db	?
W32EvtChar	db	?
W32PackWORD	dw	?

_Win32DCB	ENDS

Win32DCB	TYPEDEF	_Win32DCB

fWin32Mask	EQU	(fBaudRate+fBitMask+fXonLim+fXoffLim+fByteSize+ \
			 fbParity+fStopBits+fXonChar+fXoffChar+fErrorChar+ \
			 fEofChar+fEvtChar1)

_Win32ComStat	STRUC

W32Status	dd	?	; various status flags
W32CBInQue	dd	?	; #bytes in Rx Q
W32CBOutQue	dd	?	; #bytes in Tx q

_Win32ComStat	ENDS

Win32ComStat	TYPEDEF	_Win32ComStat

;
; DTR control values
;
DTR_CONTROL_DISABLE	EQU	0
DTR_CONTROL_ENABLE	EQU	1
DTR_CONTROL_HANDSHAKE	EQU	2

;
; RTS control flow values
;
RTS_CONTROL_DISABLE	EQU	0
RTS_CONTROL_ENABLE	EQU	1
RTS_CONTROL_HANDSHAKE	EQU	2

_Win32COMMTIMEOUTS	STRUC

ReadIntervalTimeout		dd ?	; Maximum time between read chars */
ReadTotalTimeoutMultiplier	dd ?	; multiplier of chars
ReadTotalTimeoutConstant	dd ?	; constant in ms
WriteTotalTimeoutMultiplier	dd ?	; Multiplier of chars
WriteTotalTimeoutConstant	dd ?	; Constant in ms

_Win32COMMTIMEOUTS	ENDS

Win32COMMTIMEOUTS	TYPEDEF	_Win32COMMTIMEOUTS

_COMM_CONFIG	STRUCT	4t
cc_dwSize	DWORD		?
cc_wVersion	WORD		?
cc_wAlignDCB	WORD		?
cc_dcb		_Win32DCB	<>
cc_dwProviderSubType	DWORD	?
cc_dwProviderOffset	DWORD	?
cc_dwProviderSize	DWORD	?
cc_wcProviderData	BYTE	1t DUP (?)
_COMM_CONFIG	ENDS

COMM_CONFIG	TYPEDEF	_COMM_CONFIG


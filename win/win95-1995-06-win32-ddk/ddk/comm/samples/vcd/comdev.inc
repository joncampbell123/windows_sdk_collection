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

;=========================================================================
; Communications Device Driver Definitions
;=========================================================================

.xcref

; Device Control Block

DCB struc
  ID            db      ?               ;Device ID byte
  BaudRate      dw      ?               ;Baud Rate
  ByteSize      db      ?               ;TX/RX byte size
  Parity        db      ?               ;Parity
  StopBits      db      ?               ;Stop bits
  RLSTimeout    dw      ?               ;Line-Signal-Detect Timeout
  CTSTimeout    dw      ?               ;Clear-To-Send Timeout
  DSRTimeout    dw      ?               ;Data-Set-Ready Timeout
  Flags         db      ?               ;Flags byte
  Flags2        db      ?               ;More flags
  XONChar       db      ?               ;X-ON Character
  XOFFChar      db      ?               ;X-OFF Character
  XONLim        dw      ?               ;X-ON Threshold
  XOFFLim       dw      ?               ;X-OFF Threshold
  PEChar        db      ?               ;Parity Error Replacement char
  EOFChar       db      ?               ;End of Input Character
  EVTChar       db      ?               ;Event-generating character
  TXDelay       dw      ?               ;Transmit Delay
DCB ends

DCBSize         equ     SIZE DCB

LPTx            equ     10000000b       ;Flags an ID as being an LPT port


; Flag Byte 1 equates

fBinary         equ     00000001b       ;Binary mode
fRTSDisable     equ     00000010b       ;Disable RTS
fParity         equ     00000100b       ;Perform Parity Checking
fOutXCTSFlow    equ     00001000b       ;Output handshaking using CTS
fOutXDSRFlow    equ     00010000b       ;Output handshaking using DSR
fEnqAck         equ     00100000b       ;ENQ/ACK software handshaking [rkh] ...
fEtxAck         equ     01000000b       ;ETX/ACK software handshaking
fDTRDisable     equ     10000000b       ;Disable DTR


; Flag Byte 2 equates

fOutX           equ     00000001b       ;Output X-ON/X-OFF
fInX            equ     00000010b       ;Input X-ON/X-OFF
fPErrChar       equ     00000100b       ;Parity Error Replacement char active
fNullStrip      equ     00001000b       ;Null Stripping
fCharEvent      equ     00010000b       ;Character event
fDTRFlow        equ     00100000b       ;Input handshaking using DTR
fRTSFlow        equ     01000000b       ;Input handshaking using RTS
;               equ     10000000b


; Values for Parity

NoParity        equ     0
OddParity       equ     1
EvenParity      equ     2
MarkParity      equ     3
SpaceParity     equ     4


; Values for RLSTimeout, CTSTimeout, DSRTimeout

Ignore          equ     0
Infinite        equ     0FFFFh


; Values For StopBits

OneStopBit      equ     0
One5StopBits    equ     1
TwoStopBits     equ     2


; Comm Device Driver Error Bits Stored in ComErr

CE_RXOVER       equ     0001h           ;Receive Queue overflow
CE_OVERRUN      equ     0002h           ;Receive Overrun Error
CE_RXPARITY     equ     0004h           ;Receive Parity Error
CE_FRAME        equ     0008h           ;Receive Framing error
CE_BREAK        equ     0010h           ;Never told them about this!
CE_CTSTO        equ     0020h           ;CTS Timeout
CE_DSRTO        equ     0040h           ;DSR Timeout
CE_RLSDTO       equ     0080h           ;RLSD Timeout
CE_TXFULL       equ     0100h           ;TX Queue is full
CE_PTO          equ     0200h           ;LPTx Timeout
CE_IOE          equ     0400h           ;LPTx I/O Error
CE_DNS          equ     0800h           ;LPTx Device not selected
CE_OOP          equ     1000h           ;LPTx Out-Of-Paper
;               equ     2000h
;               equ     4000h
CE_MODE         equ     8000h           ;Requested mode unsupported


; Status structure (as returned by $STACOM)

stat struc
  StatFlags     db      ?               ;Various flags
  StatRxCount   dw      ?               ;Count of bytes in Rx queue
  StatTxCount   dw      ?               ;Count of bytes in Tx queue
stat ends


; Status structure flag byte equates

fCTSHold        equ     00000001b       ;Tx is on CTS hold
fDSRHold        equ     00000010b       ;Tx is on DSR hold
fRLSDHold       equ     00000100b       ;Tx is on RLSD hold
fXOFFHold       equ     00001000b       ;Received an X-OFF
fXOFFSent       equ     00010000b       ;Sent an X-OFF
fEOF            equ     00100000b       ;Received defined EOF character
fTxImmed        equ     01000000b       ;There's a char to transmit immediate
;               equ     10000000b


; Initialization Error Codes

IE_BadID        equ     -1              ;Invalid or unsupported id
IE_Open         equ     -2              ;Device Already Open
IE_NOpen        equ     -3              ;Device Not Open
IE_Memory       equ     -4              ;Unable to allocate queues
IE_Default      equ     -5              ;Error in default parameters
IE_Hardware     equ     -10             ;Hardware Not Present
IE_ByteSize     equ     -11             ;Illegal Byte Size
IE_Baudrate     equ     -12             ;Unsupported BaudRate


;       Event mask definitions.  Used by SetCommEventMask and GetCommEventMask
;
;       RXCHAR  - Set when any character is received and placed in the input
;                 queue.
;       RXFLAG  - Set when a particular character, as defined in the DCB,
;                 is received and placed in the input queue.
;       TXEMPTY - Set when the last character in the transmit queue is
;                 transmitted.
;       CTS     - Set when the CTS signal changes state.
;       DSR     - Set when the DSR signal changes state.
;       RLSD    - Set when the RLSD signal changes state.
;       BREAK   - Set when a break is detected on input.
;       ERR     - Set when a line status error occurs.
;       RING    - Set when Ring Indicator is detected

EV_RxChar       equ     0001h           ;Any Character received
EV_RxFlag       equ     0002h           ;Received certain character
EV_TxEmpty      equ     0004h           ;Transmitt Queue Empty
EV_CTS          equ     0008h           ;CTS changed state
EV_DSR          equ     0010h           ;DSR changed state
EV_RLSD         equ     0020h           ;RLSD changed state
EV_Break        equ     0040h           ;BREAK received
EV_Err          equ     0080h           ;Line status error occurred
EV_Ring         equ     0100h           ;Ring signal detected
EV_PErr         equ     0200h           ;Printer error occured
EV_CTSS         equ     0400h           ;CTS state  [rkh]...
EV_DSRS         equ     0800h           ;DSR state
EV_RLSDS	equ	1000h		;RLSD state
EV_RingTe       equ     2000h           ;Ring Trailing Edge Indicator
;               equ     4000h
;               equ     8000h


;=========================================================================
;
;       qdb
;       Queue definition block. Passed to setqueue, defines the location and
;       size of the transmit and receive circular queue's used for interrupt
;       transmit and recieve processing.
;
;=========================================================================

QDB struc
  QueueRxAddr   dd      ?               ;Pointer to RX Queue, Offset
  QueueRxSize   dw      ?               ;Size of RX Queue in bytes
  QueueTxAddr   dd      ?               ;Pointer to TX Queue, Offset
  QueueTxSize   dw      ?               ;Size of TX Queue in bytes
QDB ends


pout MACRO port,val
     out   port,val
     ENDM

pin  MACRO port,val
     in    port,val
     ENDM

.cref

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

; Include file originally taken from Windows 3.00 COMM driver
; Equates and structures must match

MAXCOM   equ   3        ;Two   COMs supported (COM1,2,3,4) [rkh]

RS232B   equ   0h       ;RS232 Card(s) I/O addr 40:Save area.

IRQ3	 equ   0bh	; Int vector for Com card @ 2xxh
IRQ4	 equ   0ch	; Int vector for Com card @ 3xxh

INTA0    equ   20h      ;X'20' 8259 Interrupt Control Port
INTA1    equ   21h      ;X'21' 8259 Interrupt Mask Port
EOI      equ   20h      ;X'20' 8259 End-of-Interrupt ack

Open     equ   0201h    ;Int 2F open request
Close    equ   0202h    ;Int 2F close request
Lock2F   equ   0203h    ;Int 2F lock request
Unlock2F equ   0204h    ;Int 2F unlock request

; COMDEB - Communications Device Equipment Block.
;
; This is essentially a superset of the DCB used outside of this
; module. The DCB is contained within the DEB as the first fields.
; The fields which follow are data and status fields which
; are unique to this implementation.

ComDEB struc                           ;RS232 Data Equip Block

  ComDCB        db      ((DCBSize+1) and 0FFFEh) DUP (0)

  ComErr        dw      0               ;Non-zero if I/O error
  Port          dw      0               ;Base I/O Address
  BIOSPortLoc   dw      0               ;Offset to port location (i.e. 40:0)

; The following fields are specific to com ports only

  XOffPoint     dw      0               ;Q count where XOff is sent
  EvtWord       dw      0               ;Event flags
  EvtMask       dw      0               ;Mask of events to check for
  QInAddr       dd      0               ;Address of the queue
  QInSize       dw      0               ;Length of queue in bytes
  QOutAddr      dd      0               ;Address of the queue
  QOutSize      dw      0               ;Length of queue in bytes
  QInCount      dw      0               ;Number of bytes currently in queue
  QInGet        dw      0               ;Offset into queue to get bytes from
  QInPut        dw      0               ;Offset into queue to put bytes in
  QOutCount     dw      0               ;Number of bytes currently in queue
  QOutGet       dw      0               ;Offset into queue to get bytes from
  QOutPut       dw      0               ;Offset into queue to put bytes in
  EFlags        db      0               ;Extended flags
  ErrorMask     db      0               ;Default error-checking mask
  RxMask        db      0               ;Character mask
  ImmedChar     db      0               ;Char to be transmitted immediately
  HSFlag        db      0               ;Handshake flag
  HHSLines      db      0               ;8250 DTR/RTS bits for handshaking
  OutHHSLines   db      0               ;Lines that must be high to output
  MSRShadow     db      0               ;Modem Status Register Shadow
  MSRMask       db      0               ;Mask of Modem Lines to check
  MSRInfinite   db      0               ;Mask of MSR lines that must be high
  Mask8259      db      0               ;8259 Interrupt mask bit
  IntVecNum     db      0               ;Interrupt vector number
; [rkh]  OldIntVec     dd      0               ;Old interrupt vector
  ForcedInt	db	0		; flag to indicate Tx int being forced
  LSRShadow	db	0		;Line Status Register shadow
  QOutMod       dw      0               ; characters sent mod xOnLim ENQ/ETX [rkh]

    ; [gps] real mode queue addresses
  QInSegment	dd	0		;Segment:offset of real mode input queue
  QOutSegment	dd	0		;Segment:offset of real mode output q

ComDEB ends

      .errnz   (SIZE ComDEB) and 1


; The LptDEB is identical to the ComDEB structure, except
; all the COM port specific stuff has been removed (which
; convientiently was stored at the end so offsets would
; be correct).  This allows the code to act indifferently
; when accessing the strucutres for things like the port.

LptDEB struc
  xComDCB       db      ((DCBSize+1) AND 0FFFEh) dup (0)
  xComErr       dw      0               ;Non-zero if I/O error
  xPort         dw      0               ;Base I/O Address
  xBIOSPortLoc  dw      0               ;Offset to port location (i.e. 40:0)
LptDEB ends

		.errnz	 xComDCB-ComDCB
		.errnz	 xComErr-ComErr
		.errnz	 xPort-Port
		.errnz	 xBIOSPortLoc-BIOSPortLoc


; Values for the handshake flag
;
; BreakSet        - True if break was set - stops transmission
; XOffSent        - True if we have sent the XOff character
; XOffPending     - True if XOff character needs to be sent
; XOnPending      - True if XOn  character needs to be sent
; HHSDown         - True if host dropped required hardware lines
; HHSDropped      - True if we dropped our hardware handshake lines
; XOffReceived    - True if XOff received from host
; HSPending       - Mask to return non-zero if XOn or Xoff must be sent
; HSReceived      - Mask to return non-zero if handshake has been
;                   received from host stopping transmission
; CannotXmit      - Mask to return non-zero if any condition
;                   exists which prevents us from tranmitting.
; HSSent          - Mask to return non-zero if we sent a handshake
; Discard         - True if to discard data (set by $TRMCOM)


XOffPending     equ     00000001b       ;XOff needs to be sent
EnqPending      equ     00000001b       ;Enq needs to be sent [rkh]
EtxPending      equ     00000001b       ;Etx needs to be sent

HHSDropped      equ     00000010b       ;Our hardware handshake lines are down

XOnPending      equ     00000100b       ;XOn needs to be sent
AckPending      equ     00000100b       ;Ack needs to be sent (ENQ/ACK & ETX/ACK)

XOffReceived    equ     00001000b       ;XOff character received
EnqSent         equ     00001000b       ;Enq has been sent
EtxSent         equ     00001000b       ;Etx has been sent

XOffSent        equ     00010000b       ;XOff has been sent
EnqReceived     equ     00010000b       ;Enq character received (ENQ/ACK)
EtxReceived     equ     00010000b       ;Etx character received (ETX/ACK)

HHSDown         equ     00100000b       ;Host hardware handshake lines are down

BreakSet        equ     01000000b       ;Break has been set

Discard         equ     10000000b       ;Discard recevied data


HSPending       equ     XOffPending+XOnPending
HSReceived      equ     XOffReceived+HHSDown
HSSent          equ     XOffSent+HHSDropped
CannotXmit      equ     HSSent+HSPending+HSReceived+BreakSet

pause   macro                ;;macro to insure that an instruction
        jmp     $+2          ;;  fetch occurs between IN and/or OUT
        endm                 ;;  instructions on the PC-AT machine


TimeoutError   equ   -2      ;Timeout error code for $TRMCOM
Timeout        equ   30      ;30 second timeout

;       Status bits returned from the BIOS for LPT ports

PS_NotBusy      equ     10000000b       ;Printer not busy
PS_Ack          equ     01000000b       ;Data acknowledged
PS_PaperOut     equ     00100000b       ;Out of paper
PS_Select       equ     00010000b       ;Device is selected
PS_IOError      equ     00001000b       ;IO error
PS_Timeout      equ     00000001b       ;Timeout occured

;	WinFlags constants...remove when included in windows.inc

WF_PMODE	equ	01h
WF_WIN286	equ	10h

;	Interrupt 31h service call equates

include int31.inc

Get_RM_IntVector    equ     (Int31_Int_Serv SHL 8 ) OR Int_Get_Real_Vec
Set_RM_IntVector    equ     (Int31_Int_Serv SHL 8 ) OR Int_Set_Real_Vec

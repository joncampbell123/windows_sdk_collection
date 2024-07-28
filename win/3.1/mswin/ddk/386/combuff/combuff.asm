PAGE 58,132
;******************************************************************************
TITLE combuff.asm -
;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp.  All Rights Reserved, 1989, 1990
;
;   Title:	combuff.asm -
;
;   Version:	1.00
;
;   Date:	30-Jan-1990
;
;   Author:	RAL
;
;------------------------------------------------------------------------------
;
;   Change log:
;
;      DATE	REV		    DESCRIPTION
;   ----------- --- -----------------------------------------------------------
;   30-Jan-1990 RAL Original
;   31-Jan-1990 RAL Added support for XON/XOFF protocol
;   13-Feb-1990 RAP Check for version 3.00 of VCD in Sys_Critical_Init
;
;==============================================================================
;
;   DESCRIPTION:
;	This device is responsible for buffering COM I/O.  It is closely
;	tied to the Virtual COM Device.  See the VCD documentation for
;	details.
;
;******************************************************************************

	.386p


;******************************************************************************
;			      I N C L U D E S
;******************************************************************************

	.XLIST
	INCLUDE VMM.Inc
	INCLUDE Debug.Inc
	INCLUDE VPICD.inc
	INCLUDE VCD.Inc
	.LIST


;******************************************************************************
;		 V I R T U A L	 D E V I C E   D E C L A R A T I O N
;******************************************************************************

Declare_Virtual_Device COMBUFF, 1, 0, COMBuff_Control


;******************************************************************************
;				 E Q U A T E S
;******************************************************************************

CB_No_Proto	EQU	0
CB_XOFF_Proto	EQU	1

XOFF_Char	EQU	13h

Default_Buf_Size EQU	128


;******************************************************************************
;			     S T R U C T U R E S
;******************************************************************************

COMBuff_Extended_COM_Struc STRUC
Filler		db	SIZE VCD_COM_Struc dup (?)

VCD_Vier	db	?	; Virtual Interrupt Enable register
VCD_Vlcr	db	?	; Virtual Line Control register
VCD_Vmcr	db	?	; Virtual Modem Control register
VCD_Viir	db	?	; Virtual Interrupt Identity register
VCD_Vlsr	db	?	; Virtual Line status register
VCD_Vrxb	db	?	; Virtual Rx Buffer
VCD_Vmsr	db	0	; Virtual Modem status register
VCD_Virr	db	0	; Virtual Interrupt Request register

VCD_XOFF_Flag	db	0
VCD_Protocol	db	?

IFDEF DEBUG
VCD_Max_Queued	dw	?	; max # of chars ever queued
ENDIF

VCD_RxQin	dw	0	; First In...
VCD_RxQout	dw	0	; ...First Out
VCD_RxQcount	dw	0	; How many in queue
VCD_RxQlen	dw	?	; Total size of queue in
VCD_RxQ 	dw	?	; QUEUE STARTS HERE -- Continues past here
COMBuff_Extended_COM_Struc ENDS

.errnz SIZE COMBuff_Extended_COM_Struc - VCD_RxQ - 2


;
;   Compute extra size of data area EXCLUDING the word used for the first
;   queue entry (total queue size will be added on later).
;
COMBuff_Extra_Data_Area = SIZE COMBuff_Extended_COM_Struc-SIZE VCD_COM_Struc-2


;******************************************************************************
;			   F L A G   E Q U A T E S
;******************************************************************************


;******************************************************************************
;		   I N I T I A L I Z A T I O N	 D A T A
;******************************************************************************

VxD_IDATA_SEG

EXTRN COMBuff_Size_Ini_String:BYTE
EXTRN COMBuff_Size_Ini_Number:BYTE
EXTRN COMBuff_Prot_Ini_String:BYTE
EXTRN COMBuff_Prot_Ini_Number:BYTE
EXTRN COMBuff_XOFF_String:BYTE

VxD_IDATA_ENDS


;******************************************************************************
;			    L O C A L	D A T A
;******************************************************************************

VxD_DATA_SEG

COMBuff_ProcList LABEL DWORD
	dd	OFFSET32 COMBuff_VCD_Control
	dd	OFFSET32 COMBuff_Int
	dd	0
	dd	OFFSET32 COMBuff_EOI
	dd	OFFSET32 COMBuff_Mask
	dd	OFFSET32 COMBuff_Test_Int_Req
	dd	OFFSET32 COMBuff_In_RxTxB
	dd	OFFSET32 COMBuff_Out_RxTxB
	dd	OFFSET32 COMBuff_In_IER
	dd	OFFSET32 COMBuff_Out_IER
	dd	OFFSET32 COMBuff_In_IIR
	dd	OFFSET32 COMBuff_Out_IIR
	dd	OFFSET32 COMBuff_In_LCR
	dd	OFFSET32 COMBuff_Out_LCR
	dd	OFFSET32 COMBuff_In_MCR
	dd	OFFSET32 COMBuff_Out_MCR
	dd	OFFSET32 COMBuff_In_LSR
	dd	OFFSET32 COMBuff_Out_LSR
	dd	OFFSET32 COMBuff_In_MSR
	dd	OFFSET32 COMBuff_Out_MSR

.ERRNZ $-COMBuff_ProcList-SIZE VCD_ProcList_Struc


IFDEF DEBUG
COM1_Handle	dd  0
COM2_Handle	dd  0
COM3_Handle	dd  0
COM4_Handle	dd  0
ENDIF

VxD_DATA_ENDS


;******************************************************************************
;	       D E V I C E   C O N T R O L   P R O C E D U R E
;******************************************************************************

VxD_CODE_SEG

;******************************************************************************
;
;   COMBuff_Control
;
;   DESCRIPTION:
;	This is the device control procedure for the COM Buffer device.
;
;   ENTRY:
;	EAX = Control call ID
;
;   EXIT:
;	If carry clear then
;	    Successful
;	else
;	    Control call failed
;
;   USES:
;	EAX, EBX, ECX, EDX, ESI, EDI, Flags
;
;==============================================================================

BeginProc COMBuff_Control

	Control_Dispatch Sys_Critical_Init, COMBuff_Sys_Crit_Init

IFDEF DEBUG
	Control_Dispatch Debug_Query, COMBuff_Debug_Query
ENDIF

	clc					; Ignore other control calls
	ret

EndProc COMBuff_Control

VxD_CODE_ENDS


;******************************************************************************
;		    I N I T I A L I Z A T I O N   C O D E
;******************************************************************************

VxD_ICODE_SEG

;******************************************************************************
;
;   COMBuff_Sys_Crit_Init
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc COMBuff_Sys_Crit_Init

	VxDCall VCD_Get_Version
	cmp	eax, 300h
	jb	fail_init
	xor	ecx, ecx

CB_SCI_Loop:
	inc	ecx
	push	ecx

	lea	eax, [ecx+"0"]
	mov	[COMBuff_Size_Ini_Number], al
	mov	[COMBuff_Prot_Ini_Number], al
	mov	eax, Default_Buf_Size
	xor	esi, esi			; Null default program name
	mov	edi, OFFSET32 COMBuff_Size_Ini_String
	VMMcall Get_Profile_Decimal_Int
	test	eax, eax
	jz	DEBFAR CB_SCI_No_Buffer

	cmp	eax, 10000
	jb	SHORT CB_SCI_Reasonable_Size
	Debug_Out "Silly size of #EAX for COM buffer ignored -- Using a 10K buffer"
	mov	eax, 10000
CB_SCI_Reasonable_Size:

	push	eax
	push	ecx
	xchg	eax, ecx			; EAX = Port #, ECX=Queue len
	mov	ebx, COM_IRQ_Sharable
	add	ecx, ecx			; 2 bytes per queue entry
	add	ecx, COMBuff_Extra_Data_Area	; Add in size of data struc
	xor	edx, edx
	mov	esi, OFFSET32 COMBuff_ProcList
	VxDcall VCD_Virtualize_Port
	mov	ebx, eax
	pop	ecx
	pop	eax
	jc	SHORT CB_SCI_No_Buffer

IFDEF DEBUG
	; save COM handle
	push	esi
	mov	esi, ecx
	dec	esi
	mov	[esi*4][COM1_Handle], ebx
	pop	esi
ENDIF
	mov	[ebx.VCD_RxQlen], ax		; Remember queue size

	xor	edx, edx
	xor	esi, esi			; Null default program name
	mov	edi, OFFSET32 COMBuff_Prot_Ini_String
	VMMcall Get_Profile_String

	mov	[ebx.VCD_Protocol], CB_No_Proto

	test	edx, edx
	jz	SHORT CB_SCI_Next_Port

	mov	esi, OFFSET32 COMBuff_XOFF_String
	mov	edi, edx
	call	Compare_Strings
	jc	SHORT CB_SCI_Next_Port

	mov	[ebx.VCD_Protocol], CB_XOFF_Proto


CB_SCI_Next_Port:
CB_SCI_No_Buffer:
	pop	ecx
	cmp	ecx, 4
	jb	CB_SCI_Loop

	clc
fail_init:
	ret

EndProc COMBuff_Sys_Crit_Init



;******************************************************************************
;
;   Compare_Strings
;
;   DESCRIPTION:
;	This procedure compares two strings.  If all of the characters up
;	to the NULL terminator of the first string match the characters
;	of the second string the procedure returns with carry clear,
;	otherwise carry is set.  On exit ECX contains the OFFSET of the
;	character that did not match.  Othewise it contains the length of
;	the first string (NOT including the NULL terminator).
;
;   ENTRY:
;	ESI = pointer to ASCIIZ string
;	EDI = pointer to String (does not have to be null terminated)
;
;   EXIT:
;	Carry clear if strings matched.
;
;   USES:
;	Flags
;
;==============================================================================

BeginProc Compare_Strings

	pushad

	xor	ecx, ecx
	cld
CS_Loop:
	lodsb
	test	al, al
	jz	SHORT CS_Match
	call	Force_Upper_Case
	mov	bl, al
	mov	al, BYTE PTR [edi]
	inc	edi
	call	Force_Upper_Case
	cmp	al, bl
	jne	SHORT CS_Failed
	inc	ecx
	jmp	CS_Loop

CS_Failed:
	stc
	jmp	SHORT CS_Exit
CS_Match:
	clc
CS_Exit:
	popad
	ret

EndProc Compare_Strings


;******************************************************************************
;
;   Force_Upper_Case
;
;   DESCRIPTION:
;	This routine takes an ASCII character in AL and, if it is a lower
;	case letter, converts it to upper case.
;
;   ENTRY:
;	AL = ASCII character
;
;   EXIT:
;	AL = upper case letter or other ASCII character(unchanged)
;
;   USES:
;
;==============================================================================

BeginProc Force_Upper_Case

	cmp	al, "a"
	jb	SHORT FUC_Done
	cmp	al, "z"
	ja	SHORT FUC_Done
	sub	al, "a"-"A"
FUC_Done:
	ret

EndProc Force_Upper_Case


VxD_ICODE_ENDS


;------------------------------------------------------------------------------

VxD_CODE_SEG

;******************************************************************************
;      P R O C	 F O R	 C O N T R O L	 C A L L S   F R O M   V C D
;******************************************************************************

;******************************************************************************
;
;   COMBuff_VCD_Control
;
;   DESCRIPTION:
;
;   ENTRY:
;	EAX = message
;	EBX = new VM, or 0
;	EDX = old VM, or 0
;	ESI -> COM Struc
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc COMBuff_VCD_Control

	cmp	eax, VCD_Control_Set_Owner
	jne	SHORT COMBuff_VC_Ignore

	xchg	ebx, edx
	or	ebx, ebx			;Q: non-zero old VM handle
	jz	SHORT CB_no_int_req		;   N:
	mov	eax, [esi.VCD_IRQ_Handle]	;   Y: check about clearing
						;      any outstanding int request
	or	eax, eax
	jz	short CB_no_int_req
	VxDCall VPICD_Get_Complete_Status
	test	ecx, VPICD_Stat_Virt_Dev_Req	;Q: we requested int?
	jz	SHORT CB_no_int_req		;   N:
	VxDCall VPICD_Clear_Int_Request 	;   Y: clear int request

CB_no_int_req:
	mov	ebx, edx

	test	ebx, ebx			;Q: assigning a new VM
	jz	SHORT COMBuff_VC_Ignore 	;   N:

	call	COMBuff_Clear_Queue

	mov	eax, [esi.VCD_IRQ_Handle]
	or	eax, eax
	jz	short @F
	VxDCall VPICD_Get_Complete_Status
	test	ecx, VPICD_Stat_Virt_Mask	;Q: VM has IRQ masked?
	jz	short VC_not_masked		;   N:
@@:
	call	COMBuff_Disable_Trapping	;   Y: disable port trapping
VC_not_masked:

	lea	edi, [esi.VCD_Vier]
	mov	esi, [esi.VCD_CB_Offset]
	lea	esi, [esi][ebx.VCD_IER]
	cld
	movsw					; Copy IER and LCR
	movsb					; Copy MCR

COMBuff_VC_Ignore:
	clc
	ret

EndProc COMBuff_VCD_Control


;******************************************************************************
;		    I / O   T R A P   P R O C E D U R E S
;******************************************************************************


;******************************************************************************
;
;   COMBuff_In_RxTxB
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc COMBuff_In_RxTxB, High_Freq

	cli
	test	[esi.VCD_Vlcr], LCR_DLAB	; Q: Access is to Divisor latch?
	jnz	SHORT CB_In_Divisor_Latch	;    Y: Yes, go do it
;
;   Input from data buffer
;
	mov	al, [esi.VCD_Vrxb]		; Get current character
	and	[esi.VCD_Vlsr], NOT LSR_DR	; Preserve error bits, reset DR
	and	[esi.VCD_Virr], NOT IER_DR	; Clear DR virtual interrupt
	call	COMBuff_Extract_Queue		; Suck out new status/char

IFDEF DEBUG_XOFF
	push	eax
	and	al, 7Fh
	cmp	al, XOFF_char
	jne	short D01_not_xoff
	mov	al, [esi.VCD_Number]
	Trace_Out 'XOFF read on COM#al'
D01_not_xoff:
	pop	eax
ENDIF

	sti
	ret

;
;   Input from divisor latch LSB -- No virtualization
;
CB_In_Divisor_Latch:
	in	al, dx
	sti
	ret

EndProc COMBuff_In_RxTxB



;******************************************************************************
;
;   COMBuff_Out_RxTxB
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc COMBuff_Out_RxTxB, High_Freq

	cli
	test	[esi.VCD_Vlcr], LCR_DLAB	; Q: Access is to Divisor latch?
	jnz	SHORT CB_Out_Divisor_Latch	;    Y: Yes, go do it

	and	[esi.VCD_Virr], NOT IER_THRE	; Write to THR clears interrupt
	cmp	al, XOFF_Char			; Q: Is this a Ctrl-S
	je	SHORT CB_THR_Sent_XOFF		;    Y: Force delay
	cmp	[esi.VCD_XOFF_Flag], True	;    N: Q: Delaying now?
	je	SHORT CB_THR_Restart		;	   Y: Stop delay now
						;	   N: Just send char
CB_Out_Divisor_Latch:
	out	dx, al				; Do ouput
	sti
	ret

;
;   Some other character was
;
CB_THR_Restart:
	mov	[esi.VCD_XOFF_Flag], False
	push	eax
	push	edx
	call	COMBuff_Extract_Queue
	call	COMBuff_Test_Int_Req
	pop	edx
	pop	eax
	out	dx, al
	sti
	ret


CB_THR_Sent_XOFF:
	cmp	[esi.VCD_Protocol], CB_XOFF_Proto
	jne	SHORT CB_THR_Output_Byte
	mov	[esi.VCD_XOFF_Flag], True
CB_THR_Output_Byte:
	out	dx, al
	sti
	ret

EndProc COMBuff_Out_RxTxB



;******************************************************************************
;
;   COMBuff_In_IER
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc COMBuff_In_IER, High_Freq

	cli
	test	[esi.VCD_Vlcr], LCR_DLAB	; Q: Access is to divisor latch
	jnz	CB_In_Divisor_Latch		;    Y: Do input from port

	mov	al, [esi.VCD_Vier]		; return virtual reg
	sti
	ret

EndProc COMBuff_In_IER

;******************************************************************************
;
;   COMBuff_Out_IER
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc COMBuff_Out_IER

	cli
	test	[esi.VCD_Vlcr], LCR_DLAB	; Q: Access is to divisor latch
	jnz	CB_Out_Divisor_Latch		;    Y: Do output to port

	xchg	al, [esi.VCD_Vier]		; What the VM thought it was
	out	dx, al				; Make sure HW sees change
	IO_Delay				; Give hardware lots of time
	mov	al, [esi.VCD_Vier]		; Recover what the VM wants
	or	al, IER_DR			; Make sure we get Rx ints!
	out	dx, al				; Set it...
	sti
	ret

EndProc COMBuff_Out_IER


;******************************************************************************
;
;   COMBuff_In_IIR
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================

VxD_DATA_SEG
;
; Table to translate enabled active interrupts to IIR value
;	( see VCD_Trap_IIR )
;
COMBuff_IIR_Table LABEL BYTE
		db	IIR_NONE	;    0	    0	   0	    0
		db	IIR_DR		;    0      0      0        1
		db	IIR_THRE	;    0      0      1        0
		db	IIR_DR		;    0      0      1        1
		db	IIR_LS		;    0      1      0        0
		db	IIR_LS		;    0      1      0        1
		db	IIR_LS		;    0      1      1        0
		db	IIR_LS		;    0      1      1        1
		db	IIR_MS		;    1      0      0        0
		db	IIR_DR		;    1      0      0        1
		db	IIR_THRE	;    1      0      1        0
		db	IIR_DR		;    1      0      1        1
		db	IIR_LS		;    1      1      0        0
		db	IIR_LS		;    1      1      0        1
		db	IIR_LS		;    1      1      1        0
		db	IIR_LS		;    1	    1	   1	    1

VxD_DATA_ENDS


BeginProc COMBuff_In_IIR

	cli
	movzx	eax, [esi.VCD_Virr]		; Interrupts requested
	and	al, [esi.VCD_Vier]		; Interrupts enabled
	and	al, 1111b			; Make sure only low nibble
	mov	al, COMBuff_IIR_Table[eax]	; Pick up value from table
	cmp	al, IIR_THRE			; Q: Strange one?
	je	SHORT CB_In_IIR_Reset_THRE	;    Y: Must reset THRE
	sti
	ret

CB_In_IIR_Reset_THRE:
	and	[esi.VCD_Virr], NOT IER_THRE	; not pending any more
	sti
	ret

EndProc COMBuff_In_IIR


;******************************************************************************
;
;   COMBuff_Out_IIR
;
;   DESCRIPTION:
;	This procedure should normally not be called!
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc COMBuff_Out_IIR

	mov	[esi.VCD_Viir], al
	ret

EndProc COMBuff_Out_IIR



;******************************************************************************
;
;   COMBuff_In_LCR
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc COMBuff_In_LCR

	cli
	mov	al, [esi.VCD_Vlcr]		; no point worrying device
	sti
	ret

EndProc COMBuff_In_LCR


;******************************************************************************
;
;   COMBuff_Out_LCR
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc COMBuff_Out_LCR

	cli
	mov	[esi.VCD_Vlcr], al		; Output: save it for DLAB
	sti
	out	dx, al				;	  and do it.
	ret

EndProc COMBuff_Out_LCR


;******************************************************************************
;
;   COMBuff_In_MCR
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc COMBuff_In_MCR

	cli
	mov	al, [esi.VCD_Vmcr]		; no point worrying device
	sti
	ret

EndProc COMBuff_In_MCR


;******************************************************************************
;
;   COMBuff_Out_MCR
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc COMBuff_Out_MCR

	cli
	xchg	al, [esi.VCD_Vmcr]		; Get what the VM thought it was
	out	dx, al				; This may disable interrupt!
	IO_Delay				; Give hardware lots of time
	mov	al, [esi.VCD_Vmcr]		; Get what the VM wants
	or	al, MCR_INTEN			; Assume we want to enable ints
	out	dx, al				;	  and do it.
	test	al, MCR_DTR			; Q: Is DTR is on?
	jz	COMBuff_Clear_Queue		;    N: Clear queue & return
	sti
	ret					;    Y: Done!

EndProc COMBuff_Out_MCR


;******************************************************************************
;
;   COMBuff_In_LSR
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc COMBuff_In_LSR, High_Freq

	cli
	in	al, dx				; Get real status
	test	al, (LSR_DR + LSR_ERRBITS)	; any Rx status?
	jz	short VCD_Trap_LSR_Got_Status	; 	no, nothing to do
	push	eax
	and	al, (LSR_DR + LSR_ERRBITS)	; junk transmit status
	call	COMBuff_Poll_Status		; Deal with Rx status
	pop	eax				; (ax got splattered!)
	and	al, LSR_TXBITS			; Real Tx status
VCD_Trap_LSR_Got_Status:
	mov	bl, [esi.VCD_Vlsr]		; get virtual Rx status
	or	al, bl				; plus real Tx status
	and	bl, LSR_DR			; preserve DR bit, clear error
	mov	[esi.VCD_Vlsr], bl		; bits when reading LSR
	and	[esi.VCD_Virr], NOT IER_LS	; clear interrupt request
	sti
	ret

EndProc COMBuff_In_LSR


;******************************************************************************
;
;   COMBuff_Out_LSR
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc COMBuff_Out_LSR

	cli
	and	al, (LSR_DR + LSR_ERRBITS)	; what the chip does!
	mov	[esi.VCD_Vlsr], al		; weird!
						; Emulate the H/W....
	and	[esi.VCD_Virr], NOT (IER_LS + IER_DR)
	test	al, LSR_DR
	jz	SHORT CB_LSR_NoDR
	or	[esi.VCD_Virr], IER_DR
CB_LSR_NoDr:
	test	al, LSR_ERRBITS
	jz	SHORT CB_LSR_Exit
	or	[esi.VCD_Virr], IER_LS
CB_LSR_Exit:
	sti
	ret

EndProc COMBuff_Out_LSR


;******************************************************************************
;
;   COMBuff_In_MSR
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc COMBuff_In_MSR

	cli
	in	al, dx				; get real status
	or	al, [esi.VCD_Vmsr]		; and any saved delta bits
	mov	[esi.VCD_Vmsr], 0
	and	[esi.VCD_Virr], NOT IER_MS	; clear modem interrupt request
	sti
	ret

EndProc COMBuff_In_MSR

;******************************************************************************
;
;   COMBuff_Out_MSR
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc COMBuff_Out_MSR

	cli
	out	dx, al				; do it!
	mov	[esi.VCD_Vmsr], 0		; No saved delta bits now!
	sti
	ret

EndProc COMBuff_Out_MSR


;******************************************************************************
;	  H A R D W A R E   I N T E R R U P T	P R O C E D U R E S
;******************************************************************************


;******************************************************************************
;
;   COMBuff_Int
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================


VxD_DATA_SEG

CB_Int_Jmp_Table LABEL DWORD
	dd	OFFSET32 CB_Modem_Int
	dd	OFFSET32 CB_THRE_Int
	dd	OFFSET32 CB_Rx_Int
	dd	OFFSET32 CB_Rx_Int

VxD_DATA_ENDS


BeginProc COMBuff_Int, High_Freq

	mov	edx, [esi.VCD_IObase]		; Get start of ports
	add	dx, UART_IIR
	in	al, dx				; get interrupt identity
	test	al, IIR_NONE			; Came from this device?
	jz	SHORT CB_Int_Jump		;     Yep, jump to handler
						;     Nope - Reflect it
	stc					;     to next guy in chain
	ret


CB_Int_Loop:
	mov	edx, [esi.VCD_IObase]		; Get start of ports
	add	dx, UART_IIR
	in	al, dx				; Get interrupt identity
	test	al, IIR_NONE			; Came from this device?
	jnz	SHORT CB_Int_Done		;	nope, just return!

CB_Int_Jump:

IFDEF DEBUG
	test	al, NOT 07h
	jz	short CB_IIR_Okay
	Debug_Out "COMBUF: IIR (#DX) has too many bits set: #AL"
CB_IIR_Okay:
ENDIF
	and	al, 07h
	movzx	eax, al
	jmp	CB_Int_Jmp_Table[eax*2] 	; Go deal with it

CB_Modem_Int:
	add	dx, (UART_MSR - UART_IIR)	; pick up current modem status
	in	al, dx
	and	al, MSR_DELTA			; only want Delta bits
	mov	[esi.VCD_Vmsr], al		; save in virtual MSR
	or	[esi.VCD_Virr], IER_MS		; set modem interrupt request
	jmp	CB_Int_Loop

CB_THRE_Int:
	or	[esi.VCD_Virr], IER_THRE	; set THRE interrupt request
	jmp	CB_Int_Loop

CB_Rx_Int:
	add	dx, (UART_LSR - UART_IIR)	; yes, get character/status
	in	al, dx
	and	al, (LSR_DR + LSR_ERRBITS)	; junk transmit status
	jz	CB_Int_Loop			; Hmmm!
	call	COMBuff_Poll_Status
	jmp	CB_Int_Loop

CB_Int_Done:
	mov	eax, [esi.VCD_IRQ_Handle]
	VxDcall VPICD_Phys_EOI
	call	COMBuff_Extract_Queue
	call	COMBuff_Test_Int_Req		; Will "return" for us
	clc
	ret

EndProc COMBuff_Int


;******************************************************************************
;
;   COMBuff_EOI
;
;   DESCRIPTION:
;	Here we mindlessly clear the interrupt request for the COM port
;	since it dosen't matter if the buffer was serviced yet or not.
;	If it was not, then the IRET procedure will set the int request
;	again and everyon will still be happy.
;
;   ENTRY:
;	EAX = IRQ handle
;	EBX = Current VM handle
;	ESI -> COM structure
;
;   EXIT:
;
;   USES:
;
;==============================================================================


BeginProc COMBuff_EOI, High_Freq

	VxDjmp	VPICD_Clear_Int_Request

EndProc COMBuff_EOI


;******************************************************************************
;
;   COMBuff_Mask
;
;   DESCRIPTION:    Routine called when VM is changing a COM port's IRQ mask
;
;   ENTRY:	    EAX = IRQ handle
;		    EBX = VM handle
;		    ESI -> COM data structure
;		    ECX = 0, if unmasking
;
;   EXIT:	    none
;
;   USES:	    flags
;
;==============================================================================
BeginProc COMBuff_Mask

	jecxz	short COMBuff_Enable_Trapping
	VxDCall VPICD_Get_Complete_Status
	test	ecx, VPICD_Stat_In_Service	;Q: IRQ in service?
	jz	short COMBuff_Disable_Trapping	;   N: disable port trapping
	ret					;   Y: don't disable trapping

EndProc COMBuff_Mask


;******************************************************************************
;
;   COMBuff_Disable_Trapping
;
;   DESCRIPTION:    Disable local trapping of a COM port's I/O ports
;
;   ENTRY:	    ESI -> COM data structure
;
;   EXIT:	    none
;
;   USES:	    flags
;
;==============================================================================
BeginProc COMBuff_Disable_Trapping

	pushad
IFDEF DEBUG_verbose
	mov	al, [esi.VCD_Number]
	Trace_Out 'COMBUFF - disabling trapping for COM#al'
ENDIF
	mov	edx, [esi.VCD_IObase]
	mov	ecx, UART_PORTS
@@:
	VMMcall Disable_Local_Trapping
	inc	edx
	loopd	@B
	popad
	ret

EndProc COMBuff_Disable_Trapping


;******************************************************************************
;
;   COMBuff_Enable_Trapping
;
;   DESCRIPTION:    Enable local trapping of a COM port's I/O ports
;
;   ENTRY:	    ESI -> COM data structure
;
;   EXIT:	    none
;
;   USES:	    flags
;
;==============================================================================
BeginProc COMBuff_Enable_Trapping

	pushad
IFDEF DEBUG_verbose
	mov	al, [esi.VCD_Number]
	Trace_Out 'COMBUFF - enabling trapping for COM#al'
ENDIF
;
; First read current state of IER, LCR & MCR
;
	mov	edx, [esi.VCD_IObase]
	inc	edx
.errnz UART_IER-1
	in	al, dx
	mov	[esi.VCD_Vier], al
	add	dl, UART_LCR-UART_IER
	IO_Delay
	in	al, dx
	mov	[esi.VCD_Vlcr], al
	inc	edx
.errnz UART_MCR-UART_LCR-1
	IO_Delay
	in	al, dx
	mov	[esi.VCD_Vmcr], al
	sub	dl, UART_MCR

	mov	ecx, UART_PORTS
@@:
	VMMcall Enable_Local_Trapping
	inc	edx
	loopd	@B
	popad
	ret

EndProc COMBuff_Enable_Trapping


;******************************************************************************
;
;   COMBuff_Test_Int_Req
;
;   DESCRIPTION:
;
;   ENTRY:
;	ESI -> COM structure
;
;   EXIT:
;
;   USES:
;	Flags
;
;==============================================================================

BeginProc COMBuff_Test_Int_Req

	push	eax
	push	ebx
	push	ecx
	mov	ebx, [esi.VCD_Owner]		; EBX = Handle of port owner
	Assert_VM_Handle ebx			; Paranoia...
	mov	cl, [esi.VCD_Virr]		; Interrupts pending
	and	cl, [esi.VCD_Vier]		; and enabled?
	jz	SHORT CB_IR_Exit		;    N: Clear int request
	test	[esi.VCD_Vlcr], LCR_DLAB	; Q: Access is to divisor latch
	jnz	short CB_IR_Exit		;	yes, can't send it!
	test	[esi.VCD_Vmcr], MCR_INTEN	; and enabled?
	jz	short CB_IR_Exit		;	no, can't send it!
	mov	eax, [esi.VCD_IRQ_Handle]	; EAX = IRQ handle for VPICD
	or	eax, eax
	jz	short CB_IR_Exit
	VxDcall VPICD_Get_Status
	test	ecx, VPICD_Stat_IRET_Pending	; Q: Waiting for IRET?
	jnz	SHORT CB_IR_Exit		;    Y: Wait a little longer
	VxDcall VPICD_Set_Int_Request
CB_IR_Exit:
	pop	ecx
	pop	ebx
	pop	eax
	ret

EndProc COMBuff_Test_Int_Req



;******************************************************************************
;
;   COMBuff_Poll_Status
;
;   DESCRIPTION:
;      Read the Line Status Register.  Put Transmit status in
;      vitual register, receive status in queue or virtual
;      register as appropriate.
;
;   ENTRY:
;      AL  = LSR Rx bits
;      DX  = LSR I/O port
;      ESI = pointer to VCD_COM structure
;
;   EXIT:
;
;   USES:
;	EAX, EBX, EDX
;
;==============================================================================

BeginProc COMBuff_Poll_Status

	mov	ah, al				; Save status in ah
	test	ah, LSR_DR			; data ready?
	jz	SHORT CB_NoRxChar		;	no, don't bother reading
	add	edx, (UART_RBR - UART_LSR)	; DX => Receive buffer
	in	al, dx				; get character in al
;;;;;;;;;;;;;; DEBUGGING CODE WILL INSERT @ IF HARDWARE OVERFLOW ;;;;;;;;;;
IFDEF DEBUG
	test	ah, LSR_OE
	jz	SHORT OkieDokie
	mov	al, '@'
	and	ah, NOT LSR_OE
OkieDokie:
ENDIF
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

IFDEF DEBUG_XOFF
	push	eax
	and	al, 7Fh
	cmp	al, XOFF_Char
	jne	short D00_not_xoff
	mov	al, [esi.VCD_Number]
	Trace_Out 'Received XOFF on COM#al'
D00_not_xoff:
	pop	eax
ENDIF

CB_NoRxChar:
	mov	bl, [esi.VCD_Vlsr]		; Set virtual regs
	test	bl, LSR_DR			; Should we queue it?
	jnz	SHORT COMBuff_Insert_Queue	;	yes, put in queue
	cmp	[esi.VCD_XOFF_Flag], True	; XOFF sent?
	je	SHORT COMBuff_Insert_Queue	;	yes, put in queue

	or	bl, ah
	mov	[esi.VCD_Vlsr], bl
	test	ah, LSR_DR			; Data to put there?
	jz	SHORT CB_Poll_Status_Ret	;	no, don't bother
	mov	[esi.VCD_Vrxb], al		; (should usually have DR on)
	or	[esi.VCD_Virr], IER_DR		; Set DR interrupt request
	test	bl, LSR_ERRBITS			; Error bits on?
	jnz	SHORT CB_Poll_Set_Int_Status	;	yes, go set interrupt
CB_Poll_Status_Ret:
	ret

CB_Poll_Set_Int_Status:
	or	[esi.VCD_Virr], IER_LS		; Set LS interrupt request
	ret

EndProc COMBuff_Poll_Status


;******************************************************************************
;		       Q U E U E    P R O C E D U R E S
;******************************************************************************


;******************************************************************************
;
;   COMBuff_Insert_Queue
;
;   DESCRIPTION:
;
;   ENTRY:
;	AH = Status byte, AL = Data byte
;
;   EXIT:
;
;   USES:
;	EBX, EAX
;
;==============================================================================

BeginProc COMBuff_Insert_Queue

	mov	bx, [esi.VCD_RxQlen]
	cmp	[esi.VCD_RxQcount], bx		; queue full?
	je	SHORT CB_IQ_Full
	movzx	ebx, [esi.VCD_RxQin]
	mov	[esi.VCD_RxQ+ebx*2], ax		; into the queue
	inc	[esi.VCD_RxQcount]
	inc	ebx
	cmp	bx, [esi.VCD_RxQlen]
	jb	SHORT CB_IQ_Set_New_Len
	xor	ebx, ebx
CB_IQ_Set_New_Len:
	mov	[esi.VCD_RxQin], bx		; put index back
IFDEF DEBUG
	mov	bx, [esi.VCD_RxQcount]
	cmp	bx, [esi.VCD_Max_Queued]
	jbe	short CB_not_max
	mov	[esi.VCD_Max_Queued], bx
CB_not_max:
ENDIF
	ret

CB_IQ_Full:
	movzx	ebx, [esi.VCD_RxQin]
	or	ah, LSR_OE			; simulate overrun error
	mov	[esi.VCD_RxQ+ebx*2], ax		; into the queue
	ret

EndProc COMBuff_Insert_Queue



;******************************************************************************
;
;   COMBuff_Extract_Queue
;
;   DESCRIPTION:
;
;   ENTRY:
;	ESI -> COM data structure
;
;   EXIT:
;
;   USES:   EBX, EDX
;
;==============================================================================


BeginProc COMBuff_Extract_Queue, High_Freq

	cmp	[esi.VCD_RxQcount], 0		; Anything in queue?
	je	SHORT CB_EQ_Exit		;	no, nothing to extract
	cmp	[esi.VCD_XOFF_Flag], True	; Q: XOFF?
	je	SHORT CB_EQ_Exit		;    Y: Don't fill buffer
CB_EQ_Loop:
	test	[esi.VCD_Vlsr], LSR_DR
	jnz	SHORT CB_EQ_Exit
	dec	[esi.VCD_RxQcount]
	movzx	ebx, [esi.VCD_RxQout]
	mov	dx, [esi.VCD_RxQ+ebx*2]		; get character from the queue
	inc	ebx
	cmp	bx, [esi.VCD_RxQlen]
	jb	SHORT CB_EQ_Set_Out_Ptr
	xor	ebx, ebx
CB_EQ_Set_Out_Ptr:
	mov	[esi.VCD_RxQout], bx		; put index back
	or	dh, [esi.VCD_Vlsr]	 	; add in old status
	mov	[esi.VCD_Vlsr], dh		; and set virtual LSR
	test	dh, LSR_DR			; Was there data too?
	jz	short CB_EQ_NoData
	mov	[esi.VCD_Vrxb], dl		;	yes, put in virtual reg
	or	[esi.VCD_Virr], IER_DR		; and set interrupt request
	test	dh, LSR_ERRBITS			; Line status interrupt?
	jz	SHORT CB_EQ_Exit	       ;       no, just return
	or	[esi.VCD_Virr], IER_LS		;	yes, set interrupt request
	jmp	SHORT CB_EQ_Exit
CB_EQ_NoData:
	cmp	[esi.VCD_RxQcount], 0		; queue empty?
	jne	short CB_EQ_Loop	       ;       no, continue until queue
						;	empty, or LSR_DR is set
	test	dh, LSR_ERRBITS			; Line status interrupt?
	jz	SHORT CB_EQ_Exit		     ;	     no, just return
	or	[esi.VCD_Virr], IER_LS		;	yes, set interrupt request
CB_EQ_Exit:
	ret


EndProc COMBuff_Extract_Queue


;******************************************************************************
;
;   COMBuff_Clear_Queue
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc COMBuff_Clear_Queue

	mov	[esi.VCD_RxQcount], 0		; Clear receive Q
	mov	[esi.VCD_RxQin], 0
	mov	[esi.VCD_RxQout], 0
	and	[esi.VCD_Vlsr], NOT LSR_DR	; preserve error bits, reset DR
	and	[esi.VCD_Virr], NOT IER_DR	; clear DR virtual interrupt
	mov	[esi.VCD_XOFF_Flag], False
	ret

EndProc COMBuff_Clear_Queue




;******************************************************************************
;			  D E B U G G I N G   C O D E
;******************************************************************************

IFDEF DEBUG

;******************************************************************************
;
;   COMBuff_Debug_Query
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc COMBuff_Debug_Query

	pushad
	mov	ecx, 4
	mov	al, 1
	mov	esi, OFFSET32 COM1_Handle
dump_port_data:
	mov	ebx, [esi]
	or	ebx, ebx
	jz	short debug_no_com_port

	movzx	edx, [ebx.VCD_Max_Queued]
	movzx	edi, [ebx.VCD_RxQlen]
	Trace_Out "COM#al: buffer size=#di, max queued=#dx"
	jmp	short debug_next_port

debug_no_com_port:
	Trace_Out "COM#al: no buffer assigned"

debug_next_port:
	inc	al
	add	esi, 4
	loop	dump_port_data

	popad
	ret

EndProc COMBuff_Debug_Query


ENDIF

VxD_CODE_ENDS

	END

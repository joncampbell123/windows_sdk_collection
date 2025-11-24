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

          PAGE 58,132
;*******************************************************************************
TITLE SERIAL.ASM Windows/386 Int 33h Mouse Driver - SERIAL Mouse Specific Code
;*******************************************************************************
;
;  Title:      SERIAL.ASM
;
;  Version:    1.0
;
;*******************************************************************************

          .386p

;*******************************************************************************
;                                I N C L U D E S
;*******************************************************************************

SERMOU_Dynamic	equ	1

          .xlist

          INCLUDE VMM.INC
          INCLUDE DEBUG.INC
          INCLUDE OPTTEST.INC
          INCLUDE VPICD.INC
          INCLUDE VMD.INC
	  INCLUDE VCD.INC
	  INCLUDE INS8250.INC
	  INCLUDE CONFIGMG.INC
          .list



Declare_Virtual_Device SERMOU,1,0,SERMOU_Control,Undefined_Device_Id, \
		Undefined_Init_Order,,,

VxD_LOCKED_DATA_SEG

COM_INFO STRUC

COM_Port	dw	?
COM_PacketSize	db	?
COM_Packet	dd	?
COM_Index	db	?

COM_INFO ENDS

Real_Manipulator	dd	0
toggle	db	0

VxD_LOCKED_DATA_ENDS

VxD_Pageable_DATA_Seg

Serial_IRQ_Desc VPICD_IRQ_Descriptor <0>

PortInfo STRUC

PortBase	dw	?
PortIrq		dw	?
PortHandler	dd	?

PortInfo ENDS

SerPort		PortInfo	<0,0,OFFSET32 Serial_Hw_Int_Proc>

PacketSize	db	0

prd_Temp	dd	?
cominfo		db	'COMINFO',0

BallPoint_ID	dw	0

CurrentMouseInstance	dd	0

VxD_Pageable_Data_Ends

VxD_Locked_Code_Seg

;******
;
; SERMOU_Control
;
; control procedure.
;
;
BeginProc SERMOU_Control

	Control_Dispatch SYS_DYNAMIC_Device_Init, Sermou_Initialize

IFDEF	DEBUG
	Control_Dispatch Debug_Query,	SERMOU_Debug_Query
ENDIF
	clc
	ret

EndProc SERMOU_Control

;==============================================================================

;******************************************************************************
;
; Serial Hardware interrupt procs
;
; Description:
;	Because we can have multiple mice, we have four possible serial mouse
;	hardware interrupts.
;
;	Serial packet defintion:
;
;	Serial	D7   D6   D5   D4   D3   D2   D1   D0
;	----------------------------------------------------
;	Byte 1	--   1    sw1  sw3  y7   y6   x7   x6
;	Byte 2	--   0    x5   x4   x3   x2   x1   x0
;	Byte 3	--   0    y5   y4   y3   y2   y1   y0
;	Byte 4	--   0    rsv  rsv sw2  sw4   y8   x8  for BallPoint only
;
;******************************************************************************

BeginProc Serial_Hw_Int_Proc, High_Freq

	mov	edi, edx			; EDX = reference data at entry

	movzx	edx, [edi.COM_Port]		; get base port (xmit/recv reg)
	add	dl, ACE_IIDR			; Interrupt ID reg
	push	eax				; SAVE IRQ handle
	in	al, dx				; get the value
	test	al, ACE_IIP			; Q: Is interrupt pending ?
	pop	eax				; RETRIEVE irq handle
	jnz	SerInt_LetGo			;    N: get out
	sub	dl, ACE_IIDR			;    Y: handle it EDX = base

	VxDCall	VPICD_Phys_EOI

	in	al,dx				; read byte
	mov	ah,al				; AH = data byte
	add	dl, ACE_LSR			; Line status register
	in	al,dx				; get status
	sub	dl, ACE_LSR			; data reg again
	xchg	ah,al				; AH = status, AL = data
	test	ah, 010b			; data overrun ?
	jnz	SerInt_AbortPacket		; yes, abort the packet
 ;
 ; Store byte in COM_Packet
 ;
	movzx	ecx,[edi.COM_Index]
	jecxz	SerInt_FirstByte		; first byte of packet
	test	al,01000000b			; is sync bit 0 ?
	jnz	SerInt_FirstByte		; No, process as new packet
	mov	BYTE PTR [ecx+edi.COM_Packet],al ; store in packet data
	inc	ecx				; new index
	cmp	[edi.COM_PacketSize],cl		; do we have a complete packet?
	je	SerInt_ProcessPacket		; yes, go process it
	mov	[edi.COM_Index],cl		; save new index

SerInt_Done:
	clc
	ret

SerInt_LetGo:
	stc
	ret

SerInt_FirstByte:
	mov	BYTE PTR [edi.COM_Packet],al	; store first byte
	mov	[edi.COM_Index],1
	test	al,01000000b			; is sync bit 1 ?
	jnz	SerInt_Done			; yes, truly first byte

SerInt_AbortPacket:
	mov	[edi.COM_Index],0		; start with new packet
	jmp	SerInt_Done

SerInt_ProcessPacket:

	mov	ah, [edi.COM_PacketSize]
	mov	[edi.COM_Index],0		; reset index
	mov	edx,edi

	mov	bx, WORD PTR [edx.COM_Packet]
	mov	al, bl				; al =
	shl	al, 6				; al =
	or	al, bh				; al =
	movsx	esi, al				; esi = delta X

	mov	al, bl				; al =
	shr	al, 2				; al =
	shl	al,6				; al =
	or	al, BYTE PTR [edx.COM_Packet+2]	; al =
	movsx	edi, al				; edi = delta Y

	mov	al, bl				; al =
	and	al, 00110000b			; al =
	cmp	ah, 4				; 4 packet serial mouse ?
	jne	Serial_Read_Not_BallPoint
	mov	bl, BYTE PTR [edx.COM_Packet+3]
	and	bl, 00001100b
	or	al, bl

Serial_Read_Not_BallPoint:

	VxDCall	VMD_Post_Pointer_Message
	clc
	ret

EndProc Serial_Hw_Int_Proc

;****
;
; My_Manipulator
;
; Description:
;	Allows us to manipulate pointer data. We hook this to manipulate
;	mouse message data.
;	In this implementation, we just set delta X coords to delta Y
;	and vice versa (if toggle is on). This behavior is toggled by
;	entering .sermou in the wdeb386 debugger.
;
; Entry:
;	AL = button status in the form: 0 0 b1 b3 b2 b4 0 0
;	ESI = Raw delta X counts
;	EDI = Raw delta Y counts
;
; Exit:
;	AL = mapped button status, form x 0 b1 b3 b2 b4 0 0
;	     set x if buttons have been mapped
;	ESI = scaled/rotated X counts
;	EDI = scaled/rotated Y counts
; Uses:
;	Everything except EDX
;==============================================================================
BeginProc My_Manipulator, PUBLIC, High_Freq, Hook_Proc, Real_Manipulator

	test	toggle,1
	jz	MM_Done
	xchg	esi,edi
	ret

MM_Done:
	jmp	[Real_Manipulator]

EndProc My_Manipulator

IFDEF	DEBUG
BeginProc Sermou_Debug_Query

	xor	[toggle],1
	ret

EndProc Sermou_Debug_Query
ENDIF

VxD_LOCKED_CODE_ENDS

VxD_PAGEABLE_CODE_SEG

BeginDoc
;******************************************************************************
;
;  SERMOU_Initialize
;
;  DESCRIPTION:
;		Detects if a serial mouse is attached at the given port.
;		If so, initializes it, otherwise doesn't do anything.
;  ENTRY:
;         EBX = system VM handle
;	  ECX = 0 look at all ports.
;	        else -> Mouse_Instance struct defined in VMD.INC
;
;  EXIT:
;         Carry clear if a serial mouse exists, else CY
;
;  USES:
;	  Everything except EBX.
;
;******************************************************************************
EndDoc

BeginProc Sermou_Initialize, Public, RARE

	mov	[CurrentMouseInstance], ecx
	mov	Ballpoint_ID, 0

	test	ecx, ecx		; check only the port for which
	jz	Serial_DI_Done		; sermou appears in the registry.

	or	[ecx.Mouse_Instance.MI_Flags], MIF_BadDevNode ; device absent

 ;
 ; For serial mice, we will find in the registry for the given devnode,
 ; an entry cominfo=base,irq,portnum in the hardware section for now.
 ; we use it to find the mouse and then virtualize irq etc.
 ;
	mov	edi, ecx		; save mouseinstance
	mov	[prd_Temp], 4		; size of entry to read
	lea	eax, [edi.Mouse_Instance.MI_IO_Base]
	VxDCall	_CONFIGMG_Read_Registry_Value,< \
		[edi.Mouse_Instance.MI_hDevNode], \
		0, OFFSET32 cominfo, REG_BINARY, \
		eax, OFFSET32 prd_Temp, CM_REGISTRY_HARDWARE>
	test	eax, eax
	jnz	Serial_DI_Done

	mov	ax, [edi.Mouse_Instance.MI_IO_Base]
	mov	esi, OFFSET32 SerPort
	mov	[esi.PortBase], ax
	movzx	ax, [edi.Mouse_Instance.MI_IRQNumber]
	mov	[esi.PortIrq], ax

	call	SearchForSerialMouse
	jc	Serial_DI_Done

	call	Install_Mouse_Handler	; install our handler
	jc	Serial_DI_Done

	mov	ah, VMD_Type_Serial
	mov	edi, [CurrentMouseInstance]
	mov	al, [edi.Mouse_Instance.MI_IRQNumber]
	mov	cx, Ballpoint_ID
	xor	ebx, ebx
	jecxz	@F
	or	bl, 1			; we have a ballpoint mouse
@@:
	mov	dh, 2			; button count
	mov	dl, [edi.Mouse_Instance.MI_PortNum] ; port number
	VxDCall	VMD_Set_Mouse_Data

	and	[edi.Mouse_Instance.MI_Flags], NOT MIF_BadDevNode
	or	[edi.Mouse_Instance.MI_Flags], MIF_Detected

	cmp	[Real_Manipulator], 0
	jnz	Serial_DI_Done

	mov	eax,@@VMD_Manipulate_Pointer_Message
	mov	esi,OFFSET32 My_Manipulator
	VMMCall	Hook_Device_Service

Serial_DI_Done:

	ret

EndProc Sermou_Initialize

;******************************************************************************
;
;  Install_Mouse_Handler
;
;  DESCRIPTION:	Installs a mouse handler for a given port.
;
;  Entry:	ESI -> struct for the port.
;  Exit:
;		Carry clear if success.
;  Uses:
;		EAX,ECX,EDX,EDI
;
;******************************************************************************
BeginProc Install_Mouse_Handler, PUBLIC, RARE

	VMMCall	_HeapAllocate, <SIZE COM_INFO, HEAPZEROINIT>
	or	eax, eax
	jnz	@F
	VMMCall	Fatal_Memory_Error
@@:
	mov	edx, eax			; EDX -> COM_INFO struct

	mov	ax, [esi.PortInfo.PortBase]
	mov	[edx.COM_INFO.COM_Port], ax	; set port base
	mov	al, [packetsize]		; save packet size
	mov	[edx.COM_INFO.COM_PacketSize], al

	mov	edi, OFFSET32 Serial_IRQ_Desc
	movzx	eax,[esi.PortIrq]		; get irq #
	mov	[edi.VID_IRQ_Number],ax		; set it
	mov	[edi.VID_Hw_Int_Proc], OFFSET32 Serial_Hw_Int_Proc
	mov	[edi.VID_Options],VPICD_Opt_Can_Share OR VPICD_OPT_REF_DATA
	mov	[edi.VID_Hw_Int_Ref], edx	; COM_INFO struct is refdata
	VxDCall	VPICD_Virtualize_IRQ
	jc	IMH_Done

	VxDCall	VPICD_Phys_EOI
	VxDCall	VPICD_Physically_Unmask

	mov	ecx, [CurrentMouseInstance]
	mov	[ecx.Mouse_Instance.MI_hIrq], eax	; save IRQ handle
	mov	[ecx.Mouse_Instance.MI_Disable], OFFSET32 Serial_Disable
	mov	[ecx.Mouse_Instance.MI_Reference], edx

	call	Serial_Reset_Hardware		; reset the mouse
	clc

IMH_Done:
	ret

EndProc Install_Mouse_Handler

BeginDoc
;******************************************************************************
;
; Serial_Disable
;
; Description:
;		Called by VMOUSE to disable the mouse.
;
; Entry:
;		ESI -> Mouse_Instance
; Exit:
;		None
; Uses:
;		ALL
;==============================================================================
EndDoc

BeginProc Serial_Disable

	movzx	edx, [esi.Mouse_Instance.MI_IO_Base]
	or	edx, edx
	jz	@F
	add	dl, ACE_MCR
	in	al, dx				; get modem control reg
	and	al, NOT 01000b			; reset OUT2 bit
	out	dx, al				; no more com interrupts
@@:
	xor	eax, eax
	xchg	eax, [esi.Mouse_Instance.MI_hIRQ]
	or	eax, eax
	jz	@F
	VxDCall	VPICD_Force_Default_Behavior
@@:
	xor	eax, eax
	xchg	eax, [esi.Mouse_Instance.MI_Reference]	; get COM_INFO
	or	eax, eax
	jz	@F
	VxDCall	_HeapFree,<eax, 0>
@@:
	mov	eax,@@VMD_Manipulate_Pointer_Message
	mov	esi,OFFSET32 My_Manipulator
	VMMCall	UnHook_Device_Service

	ret

EndProc Serial_Disable

;******************************************************************************
;
;  SearchForSerialMouse
;
;  DESCRIPTION:
;
;	Looks for a serial mouse on a comm port.
;
;  ENTRY:
;	ESI -> PortInfo <port#,Irq,handler>
;
;  EXIT:
;         If carry clear,
;              Serial mouse is present
;         Else
;              Serial mouse is not present
;
;  USES:
;         Same as Sermouse_Test_IO_Base
;
;******************************************************************************

BeginProc SearchForSerialMouse, PUBLIC, RARE

	push	esi
	movzx	esi,[esi.PortBase]		; get port #

	call	SERMOUSE_Test_IO_Base
	pop	esi
	ret

EndProc SearchForSerialMouse

;******************************************************************************
;
;  SERMOUSE_Test_IO_Base
;
;  DESCRIPTION:
;
;	Looks for a serial mouse at base in ESI.
;
;  ENTRY:
;	ESI = base port for COMM.
;
;  EXIT:
;	if carry clear, mouse is present, else NOT.
;
;  USES:
;	EDX, AL, flags and the ones used by Serial_TestLoop
;
;******************************************************************************

BeginProc SERMOUSE_Test_IO_Base, RARE

;
;  The Microsoft serial mouse communicates across the serial port at 1200 
;  baud, 7 data bits, and 1 stop bit.  When we test the port, we'll 
;  preserve the existing port parameters, set the serial mouse parameters,
;  and then test the mouse.  If a mouse isn't attached to the serial port,
;  then the original port parameters are restored.
;
	lea	edx,[esi].ACE_LCR           ; Preserve Line Control Register
	in	al, dx
	push	ax

	mov	al, ACE_DLAB	; Latch the baud rate divisor registers
	IO_Delay
	out	dx, al

	lea	edx, [esi].ACE_DLM ; Preserve high byte of the baud rate
	IO_Delay                        ;    divisor
	in	al, dx
	push	ax

	mov	al, 0
	IO_Delay
	out	dx, al

	lea	edx, [esi].ACE_DLL	; Preserve low byte of the baud rate
	IO_Delay                        ;    divisor
	in	al, dx
	push	ax

	mov	al,BAUD_DIVISOR		; 96 for 1200 baud.
	IO_Delay
	out	dx, al

	lea	edx, [esi].ACE_LCR	; Set the Line Control Register to
	mov	al, 0010b               ;    7 data bits, 1 stop bit
	out	dx, al

	lea	edx,[esi].ACE_IER	; Preserve Interrupt Enable Register
	IO_Delay
	in	al, dx
	push	ax

	xor	al, al			; Disable all of the 8250 interrupts
	IO_Delay
	out	dx, al

	lea	edx,[esi].ACE_LCR	; line status
	IO_Delay
	in	al,dx			; clear error flags

	lea	edx,[esi].ACE_MCR       ; Preserve Modem Control Register
	IO_Delay
	in	al, dx
	push	ax

;
;  test if the mouse is present. If so, enable data available interrupt
;  and get out, else restore the hardware state.
;
	call	Serial_TestLoop
	jnc	SERMOUSE_TIB_Mouse_Found

	lea	edx,[esi].ACE_MCR           ; Restore Modem Control Register
	pop	ax
	IO_Delay
	out	dx, al

	lea	edx,[esi].ACE_IER         ; Restore Interrupt Enable Register
	pop	ax
	IO_Delay
	out	dx, al

	lea	edx,[esi].ACE_LCR        ; Access the baud rate divisor latches
	mov	al, ACE_DLAB
	IO_Delay
	out	dx, al

	lea	edx,[esi].ACE_DLL      ; Restore low byte of the baud rate
	pop	ax                          ;    divisor
	IO_Delay
	out	dx, al

	lea	edx,[esi].ACE_DLM    ; Restore high byte of the baud rate
	pop	ax                          ;    divisor
	IO_Delay
	out	dx, al

	lea	edx,[esi].ACE_LCR           ; Restore Line Control Register
	pop	ax
	IO_Delay
	out	dx, al

	stc
	ret

;
; We have found the mouse and programmed it correctly.
; We are only interested in reading the data returned from the mouse
; and so enable only the data ready interrupt.
;

SERMOUSE_TIB_Mouse_Found:
	inc	edx			; point to Interrupt enable register
	mov	al, 1b
	out	dx, al			; enable data available interrupt
	add	esp, 10			; discard pushed paramters

	clc
	ret

EndProc SERMOUSE_Test_IO_Base

;******************************************************************************
;
; Serial_TestLoop
;
; DESCRIPTION:
;		Powers down the mouse for 100ms. Then powers up the mouse
;		and reads three characters returned from the serial port.
;		MS mice return an 'M' or a 'B' (ballpoint) within 50ms
;		of powering up.
;
; ENTRY:	EDX = Modem control register
;
; Exit:
;		if carry clear, mouse is present
;
; Uses:
;		EAX,EDX,ECX,EDI
;
;******************************************************************************

BeginProc Serial_TestLoop, RARE

	mov	al,1			; set RTS, reset DTR
	out	dx,al			; power down the mouse
	sub	dl,4			; Data reg

	VMMCall	Get_System_Time
	mov	ecx,eax			; save current time
	in	al,dx			; read data

Serial_TL_Loop_0:
	VMMCall	Get_System_Time		; get stray data for 0-55msec.
	cmp	eax,ecx
	je	Serial_TL_Loop_0
	in	al,dx
	mov	edi,100			; keep it powered down for 100ms

Serial_TL_Loop_1:
	VMMCall	Get_System_Time
	sub	eax,ecx
	cmp	edi,eax			; get stray data for full delay time
	jae	Serial_TL_Loop_1
	add	dl,5			; Line status register
	in	al,dx			; clear error flags
	dec	dx			; modem control
	mov	al,01011b		; reset RTS, reset DTR
	out	dx,al			; power up the mouse

	mov	ebx,'M' SHL 16		; cmp al,'M' leaves 'M' floating
	mov	bx,('B' SHL 8) OR 3	; we will read 3 chars

Serial_TL_ReadLoop:
	lea	edx,[esi].ACE_LSR	; line status
	VMMCall	Get_System_Time
	mov	ecx,eax			; get original time
	mov	edi,50			; desired delay time

Serial_TL_Loop_2:
	in	al,dx			; read line status
	test	al,1			; is data ready ?
	jnz	Serial_TL_Got_Data	; yes, go look at it
	VMMCall	Get_System_Time
	sub	eax,ecx
	cmp	edi,eax
	jae	Serial_TL_Loop_2
	stc
	ret

Serial_TL_Got_Data:
	mov	edx,esi			; get back data reg
	in	al,dx			; read from talker
	cmp	al,bh			; Q: is it a 'B'
	jne	Serial_TL_NotBallPoint	;  N: not!
	mov	[packetsize],4		; yes it is, packet size is 4
	inc	[BallPoint_ID]
	jmp	Serial_TL_Device_Found

Serial_TL_NotBallPoint:
	ror	ebx,8
	cmp	al,bh
	je	Serial_TL_Mouse_Found
	rol	ebx,8
	dec	bl
	jnz	Serial_TL_ReadLoop
	stc
	ret

Serial_TL_Mouse_Found:
	mov	[packetsize],3

Serial_TL_Device_Found:
	clc
	ret

EndProc Serial_TestLoop

;*****************************************************************************
;
; Serial_Reset_Hardware:
;
; Description:	resets serial mouse hardware
;
; Entry:	ESI -> port information
; Exit:		None
; Uses:		EDX,AL,EDI
;
;*****************************************************************************

BeginProc Serial_Reset_Hardware, RARE

	movzx	edi, [esi.PortBase]		; 
	mov	edx, edi
	add	edx, ACE_MCR
	mov	al, 11b
	out	dx, al
	IO_Delay
	IO_Delay

	mov	edx,edi
	add	edx, ACE_IER
	mov	al, 1
	out	dx, al
	IO_Delay

	mov	edx, edi
	add	edx, ACE_MCR
	IO_Delay
	IO_Delay
	IO_Delay
	in	al, dx
	or	al, 1000b
	IO_Delay
	IO_Delay
	out	dx, al

        ret

EndProc Serial_Reset_Hardware

VxD_PAGEABLE_CODE_ENDS

	END

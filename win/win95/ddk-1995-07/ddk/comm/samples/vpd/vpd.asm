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
;******************************************************************************
TITLE VPD.ASM - Virtual Printer Device
;******************************************************************************
;
;   Title:	VPD.ASM - Virtual Printer Device
;
;==============================================================================
;
;   DESCRIPTION:
;
;	This device virtualizes the parallel printer ports (up to LPT3).  It's
;	only function is to prevent two virtual machines from accessing the
;	same printer at the same time.	If more that one VM attempts to use
;	the same printer then a dialog box will be presented to the user
;	allowing him/her to select which virtual machine should be allowed to
;	use the printer.  Network printer contention is only detectable when
;	an application prints using the BIOS Int 17h service.
;
;	If a special switch is set in the system initialization file then
;	VPD provides special support for a Microsoft debugging program called
;	"SGRAB".  Sgrab is a utility used to grab screen shots from a
;	computer through a parallel printer port.
;
;	VPD virtualizes the printer at three levels:
;	    I/O ports
;	    BIOS service interrupt 17h
;	    Hardware interrupt requset 7 (IRQ 7 is ususally interrupt 0Fh)
;
;
;   PRINTER I/O PORTS:
;
;	PC parallel ports each have three byte-wide I/O ports:	Data, Status,
;	and Control.  An interesting note is that the physical address of the
;	I/O ports for a given LPT is not fixed.  If there is a monochrome
;	display adaptor type LPT port then LPT1's I/O ports will start with
;	port 3BCh. Compaq 386 machines have LPT port at this I/O address on
;	the mutipurpose fixed disk controller board.  On other machines LPT1
;	may be located at port 278h-27Ah.
;
;	There are four word values in the BIOS data area from 0040:0008 to
;	0040:000D which specify the base I/O port of the LPT hardware.
;	The word at 0040:0008 specifies the base I/O port for LPT1, the
;	word at 0040:000A specifies the base I/O port for LPT2, etc.
;
;	To print a byte of data through a physical LPT port a virtual machine
;	must do the following:
;
;	    1.	The VM checks the printer status port.	If the busy, paper out
;		or printer fault signals are active, the VM either waits
;		until the status changes or it shows an error message.
;	    2.	The VM sends a byte of data to the printer data port, then
;		pulses the printer strobe signal (through the printer control
;		port) for 500 ns (or more).
;	    3.	The VM then monitors the printer status port for acknowledge-
;		ment of the data byte before sending the next byte.
;
;	DATA PORT (offset 0 from base):
;	    This is a read-write port that is used to latch a byte of data
;	    that is about to be sent to the printer.  Note that this port
;	    CAN be used to receive data on most machines so it is valid for
;	    a VM to read this port.
;
;	STATUS PORT (offset 1 from base):
;	    This is a read-only port that returns the current printer status.
;
;		BIT MASK
;		-----XXX  000 (Reserved)
;		----X---  0 = Printer error
;		---X----  1 = Printer selected (on line)
;		--X-----  1 = Out of paper
;		-X------  0 = Printer acknowledges correct receipt of data
;		X-------  0 = Printer busy
;
;	CONTROL PORT (offset 2 from base):
;	    This port selects the printer for output, strobes the data into
;	    the printer, and performs other printer control functions.
;
;		BIT MASK
;		-------X  1 = Printer strobe
;		------X-  1 = Printer auto line feed
;		-----X--  0 = Initialize printer
;		----X---  1 = Printer select
;		---X----  1 = Enable hardware interrupt
;		XXX-----  000 (Reserved)
;
;
;   BIOS SERVICE INTERRUPT 17h:
;
;	The IBM system BIOS provides a limited set of services for sending
;	data to a printer and reading its status.  The main purpose of hooking
;	Int 17h is to detect network printer contention.  Since printing
;	to a network printer will not access a hardware printer port, we
;	hook the BIOS service so we can raise contention on applications that
;	use Int 17h.  The three services provided by the system BIOS are:
;
;	    AH = 00h - Print Character
;		 AL = Character to print
;		 DX = Printer to use (0=LPT1, 1=LPT2, etc.)
;		 Returns:
;		    AH = Status
;		       Bit 7 = 1 - Not busy
;		       Bit 6 = 1 - Acknowledge
;		       Bit 5 = 1 - Out of paper
;		       Bit 4 = 1 - Selected
;		       Bit 3 = 1 - I/O error
;		       Bits 2, 1 - Reserved
;		       Bit 0 = 1 - Time-out
;
;	    AH = 01h - Initialize the Printer Port
;		 DX = Printer to use (0=LPT1, 1=LPT2, etc.)
;		 Returns:
;		    AH = Status
;		       Bit 7 = 1 - Not busy
;		       Bit 6 = 1 - Acknowledge
;		       Bit 5 = 1 - Out of paper
;		       Bit 4 = 1 - Selected
;		       Bit 3 = 1 - I/O error
;		       Bits 2, 1 - Reserved
;		       Bit 0 = 1 - Time-out
;
;	    AH = 02h - Read Status
;		 DX = Printer to use (0=LPT1, 1=LPT2, etc.)
;		 Returns:
;		    AH = Status
;		       Bit 7 = 1 - Not busy
;		       Bit 6 = 1 - Acknowledge
;		       Bit 5 = 1 - Out of paper
;		       Bit 4 = 1 - Selected
;		       Bit 3 = 1 - I/O error
;		       Bits 2, 1 - Reserved
;		       Bit 0 = 1 - Time-out
;
;	    AH = 03h to FFh - Reserved
;
;	For each of these services, if the current VM can not access the
;	printer, we will return a status with the time-out and out of paper
;	bits set.  If a VM can access the printer then the Int 17h is
;	reflected to the virtual machine and is handled normally by the
;	BIOS or the network software.
;
;
;   HARDWARE IRQ 7:
;
;	Most programs never use the printer hardware interrupt because it
;	is unreliable on many computers.  Therefore, VPD only virtualizes
;	the IRQ if Sgrab support is enabled.  If Sgrab is NOT enabled then
;	VPD will not virtualize the interrupt, thus leaving the default
;	interrupt handling to VPICD.  If Sgrab is enabled then all printer
;	interrupts are reflected to the virtual machine that owns the
;	display.
;
;	The VPD hardware interrupt handlers are simple.  VPD only pays
;	attention to hardware interrupts (VPD_Hw_Int) and the virtual
;	End Of Interrupt (VPD_Virt_EOI).
;
;	The hardware interrupt procedure requests an interrupt for the
;	current display owner.	When the display owner VM services the
;	interrupt and executes an EOI sequence, VPD_Virt_EOI clears the
;	virtual interrupt request and calls VPICD_Phys_EOI to end the actual
;	hardware interrupt.
;
;
;   SGRAB SUPPORT:
;
;	This VxD also contains support for a Microsoft program called
;	"Sgrab" that is used to grab the current display contents for
;	testing purposes.  Sgrab uses an LPT port and the printer hardware
;	interrupt to perform the screen grab.  The interrupt is used to
;	indicate that a screen grab should be performed and the data is
;	transmitted to a second computer through the LPT I/O ports.
;
;	The user can indicate that he is using Sgrab by including the line:
;	    SgrabLPT=x (where x is a number from 1 to 3)
;	in the system initialization file under the [Win386] section.  This
;	disables trapping of the physical I/O ports and causes VPD to send
;	all printer hardware interrupts to the current display owner.  VPD
;	keeps track of which VM owns the display by watching all "Device_-
;	Set_Focus" control calls that change the ownership of the Virtual
;	Display Device (VDD).
;
;
;   PROTECTED MODE API:
;
;	VPD supports several API calls that can only be made by programs
;	running in protected mode.  These calls are primarily designed for
;	use by the Windows control pannel and print spooler.  The services
;	provided allow Windows to:
;
;	    Get the VPD version
;	    Get information about which printer ports are valid
;	    Get the contention behavior for a specified port
;	    Set the contention behavior for a specified port
;	    Aquire ownership of a specified port
;	    Release ownership of a specified port
;
;******************************************************************************

	.386p

;******************************************************************************
;***	All WPS changes are enclosed in "if WPS"
WPS	EQU	1
;******************************************************************************


;******************************************************************************
;			      I N C L U D E S
;******************************************************************************
VPD_Dynamic     EQU     1	; need-work, test only code

	.XLIST
	INCLUDE VMM.Inc
	INCLUDE Debug.Inc
	INCLUDE VPICD.inc
	INCLUDE SHELL.Inc
	INCLUDE DOSMGR.Inc
	INCLUDE	VCOMM.INC
	INCLUDE	OPTTEST.INC
	.LIST

;******************************************************************************
;		 V I R T U A L	 D E V I C E   D E C L A R A T I O N
;******************************************************************************

Declare_Virtual_Device VPD, 3, 0, VPD_Control, VPD_Device_ID, \
		       VPD_Init_Order+20, , VPD_PM_API_Handler


;******************************************************************************
;				 E Q U A T E S
;******************************************************************************

VPD_Max_Virt_LPTs	EQU	3		; Max of 3 LPT ports
VPD_BIOS_Base_Addr	EQU	408h		; BIOS data for I/O base addr
VPD_BIOS_Timeout_Addr	EQU	478h		; BIOS data for LPT timeouts
VPD_BIOS_Timeout_Size	EQU	VPD_Max_Virt_LPTs
VPD_IRQ 		EQU	7		; IRQ 7 is the LPT interrupt
VPD_Num_Ports_Per_LPT	EQU	3		; Number of I/O ports per LPT
VPD_Default_Timeout	EQU	60000		; 60 seconds

if	WPS
VJPD_Hide_Time          EQU     4000            ; Time to not check port
VJPD_Switch_Wait_Time   EQU     56000           ; Max mode switch takes 26 sec
endif ; WPS

;
;   Status byte to return if printer is being used by another VM.
;
VPD_Busy_Status 	EQU	10111000b

if	WPS
;
;   Status register values		  | "ready" value
;					  v
;				BIT MASK
;				X-------  1 = Not Busy
;				-X------  1 = No Ack data
;				--X-----  0 = No Paper Error
;				---X----  1 = Selected (on line)
;				----X---  1 = No Fault
;				-----X--  0 (Reserved)
;				------X-  0 (Reserved)
;				-------X  0 (Reserved)
VJPD_Not_Busy		EQU	10000000b
VJPD_Not_Ack		EQU	01000000b
VJPD_Paper_Err		EQU	00100000b
VJPD_Selected		EQU	00010000b
VJPD_Not_Fault		EQU	00001000b
VJPD_Reserved		EQU	00000111b
;
; to see if printer is in PCL ready mode we check that the bits specified
; by VJPD_Status_Ready_Mask are set to the values in VJPD_Status_Ready
VJPD_Status_Ready_Mask  EQU     10111000b
VJPD_Status_Ready       EQU     10011000b
;
;   Control byte to set during Mode Switch message out
;
VJPD_Data_Strobe	EQU	000000001b
;
;   Mask reserved bits in control register
;
VJPD_Control_Mask       EQU     00011111b
endif ; WPS

if	WPS
;
;******************************************************************************
;                                M A C R O S
;******************************************************************************

;
;   Output a byte with 2 pre-fetch flushes (JMP $+2) to delay for I/O Bus
;
OutP MACRO Port, Value
        out     Port, Value
        jmp     $+2
        jmp     $+2
ENDM OutP
endif ; WPS

;******************************************************************************
;			     S T R U C T U R E S
;******************************************************************************

VPD_Struc STRUC
VPD_LPT_Number		dd	?		; Printer port number (1-3)
VPD_Timeout_Limit	dd	?		; Ownership time-out limit
VPD_Proc_Addr		dd	?		; Address of I/O trap routine
VPD_Name		db	"LPTx    "	; Printer port name "LPTx"
VPD_IO_Base		dd	0		; Base I/O port (0=not phys)
VPD_Last_Touched	dd	?		; Time LPT was last accessed
VPD_Owner		dd	0		; VM handle of printer owner
VPD_Flags		dd	0		; Flags
VPD_NotifyProc		dd	0		; procedure to notify for
VPD_NotifyRefData	dd	0		; loss of port and its refdata
VPD_Alt_NotifyProc	dd	0		; alternate owner procedure
VPD_Alt_NotifyRefData	dd	0		; alternate reference data

if	WPS
VJPD_Non_WPS_Mode       	dd      0      	; Address of non-WPS mode switch
VJPD_Non_WPS_Mode_Size  	dd      0      	; Size of non-WPS mode switch
VJPD_Send_Switch_Timeout	dd	0	; Mode Switch Timeout time
endif ; WPS

VPD_Struc ENDS


;******************************************************************************
;			   F L A G   E Q U A T E S
;******************************************************************************

VPDF_Special_Owner	EQU	00000001b
VPDF_Special_Owner_Bit	EQU	00h
VPDF_Is_Connected	EQU	00000010b
VPDF_Is_Connected_Bit	EQU	01h
VPDF_AltOwnerActive	EQU	00000100b
VPDF_AltOwnerActiveBit	EQU	02h

if	WPS
VJPDF_Jumbo             EQU     00001000b	; owned (or last owned) by WPS
VJPDF_Jumbo_Bit         EQU     03h
VJPDF_NONJumbo          EQU     00010000b	; Non Jumbo WPS
VJPDF_NONJumbo_Bit      EQU     04h
VJPDF_PCLSwitch		EQU	00100000b	; mode switch in progress
VJPDF_PCLSwitch_Bit	EQU	05h
VJPDF_JumboAcq		EQU     01000000b	; Acquire request by Jumbo
VJPDF_JumboAcq_Bit	EQU     06h
endif ; WPS

VPDF_Enabled		EQU	10000000b
VPDF_EnabledBit		EQU	07h

;******************************************************************************
;		   I N I T I A L I Z A T I O N	 D A T A
;******************************************************************************

VxD_IDATA_SEG

;
;   IRQ descriptor used to virtualize the LPT IRQ (7)
;
VPD_IRQ_Descriptor VPICD_IRQ_Descriptor <VPD_IRQ,,OFFSET32 VPD_Hw_Int,,\
					 OFFSET32 VPD_Virt_EOI>

;
;   Instance data structures 40:78-40:7B
;
VPD_Inst_Timeout   InstDataStruc <,,VPD_BIOS_Timeout_Addr,VPD_BIOS_Timeout_Size,ALWAYS_Field>

BeginMsg
;
;   String used to look for "SgrabLPT=x" entry in SYSTEM.INI file
;
Sgrab_Ini_String db "SGRABLPT", 0

;
;   String used to get "LPTxAUTOASSIGN" SYSTEM.INI entry values.
;
VPD_Auto_Assign_Str db "LPT"
VPD_Auto_Assign_Num db "x"
		    db "AUTOASSIGN", 0

;
;   String used to get "LPTxDisable" SYSTEM.INI entry values.
;
VPD_Disable_Str	db	"LPT"
VPD_Disable_Num	db	"x"
		db	"DISABLE", 0

if	WPS
;
;   String used to get PCL Mode switch timeout SYSTEM.INI [386enh] entry value.
;
VJPD_Switch_Timeout_Key db "LPTSwitchTimeout", 0
;
;   String used to get Jumbo LPT Flags from SYSTEM.INI [386enh] entries.
;
VJPD_Jumbo_LPT          db "WPSLPT"
VJPD_Jumbo_LPT_Num      db "x", 0
VJPD_NONJumbo_LPT       db "NONJUMBOLPT"
VJPD_NONJumbo_LPT_Num   db "x", 0
endif ; WPS

EndMsg

BeginDoc
;
;   String used to instance the DOS PRN: device.
;
VPD_PRN_Device_Name db "PRN     "
;
;   String used to instance IBM EPT device driver
;
;;;VPD_EPT_Device_Name db "EPT     "
EndDoc


VxD_IDATA_ENDS


;******************************************************************************
;	      R E A L	M O D E   I N I T I A L I Z A T I O N
;******************************************************************************

VxD_REAL_INIT_SEG

;
;   Data for real mode code is in the same segment.  This is the device
;   driver's name for the EPT port.
;
;;;EPT_Device_Name db "EPT", 0


;******************************************************************************
;
;   VPD_Real_Mode_Init
;
;   DESCRIPTION:
;	An IBM postscript printer uses a device driver named "EPT" that
;	must be instanced.
;
;   ENTRY:
;	CS, DS = Real mode segment
;	BX = Flags
;
;   EXIT:
;	EDX = Reference data to pass to protected mode portion of VxD
;
;   USES:
;	AX, BX, DX, SI, Flags
;
;==============================================================================

BeginProc VPD_Real_Mode_Init

;
;   If another printer VxD is loaded then don't load -- Just abort our load
;
	test	bx, Duplicate_From_INT2F OR Duplicate_Device_ID
	jnz	SHORT VPD_RI_Abort_Load


;
;   Try to open a file named "EPT" (hopefully this will be the EPT device)
;
;;;	mov	dx, OFFSET EPT_Device_Name	; DS:DX -> "EPT", 0
;;;	mov	ax, 3D00h			; Open file
;;;	int	21h				; Q: Can Mr. DOS find the file?
;;;	jc	SHORT VPD_RMI_No_EPT		;    N: Done!
						;    Y: Make sure it's device
;
;   The file open worked.  Now make sure it's a device.
;
;;;	xor	dx, dx				; 0 in case it fails
;;;	mov	bx, ax				; BX = File handle
;;;	mov	ax, 4400h			; Get device info IOCTL
;;;	int	21h				; Call Mr. Operating System
	; Ignore any error!			; (returns result in DX)

;;;	mov	ah, 3Eh
;;;	int	21h				; Close the file

;;;	test	dx, 10000000b			; Q: Is this a device
;;;	jnz	SHORT VPD_RMI_Exit		;    Y: EDX != 0 on init call

VPD_RMI_No_EPT:
	xor	edx, edx			; EDX = 0 to indicate no EPT

VPD_RMI_Exit:
	xor	bx, bx
	xor	si, si
	mov	ax, Device_Load_Ok
	ret

;
;   Another VPD device has already been loaded so we won't.
;
VPD_RI_Abort_Load:
	xor	bx, bx
	xor	si, si
	mov	ax, Abort_Device_Load + No_Fail_Message
	ret

EndProc VPD_Real_Mode_Init

VxD_REAL_INIT_ENDS


;******************************************************************************
;			    L O C A L	D A T A
;******************************************************************************

VxD_DATA_SEG

VPD_CB_Offset	dd	?

;
;   Printer data structures.
;   THESE MUST REMAIN IN ORDER FROM LPT1 --> LPTn
;
VPD_LPT1 VPD_Struc <1, VPD_Default_Timeout, OFFSET32 VPD_LPT1_Trap,"LPT1">
VPD_LPT2 VPD_Struc <2, VPD_Default_Timeout, OFFSET32 VPD_LPT2_Trap,"LPT2">
VPD_LPT3 VPD_Struc <3, VPD_Default_Timeout, OFFSET32 VPD_LPT3_Trap,"LPT3">

if	WPS
;
;   Mode Command char defintions
;
ESCAPE	EQU	01Bh
FM	EQU	01Bh
QM	EQU	082h
;
;   Jumbo Printer PCL Mode Switch Command String
;
VJPD_Jumbo_PCL_Mode	db	(8) dup (0)
			db	ESCAPE, "*c16384D"
			db	ESCAPE, ")s39W"
			db	FM, FM, FM, FM, QM
			dw	-1
			dw	01800h
			dd	-1
			dw	00600h, 00800h
			dd	0, 0, 0, 0
			db	033h, 003h, 01Dh, 0E5h, FM, FM
VJPD_Jumbo_PCL_Mode_Size EQU $ - VJPD_Jumbo_PCL_Mode

VJPD_Send_Switch_Limit	dd	4000		; Timeout on sending mode switch
VJPD_Null_Size		EQU	128	 	; nulls to send after a mode switch

endif ; WPS

VxD_DATA_ENDS


VxD_LOCKED_DATA_SEG
;
;   Data for SGRAB support.  The VPD_Sgrab_LPT variable will contain a pointer
;   to the appropriate LPT data structure if SGRAB is enabled.	VPD_Hw_Int_
;   Owner will contain the handle of the virtual machine that currently owns
;   the display.
;
VPD_Sgrab_LPT		dd	0	    ; Turn off contention for this LPT
VPD_Hw_Int_Owner	dd	0	    ; Owner of next hardware interrupt

VxD_LOCKED_DATA_ENDS



;******************************************************************************
;	       D E V I C E   C O N T R O L   P R O C E D U R E
;******************************************************************************

VxD_CODE_SEG

;******************************************************************************
;
;   VPD_Control
;
;   DESCRIPTION:
;	This is the Virtual Printer Device's control procedure.
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

BeginProc VPD_Control

	Control_Dispatch Device_Init, VPD_Device_Init
	Control_Dispatch Init_Complete, VPD_Init_Complete
	Control_Dispatch Set_Device_Focus, <VPD_Set_Focus>
	Control_Dispatch VM_Not_Executeable, <VPD_VM_Not_Executeable>
	Control_Dispatch GET_CONTENTION_HANDLER, VPD_Get_Contention_Handler

if	WPS
        Control_Dispatch System_Exit, <VJPD_System_Exit>
endif ; WPS

IFDEF DEBUG
	Control_Dispatch Debug_Query, VPD_Debug_Query
ENDIF

	clc					; Ignore other control calls
	ret

EndProc VPD_Control

VxD_CODE_ENDS


;******************************************************************************
;		    I N I T I A L I Z A T I O N   C O D E
;******************************************************************************

; VxD_ICODE_SEG   need-work, tmp workaround
VxD_CODE_SEG

;******************************************************************************
;
;   VPD_Device_Init
;
;   DESCRIPTION:
;	This procedure searches the system BIOS data area for any printers.
;	If there are no printers then this procedure will return with carry
;	set to indicate that the VPD should never be called again.  If it
;	does locate one or more printers it will hook the appropriate I/O
;	ports and software interrupt 17h.  It will also instance the
;	printer time-out values.  This allows every virtual machine to set
;	a different length time-out without effecting other VM's states.
;
;	If a "SGRABLPT=x" entry exists in the system initialization file
;	then this procedure will also virtualize the LPT interrupt (IRQ 7).
;
;   ENTRY:
;	EBX = System VM handle
;
;   EXIT:
;	If carry flag set then
;	    VPD should never be called again (no printers)
;	else
;	    Printers present in system
;
;   USES:
;	EAX, EBX, ECX, EDX, ESI, EDI, Flags
;
;==============================================================================

BeginProc VPD_Device_Init

;
;   Try to find the "SGRABLPT=x" entry in the System.Ini file.	If it is not
;   found then leave VPD_Sgrab_LPT to 0 (default value), otherwise set it
;   to the value returned from Get_Profile_Decimal_Int.
;
	xor	esi, esi			; Look in [WIN386] section

if	WPS

ifdef MAXDEBUG
	Trace_Out "VPD_Device_Init, ver 3/30 11:30"
endif

	mov	edi, OFFSET32 VJPD_Switch_Timeout_Key	; SYSTEM.INI Keyword=
	mov	eax, [VJPD_Send_Switch_Limit]		; Default Value
	VMMcall Get_Profile_Decimal_Int
	mov	[VJPD_Send_Switch_Limit], eax		; Returned Value
endif ; WPS

	mov	edi, OFFSET32 Sgrab_Ini_String	; Look for this entry
	VMMcall Get_Profile_Decimal_Int 	; EAX = Value of "SGRABLPT"
	jc	SHORT VPD_Init_Ports		; If not found then leave 0
	test	eax, eax			; Q: Is it 0?
	jz	SHORT VPD_Init_Ports		;    Y: Forget it
	cmp	eax, VPD_Max_Virt_LPTs		; Q: Is this a valid value?
	ja	SHORT VPD_Init_Ports		;    N: Leave it 0
						;    Y: Point at structure
	dec	eax				; EAX = 0-3
	imul	eax, SIZE VPD_Struc		; EAX = Offset in LPT table
	add	eax, OFFSET32 VPD_LPT1		; EAX -> Appropriate structure
	mov	[VPD_Sgrab_LPT], eax		; Save it for later
	mov	[VPD_Hw_Int_Owner], ebx 	; Set interrupt owner to Sys_VM

;
;   Loop to initialize all existing LPTs.
;
VPD_Init_Ports:
	xor	edi, edi			; EDI = Count of LPTs so far
	xor	ecx, ecx			; ECX = LPT number
	mov	esi, OFFSET32 VPD_LPT1		; ESI -> First LPT structure

VPD_Init_Loop:
;
;   Check for an entry "LPTnDisable=true/false" in the SYSTEM.INI file.
;   If true, then don't hook ports here.
;   Check for an entry "LPTnAUTOASSIGN=x" in the SYSTEM.INI file.  If one
;   is not found the use the default time-out value (1 minute).
;
	push	ecx				; Save these for later
	push	esi
	push	edi
	add	cl, "1" 			; CL = ASCII LPT number (1-3)

if	WPS
	mov	[VJPD_Jumbo_LPT_Num], cl	; Set WPSLPTn string
	mov	[VJPD_NONJumbo_LPT_Num], cl	; Set NONJUMBOLPTn string
endif ; WPS

	mov	[VPD_Disable_Num], cl		; set string to this LPT
	mov	[VPD_Auto_Assign_Num], cl	; Set string to this LPT

	xor	esi, esi
	mov	edi, OFFSET32 VPD_Disable_Str	; look for this string
	xor	eax, eax			; assume not disable...
	VMMCall	Get_Profile_Boolean
	test	eax, eax
	pop	edi
	pop	esi
	pop	ecx
	TRAPNZ
	jnz	VPD_Init_Next_LPT

	push	ecx
	push	esi
	push	edi

	xor	esi, esi			; Use [WIN386] section
	mov	edi, OFFSET32 VPD_Auto_Assign_Str;Look for this string
	mov	eax, VPD_Default_Timeout	; Default time-out value
	mov	ecx, 3				; 3 decimal points
	VMMcall Get_Profile_Fixed_Point 	; Get entry from SYSTEM.INI
	cmp	eax, -1000			; Q: Is entry -1.000?
	jne	SHORT VPD_Valid_Timeout 	;    N: Use value in EAX
	or	eax, -1 			;    Y: Set to -1
VPD_Valid_Timeout:
	pop	edi				; Restore registers
	pop	esi
	pop	ecx
	mov	[esi.VPD_Timeout_Limit], eax	; Save time-out value

;
;   To determine if a printer port exists we look at the BIOS data area for
;   non-zero entries in the printer base address fields.  If the entry contains
;   a value > 0FFh then we assume it is the base hardware port for the LPT,
;   otherwise we assume it is a network printer connection (networks tend to
;   put a 1 in the this field).  If the LPT is a hardware parallel printer port
;   then VPD hooks three I/O ports.
;
	movzx	edx, WORD PTR [VPD_BIOS_Base_Addr][ecx*2]
	test	edx, edx			; Q: Is there a printer here?
	jz	VPD_Init_Next_LPT 		;    N: Test next one
	mov	[esi.VPD_IO_Base], edx		;    Y: Save port base addr
	inc	edi				; One more LPT virtualized
	test	dh, dh				; Q: Hardware port? (> 0FFh)
	jz	VPD_Init_Next_LPT		;    N: Network printer!
	cmp	esi, [VPD_Sgrab_LPT]		;    Y: Q: SGRAB using it?
	je	VPD_Init_Next_LPT		;	   Y: Don't trap I/O
						;	   N: Hook I/O ports
;
;   Some networks place the I/O address for LPT1 into the LPT2 and LPT3
;   base address fields.  Make sure that hasn't happened.
;
	cmp	esi, OFFSET32 VPD_LPT1
	je	SHORT VPD_Hardware_Port
	cmp	[VPD_LPT1.VPD_IO_Base], edx
	je	SHORT VPD_Duplicate_IO_Port
	cmp	esi, OFFSET32 VPD_LPT2
	je	SHORT VPD_Hardware_Port
	cmp	[VPD_LPT2.VPD_IO_Base], edx
	jne	SHORT VPD_Hardware_Port
	.ERRNZ VPD_Max_Virt_LPTs - 3

VPD_Duplicate_IO_Port:				; I/O base address duplicated
	mov	[esi.VPD_IO_Base], 1		; Set IO base to 1 to indicate
	jmp	SHORT VPD_Init_Next_LPT 	; a network printer port


;
;   This is a physical printer port.  Hook the I/O ports starting with
;   port # in EDX.
;
VPD_Hardware_Port:

if	WPS
;
; Does this port have a Jumbo 2Way Printer? If so, set flags
;
	pushad
	xor	esi, esi			; Use [386Enh] section
	mov	edi, OFFSET32 VJPD_Jumbo_LPT	; SYSTEM.INI Keyword=
	xor	eax, eax			; Default Value = FALSE
	VMMcall Get_Profile_Boolean
	cmp	eax, 0
	popad
	je	SHORT VJPD_Not_Jumbo_Device

;
; check for WPS but not JUMBO
;
	pushad
	xor	esi, esi			; Use [386Enh] section
	mov	edi, OFFSET32 VJPD_NONJumbo_LPT ; SYSTEM.INI Keyword=
	xor	eax, eax			; Default Value = FALSE
	VMMcall Get_Profile_Boolean
	cmp	eax, 0
	popad
	je	SHORT @F
	or	[esi.VPD_Flags], VJPDF_NONJumbo
@@:

VJPD_Not_Jumbo_Device:

endif ; WPS

	push	ecx				; Save loop counter
	push	esi				; and data structure pointer

	mov	esi, [esi.VPD_Proc_Addr]	; ESI -> Procedure to call
	mov	ecx, VPD_Num_Ports_Per_LPT	; ECX = # of ports to hook
VPD_ITrap_Loop:
	VMMcall Install_IO_Handler		; Install port hook
	jc	VPD_ITrapped
	VMMCall	Disable_Global_Trapping
	inc	edx				; EDX = Next LPT port to hook
	loopd	VPD_ITrap_Loop			; Loop until all hooked
	clc

VPD_ITrapped:
	pop	esi				; Restore
	pop	ecx
	jnc	VPD_Init_Next_LPT
	mov	[esi.VPD_IO_Base], 0

VPD_Init_Next_LPT:
	add	esi, SIZE VPD_Struc		; ESI -> Next data structure
	inc	ecx				; Bump counter
	cmp	ecx, VPD_Max_Virt_LPTs		; Q: More LPTs to init?
	jb	VPD_Init_Loop			;    Y: Loop

;
;   All printer ports hooked.  At this point we check to see if there are
;   any printers (network or hardware) connected.  If there are none then
;   VPD_Device_Init will return with carry SET to indicate that it should
;   never be called again.  If any printers do exist then VPD hooks IRQ 7
;   hardware interrupt and software interrpt 17h.
;
	test	edi, edi			; Q: ANY printers virtualized?
	jnz	SHORT VPD_LPTs_Found		;    Y: VPD is needed
						;    N: Nothing to do
	stc					; Tell VMM to never call us
	ret

;
;   Set up for simulated hardware interrupts.  If VPD can not hook IRQ 7
;   then it will disable SGRAB support since SGRAB uses the printer
;   interrupt.
;
VPD_LPTs_Found:
	cmp	[VPD_Sgrab_LPT], 0		; Q: Is SGRAB enabled?
	je	SHORT VPD_Alloc_CB_Area 	;    N: Let VPICD handle IRQ
						;    Y: Want to virtualize it
	mov	edi,OFFSET32 VPD_IRQ_Descriptor ; EDI -> VPD IRQ descriptor
	VxDCall VPICD_Virtualize_IRQ		; Q: Can we virtualize IRQ?
	jnc	SHORT VPD_IRQ_Virt_OK		;    Y: OK -- Save IRQ handle
						;    N: ERROR!
	Debug_Out "WARNING:  VPD could not virtualize IRQ"
	mov	[VPD_Sgrab_LPT], 0		; Can't use SGRAB!
	jmp	SHORT VPD_Alloc_CB_Area 	; Go to next step of init

VPD_IRQ_Virt_OK:
;
;   NOTE:  At this point EAX contains the IRQ handle returned from
;   VPICD_Virtualize_IRQ.  However, since it is always passed back to
;   the VPD interrupt procedures, there is no need to save it.	Normally,
;   a VxD would save the handle in a global variable at this point:
;	mov	[VPD_IRQ_Handle], eax
;


VPD_Alloc_CB_Area:
;
;   Allocate per-VM data area in the control block.
;
	VMMcall _Allocate_Device_CB_Area, <4,0> ; One DWORD for contention info
	test	eax, eax
	jnz	SHORT VPD_CB_Ok
	Debug_Out "VPD ERROR:  Could not alloc control block data area space"
	VMMcall Fatal_Memory_Error

VPD_CB_Ok:
	mov	[VPD_CB_Offset], eax		; Save offset

;
;   Instance the printer BIOS areas.  This will make each virtual machine
;   have its own copy of the printer timeout values.
;
	VMMCall _AddInstanceItem, <<OFFSET32 VPD_Inst_Timeout>, 0>
	test	eax, eax
	jnz	SHORT VPD_Inst_Ok
	Debug_Out "VPD ERROR:  Could not instance BIOS printer area"
	VMMcall Fatal_Memory_Error

VPD_Inst_Ok:
;
;   Hook Int 17h BIOS software interrupt to detect contention.	This allows
;   detection of some network printer contention also.
;
	mov	eax, 17h			; # of interrupt to hook
	mov	esi, OFFSET32 VPD_Int_17h	; Where to call
	VMMcall Hook_V86_Int_Chain		; Hook the interrupt

;
;   Done!  Return with carry flag clear to indicate success.
;
	clc					; Successful initialization
	ret

EndProc VPD_Device_Init



;******************************************************************************
;
;   VPD_Init_Complete
;
;   DESCRIPTION:
;	Instance DOS devices for LPT1-LPT3 and PRN.  This must be delayed
;	until the Init_Complete control call since the DOSMGR service is
;	not valid to call until this time.  We do not check the carry flag
;	after calling DOSMGR_Instance_Device because the DOS device may
;	may not exist or may reside in the DOS BIOS (devices in the DOS
;	BIOS can not be instanced by DOSMGR_Instance_Device).  In any case,
;	no error is flaged if one or more of the devices can not be instanced.
;
;   ENTRY:
;	EBX = System VM Handle
;
;   EXIT:
;	Carry clear (Always works)
;
;   USES:
;
;==============================================================================

BeginProc VPD_Init_Complete

;
;   Instance devices LPT1, LPT2, and LPT3
;
ifdef MAXDEBUG
	Trace_Out "VPD_Init_Complete"
endif
	mov	ecx, VPD_Max_Virt_LPTs		; Number of LPT devices
	lea	esi, VPD_LPT1.VPD_Name		; ESI -> "LPT1    "
VPD_IC_Instance_Loop:
	VxDcall DOSMGR_Instance_Device		; Instance this LPT
	add	esi, SIZE VPD_Struc		; ESI -> Next name
	loopd	VPD_IC_Instance_Loop		; Loop for all LPTs

;
;   Instance the PRN device
;
	mov	esi, OFFSET32 VPD_PRN_Device_Name
	VxDcall DOSMGR_Instance_Device

;
;   If EPT device is installed then instance it.
;
;;;	test	edx, edx
;;;	jz	SHORT VPD_IC_Exit
;;;	mov	esi, OFFSET32 VPD_EPT_Device_Name
;;;	VxDcall DOSMGR_Instance_Device

;
;   Return with carry clear to indicate success
;
VPD_IC_Exit:
	clc
	ret

EndProc VPD_Init_Complete

; VxD_ICODE_ENDS   need-work, tmp workaround
VxD_CODE_ENDS

VxD_Pageable_Data_Seg

Contention_Handler	dd	OFFSET32 _VPD_Map_Name_To_Resource
			dd	OFFSET32 _VPD_Acquire_Resource
			dd	OFFSET32 _VPD_Steal_Resource
			dd	OFFSET32 _VPD_Release_Resource
			dd	OFFSET32 _VPD_Add_Resource

VxD_Pageable_Data_Ends

VxD_Pageable_Code_Seg

;******************************************************************************
;
; VPD_Map_Name_To_Resource
;
; Description: maps a name such as LPTx to a resource
;
; Parameters:
;
;	FunctionCode = MAP_NAME_TO_RESOURCE
;	PName -> Name to map
;	hDevNode = devnode corresponding to the device
;	IOBase	= Base allocated to the device
;
; Returns:
;	0 if could not be mapped, else Struct for the port.
;
;=============================================================================
BeginProc VPD_Map_Name_To_Resource,CCALL,PUBLIC

ArgVar	FunctionName,DWORD
ArgVar	PName,DWORD
ArgVar	hDevNode,DWORD
ArgVar	IOBase,DWORD

	EnterProc
ifdef MAXDEBUG
	Trace_Out "VPD_MapName_To_Resource"
endif

	mov_b	ecx,VPD_Max_Virt_LPTs
	mov	edx,OFFSET32 VPD_LPT1		; first LPT structure
	mov	eax,IOBase

MapName_Loop:
	cmp	[edx.VPD_IO_Base],eax
	je	MapName_Done
	lea	edx,[edx+ SIZE VPD_Struc]
	loopd	MapName_Loop
	xor	edx,edx				; Couldn't map

MapName_Done:
	mov	eax,edx				; return PTR to VPD_Struc

ifdef MAXDEBUG
	Trace_Out "Leaving"
endif
	LeaveProc
	return

EndProc VPD_Map_Name_To_Resource

;******************************************************************************
;
; VPD_Steal_Resource
;
; Description:
;	Called to steal a port which was lost due to someone else
;	wanting control.
;
; Parameters:
;	FunctionCode = STEAL_RESOURCE
;	ResourceHnd = resource handle
;	NotifyProc = Address of proc set during Acquire_Resource.
;
; Exit:
;	EAX != 0 if stolen, else = 0.
;
;==============================================================================
BeginProc VPD_Steal_Resource,CCALL,PUBLIC

ArgVar	FunctionCode,DWORD
ArgVar	Resource,DWORD
ArgVar	NotifyProc,DWORD

	EnterProc

	SaveReg	<esi,edi,ebx>
ifdef MAXDEBUG
	Trace_Out "VPD_Steal_Resource"
endif

	mov	esi,Resource
	test	esi,esi
	jz	VSR_Failed
	VMMCall	Get_Sys_VM_Handle
	mov	eax,NotifyProc
	cmp	[esi.VPD_Alt_NotifyProc],eax ; Q: Alternate owner ?
	jne	VSR_Primary_Owner_Steal	;    N: steal it for primary owner
	cmp	ebx,[esi.VPD_Owner]	; Q: Does primary owner own it ?
	jnz	VSR_Switch_Owners	;    N: Switch owners and try stealing.
	cCall	[esi.VPD_NotifyProc],<[esi.VPD_NotifyRefData],0> ; losing port
	test	eax,eax			; Q: Ok to steal from primary owner ?
	jz	VSR_Done		;   N:
	mov	eax,NotifyProc

VSR_Switch_Owners:
 ;
 ; Some other VM owns it, and alternate owner wants to steal. We will make
 ; it primary owner and try to steal.
 ;
	xchg	eax,[esi.VPD_NotifyProc] ; VPD_NotifyProc = alt notifyProc
	xchg	eax,[esi.VPD_Alt_NotifyProc]	; VPD_Alt_NotifyProc = prim own
	mov	eax,[esi.VPD_NotifyRefData]
	xchg	eax,[esi.VPD_Alt_NotifyRefData] ; VPD_Alt_NotifyRef = prim ref
	mov	[esi.VPD_NotifyRefData],eax	; VPD_NotifyRefData = alt ref

VSR_Primary_Owner_Steal:
	cmp	ebx,[esi.VPD_Owner]
	jz	VSR_Success
	xor	al,al			; set Z => Don't force ownership
	call	VPD_Acquire_Port
	jnc	VSR_Success

VSR_Failed:
	xor	eax,eax
	jmp	VSR_Done

VSR_Success:
	or	al,1

VSR_Done:
	RestoreReg <ebx,edi,esi>
ifdef MAXDEBUG
	Trace_Out "Leaving"
endif
	LeaveProc
	return

EndProc VPD_Steal_Resource

;******************************************************************************
;
; VPD_Acquire_Resource
;
; Description:
;	Tries to acquire the port.
;
; Entry:
;	FunctionCode = ACQUIRE_RESOURCE
;	Resource = VPD_Struc
;	NotifyProc -> Address of procedure to call
;	NotifyRefData = reference data to call with.
;	StealFlag = if 0, don't even try to acquire if owned
;		    if 1, acquire it, try to steal if owned.
;
; Exit:
;	0 if not acquired, else port handle
; Uses:
;	C style.
;==============================================================================
BeginProc VPD_Acquire_Resource,CCALL,PUBLIC,esp

ArgVar	FunctionCode,DWORD
ArgVar	Resource,DWORD
ArgVar	NotifyProc,DWORD
ArgVar	NotifyRefData,DWORD
ArgVar	StealFlag,DWORD

	EnterProc

	SaveReg	<esi,edi,ebx>
ifdef MAXDEBUG
	Trace_Out "VPD_Acquire_Resource"
endif

	mov	esi,Resource		; VPD_STRUC
	test	esi,esi
	jz	VAR_Failed

if	WPS
	TestMem	[esi.VPD_Flags], VJPDF_Jumbo
	jz	@F
	mov	eax,Resource
	VMMCall	Get_Cur_VM_Handle
	cmp	[esi.VPD_Owner], ebx		; Q: Does this VM own it?
	je	VAR_Done			;   Y: no switch needed
@@:
endif

	TestMem	[esi.VPD_Flags],VPDF_Enabled	; Q: Ports already hooked ?
	jnz	@F				;    Y: no need to rehook
	call	VPD_Enable_Trapping
@@:
	TestMem	[esi.VPD_Flags],VPDF_AltOwnerActive ; Q: Two owners pending ?
	jnz	VAR_Failed		;  Y: no more owners allowed.

	mov	ecx,[esi.VPD_NotifyProc] ; Q: Owned port ?
	jecxz	VAR_Acquire		;    N: let it be acquired.
	cmp	StealFlag,0		; Y: Should we try to acquire ?
	je	VAR_Failed		;    N:
	cCall	ecx,<[esi.VPD_NotifyRefData],0> ; losing port
	test	eax,eax			; Q: Should we allow it ?
	jz	VAR_Done		;    N:
	mov	ecx,[esi.VPD_NotifyProc] ; Q: Did owner free the port ?
	jecxz	VAR_Acquire		; Y: normal acquire
	mov	edx,[esi.VPD_NotifyRefData]
	mov	[esi.VPD_Alt_NotifyProc],ecx
	mov	[esi.VPD_Alt_NotifyRefData],edx
	SetFlag	[esi.VPD_Flags],VPDF_AltOwnerActive

VAR_Acquire:
	VMMCall	Get_Sys_VM_Handle
	xor	al,al			; set Z => Don't force ownership
	call	VPD_Acquire_Port
	jc	VAR_Failed
	mov	eax,NotifyProc
	mov	[esi.VPD_NotifyProc],eax
	mov	eax,NotifyRefData
	mov	[esi.VPD_NotifyRefData],eax
	mov	eax,Resource		; return this as the handle also.
	jmp	VAR_Done

VAR_Failed:
	xor	eax,eax

VAR_Done:

	RestoreReg <ebx,edi,esi>
ifdef MAXDEBUG
	Trace_Out "Leaving"
endif
	LeaveProc
	return

EndProc VPD_Acquire_Resource

;******************************************************************************
;
; VPD_Release_Resource
;
; Description:
;	Called to release a resource acquired via Acquire_Resource.
;
; Parameters:
;	FunctionCode = RELEASE_RESOURCE
;	ResourceHnd = handle of the resource
;	NotifyProc = Address of proc set during Acquire_Resource.
; Exit:
;	None
;
; Uses:	C Style
;==============================================================================
BeginProc VPD_Release_Resource,CCALL,PUBLIC,esp

ArgVar	FunctionCode,DWORD
ArgVar	ResourceHnd,DWORD
ArgVar	NotifyProc,DWORD

	EnterProc

	SaveReg	<esi,edi,ebx>
ifdef MAXDEBUG
	Trace_Out "VPD_Release_Resource"
endif

	mov	esi,ResourceHnd
	mov	eax,NotifyProc
	cmp	[esi.VPD_Alt_NotifyProc],eax	; Q: Alternate owner ?
	jz	VRR_Alt_Owner			;    Y: just clear the flag

	VMMCall	Get_Sys_VM_Handle
	cmp	[esi.VPD_Owner],ebx
	jne	VRR_Release_Resource_Done

	call	VPD_Detach

VRR_Release_Resource_Done:
	mov	[esi.VPD_NotifyProc],0
	btr	[esi.VPD_Flags],VPDF_AltOwnerActiveBit ; Q: Pending alt_Owner ?
	jnc	VRR_Done

	cCall	VPD_Acquire_Resource,<ACQUIRE_RESOURCE,esi, \
		[esi.VPD_Alt_NotifyProc], [esi.VPD_Alt_NotifyRefData], \
		0>

	cCall	[esi.VPD_NotifyProc],<[esi.VPD_NotifyRefData],1>

VRR_Alt_Owner:
	ClrFlag	[esi.VPD_Flags],VPDF_AltOwnerActive

VRR_Done:
	cmp	[esi.VPD_NotifyProc], 0
	jnz	@F
	call	VPD_Disable_Trapping
@@:
	RestoreReg <ebx,edi,esi>
ifdef MAXDEBUG
	Trace_Out "Leaving"
endif
	LeaveProc
	return

EndProc VPD_Release_Resource

;******************************************************************************
;
; VPD_Add_Resource
;
;
; Decription: We verify that the port exists. Then we take limited actions
;	to add this port to our list of ports if not already present.
;
; Entry:
;	
; Exit:
;	0 if port not present, else non-zero
;==============================================================================
BeginProc VPD_Add_Resource, CCALL, PUBLIC, esp, RARE

ArgVar	FunctionCode, DWORD
ArgVar	hDevNode, DWORD
ArgVar	AllocBase, DWORD
ArgVar	AllocIRQ, DWORD

	EnterProc

	mov	edx, AllocBase
	inc	edx				; status port
	in	al, dx				; read status
	mov	ch, al
	inc	edx				; control port
	in	al, dx				; read control
	mov	cl, al
	xor	eax, eax
	cmp	cl, 0FFh			; Q: invalid ?
	jz	VARD_Done
	cmp	cl, ch
	jz	VARD_Done

	xor	ecx, ecx
	mov	eax, AllocBase

VARD_Loop:
	movzx	edx, WORD PTR [ecx*2+0408h]
	cmp	eax, edx			; Q: BDA know about it ?
	je	VARD_Done			;    Y: get out
	test	edx, edx			;    N: fill the area.
	jz	VARD_Fill_It
	test	dh, 0FFh
	jz	VARD_Done
	inc	cl
	cmp	cl, 2
	jbe	VARD_Loop
	jmp	VARD_Done

VARD_Fill_It:
	Trace_Out "VPD: Adding port #EAX to BDA"

	mov	WORD PTR [ecx*2+0408h], ax

VARD_Done:
	LeaveProc
	return

EndProc VPD_Add_Resource

;******************************************************************************
;
; VPD_Get_Contention_Handler
;
; Description:
;	Called to get the contention handler's address by VCOMM
;
; Entry:
;	EAX = GET_CONTENTION_HANDLER
; Exit:
;	VPD's Contention handler address
; Uses:
;	EAX,flags
;==============================================================================
BeginProc VPD_Get_Contention_Handler,PUBLIC

ifdef MAXDEBUG
	Trace_Out "VPD_GetContention_Handler"
endif
	mov	eax,OFFSET32 _VPD_Contention_Handler
	clc
	ret

EndProc VPD_Get_Contention_Handler

;******************************************************************************
;
; VPD_Contention_Handler
;
; Description:
;	Calls appropriate function handler
; Entry:
;	FunctionCode = function to call
; Exit:
;	That of Function
; Uses:
;	C style
;==============================================================================
BeginProc VPD_Contention_Handler,CCALL,esp,PUBLIC

ArgVar	FunctionCode,DWORD

	EnterProc
	mov	eax,FunctionCode
ifdef MAXDEBUG
	Trace_Out "VPD_Contention_Handler (function code #EAX)"
endif
	cmp	eax,ADD_RESOURCE
	ja	VCH_Done
	jmp	[Contention_Handler+eax*4]

VCH_Done:
	xor	eax,eax
ifdef MAXDEBUG
	Trace_Out "Leaving"
endif
	LeaveProc
	return

EndProc VPD_Contention_Handler

VxD_Pageable_Code_Ends

;******************************************************************************
;	       D E V I C E   C O N T R O L   H A N D L E R S
;******************************************************************************

VxD_CODE_SEG

;******************************************************************************
;
;   VPD_Set_Focus
;
;   DESCRIPTION:
;	This control call is only important if Sgrab support is enabled.
;	If Sgrab is enabled then this procedure will watch for display set-
;	focus calls to track the display owner.
;
;   ENTRY:
;	EBX = VM handle for new display focus
;
;   EXIT:
;	Carry clear (no error)
;
;   USES:
;	Flags
;
;==============================================================================

BeginProc VPD_Set_Focus

	cmp	[VPD_Sgrab_LPT], 0		    ; Q: Is Sgrab enabled?
	je	SHORT VPD_SF_Exit		    ;	 N: Then we don't care!
						    ;	 Y: Display may change
	test	edx, edx			    ; Q: Critical set-focus?
	jz	SHORT VPD_SF_Display_Changed	    ;	 Y: Set new Sgrab focus
	cmp	edx, VDD_Device_ID		    ;	 N: Q: VDD device?
	jne	SHORT VPD_SF_Exit		    ;	       N: Ignore it
VPD_SF_Display_Changed:
	mov	[VPD_Hw_Int_Owner], ebx 	    ; Ints go to display owner

VPD_SF_Exit:
	ret

EndProc VPD_Set_Focus


;******************************************************************************
;
;   VPD_VM_Not_Executeable
;
;   DESCRIPTION:
;	The "VM_Not_Executeable" control call indicates that the specified
;	VM will never run again.  This procedure releases ownership of any
;	LPT that the VM currently owns.
;
;   ENTRY:
;	EBX = Handle of terminated VM
;
;   EXIT:
;	Carry clear (no error)
;
;   USES:
;       Flags
;
;==============================================================================

BeginProc VPD_VM_Not_Executeable
ifdef MAXDEBUG
	Trace_Out "VPD_VM_Not_Executable (vm=#EBX)"
endif

	mov	ecx, VPD_Max_Virt_LPTs		; ECX = Max number of LPTs
	mov	esi, OFFSET32 VPD_LPT1		; ESI -> First LPT structure
VPD_Destroy_Loop:
	cmp	ebx, [esi.VPD_Owner]		; Q: Owned by dead VM?
	jne	SHORT VPD_Destroy_Next		;    N: Try the next one
	call	VPD_Detach			;    Y: Not anymore!
	cmp	[esi.VPD_NotifyProc],0
	je	VPD_Destroy_Next
	cCall	VPD_Acquire_Resource,<ACQUIRE_RESOURCE,esi, \
		[esi.VPD_Alt_NotifyProc], [esi.VPD_Alt_NotifyRefData], \
		0>

	cCall	[esi.VPD_NotifyProc],<[esi.VPD_NotifyRefData],1>

VPD_Destroy_Next:
	add	esi, SIZE VPD_Struc		; ESI -> Next LPT structure
	loopd	VPD_Destroy_Loop		; Loop for all LPTs

	ret

EndProc VPD_VM_Not_Executeable

if	WPS
;******************************************************************************
;
;   VJPD_System_Exit
;
;   DESCRIPTION:
;	The "System_Exit" control call indicates that Windows is exiting,
;	normally or via a crash.  Interrupts are enabled.
;
;	If a printer on any LPT port was left in Jumbo mode, switch it
;	back to PCL mode now.
;
;   ENTRY:
;	none
;
;   EXIT:
;	none
;
;   USES:
;	Flags
;
;==============================================================================

BeginProc VJPD_System_Exit
ifdef MAXDEBUG
	Trace_Out "VJPD_System_Exit"
endif

	mov	ecx, VPD_Max_Virt_LPTs 		; ECX = Max number of LPTs
	mov	esi, OFFSET32 VPD_LPT1 		; ESI -> First LPT structure

VJPD_System_Exit_Loop:
	bt	[esi.VPD_Flags], VJPDF_Jumbo_Bit
	jnc	SHORT VJPD_System_Exit_Next
	call	VJPD_Jumbo_PCL_Mode_Switch
VJPD_System_Exit_Next:
	add	esi, SIZE VPD_Struc		; ESI -> Next LPT structure
	loopd	VJPD_System_Exit_Loop		; Loop for all LPTs

	ret

EndProc VJPD_System_Exit

endif ; WPS

;******************************************************************************
;  A P I   H A N D L E R S   F O R   W I N D O W S   C O N T R O L   P A N E L
;			    A N D   S P O O L E R
;******************************************************************************

VxD_DATA_SEG

VPD_API_Call_Table LABEL DWORD
	dd	OFFSET32 VPD_PM_API_Get_Ver		; 0
	dd	OFFSET32 VPD_PM_API_Get_Port_Array	; 1
	dd	OFFSET32 VPD_PM_Get_Port_Behavior	; 2
	dd	OFFSET32 VPD_PM_Set_Port_Behavior	; 3
	dd	OFFSET32 VPD_PM_Aquire_Port		; 4
	dd	OFFSET32 VPD_PM_Release_Port		; 5

if	WPS
	dd	0					; 6
	dd	0					; 7
	dd	0					; 8
	dd	0					; 9
	dd	0					; 10
	dd	0					; 11
	dd	0					; 12
	dd	0					; 13
	dd	0					; 14
	dd	OFFSET32 VJPD_PM_SetNonWPSModeSwitch	; 15
	dd	OFFSET32 VJPD_PM_Jumbo_Acquire_Port	; 16
	dd	OFFSET32 VJPD_PM_Jumbo_Release_Port	; 17
	dd	OFFSET32 VJPD_PM_Get_True_Status	; 18
endif ; WPS


VPD_Max_API_Function = ($ - VPD_API_Call_Table) / 4 - 1

VPD_Jmp_Table	LABEL	DWORD
	dd	OFFSET32 VPD_Read_Phys_Port	; Read data port
	dd	OFFSET32 VPD_Out_Owner_Only	; Write data port
	dd	OFFSET32 VPD_Read_Status	; Read status port
	dd	OFFSET32 VPD_Out_Owner_Only	; Write status port
	dd	OFFSET32 VPD_Read_Phys_Port	; Read control port
	dd	OFFSET32 VPD_Out_Owner_Only	; Write control port

VxD_DATA_ENDS


;******************************************************************************
;
;   VPD_PM_API_Handler
;
;   DESCRIPTION:
;
;   ENTRY:
;	EBX = Handle of VM that called API
;	EBP -> Client register structure
;
;   EXIT:
;	Client registers and flags may be altered
;
;   USES:
;	All registers and flags
;
;==============================================================================

BeginProc VPD_PM_API_Handler

	VMMcall Test_Sys_VM_Handle		; Q: Called from Windows?
	jne	SHORT VPD_PM_API_Failed 	;    N: Fail it

	movzx	eax, [ebp.Client_DX]
	cmp	eax, VPD_Max_API_Function	; Q: Is this a valid function?
	ja	SHORT VPD_PM_API_Failed

if	WPS
        mov     ecx, VPD_API_Call_Table[eax*4]	; Q: is this an unused table entry
        jecxz   SHORT VPD_PM_API_Failed		;  Y: do not use it
endif ; WPS

	CallRet VPD_API_Call_Table[eax*4]

VPD_PM_API_Failed:
ifdef MAXDEBUG
	Trace_Out "Failed"
endif
	or	[ebp.Client_EFlags], CF_Mask
	ret

VPD_PM_API_Success:
ifdef MAXDEBUG
	Trace_Out "Success"
endif
	and	[ebp.Client_EFlags], NOT CF_Mask
	ret

EndProc VPD_PM_API_Handler


;******************************************************************************
;
;   VPD_PM_API_Get_Ver
;
;   DESCRIPTION:
;	Return the VPD version to Windows control panel.
;
;   ENTRY:
;	Client_DX = 0
;
;   EXIT:
;	Client_AX = 300h
;	Client_DX = 0CB01h
;	Client carry flag clear
;
;   USES:
;	EAX, Flags, Client_AX, Client_Flags
;
;==============================================================================

BeginProc VPD_PM_API_Get_Ver
ifdef MAXDEBUG
	Trace_Out "VPD_PM_API_Get_Ver"
endif

if	WPS
        mov     [ebp.Client_AX], 30Bh
        mov     [ebp.Client_DX], 0CB01h
else
	mov	[ebp.Client_AX], 300h
endif ; WPS

	jmp	VPD_PM_API_Success

EndProc VPD_PM_API_Get_Ver


;******************************************************************************
;
;   VPD_PM_API_Get_Port_Array
;
;   DESCRIPTION:
;	This API returns a bit array in the Client_AX register that indicates
;	which LPT ports exists.
;
;   ENTRY:
;	Client_DX = 1
;
;   EXIT:
;	Client_AX = Bit array of valid ports.
;	    Bit 0 = 1 if LPT1 exists
;	    Bit 1 = 1 if LPT2 exists
;	    Bit 2 = 1 if LPT3 exists
;	    etc.
;	Client carry flag clear (no error possible).
;
;   USES:
;	EAX, ECX, ESI, Flags, Client_AX, Client_Flags
;
;==============================================================================

BeginProc VPD_PM_API_Get_Port_Array

	mov	esi, OFFSET32 VPD_LPT1
	xor	eax, eax
	mov	ecx, eax
VPD_GPA_Loop:
	cmp	[esi.VPD_IO_Base], 0
	je	SHORT VPD_GPA_Next_LPT
	bts	eax, ecx
VPD_GPA_Next_LPT:
	add	esi, SIZE VPD_Struc
	inc	ecx
	cmp	ecx, VPD_Max_Virt_LPTs
	jb	VPD_GPA_Loop

	mov	[ebp.Client_AX], ax
ifdef MAXDEBUG
	Trace_Out "VPD_PM_API_Get_Port_Array (#EAX)"
endif
	jmp	VPD_PM_API_Success

EndProc VPD_PM_API_Get_Port_Array


;******************************************************************************
;
;   VPD_PM_Get_Port_Behavior
;
;   DESCRIPTION:
;	This API will return the current contention behavior value for the
;	specified LPT port.
;
;   ENTRY:
;	Client_DX = 2
;	Client_CX = Port index
;		    0 = LPT1
;		    1 = LPT2
;		    etc.
;
;   EXIT:
;	If client carry set then
;	    ERROR:  Port index is invalid
;	else
;	    Client_BX:Client_AX = Port behavior value
;		-1 = Always warn on contenetion (never auto assign)
;		0 = Never warn on contention (always auto assign)
;		Other = Number of milliseconds to delay before auto assign OK
;
;   USES:
;	EAX, ECX, Client_BX, Client_AX, Client_Flags,
;
;==============================================================================

BeginProc VPD_PM_Get_Port_Behavior
ifdef MAXDEBUG
	Trace_Out "VPD_PM_Get_Port_Behavior"
endif

	movzx	ecx, [ebp.Client_CX]
	cmp	ecx, VPD_Max_Virt_LPTs
	jae	VPD_PM_API_Failed

	imul	ecx, SIZE VPD_Struc
	add	ecx, OFFSET32 VPD_LPT1
	mov	eax, [ecx.VPD_Timeout_Limit]
	mov	[ebp.Client_AX], ax
	shr	eax, 16
	mov	[ebp.Client_BX], ax
	jmp	VPD_PM_API_Success

EndProc VPD_PM_Get_Port_Behavior

;******************************************************************************
;
;   VPD_PM_Set_Port_Behavior
;
;   DESCRIPTION:
;	This API will modify the contention behavior value for the sepecified
;	LPT port.
;
;   ENTRY:
;	Client_DX = 3
;	Client_CX = Port index
;		    0 = LPT1
;		    1 = LPT2
;		    etc.
;	Client_BX:Client_AX = Port behavior value
;	    -1 = Always warn on contenetion (never auto assign)
;	    0 = Never warn on contention (always auto assign)
;	    Other = Number of milliseconds to delay before auto assign OK
;
;   EXIT:
;	If client carry set then
;	    ERROR:  Port index is invalid
;	else
;	    Port behavior modified
;
;   USES:
;	EAX, ECX, Client_BX, Client_AX, Client_Flags,
;
;==============================================================================

BeginProc VPD_PM_Set_Port_Behavior
ifdef MAXDEBUG
	Trace_Out "VPD_PM_Set_Port_Behavior"
endif

	movzx	ecx, [ebp.Client_CX]
	cmp	ecx, VPD_Max_Virt_LPTs
	jae	VPD_PM_API_Failed

	imul	ecx, SIZE VPD_Struc
	add	ecx, OFFSET32 VPD_LPT1
	mov	ax, [ebp.Client_BX]
	shl	eax, 16
	mov	ax, [ebp.Client_AX]
	mov	[ecx.VPD_Timeout_Limit], eax
	jmp	VPD_PM_API_Success

EndProc VPD_PM_Set_Port_Behavior


;******************************************************************************
%OUT Document me!

BeginProc VPD_Acquire_Port

	jnz	SHORT VPD_AP_Force_Owner

ifdef MAXDEBUG
	Trace_Out "VPD_Acquire_Port"
endif
	mov	eax, [VPD_CB_Offset]
	add	eax, ebx			; EAX -> VPD control block data
	mov	ecx, [esi.VPD_LPT_Number]	; ECX = LPT number
	push	DWORD PTR [eax]
	bts	DWORD PTR [eax], ecx		; Fake contention
	call	VPD_Test_And_Set_Owner
	pop	DWORD PTR [eax]
	jc	VPD_AP_Done

; need-work, if the Test_And_Set_Owner above succeeds, VPD_Attach has already
; been called, this then falls through and calls it again. This path
; does not introduce any bugs, but it can cause redundant changing of the
; state of 'trapping', is that an efficiency problem ?

VPD_AP_Force_Owner:
ifdef MAXDEBUG
	Trace_Out "VPD_Acquire_Port, attaching"
endif
	xor	ecx, ecx
	dec	ecx				; "Special" ownership will turn
	call	VPD_Attach			; off trapping of ALL ports
	clc

VPD_AP_Done:
	ret

EndProc VPD_Acquire_Port

;******************************************************************************
;
;   VPD_Enable_Trapping
;
;   DESCRIPTION:
;
;   ENTRY:	    ESI -> VPD_Struc
;
;   EXIT:	    Carry set, if port isn't physical
;
;   USES:	    ECX, EDX, Flags
;
;==============================================================================
BeginProc VPD_Enable_Trapping

	mov	edx, [esi.VPD_IO_Base]
	cmp	edx, 2
	jb	short vet_exit
	mov	ecx, VPD_Num_Ports_Per_LPT
@@:
	VMMCall Enable_Global_Trapping
	inc	edx
	loop	@B

	or	[esi.VPD_Flags], VPDF_Enabled
	clc
vet_exit:
	ret

EndProc VPD_Enable_Trapping


;******************************************************************************
;
;   VPD_Disable_Trapping
;
;   DESCRIPTION:
;
;   ENTRY:	    ESI -> VPD_Struc
;
;   EXIT:	    Carry set, if port isn't physical
;
;   USES:	    ECX, EDX, Flags
;
;==============================================================================
BeginProc VPD_Disable_Trapping

	mov	edx, [esi.VPD_IO_Base]
	cmp	edx, 2
	jb	short vdt_exit
	mov	ecx, VPD_Num_Ports_Per_LPT
@@:
	VMMCall Disable_Global_Trapping
	inc	edx
	loop	@B

	and	[esi.VPD_Flags], NOT VPDF_Enabled
	clc
vdt_exit:
	ret

EndProc VPD_Disable_Trapping

;******************************************************************************
;
;   VPD_PM_Aquire_Port
;
;   DESCRIPTION:
;
;   ENTRY:
;	Client_DX = 4
;	Client_CX = Port index
;		    0 = LPT1
;		    1 = LPT2
;		    etc.
;	Client_AX = Flags
;		    Bit 0 = 1 if port should be assigned regardless of current
;			    ownership
;		    All other flags must be zero
;
;   EXIT:
;
;
;   USES:
;
;==============================================================================

BeginProc VPD_PM_Aquire_Port
ifdef MAXDEBUG
	Trace_Out "VPD_PM_Acquire_Port (vm=#EBX)"
endif

	movzx	ecx, [ebp.Client_CX]
	cmp	ecx, VPD_Max_Virt_LPTs
	jae	VPD_PM_API_Failed

	imul	ecx, SIZE VPD_Struc
	lea	esi, VPD_LPT1[ecx]

if	WPS
	TestMem	[esi.VPD_Flags], VJPDF_Jumbo
	jz	@F
	VMMCall	Get_Cur_VM_Handle
	cmp	[esi.VPD_Owner], ebx		; Q: Does this VM own it?
	je	VPD_PM_API_Success		;   Y: no switch needed
@@:
endif

	test	[ebp.Client_AL], 1
	call	VPD_Acquire_Port

	jnc	VPD_PM_API_Success
	jc	VPD_PM_API_Failed

EndProc VPD_PM_Aquire_Port


;******************************************************************************
;
;   VPD_PM_Release_Port
;
;   DESCRIPTION:
;
;   ENTRY:
;	Client_DX = 5
;	Client_CX = Port index
;		    0 = LPT1
;		    1 = LPT2
;		    etc.
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc VPD_PM_Release_Port
ifdef MAXDEBUG
	Trace_Out "VPD_PM_Release_Port (vm=#EBX)"
endif

	movzx	ecx, [ebp.Client_CX]
	cmp	ecx, VPD_Max_Virt_LPTs
	jae	VPD_PM_API_Failed

	imul	ecx, SIZE VPD_Struc
	lea	esi, VPD_LPT1[ecx]

	cmp	[esi.VPD_Owner], ebx
	jne	VPD_PM_API_Failed
	call	VPD_Detach
	jmp	VPD_PM_API_Success

EndProc VPD_PM_Release_Port

if	WPS
;******************************************************************************
;
;   VJPD_PM_SetNonWPSModeSwitch
;
;   DESCRIPTION:
;	Set the non-WPS mode switch string.
;
;   ENTRY:
;	EBX = Windows VM
;	Client_DX = 15
;	Client_CX = Port index
;		    0 = LPT1
;		    1 = LPT2
;		    etc.
;	Client_AX = Non-WPS mode switch string length
;	Client_ES:Client_DI = Non-WPS mode switch string
;   EXIT:
;	If client carry set then
;	    ERROR:  Port index is invalid, not a real port
;
;   USES:
;
;==============================================================================

BeginProc VJPD_PM_SetNonWPSModeSwitch

	movzx	ecx, [ebp.Client_CX]		    ; ECX = Port index
ifdef MAXDEBUG
	Trace_Out "VJPD_PM_SetNonWPSModeSwitch (VM=#EBX, LPT=#CX)"
endif
	cmp	ecx, VPD_Max_Virt_LPTs 	    	    ; Q: Port index valid?
	jae	VPD_PM_API_Failed			    ;	N: Return failure
						    ;	Y: Continue
	imul	ecx, SIZE VPD_Struc
	lea	esi, VPD_LPT1[ecx]		    ; ESI -> LPT structure

	movzx	ecx, [ebp.Client_AX]		    ; ECX = Mode switch size
	cmp	ecx, [esi.VJPD_Non_WPS_Mode_Size]   ; Q: Size changed?
	jne	SHORT @F			    ;	Y: Free the buffer
	or	ecx, ecx			    ; Q: Size zero?
	jz	VPD_PM_API_Success 		    ;	Y: Return success
	jmp	SHORT VJPD_SetNWMS_Copy 	    ;	N: Copy new mode switch
@@:
	mov	eax, [esi.VJPD_Non_WPS_Mode]	    ; EAX = Buffer address
	or	eax, eax			    ; Q: Buffer allocated?
	jz	SHORT @F			    ;	N: Don't free the buffer
	VMMCall _HeapFree, <EAX, 0>		    ;	Y: Free the buffer
	mov	[esi.VJPD_Non_WPS_Mode], 0	    ; Zero the buffer address
@@:
	mov	[esi.VJPD_Non_WPS_Mode_Size], ecx   ; Save new mode size
	or	ecx, ecx			    ; Q: Size zero?
	jz	VPD_PM_API_Success 		    ;	Y: Return success

	push	ecx				    ; Save buffer size
	VMMCall _HeapAllocate, <ECX, HeapZeroInit>  ; Allocate new buffer
	pop	ecx				    ; Restore buffer size
	or	eax, eax			    ; Q: Buffer OK?
	jnz	SHORT @F			    ;	Y: Continue
	mov	[esi.VJPD_Non_WPS_Mode_Size], 0     ;	N: Mark as no buffer
	jmp	VPD_PM_API_Failed			    ;	   and return failure
@@:
	mov	[esi.VJPD_Non_WPS_Mode], eax	    ; Save buffer address

	; Copy the new mode switch command

VJPD_SetNWMS_Copy:

ifdef MAXDEBUG
	Trace_Out "Copying non-WPS mode switch string"
endif
	mov	edi, [esi.VJPD_Non_WPS_Mode]	    ; EDI -> buffer

	mov	ax, (Client_ES SHL 8) + Client_DI   ; Map source address
	VMMCall Map_Flat
	cmp	eax, -1 			    ; Q: Mapping OK?
	jne	SHORT @F			    ;	Y: Continue
						    ;	N: Free the buffer
	mov	eax, [esi.VJPD_Non_WPS_Mode]	    ; EAX = Buffer address
	VMMCall _HeapFree, <EAX, 0>
	mov	[esi.VJPD_Non_WPS_Mode], 0	    ; Zero the buffer address
	mov	[esi.VJPD_Non_WPS_Mode_Size], 0     ; Zero the buffer size
	jmp	VPD_PM_API_Failed		    ; Return failure
@@:
	mov	esi, eax			    ; ESI = Source address
	cld					    ; Clear the direction
	rep	movsb				    ; Copy the string
	jmp	VPD_PM_API_Success 		    ; Return success

EndProc VJPD_PM_SetNonWPSModeSwitch

;******************************************************************************
;
;   VJPD_PM_Jumbo_Acquire_Port
;
;   DESCRIPTION:
;	Acquires the port for the Jumbo software ONLY if the port is
;	unowned or the owner has timed out.
;
;	DON'T succeed if Jumbo already owns the port, otherwise, two
;	Jumbo tasks (INSTALL and QP) may think they own the port, use
;	it, and call Jumbo_Release_Port so that neither owns it.
;
;   ENTRY:
;	Client_DX = 16
;	Client_CX = Port index
;		    0 = LPT1
;		    1 = LPT2
;		    etc.
;	Client_AX = 0
;
;   EXIT:
;	If client carry set then
;	    ERROR:	Port index is invalid, or
;			Port in use by someone else
;	else
;	    Port acquired
;
;
;   USES:
;
;==============================================================================

BeginProc VJPD_PM_Jumbo_Acquire_Port

	movzx	ecx, [ebp.Client_CX]
	cmp	ecx, VPD_Max_Virt_LPTs
	jae	VPD_PM_API_Failed

	imul	ecx, SIZE VPD_Struc
	lea	esi, VPD_LPT1[ecx]

ifdef MAXDEBUG
	mov	eax, [esi.VPD_Flags]
	Trace_Out "VJPD_PM_Jumbo_Acquire_Port (VM=#EBX, LPT=#ECX, Flags=#EAX)"
endif

	; If mode switch to PCL in progress, disallow this acquire
	TestMem	[esi.VPD_Flags], VJPDF_PCLSwitch
	jnz	VPD_PM_API_Failed

	; If COMM OR JUMBO(!!) already own it, fail
	TestMem	[esi.VPD_Flags], VPDF_Special_Owner
	jnz	VPD_PM_API_Failed

	; ensure that TSO will not go to user for contention
	cmp	[esi.VPD_Owner], 0		; Q: Does any VM own printer?
	je	SHORT VPD_Jumbo_OK_To_Assign	;    N: Then ok to acquire

	cmp	[esi.VPD_Timeout_Limit], -1	; Q: Can we time-out?
	je	SHORT VPD_Jumbo_OK_To_Assign	;    N: Then MUST ask user
						;    Y: See if it happened
	VMMCall Get_System_Time 		; EAX = Current system time
	sub	eax, [esi.VPD_Last_Touched]	; EAX = Elapsed time
	cmp	eax, [esi.VPD_Timeout_Limit]	; Q: Has owner timed-out?
	jb	VPD_PM_API_Failed		;    N: keep waiting
						;    Y: OK to change owners
VPD_Jumbo_OK_To_Assign:
	xor	al,al			; set Z => Don't force ownership
	or	[esi.VPD_Flags], VJPDF_JumboAcq
	call	VPD_Acquire_Port
	jnc	@F
	and	[esi.VPD_Flags], NOT VJPDF_JumboAcq
	jmp	VPD_PM_API_Failed
@@:
	and	[esi.VPD_Flags], NOT VJPDF_JumboAcq
	or	[esi.VPD_Flags], VJPDF_Jumbo
	jmp	VPD_PM_API_Success

EndProc VJPD_PM_Jumbo_Acquire_Port


;******************************************************************************
;
;   VJPD_PM_Jumbo_Release_Port
;
;   DESCRIPTION:
;	Give up Jumbo's ownership of a port, leaving the Jumbo_Owned
;	flag on to warn the next non-Jumbo owner that it has to switch
;	the printer back to PCL mode.
;
;   ENTRY:
;	Client_DX = 17
;	Client_CX = Port index
;		    0 = LPT1
;		    1 = LPT2
;		    etc.
;
;   EXIT:
;	If client carry set then
;	    ERROR:  Port index is invalid / port unacquired
;	else
;	    Port released
;
;   USES:
;
;==============================================================================

BeginProc VJPD_PM_Jumbo_Release_Port

	movzx	ecx, [ebp.Client_CX]
	cmp	ecx, VPD_Max_Virt_LPTs
	jae	VPD_PM_API_Failed

	imul	ecx, SIZE VPD_Struc
	lea	esi, VPD_LPT1[ecx]

ifdef MAXDEBUG
	mov	eax, [esi.VPD_Flags]
	Trace_Out "VJPD_PM_Jumbo_Release_Port (VM=#EBX, LPT=#ECX, Flags=#EAX)"
endif

	TestMem	[esi.VPD_Flags], VJPDF_Jumbo
	jz	VPD_PM_API_Failed

	; need-work, while jumbo owns port, VPD_PM_Acquire_Port & Release
	; are executed causing the Special owner bit and actual ownership
	; to be lost
	; test	[esi.VPD_Flags], VPDF_Special_Owner
	; jz	VPD_PM_API_Failed

	call	VPD_Detach
	jmp	VPD_PM_API_Success

EndProc VJPD_PM_Jumbo_Release_Port


;******************************************************************************
;
;   VJPD_PM_Get_True_Status
;
;   DESCRIPTION:
;	Reads the status register without affecting ownership in any way.
;
;   ENTRY:
;	EBX = Windows VM
;	Client_DX = 18
;	Client_CX = Port index
;		    0 = LPT1
;		    1 = LPT2
;		    etc.
;
;   EXIT:
;	If client carry set then
;	    ERROR:  Port index is invalid, not a real port
;	else
;	    Client_AX = True Status of LPT Port
;
;   USES:
;
;==============================================================================

BeginProc VJPD_PM_Get_True_Status
	movzx	ecx, [ebp.Client_CX]
ifdef MAXDEBUG
	Trace_Out "VJPD_PM_Get_True_Status (VM=#EBX, LPT=#CX)"
endif
	cmp	ecx, VPD_Max_Virt_LPTs
	jae	VPD_PM_API_Failed

	imul	ecx, SIZE VPD_Struc
	lea	esi, VPD_LPT1[ecx]

	mov	edx, [esi.VPD_IO_Base]
	test	dh, dh
	jnz	SHORT VJPD_GTS_Physical_Port
	jmp	VPD_PM_API_Failed

VJPD_GTS_Physical_Port:
	inc	dx
	in	al, dx
	xor	ah, ah
	mov	[ebp.Client_AX], ax
	jmp	VPD_PM_API_Success

EndProc VJPD_PM_Get_True_Status

endif ; WPS

;******************************************************************************
;		    I / O   T R A P   P R O C E D U R E S
;******************************************************************************

;******************************************************************************
;
;   VPD_LPTx_Trap
;
;   DESCRIPTON:
;	All printer port traps use the same code.  These entry points set
;	ESI to point to the appropriate LPT data structure.  They then jump
;	to a common I/O trap handler.
;
;   ENTRY:
;	If output then EAX/AX/AL = Data output
;	EBX = Current VM Handle
;	ECX = Type of I/O
;	EDX = Port number
;
;   EXIT:
;	If input then AL = Virtual port value
;
;   USES:
;	EAX, ESI, EDI, Flags
;
;==============================================================================

BeginProc VPD_LPT1_Trap, High_Freq

	mov	esi, OFFSET32 VPD_LPT1
	jmp	SHORT VPD_Common_IO_Trap

EndProc VPD_LPT1_Trap, High_Freq

;------------------------------------------------------------------------------

BeginProc VPD_LPT2_Trap, High_Freq

	mov	esi, OFFSET32 VPD_LPT2
	jmp	SHORT VPD_Common_IO_Trap

EndProc VPD_LPT2_Trap

;------------------------------------------------------------------------------

BeginProc VPD_LPT3_Trap, High_Freq

	mov	esi, OFFSET32 VPD_LPT3
	jmp	SHORT VPD_Common_IO_Trap

EndProc VPD_LPT3_Trap, High_Freq


;******************************************************************************
;
;   VPD_Common_IO_Trap
;
;   DESCRIPTION:
;	This procedure dispatches I/O traps for every I/O port of every
;	LPT.  It jumps through the VPD_Jmp_Table using the following
;	formula to determine which procedure should handle the trap:
;	    (I/O Port MOD 4) * 8 + I/O type
;
;	All non-byte I/O instructions are emulated by the "Emulate_Non_-
;	Byte_IO" macro which will recursively call the trap procedures
;	with simpler I/O types.  See the documentation on I/O trapping
;	for more details about this macro.
;
;   ENTRY:
;	If output then EAX/AX/AL = Data output
;	EBX = Current VM Handle
;	ECX = Type of I/O
;	EDX = Port number
;	ESI -> LPT data structure
;
;   EXIT:
;	If input then AL = Virtual port value
;
;   USES:
;	EAX, EDI, Flags
;
;==============================================================================

;------------------------------------------------------------------------------

;
;   A few paranoid MASM tests here.  This procedure relys on the fact that
;   byte input is 0 and byte output is 4.  All non-byte I/O requests are
;   handled by the "Emulate_Non_Byte_IO" macro (which will recursively
;   call this procedure).
;
.ERRNZ	Byte_Input
.ERRNZ	Byte_Output - 4

BeginProc VPD_Common_IO_Trap, High_Freq

	Emulate_Non_Byte_IO			; Non-byte I/O emulated

	mov	edi, edx			; EDI = trapped port
	and	edi, 11b			; Only lowest two bits matter
	CallRet VPD_Jmp_Table[edi*8][ecx]	; Call appropriate handler and
						; return
EndProc VPD_Common_IO_Trap


;******************************************************************************
;
;   VPD_Read_Phys_Port
;
;   DESCRIPTION:
;	The VM is reading either the data or control port.  We allow all VMs
;	to read from the physical port regardless of ownership.
;
;   ENTRY:
;	EDX = I/O port
;
;   EXIT:
;	AL = Data read
;
;   USES:
;	AL
;
;==============================================================================

BeginProc VPD_Read_Phys_Port

	in	al, dx				; Read port into AL
	ret

EndProc VPD_Read_Phys_Port


;******************************************************************************
;
;   VPD_Out_Owner_Only
;
;   DESCRIPTION:
;	The VM is writing data to the printer data or control port.  If the
;	printer is owned by another virtual machine then throw the I/O away.
;	Otherwise, output the data to the physical port.
;
;   ENTRY:
;	 AL = Byte output
;	EDX = I/O port
;
;   EXIT:
;	None
;
;   USES:
;	Flags
;
;==============================================================================

BeginProc VPD_Out_Owner_Only, High_Freq

	call	VPD_Test_And_Set_Owner		; Q: Does current VM own port?
	jc	SHORT VPD_OD_Exit		;    N: Ignore output
	out	dx, al				;    Y: Output byte to port
VPD_OD_Exit:					;	and return
	ret

EndProc VPD_Out_Owner_Only


;******************************************************************************
;
;   VPD_Read_Status
;
;   DESCRIPTION:
;	If the current VM owns the LPT or no VM owns the LPT then the actual
;	status is returned.  Otherwise, a "busy" status will be returned.
;
;   ENTRY:
;	EDX = I/O port
;
;   EXIT:
;	AL = Status
;
;   USES:
;	AL, Flags
;
;==============================================================================

BeginProc VPD_Read_Status

	in	al, dx				; Read the status into AL
	cmp	[esi.VPD_Owner], 0		; Q: Does no VM own LPT?
	jz	SHORT VPD_RS_Exit		;    Y: Return status in AL
	call	VPD_Test_And_Set_Owner		; Q: Does current VM own LPT?
	jnc	SHORT VPD_RS_Exit		;    Y: Return status in AL
	mov	al, VPD_Busy_Status		;    N: Status = none connected
VPD_RS_Exit:
	ret

EndProc VPD_Read_Status


;******************************************************************************
;      S O F T W A R E	 I N T E R R U P T   H O O K   P R O C E D U R E S
;******************************************************************************

;******************************************************************************
;
;   VPD_Int_17h
;
;   DESCRIPTION:
;	This procedure monitors the BIOS printer interrupt service and will
;	return a busy error status if the printer is not owned by the current
;	VM.
;
;   ENTRY:
;	EAX = 17h (interrupt #)
;	EBX = Current VM handle
;
;   EXIT:
;	If carry clear then interrupt eaten (error status returned in AH)
;	else interrupt is reflected to current VM
;
;   USES:
;	ECX, ESI, Client_AH, Flags
;
;==============================================================================

%OUT Equates for this procedure

BeginProc VPD_Int_17h, High_Freq

	Assert_Cur_VM_Handle ebx

	cmp	[ebp.Client_AH], 2		; Q: Get status or unknown?
	jae	SHORT VPD_I17_Reflect		;    Y: Just reflect it
						;    N: Check for contention
	movzx	esi, [ebp.Client_DX]		; ESI = Port # (0-3)
	cmp	esi, VPD_Max_Virt_LPTs-1	; Q: Valid port?
	ja	SHORT VPD_I17_Reflect		;    N: Just let it go
	imul	esi, SIZE VPD_Struc		; ESI = Offset in LPT list
	add	esi, OFFSET32 VPD_LPT1		; ESI -> LPT data structure

	TestMem	[esi.VPD_Flags], VPDF_Enabled	; Q: Trapping on this port ?
	jz	SHORT VPD_I17_Reflect		;    N: just reflect it.

	call	VPD_Test_And_Set_Owner		; Q: Does this VM own it?
	jnc	SHORT VPD_I17_Test_Connected	;    Y: Make sure OK to print

VPD_I17_Fail:
	mov	[ebp.Client_AH], 00100001b	; Time-out, Out of paper
	clc					; Eat the interrupt
	ret					; (The VM will never see it)

;
;   If this is function call 0 then make sure the printer is connected
;   before reflecting it to the BIOS to prevent time-out hang of doom.
;
VPD_I17_Test_Connected:
	cmp	[ebp.Client_AH], 0		; Q: Is this a print char call?
	jne	SHORT VPD_I17_Reflect		;    N: Reflect init calls

	TestMem	[esi.VPD_Flags], VPDF_Is_Connected ; Q: Detected printer yet?
	jnz	SHORT VPD_I17_Reflect		   ;	Y: Just reflect it

	VMMcall Begin_Nest_V86_Exec		; Prepare to reflect int
	mov	ecx, [ebp.Client_EAX]		; ECX = Caller's EAX
	mov	[ebp.Client_AH], 02h		; Read status command
	VMMcall Exec_Int			; Relfect Int 17h (in EAX)
	xchg	ecx, [ebp.Client_EAX]		; Get client EAX/Restore old
	VMMcall End_Nest_Exec			; End of nested exectuion
	test	ch, 10000000b			; Q: Is printer NOT busy
	jz	SHORT VPD_I17_Fail		;    N: Time-out RIGHT NOW!
	test	ch, 00100000b			;    Y: Q: Any paper in it?
	jnz	SHORT VPD_I17_Fail		;	   N: Error
	or	[esi.VPD_Flags], VPDF_Is_Connected ;	   Y: Set flag

;
;   Reflect this int 17h to the virtual machine.
;
VPD_I17_Reflect:
	stc
	ret

EndProc VPD_Int_17h


;******************************************************************************
;	  H A R D W A R E   I N T E R R U P T	P R O C E D U R E S
;******************************************************************************

;******************************************************************************
;
;   VPD_Hw_Int
;
;   DESCRIPTION:
;	This procedure is called whenever a hardware interrupt occurs on
;	IRQ 7.	Note that this procedure will not be called unless Sgrab
;	support is enabled since VPD will only virtualize the printer
;	interrupt if Sgrab is enabled.
;
;	VPD_Hw_Int sets the interrupt request for the virtual printer IRQ
;	line for the VM that is the current display owner.  Note that
;	this procedure does NOT service the interrupting hardware.  It is
;	assumed that the Sgrab code running in the virtual machine will
;	service the interrupt and issue an End Of Interrupt (EOI).
;
;	Note that this procedure is delared in a LOCKED code segment.  All
;	hardware interrupt handlers must be in locked memory.  VPD_Virt_EOI,
;	however, does not need to be in locked memory since it is called
;	when the VM issues an EOI.  Therefore, the virtual EOI is synchronous
;	with Windows/386.  In other words, a hardware interrupt may happen
;	in the middle of a VxD operation but a virtual EOI can not.
;
;   ENTRY:
;	EAX = IRQ handle
;	EBX = Current VM handle
;
;   EXIT:
;	None
;
;   USES:
;	EBX, Flags
;
;==============================================================================

BeginProc VPD_Hw_Int

	mov	ebx, [VPD_Hw_Int_Owner] 	; Reflect interrupt to this VM
	VxDjmp	VPICD_Set_Int_Request		; Set int request and return

EndProc VPD_Hw_Int



;------------------------------------------------------------------------------

;******************************************************************************
;
;   VPD_Virt_EOI
;
;   DESCRIPTION:
;	This procedure is called whenever a VM issues an End Of Interrupt for
;	IRQ 7.	Note that this procedure will not be called unless Sgrab
;	support is enabled since VPD will only virtualize the printer
;	interrupt if Sgrab is enabled.
;
;	At this point the Virtual Machine has serviced the simulated hardware
;	interrupt and is signaling the Virtual PIC Device that it is ready
;	to receive another interrupt.  This procedure lowers the virtual
;	interrupt request to prevent simulating another unwanted interrupt
;	and then issues an End Of Interrupt call for the PHYSICAL hardware
;	interrupt.
;
;   ENTRY:
;	EAX = IRQ handle
;	EBX = VM handle
;
;   EXIT:
;	None
;
;   USES:
;	EBX, Flags
;
;==============================================================================

BeginProc VPD_Virt_EOI

	VxDCall VPICD_Clear_Int_Request 	; Clear virtual IRQ request
	VxDjmp	VPICD_Phys_EOI			; Physical end of interrupt

EndProc VPD_Virt_EOI


;******************************************************************************
;			L O C A L   P R O C E D U R E S
;******************************************************************************

;******************************************************************************
;
;   VPD_Detach
;
;   DESCRIPTION:
;	Detaches the specified LPT from its current owner.  If the LPT is
;	currently owned and is a hardware (non-network) printer port then
;	this procedure will re-enable trapping of the I/O ports
;
;   ENTRY:
;	ESI -> VPD data structure
;
;   EXIT:
;	None
;
;   USES:
;       Flags
;
;==============================================================================

BeginProc VPD_Detach

	pushad
ifdef MAXDEBUG
	Trace_Out "VPD_Detach (VM=#EBX)"
endif

	and	[esi.VPD_Flags], NOT VPDF_Special_Owner

	mov	ebx, [esi.VPD_Owner]		; EBX = Handle of owner
	test	ebx, ebx			; Q: Does any VM own printer?
	jz	SHORT VPD_D_Exit		;    N: Do nothing!
						;    Y: Detach it
	mov	edx, [esi.VPD_IO_Base]		; EDX = Base I/O port
	test	dh, dh				; Q: Physical LPT?
	jz	SHORT VPD_D_Exit		;    N: Done!
						;    Y: Enable port trapping
	mov	ecx, VPD_Num_Ports_Per_LPT	; ECX = # I/O ports per LPT
VPD_D_Loop:
	VMMCall Enable_Local_Trapping		; Enable trapping for port EDX
	inc	edx				; Bump to next port
	loopd	VPD_D_Loop			; Loop for all LPT I/O ports

	mov	[esi.VPD_Owner], 0		; LPT is not owned

VPD_D_Exit:
	popad
	ret

EndProc VPD_Detach


;******************************************************************************
;
;   VPD_Attach
;
;   DESCRIPTION:
;	Assigns ownership of a LPT port to a VM.  When a VM owns a physical
;	(non-network) LPT we disable trapping of the status and control ports
;	to avoid the overhead of port traps.  We continue to trap the data
;	port so that we can time-out printer ownership.
;
;   ENTRY:
;	EBX = Handle of VM to own LPT
;	ESI -> VPD data structure
;	ECX = 0 if normal attach (LPT should time out)
;	    != 0 if special owner (tells us when port free)
;
;   EXIT:
;	None
;
;   USES:
;       Flags
;
;==============================================================================

BeginProc VPD_Attach

	Assert_VM_Handle ebx			; Paranoia...
ifdef MAXDEBUG
	Trace_Out "VPD_Attach (VM=#EBX)"
endif

	pushad

	mov	edx, [esi.VPD_IO_Base]		; EDX = Base I/O port
	test	dh, dh				; Q: Phys LPT (has I/O ports)?
	jz	SHORT VPD_Attach_Exit		;    N: Done
						;    Y: Disable port traps

	call	VPD_Detach			; Detach from current owner
	mov	[esi.VPD_Owner], ebx		; Set new owner

if	WPS
;
; If previous owner was jumbo and new owner is not jumbo, perform mode switch
	TestMem	[esi.VPD_Flags], VJPDF_Jumbo
	jz	@F
	TestMem	[esi.VPD_Flags], VJPDF_JumboAcq
	jnz	@F

	; clear flag before switch to avoid multiple entries
	and	[esi.VPD_Flags], NOT VJPDF_Jumbo
	call	VJPD_Jumbo_PCL_Mode_Switch
	call    VJPD_Nulls_After_Mode_Switch
@@:
endif ; WPS

	jecxz	VPD_A_Reset_Contention
	or	[esi.VPD_Flags], VPDF_Special_Owner
VPD_A_Reset_Contention:

	mov	eax, [VPD_CB_Offset]
	add	eax, ebx			; EAX -> VPD control block data
	mov	edi, [esi.VPD_LPT_Number]	; EDI = LPT number
	btr	DWORD PTR [eax], edi		; Reset contention flag in VM

	jecxz	VPD_A_Disable_Last_2_Ports	; If special owner then
	VMMcall Disable_Local_Trapping		; Turn off data too
VPD_A_Disable_Last_2_Ports:
	inc	edx				; EDX = Status port
	VMMcall Disable_Local_Trapping		; Turn off trapping
	inc	edx				; EDX = Control port
	VMMcall Disable_Local_Trapping		; Turn off trapping

VPD_Attach_Exit:
	popad
	ret

EndProc VPD_Attach


;******************************************************************************
;
;   VPD_Test_And_Set_Owner
;
;   DESCRIPTION:
;       This procedure will test to see if the current VM owns the printer
;	port.  This procedure has several possible results:
;
;	If no VM owns the printer then the printer will be assigned to
;	    the current VM.
;	If another VM owns the printer but has timed-out the current VM
;	    will get ownersimp of the printer.
;	If another VM owns the printer and has not timed-out AND the current
;	    VM has never contended for the LPT port then the user will be
;	    presented with a contention dialogue box had he/she will be
;	    given the opportunity to select the VM that is allowed to use
;	    the printer.
;	If another VM owns the printer and has not timed-out and the current
;	    VM has previously contended for the printer then this procedure
;	    will return with carry set to indicate that an error status should
;	    be returned for the current VM.
;
;   ENTRY:
;	EBX = VM to assign printer to
;	ESI -> LPT data structure
;
;   EXIT:
;	Returns with carry clear if VM owns the printer port.  Returns with
;	carry flag set if error / dummy data should be returned.  If contention
;	event is generated then this procedure will not return.
;
;   USES:
;	Flags
;
;==============================================================================

BeginProc VPD_Test_And_Set_Owner, High_Freq
	push	eax				; We'll use EAX

	cmp	[esi.VPD_Owner], ebx		; Q: Does this VM own it?
	je	SHORT VPD_TASO_Assigned 	;    Y: Done!
ifdef MAXDEBUG
	Trace_Out "VPD_Test_And_Set_Owner (VM=#EBX)"
endif
	cmp	[esi.VPD_Owner], 0		; Q: Does any VM own printer?
	je	SHORT VPD_TASO_OK_To_Assign	;    N: Assign it to this VM
						;    Y: Test for time-out

	TestMem	[esi.VPD_Flags], VPDF_Special_Owner
	jnz	SHORT VPD_TASO_Contention

	cmp	[esi.VPD_Timeout_Limit], -1	; Q: Can we time-out?
	je	SHORT VPD_TASO_Contention	;    N: Skip this part
						;    Y: See if it happened
	VMMCall Get_System_Time 		; EAX = Current system time
	sub	eax, [esi.VPD_Last_Touched]	; EAX = Elapsed time
	cmp	eax, [esi.VPD_Timeout_Limit]	; Q: Has owner timed-out?
	jb	SHORT VPD_TASO_Contention	;    N: Resolve this contention
						;    Y: OK to change owners
VPD_TASO_OK_To_Assign:
	xor	ecx, ecx			; NOT a special owner
	call	VPD_Attach			; New LPT owner = VM in EBX

VPD_TASO_Assigned:
	VMMCall Get_System_Time 		; EAX = Current system time
	mov	[esi.VPD_Last_Touched], eax	; Save this for time-out
	pop	eax				; Restore EAX
	clc					; Return with carry clear
	ret					; (VM owns printer)

;
;   The printer is currently begin used by another VM.
;
VPD_TASO_Contention:
	pushad

ifdef MAXDEBUG
	Trace_Out "VPD_Test_And_Set_Owner, shell resolved contension"
endif
	mov	eax, [VPD_CB_Offset]
	add	eax, ebx			; EAX -> VPD control block data
	mov	ecx, [esi.VPD_LPT_Number]	; ECX = LPT number
	bts	DWORD PTR [eax], ecx		; Q: Has VM contended already?
	jc	SHORT VPD_TASO_Assign_Failed	;    Y: Don't ask user again
						;    N: Display a dialogue box
	mov	eax, [esi.VPD_Owner]		; EAX = Current owner
	push	esi				; EBX = Contender
	lea	esi, [esi.VPD_Name]		; ESI -> Device name ("LPTx")
	VxDcall SHELL_Resolve_Contention	; EBX = Contention winner
	pop	esi
	mov	eax, [esi.VPD_Owner]
	cmp	ebx, eax			; Q: Same owner?
	je	SHORT VPD_TASO_Assign_Failed	;    Y: VM lost contention

	mov	ecx, [esi.VPD_LPT_Number]	; ECX = LPT number
	add	eax, [VPD_CB_Offset]		; EAX -> Contention flags
	bts	DWORD PTR [eax], ecx		; Set contention bit

	xor	ecx, ecx			; NOT a special owner
	call	VPD_Attach
	popad
	jmp	VPD_TASO_Assigned

VPD_TASO_Assign_Failed:
	popad
	pop	eax
	stc
	ret

EndProc VPD_Test_And_Set_Owner

if	WPS
;******************************************************************************
;
;   VJPD_Jumbo_PCL_Mode_Switch
;
;   DESCRIPTION:
;	Sends the Jumbo GoToPCL command out the port.
;
;   ENTRY:
;	EBX = Handle of VM to own LPT
;	ESI -> LPT_Struct (Moved to EDI)
;
;   EXIT:
;	None
;
;   USES:
;	None(All Pushed).
;
;==============================================================================

BeginProc VJPD_Jumbo_PCL_Mode_Switch
	pushad

ifdef MAXDEBUG
	mov	eax, esi
	Trace_Out "VJPD_Jumbo_PCL_Mode_Switch (VM=#EBX, LPT Struc=#EAX)"
endif
;
; Save the LPT struct in EDI
; Get the string base address and the string size depending on WPS type
;
	mov	edi, esi

        bt      [edi.VPD_Flags], VJPDF_NONJumbo_Bit ; Q: Jumbo printer?
        jnc     SHORT VJPD_JPMS_Jumbo               ;   Y: Use Jumbo string
                                                    ;   N: Use non-WPS string

; non Jumbo
        mov     esi, [edi.VJPD_Non_WPS_Mode]        ; ESI -> mode switch
        mov     ecx, [edi.VJPD_Non_WPS_Mode_Size]   ; ECX = mode switch size
        jcxz    VJPD_JPMS_Exit                      ; Exit if zero length
        jmp     SHORT VJPD_JPMS_Send

VJPD_JPMS_Jumbo:
	lea	esi, VJPD_Jumbo_PCL_Mode	    ; ESI -> mode switch
	mov	ecx, [VJPD_Jumbo_PCL_Mode_Size]	    ; ECX = mode switch size
;
; Output the string
;
VJPD_JPMS_Send:
	call	VJPD_Send_LPT_String

VJPD_JPMS_Exit:
	popad
	ret
EndProc VJPD_Jumbo_PCL_Mode_Switch

;******************************************************************************
;
;   VJPD_Nulls_After_Mode_Switch
;
;   DESCRIPTION:
;	Sends 128 nulls out the port.	This covers up a
;	printer bug that eats data when printer first comes online
;
;   ENTRY:
;	EBX = Handle of VM to own LPT
;	ESI -> LPT_Struct (Moved to EDI)
;
;   EXIT:
;	None
;
;   USES:
;	None(All Pushed).
;
;==============================================================================

BeginProc VJPD_Nulls_After_Mode_Switch
	pushad
	mov	edi, esi
ifdef MAXDEBUG
	Trace_Out "VJPD_Nulls_After_Mode_Switch"
endif
	cld

;
; Wait for port to stabalize from mode switch before sending nulls
;
	or	[edi.VPD_Flags], VJPDF_PCLSwitch
	VMMcall	Begin_Nest_Exec
ifdef MAXDEBUG
	Trace_Out "In Nest Exec"
endif

        VMMcall Get_System_Time
	add	eax, VJPD_Hide_Time
	mov	esi, eax
@@:
	VMMcall	Resume_Exec
        VMMcall Get_System_Time
	cmp	eax, esi
	jc	@B
ifdef MAXDEBUG
	Trace_Out "Done hiding, start checking port"
endif

;
; Wait for mode switch
;
        VMMcall Get_System_Time
	add	eax, VJPD_Switch_Wait_Time
	mov	esi, eax

	mov	edx, [edi.VPD_IO_Base]		    ; EDX = Base I/O port
	inc	dx

;
; Loop till printer ready or timer expires
@@:
	VMMcall	Resume_Exec
        in      al, dx                              ; Read the status into AL
        and     al, VJPD_Status_Ready_Mask
        cmp     al, VJPD_Status_Ready               ; Q: Printer ready?
        je      SHORT VJPD_JPMS_Wait_Done

        VMMcall Get_System_Time
	cmp	eax, esi
	jc	@B

VJPD_JPMS_Wait_Done:
        VMMcall End_Nest_Exec
	and	[edi.VPD_Flags], NOT VJPDF_PCLSwitch
ifdef MAXDEBUG
	Trace_Out "Mode switch complete, now send nulls"
endif

;
; Acorn does not need the Nulls, just the wait
;
	bt      [edi.VPD_Flags], VJPDF_NONJumbo_Bit
	jc      SHORT VJPD_SNL_Exit
;
; Set the LPT mode switch timeout value (Timeout + Current_System_Time)
;
ifdef MAXDEBUG
	Trace_Out "Actually sending the nulls now"
endif
	VMMcall Get_System_Time
	add	eax, [VJPD_Send_Switch_Limit]
	mov	[edi.VJPD_Send_Switch_Timeout], eax
;
; Get the LPT Base I/O Port Address
;
	mov	edx, [edi.VPD_IO_Base]
	in	al, dx
	mov	ah, al

	mov     ecx, [VJPD_Null_Size]

VJPD_SNL_Start:
;
; Get the next character to send
;
	xor	eax, eax
	call	VJPD_Send_LPT_Char
	jc	SHORT VJPD_SNL_Done		; If timeout
	loop	VJPD_SNL_Start
	clc
;
; Done with the string
;
VJPD_SNL_Done:
	mov	al, ah
	out	dx, al
VJPD_SNL_Exit:
ifdef MAXDEBUG
	Trace_Out "VJPD_Nulls_After_Mode_Switch exit"
endif
	popad
	ret

EndProc VJPD_Nulls_After_Mode_Switch

;******************************************************************************
;
;   VJPD_Send_LPT_String
;
;   DESCRIPTION:
;	Sends a string to an LPT port
;
;   ENTRY:
;	EBX = Handle of VM to own LPT
;	EDI -> LPT_Struct
;	ESI -> String to Send
;	ECX = Size of String
;
;   EXIT:
;	Carry if timed-out
;
;   USES:
;	None(All Pushed).
;
;==============================================================================


BeginProc VJPD_Send_LPT_String
	pushad
ifdef MAXDEBUG
	Trace_Out "VJPD_Send_LPT_String"
endif
	cld
;
; Set the LPT mode switch timeout value (Timeout + Current_System_Time)
;
	VMMcall Get_System_Time
	add	eax, [VJPD_Send_Switch_Limit]
	mov	[edi.VJPD_Send_Switch_Timeout], eax
;
; Get the LPT Base I/O Port Address
;
	mov	edx, [edi.VPD_IO_Base]
	in	al, dx
	mov	ah, al

VJPD_SLS_Start:
;
; Get the next character to send
;
	lodsb
	call	VJPD_Send_LPT_Char
	jc	SHORT VJPD_SLS_Exit		; If timeout
	loop	VJPD_SLS_Start
	clc
;
; Done with the string
;
VJPD_SLS_Exit:
	mov	al, ah
	out	dx, al
	popad
	ret
EndProc VJPD_Send_LPT_String

;******************************************************************************
;
;   VJPD_Send_LPT_Char
;
;   DESCRIPTION:
;	Sends a single char to an LPT port
;
;   ENTRY:
;	EAX = Value to Output
;	EBX = Handle of VM to own LPT
;	EDI -> LPT_Struct
;   EXIT:
;	Carry = Timeout
;   USES:
;	None(All Pushed).
;
;==============================================================================


BeginProc VJPD_Send_LPT_Char
	pushad

;
; Save the char to be output
;
	push	eax
;
; Get the base LPT I/O port
;
	mov	edx, [edi.VPD_IO_Base]
;
; Select the status line port
;
	inc	dx
;
; Check for a timeout
;
VJPD_SLC_Check_Timeout:
	VMMcall Get_System_Time
	cmp	[edi.VJPD_Send_Switch_Timeout], eax
	jbe	SHORT VJPD_SLC_Timeout
;
; Read the status port for "Not Busy"
;
VJPD_SLC_Read_Status:
	in	al, dx
	test	al, VJPD_Not_Busy
	jnz	SHORT VJPD_SLC_Send_Char
;
; Let any outstanding events be processed...
;
	VMMcall Begin_Nest_Exec
	VMMcall Resume_Exec
	VMMcall End_Nest_Exec
	jmp	SHORT VJPD_SLC_Check_Timeout
;
; Output the character to the LPT data port
;
VJPD_SLC_Send_Char:
	dec	dx
	pop	eax
	OutP	dx, al
;
; Select the control line port
;
	inc	dx
	inc	dx
;
; Set the data strobe control bit (Repeat for I/O bus delay)
;
	in	al, dx
        and     al, VJPD_Control_Mask
	or	al, VJPD_Data_Strobe
	OutP	dx, al
;
; Delay several cycles to allow for printer timing
;
	push	ecx
	mov	ecx, 100h
delay_loop:
	loop	SHORT delay_loop
	pop	ecx
;
; Clear the data strobe control line
;
	and	al, NOT VJPD_Data_Strobe
	OutP	dx, al
;
; Done with this character.
;
VJPD_SLC_Done:
	popad
	clc
	ret
;
; Timeout occurred!
;
VJPD_SLC_Timeout:
	pop	eax

ifdef MAXDEBUG
	Trace_Out "Send Char Timeout!"
endif
	popad
	stc
	ret
EndProc VJPD_Send_LPT_Char

endif ; WPS

;******************************************************************************
;		       D E B U G G I N G   C O D E
;******************************************************************************

;******************************************************************************
;
;   VPD_Debug_Query
;
;   DESCRIPTION:
;	This procedure is only assembled in the debug version of VPD.  It
;	will dump the status of each LPT port to the debugging terminal and
;	display the state of SGRAB ownership if SGRAB is enabled.
;
;	Note that since this procedure can be called at any time by the
;	debugger it is not allowed to call any non-asynchronous services and
;	it must be in a LOCKED code segment.
;
;   ENTRY:
;	EAX = Debug_Query (equate)
;
;   EXIT:
;	None
;
;   USES:
;	EBX, ECX, EDX, ESI, Flags
;
;==============================================================================

IFDEF DEBUG

BeginProc VPD_Debug_Query

	Trace_Out "LPT#   Type         Owner     Base Port"

	mov	ecx, 1				; ECX = LPT #
	mov	esi, OFFSET32 VPD_LPT1		; ESI -> First LPT structure
VPD_DQ_LPT_Loop:
	mov	ebx, [esi.VPD_Owner]		; EBX = VM handle of owner
	mov	edx, [esi.VPD_IO_Base]		; ESI = Base I/O port

	test	dh, dh				; Q: Port # > 0FFh?
	jz	SHORT VPD_DQ_Not_Hardware	;    N: Not a hardware port
						;    Y: Print hardware info
	Trace_Out " #CL   Hardware    #EBX      #DX"
	jmp	SHORT VPD_DQ_Next_LPT

VPD_DQ_Not_Hardware:				; Not a hardware LPT
	test	edx, edx			; Q: Does it even exist?
	jz	SHORT VPD_DQ_None		;    N: No LPT here
	Trace_Out " #CL   Network     #EBX"	;    Y: Network LPT
	jmp	SHORT VPD_DQ_Next_LPT
VPD_DQ_None:
	Trace_Out " #CL   (None)"
	jmp	SHORT VPD_DQ_Next_LPT

VPD_DQ_Next_LPT:
	add	esi, SIZE VPD_Struc		; ESI -> Next LPT data structure
	inc	ecx				; Inc counter
	cmp	ecx, VPD_Max_Virt_LPTs		; Q: End of list?
	jbe	VPD_DQ_LPT_Loop 		;    N: Loop

	Trace_Out " "

	mov	ecx, [VPD_Sgrab_LPT]		; Q: Is SGRAB enabled?
	jecxz	VPD_DQ_Exit			;    N: Skip this part
						;    Y: Report sgrab LPT number
	mov	ecx, [ecx.VPD_LPT_Number]
	Trace_Out "SGRAB LPT = #CL"
	Trace_Out " "

VPD_DQ_Exit:
	ret

EndProc VPD_Debug_Query

ENDIF

VxD_CODE_ENDS


	END VPD_Real_Mode_Init

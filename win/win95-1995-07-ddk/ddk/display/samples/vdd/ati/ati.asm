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

page		,132
title		ATI Mini-VDD Support Functions
.386p
;
;
.xlist
include 	VMM.INC
include 	MINIVDD.INC
include 	ATI.INC
include 	VESA.INC
.list
;
;
MACH32VESASupport	equ	0	;disable Mach32's VESA BIOS code
					;this should be enabled post Win '95
MACH64VGAVirt		equ	0	;disable Mach64's 4 plane VGA support
;
;
subttl		Virtual Device Declaration
page +
Declare_Virtual_Device	ATI,					\
			3,					\
			1,					\
			MiniVDD_Control,			\
			Undefined_Device_ID,			\
			VDD_Init_Order, 			\
			,					\
			,                                       \					
			,
                        
;
;
subttl		General Data Area
page +
VxD_DATA_SEG
ATISig			db	' 761295520'	;ATI signature at ROM offset 30H
ATISigLen		equ	$-OFFSET32 ATISig
;
public	WindowsVMHandle
WindowsVMHandle 	dd	?		;init'd at Device_Init
;
public	TotalMemorySize
TotalMemorySize 	dd	?		;init'd at Sys_Critical_Init
;
public	OurCBDataPointer
OurCBDataPointer	dd	?		;init'd at Device_Init
;
if MACH64VGAVirt
public	Mach64VDDBank, Mach64WriteBankSave, Mach64ReadBankSave
public	Mach64WriteLatchBankSave, Mach64ReadLatchBankSave
public	Mach64ConfigCtrlSave
Mach64VDDBank			dd	?		;
Mach64WriteBankSave		dd	-1		;
Mach64ReadBankSave		dd	?		;
Mach64WriteLatchBankSave	dd	?		;
Mach64ReadLatchBankSave 	dd	?		;
Mach64ConfigCtrlSave		db	?		;
Mach64LatchConfigCtrlSave	db	?		;
ALIGN 4
endif	;MACH64VGAVirt
;
public	DOSVMCount
DOSVMCount		dd	0		;
;
if MACH32VESASupport
public	VidMemLinearAddr
VidMemLinearAddr	dd	0		;linear address of A000:0H
;
public	VESAStubLinearAddr
VESAStubLinearAddr	dd	0		;
endif	;MACH32VESASupport
;
public	LinearSelector, MMIOSelector
LinearSelector		dd	0		;
MMIOSelector		dd	0		;
;
public	UltraROMSegment
UltraROMSegment 	dw	?		;
;
public	VDDBankLow, VDDBankHigh, BankRegSaveLow
public	BankRegSaveHigh, LatchSave2EH
public	LatchSave32H
VDDBankLow		db	?		;init'd at GetVDDBank
VDDBankHigh		db	?		;
BankRegSaveLow		db	0ffh		;this flag must be init'd!
BankRegSaveHigh 	db	?		;
LatchSave2EH		db	?		;
LatchSave32H		db	?		;
;
public	SuperVGAModeFlag
SuperVGAModeFlag	db	0		;FFH if display driver (ATI
						;HiRes) has registered
;
public	VDDPageOffsetFromBank
VDDPageOffsetFromBank	db	0		;
;
public	ChipID
ChipID			db	0		;init'd at Sys_Critical_Init
;
public	InCRTCModeChange
InCRTCModeChange	db	0		;
;
VxD_DATA_ENDS
;
;
subttl		Dynamic Device Initialization
page +
VxD_ICODE_SEG
;
;
public	MiniVDD_Dynamic_Init
BeginProc MiniVDD_Dynamic_Init
;
;Entry:
;	EBX contains the VM handle of the Windows VM.
;Exit:
;	If success, return NC.
;	If failure, return CY.
;
	cld				;set direction forward
	mov	WindowsVMHandle,ebx	;save the Windows VM handle
if MACH32VESASupport
;
;Get the linear address of physical A000:0H:
;
	VMMCall _MapPhysToLinear,<0a0000h,10000h,0>
	cmp	eax,-1			;was there an error?
	je	MVDI_ErrorExit		;yes, we can't run!
	mov	VidMemLinearAddr,eax	;everything's OK, save the linear addr
endif ;MACH32VESASupport
;
;First, see if we have a Mach64:
;
	mov	edx,42ech		;this is IO Scratchpad Register 0
	in	al,dx			;get first byte
	ror	eax,8			;
	inc	dl			;
	in	al,dx			;get second byte
	ror	eax,8			;
	inc	dl			;
	in	al,dx			;get third byte
	ror	eax,8			;
	inc	dl			;
	in	al,dx			;get fourth byte
	ror	eax,8			;now EAX has contents of complete port
	mov	ecx,eax 		;save it
;
	mov	dl,0ech 		;EDX --> IO Scratchpad Register 0
	mov	eax,0a55aa55ah		;write out a cute pattern
	out	dx,al			;write first byte
	ror	eax,8			;
	inc	dl			;
	out	dx,al			;write second byte
	ror	eax,8			;
	inc	dl			;
	out	dx,al			;write third byte
	ror	eax,8			;
	inc	dl			;
	out	dx,al			;write fourth byte
;
	mov	dl,0ech 		;EDX --> IO Scratchpad Register 0
	in	al,dx			;get first byte
	ror	eax,8			;
	inc	dl			;
	in	al,dx			;get second byte
	ror	eax,8			;
	inc	dl			;
	in	al,dx			;get third byte
	ror	eax,8			;
	inc	dl			;
	in	al,dx			;get fourth byte
	ror	eax,8			;now EAX has contents of complete port
;
	mov	dl,0ech 		;EDX --> IO Scratchpad Register 0
	xchg	ecx,eax 		;get previous value in of 42ECH in EAX
					;get readback value in ECX
	out	dx,al			;write first byte
	ror	eax,8			;
	inc	dl			;
	out	dx,al			;write second byte
	ror	eax,8			;
	inc	dl			;
	out	dx,al			;write third byte
	ror	eax,8			;
	inc	dl			;
	out	dx,al			;write fourth byte
	cmp	ecx,0a55aa55ah		;did we get back what we wrote?
	jne	MVDI_NotaMach64 	;nope, we don't have a Mach 64
;
;Second test:
;
	mov	edx,46ech		;this is IO Scratchpad Register 1
	in	al,dx			;get first byte
	ror	eax,8			;
	inc	dl			;
	in	al,dx			;get second byte
	ror	eax,8			;
	inc	dl			;
	in	al,dx			;get third byte
	ror	eax,8			;
	inc	dl			;
	in	al,dx			;get fourth byte
	ror	eax,8			;now EAX has contents of complete port
	mov	ecx,eax 		;save it
;
	mov	dl,0ech 		;EDX --> IO Scratchpad Register 1
	mov	eax,12345678h		;write out a cute pattern
	out	dx,al			;write first byte
	ror	eax,8			;
	inc	dl			;
	out	dx,al			;write second byte
	ror	eax,8			;
	inc	dl			;
	out	dx,al			;write third byte
	ror	eax,8			;
	inc	dl			;
	out	dx,al			;write fourth byte
;
	mov	dl,0ech 		;EDX --> IO Scratchpad Register 1
	in	al,dx			;get first byte
	ror	eax,8			;
	inc	dl			;
	in	al,dx			;get second byte
	ror	eax,8			;
	inc	dl			;
	in	al,dx			;get third byte
	ror	eax,8			;
	inc	dl			;
	in	al,dx			;get fourth byte
	ror	eax,8			;now EAX has contents of complete port
;
	mov	dl,0ech 		;EDX --> IO Scratchpad Register 1
	xchg	ecx,eax 		;get previous value in of 46ECH in EAX
					;get readback value in ECX
	out	dx,al			;write first byte
	ror	eax,8			;
	inc	dl			;
	out	dx,al			;write second byte
	ror	eax,8			;
	inc	dl			;
	out	dx,al			;write third byte
	ror	eax,8			;
	inc	dl			;
	out	dx,al			;write fourth byte
	cmp	ecx,12345678h		;did we get back what we wrote?
	jne	MVDI_NotaMach64 	;nope, we don't have a Mach 64
	mov	ChipID,64h		;we have a Mach64!
	jmp	MVDI_GotCardID		;
;
public	MVDI_NotAMach64
MVDI_NotAMach64:
;
;Detect a Mach8 or Mach32 chipset:
;
	mov	dx,42e8h		;this is 8514, Mach8, Mach32 register
	mov	ax,900fh		;perform a reset of the engine
	out	dx,ax			;
	call	Delay			;delay for a while
	mov	ax,400fh		;return engine to normal operation
	out	dx,ax			;
;
;Write bit pattern to register 92E8H and read it back.
;
	mov	dx,92e8h		;
	mov	ax,5555h		;
	out	dx,ax			;
	call	Delay			;delay for a while
	in	ax,dx			;
	cmp	ax,5555h		;
	jne	MVDI_NotMach8or32	;oops, not a Mach32 or Mach8!
;
; Write a different bit pattern to err_term register and read it back.
;
	mov	ax,0AAAAh		;
	out	dx,ax			;
	call	Delay			;delay for a while
	in	ax,dx			;
	cmp	ax,0AAAAh		;
	jne	MVDI_NotMach8or32	;oops, not a Mach32 or Mach8!
;
;We now are convinced that an 8514 (or compatible) exists.
;
	mov	dx,52eeh		;a register unique to Mach8 & Mach32
	in	ax,dx			;get and save original value
	ror	eax,16			;
	mov	ax,5555h		;write this pattern to the board
	out	dx,ax			;bits 7 and 15 must be zero.
	call	Delay			;delay for a while
	in	ax,dx			;
	ror	eax,16			;restore original value to register
	out	dx,ax			;
	ror	eax,16			;get value we just read for compare
	cmp	ax,5555h		;can we read back the value?
	jne	MVDI_NotMach8or32	;oops, not a Mach32 or Mach8!
;
;We have a Mach8 or a Mach32!  Now to differentiate between them....
;
	mov	ChipID,08H		;assume we have a Mach8
;
;Write to SRC_X register and read it back from R_SRC_X register.
;R_SRC_X does not exist on Mach8 hardware.
;
	mov	dx,8ee8h		;this is Src_X register
	mov	ax,0AAAAh		;write a patter
	out	dx,ax			;
	mov	dx,0daeeh		;switch to the R_Src_X register
	in	ax,dx			;try reading its value
	cmp	ax,02AAh		;is it valid?
	jne	MVDI_GotCardID		;nope, must be a Mach8
	mov	ChipID,32h		;indicate that we have a Mach32
	jmp	MVDI_GotCardID		;
;
public	MVDI_NotMach8or32
MVDI_NotMach8or32:
;
;We'd best try to detect an ATI VGA Wonder or other similar card.
;
;First, call _MapPhysToLinear to get the linear address of C0000H which is
;where the ATI ROM signature is:
;
	VMMCall _MapPhysToLinear,<0c0000h, 100h, 0>
	cmp	eax,-1			;is there no ROM there?
	je	MVDI_ErrorExit		;oops, no ROM!
	cld				;set direction forward
	mov	edi,eax 		;get ROM's linear addr in EDI
	add	edi,30h 		;EDI --> place where ROM sig is
	mov	esi,OFFSET32 ATISig	;ESI --> what string should be
	mov	ecx,ATISigLen		;ECX = number of bytes to compare
	repe	cmps byte ptr es:[edi],byte ptr [esi]
					;is it an ATI card?
	jnz	MVDI_ErrorExit		;it's not an ATI card
	cmp	byte ptr [eax+40h],'3'	;is the VGA Wonder product code there?
	jne	MVDI_ErrorExit		;nope, it's not a VGA Wonder
	push	ebp			;save EBP over this test
	push	ebx			;save EBX over this test
	mov	eax,1203h		;
	mov	ebx,5506h		;
	mov	ebp,0ffffh		;
	push	10h			;
	VMMCall Exec_VxD_Int		;
	cmp	bp,0ffffh		;did ROM BIOS do something to EBP?
	pop	ebx			;(restore EBX == Windows VM handle)
	pop	ebp			;(restore saved EBP)
	je	MVDI_ErrorExit		;nope, it's not a VGA Wonder
	mov	ChipID,0		;zero indicates a VGA Wonder
;
public	MVDI_GotCardID
MVDI_GotCardID:
;
;Next, we want to allocate space in the per-VM CB data structure.  This will
;be used by the mini-VDD to store per-VM data relating to states of registers.
;
	VMMCall _Allocate_Device_CB_Area,<<size PerVMData>,0>
	mov	OurCBDataPointer,eax	;save offset our our VxD's area
	or	eax,eax 		;was call sucessful?
	jz	MVDI_ErrorExit		;nope, leave fatally!
;
;The "master" VDD (VDD.386) contains all of the routines and procedures
;that are necessary to virtualize a standard VGA environment on high
;resolution super-VGA cards.  This "mini-VDD" provides some simple
;services which provide support which are peculiar to the chipset
;that we're providing support for.
;
;We must register the entry point addresses of our chipset-dependent
;routines with the "master" VDD.  To do this, we call a service in the
;"master" VDD, which returns the address of a dispatch table of the
;which is an array of the format:
;
;		dd	Address of support routine in this mini-VDD.
;			This address will be initialized to 0 by the "master"
;			VDD.  If we need to implement this functionality,
;			we fill in the address of our routine.
;
;Then, when the "master" VDD needs to call this function, it'll see if we've
;filled in the address field in for the function's entry and will call our
;routine if the address is filled in.  Otherwise, the VDD will skip the call
;and continue on its way without calling us.
;
;The following function calls the "master" VDD and returns a pointer to the
;dispatch table documented above in EDI and the number of functions
;supported in ECX. If the number of functions returned by the "master" VDD
;isn't equal to what we think it is, we return an error and don't allow
;Windows to continue running.
;
	VxDCall VDD_Get_Mini_Dispatch_Table
	cmp	ecx,NBR_MINI_VDD_FUNCTIONS	;perform a cursory version check
	jb	MVDI_ErrorExit			;oops, versions don't match!
;
public	MVDI_FillInTable
MVDI_FillInTable:
;
;Fill in the addresses of all the functions that we need to handle in this
;mini-VDD in the table provided by the "master" VDD whose address is
;contained in EDI.  Note that if you do not need to support a function,
;you do not need to fill in the dispatch table entry for that function.
;If you do not fill in an entry, the "master" VDD won't try to call
;this mini-VDD to support the function.  It'll just handle it in a
;default manner.
;
	cmp	ChipID,0		;got a Mach8, Mach32, or Mach64?
	je	MVDI_Common		;nope, skip these considerations
	MiniVDDDispatch GET_VDD_BANK,GetVDDBank
	MiniVDDDispatch PRE_INT_10_MODE_SET,PreInt10ModeSet
	cmp	ChipID,32h		;running on a Mach32?
	jb	MVDI_Common		;nope, Mach8 doesn't need these
	MiniVDDDispatch REGISTER_DISPLAY_DRIVER,RegisterDisplayDriver
	MiniVDDDispatch GET_BANK_SIZE,GetBankSize
	MiniVDDDispatch GET_TOTAL_VRAM_SIZE,GetTotalVRAMSize
	ja	MVDI_SetupMach64Routines;nope, go do Mach64 setup
	MiniVDDDispatch SET_VDD_BANK,SetVDDBank_32
	MiniVDDDispatch RESET_BANK,ResetBank_32
	MiniVDDDispatch SET_LATCH_BANK,SetLatchBank_32
	MiniVDDDispatch RESET_LATCH_BANK,ResetLatchBank_32
	MiniVDDDispatch MAKE_HARDWARE_NOT_BUSY,MakeHardwareNotBusy
if MACH32VESASupport
	MiniVDDDispatch VESA_SUPPORT,VESASupport
else	;MACH32VESASupport
	MiniVDDDispatch SET_BANK,SetBank_32
	MiniVDDDispatch GET_CURRENT_BANK_WRITE,GetCurrentBankWrite
	MiniVDDDispatch GET_CURRENT_BANK_READ,GetCurrentBankRead
	MiniVDDDispatch PRE_HIRES_SAVE_RESTORE,PreHiResSaveRestore_32
	MiniVDDDispatch POST_HIRES_SAVE_RESTORE,PostHiResSaveRestore_32
endif	;MACH32VESASupport
	MiniVDDDispatch CHECK_SCREEN_SWITCH_OK,CheckScreenSwitchOK
	MiniVDDDispatch CHECK_HIRES_MODE,CheckHiResMode
	jmp	MVDI_Common		;go do common processing
;
public	MVDI_SetupMach64Routines
MVDI_SetupMach64Routines:
if MACH64VGAVirt
	MiniVDDDispatch SET_VDD_BANK,SetVDDBank_64
	MiniVDDDispatch RESET_BANK,ResetBank_64
	MiniVDDDispatch SET_LATCH_BANK,SetLatchBank_64
	MiniVDDDispatch RESET_LATCH_BANK,ResetLatchBank_64
endif	;MACH64VGAVirt
	MiniVDDDispatch GET_CURRENT_BANK_WRITE,GetCurrentBankWrite
	MiniVDDDispatch GET_CURRENT_BANK_READ,GetCurrentBankRead
;
MVDI_Common:
	MiniVDDDispatch PRE_HIRES_TO_VGA,PreHiResToVGA
	MiniVDDDispatch POST_HIRES_TO_VGA,PostHiResToVGA
	MiniVDDDispatch PRE_VGA_TO_HIRES,PreVGAToHiRes
	MiniVDDDispatch POST_VGA_TO_HIRES,PostVGAToHiRes
	MiniVDDDispatch ENABLE_TRAPS,EnableTraps
	MiniVDDDispatch DISABLE_TRAPS,DisableTraps
	MiniVDDDispatch GET_CHIP_ID,GetChipID
;
public	MVDI_SetupPortTrapping
MVDI_SetupPortTrapping:
;
;Now comes the hard part (conceptually).  We must call the "master" VDD to
;setup port trapping for any port that the Windows display driver would
;write to (or read from) in order to draw onto the display when it's
;running in Windows Hi-Res mode.  For example, on the IBM 8514/A display
;card, the hardware BLTer is used to draw onto the screen.  Ports such as
;9AE8H, E2E8H, BEE8H are used on the 8514/A to perform the drawing.  The
;VDD "system" must set I/O port traps on these registers so that we are
;informed that the Windows display driver is writing or reading these
;ports.   Note that you should not set traps for the standard CRTC,
;Sequencer, or GCR ports (3D4H, 3C4H, 3CEH) as these are handled in a
;default manner by the "master" VDD.  Only set traps on those ports which
;the display driver would write or read in the process of drawing.
;You must also inform the "master" VDD whether these ports are byte or
;word sized so that it can virtualize them properly.  For example, port
;9AE8H can be accessed on the 8514/A as either a byte or word sized port
;so you'd register it as a word sized port with the "master" VDD.  However,
;port 9AE9H can only be accessed as a byte lengthed port so you must
;register it as a byte sized port with the "master" VDD.  Notice that you
;MUST register 9AE9H as a byte lengthed port even though you've already
;registered 9AE8H as a word lengthed port.
;
	cmp	ChipID,32h		;running on a Mach32?
	jne	MVDI_Virtualize1CEH	;nope, we don't need to do BLTer ports
;
	mov	edx,76EEh		;GE_PITCH
	mov	cl,WORD_LENGTHED	;as a word lengthed port
	VxDCall VDD_Register_Virtual_Port
;
	mov	edx,82E8h		;CUR_Y
	mov	cl,WORD_LENGTHED	;as a word lengthed port
	VxDCall VDD_Register_Virtual_Port
;
	mov	edx,82EEh               ;PATT_DATA_INDEX
	mov	cl,WORD_LENGTHED	;as a word lengthed port
	VxDCall VDD_Register_Virtual_Port
;
	mov	edx,86E8h               ;CUR_X
	mov	cl,WORD_LENGTHED	;as a word lengthed port
	VxDCall VDD_Register_Virtual_Port
;
	mov	edx,8AE8h               ;SRC_Y/DEST_Y
	mov	cl,WORD_LENGTHED	;as a word lengthed port
	VxDCall VDD_Register_Virtual_Port
;
	mov	edx,8EE8h               ;SRC_X/DEST_X
	mov	cl,WORD_LENGTHED	;as a word lengthed port
	VxDCall VDD_Register_Virtual_Port
;
	mov	edx,8EEEh               ;PATT_DATA
	mov	cl,WORD_LENGTHED	;as a word lengthed port
	VxDCall VDD_Register_Virtual_Port
;
	mov	edx,92E8h               ;ERR_TERM
	mov	cl,WORD_LENGTHED	;as a word lengthed port
	VxDCall VDD_Register_Virtual_Port
;
	mov	edx,9AEEh               ;LINEDRAW_INDEX
	mov	cl,WORD_LENGTHED	;as a word lengthed port
	VxDCall VDD_Register_Virtual_Port
;
	mov	edx,0A2E8h              ;BKGD_COLOR
	mov	cl,WORD_LENGTHED	;as a word lengthed port
	VxDCall VDD_Register_Virtual_Port
;
	mov	edx,0A2EEh              ;LINEDRAW_OPT
	mov	cl,WORD_LENGTHED	;as a word lengthed port
	VxDCall VDD_Register_Virtual_Port
;
	mov	edx,0A6E8h              ;FRGD_COLOR
	mov	cl,WORD_LENGTHED	;as a word lengthed port
	VxDCall VDD_Register_Virtual_Port
;
	mov	edx,0A6EEh              ;DEST_X_START
	mov	cl,WORD_LENGTHED	;as a word lengthed port
	VxDCall VDD_Register_Virtual_Port
;
	mov	edx,0AAEEh              ;DEST_X_END
	mov	cl,WORD_LENGTHED	;as a word lengthed port
	VxDCall VDD_Register_Virtual_Port
;
	mov	edx,0AEEEh              ;DEST_Y_END
	mov	cl,WORD_LENGTHED	;as a word lengthed port
	VxDCall VDD_Register_Virtual_Port
;
	mov	edx,0B2EEh              ;SRC_X_START
	mov	cl,WORD_LENGTHED	;as a word lengthed port
	VxDCall VDD_Register_Virtual_Port
;
	mov	edx,0BAEEh		;ALU_FG_FN
	mov	cl,WORD_LENGTHED	;as a word lengthed port
	VxDCall VDD_Register_Virtual_Port
;
	mov	edx,0BEE8h              ;SCISSOR...
	mov	cl,WORD_LENGTHED	;as a word lengthed port
	VxDCall VDD_Register_Virtual_Port
;
	mov	edx,0BEEEh              ;SRC_X_END
	mov	cl,WORD_LENGTHED	;as a word lengthed port
	VxDCall VDD_Register_Virtual_Port
;
	mov	edx,0C2EEh              ;SRC_Y_DIR
	mov	cl,WORD_LENGTHED	;as a word lengthed port
	VxDCall VDD_Register_Virtual_Port
;
	mov	edx,0CAEEh              ;SCAN_X
	mov	cl,WORD_LENGTHED	;as a word lengthed port
	VxDCall VDD_Register_Virtual_Port
;
	mov	edx,0CEEEh              ;DP_CONFIG
	mov	cl,WORD_LENGTHED	;as a word lengthed port
	VxDCall VDD_Register_Virtual_Port
;
	mov	edx,0D2EEh              ;PATT_LENGTH
	mov	cl,WORD_LENGTHED	;as a word lengthed port
	VxDCall VDD_Register_Virtual_Port
;
	mov	edx,0D6EEh              ;PATT_INDEX
	mov	cl,WORD_LENGTHED	;as a word lengthed port
	VxDCall VDD_Register_Virtual_Port
;
	mov	edx,0DAEEh              ;EXT_SCISSOR_L
	mov	cl,WORD_LENGTHED	;as a word lengthed port
	VxDCall VDD_Register_Virtual_Port
;
	mov	edx,0DEEEh              ;EXT_SCISSOR_T
	mov	cl,WORD_LENGTHED	;as a word lengthed port
	VxDCall VDD_Register_Virtual_Port
;
	mov	edx,0E2E8h              ;PIX_TRANS
	mov	cl,WORD_LENGTHED	;as a word lengthed port
	VxDCall VDD_Register_Virtual_Port
;
	mov	edx,0E2EEh		;EXT_SCISSOR_R
	mov	cl,WORD_LENGTHED	;as a word lengthed port
	VxDCall VDD_Register_Virtual_Port
;
	mov	edx,0E6EEh              ;EXT_SCISSOR_B
	mov	cl,WORD_LENGTHED	;as a word lengthed port
	VxDCall VDD_Register_Virtual_Port
;
	mov	edx,0FEEEh              ;LINEDRAW
	mov	cl,WORD_LENGTHED	;as a word lengthed port
	VxDCall VDD_Register_Virtual_Port
;
;We need to trap and get the state of all writes that a particular VM would
;do to ATI Mach32 Extension Register 4AEEH.  Bit 0 of this register determines
;whether we're in HiRes mode or not.  We use this to determine whether a
;particular VM is in an ATI Mach32 HiRes mode.
;
	mov	esi,OFFSET32 MiniVDD_Virtual4AEEH
	mov	edx,4aeeh		;
	VMMCall Install_IO_Handler	;
	jc	MVDI_ErrorExit		;oops, couldn't install procedure
	mov	edx,4aeeh		;
	VMMCall Enable_Global_Trapping	;always trap this
	mov	edx,4aefh		;
	VMMCall Install_IO_Handler	;
	jc	MVDI_ErrorExit		;oops, couldn't install procedure
	mov	edx,4aefh		;
	VMMCall Enable_Global_Trapping	;always trap this
;
;We need to trap and get the state of all writes that a particular VM would
;do to ATI Mach32 Extension Register 4AE8H.  Bit 0 of this register determines
;whether we're in HiRes mode or not.  We use this to determine whether
;a particular VM is in an ATI Mach32 HiRes mode.
;
	mov	esi,OFFSET32 MiniVDD_Virtual4AE8H
	mov	edx,4ae8h		;
	VMMCall Install_IO_Handler	;
	jc	MVDI_ErrorExit		;oops, couldn't install procedure
	mov	edx,4ae8h		;
	VMMCall Enable_Global_Trapping	;always trap this
	mov	edx,4ae9h		;
	VMMCall Install_IO_Handler	;
	jc	MVDI_ErrorExit		;oops, couldn't install procedure
	mov	edx,4ae9h		;
	VMMCall Enable_Global_Trapping	;always trap this
;
public	MVDI_InstallVESAStub
MVDI_InstallVESAStub:
if MACH32VESASupport
;
;Many VESA apps ignore the VESA spec and make far calls to INT 10H, function
;4F05H instead of doing the INT 10H.  Therefore, we have to place a stub in
;the virtual machine's code area which will translate this FAR call into
;a simple INT 10H, function 4F05H call.
;
;Now, go allocate some memory in the V86 area so we can place our banking stub
;there:
;
	VMMCall _Allocate_Global_V86_Data_Area,<VESA_STUB_SIZE, 0>
;
;The address that we get is a linear address.  We need to convert this to
;a V86 Segment:Offset address for the benefit of our real mode VESA apps:
;
	mov	edi,eax 		;we need this linear address for later
	mov	esi,eax 		;copy this for linear --> Seg:Off calc
	and	eax,0fh 		;get offset portion of address in EAX
	and	esi,NOT 0fh		;get segment portion of address in ESI
	shl	esi,12			;get segment portion into high word
	or	eax,esi 		;now EAX has Segment:Offset address
	mov	VESAStubLinearAddr,eax	;save return value
	or	eax,eax 		;did the call fail?
	jz	MVDI_Virtualize1CEH	;yes, but it's not fatal!
	mov	esi,OFFSET32 VESA_4F05_Stub
	mov	ecx,VESA_STUB_SIZE	;
	rep	movsb			;mov 'er in!
endif	;MACH32VESASupport
;
public	MVDI_Virtualize1CEH
MVDI_Virtualize1CEH:
;
;As we stated above, the registration of the virtualization of the above
;ports was done simply so that the VDD would "know" when the Windows
;display driver was accessing the hardware (so the VDD could do a context
;switch to the Windows VM when simultaneously running a windowed or
;background mode DOS VM).  The VDD will handle the virtualization of these
;ports in a default way such that the VDD will do a "context switch" to
;Windows whenever these ports are trapped and then perform the true port
;action (OUT, IN, REP OUTSB etc.) that the display driver intended.  However,
;some ports need to be truly virtualized by this mini-VDD because sometimes
;the action requested by the display card's ROM BIOS or the display driver
;must not take place.  Therefore, we must register our OWN port trapping
;routine and handle these ports locally in the mini-VDD.  An example of
;this is port 1CEH and 1CFH which are written to during ROM BIOS mode changes.
;If we're running a DOS VM in a window, we don't want to touch 1CEH or 1CFH
;just because the "virtual" DOS box is doing a mode change (we're running
;in a window so we really are "faking" the ROM BIOS's mode change).  Thus,
;we trap them at the appropriate times and "eat" the write to it if
;necessary.  See the code at MiniVDD_Virtual1CE for more details.
;Anyway, we set up a direct trapping procedure for these registers with
;the VMM and avoid any dealings with the "master" VDD for these special ports.
;
	mov	esi,OFFSET32 MiniVDD_Virtual1CE
	mov	edx,2e8h		;
	VMMCall Install_IO_Handler	;
	VMMCall Disable_Global_Trapping ;don't EVER set trapping on this

	mov	esi,OFFSET32 MiniVDD_Virtual1CE
	mov	edx,1ceh		;
	VMMCall Install_IO_Handler	;
	jc	MVDI_ErrorExit		;oops, couldn't install procedure
	VMMCall Disable_Global_Trapping ;don't trap yet, set it later
;
	mov	esi,OFFSET32 MiniVDD_Virtual1CE
	mov	edx,01cfh		;
	VMMCall Install_IO_Handler	;
	jc	MVDI_ErrorExit		;oops, couldn't install procedure
	VMMCall Disable_Global_Trapping ;don't trap yet, set it later
	mov	DOSVMCount,0		;make sure this is initialized
;
	cmp	ChipID,64h		;running on a Mach64?
	jne	MVDI_GetROMEntryPoint	;nope, continue
;
;We need to virtualize the Mach64 Setup and Control Registers so that BIOS
;mode changes in windowed VM's don't touch them:
;
	mov	esi,OFFSET32 MiniVDD_Virtual1CE
	mov	edx,4eech		;
	call	InstallDWordPort	;
	jc	MVDI_Exit		;oops, couldn't install procedure
;
	mov	esi,OFFSET32 MiniVDD_Virtual1CE
	mov	edx,52ech		;
	call	InstallDWordPort	;
	jc	MVDI_Exit		;oops, couldn't install procedure
;
	mov	esi,OFFSET32 MiniVDD_Virtual1CE
	mov	edx,56ech		;
	call	InstallDWordPort	;
	jc	MVDI_Exit		;oops, couldn't install procedure
;
	mov	esi,OFFSET32 MiniVDD_Virtual1CE
	mov	edx,5aech		;
	call	InstallDWordPort	;
	jc	MVDI_Exit		;oops, couldn't install procedure
;
	mov	esi,OFFSET32 MiniVDD_Virtual1CE
	mov	edx,66ech		;
	call	InstallDWordPort	;
	jc	MVDI_Exit		;oops, couldn't install procedure
;
	mov	esi,OFFSET32 MiniVDD_Virtual1CE
	mov	edx,6aech		;
	call	InstallDWordPort	;
	jc	MVDI_Exit		;oops, couldn't install procedure
;
	mov	esi,OFFSET32 MiniVDD_Virtual1CE
	mov	edx,7eech		;
	call	InstallDWordPort	;
	jc	MVDI_Exit		;oops, couldn't install procedure
;
	mov	esi,OFFSET32 MiniVDD_Virtual1CE
	mov	edx,6ech		;
	call	InstallDWordPort	;
	jc	MVDI_Exit		;oops, couldn't install procedure
;
	mov	esi,OFFSET32 MiniVDD_Virtual1CE
	mov	edx,0aech		;
	call	InstallDWordPort	;
	jc	MVDI_Exit		;oops, couldn't install procedure
;
	mov	esi,OFFSET32 MiniVDD_Virtual1CE
	mov	edx,0eech		;
	call	InstallDWordPort	;
	jc	MVDI_Exit		;oops, couldn't install procedure
;
	mov	esi,OFFSET32 MiniVDD_Virtual1CE
	mov	edx,12ech		;
	call	InstallDWordPort	;
	jc	MVDI_Exit		;oops, couldn't install procedure
;
	mov	esi,OFFSET32 MiniVDD_Virtual1CE
	mov	edx,16ech		;
	call	InstallDWordPort	;
	jc	MVDI_Exit		;oops, couldn't install procedure
;
	mov	esi,OFFSET32 MiniVDD_Virtual1CE
	mov	edx,1aech		;
	call	InstallDWordPort	;
	jc	MVDI_Exit		;oops, couldn't install procedure
;
	mov	esi,OFFSET32 MiniVDD_Virtual1CE
	mov	edx,1eech		;
	call	InstallDWordPort	;
	jc	MVDI_Exit		;oops, couldn't install procedure
;
	mov	esi,OFFSET32 MiniVDD_Virtual1CE
	mov	edx,22ech		;
	call	InstallDWordPort	;
	jc	MVDI_Exit		;oops, couldn't install procedure
;
	mov	esi,OFFSET32 MiniVDD_Virtual1CE
	mov	edx,26ech		;
	call	InstallDWordPort	;
	jc	MVDI_Exit		;oops, couldn't install procedure
;
	mov	esi,OFFSET32 MiniVDD_Virtual1CE
	mov	edx,2aech		;
	call	InstallDWordPort	;
	jc	MVDI_Exit		;oops, couldn't install procedure
;
	mov	esi,OFFSET32 MiniVDD_Virtual1CE
	mov	edx,2eech		;
	call	InstallDWordPort	;
	jc	MVDI_Exit		;oops, couldn't install procedure
;
	mov	esi,OFFSET32 MiniVDD_Virtual1CE
	mov	edx,32ech		;
	call	InstallDWordPort	;
	jc	MVDI_Exit		;oops, couldn't install procedure
;
	mov	esi,OFFSET32 MiniVDD_Virtual1CE
	mov	edx,36ech		;
	call	InstallDWordPort	;
	jc	MVDI_Exit		;oops, couldn't install procedure
;
	mov	esi,OFFSET32 MiniVDD_Virtual1CE
	mov	edx,3aech		;
	call	InstallDWordPort	;
	jc	MVDI_Exit		;oops, couldn't install procedure
;
	mov	esi,OFFSET32 MiniVDD_Virtual1CE
	mov	edx,3eech		;
	call	InstallDWordPort	;
	jc	MVDI_Exit		;oops, couldn't install procedure
;
	mov	esi,OFFSET32 MiniVDD_Virtual1CE
	mov	edx,4aech		;
	call	InstallDWordPort	;
	jc	MVDI_Exit		;oops, couldn't install procedure
;
	mov	esi,OFFSET32 MiniVDD_Virtual1CE
	mov	edx,5eech		;
	call	InstallDWordPort	;
	jc	MVDI_Exit		;oops, couldn't install procedure
;
	mov	esi,OFFSET32 MiniVDD_Virtual1CE
	mov	edx,62ech		;
	call	InstallDWordPort	;
	jc	MVDI_Exit		;oops, couldn't install procedure
;
public	MVDI_GetROMEntryPoint
MVDI_GetROMEntryPoint:
;
;We need to get the linear address of the ROM entry point which sets
;us into VGA passthrough mode.	The Windows driver will get us into
;HiRes mode, but we're responsible for getting ourselves out of HiRes
;mode and into VGA mode.
;
	cmp	ChipID,32h		;are we on a Mach32?
	jne	MVDI_GoodExit		;nope, Mach8 & 64 do it differently
;
;Get the value from register 52EEH which contains the base address of
;our ROM BIOS:
;
	mov	edx,52eeh		;
	in	al,dx			;get value from 52EEH
	and	eax,7fh 		;mask to get significant bits
	mov	edx,80h 		;multiply by 80H
	mul	edx			;
	add	eax,0c000h		;now EAX has the base segment
	mov	UltraROMSegment,ax	;save this for later use
;
MVDI_GoodExit:
	clc				;return success
	jmp	short MVDI_Exit 	;
;
MVDI_ErrorExit:
	stc
;
MVDI_Exit:
	ret
EndProc MiniVDD_Dynamic_Init
;
;
subttl		Utility Routine For Installing Mach64 DWord Ports
page +
public	InstallDWordPort
BeginProc InstallDWordPort
;
;Install I/O trapping for 4 ports beginning with the one in EDX.  Then,
;disable trapping for each of the 4 ports.  Returns CY if there's a problem.
;
	VMMCall Install_IO_Handler	;
	jc	IDPExit 		;oops, couldn't install procedure
	VMMCall Disable_Global_Trapping ;don't trap yet, set it later
	inc	edx			;bump to next port of the 4
;
	VMMCall Install_IO_Handler	;
	jc	IDPExit 		;oops, couldn't install procedure
	VMMCall Disable_Global_Trapping ;don't trap yet, set it later
	inc	edx			;bump to next port of the 4
;
	VMMCall Install_IO_Handler	;
	jc	IDPExit 		;oops, couldn't install procedure
	VMMCall Disable_Global_Trapping ;don't trap yet, set it later
	inc	edx			;bump to next port of the 4
;
	VMMCall Install_IO_Handler	;
	jc	IDPExit 		;oops, couldn't install procedure
	VMMCall Disable_Global_Trapping ;don't trap yet, set it later
	clc				;return good
;
IDPExit:
	ret				;
EndProc InstallDWordPort
;
;
subttl		Return ChipID To Main VDD
page +
public	MiniVDD_GetChipID
BeginProc MiniVDD_GetChipID
;
;Entry:
;	Nothing assumed.
;Exit:
;	EAX contains the ChipID.
;	Preserve ALL other registers.
;
;This routine is used by the Main VDD's Plug&Play code to determine whether
;a card has been changed since the last time that Windows was booted.  We
;are called to return to ChipID.  This assures us that Plug&Play will detect
;a different card, even if both cards use this same MiniVDD.  If the ChipID
;has changed, Plug&Play will get wind of it and will take appropriate action.
;
	movzx	eax,ChipID		;return ChipID in EAX
	ret				;
EndProc 	MiniVDD_GetChipID
;
;
subttl		Int 10H VESA Stub For Supporting Calls to 4F05H
page +
if MACH32VESASupport
public	VESA_4F05_Stub
VESA_4F05_Stub		proc	near
	mov	ah,dl			;get bank to set
	db	0bah,0ceh,01h		;this is MOV DX,1CEH
	or	bh,bh			;setting the bank?
	jz	VFS5SetBank		;yes, go do it
;
public	VFS5GetBank
VFS5GetBank:
	mov	al,0b2h 		;set to ATI register 32H
	out	dx,al			;
	inc	dl			;EDX --> ATI data register
	in	al,dx			;
	shr	al,1			;get banking bits into low nibble of AL
	and	al,0fh			;mask to get only banking bits
	mov	ah,al			;save this in AH
	dec	dl			;EDX --> ATI index register
	mov	al,0aeh 		;set to ATI register 2EH
	out	dx,al			;
	inc	dl			;EDX --> ATI data register
	in	al,dx			;
	and	al,03h			;mask to get only banking bits
	shl	al,4			;get them into high nibble of AL
	or	al,ah			;now AL has the current bank
	xor	ah,ah			;zero out AH
	db	89h,0c2h		;this is MOV DX,AX
	jmp	VFS5GoodExit		;and return success
;
public	VFS5SetBank
VFS5SetBank:
	push	eax			;save bank in AH
	and	ah,0fh			;mask to get only low nibble
	shl	ah,1			;get bank into place in AH
	mov	al,0b2h 		;set this into ATI register 32H
	db	0efh			;this is OUT DX,AX
	pop	eax			;restore bank to AH
;
	mov	al,0aeh 		;set to ATI register 2EH
	out	dx,al			;
	inc	dl			;EDX --> ATI data register
	in	al,dx			;get current value
	and	al,NOT 0fh		;get rid of banking bits in 2EH
	shr	ah,4			;get bank into place in AH
	or	al,ah			;now AL has correct value for 2EH
	out	dx,al			;
;
public	VFS5GoodExit
VFS5GoodExit:
	db	0b8h,4fh,00h		;this is MOV AX,004FH (success code)
	retf				;and do a far return to caller
VESA_4F05_Stub		endp
VESA_STUB_SIZE		equ	$ - OFFSET32 VESA_4F05_Stub
endif	;MACH32VESASupport
;
;
VxD_ICODE_ENDS
;
;
subttl		Dispatch Table for VMM Calling This Mini-VDD
page +
VxD_LOCKED_CODE_SEG
;
;
Begin_Control_Dispatch	MiniVDD
	Control_Dispatch Sys_Dynamic_Device_Init, MiniVDD_Dynamic_Init
	Control_Dispatch Device_Init,	     MiniVDD_Dynamic_Init
	Control_Dispatch Sys_VM_Terminate,   MiniVDD_Sys_VM_Terminate
	Control_Dispatch Create_VM,	     MiniVDD_Create_VM
	Control_Dispatch Destroy_VM,	     MiniVDD_Destroy_VM
End_Control_Dispatch MiniVDD
;
;
subttl		Windows VM Termination Handler
page +
public	MiniVDD_Sys_VM_Terminate
BeginProc MiniVDD_Sys_VM_Terminate, DOSVM
;
;We must call SetVGAPassthrough in order to assure ourselves that we're
;out of HiRes mode upon termination of Windows.
;
	call	SetVGAPassthrough	;will be a NOP if not a Mach8 or Mach32
;
MSVTExit:
	clc
	ret
EndProc MiniVDD_Sys_VM_Terminate
;
;
subttl		Initialize for a Newly Created VM
page +
public	MiniVDD_Create_VM
BeginProc MiniVDD_Create_VM, VMCREATE
;
;Whenever a DOS VM is created, we increment a counter which when non-zero,
;indicates that we must keep trapping on for ATI extension ports 1CEH and
;1CFH.	When the VM is destroyed, we decrement the counter and when it
;becomes zero, we remove trapping on 1CEH and 1CFH.
;
	mov	edx,01ceh		;ATI VGA extended register index
	VMMCall Enable_Global_Trapping
;
	mov	edx,01cfh               ;ATI VGA extended register data
	VMMCall Enable_Global_Trapping
;
	inc	DOSVMCount		;add another DOS VM into system
	clc
	ret
EndProc MiniVDD_Create_VM
;
;
subttl		Handler for a VM in Process of Destruction
page +
public	MiniVDD_Destroy_VM
BeginProc MiniVDD_Destroy_VM, VMDESTROY
;
;We decrement the DOSVMCount.  If this gets to zero, our DisableTraps routine
;will allow ports 1CEH and 1CFH to have trapping disabled.  Otherwise, we'll
;keep trapping on 1CEH and 1CFH as long as they're windowed or background.
;
	dec	DOSVMCount			;bump down our DOS box count
	jge	short @F			;if we're zero or above it's OK
	mov	DOSVMCount,0			;never go negative though!
@@:	clc
	ret
EndProc MiniVDD_Destroy_VM
;
;
subttl		Register Board Dependent Variables With MiniVDD
page +
public	MiniVDD_RegisterDisplayDriver
BeginProc MiniVDD_RegisterDisplayDriver, RARE
;
;This will only be called for the Mach32 and Mach64 chipsets!
;
;Oft-times, the display driver must set flags whenever it is doing
;certain things to a hardware BLTer (such as "I'm in the middle of
;transferring a source bitmap").  This routine is called to allow
;this mini-VDD to get data (usually containing addresses inside the
;display driver's Data segment) directly from the display driver.
;We can also save appropriately needed states here.
;
;	The only client registers that are reserved (and cannot be used
;to pass data in) are EAX (which contains the function code which the
;display driver uses to call the "main" VDD) and EBX (which contains the
;Windows VM handle).  When we get called by the "main" VDD, we'll be
;passed a flag in Client_AL which will be set to non-zero if the VFLATD
;VxD is being used for banked support or zero if VFLATD is NOT involved in
;this session.
;
;Entry:
;	EBP --> the Client Register Structure (documented in VMM.INC)
;	Client_ECX contains the total VRAM size on the card.
;	Client_EDX contains physical segment for linear aperture.
;	Client_ESI contains physical segment for MMIO registers.
;Exit:
;	Client_DX contains the LDT selector for linear memory.
;	Client_SI contains the LDT selector for MMIO memory.
;
	mov	eax,[ebp].Client_ECX	;get total memory size on card
	mov	TotalMemorySize,eax	;save it off
;
;We need to allocate and register a screen selector for the linear aperture:
;
	mov	edx,[ebp].Client_EDX	;get passed in physical addr
	or	edx,edx 		;did display driver pass us something?
	jz	MRDDAllocMMIOSelector	;nope, skip all this
	cmp	LinearSelector,0	;assume we already have a selector
	jne	@F			;yes, just return the same one
	VMMCall _MapPhysToLinear,<edx, 8*1024*1024, 0>
					;get an 8 meg linear aperture
;
;Make the descriptor for this linear address so we can use it to allocate the
;LDT selector that we need:
;
	VMMCall _BuildDescriptorDWORDs,<eax, (8*1024*1024)/4096, RW_DATA_TYPE, D_GRAN_PAGE, 0>
;
;Now, create an LDT selector for this linear memory:
;
	VMMCall _Allocate_LDT_Selector,<ebx, edx, eax, 1, 0>
	mov	LinearSelector,eax	;save it for our own use
	push	eax			;save selector over call
	VxDCall VDD_Register_Extra_Screen_Selector
	pop	eax			;restore saved selector
@@:	mov	[ebp].Client_EDX,eax	;return newly alloc'd selector to caller
;
public	MRDDAllocMMIOSelector
MRDDAllocMMIOSelector:
;
;We need to allocate and register a screen selector for the memory mapped IO
;ports aperture:
;
	mov	edx,[ebp].Client_ESI	;get passed in physical addr
	or	edx,edx 		;did display driver pass us something?
	jz	MRDDExit		;nope, skip all this
	cmp	MMIOSelector,0		;assume we already have a selector
	jne	@F			;yes, just return the same one
	VMMCall _MapPhysToLinear,<edx, 8*1024, 0>
					;get an 8K MMIO aperture
;
;Make the descriptor for this linear address so we can use it to allocate the
;LDT selector that we need:
;
	VMMCall _BuildDescriptorDWORDs,<eax, (8*1024)/4096, RW_DATA_TYPE, D_GRAN_PAGE, 0>
;
;Now, create an LDT selector for this linear memory:
;
	VMMCall _Allocate_LDT_Selector,<ebx, edx, eax, 1, 0>
	mov	MMIOSelector,eax	;save it for our own use
	push	eax			;save selector over call
	VxDCall VDD_Register_Extra_Screen_Selector
	pop	eax			;restore saved selector
@@:	mov	[ebp].Client_ESI,eax	;return newly alloc'd selector to caller
;
MRDDExit:
	ret				;
EndProc MiniVDD_RegisterDisplayDriver
;
;
subttl		Calculate and Save Banking Register Values
page +
public	MiniVDD_GetVDDBank
BeginProc MiniVDD_GetVDDBank, RARE
;
;This routine will only be called for the Mach8, Mach32, and Mach64!
;
;In order to virtualize VGA graphics mode applications in a window on the
;Windows desktop, the VDD requires you to reserve at least 32K of off-screen
;video memory for the exclusive use of the VDD.  Although it's best to
;allocate a full 64K of off-screen video memory, the VDD can work with a
;minimum of 32K.  This function is called by the "main" VDD when the
;display driver registers with it.  This function must calculate the
;correct bank of the off-screen memory and the offset within that bank of
;the VDD reserved memory.  Note that it's best if the offset value returned
;is zero (that is, you start on a bank boundry).
;
;The mini-VDD saves the banking register values locally for later
;use during VGA virtualization.
;
;Note that the "main" VDD assumes that the hardware banks are 64K in length.
;
;Entry:
;	EBX contains the Windows VM handle (which must be preserved).
;	ECX contains the byte offset of where in video memory the VDD
;	    memory is to be placed (this will be the first byte of
;	    off-screen memory).
;	Client_ECX has the same thing as ECX has.
;Exit:
;	EBX must still contain the Windows VM handle.
;	ECX contains the starting address of the VDD virtualization area
;	EDX contains the amount of memory that we allocated to
;	the VDD virtualization area.  We set EDX == ECX if we are in
;	a "memory shy" configuration.  That is, to tell the "main"
;	VDD that we cannot support VGA 4 plane graphics in the background.
;
	mov	SuperVGAModeFlag,0ffh	;flag so that routines will know
					;that we're not running 4 plane mode
	cmp	ChipID,32h		;running on a Mach32 or Mach64?
if MACH64VGAVirt
	jae	MGVBGetMach32Bank	;yes, go do complicated stuff
else
	je	MGVBGetMach32Bank	;
	cmp	ChipID,64h		;running on a Mach 64?
	je	MGVBMemoryShy		;yes, return bad so we don't do 4 plane
					;virtualization
endif	;MACH64VGAVirt
;
public	MGVBGetMach8Bank
MGVBGetMach8Bank:
;
;Since the Mach8 has a full VGA on board and doesn't share the controller
;and VRAM between the HiRes card and the VGA card, we simply return stuff
;so that the main VDD will know that we can do virtualization in a window:
;
	xor	ecx,ecx 		;this is start addr of virtualization
	mov	edx,64*1024		;we allow 64K of VGA space for this
	xor	ah,ah			;no page offsets
	jmp	MGVBExit		;we're done!
;
public	MGVBGetMach32Bank
MGVBGetMach32Bank:
;
;For the ATI chipsets, our VGA virtualization banks are 64K in length.
;So, let's find out in which bank the off-screen memory starts:
;
	mov	eax,ecx 		;get start address of off-screen memory
	shr	eax,16			;divide by 64K per bank - AL has bank #
;
;At this point:
;	AL contains the bank number of the start of off-screen memory
;	CX contains the offset within the bank of the start of
;	   off-screen memory.
;
;It is desirable for us to start the VDD memory area on an even bank
;boundry.  This is possible if we have at least one full 64K bank
;of video memory remaining.  To determine this, we must calculate
;the total number of banks available in video memory.  If there's
;at least one more than what's in AL, we can start on an even bank
;boundry!
;
	or	cx,cx			;are we already on an even bank boundry?
	jz	MGVBCalcAmountToUse	;yes, go see if we can alloc 64K
	mov	edx,TotalMemorySize	;get total amount of video memory
	shr	edx,16			;now DL has total banks on system
	dec	dl			;now DL has last possible bank nbr
	cmp	al,dl			;can we start on an even bank boundry?
	jae	MGVBMemoryShy		;nope, turn off VGA virtualization
	inc	al			;yes! put ourselves at next bank boundry
	xor	cx,cx			;zero the offset in ECX
	add	ecx,10000h		;and set ECX = new start of VDD area
;
public	MGVBCalcAmountToUse
MGVBCalcAmountToUse:
;
;At this point:
;	AL contains the bank number for VDD use.
;	ECX contains the 32 bit address of the start of this memory.
;
	mov	edx,TotalMemorySize	;get total amount of video memory
	sub	edx,ecx 		;EDX = amount of memory from start
					;of VDD bank till end of video memory
	cmp	edx,64*1024		;do we have more than 64K left?
	mov	edx,64*1024		;(assume we do have more than 64K left)
	ja	MGVBGetBankingValues	;we can use an entire 64K bank for VDD!
	mov	edx,32*1024		;we can only use 32K for the VDD!
;
public	MGVBGetBankingValues
MGVBGetBankingValues:
;
;At this point:
;	AL contains the 64K bank number for the VDD virtualization bank.
;	ECX contains the 32 bit start address of the VDD virtualization area.
;	EDX contains the size of the VDD virtualization area (either 32K
;	    or 64K).
;
;We should setup the values for the ATI banking registers so that we can set
;them quickly when called upon by the "main" VDD to do so:
;
	push	edx			;save size of VDD area for now
	push	ebx			;we need this as a work register
	mov	BankRegSaveLow,0ffh	;make sure this wasn't messed up
					;by board initialization
if MACH64VGAVirt
	mov	Mach64WriteBankSave,-1	;(for the Mach64)
endif	;MACH64VGAVirt
;
;When the MemC is in 4 plane mode (such as when virtualizing 4 plane
;VGA apps in a window), we must actually divide the bank number that
;we calculated above (currently in AL) by 4.  If our bank number isn't
;easily divisible by 4, we must return the odd number to the main VDD
;so it can adjust accordingly:
;
if MACH64VGAVirt
	mov	VDDPageOffsetFromBank,0 ;assume we're on a Mach64
	cmp	ChipID,64h		;running on a Mach64?
	je	MGVBMach64BankingValues ;yes, go do it!
endif	;MACH64VGAVirt
	mov	ah,al			;copy bank number to AH for odd calc
	shr	al,2			;this is the physical bank that we use
	and	ah,03h			;this is page offset from bank's start
	shl	ah,2			;
	mov	VDDPageOffsetFromBank,ah;save this for SetVDDBank
;
public	MGVBMach32BankingValues
MGVBMach32BankingValues:
	mov	bl,al			;copy bank number to BL & BH for now
	mov	bh,al			;
	and	bl,0fh			;get low bits of VDD bank in BL
        shl     bl,1                    ;get bank bits in place
	mov	VDDBankLow,bl		;save this for use in virtualization
	shr	bh,4			;get high bits of VDD bank in BH
        mov     edx,1CEh
        mov     al,0AEh
        out     dx,al
        inc     edx
        in      al,dx
        and     al,0f0h
        or      al,bh
	mov	VDDBankHigh,al		;save this for use in virtualization
	pop	ebx			;restore saved registers
	pop	edx			;
	jmp	MGVBExit		;
;
if MACH64VGAVirt
public	MGVBMach64BankingValues
MGVBMach64BankingValues:
;
;The Mach64 uses two 32K banks:
;
	shl	al,1			;we calculated banks of 64K in size
	mov	byte ptr Mach64VDDBank,al
	mov	byte ptr Mach64VDDBank+1,0
	inc	al			;
	mov	byte ptr Mach64VDDBank+2,al
	mov	byte ptr Mach64VDDBank+3,0
	pop	ebx			;(restore saved registers)
	pop	edx			;
	jmp	MGVBExit		;
endif	;MACH64VGAVirt
;
public	MGVBMemoryShy
MGVBMemoryShy:
;
;If we reach this point, it means that the Windows visible screen overlaps
;into the very last bank of available video memory.  This creates a
;"memory-shy" configuration which prohibits us from running windowed
;and background EGA/VGA graphics mode apps.
;
;At this point:
;	Client_ECX contains the value passed in at the beginning of the routine.
;
	mov	edx,[ebp].Client_ECX	;indicate a "memory-shy" configuration
;
MGVBExit:
	ret
EndProc MiniVDD_GetVDDBank
;
;
subttl		Banking Handling Code for VGA Virtualization
page +
public	MiniVDD_SetVDDBank_32
BeginProc MiniVDD_SetVDDBank_32, DOSVM
;
;This routine is only called on a Mach32!
;
;This routine is called when we need to virtualize something in the
;off-screen region of the video memory.  You should save the current
;state of the banking registers and then set your banking registers to
;the off-screen memory region.
;
;Entry:
;	EAX contains the page number requested for the VDD area.
;	EBX contains the MemC owner's VM handle.
;Exit:
;	Save any registers that you use.
;
	push	eax
	push	edx
	mov	edx,42eeh
	xor	eax,eax
	out	dx,ax
	pop	edx
	pop	eax
	cmp	BankRegSaveLow,0ffh	;are we already set to the VDD bank?
	jne	MSTVAdjustPage_32	;yes! just go adjust mid-bank offsets
	cmp	SuperVGAModeFlag,0	;running in 4 plane VGA mode?
	je	MSTVExit_32		;yes, don't do anything here
	push	eax			;
	push	edx			;
	mov	edx,1CEh		;EDX --> ATI extension index register
	in	al,dx			;get current ATI extension index
	ror	eax,8			;save it in top byte of EAX
;
	mov	al,0B2h			;set to low banking register (B2H)
	out	dx,al			;
	inc	edx			;
	in	al,dx			;now AL has current bank state
	mov	BankRegSaveLow,al	;save this for later restore
	dec	edx			;EDX --> ATI extended index register
	mov	ah,VDDBankLow		;set to our reserved bank
	mov	al,0b2h 		;always reset index after doing I/O
	out	dx,ax			;
;
	mov	al,0AEh			;now get the high banking register
	out	dx,al			;
	inc	edx			;
	in	al,dx			;
	mov	BankRegSaveHigh,al	;
	dec	edx			;EDX --> ATI extended index register
	mov	ah,VDDBankHigh		;set to our reserved bank
	mov	al,0aeh 		;always reset index after doing I/O
	out	dx,ax			;
;
	rol	eax,8			;AL now contains saved ATI index value
	out	dx,al			;
	pop	edx			;restore saved registers
	pop	eax			;
;
public	MSTVAdjustPage_32
MSTVAdjustPage_32:
;
;Our VDD virtualization area sometimes starts in the middle of a bank.	We
;therefore must offset the page passed to us in EAX (actually in AL) by the
;number of pages that the VDD virtualization area is offset from the start
;of the bank.  This value was calculated at GetVDDBank.  We therefore
;modify the passed in value in AL so that the VGA app starts writing in
;the proper place (below the screen instead of on top of it!!!).
;
	and	al,0fh			;get page offset within bank
	add	al,VDDPageOffsetFromBank;correct for any offset that we have
;
MSTVExit_32:
	ret				;
EndProc MiniVDD_SetVDDBank_32
;
;
public	MiniVDD_ResetBank_32
BeginProc MiniVDD_ResetBank_32, DOSVM
;
;This routine is only called on a Mach32!
;
;This routine is called when the VDD is done using the off-screen memory
;and we want to restore to the bank that we were in before we started using
;the off-screen memory.  Note that if the bank that's currently set in the
;hardware's banking registers is NOT the VDD bank, that we do not wish to
;reset the banking since someone else purposely changed it and we don't
;want to override those purposeful changes.
;
;Entry:
;	EBX contains the VM handle.
;Exit:
;	Save anything that you use.
;
	cmp	BankRegSaveLow,0ffh	;is there any banking to restore?
	je	MRBExit_32		;nope, skip this!
;
;We may need to do something.  Save off the current ATI index register state:
;
	push	eax			;
	push	edx			;
	mov	edx,1CEh		;
	in	al,dx			;get current ATI extended index
	ror	eax,8			;save it in top byte of EAX
;
;Get the values of both banking registers so we can see if they're still
;set to "our" bank.  If they aren't, then Windows has already switched
;them to something for its own purposes and we'd best not restore them!
;
	mov	al,0AEh			;get high banking register first
	out	dx,al			;
	inc	edx			;
	in	al,dx			;
	mov	ah,al			;save it in AH
	dec	edx			;
;
	mov	al,0B2h			;get low banking register
	out	dx,al			;
	inc	edx			;
	in	al,dx			;into AL
;
	mov	dl,VDDBankLow		;
	mov	dh,VDDBankHigh		;
	cmp	ax,dx			;are registers still set to VDD bank?
	jne	MRBResetBankSaveFlags_32;nope, don't physically reset bank regs
;
;It's safe to restore the banking registers!
;
	mov	edx,1CEh		;reset EDX --> ATI index register
	mov	al,0b2h 		;always reset index after doing I/O
	mov	ah,BankRegSaveLow	;get bank to restore
	out	dx,ax			;
	mov	al,0AEh 		;do the same for the high banking reg
	mov	ah,BankRegSaveHigh	;get bank to restore
	out	dx,ax			;
;
public	MRBResetBankSaveFlags_32
MRBResetBankSaveFlags_32:
	mov	BankRegSaveLow,0ffh	;flag that we're not set to VDD bank
;
;Lastly, restore the ATI index register that we saved earlier.
;
	mov	edx,1CEh		;EDX --> ATI Extension index register
	rol	eax,8			;AL now contains saved ATI index value
	out	dx,al			;
	pop	edx			;restore saved registers
	pop	eax			;
;
MRBExit_32:
	ret				;
EndProc MiniVDD_ResetBank_32
;
;
subttl		Set To Latch Scratchpad Bank
page +
public	MiniVDD_SetLatchBank_32
BeginProc MiniVDD_SetLatchBank_32, DOSVM
;
;This routine is only called on a Mach32!
;
;When virtualizing the VGA 4 plane mode, we have to save and restore the
;latches occasionally.	This routine allows you to set to an off-screen
;bank (in this case and in most cases, the VDD bank) in order to prepare
;for restoring the VGA latches.
;
;Entry:
;	Nothing assumed.
;Exit:
;	Save anything that you use.
;
	push	eax			;save registers that we use
	push	edx			;
	cmp	SuperVGAModeFlag,0	;running in 4 plane VGA mode?
	je	MSLBExit_32		;yes, don't do anything here
;
	mov	edx,1CEh		;EDX --> ATI extended index register
	in	al,dx			;get and save index value
	ror	eax,8			;
;
;And set to our reserved latch bank:
;
	mov	al,0B2h 		;set to ATI extended register 32H
	out	dx,al			;
	inc	edx			;EDX --> ATI extended data register
	in	al,dx			;get the current value of port 32H
	mov	LatchSave32H,al 	;save it for later restore
	dec	edx			;EDX --> ATI extended index register
	mov	al,0B2h 		;always reset index after doing I/O
	mov	ah,VDDBankLow		;set to the VDD bank
	out	dx,ax			;
;
	mov	al,0AEh 		;set to ATI extended register 2EH
	out	dx,al			;
	inc	edx			;EDX --> ATI extended data register
	in	al,dx			;get the current value of port 2EH
	mov	LatchSave2EH,al 	;save it for later restore
	dec	edx			;EDX --> ATI extended index register
	mov	al,0AEh 		;always reset index after doing I/O
	mov	ah,VDDBankHigh		;set to the VDD bank
	out	dx,ax			;
;
;Restore the ATI extended index register:
;
	rol	eax,8			;
	out	dx,al			;
;
MSLBExit_32:
	pop	edx			;restore saved registers
	pop	eax			;
	ret				;
EndProc MiniVDD_SetLatchBank_32
;
;
subttl		Reset Banking After Latch Operations
page +
public	MiniVDD_ResetLatchBank_32
BeginProc MiniVDD_ResetLatchBank_32, DOSVM
;
;This routine is only called on a Mach32!
;
;This routine reverses the latch save that we did prior to restoring the
;latches.  Just restore the states that you saved.
;
;Entry:
;	Nothing assumed.
;Exit:
;	Save anything that you use.
;
	push	eax			;save registers that we use
	push	edx			;
	cmp	SuperVGAModeFlag,0	;running in 4 plane VGA mode?
	je	MRLBExit_32		;yes, don't do anything here
;
	mov	edx,1CEh		;EDX --> ATI extended index register
	in	al,dx			;get and save extended index value
	ror	eax,8			;
;
;Reset the low banking register:
;
	mov	al,0B2h			;
	mov	ah,LatchSave32H 	;
	out	dx,ax			;
;
;Reset the high banking register:
;
	mov	al,0AEh			;
	mov	ah,LatchSave2EH 	;
	out	dx,ax			;
;
;Restore the ATI extended index register:
;
	rol	eax,8			;
	out	dx,al			;
;
MRLBExit_32:
	pop	edx			;restore saved registers
	pop	eax			;
	ret				;
EndProc MiniVDD_ResetLatchBank_32
;
;
subttl		Banking Handling Code for VGA Virtualization
page +
if MACH64VGAVirt
public	MiniVDD_SetVDDBank_64
BeginProc MiniVDD_SetVDDBank_64, DOSVM
;
;This routine is only called on a Mach64!
;
;This routine is called when we need to virtualize something in the
;off-screen region of the video memory.  You should save the current
;state of the banking registers and then set your banking registers to
;the off-screen memory region.
;
;Entry:
;	EAX contains the page number requested for the VDD area.
;	EBX contains the MemC owner's VM handle.
;Exit:
;	Save any registers that you use.
;
	cmp	Mach64WriteBankSave,-1	;are we already set to the VDD bank?
	jne	MSTVAdjustPage_64	;yes! just go adjust mid-bank offsets
	cmp	SuperVGAModeFlag,0	;running in 4 plane VGA mode?
	je	MSTVExit_64		;yes, don't do anything here
	push	eax			;
	push	edx			;
;
;Turn on VGA aperture addressing after saving the current state:
;
	mov	edx,6aech		;EDX --> ATI Configuration Control
	in	al,dx			;we only need the first byte of this
	mov	Mach64ConfigCtrlSave,al ;
	or	al,04h			;now enable the VGA aperture
	out	dx,al			;
;
	mov	edx,56ech		;EDX --> ATI Write Bank Register
	in	al,dx			;read this register a byte at a time
	ror	eax,8			;
	inc	dl			;
	in	al,dx			;
	ror	eax,8			;
	inc	dl			;
	in	al,dx			;
	ror	eax,8			;
	inc	dl			;
	in	al,dx			;
	ror	eax,8			;
	mov	Mach64WriteBankSave,eax ;
;
	mov	edx,5aech		;EDX --> ATI Read Bank Register
	in	al,dx			;read this register a byte at a time
	ror	eax,8			;
	inc	dl			;
	in	al,dx			;
	ror	eax,8			;
	inc	dl			;
	in	al,dx			;
	ror	eax,8			;
	inc	dl			;
	in	al,dx			;
	ror	eax,8			;
	mov	Mach64ReadBankSave,eax	;
;
	mov	edx,56ech		;EDX --> ATI Write Bank Register
	mov	eax,Mach64VDDBank	;set to our reserved bank
	out	dx,al			;write this register a byte at a time
	ror	eax,8			;
	inc	dl			;
	out	dx,al			;
	ror	eax,8			;
	inc	dl			;
	out	dx,al			;
	ror	eax,8			;
	inc	dl			;
	out	dx,al			;
;
	mov	edx,5aech		;EDX --> ATI Read Bank Register
	mov	eax,Mach64VDDBank	;set to our reserved bank
	out	dx,al			;write this register a byte at a time
	ror	eax,8			;
	inc	dl			;
	out	dx,al			;
	ror	eax,8			;
	inc	dl			;
	out	dx,al			;
	ror	eax,8			;
	inc	dl			;
	out	dx,al			;
	pop	edx			;restore saved registers
	pop	eax			;
;
public	MSTVAdjustPage_64
MSTVAdjustPage_64:
;
;Our VDD virtualization area sometimes starts in the middle of a bank.	We
;therefore must offset the page passed to us in EAX (actually in AL) by the
;number of pages that the VDD virtualization area is offset from the start
;of the bank.  This value was calculated at GetVDDBank.  We therefore
;modify the passed in value in AL so that the VGA app starts writing in
;the proper place (below the screen instead of on top of it!!!).
;
	and	al,0fh			;get page offset within bank
	add	al,VDDPageOffsetFromBank;correct for any offset that we have
;
MSTVExit_64:
	ret				;
EndProc MiniVDD_SetVDDBank_64
endif	;MACH64VGAVirt
;
;
if MACH64VGAVirt
public	MiniVDD_ResetBank_64
BeginProc MiniVDD_ResetBank_64, DOSVM
;
;This routine is only called on a Mach64!
;
;This routine is called when the VDD is done using the off-screen memory
;and we want to restore to the bank that we were in before we started using
;the off-screen memory.  Note that if the bank that's currently set in the
;hardware's banking registers is NOT the VDD bank, that we do not wish to
;reset the banking since someone else purposely changed it and we don't
;want to override those purposeful changes.
;
;Entry:
;	EBX contains the VM handle.
;Exit:
;	Save anything that you use.
;
	cmp	Mach64WriteBankSave,-1	;is there any banking to restore?
	je	MRBExit_64		;nope, skip this!
;
;We may need to do something.  Save off the current ATI index register state:
;
	push	eax			;
	push	edx			;
;
public	MRBResetWriteBank_64
MRBResetWriteBank_64:
;
;Get the values of both banking registers so we can see if they're still
;set to "our" bank.  If they aren't, then Windows has already switched
;them to something for its own purposes and we'd best not restore them!
;
	mov	edx,56ech		;EDX --> ATI Write Bank Register
	in	al,dx			;read this register a byte at a time
	ror	eax,8			;
	inc	dl			;
	in	al,dx			;
	ror	eax,8			;
	inc	dl			;
	in	al,dx			;
	ror	eax,8			;
	inc	dl			;
	in	al,dx			;
	ror	eax,8			;now we have current contents in EAX
	cmp	eax,Mach64VDDBank	;did the contents change?
	jne	MRBResetReadBank_64	;yes, don't fool with write bank
	mov	eax,Mach64WriteBankSave ;no, reset the write bank
	mov	dl,0ech 		;EDX --> ATI Write Bank Register
	out	dx,al			;write this register a byte at a time
	ror	eax,8			;
	inc	dl			;
	out	dx,al			;
	ror	eax,8			;
	inc	dl			;
	out	dx,al			;
	ror	eax,8			;
	inc	dl			;
	out	dx,al			;
;
public	MRBResetReadBank_64
MRBResetReadBank_64:
	mov	edx,5aech		;EDX --> ATI Read Bank Register
	in	al,dx			;read this register a byte at a time
	ror	eax,8			;
	inc	dl			;
	in	al,dx			;
	ror	eax,8			;
	inc	dl			;
	in	al,dx			;
	ror	eax,8			;
	inc	dl			;
	in	al,dx			;
	ror	eax,8			;now we have current contents in EAX
	cmp	eax,Mach64VDDBank	;did the contents change?
	jne	MRBResetBankSaveFlags_64;yes, don't fool with read bank
	mov	eax,Mach64ReadBankSave	;no, reset the read bank
	mov	dl,0ech 		;EDX --> ATI Read Bank Register
	out	dx,al			;write this register a byte at a time
	ror	eax,8			;
	inc	dl			;
	out	dx,al			;
	ror	eax,8			;
	inc	dl			;
	out	dx,al			;
	ror	eax,8			;
	inc	dl			;
	out	dx,al			;
;
public	MRBResetBankSaveFlags_64
MRBResetBankSaveFlags_64:
;
;Restore the state of the Mach 64 CONFIG_CTRL register:
;
	mov	al,Mach64ConfigCtrlSave ;
	mov	edx,6aech		;EDX --> ATI CONFIG_CTRL register
	out	dx,al			;we only care about the 1st byte
;
	mov	Mach64WriteBankSave,-1	;flag that we're not set to VDD bank
	pop	edx			;restore saved registers
	pop	eax			;
;
MRBExit_64:
	ret				;
EndProc MiniVDD_ResetBank_64
endif	;MACH64VGAVirt
;
;
subttl		Set To Latch Scratchpad Bank
page +
if MACH64VGAVirt
public	MiniVDD_SetLatchBank_64
BeginProc MiniVDD_SetLatchBank_64, DOSVM
;
;This routine is only called on a Mach64!
;
;When virtualizing the VGA 4 plane mode, we have to save and restore the
;latches occasionally.	This routine allows you to set to an off-screen
;bank (in this case and in most cases, the VDD bank) in order to prepare
;for restoring the VGA latches.
;
;Entry:
;	Nothing assumed.
;Exit:
;	Save anything that you use.
;
	push	eax			;save registers that we use
	push	edx			;
	cmp	SuperVGAModeFlag,0	;running in 4 plane VGA mode?
	je	MSLBExit_64		;yes, don't do anything here
;
;Turn on VGA aperture addressing after saving the current state:
;
	mov	edx,6aech		;EDX --> ATI Configuration Control
	in	al,dx			;read this register a byte at a time
	mov	Mach64LatchConfigCtrlSave,al
	or	al,04h			;now enable the VGA aperture
	out	dx,al			;write this register a byte at a time
;
;And set to our reserved latch bank:
;
	mov	edx,56ech		;EDX --> ATI Write Bank Register
	in	al,dx			;read this register a byte at a time
	ror	eax,8			;
	inc	dl			;
	in	al,dx			;
	ror	eax,8			;
	inc	dl			;
	in	al,dx			;
	ror	eax,8			;
	inc	dl			;
	in	al,dx			;
	ror	eax,8			;
	mov	Mach64WriteLatchBankSave,eax
;
	mov	edx,5aech		;EDX --> ATI Read Bank Register
	in	al,dx			;read this register a byte at a time
	ror	eax,8			;
	inc	dl			;
	in	al,dx			;
	ror	eax,8			;
	inc	dl			;
	in	al,dx			;
	ror	eax,8			;
	inc	dl			;
	in	al,dx			;
	ror	eax,8			;
	mov	Mach64ReadLatchBankSave,eax
;
	mov	edx,56ech		;EDX --> ATI Write Bank Register
	mov	eax,Mach64VDDBank	;set to our reserved bank
	out	dx,al			;write this register a byte at a time
	ror	eax,8			;
	inc	dl			;
	out	dx,al			;
	ror	eax,8			;
	inc	dl			;
	out	dx,al			;
	ror	eax,8			;
	inc	dl			;
	out	dx,al			;
;
	mov	edx,5aech		;EDX --> ATI Read Bank Register
	mov	eax,Mach64VDDBank	;set to our reserved bank
	out	dx,al			;write this register a byte at a time
	ror	eax,8			;
	inc	dl			;
	out	dx,al			;
	ror	eax,8			;
	inc	dl			;
	out	dx,al			;
	ror	eax,8			;
	inc	dl			;
	out	dx,al			;
;
MSLBExit_64:
	pop	edx			;restore saved registers
	pop	eax			;
	ret				;
EndProc MiniVDD_SetLatchBank_64
endif	;MACH64VGAVirt
;
;
subttl		Reset Banking After Latch Operations
page +
if MACH64VGAVirt
public	MiniVDD_ResetLatchBank_64
BeginProc MiniVDD_ResetLatchBank_64, DOSVM
;
;This routine is only called on a Mach64!
;
;This routine reverses the latch save that we did prior to restoring the
;latches.  Just restore the states that you saved.
;
;Entry:
;	Nothing assumed.
;Exit:
;	Save anything that you use.
;
	push	eax			;save registers that we use
	push	edx			;
	cmp	SuperVGAModeFlag,0	;running in 4 plane VGA mode?
	je	MRLBExit_64		;yes, don't do anything here
	mov	edx,56ech		;EDX --> ATI Write Bank Register
	mov	eax,Mach64WriteLatchBankSave
					;restore our saved bank
	out	dx,al			;write this register a byte at a time
	ror	eax,8			;
	inc	dl			;
	out	dx,al			;
	ror	eax,8			;
	inc	dl			;
	out	dx,al			;
	ror	eax,8			;
	inc	dl			;
	out	dx,al			;
;
	mov	edx,5aech		;EDX --> ATI Read Bank Register
	mov	eax,Mach64ReadLatchBankSave
					;restore our saved bank
	out	dx,al			;write this register a byte at a time
	ror	eax,8			;
	inc	dl			;
	out	dx,al			;
	ror	eax,8			;
	inc	dl			;
	out	dx,al			;
	ror	eax,8			;
	inc	dl			;
	out	dx,al			;
;
;Restore the state of the Mach 64 CONFIG_CTRL register:
;
	mov	al,Mach64LatchConfigCtrlSave
	mov	edx,6aech		;EDX --> ATI CONFIG_CTRL register
	out	dx,al			;we only messed with first byte
;
MRLBExit_64:
	pop	edx			;restore saved registers
	pop	eax			;
	ret				;
EndProc MiniVDD_ResetLatchBank_64
endif	;MACH64VGAVirt
;
;
subttl		Prepare to Enter a Standard VGA Mode from HiRes Mode
page +
public	MiniVDD_PreHiResToVGA
BeginProc MiniVDD_PreHiResToVGA, DOSVM
;
;This routine is called for all cards that are running this MiniVDD!
;
;When the VDD is about to switch from Windows HiRes mode to any of the
;standard VGA modes (for real -- not virtualized), this routine will be
;called.  You need to determine exactly what your hardware requires in
;order to accomplish this task.  For example, you should disable trapping
;on registers that the mini-VDD is specifically handling (such as 1CEH
;in the case of the ATI chipset), make sure that the hardware is "ready"
;to undergo a mode transition (make sure that your hardware's graphics
;engine isn't in the middle of an operation that can't be interrupted)
;etc.  If your hardware does not return to a standard VGA mode via
;a call to INT 10H, function 0, you should also make sure to do whatever
;it takes to restore your hardware to a standard VGA mode at this time.
;Try not to touch video memory during your transition to standard VGA as
;this will disturb the reliability of the system.
;
;Next, disable trapping on registers 1CEH and 1CFH which MAY have trapping
;enabled at this time.	Since we're going to do a physical mode change here, we
;want trapping not to be enabled so that the BIOS's write will
;actually go out the physical port.  We will however, reenable the
;trapping in the MiniVDD_PostHiResToVGA routine.
;
	mov	InCRTCModeChange,0ffh	;disable virtualization
	mov	edx,01ceh		;
	VMMCall Disable_Global_Trapping ;go disable trapping on port in EDX
;
	mov	edx,01cfh		;
	VMMCall Disable_Global_Trapping ;go disable trapping on port in EDX
;
	cmp	ChipID,64h		;running on a Mach64?
	jne	@F			;nope skip the following
	call	DisableMach64Ports	;go disable the Mach64 ports
@@:	call	SetVGAPassthrough	;go switch from HiRes to VGA mode
;
MHTVExit:
	ret
EndProc MiniVDD_PreHiResToVGA
;
;
public	MiniVDD_PostHiResToVGA
BeginProc MiniVDD_PostHiResToVGA, DOSVM
;
;This routine is called for all cards that are running this MiniVDD!
;
;This routine is called after the ROM BIOS call is made to place the hardware
;back in the standard VGA state.  You should reenable trapping and do any
;other post-processing that your hardware might need:
;
;Reenable the trapping on the ports that we disabled in the above routine:
;
	mov	edx,01ceh		;
	VMMCall Enable_Global_Trapping	;go reenable trapping on port in EDX
;
	mov	edx,01cfh		;
	VMMCall Enable_Global_Trapping	;go reenable trapping on port in EDX
;
	cmp	ChipID,64h		;running on a Mach64?
	jne	MPHVExit		;nope skip the following
	call	EnableMach64Ports	;go enable the Mach64 ports
;
MPHVExit:
	mov	InCRTCModeChange,0	;tell virtualization to continue
	ret				;
EndProc MiniVDD_PostHiResToVGA
;
;
subttl		Prepare to Enter a Standard VGA Mode from HiRes Mode
page +
public	MiniVDD_PreVGAToHiRes
BeginProc MiniVDD_PreVGAToHiRes, DOSVM
;
;This routine is called for all cards that are running this MiniVDD!
;
;The VDD will now switch us from VGA full-screen mode to Windows
;HiRes mode.  We need to make sure that the Sequencer register 4 value
;indicates Chain4 (packed pixel) graphics.  Although the hardware doesn't
;really care, our VGA virtualization routines do in the main VDD.  This
;prevents the main VDD from thinking that we're running in text mode.
;
	mov	InCRTCModeChange,0ffh	;disable virtualization
	cmp	SuperVGAModeFlag,0	;running in 4 plane graphics mode?
	je	MPVHExit		;yes, don't do this stuff?
	mov	edx,3c4h		;EDX --> Sequencer index register
	in	al,dx			;get and save the current index
	ror	eax,8			;
	mov	ax,0e04h		;set to "chain 4" in the VGA registers
	out	dx,ax			;
	rol	eax,8			;restore previous value of index reg
	out	dx,al			;
;
MPVHExit:
	ret
EndProc MiniVDD_PreVGAToHiRes
;
;
public	MiniVDD_PostVGAToHiRes
BeginProc MiniVDD_PostVGAToHiRes, DOSVM
;
;This routine is called for all cards that are running this MiniVDD!
;
;We're done switching into HiRes mode.  Re-enable the trapping.
;
	mov	InCRTCModeChange,0	;reenable virtualization
	ret
EndProc MiniVDD_PostVGAToHiRes
;
;
subttl		Set ATI Mach8 or Mach32 Out of Windows HiRes Mode
page +
public	SetVGAPassthrough
BeginProc SetVGAPassthrough, DOSVM
;
;This routine will only be called for the Mach8 and Mach32!
;
	cmp	SuperVGAModeFlag,0	;running in 4 plane VGA mode?
	je	SVPExit 		;yes, don't do anything here
	cmp	ChipID,08h		;running on a Mach8?
	je	SVPMach8		;yes, go do it for the Mach8
	cmp	ChipID,32h		;running on some other chipset?
	jne	SVPExit 		;yes, exit
;
public	SVPMach32
SVPMach32:
;
;Call the ROM code to set us back to VGA passthrough mode.
;
;Since our nested execution call to the ROM BIOS uses the registers of the
;current VM, we need to establish EBP --> current VM's client registers.  In
;most cases this will be set.  However, if we're being called because of going
;into blue-screen message mode, EBP may point to the CRTC owner's client regs
;instead of the currently executing VM's.
;
	push	ebx			;save Windows VM handle in EBX
	push	ebp			;save EBP --> some VM's client regs
	VMMCall Get_Cur_VM_Handle	;EBX contains the Current VM handle
	mov	ebp,[ebx].CB_Client_Pointer
					;establish EBP --> VM's Client regs
	Push_Client_State		;save current client register state
	VMMCall Begin_Nest_V86_Exec	;so we can call software in a VM
	mov	edx,68h 		;this is address of SetMode call
	mov	cx,UltraROMSegment	;this is physical segment of ROM
	mov	[ebp].Client_EAX,0	;set to VGA passthrough mode!
	VMMCall Simulate_Far_Call	;setup call to routine at CX:EDX
	VMMCall Resume_Exec		;call the VM to execute the far call
	VMMCall End_Nest_Exec		;we're done calling routines in VM
	Pop_Client_State		;return client registers to prev state
	pop	ebp			;restore whatever was in EBP
	pop	ebx			;restore Windows VM handle to EBX
	jmp	SVPExit 		;we're done on the Mach32
;
public	SVPMach8
SVPMach8:
;
;Clear the 1's bit from register 4AEEH.  This'll set us back into VGA
;passthrough mode:
;
	mov	edx,4aeeh		;EDX --> 4AEEH
	in	ax,dx			;
	and	al,NOT 01h		;set to VGA passthrough mode
	out	dx,ax			;
;
SVPExit:
	ret				;
EndProc 	SetVGAPassthrough
;
;
subttl		Enable and Disable BLTer Register Trapping
page +
public	MiniVDD_EnableTraps
BeginProc MiniVDD_EnableTraps, DOSVM
;
;This routine is called for all cards that are running this MiniVDD!
;
;As stated elsewhere, the VDD needs to set traps on the BLTer registers used
;by the display driver whenever it is using the off-screen memory to
;virtualize VGA graphics mode in the background or in a window.  The reason
;the VDD does this is to receive notification on the first access to a
;BLTer register AFTER returning to the Windows VM.  Then, the VDD switches
;the state of the hardware back to that appropriate for running Windows
;HiRes mode.  In this routine, all you need to do is call the VMM service
;Enable_Global_Trapping for each of the ports that you registered at
;MiniVDD_Device_Init.  Then, the VDD will receive notification at the
;proper time and will switch states properly.
;
;Entry:
;	Nothing assumed.
;Exit:
;	Just Preserve EBX, and ESI.
;
	mov	edx,01ceh		;ATI VGA extended register index
	VMMCall Enable_Global_Trapping
;
	mov	edx,01cfh               ;ATI VGA extended register data
	VMMCall Enable_Global_Trapping
;
	cmp	ChipID,64h		;running on a Mach64?
	jne	@F			;nope skip the following
	call	EnableMach64Ports	;go enable the trapping of Mach64 ports
;
@@:	cmp	ChipID,32h		;running on a Mach32?
	jne	MERTExit		;nope, don't do anything here
	cmp	SuperVGAModeFlag,0	;running in 4 plane VGA mode?
	je	MERTExit		;yes, don't do anything here
;
	mov	edx,76EEh		;GE_PITCH
	VMMCall Enable_Global_Trapping
	mov	edx,76EFh		;GE_PITCH
	VMMCall Enable_Global_Trapping
;
	mov	edx,82E8h		;CUR_Y
	VMMCall Enable_Global_Trapping
	mov	edx,82E9h		;CUR_Y
	VMMCall Enable_Global_Trapping
;
	mov	edx,82EEh		;PATT_DATA_INDEX
	VMMCall Enable_Global_Trapping
	mov	edx,82EFh		;PATT_DATA_INDEX
	VMMCall Enable_Global_Trapping
;
	mov	edx,86E8h		;CUR_X
	VMMCall Enable_Global_Trapping
	mov	edx,86E9h		;CUR_X
	VMMCall Enable_Global_Trapping
;
	mov	edx,8AE8h		;SRC_Y/DEST_Y
	VMMCall Enable_Global_Trapping
	mov	edx,8AE9h		;SRC_Y/DEST_Y
	VMMCall Enable_Global_Trapping
;
	mov	edx,8EE8h		;SRC_X/DEST_X
	VMMCall Enable_Global_Trapping
	mov	edx,8EE9h		;SRC_X/DEST_X
	VMMCall Enable_Global_Trapping
;
	mov	edx,8EEEh		;PATT_DATA
	VMMCall Enable_Global_Trapping
	mov	edx,8EEFh		;PATT_DATA
	VMMCall Enable_Global_Trapping
;
	mov	edx,92E8h		;ERR_TERM
	VMMCall Enable_Global_Trapping
	mov	edx,92E9h		;ERR_TERM
	VMMCall Enable_Global_Trapping
;
	mov	edx,9AEEh		;LINEDRAW_INDEX
	VMMCall Enable_Global_Trapping
	mov	edx,9AEFh		;LINEDRAW_INDEX
	VMMCall Enable_Global_Trapping
;
	mov	edx,0A2E8h		;BKGD_COLOR
	VMMCall Enable_Global_Trapping
	mov	edx,0A2E9h		;BKGD_COLOR
	VMMCall Enable_Global_Trapping
;
	mov	edx,0A2EEh		;LINEDRAW_OPT
	VMMCall Enable_Global_Trapping
	mov	edx,0A2EFh		;LINEDRAW_OPT
	VMMCall Enable_Global_Trapping
;
	mov	edx,0A6E8h		;FRGD_COLOR
	VMMCall Enable_Global_Trapping
	mov	edx,0A6E9h		;FRGD_COLOR
	VMMCall Enable_Global_Trapping
;
	mov	edx,0A6EEh		;DEST_X_START
	VMMCall Enable_Global_Trapping
	mov	edx,0A6EFh		;DEST_X_START
	VMMCall Enable_Global_Trapping
;
	mov	edx,0AAEEh		;DEST_X_END
	VMMCall Enable_Global_Trapping
	mov	edx,0AAEFh		;DEST_X_END
	VMMCall Enable_Global_Trapping
;
	mov	edx,0AEEEh		;DEST_Y_END
	VMMCall Enable_Global_Trapping
	mov	edx,0AEEFh		;DEST_Y_END
	VMMCall Enable_Global_Trapping
;
	mov	edx,0B2EEh		;SRC_X_START
	VMMCall Enable_Global_Trapping
	mov	edx,0B2EFh		;SRC_X_START
	VMMCall Enable_Global_Trapping
;
	mov	edx,0BAEEh		;ALU_FG_FN
	VMMCall Enable_Global_Trapping
	mov	edx,0BAEFh		;ALU_FG_FN
	VMMCall Enable_Global_Trapping
;
	mov	edx,0BEE8h		;SCISSOR...
	VMMCall Enable_Global_Trapping
	mov	edx,0BEE9h		;SCISSOR...
	VMMCall Enable_Global_Trapping
;
	mov	edx,0BEEEh		;SRC_X_END
	VMMCall Enable_Global_Trapping
	mov	edx,0BEEFh		;SRC_X_END
	VMMCall Enable_Global_Trapping
;
	mov	edx,0C2EEh		;SRC_Y_DIR
	VMMCall Enable_Global_Trapping
	mov	edx,0C2EFh		;SRC_Y_DIR
	VMMCall Enable_Global_Trapping
;
	mov	edx,0CAEEh		;SCAN_X
	VMMCall Enable_Global_Trapping
	mov	edx,0CAEFh		;SCAN_X
	VMMCall Enable_Global_Trapping
;
	mov	edx,0CEEEh		;DP_CONFIG
	VMMCall Enable_Global_Trapping
	mov	edx,0CEEFh		;DP_CONFIG
	VMMCall Enable_Global_Trapping
;
	mov	edx,0D2EEh		;PATT_LENGTH
	VMMCall Enable_Global_Trapping
	mov	edx,0D2EFh		;PATT_LENGTH
	VMMCall Enable_Global_Trapping
;
	mov	edx,0D6EEh		;PATT_INDEX
	VMMCall Enable_Global_Trapping
	mov	edx,0D6EFh		;PATT_INDEX
	VMMCall Enable_Global_Trapping
;
	mov	edx,0DAEEh		;EXT_SCISSOR_L
	VMMCall Enable_Global_Trapping
	mov	edx,0DAEFh		;EXT_SCISSOR_L
	VMMCall Enable_Global_Trapping
;
	mov	edx,0DEEEh		;EXT_SCISSOR_T
	VMMCall Enable_Global_Trapping
	mov	edx,0DEEFh		;EXT_SCISSOR_T
	VMMCall Enable_Global_Trapping
;
	mov	edx,0E2E8h		;PIX_TRANS
	VMMCall Enable_Global_Trapping
	mov	edx,0E2E9h		;PIX_TRANS
	VMMCall Enable_Global_Trapping
;
	mov	edx,0E2EEh		;EXT_SCISSOR_R
	VMMCall Enable_Global_Trapping
	mov	edx,0E2EFh		;EXT_SCISSOR_R
	VMMCall Enable_Global_Trapping
;
	mov	edx,0E6EEh		;EXT_SCISSOR_B
	VMMCall Enable_Global_Trapping
	mov	edx,0E6EFh		;EXT_SCISSOR_B
	VMMCall Enable_Global_Trapping
;
	mov	edx,0FEEEh		;LINEDRAW
	VMMCall Enable_Global_Trapping
	mov	edx,0FEEFh		;LINEDRAW
	VMMCall Enable_Global_Trapping
;
MERTExit:
	ret				;
EndProc MiniVDD_EnableTraps
;
;
public	MiniVDD_DisableTraps
BeginProc MiniVDD_DisableTraps, DOSVM
;
;This routine is called for all cards that are running this MiniVDD!
;
;See comment at EnableRegTraps.
;
;Entry:
;	Nothing assumed.
;Exit:
;	Just Preserve EBX and ESI.
;
	cmp	DOSVMCount,0		;any DOS VM's running?
	jg	@F			;yes! never disable 1CE or 1CF!
comment |
;
	mov	edx,01ceh		;ATI VGA extended register index
	VMMCall Disable_Global_Trapping
;
	mov	edx,01cfh		;ATI VGA extended register data
	VMMCall Disable_Global_Trapping
end comment |
;
	cmp	ChipID,64h		;running on a Mach64?
	jne	@F			;nope skip the following
	call	DisableMach64Ports	;go disable the trapping of Mach64 ports
;
@@:	cmp	ChipID,32h		;running on a Mach32?
	jne	MDRTExit		;nope, don't do anything here
	cmp	SuperVGAModeFlag,0	;running in 4 plane VGA mode?
	je	MDRTExit		;yes, don't do anything here
;
	mov	edx,76EEh		;GE_PITCH
	VMMCall Disable_Global_Trapping
	mov	edx,76EFh		;GE_PITCH
	VMMCall Disable_Global_Trapping
;
	mov	edx,82E8h		;CUR_Y
	VMMCall Disable_Global_Trapping
	mov	edx,82E9h		;CUR_Y
	VMMCall Disable_Global_Trapping
;
	mov	edx,82EEh		;PATT_DATA_INDEX
	VMMCall Disable_Global_Trapping
	mov	edx,82EFh		;PATT_DATA_INDEX
	VMMCall Disable_Global_Trapping
;
	mov	edx,86E8h		;CUR_X
	VMMCall Disable_Global_Trapping
	mov	edx,86E9h		;CUR_X
	VMMCall Disable_Global_Trapping
;
	mov	edx,8AE8h		;SRC_Y/DEST_Y
	VMMCall Disable_Global_Trapping
	mov	edx,8AE9h		;SRC_Y/DEST_Y
	VMMCall Disable_Global_Trapping
;
	mov	edx,8EE8h		;SRC_X/DEST_X
	VMMCall Disable_Global_Trapping
	mov	edx,8EE9h		;SRC_X/DEST_X
	VMMCall Disable_Global_Trapping
;
	mov	edx,8EEEh		;PATT_DATA
	VMMCall Disable_Global_Trapping
	mov	edx,8EEFh		;PATT_DATA
	VMMCall Disable_Global_Trapping
;
	mov	edx,92E8h		;ERR_TERM
	VMMCall Disable_Global_Trapping
	mov	edx,92E9h		;ERR_TERM
	VMMCall Disable_Global_Trapping
;
	mov	edx,9AEEh		;LINEDRAW_INDEX
	VMMCall Disable_Global_Trapping
	mov	edx,9AEFh		;LINEDRAW_INDEX
	VMMCall Disable_Global_Trapping
;
	mov	edx,0A2E8h		;BKGD_COLOR
	VMMCall Disable_Global_Trapping
	mov	edx,0A2E9h		;BKGD_COLOR
	VMMCall Disable_Global_Trapping
;
	mov	edx,0A2EEh		;LINEDRAW_OPT
	VMMCall Disable_Global_Trapping
	mov	edx,0A2EFh		;LINEDRAW_OPT
	VMMCall Disable_Global_Trapping
;
	mov	edx,0A6E8h		;FRGD_COLOR
	VMMCall Disable_Global_Trapping
	mov	edx,0A6E9h		;FRGD_COLOR
	VMMCall Disable_Global_Trapping
;
	mov	edx,0A6EEh		;DEST_X_START
	VMMCall Disable_Global_Trapping
	mov	edx,0A6EFh		;DEST_X_START
	VMMCall Disable_Global_Trapping
;
	mov	edx,0AAEEh		;DEST_X_END
	VMMCall Disable_Global_Trapping
	mov	edx,0AAEFh		;DEST_X_END
	VMMCall Disable_Global_Trapping
;
	mov	edx,0AEEEh		;DEST_Y_END
	VMMCall Disable_Global_Trapping
	mov	edx,0AEEFh		;DEST_Y_END
	VMMCall Disable_Global_Trapping
;
	mov	edx,0B2EEh		;SRC_X_START
	VMMCall Disable_Global_Trapping
	mov	edx,0B2EFh		;SRC_X_START
	VMMCall Disable_Global_Trapping
;
	mov	edx,0BAEEh		;ALU_FG_FN
	VMMCall Disable_Global_Trapping
	mov	edx,0BAEFh		;ALU_FG_FN
	VMMCall Disable_Global_Trapping
;
	mov	edx,0BEE8h		;SCISSOR...
	VMMCall Disable_Global_Trapping
	mov	edx,0BEE9h		;SCISSOR...
	VMMCall Disable_Global_Trapping
;
	mov	edx,0BEEEh		;SRC_X_END
	VMMCall Disable_Global_Trapping
	mov	edx,0BEEFh		;SRC_X_END
	VMMCall Disable_Global_Trapping
;
	mov	edx,0C2EEh		;SRC_Y_DIR
	VMMCall Disable_Global_Trapping
	mov	edx,0C2EFh		;SRC_Y_DIR
	VMMCall Disable_Global_Trapping
;
	mov	edx,0CAEEh		;SCAN_X
	VMMCall Disable_Global_Trapping
	mov	edx,0CAEFh		;SCAN_X
	VMMCall Disable_Global_Trapping
;
	mov	edx,0CEEEh		;DP_CONFIG
	VMMCall Disable_Global_Trapping
	mov	edx,0CEEFh		;DP_CONFIG
	VMMCall Disable_Global_Trapping
;
	mov	edx,0D2EEh		;PATT_LENGTH
	VMMCall Disable_Global_Trapping
	mov	edx,0D2EFh		;PATT_LENGTH
	VMMCall Disable_Global_Trapping
;
	mov	edx,0D6EEh		;PATT_INDEX
	VMMCall Disable_Global_Trapping
	mov	edx,0D6EFh		;PATT_INDEX
	VMMCall Disable_Global_Trapping
;
	mov	edx,0DAEEh		;EXT_SCISSOR_L
	VMMCall Disable_Global_Trapping
	mov	edx,0DAEFh		;EXT_SCISSOR_L
	VMMCall Disable_Global_Trapping
;
	mov	edx,0DEEEh		;EXT_SCISSOR_T
	VMMCall Disable_Global_Trapping
	mov	edx,0DEEFh		;EXT_SCISSOR_T
	VMMCall Disable_Global_Trapping
;
	mov	edx,0E2E8h		;PIX_TRANS
	VMMCall Disable_Global_Trapping
	mov	edx,0E2E9h		;PIX_TRANS
	VMMCall Disable_Global_Trapping
;
	mov	edx,0E2EEh		;EXT_SCISSOR_R
	VMMCall Disable_Global_Trapping
	mov	edx,0E2EFh		;EXT_SCISSOR_R
	VMMCall Disable_Global_Trapping
;
	mov	edx,0E6EEh		;EXT_SCISSOR_B
	VMMCall Disable_Global_Trapping
	mov	edx,0E6EFh		;EXT_SCISSOR_B
	VMMCall Disable_Global_Trapping
;
	mov	edx,0FEEEh		;LINEDRAW
	VMMCall Disable_Global_Trapping
	mov	edx,0FEEFh		;LINEDRAW
	VMMCall Disable_Global_Trapping
;
MDRTExit:
	ret				;
EndProc MiniVDD_DisableTraps
;
;
subttl		Enable Trapping For Mach64 Setup Ports
page +
public	EnableMach64Ports
BeginProc EnableMach64Ports, DOSVM
;
	mov	edx,4eech		;
	call	EnableDWordPort 	;
;
	mov	edx,52ech		;
	call	EnableDWordPort 	;
;
	mov	edx,56ech		;
	call	EnableDWordPort 	;
;
	mov	edx,5aech		;
	call	EnableDWordPort 	;
;
	mov	edx,66ech		;
	call	EnableDWordPort 	;
;
	mov	edx,6aech		;
	call	EnableDWordPort 	;
;
	mov	edx,7eech		;
	call	EnableDWordPort 	;
;
	mov	edx,6ech		;
	call	EnableDWordPort 	;
;
	mov	edx,0aech		;
	call	EnableDWordPort 	;
;
	mov	edx,0eech		;
	call	EnableDWordPort 	;
;
	mov	edx,12ech		;
	call	EnableDWordPort 	;
;
	mov	edx,16ech		;
	call	EnableDWordPort 	;
;
	mov	edx,1aech		;
	call	EnableDWordPort 	;
;
	mov	edx,1eech		;
	call	EnableDWordPort 	;
;
	mov	edx,22ech		;
	call	EnableDWordPort 	;
;
	mov	edx,26ech		;
	call	EnableDWordPort 	;
;
	mov	edx,2aech		;
	call	EnableDWordPort 	;
;
	mov	edx,2eech		;
	call	EnableDWordPort 	;
;
	mov	edx,32ech		;
	call	EnableDWordPort 	;
;
	mov	edx,36ech		;
	call	EnableDWordPort 	;
;
	mov	edx,3aech		;
	call	EnableDWordPort 	;
;
	mov	edx,3eech		;
	call	EnableDWordPort 	;
;
	mov	edx,4aech		;
	call	EnableDWordPort 	;
;
	mov	edx,5eech		;
	call	EnableDWordPort 	;
;
	mov	edx,62ech		;
	call	EnableDWordPort 	;
;
EMPExit:
	ret				;
EndProc EnableMach64Ports
;
;
subttl		Disable Trapping For Mach64 Setup Ports
page +
public	DisableMach64Ports
BeginProc DisableMach64Ports, DOSVM
;
	mov	edx,4eech		;
	call	DisableDWordPort	;
;
	mov	edx,52ech		;
	call	DisableDWordPort	;
;
	mov	edx,56ech		;
	call	DisableDWordPort	;
;
	mov	edx,5aech		;
	call	DisableDWordPort	;
;
	mov	edx,66ech		;
	call	DisableDWordPort	;
;
	mov	edx,6aech		;
	call	DisableDWordPort	;
;
	mov	edx,7eech		;
	call	DisableDWordPort	;
;
	mov	edx,6ech		;
	call	DisableDWordPort	;
;
	mov	edx,0aech		;
	call	DisableDWordPort	;
;
	mov	edx,0eech		;
	call	DisableDWordPort	;
;
	mov	edx,12ech		;
	call	DisableDWordPort	;
;
	mov	edx,16ech		;
	call	DisableDWordPort	;
;
	mov	edx,1aech		;
	call	DisableDWordPort	;
;
	mov	edx,1eech		;
	call	DisableDWordPort	;
;
	mov	edx,22ech		;
	call	DisableDWordPort	;
;
	mov	edx,26ech		;
	call	DisableDWordPort	;
;
	mov	edx,2aech		;
	call	DisableDWordPort	;
;
	mov	edx,2eech		;
	call	DisableDWordPort	;
;
	mov	edx,32ech		;
	call	DisableDWordPort	;
;
	mov	edx,36ech		;
	call	DisableDWordPort	;
;
	mov	edx,3aech		;
	call	DisableDWordPort	;
;
	mov	edx,3eech		;
	call	DisableDWordPort	;
;
	mov	edx,4aech		;
	call	DisableDWordPort	;
;
	mov	edx,5eech		;
	call	DisableDWordPort	;
;
	mov	edx,62ech		;
	call	DisableDWordPort	;
;
DMPExit:
	ret				;
EndProc DisableMach64Ports
;
;
subttl		Utility Routines for Enabling and Disabling DWord Traps
page +
public	EnableDWordPort
BeginProc EnableDWordPort, DOSVM
;
;This routine enables trapping on 4 ports beginning with the one passed in
;EDX.  No errors are returned and no registers are used except for EDX.
;
	VMMCall Enable_Global_Trapping	;
	inc	edx			;
	VMMCall Enable_Global_Trapping	;
	inc	edx			;
	VMMCall Enable_Global_Trapping	;
	inc	edx			;
	VMMCall Enable_Global_Trapping	;
	ret				;
EndProc EnableDWordPort
;
;
public	DisableDWordPort
BeginProc DisableDWordPort, DOSVM
;
;This routine disables trapping on 4 ports beginning with the one passed in
;EDX.  No errors are returned and no registers are used except for EDX.
;
	VMMCall Disable_Global_Trapping ;
	inc	edx			;
	VMMCall Disable_Global_Trapping ;
	inc	edx			;
	VMMCall Disable_Global_Trapping ;
	inc	edx			;
	VMMCall Disable_Global_Trapping ;
	ret				;
EndProc DisableDWordPort
;
;
subttl		ATI Extension Register Virtualization Handler
page +
public	MiniVDD_Virtual1CE
BeginProc MiniVDD_Virtual1CE, DOSVM
;
;This routine is called for all cards that are running this MiniVDD!
;
;The ATI BIOS's touch lots of the ATI extension registers while
;changing modes (such as from HiRes Windows mode to full-screen VGA mode).
;Therefore, when we're running in a window, and don't really want these
;extension registers to be messed with, we must virtualize them so that
;they are available to be read if necessary by the BIOS code, but not
;actually physically written to the board.  This code virtualizes the
;ATI Indexed Extension Register Pair 1CEH/1CFH and the Mach64 extension
;registers.
;
;Entry:
;	AX (or AL) contains the value to be read/written on port.
;	EBX contains the handle of the VM that is accessing the register.
;	ECX contains flags telling us what kind of I/O to do (see VMM.INC).
;	DX contains the destination port (either 1CEH or 1CFH).
;
	VxDCall VDD_Get_VM_Info 	;get CRTC owner VM handle in EDI
	cmp	edi,WindowsVMHandle	;is CRTC controlled by Windows?
	jne	MVAEPerformPhysicalIO	;nope, allow the physical I/O
	cmp	ebx,edi 		;is calling VM handle also in Windows?
	jne	MVAEVirtualize		;nope, we need to virtualize it
;
public	MVAEPerformPhysicalIO
MVAEPerformPhysicalIO:
	VxDJmp	VDD_Do_Physical_IO	;go let the Main VDD handle the I/O
;
public	MVAEVirtualize
MVAEVirtualize:
	cmp	ChipID,64h		;running on a Mach64?
	jne	@F			;nope, go virtualize 1CEH or 1CFH
	cmp	dl,0ech 		;virtualizing a Mach64 weirdo register?
	jae	MVAEExit		;yes, eat the I/O
@@:	mov	edi,ebx 		;get VM handle of caller
	add	edi,OurCBDataPointer	;EDI --> virtual registers
	cmp	ecx,Byte_Output 	;does he want to output a byte?
	je	MVAEVirtualByteOutput	;yes, go do it
	cmp	ecx,Word_Output 	;does he want to output a word?
	je	MVAEVirtualWordOutput	;yes, go do it
;
public	MVAEVirtualByteInput
MVAEVirtualByteInput:
;
;Check to see what register index we're supposed to read, or, maybe we're
;to read the index register itself!
;
	cmp	dx,1ceh 		;are we to read the index reg itself?
	jne	@F			;nope, go read the virtual data
	mov	al,[edi].ATIExIndex	;AL has the virtual index value
	jmp	MVAEExit		;and return it to caller
;
;We are to return the virtual data for the index specified in ATIExIndex.
;There are a couple of exceptions to this.  These are the read/write
;vertical timing registers 28H and 29H as well as the BIOS scratchpad
;registers 3AH and 3BH.  We also read the physical value for registers
;36H and 30H since these are part of the CRTC state and not part of the
;MemC state.
;
@@:	movzx	eax,[edi].ATIExIndex	;get virtual index value as a pointer
	cmp	al,0b6h 		;is it register 36H?
	je	MVAEDoPhysicalInput	;yes, go get real physical value
	cmp	al,0b0h 		;is it register 30H?
	je	MVAEDoPhysicalInput	;yes, go get real physical value
	cmp	al,0bah 		;is it register 3AH?
	je	MVAEDoPhysicalInput	;yes, go get real physical value
	cmp	al,0bbh 		;is it register 3BH?
	je	MVAEDoPhysicalInput	;yes, go get real physical value
	cmp	al,0a8h 		;is it register 28H?
	je	MVAEDoPhysicalInput	;yes, go get real physical value
	cmp	al,0a9h 		;is it register 29H?
	jne	@F			;nope, go do virtual input
;
public	MVAEDoPhysicalInput
MVAEDoPhysicalInput:
	dec	edx			;send index to hardware
	out	dx,al			;
	inc	edx			;and get back the physical value
	in	al,dx			;
	jmp	MVAEExit		;
;
;We're not dealing with something that requires the physical value,
;proceed with the virtualization:
;
@@:	sub	al,80h			;make index offset from zero
;
;We special case ATI extended registers 28H and 29H since these are read-only
;line counter registers which are set by the hardware and therefore have no
;virtual values.
;
	mov	al,[edi+eax]		;AL has the data to return
	jmp	MVAEExit		;and return it to caller
;
public	MVAEVirtualWordOutput
MVAEVirtualWordOutput:
;
;Save the index value passed to us in AL and the data (in AH) in the
;appropriate place:
;
	mov	[edi].ATIExIndex,al	;save the index value
	sub	al,80h			;offset index from zero
	mov	cl,ah			;hold the data to save in CL for now
	movzx	eax,al			;make index value a pointer
	mov	[edi+eax],cl		;save the data in the correct place
	jmp	MVAEExit		;and we're done
;
public	MVAEVirtualByteOutput
MVAEVirtualByteOutput:
;
;Check to see if the virtual byte output is the index register itself!
;
	cmp	edx,1ceh		;are we to output the index value?
	jne	@F			;nope, go virtually output the data
	mov	[edi].ATIExIndex,al	;yes! virtually save the index value
	jmp	MVAEExit		;and we're done
@@:	mov	cl,al			;hold the data to save in CL for now
	movzx	eax,[edi].ATIExIndex	;get the virtualized index value
	sub	al,80h			;offset index from zero
	mov	[edi+eax],cl		;save the data in the correct place
;
MVAEExit:
	ret				;
EndProc MiniVDD_Virtual1CE
;
;
subttl		Track the State of Mach32 Extension Register 4AEEH
page +
public	MiniVDD_Virtual4AEEH
BeginProc MiniVDD_Virtual4AEEH
;
;This routine is ONLY called on a Mach32!
;
;Bit 0 of this register determines whether the ATI Mach32 is in standard VGA
;or ATI HiRes mode.  We therefore need to track and save the state of this
;register in our CB data structure so that we can determine if a full-screen
;VM is in an ATI HiRes mode.  Otherwise, we just do physical OUTs and INs as
;requested.
;
;Entry:
;	AX (or AL) contains the value to be read/written on port.
;	EBX contains the handle of the VM that is accessing the register.
;	ECX contains flags telling us what kind of I/O to do (see VMM.INC).
;	DX contains the destination port (either 4AEEH or 4AEFH).
;
	cmp	InCRTCModeChange,0	;do we want to virtualize?
	jne	MV4DoPhysicalIO 	;nope, skip it
	mov	edi,ebx 		;make EDI --> our CB data area
	add	edi,OurCBDataPointer	;
	test	ecx,OUTPUT		;doing output to the port?
	jz	MV4DoPhysicalIO 	;nope, we don't care about it
	cmp	ecx,WORD_OUTPUT 	;doing a word OUT to 4AEEH?
	jne	MV4ByteOUT		;nope, try something else
;
public	MV4WordOUT
MV4WordOUT:
	mov	[edi].ATI4AEEState,ax	;save the state
	jmp	MV4DoPhysicalIO 	;and we're done processing it
;
public	MV4ByteOUT
MV4ByteOUT:
	cmp	ecx,BYTE_OUTPUT 	;doing a byte OUT to 4AEEH?
	jne	MV4DoPhysicalIO 	;nope, we don't understand this!
	add	edi,ATI4AEEState	;EDI --> place to save 4AEEH state
	cmp	dl,0eeh 		;doing a byte OUT to 4AEEH?
	je	@F			;yes, go save the byte at EDI
	inc	edi			;doing byte OUT to 4AEFH
@@:	mov	[edi],al		;save the state of the byte OUT
;
public	MV4DoPhysicalIO
MV4DoPhysicalIO:
	VxDjmp	VDD_Do_Physical_IO	;go do the physical I/O
EndProc MiniVDD_Virtual4AEEH
;
;
subttl		Track the State of Mach32 Extension Register 4AE8H
page +
public	MiniVDD_Virtual4AE8H
BeginProc MiniVDD_Virtual4AE8H
;
;This routine is ONLY called on a Mach32!
;
;Bit 0 of this register determines whether the ATI Mach32 is in standard VGA
;or 8514 HiRes mode.  We therefore need to track and save the state of this
;register in our CB data structure so that we can determine if a full-screen
;VM is in an 8514 HiRes mode.  Otherwise, we just do physical OUTs and INs as
;requested.
;
;Entry:
;	AX (or AL) contains the value to be read/written on port.
;	EBX contains the handle of the VM that is accessing the register.
;	ECX contains flags telling us what kind of I/O to do (see VMM.INC).
;	DX contains the destination port (either 4AE8H or 4AE9H).
;
	cmp	InCRTCModeChange,0	;do we want to virtualize?
	jne	MV4ADoPhysicalIO	;nope, skip it
	mov	edi,ebx 		;make EDI --> our CB data area
	add	edi,OurCBDataPointer	;
	test	ecx,OUTPUT		;doing output to the port?
	jz	MV4ADoPhysicalIO	;nope, we don't care about it
	cmp	ecx,WORD_OUTPUT 	;doing a word OUT to 4AE8H?
	jne	MV4AByteOUT		;nope, try something else
;
public	MV4AWordOUT
MV4AWordOUT:
	mov	[edi].ATI4AE8State,ax	;save the state
	jmp	MV4ADoPhysicalIO	;and we're done processing it
;
public	MV4AByteOUT
MV4AByteOUT:
	cmp	ecx,BYTE_OUTPUT 	;doing a byte OUT to 4AE8H?
	jne	MV4ADoPhysicalIO	;nope, we don't understand this!
	add	edi,ATI4AE8State	;EDI --> place to save 4AE8H state
	cmp	dl,0e8h 		;doing a byte OUT to 4AE8H?
	je	@F			;yes, go save the byte at EDI
	inc	edi			;doing byte OUT to 4AE9H
@@:	mov	[edi],al		;save the state of the byte OUT
;
public	MV4ADoPhysicalIO
MV4ADoPhysicalIO:
	VxDjmp	VDD_Do_Physical_IO	;go do the physical I/O
EndProc MiniVDD_Virtual4AE8H
;
;
subttl		Set Hardware to a Not Busy State
page +
public	MiniVDD_MakeHardwareNotBusy
BeginProc MiniVDD_MakeHardwareNotBusy, DOSVM
;
;This routine is only called on the Mach32!
;
;Quite often, we need to make sure that the hardware state is not busy
;before changing the MemC mode from the Windows HiRes state to VGA state
;or vice-versa.  This routine allows you to do this (to the best of your
;ability).  You should try to return a state where the hardware BLTer
;isn't busy.
;
;Entry:
;	EAX contains the CRTC owner's VM handle.
;	EBX contains the currently running VM handle.
;	ECX contains the MemC owner's VM handle.
;	EDX contains the CRTC index register.
;Exit:
;	You must save all registers that you destroy except for EAX & ECX.
;
	push	edx			;save registers that we use for test
	cmp	eax,WindowsVMHandle	;is CRTC VM Windows?
	jne	MHNBBoardNotBusy	;nope, skip this entire mess!
	cmp	SuperVGAModeFlag,0	;running in 4 plane VGA mode?
	je	MHNBBoardNotBusy	;yes, don't do anything here
;
;Now, perform the actual test for board-not-busy:
;
	mov	ecx,1000h		;this is a safety net loop counter
;
MHNBBoardBusyCheck:
        mov     dx,9aeeh                ;IO_EXT_FIFO_STATUS
        in      ax,dx
        test    ax,0ffffh               ;SIXTEEN_WORDS
        jnz     @F
        mov     dx,062eeh               ;IO_EXT_GE_STATUS
        in      ax,dx
        test    ax,02000h               ;GE_ACTIVE
        jnz     @F
        mov     dx,09AE8h               ;IO_GE_STAT
        in      ax,dx
        test    ax,00200h               ;GE_BUSY
        jz      MHNBBoardNotBusy
@@:
	call	Delay			;give it a rest to let it finish
	dec	ecx			;bump down our loop counter
	jnz	MHNBBoardBusyCheck	;wait for it to finish
;
public	MHNBBoardNotBusy
MHNBBoardNotBusy:
	pop	edx			;restore CRTC port number
;
MHNBExit:
	ret				;
EndProc MiniVDD_MakeHardwareNotBusy
;
;
BeginProc Delay, DOSVM
;
;This process delays for a while so as to let the ATI BLTer finish its work:
;
	push	ecx
	mov	ecx,1000
@@:	push	ecx
	pop	ecx
	loop	@B
	pop	ecx
	ret
EndProc Delay
;
;
subttl		Routine Called When VM Does an Int 10H Mode Set
page +
public	MiniVDD_PreInt10ModeSet
BeginProc MiniVDD_PreInt10ModeSet, DOSVM
;
;Entry:
;	EAX contains the mode that we're setting to (in AL).
;	EBX contains the VM handle doing the mode set.
;	EBP --> VM's Client Registers.
;Exit:
;	Preserve everything that's used.
;
;This routine is called whenever a VM (including the Windows VM) does an
;INT 10H, function 0 mode set call.  The MiniVDD uses this routine to
;do anything necessary to implement the mode set.  In the case of the Mach32
;where the ROM BIOS itself doesn't take the screen out of HiRes mode (if it's
;currently in a Mach32 HiRes mode), we take care of restoring the screen to
;VGA mode.  This routine is called regardless of whether the VM is windowed
;or full-screen.  We therefore must be careful not to take the screen out
;of HiRes mode if this is a windowed VM!
;
;Record the mode number in our CB data area:
;
	push	edi			;save everything that we use
	push	esi			;
	mov	edi,ebx 		;make EDI --> our CB data area
	add	edi,OurCBDataPointer	;
	mov	[edi].CurrentMode,ax	;
;
;Unless we're running on a Mach8 or Mach32, we don't need to do anything else:
;
	cmp	ChipID,08h		;running on a Mach8?
	je	@F			;yes, continue
	cmp	ChipID,32h		;running on a Mach32?
	jne	MPIMCommon		;nope, skip SetVGAPassthrough stuff!
;
;If we're not the CRTC owner, don't change the state of the CRTC!
;
@@:	VxDCall VDD_Get_VM_Info 	;get CRTC owner VM handle in EDI
	cmp	ebx,edi 		;do we currently own the CRTC?
	jne	MPIMCommon		;no, don't call SetVGAPassthrough!
	and	al,NOT 80h		;get rid of "don't erase screen" bit
	cmp	al,13h			;going to a LoRes VGA mode?
	ja	MPIMCommon		;no, don't call SetVGAPassthrough!
;
public	MPIMSetVGAPassthrough
MPIMSetVGAPassthrough:
	pushad				;better save everything over this
	call	SetVGAPassthrough	;go set ourselves into VGA mode
	popad				;restore saved registers
;
MPIMCommon:
	pop	esi			;restore saved registers
	pop	edi			;
;
MPIMExit:
	ret					;
EndProc MiniVDD_PreInt10ModeSet
;
;
subttl		Return Total Memory Size on Card
page +
public	MiniVDD_GetTotalVRAMSize
BeginProc MiniVDD_GetTotalVRAMSize, DOSVM
;
;This routine is called only on the Mach32 and Mach64.
;
;Entry:
;	EBX contains the Current VM Handle (which is also the CRTC owner's
;	VM Handle).
;	EBP --> VM's Client Registers.
;Exit:
;	CY is returned upon success.
;	All registers (except ECX) must be preserved over the call.
;	ECX will contain the total VRAM size in bytes.
;
	mov	ecx,TotalMemorySize	;
	stc				;
	ret				;return to caller
EndProc MiniVDD_GetTotalVRAMSize
;
;
subttl		Return the Bank Size
page +
public	MiniVDD_GetBankSize
BeginProc MiniVDD_GetBankSize, DOSVM
;
;This routine is called on the Mach32 and Mach64.
;
;Entry:
;	EBX contains the VM Handle (Always the "CurrentVM").
;	ECX contains the BIOS mode number that we're currently in.
;	EBP --> Client Register structure for VM
;Exit:
;	CY is returned upon success.
;	All registers (except EDX & EAX) must be preserved over the call.
;	EDX will contain the current bank size.
;	EAX will contain the physical address of the memory aperture or
;		zero to indicate VRAM at A000H.
;
;For the Mach32 & Mach64, our bank size is ALWAYS 64K.
;
	mov	edx,64*1024		;assume 64K banks at A000H
	xor	eax,eax 		;indicate a 64K bank at A000H
;
MGBSExit:
	stc				;return success to caller
	ret				;
EndProc MiniVDD_GetBankSize
;
;
subttl		Check To See If We're Running a non-VESA HiRes Mode
page +
public	MiniVDD_CheckScreenSwitchOK
BeginProc MiniVDD_CheckScreenSwitchOK
;
;This routine is called only on a Mach32.
;
;This routine is called by the Main VDD if we're trying to switch away from a
;full-screen DOS box which may or may not be running in an ATI HiRes mode.
;
;Entry:
;	EAX contains 0 if not a VESA mode or -1 if it is a VESA mode.
;	EBX contains the VM Handle.
;	ECX contains the INT 10H mode number.
;Exit:
;	EBX & EDI must be preserved.
;	CY returned indicates that screen switch is to be disallowed
;	If NC returned:
;		If we're in a VESA mode -- screen switch will occur.
;		If we're in a standard VGA mode -- screen switch will occur.
;		If we return 0 in EAX -- screen switch will occur.
;		If we return non-zero in EAX -- screen switch is disallowed.
;
	or	eax,eax 		;running a VESA mode?
	jnz	MCSOAllowScreenSwitch	;yes, unconditionally allow switch
	call	MiniVDD_CheckHiResMode	;go determine if we're in HiRes mode
	jc	MCSODisallowScreenSwitch;we're in a non-VESA HiRes mode
;
public	MCSOAllowScreenSwitch
MCSOAllowScreenSwitch:
	clc				;we're in a regular VGA mode
	jmp	MCSOExit		;
;
public	MCSODisallowScreenSwitch
MCSODisallowScreenSwitch:
	stc				;we disllow screen switches
;
MCSOExit:
	ret				;
EndProc MiniVDD_CheckScreenSwitchOK
;
;
subttl		Determine If VM Is Running a non-VESA HiRes Mode
page +
public	MiniVDD_CheckHiResMode
BeginProc MiniVDD_CheckHiResMode
;
;Entry:
;	EBX contains the VM Handle for the inquiry.
;Exit:
;	Preserve all registers that you use.
;	CY returned if VM is in a HiRes mode.
;	NC returned if VM is in a VGA standard mode.
;
;Just check the CurrentMode variable in the CB data area to see if we're
;in a HiRes mode or not:
;
	push	eax			;save registers that we use
	push	edi			;
	mov	edi,ebx 		;make EDI --> our CB data area
	add	edi,OurCBDataPointer	;
	mov	ax,[edi].CurrentMode	;get the mode that we're in
	and	ax,NOT 0c080h		;get rid of special flags in mode nbr
	cmp	ax,13h			;are we in a standard VGA mode?
	ja	MCHMInHiResMode 	;nope, return that we're HiRes
	mov	al,byte ptr [edi].ATI4AEEState
	or	al,byte ptr [edi].ATI4AE8State
	test	al,01h			;are we in an ATI HiRes mode?
	jnz	MCHMInHiResMode 	;yes, go indicate it to caller
;
MCHMNotInHiResMode:
	clc				;we're not in a HiRes mode
	jmp	MCHMExit		;
;
MCHMInHiResMode:
	stc				;
;
MCHMExit:
	pop	edi			;restore saved registers
	pop	eax			;
	ret				;
EndProc MiniVDD_CheckHiResMode
;
;
subttl		Prepare For a Mach32 HiRes Screen Save/Restore
page +
public	MiniVDD_PreHiResSaveRestore_32
BeginProc MiniVDD_PreHiResSaveRestore_32, DOSVM
ife	MACH32VESASupport
;
;Entry:
;	EBX contains the VM Handle (Always the "CurrentVM").
;	ECX contains the BIOS mode number that we're currently in.
;	EBP --> Client Register structure for VM
;Exit:
;	CY is returned upon success.
;	All registers must be preserved over the call.
;
;For ATI, all we need to make sure of is that we are using the single bank
;mode (ie: the write bank bits control both read and write banking).
;
	push	esi			;
	push	eax			;
	push	edx			;
	mov	esi,ebx 		;make ESI --> VM's CB data area
	add	esi,OurCBDataPointer	;
	mov	edx,1ceh		;EDX --> ATI index register
	in	al,dx			;save current state of ATI index reg
	ror	eax,8			;
;
	mov	al,0beh 		;save state of bank control register BEH
	out	dx,al			;
	inc	dl			;EDX --> ATI Data register
	in	al,dx			;
	mov	[esi].ATIEx3e,al	;
	and	al,NOT 08h		;set common read/write banking
	out	dx,al			;and write the new value to the card
	dec	dl			;EDX --> ATI Index register
;
	rol	eax,8			;restore saved ATI index register
	out	dx,al			;
	pop	edx			;restore saved registers
	pop	eax			;
	pop	esi			;
	stc				;return success to caller
	ret				;
endif	;MACH32VESASupport
EndProc MiniVDD_PreHiResSaveRestore_32
;
;
subttl		Clean Up After a Mach32 HiRes Screen Save/Restore
page +
public	MiniVDD_PostHiResSaveRestore_32
BeginProc MiniVDD_PostHiResSaveRestore_32, DOSVM
ife	MACH32VESASupport
;
;Entry:
;	EBX contains the VM Handle (Always the "CurrentVM").
;	ECX contains the BIOS mode number that we're currently in.
;	EBP --> Client Register structure for VM
;Exit:
;	CY is returned upon success.
;	All registers must be preserved over the call.
;
;For ATI, all we need to do is restore the state(s) saved above.
;
	push	esi			;
	push	eax			;
	push	edx			;
	mov	esi,ebx 		;make ESI --> VM's CB data area
	add	esi,OurCBDataPointer	;
	mov	edx,1ceh		;EDX --> ATI index register
	in	al,dx			;save current state of ATI index reg
	ror	eax,8			;
;
	mov	al,0beh 		;restore state of bank control reg BEH
	mov	ah,[esi].ATIEx3e	;
	out	dx,ax			;
;
	rol	eax,8			;restore saved ATI index register
	out	dx,al			;
	pop	edx			;restore saved registers
	pop	eax			;
	pop	esi			;
	stc				;return success to caller
	ret				;
endif	;MACH32VESASupport
EndProc MiniVDD_PostHiResSaveRestore_32
;
;
subttl		Set the Bank During a Mach32 HiRes Save/Restore
page +
public	MiniVDD_SetBank_32
BeginProc MiniVDD_SetBank_32, DOSVM
;
;Entry:
;	EAX contains the read bank to set.
;	EDX contains the write bank to set.
;	EBX contains the VM Handle (Always the "CurrentVM").
;	EBP --> Client Register structure for VM
;Exit:
;	CY is returned upon success.
;	All registers must be preserved over the call.
;	EDX will contain the current write bank (bank A) set in hardware.
;
	push	edx			;
	push	eax			;
	push	ecx			;
	mov	cl,dl			;save write bank in CL
	mov	ch,al			;save read bank in CH
	mov	edx,1ceh		;EDX --> ATI index register
	in	al,dx			;get and save current ATI index
	ror	eax,8			;
;
;Combine the bits to get the value for the low banking register:
;
	mov	ax,cx			;get write bank in AL, read bank in AH
	and	ax,070fh		;get low 4 bits of write bank
					;get low 3 bits of read bank
	shl	al,1			;get the low write bank into position
	shl	ah,5			;get the low read bank into position
	or	ah,al			;now AH has the high banking bits
	mov	al,0b2h 		;set to index B2H
	out	dx,ax			;
;
;Combine the bits to get the value for the high banking register:
;
	mov	ax,cx			;get write bank in AL, read bank in AH
	and	ax,1830h		;get high 2 bits of write bank
					;get high 2 bits of read bank
	shr	al,4			;get the high write bank into position
	shr	ah,1			;get the high read bank into position
	or	ah,al			;now AH has the high banking bits
	mov	al,0aeh 		;set to index AEH
	out	dx,al			;
	inc	dl			;EDX --> ATI data register
	in	al,dx			;get current value of high bank register
	and	al,0f0h 		;mask to preserve non-banking bits
	or	al,ah			;now AL has value for register AEH
	out	dx,al			;
	dec	dl			;EDX --> ATI index register
;
	rol	eax,8			;restore ATI index register
	out	dx,al			;
	pop	ecx			;restore saved registers
	pop	eax			;
	pop	edx			;
	stc				;return success to caller
	ret				;
EndProc MiniVDD_SetBank_32
;
;
subttl		Get Mach32 or Mach64 VESA Write Bank
page +
public	MiniVDD_GetCurrentBankWrite
BeginProc MiniVDD_GetCurrentBankWrite, DOSVM
;
;This routine is only called on the Mach32 or Mach64.
;
;Entry:
;	EBX contains the VM Handle (Always the current VM).
;Exit:
;	DX contains the current write bank set to the hardware.
;	EAX & EDX may be destroyed by the call.
;
;The ATI VESA BIOS messes up the VESA call 4F05H (Get Current Bank).
;We therefore have to do it properly here.
;
;First, see if we're using the extended banking registers, or the old
;fashioned banking registers (1CEH).  We do this by reading 6AECH, bit 2:
;
	cmp	ChipID,32h		;running on a Mach32?
	je	GCBWGetOldStyleBank	;yes, no new style banking on Mach32
	mov	edx,6aech		;EDX --> CONFIG_CNTL
	in	al,dx			;get the current value
	test	al,04h			;using the old style VGA aperture?
	jz	GCBWGetOldStyleBank	;yes, go get the old style bank
;
public	GCBWGetNewStyleBank
GCBWGetNewStyleBank:
	mov	dh,56h			;EDX --> MEM_VGA_WP_SEL (56EC)
	in	al,dx			;get the current write bank
	shr	al,1			;their banks are 32K each
	jmp	GCBWExit		;and we're done!
;
public	GCBWGetOldStyleBank
GCBWGetOldStyleBank:
	mov	edx,1ceh		;EDX --> ATI extended index register
	in	al,dx			;get current index
	ror	eax,8			;save it in high byte of EAX
;
public	GCBWGetReadBankEntry
GCBWGetReadBankEntry:
;
;We jump here from MiniVDD_GetCurrentBankRead if we're not using
;separate read/write banks:
;
	mov	al,0b2h 		;set to ATI low banking register
	out	dx,al			;
	inc	dl			;EDX --> ATI extended data register
	in	al,dx			;get the low bits of the bank
	mov	ah,al			;save it for now in AH
	dec	dl			;EDX --> ATI extended index register
	mov	al,0aeh 		;set to ATI high banking register
	out	dx,al			;
	inc	dl			;EDX --> ATI extended data register
	in	al,dx			;get the high bits of the bank
	dec	dl			;EDX --> ATI extended index register
	shr	ah,1			;get low write bank bits into position
	shl	al,4			;get high write bank bits into position
	and	ax,0f30h		;get rid of extraneous bits
	or	al,ah			;now AL has the bank
	rol	eax,8			;restore the old index
	out	dx,al			;
	mov	al,ah			;now AL has the bank
;
GCBWExit:
	movzx	edx,al			;return bank nbr in EDX
	stc				;indicate that we handled the call
	ret				;
EndProc MiniVDD_GetCurrentBankWrite
;
;
subttl		Get Mach32 or Mach64 VESA Read Bank
page +
public	MiniVDD_GetCurrentBankRead
BeginProc MiniVDD_GetCurrentBankRead, DOSVM
;
;This routine is only called on the Mach32 or Mach64.
;
;Entry:
;	EBX contains the VM Handle (Always the current VM).
;Exit:
;	EDX contains the current read bank set to the hardware.
;	EAX & EDX may be destroyed by the call.
;
;The ATI VESA BIOS messes up the VESA call 4F05H (Get Current Bank).
;We therefore have to do it properly here.
;
;First, see if we're using the extended banking registers, or the old
;fashioned banking registers (1CEH).  We do this by reading 6AECH, bit 2:
;
	cmp	ChipID,32h		;running on a Mach32?
	je	GCBRGetOldStyleBank	;yes, no new style banking on Mach32
	mov	edx,6aech		;EDX --> CONFIG_CNTL
	in	al,dx			;get the current value
	test	al,04h			;using the old style VGA aperture?
	jz	GCBRGetOldStyleBank	;yes, go get the old style bank
;
public	GCBRGetNewStyleBank
GCBRGetNewStyleBank:
	mov	dh,5ah			;EDX --> MEM_VGA_RP_SEL (5AEC)
	in	al,dx			;get the current read bank
	shr	al,1			;their banks are 32K each
	jmp	GCBRExit		;and we're done!
;
public	GCBRGetOldStyleBank
GCBRGetOldStyleBank:
	mov	edx,1ceh		;EDX --> ATI extended index register
	in	al,dx			;get current index
	ror	eax,8			;save it in high byte of EAX
;
;We need to see if we are using separate read and write banks.	If we aren't,
;we simply jump to MiniVDD_GetCurrentBankWrite (since the code is the same)
;
	mov	al,0beh 		;set to banking control register
	out	dx,al			;
	inc	dl			;EDX --> ATI extended data register
	in	al,dx			;get the banking control value
	dec	dl			;EDX --> ATI extended index register
	test	al,08h			;using separate read/write banks?
	jz	GCBWGetReadBankEntry	;no, just do the same thing as write!
;
public	GCBRSeparateReadBank
GCBRSeparateReadBank:
	mov	al,0b2h 		;set to ATI low banking register
	out	dx,al			;
	inc	dl			;EDX --> ATI extended data register
	in	al,dx			;get the low bits of the bank
	mov	ah,al			;save it for now in AH
	dec	dl			;EDX --> ATI extended index register
	mov	al,0aeh 		;set to ATI high banking register
	out	dx,al			;
	inc	dl			;EDX --> ATI extended data register
	in	al,dx			;get the high bits of the bank
	dec	dl			;EDX --> ATI extended index register
	shr	ah,5			;get low read bank bits into position
	shl	al,1			;get high read bank bits into position
	and	ax,0718h		;get rid of extraneous bits
	or	al,ah			;now AL has the bank
	rol	eax,8			;restore the old index
	out	dx,al			;
	mov	al,ah			;now AL has the bank
;
GCBRExit:
	movzx	edx,al			;return bank nbr in EDX
	stc				;indicate that we handled the call
	ret				;
EndProc MiniVDD_GetCurrentBankRead
;
;
subttl		Mach32 VESA Tables
page +
BeginProc VESADataTables, DOSVM
if MACH32VESASupport
;
public	VESADispatchTable
VESADispatchTable	label	dword
	dd	OFFSET32 VESAFunc0
	dd	OFFSET32 VESAFunc1
	dd	OFFSET32 VESAFunc2
	dd	OFFSET32 VESAFunc3
	dd	OFFSET32 VESAFunc4
	dd	OFFSET32 VESAFunc5
	dd	OFFSET32 VESAFunc6
	dd	OFFSET32 VESAFunc7
	dd	OFFSET32 VESAFunc8
	dd	OFFSET32 VESAFunc9
	dd	OFFSET32 VESAFuncA
LAST_VESA_FUNC	 equ	 (($-OFFSET32 VESADispatchTable) / 4) - 1
;
;
public	VESAModesTable
VESAModesTable		label	word
	dw	100h			;640x400x8
	dw	101h			;640x480x8
	dw	103h			;800x600x8
	dw	105h			;1024x768x8
	dw	107h			;1280x1024x8
	dw	111h			;640x480x16 (5:6:5)
	dw	114h			;800x600x16 (5:6:5)
	dw	117h			;1024x768x16 (5:6:5)
NbrVESAModes		equ	($ - OFFSET32 VESAModesTable) / 2
	dw	-1			;terminator
VESAModesTableLen	equ	$ - OFFSET32 VESAModesTable
;
;
VESAModeEntry		struc
	ScreenWidthInPixels	dw	?
	ScreenPitch		dw	?
	ScreenHeight		dw	?
	ScreenBPP		db	?
	ShadowSetModeNbr	db	?
	PitchCode		db	?
	BPPCode 		db	?
	VGAModeNbr		db	?
	VESA_Filler		db	5 dup(?)
VESAModeEntry		ends
.errnz	SIZE VESAModeEntry - 16
;
;
VESAModeLookupTable	label	dword
Mode100H  VESAModeEntry <640, 640, 400, 8, 0, 0, 0, 61h>
Mode101H  VESAModeEntry <640, 640, 480, 8, 0, 0, 0, 62h>
Mode102H  VESAModeEntry <>
Mode103H  VESAModeEntry <800, 800, 600, 8, 0, 0, 0, 63h>
Mode104H  VESAModeEntry <>
Mode105H  VESAModeEntry <1024, 1024, 768, 8, 0, 0, 0, 64h>
Mode106H  VESAModeEntry <>
Mode107H  VESAModeEntry <>
Mode108H  VESAModeEntry <>
Mode109H  VESAModeEntry <>
Mode10AH  VESAModeEntry <>
Mode10BH  VESAModeEntry <>
Mode10CH  VESAModeEntry <>
Mode10DH  VESAModeEntry <>
Mode10EH  VESAModeEntry <>
Mode10FH  VESAModeEntry <>
Mode110H  VESAModeEntry <>
Mode111H  VESAModeEntry <640, 1280, 480, 16, 21h, 1, 0dh, 62h>
Mode112H  VESAModeEntry <>
Mode113H  VESAModeEntry <>
Mode114H  VESAModeEntry <800, 1664, 600, 16, 01h, 1, 0dh, 63h>
Mode115H  VESAModeEntry <>
Mode116H  VESAModeEntry <>
Mode117H  VESAModeEntry <1024, 2048, 768, 16, 41h, 1, 0dh, 64h>
HighestMode	equ (($ - OFFSET32 VESAModeLookupTable) / SIZE VESAModeEntry) - 1 + 100h
endif	;MACH32VESASupport
;
EndProc VESADataTables
;
;
subttl		Dispatch To Proper VESA Handling Routine
page +
public	MiniVDD_VESASupport
BeginProc	MiniVDD_VESASupport, DOSVM
;
;This routine is only called on Mach 32's!
;
;Entry:
;	AX contains the VESA function code.
;	EBX contains the VM handle that wants to do the VESA call.
;	EBP --> VM's Client Registers.
;Exit:
;	CY returned means that MiniVDD has handled the VESA call completely.
;
;This is the dispatch point for VESA handling on the Mach32 under Win '95.  We
;dispatch to each routine as defined by the VESA spec.
;
if MACH32VESASupport
	pushad				;
;
;If the application calls VESA function 4F13H -- the VESA audio interface
;(specifically, Nickelodeon's "Are You Afraid of the Dark" app calls this),
;we want to chain to the VESA.COM hopefully provided by the application since
;we assume that if it's calling 4F13H, it knows what it's doing.  Therefore,
;we return that we didn't handle the call by setting CY clear:
;
	cmp	al,13h			;calling VESA sound interface 4F13H?
	je	MVVSExit		;yes, set CY clear -- we don't handle it
	cmp	al,LAST_VESA_FUNC	;is it a valid function?
	ja	MVVSBadFunctionCode	;nope, just return failure to caller
	movzx	eax,al			;make sure top word of EAX is 0
	call	[(eax*4)+OFFSET32 VESADispatchTable]
					;call the function to do
	jmp	MVVSWeHandledCall	;and we're done!
;
public	MVVSBadFunctionCode
MVVSBadFunctionCode:
	mov	[ebp].Client_AX,0100h	;indicate that function code is bad
;
public	MVVSWeHandledCall
MVVSWeHandledCall:
	stc				;indicate we handled VESA call
;
public	MVVSExit
MVVSExit:
	popad				;
	ret				;and return to caller
endif	;MACH32VESASupport
EndProc MiniVDD_VESASupport
;
;
subttl		Support For VESA Function 0 -- Return Controller Info
page +
public	VESAFunc0
BeginProc VESAFunc0, DOSVM
if MACH32VESASupport
;
;Entry:
;	EBX contains the VM handle.
;	EBP --> Client Registers.
;	Client_ES:Client_DI --> Seg:Off addr of buffer to stuff data into.
;Exit:
;	Client_AX contains the return status from the call.
;
;First, get the linear address of the guy's VBE information block:
;
	mov	ah,Client_ES		;get client register equates in
	mov	al,Client_DI		;
	VMMCall Map_Flat		;EAX contains the linear address
	cmp	eax,-1			;did the call fail?
	je	VF0Failure		;yep, return it to caller
;
;Check to see if he wants VBE 2.0 data or VBE 1.0 data by seeing the
;VbeSignature that he passed down.  If it's 'VBE2', then he's passed a
;512 byte buffer and wants VBE 2.0 extensions.	If not, we assume that
;he only wants VBE 1.0 stuff.
;
	cld				;set direction forward
	mov	cl,1			;assume he wants only VBE 1.0 stuff
	mov	edx,dword ptr [eax].VbeSignature
					;get the VbeSignature string
	cmp	dl,'V'			;is it what we want?
	je	@F			;yes, continue
	cmp	dl,'v'			;is it what we want?
	jne	VF0GotVersion		;nope, he wants VBE 1.0
;
@@:	cmp	dh,'B'			;is it what we want?
	je	@F			;yes, continue
	cmp	dh,'b'			;is it what we want?
	jne	VF0GotVersion		;nope, he wants VBE 1.0
;
@@:	shr	edx,16			;get access to high word
	cmp	dl,'E'			;is it what we want?
	je	@F			;yes, continue
	cmp	dl,'e'			;is it what we want?
	jne	VF0GotVersion		;nope, he wants VBE 1.0
;
@@:	cmp	dh,'2'			;is it what we want?
	jne	VF0GotVersion		;nope, he wants VBE 1.0
	mov	cl,2			;yes! he wants VBE 2.0
;
public	VF0GotVersion
VF0GotVersion:
;
;At this point:
;	CL contains 1 for VBE 1.0 info or 2 for VBE 2.0 info.
;
;Now, put the string "VESA" into VbeSignature:
;
	mov	dword ptr [eax].VbeSignature,'ASEV'
;
;And the version into VbeVersion:
;
	mov	[eax].VbeVersion,0200h
;
;Put the string 'ATI' into the OemStringPtr area and establish a pointer
;to it.
;
	mov	si,[ebp].Client_ES	;get segment addr of OemString
	shl	esi,16			;
	lea	edx,[eax].VbeV1Scratch	;assume caller passed a VBE1 structure
	mov	si,VbeV1Scratch 	;
	cmp	cl,1			;did he pass a VBE 1 structure?
	je	@F			;yes, EDX --> place to put string
	lea	edx,[eax].VESAOemData	;it's a VBE 2 structure
	mov	si,VESAOemData		;
@@:	mov	[eax].OemStringPtr,esi	;stuff the address of the Oem string
	mov	dword ptr [edx],'ITA'	;
	mov	byte ptr [edx+3],0	;NULL terminate the string
;
;Set the capabilities for the Mach32 as 6 bit fixed DAC, NOT VGA compatible,
;Normal RAMDAC operation:
;
	mov	dword ptr [eax].VESACaps,02h
;
;Now, construct the video mode table.  For VESA 1.0 structures, we place this
;in the fourth byte of the VbeV1Scratch area.  For VESA 2.0 structures,
;we place this in the VESAReserved area.
;
	push	ecx			;save the structure type in CL
	mov	si,[ebp].Client_ES	;get segment addr of Modes list
	shl	esi,16			;
	lea	edi,[eax].VbeV1Scratch+4;assume caller passed a VBE1 structure
	mov	si,VbeV1Scratch+4	;
	cmp	cl,1			;did he pass a VBE 1 structure?
	je	@F			;yes, EDI --> place to put Modes list
	lea	edi,[eax].VESAReserved	;it's a VBE 2 structure
	mov	si,VESAReserved 	;
@@:	mov	[eax].VideoModePtr,esi	;stuff the address of the Modes list
	mov	esi,OFFSET32 VESAModesTable
	mov	ecx,VESAModesTableLen	;
	rep	movsb			;
	pop	ecx			;restore structure type to CL
;
;Next, the VESA total memory size in number of 64K blocks available:
;
	mov	edx,TotalMemorySize	;
	shr	edx,16			;divide by 64K
	mov	[eax].VESATotalMemory,dx;
;
;If we were passed a VESA 1 structure, we're done!
;
	cmp	cl,1			;passed a VBE 1.0 structure?
	je	VF0GoodExit		;yes, we're done!
;
;Fill in the OemSoftwareRev as VBE version 2.0:
;
	mov	[eax].OemSoftwareRev,0200h
;
;We're pretty sure that VESA authors don't care too much about the
;vendor name, product name, or OEM Product rev information.  Just pass
;back NULL pointers for this:
;
	mov	[eax].OemVendorNamePtr,0
	mov	[eax].OemProductNamePtr,0
	mov	[eax].OemProductRevPtr,0
;
public	VF0GoodExit
VF0GoodExit:
	mov	[ebp].Client_AX,4fh	;return success!
	jmp	VF0Exit 		;
;
public	VF0Failure
VF0Failure:
	mov	[ebp].Client_AX,014fh	;return failure to caller
;
VF0Exit:
	ret				;return to dispatch routine
endif	;MACH32VESASupport
EndProc VESAFunc0
;
;
subttl		Support For VESA Function 1 -- Return Mode Info
page +
public	VESAFunc1
BeginProc VESAFunc1, DOSVM
;
;Entry:
;	EBX contains the VM handle.
;	EBP --> Client Registers.
;	Client_CX contains the mode number to query.
;	Client_ES:Client_DI --> Seg:Off addr of buffer to stuff data into.
;Exit:
;	Client_AX contains the return status from the call.
;
;First, get the linear address of the guy's VBE information block:
;
if MACH32VESASupport
	mov	ah,Client_ES		;get client register equates in
	mov	al,Client_DI		;
	VMMCall Map_Flat		;EAX contains the linear address
	cmp	eax,-1			;did the call fail?
	je	VF1Failure		;yep, return it to caller
;
;Clear the mode information block to all zeroes:
;
	cld				;set direction forward
	push	eax			;save EAX --> Mode Information Block
	mov	edi,eax 		;EDI --> Mode Information Block
	xor	eax,eax 		;
	mov	ecx,256/4		;clear 256 bytes
	rep	stosd			;
;
;See if this mode is one of those that we might support:
;
	mov	ax,[ebp].Client_CX	;AX has the mode that we want
	and	ax,NOT 0c080h		;get rid of "don't erase screen" bits
	mov	ecx,NbrVESAModes	;this is nbr that we potentially support
	mov	edi,OFFSET32 VESAModesTable
	repne	scasw			;
	movzx	ecx,ax			;copy mode number into ECX
	pop	eax			;restore EAX --> Mode Information Block
	jne	VF1Failure		;mode wasn't in table, fail the call
;
;We setup the default attributes for the Mode Attributes flag.	We also
;assume (until proven otherwise), that we support the mode.
;
	or	[eax].VModeAttributes,3bh
;
public	VF1CheckMemory
VF1CheckMemory:
;
;If we're running in 512K, we can only do 640x480 and  800x600:
;
	cmp	cl,03h			;running above 800x600x8?
	jbe	VF1FillInWinAttributes	;nope, we can run on 512K
	cmp	TotalMemorySize,1024*1024
					;do we have at least 1 meg?
	jb	VF1ModeNotSupported	;nope, we can't support this mode
	cmp	cl,11h			;running 640x480x16 or below?
	jbe	VF1FillInWinAttributes	;yes, we can support this in 1 meg
	cmp	TotalMemorySize,2*1024*1024
					;do we have 2 megs?
	jae	VF1FillInWinAttributes	;yes, we can do it!
;
public	VF1ModeNotSupported
VF1ModeNotSupported:
	and	byte ptr [eax].VModeAttributes,NOT 01h
					;can't do the mode!
;
public	VF1FillInWinAttributes
VF1FillInWinAttributes:
;
;We only have one "relocatable window", that is, we have only one banking
;register:
;
	mov	[eax].VWinAAttributes,07h
	mov	[eax].VWinBAttributes,00h
	mov	[eax].VWinGranularity,64;we align banks on 64K boundries
	mov	[eax].VWinSize,64	;our banks are 64K each
	mov	[eax].VWinASegment,0a000h
					;we bank memory in at A000H
	mov	[eax].VWinBSegment,0	;no "B" window
	mov	esi,VESAStubLinearAddr	;give him address of callable 4F05H
	mov	[eax].VWinFuncPtr,esi	;
;
public	VF1FillInScreenAttributes
VF1FillInScreenAttributes:
;
;Retrieve resolution and BPP information from the mode table:
;
	movzx	esi,cl			;get mode number (offset from 0) in ESI
	shl	esi,4			;multiply by 16 bytes per entry
.errnz	SIZE VESAModeEntry - 16
	mov	edi,OFFSET32 VESAModeLookupTable
	add	edi,esi 		;EDI --> entry for our mode
	mov	dx,[edi].ScreenWidthInPixels
	mov	[eax].VXResolution,dx	;
	mov	dx,[edi].ScreenPitch	;
	mov	[eax].VBytesPerScanLine,dx
	mov	dx,[edi].ScreenHeight	;
	mov	[eax].VYResolution,dx	;
	mov	dl,[edi].ScreenBPP	;
	mov	[eax].VBitsPerPixel,dl	;
;
;Fill in other miscallaneous fields:
;
	mov	[eax].VXCharSize,0	;these are graphics only modes
	mov	[eax].VYCharSize,0	;
	mov	[eax].VNbrOfPlanes,1	;we are running packed pixel
	mov	[eax].VMemoryModel,04h	;we're packed pixel memory model
	mov	[eax].VNbrOfBanks,1	;
	mov	[eax].VBankSize,0	;
	mov	[eax].VNbrOfImagePages,0;no hardware panning
	mov	[eax].VReserved,1	;
;
;Now, fill in the RGB information for the 16 BPP mode:
;
	cmp	cl,11h			;running in 640x480x16 BPP mode?
	jne	@F			;nope, no need to fill in RGB info
	mov	[eax].VRedMaskSize,5	;
	mov	[eax].VRedFieldPosition,11
	mov	[eax].VGreenMaskSize,6	;
	mov	[eax].VGreenFieldPosition,5
	mov	[eax].VBlueMaskSize,5	;
	mov	[eax].VBlueFieldPosition,0
;
;And lastly, the off-screen information:
;
@@:	push	eax			;save EAX --> Mode Info block
	movzx	edx,[eax].VBytesPerScanline
					;get total bytes on screen
	movzx	eax,[eax].VYResolution	;
	mul	edx			;EAX has total bytes on-screen
	mov	edx,eax 		;save this in EDX
	pop	eax			;restore EAX --> Mode Info block
	mov	[eax].VOffScreenMemOffset,edx
					;this is start of off-screen memory
	mov	ecx,TotalMemorySize	;
	sub	ecx,edx 		;this is off-screen memory available
	shr	ecx,10			;get it in kilobytes
	mov	[eax].VOffScreenMemSize,cx
;
public	VF1GoodExit
VF1GoodExit:
	mov	[ebp].Client_AX,4fh	;return success!
	jmp	VF1Exit 		;
;
public	VF1Failure
VF1Failure:
	mov	[ebp].Client_AX,014fh	;return failure to caller
;
VF1Exit:
	ret				;return to dispatch routine
endif	;MACH32VESASupport
EndProc VESAFunc1
;
;
subttl		Support For VESA Function 2 -- Set Mode
page +
public	VESAFunc2
BeginProc VESAFunc2, DOSVM
if MACH32VESASupport
;
;Entry:
;	EBX contains the VM handle.
;	EBP --> Client Registers.
;	Client_BX contains the mode number to set.
;Exit:
;	Client_AX contains the return status from the call.
;
;See if this mode is one of those that we might support:
;
	cld				;set direction forward
	movzx	eax,[ebp].Client_BX	;get the passed in mode number
	test	ah,04h			;trying to use a linear frame buffer?
	jnz	VF2Failure		;yes, we don't support them!
	and	ax,NOT 0c080h		;get rid of flag bits in mode nbr
	cmp	ax,13h			;is it a standard VGA mode?
	jbe	VF2StandardVGAMode	;yes, just go do an INT 10H, function 0
	mov	ecx,NbrVESAModes	;this is nbr that we potentially support
	mov	edi,OFFSET32 VESAModesTable
	repne	scasw			;
	jne	VF2Failure		;mode wasn't in table, fail the call
;
public	VF2CheckMemory
VF2CheckMemory:
;
;If we're running in 512K, we can only do 640x480 & 800x600:
;
	cmp	al,03h			;running above 800x600x8?
	jbe	VF2SetShadow		;nope, we can run on 512K
	cmp	TotalMemorySize,1024*1024
					;do we have at least 1 meg?
	jb	VF2Failure		;nope, we can't support this mode
	cmp	al,11h			;running 640x480x16 or below?
	jbe	VF2SetShadow		;yes, we can support this in 1 meg
	cmp	TotalMemorySize,2*1024*1024
					;do we have 2 megs?
	jb	VF2Failure		;nope, we can't support this mode
;
public	VF2SetShadow
VF2SetShadow:
	sub	eax,100h		;make mode number offset from 100H
	shl	eax,4			;multiply by 16 bytes per entry
.errnz	SIZE VESAModeEntry - 16
	add	eax,OFFSET32 VESAModeLookupTable
					;EAX --> table entry for our mode
;
;Get ready to call ROM code:
;
	Push_Client_State		;save current client register state
	VMMCall Begin_Nest_V86_Exec	;so we can call software in a VM
;
;Set the VGA up into a special ATI packed pixel mode (set bit 7 so we
;don't erase the screen):
;
	push	eax			;save EAX --> mode table entry
	movzx	eax,[eax].VGAModeNbr	;get the VGA mode to set
	or	al,80h			;set "don't erase screen" bit
	mov	[ebp].Client_EAX,eax	;this is mode to set
	mov	eax,10h 		;this is INT to execute
	VMMCall Exec_Int		;set the VGA mode
	pop	eax			;restore EAX --> mode table entry
;
;The Mach32 distinguishes between "accellerater modes" (such as the HiRes
;modes that the Windows display driver uses) and "VGA HiRes modes" (such
;as the one we've just set). In "accellerater modes", we do not access
;the palette through the standard VGA palette ports (3C8H etc.) but in the
;"VGA HiRes modes", we do.  Therefore, for 8 BPP (where VESA apps often
;directly set the VGA DAC registers), it is undesirable for us to go
;into the "accellerater mode".	So, for 8 BPP modes, we're done!
;
	cmp	[eax].ScreenBPP,8	;running 8 BPP?
	jbe	VF2DoneSettingMode	;yes, we're done setting the mode!
;
;Temporarily turn the memory boundry ON during the mode set by turning on
;bit 4 of the Memory Boundry Register:
;
	push	eax			;save EAX --> mode table entry
	mov	edx,42eeh		;EDX --> Memory Boundry Register
	in	al,dx			;get current value
	or	al,10h			;set memory boundry ON
	out	dx,al			;
;
;Now, call the ROM to turn on the aperture:
;
	mov	[ebp].Client_AX,3ffh	;turn on the aperture
	mov	edx,70h 		;this is address to call in ROM
	mov	cx,UltraROMSegment	;this is physical segment of ROM
	VMMCall Simulate_Far_Call	;setup call to routine at CX:EDX
	VMMCall Resume_Exec		;call the VM to execute the far call
	pop	eax			;restore EAX --> mode table entry
;
;Load up the client registers with the required info for the Set Shadow call:
;
	mov	dl,[eax].ShadowSetModeNbr
					;get the shadow set code
	mov	dh,[eax].BPPCode	;get the color format code
	mov	[ebp].Client_AX,dx	;set these into client AL & AH
	movzx	dx,[eax].PitchCode	;get the coded pitch
	mov	[ebp].Client_BX,dx	;set this into client BX for call
	mov	edx,64h 		;this is address of LoadShadowSet call
	mov	cx,UltraROMSegment	;this is physical segment of ROM
	VMMCall Simulate_Far_Call	;setup call to routine at CX:EDX
	VMMCall Resume_Exec		;call the VM to execute the far call
;
;Now, load up the client registers for the SetMode call:
;
	mov	[ebp].Client_AL,01h	;set ourselves into HiRes mode
	mov	edx,68h 		;this is address of SetMode call
	mov	cx,UltraROMSegment	;this is physical segment of ROM
	VMMCall Simulate_Far_Call	;setup call to routine at CX:EDX
	VMMCall Resume_Exec		;call the VM to execute the far call
;
public	VF2DoneSettingMode
VF2DoneSettingMode:
	VMMCall End_Nest_Exec		;we're done calling routines in VM
	Pop_Client_State		;return client registers to prev state
;
public	VF2PostModeChangeSetup
VF2PostModeChangeSetup:
;
;Turn off the memory boundry by writing a 0 to the Memory Boundry Register:
;
	mov	edx,42eeh		;EDX --> Memory Boundry Register
	xor	al,al			;set memory boundry OFF
	out	dx,al			;
;
;Get rid of independent read/write banking by setting ATI register 3EH, bit 3
;to a 0:
;
	mov	edx,1ceh		;EDX --> ATI index register
	mov	al,0beh 		;set to index 3EH
	out	dx,al			;
	inc	dl			;EDX --> ATI data register
	in	al,dx			;get current value
	and	al,NOT 08h		;zero bit 3
	out	dx,al			;and write it back to ATI register 3EH
;
public	VF2SaveCurrentMode
VF2SaveCurrentMode:
;
;We've now successfully setup the HiRes mode for running our VESA app.  Let's
;save the mode in our CB data area for later use:
;
	mov	ax,[ebp].Client_BX	;get mode that we just set
	mov	edi,ebx 		;make EDI --> CB data area for this VM
	add	edi,OurCBDataPointer	;
	mov	[edi].CurrentMode,ax	;save the mode number
;
public	VF2EraseScreen
VF2EraseScreen:
;
;If the sign bit of our mode number is set, we don't erase the screen.
;
	test	byte ptr [edi].CurrentMode+1,80h
	jnz	VF2GoodExit		;do not erase the screen
	mov	esi,TotalMemorySize	;we want to clear all of VRAM
	mov	edx,1ceh		;EDX --> ATI index register
	xor	cl,cl			;start at bank 0
;
@@:	push	ecx			;save current bank in CL
	mov	ah,cl			;get bank into AH
	and	ah,0fh			;mask to get only low nibble
	shl	ah,1			;get bank into place in AH
	mov	al,0b2h 		;set this into ATI register 32H
	out	dx,ax			;
;
	mov	al,0aeh 		;set to ATI register 2EH
	out	dx,al			;
	inc	dl			;EDX --> ATI data register
	in	al,dx			;get current value
	and	al,NOT 0fh		;get rid of banking bits in 2EH
	mov	ah,cl			;get bank into AH
	shr	ah,4			;get bank into place in AH
	or	al,ah			;now AL has correct value for 2EH
	out	dx,al			;
	dec	dl			;EDX --> ATI index register
;
	mov	edi,VidMemLinearAddr	;EDI --> linear address of A0000H
	mov	ecx,(64*1024)/4 	;ECX has nbr of DWORDS to clear
	xor	eax,eax 		;clear to zero
	rep	stosd			;clear 64K
	pop	ecx			;restore saved bank value to CL
	inc	cl			;bump to the next bank
	sub	esi,64*1024		;are we done yet?
	jg	@B			;nope, go do next 64K
;
;Now, reset to bank 0:
;
	mov	ax,00b2h		;set low banking register to 0
	out	dx,ax			;
	mov	al,0aeh 		;set to ATI register 2EH
	out	dx,al			;
	inc	dl			;EDX --> ATI data register
	in	al,dx			;get current value
	and	al,NOT 0fh		;zero the banking bits
	out	dx,al			;and set it to ATI register 2EH
	jmp	VF2GoodExit		;hey we're done!
;
public	VF2StandardVGAMode
VF2StandardVGAMode:
	call	SetVGAPassthrough	;get ourselves out of HiRes mode
	movzx	eax,[ebp].Client_BL	;
	push	10h			;
	VMMCall Exec_VxD_Int		;just do a standard INT 10H mode set
;
public	VF2GoodExit
VF2GoodExit:
	mov	[ebp].Client_AX,4fh	;return success!
	jmp	VF2Exit 		;
;
public	VF2Failure
VF2Failure:
	mov	[ebp].Client_AX,014fh	;return failure to caller
;
VF2Exit:
	ret				;return to dispatch routine
endif	;MACH32VESASupport
EndProc VESAFunc2
;
;
subttl		Support For VESA Function 3 -- Retrieve Mode
page +
public	VESAFunc3
BeginProc VESAFunc3, DOSVM
if MACH32VESASupport
;
;Entry:
;	EBX contains the VM handle.
;	EBP --> Client Registers.
;Exit:
;	Client_AX contains the return status from the call (always success).
;	Client_BX contains the mode number that VM is running in.
;
	mov	edi,ebx 		;make EDI --> our CB data area
	add	edi,OurCBDataPointer	;
	mov	ax,[edi].CurrentMode	;get the mode that we're in
	mov	[ebp].Client_BX,ax	;and send it back to the caller
	mov	[ebp].Client_AX,4fh	;return success!
;
VF3Exit:
	ret				;return to dispatch routine
endif	;MACH32VESASupport
EndProc VESAFunc3
;
;
subttl		Support For VESA Function 4 -- Save/Restore State
page +
public	VESAFunc4
BeginProc VESAFunc4, DOSVM
if MACH32VESASupport
;
;Entry:
;	EBX contains the VM handle.
;	EBP --> Client Registers.
;	Client_DL contains 0 if querying the save/restore buffer size.
;	Client_DL contains 1 if requesting a save state.
;	Client_DL contains 2 if requesting a restore state.
;	Client_CX contains the requested states bitmask:
;		Bit 0 set == save/restore controller state.
;		Bit 1 set == save/restore BIOS data area state.
;		Bit 2 set == save/restore DAC state.
;		Bit 3 set == save/restore register state.
;	Client_ES:Client_BX --> Buffer to save/restore states to/from.
;Exit:
;	Client_AX contains the return status from the call.
;	Client_BX contains the state size if Client_DL == 0.
;
;We don't save/restore states.  Just return 0 if it's sub-function 0 and
;return success.
;
	cmp	[ebp].Client_DL,0	;is it a "get state save size" request?
	jne	VF4GoodExit		;nope, just leave successfully
	mov	[ebp].Client_BX,0	;yes, return state size 0
;
public	VF4GoodExit
VF4GoodExit:
	mov	[ebp].Client_AX,4fh	;return success!
;
VF4Exit:
	ret				;return to dispatch routine
endif	;MACH32VESASupport
EndProc VESAFunc4
;
;
subttl		Support For VESA Function 5 -- Display Window Control
page +
public	VESAFunc5
BeginProc VESAFunc5, DOSVM
if MACH32VESASupport
;
;Entry:
;	EBX contains the VM handle.
;	EBP --> Client Registers.
;	Client_BH == 0 if we're to set the bank.
;	Client_BH == 1 if we're to get the bank.
;	Client_BL contains the "window" number (0 or 1).
;	Client_DX contains the bank to set (if Client_BH == 0).
;Exit:
;	Client_AX contains the return status from the call.
;	Client_DX contains the bank number (if Client_BH == 1).
;
	mov	edx,1ceh		;EDX --> ATI index register
	cmp	[ebp].Client_BH,0	;setting the bank?
	je	VF5SetBank		;yes, go do it
;
public	VF5GetBank
VF5GetBank:
	mov	al,0b2h 		;set to ATI register 32H
	out	dx,al			;
	inc	dl			;EDX --> ATI data register
	in	al,dx			;
	shr	al,1			;get banking bits into low nibble of AL
	and	al,0fh			;mask to get only banking bits
	mov	ah,al			;save this in AH
	dec	dl			;EDX --> ATI index register
	mov	al,0aeh 		;set to ATI register 2EH
	out	dx,al			;
	inc	dl			;EDX --> ATI data register
	in	al,dx			;
	and	al,03h			;mask to get only banking bits
	shl	al,4			;get them into high nibble of AL
	or	al,ah			;now AL has the current bank
	xor	ah,ah			;zero out AH
	mov	[ebp].Client_DX,ax	;return it in the expected place
	jmp	VF5GoodExit		;and return success
;
public	VF5SetBank
VF5SetBank:
	mov	ah,[ebp].Client_DL	;get bank to set
	and	ah,0fh			;mask to get only low nibble
	shl	ah,1			;get bank into place in AH
	mov	al,0b2h 		;set this into ATI register 32H
	out	dx,ax			;
;
	mov	al,0aeh 		;set to ATI register 2EH
	out	dx,al			;
	inc	dl			;EDX --> ATI data register
	in	al,dx			;get current value
	and	al,NOT 0fh		;get rid of banking bits in 2EH
	mov	ah,[ebp].Client_DL	;get bank into AH
	shr	ah,4			;get bank into place in AH
	or	al,ah			;now AL has correct value for 2EH
	out	dx,al			;
;
public	VF5GoodExit
VF5GoodExit:
	mov	[ebp].Client_AX,4fh	;return success!
;
VF5Exit:
	ret				;return to dispatch routine
endif	;MACH32VESASupport
EndProc VESAFunc5
;
;
subttl		Support For VESA Function 6 -- Get/Set Logical Scanline Length
page +
public	VESAFunc6
BeginProc VESAFunc6, DOSVM
if MACH32VESASupport
;
;Entry:
;	EBX contains the VM handle.
;	EBP --> Client Registers.
;	BL contains the sub-function:
;		-- 0 == Set scanline length in pixels.
;		-- 1 == Get scanline length.
;		-- 2 == Set scanline length in bytes.
;		-- 3 == Get maximum scanline length.
;	Client_CX contains the scanline lengths in pixels/bytes if BL == 0/2.
;Exit:
;	Client_AX contains the return status from the call.
;	Client_BX contains bytes per line.
;	Client_CX contains pixels per line.
;	Client_DX contains the maximum number of scanlines.
;
	movzx	ecx,[ebp].Client_BL	;
	cmp	cl,NbrVF6Funcs		;is function code request OK?
	jae	VF6Failure		;nope, leave with a failure
	mov	edi,ebx 		;make EDI --> CB data area
	add	edi,OurCBDataPointer	;
	movzx	edi,[edi].CurrentMode	;EDI contains the mode number to lookup
	cmp	edi,HighestMode 	;is it a valid mode?
	ja	VF6Failure		;nope, leave now!
	sub	edi,100h		;offset the mode number from zero
	jl	VF6Failure		;bad mode!  don't support it
	shl	edi,4			;multiply by 16 bytes per entry
	movzx	esi,[edi+OFFSET32 VESAModeLookupTable].ScreenWidthInPixels
					;ESI has width of screen in pixels
	movzx	edi,[edi+OFFSET32 VESAModeLookupTable].ScreenPitch
					;EDI has width of screen in bytes
	or	esi,esi 		;did we get a zero entry?
	jz	VF6Failure		;yes, bad mode!
	mov	eax,TotalMemorySize	;get bytes of total memory
	xor	edx,edx 		;zero EDX for dword lengthed divide
	div	edi			;now EAX has total nbr of scanlines
;
;The VESA spec is unclear regarding when we're supposed to return data.  We
;therefore always return the data in the client registers:
;
	movzx	edx,[ebp].Client_CX	;save input data from Client_CX
	mov	[ebp].Client_BX,di	;return bytes per scanline in Client_BX
	mov	[ebp].Client_CX,si	;return pixels per scanline in Client_CX
	mov	[ebp].Client_DX,ax	;return total scanlines in Client_DX
;
;Now, dispatch to the proper handler:
;
	jmp	[(ecx*4)+OFFSET32 VF6DispatchTable]
;
public	VF6SetLengthInPixels
VF6SetLengthInPixels:
;
;At this point:
;	EDX contains the data passed into this function from Client_CX,
;		that is, the desired width in pixels/bytes.
;
	cmp	edx,esi 		;is it the same as what it's set to?
	jne	VF6Failure		;nope, leave with a failure
	jmp	VF6GoodExit		;yes, leave successfully!
;
public	VF6SetLengthInBytes
VF6SetLengthInBytes:
;
;At this point:
;	EDX contains the data passed into this function from Client_CX,
;		that is, the desired width in pixels/bytes.
;
	cmp	edx,edi 		;is it the same as what it's set to?
	jne	VF6Failure		;nope, leave with a failure
;
public	VF6GoodExit
VF6GoodExit:
	mov	[ebp].Client_AX,4fh	;return success!
	jmp	VF6Exit 		;
;
public	VF6Failure
VF6Failure:
	mov	[ebp].Client_AX,014fh	;return failure to caller
;
VF6Exit:
	ret				;return to dispatch routine
;
;
VF6DispatchTable	label	dword
	dd	OFFSET32 VF6SetLengthInPixels
	dd	OFFSET32 VF6GoodExit
	dd	OFFSET32 VF6SetLengthInBytes
	dd	OFFSET32 VF6GoodExit
NbrVF6Funcs	equ ($ - OFFSET32 VF6DispatchTable)/4
;
;
endif	;MACH32VESASupport
EndProc VESAFunc6
;
;
subttl		Support For VESA Function 7 -- Set/Get Display Start
page +
public	VESAFunc7
BeginProc VESAFunc7, DOSVM
if MACH32VESASupport
;
;Entry:
;	EBX contains the VM handle.
;	EBP --> Client Registers.
;	BL contains the sub-function code:
;		-- 0 = Set Display Start.
;		-- 1 = Get Display Start.
;		-- 80h = Set Display Start during vertical retrace.
;	BH must be 0.
;	CX = Starting X pixel.
;	DX = Starting Y pixel.
;Exit:
;	Client_AX contains the return status from the call.
;	BH must be 0.
;	CX = Starting X pixel.
;	DX = Starting Y pixel.
;
	cmp	[ebp].Client_BL,01h	;is it a "get display start" call?
	jne	VF7Failure		;nope, fail the call
	mov	[ebp].Client_BH,0	;return a zero in BH
	mov	[ebp].Client_CX,0	;return display start in X as 0
	mov	[ebp].Client_DX,0	;return display start in Y as 0
;
public	VF7GoodExit
VF7GoodExit:
	mov	[ebp].Client_AX,4fh	;return success!
	jmp	VF7Exit 		;
;
public	VF7Failure
VF7Failure:
	mov	[ebp].Client_AX,014fh	;return failure to caller
;
VF7Exit:
	ret				;return to dispatch routine
endif	;MACH32VESASupport
EndProc VESAFunc7
;
;
subttl		Support For VESA Function 8 -- Set/Get Palette Format
page +
public	VESAFunc8
BeginProc VESAFunc8, DOSVM
if MACH32VESASupport
;
;Entry:
;	EBX contains the VM handle.
;	EBP --> Client Registers.
;	Client_BL contains 0 for Set Format or 1 for Get Format.
;	Client_BH contains the new format to set.
;Exit:
;	Client_AX contains the return status from the call.
;
	cmp	[ebp].Client_BL,1	;does caller want to do a "Get Format"?
	jne	VF8Failure		;nope, return failure
	mov	[ebp].Client_BH,6	;we only have a 6 bit DAC
;
public	VF8GoodExit
VF8GoodExit:
	mov	[ebp].Client_AX,4fh	;return success!
	jmp	VF8Exit 		;
;
public	VF8Failure
VF8Failure:
	mov	[ebp].Client_AX,014fh	;return failure to caller
;
VF8Exit:
	ret				;return to dispatch routine
endif	;MACH32VESASupport
EndProc VESAFunc8
;
;
subttl		Support For VESA Function 9 -- Set/Get Palette Data
page +
public	VESAFunc9
BeginProc VESAFunc9, DOSVM
if MACH32VESASupport
;
;Entry:
;	EBX contains the VM handle.
;	EBP --> Client Registers.
;	Client_BL has the sub-function:
;		-- 0 == Set Palette Data.
;		-- 1 == Get Palette Data.
;		-- 2 == Set Secondary Palette Data.
;		-- 3 == Get Secondary Palette Data.
;		-- 80H == Set Palette Data during vertical retrace with screen
;			  blanked.
;	Client_CX contains the number of palette entries to do.
;	Client_DX contains the first palette entry to do.
;	Client_ES:Client_DI --> palette entries to do.
;	Format of palette data pointed to by Client_ES:Client_DI is
;		Byte 0 = alignment byte.
;		Byte 1 = RED value (in lowest 6 bits of byte).
;		Byte 2 = GREEN value (in lowest 6 bits of byte).
;		Byte 3 = BLUE value (in lowest 6 bits of byte).
;Exit:
;	Client_AX contains the return status from the call.
;
	mov	cl,[ebp].Client_BL	;get subfunction code in CL
	and	cl,NOT 80h		;get rid of 80H bit (not needed on XGA)
	or	cl,cl			;setting palette?
	jz	VF9SetPalette		;yes, go do it
	cmp	cl,1			;getting palette?
	jne	VF9NoSecondaryPalette	;nope, send back error return value
;
public	VF9GetPalette
VF9GetPalette:
	mov	ah,Client_ES		;get client register equates in
	mov	al,Client_DI		;
	VMMCall Map_Flat		;EAX contains the linear address
	cmp	eax,-1			;did the call fail?
	je	VF9Failure		;yes, don't go any further!
	mov	edi,eax 		;EDI --> place to put palette entries
	movzx	ecx,[ebp].Client_DX	;get starting index into ECX
	movzx	esi,[ebp].Client_CX	;get nbr of entries to do in ESI
;
;Get ready to read the palette entries:
;
	mov	edx,3c7h		;EDX --> DAC read index register
;
public	VF9ReadDACLoop
VF9ReadDACLoop:
	mov	al,cl			;get palette entry to read
	out	dx,al			;set the index
;
	mov	dl,0c9h 		;this is DAC Data register
	mov	[edi],cl		;set the index as the alignment byte
	in	al,dx			;get RED value
	mov	[edi+1],al		;save it in return area
	in	al,dx			;get GREEN value
	mov	[edi+2],al		;save it in return area
	in	al,dx			;get BLUE value
	mov	[edi+3],al		;save it in return area
;
	mov	dl,0c7h 		;EDX --> DAC read index register
	add	edi,4			;bump to next entry in return area
	inc	ecx			;bump to next DAC entry to read
	dec	esi			;any more to do?
	jnz	VF9ReadDACLoop		;yes, go read the next entry!
	jmp	VF9GoodExit		;hey! we're done!
;
public	VF9SetPalette
VF9SetPalette:
	mov	ah,Client_ES		;get client register equates in
	mov	al,Client_DI		;
	VMMCall Map_Flat		;EAX contains the linear address
	cmp	eax,-1			;did the call fail?
	je	VF9Failure		;yes, don't go any further!
	mov	esi,eax 		;ESI --> place to read palette entries
	movzx	ecx,[ebp].Client_DX	;get starting index into ECX
	movzx	edi,[ebp].Client_CX	;get nbr of entries to do in EDI
;
;Get ready to set the palette entries:
;
	mov	edx,3c8h		;EDX --> DAC Write Index register
;
public	VF9WriteDACLoop
VF9WriteDACLoop:
	mov	al,cl			;get palette entry to write
	out	dx,al			;set the index
	inc	dl			;EDX --> DAC Data Register
	mov	al,[esi+1]		;get RED value
	out	dx,al			;
	mov	al,[esi+2]		;get GREEN value
	out	dx,al			;
	mov	al,[esi+3]		;get BLUE value
	out	dx,al			;
	dec	dl			;EDX --> DAC Write Index register
	add	esi,4			;bump to next DAC entry
	inc	ecx			;bump to next DAC entry to write
	dec	edi			;any more to do?
	jnz	VF9WriteDACLoop 	;yes, go write the next entry!
;
public	VF9GoodExit
VF9GoodExit:
	mov	[ebp].Client_AX,4fh	;return success!
	jmp	VF9Exit 		;
;
public	VF9Failure
VF9Failure:
	mov	[ebp].Client_AX,014fh	;return failure to caller
	jmp	VF9Exit 		;
;
public	VF9NoSecondaryPalette
VF9NoSecondaryPalette:
	mov	[ebp].Client_AX,024fh	;return failure to caller
;
VF9Exit:
	ret				;return to dispatch routine
endif	;MACH32VESASupport
EndProc VESAFunc9
;
;
subttl		Support For VESA Function 10 -- Return Protected Mode Interface
page +
public	VESAFuncA
BeginProc VESAFuncA, DOSVM
if MACH32VESASupport
;
;Entry:
;	EBX contains the VM handle.
;	EBP --> Client Registers.
;Exit:
;	Client_AX contains the return status from the call.
;
public	VFAFailure
VFAFailure:
	mov	[ebp].Client_AX,014fh	;return failure to caller
;
VFAExit:
	ret				;return to dispatch routine
endif	;MACH32VESASupport
EndProc VESAFuncA
;
;
VxD_LOCKED_CODE_ENDS
end

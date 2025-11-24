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
title		S3 Mini-VDD Support Functions
.386p
;
;
.xlist
include 	VMM.INC
include 	MINIVDD.INC
include 	S3MINI.INC
.list
;
;
subttl		Virtual Device Declaration
page +
Declare_Virtual_Device	S3MINI, 				\
			3,					\
			1,					\
			MiniVDD_Control,			\
			Undefined_Device_ID,			\
			VDD_Init_Order, 			\
			,					\
			,					\
			,
;
;
subttl		Initialization Data
page +
VxD_IDATA_SEG
VxD_IDATA_ENDS
;
;
subttl		Locked Data Area
page +
VxD_LOCKED_DATA_SEG
VxD_LOCKED_DATA_ENDS
;
;
subttl		General Data Area
page +
VxD_DATA_SEG
;
public	WindowsVMHandle
WindowsVMHandle 	dd	?		;init'd at Dynamic_Init
;
public	PlanarStateID, MessageModeID
PlanarStateID		dd	?		;init'd at Dynamic_Init
MessageModeID		dd	?		;init'd at Dynamic_Init
;
public	TotalMemorySize
TotalMemorySize 	dd	?		;init'd at Dynamic_Init
;
public	Vid_PhysA0000
Vid_PhysA0000		dd	0		;
;
public	OurCBDataPointer
OurCBDataPointer	dd	?		;init'd at Device_Init
;
public	InSrcBLTAddr, CurrentChipModeAddr	;S3 specific variable!
InSrcBLTAddr		dd	?		;init'd at RegisterDisplayDriver
CurrentChipModeAddr	dd	?		;init'd at RegisterDisplayDriver
bMMIOActiveAddr 	dd	?		;init'd at RegisterDisplayDriver
;
public	Old3C2Handler
Old3C2Handler		dd	0		;
;
public	MessageModeCBData
MessageModeCBData	db	size PerVMData dup(0)
;
public	ForcedPlanarCBData
ForcedPlanarCBData	db	size PerVMData dup(0)
;
public	DisplayInfoStructure
DisplayInfoStructure	db	size DISPLAYINFO dup(0)
;
LAWAddrRegLow		dw	?		;
LAWAddrRegHigh		dw	?		;
;
public	VFLATDFlag
VFLATDFlag		db	?		;init'd at RegisterDisplayDriver
;
public	ChipID, NewBankingSupport
ChipID			db	?		;init'd at Dynamic_Init
NewBankingSupport	db	0		;FFH if this is an 864 or above
;
public	S3ModeChangeFlags
S3ModeChangeFlags	db	?		;=FFH when in BIOS mode change
;
public	DisplayEnabledFlag, SuperVGAModeFlag
DisplayEnabledFlag	db	0		;=FFH when display is enabled
SuperVGAModeFlag	db	0		;=FFH when NOT running VGA mode
MsgModeInitDone 	db	0		;=FFH when collecting states
;
public	VDDBankLow, VDDBankHigh, BankEnableSave, BankRegSaveLow
public	BankRegSaveHigh, LatchSave51H
public	LatchSave35H, LatchSave31H		;S3 specific variables!
public	LAWAddrRegLow, LAWAddrRegHigh, LAWApertureSizeBits
%out Uncomment the following two lines post-Win '95 (bug 13111):
comment |
public	Register6AMask
Register6AMask		db	0ffh		;masks out unused bits on Trio
end comment |
VDDBankLow		db	?		;init'd at GetVDDBank
VDDBankHigh		db	?		;
BankEnableSave		db	?		;
BankRegSaveLow		db	0ffh		;this flag must be init'd!
BankRegSaveHigh 	db	?		;
LatchSave31H		db	?		;
LatchSave35H		db	?		;
LatchSave51H		db	?		;
LAWApertureSizeBits	db	?		;
WindowsMiscOutputState	db	0		;
;
VxD_DATA_ENDS
;
;
VxD_ICODE_SEG
;
;
subttl		MiniVDD Sys Critical Initialization
page +
public	MiniVDD_Sys_Critical_Init
BeginProc MiniVDD_Sys_Critical_Init
;
;NOTE:	This routine is ONLY called if we're being statically loaded from
;	SYSTEM.INI.  It isn't called (and doesn't need to be called) if
;	the Main VDD is dynamically loading us.
;
;If we're being statically loaded, the Main VDD is going to try to setup
;its Message Mode and Forced Planar Mode states by calling the S3 card's
;ROM BIOS.  The ROM BIOS depends on the fact that the S3 extension registers
;are unlocked (which won't happen since the Main VDD will trap all of the
;CRTC ports and we won't be around to virtualize them till after the Main
;VDD does the BIOS call.  Therefore, we must do an explicit unlock here
;if we're being statically loaded.
;
	mov	edx,3d4h		;EDX --> CRTC registers
	in	al,dx			;get current index
	ror	eax,8			;save it
	mov	ax,4838h		;unlock registers 30H-3FH
	out	dx,ax			;
	mov	ax,0a539h		;unlock registers 40H-80H & 36H
	out	dx,ax			;
	rol	eax,8			;restore CRTC index register
	out	dx,al			;
;
MVSI_Exit:
	clc				;return success (always)
	ret				;
EndProc MiniVDD_Sys_Critical_Init
;
;
subttl		MiniVDD Dynamic Initialization
page +
public	MiniVDD_Dynamic_Init
BeginProc MiniVDD_Dynamic_Init
;
;Entry:
;	EBX contains the VM handle of the Windows VM.
;Exit:
;	If success, return NC.
;	If failure, return CY.
;
;
;NOTE: MiniVDD_Dynamic_Init MUST be called only during the Device_Init
;      phase of system initialization.	We make a bunch of service calls
;      to the Main VDD (*VDD) that are located in its VxD_ICODE_Seg which
;      will be discarded after the Device_Init phase of system initialization.
;
	mov	WindowsVMHandle,ebx	;save the Windows VM handle
	mov	S3ModeChangeFlags,0	;initialize this
;
;Get the linear address of physical A0000H addressable for 64K:
;
	VMMCall _MapPhysToLinear,<0A0000h,10000h,0>
	cmp	eax,-1			;was there an error?
	je	MVDI_ErrorExit		;yes, better not load!
	mov	Vid_PhysA0000,eax	;save this off!
;
;Get the chip ID from the CRTC index register 30H:
;
	mov	edx,3d4h		;
	mov	ax,4838h		;unlock the S3 extension registers
	out	dx,ax			;
	mov	al,30h			;now read the chip ID
	out	dx,al			;
	inc	dx			;
	in	al,dx			;
	cmp	al,0e1h 		;is this a generation 4 chipset?
	je	MVDI_FourthGenerationChipsets
	mov	ch,al			;save chip ID in CH for now
	and	ch,0f0h 		;get rid of minor chip revision stuff
	cmp	ch,S3911		;is it an S3-911 or S3-924?
	je	@F			;yes, go do another test
	cmp	ch,S3928		;is it an S3-928?
	je	@F			;yes, go do another test
	cmp	ch,S3928PCI		;is it an S3-928/PCI bus chip?
	je	@F			;yes, go do another test
	cmp	ch,S3801		;is it an S3-801 or S3-805?
	je	@F			;yes, go do another test
	mov	NewBankingSupport,0ffh	;this chipset uses banking register 6AH
	cmp	ch,S3864		;is it an S3-864?
	je	@F			;yes, go do another test
	cmp	ch,S3964		;is it an S3-964?
	je	@F			;yes, go do another test
	cmp	ch,S3764		;is it an S3-764?
	jne	MVDI_ErrorExit		;nope, we don't recognize it
;
public	MVDI_FourthGenerationChipsets
MVDI_FourthGenerationChipsets:
;
;The following are tests for the fourth generation S3 chipsets.  These
;are the S3-732, S3-866, S3-868, and S3-968:
;
	mov	NewBankingSupport,0ffh	;this chipset uses banking register 6AH
	dec	dl			;EDX --> CRTC index register
	mov	al,2eh			;set to new generation ID port
	out	dx,al			;
	inc	dl			;EDX --> CRTC data register
	in	al,dx			;get chip ID value from register 2EH
	and	al,0f0h 		;get rid of minor revision
	or	al,FOURTH_GEN		;and set "4th generation" bit in ID
	mov	ch,al			;and get it into CH
	cmp	ch,S3732		;is it an S3-732?
	je	@F			;yes, go do another test
	cmp	ch,S3866		;is it an S3-866?
	je	@F			;yes, go do another test
	cmp	ch,S3868		;is it an S3-868?
	je	@F			;yes, go do another test
	cmp	ch,S3968		;is it an S3-968?
	je	@F			;yes, go do another test
	cmp	ch,S3968_B		;is it an S3-968 (production version)?
	jne	MVDI_ErrorExit		;nope, we don't recognize it
;
;Now try writing register 30H and see if it is read-write.  If it is, we
;aren't an S3 chip!
;
@@:	dec	dl			;EDX --> CRTC index register
	mov	al,30h			;set to index 30H again
	out	dx,al			;
	inc	dl			;EDX --> CRTC data register
	in	al,dx			;get current value of register 30H
	inc	al			;make it something else
	mov	ah,al			;(save it for later compare)
	out	dx,al			;try to write it back to register 30H
	in	al,dx			;read back the value
	cmp	al,ah			;is it the same as what we wrote?
	je	MVDI_ErrorExit		;yes, can't be an S3 chipset
	dec	dl			;EDX --> CRTC index register again
;
;Now, get the total amount of memory on the S3 card:
;
	mov	al,36h			;
	out	dx,al			;
	inc	dx			;point to data register 3D5H
	in	al,dx			;get the memory size bits
	dec	dx			;restore DX --> CRTC index register
	mov	cl,al			;save this value for now
;
;Dispatch to the proper routine for getting the memory size:
;
	cmp	ch,S3911		;running on the S3-911 or S3-924?
	je	MVDI_GetMemSize_911	;yes, go handle it
;
MVDI_GetMemSize_928:
	shr	cl,5			;now memory size bits are in low bits
	mov	eax,4*1024*1024 	;assume we have 4 megabytes
	or	cl,cl			;do we have 4 megabytes?
	jz	MVDI_GotMemSize 	;yes, return it
;
	mov	eax,3*1024*1024 	;assume we have 3 megabytes
	cmp	cl,2			;do we have 3 megabytes?
	je	MVDI_GotMemSize 	;yes, return it
;
	mov	eax,8*1024*1024 	;assume we have 8 megabytes
	cmp	cl,3			;do we have 8 megabytes?
	jz	MVDI_GotMemSize 	;yes, return it
;
	mov	eax,2*1024*1024 	;assume we have 2 megabytes
	cmp	cl,4			;do we have 2 megabytes?
	je	MVDI_GotMemSize 	;yes, return it
;
	mov	eax,6*1024*1024 	;assume we have 6 megabytes
	cmp	cl,5			;do we have 6 megabytes?
	jz	MVDI_GotMemSize 	;yes, return it
;
	mov	eax,1*1024*1024 	;assume we have 1 megabyte
	cmp	cl,6			;do we have 1 megabyte?
	je	MVDI_GotMemSize 	;yes, return it
;
	mov	eax,512*1024		;return that we have 512K
	jmp	MVDI_GotMemSize 	;
;
MVDI_GetMemSize_911:
	mov	eax,1024*1024		;assume we've got 1 meg of memory
	test	cl,20h			;
	jz	MVDI_GotMemSize 	;we have 1 meg of memory
	mov	eax,512*1024		;oops, we only have 512K of memory!
;
MVDI_GotMemSize:
	mov	TotalMemorySize,eax	;save the amount of memory on card
	mov	ChipID,ch		;save the Chip ID
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
	MiniVDDDispatch REGISTER_DISPLAY_DRIVER,RegisterDisplayDriver
	MiniVDDDispatch GET_VDD_BANK,GetVDDBank
	MiniVDDDispatch SET_VDD_BANK,SetVDDBank
	MiniVDDDispatch RESET_BANK,ResetBank
	MiniVDDDispatch SET_LATCH_BANK,SetLatchBank
	MiniVDDDispatch RESET_LATCH_BANK,ResetLatchBank
	MiniVDDDispatch PRE_HIRES_TO_VGA,PreHiResToVGA
	MiniVDDDispatch POST_HIRES_TO_VGA,PostHiResToVGA
	MiniVDDDispatch PRE_VGA_TO_HIRES,PreVGAToHiRes
	MiniVDDDispatch POST_VGA_TO_HIRES,PostVGAToHiRes
	MiniVDDDispatch SAVE_REGISTERS,SaveRegisters
	MiniVDDDispatch RESTORE_REGISTERS,RestoreRegisters
	MiniVDDDispatch ACCESS_VGA_MEMORY_MODE,AccessVGAMemoryMode
	MiniVDDDispatch ACCESS_LINEAR_MEMORY_MODE,AccessLinearMemoryMode
	MiniVDDDispatch ENABLE_TRAPS,EnableTraps
	MiniVDDDispatch DISABLE_TRAPS,DisableTraps
	MiniVDDDispatch MAKE_HARDWARE_NOT_BUSY,MakeHardwareNotBusy
	MiniVDDDispatch VIRTUALIZE_CRTC_IN,VirtualCRTCIn
	MiniVDDDispatch VIRTUALIZE_CRTC_OUT,VirtualCRTCOut
	MiniVDDDispatch SAVE_FORCED_PLANAR_STATE,SaveForcedPlanarState
	MiniVDDDispatch SAVE_MESSAGE_MODE_STATE, SaveMsgModeState
	MiniVDDDispatch DISPLAY_DRIVER_DISABLING,DisplayDriverDisabling
	MiniVDDDispatch PRE_CRTC_MODE_CHANGE,PreCRTCModeChange
	MiniVDDDispatch GET_TOTAL_VRAM_SIZE,GetTotalVRAMSize
	MiniVDDDispatch GET_CURRENT_BANK_WRITE,GetCurrentBank
	MiniVDDDispatch GET_CURRENT_BANK_READ,GetCurrentBank
	MiniVDDDispatch GET_BANK_SIZE,GetBankSize
	MiniVDDDispatch SET_BANK,SetBank
	MiniVDDDispatch PRE_HIRES_SAVE_RESTORE,PreHiResSaveRestore
	MiniVDDDispatch POST_HIRES_SAVE_RESTORE,PostHiResSaveRestore
	MiniVDDDispatch GET_CHIP_ID, GetChipID
	MiniVDDDispatch VIRTUALIZE_BLTER_IO, VirtualizeBLTerIO
	MiniVDDDispatch VESA_CALL_POST_PROCESSING, VESACallPostProcessing
	cmp	ChipID,S3911			;on a 911 or 924?
	jne	MVDI_Takeover3C2		;nope, skip this hook
	MiniVDDDispatch SET_HIRES_MODE, SetHiResMode
;
public	MVDI_Takeover3C2
MVDI_Takeover3C2:
;
;We need to intercept and save the values that are being set into the
;Miscallaneous Output Register (port 3C2H).  Since there's no
;VIRTUALIZE_MISC_OUTPUT_OUT MiniVDD hook, we just take it over using
;VDD_Takeover_VGA_Port servier and then pass it onto the original routine
;after saving the value.
;
	mov	edx,3c2h			;this is port to takeover
	mov	ecx,OFFSET32 MiniVDD_VirtualMiscOutput
	VxDCall VDD_Takeover_VGA_Port		;
	mov	Old3C2Handler,ecx		;save old handler's address
;
public	MVDI_GetSpecialVMs
MVDI_GetSpecialVMs:
;
;There are two special VM states that the "main" VDD establishes.  The
;first is the planar state which is simply a state that the "main" VDD
;restores to establish the VGA 4 plane mode.  When we restore the states
;at MiniVDD_RestoreRegisters, we check for the special identifier which
;flags that we're restoring this state.  If we find that we are restoring
;the planar state, we have the opportunity to special case the register
;state restore.
;
;Similarly, there is a special state called "Message Mode".  This is the
;state when the Shell VxD is displaying a full-screen message (usually
;with a blue background) telling the user of a serious anomaly in the
;running state of Windows.  We also retrieve the special VM handle
;for the "Message Mode" state so we can handle it special too if needed.
;
	VxDCall VDD_Get_Special_VM_IDs	;go get special VM information
	mov	PlanarStateID,esi	;save off returned identifiers
	mov	MessageModeID,edi	;
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
;so you'd register it as a word sized port with the "master" VDD.
;In this VDD, we never have discretely accessed byte lengthed ports.  So,
;although the display driver might access the high byte of port 9AE8H for
;example, since 9AE8H was registered as a word lengthed port, it covers
;accesses to 9AE9H as well, even if the display driver does a byte lengthed
;OUT to 9AE9H separately.
;
	mov	edx,9ae8h		;register port 9AE8H...
	mov	cl,WORD_LENGTHED	;as a word lengthed port
	VxDCall VDD_Register_Virtual_Port
;
	mov	edx,0e2e8h		;register port E2E8H...
	mov	cl,WORD_LENGTHED	;as a word lengthed port
	VxDCall VDD_Register_Virtual_Port
;
	mov	edx,0bee8h		;register port BEE8H...
	mov	cl,WORD_LENGTHED	;as a word lengthed port
	VxDCall VDD_Register_Virtual_Port
;
	mov	edx,96e8h		;register port 96E8H...
	mov	cl,WORD_LENGTHED	;as a word lengthed port
	VxDCall VDD_Register_Virtual_Port
;
	mov	edx,86e8h		;register port 86E8H...
	mov	cl,WORD_LENGTHED	;as a word lengthed port
	VxDCall VDD_Register_Virtual_Port
;
	mov	edx,82e8h		;register port 82E8H...
	mov	cl,WORD_LENGTHED	;as a word lengthed port
	VxDCall VDD_Register_Virtual_Port
;
	mov	edx,8ee8h		;register port 8EE8H...
	mov	cl,WORD_LENGTHED	;as a word lengthed port
	VxDCall VDD_Register_Virtual_Port
;
	mov	edx,8ae8h		;register port 8AE8H...
	mov	cl,WORD_LENGTHED	;as a word lengthed port
	VxDCall VDD_Register_Virtual_Port
;
	mov	edx,0b6e8h		;register port B6E8H...
	mov	cl,WORD_LENGTHED	;as a word lengthed port
	VxDCall VDD_Register_Virtual_Port
;
	mov	edx,0bae8h		;register port BAE8H...
	mov	cl,WORD_LENGTHED	;as a word lengthed port
	VxDCall VDD_Register_Virtual_Port
;
	mov	edx,0aae8h		;register port AAE8H...
	mov	cl,WORD_LENGTHED	;as a word lengthed port
	VxDCall VDD_Register_Virtual_Port
;
	mov	edx,0aee8h		;register port AEE8H...
	mov	cl,WORD_LENGTHED	;as a word lengthed port
	VxDCall VDD_Register_Virtual_Port
;
	mov	edx,0a2e8h		;register port A2E8H...
	mov	cl,WORD_LENGTHED	;as a word lengthed port
	VxDCall VDD_Register_Virtual_Port
;
	mov	edx,0a6e8h		;register port A6E8H...
	mov	cl,WORD_LENGTHED	;as a word lengthed port
	VxDCall VDD_Register_Virtual_Port
;
	mov	edx,92e8h		;register port 92E8H...
	mov	cl,WORD_LENGTHED	;as a word lengthed port
	VxDCall VDD_Register_Virtual_Port
;
	mov	edx,0b2e8h		;register port B2E8H...
	mov	cl,WORD_LENGTHED	;as a word lengthed port
	VxDCall VDD_Register_Virtual_Port
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
;this is port 4AE8H which is written to during ROM BIOS mode changes.
;If we're running a DOS VM in a window, we don't want to touch 4AE8H
;just because the "virtual" DOS box is doing a mode change (we're running
;in a window so we really are "faking" the ROM BIOS's mode change).  Thus,
;we trap 4AE8H at the appropriate times and "eat" the write to it if
;necessary.  See the code at MiniVDD_Virtual4AE8H for more details.  Anyway,
;we set up a direct trapping procedure for 4AE8H with the VMM and avoid
;any dealings with the "master" VDD for this special port.
;
;S3 BLTer register 4AE8H is a relic from the 8514/A compatibility.  On the S3
;chipsets, its functionality has been superceded by the S3 extension (CRTC)
;registers such as CRTC register 40H.  However, on certain chipsets, it can
;cause us problems if it is set to the wrong state.  We therefore need to
;virtualize it and discard writes to it if we are running a VGA application
;in a window.  Therfore, we setup its virtualization here.  We do not begin
;trapping it until the first old VGA app begins executing.  From that point
;on in the session, we virtualize it until the user quits Windows.
;
	mov	esi,OFFSET32 MiniVDD_Virtual4AE8H
	mov	edx,4ae8h		;
	VMMCall Install_IO_Handler	;
	jc	MVDI_ErrorExit		;oops, couldn't install procedure
	mov	edx,4ae8h		;
	VMMCall Disable_Global_Trapping ;don't trap yet, set it later
	mov	edx,4ae9h		;
	VMMCall Install_IO_Handler	;
	jc	MVDI_ErrorExit		;oops, couldn't install procedure
	VMMCall Disable_Global_Trapping ;don't trap yet, set it later
;
MVDI_GoodExit:
	mov	al,ChipID		;return ChipID to caller in AL
	clc				;return success
	jmp	MVDI_Exit		;
;
MVDI_ErrorExit:
	stc
;
MVDI_Exit:
	ret
EndProc MiniVDD_Dynamic_Init
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
	movzx	eax,ChipID		;always return 'V7' to caller
	ret				;
EndProc 	MiniVDD_GetChipID
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
	Control_Dispatch Sys_Critical_Init   MiniVDD_Sys_Critical_Init
	Control_Dispatch Device_Init,	     MiniVDD_Dynamic_Init
	Control_Dispatch Sys_Dynamic_Device_Init, MiniVDD_Dynamic_Init
End_Control_Dispatch MiniVDD
;
;
subttl		Register Display Driver Dependent Data
page +
public	MiniVDD_RegisterDisplayDriver
BeginProc MiniVDD_RegisterDisplayDriver, RARE
;
;This will only be called if S3.DRV is the display driver, not if VGA.DRV
;or SUPERVGA.DRV are the display drivers.
;
;Oft-times, the display driver must set flags whenever it is doing
;certain things to a hardware BLTer (such as "I'm in the middle of
;transferring a source bitmap").  This routine is called to allow
;this mini-VDD to get data (usually containing addresses inside the
;display driver's Data segment) directly from the display driver.
;
;	In this example, we obtain several pieces of data from the
;display driver.  First, the address (in the driver's Data segment)
;of a flag called InSrcBLT.  Since the S3 card is always forced into
;a "hardware busy" state whenever a bitmap is being transferred
;through the BLTer, the display driver sets this flag before
;beginning a transfer of a bitmap to the BLTer.  We then read the
;state of this flag in the routine MiniVDD_CheckHardwareBusy.  If
;the flag is set, we simply skip the hardware busy check and return
;success.  In order to access this flag (which is in a Data segment
;running in a different CPU protection ring), we must obtain its
;linear address by using the VMM service Map_Flat.  We are also
;passed some register states which are used by the mini-VDD to
;change the state of the hardware from a banked state to a linear
;state.  This is peculiar to the S3 hardware and may not be necessary
;on your particular device.
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
;	Client_DS:Client_DX --> InSrcBLTFlag in display driver's Data segment.
;	Client_DS:Client_SI --> CurrentChipMode in display driver's Data segment.
;	Client_CL contains value for Windows VM state of S3 register 5AH.
;	Client_CH contains value for Windows VM state of S3 register 59H.
;	Client_ECX (byte 3) is zero if not banking, non-zero if so.
;	Client_ECX (byte 2) contains value for Windows VM state of register 58H.
;
;Use the VMM service Map_Flat to obtain the linear address of InSrcBLT whose
;Selector:Offset address was passed to us in the display driver's DS:DX.
;
	mov	ax,(Client_DS SHL 8) + Client_DX
	VMMcall Map_Flat		;returns EAX = the linear address
	mov	InSrcBLTAddr,eax	;save it in linear format
;
;Use the VMM service Map_Flat to obtain the linear address of CurrentChipMode
;whose Selector:Offset address was passed to us in the display driver's DS:SI:
;
	mov	ax,(Client_DS SHL 8) + Client_SI
	VMMcall Map_Flat		;returns EAX = the linear address
	mov	CurrentChipModeAddr,eax ;save it in linear format
;
;Use the VMM service Map_Flat to obtain the linear address of bMMIOActive
;whose Selector:Offset address was passed to us in the display driver's DS:DI:
;
	mov	ax,(Client_DS SHL 8) + Client_DI
	VMMcall Map_Flat		;returns EAX = the linear address
	mov	bMMIOActiveAddr,eax	;save it in linear format
;
;Save off the various register states contained in Client_ECX:
;
	mov	ecx,[ebp].Client_ECX	;get contents of client's ECX register
	mov	al,5ah			;setup value for register 5AH in AX
	mov	ah,cl			;now AX has LAWAddrRegLow
	mov	LAWAddrRegLow,ax	;
	mov	al,59h			;setup value for register 59H in AX
	mov	ah,ch			;now AX has LAWAddrRegHigh
	mov	LAWAddrRegHigh,ax	;
	shr	ecx,16			;get access to LAWApertureSizeBits
	mov	LAWApertureSizeBits,cl	;save these for later use
	mov	VFLATDFlag,ch		;save flag for later use in mini-VDD
;
;There is a bug on certain Dell PC's with the S3 cards that results in
;loss of synch when switching from full-screen back into Windows HiRes mode.
;This is caused by a race condition in the Dell ROM BIOS which results in
;a mis-setting of the Miscallaneous Output Register.  Since we know that
;we're in Windows HiRes mode now, we must save off the correct state of
;this register so that we can restore it properly when needed:
;
	mov	edx,3cch		;this is where we read Misc Output Reg
	in	al,dx			;
	mov	WindowsMiscOutputState,al
;
MRDDExit:
	ret				;
EndProc MiniVDD_RegisterDisplayDriver
;
;
subttl		Set Refresh Rates Before Display Driver Sets Mode
page +
public	MiniVDD_PreCRTCModeChange
BeginProc MiniVDD_PreCRTCModeChange, RARE
;
;This routine allows the display driver to call us before it does the
;mode set using INT 10H.  This MiniVDD is going to use the routine to
;get the refresh rate by getting a DISPLAYINFO data structure (see
;MINIVDD.INC) and then setting the refresh rate in the BIOS scratchpad
;registers 41H and 52H.
;
;Entry:
;	Nothing assumed.
;Exit:
;	Nothing assumed.
;
;The Main VDD has already retrieved the Refresh_Rate= and X_Resolution entries
;from SYSTEM.INI and will return those values if the registry has nothing
;better to offer.
;
	mov	eax,OFFSET32 DisplayInfoStructure
	mov	ecx,SIZE DISPLAYINFO	;pass size in ECX, address in EAX
	mov	[eax].diHdrSize,cx	;fill in this entry
	push	eax			;save EAX --> DisplayInfoStructure
	VxDCall VDD_Get_DISPLAYINFO	;get information from the VDD
	pop	eax			;restore EAX --> DisplayInfoStructure
	test	[eax].diInfoFlags,MONITOR_INFO_DISABLED_BY_USER OR \
			MONITOR_INFO_NOT_VALID OR REGISTRY_RESOLUTION_NOT_VALID
	jnz	MPMCExit		;we are not to set the refresh rate!
	movzx	edi,[eax].diRefreshRateMax
	or	edi,edi 		;any valid data there?
	jz	MPMCExit		;nope, let him suffer
	movzx	esi,[eax].diXRes	;
	or	esi,esi 		;any valid data there?
	jz	MPMCExit		;nope, let him suffer
;
public	MPMCDispatch
MPMCDispatch:
;
;First, unlock the S3 CRTC registers, just in case:
;
	mov	edx,3d4h		;EDX --> CRTC index register
	in	al,dx			;get current index value
	push	eax			;save this index value
	mov	eax,0a539h		;unlock the registers (including 36H)
	out	dx,ax			;
;
;Now, dispatch:
;
	cmp	ChipID,S3911		;on an S3-911 or 924?
	je	MPMCSeries1Refresh	;yes, go handle it
	cmp	ChipID,S3801		;on an S3-801?
	je	MPMCSeries2Refresh	;yes, go handle it
	cmp	ChipID,S3805		;on an S3-805?
	je	MPMCSeries2Refresh	;yes, go handle it
	cmp	ChipID,S3928		;on an S3-928?
	je	MPMCSeries2Refresh	;yes, go handle it
	cmp	ChipID,S3928PCI 	;on an S3-928-PCI?
	je	MPMCSeries2Refresh	;yes, go handle it
	cmp	ChipID,S3864		;on an S3-864?
	je	MPMCSeries3Refresh	;yes, go handle it
	cmp	ChipID,S3964		;on an S3-964?
	je	MPMCSeries3Refresh	;yes, go handle it
	cmp	ChipID,S3764		;on an S3-764?
	je	MPMCSeries3Refresh	;yes, go handle it
	cmp	ChipID,S3732		;on an S3-732?
	je	MPMCSeries3Refresh	;yes, go handle it
	cmp	ChipID,S3866		;on an S3-866?
	je	MPMCSeries3Refresh	;yes, go handle it
	cmp	ChipID,S3868		;on an S3-868?
	je	MPMCSeries3Refresh	;yes, go handle it
	cmp	ChipID,S3968		;on an S3-968?
	je	MPMCSeries3Refresh	;yes, go handle it
;
public	MPMCSeries1Refresh
MPMCSeries1Refresh:
	mov	al,41h			;set to index 41H
	out	dx,al			;
	inc	dl			;EDX --> CRTC data register
	in	al,dx			;get current value of register 41H
	and	al,NOT 07h		;mask out the monitor refresh rate bits
;
public	MPMC1Dispatch
MPMC1Dispatch:
	cmp	esi,640 		;running 640x480?
	je	MPMC1_640		;yes, continue
	cmp	esi,800 		;running 800x600?
	je	MPMC1_800		;yes, continue
	cmp	esi,1024		;running 1024x768?
	je	MPMC1_1024		;yes, continue
;
public	MPMC1_1280
MPMC1_1280:
	mov	ah,7			;set interlaced mode unconditionally
	jmp	MPMC1Cleanup		;and we're done
;
public	MPMC1_1024
MPMC1_1024:
	mov	ah,7			;setup for 1024x768x70 Hz
	cmp	edi,70			;does he want 70 Hz or above?
	jae	MPMC1Cleanup		;yes! go do it
	mov	ah,6			;setup for 1024x768x60 Hz
	cmp	edi,60			;does he want 60 Hz or above?
	jae	MPMC1Cleanup		;yes! go do it
	mov	ah,5			;setup for 1024x768x43 Hz
	jmp	MPMC1Cleanup		;yes! go do it
;
public	MPMC1_800
MPMC1_800:
	mov	ah,3			;setup for 800x600x72 Hz
	cmp	edi,72			;does he want 72 Hz or above?
	jae	MPMC1Cleanup		;yes! go do it
	mov	ah,2			;setup for 800x600x60 Hz
	cmp	edi,60			;does he want 60 Hz or above?
	jae	MPMC1Cleanup		;yes! go do it
	mov	ah,4			;setup for 800x600x56 Hz
	jmp	MPMC1Cleanup		;yes! go do it
;
public	MPMC1_640
MPMC1_640:
	mov	ah,1			;setup for 640x480x72 Hz
	cmp	edi,72			;does he want 72 Hz or above?
	jae	MPMC1Cleanup		;yes! go do it
	xor	ah,ah			;setup for 640x480x60 Hz
;
public	MPMC1Cleanup
MPMC1Cleanup:
;
;At this point:
;	AH contains our refresh rate bits.
;	AL contains the other bits contained in register 41H
;	EDX --> CRTC Data register.
;
	or	al,ah			;combine in refresh rate bits
	out	dx,al			;set the refresh rate
	jmp	MPMCRestoreCRTC 	;and go finish up
;
public	MPMCSeries2Refresh
MPMCSeries2Refresh:
;
;First, determine the mask for the resolution:
;
	mov	ch,03h			;assume 640x480
	cmp	esi,640 		;running 640x480?
	je	@F			;yes, go determine refresh rate value
;
	mov	ch,0ch			;assume 800x600
	cmp	esi,800 		;running 800x600?
	je	@F			;yes, go determine refresh rate value
;
	mov	ch,30h			;assume 1024x768
	cmp	esi,1024		;running 1024x768?
	je	@F			;yes, go determine refresh rate value
;
	mov	ch,30h			;assume 1152x864
	cmp	esi,1152		;running 1152x864?
	je	@F			;yes, go determine refresh rate value
;
	mov	ch,0c0h 		;assume 1280x1024
	cmp	esi,1280		;running 1280x1024?
	jne	MPMCRestoreCRTC 	;nope, we don't understand him
;
;Now, get the value of CRTC register 52H and mask out the resolution bits
;that are set in it:
;
@@:	mov	cl,ch			;copy this for masking purposes
	not	cl			;and make a protect mask in CL
;
;Now, determine the refresh rate and put the appropriate value into AH:
;
	mov	ah,0ffh 		;assume 72 Hz
	cmp	edi,72			;does he want 72 Hz?
	jae	@F			;yes, go set the physical value
;
	mov	ah,0aah 		;assume 60 Hz
	cmp	edi,60			;does he want 60 Hz?
	jae	@F			;yes, go set the physical value
;
	mov	ah,55h			;assume 56 Hz
	cmp	edi,56			;does he want 56 Hz?
	jae	@F			;yes, go set the physical value
;
	xor	ah,ah			;he gets 43 Hz interlaced
;
;Get the current value of register 52H, mask in our resolution's bits, and
;write the correct value back:
;
@@:	mov	al,52h			;this is S3 scratchpad register index
	out	dx,al			;
	inc	edx			;EDX --> CRTC data register
	in	al,dx			;get current value of this register
	and	ax,cx			;mask out bits of interest to us
	or	al,ah			;combine in the new bits
	out	dx,al			;and we're done setting the refresh!
	jmp	MPMCRestoreCRTC 	;
;
public	MPMCSeries3Refresh
MPMCSeries3Refresh:
;
;For the 732 and 764 chipsets, we read from register 42H. For the others
;series 3/4 chips, we use register 5BH:
;
	mov	ah,42h			;assume 732 or 764
	cmp	ChipID,S3764		;running on a 764?
	je	MPMC3_640		;yes, we've got proper register
	cmp	ChipID,S3732		;running on a 732?
	je	MPMC3_640		;yes, we've got proper register
	mov	ah,5bh			;we have an 86x or 96x series chipset
;
;Get the value for the particular resolution that we want:
;
public	MPMC3_640
MPMC3_640:
;
;At 640x480, the user can select 60 Hz, 72 Hz, or 75 Hz refresh rates:
;
	cmp	esi,640 		;running 640x480?
	jne	MPMC3_800		;nope, go try 800x600
	mov	cx,1000111100100000b	;CH has protect mask, CL has rate bits
	cmp	edi,75			;does he want 75 Hz or greater?
	jae	@F			;yes, go set it up!
	mov	cx,1000111100010000b	;CH has protect mask, CL has rate bits
	cmp	edi,72			;does he want 72 Hz?
	jae	@F			;yes, go set it up!
	mov	cx,1000111100000000b	;CH has protect mask, CL has rate bits
					;he must want 60 Hz
;
;Get the current value of register 52H, mask in our resolution's bits, and
;write the correct value back:
;
@@:	mov	al,52h			;this is S3 scratchpad register index
	out	dx,al			;
	inc	edx			;EDX --> CRTC data register
	in	al,dx			;get current value of this register
	and	al,ch			;mask out bits of interest to us
	or	al,cl			;combine in the new bits
	out	dx,al			;and we're done setting the refresh!
	jmp	MPMCRestoreCRTC 	;
;
public	MPMC3_800
MPMC3_800:
;
;At 800x600, the user can select 56 Hz, 60 Hz, 72 Hz, or 75 Hz refresh rates:
;
	cmp	esi,800 		;running 800x600?
	jne	MPMC3_1024		;nope, go try 1024x768
	mov	cx,0000000110000000b	;CH has 5BH/42H value, CL has 52H value
	cmp	edi,75			;does he want 75 Hz or greater?
	jae	@F			;yes, go set it up!
	mov	cx,0000000100000000b	;CH has 5BH/42H value, CL has 52H value
	cmp	edi,72			;does he want 72 Hz?
	jae	@F			;yes, go set it up!
	mov	cx,0000000010000000b	;CH has 5BH/42H value, CL has 52H value
	cmp	edi,60			;does he want 60 Hz?
	jae	@F			;yes, go set it up!
	mov	cx,0000000000000000b	;CH has 5BH/42H value, CL has 52H value
					;he must want 56 Hz
;
;Get the current value of register 52H, mask in our resolution's bits, and
;write the correct value back:
;
@@:	mov	al,52h			;this is S3 scratchpad register index
	out	dx,al			;
	inc	edx			;EDX --> CRTC data register
	in	al,dx			;get current value of this register
	and	al,NOT 80h		;mask out bits of interest to us
	or	al,cl			;combine in the new bits
	out	dx,al			;and write out register 52H
	dec	edx			;EDX --> CRTC index register
;
;Now, register 5BH (42H on the 732 & 764 series):
;
	mov	al,ah			;get proper index to set
	out	dx,al			;
	inc	edx			;EDX --> CRTC data register
	in	al,dx			;get current value of this register
	and	al,NOT 03h		;mask out bits of interest to us
	or	al,cl			;combine in the new bits
	out	dx,al			;and write out register 5BH/42H
	jmp	MPMCRestoreCRTC 	;and we're done setting the refresh!
;
public	MPMC3_1024
MPMC3_1024:
;
;At 1024x768, the user can select 43 Hz, 60 Hz, 70 Hz, or 75 Hz refresh rates:
;
	cmp	esi,1024		;running 1024x768?
	je	@F			;yes, continue
	cmp	esi,1152		;running 1152x864?
	jne	MPMC3_1280		;nope, go try 1280x1024
@@:	mov	cl,00010000b		;CL has 5BH/42H value
	cmp	edi,75			;does he want 75 Hz or greater?
	jae	@F			;yes, go set it up!
	mov	cl,00001100b		;CL has 5BH/42H value
	cmp	edi,70			;does he want 70 Hz?
	jae	@F			;yes, go set it up!
	mov	cl,00001000b		;CL has 5BH/42H value
	cmp	edi,60			;does he want 60 Hz?
	jae	@F			;yes, go set it up!
	mov	cl,00000000b		;CL has 5BH/42H value
					;he must want 43 Hz (Interlaced)
;
;Get the current value of register 5BH (42H on the 764 series), mask in our
;resolution's bits, and write the correct value back:
;
@@:	mov	al,ah			;get proper index register value
	out	dx,al			;
	inc	edx			;EDX --> CRTC data register
	in	al,dx			;get current value of this register
	and	al,NOT 1ch		;mask out bits of interest to us
	or	al,cl			;combine in the new bits
	out	dx,al			;and write out register 5BH/42H
	jmp	MPMCRestoreCRTC 	;and we're done setting the refresh!
;
public	MPMC3_1280
MPMC3_1280:
;
;At 1280x1024, the user can select 45 Hz, 60 Hz, 72 Hz, or 75 Hz refresh rates:
;
	cmp	esi,1280		;running 1280x1024?
	jne	MPMCRestoreCRTC 	;nope, we don't understand him
	mov	cl,11000000b		;CL has 5BH/42H value
	cmp	edi,75			;does he want 75 Hz or greater?
	jae	@F			;yes, go set it up!
	mov	cl,10100000b		;CL has 5BH/42H value
	cmp	edi,72			;does he want 72 Hz?
	jae	@F			;yes, go set it up!
	mov	cl,10000000b		;CL has 5BH/42H value
	cmp	edi,60			;does he want 60 Hz?
	jae	@F			;yes, go set it up!
	mov	cl,00000000b		;CL has 5BH/42H value
					;he must want 45 Hz (Interlaced)
;
;Get the current value of register 5BH (42H on the 764 series), mask in our
;resolution's bits, and write the correct value back:
;
@@:	mov	al,ah			;get proper index register to set
	out	dx,al			;
	inc	edx			;EDX --> CRTC data register
	in	al,dx			;get current value of this register
	and	al,NOT 0e0h		;mask out bits of interest to us
	or	al,cl			;combine in the new bits
	out	dx,al			;and write out register 5BH/42H
;
public	MPMCRestoreCRTC
MPMCRestoreCRTC:
	pop	eax			;restore previous CRTC index register
	dec	edx			;EDX --> CRTC index register
	out	dx,al			;
;
MPMCExit:
	ret				;return to main VDD
EndProc MiniVDD_PreCRTCModeChange
;
;
subttl		Calculate and Save Banking Register Values
page +
public	MiniVDD_GetVDDBank
BeginProc MiniVDD_GetVDDBank, DOSVM
;
;NOTE: This routine is the first one called after the display driver sets
;      itself into Windows HiRes mode after initialization.  We therefore
;      set the DisplayEnabledFlag here:
;
;This will only be called if S3.DRV is the display driver, not if VGA.DRV
;or SUPERVGA.DRV are the display drivers.  Thus, we set SuperVGAModeFlag=FFH
;in this routine.
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
;Exit:
;	AH contains the page offset from start of bank of virtualization area.
;	EBX must still contain the Windows VM handle.
;	ECX contains the starting address of the VDD virtualization area
;	EDX contains the amount of memory that we allocated to
;	the VDD virtualization area.  We set EDX == ECX if we are in
;	a "memory shy" configuration.  That is, to tell the "main"
;	VDD that we cannot support VGA 4 plane graphics in the background.
;
	mov	DisplayEnabledFlag,0ffh ;indicate that display is in HiRes mode
	mov	SuperVGAModeFlag,0ffh	;indicate that display driver is not VGA
;
;For the S3 chipsets, our banks are 64K in length.  So, let's find out in
;which bank the off-screen memory starts:
;
	mov	eax,ecx 		;get start address of off-screen memory
	shr	eax,16			;divide by 64K per bank -- AL has bank #
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
	xor	cx,cx			;zero the offset in low word of ECX
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
;	AL contains the bank number for the VDD virtualization bank.
;	ECX contains the 32 bit start address of the VDD virtualization area.
;	EDX contains the size of the VDD virtualization area (either 32K,
;	    64K, or -1 indicating a memory shy configuration).
;
;We should setup the values for the S3 banking registers so that we can set
;them quickly when called upon by the "main" VDD to do so:
;
	cmp	NewBankingSupport,0	;are we on a 928 or below?
	je	@F			;yes, go setup registers 35H and 51H
	mov	VDDBankLow,al		;we're on a new chipset
%out Uncomment the following block post Win '95 RTM (bug 13111)
comment |
;
;The 732 and 764 (Trio) cards don't use bits 6 & 7 of the banking register
;6AH.  Therefore, we want to create a mask (which we call Register6AMask)
;which is normally FFH, but will be set to 3FH for the Trio cards:
;
	mov	Register6AMask,3fh	;assume we're on a 764
	cmp	ChipID,S3764		;is it a 764?
	je	MGVBExit		;yes, we have the proper mask
	cmp	ChipID,S3732		;is it a 732?
	je	MGVBExit		;yes, we have the proper mask
	mov	Register6AMask,0ffh	;it's not a Trio, unset the mask

end comment |
	jmp	MGVBExit		;
@@:
	push	edx			;save size of VDD area for now
	push	ebx			;we need this as a work register
	mov	bl,al			;copy bank number to BL & BH for now
	mov	bh,al			;
	and	bl,0fh			;get low bits of VDD bank in BL
	shr	bh,4			;get high bits of VDD bank in BH
	mov	edx,3d4h		;EDX --> CRTC index register
	mov	al,35h			;set to low banking register
	out	dx,al			;
	inc	edx			;EDX --> CRTC data register
	in	al,dx			;get current value of low bank reg
	and	al,NOT 0fh		;get rid of all banking bits
	or	al,bl			;now AL has low bank value for VDD bank
	mov	VDDBankLow,al		;save this for use in virtualization
	mov	VDDBankHigh,0		;assume we're on an S3-911
	cmp	ChipID,S3911		;are we running on an S3-911 or S3-924?
	je	@F			;yes, there's no high banking stuff!
	dec	edx			;EDX --> CRTC index register
	mov	al,51h			;set to high banking register
	out	dx,al			;
	inc	edx			;
	in	al,dx			;get current value of high bank reg
	and	al,NOT 0ch		;get rid of banking bits in register
	shl	bh,2			;get VDD high bank bits in position
	or	al,bh			;now AL has VDD high bank value
	mov	VDDBankHigh,al		;save this for use in virtualization
@@:	pop	ebx			;restore saved registers
	pop	edx			;
	jmp	MGVBExit		;and we're done
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
;	ECX contains the value passed in at the beginning of the routine.
;
	mov	edx,ecx 		;indicate a "memory-shy" configuration
;
MGVBExit:
	xor	ah,ah			;we never have banking offsets
	ret
EndProc MiniVDD_GetVDDBank
;
;
subttl		Banking Handling Code for VGA Virtualization
page +
public	MiniVDD_SetVDDBank
BeginProc MiniVDD_SetVDDBank, DOSVM
;
;This routine is called when we need to virtualize something in the
;off-screen region of the video memory.  You should save the current
;state of the banking registers and then set your banking registers to
;the off-screen memory region.
;
;Entry:
;	EBX contains the MemC owner's VM handle.
;Exit:
;	Save any registers that you use.
;
	cmp	BankRegSaveLow,0ffh	;are we already set to the VDD bank?
	jne	MSTVExit		;yes! don't do it again!
	cmp	SuperVGAModeFlag,0	;are we running S3.DRV?
	je	MSTVExit		;nope, don't do anything here
	push	eax			;
	push	edx			;
	mov	edx,3d4h		;EDX --> CRTC index register
	in	al,dx			;get current CRTC index
	ror	eax,8			;save it in top byte of EAX
;
;Let's make sure that the S3 extended registers are unlocked.  This is
;probably not necessary when our CRTC VM is the Windows VM but most likely
;is when the CRTC VM is not the Windows VM:
;
	mov	ax,4838h		;unlock the S3 registers
	out	dx,ax			;
	mov	ax,0a539h		;
	out	dx,ax			;
;
;Make sure banking is turned on!
;
	mov	al,31h			;
	out	dx,al			;
	inc	edx			;
	in	al,dx			;
	mov	BankEnableSave,al	;save current state for later restore
	or	al,01h			;enable banking
	out	dx,al			;
	dec	edx			;EDX --> CRTC index register
;
;Dispatch to either the new-style banking or the old style:
;
	cmp	NewBankingSupport,0	;on a 928 or older chipset?
	je	MSTVOldBanking		;yes, go do it with 35H and 51H
;
public	MSTVNewBanking
MSTVNewBanking:
	mov	al,6ah			;set to new banking register 6AH
	out	dx,al			;
	inc	dl			;EDX --> CRTC data register
	in	al,dx			;now AL has current bank state
	mov	BankRegSaveLow,al	;save this for later restore
	mov	al,VDDBankLow		;set to our reserved bank
	out	dx,al			;
	dec	dl			;EDX --> CRTC index register
	jmp	@F			;we're done setting VDD bank!
;
public	MSTVOldBanking
MSTVOldBanking:
	mov	al,35h			;set to low banking register (35H)
	out	dx,al			;
	inc	edx			;
	in	al,dx			;now AL has current bank state
	mov	BankRegSaveLow,al	;save this for later restore
	mov	al,VDDBankLow		;set to our reserved bank
	out	dx,al			;
	dec	edx			;EDX --> CRTC index register
;
	cmp	ChipID,S3911		;running on the S3-911?
	je	@F			;yes, no register 51H on S3-911
	mov	al,51h			;now get the high banking register
	out	dx,al			;
	inc	edx			;
	in	al,dx			;
	mov	BankRegSaveHigh,al	;
	mov	al,VDDBankHigh		;set to our reserved bank
	out	dx,al			;
	dec	edx			;EDX --> CRTC index register
;
@@:	rol	eax,8			;AL now contains saved CRTC index value
	out	dx,al			;
	pop	edx			;restore saved registers
	pop	eax			;
;
MSTVExit:
	ret				;
EndProc MiniVDD_SetVDDBank
;
;
public	MiniVDD_ResetBank
BeginProc MiniVDD_ResetBank, DOSVM
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
;No need to check the SuperVGAModeFlag here because if we're running VGA.DRV
;or SUPERVGA.DRV, BankRegSaveLow will always be FFH:
;
	cmp	BankRegSaveLow,0ffh	;is there any banking to restore?
	je	MRBExit 		;nope, skip this!
;
;We may need to do something.  Save off the current CRTC index register state:
;
	push	eax			;
	push	edx			;
	mov	edx,3d4h		;
	in	al,dx			;get current CRTC index
	ror	eax,8			;save it in top byte of EAX
;
;Probably not necessary when our CRTC VM is the Windows VM but most likely
;is when the CRTC VM is not the Windows VM:
;
	mov	ax,4838h		;unlock the S3 registers
	out	dx,ax			;
	mov	ax,0a539h		;
	out	dx,ax			;
;
;Dispatch to either the new-style banking or the old style:
;
	cmp	NewBankingSupport,0	;on a 928 or older chipset?
	je	MRBOldBanking		;yes, go do it with 35H and 51H
;
public	MRBNewBanking
MRBNewBanking:
	mov	al,6ah			;set to new banking register 6AH
	out	dx,al			;
	inc	dl			;EDX --> CRTC data register
	in	al,dx			;now AL has current bank state
%out Uncomment the following line post-Win '95 RTM (bug 13111)
comment |
	and	al,Register6AMask	;get rid of undefined bits on Trio
end comment |
	cmp	al,VDDBankLow		;still set to "our" bank?
	jne	MRBResetBankSaveFlags	;nope, don't physically reset bank regs
	mov	al,BankRegSaveLow	;get back bank state to restore
	out	dx,al			;
	jmp	MRBResetBankSaveFlags	;and we're done
;
public	MRBOldBanking
MRBOldBanking:
;
;Get the values of both banking registers so we can see if they're still
;set to "our" bank.  If they aren't, then Windows has already switched
;them to something for its own purposes and we'd best not restore them!
;
	xor	ah,ah			;assume we're on the S3-911
	cmp	ChipID,S3911		;running on the S3-911?
	je	@F			;yes, no register 51H on the 911!
;
	mov	al,51h			;get high banking register first
	out	dx,al			;
	inc	edx			;
	in	al,dx			;
	mov	ah,al			;save it in AH
	dec	edx			;
;
@@:	mov	al,35h			;get low banking register
	out	dx,al			;
	inc	edx			;
	in	al,dx			;into AL
;
	mov	dl,VDDBankLow		;
	mov	dh,VDDBankHigh		;will be set to zero on the S3-911
	cmp	ax,dx			;are registers still set to VDD bank?
	jne	MRBResetBankSaveFlags	;nope, don't physically reset bank regs
;
;It's safe to restore the banking registers!
;
	mov	edx,3d5h		;reset EDX --> CRTC data register
	mov	al,BankRegSaveLow	;get bank to restore
	out	dx,al			;
	dec	edx			;DX --> CRTC index register
	cmp	ChipID,S3911		;running on the S3-911?
	je	@F			;yes! no register 51H on the 911
	mov	al,51h			;do the same for the high banking reg
	out	dx,al			;
	inc	edx			;bump to CRTC data register
	mov	al,BankRegSaveHigh	;get bank to restore
	out	dx,al			;
	dec	edx			;
;
;Restore the saved banking enable flag that we saved when we set to the
;VDD bank:
;
@@:	mov	al,31h			;this is index of banking enable flag
	mov	ah,BankEnableSave	;this is value to restore
	out	dx,ax			;restore the register
;
public	MRBResetBankSaveFlags
MRBResetBankSaveFlags:
	mov	BankRegSaveLow,0ffh	;flag that we're not set to VDD bank
;
;Lastly, restore the CRTC index register that we saved earlier.
;
	mov	edx,3d4h		;EDX --> CRTC index register
	rol	eax,8			;AL now contains saved CRTC index value
	out	dx,al			;
	pop	edx			;restore saved registers
	pop	eax			;
;
MRBExit:
	ret				;
EndProc MiniVDD_ResetBank
;
;
subttl		Set To Latch Scratchpad Bank
page +
public	MiniVDD_SetLatchBank
BeginProc MiniVDD_SetLatchBank, DOSVM
;
;When virtualizing the VGA 4 plane mode, we have to save and restore the
;latches occasionally.	This routine allows you to set to an off-screen
;bank (in this case and in most cases, the VDD bank) in order to prepare
;for restoring the VGA latches.  This routine is NOT called for saving
;the latches since this is done by simply using the standard VGA CRTC
;register 22H which all super-VGA's possess (we hope).
;
;Entry:
;	Nothing assumed.
;Exit:
;	Save anything that you use.
;
	push	eax			;save registers that we use
	push	edx			;
	cmp	SuperVGAModeFlag,0	;are we running S3.DRV?
	je	MSLBExit		;nope, don't do anything here
;
	mov	edx,3d4h		;EDX --> CRTC index register
	in	al,dx			;get and save CRTC index value
	ror	eax,8			;
;
	mov	ax,4838h		;make sure we can access S3 regs
	out	dx,ax			;
	mov	ax,0a539h		;
	out	dx,ax			;
;
;Now, make sure banking is enabled:
;
	mov	al,31h			;
	out	dx,al			;
	inc	edx			;
	in	al,dx			;
	mov	LatchSave31H,al 	;
	or	al,01h			;
	out	dx,al			;
	dec	edx			;
;
;Dispatch to either the new-style banking or the old style:
;
	cmp	NewBankingSupport,0	;on a 928 or older chipset?
	je	MSLBOldBanking		;yes, go do it with 35H and 51H
;
public	MSLBNewBanking
MSLBNewBanking:
	mov	al,6ah			;set to new banking register 6AH
	out	dx,al			;
	inc	dl			;EDX --> CRTC data register
	in	al,dx			;now AL has current bank state
	mov	LatchSave35H,al 	;save this for later restore
	mov	al,VDDBankLow		;set to our reserved bank
	out	dx,al			;
	dec	dl			;EDX --> CRTC index register
	jmp	@F			;we're done setting VDD bank!
;
public	MSLBOldBanking
MSLBOldBanking:
;
;Set to our reserved latch bank using banking registers 35H and 51H:
;
	mov	al,35h			;
	out	dx,al			;
	inc	edx			;
	in	al,dx			;
	mov	LatchSave35H,al 	;
	mov	al,VDDBankLow		;
	out	dx,al			;
	dec	edx			;
;
	cmp	ChipID,S3911		;running on an S3-911 or S3-924?
	je	@F			;yep, don't fool with register 51H
	mov	al,51h			;
	out	dx,al			;
	inc	edx			;
	in	al,dx			;
	mov	LatchSave51H,al 	;
	mov	al,VDDBankHigh		;
	out	dx,al			;
	dec	edx			;
;
;Restore the CRTC index register:
;
@@:	rol	eax,8			;
	out	dx,al			;
;
MSLBExit:
	pop	edx			;restore saved registers
	pop	eax			;
	ret				;
EndProc MiniVDD_SetLatchBank
;
;
subttl		Reset Banking After Latch Operations
page +
public	MiniVDD_ResetLatchBank
BeginProc MiniVDD_ResetLatchBank, DOSVM
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
	cmp	SuperVGAModeFlag,0	;are we running S3.DRV?
	je	MRLBExit		;nope, don't do anything here
;
	mov	edx,3d4h		;EDX --> CRTC index register
	in	al,dx			;get and save CRTC index value
	ror	eax,8			;
;
;Restore banking-enabling register:
;
	mov	al,31h			;
	mov	ah,LatchSave31H 	;
	out	dx,ax			;
;
;Dispatch to either the new-style banking or the old style:
;
	cmp	NewBankingSupport,0	;on a 928 or older chipset?
	je	MRLBOldBanking		;yes, go do it with 35H and 51H
;
public	MRLBNewBanking
MRLBNewBanking:
	mov	al,6ah			;set to new banking register 6AH
	mov	ah,LatchSave35H 	;
	out	dx,ax			;
	jmp	@F			;and we're done
;
public	MRLBOldBanking
MRLBOldBanking:
;
;Reset the low banking register:
;
	mov	al,35h			;
	mov	ah,LatchSave35H 	;
	out	dx,ax			;
;
	cmp	ChipID,S3911		;running on an S3-911 or S3-924?
	je	@F			;yep, don't fool with register 51H
	mov	al,51h			;
	out	dx,al			;
	inc	edx			;
	mov	al,LatchSave51H 	;do it separately to avoid screen jump
	out	dx,al			;
	dec	edx			;
;
;Restore the CRTC index register:
;
@@:	rol	eax,8			;
	out	dx,al			;
;
MRLBExit:
	pop	edx			;restore saved registers
	pop	eax			;
	ret				;
EndProc MiniVDD_ResetLatchBank
;
;
subttl		Prepare to Enter a Standard VGA Mode from HiRes Mode
page +
public	MiniVDD_PreHiResToVGA
BeginProc MiniVDD_PreHiResToVGA, DOSVM
;
;This routine will be called if we're running VGA.DRV, SUPERVGA.DRV, or
;S3.DRV and applies to all drivers.
;
;When the VDD is about to switch from Windows HiRes mode to any of the
;standard VGA modes (for real -- not virtualized), this routine will be
;called.  You need to determine exactly what your hardware requires in
;order to accomplish this task.  For example, you should disable trapping
;on registers that the mini-VDD is specifically handling (such as 4AE8H
;in the case of the S3 chipset), make sure that the hardware is "ready"
;to undergo a mode transition (make sure that your hardware's graphics
;engine isn't in the middle of an operation that can't be interrupted)
;etc.  If your hardware does not return to a standard VGA mode via
;a call to INT 10H, function 0, you should also make sure to do whatever
;it takes to restore your hardware to a standard VGA mode at this time.
;Try not to touch video memory during your transition to standard VGA as
;this will disturb the reliability of the system.
;
	or	S3ModeChangeFlags,GOING_TO_VGA_MODE
	mov	eax,CurrentChipModeAddr ;EAX --> display driver's flags
	or	eax,eax 		;using the S3 driver (not VGA)?
	jz	MHTVDisable4AE8Trap	;nope, don't update NULL memory!
	mov	word ptr [eax],MEMORY	;flag that BLTer regs are disabled
;
;The S3 chipset's mode change will get "stuck" if we're in the middle of
;transferring a source bitmap to the graphics engine.  Thus, we flush the
;BLTer if it's busy.
;
;First, make sure the BLTer registers are enabled:
;
	mov	edx,3d4h		;EDX --> CRTC index register
	in	al,dx			;save current index value
	ror	eax,8			;
	mov	ax,0a539h		;unlock S3 extension registers
	out	dx,ax			;
	mov	al,40h			;set to index 40H
	out	dx,al			;
	inc	dl			;EDX --> CRTC data register
	in	al,dx			;
	or	al,01h			;turn on the BLTer
	and	al,NOT 08h		;turn off the fast write stuff
	out	dx,al			;
	dec	dl			;EDX --> CRTC index register
	rol	eax,8			;restore saved CRTC index
	out	dx,al			;
;
public	MHTVDiscardSrcBLT
MHTVDiscardSrcBLT:
	mov	edx,9ae9h		;EDX --> BLT status register
	in	al,dx			;get BLT status
	test	al,02h			;is BLTer busy?
	jz	MHTVDisable4AE8Trap	;nope, continue
	cmp	al,0ffh 		;is this register just garbage?
	je	BYONG			;yes, there's a problem in driver logic!
	mov	edx,0e2e8h		;EDX --> Pixel transfer register
	out	dx,ax			;send out a word
	jmp	MHTVDiscardSrcBLT	;and go try it again!

public	BYONG
BYONG:
	int	3			;OUCH!!!
;
MHTVDisable4AE8Trap:
;
;Next, disable trapping on register 4AE8H which MAY have trapping enabled
;at this time.	Since we're going to do a physical mode change here, we
;want 4AE8H trapping not to be enabled so that the BIOS's write will
;actually go out the physical port.  We will however, reenable the
;trapping in the MiniVDD_PostHiResToVGA routine.
;
	mov	edx,4ae8h		;
	VMMCall Disable_Global_Trapping ;go disable trapping on port in EDX
	mov	edx,4ae9h		;
	VMMCall Disable_Global_Trapping ;go disable trapping on port in EDX
;
MHTVExit:
	ret
EndProc MiniVDD_PreHiResToVGA
;
;
public	MiniVDD_PostHiResToVGA
BeginProc MiniVDD_PostHiResToVGA, DOSVM
;
;Entry:
;	EBX contains the VM handle of the new CRTC owner.
;
;This routine will be called if we're running VGA.DRV, SUPERVGA.DRV, or
;S3.DRV and applies to all drivers.
;
;This routine is called after the ROM BIOS call is made to place the hardware
;back in the standard VGA state.  You should reenable trapping and do any
;other post-processing that your hardware might need:
;
;Reenable the trapping on port 4AE8H that we disabled in the above routine:
;
	mov	edx,4ae8h		;
	VMMCall Enable_Global_Trapping	;go reenable trapping on port in EDX
	mov	edx,4ae9h		;
	VMMCall Enable_Global_Trapping	;go reenable trapping on port in EDX
	mov	S3ModeChangeFlags,0	;flag that we're done with mode change
;
;We have just entered a full screen VGA DOS box and none of the CRTC registers
;were trapped.	We need to get a snapshot of the S3 extended registers in
;this new CRTC mode since we may have inappropriate states saved.
;
	cmp	ebx,MessageModeID	;are we switching to Message Mode?
	je	MPHVExit		;yes, don't do the save!
	call	MiniVDD_SaveRegisters	;go save new state of registers
;
MPHVExit:
	ret				;
EndProc MiniVDD_PostHiResToVGA
;
;
subttl		Prepare to Enter HiRes Mode From a Standard VGA Mode
page +
public	MiniVDD_PreVGAToHiRes
BeginProc MiniVDD_PreVGAToHiRes, DOSVM
;
;We are notified that the VDD is about to call the display driver to
;switch into HiRes mode from a full screen VGA DOS box.  We can do
;anything necessary to prepare for this.  In the case of the S3 VDD,
;we simply set a flag telling us that we're about to change modes.
;
;Entry:
;	EBX contains the Windows VM handle.
;Exit:
;	Nothing assumed.
;
	or	S3ModeChangeFlags,GOING_TO_WINDOWS_MODE
;
MSVHExit:
	ret				;
EndProc MiniVDD_PreVGAToHiRes
;
;
public	MiniVDD_PostVGAToHiRes
BeginProc MiniVDD_PostVGAToHiRes, DOSVM
;
;We are notified that the VDD is done setting the hardware state back
;to HiRes mode.  We simply unset our S3ModeChangeFlags in this case.
;
;Entry:
;	EBX contains the Windows VM handle.
;Exit:
;	Nothing assumed.
;
	mov    S3ModeChangeFlags,0	;
;
MPVHExit:
	ret				;
EndProc MiniVDD_PostVGAToHiRes
;
;
subttl		Save and Restore Routines for Extension Registers
page +
public	MiniVDD_SaveRegisters
BeginProc MiniVDD_SaveRegisters, DOSVM
;
;This routine is called whenever the "main" VDD is called upon to save
;the register state.  The "main" VDD will save all of the states of the
;"normal" VGA registers (CRTC, Sequencer, GCR, Attribute etc.) and will
;then call this routine to allow the mini-VDD to save register states
;that are hardware dependent.  These registers will be restored during the
;routine MiniVDD_RestoreRegisters.  Typically, only a few registers
;relating to the memory access mode of the board are necessary to save
;and restore.  Specific registers that control the hardware BLTer probably
;need not be saved.  The register state is saved in the mini-VDD's
;"CB data area" which is a per-VM state saving area.  The mini-VDD's
;CB data area was allocated at Device_Init time.
;
;The S3 chipset requires that CRTC extension registers be set to proper
;values depending on whether the BLTer or linear access is being used.
;These few registers are the only S3 states that need to be saved and
;restored.
;
;Entry:
;	EBX contains the VM handle for which these registers are being saved.
;Exit:
;	You must preserve EBX, EDX, EDI, and ESI.
;
	cmp	SuperVGAModeFlag,0		;running S3.DRV?
	je	MSRSExit			;nope, don't do this
	push	edx				;must save this register!
	push	edi				;must save this register too!
	mov	edi,ebx 			;point to our personal CB data
	add	edi,OurCBDataPointer		;now EDI --> Our CB data area
	mov	edx,3d4h			;EDX --> CRTC index register
	in	al,dx				;save current index reg value...
	ror	eax,8				;in high byte of EAX
	mov	ax,0a539h			;make sure S3 regs are enabled
	out	dx,ax				;
;
	mov	al,40h				;save S3 CRTC register 40H
	out	dx,al				;
	inc	edx				;
	in	al,dx				;
	dec	edx				;
	mov	[edi].Reg40HState,al		;save this state
;
	mov	al,42h				;save S3 CRTC register 42H
	out	dx,al				;
	inc	edx				;
	in	al,dx				;
	dec	edx				;
	mov	[edi].Reg42HState,al		;save this state
;
	cmp	ChipID,S3911			;running on an S3-911 or 924?
	je	MSRSGet3C2State 		;yep, no need to save these
	mov	al,53h				;save S3 CRTC register 53H
	out	dx,al				;
	inc	edx				;
	in	al,dx				;
	dec	edx				;
	mov	[edi].Reg53HState,al		;save this state
;
	mov	al,58h				;save S3 CRTC register 58H
	out	dx,al				;
	inc	edx				;
	in	al,dx				;
	mov	[edi].Reg58HState,al		;
;
public	MSRSGet3C2State
MSRSGet3C2State:
;
;First, restore the CRTC index register state:
;
	mov	dl,0d4h 			;EDX --> CRTC index register
	rol	eax,8				;restore CRTC index register
	out	dx,al				;
;
;Now, get and save the value of the Misc. Output Regster 3C2H (read at 3CCH):
;
	mov	dl,0cch 			;EDX --> Misc Output Register
	in	al,dx				;
	mov	[edi].Reg3C2State,al		;
;
	pop	edi				;restore saved registers
	pop	edx				;
;
MSRSExit:
	ret
EndProc MiniVDD_SaveRegisters
;
;
public	MiniVDD_RestoreRegisters
BeginProc MiniVDD_RestoreRegisters, DOSVM
;
;This routine is called whenever the "main" VDD is called upon to restore
;the register state.  The "main" VDD will restore all of the states of the
;"normal" VGA registers (CRTC, Sequencer, GCR, Attribute etc.) and will
;then call this routine to allow the mini-VDD to restore register states
;that are hardware dependent.  These registers were saved during the
;routine MiniVDD_SaveRegisters.  Typically, only a few registers
;relating to the memory access mode of the board are necessary to save
;and restore.  Specific registers that control the hardware BLTer probably
;need not be saved.  The register state is saved in the mini-VDD's
;"CB data area" which is a per-VM state saving area.  The mini-VDD's
;CB data area was allocated at Device_Init time.
;
;Entry:
;	ESI contains the VM handle for the MemC owner VM.
;	ECX contains the VM handle for the CRTC owner VM.
;Exit:
;	You must preserve EBX and EDX.	The caller preserves everything else.
;
;A short explanation of the terms "CRTC owner" and "MemC owner" is in order.
;The CRTC owner VM is the VM that owns the screen mode.  If you're running
;on the Windows desktop, then the Windows VM is the CRTC owner.  If you're
;running a full-screen DOS box, then that DOS box's VM is the CRTC owner.
;If you're running a DOS box in a window, then the CRTC owner is Windows
;but the MemC owner is the DOS VM.  What significance does this have?
;Well, when you restore the register state of a DOS VM running in a
;Window, it means that you're getting ready to VIRTUALIZE the VGA by
;using the off-screen memory.  Your VGA hardware must be setup to write
;to this memory in EXACTLY THE SAME WAY AS IF THE VGA WAS RUNNING IN
;NATIVE VGA MODE.  But...  you also must preserve the appearance of the
;Windows screen which is being displayed on the VISIBLE screen.  Thus,
;we have the screen looking like it's running in Windows HiRes packed
;pixel mode from the user's perspective, but the CPU sees the video
;memory as a 4 plane VGA.  Thus, we present this routine with both
;the CRTC owner and the MemC owner's VM handles.  Therefore, you can
;restore those states from the CRTC owner that preserve the appearance
;of the screen while restoring those states from the MemC owner that
;control how the CPU sees the video memory.
;
	push	edx				;save caller's EDX
	cmp	DisplayEnabledFlag,0		;is display in HiRes mode yet?
	je	MRRSExit			;nope, don't do anything yet
	cmp	SuperVGAModeFlag,0		;running S3.DRV?
	je	MRRSExit			;nope, don't do this
;
public	MRRSCheckMessageModeRestore
MRRSCheckMessageModeRestore:
;
;If we're to restore the Message Mode state, we substitute in a
;fixed up value so that our pointer arithmatic will result in us pointing to
;the correct CB data area for each of these states to restore.	Note
;that Message Mode involves putting up a blue screen message so it involves
;both the CRTC and MemC states.  Therefore, we point BOTH the MemC pointer
;and the CRTC pointer at our Message Mode state.
;
	cmp	ecx,MessageModeID		;restoring for Message Mode?
	jne	MRRSCheckForcedPlanarRestore	;nope, go try forced planar
	mov	ecx,OFFSET32 MessageModeCBData	;ECX --> CB data area
	sub	ecx,OurCBDataPointer		;correct for pointer addition
	mov	esi,ecx 			;both MemC and CRTC must be set
	jmp	MRRSFixClock			;go make sure clock is setup
;
public	MRRSCheckForcedPlanarRestore
MRRSCheckForcedPlanarRestore:
;
;If we're to restore the Forced Planar State, we substitute in a
;fixed up value so that our pointer arithmatic will result in us pointing to
;the correct CB data area for each of these states to restore.	The forced
;planar state (unlike Message Mode) is only a MemC state and does not affect
;the CRTC:
;
	cmp	esi,PlanarStateID		;restoring MemC for font load?
	jne	MRRSFixClock			;nope, go setup the clock
	mov	esi,OFFSET32 ForcedPlanarCBData ;ESI --> CB data area
	sub	esi,OurCBDataPointer		;correct for pointer addition
;
public	MRRSFixClock
MRRSFixClock:
;
;We need to make sure that the Misc Output Register state is correct.
;Because S3 chipsets use the Misc Output Register (3C2H) bits 2 & 3 to
;flag the use of their extended clock register 42H.  This messes up on
;a lot of boards because many board manufacturers have BIOS's that don't
;restore either 3C2H or 42H correctly.
;
	cmp	ecx,WindowsVMHandle		;Windows owns the CRTC state?
	jne	MRRSFixClockNonWindows		;nope, skip this
	mov	al,WindowsMiscOutputState	;
	or	al,al				;have we init'd this state yet?
	jz	MRRSDispatch			;nope! don't set a bogus value!
	mov	edx,3c2h			;write the MiscOutput Register
	out	dx,al				;restore the correct state
	jmp	MRRSDispatch			;we're done if Windows owns CRTC
;
public	MRRSFixClockNonWindows
MRRSFixClockNonWindows:
;
;It's possible that the Main VDD has restored an incorrect state for the Misc.
;Output Register.  Since our SAVE_REGISTERS routine has saved the correct
;value for the CRTC owner, we make sure it's restored to the hardware here,
;overruling the value set in the Main VDD:
;
	add	ecx,OurCBDataPointer		;ECX --> Our CB data structure
	mov	edx,3c2h			;EDX --> Misc. Output Register
	mov	al,[ecx].Reg3C2State		;get correct state for register
	out	dx,al				;make sure it's in hardware
;
;Now, see if the CRTC owner is using the external clock (rather than the
;default VGA clock which is described completely in 3C2).  This use of the
;external clock is flagged by bits 2 & 3 being set in register 3C2H:
;
	and	al,0ch				;mask off clock select bits
	cmp	al,0ch				;is he using Register 42H clock?
	jne	@F				;no, clock was set by 3C2 alone
;
;Restore the value of the external clock value which is set in S3 extension
;register 42H:
;
	mov	dl,0d4h 			;EDX --> CRTC index reg
	in	al,dx				;get and save CRTC index
	ror	eax,8				;
	mov	al,42h				;set CRTC index = 42H
	mov	ah,[ecx].Reg42HState		;
	out	dx,ax				;
	rol	eax,8				;restore CRTC index register
	out	dx,al				;
@@:	sub	ecx,OurCBDataPointer		;ECX == CRTC owner's VM handle
;
public	MRRSDispatch
MRRSDispatch:
	cmp	ChipID,S3911			;running on an S3-911?
	je	MRRSExit			;yep, no need to restore these
	cmp	esi,WindowsVMHandle		;need to restore Windows VM?
	jne	MRRSRestoreVMState		;nope, go restore VM's state
;
public	MRRSRestoreWindowsVMState
MRRSRestoreWindowsVMState:
;
;We restore the Windows VM state to registers 40H & 58H.  Since these are
;best tracked by the CurrentChipMode variable in the display driver, we can
;restore them direct from there and not from the saved state.
;
	test	S3ModeChangeFlags,GOING_TO_VGA_MODE OR GOING_TO_WINDOWS_MODE
	jnz	MRRSRestoreVMState		;oops, best restore VM state
	mov	edx,3d4h			;EDX --> CRTC index register
	in	al,dx				;get current CRTC index
	ror	eax,8				;and save it in high byte of EAX
;
	mov	ax,0a539h			;make sure S3 regs are enabled
	out	dx,ax				;
;
	mov	al,40h				;set to CRTC register 40H
	out	dx,al				;
	inc	edx				;EDX --> CRTC data register
	in	al,dx				;get current value of 40H
	mov	edi,CurrentChipModeAddr 	;EDI --> driver CurrentChipMode
	mov	ah,[edi]			;get CurrentChipMode
	and	ax,01feh			;mask the correct bits
.errnz	ENGINE - 1
	or	al,ah				;turn on BLTer mode if set
	out	dx,al				;
	dec	edx				;EDX --> CRTC index register
;
	mov	al,53h				;set to CRTC register 53H
	out	dx,al				;
	inc	edx				;EDX --> CRTC data register
	in	al,dx				;get current value of 53H
	mov	edi,bMMIOActiveAddr		;EDI --> display driver flag
	mov	ah,[edi]			;get bMMIOActiveFlag from
						;display driver
	and	ax,10efh			;mask the correct bits
	or	al,ah				;turn on MMIO if set
	out	dx,al				;
	dec	edx				;EDX --> CRTC index register
;
	mov	al,58h				;set to CRTC register 58H
	out	dx,al				;
	inc	edx				;EDX --> CRTC data register
	in	al,dx				;get current value of 58H
	mov	edi,CurrentChipModeAddr 	;EDI --> driver CurrentChipMode
	mov	ah,[edi]			;get CurrentChipMode
	not	ah				;set to 0 if in engine mode
						;set to FFH if in memory mode
	and	ax,01efh			;mask the correct bits
.errnz	ENGINE - 1
	shl	ah,4				;get ENGINE bit into 10h's bit
	or	al,ah				;turn on memory mode if set
	out	dx,al				;
	dec	edx				;EDX --> CRTC index register
;
	rol	eax,8				;restore CRTC index register
	out	dx,al				;
	jmp	MRRSExit			;and we're done
;
public	MRRSRestoreVMState
MRRSRestoreVMState:
	add	esi,OurCBDataPointer		;now ESI --> MemC's state save
	mov	edx,3d4h			;EDX --> CRTC index register
	in	al,dx				;save current index reg value...
	ror	eax,8				;in high byte of EAX
	mov	ax,0a539h			;make sure S3 regs are enabled
	out	dx,ax				;
;
	mov	al,40h				;restore S3 CRTC register 40H
	out	dx,al				;
	inc	edx				;
	in	al,dx				;get current state from card
	mov	ah,[esi].Reg40HState		;get a few bits from saved MemC
	and	ax,09f6h			;mask to get 1's & 8's bit
	or	al,ah				;
	out	dx,al				;
	dec	edx				;
;
	mov	al,53h				;restore S3 CRTC register 53H
	out	dx,al				;
	inc	edx				;
	in	al,dx				;get current value
	mov	ah,[esi].Reg53HState		;get a few bits from saved MemC
	and	ax,10efh			;mask to get 10's bit from state
	or	al,ah				;
	out	dx,al				;
	dec	edx				;
;
	mov	al,58h				;restore S3 CRTC register 58H
	out	dx,al				;
	inc	edx				;
	in	al,dx				;get current value
	mov	ah,[esi].Reg58HState		;get a few bits from saved MemC
	and	ax,10efh			;mask to get 10's bit
	or	al,ah				;
	out	dx,al				;
	dec	edx				;EDX --> CRTC index register
;
	rol	eax,8				;restore CRTC index register
	out	dx,al				;
;
MRRSExit:
	pop	edx				;
	ret					;
EndProc MiniVDD_RestoreRegisters
;
;
subttl		Setup Hardware to Access Standard VGA Memory Architecture
page +
public	MiniVDD_AccessVGAMemoryMode
BeginProc MiniVDD_AccessVGAMemoryMode, DOSVM
;
;Oft-times, a Super-VGA will want to operate in a flat linear mode, especially
;if it's a VL (local) bus card.  Therefore, its memory aperture will be
;changed from standard VGA access mode (64K at physical A000:0H) to access
;memory at some huge address.  Now, when we're virtualizing VGA in a window
;or in the background, we must change the memory accessing mode of the
;hardware back to standard VGA mode.  This routine will be called at
;the appropriate time in order to accomplish this.
;
;Entry:
;	Nothing assumed.
;Exit:
;	Nothing needs to be preserved.
;
	cmp	ChipID,S3911		;running on an S3-911?
	je	MSVAExit		;yes, no LAW registers on a 911!
	cmp	SuperVGAModeFlag,0	;running S3.DRV?
	je	MSVAExit		;nope, don't do this
	mov	edx,3d4h		;
	in	al,dx			;get current CRTC index register value
	ror	eax,8			;save it in high byte of EAX
	mov	ax,0a539h		;unlock the S3 extension registers
	out	dx,ax			;
;
	mov	ax,0a5ah		;LAW window pos must be set to 0AH
	out	dx,ax			;
	mov	ax,0059h		;
	out	dx,ax			;
;
	mov	al,58h			;
	out	dx,al			;
	inc	edx			;
	in	al,dx			;get current LAW register value
	and	al,NOT 03h		;get rid of aperture size bits
	out	dx,al			;
	dec	edx			;
;
	mov	ax,0039h		;re-lock the extension registers
	out	dx,ax			;
	rol	eax,8			;get back saved CRTC index reg value
	out	dx,al			;
;
MSVAExit:
	ret
EndProc MiniVDD_AccessVGAMemoryMode
;
;
subttl		Setup Hardware to Access VL Bus Memory Architecture
page +
public	MiniVDD_AccessLinearMemoryMode
BeginProc MiniVDD_AccessLinearMemoryMode, DOSVM
;
;See comment at SetupVGAAccess.  This routine does the opposite and sets
;us up to access the video memory at a flat local bus address.
;
;Entry:
;	Nothing assumed.
;Exit:
;	Nothing needs to be saved.
;
	cmp	ChipID,S3911		;running on the S3-911
	je	MSLAExit		;yes! don't set LAW registers on 911!
	cmp	SuperVGAModeFlag,0	;running S3.DRV?
	je	MSLAExit		;nope, don't do this
;
;We don't want to change anything if our Windows HiRes screen is using the
;64K aperture at A000:0H.  Since this is the same architecture as would be
;used by the standard VGA, we don't need to change the access mode at all!
;
	cmp	VFLATDFlag,0		;using banking provided by VFLATD?
	jne	MSLAExit		;you bet! don't bother doing anything!
;
;We're in VL bus mode and not using banking.  Let's setup the state for this
;again:
;
	mov	edx,3d4h		;
	in	al,dx			;get current CRTC index register value
	ror	eax,8			;save it in high byte of EAX
	mov	ax,4838h		;assure S3 extension regs are unlocked
	out	dx,ax			;
	mov	ax,0a539h		;
	out	dx,ax			;
;
	mov	al,31h			;better disable banking
	out	dx,al			;
	inc	edx			;
	in	al,dx			;
	and	al,NOT 01h		;this disables banking
	out	dx,al			;
	dec	edx			;
;
	mov	ax,LAWAddrRegLow	;set these to "local bus state"
	out	dx,ax			;
	mov	ax,LAWAddrRegHigh	;
	out	dx,ax			;
;
	mov	al,58h			;reset aperture size bits in LAW
	out	dx,al			;
	inc	edx			;
	in	al,dx			;
	mov	ah,LAWApertureSizeBits	;
	and	ax,03fch		;mask the bits
	or	al,ah			;
	out	dx,al			;
	dec	edx			;
;
	rol	eax,8			;get back saved CRTC index reg value
	out	dx,al			;
;
MSLAExit:
	ret
EndProc MiniVDD_AccessLinearMemoryMode
;
;
subttl		Enable and Disable BLTer Register Trapping
page +
public	MiniVDD_EnableTraps
BeginProc MiniVDD_EnableTraps, DOSVM
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
	cmp	SuperVGAModeFlag,0	;running S3.DRV?
	je	@F			;nope, don't set BLTer traps
	mov	edx,9ae8h
	VMMCall Enable_Global_Trapping
	mov	edx,9ae9h
	VMMCall Enable_Global_Trapping
	mov	edx,0e2e8h
	VMMCall Enable_Global_Trapping
	mov	edx,0bee8h
	VMMCall Enable_Global_Trapping
	mov	edx,96e8h
	VMMCall Enable_Global_Trapping
	mov	edx,86e8h
	VMMCall Enable_Global_Trapping
	mov	edx,82e8h
	VMMCall Enable_Global_Trapping
	mov	edx,8ee8h
	VMMCall Enable_Global_Trapping
	mov	edx,8ae8h
	VMMCall Enable_Global_Trapping
	mov	edx,0b6e8h
	VMMCall Enable_Global_Trapping
	mov	edx,0bae8h
	VMMCall Enable_Global_Trapping
	mov	edx,0aae8h
	VMMCall Enable_Global_Trapping
	mov	edx,0aee8h
	VMMCall Enable_Global_Trapping
	mov	edx,0a2e8h
	VMMCall Enable_Global_Trapping
	mov	edx,0a6e8h
	VMMCall Enable_Global_Trapping
	mov	edx,92e8h
	VMMCall Enable_Global_Trapping
	mov	edx,0b2e8h
	VMMCall Enable_Global_Trapping
@@:	mov	edx,4ae8h		;make sure this one is still trapped
	VMMCall Enable_Global_Trapping
	mov	edx,4ae9h		;make sure this one is still trapped
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
;See comment at EnableTraps.
;
;Entry:
;	Nothing assumed.
;Exit:
;	Just Preserve EBX and ESI.
;
	cmp	SuperVGAModeFlag,0	;running S3.DRV?
	je	MDRTExit		;nope, don't set BLTer traps
	mov	edx,9ae8h
	VMMCall Disable_Global_Trapping
	mov	edx,9ae9h
	VMMCall Disable_Global_Trapping
	mov	edx,0e2e8h
	VMMCall Disable_Global_Trapping
	mov	edx,0bee8h
	VMMCall Disable_Global_Trapping
	mov	edx,96e8h
	VMMCall Disable_Global_Trapping
	mov	edx,86e8h
	VMMCall Disable_Global_Trapping
	mov	edx,82e8h
	VMMCall Disable_Global_Trapping
	mov	edx,8ee8h
	VMMCall Disable_Global_Trapping
	mov	edx,8ae8h
	VMMCall Disable_Global_Trapping
	mov	edx,0b6e8h
	VMMCall Disable_Global_Trapping
	mov	edx,0bae8h
	VMMCall Disable_Global_Trapping
	mov	edx,0aae8h
	VMMCall Disable_Global_Trapping
	mov	edx,0aee8h
	VMMCall Disable_Global_Trapping
	mov	edx,0a2e8h
	VMMCall Disable_Global_Trapping
	mov	edx,0a6e8h
	VMMCall Disable_Global_Trapping
	mov	edx,92e8h
	VMMCall Disable_Global_Trapping
	mov	edx,0b2e8h
	VMMCall Disable_Global_Trapping
;
MDRTExit:
	ret				;
EndProc MiniVDD_DisableTraps
;
;
subttl		Handler for Device Dependent Trapped Ports
page +
public	MiniVDD_Virtual4AE8H
BeginProc MiniVDD_Virtual4AE8H, DOSVM
;
;This routine is called regardless of whether we're running with VGA.DRV,
;SUPERVGA.DRV, or S3.DRV and applies to all three.
;
;This is one of the few ports which the mini-VDD must specifically trap and
;virtualize.  On your own mini-VDD, you may have to virtualize other ports
;or none at all depending on the functionality of your hardware.  On the
;S3 drawing engine, register 4AE8H has bits set in it depending on whether
;the S3 card is set into "hi-res" mode or standard VGA mode.  The ROM BIOS
;of the S3 card sets register 4AE8H whenever an application calls INT 10H
;to do a mode change.  If we're virtualizing the VGA in a window on the
;hi-res Windows desktop, we certainly don't want the ROM BIOS to change
;the bits in 4AE8H to reflect that we're running in standard VGA mode since
;we're really running in hi-res mode and simply "faking" that we're in
;standard VGA mode.  Therefore, we must trap register 4AE8H and determine
;whether we really want to change it.
;
;Entry:
;	AX (or AL) contains the value to be written to 4AE8H.
;	EBX contains the handle of the VM that is writing to 4AE8H.
;	ECX contains flags telling us what kind of I/O to do (see VMM.INC).
;	DX contains the destination port (4AE8H).
;
	push	edi			;save these just in case
	push	esi			;
	VxDCall VDD_Get_VM_Info 	;get CRTC owner VM handle in EDI
	cmp	edi,WindowsVMHandle	;is CRTC controlled by Windows?
	jne	MV4PerformPhysicalIO	;nope, allow the physical I/O
	cmp	ebx,edi 		;is calling VM handle also in Windows?
	jne	MV4Exit 		;nope, skip the stupid write
;
public	MV4PerformPhysicalIO
MV4PerformPhysicalIO:
	cmp	ecx,Byte_Output 	;does he want to output a byte?
	je	MV4PhysicalByteOutput	;yes, go do it
;
MV4PhysicalWordOutput:
	out	dx,ax			;just write to it
	jmp	MV4Exit 		;and we're done
;
MV4PhysicalByteOutput:
	out	dx,al			;just write the byte to it
;
MV4Exit:
	pop	esi			;restore saved registers
	pop	edi			;
	ret				;
EndProc MiniVDD_Virtual4AE8H
;
;
subttl		Set Hardware to a Not Busy State
page +
public	MiniVDD_MakeHardwareNotBusy
BeginProc MiniVDD_MakeHardwareNotBusy, DOSVM
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
	cmp	SuperVGAModeFlag,0	;running S3.DRV?
	je	MHNBBoardNotBusy	;nope, skip this entire mess!
;
	in	al,dx			;save current CRTC index
	ror	eax,8			;
	mov	ax,4838h		;make sure S3 registers are unlocked
	out	dx,ax			;
	mov	ax,0a539h		;
	out	dx,ax			;
;
	mov	al,40h			;see if Enhanced registers are enabled
	out	dx,al			;
	inc	edx			;
	in	al,dx			;
	dec	edx			;
	rol	eax,8			;restore saved index of CRTC port
	out	dx,al			;
	test	ah,01h			;are Enhanced registers enabled?
	jz	MHNBBoardNotBusy	;nope, best not do the test!
	cmp	ChipID,S3911		;running on an S3-911?
	je	@F			;yes, skip to check on InSrcBLT
	test	ah,08h			;is write posting on?
	jnz	MHNBBoardNotBusy	;yep, best not do the test!
;
	ror	eax,8			;save index of CRTC port again
	mov	al,58h			;see if we're in linear mode
	out	dx,al			;
	inc	edx			;
	in	al,dx			;
	dec	edx			;
	rol	eax,8			;restore saved index of CRTC port
	out	dx,al			;
	test	ah,10h			;is board in linear mode?
	jnz	MHNBBoardNotBusy	;yes, don't do this check!
;
;If the display driver has set the InSrcBLT flag (indicating that the command
;involved waiting for data to come down port E2E8H), we don't want to wait
;for the hardware to become not-busy.
;
@@:	mov	eax,InSrcBLTAddr	;
	cmp	byte ptr [eax],0	;are we in the midst of a E2E8 write?
	jne	MHNBBoardNotBusy	;yes, don't do this!
;
;Now, perform the actual test for board-not-busy:
;
	mov	ecx,1000h		;this is a safety net loop counter
;
MHNBBoardBusyCheck:
	mov	edx,9ae8h		;
	in	ax,dx			;
	test	ah,02h			;is hardware busy?
	jz	MHNBBoardNotBusy	;nope, continue
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
;This process delays for a while so as to let the S3 BLTer finish its work:
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
subttl		Display Driver Is Being Disabled Notification
page +
public	MiniVDD_DisplayDriverDisabling
BeginProc	MiniVDD_DisplayDriverDisabling, RARE
;
;The display driver is in its Disable routine and is about to set the
;hardware back into VGA text mode.  Since this could either mean that
;the Windows session is ending or that some Windows application is switching
;to a VGA mode to display something full screen (such as MediaPlayer), we
;need to disable our MiniVDD_RestoreRegisters code because we're liable
;to restore a Windows HiRes state when we shouldn't!  Thus, clear the
;DisplayEnabledFlag to prevent this:
;
	mov	DisplayEnabledFlag,0		;don't do a RestoreRegState
;
;Now, make sure all traps are disabled on the BLTer registers:
;
	call	MiniVDD_DisableTraps	;
;
;And make sure that trapping of 4AE8H is disabled:
;
	mov	edx,4ae8h		;
	VMMCall Disable_Global_Trapping ;go disable trapping on port in EDX
	mov	edx,4ae9h		;
	VMMCall Disable_Global_Trapping ;go disable trapping on port in EDX
;
MDDDExit:
	ret				;
EndProc MiniVDD_DisplayDriverDisabling
;
;
subttl		Virtualize CRTC Register Extensions
page +
public	MiniVDD_VirtualCRTCIn
BeginProc MiniVDD_VirtualCRTCIn, DOSVM
;
;This virtualization applies whether we're running with VGA.DRV, SUPERVGA.DRV,
;or S3.DRV.
;
;This routine is called when the CRTC index or data register is trapped
;when a VM is being virtualized.  It gives you the opportunity to special
;case virtualization of the CRTC registers for your particular device's needs.
;
;Entry:
;	EBX contains the VM Handle for this CRTC input.
;	ECX contains the CRTC index of the port to virtualize.
;	EDX contains the CRTC data port (3D5H).
;Exit:
;	Set CY to tell caller that we virtualized the port and AL has the value.
;	EBX, ECX, and EDX should be preserved.
;
;The S3 801/805/928 BIOS reads CRTC register 40H to see if the card is
;in Enhanced Mode during mode sets.  We therefore must return the
;physical value from port 40H in this situation.
;
	cmp	cl,30h			;virtualizing S3 registers?
	jae	@F			;yes, maybe something to do
	clc				;nope, tell caller to handle it
	jmp	MVCIExit		;nope, we're done
;
;Physically unlock the S3 extension registers:
;
@@:	dec	dl			;EDX --> CRTC index register
	in	al,dx			;get and save current CRTC index
	mov	ch,al			;save it in CH
	mov	ax,4838h		;unlock registers 30H thru 3FH
	out	dx,ax			;
	mov	ax,0a539h		;unlock registers 40H and up
	out	dx,ax			;
;
;Now, check for the registers that we want to virtualize
;
	cmp	cl,40h			;is it S3 specific register 40H?
	jne	MVCICheckChipIDReg	;nope, go see about register 30H
;
public	MVCIVirtual40H
MVCIVirtual40H:
	mov	al,40h			;set the index 40H
	out	dx,al			;
	inc	dl			;bump to CRTC data register
	in	al,dx			;get the true value
	jmp	MVCIWeHandledIt 	;
;
public	MVCICheckChipIDReg
MVCICheckChipIDReg:
;
;The 928 BIOS tries to be cute and avoid reads and writes to 4AE8H if
;he detects that the screen is being virtualized.  I don't like this and
;want to control my own accesses to it.  Therefore, I virtualize
;his "detection" mechanism which consists of trying to write a value
;to CRTC register 30H and then reading it back.  Since this is a
;read-only register on the S3 chip sets, if he gets back the same thing
;he wrote, he assumes that he's virtualized.  Well, I'll trick him by
;returning the correct value!
;
	cmp	cl,30h			;is he being cute?
	jne	MVCIDidNotHandle	;nope, go let the caller handle it
;
public	MVCIVirtual30H
MVCIVirtual30H:
;
;Get the REAL 30H value from the port:
;
	mov	al,30h			;
	out	dx,al			;
	inc	dl			;
	in	al,dx			;get REAL value
;
MVCIWeHandledIt:
	dec	dl			;EDX --> CRTC index register again
	stc				;flag that we handled the IN
	jmp	MVCIRestoreCRTCIndex	;
;
MVCIDidNotHandle:
	clc				;tell caller to handle it
;
public	MVCIRestoreCRTCIndex
MVCIRestoreCRTCIndex:
	xchg	al,ch			;restore saved CRTC index to AL
					;save any return value that we have
	out	dx,al			;reset the saved CRTC index value
	mov	dl,0d5h 		;EDX --> CRTC data register again
	mov	al,ch			;get back return value to AL
	mov	ch,0			;(ECX must be preserved for return)
					;(don't mess up the flags either)
;
MVCIExit:
	ret
EndProc MiniVDD_VirtualCRTCIn
;
;
public	MiniVDD_VirtualCRTCOut
BeginProc MiniVDD_VirtualCRTCOut, DOSVM
;
;This virtualization applies whether we're running with VGA.DRV, SUPERVGA.DRV,
;or S3.DRV.
;
;This routine is called when the CRTC index or data register is trapped
;when a VM is being virtualized.  It gives you the opportunity to special
;case virtualization of the CRTC registers for your particular device's needs.
;
;Entry:
;	EBX contains the VM Handle for this CRTC output.
;	ECX contains the CRTC index of the port to virtualize.
;	EDX contains the physical register to write or virtualize.
;Exit:
;	Preserve everything (flags included) if we aren't handling the OUT.
;	Set CY to tell caller that we handled the OUT.
;	Save everything (except EDI) that you mess with.
;
;We don't want to handle any registers below 30H:
;
	cmp	cl,30h			;set CY if register is < 30H
	cmc				;return NC if register is < 30H
	jnc	MVCOExit		;let caller handle it (return NC)
;
;We don't want to do any physical OUT's if Message Mode isn't yet init'd:
;
	cmp	MsgModeInitDone,0ffh	;set CY if not done with MsgMode init
					;set NC if done with MsgMode Init
	jc	MVCOExit		;not done with init, return CY to caller
;
;Physically unlock the S3 extension registers:
;
	dec	dl			;EDX --> CRTC index register
	push	eax			;save whatever's in EAX
	in	al,dx			;get and save current CRTC index
	ror	eax,8			;
	mov	ax,4838h		;unlock registers 30H thru 3FH
	out	dx,ax			;
	mov	ax,0a539h		;unlock registers 40H and up
	out	dx,ax			;
	rol	eax,8			;restore saved CRTC index
	out	dx,al			;
	pop	eax			;restore saved EAX
	inc	dl			;EDX --> CRTC data register
;
public	MVCOTryRegister40H
MVCOTryRegister40H:
;
;Since we'll be restoring the states of CRTC registers 40H, 42H, and 58H,
;we'd best collect their states here, at least for initialization of the
;state variables:
;
	cmp	cl,40h			;virtualizing register 40H?
	jne	MVCOTryRegister42H	;nope, continue
	mov	edi,ebx 		;
	add	edi,OurCBDataPointer	;
	mov	[edi].Reg40HState,al	;
	jmp	MVCOLetCallerHandleIt	;and we're done with register 40H
;
public	MVCOTryRegister42H
MVCOTryRegister42H:
	cmp	cl,42h			;virtualizing register 42H?
	jne	MVCOTryRegister53H	;nope, continue
	mov	edi,ebx 		;
	add	edi,OurCBDataPointer	;
	mov	[edi].Reg42HState,al	;
	jmp	MVCOLetCallerHandleIt	;and we're done with register 40H
;
public	MVCOTryRegister53H
MVCOTryRegister53H:
	cmp	cl,53h			;virtualizing register 53H?
	jne	MVCOTryRegister58H	;nope, continue
	mov	edi,ebx 		;
	add	edi,OurCBDataPointer	;
	mov	[edi].Reg53HState,al	;
	jmp	MVCOLetCallerHandleIt	;and we're done with register 40H
;
public	MVCOTryRegister58H
MVCOTryRegister58H:
	cmp	cl,58h			;virtualizing register 58H?
	jne	MVCOLetCallerHandleIt	;nope, we're done
	mov	edi,ebx 		;
	add	edi,OurCBDataPointer	;
	mov	[edi].Reg58HState,al	;
;
public	MVCOLetCallerHandleIt
MVCOLetCallerHandleIt:
	clc				;let caller determine what to do
;
MVCOExit:
	ret				;return to caller
EndProc MiniVDD_VirtualCRTCOut
;
;
subttl		Virtualize I/O To BLTer Ports 9AE8H and 9AE9H
page +
public	MiniVDD_VirtualizeBLTerIO
BeginProc MiniVDD_VirtualizeBLTerIO
;
;Some S3 ROM BIOS's check the busy bits in ports 9AE8H and 9AE9H during their
;mode change routines.	If we're running windowed or we're virtualizing
;S3 register 40H, we may cause the BIOS to erroneously read these ports when
;the physical state of register 40H is not ready for this read.  We therefore
;do some adjustments to the hardware state so that the BIOS won't go into an
;infinite loop while reading these ports.
;
;Entry:
;	EAX contains value to be written to port.
;	EBX contains the VM Handle for which this I/O is being done.
;	ECX contains the I/O type flags (see VMM.INC for details).
;	EDX contains the port number.
;Exit:
;	CY clear means that we didn't handle the I/O (let caller do it).
;		-- All registers MUST be preserved in this case.
;	CY set means that we handled the I/O completely.
;		-- Preserve EBX only in this case.
;	EAX contains result of an IN operation.
;
	cmp	ebx,WindowsVMHandle	;is I/O being done for Windows VM?
	je	MVBINotInterested	;yes! let the caller handle it!
	cmp	edx,9ae8h		;are we interested?
	je	@F			;maybe, go check!
	cmp	edx,9ae9h		;are we interested?
	jne	MVBINotInterested	;
@@:	cmp	ecx,BYTE_INPUT		;doing an IN to 9AE8 or 9AE9?
	je	MVBIByteInput		;yes! go handle it!
	cmp	ecx,WORD_INPUT		;doing an IN to 9AE8?
	jne	MVBINotInterested	;nope, we're not interested!
;
public	MVBIWordInput
MVBIWordInput:
	mov	ecx,0ffffh		;get a safety loop counter
;
public	MVBIWordInputLoop
MVBIWordInputLoop:
	in	ax,dx			;do the physical input
	cmp	ax,-1			;is 40H state wrong?
	je	@F			;yes, definitely, bop out of here!
	test	ah,02h			;is hardware physically busy?
	jz	MVBIWeHandledIt 	;nope, return OK
	call	Delay			;delay for a while
	loop	MVBIWordInputLoop	;and go try again
@@:	and	ah,NOT 02h		;40H is probably wrong, return NOT BUSY
	jmp	MVBIWeHandledIt 	;and pretend that we handled it
;
public	MVBIByteInput
MVBIByteInput:
;
;We're really only interested in reads from port 9AE9H since these are the ones
;that mess us up in BIOS mode changes.
;
	cmp	dl,0e9h 		;reading from port 9AE9H?
	jne	MVBINotInterested	;nope, let Main VDD handle it!
	mov	ecx,0ffffh		;get a safety loop counter
;
public	MVBIByteInputLoop
MVBIByteInputLoop:
	in	al,dx			;do the physical input
	cmp	al,0ffh 		;is 40H state wrong?
	je	@F			;yes, definitely, bop out of here!
	test	al,02h			;is hardware physically busy?
	jz	MVBIWeHandledIt 	;nope, return OK
	call	Delay			;delay for a while
	loop	MVBIByteInputLoop	;and go try again
@@:	and	al,NOT 02h		;40H is probably wrong, return NOT BUSY
;
public	MVBIWeHandledIt
MVBIWeHandledIt:
	stc				;return that we handled it
	jmp	MVBIExit		;and return IN data in AL/AX to caller
;
public	MVBINotInterested
MVBINotInterested:
	clc				;return to let caller handle it!
;
MVBIExit:
	ret				;
EndProc MiniVDD_VirtualizeBLTerIO
;
;
subttl		Collect States Being Sent to Misc Output Register
page +
public	MiniVDD_VirtualMiscOutput
BeginProc MiniVDD_VirtualMiscOutput, DOSVM
;
;We need to intercept and save the values that are being set into the
;Miscallaneous Output Register (port 3C2H).  Since there's no
;VIRTUALIZE_MISC_OUTPUT_OUT MiniVDD hook, we just take it over using
;VDD_Takeover_VGA_Port servier and then pass it onto the original routine
;after saving the value.
;
;Why do we need to save these values?  Because if we're running a full-screen
;VM and we change back and forth between an S3 or VESA HiRes mode and VGA modes,
;we don't get a call to SAVE_REGISTERS.  We therefore need to collect the states
;by virtualize 3C2H.
;
;Entry:
;	AL contains value for 3C2H.
;	EBX contains VM for which output is being done.
;	ECX contains the I/O type (probably always Byte_Output).
;	EDX contains the port (3C2H).
;Exit:
;	Everything (including flags) must be preserved since we're passing on
;	the call back to the default handler.
;
	pushfd				;save the flags
	test	cl,OUTPUT		;are we doing output to port?
	jz	MVMOExit		;nope, don't collect the state
	push	ebx			;save register that we use
	add	ebx,OurCBDataPointer	;EBX --> VM's CB data area
	mov	[ebx].Reg3C2State,al	;record what's being set into 3C2H
	pop	ebx			;restore saved register
;
MVMOExit:
	popfd				;restore saved flags
	jmp	Old3C2Handler		;jump to default handler in Main VDD
EndProc MiniVDD_VirtualMiscOutput
;
;
subttl		Save Message Mode State For S3 Extended Registers
page +
public	MiniVDD_SaveMsgModeState
BeginProc MiniVDD_SaveMsgModeState, RARE
;
;This routine allows the MiniVDD to copy virtualized states and save them in
;a special Message Mode state structure.  When this routine is called, the
;Main VDD has executed a totally virtualized mode change to the blue-screen
;Message Mode state (ie: Mode 3 in SBCS Windows and Mode 12H in Far East
;Windows).  Therefore, no screen changes are evident but the states have
;been saved in the CB data structure for the Windows VM (since the virtualized
;mode change was done when the Windows VM was the "Currently executing VM".
;Thus, when this routine is called, we have the Message Mode state for all
;of the S3 Extension Registers saved in the CB data structure for the
;Windows VM.  We simply copy this state into the special Message Mode state
;data structure.
;
;Entry:
;	EBX --> Windows VM.
;	DS --> Windows VM's BIOS data area.
;Exit:
;	Save everything that you use!
;
;The S3 graphics engine has some complex interactions that prohibit it from
;accurately setting a virtualized mode such as the Message Mode initialization
;tries to do.  Therefore, we must hard-code the register values that are
;restored during MiniVDD_RestoreRegisters to appropriate values for Message
;Mode.	It's important to remember that Message Mode may be either displayed
;in Mode 03H (in SBCS Windows) or Mode 12H (in Far East DBCS Windows).
;
	push	eax			;
	push	edi			;
	mov	edi,OFFSET32 MessageModeCBData
	mov	al,67h			;assume Message Mode is text mode
	cmp	byte ptr ds:[449h],3	;is Message Mode text mode?
	jbe	@F			;yes, AL has value for Misc Output reg
	mov	al,63h			;use Mode 12H value for Misc Output
@@:	mov	[edi].Reg3C2State,al	;save value for Misc Output Reg
	mov	[edi].Reg40HState,00h	;set BLTer off, no write posting
	mov	[edi].Reg53HState,00h	;no Memory Mapped I/O for Msg Mode
	mov	[edi].Reg58HState,00h	;disable linear addressing
	mov	MsgModeInitDone,0ffh	;flag that we're done collecting states
;
SMMSExit:
	pop	edi			;restore saved registers
	pop	eax			;
	ret				;we're done
EndProc MiniVDD_SaveMsgModeState
;
;
subttl		Save Forced Planar State For S3 Extended Registers
page +
public	MiniVDD_SaveForcedPlanarState
BeginProc MiniVDD_SaveForcedPlanarState, RARE
;
;This routine allows the MiniVDD to copy virtualized states and save them in
;a special Forced Planar state structure.  When this routine is called, the
;Main VDD has executed a totally virtualized mode change to the planar state
;(ie: Mode 12H).  Therefore, no screen changes are evident but the states have
;been saved in the CB data structure for the Windows VM (since the virtualized
;mode change was done when the Windows VM was the "Currently executing VM".
;Thus, when this routine is called, we have the planar state for all
;of the S3 Extension Registers saved in the CB data structure for the
;Windows VM.  We simply copy this state into the special Planar State
;data structure.
;
;Entry:
;	Nothing assumed.
;Exit:
;	Save everything that you use!
;
;The source of our state save copy is the Windows VM's CB data structure:
;
	cld				;make sure direction is set forward
	push	edi			;
	mov	edi,OFFSET32 ForcedPlanarCBData
					;EDI --> Forced Planar VM's CB data
	mov	[edi].Reg40HState,00h	;set BLTer off, no write posting
	mov	[edi].Reg53HState,00h	;no Memory Mapped I/O for Msg Mode
	mov	[edi].Reg58HState,00h	;disable linear addressing
;
SFPSExit:
	pop	edi			;
	ret				;we're done
EndProc MiniVDD_SaveForcedPlanarState
;
;
subttl		Return Total Memory Size on Card
page +
public	MiniVDD_GetTotalVRAMSize
BeginProc MiniVDD_GetTotalVRAMSize, DOSVM
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
subttl		Return The Current Write Bank
page +
public	MiniVDD_GetCurrentBank
BeginProc MiniVDD_GetCurrentBank, DOSVM
;
;Entry:
;	EBX contains the VM Handle (Always the "CurrentVM").
;	EBP --> Client Register structure for VM
;Exit:
;	CY is returned upon success.
;	All registers (except EDX & EAX) must be preserved over the call.
;	EDX will contain the current write bank (bank A) set in hardware.
;
	mov	edx,3d4h		;EDX --> CRTC index register
	in	al,dx			;get current index register value
	ror	eax,8			;save it in top byte of EAX
	cmp	NewBankingSupport,0	;running on an 928 or older chipset?
	je	@F			;yes, don't fool with register 6AH
;
	mov	al,6ah			;
	out	dx,al			;
	inc	dl			;EDX --> CRTC data register
	in	al,dx			;
	dec	dl			;EDX --> CRTC index register
	or	al,al			;using new banking support?
	jnz	MGBWGotBank		;yes, the bank is in AL
;
;We're on a 928 or older chipset, or the application may not be using the
;new banking support.  Check registers 35H and 51H for the bank....
;
@@:	xor	ah,ah			;assume we're on the S3-911
	cmp	ChipID,S3911		;running on the S3-911?
	je	@F			;yes, no register 51H on the 911!
;
	mov	al,51h			;get high banking register first
	out	dx,al			;
	inc	dl			;
	in	al,dx			;
	mov	ah,al			;save it in AH
	dec	dl			;
;
@@:	mov	al,35h			;get low banking register
	out	dx,al			;
	inc	dl			;
	in	al,dx			;into AL
;
	and	ax,0c0fh		;mask proper bits from 35H & 51H
	shl	ah,2			;get high bits in proper position
	or	al,ah			;now AL has our bank number
;
public	MGBWGotBank
MGBWGotBank:
;
;The bank is in AL.
;
	mov	dl,0d4h 		;EDX --> CRTC index register
	rol	eax,8			;restore the previous bank
	out	dx,al			;
	movzx	edx,ah			;return bank in EDX register
;
MGBWExit:
	stc				;return success to caller
	ret				;
EndProc MiniVDD_GetCurrentBank
;
;
subttl		Return the Bank Size
page +
public	MiniVDD_GetBankSize
BeginProc MiniVDD_GetBankSize, DOSVM
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
;Our bank size is ALWAYS 64K.
;
	mov	edx,64*1024		;
	xor	eax,eax 		;return VRAM is at A000H
	stc				;return success to caller
	ret				;
EndProc MiniVDD_GetBankSize
;
;
subttl		Prepare For a HiRes Screen Save/Restore
page +
public	MiniVDD_PreHiResSaveRestore
BeginProc MiniVDD_PreHiResSaveRestore, DOSVM
;
;Entry:
;	EBX contains the VM Handle (Always the "CurrentVM").
;	ECX contains the BIOS mode number that we're currently in.
;	EBP --> Client Register structure for VM
;Exit:
;	CY is returned upon success.
;	All registers must be preserved over the call.
;
	push	esi			;
	push	eax			;
	push	edx			;
	mov	esi,ebx 		;make ESI --> VM's CB data area
	add	esi,OurCBDataPointer	;
	mov	edx,3d4h		;EDX --> CRTC index register
	in	al,dx			;save current state of CRTC index reg
	ror	eax,8			;
;
	mov	ax,4838h		;unlock S3 registers 30H to 3FH
	out	dx,ax			;
	mov	ax,0a539h		;unlock S3 registers 40H to whatever
	out	dx,ax			;
;
	mov	al,31h			;save state of bank enable register 31H
	out	dx,al			;
	inc	dl			;
	in	al,dx			;
	mov	[esi].Reg31HState,al	;
	or	al,01h			;set banking ON
	out	dx,al			;and write the new value to the card
	dec	dl			;
;
	mov	al,35h			;save state of low banking register 35H
	out	dx,al			;
	inc	dl			;
	in	al,dx			;
	mov	[esi].Reg35HState,al	;
	dec	dl			;
;
	cmp	ChipID,S3911		;running on a 911 or 924?
	je	MPSRDoneSaving		;yes, skip the rest of the saves
	mov	al,51h			;save state of high banking register 51H
	out	dx,al			;
	inc	dl			;
	in	al,dx			;
	mov	[esi].Reg51HState,al	;
	out	dx,al			;send bank 0 to the hardware
	dec	dl			;
;
	cmp	NewBankingSupport,0	;running on a 928 or older chipset?
	je	@F			;yes, don't fool with register 6AH
	mov	al,6ah			;save state of new banking register 6AH
	out	dx,al			;
	inc	dl			;
	in	al,dx			;
	mov	[esi].Reg6AHState,al	;
	dec	dl			;
;
@@:	mov	al,40h			;save state of BLTer enable register 40H
	out	dx,al			;
	inc	dl			;
	in	al,dx			;
	mov	[esi].Reg40HState,al	;
	and	al,NOT 01h		;turn off BLTer registers
	or	al,08h			;and turn on fast write buffer
	out	dx,al			;and write the new value to the card
	dec	dl			;
;
	mov	al,53h			;save state of MMIO enable register 53H
	out	dx,al			;
	inc	dl			;
	in	al,dx			;
	mov	[esi].Reg53HState,al	;
	and	al,NOT 10h		;turn off MMIO
	out	dx,al			;and write the new value to the card
	dec	dl			;
;
	mov	al,59h			;save state linear addr register 59H
	out	dx,al			;
	inc	dl			;
	in	al,dx			;
	mov	[esi].Reg59HState,al	;
	xor	al,al			;set to 64K banks at A000H
	out	dx,al			;and write the new value to the card
	dec	dl			;
;
	mov	al,5ah			;save state linear addr register 5AH
	out	dx,al			;
	inc	dl			;
	in	al,dx			;
	mov	[esi].Reg5AHState,al	;
	mov	al,0ah			;set to 64K banks at A000H
	out	dx,al			;and write the new value to the card
	dec	dl			;
;
	mov	al,58h			;save state linear addr register 58H
	out	dx,al			;
	inc	dl			;
	in	al,dx			;
	mov	[esi].Reg58HState,al	;
	and	al,NOT 03h		;set to 64K banks at A000H
	or	al,10h			;enable linear addressing
	out	dx,al			;and write the new value to the card
	dec	dl			;
;
public	MPSRDoneSaving
MPSRDoneSaving:
	rol	eax,8			;restore saved CRTC index register
	out	dx,al			;
	pop	edx			;restore saved registers
	pop	eax			;
	pop	esi			;
	stc				;return success to caller
	ret				;
EndProc MiniVDD_PreHiResSaveRestore
;
;
subttl		Clean Up After a HiRes Screen Save/Restore
page +
public	MiniVDD_PostHiResSaveRestore
BeginProc MiniVDD_PostHiResSaveRestore, DOSVM
;
;Entry:
;	EBX contains the VM Handle (Always the "CurrentVM").
;	ECX contains the BIOS mode number that we're currently in.
;	EBP --> Client Register structure for VM
;Exit:
;	CY is returned upon success.
;	All registers must be preserved over the call.
;
	push	esi			;
	push	eax			;
	push	edx			;
	mov	esi,ebx 		;make ESI --> VM's CB data area
	add	esi,OurCBDataPointer	;
	mov	edx,3d4h		;EDX --> CRTC index register
	in	al,dx			;save current state of CRTC index reg
	ror	eax,8			;
;
	mov	ax,4838h		;unlock S3 registers 30H to 3FH
	out	dx,ax			;
	mov	ax,0a539h		;unlock S3 registers 40H to whatever
	out	dx,ax			;
;
	mov	al,31h			;restore state of bank enable reg 31H
	mov	ah,[esi].Reg31HState	;
	out	dx,ax			;
;
	mov	al,35h			;restore state of banking reg 35H
	mov	ah,[esi].Reg35HState	;
	out	dx,ax			;
;
	cmp	ChipID,S3911		;running on a 911 or 924?
	je	MPSRDoneRestoring	;yes, don't restore anything else
;
	mov	al,51h			;restore state of banking reg 51H
	mov	ah,[esi].Reg51HState	;
	out	dx,ax			;
;
	cmp	NewBankingSupport,0	;running on a 928 or older chipset?
	je	@F			;yes, don't fool with register 6AH
	mov	al,6ah			;restore state of banking reg 6AH
	mov	ah,[esi].Reg6AHState	;
	out	dx,ax			;
;
@@:	mov	al,40h			;restore state of BLTer enable reg 40H
	mov	ah,[esi].Reg40HState	;
	out	dx,ax			;
;
	mov	al,53h			;restore state of MMIO enable reg 40H
	mov	ah,[esi].Reg53HState	;
	out	dx,ax			;
;
	mov	al,59h			;
	mov	ah,[esi].Reg59HState	;
	out	dx,ax			;
;
	mov	al,5ah			;
	mov	ah,[esi].Reg5AHState	;
	out	dx,ax			;
;
	mov	al,58h			;
	mov	ah,[esi].Reg58HState	;
	out	dx,ax			;
;
public	MPSRDoneRestoring
MPSRDoneRestoring:
	rol	eax,8			;restore saved CRTC index register
	out	dx,al			;
	pop	edx			;restore saved registers
	pop	eax			;
	pop	esi			;
	stc				;return success to caller
	ret				;
EndProc MiniVDD_PostHiResSaveRestore
;
;
subttl		Set the Bank During a HiRes Save/Restore
page +
public	MiniVDD_SetBank
BeginProc MiniVDD_SetBank, DOSVM
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
;For the S3 chipset, we'd better set the bank into registers 35H/51H and 6AH
;because we don't know which is going to be used by the VESA stuff!
;
	push	edx			;
	push	eax			;
	push	ecx			;
	mov	cl,dl			;get bank value into CL for now
	mov	edx,3d4h		;EDX --> CRTC index register
	in	al,dx			;get and save current CRTC index
	ror	eax,8			;
;
	mov	ax,4838h		;unlock S3 registers 30H to 3FH
	out	dx,ax			;
	mov	ax,0a539h		;unlock S3 registers 40H to whatever
	out	dx,ax			;
;
	mov	al,35h			;
	mov	ah,cl			;
	and	ah,0fh			;mask value for register 35H
	out	dx,ax			;
;
	cmp	ChipID,S3911		;running on a 911 or 924?
	je	MSVDoneSettingBank	;yes, skip the rest of the restoring
	mov	al,51h			;
	out	dx,al			;
	inc	dl			;EDX --> CRTC data register
	in	al,dx			;get current value of register 51H
	and	al,NOT 0ch		;mask out banking bits
	mov	ah,cl			;
	and	ah,30h			;mask value for register 51h
	shr	ah,2			;get the bits into position
	or	al,ah			;now AL has value for register 51h
	out	dx,al			;
	dec	dl			;EDX --> CRTC index register
	cmp	NewBankingSupport,0	;running on a 928 or older chipset?
	je	MSVDoneSettingBank	;yes, don't fool with register 6AH
;
;We don't want to unconditionally set the bank using the new banking support
;because the application might be using only registers 35H/51H.  We therefore
;see if the bank number is small enough to fit in registers 35H/51H.  If it
;is, we set a zero into 6AH thereby using registers 35H/51H.  If the bank
;number is too big to fit in 35H/51H, then the app MUST be using 6AH.
;
	mov	al,6ah			;
	mov	ah,cl			;assume app is using 6AH
	test	cl,0c0h 		;is bank too big for 35H/51H?
	jnz	@F			;yes, use 6AH
	xor	ah,ah			;nope, just use 35H/51H
@@:	out	dx,ax			;
;
public	MSVDoneSettingBank
MSVDoneSettingBank:
	rol	eax,8			;restore CRTC index register
	out	dx,al			;
	pop	ecx			;restore saved registers
	pop	eax			;
	pop	edx			;
	stc				;return success to caller
	ret				;
EndProc MiniVDD_SetBank
;
;
subttl		Do Special Pre-VESA Restore Code on S3-911/924
page +
public	MiniVDD_SetHiResMode
BeginProc MiniVDD_SetHiResMode, DOSVM
;
;Entry:
;	EAX contains the HiRes mode number to set.
;	EBX contains the VM handle trying to set the mode (always Current VM).
;	EBP --> VM's Client Registers.
;Exit:
;	Save anything that you use.
;	CY returned if MiniVDD did the mode change.
;	NC returned if VDD is to do a VESA HiRes mode set.
;
;The S3-911's ROM BIOS has a quirk in it in that when setting a VESA mode such
;as 100h, 101h, 102h etc. directly from an S3 proprietary HiRes mode  (such as
;205h, 201h etc.), register 40H is not properly setup by the BIOS leading to
;screen distortions etc.  This routine, strange as it may seem, is called
;by the Main VDD's RestoreHiResScreen routine before doing a VESA mode set.
;For the 911/924, we set the mode to BIOS mode 3 so that we avoid this ROM
;BIOS bug.
;
	push	eax				;save everything that we use
	mov	eax,86h 			;set to mode 3, no screen erase
	push	10h				;
	VMMCall Exec_VxD_Int			;
	pop	eax				;
	clc					;allow Main VDD to do mode set
	ret					;
;
EndProc MiniVDD_SetHiResMode
;
;
subttl		Special VESA Call 4F01H Post Processing
page +
public	MiniVDD_VESACallPostProcessing
BeginProc MiniVDD_VESACallPostProcessing
;
;Entry:
;	EDX (low word) contains the VESA function code that was just done.
;	EDX (high word) contains the VESA mode number IF AND ONLY IF
;	      EDX (low word) contains 4F02H.
;	VM's Client registers contain the VESA return values.
;	EBX contains the VM in which this VESA call was made.
;	EBP --> VM's Client Registers.
;Exit:
;	All registers that are used MUST be preserved.
;	Nothing returned to caller.
;
	cmp	dx,4f00h		;was it a function 4F00H?
	je	MVPPPost4F00H		;yes go handle it
	cmp	dx,4f02h		;was it a VESA mode set?
	jne	MVPPExit		;nope, we don't want to do this
;
public	MVPPPost4F02H
MVPPPost4F02H:
;
;If this didn't happen in the Windows VM, we need to save the S3 extended
;registers:
;
	cmp	ebx,WindowsVMHandle	;did this happen in the Windows VM?
	je	MVPPExit		;yes, display driver will order save
	call	MiniVDD_SaveRegisters	;nope, go snapshot the S3 registers
	jmp	MVPPExit		;
;
public	MVPPPost4F00H
MVPPPost4F00H:
;
;The S3 VESA BIOS sometimes locks the extended registers.  We always want these
;unlocked after a VESA BIOS call.  So, we post-process the VESA BIOS call to
;assure that they're unlocked.
;
	cmp	ebx,WindowsVMHandle	;did this 4F00H happen in Windows VM?
	jne	MVPPExit		;nope, we don't want to do this
	push	eax			;save all that we use
	push	edx			;
	mov	edx,3d4h		;EDX --> CRTC index register
	in	al,dx			;get and save current index
	ror	eax,8			;
	mov	ax,4838h		;unlock 'em
	out	dx,ax			;
	mov	ax,0a539h		;
	out	dx,ax			;
	rol	eax,8			;now restore previous CRTC index
	out	dx,al			;
	pop	edx			;restore saved registers
	pop	eax			;
;
MVPPExit:
	ret				;done with post-processing
EndProc MiniVDD_VESACallPostProcessing
;
;
VxD_LOCKED_CODE_ENDS
;
;
end

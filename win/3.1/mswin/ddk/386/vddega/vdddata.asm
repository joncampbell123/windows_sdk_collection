;******************************************************************************
;
;VDDDATA - Virtual Display Device Data
;
;   Author: MDW
;
;   (C) Copyright MICROSOFT Corp. 1986-1991
;
;   February, 1988
;
;DESCRIPTION:
;	Contains data local to VDD, global to various VDD modules.
;
;******************************************************************************

	.386p

	INCLUDE VMM.Inc
	INCLUDE ega.inc

VxD_DATA_SEG

;******************************************************************************
; Global VDD data
;

PUBLIC	Vid_VM_Handle
PUBLIC	Vid_VM_Handle2
PUBLIC	Vid_VM_HandleRun
PUBLIC	Vid_Flags

Vid_VM_Handle	    dd	?	; Handle of VM attached to 1st EGA(&CRT)
Vid_VM_Handle2	    dd	?	; Handle of VM attached to 2nd EGA
Vid_VM_HandleRun    dd	?	; Handle of VM running on EGA
Vid_Flags	    dd	?	; global flags


;******************************************************************************
; Control block offset for VDD
;
	ALIGN 4h
PUBLIC	VDD_CB_Off
VDD_CB_Off  DD	?

;******************************************************************************
;
; Time-out frequency for window updates
;
PUBLIC	VDD_Win_Update_Time
VDD_Win_Update_Time DD	?


;******************************************************************************
;
; VDD_Initial_Text_Rows - INI variable value
;
PUBLIC  VDD_Initial_Text_Rows
VDD_Initial_Text_Rows DB ?

;******************************************************************************
; A instance data offset of interest
;
;	 ALIGN 4h
;PUBLIC  VDDSzInstOff
;VDDSzInstOff	 DD  ?			     ; Offset in instance buf to VRAM sz
;
;******************************************************************************
; Display dependent routine vectors - EGA default
;
;	 ALIGN 4h
;PUBLIC  VDD_ProcTab
;VDD_ProcTab	 DD  OFFSET32 EGA_Procs      ; Address of vector table
;
; Vector table for EGA
;	 EXTRN	 VDD_EGA_RState:NEAR
;	 EXTRN	 VDD_EGA_PreSRAM:NEAR
;	 EXTRN	 VDD_EGA_RestIndx:NEAR
;	 EXTRN	 VDD_EGA_RestCtlr:NEAR
;	 EXTRN	 VDD_EGA_RestCtl2:NEAR
;;;	EXTRN	VDD_EGA_Unmapped:NEAR

;	 ALIGN 4h
;PUBLIC  EGA_Procs
;
;EGA_Procs   label   DWORD
;DefProc EGA_Procs,VP_RState,VDD_EGA_RState
;DefProc EGA_Procs,VP_PreSRAM,VDD_EGA_PreSRAM
;DefProc EGA_Procs,VP_RestIndx,VDD_EGA_RestIndx
;DefProc EGA_Procs,VP_RestCtlr,VDD_EGA_RestCtlr
;DefProc EGA_Procs,VP_RestCtl2,VDD_EGA_RestCtl2

IF2
%OUT	This replaced by INT 2F stuff
ENDIF
IF  0
;******************************************************************************
;Initial state of EGA controller except CRTC and Misc regs.(Mode 10+) for VM1
;
PUBLIC	EGA_Vid_Stt_VM1
;Attribute controller
EGA_Vid_Stt_VM1 DB	0,03Ch,03Ah,03Eh,039h,03Dh,03Bh,03Fh
		DB	0,03Ch,03Ah,03Eh,039h,03Dh,03Bh,03Fh,1,0,0Fh,0
		DB	AL_Resvd DUP (?)	; unused fields
;Sequencer
		DB	3,1,0Fh,0,6
		DB	SL_Resvd DUP (?)	; unused fields
;Graphics controller
		DB	0Fh,0,0,0,2,0,5,07h,0FFh
		DB	GL_Resvd DUP (?)	; unused fields
;Index registers
		DB	31h,2,18h,8
;External output registers
		DB	0,1,0
;Status input registers
		DB	fStt0VRTC
		DB	fStatEna+fStatDots
		DB	VL_Resvd DUP (?); padding
.ERRNZ	($-EGA_Vid_Stt_VM1)-((SIZE EGA_State_Struc)-(SIZE CRTC_struc)-1)
ENDIF

VxD_DATA_ENDS

END


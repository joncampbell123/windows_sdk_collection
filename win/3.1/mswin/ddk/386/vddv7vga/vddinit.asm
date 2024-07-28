       TITLE   VDD - Virtual Display Device for EGA   version 3.00  1/89
;******************************************************************************
;
;VDDINIT - Virtual Display Device System Initialization
;
;   Author: MDW
;
;   (C) Copyright MICROSOFT Corp. 1986, 1987, 1988, 1989
;
;   January, 1989
;
;DESCRIPTION:
;	This module initializes VDD internal data structures
;
;COMPILE:
;..\..\tools\masm5 -t -Mx -DDEBUG -DEGA -Ddevtest -I..\..\INCLUDE -p vddinit.asm,,,;
;
;******************************************************************************

	.386p

	INCLUDE vmm.inc
	INCLUDE ega.inc
	INCLUDE vdd.inc
	INCLUDE vdd2.inc
	INCLUDE debug.inc

; Because "IFDEF V7VGA OR TLVGA" doesn't work
IFDEF	V7VGA
ReserveRomC6C7=-1
ENDIF
IFDEF	TLVGA
ReserveRomC6C7=-1
ENDIF
IFDEF	PVGA
ReserveRomC6C7=-1
ENDIF

;******************************************************************************
; EXTRN routines
;
VxD_CODE_SEG
	EXTRN	VDD_VMSetFocus:NEAR
	EXTRN	VDD_VMCreate:NEAR
IFDEF	ReserveRomC6C7
	EXTRN	VDD_VM_Critical_Init:NEAR
ENDIF
	EXTRN	@VDD_VMIdle:NEAR
;;;	EXTRN	@VDD_Set_Time_Slice:NEAR
	EXTRN	VDD_PFault:NEAR
	EXTRN	VDD_Int_10:NEAR
	EXTRN	VDD_Int_2F:NEAR
	EXTRN	VDD_PM_Int_2F:NEAR
	EXTRN	VDD_I10_Mode_Switch:NEAR
	EXTRN	VDD_I10_Mode_Sw_Done:NEAR
VxD_CODE_ENDS

;******************************************************************************
; EXTRN data
;
VxD_DATA_SEG
	Extern_vgVDD
	EXTRN	VDD_I10_OldVector:DWORD
	EXTRN	VDD_Stub_I10_Vector:DWORD
	EXTRN	VDD_I2F_Next_CS:DWORD
	EXTRN	VDD_I2F_Next_EIP:DWORD
	EXTRN	VDD_CB_Off:DWORD
	EXTRN	VDD_PhysA0000:DWORD
	EXTRN	VDD_2EGA_Start:DWORD
	EXTRN	VDD_Phys2EGA:DWORD
	EXTRN	VDD_Latch_Addr:DWORD
IFDEF	EGA
	EXTRN	VDD_Sys_VRAMAddr:DWORD
ENDIF
	EXTRN	VDD_VMIdle_Chain:DWORD
	EXTRN	VDD_SetTime_Chain:DWORD
	EXTRN	VDD_Rest_Event_Handle:DWORD
	EXTRN	VDD_Rest2_Event_Handle:DWORD
	EXTRN	VDD_Focus_VM:DWORD
	EXTRN	VDD_Win_Update_Time:DWORD
	EXTRN	VDD_Initial_Text_Rows:BYTE
	EXTRN	VDD_Msg_Pseudo_VM:DWORD
VxD_DATA_ENDS

VxD_IDATA_SEG
Inst_1 InstDataStruc <,,EGA_I_Addr1,EGA_I_Len1,ALWAYS_Field> ; Video stuff
Inst_2 InstDataStruc <,,EGA_I_Addr2,EGA_I_Len2,ALWAYS_Field> ; EGA variables
Inst_3 InstDataStruc <,,EGA_I_Addr3,EGA_I_Len3,ALWAYS_Field> ; Video parms ptr
Inst_4 InstDataStruc <,,EGA_I_Addr4,EGA_I_Len4,ALWAYS_Field>

VMPagesBuf	dd  9 dup (?)			       ; Buf for reserving pgs

	EXTRN	VDD_Str_BadDevice:BYTE
	EXTRN	VDD_Str_CheckVidPgs:BYTE
	EXTRN	VDD_Time_Ini:BYTE
        EXTRN   VDD_Text_Rows_Ini:BYTE
	EXTRN	VDD_Text_Rows_Sect:BYTE
	EXTRN	VDD_2nd_Ini:BYTE

VxD_IDATA_ENDS

VxD_ICODE_SEG
	EXTRN	VDD_IO_Init:NEAR

;******************************************************************************
;
;   NAME:
;	VDD_Sys_Critical_Init
;
;   DESCRIPTION:
;
;
;   ENTRY:
;	EBX = Sys VM handle
;	EDX = reference data from real mode INIT, initial flags value
;
;   EXIT:
;       CF set if error
;
;   USES:
;	Flags, EAX, ECX, EDX, ESI, EDI
;
;------------------------------------------------------------------------------

BeginProc VDD_Sys_Critical_Init,PUBLIC

;*******
;Initialize global data structures
;
	xor	eax,eax
	mov	[Vid_VM_Handle2],eax	    ; Nothing attached 2nd EGA
	mov	[VDD_Rest_Event_Handle],eax
	mov	[VDD_Rest2_Event_Handle],eax

	mov	[VDD_Focus_VM],ebx	    ; System VM has focus and is
	mov	[Vid_VM_Handle],ebx	    ;	running on hardware
	mov	[Vid_VM_HandleRun],ebx

; IF DX = initial value of flags
	mov	[Vid_Flags],edx 		; Init flags

;*******
;Allocate part of VM control block for VDD usage
;
	VMMCall _Allocate_Device_CB_Area, <<SIZE CB_EGA_Struc>, 0>
	test	eax, eax
	jnz	SHORT VDD_SI_Got_CB_Area
Debug_Out "VDD ERROR:  Could not allocate control block area"
	jmp	VDD_SI_ErrBad
VDD_SI_Got_CB_Area:
	mov	[VDD_CB_Off], eax

	sub	[VDD_Msg_Pseudo_VM], eax	; The pseudo VM handle must
						; adjusted to have the same
						; offset to the start of the
						; VDD info
	mov	edi,ebx
	add	edi,eax

;*******
; Set up physical addresses for the video memory
;
	VMMCall _MapPhysToLinear,<0A0000h,20000h,0>
	inc	eax
	jnz	SHORT VDD_SI_GotPhysAddr
Debug_Out "VDD: Cannot address video memory(A0000h-BFFFFh)"
	jmp	VDD_SI_ErrPgs
VDD_SI_GotPhysAddr:
	dec	eax
	mov	[VDD_PhysA0000],eax
	push	eax
IFDEF	VGA
	mov	[VDD_2EGA_Start],0		; Init'd when 2nd VM created
ELSE
IFDEF	EGA
	mov	edx,0A8000h
	mov	[VDD_2EGA_Start],edx
	sub	edx,0A0000h
	add	eax,edx
	mov	[VDD_Phys2EGA],eax
	dec	eax
	and	al,0F0h
	mov	[VDD_Sys_VRAMAddr],eax
ELSE
Need to define VDD_2EGA meaning
ENDIF	; EGA
ENDIF	; VGA
	pop	eax
	add	eax,0AFFFFh-0A0000h
	mov	[VDD_Latch_Addr],eax

;*******
; Reserve and hook pages A0-AF and B8-BF
;   Also reserve and hook B0-B7 for secondary display if
;	1) Secondary VDD does not exist (it will reserve it) AND
;	2) DUALDISPLAY=YES
;	    OR Secondary display is attached
;   Also reserve and hook B0-B7 and set fVid_Mono if
;	1) VGAMONO is supported AND
;	2) DUALDISPLAY = NO
;	    OR Secondary display is not attached
;
	VxDCall VDD2_Get_Version		; Q: 2nd VDD exist?
	jnc	VDD_SI_B0_Done			;   Y: Don't reserve B0-B7

	xor	esi,esi 			; [WIN386] section
	mov	edi, OFFSET32 VDD_2nd_Ini	; Find this string
	VMMcall Get_Profile_Boolean		; Q: DUALDISPLAY Value spec'd?
	jc	SHORT VDD_SI_B0_Check		;   N: Check for display
	jz	SHORT VDD_SI_B0_Check		;   N: Check for display

	or	eax,eax 			;   Y: Q: DUALDISPLAY=YES?
	jnz	SHORT VDD_SI_B0_Resv		;	Y: Reserve memory
IFDEF	VGAMONO
; Support MONO mapping as well
	or	[Vid_Flags],fVid_Mono		;	N: VDD handles MONO mem
	jmp	SHORT VDD_SI_B0_Resv		;	   Reserve memory
ELSE
	jmp	SHORT VDD_SI_B0_Done		;	N: Leave memory alone
ENDIF

; No DUALDISPLAY .INI switch, check for memory at B0000 or VDD2 existence
VDD_SI_B0_Check:
	mov	esi,[VDD_PhysA0000]
	add	esi,0B0000h-0A0000h
	mov	ax,[esi]
	push	eax
	xor	eax,-1
	mov	[esi],ax
	cmp	ax,[esi]			; Q: Memory at B0000h?
	pop	eax
	mov	[esi],ax
IFDEF	VGAMONO
	jz	SHORT VDD_SI_B0_Resv		;   Y: Reserve memory
	or	[Vid_Flags],fVid_Mono		;	N: VDD handles MONO mem
ELSE
	jnz	SHORT VDD_SI_B0_Done		;   N: Don't reserve
ENDIF

VDD_SI_B0_Resv:
IFDEF	DEBUG
	xor	eax,eax
	VMMCall _Get_Device_V86_Pages_Array,<eax,<OFFSET32 VMPagesBuf>,eax>
	mov	eax,[VMPagesBuf+((0A0h/32)*4)]
	test	eax,000FF0000h			;   N: Q: B0-B7 already resvd?
	jz	SHORT VDD_SI_B0_DBG0		;	Y: Forget it (weird)
Trace_Out "VDD: Mono pages already reserved: #EAX"
VDD_SI_B0_DBG0:
ENDIF
	xor	eax,eax 			;	N: Reserve the pages
	VMMCall _Assign_Device_V86_Pages,<0B0h,8,eax,eax>
IFDEF	DEBUG
	or	eax,eax 			; Q: Successful?
	jnz	SHORT VDD_SI_B0_DBG1		;   Y: continue
Trace_Out "VDD: Mono pages already allocated: #EAX"
VDD_SI_B0_DBG1:
ENDIF

VDD_SI_B0_Done:
	xor	eax,eax
	VMMCall _Get_Device_V86_Pages_Array,<eax,<OFFSET32 VMPagesBuf>,eax>
	mov	eax,[VMPagesBuf+((0A0h/32)*4)]
	test	eax,0FF00FFFFh
	jz	SHORT VDD_SI_PagesFree
Debug_Out "VDD ERROR:  Video pages already allocated: #EAX"
	jmp	VDD_SI_ErrPgs
VDD_SI_PagesFree:
	xor	eax,eax
	VMMCall _Assign_Device_V86_Pages,<0A0h,16,eax,eax>
	or	eax,eax
	jnz	SHORT VDD_SI_GotGrap
Debug_Out "VDD ERROR:  Could not allocate A0-AF video pages"
IFDEF EGA
	jmp	SHORT VDD_SI_ErrPgs
ELSE
	jmp	VDD_SI_ErrPgs
ENDIF        
VDD_SI_GotGrap:
	xor	eax,eax
	VMMCall _Assign_Device_V86_Pages,<0B8h,8,eax,eax>
	or	eax,eax
	jnz	SHORT VDD_SI_GotText
Debug_Out "VDD ERROR:  Could not allocate B8-BF video pages"
IFDEF	ReserveRomC6C7
	jmp	VDD_SI_ErrPgs
ELSE
	jmp	SHORT VDD_SI_ErrPgs
ENDIF
VDD_SI_GotText:
	mov	esi,OFFSET32 VDD_PFault
	mov	eax,0A0h
	mov	ecx,16
VDD_SI_NxtPgGrap:
	VMMCall Hook_V86_Page
	inc	eax
	loopd	VDD_SI_NxtPgGrap

	mov	eax,0B8h
	mov	ecx,8
VDD_SI_NxtPgText:
	VMMCall Hook_V86_Page
	inc	eax
	loopd	VDD_SI_NxtPgText

IFDEF	VGAMONO
	test	[Vid_Flags],fVid_Mono		; Q: Mono supported?
	jz	SHORT VDD_SI_NotMono		;   N: Don't hook pages
	mov	eax,0B0h			;   Y: Hook B0-B7
	mov	ecx,8
VDD_SI_NxtPgMono:
	VMMCall Hook_V86_Page
	inc	eax
	loopd	VDD_SI_NxtPgMono

VDD_SI_NotMono:
ENDIF

IFDEF	ReserveRomC6C7
; Both Tseng Labs and Video 7 have ROM without signature at C6, C7 pages
;
; Reserve additional address space for ROM without ROM ID for Tseng Labs
	test	[Vid_Flags],fVid_TLVGA+fVid_V7VGA+fVid_PVGARom
	jz	SHORT VDD_SI_Not_TL_V7
Trace_Out "Reserve and Map the ROM at C6, C7"
	mov	eax,[VMPagesBuf+((0C0h/32)*4)]
	test	al,0C0h
	jz	SHORT VDD_SI_RomFree
;       Don't want to do an error condition here in case we're running
;       a memory manager like 386Max which shadows the ROM.
	jmp	SHORT VDD_SI_GotRom	; Pretend we got it even though we didn't
;Debug_Out "VDD ERROR:  C6, C7 ROM pages already allocated: #EAX"
;	 jmp	 SHORT VDD_SI_ErrPgs
VDD_SI_RomFree:
%OUT	Tseng Labs: Check for additional ROM mapped into address space?
	xor	eax,eax
	VMMCall _Assign_Device_V86_Pages,<0C6h,2,eax,eax> ; Reserve the address
	or	eax,eax
	jnz	SHORT VDD_SI_GotRom
Debug_Out "VDD ERROR:  Could not allocate C6-C7 ROM pages"
	jmp	SHORT VDD_SI_ErrPgs
VDD_SI_GotRom:

	call	VDD_VM_Critical_Init		    ; Map the ROM in system VM
VDD_SI_Not_TL_V7:
ENDIF
	clc
	ret

VDD_SI_ErrPgs:
	Fatal_Error <OFFSET32 VDD_Str_CheckVidPgs>
	stc
	ret

VDD_SI_ErrBad:
	Fatal_Error <OFFSET32 VDD_Str_BadDevice>
	stc
	ret
EndProc VDD_Sys_Critical_Init


;******************************************************************************
;
;   VDD_VM_Int_10_Stub
;
;   DESCRIPTION:    Stub of code which is copied down into V86 memory and is
;		    hooked into the INT 10 chain so that VDD can be notfied
;		    of mode change INT 10's
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================
VDD_VM_Int_10_Stub PROC NEAR	;don't use BeginProc/EndProc to avoid any
				;preamble stuff!
	or	ah,ah			    ; Q: Mode setting?
	jz	SHORT VIS_mode		    ;	Y: notify VDD and reflect
	cmp	ah,11h			    ; Q: Mode setting(via char set chg)?
	jnz	SHORT VIS_reflect	    ;	N: reflect
	cmp	al,30h			    ; Q: Get font info?
	jz	SHORT VIS_reflect	    ;	Y: reflect
VIS_mode:
VIS_start_bp equ $ - VDD_VM_Int_10_Stub
	nop				    ; a V86 brkpt will be installed here

	pushfd				    ; in 16 bit segment this is a pushf
	;call old handler
	db	9Ah
VIS_call equ $ - VDD_VM_Int_10_Stub
	dd	?

VIS_end_bp equ $ - VDD_VM_Int_10_Stub
	nop				    ; a V86 brkpt will be installed here
	iretd				    ; in 16 bit segment this is an iret
VIS_reflect:

	;jmp to old handler
	db	0EAh
VIS_jmp equ $ - VDD_VM_Int_10_Stub
	dd	?

VIS_stub_size equ $ - VDD_VM_Int_10_Stub

VDD_VM_Int_10_Stub ENDP

;******************************************************************************
;
;   VDD_Device_Init
;
;   DESCRIPTION:
;	Specify instance data, do any other initialization necessary
;
;   ENTRY:
;	EBX = Sys VM's handle
;
;   EXIT:
;	Carry clear if no error
;
;   USES:
;	Nothing
;
;   ASSUMES:
;	Interrupts have been (and still are) disabled.
;
;==============================================================================

BeginProc VDD_Device_Init,PUBLIC

	pushad
	mov	edi,ebx
	add	edi,[VDD_CB_Off]

IFDEF	DEBUG
IFDEF	TLVGA
	test	[Vid_Flags],fVid_TLVGA
	jz	SHORT VDI_Not_TL
Trace_Out "Tseng Labs VGA support"
VDI_Not_TL:
ENDIF
IFDEF	V7VGA
	test	[Vid_Flags],fVid_V7VGA
	jz	SHORT VDI_Not_V7
Trace_Out "Video 7 VGA support"
VDI_Not_V7:
ENDIF
IFDEF	PVGA
	test	[Vid_Flags],fVid_PVGA
	jz	SHORT VDI_Not_PVGA
Trace_Out "Paradise VGA support"
VDI_Not_PVGA:
	test	[Vid_Flags],fVid_PVGARom
	jz	SHORT VDI_Not_PVGAROM
Trace_Out "Paradise VGA ROM, C6, C7 page excluded"
VDI_Not_PVGARom:
ENDIF
IFDEF	ATIEGA
	test	[Vid_Flags],fVid_ATI800
	jz	SHORT VDI_Not_ATI800
Trace_Out "ATI EGA 800 support"
VDI_Not_ATI800:
	test	[Vid_Flags],fVid_ATIWon
	jz	SHORT VDI_Not_ATIWon
Trace_Out "ATI EGA Wonder support"
VDI_Not_ATIWon:
ENDIF
ENDIF

;*******
;Copy INT 10 stub down into V86 memory and save old Int vector
;
	mov	eax, 10h
	VMMCall Get_V86_Int_Vector
	shl	ecx, 16
	mov	cx, dx
	mov	[VDD_I10_OldVector], ecx

	push	ecx
	VMMCall _Allocate_Global_V86_Data_Area, <VIS_stub_size, 0>
	pop	edx
	or	eax, eax
	jnz	SHORT alloc_ok
Debug_Out "ERROR: Could not allocate global VM data area for VDD int 10 hook"
	VMMCall Fatal_Memory_Error
alloc_ok:
	mov	edi, eax
	mov	esi, OFFSET32 VDD_VM_Int_10_Stub
	mov	ecx, VIS_stub_size
	cld
	rep	movsb
	mov	edi, eax
	mov	[edi+VIS_call], edx	    ; patch CALL to point to old handler
	mov	[edi+VIS_jmp], edx	    ; patch JMP to point to old handler
	shr	eax, 4
	movzx	ecx, ax
	mov	edx, edi
	and	edx, 0Fh
	mov	eax, 10h
	VMMCall Set_V86_Int_Vector
	mov	eax, ecx
	shl	eax, 16
	mov	ax, dx
	mov	[VDD_Stub_I10_Vector], eax  ; save our new vector setting
	add	ax, VIS_start_bp
	xor	edx, edx
	mov	esi, OFFSET32 VDD_I10_Mode_Switch
	VMMCall Install_V86_Break_Point
	add	ax, VIS_end_bp - VIS_start_bp
	mov	esi, OFFSET32 VDD_I10_Mode_Sw_Done
	VMMCall Install_V86_Break_Point

;*******
;Initialize I/O trapping
;
	call	VDD_IO_Init		    ; Initialize I/O port trapping
	jc	VDD_DI_Err

;*******
;Initialize interrupt trapping
;
	mov	esi,OFFSET32 VDD_Int_10     ; ESI = address of handler
	mov	eax,10h
	VMMCall Hook_V86_Int_Chain	    ; Hook Int 10h


	mov	esi,OFFSET32 VDD_Int_2F     ; ESI = address of handler
	mov	eax,2Fh
	VMMCall Hook_V86_Int_Chain	    ; Hook V86 Int 2Fh

	mov	eax, 2Fh
	VMMcall Get_PM_Int_Vector
	mov	[VDD_I2F_Next_CS], ecx
	mov	[VDD_I2F_Next_EIP], edx

	mov	esi, OFFSET32 VDD_PM_Int_2F
	VMMcall Allocate_PM_Call_Back

	movzx	edx, ax
	mov	ecx, eax
	shr	ecx, 16
	mov	eax, 2Fh
	VMMcall Set_PM_Int_Vector


; Hook Release Time slice so we can do Window updates when app is idle
	mov	esi,OFFSET32 @VDD_VMIdle
	mov	eax,Release_Time_Slice
	VMMCall Hook_Device_Service
	mov	[VDD_VMIdle_Chain],esi

; Hook Set_Time_Slice so we can refuse setting misbehaved apps to background
;;;	mov	esi,OFFSET32 @VDD_Set_Time_Slice
;;;	mov	eax,Set_Time_Slice_Priority
;;;	VMMCall Hook_Device_Service
;;;	mov	[VDD_SetTime_Chain],esi

;*******
;Specify instanced RAM
;
	VMMCall _AddInstanceItem, <<OFFSET32 Inst_1>, 0>
	jz	VDD_DI_Err
	VMMCall _AddInstanceItem, <<OFFSET32 Inst_2>, 0>
	jz	VDD_DI_Err
	VMMCall _AddInstanceItem, <<OFFSET32 Inst_3>, 0>
	jz	VDD_DI_Err
	VMMCall _AddInstanceItem, <<OFFSET32 Inst_4>, 0>
IFDEF	CTVGA
	jz	VDD_DI_Err
ELSE
	jz	SHORT VDD_DI_Err
ENDIF

;
; Get the Win.Ini entry for window update time
;
	mov	eax, VDD_Def_Update_Time    ; EAX = Default update time
	mov	ecx, 3			    ; 3 digits after decimal point
	xor	esi, esi		    ; Use [Win386] section
	mov	edi, OFFSET32 VDD_Time_Ini  ; Find this string
	VMMcall Get_Profile_Fixed_Point     ; EAX = User specified value
	test	eax, eax		    ; Q: Did bozo say "0"?
	jz	SHORT VDD_DI_Use_Default    ;	 Y: Ignore it
	cmp	eax, 10000		    ;	 N: Q: Longer than 10 sec?
	jb	SHORT VDD_DI_Set_Time	    ;	       N: Use this one
VDD_DI_Use_Default:			    ;	       Y: TOO LONG!
	mov	eax, VDD_Def_Update_Time    ; Use the default update time
VDD_DI_Set_Time:
	mov	[VDD_Win_Update_Time], eax  ; Save for time-out routine

	mov	edi, OFFSET32 VDD_Text_Rows_Ini	; Find this string
	mov	esi, OFFSET32 VDD_Text_Rows_Sect ; in this section
	VMMcall Get_Profile_Decimal_Int     ; Q: Initial_Text_Rows spec'd?
        mov     [VDD_Initial_Text_Rows],0FFh;   Assume NO
	jc	SHORT VDD_DI_TextRowsSet    ;   N: String not found,ask BIOS later
	jz	SHORT VDD_DI_TextRowsSet    ;   N: No value exists,ask BIOS later
;
; Set [VDD_Initial_Text_Rows] = # of rows - 1
; Round EAX to a reasonable value - 24/42 for EGA,24/49 lines for VGA
;                        
IFDEF   EGA
        cmp     eax,43                      ; EGA-Less than 43 => 25 line mode
ELSE
        cmp     eax,50                      ; VGA-Less than 50 => 25 line mode
%OUT What about check for 43 vs. 50 line mode?
ENDIF
        jl      SHORT VDD_DI_25Lines

IFDEF   EGA
        mov     [VDD_Initial_Text_Rows],42
ELSE
        mov     [VDD_Initial_Text_Rows],49
ENDIF
        jmp     SHORT VDD_DI_TextRowsSet
VDD_DI_25Lines:
        mov     [VDD_Initial_Text_Rows],24
VDD_DI_TextRowsSet:        
        
;*******
;Initialize Sys VM control block
;
	call	VDD_VMCreate		    ; Init Sys VM's control block
	jc	SHORT VDD_DI_Err

IFDEF CTVGA
;*******
;Disable traps on CRT Regs by resetting trap bit in CRT emulation mode Reg.
;
IFDEF	VGAMONO
%OUT	Need 3Dx/3Bx addressing detection here
ENDIF
        mov     dx,pIndx6845Colr
        mov     ax,0FFh                     ; Emulation mode Reg-CRT Reg in CTVGA
        out     dx,ax
        inc     dx
        in      ax,dx
        and     ax,11011111B                ; Reset trap bit 5, others unchanged 
        out     dx,ax

        VMMCall Get_Sys_VM_Handle
        mov     edi,ebx
        add     edi,[VDD_CB_Off]
	mov	byte ptr [edi.VDD_Stt.G_CTCtl],20h
	mov	byte ptr [edi.VDD_Stt.C_CT400],7Ch
	mov	byte ptr [edi.VDD_Stt.C_CTTempFE],07h
ENDIF
        
	mov	edx, VDD_Device_ID	    ; Set focus to Sys VM
	call	VDD_VMSetFocus
	clc
	popad
	ret
VDD_DI_Err:
	stc
	popad
	ret
EndProc VDD_Device_Init

VxD_ICODE_ENDS

;******************************************************************************
;******************************************************************************
;
; Real mode initialization code
;
;******************************************************************************

VxD_REAL_INIT_SEG

;******************************************************************************
;
;   VDD_Real_Init
;
;   DESCRIPTION:
;	This does some detection of video hardware/software and returns
;	an initial value for Vid_Flags
;
;   ENTRY:
;	AX = VMM version number
;	BX = flags (see VMM.INC)
;	SI = environment segment
;	DX = reference data when device specified from INT 2F (not applicable)
;
;   EXIT:
;	AX = Device_Load_Ok
;	BX = 0
;	EDX = Initial value for Vid_Flags
;
;   USES:
;	Flags, AX, BX, CX, DX, BP, SI, DI
;
;==============================================================================
IFDEF	ATIEGA
ATI_Sig:	DB  " 761295520"    ; ATI EGA signature at C000:30
ATI_Sig_Len	EQU $-ATI_Sig

ENDIF
IFDEF	PVGA
str_Paradise	DB  "PARADISE"
len_str_Paradise EQU $-str_Paradise
str_WDIGITAL	DB  "WESTERN DIGITAL"
len_str_WDIGITAL EQU $-str_WDIGITAL
ENDIF
BeginProc VDD_Real_init

;********************************
; EGA real mode adapter detection
IFDEF	EGA
IFDEF	ATIEGA
	mov	si,OFFSET ATI_Sig
	mov	ax,0C000h
	mov	es,ax
	mov	di,30h
	mov	cx, ATI_Sig_Len
	rep cmpsb
	jnz	DEBFAR Not_ATI_EGA
; Further checks for version of ATI EGA
	mov	di,42h
	mov	ax,"SN"
	cmp	ax,es:[di]
	jz	SHORT Is_ATI_Won
	mov	ax,"QC"
	cmp	ax,es:[di]
	jz	SHORT Is_ATI_Won
; Assume everything else is at EGA 800
;   The two that we know about and didn't test for are "24SC" and "25SC"
	mov	ax,"CS"
	cmp	eax,es:[di]
	jnz	SHORT Not_ATI_EGA		; 800+ has no special software
Is_ATI_800:
	mov	edx,fVid_ATI800
	jmp	SHORT VRI_Exit
Is_ATI_Won:
	mov	edx,fVid_ATIWon
	jmp	SHORT VRI_Exit
Not_ATI_EGA:
ENDIF
ENDIF


;********************************
; VGA real mode adapter detection
IFDEF	VGA
IFDEF	V7VGA
;
; check for any V7 board
;
	xor	bx,bx		; clear it out
        mov     ax,6f00h
	int	10h
	cmp	bx,'V7'         ; any of the products?
	jnz	SHORT VRI_NotV7 ; nope...
;
; check the chip version #
;
	mov	cx,0ffffh
	mov	ax,06f07h	; get the # from the bios
	int	10h
	xor	dx,dx		; Assume no flags passed in
	or	cx,cx		; zero?
	jne	SHORT VRI_NotV7
	cmp	bh,70h		; V7VGA chip?
	jl	SHORT VRI_NotV7 ; nope...  must be in range 70h-7fh
;
; if we get here it is a video seven board with all the trimmings
;
; well, it's a VRAM, Fastwrite, or 1024i
;
	mov	edx,fVid_V7VGA			;      DX = -1 (true)
	jmp	VRI_Exit
ENDIF ; V7VGA
VRI_NotV7:
IFDEF	PVGA
	mov	dx,3CEh
	mov	al,0Fh
	out	dx,al
	inc	dx
	in	al,dx
	push	ax
	and	al,0F0h
	mov	ah,al
	xor	al,0F0h 			; Reverse upper 4 bits
	or	al,5
	out	dx,al
	IO_Delay
	in	al,dx
	xor	al,ah				; Q: Are lower bits 5 and
	cmp	al,5				;	upper bits unchanged?
	pop	ax
	out	dx,al				; Restore original value
	jnz	SHORT NotPVGA			;   N: Must not be PVGA
; Paradise VGA, check for Paradise VGA ROM
	mov	ax,0C000h
	mov	es,ax
	mov	si,OFFSET str_Paradise
	xor	di,di
	mov	cx,128
	mov	edx,fVid_PVGA			; Assume not Paradise ROM
PVGA_FindROMLoop:
	push	di
	push	si
	push	cx
	mov	cx,len_str_Paradise
	repe cmpsb				; Q: Paradise ROM?
	pop	cx
	pop	si
	pop	di
	jz	SHORT IsPVGARom 		;   Yes
	inc	di
	loop	PVGA_FindROMLoop		;   No, look thru 128 bytes
; PARADISE not found, now look for WESTERN DIGITAL
	mov	si,OFFSET str_WDIGITAL
	xor	di,di
	mov	cx,128
PVGA_FindROMLoop1:
	push	di
	push	si
	push	cx
	mov	cx,len_str_WDigital
	repe cmpsb				; Q: Paradise ROM?
	pop	cx
	pop	si
	pop	di
	jz	SHORT IsPVGARom 		;   Yes
	inc	di
	loop	PVGA_FindROMLoop1		;   No, look thru 128 bytes
	jmp	NotPVGA
IsPVGARom:
	mov	edx,fVid_PVGA+fVid_PVGARom
	jmp	VRI_Exit
NotPVGA:
ENDIF ; PVGA
IFDEF	TLVGA

;*******
; Test for Tseng Labs VGA (we need to exclude some V86 memory if present)
;	Attribute Controller Reg 16h is known to exist only on Tseng Labs VGA
	mov	dx,pMiscVGAIn
	in	al,dx
	test	al,1
	mov	dl,(pStatColr AND 0FFh)
	jnz	SHORT VSI_Colr
	mov	dl,(pStatMono AND 0FFh)
VSI_Colr:
	push	dx
	in	al,dx
	IO_Delay
	mov	dl,(pAttrEGA AND 0FFh)
	in	al,dx
	push	ax
	IO_Delay
	mov	al,16h+20h			; Select 16h (leave video on)
	out	dx,al
	IO_Delay
	inc	dx
	in	al,dx
	IO_Delay
	dec	dx
	mov	ah,al				; Save current reg 16h in AH
	xor	al,10h				; Complement bit 4
	out	dx,al				; Write it out
	IO_Delay
	pop	dx
	push	dx
	in	al,dx
	mov	dx,pAttrEGA
	mov	al,16h+20h			; Select 16h (leave video on)
	out	dx,al
	IO_Delay
	inc	dx
	in	al,dx
	IO_Delay
	xor	cx,cx
	dec	dx
	xor	al,10h
	cmp	al,ah				; Q: Is value same as written?
	jnz	SHORT Not_TLVGA
	mov	ecx,fVid_TLVGA
Not_TLVGA:
	mov	al,ah
	out	dx,al
	IO_Delay
	pop	dx
	in	al,dx
	IO_Delay
	mov	dl,(pAttrEGA AND 0FFh)
	pop	ax
	out	dx,al
	mov	edx,ecx
	or	edx,edx
	jnz	VRI_Exit
ENDIF ; TLVGA

IFDEF   CLVGA        
; First save SR6 and SRIndx
        xor     cx,cx                   ; assume not CLVGA
        mov     dx,3c4h
        in      al,dx
        IO_Delay
        mov     ah,al                   ; AH = SR index
        mov     al,6
        out     dx,al
	IO_Delay
        inc     dx
        in      al,dx                    
        IO_Delay
        push    ax                      ; AH=SRindx,AL=SR6 value
;enable extension register in the CLVGA
        mov     al,0EAh
        out     dx,al        
	IO_Delay

        in      al,dx
	IO_Delay
        cmp     al,1                    ; Q: Could enable Ext?
	jne	SHORT Not_CLVGA 	;   N: cannot be CLVGA

; additional tests to confirm it is CLVGA

; attr index readable at SR83h and 3c0h for CLVGA
        dec     dx
        mov     al,083h
        out     dx,al
        IO_Delay
        inc     dx
        in      al,dx
        IO_Delay
        mov     ah,al                   ; AH = value of ARX at SR83h
        
        mov     dx,03C0h
        in      al,dx                   ; AL = value of ARX at 3C0h
        and     ax,01F1Fh               ; mask to five LSBs
        cmp     ah,al                   ; Q: Same value read at 3c0 and 83H?
	jne	SHORT Not_CLVGA 	;   N: cannot be CLVGA

; test if we can toggle the high bit in SR83 for Attr index/data
        
; read from 3da should clear high bit of SR83
        mov     dx,3dah
        in      al,dx
        IO_Delay

        mov     dx,3c5h
        in      al,dx
        IO_Delay
        test    al,80h
	jnz	SHORT Not_CLVGA

; write to 3c0 now should set the high bit of SR83 
        mov     dx,3c0h
        in      al,dx
        IO_Delay
        out     dx,al                   

        mov     dx,3c5h
        in      al,dx
        IO_Delay

        test    al,80h
	jz	SHORT Not_CLVGA

	mov	ecx,fVid_CLVGA			; Indicate is Cirrus Logic chip
Not_CLVGA:
        mov     dx,3c4h
        mov     al,6
        out     dx,al           
        IO_Delay
        pop     ax
	or	cx,cx
	jz	SHORT Restor_SR6
	or	al,al
	mov	al,0EAh
	jnz	SHORT Restor_SR6
	mov	al,0AEh
Restor_SR6:
        inc     dx
        out     dx,al           
        IO_Delay
        mov     al,ah
        dec     dx
        out     dx,al           
        IO_Delay
	mov	edx,ecx
	or	edx,edx
        jnz     SHORT VRI_Exit
ENDIF ; CLVGA
ENDIF ; VGA

	xor	edx,edx
; DX = initial flags for VDD
VRI_Exit:
	xor	bx,bx				; No pages to exclude
	mov	si,bx				; No instance data
	mov	ax, Device_Load_Ok		; Continue with load
	ret

EndProc VDD_Real_init

VxD_REAL_INIT_ENDS

	END VDD_Real_init

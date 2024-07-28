PAGE 58,132
;******************************************************************************
TITLE vkd.asm -
;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp., 1988-1990.  All rights reserved.
;
;   Title:	vkd.asm - virtual keyboard device
;
;   Version:	2.00
;
;   Date:	29-Jul-1988
;
;   Author:	RAP
;
;------------------------------------------------------------------------------
;
;   Change log:
;
;      DATE	REV		    DESCRIPTION
;   ----------- --- -----------------------------------------------------------
;   29-Jul-1988 RAP Started complete rewrite
;
;==============================================================================

	.386p



;******************************************************************************
;			      I N C L U D E S
;******************************************************************************

.XLIST
	INCLUDE VMM.INC
	INCLUDE Debug.INC
	INCLUDE VPICD.INC
	INCLUDE VSD.INC
	INCLUDE VDD.INC

	Create_VKD_Service_Table EQU 1	    ; VKD service table created
	INCLUDE VKD.INC

	INCLUDE VKDSYS.INC
	INCLUDE OPTTEST.Inc
.LIST


IFNDEF DebugPorts
DebugPorts = 0	   ; no debugging
ENDIF

.errnz VKD_Current_Version - VKD_Version


;******************************************************************************
;		 V I R T U A L	 D E V I C E   D E C L A R A T I O N
;******************************************************************************

Declare_Virtual_Device VKD, 2, 0, VKD_Control, VKD_Device_ID, VKD_Init_Order, \
		       , VKD_PM_API_Entry


;******************************************************************************
;		    I N I T I A L I Z A T I O N   D A T A
;******************************************************************************

VxD_IDATA_SEG

VKD_Int9_Desc LABEL DWORD
VPICD_IRQ_Descriptor <1,,OFFSET32 VKD_Int_09,OFFSET32 VKD_Virt_Int,\
		      OFFSET32 VKD_EOI,,OFFSET32 VKD_IRET>

VKD_Inst1 InstDataStruc <,,415h, 40, ALWAYS_Field>  ; Int 16 buffer + status
VKD_Inst2 InstDataStruc <,,471h,  1, ALWAYS_Field>  ; Break bit
VKD_Inst3 InstDataStruc <,,480h,  4, ALWAYS_Field>  ; Buffer start and end
VKD_Inst4 InstDataStruc <,,496h, 11, ALWAYS_Field>  ; Enhanced keyboard status

EXTRN	Kbd_Boost_Time:BYTE
EXTRN	Kbd_Alt_Delay:BYTE
EXTRN	Kbd_Paste_TimeOut:BYTE
EXTRN	Kbd_Paste_Pause_Ini:BYTE
EXTRN	Kbd_CR_Paste_Pause_Ini:BYTE
EXTRN	Kbd_SlowPaste_Delay:BYTE
EXTRN	Kbd_AltPaste_Delay:BYTE
EXTRN	Kbd_PasteBuf_Delay:BYTE
EXTRN	VKD_failed_irq_init:BYTE
EXTRN	Key_Delay_Ini:BYTE

IFDEF Support_Reboot
EXTRN	Kbd_Reboot_Ini:BYTE
ENDIF

VxD_IDATA_ENDS

;******************************************************************************
;******************************************************************************


VxD_LOCKED_DATA_SEG

PUBLIC	VKD_irq_Handle
PUBLIC	VKD_CB_Offset
PUBLIC	VKD_Kbd_Owner

VKD_irq_Handle	    dd	0		; keyboard IRQ handle from VPICD
VKD_CB_Offset	    dd	?		; offset into VM control block for VKD
VKD_Kbd_Owner	    dd	?		; VM handle of focus owner

IFDEF DEBUG
	public	VK_D_QFlag
VK_D_QFlag	db	0
ENDIF

VxD_LOCKED_DATA_ENDS


VxD_DATA_SEG

EXTRN	Hot_Key_List:DWORD
EXTRN	Hot_Key_Notify_List:DWORD

	ALIGN 4

; scan code prefixes for the enhanced keyboard
; keys with extended scan code sequences
;
;	i.e.   make   E0 AA E0 35
;	       break  E0 B5 E0 2A
;
; 35, 37, 47, 48, 49, 4B, 4D, 4F, 50, 51, 52, 53

LowKeyOfSet  equ 35h
HighKeyOfSet equ 53h

LongKeySet  equ 1111101010111000000000000000101b
;		5555444444444444444433333333333
;		3210fedcba9876543210fedcba98765

;	       10987654321098765432109876543210

;------------------------------------------------------------------------------
; time out variables & default values in milliseconds

Int_Boost_Amount      equ 1
PasteTimeout	      equ 1000
INT9_Paste_Delay      equ 3
INT9_Alt_Delay	      equ 25
INT9_buf_full_Delay   equ 200
Alt_Delay	      equ 5

PUBLIC	VKD_Int_Boost_Amount
PUBLIC	VKD_Alt_Delay
PUBLIC	VKD_PasteTimeout
PUBLIC	VKD_INT9_Paste_Delay
PUBLIC	VKD_INT9_Alt_Delay
PUBLIC	VKD_INT9_buf_full_Delay

VKD_Int_Boost_Amount	dd  ?
VKD_PasteTimeout	dd  ?
VKD_INT9_Paste_Delay	dd  ?
VKD_INT9_Alt_Delay	dd  ?
VKD_INT9_buf_full_Delay dd  ?
VKD_Alt_Delay		dd  ?

;------------------------------------------------------------------------------


PUBLIC VKD_gbl_shift_state

VKD_gbl_shift_state label dword
gbl_shift_state     dd	?		; current global shift state

real_release_service	dd  0		; original Release_Time_Slice service
					; handler

ss_queue_proc	    dd	OFFSET32 Queue_Output	;indirect call that
					; Update_Shift_State makes to queue
					; scan codes.  Normally Queue_Output
					; is used to queue the scan code into
					; the VM's output buffer, but in
					; VKD_API_Force_Key, scan codes need to
					; be forced into the hardware keyboard
					; buffer, so VKD_Put_Byte is used.

VKD_ignore_key	    dd	0		; virtual key that should be ignored
					; until the up transition occurs, this
					; up transition was sent into a VM by
					; VKD_Reflect_Hot_Key already, so we
					; want to ignore all auto-repeats and
					; the up transition so it can't be sent
					; to the wrong VM!  (0, if none)

VKD_build	    dd	0		; holding variable of virtual key that
					; is currently being built

VKD_LongKeySet	    dd	LongKeySet	; set of scan codes that can have
					; "extended" sequences of E0 and
					; possibly forced shift transitions
; bits for the 2 following sets are defined by LongKeySet above
;
VKD_LongDown	    dd	0		; bit set if special extended key down
VKD_LongWithShift   dd	0		; bit set if special extended key down
					; with pseudo shift state

VKD_locals_defined  dd	0FFFFFFFEh	; bit set, up to 31 local hot keys
					; possible (a 1 bit represents a
					; free key mask - bit 0 is reserved)

PUBLIC VKD_flags
VKD_flags	    dd	0

VKD_KEY_IDLE_DELAY  dd	0
gbl_8042_cmd_byte   db	0

VKD_pause_count     db	2
VKD_CR_pause_count  db	10

IFDEF Support_Reboot
PUBLIC VKD_attempt_reboot
VKD_attempt_reboot  db	0FFh
ENDIF

	ALIGN 4

;
; Key buffer for VM in special message mode.  A scan code byte & shift state
; modifier byte is placed in this buffer for each down stroke that occurs
; while the VM is in message mode (autorepeat & up transitions are ignored.)
; Entries can be read using the services VKD_Get_Msg_Key and VKD_Peek_Msg_Key.
; The buffer is global, because only 1 VM should be in message mode at a time.
; If overlap does occur, then the 2nd VM to go into message mode takes control
; of this buffer and read attempts in the first VM will be ignored (it is
; assumed that the first VM is about to go out of message mode!)
;
Msg_Key_Buf_Size    equ 16 * 2		; 2 bytes per entry

msg_key_buf_owner   dd	0
msg_key_head	    dd	0		; next entry to suck out of the buffer
msg_key_tail	    dd	0		; index for next entry
msg_key_buf	    dw	Msg_Key_Buf_Size/2 DUP (?)
last_msg_key	    db	0

	ALIGN 4


;------------------------------------------------------------------------------
; Define scan code to shift state conversion table
;
; The table will have a shift state status word for each scan code that needs
; to update the shift state.  It will basically be a sparse table with the
; minimum entry being scan code "shft_tab_min" and the max being "shft_tab_max"
;
; The building is done with macros, and the process is to define symbols of
;
;    "shft_tab_vXX" (where XX is the decimal scan code)
;
; for each scan code that has a shift state change, then to loop through the
; possible symbols (from min to max) and declare a word value of the status
; word for defined symbols, or 0.  This results in creating a table with
; (shft_tab_max - shft_tab_min + 1) word entries.
;

shft_tab_min	= 255	; start with maximum possible value
shft_tab_max	= 0	; start with minimum possible value

SetValue    MACRO   scanexp, statebits
	shft_tab_v&scanexp = statebits
ENDM

PlaceValue  MACRO   scanexp
    IFDEF shft_tab_v&scanexp
	dw  shft_tab_v&scanexp
    ELSE
	dw  0
    ENDIF
ENDM

DefineState MACRO   scanc, statebits
    IF scanc LT shft_tab_min
	shft_tab_min = scanc
    ENDIF
    IF scanc GT shft_tab_max
	shft_tab_max = scanc
    ENDIF

	SetValue %(scanc), %(statebits)
ENDM

BuildShiftTable MACRO
    shft_idx = shft_tab_min
    REPT shft_tab_max - shft_tab_min + 1
	PlaceValue %(shft_idx)
    shft_idx = shft_idx + 1
    ENDM
ENDM

DefineState	SC_LShf,  SS_LShift
DefineState	SC_RShf,  SS_RShift
DefineState	SC_Ctrl,  SS_LCtrl
DefineState	SC_Alt,   SS_LAlt
DefineState	SC_ScLok, SS_ScrlLock
DefineState	SC_NmLok, SS_NumLock
DefineState	SC_CpLok, SS_CapLock

shift_table	label word	; for conversion from scan codes to state
BuildShiftTable
;------------------------------------------------------------------------------

State_Conv_Struc    STRUC
	scancode    db	0
	handlerCase db	0
State_Conv_Struc    ENDS

SingleCase  equ 0 * 4		; offsets into case (jump) table
DoubleCase  equ 1 * 4
ToggleCase  equ 2 * 4
AltCase     equ 3 * 4

state_table	label byte	; for conversion from state to scan codes
    State_Conv_Struc <,>		    ; 0
    State_Conv_Struc <SC_LShf,	SingleCase> ; 1     Left-Shift
    State_Conv_Struc <SC_Ctrl,	SingleCase> ; 2     Left-Ctrl
    State_Conv_Struc <SC_Alt,	SingleCase> ; 3     Left_Alt
;;    State_Conv_Struc <SC_Alt,   AltCase>    ; 3     Left_Alt
    State_Conv_Struc <SC_ScLok, ToggleCase> ; 4     Scroll Lock
    State_Conv_Struc <SC_NmLok, ToggleCase> ; 5     Num Lock
    State_Conv_Struc <SC_CpLok, ToggleCase> ; 6     Caps Lock
    State_Conv_Struc <,>		    ; 7
    State_Conv_Struc <,>		    ; 8
    State_Conv_Struc <SC_RShf,	SingleCase> ; 9     Right-Shift
    State_Conv_Struc <SC_Ctrl,	DoubleCase> ; 10    Right-Ctrl
    State_Conv_Struc <SC_Alt,	DoubleCase> ; 11    Right_Alt
;;    State_Conv_Struc <SC_Alt,   AltCase>    ; 11    Right_Alt
    State_Conv_Struc <SC_ScLok, SingleCase> ; 12    Scroll Lock Down
    State_Conv_Struc <SC_NmLok, SingleCase> ; 13    Num Lock Down
    State_Conv_Struc <SC_CpLok, SingleCase> ; 14    Caps Lock Down


NumPad_SCs  db SC_Num0, SC_Num1, SC_Num2, SC_Num3, SC_Num4, SC_Num5
	    db SC_Num6, SC_Num7, SC_Num8, SC_Num9

VxD_DATA_ENDS


;==============================================================================

; this macro causes " (SYS VM)" to be displayed on the debugging screen, if
; EBX is SYS_VM_Handle, used in VKD_Set_Focus

SYS_VM_Out_Users = 0
SYS_VM_Out	MACRO noeol
	LOCAL	not_system
IFDEF DEBUG

SYS_VM_Out_Users = SYS_VM_Out_Users + 1
IF SYS_VM_Out_Users EQ 1
_DATA SEGMENT
??_SYS_VM_Mes db " (SYS VM)", 0
??_CRLF       db 13, 10, 0
_DATA ENDS
ENDIF
	VMMcall Test_Sys_VM_Handle	    ;Q: system VM?
	jne	short not_system
	pushfd
	pushad
	mov	esi, OFFSET32 ??_SYS_VM_Mes
	VMMcall Out_Debug_String
	popad
	popfd

not_system:
IFB <noeol>
	pushfd
	pushad
	mov	esi, OFFSET32 ??_CRLF
	VMMcall Out_Debug_String
	popad
	popfd
ENDIF
ENDIF
	ENDM



VxD_CODE_SEG

EXTRN	VKD_Enable_Kbd:NEAR
EXTRN	VKD_Disable_Kbd:NEAR
EXTRN	VKD_Clear_Data_Buf:NEAR
EXTRN	VKD_Send_Cmd:NEAR

EXTRN	VAD_Device_Init:NEAR
EXTRN	VAD_Create_VM:NEAR
EXTRN	VAD_Destroy_VM:NEAR
EXTRN	VAD_System_Exit:NEAR
EXTRN	VAD_Set_Focus:NEAR

EXTRN	VKD_OutputByte:NEAR
EXTRN	VKD_IO_Init:NEAR
EXTRN	Get_8042_Byte:NEAR
EXTRN	Chk_Hot_Keys:NEAR
EXTRN	Hot_Key_Entered:NEAR
EXTRN	Hot_Key_Ended:NEAR

IFDEF DebugHKs
EXTRN	VKD_Show_Test_HKs:NEAR
EXTRN	VKD_Test_HK:NEAR
EXTRN	VKD_Test_Msg_Mode:NEAR
EXTRN	VKD_Test_Reflect_HK:NEAR
ENDIF

VxD_CODE_ENDS


PAGE
;******************************************************************************
;	      V M D M	I N I T I A L I Z A T I O N   C O D E
;******************************************************************************

VxD_ICODE_SEG

;******************************************************************************
;
;   VKD_Wait_Busy
;
;   DESCRIPTION:    spin 64K times waiting for 8042 to not be busy
;
;   ENTRY:	    nothing
;
;   EXIT:	    Z flag set if 8042 not busy
;
;   USES:	    AL, ECX
;
;==============================================================================
BeginProc VKD_Wait_Busy

	mov	ecx, 10000h
WB_Lp:
	in	al, pstat_Kybd
	test	al, fKBS_Bsy		; Is buffer still full?
	loopdnz WB_Lp			;   Yes, wait till empty or timeout
	ret

EndProc VKD_Wait_Busy

;******************************************************************************
;
;   VKD_Wait_Empty
;
;   DESCRIPTION:    Spin 64K times waiting for 8042 to not be empty.
;		    Used to wait for a response from an 8042 command.
;
;   ENTRY:	    nothing
;
;   EXIT:	    Z flag set if 8042 still empty
;
;   USES:	    AL, ECX
;
;==============================================================================
BeginProc VKD_Wait_Empty

	mov	ecx, 10000h
Kybd_RI_Lp:
	in	al, pstat_Kybd
	test	al, fKBS_DAV
	loopdz	Kybd_RI_Lp
	ret

EndProc VKD_Wait_Empty


;******************************************************************************
;
;   VKD_Sys_Critical_Init
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

BeginProc VKD_Sys_Critical_Init

;
;   Allocate portion of each VM's control block for VKD
;
	VMMCall _Allocate_Device_CB_Area, <<SIZE VKD_CB_Struc>, 0>
	test	eax, eax
	jnz	SHORT @F
	Debug_Out "VKD ERROR:  Couldn't alloc control block data area space"
VKD_Init_Fatal:
	VMMcall Fatal_Memory_Error
@@:
	mov	[VKD_CB_Offset], eax

;
; get INI time out overrides
;

	mov	eax, Int_Boost_Amount
	mov	ecx, 3
	xor	esi, esi
	mov	edi, OFFSET32 Kbd_Boost_Time
	VMMCall Get_Profile_Fixed_Point
	mov	[VKD_Int_Boost_Amount], eax

	mov	eax, Alt_Delay
	mov	edi, OFFSET32 Kbd_Alt_Delay
	VMMCall Get_Profile_Fixed_Point
	mov	[VKD_Alt_Delay], eax

	mov	eax, PasteTimeout
	mov	edi, OFFSET32 Kbd_Paste_TimeOut
	VMMCall Get_Profile_Fixed_Point
	mov	[VKD_PasteTimeout], eax

	movzx	eax, [VKD_pause_count]
	mov	edi, OFFSET32 Kbd_Paste_Pause_Ini
	VMMCall Get_Profile_Decimal_Int
	mov	[VKD_pause_count], al

	movzx	eax, [VKD_CR_pause_count]
	mov	edi, OFFSET32 Kbd_CR_Paste_Pause_Ini
	VMMCall Get_Profile_Decimal_Int
	mov	[VKD_CR_pause_count], al

	mov	eax, INT9_Paste_Delay
	mov	edi, OFFSET32 Kbd_SlowPaste_Delay
	VMMCall Get_Profile_Fixed_Point
	mov	[VKD_INT9_Paste_Delay], eax

	mov	eax, INT9_Alt_Delay
	mov	edi, OFFSET32 Kbd_AltPaste_Delay
	VMMCall Get_Profile_Fixed_Point
	mov	[VKD_INT9_Alt_Delay], eax

	mov	eax, INT9_buf_full_Delay
	mov	edi, OFFSET32 Kbd_PasteBuf_Delay
	VMMCall Get_Profile_Fixed_Point
	mov	[VKD_INT9_buf_full_Delay], eax

	mov	eax, 500
	mov	ecx, 3
	mov	edi, OFFSET32 Key_Delay_Ini
	VMMCall Get_Profile_Fixed_Point
	mov	[VKD_KEY_IDLE_DELAY], eax

IFDEF Support_Reboot
	mov	eax, TRUE
	mov	edi, OFFSET32 Kbd_Reboot_Ini
	VMMCall Get_Profile_Boolean
	mov	[VKD_attempt_reboot], al
ENDIF

;
; hook Release_Time_Slice if it isn't already
;
	cmp	[real_release_service], 0   ;Q: service been hooked?
	jne	short sci_release_hooked    ;	Y:
	mov	eax, Release_Time_Slice
	mov	esi, OFFSET32 @VKD_Release_TS_Hook
	VMMcall Hook_Device_Service
	mov	[real_release_service], esi
sci_release_hooked:

;
; create hot key list
;
	xor	eax, eax		    ; Flags = 0
	mov	ecx, SIZE Hot_Key_Def	    ; ECX = size of each list node
	VMMCall List_Create
	jc	VKD_Init_Fatal
	mov	[Hot_Key_List], esi	    ; save list handle

;
; create hot key notification list
;
	xor	eax, eax		    ; Flags = 0
	mov	ecx, SIZE Hot_Key_Notify_Struc ; ECX = size of each list node
	VMMCall List_Create
	jc	VKD_Init_Fatal
	mov	[Hot_Key_Notify_List], esi  ; save list handle
;
; hook the keyboard IRQ
;
;   NOTE:  the IRQ is virtualized and masked before we start doing I/O to
;	   the keyboard controller, because on some machines the I/O that
;	   we are doing momentarily requests the IRQ which happened to
;	   confuse VPICD.  Most notibly this occurred on the IBM PS/2 model
;	   90 with the built in XGA; this confusion causes keyboard and
;	   mouse interrupts to get locked, so that no input was possible.
;	   By vitualizing the IRQ and masking it, if an IRQ request is
;	   generated it is held pending in the PIC and is cleared by the
;	   end of our init sequence anyway.
;
	mov	edi, OFFSET32 VKD_Int9_Desc
	VxDCall VPICD_Virtualize_IRQ
	jc	VKD_Init_Error		    ; Blow up if error
	mov	[VKD_irq_Handle], eax	    ; save handle
	VxDCall VPICD_Physically_Mask	    ; mask the IRQ
	VxDCall VPICD_Clear_Int_Request
	VxDCall VPICD_Get_Complete_Status
	TestReg ecx, VPICD_Stat_Phys_In_Serv	;Q: VPICD has this in service?
	jz	short @F
	VxDCall VPICD_Phys_EOI		    ; EOI the physical interrupt
@@:

;
; read current command byte
;
	call	VKD_Wait_Busy
	jnz	short sci_no_cmd
	mov	al, KBD_Ctl_Dis
	out	pcmd_Kybd, al
	IO_Delay

; read until 8042 stops reporting data available
;
sci_nxt_data:
	in	al, pstat_Kybd
	test	al, fKBS_DAV
	jz	short sci_empty
	in	al, pdata_Kybd
	IO_Delay
	jmp	sci_nxt_data
sci_empty:

	call	VKD_Wait_Busy
	jnz	short sci_no_cmd
	mov	al, KBD_Ctl_RdCmd
	out	pcmd_Kybd, al
	IO_Delay

	call	VKD_Wait_Busy
	jnz	short sci_no_cmd
	call	VKD_Wait_Empty
	jnz	short sci_cmd
sci_no_cmd:
	mov	al, 01000101b		; default command byte
		  ;  ^^^^^ ^----------------generate interrupts
		  ;  ||||`------------------system flag set in status
		  ;  |||`-------------------obey security lock
		  ;  ||`--------------------enable keyboard
		  ;  |`---------------------use 11-bit keyboard codes
		  ;  `----------------------convert keys to scan codes
	jmp	short sci_save_byte

sci_cmd:
	in	al, pdata_Kybd

;------------------------------------------------------------------------------
; Read status port and data port a 2nd time to cooperate with oddness in
; PS/2-55 & 65	SX machines.  On these machines the 1st read of the data
; port doesn't clear the int request, so we get a spurious int as soon as
; interrupts are enabled.  Just delaying and reading the data port a 2nd time
; wasn't enough; the status port needed to be read also even though it
; reports that no data is available!

	IO_Delay
	IO_Delay
	mov	ah, al
	in	al, pstat_Kybd
	IO_Delay
	IO_Delay
	in	al, pdata_Kybd
	IO_Delay
	IO_Delay
	mov	al, ah
;------------------------------------------------------------------------------

	or	al, fKBC_Int		; force ints on, in case we didn't
	and	al, NOT fKBC_Dis	; force keyboard enabled bit
sci_save_byte:
	mov	[gbl_8042_cmd_byte], al ; get to read the real byte

	call	VKD_Wait_Busy
	mov	al, KBD_Ctl_Ena 	; re-enable the keyboard
	out	pcmd_Kybd, al

;
; SYS VM is first owner
;
	mov	[VKD_Kbd_Owner], ebx	    ; VM1 owns the keyboard at start

;
; get initial shift state from SYS VM's BIOS data
;
	mov	esi, [VKD_CB_Offset]
	add	esi, ebx
	call	VKD_Convert_BIOS_Shift_State
	mov	[gbl_shift_state], eax
	mov	[esi.loc_shift_state], ax

	or	[esi.kbdState], KBS_critical_init ; we don't get a separate
						  ; critical init call for SYS VM

	and	ax, SS_Toggle_mask
	mov	[esi.BIOS_toggle_state], al

; init virtual command byte

	mov	al, [gbl_8042_cmd_byte]
	mov	[esi._8042_cmd_byte], al

	mov	eax, [VKD_irq_Handle]
	VxDCall VPICD_Physically_Unmask     ; unmask the IRQ

;
; hook software INT 16h
;
	mov	eax, 16h		    ; Hook INT 16h
	mov	esi, OFFSET32 VKD_Int_16
	VMMcall Hook_V86_Int_Chain

;
; claim keyboard related instance data
;
	VMMCall _AddInstanceItem, <<OFFSET32 VKD_Inst1>, 0>
	or	eax,eax
	jz	VKD_Init_Fatal
	VMMCall _AddInstanceItem, <<OFFSET32 VKD_Inst2>, 0>
	or	eax,eax
	jz	VKD_Init_Fatal
	VMMCall _AddInstanceItem, <<OFFSET32 VKD_Inst3>, 0>
	or	eax,eax
	jz	VKD_Init_Fatal
	VMMCall _AddInstanceItem, <<OFFSET32 VKD_Inst4>, 0>
	or	eax,eax
	jz	VKD_Init_Fatal
;
; trap I/O ports & check for machine specific INI configuration switches
;
	call	VKD_IO_Init

	SetFlag [VKD_flags], VKDf_initialized	; flag initialization complete

	clc
	ret

VKD_Init_Error:
	Debug_Out "Unable to initialize keyboard IRQ"
	Fatal_Error <OFFSET32 VKD_failed_irq_init>


EndProc VKD_Sys_Critical_Init


;******************************************************************************
;
;   VKD_Device_Init
;
;   DESCRIPTION:
;
;   ENTRY:	    EBX = SYS VM's handle
;
;   EXIT:	    Carry clear if no error
;
;   USES:	    Nothing
;
;   ASSUMES:	    Interrupts have been (and still are) disabled.
;
;==============================================================================

BeginProc VKD_Device_Init

;
; call VAD to let it initialize before keyboard I/O is trapped
;
	call	VAD_Device_Init
	jc	VKD_Init_Fatal

;
; clear SYS VM's BIOS keyboard buffer so that we don't get the buffer contents
; dup'ed in the instance snapshot
;
	call	VKD_Disable_Kbd
	call	VKD_Clear_Data_Buf
	call	VKD_Enable_Kbd

	Push_Client_State
	VMMcall Begin_Nest_Exec 	    ; Get ready for software ints
empty_buf_lp:
	mov	[ebp.Client_AH], 1	    ;Q: char in buffer?
	mov	eax, 16h
	VMMcall Exec_Int
	bt	[ebp.Client_Flags], ZF_Bit
	jc	short buf_empty 	    ;	N: we're done!
	mov	[ebp.Client_AH], 0	    ;	Y: read it
	mov	eax, 16h
	VMMcall Exec_Int
	jmp	empty_buf_lp

buf_empty:
	VMMcall End_Nest_Exec
	Pop_Client_State

;
; Force keys into SYS VM to get shift state to 0.  This is done in case the
; user had a shift key down and then released it while interrupts were
; disabled.  In that case, the up transition was never seen or processed, so
; shift states all showed that the shift key was still down.  If the user is
; legitimately holding the key down, e.g. to indicate that Progman shouldn't
; process the startup group, then the key will still be in auto-repeat mode,
; so we will see another down transition and update the global state again.
;
	mov	esi, [VKD_CB_Offset]
	add	esi, ebx
	mov	edx, esi
	push	ebx
	mov	ebx, [gbl_shift_state]
	and	ebx, SS_Toggle_mask	    ; don't modify toggle key state
	mov	[gbl_shift_state], ebx
	mov	ax, [esi.loc_shift_state]
	call	Update_Shift_State
	pop	ebx
	jz	short @F		    ; jump if no update necessary
	call	VKD_VM_Server		    ; call server, to start process of
					    ; sending update codes from outbuf
@@:

;------------------------------------------------------------------------------
IFDEF DebugHKs	 ; test some additional hot key combinations
%OUT declaring sample hot keys for debugging
	mov	ax, 3Bh 		    ; ShiftLk+F1
	ShiftState SS_Toggle_mask, SS_ScrlLock_Dn
	mov	esi, OFFSET32 VKD_Show_Test_HKs
	mov	cl, CallOnPress
	xor	edx, edx		    ; no reference data
	xor	edi, edi		    ; always notify
	VxDCall VKD_Define_Hot_Key

	mov	ax, 1Eh 		    ; NumLk+A
	ShiftState SS_Toggle_mask, SS_NumLock_Dn
	mov	esi, OFFSET32 VKD_Test_HK
	mov	cl, CallOnAll
	mov	edx, 1
	VxDCall VKD_Define_Hot_Key

	mov	ax, 1Eh 		    ; RAlt+A
	ShiftState <SS_Alt + SS_Toggle_mask>, SS_RAlt
	mov	cl, CallOnAll
	mov	esi, OFFSET32 VKD_Test_HK
	mov	edx, 2
	VxDCall VKD_Define_Hot_Key

	mov	ax, 1Eh 		    ; LAlt+RAlt+A
	ShiftState <SS_Alt + SS_Toggle_mask>, <SS_LAlt + SS_RAlt>
	mov	cl, CallOnAll
	mov	esi, OFFSET32 VKD_Test_HK
	mov	edx, 3
	VxDCall VKD_Define_Hot_Key

	mov	ax, SC_RShf		    ; LShft+RShft
	ShiftState <SS_Shift + SS_RShift + SS_Toggle_mask>, SS_LShift
	mov	cl, CallOnAll
	mov	esi, OFFSET32 VKD_Test_HK
	mov	edx, 4
	VxDCall VKD_Define_Hot_Key

	mov	ax, 58h 		    ; F12
	mov	ebx, HKSS_Ctrl		    ; look for a Shift key
	mov	cl, CallOnPress
	mov	esi, OFFSET32 VKD_Test_Msg_Mode
	VxDCall VKD_Define_Hot_Key

	mov	ax, AllowExtended + 53h     ; Del
	mov	ebx, HKSS_Shift 	    ; look for a Shift key
	mov	cl, CallOnAll
	mov	esi, OFFSET32 VKD_Test_HK
	mov	edx, 6
	VxDCall VKD_Define_Hot_Key

	mov	ax, SC_ScLok		    ; Scroll-lock
	ShiftState <SS_Toggle_mask>, SS_ScrlLock_Dn
	mov	cl, CallOnAll
	mov	esi, OFFSET32 VKD_Test_HK
	mov	edx, 8
	VxDCall VKD_Define_Hot_Key

VxD_DATA_SEG
tst_key1    dd	0
tst_key2    dd	0
VxD_DATA_ENDS

	mov	ax, 1Eh 		    ; A
	mov	ebx, HKSS_Ctrl		    ; look for a Ctrl key
	mov	cl, CallOnAll + Local_Key
	mov	esi, OFFSET32 VKD_Test_Reflect_HK
	mov	edx, OFFSET32 tst_key1
	VxDCall VKD_Define_Hot_Key

	mov	ax, 2Ch 		    ; Z
	mov	ebx, HKSS_Alt		    ; look for a Alt key
	mov	cl, CallOnAll + Local_Key
	mov	esi, OFFSET32 VKD_Test_Reflect_HK
	mov	edx, OFFSET32 tst_key2
	VxDCall VKD_Define_Hot_Key

	mov	ax, 2Ch 		    ; Z
	mov	ebx, HKSS_Ctrl		    ; look for a Ctrl key
	mov	cl, CallOnAll
	mov	esi, OFFSET32 VKD_Test_Reflect_HK
	xor	edx, edx
	VxDCall VKD_Define_Hot_Key
ENDIF ;DebugHKs

	clc
	ret

EndProc VKD_Device_Init

VxD_ICODE_ENDS


page
;******************************************************************************
;			     V x D   C A L L S
;******************************************************************************

VxD_CODE_SEG

EXTRN	VKD_Int_09:NEAR
EXTRN	VKD_Buf_Empty:NEAR
EXTRN	VKD_Get_Byte:NEAR
EXTRN	VKD_Put_Byte:NEAR

IFDEF Support_Reboot
EXTRN	VKD_Reboot:NEAR
ENDIF


;******************************************************************************
;
;   VKD_Control
;
;   DESCRIPTION:    dispatch control messages to the correct handlers
;
;   ENTRY:
;
;   EXIT:	    Carry clear if no error
;
;   USES:
;
;==============================================================================

Begin_Control_Dispatch VKD

	Control_Dispatch Sys_Critical_Init,  VKD_Sys_Critical_Init
	Control_Dispatch Device_Init,	     VKD_Device_Init
	Control_Dispatch Sys_VM_Init,	     VKD_VM_Init
	Control_Dispatch VM_Init,	     VKD_VM_Init
	Control_Dispatch Sys_VM_Terminate,   VKD_Sys_VM_Terminate
	Control_Dispatch System_Exit,	     VKD_System_Exit
	Control_Dispatch Sys_Critical_Exit,  VKD_Sys_Crit_Exit
	Control_Dispatch VM_Critical_Init,   VKD_VM_Critical_Init
	Control_Dispatch VM_Suspend,	     VKD_VM_Suspend
	Control_Dispatch VM_Not_Executeable, VKD_VM_Not_Executeable
	Control_Dispatch Destroy_VM,	     VKD_Destroy_VM
	Control_Dispatch Set_Device_Focus,   VKD_Set_Focus
	Control_Dispatch Begin_Message_Mode, VKD_Begin_Msg_Mode
	Control_Dispatch End_Message_Mode,   VKD_End_Msg_Mode
IFDEF Support_Reboot
	Control_Dispatch Reboot_Processor,   VKD_Reboot
ENDIF
IFDEF DEBUG
	Control_Dispatch Debug_Query,	     VKD_Debug_Query
ENDIF

End_Control_Dispatch VKD


;******************************************************************************
;
;   VKD_Disable_Keyboard
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
BeginProc VKD_Disable_Keyboard

	mov	eax, [VKD_irq_Handle]
	VxDCall VPICD_Physically_Mask	    ; mask the IRQ
	VxDCall VPICD_Clear_Int_Request     ; Clear request
	ClrFlag [VKD_flags], VKDf_initialized

	mov	edx, pcmd_Kybd		    ; disable global trapping of
	VMMcall Disable_Global_Trapping     ; 60h & 64h
	mov	edx, pdata_Kybd
	VMMcall Disable_Global_Trapping
	call	VKD_Disable_Kbd
	ret

EndProc VKD_Disable_Keyboard


;******************************************************************************
;
;   VKD_Enable_Keyboard
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
BeginProc VKD_Enable_Keyboard

	mov	edx, pcmd_Kybd		    ; enable global trapping of
	VMMcall Enable_Global_Trapping	    ; 60h & 64h
	mov	edx, pdata_Kybd
	VMMcall Enable_Global_Trapping
	SetFlag [VKD_flags], VKDf_initialized	; flag initialization complete
	mov	eax, [VKD_irq_Handle]
	VxDCall VPICD_Physically_Unmask     ; unmask the IRQ
	call	VKD_Enable_Kbd
	ret

EndProc VKD_Enable_Keyboard


;******************************************************************************
;
;   VKD_Sys_VM_Terminate
;
;   DESCRIPTION:    System VM terminted (exiting).  Its instance data is still
;		    mapped in, so read current BIOS shift state and save for
;		    System_Exit
;
;   ENTRY:
;
;   EXIT:	    Carry clear if no error
;
;   USES:
;
;==============================================================================

BeginProc VKD_Sys_VM_Terminate

;
; update the BIOS shift state for exiting
;
	BEGIN_Touch_1st_Meg
	movzx	eax, word ptr ds:[BIOS_Shift_State_Loc] ; read last shift state
	END_Touch_1st_Meg
	mov	[gbl_shift_state], eax		; save for System_Exit
	clc
	ret

EndProc VKD_Sys_VM_Terminate


;******************************************************************************
;
;   VKD_System_Exit
;
;   DESCRIPTION:    We are about to return to real mode.  Original instance
;		    data is now mapped in, so update shift state to last SYS
;		    VM shift state, saved during Sys_VM_Terminate
;
;
;   ENTRY:	    None
;
;   EXIT:	    Carry clear if no error
;
;   USES:	    Nothing
;
;==============================================================================

BeginProc VKD_System_Exit

	mov	eax, [gbl_shift_state]	    ; get last shift state
	BEGIN_Touch_1st_Meg
	mov	ds:[BIOS_Shift_State_Loc], ax ; store last shift state
	END_Touch_1st_Meg

	call	VKD_Disable_Keyboard

	call	VAD_System_Exit

	mov	eax, [VKD_irq_Handle]
	VxDCall VPICD_Force_Default_Behavior; unhook VKD as owner of IRQ

	call	VKD_Enable_Kbd

	CallRet <SHORT VKD_VM_Init>

EndProc VKD_System_Exit


;******************************************************************************
;
;   VKD_Sys_Crit_Exit
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
BeginProc VKD_Sys_Crit_Exit

	mov	al, KBD_Ctl_Ena 	    ; re-enable keyboard
	call	VKD_Send_Cmd
	clc
	ret

EndProc VKD_Sys_Crit_Exit


;******************************************************************************
;
;   VKD_VM_Critical_Init
;
;   DESCRIPTION:    Initializes the virtual state for a new VM.
;
;   ENTRY:	    EBX = Handle of VM being created
;
;   EXIT:	    Carry set if error.
;
;   USES:	    Flags & AX
;
;   ASSUMES:	    VM handle in EBX is valid.
;
;==============================================================================
BeginProc VKD_VM_Critical_Init

	call	VAD_Create_VM			; Initialize the AUX mouse
;
; Didn't change name of VAD_Create_VM, but it really doesn't matter if it
; initializes at create_VM time or at VM_critical_init time.
;

VKD_VM_Critical_Init_wo_VAD:

; initialize the new VM's VKD control block info
; (it starts out initialized to 0)
	add	ebx, [VKD_CB_Offset]

	or	[ebx.kbdState], KBS_critical_init
	mov	al, [gbl_8042_cmd_byte]
	mov	[ebx._8042_cmd_byte], al

	push	ebx
	sub	ebx, [VKD_CB_Offset]
	call	VKD_Convert_BIOS_Shift_State
	pop	ebx
	mov	[ebx.loc_shift_state], ax

	mov	dl, al
	and	dl, SS_Toggle_mask
	mov	[ebx.BIOS_toggle_state], dl

	mov	esi, ebx
	mov	edx, ebx
	mov	ebx, [gbl_shift_state]
	call	Update_Shift_State
	jz	short @F		    ; jump if no update necessary
	call	VKD_VM_Server		    ; call server, to start process of
					    ; sending update codes from outbuf
@@:
	clc
	ret

EndProc VKD_VM_Critical_Init


;******************************************************************************
;
;   VKD_VM_Init
;
;   DESCRIPTION:    Clear the VM's keyboard buffer by executing a DOSCALL 0Ch
;
;   ENTRY:	    EBX = VM handle
;		    EBP -> Client frame
;
;   EXIT:	    Carry clear
;
;   USES:	    Flags
;
;==============================================================================
BeginProc VKD_VM_Init

	pushad
	mov	eax, 0C06h
	mov	dl, 0FFh
	VxDint	21h
	popad
	clc
	ret

EndProc VKD_VM_Init


;******************************************************************************
;
;   VKD_VM_Suspend
;
;   DESCRIPTION:    Call VKD_Chk_BIOS_Toggle_State to merge the VM's BIOS toggle
;		    key state into the VM's local shift state and the global
;		    shift state, if the VM is the keyboard owner.
;
;   ENTRY:	    EBX = VM Handle of VM being suspended.
;
;   EXIT:	    none
;
;   USES:	    EAX, Flags
;
;==============================================================================
BeginProc VKD_VM_Suspend

	cmp	ebx, [VKD_Kbd_Owner]	    ;Q: VM currently owns keyboard?
	jne	short @F		    ;	N: skip
	call	VKD_Chk_BIOS_Toggle_State
@@:
	clc
	ret

EndProc VKD_VM_Suspend


;******************************************************************************
;
;   VKD_VM_Not_Executeable
;
;   DESCRIPTION:    Cancel any in progress paste
;
;   ENTRY:	    EBX = Handle of VM being created
;
;   EXIT:	    Carry set if error.
;
;   USES:	    EAX, Flags
;
;==============================================================================
BeginProc VKD_VM_Not_Executeable

	mov	eax, Paste_VM_Term
	call	VKD_End_Paste
	clc
	ret

EndProc VKD_VM_Not_Executeable


;******************************************************************************
;
;   VKD_Destroy_VM
;
;   DESCRIPTION:
;
;   ENTRY:	    EBX = Handle of VM being destroyed
;
;   EXIT:	    Carry set if error.
;
;   USES:	    Nothing
;
;==============================================================================
BeginProc VKD_Destroy_VM

;
; call VAD to handle destroy call
;
	call	VAD_Destroy_VM

	cmp	ebx, [VKD_Kbd_Owner]	;Q: Killing keyboard owner?
	jne	SHORT VKD_DV_Exit	;    N: Good!

	Trace_Out "Destroying focus VM..."
	push	ebx
	VMMcall Get_Sys_VM_Handle
	mov	eax, Set_Device_Focus ;   Y: set focus with new owner
	xor	edx, edx		; ALL DEVICES!
	VMMcall System_Control
	pop	ebx

VKD_DV_Exit:
	mov	eax, [VKD_CB_Offset]
	mov	esi, [eax+ebx.eventHandle]
	VMMCall Cancel_Global_Event

	clc
	ret

EndProc VKD_Destroy_VM


;******************************************************************************
;
;   VKD_Set_Focus
;
;   DESCRIPTION:
;
;   ENTRY:	    EBX = Handle of VM to receive the keyboard focus
;		    EDX = Device ID or 0 if critical device focus call
;
;   EXIT:
;
;   USES:
;
;==============================================================================
BeginProc VKD_Set_Focus

	Assert_VM_Handle ebx
	call	VAD_Set_Focus		    ; tell PS/2 mouse about it

	test	edx, edx		    ; if critical then do it
	jz	short focus_critical
	cmp	edx, VKD_Device_ID	    ; if not for VKD then ignore
	jne	short focus_exit

focus_critical:
	cmp	ebx, [VKD_Kbd_Owner]	    ;Q: focus actually changing?
	je	short focus_exit	    ;	N:

;;	  Queue_Out 'vkd set focus #ebx'
	call	VKD_Enable_Kbd		    ; ensure that keyboard is enabled

	push	ebx
	mov	ebx, [VKD_Kbd_Owner]
	call	VKD_Chk_BIOS_Toggle_State
	pop	ebx

	mov	[VKD_Kbd_Owner], ebx	    ; new keyboard owner

	mov	edx, [VKD_CB_Offset]	    ; the VM's control block
	add	edx, ebx
	test	[edx.kbdState], KBS_critical_init
	jz	short focus_exit	    ; skip shift state updates until
					    ;	after VM_Critical_Init
	mov	esi, edx

	movzx	eax, [edx.loc_shift_state]
	mov	ebx, [gbl_shift_state]
	call	Update_Shift_State
	jz	short sf_chk_buf	    ; jump if no update necessary
sf_kick_server:
	call	VKD_VM_Server		    ; call server, to start process of
					    ; sending update codes from outbuf
sf_skip_server:

focus_exit:
	clc
	ret

sf_chk_buf:
	push	eax
	mov	al, [edx.out_head]
	cmp	al, [edx.out_tail]	    ;Q: buffer empty?
	pop	eax
	je	sf_skip_server		    ;	Y: server doesn't need to be called
	jmp	sf_kick_server		    ;	N: kick server to simulate

EndProc VKD_Set_Focus


;******************************************************************************
;
;   VKD_Begin_Msg_Mode
;
;   DESCRIPTION:    Put VM in special message mode, where keys are going to
;		    be grabbed in VMM mode thru calls to VKD_Get_Msg_Key and
;		    VKD_Peek_Msg_Key.  No scan codes will be simulated into
;		    the VM.
;
;   ENTRY:	    EBX = VM handle
;
;   EXIT:
;
;   USES:	    EAX, ESI, Flags
;
;==============================================================================
BeginProc VKD_Begin_Msg_Mode

	Assert_VM_Handle ebx
	mov	esi, [VKD_CB_Offset]
	call	VKD_Enable_Kbd
	SetFlag [ebx+esi.kbdState], KBS_msg_mode
	mov	[msg_key_buf_owner], ebx
	xor	eax, eax
	mov	[msg_key_head], 0
	mov	[msg_key_tail], 0
	mov	[last_msg_key], 0
	clc
	ret

EndProc VKD_Begin_Msg_Mode


;******************************************************************************
;
;   VKD_End_Msg_Mode
;
;   DESCRIPTION:    End the special message mode.
;
;   ENTRY:	    EBX = VM handle
;
;   EXIT:
;
;   USES:
;
;==============================================================================
BeginProc VKD_End_Msg_Mode

	Assert_VM_Handle ebx
	mov	esi, [VKD_CB_Offset]
	ClrFlag [ebx+esi.kbdState], KBS_msg_mode
	cmp	ebx, [msg_key_buf_owner]
	jne	short end_msg_exit
	mov	[msg_key_buf_owner], 0
end_msg_exit:
	clc
	ret

EndProc VKD_End_Msg_Mode


BeginDoc
;******************************************************************************
;
;   VKD_Get_Version
;
;   DESCRIPTION:    Get VKD version number
;
;   ENTRY:	    nothing
;
;   EXIT:	    AH = major, AL = minor
;
;   USES:	    EAX, flags
;
;==============================================================================
EndDoc
BeginProc VKD_Get_Version, SERVICE

	mov	eax, VKD_Version
	clc
	ret

EndProc VKD_Get_Version


BeginDoc
;******************************************************************************
;
;   VKD_Define_Hot_Key
;
;   DESCRIPTION:    Define a hot key notification routine.  Hot keys are
;		    detected by ANDing the shift state mask with the global
;		    shift state, then comparing the resulting state with the
;		    shift state compare value, if this matches, and the key
;		    code matches, then the call-back routine is called with
;		    the specified reference data in EDX.
;
;   ENTRY:	    AL = scan code of the main key
;		    AH = 0, if normal code
;		    AH = 1, if extended code   (ExtendedKey_B)
;		    AH = 0FFh, if either       (AllowExtended_B)
;		    EBX = shift state
;			  high word is mask that is ANDed with the global
;			    shift state when checking for this hot key
;			  low word is masked shift state compare value
;		    CL	= flags
;			    CallOnPress    - Call call-back when key press
;					     is detected
;			    CallOnRelease  - Call call-back when key release
;					     is detected  (keyboard may still
;					     be in hot-key hold state)
;			    CallOnRepeat   - Call call-back when repeated
;					     press is detected
;			    CallOnComplete - Call call-back when the hot key
;					     state is ended (all shift modifier
;					     keys are released) or when a
;					     different hot key is entered
;					     (i.e. pressing ALT 1 2, if both
;					     ALT-1 and ALT-2 are defined hot
;					     keys, then ALT-1's call back will
;					     be called before ALT-2's to
;					     indicate that the ALT-1 is complete
;					     even though the ALT key is still
;					     down)
;			    CallOnUpDwn    - Call on both press and release
;			    CallOnAll	   - Call on press, release and repeats
;			    PriorityNotify - used with one of the call options
;					     to specify that the call back can
;					     only be called when interrupts are
;					     enabled and the critical section
;					     is un-owned
;			    Local_Key	   - key can be locally enabled/disabled
;
;		    ESI = offset of call-back routine
;		    EDX = reference data
;		    EDI = maximum notification delay if PriorityNotify is set,
;			  0, means always notify  (milliseconds)
;
;   EXIT:	    If Carry clear then
;		      EAX = definition handle
;		    else the definition failed	(no more room)
;
;   USES:	    flags
;
;   CALL BACK:	    Called when hot key is detected, and detection meets
;		    mask requirements.	(CallOnPress, CallOnRelease,
;		    CallOnRepeat, CallOnUpDwn, or CallOnAll)
;		    AL = scan code of key
;		    AH = 0, if key just pressed 	    (Hot_Key_Pressed)
;		       = 1, if key just released	    (Hot_Key_Released)
;		       = 2, if key is an auto-repeat press  (Hot_Key_Repeated)
;		       = 3, hot key state ended 	    (Hot_Key_Completed)
;		    (The high bit of AH is set if the hot key is a priority
;		     hot key and the VM which was the keyboard focus at the
;		     time the hot key was recognized, was suspended or not
;		     executable - in which case the priority event was
;		     scheduled for the SYS VM rather that then keyboard owner.
;		     Hot_Key_SysVM_Notify is the defined equate for this bit)
;
;		    EBX is hot key handle
;		    ECX = global shift state
;		    EDX is reference data
;		    EDI = elapsed time for delayed notification  (milliseconds)
;			    (normally 0, but if PriorityNotify is specified
;			     then this value could be larger)
;		    Procedure can modify EAX, EBX, ECX, EDX, ESI, EDI, and Flags
;
;==============================================================================
EndDoc

BeginProc VKD_Define_Hot_Key, SERVICE

	push	ecx
	push	edi

;
; create new node and add it to front of the list
;
	mov	edi, Not_Local_Key
	test	cl, Local_Key		    ;Q: key can be disabled?
	jz	short hkd_1		    ;	N:
	and	cl, NOT Local_Key
	bsf	edi, [VKD_locals_defined]   ; find first available free bit
	jz	short hkd_0		    ; jmp if none found
	btr	[VKD_locals_defined], edi   ; remove the bit from available set
	jmp	short hkd_1		    ; save bit # in hot key rec
hkd_0:
	Debug_Out 'VKD:  too many hot keys that can be locally disabled'
	stc
	jmp	short hkd_exit
hkd_1:
	push	edi			    ; push local mask
	push	esi			    ; push call back offset


	mov	edi, eax		    ; save scan code
	mov	esi, [Hot_Key_List]
	VMMCall List_Allocate		    ; EAX = new hot key node
	VMMCall List_Attach
	xchg	eax, edi		    ; now EDI

IFDEF DEBUG
	mov	[edi.Hot_Key_Self_ptr], edi
ENDIF
	mov	[edi.Hot_Key_SS_Compare], bx
	ror	ebx, 16
	mov	[edi.Hot_Key_SS_Mask], bx
	ror	ebx, 16
	mov	[edi.Hot_Key_scan_code], al
	mov	[edi.Hot_Key_last_VK_fl], 0
	or	ah, ah			    ;Q: extended?
	jz	short norm_key		    ;	N:
	cmp	ah, AllowExtended_B	    ;Q: either normal or extended?
	je	short norm_key		    ;	N:
	mov	ah, VK_Extended_B
	or	[edi.Hot_Key_last_VK_fl], VK_Extended_B
norm_key:
	mov	[edi.Hot_Key_extended], ah
					    ;Q: any bits in mask?
	test	cl, CallOnPress + CallOnRelease + CallOnRepeat + CallOnComplete
	jnz	short mask_set		    ;	Y:
					    ;	N: set defaults
	or	cl, CallOnPress + CallOnRelease + CallOnRepeat + CallOnComplete
mask_set:
	mov	[edi.Hot_Key_call_mask], cl
	pop	[edi.Hot_Key_call_back]     ; pop & store call back function
	mov	[edi.Hot_Key_ref_data], edx
	pop	[edi.Hot_Key_Local_Flag]
	pop	eax			    ; pop max delay time
	push	eax			    ; save again for POP EDI below
	mov	[edi.Hot_Key_max_elapsed], eax

	mov	eax, edi
	clc				    ; successful

hkd_exit:
	pop	edi
	pop	ecx
	ret

EndProc VKD_Define_Hot_Key


BeginDoc
;******************************************************************************
;
;   VKD_Remove_Hot_Key
;
;   DESCRIPTION:    Remove a defined hot key
;
;   ENTRY:	    EAX is hot key definition handle to be removed
;
;   EXIT:	    nothing
;
;   USES:	    flags
;
;==============================================================================
EndDoc

BeginProc VKD_Remove_Hot_Key, SERVICE

	pushad
IFDEF DEBUG
	cmp	[eax.Hot_Key_Self_ptr], eax
	je	short rhk_hk_ok
	Debug_Out 'VKD: invalid hot key handle'
	jmp	short rhk_exit
rhk_hk_ok:
ENDIF

	cmp	[eax.Hot_Key_Local_Flag], Not_Local_Key ;Q: local hot key
	je	short rhk_not_local	    ;	N:
	mov	ecx, [eax.Hot_Key_Local_Flag]
	bts	[VKD_locals_defined], ecx   ; make available again

;
; loop through all current VMs to clear this hot key bit
;
	mov	esi, [VKD_CB_Offset]
	VMMcall Get_Cur_VM_Handle
rhk_loop:
	btr	[ebx+esi.disabled_hot_keys], ecx
	VMMcall Get_Next_VM_Handle
	VMMcall Test_Cur_VM_Handle
	jne	rhk_loop

rhk_not_local:
	mov	esi, [Hot_Key_List]
	VMMCall List_Remove
	VMMCall List_Deallocate
rhk_exit:
	popad
	ret

EndProc VKD_Remove_Hot_Key


BeginDoc
;******************************************************************************
;
;   VKD_Local_Enable_Hot_Key
;
;   DESCRIPTION:    Enable a hot key in the specified VM
;
;   ENTRY:	    EAX is hot key handle
;		    EBX is VM handle
;
;   EXIT:	    nothing
;
;   USES:	    flags
;
;==============================================================================
EndDoc

BeginProc VKD_Local_Enable_Hot_Key, SERVICE

	push	ebx
	push	ecx
IFDEF DEBUG
	cmp	[eax.Hot_Key_Self_ptr], eax
	je	short ehk_hk_ok
	Debug_Out 'VKD: invalid hot key handle'
	jmp	short loc_enable_exit
ehk_hk_ok:
	Assert_VM_Handle ebx
ENDIF

	mov	ecx, [eax.Hot_Key_Local_Flag]
	cmp	cl, Not_Local_Key	    ;Q: local hot key?
IFDEF DEBUG
	jne	short loc_enable_okay	    ;	Y:
	Debug_Out 'VKD warning:  attempt to enable a non-local hot key'
	jmp	short loc_enable_exit
loc_enable_okay:
ELSE
	je	short loc_enable_exit	    ;	N:
ENDIF
	add	ebx, [VKD_CB_Offset]
	btr	[ebx.disabled_hot_keys], ecx ;test & clear disabled state bit
IFDEF DEBUG
	jc	short loc_enable_okay2
	Debug_Out 'VKD warning:  enabled an already enabled hot key'
loc_enable_okay2:
ENDIF

loc_enable_exit:
	pop	ecx
	pop	ebx
	ret

EndProc VKD_Local_Enable_Hot_Key

BeginDoc
;******************************************************************************
;
;   VKD_Local_Disable_Hot_Key
;
;   DESCRIPTION:    Disable a hot key in the specified VM
;
;   ENTRY:	    EAX is hot key handle
;		    EBX is VM handle
;
;   EXIT:	    nothing
;
;   USES:	    flags
;
;==============================================================================
EndDoc

BeginProc VKD_Local_Disable_Hot_Key, SERVICE

	push	ebx
	push	ecx
IFDEF DEBUG
	cmp	[eax.Hot_Key_Self_ptr], eax
	je	short dhk_hk_ok
	Debug_Out 'VKD: invalid hot key handle'
	jmp	short loc_disable_exit
dhk_hk_ok:
	Assert_VM_Handle ebx
ENDIF

	mov	ecx, [eax.Hot_Key_Local_Flag]
	cmp	cl, Not_Local_Key	    ;Q: local hot key?
IFDEF DEBUG
	jne	short loc_disable_okay	    ;	Y:
	Debug_Out 'VKD warning:  attempt to disable a non-local hot key'
	jmp	short loc_disable_exit
loc_disable_okay:
ELSE
	je	short loc_disable_exit	    ;	N:
ENDIF
	add	ebx, [VKD_CB_Offset]
	bts	[ebx.disabled_hot_keys], ecx ;test & set disabled state bit
IFDEF DEBUG
	jnc	short loc_disable_okay2
	Debug_Out 'VKD warning:  disabled an already disabled hot key'
loc_disable_okay2:
ENDIF

loc_disable_exit:
	pop	ecx
	pop	ebx
	ret

EndProc VKD_Local_Disable_Hot_Key


BeginDoc
;******************************************************************************
;
;   VKD_Reflect_Hot_Key
;
;   DESCRIPTION:    Reflect a hot key into a specified VM and exit hot key
;		    state.  This service is normally called by a hot key
;		    notification call-back routine.  It allows the a call-back
;		    to send the hot key into a VM and pretend that it wasn't
;		    really recognized as a hot key.  VKD will simulate the
;		    required key strokes to get the VM into the state of this
;		    specified shift state, then it will simulate the key
;		    strokes for the hot key itself, and finally simulate
;		    key strokes to get the VM to match the current global
;		    shift state.
;
;   ENTRY:	    EAX is hot key handle
;		    EBX is VM handle
;		    CX is required shift state
;
;   EXIT:	    Hot key has been reflected, and VKD is no longer in
;		    hot key state
;
;   USES:	    flags
;
;==============================================================================
EndDoc
BeginProc VKD_Reflect_Hot_Key, SERVICE

	VK_HK_Queue_Out "VKD_Reflect_Hot_Key handle #EAX to VM #EBX"

IFDEF DEBUG
	cmp	[eax.Hot_Key_Self_ptr], eax
	je	short rfhk_hk_ok
	Debug_Out 'VKD: invalid hot key handle'
	jmp	short rfhk_exit
rfhk_hk_ok:
	Assert_VM_Handle ebx
ENDIF

	pushad
	mov	esi, ebx
	add	esi, [VKD_CB_Offset]
	mov	edx, ebx
	mov	edi, eax
	movzx	eax, [esi.loc_shift_state]
	mov	ebx, ecx
	call	Update_Shift_State
	call	Simulate_VM_INT9	    ; simulate the first byte
	movzx	eax, [edi.Hot_Key_Last_VK]  ; send down transition
	call	Queue_Virtual_Key

	test	[edi.Hot_Key_call_mask], Hot_Key_Down	;Q: up received?
	jz	short key_already_up			;   Y:
	and	[edi.Hot_Key_call_mask], NOT Hot_Key_Down ; N: send up anyway
	mov	[VKD_ignore_key], eax			;      and flag VK to
							;      be ignored
key_already_up:
	or	al, SC_Break				;   Y: send up
	call	Queue_Virtual_Key

	call	Hot_Key_Ended
	popad
rfhk_exit:
	ret

EndProc VKD_Reflect_Hot_Key


BeginDoc
;******************************************************************************
;
;   VKD_Cancel_Hot_Key_State
;
;   DESCRIPTION:    Exit the hot key state
;
;   ENTRY:	    nothing
;
;   EXIT:	    keys will start being passed into the focus VM again
;
;   USES:	    nothing
;
;==============================================================================
EndDoc
BeginProc VKD_Cancel_Hot_Key_State, SERVICE

	VK_HK_Queue_Out "VKD_Cancel_Hot_Key_State"

	pushfd
	pushad
	mov	esi, [VKD_Kbd_Owner]	    ; get keyboard owner
	add	esi, [VKD_CB_Offset]	    ; point to CB data
	call	Hot_Key_Ended
	popad
	popfd
	ret

EndProc VKD_Cancel_Hot_Key_State


BeginDoc
;******************************************************************************
;
;   VKD_Force_Keys
;
;   DESCRIPTION:    Force scan codes into the keyboard buffer that look
;		    exactly like they had been typed on the physical keyboard.
;		    These keys will be processed in the context of the focus
;		    VM.
;
;   ENTRY:	    ESI points to a buffer of scan codes
;		    ECX is # of scan codes in the buffer
;
;   EXIT:	    If the keyboard buffer was overflowed, then
;			Carry set
;			ECX is # of remain scan codes that did not fit
;
;   USES:	    FLAGS
;
;==============================================================================
EndDoc

BeginProc VKD_Force_Keys, SERVICE

	pushad

;;	  trace_out "VKD_Force_Key"

	jecxz	SHORT force_exit

	mov	edx, [VKD_Kbd_Owner]	    ; get keyboard owner
	add	edx, [VKD_CB_Offset]

fk_next:
	cld
	lodsb
	call	VKD_Put_Byte
	jnc	short byte_went_in

	push	esi
	push	eax
	push	ecx
	call	VKD_VM_Server		    ; kick server to see if something
	pop	ecx			    ; can be removed from the buffer
	pop	eax
	pop	esi

	call	VKD_Put_Byte		    ; attempt put again
	jc	short buf_full		    ; failed a 2nd time, so quit

byte_went_in:
	loop	fk_next

	call	VKD_VM_Server		    ; kick server to start processing
					    ; the new input in the buffer
force_exit:
	clc
buf_full:
	popad
	ret

EndProc VKD_Force_Keys


BeginDoc
;******************************************************************************
;
;   VKD_Get_Kbd_Owner
;
;   DESCRIPTION:    Get the VM Handle of the keyboard focus VM
;
;   ENTRY:	    None
;
;   EXIT:	    EBX = VM Handle of keyboard owner
;
;   USES:	    Flags, EBX
;
;==============================================================================
EndDoc

BeginProc VKD_Get_Kbd_Owner, SERVICE, High_Freq

	mov	ebx, [VKD_Kbd_Owner]
	ret

EndProc VKD_Get_Kbd_Owner


BeginDoc
;******************************************************************************
;
;   VKD_Define_Paste_Mode
;
;   DESCRIPTION:    Select VM's paste mode, whether INT 16 pasting can be
;		    attempted or not.  Some apps hook INT 9 and do strange
;		    things which will not allow pasting to be done thru
;		    INT 16h.  Normally VKD can detect this by setting a
;		    timeout to see if any INT 16's are being done by the app,
;		    and if not, then switching to INT 9 paste.	But, some
;		    apps may do some INT 16's, in which case the paste would
;		    be broken, so a PIF bit can be set to indicate that only
;		    INT 9 pasting should be used.
;
;   ENTRY:	    AL = 0  allow INT 16 paste attempts
;		    AL = 1  force INT 9 pasting
;		    EBX = VM handle
;
;   EXIT:	    nothing
;
;   USES:	    flags
;
;==============================================================================
EndDoc
BeginProc VKD_Define_Paste_Mode, SERVICE

	Assert_VM_Handle ebx
	push	ebx
	add	ebx, [VKD_CB_Offset]
	ClrFlag [ebx.kbdState], KBS_NO_16_paste
	or	al, al
	jz	short not_forcing_int9
	SetFlag [ebx.kbdState], KBS_NO_16_paste
not_forcing_int9:
	pop	ebx
	ret

EndProc VKD_Define_Paste_Mode


BeginDoc
;******************************************************************************
;
;   VKD_Start_Paste
;
;   DESCRIPTION:    This service puts a VM into paste mode by simulating
;		    keyboard activity with keystrokes taken from the specified
;		    paste buffer.  Depending on the mode set with the service
;		    VKD_Define_Paste_Mode (default is to try INT 16 pasting),
;		    VKD waits for the VM to poll the keyboard BIOS through its
;		    INT 16 interface.  If the VM does keyboard input through
;		    the BIOS, then VKD will simulate the keyboard input at this
;		    high level (plugging in ASCII codes.)  If the VM fails to
;		    perform any INT 16's within in a timeout period, or the
;		    mode has been set to avoid INT 16 pasting, then VKD will
;		    simulate the necessary hardware interrupts to perform the
;		    pasting.  Hot keys are still processed while pasting is in
;		    progress.
;
;   ENTRY:	    EAX is linear address of paste buffer
;			the paste buffer contains an array of key structures:
;			    OEM_ASCII_value	db  ?
;			    scan_code		db  ?
;			    shift_state 	dw  ?
;				shift state bits are:
;				    0000000000000010b	shift key depressed
;				    0000000000000100b	ctrl key depressed
;			The scan code should be FFh and the shift state FFFFh,
;			if VKD should convert the key to a ALT+numpad sequence.
;			(this information is identical to what is given by
;			 the Window's keyboard routine "OEMKeyScan")
;
;		    EBX is VM handle
;		    ECX is number of paste entries in the paste buffer
;		    ESI is call back address  (can be 0)
;		    EDX is reference data
;
;
;   EXIT:	    Carry clear
;			paste is started
;		    Carry set
;			paste failed, unable to allocate memory for buffer copy
;
;   USES:	    flags
;
;   CALL BACK:	    Called when paste is completed or cancelled
;		    EAX is completion flags
;			    Paste_Complete - paste completed successfully
;			    Paste_Aborted  - paste cancelled by user
;			    Paste_VM_Term  - paste aborted because VM terminated
;		    EBX is VM handle of VM that was receiving the paste
;		    EDX is reference data
;		    Procedure can modify EAX, EBX, ECX, EDX, ESI, EDI, and Flags
;
;==============================================================================
EndDoc
BeginProc VKD_Start_Paste, SERVICE

	Assert_VM_Handle ebx
	pushad
	mov	edi, ebx
	add	edi, [VKD_CB_Offset]
	mov	[edi.paste_callback], esi
	mov	[edi.paste_callback_ref], edx
.errnz (SIZE Paste_Rec) - 4
	shl	ecx, 2			    ; convert entry count to byte length
	push	ecx
	add	ecx, 0FFFh
	shr	ecx, 12 		    ; ecx = # of pages
	VMMCall _PageAllocate <ecx, PG_VM, ebx, 0, 0, 0, 0, PageLocked>
	or	eax, eax		    ;Q: allocation failed?
	jz	short paste_alloc_failed    ;	Y:
	mov	[edi.paste_buffer], eax     ; save allocation handle
	mov	[edi.paste_ptr], edx	    ; start of buffer
	pop	ecx
	mov	eax, edx
	add	eax, ecx		    ; end of buffer
	mov	[edi.paste_end], eax
	xchg	edi, edx		    ; edi is destination for buffer copy
	mov	esi, [esp.Pushad_EAX]	    ; esi points to client's buffer
	cld
	shr	ecx, 2
	rep	movsd			    ; copy client's buffer to
					    ; allocated page memory

	xchg	edi, edx		    ; edi -> CB data
	TestMem [edi.kbdState], KBS_NO_16_paste ;Q: INT 16 paste allowed?
	jnz	short force_int9	    ;	N:

;
; setup for starting INT 16 pasting
;   a timeout is set to revert to INT 9 pasting, if the app doesn't
;   execute any INT 16's
;
	SetFlag [edi.kbdState], KBS_INT16_paste ;Y:

	mov	esi, BIOS_head_loc
	add	esi, [ebx.CB_High_Linear]
	movzx	eax, word ptr [esi]
	cmp	ax, [esi+2]		    ;Q: anything in BIOS buffer?
	je	short BIOS_buf_empty	    ;	N:
	SetFlag [edi.kbdState], KBS_empty_BIOS_buf ;Y: flag, so it will be
						   ;   emptied before the
						   ;   paste begins

BIOS_buf_empty:
	mov	eax, [VKD_PasteTimeout]
	mov	edx, ebx
	mov	esi, OFFSET32 VKD_Abort_INT16_Paste
	VMMCall Set_VM_Time_Out
	mov	[edi.paste_timeout], esi
	jmp	short sp_exit

;
; setup for starting INT 9 pasting
;
force_int9:
	SetFlag [edi.kbdState], KBS_INT9_paste

; set timeout to get server started
;
	mov	eax, [VKD_INT9_Paste_Delay]
	mov	edx, edi		    ; ptr to VKD data in CB is reference data
	mov	esi, OFFSET32 VKD_VM_Server
	VMMCall Set_VM_Time_Out

sp_exit:
	popad
	clc
	ret

paste_alloc_failed:
	pop	ecx
	popad
	stc
	ret

EndProc VKD_Start_Paste


BeginDoc
;******************************************************************************
;
;   VKD_Cancel_Paste
;
;   DESCRIPTION:    Cancel the paste that was started in the VM with
;		    VKD_Start_paste
;
;   ENTRY:	    EBX is VM handle
;
;   EXIT:
;
;   USES:
;
;==============================================================================
EndDoc
BeginProc VKD_Cancel_Paste, SERVICE

	mov	eax, Paste_Aborted
	call	VKD_End_Paste
	ret

EndProc VKD_Cancel_Paste


BeginDoc
;******************************************************************************
;
;   VKD_Get_Msg_Key
;
;   DESCRIPTION:    Return the next available key from the special message
;		    mode input buffer and remove it from the buffer.  If
;		    no key is available, then return with the Z flag set.
;		    (This is not a blocking read!)
;
;   ENTRY:	    EBX = VM handle
;
;   EXIT:	    Z flag clear, if key was read
;		      AL = scan code
;		      AH = modifier flags
;			    MK_Shift	  - a shift key is down
;			    MK_Ctrl	  - a control key is down
;			    MK_Alt	  - an alt key is down
;			    MK_Extended   - the key is an extended key
;		    Z flag set, if no key available
;
;   USES:	    EAX, Flags
;
;==============================================================================
EndDoc
BeginProc VKD_Get_Msg_Key, SERVICE

	Assert_VM_Handle ebx
	cmp	ebx, [msg_key_buf_owner]
	jne	short bad_msg_get
	mov	eax, [msg_key_head]
	cmp	eax, [msg_key_tail]	;Q: buffer empty?
	je	short gmk_empty 	;   Y: exit
	add	eax, 2
	and	eax, Msg_Key_Buf_Size - 1    ; wrap around for circular Q
	mov	[msg_key_head], eax
	movzx	eax, [eax+msg_key_buf]
	or	ebx, ebx		; clear Z
	ret

bad_msg_get:
	Trace_Out 'VKD_Get_Msg_Key called for VM other than msg buf owner  #ebx'
gmk_empty:
	xor	eax, eax		; Z flag set
	ret

EndProc VKD_Get_Msg_Key

BeginDoc
;******************************************************************************
;
;   VKD_Peek_Msg_Key
;
;   DESCRIPTION:    Return the next available key from the special message
;		    mode input buffer without removing it from the buffer.
;		    If no key is available, then return with the Z flag set.
;
;   ENTRY:	    EBX = VM handle
;
;   EXIT:	    Z flag clear, if key available
;		      AL = scan code
;		      AH = modifier flags
;			    MK_Shift	  - a shift key is down
;			    MK_Ctrl	  - a control key is down
;			    MK_Alt	  - an alt key is down
;			    MK_Extended   - the key is an extended key
;		    Z flag set, if no key available
;
;   USES:	    EAX, Flags
;
;==============================================================================
EndDoc
BeginProc VKD_Peek_Msg_Key, SERVICE

	Assert_VM_Handle ebx
	cmp	ebx, [msg_key_buf_owner]
	jne	short bad_msg_peek
	mov	eax, [msg_key_head]
	cmp	eax, [msg_key_tail]	;Q: buffer empty?
	je	short pmk_empty 	;   Y: exit
	add	eax, 2
	and	eax, Msg_Key_Buf_Size - 1    ; wrap around for circular Q
	movzx	eax, [eax+msg_key_buf]
	or	ebx, ebx		; clear Z
	ret

bad_msg_peek:
	Trace_Out 'VKD_Peek_Msg_Key called for VM other than msg buf owner  #ebx'
pmk_empty:
	xor	eax, eax		; Z flag set
	ret

EndProc VKD_Peek_Msg_Key

BeginDoc
;******************************************************************************
;
;   VKD_Flush_Msg_Key_Queue
;
;   DESCRIPTION:    Flush any available keys from the special message mode
;		    input buffer.
;
;   ENTRY:	    EBX = VM handle
;
;   EXIT:	    input buffer has been cleared
;
;   USES:	    flags
;
;==============================================================================
EndDoc
BeginProc VKD_Flush_Msg_Key_Queue, SERVICE

	Assert_VM_Handle ebx
	cmp	ebx, [msg_key_buf_owner]
	jne	short bad_msg_flush
	mov	[msg_key_head], 0
	mov	[msg_key_tail], 0
	mov	[last_msg_key], 0
	ret

bad_msg_flush:
	Trace_Out 'VKD_Flush_Msg_Key_Queue called for VM other than msg buf owner  #ebx'
	ret

EndProc VKD_Flush_Msg_Key_Queue


;------------------------------------------------------------------------------

;******************************************************************************
;
;   VKD_PM_API_Entry
;
;   DESCRIPTION:    Dispatch service calls coming for PM apps.
;
;   ENTRY:	    Client_EAX = function #
;
;   EXIT:
;
;   USES:
;
;==============================================================================
BeginProc VKD_PM_API_Entry

Max_Services equ 2

	mov	eax,[ebp.Client_EAX]
	cmp	eax, Max_Services	    ;Q: valid service request?
	jae	short service_err	    ;	N:

	shl	eax, 2
	jmp	cs:API_Services[eax]

service_err:
	or	[ebp.Client_Flags], CF_Mask
	ret

API_Services	label dword
	dd	OFFSET32 VKD_API_Get_Version	; [0]
	dd	OFFSET32 VKD_API_Force_Key	; [1]

EndProc VKD_PM_API_Entry


BeginDoc
;******************************************************************************
;
;   VKD_API_Get_Version
;
;   DESCRIPTION:    Get version # of VKD device
;
;   ENTRY:	    nothing
;
;   EXIT:	    AH = major, AL = minor
;		    Carry clear
;
;   USES:
;
;==============================================================================
EndDoc
BeginProc VKD_API_Get_Version

	mov	[ebp.Client_EAX], VKD_Version
	ClrFlag [ebp.Client_Flags], CF_Mask
	ret

EndProc VKD_API_Get_Version

BeginDoc
;******************************************************************************
;
;   VKD_API_Force_Key
;
;   DESCRIPTION:    Force a key into a VM as if it was typed on the keyboard.
;		    VKD will scan these forced keys for hot keys, so forcing
;		    VKD hot keys is allowed.
;
;   ENTRY:	    EBX = VM handle  (0 for current focus)
;		    CH = scan code
;		    CL = repeat count (1 or more)
;		    EDX = shift state (-1 means no change)
;
;   EXIT:	    Carry Set, if error
;
;   USES:
;
;   NOTE:	    Currently limited to focus VM, so service will fail if
;		    EBX # 0 or EBX # focus VM handle
;
;==============================================================================
EndDoc
BeginProc VKD_API_Force_Key

	pushad
	ClrFlag [ebp.Client_Flags], CF_Mask ; assume ok
	mov	ebx,[ebp.Client_EBX]
	mov	ecx,[ebp.Client_ECX]
	mov	edx,[ebp.Client_EDX]

	trace_out "VKD_API_Force_Key VM #EBX scan/cnt #CX shift #EDX"

	or	ebx, ebx		;Q: selecting current focus?
	jz	short afk_get_focus	;   Y:

	VMMCall Validate_VM_Handle	; Handle good?
	jc	afk_failx		; No, ignore call
	cmp	ebx, [VKD_Kbd_Owner]
	je	short afk_proceed	;   Y:
	Trace_Out "VKD_API_Force_Key called with VM handle other than current focus"
	jmp	afk_fail

afk_get_focus:
	mov	ebx, [VKD_Kbd_Owner]
afk_proceed:
	mov	esi, ebx
	add	esi, [VKD_CB_Offset]
	cmp	edx, -1 		;Q: shift state change?
	je	short afk_no_ss 	;   N:
	and	edx, SS_Toggle_mask OR SS_Toggle_Dn_mask OR SS_Shift_mask
	mov	eax, [gbl_shift_state]
	push	eax			; save global shift state
	push	[ss_queue_proc]
	mov	[ss_queue_proc], OFFSET32 VKD_Put_Byte
	movzx	eax, [esi.loc_shift_state]
	push	ebx
	mov	ebx, edx
	mov	[esi._8042_last_data], 0 ; make sure last wasn't an ALT
	call	Update_Shift_State
	pop	ebx
	pop	[ss_queue_proc]

	push	ecx
	push	edx
	mov	edx, esi
	call	VKD_VM_Server
	pop	edx
	pop	ecx

afk_no_ss:
	mov	al, ch
	movzx	ecx, cl
afk_lp:
	call	VKD_Put_Byte
	jnc	short afk_ok
	Debug_Out 'VKD: hardware buffer overflow during VKD_API_Force_Key'
	push	edx
	mov	edx, esi
	call	VKD_VM_Server
	pop	edx
afk_ok:
	loop	afk_lp

	cmp	edx, -1 		;Q: shift state change?
	je	short afk_call_server	;   N:

	push	edx
	mov	edx, esi
	call	VKD_VM_Server
	pop	edx
	xchg	ebx, [esp]		; save ebx, ebx = saved global shift state
	movzx	eax, [esi.loc_shift_state]
	push	[ss_queue_proc]
	mov	[ss_queue_proc], OFFSET32 VKD_Put_Byte
	mov	[esi._8042_last_data], 0 ; make sure last wasn't an ALT
	call	Update_Shift_State
	pop	[ss_queue_proc]
	pop	ebx

afk_call_server:
	mov	edx, esi
	call	VKD_VM_Server

	jmp	short afk_exit

afk_failx:
	Trace_Out "VKD_API_Force_Key called with invalid VM handle #EBX"
afk_fail:
	or	[ebp.Client_Flags], CF_Mask
afk_exit:
	popad
	ret

EndProc VKD_API_Force_Key


page
;******************************************************************************
;  VM server routines
;******************************************************************************


;******************************************************************************
;
;   VKD_VM_Server & VKD_VM_Service_Phys
;
;   DESCRIPTION:    VKD_VM_Service_Phys clears the eventHandle flag to allow
;		    the interrupt routine to schedule more event calls to this
;		    server routine then enters VKD_VM_Server
;
;		    VKD_VM_Server attempts to simulate hardware interrupts from
;		    bytes in the output queue maintained for each VM
;		    handles fetching bytes from the input queue and converting
;		    them to virtual keys
;		    handles hot key detection
;		    queues bytes that should be sent to the VM into the VM's
;		    output queue
;
;   ENTRY:	    EDX points to VKD data in VM's CB
;		       (VM handle + VKD_CB_Offset)
;
;   EXIT:	    nothing
;
;   USES:
;
;
;==============================================================================

BeginProc VKD_VM_Service_Phys, High_Freq
	mov	[edx.eventHandle], 0	    ; clear event request flag
EndProc VKD_VM_Service_Phys

BeginProc VKD_VM_Server, High_Freq
	mov	esi, edx
	sub	edx, [VKD_CB_Offset]

server_entry:
	Assert_VM_Handle edx

	TestMem [VKD_flags], VKDf_initialized	;Q: initialization complete?
	jz	chk_msg_mode			;   N:
	TestMem [esi.kbdState], KBS_INT9_paste	;Q: INT 9 pasting?
	jz	short server_not_pasting	;   N: don't queue anything
	call	QueueFromPasteBuf		;   Y: attempt to queue more
						;      scan codes

server_not_pasting:
	call	Simulate_VM_INT9	    ; try to send scan code from outbuf

	cmp	edx, [VKD_Kbd_Owner]	    ;Q: this VM the focus?
	jne	short upper_server_exit     ;	N: don't do anything else

	TestMem [VKD_flags], VKDf_HK_hold   ;Q: holding for hot keys?
	jnz	service_hot_keys	    ;	Y: call hot key service routine

;
; Check to see if there is room for 3 more scan codes to be placed in the VM's
; outbuf, if not, then exit without processing any keys from the physical
; keyboard buffer in an attempt to prevent losing key strokes.
;
	mov	ah, [esi.out_head]
	mov	al, [esi.out_tail]
	cmp	al, ah
	jz	short server_queue_empty
	jb	short @F
	add	ah, VKD_OutBuf_Size
@@:
	add	al, 3
	cmp	al, ah			    ;Q: buffer full?
	jae	short upper_server_exit     ;	Y:

server_queue_empty:
	call	VKD_Get_Key		    ; remove byte from input queue
	jc	short upper_server_exit     ; queue empty!

	mov	edi, eax
	and	edi, NOT SC_Break
	cmp	edi, [VKD_ignore_key]	    ;Q: ignore this key?
	jne	short vk_okay		    ;	N:
	test	al, SC_Break		    ;Q: this the up transition?
	jz	server_not_pasting	    ;	N: just ignore it
	mov	[VKD_ignore_key], 0	    ;	   don't need to ignore anymore
	jmp	server_not_pasting

upper_server_exit:
	ret

vk_okay:
	mov	edi, [gbl_shift_state]
	call	Chk_Shift_State 	    ; virtual key in ax, must be preserved

	call	Chk_Hot_Keys		    ;Q: key ends a hot key def?
	jc	short first_hot_key_found   ;	Y: hot key combination found,
					    ;	   so ebx is key handle

	TestMem [esi.kbdState], KBS_msg_mode ;Q: special message mode?
	jnz	short put_msg_key	    ;	Y:

	TestMem [esi.kbdState], KBS_Pasting ;Q: VM pasting?
	jz	short not_pasting	    ;	N:
	jmp	short VKD_Beep

not_pasting:
	mov	[esi.loc_shift_state], cx   ;	N: store new local shift state

; this scan code is not part of a hot key sequence, so we need to simulate it
; into the VM, we can do this by putting it into outbuf, so it will be sent
; when it is possible
	TestMem [edx.CB_VM_Status], VMStat_Not_Executeable
	jnz	short server_exit
	TestMem [edx.CB_VM_Status], VMStat_Suspended	;Q: VM suspended?
	jnz	short VKD_Beep				;   Y:
	jmp	Queue_Virtual_Key			;   N: queue key

chk_msg_mode:
	call	VKD_Get_Key		    ; remove byte from input queue
	jc	short server_exit	    ; queue empty!
	TestMem [esi.kbdState], KBS_msg_mode ;Q: special message mode?
	jz	short server_exit	    ;	N:
put_msg_key:
	jmp	Queue_Msg_Mode_Key

first_hot_key_found:
; setup for updating the VM's shift state

	push	eax
	push	ebx
	mov	ebx, [gbl_shift_state]
	and	ebx, SS_Toggle_mask	    ; clear all "down" bits
	movzx	eax, [esi.loc_shift_state]
	call	Update_Shift_State
	pop	ebx
	pop	eax
	jmp	short hot_key_found	    ; pass hot key handle to call-back

service_hot_keys:
	call	VKD_Get_Key		    ; remove byte from input queue
	jc	short server_exit	    ; queue empty!

	call	Chk_Shift_State 	    ; scan code in al, must be preserved

	TestMem [gbl_shift_state], <SS_Shift_mask + SS_Toggle_Dn_mask>
					    ;Q: are any shift state keys down?
	jz	short HK_ended		    ;  N: release from hold state

	call	Chk_Hot_Keys
	jc	short hot_key_found

;;	  Trace_Out 'bad hot key #ax'
	call	VKD_Beep
	jmp	short chk_for_more

hot_key_found:				    ; pass hot key handle to call-back
	call	Hot_Key_Entered
chk_for_more:
	call	VKD_Buf_Empty
	jz	short server_exit
	jmp	server_entry

HK_ended:
	call	Hot_Key_Ended
server_exit:
.errnz $-server_exit	; we assume above that server_exit points to a
			; "ret" instruction
	ret

EndProc VKD_VM_Server


;******************************************************************************
;
;   VKD_Beep
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

BeginProc VKD_Beep

	call	IsModifierKey		    ;Q: a modifier key?
	jz	short VKD_Beep_exit	    ;	Y:
	test	al, SC_Break		    ;Q: key released?
	jnz	short VKD_Beep_exit	    ;	Y:
	VxDCall VSD_Bell		    ;	N: sound warning
VKD_Beep_exit:
	ret

EndProc VKD_Beep


;******************************************************************************
;
;   VKD_Get_Key
;
;   DESCRIPTION:    get bytes from the input queue and convert them to virtual
;		    keys
;
;   ENTRY:	    nothing
;
;   EXIT:	    Carry flag set, if queue empty
;		    else EAX is new virtual key (a virtual key has the normal
;		    scan code plus break bit in the low byte and an upper
;		    byte of flags:
;
;		      VK_Extended    - extended key (E0 scan code - as in
;				       Right ALT, Right CTRL, cursor pad keys,
;				       etc.)
;
;   USES:	    EAX and flags
;
;==============================================================================
BeginProc VKD_Get_Key, High_Freq

	bts	[VKD_flags], VKDf_build_guard_bit ;Q: reentering?
	jc	short gk_reentered	    ;	Y:

get_lp:
	call	VKD_Get_Byte		    ; get a scan code from the kb buf
	jc	short gk_no_byte	    ; jump if no code available
	call	Build_Key
	jz	get_lp			    ; not a full key yet, try again
	ClrFlag [VKD_flags], VKDf_build_guard
	VK_HK_Queue_Out 'get_key #ax'

	clc
	ret

gk_no_byte:
	ClrFlag [VKD_flags], VKDf_build_guard
	VK_HK_Queue_Out 'get_key (none)'
gk_reentered:
gk_no_key:
	stc
	ret

EndProc VKD_Get_Key


;******************************************************************************
;
;   Build_Key
;
;   DESCRIPTION:    combine scan codes into a single virtual key
;
;   ENTRY:	    AL is new scan code
;
;   EXIT:	    Z set if scan code didn't complete a virtual key
;		    Z clear if scan code completed a virtual key
;			EAX is the new virtual key
;
;   USES:
;
;   NOTE:   this routine can not be reentered, because it is trying to build
;	    a template virtual key from a sequence of scan code given one at
;	    a time
;
;==============================================================================
BeginProc Build_Key

	push	ebx
	mov	ebx, [VKD_build]
	cmp	al, SC_Ext0		;Q: extended code?
	je	short vkb_set_ext	;   Y: flag extended and return
	TestReg ebx, VK_Extended	;Q: extended vk?
	jz	short vkb_done_1	;   N:

	call	IsShiftKey		;Q: is code a shift key?
	jz	short vkb_ignore	;   Y: ignore pseudo shift sequence

vkb_done_1:
	mov	bl, al			; add code to build temp
	mov	eax, ebx		; return virtual key
	mov	[VKD_build], 0		; clear the build temp for next VK
	or	bl, 1			; clear the Z flag
	pop	ebx
	ret

vkb_ignore:
	xor	ebx, ebx		; reset VKD_build to start over
	jmp	short vkb_not_done_1

vkb_set_ext:
	or	bh, VK_Extended SHR 8
.errnz VK_Extended AND 0FFFF00FFh

vkb_not_done_1:
	mov	[VKD_build], ebx	; save VK status
	xor	ebx, ebx		; set the Z flag
	pop	ebx
	ret

EndProc Build_Key


;******************************************************************************
;
;   Simulate_VM_INT9
;
;   DESCRIPTION:    simulate a hardware interrupt with the next available byte
;		    in the output queue, if one exists
;
;   ENTRY:	    EDX = Handle of VM to service
;		    ESI points to VKD data in VM's control block
;
;   EXIT:
;
;   USES:
;
;   DESIGN:
;	get pic status
;	IF NOT IRQ_In_Service THEN   (* no EOI pending *)
;	  IF port empty THEN
;	    IF byte in queue THEN
;	      move byte from queue into port
;	      update status  (* port not empty *)
;	    ELSE
;	      RETURN	(* no reason to int VM *)
;	    END
;	  END;
;	  IF NOT IRET_pending THEN
;	    IF keyboard enabled THEN
;	      request int
;	    END
;	  END
;	END
;
;==============================================================================
BeginProc Simulate_VM_INT9, High_Freq, PUBLIC

	Assert_VM_Handle edx
	TestMem [esi.kbdState], <KBS_sim_hold OR KBS_sim_alt_hld>
					    ;Q: holding to timeout?
	jnz	s9_skip 		    ;	Y: can't simulate

	push	eax
	push	ebx
	push	ecx

	mov	eax, [VKD_irq_Handle]
	mov	ebx, edx
	VxDCall VPICD_Get_Status

	test	[esi._8042_cmd_byte], fKBC_Int	;Q: 8042 int disabled?
	jz	short dont_poll_EOI	    ;	Y: don't check EOI state

	TestReg ecx, VPICD_Stat_In_Service  ;Q: EOI pending?
	jnz	s9_exit 		    ;	Y: can't send more yet!

dont_poll_EOI:
	test	[esi._8042_status], fKBS_DAV;Q: output buffer full?
	jnz	DEBFAR port_busy	    ;	Y: can't send yet, but see if
					    ;	   we should request the int

	TestMem [esi.kbdState], KBS_8042_hold ;Q: processing special 8042 cmds?
	jnz	s9_exit 		      ;   Y:

	test	[esi._8042_cmd_byte], fKBC_Dis ;Q: keyboard disabled?
	jnz	DEBFAR s9_exit		    ;  Y: don't int, VM must be
					    ;	  polling, or waiting for
					    ;	  another int

; all clear, so get char from outbuf to simulate

	call	Get_8042_Byte		    ;Q: 8042 response byte available?
IF DebugPorts GE 3
	jc	short no_8042_bytes
	Trace_Out '8',\noeol
no_8042_bytes:
ENDIF
	jnc	short skip_hcurtrk	    ;	Y: simulate it!

	movzx	eax, [esi.out_head]
	cmp	al, [esi.out_tail]	    ;Q: buffer empty?
	je	DEBFAR s9_exit		    ;	Y: ret
	inc	eax
	and	eax, VKD_OutBuf_Size - 1    ; wrap around for circular Q
	mov	[esi.out_head], al
	mov	al, [eax+esi.outbuf]	    ; get byte from buffer

	cmp	[VKD_KEY_IDLE_DELAY], 0
	je	short sim_no_key_delay
	push	eax
	VMMCall Get_VM_Exec_Time
	mov	[esi.last_key_time], eax
	pop	eax
sim_no_key_delay:

IFDEF DEBUG
	push	ebx
	movzx	ebx, [esi.sent_ptr]
	inc	ebx
	and	ebx, VKD_SentBuf_Size - 1; wrap around for circular Q
	mov	[esi.sent_ptr], bl
	mov	[ebx+esi.sent_buf], al
	pop	ebx
ENDIF
	xor	ah, ah
	TestMem [esi.kbdState], KBS_INT9_paste	;Q: pasting?
	jnz	short skip_hcurtrk		;   Y: don't track cursor
;
; copy scan code with SC_Break removed into AH
;
	mov	ah, al
	and	ah, NOT SC_Break
;
; don't call VDD to track cursor movement on shift keys
;
	call	IsShiftKey
	je	short skip_hcurtrk
	cmp	ah, SC_Ctrl
	je	short skip_hcurtrk
	cmp	ah, SC_Alt
	je	short skip_hcurtrk
	VxDCall VDD_Set_HCurTrk 	    ; call VDD to track cursor

skip_hcurtrk:
	call	VKD_OutputByte		    ; setup for next port read
IF DebugPorts GE 3
	Trace_Out 'l#al',\noeol
ENDIF
port_busy:
	test	[esi._8042_cmd_byte], fKBC_Int	;Q: 8042 int disabled?
	jz	short s9_exit		    ;	Y: don't simulate int

	TestReg ecx, VPICD_Stat_IRET_Pending;Q: iret pending?
	jnz	short s9_exit		    ;	Y: don't req. another int

; try to simulate an int to the VM

	cmp	ah, SC_Alt
	jne	short sim_not_alt
	SetFlag [esi.kbdState], KBS_sim_alt_hld ; delay next scan code
sim_not_alt:

	mov	ebx, edx		    ; pass current VM handle

	mov	eax, [VKD_irq_Handle]	    ;	N: go ahead with simulation
	VxDCall VPICD_Set_Int_Request	    ; request a VM int
IF DebugPorts GE 3
	Trace_Out 'h',\noeol
ENDIF

s9_exit:
	pop	ecx
	pop	ebx
	pop	eax
s9_skip:
	ret

EndProc Simulate_VM_INT9


;******************************************************************************
;
;   Queue_Output
;
;   DESCRIPTION:    queue a byte in the VM's output queue
;
;   ENTRY:	    AL is scan code to queue
;		    ESI points to VKD data in VM's control block
;
;   EXIT:	    Carry set if outbuf is full
;
;   USES:	    flags
;
;==============================================================================

BeginProc Queue_Output, High_Freq

	push	ebx
	movzx	ebx, [esi.out_tail]
	inc	ebx
	and	ebx, VKD_OutBuf_Size - 1; wrap around for circular Q
	cmp	bl, [esi.out_head]	;Q: buffer full?
	je	short outbuf_full	;   Y:
	mov	[esi.out_tail], bl	;   N: update tail index & store code
	mov	[ebx+esi.outbuf], al

	clc
	jmp	short qo_exit

outbuf_full:
	stc				; error!
qo_exit:
	pop	ebx
	ret

EndProc Queue_Output

;******************************************************************************
;
;   Queue_Ext_Shift
;
;   DESCRIPTION:    Queue shift transition bytes in the VM's output queue, if
;		    they are part of the virtual key.  Shift transitions are
;		    a combination of 0E0h followed by a shift scan code
;
;		    i.e.  E0 2A or E0 B6
;
;   ENTRY:	    AX is virtual key
;		    ESI points to VKD data in VM's control block
;
;   EXIT:	    Z flag set, if none queued
;
;   USES:	    flags
;
;==============================================================================
BeginProc Queue_Ext_Shift, High_Freq

	push	eax
	TestMem [esi.kbdState], <KBS_pseudo_left OR KBS_pseudo_right>
					    ;Q: end pseudo shift pending?
	je	short qes_no_ps_clr	    ;	N:
	cmp	eax, [esi.key_queued_ext_shft] ;Y: Q: same virtual key?
	je	short qes_exit		    ;	      Y: don't need another
	btr	al, bSC_Break		    ;Q: up transition?
	jnc	short queue_end_pseudo	    ;	N: a new key was pressed, so
					    ;	    send the pseudo shift
	cmp	eax, [esi.key_queued_ext_shft] ;Y: Q: same virtual key?
	je	short queue_end_pseudo	    ;	    Y: this is the up transition
					    ;	       of the key that is pending
					    ;	       a pseudo shift, so send it
	cmp	al, SC_LShf		    ;Q: left shift release?
	jne	short @F		    ;	N:
	ClrFlag [esi.kbdState], KBS_pseudo_left ; clear if part of pending end
	jmp	short qes_exit
@@:
	cmp	al, SC_RShf		    ;Q: right shift release?
	jne	short qes_exit		    ;	N:
	ClrFlag [esi.kbdState], KBS_pseudo_right ; clear if part of pending end
	jmp	short qes_exit

qes_no_shift:
	pop	edx
	pop	ecx
qes_exit:
	xor	al, al			    ; set Z flag
	pop	eax
	ret

;
; end the pseudo shift state
;
queue_end_pseudo:
	mov	ah, SC_RShf
	btr	[esi.kbdState], KBS_pseudo_right_bit
	call	queue_pseudo_shift
	mov	ah, SC_LShf
	btr	[esi.kbdState], KBS_pseudo_left_bit
	call	queue_pseudo_shift
	mov	eax, [esp]

qes_no_ps_clr:
	test	al, SC_Break		    ;Q: up transition?
	jnz	short qes_exit		    ;	Y: nothing else to do

	TestReg eax, VK_Extended	    ;Q: extended key?
	jz	short qes_exit		    ;	N:

	movzx	eax, al
	cmp	al, HighKeyOfSet	    ;Q: is key one with possible shift trans
	ja	short qes_exit		    ;	N:
	sub	al, LowKeyOfSet
	jl	short qes_exit		    ;	N:
	bt	[VKD_LongKeySet], eax	    ;Q: one of the special keys?
	jnc	short qes_exit		    ;	N:

	push	ecx
	push	edx
	movzx	ecx, [esi.loc_shift_state]
	and	ecx, SS_NumLock OR SS_Shift OR SS_Ctrl OR SS_Alt

	xor	eax, eax		    ; assume not breaking key
	mov	edx, [esp+8]
	cmp	dl, SC_PrtScr		    ;Q: print screen?
	je	short qes_check_shift	    ;	Y: look for shift up
	or	al, SS_Shift
	cmp	dl, SC_NumSlash 	    ;Q: numpad '/'?
	je	short qes_check_shift	    ;	Y: look for shift down

	TestReg ecx, SS_NumLock 	    ;Q: num lock on?
	jz	short qes_check_shift	    ;	N: look for shift down
	xor	al, al			    ;	Y: look for shift up

qes_check_shift:
	and	cl, NOT SS_NumLock
.erre SS_NumLock AND 0FFh
	cmp	cx, ax			    ;Q: required shift state?
	jne	short qes_no_shift	    ;	N:

	ClrFlag [esi.kbdState], KBS_need_shift_up
	mov	ah, SC_LShf		    ; assume left shift down
	mov	edx, KBS_pseudo_left_bit
	TestReg ecx, SS_Shift		    ;Q: shift down?
	jz	short qes_shift 	    ;	N: send left down
	movzx	ecx, [esi.loc_shift_state]
	SetFlag [esi.kbdState], KBS_need_shift_up
	TestReg ecx, SS_LShift		    ;Q: left shift?
	jz	short @F		    ;	N:
	TestReg ecx, SS_RShift		    ;Q: right shift down also?
	jz	short qes_shift 	    ;	N: just send left up
	bts	[esi.kbdState], edx	    ;	Y: send left up first
	stc
	call	queue_pseudo_shift
@@:
	mov	ah, SC_RShf		    ; send right up transition
	mov	edx, KBS_pseudo_right_bit

qes_shift:
	bts	[esi.kbdState], edx
	stc
	call	queue_pseudo_shift
	btc	[esi.kbdState], KBS_need_shift_up_bit
	or	al, 1			    ; force Z-flag clear for return
	pop	edx
	pop	ecx
	pop	eax			    ; the new virtual key
	mov	[esi.key_queued_ext_shft], eax
	ret

; small sub called to actually queue a pseudo shift key sequence
;   ENTER: AH = scan code of shift key to queue
;	   Carry flag set, if key should be queued
queue_pseudo_shift:
	jnc	short qps_exit
	mov	al, SC_Ext0
	call	Queue_Output
	mov	al, ah
	TestMem [esi.kbdState], KBS_need_shift_up
	jz	short @F
	or	al, SC_Break
@@:
	call	Queue_Output
qps_exit:
	ret

EndProc Queue_Ext_Shift

;******************************************************************************
;
;   Queue_Virtual_Key
;
;   DESCRIPTION:    Queue scan codes that create a virtual key to be simulated
;		    into a VM
;
;   ENTRY:	    EAX = virtual key
;		    EDX = VM handle
;		    ESI points to VKD data in VM's control block
;
;   EXIT:	    nothing
;
;   USES:	    Flags
;
;==============================================================================

BeginProc Queue_Virtual_Key

	test	al, SC_Break		    ;Q: breaking key?
	jnz	short key_break 	    ;	Y:
	call	Queue_Ext_Shift 	    ;	N: queue extended codes if necessary
	jnz	short queue_ext 	    ;	   jump if codes sent, because
					    ;	   we know that the key is
					    ;	   definitely extended
key_break:
	TestReg eax, VK_Extended	    ;Q: extended key?
	jz	short no_ext		    ;	N:
queue_ext:
	push	eax			    ;	Y: queue it
	mov	al, SC_Ext0
	call	Queue_Output
	pop	eax
no_ext:
	call	Queue_Output		    ; queue the real code
	test	al, SC_Break		    ;Q: breaking key?
	jz	short key_queued	    ;	N:
	call	Queue_Ext_Shift 	    ;	Y: queue extended codes if necessary
key_queued:
	mov	[esi.last_queued], eax	    ; record virtual key
	call	Simulate_VM_INT9	    ; simulate the first byte
	ret

EndProc Queue_Virtual_Key


;******************************************************************************
;
;   Queue_Msg_Mode_Key
;
;   DESCRIPTION:
;
;   ENTRY:	    EAX = virtual key
;		    EDX = VM handle
;		    ESI points to VKD data in VM's control block
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc Queue_Msg_Mode_Key

	cmp	edx, [msg_key_buf_owner] ;Q: current msg buf owner?
	jne	short msg_buf_error	;   N:

	push	eax
	push	ebx

	call	IsModifierKey		;Q: a modifier key?
	jz	short msg_buf_exit	;   Y: don't queue

	test	al, SC_Break		;Q: up transition?
	jnz	short msg_buf_set_last	;   Y: don't queue

	cmp	al, [last_msg_key]	;Q: repeat?
	je	short msg_buf_exit	;   Y: don't queue

	mov	ebx, [gbl_shift_state]
	or	bl, bh
	and	bl, MK_Shift + MK_Ctrl + MK_Alt
.errnz VK_Extended_B - MK_Extended
	and	ah, MK_Extended
	or	ah, bl			; al = scan code, ah = modifier

	mov	ebx, [msg_key_tail]
	add	ebx, 2
	and	ebx, Msg_Key_Buf_Size - 1 ; wrap around for circular Q
	cmp	ebx, [msg_key_head]	;Q: buffer full?
	je	short msg_buf_full	;   Y: fatal error???
	mov	[msg_key_tail], ebx	;   N: update tail index & store code
	mov	[ebx+msg_key_buf], ax

;;	  Trace_Out "Msg_Key available: #ax"

msg_buf_set_last:
	mov	[last_msg_key], al
	jmp	short msg_buf_exit

msg_buf_full:
	VxDCall VSD_Bell		; sound warning

msg_buf_exit:
	pop	ebx
	pop	eax
	ret

msg_buf_error:
	Debug_Out "cur VM in msg mode, but doesn't own buffer"
	ret

EndProc Queue_Msg_Mode_Key


;******************************************************************************
;
;   QueueFromPasteBuf
;
;   DESCRIPTION:    Check to see if INT 9 pasting is in progress, if so then
;		    convert a paste entry into necessary scan codes and
;		    place in the output buffer.
;
;   ENTRY:	    EDX is VM handle
;		    ESI points to VKD data in VM's CB
;
;   EXIT:
;
;   USES:	    EAX, EBX, ECX, EDI, flags
;
;==============================================================================

Queue_Stroke MACRO scan_code
IFNB <scan_code>
	mov	al, scan_code
ENDIF
	call	Queue_Output
	or	al, SC_Break
	call	Queue_Output
	ENDM

BeginProc QueueFromPasteBuf

	TestMem [esi.kbdState], KBS_queue_hold	;Q: holding?
	jnz	DEBFAR qp_exit			;   Y: don't queue anything new!

	mov	al, [esi.out_tail]
	cmp	al, [esi.out_head]	    ;Q: output buffer empty?
	jne	short qp_exit		    ;	N: don't queue anything new!

	mov	edi, BIOS_head_loc
	add	edi, [edx.CB_High_Linear]
	movzx	eax, word ptr [edi]	    ; get head ptr
	movzx	ebx, word ptr [edi+2]	    ; get tail ptr
.errnz BIOS_tail_loc-BIOS_head_loc-2
	cmp	ebx, eax
	je	short qp_bios_buf_ok	    ; jump if buffer empty
	ja	short tail_gtr		    ; jump if tail greater than head
	mov	edi, BIOS_buf_start	    ; adjust tail so that it is the
	add	edi, [edx.CB_High_Linear]
	sub	bx, [edi]		    ; same amount beyond buf_end as
	add	bx, [edi+2]		    ; it currently is beyond buf_start
					    ; (flatten it)
.errnz BIOS_buf_end - BIOS_buf_start - 2
tail_gtr:
	sub	ebx, eax
	shr	ebx, 1			    ; # of entries between head & tail
	cmp	ebx, 8			    ;Q: half or less filled?
	jbe	short qp_bios_buf_ok	    ;	Y:

	Trace_Out 'BIOS buffer half full - delaying paste'
	push	edx			    ;	N: delay to give app time to
	push	esi			    ;	   catch up
	mov	eax, [VKD_INT9_buf_full_Delay]	 ; set timeout to kick server
	mov	ebx, edx		    ;	   to get started again
	mov	edx, esi
	mov	esi, OFFSET32 VKD_Paste_Restart
	VMMCall Set_VM_Time_Out
	pop	esi
	pop	edx
	SetFlag [esi.kbdState], KBS_queue_hold
	jmp	short qp_exit

qp_bios_buf_ok:
	mov	edi, [esi.paste_ptr]
	cmp	edi, [esi.paste_end]	    ;Q: paste buffer empty?
	jb	short qp_paste_char_avail   ;	N:

	movzx	eax, [esi.loc_shift_state]
	mov	ebx, [gbl_shift_state]
	call	Update_Shift_State

	mov	eax, Paste_Complete
	mov	ebx, edx
	call	VKD_End_Paste
qp_exit:
	ret

qp_paste_char_avail:
	mov	eax, edi
	add	eax, SIZE Paste_Rec	    ; update paste buffer ptr
	mov	[esi.paste_ptr], eax

	movzx	eax, [esi.loc_shift_state]
	movzx	ebx, [edi.Paste_ShiftState]

	cmp	ebx, 0FFFFh		    ;Q: shift state valid?
	jne	short qp_shift_ok	    ;	Y:
	mov	ebx, 0			    ;	N: zero it!
qp_shift_ok:
	pushfd
	and	ebx, BSS_LShift + BSS_Ctrl  ; eliminate ignored bits
	call	Update_Shift_State

	popfd
	je	short qp_alt_seq	    ; jump if shift state = 0FFFFh

	Queue_Stroke [edi.Paste_ScanCode]
	jmp	short qp_exit

;
; do ALT+numpad sequence
;
qp_alt_seq:
	mov	al, SC_Alt
	call	Queue_Output
	mov	al, [edi.Paste_Char]
	aam
	mov	cl, al			    ; al = least significant digit
	mov	al, ah
	aam				    ; ah = most & al = middle
	mov	ch, al
	movzx	ebx, ah
	Queue_Stroke [ebx+NumPad_SCs]
	movzx	ebx, ch
	Queue_Stroke [ebx+NumPad_SCs]
	movzx	ebx, cl
	Queue_Stroke [ebx+NumPad_SCs]
	mov	al, SC_Alt + SC_Break
	call	Queue_Output
	jmp	qp_exit


EndProc QueueFromPasteBuf


;******************************************************************************
;
;   VKD_Paste_Restart
;
;   DESCRIPTION:    VKD_Paste_Restart clears the queue hold flag in kbdState
;		    and enters VKD_VM_Server, it is called as a result of
;		    a timeout set during INT 9 pasting.
;
;   ENTRY:	    EDX -> VKD data in VM's CB  (VM handle + VKD_CB_Offset)
;
;   EXIT:	    nothing
;
;   USES:	    flags
;
;==============================================================================
BeginProc VKD_Paste_Restart

	ClrFlag [edx.kbdState], KBS_queue_hold
	CallRet VKD_VM_Server

EndProc VKD_Paste_Restart


PAGE
;******************************************************************************
;
;   VKD_send_ralt_pre
;
;   DESCRIPTION:    IF Right-Alt, then set the extended key prefix
;
;   ENTRY:	    AL = scan code for alt stroke  (SC_Break may be set)
;		    BX = global shift state
;		    DX = local shift state
;
;   EXIT:	    Carry set, if queueing failed
;
;   USES:	    AH, Flags
;
;==============================================================================
BeginProc VKD_send_ralt_pre

	push	ebx
	or	ebx, edx
	TestReg ebx, SS_RAlt	;Q: right alt?
	pop	ebx
	clc
	jz	short ralt_exit ;   N:
	mov	ah, al
	mov	al, SC_Ext0	; queue extended key prefix
	call	[ss_queue_proc]
	mov	al, ah
ralt_exit:
	ret

EndProc VKD_send_ralt_pre


;******************************************************************************
;
;   Update_Shift_State
;
;   DESCRIPTION:    Fill outbuf with scan codes necessary to get local
;		    shift state to equal global shift state
;
;   ENTRY:	    AX is local shift state
;		    BX is global shift state
;		    ESI points to current VM's VKD control block info
;
;
;   EXIT:	    VM's loc_shift_state field is updated
;		    Z flag set, if no update necessary
;		    Carry flag set, if output buffer overflowed
;
;   USES:
;
;==============================================================================
BeginProc Update_Shift_State

	push	ecx
	push	edi
	push	edx

	mov	edx, eax		; save local state
	mov	edi, ebx		; save global state
	mov	ecx, (NOT (SS_Shift + SS_Ctrl + SS_Alt)) AND 0FFFFh
	and	edx, ecx		; remove the "either" bits
	and	edi, ecx
	cmp	edi, edx		;Q: any differences
	je	short uss_exit		;   N: return Z flag set & Carry clear

	mov	ecx, (NOT (SS_Toggle_Mask OR SS_LAlt OR SS_RAlt)) AND 0FFFFh
	and	eax, ecx		; isolate SS_Alt bit and other down bits
	cmp	eax, SS_Alt		;Q: alt down in local state?
	jne	short update_toggles	;   N:
	and	ecx, ebx		;Q: everything up in global state?
	jnz	short update_toggles	;   N:

	mov	al, [esi.last_queued_sc] ; get last byte queued
	and	al, NOT SC_Break
	cmp	al, SC_Alt		;Q: last key an ALT?
	jne	short update_toggles	;   N:

	or	al, SC_Break
	call	VKD_send_ralt_pre
	jc	short update_toggles
	call	[ss_queue_proc] 	; send up stroke of alt
	jc	short uss_exit

	xor	al, SC_Break
	call	[ss_queue_proc] 	; send down stroke of left alt
	jc	short uss_exit
;;	  call	  [ss_queue_proc]	  ; send 2nd down stroke of left alt
;;	  jc	  short uss_exit
	or	al, SC_Break
	call	[ss_queue_proc] 	; send up stroke of left alt
	jc	short uss_exit
	and	edx, NOT SS_Either_Alt	; remove the alt bits from the local
					; shift state
update_toggles:
	mov	ecx, edx
	xor	ecx, edi		; determine bits that are different,
					; by XORing local with global
	jz	short update_done	; jump if no differences left
	and	ecx, (SS_CapLock + SS_NumLock + SS_ScrlLock)	; isolate toggles
	jz	short update_lp 	; jump if no more toggle differences
	call	process_differences
	jc	short uss_exit		; output buffer full
	jmp	update_toggles		; check for more differences

update_lp:
	mov	ecx, edx
	xor	ecx, edi		; determine bits that are different,
					; by XORing local with global
	jz	short update_done	; jump if no differences left
	call	process_differences
	jc	short uss_exit		; output buffer full
	jmp	update_lp
update_done:
	mov	eax, edx		; get resulting local in ax
	call	CombineShiftKeys
	mov	[esi.loc_shift_state], ax ; update local state
	or	esi, esi		; make sure Z flag & Carry are clear

uss_exit:
	pop	edx
	pop	edi
	pop	ecx
	ret


process_differences:
	bsf	ecx, ecx		; find first difference
	mov	ebx, 1
	shl	ebx, cl 		; shift bit into correct position again
	xor	edx, ebx		; toggle the local state bit

	shl	ecx, 1			; ecx = ecx * SIZE(State_Conv_Struc) [2]
	mov	al, [state_table+ecx.scancode]
	movzx	ecx, [state_table+ecx.handlerCase]
	jmp	[ecx+KeyCase]

KeyCase label	dword
	dd	OFFSET32 SingleKey
	dd	OFFSET32 DoubleKey
	dd	OFFSET32 ToggleKey
	dd	OFFSET32 AltKey

DoubleKey:
	mov	cl, al
	mov	al, SC_Ext0	; queue extended key prefix
	call	[ss_queue_proc]
	jc	short pd_exit
	mov	al, cl
				; then handle actual scan same as a SingleKey
SingleKey:
	test	di, bx		;Q: global bit set?
	jnz	short last_key	;  Y: send down stroke to VM
send_up:
	or	al, SC_Break	;  N: send up stroke
	jmp	short last_key

ToggleKey:
	xchg	bh, bl
	test	dx, bx		;Q: is down bit set in local?
	jz	short nrm_toggle;  Y: toggle key down when focus switched
	or	al, SC_Break
	call	[ss_queue_proc] ;     so send up stroke
	jc	short pd_exit
	xor	al, SC_Break	;     (toggle key state also switched while
				;      away, so we need to send down & up
				;      strokes to toggle the state)
nrm_toggle:
	call	[ss_queue_proc] ;  N: queue down stroke
	jc	short pd_exit
	test	di, bx		;Q: is down bit set in global?
	jnz	short pd_exit	;   Y: don't send an up stroke
	jz	send_up

AltKey:
	call	VKD_send_ralt_pre
	jc	short pd_exit
	test	di, bx		;Q: global bit set?
	jnz	short alt_down	;  Y: send down stroke to VM
				;  N: send up down up strokes to VM
	or	al, SC_Break
	jc	short pd_exit
	call	[ss_queue_proc] ; send up stroke of alt
	jc	short pd_exit

	xor	al, SC_Break
	call	[ss_queue_proc] ; send down stroke of left alt
	jc	short pd_exit
	call	[ss_queue_proc] ; send 2nd down stroke of left alt
	jc	short pd_exit
	or	al, SC_Break	; send up stroke of left alt
	jmp	short last_key

alt_down:
last_key:
	call	[ss_queue_proc]

pd_exit:
	ret

EndProc Update_Shift_State


;******************************************************************************
;
;   VKD_EOI
;
;   DESCRIPTION:    service the simulated hardware interrupt's EOI, by
;		    cancelling the Time_out set, and boosting the VM's priority
;		    slightly to allow the VM to finish processing the interrupt
;		    more quickly!
;
;   ENTRY:	    EAX = IRQ Handle
;		    EBX = Handle of current VM
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc VKD_EOI, PUBLIC, High_Freq

	VK_HK_Queue_Out "VKD_EOI"

IF DebugPorts GE 3
	Trace_Out 'e',/noeol
ENDIF
	mov	edi, ebx		; set edi to point to VKD data in
	add	edi, [VKD_CB_Offset]	; the VM's control block
	VxDCall VPICD_Clear_Int_Request
	sti

	TestMem [edi.kbdState], KBS_INT9_paste	;Q: INT 9 pasting?
	jz	short eoi_give_boost		;   N:
	SetFlag [edi.kbdState], KBS_sim_hold	;   Y: flag to delay next
	jmp	short eoi_no_boost		;      int 9 simulation

eoi_give_boost:
	mov	eax, [VKD_Int_Boost_Amount]
	VMMcall Adjust_Execution_Time

eoi_no_boost:
	ClrFlag [edi.kbdState], KBS_ACK

	mov	edx, edi		; pass ptr to CB data
	CallRet VKD_VM_Server

EndProc VKD_EOI

;******************************************************************************
;
;   VKD_Virt_Int
;
;   DESCRIPTION:    Check to see if current BIOS toggle key state matches
;		    our tracked state, and save the current BIOS toggle
;		    key state for comparison at virtual IRET time.
;
;   ENTRY:	    EAX = IRQ Handle
;		    EBX = Handle of current VM
;
;   EXIT:	    none
;
;   USES:	    EAX, EBX, Flags
;
;==============================================================================
BeginProc VKD_Virt_Int

	call	VKD_Chk_BIOS_Toggle_State
	mov	eax, BIOS_Shift_State_Loc
	add	eax, [ebx.CB_High_Linear]
	mov	al, [eax]
	and	al, SS_Toggle_mask
	add	ebx, [VKD_CB_Offset]
	mov	[ebx.Int_toggle_state], al
	ret

EndProc VKD_Virt_Int


;******************************************************************************
;
;   VKD_IRET
;
;   DESCRIPTION:    call the server to see if more interrupts should be
;		    simulated
;
;   ENTRY:	    EAX = IRQ Handle
;		    EBX = Handle of current VM
;		    Carry set if timed out
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc VKD_IRET, PUBLIC, High_Freq

	sti
IF DebugPorts GE 3
	Trace_Out 'r'
ENDIF
	mov	edx, ebx		; pass current VM handle to servicer
	add	edx, [VKD_CB_Offset]
	mov	esi, edx
;
; Compare ending BIOS toggle key state with beginning state.  If they differ,
; then record this ending state as the current tracking state.
;
	mov	eax, BIOS_Shift_State_Loc
	add	eax, [ebx.CB_High_Linear]
	mov	al, [eax]
	and	al, SS_Toggle_mask
	cmp	al, [esi.Int_toggle_state]
	je	short @F
	mov	[esi.BIOS_toggle_state], al
@@:

	TestMem [esi.kbdState], KBS_sim_alt_hld
	jnz	short iret_alt_delay
	TestMem [esi.kbdState], KBS_INT9_paste	;Q: INT 9 pasting?
	jz	short iret_exit 		;   N:
						;   Y: delay next int 9
						;      simulation
	mov	al, [esi._8042_last_data]	; get last byte
	and	al, NOT SC_Break
	mov	esi, OFFSET32 VKD_Next_Paste_Code
	cmp	al, SC_Alt			;Q: last key an ALT?
	mov	eax, [VKD_INT9_Paste_Delay]	;   N: use standard delay
	jne	short iret_not_alt
	mov	eax, [VKD_INT9_Alt_Delay]	;   Y: use ALT key delay
iret_not_alt:
	VMMCall Set_VM_Time_Out

iret_exit:
	CallRet VKD_VM_Server

iret_alt_delay:
	mov	esi, OFFSET32 VKD_Alt_Timeout
	mov	eax, [VKD_Alt_Delay]
	VMMCall Set_VM_Time_Out

EndProc VKD_IRET


;******************************************************************************
;
;   VKD_Next_Paste_Code
;
;   DESCRIPTION:
;
;   ENTRY:	    EDX -> VKD data in VM's CB  (VM handle + VKD_CB_Offset)
;
;   EXIT:	    nothing
;
;   USES:	    flags
;
;==============================================================================
BeginProc VKD_Next_Paste_Code

	ClrFlag [edx.kbdState], KBS_sim_hold
	CallRet VKD_VM_Server

EndProc VKD_Next_Paste_Code

;******************************************************************************
;
;   VKD_Alt_Timeout
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
BeginProc VKD_Alt_Timeout

	ClrFlag [edx.kbdState], KBS_sim_alt_hld
	CallRet VKD_VM_Server

EndProc VKD_Alt_Timeout


;******************************************************************************
;
;   VKD_Int_16
;
;   DESCRIPTION:    This software interrupt handler will convert blocking
;		    int 16s into polling int 16s.  If no key is ready then
;		    the current VMs time slice will be given up.
;
;   ENTRY:	    EBX = Handle of current VM
;		    EBP points to the client frame on the stack
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc VKD_Int_16, High_Freq

	mov	edi, ebx
	add	edi, [VKD_CB_Offset]

	TestMem [edi.kbdState], KBS_INT16_ignore ;Q: ignore this INT 16?
	jnz	I16_reflect			 ;  Y:

	TestMem [edi.kbdState], KBS_INT16_paste ;Q: paste in progress?
	jz	I16_reflect			;   N:

; cancel INT 16 timeout scheduled during VKD_Start_Paste
	xor	esi, esi
	xchg	esi, [edi.paste_timeout]
	VMMCall Cancel_Time_Out

	TestMem [edi.kbdState], KBS_empty_BIOS_buf ;Q: BIOS buffer empty?
	jz	short I16_simulate		   ;	Y:
	mov	esi, BIOS_head_loc
	add	esi, [ebx.CB_High_Linear]
	movzx	eax, word ptr [esi+2]	    ; get tail ptr
	cmp	ax, word ptr [esi]	    ;Q: anything in BIOS buffer?
	jne	short I16_reflect	    ;	Y:
	ClrFlag [edi.kbdState], KBS_empty_BIOS_buf  ;N: clear flag

I16_simulate:
;
; remove a char from the paste buffer and attempt to write it into the BIOS buffer
;

	TestMem [edi.kbdState], KBS_pause_paste
	jnz	short I16_reflect

I16_cont_sim:
	mov	eax, [edi.paste_ptr]
	cmp	eax, [edi.paste_end]	    ;Q: paste buffer empty?
	jb	short I16_paste_char_avail  ;	N:

	mov	eax, Paste_Complete
	call	VKD_End_Paste
	jmp	short I16_reflect

I16_paste_char_avail:
	mov	ebx, eax
	mov	ax, [ebx.Paste_Key]
	add	ebx, SIZE Paste_Rec	    ; update paste buffer ptr
	mov	[edi.paste_ptr], ebx
	cmp	ah, 0FFh		    ;Q: valid scan code?
	jne	short I16_sc_ok 	    ;	Y:
	xor	ah, ah			    ;	N: zero it!
I16_sc_ok:
	SetFlag [edi.kbdState], KBS_pause_paste
	mov	cl, [VKD_pause_count]
	cmp	al, 0Dh 		    ;Q: CR?
	jne	short @F		    ;	N:
	mov	cl, [VKD_CR_pause_count]    ;	Y: fail some more polls
@@:
	mov	[edi.poll_count], cl
	call	VKD_INT16_write
	or	al, al			    ;Q: write successful?
	jz	short I16_reflect	    ;	Y:
	sub	ebx, SIZE Paste_Rec	    ; back to current entry
	mov	[edi.paste_ptr], ebx
	cmp	al, 0FFh		    ;Q: int 16 write supported?
	jne	short I16_reflect	    ;	Y: just reflect int 16 & try again
	push	edi
	call	VKD_INT16_Paste_Not_Supported ; N: abort int 16 paste
	pop	edi			    ;	   and reflect int 16
;
; reflect the interupt down to the VM
;
I16_reflect:
	mov	al, [ebp.Client_AH]
	and	al, NOT 10h
	jz	SHORT I16_Blocking_Read 	; jump if blocking read (0 or 10h)

	SetFlag [edi.kbdState], KBS_allow_block ; seen get status
	TestMem [edi.kbdState], KBS_INT16_paste ;Q: paste in progress?
	jz	short @F			;   N:
	xor	eax, eax			;   Y: hook return to add delay
	mov	esi, OFFSET32 I16_Return
	VMMCall Call_When_VM_Returns
@@:
force_reflect:
	stc
	ret

;------------------------------------------------------------------------------


I16_Blocking_Read:
	btr	[edi.kbdState], KBS_allow_block_bit ; Q: poll call done before ?
	jc	short force_reflect		    ;	Y:

IF 0
; If the VM's out queue is empty, make sure the VM and global shift state match

	VMMcall Test_Sys_VM_Handle		; not for SYS VM
	jz	short I16_shift_okay

	mov	al, [edi.out_head]		; queue empty?
	cmp	al, [edi.out_tail]
	jne	short I16_shift_okay

	mov	edx, edi
	push	ebx
	call	VKD_Convert_BIOS_Shift_State	; BIOS match our global state?
	mov	ebx, NOT (SS_Toggle_mask OR SS_Toggle_Dn_mask)
	and	eax, ebx
	and	ebx, [gbl_shift_state]
	cmp	eax, ebx
	jz	short @f

IFDEF DEBUG   ;;_VERBOSE
	push	ecx
	mov	cx, [edx.loc_shift_state]
	Trace_Out "BIOS #AX  GBL #BX  LCL #CX"
	pop	ecx
ENDIF
	mov	esi, edx

	movzx	ebx,  [edx.loc_shift_state]
	and	ebx, (SS_Toggle_mask OR SS_Toggle_Dn_Mask)
	or	eax, ebx
	mov	ebx, [gbl_shift_state]

	call	Update_Shift_State
	jz	short @f		    ; jump if no update necessary
	call	VKD_VM_Server		    ; call server, to start process of
	;;;VxDcall VSD_Bell	   ; !!! Just for now
@@:					    ; sending update codes from outbuf
	pop	ebx

I16_shift_okay:
ENDIF


	Push_Client_State
	VMMcall Begin_Nest_Exec 		; Nest execution level

	mov	dh, [ebp.Client_AH]
	inc	dh				; Polling int 16  (0 - 1)
						; extended polling (10h - 11h)
I16_Poll_Loop:
	mov	[ebp.Client_AH], dh
	mov	eax, 16h
	VMMcall Exec_Int
	TestMem [ebp.Client_Flags], ZF_Mask	; Q: Key ready?
	jz	SHORT I16_Key_Ready		;    Y: Done
	VMMCall Release_Time_Slice
	jmp	I16_Poll_Loop

I16_Key_Ready:

	VMMcall End_Nest_Exec			; Pop off execution level
	Pop_Client_State
	stc					; Reflect blocking int 16 now
	ret


I16_Return:
	TestMem [ebp.Client_Flags], ZF_Mask
	jz	short I16_not_clear
	add	ebx, [VKD_CB_Offset]
	dec	[ebx.poll_count]
	jnz	short I16_not_clear
	ClrFlag [ebx.kbdState], KBS_pause_paste
I16_not_clear:
	ret

EndProc VKD_Int_16


;******************************************************************************
;
;   VKD_INT16_write
;
;   DESCRIPTION:    Attempt an INT 16 function 5 call to write an ASCII/scan
;		    code pair into the BIOS keyboard buffer
;
;   ENTRY:	    AL = ASCII value
;		    AH = scan code
;		    EDI points to VKD data in VM's CB
;
;   EXIT:	    AL = 0,   if successful
;			 1,   if buffer full
;			 FFh, if not supported
;
;   USES:
;
;==============================================================================

BeginProc VKD_INT16_write

	Push_Client_State
	VMMcall Begin_Nest_Exec 		; Nest execution level
	SetFlag [edi.kbdState], KBS_INT16_ignore ; ignore simulated INT 16's
;;	  Queue_Out ">> #ax"
	mov	[ebp.Client_CX], ax
	mov	[ebp.Client_AX], 5FFh		; write keyboard buffer
	mov	eax, 16h
	VMMcall Exec_Int
	mov	al, [ebp.Client_AL]
	ClrFlag [edi.kbdState], KBS_INT16_ignore ; clear flag
	VMMcall End_Nest_Exec			; Pop off execution level
	Pop_Client_State
	ret

EndProc VKD_INT16_write

;******************************************************************************
;
;   VKD_Release_TS_Hook
;
;   DESCRIPTION:    Look at release calls and ignore them, if the VM is
;		    pasting, else pass it on to the real handler.
;
;   ENTRY:	    nothing
;
;   EXIT:	    nothing
;
;   USES:	    flags
;
;==============================================================================

BeginProc VKD_Release_TS_Hook, SERVICE

	push	ebx
	push	esi
	VMMCall Get_Cur_VM_Handle
	mov	esi, ebx
	add	esi, [VKD_CB_Offset]

	cmp	[esi.last_key_time], 0		;Q: a key simulated?
	je	short rts_no_key		;   N:
	push	eax
	VMMCall Get_VM_Exec_Time
	sub	eax, [esi.last_key_time]
	cmp	eax, [VKD_KEY_IDLE_DELAY]
	pop	eax
	jb	short rts_eat
	mov	[esi.last_key_time], 0
rts_no_key:

	TestMem [esi.kbdState], KBS_Pasting	;Q: VM pasting?
	jnz	short rts_pasting		;   Y:
	pop	esi
	pop	ebx
	jmp	[real_release_service]

rts_pasting:
	TestMem [esi.kbdState], KBS_INT16_paste ;Q: VM INT 16 pasting?
	jz	short rts_eat			;   N: just eat release
	ClrFlag [esi.kbdState], KBS_pause_paste ;   Y: force another char to
						;      become available
rts_eat:
	pop	esi
	pop	ebx
	clc
	ret

EndProc VKD_Release_TS_Hook


;******************************************************************************
;
;   VKD_Abort_INT16_Paste & VKD_INT16_Paste_Not_Supported
;
;   DESCRIPTION:    INT 16 paste attempt, failed, since no INT 16's were
;		    performed within a specified time limit.
;
;   ENTRY:	    EDX is VM handle
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc VKD_Abort_INT16_Paste

	Trace_Out 'INT 16 paste failed - (timed out) - switching to INT 9 paste'
VKD_INT16_Paste_Not_Supported:
	mov	ebx, edx
	add	edx, [VKD_CB_Offset]
	mov	eax, [edx.kbdState]
	and	ah, NOT ((KBS_empty_BIOS_buf + KBS_INT16_paste) SHR 8)
.errnz 0FFFF00FFh AND (KBS_empty_BIOS_buf + KBS_INT16_paste)
	or	ah, KBS_INT9_paste SHR 8
.erre KBS_INT9_paste AND 0FF00h
	mov	[edx.kbdState], eax
	CallRet VKD_VM_Server	    ; kick server to get INT 9 pasting started

EndProc VKD_Abort_INT16_Paste


;******************************************************************************
;
;   VKD_End_Paste
;
;   DESCRIPTION:    Reset the paste flags in kbdState, free the allocated
;		    paste buffer and call registered call-back.
;
;   ENTRY:	    EAX = completion flag
;		    EBX = VM handle
;
;   EXIT:
;
;   USES:	    flags
;
;==============================================================================

BeginProc VKD_End_Paste

	Assert_VM_Handle ebx
	pushad
	mov	edi, ebx
	add	edi, [VKD_CB_Offset]
	TestMem [edi.kbdState], KBS_Pasting ;Q: pasting in progress?
	jz	short ep_exit		    ;	N:
	mov	[edi.paste_ptr], 0	    ;	Y: end it!

KBS_pflags equ KBS_Pasting+KBS_pause_paste+KBS_empty_BIOS_buf+KBS_sim_hold+KBS_queue_hold
	ClrFlag [edi.kbdState], KBS_pflags  ; clear paste state flags

	push	eax			    ; Save completion code
	mov	eax, [edi.paste_buffer]     ; get allocation handle
	VMMCall _PageFree <eax, 0>

	pop	eax			    ; Recover completion code
	cmp	[edi.paste_callback], 0     ;Q: registered call-back?
	jz	short ep_exit		    ;	N:
	mov	edx, [edi.paste_callback_ref] ; reference data
	call	[edi.paste_callback]

ep_exit:
	popad
	ret

EndProc VKD_End_Paste


page
;******************************************************************************
;   I N T E R N A L   S E R V I C E S
;******************************************************************************


;******************************************************************************
;
;   IsShiftKey
;
;   DESCRIPTION:    Determine is the scan code is for a shift key (left or right)
;
;   ENTRY:	    AL is scan code to check
;
;   EXIT:	    Z flag set, if the scan code is a shift key
;		    Carry set, if right shift
;
;   USES:	    Flags
;
;==============================================================================
BeginProc IsShiftKey

	push	eax
	and	al, NOT SC_Break
	cmp	al, SC_LShf
	je	short is_shift
	cmp	al, SC_RShf
	stc
is_shift:
	pop	eax
	ret

EndProc IsShiftKey

;******************************************************************************
;
;   IsModifierKey
;
;   DESCRIPTION:    Determine if the virtual key is for a shift key, alt key,
;		    control key, shift lock key, num lock key, or scroll lock key
;
;   ENTRY:	    AX is the virtual key
;
;   EXIT:	    Z flag set, if the scan code is a shift key
;
;   USES:
;
;==============================================================================
BeginProc IsModifierKey

	push	eax
	and	al, NOT SC_Break	; mask off "break" bit
	cmp	al, shft_tab_max	;Q: above table?
	ja	short not_modifier	;  Y: not a modifier
	sub	al, shft_tab_min	;Q: below table?
	jb	short not_modifier	;  Y: not a modifier
	movzx	eax, al
	shl	eax, 1			; convert word to a byte offset
	cmp	shift_table[eax], 0
	je	short not_modifier
	xor	eax, eax		; set Z flag
	jmp	short mod_exit
not_modifier:
	or	al, 1			; clear Z flag
mod_exit:
	pop	eax
	ret

EndProc IsModifierKey


;******************************************************************************
;
;   VKD_Chk_BIOS_Toggle_State
;
;   DESCRIPTION:    Merge the toggle key states (CapsLock, NumLock, ScrollLock)
;		    into the VM's local shift state and the global shift state.
;		    If the VM is suspended, then don't do anything, because
;		    we don't want to instance fault on the BIOS shift state
;		    word.
;
;   ENTRY:	    EBX = VM Handle
;
;   EXIT:	    none
;
;   USES:	    EAX, Flags
;
;==============================================================================
BeginProc VKD_Chk_BIOS_Toggle_State

	push	ebx
	call	VKD_Convert_BIOS_Shift_State
	jc	short no_toggle_update	    ; skip shift state update if VM suspended
	and	al, SS_Toggle_mask
	mov	ah, al
	add	ebx, [VKD_CB_Offset]
	xor	al, [ebx.BIOS_toggle_state] ; 1 bits are toggles that are different
	jz	short no_toggle_update
	xor	[ebx.BIOS_toggle_state], al ; update the toggle state
	xor	ah, byte ptr [ebx.loc_shift_state]
					    ;Q: VM's BIOS state = loc_shift_state
	and	ah, al			    ;	for toggles that have changed?
	jz	short no_toggle_update	    ;	Y: don't update the toggle state
;;	  Trace_Out 'toggle state mismatch'
	xor	byte ptr [gbl_shift_state], al
	xor	byte ptr [ebx.loc_shift_state], al
no_toggle_update:
	pop	ebx
	ret

EndProc VKD_Chk_BIOS_Toggle_State


;******************************************************************************
;
;   VKD_Convert_BIOS_Shift_State
;
;   DESCRIPTION:    Look at BIOS to determine current shift state
;
;   ENTRY:	    EBX = VM Handle
;
;   EXIT:	    EAX = converted shift state
;		    Carry set, if VM is currently suspended
;
;   USES:	    EAX, Flags
;
;==============================================================================

BeginProc VKD_Convert_BIOS_Shift_State

	TestMem [ebx.CB_VM_Status], VMStat_Suspended
	jz	short @F
	mov	eax, [VKD_CB_Offset]
	movzx	eax, [ebx+eax.loc_shift_state]
	stc
	ret
@@:
	push	ebx
	mov	ebx, [ebx.CB_High_Linear]
	movzx	eax, word ptr [ebx.BIOS_Shift_State_Loc]
	mov	bh, [ebx.BIOS_Mode_State_Loc]
	and	bh, (SS_RCtrl+SS_RAlt) SHR 8 ; BH = right key state

	mov	bl, ah
	and	bl, 3			    ; isolate left ctrl & alt
	shl	bl, 2			    ; ctrl is bit 2, alt is bit 3
					    ; BL = left key state

	and	ah, NOT ((SS_Unused + BSS_Unused) SHR 8)  ; remove unused bits
.errnz 0FFFF00FFh AND (SS_Unused + BSS_Unused)

	test	al, BSS_Ctrl + BSS_Alt
	jz	short @F
	or	ah, bh			    ; or in right ctrl & alt bits

	or	bh, bl			    ;Q: are either right or left bits set?
	jz	short @F		    ;	N: BIOS must not support 101 kybd
					    ;	   leave left bits alone
	and	al, NOT (BSS_Ctrl + BSS_Alt)
	or	al, bl			    ; or in left ctrl & alt bits

@@:
	TestReg eax, BSS_RShift
	jz	short no_rshift
	and	al, NOT BSS_RShift	    ; convert BIOS bit
	or	eax, SS_RShift		    ; to our bit
no_rshift:
	pop	ebx

	Assumes_Fall_Through CombineShiftKeys

EndProc VKD_Convert_BIOS_Shift_State


;******************************************************************************
;
;   CombineShiftKeys
;
;   DESCRIPTION:    set the "either" bits in the shift state mask for the
;		    shift, ctrl and alt left/right pairs
;
;   ENTRY:	    AX is the shift mask to adjust
;
;   EXIT:	    AX is the new shift mask
;		    Carry clear
;
;   USES:	    AX, Flags
;
;==============================================================================

BeginProc CombineShiftKeys

	and	ax, NOT (SS_Shift + SS_Ctrl + SS_Alt)
	test	ax, SS_LShift + SS_RShift
	jz	short no_Shift
	or	al, SS_Shift			; indicate that 1 or both is down
.erre SS_Shift AND 0FFh
no_Shift:
	test	ax, SS_LCtrl + SS_RCtrl
	jz	short no_Ctrl
	or	al, SS_Ctrl			; indicate that 1 or both is down
.erre SS_Ctrl AND 0FFh
no_Ctrl:
	test	ax, SS_LAlt + SS_RAlt
	jz	short no_Alt
	or	ah, SS_Alt SHR 8		; indicate that 1 or both is down
.erre SS_Alt AND 0FF00h
no_Alt:
;;	clc				; all of the above instructions clear
					; carry as a side effect and this is
					; assumed by VKD_Convert_BIOS_Shift_State
	ret

EndProc CombineShiftKeys


;******************************************************************************
;
;   Chk_Shift_State
;
;   DESCRIPTION:    Update the global shift state and return the updated local
;		    shift state as a result of a virtual key being pressed or
;		    released.  The states are only modified, if the virtual key
;		    is a modifier key (shift, alt, ctrl, shift lock, num lock,
;		    or scroll lock)  The VM's local shift state is not actually
;		    changed, but its new value is returned in CX, this is for
;		    when servicing hot keys, where the global shift state should
;		    change, but the local shift state isn't.
;
;   ENTRY:	    AX is the virtual key to check
;		    high byte indicates code is extensions
;		    ESI points to control block data for focus VM
;
;   EXIT:	    AX is preserved
;		    CX is the resulting local shift state
;		    gbl_shift_state is updated, but the VM's local shift state
;		    has not (storing CX into the control block location will
;		    accomplish the necessary update)
;
;   USES:	    nothing
;
;==============================================================================

BeginProc Chk_Shift_State

	push	eax
	push	ebx
	push	edx

; get current state words
	mov	ebx, [gbl_shift_state]	 ; global state
	movzx	edx, [esi.loc_shift_state] ; local state

	mov	ecx, eax
	and	al, NOT SC_Break	; mask off "break" bit
	cmp	al, shft_tab_max	;Q: above table?
	ja	ss_exit 		;  Y: can't change shift state
	sub	al, shft_tab_min	;Q: below table?
	jb	short ss_exit		;  Y: can't change shift state
	movzx	eax, al
	shl	eax, 1			; convert word to a byte offset
	mov	ax, shift_table[eax]	;  N: get status byte
	or	eax, eax		;Q: non-zero status byte?
	jz	short ss_exit		;  N: no need to change state

	test	ax, SS_Toggle_mask	;Q: toggle key?
	jnz	short toggle_key	;  Y: then toggle state

	TestReg ecx, VK_Extended	;Q: extended keyboard ext prefix?
	jz	short no_prefix 	;  N:
	xchg	ah, al			;  Y: move bits to high byte to
					;     represent, right key vs. left
					;     should only happen for ctrl & alt
no_prefix:
	test	cl, SC_Break		;Q: key press?
	jz	short key_press
	not	eax			;  N: exclude new status bits
	and	ebx, eax		;     modify both global & local
	and	edx, eax
	jmp	short update

key_press:				;  Y: include new status bits
	or	ebx, eax		;     modify both global & local
	or	edx, eax
	jmp	short update

toggle_key:
	mov	ah, al			;  add down bit to mask
	test	bx, SS_Ctrl		;Q: control key down?
	jnz	short toggle_dn 	;   Y: toggle only the down bits
	TestMem [VKD_flags], VKDf_HK_hold ;Q: holding for hot keys?
	jz	short no_hold		;  N: normal operation
toggle_dn:
	and	al, NOT SS_Toggle_mask	;  Y: don't toggle the state bits
.errnz	SS_Toggle_mask AND 0FF00h

no_hold:
	test	cl, SC_Break		;Q: key press?
	jnz	short key_up		;  N: toggle down flag bits
	test	bh, ah			;Q: down flag set?
	jz	short toggle_state	;  N: toggle down and/or state flag bits
	jmp	short ss_exit		;  Y: don't update

key_up: and	al, NOT SS_Toggle_mask	;  Y: leave state bits untouched
.errnz	SS_Toggle_mask AND 0FF00h
	not	ah
	and	bh, ah			;     clear down bits
	and	dh, ah
	xor	ah, ah

toggle_state:
	xor	bl, al			;toggle state bits for both
	xor	dl, al			;global and local states
	or	bh, ah			; set down bits, if ah # 0 (so it won't
	or	dh, ah			; do anything if we came through key_up)

update: ; store new state bytes
	mov	eax, ebx		; new global
	call	CombineShiftKeys
	mov	ebx, eax		; new global after combining shift keys
	mov	eax, edx		; new local
	call	CombineShiftKeys
	mov	edx, eax		; new local after combining shift keys
	mov	[gbl_shift_state], ebx

ss_exit:

	mov	ecx, edx		; move local state into cx, for return
	pop	edx
	pop	ebx
	pop	eax
	ret

EndProc Chk_Shift_State


IFDEF DEBUG
;******************************************************************************
;
;   Show_VMs_VKD_State, Show_VKD_Hot_Key & VKD_Debug_Query
;
;   DESCRIPTION:    Special debugging code to show the current state of VKD
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc Show_VMs_VKD_State
	push	ebx
	push	esi
	mov	esi, [VKD_CB_Offset]
	add	esi, ebx
	mov	eax, [esi.kbdState]
	Trace_Out "kbdState: #eax"
	mov	ax, [esi.loc_shift_state]
	Trace_Out "local shift state: #ax"

	Trace_Out "Last sent = " /noeol
	movzx	ebx, [esi.sent_ptr]
show_sent_buf:
	inc	ebx
	and	ebx, VKD_SentBuf_Size - 1; wrap around for circular Q
	mov	al, [ebx+esi.sent_buf]
	Trace_Out " #al" /noeol
	cmp	bl, [esi.sent_ptr]
	jne	show_sent_buf
	Trace_Out " "

	Trace_Out "Outbuf    = " /noeol
	movzx	ebx, [esi.out_head]
show_buf_lp:
	cmp	bl, [esi.out_tail]	    ;Q: buffer empty?
	je	short q_bufend		    ;	Y:
	inc	ebx
	and	ebx, VKD_OutBuf_Size - 1    ; wrap around for circular Q
	mov	al, [ebx+esi.outbuf]	    ; get byte from buffer
	Trace_Out " #al" /noeol
	jmp	show_buf_lp
q_bufend:
	Trace_Out " "

	TestMem [esi.kbdState], KBS_Pasting
	jz	short q_no_paste
	mov	eax, [esi.paste_buffer]
	mov	ebx, [esi.paste_ptr]
	mov	ecx, [esi.paste_callback]
	mov	edx, [esi.paste_timeout]
	Trace_Out "Paste:  buf = #eax, ptr = #ebx, to = #edx, callback = ?ecx"
	jmp	short q_exit
q_no_paste:
	Trace_Out "Paste:  none"
q_exit:
	pop	esi
	pop	ebx
	ret
EndProc Show_VMs_VKD_State


BeginProc Show_VKD_Hot_Key
	mov	al, [edi.Hot_Key_scan_code]
	Trace_Out "#al " /noeol
	cmp	[edi.Hot_Key_extended], 0
	je	short norm_hk
	Trace_Out "ext" /noeol
	jmp	short show_call_flags
norm_hk:
	Trace_Out "nrm" /noeol
show_call_flags:
	movzx	eax, [edi.Hot_Key_call_mask]
	VMMCall Debug_Convert_Hex_Binary
	mov	ebx, eax
	movzx	eax, byte ptr [edi.Hot_Key_SS_Mask+1]
	VMMCall Debug_Convert_Hex_Binary
	mov	ecx, eax
	movzx	eax, byte ptr [edi.Hot_Key_SS_Mask]
	VMMCall Debug_Convert_Hex_Binary
	mov	edx, [edi.Hot_Key_call_back]
	Trace_Out " #ebx   #ecx#eax ?edx "
	mov	edx, [edi.Hot_Key_ref_data]
	mov	ebx, [edi.Hot_Key_max_elapsed]
	movzx	eax, byte ptr [edi.Hot_Key_SS_Compare+1]
	VMMCall Debug_Convert_Hex_Binary
	mov	ecx, eax
	movzx	eax, byte ptr [edi.Hot_Key_SS_Compare]
	VMMCall Debug_Convert_Hex_Binary
	Trace_Out "#edx #ebx #ecx#eax"
	mov	ecx, [edi.Hot_Key_Local_Flag]
	jecxz	short debug_not_local

	mov	eax, [VKD_CB_Offset]
	VMMCall Get_Cur_VM_Handle
chk_hk_disabled:
	bt	[eax+ebx.disabled_hot_keys], ecx
	jnc	short debug_not_disabled
	Trace_Out "  disabled in VM #ebx"
debug_not_disabled:
	VMMCall Get_Next_VM_Handle
	VMMCall Test_Cur_VM_Handle
	jne	chk_hk_disabled

debug_not_local:
	ret
EndProc Show_VKD_Hot_Key


BeginProc VKD_Debug_Query

	pushad
DebAgn:
	trace_out "Select desired VKD debug function:"
	trace_out "   [0]   - General info"
	trace_out "   [1]   - Hot Key info"
	trace_out "   [2]   - Per VM info"
	trace_out "   [3]   - Set VKD queue_outs"
	trace_out "   [ESC] - Exit VKD debug querry"
	trace_out " ? ",NO_EOL
	VMMcall In_Debug_Chr
	Trace_Out " "
	jz	short NoVKDDebInf
	cmp	al,'0'
	jne	short Try1
	call	VKD_Gen_Info
	jmp	DebAgn

Try1:
	cmp	al,'1'
	jne	short Try2
	call	VKD_HtKy_Info
	jmp	DebAgn

Try2:
	cmp	al,'2'
	jne	short Try3
	call	VKD_VM_Info
	jmp	DebAgn

Try3:
	cmp	al,'3'
	jne	DebAgn
	call	VKD_Queue_Info
	jmp	DebAgn

NoVKDDebInf:
	popad
	ret


EndProc VKD_Debug_Query

BeginProc VKD_Queue_Info

	mov	al,[VK_D_QFlag]
	test	al,VK_DQ_VADint
	jz	short VAD_Off
	trace_out "VAD       queue_outs are ON"
	jmp	short VAD_On

VAD_Off:
	trace_out "VAD       queue_outs are OFF"
VAD_On:
	test	al,VK_DQ_HtKy
	jz	short HK_Off
	trace_out "HOT KEY   queue_outs are ON"
	jmp	short HK_On

HK_Off:
	trace_out "HOT KEY   queue_outs are OFF"
HK_On:
	trace_out " "
	trace_out "   [0]   - Turn VAD       queue_outs OFF"
	trace_out "   [1]   - Turn VAD       queue_outs ON"
	trace_out "   [2]   - Turn HOT KEY   queue_outs OFF"
	trace_out "   [3]   - Turn HOT KEY   queue_outs ON"
	trace_out "   [ESC] - Done"
	trace_out " ? ",NO_EOL
	VMMcall In_Debug_Chr
	Trace_Out " "
	jz	short QDDone
	cmp	al,'0'
	jne	short Try1a
	ClrFlag [VK_D_QFlag], VK_DQ_VADint
	jmp	VKD_Queue_Info

Try1a:
	cmp	al,'1'
	jne	short Try2a
	SetFlag [VK_D_QFlag], VK_DQ_VADint
	jmp	VKD_Queue_Info

Try2a:
	cmp	al,'2'
	jne	short Try3a
	ClrFlag [VK_D_QFlag], VK_DQ_HtKy
	jmp	VKD_Queue_Info

Try3a:
	cmp	al,'3'
	jne	HK_On
	SetFlag [VK_D_QFlag], VK_DQ_HtKy
	jmp	VKD_Queue_Info

QDDone:
	ret

EndProc VKD_Queue_Info

BeginProc VKD_HtKy_Info

	mov	esi, [Hot_Key_List]
	VMMCall List_Get_First
	jz	short VKD_HtKy_Exit
	mov	edi, eax
	jmp	short FirstHK

show_hk_group:
	VMMcall In_Debug_Chr
	jz	short VKD_HtKy_Exit
FirstHK:
	mov	ecx, 5

	Trace_Out "SC     D--PCRrp   -dddrrree      e CallBack"
	Trace_Out "                   CNSACSACCNSACSS"
show_hk:
	push	ecx
	call	Show_VKD_Hot_Key
	pop	ecx
	mov	eax, edi
	VMMCall List_Get_Next		    ; EAX is next key definition
	jz	short VKD_HtKy_Exit
	mov	edi, eax
	loop	show_hk
	jmp	show_hk_group

VKD_HtKy_Exit:
	trace_out " "
	ret

EndProc VKD_HtKy_Info

BeginProc VKD_VM_Info

	mov	ebx, [VKD_Kbd_Owner]
	Trace_Out <13, 10, "focus VM (#ebx) state:">
	call	Show_VMs_VKD_State

	VMMcall Test_Cur_VM_Handle
	jz	short skip_current	    ; current = focus

	VMMcall Get_Cur_VM_Handle
	Trace_Out <13, 10, "Cur VM (#ebx) state:">
	call	Show_VMs_VKD_State
skip_current:
	mov	ecx, 1			    ; set count to fall out of loop
	jmp	short get_next_vm	    ; the first time

Show_VM_states:
	Trace_Out <13, 10, "VM (#ebx) state:">
	call	Show_VMs_VKD_State
get_next_vm:
	VMMcall Get_Next_VM_Handle
	VMMcall Test_Cur_VM_Handle
	jz	SHORT VKD_VM_Done
	cmp	ebx, [VKD_Kbd_Owner]
	je	get_next_vm
	loop	Show_VM_states
	mov	ecx, 3			    ; set count for next loop
	VMMcall In_Debug_Chr
	jnz	Show_VM_states
VKD_VM_Done:
	trace_out " "
	ret

EndProc VKD_VM_Info

BeginProc VKD_Gen_Info

	Trace_Out "VKD STATUS"
	mov	eax, [VKD_Kbd_Owner]
	mov	esi, [VKD_CB_Offset]
	Trace_Out "focus:  #eax    (CB offset = #esi)"
	mov	eax, [gbl_shift_state]
	Trace_Out "global shift state: #ax"
	mov	eax, [VKD_flags]
	Trace_Out "VKD flags: #eax"
	TestMem [VKD_flags], VKDf_build_guard
	jz	short no_partial_vk
	mov	eax, [VKD_build]
	Trace_Out "partial vk: #ax"
no_partial_vk:

EXTRN Dump_KeyBuf:NEAR
	Trace_Out "global keybuf =" /noeol
	call	Dump_KeyBuf

	mov	eax, [msg_key_buf_owner]
	Trace_Out "msg mode buffer owner: #eax"
	or	eax, eax
	jz	short no_msg_owner
	mov	eax, [msg_key_head]
	mov	ebx, [msg_key_tail]
	mov	ecx, OFFSET32 msg_key_buf
	Trace_Out "buf @#ecx, head = #eax, tail = #ebx"
no_msg_owner:

VKD_Gen_Exit:
	trace_out " "
	ret

EndProc VKD_Gen_Info

ENDIF

VxD_CODE_ENDS

	END

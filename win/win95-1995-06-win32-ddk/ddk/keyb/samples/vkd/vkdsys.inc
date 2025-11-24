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

;******************************************************************************
;
;   Title:	vkdsys.inc
;
;   Version:	1.00
;
;==============================================================================

IFDEF DEBUG
;
;Bits of VK_D_QFlag
;
VK_DQ_HtKy	EQU	00000010b
VK_DQ_HtKyBit	EQU	1

ENDIF

VK_HK_Queue_Out MACRO S, V1, V2
	LOCAL	NoQueue
IFDEF DEBUG
VxD_DATA_SEG
IFNDEF VK_D_QFlag
Extrn VK_D_QFlag:byte
ENDIF
VxD_DATA_ENDS
	pushfd
	testmem [VK_D_QFlag],VK_DQ_HtKy
	jz	short NoQueue
  IFNB <V2>
	queue_out	S, V1, V2
  ELSE
    IFNB <V1>
	queue_out	S, V1
    ELSE
	queue_out	S
    ENDIF
  ENDIF
ENDIF
NoQueue:
IFDEF DEBUG
	popfd
ENDIF
	ENDM

IFDEF DEBUG
VKD_SentBuf_Size    equ     32
ENDIF

VKD_OutBuf_Size     equ     128 	; max # of scan codes in output buffer
_8042_OutBuf_Size   equ     32

;***********
;  Keyboard control block data
;
;   Note that outbuf is at the very end, so that we can squeeze as much
;   into the "short" portion as possible.  Fortunately, all the RETAIL
;   fields jut barely fit.  (outbuf is currently at offset 7Ah, just
;   squeaking in under the 7Fh limit.)
;
VKD_CB_Struc	STRUC
    kbdState		dd  ?	; equates are below
    eventHandle 	dd  ?	; 0 - if no event scheduled
    time_out_handle	dd  0	; handle for timing out VM boost
    disabled_hot_keys	dd  0	; a 1 bit means the key is disabled
    paste_buffer	dd  0
    paste_ptr		dd  0	; pointer to next byte of paste data
    paste_end		dd  0
    paste_callback	dd  0
    paste_callback_ref	dd  0
    paste_timeout	dd  0
    key_queued_ext_shft dd  0	; virtual key code of key that needed pseudo shift
    last_queued 	dd  0	; last virtual key queued by Queue_Virtual_Key
    last_key_time	dd  0	; record VM time of last simulated key
    out_head		db  ?	; position of next scan code to remove
    out_tail		db  ?	; position of last scan code entered
    loc_shift_state	dw  ?	; VM's shift state
    poll_count		db  ?	; count of failed polling int 16h's while pasting
    _8042_data		db  ?	; virtual 8042 data byte
    _8042_last_data	db  ?	; last virtual 8042 data byte
    _8042_status	db  ?	; virtual 8042 status
    _8042_cmd_byte	db  ?	; virtual 8042 command byte
    _8042_last_cmd	db  ?	; last 8042 command
    _8042_out_head	db  ?	; position of next 8042 data byte to remove
    _8042_out_tail	db  ?	; position of last 8042 data byte entered
    _8042_expected	db  ?	; byte count expected from input
    BIOS_toggle_state	db  ?	; tracked BIOS toggle key state
    Int_toggle_state	db  ?	; BIOS toggle key state at start of virtual INT 9


    AltKeyDelay		dd	? ; AltKeyDelay VKD_Alt_Delay
    AltPasteDelay	dd	? ; AltPasteDelay VKD_INT9_Alt_Delay
    PasteDelay		dd	? ; KeyPasteDelay VKD_INT9_Paste_Delay
    PasteFullDelay	dd	? ; KeyBufferDelay VKD_INT9_Buf_full_Delay
    Paste_Time_out	dd	? ; KeyPasteTimeOut VKD_PasteTimeOut
    PasteSkip		db	? ; keypasteskipcount VKD_Pause_Count
    PasteCRSkip		db	? ; KeyPasteCRSkipCount VKD_CR_Pause_Count
    ShellInformedUs	db	? ; has shell informed us about the values

    _8042_outbuf	db  _8042_OutBuf_Size DUP (?)
    outbuf		db  VKD_OutBuf_Size DUP (?)
IFDEF DEBUG
    sent_buf		db  VKD_SentBuf_Size DUP (?)
    sent_ptr		db  ?
ENDIF
VKD_CB_Struc	ENDS

last_queued_sc	equ byte ptr last_queued


; kbdState bits:
;
KBS_ACK 	    equ  0000000000000001b  ; an ACK is being sent
KBS_ACK_bit	    equ 		0
KBS_LED_Cmd	    equ  0000000000000010b  ; waiting for 2nd LED command byte
KBS_LED_Cmd_bit     equ 	       1
KBS_Rpt_Cmd	    equ  0000000000000100b  ; waiting for 2nd repeat rate
KBS_Rpt_Cmd_bit     equ 	      2     ;	command byte
KBS_WrCmd_Cmd	    equ  0000000000001000b  ; waiting for KBD_Ctl_WrCmd data byte
KBS_WrCmd_Cmd_bit   equ 	     3
KBS_WrOut_Cmd	    equ  0000000000010000b  ; waiting for KBD_Ctl_WrOut data byte
KBS_WrOut_Cmd_bit   equ 	    4
KBS_8042Writing     equ  0000000000100000b  ; waiting for 8042 data bytes
KBS_8042Writing_bit equ 	   5
KBS_8042Ignoring    equ  0000000001000000b  ; waiting for 8042 data bytes
KBS_8042Ignore_bit  equ 	  6
KBS_INT16_ignore    equ  0000000010000000b  ; ignore simulated INT 16's
KBS_INT16_ign_bit   equ 	 7
KBS_pause_paste     equ  0000000100000000b  ; don't force another paste char into
KBS_pause_paste_bit equ 	8	    ;	buffer, until polling reads
					    ;	start failing
KBS_empty_BIOS_buf  equ  0000001000000000b  ; BIOS buffer needs to be emptied
KBS_empty_BIOS_bit  equ        9	    ; before INT 16 paste can begin
KBS_INT16_paste     equ  0000010000000000b  ; INT 16 paste in progress
KBS_INT16_paste_bit equ       10
KBS_INT9_paste	    equ  0000100000000000b  ; INT 9 paste in progress
KBS_INT9_paste_bit  equ      11
KBS_sim_hold	    equ  0001000000000000b  ; wait for timeout before simulating
KBS_sim_hold_bit    equ     12		    ; another INT 9
KBS_queue_hold	    equ  0010000000000000b  ; wait for timeout before queueing
KBS_queue_hold_bit  equ    13		    ; any more scan codes for int 9 paste
KBS_msg_mode	    equ  0100000000000000b  ; in special message mode
KBS_msg_mode_bit    equ   14		    ;	no INT 9 simulations
KBS_NO_16_paste     equ  1000000000000000b  ; set if int 16 paste not allowed
KBS_NO_16_paste_bit equ  15
KBS_sim_alt_hld     equ 10000000000000000b
KBS_sim_alt_hld_bit equ 16
KBS_need_shift_up	equ 20000h	    ; set if up transitions of pseudo
KBS_need_shift_up_bit	equ 17		    ;	shift state keys are pending
KBS_pseudo_left 	equ 40000h	    ; set if a pseudo left shift is
KBS_pseudo_left_bit	equ 18		    ;	pending
KBS_pseudo_right	equ 80000h	    ; set if a pseudo right shift is
KBS_pseudo_right_bit	equ 19		    ;	pending
KBS_critical_init	equ 100000h	    ; set if VKD_VM_Critical_Init
KBS_critical_init_bit	equ 20		    ;	has been completed
KBS_allow_block 	equ 200000h	    ; set after a polling read to
KBS_allow_block_bit	equ 21		    ;	allow the next blocking read
					    ;	to be reflected without conversion
KBS_Forced_Client_Sti	equ 400000h	    ; set after we enable VM ints for
KBS_Forced_Client_Sti_bit	equ 22	    ;   the client whose ints were 
					    ;   disabled. (in int 15h handler).
KBS_paused		equ 800000h	    ; set when about to simulate PAUSE
KBS_paused_bit		equ 23		    ;	key press for VM
KBS_skip_pause		equ 1000000h
KBS_skip_pause_bit	equ 24
KBS_paused_blocked	equ 2000000h
KBS_paused_blocked_bit	equ 25
KBS_gravis_prefix	equ 4000000h
KBS_gravis_prefix_bit	equ 26
KBS_gravis_seq		equ 8000000h
KBS_gravis_seq_bit	equ 27
KBS_Sim9Sem		equ 10000000h
KBS_Sim9Sem_bit		equ 28

KBS_8042_hold	    equ KBS_ACK + KBS_LED_Cmd + KBS_Rpt_Cmd
KBS_Cmd_hold	    equ KBS_LED_Cmd + KBS_Rpt_Cmd
KBS_Pasting	    equ KBS_INT16_paste + KBS_INT9_paste


;***********
;  Keyboard controller equates
;
pcmd_Kybd   equ 64h		    ; Keyboard command
KBD_Ctl_RdCmd	equ 020h		; Read Command Byte
KBD_Ctl_MxRdReg equ 03Fh		; Max Read Reg Byte
KBD_Ctl_WrCmd	equ 060h		; Write Command Byte
KBD_Ctl_MxWrReg equ 07Fh		; Max Write Reg Byte
KBD_Ctl_EnaSpd	equ 0A3h		; Enable speed control	  [Compaq]
KBD_Ctl_Toggle	equ 0A4h		; Toggle speed		  [Compaq]
KBD_Ctl_TstPass equ 0A4h		; Test password 	  [MCA]
KBD_Ctl_SpecRd	equ 0A5h		; Special read		  [Compaq]
KBD_Ctl_LdPass	equ 0A5h		; Load password 	  [MCA]
KBD_Ctl_EnPass	equ 0A6h		; Enable security	  [MCA]
KBD_Ctl_DisAux	equ 0A7h		; Disable AUX		  [MCA]
KBD_Ctl_EnAux	equ 0A8h		; Enable AUX		  [MCA]
KBD_Ctl_TstAux	equ 0A9h		; Test AUX		  [MCA]
KBD_Ctl_Init	equ 0AAh		; Initialization command
KBD_Ctl_Test	equ 0ABh		; Interface test
KBD_Ctl_Diag	equ 0ACh		; Diagnostic dump	  [Compaq]
KBD_Ctl_Dis	equ 0ADh		; Disable keyboard
KBD_Ctl_Ena	equ 0AEh		; Enable keyboard
KBD_Ctl_RdIn	equ 0C0h		; Read Input Port
KBD_Ctl_PollInL equ 0C1h		; Poll In port low 4 bits [MCA]
KBD_Ctl_PollInH equ 0C2h		; Poll In port hig 4 bits [MCA]
KBD_Ctl_RdOut	equ 0D0h		; Read Output Port
KBD_Ctl_WrOut	equ 0D1h		; Write controller output port
KBD_Ctl_WrOBuf	equ 0D2h		; Write next out to 60h   [MCA]
					; back to 60h as if it had been
					; sent from the kbd
KBD_Ctl_WrAOBf	equ 0D3h		; same as D2, for AUX	  [MCA]
KBD_Ctl_WrAux	equ 0D4h		; Write to aux device	  [MCA]
KBD_Ctl_RdTest	equ 0E0h		; Read test port
KBD_Ctl_Pulse	equ 0F0h		; first pulse output port cmd

pstat_Kybd  equ 64h		    ; Keyboard status
fKBS_PE 	equ 10000000b	    ;	parity error
fKBS_PEBit	equ 7
fKBS_RTO	equ 01000000b	    ;	timeout on reception from kybd
fKBS_RTOBit	equ 6
fKBS_AuxData	equ 00100000b	    ;	Aux data available
fKBS_AuxDataBit equ 5
fKBS_UnLock	equ 00010000b	    ;	keyboard not locked
fKBS_UnLockBit	equ 4
fKBS_Cmd	equ 00001000b	    ;	last output was to pcmd_Kybd
fKBS_CmdBit	equ 3
fKBS_Sys	equ 00000100b	    ;	system flag bit
fKBS_SysBit	equ 2
fKBS_Bsy	equ 00000010b	    ;	data on command port not read
fKBS_BsyBit	equ 1
fKBS_DAV	equ 00000001b	    ;	data available on data port
fKBS_DAVBit	equ 0

; mask for Virtual status bits
fKBS_VMask  equ (fKBS_DAV + fKBS_Bsy + fKBS_Cmd)

; mask for Real status bits
fKBS_RMask  equ NOT (fKBS_VMask)

;
; 8042 cmd byte bits
;
fKBC_Int		equ	00000001b	; output buffer full int ON
fKBC_IntBit		equ	0
fKBC_AuxInt		equ	00000010b	; auxiliary int ON
fKBC_AuxIntBit		equ	1
fKBC_Sys		equ	00000100b	; system flag
fKBC_SysBit		equ	2
fKBC_OverLock		equ	00001000b	; override security lock
fKBC_OverLockBit	equ	3
fKBC_Dis		equ	00010000b	; disable keyboard
fKBC_DisBit		equ	4
fKBC_AuxDis		equ	00100000b	; disable PS2 aux
fKBC_AuxDisBit		equ	5
fKBC_PCCnvrt		equ	01000000b	; PC compatibility mode
fKBC_PCCnvrtBit 	equ	6

pdata_Kybd  equ 60h		    ; Keyboard data port
; special data returned from keyboard
KBD_Rsp_Ack	equ 0FAh		    ;	 Ack
KBD_Rsp_Ech	equ 0EEh		    ;	 Echo
KBD_Rsp_Rsn	equ 0FEh		    ;	 Resend
KBD_Rsp_Rst	equ 0AAh		    ;	 Reset confirmation
; Commands sent to keyboard
KBD_Cmd_LED	equ 0EDh		    ;	 Set LED
KBD_Cmd_Ech	equ 0EEh		    ;	 Echo
KBD_Cmd_N1F	equ 0EFh		    ;	 First NOP in 1st range
KBD_Cmd_N1L	equ 0F2h		    ;	 Last NOP in 1st range
KBD_Cmd_Rpt	equ 0F3h		    ;	 Set key repeat rate
KBD_Cmd_Enb	equ 0F4h		    ;	 Enable keyboard
KBD_Cmd_Dis	equ 0F5h		    ;	 Disable keyboard
KBD_Cmd_Def	equ 0F6h		    ;	 Set Default
KBD_Cmd_N2F	equ 0F7h		    ;	 First NOP in 2nd range
KBD_Cmd_N2L	equ 0FDh		    ;	 Last NOP in 2nd range
KBD_Cmd_Rsn	equ 0FEh		    ;	 Resend
KBD_Cmd_Rst	equ 0FFh		    ;	 Reset

; Command equates specific to Gravis Pheonix joystick:
;
KBD_Cmd_Gravis	    equ 093h		    ; Special command prefix
KBD_Gravis_Start    equ 1		    ; start programming sequence
KBD_Gravis_Test     equ 3		    ; initiate self test
KBD_Gravis_Clear    equ 5		    ; clear and previous state
					    ;	(this is a guess, when they are
					    ;	 about to start programming they
					    ;	 send this command followed by
					    ;	 the 93/1 command)

;******************************************************************************
;
;scan codes for shift keys
;
SC_RShf 	equ	36h		; Right shift scan code
SC_LShf 	equ	2Ah		; Left shift scan code
SC_Ctrl 	equ	1Dh		; Control shift scan code
SC_Alt		equ	38h		; Alt shift scan code
SC_ScLok	equ	46h		; Scroll lock scan code
SC_NmLok	equ	45h		; Num lock scan code
SC_CpLok	equ	3Ah		; Caps lock scan code
SC_Insrt	equ	52h		; Insert scan code
SC_Enter	equ	1Ch		; Enter scan code
SC_PrtScr	equ	37h		; Print Screen scan code = "*" scan code
SC_Escape	equ	1h		; Escape scan code
SC_Pause	equ	45h		; Pause scane code
SC_NumSlash	equ	35h		; "/" on numpad

SC_Del		equ	53h		; Del key scan code
SC_Ext0		equ	0E0h		; Extended K/B shift
SC_Ext1		equ	0E1h		; Extended K/B shift

SC_Break	equ	80h		;   Bit to set for break code
bSC_Break	equ	7

SC_Num0 	equ	52h
SC_Num1 	equ	4Fh
SC_Num2 	equ	50h
SC_Num3 	equ	51h
SC_Num4 	equ	4Bh
SC_Num5 	equ	4Ch
SC_Num6 	equ	4Dh
SC_Num7 	equ	47h
SC_Num8 	equ	48h
SC_Num9 	equ	49h


; BIOS shift state bits
		      ;;5432109876543210
BSS_LShift	equ	SS_LShift
BSS_RShift	equ	0000000000000001b
BSS_Ctrl	equ	SS_LCtrl
BSS_Alt 	equ	SS_LAlt
BSS_CapLock	equ	SS_CapLock
BSS_NumLock	equ	SS_NumLock
BSS_ScrlLock	equ	SS_ScrlLock
BSS_CapLock_Dn	equ	SS_CapLock_Dn
BSS_NumLock_Dn	equ	SS_NumLock_Dn
BSS_ScrlLock_Dn equ	SS_ScrlLock_Dn

		       ;7654321076543210
BSS_Unused	equ	0000111100000000b

SS_CapLock_Bit	equ	6
SS_NumLock_Bit	equ	5
SS_ScrlLock_Bit equ	4
.errnz SS_CapLock  XOR 0000000001000000b
.errnz SS_NumLock  XOR 0000000000100000b
.errnz SS_ScrlLock XOR 0000000000010000b

BIOS_Shift_State_Loc	equ 417h
BIOS_Mode_State_Loc	equ 496h
BIOS_head_loc		equ 41Ah
BIOS_tail_loc		equ 41Ch
BIOS_buf_start		equ 480h
BIOS_buf_end		equ 482h

; flags for VKD_flags
VKDf_HK_hold		equ 00000001b	; set if in hot key hold state
VKDf_HK_hold_bit	equ	   0
VKDf_build_guard	equ 00000010b	; set if entered VKD_Get_Key, used to
VKDf_build_guard_bit	equ	  1	;   lock out re-entering
VKDf_wait_cmd_res	equ 00000100b	; set if waiting for 8042 cmd response
VKDf_wait_cmd_res_bit	equ	 2
VKDf_HK_event		equ 00001000b	; set if priority event has been
VKDf_HK_event_bit	equ	3	;   scheduled
VKDf_initialized	equ 00010000b	; set at end of device_init
VKDf_initialized_bit	equ    4	; set at end of device_init

;----

VK_Extended	= 8000h 	; extended key (E0 scan code - as in Right ALT
				;     Right CTRL, cursor pad keys, etc.)
VK_Extended_B	= VK_Extended SHR 8   ; byte value
VK_Extended2	= 4000h 	; 2nd extended key (E1 scan code) used only
				;     for PAUSE
VK_Extended2_B	= VK_Extended2 SHR 8  ; byte value


; Special single key hot keys (hot keys that don't require a shift state)

Escape_HK   equ SC_Escape
Pause_HK    equ VK_Extended OR SC_Pause
PrtScr_HK   equ VK_Extended OR SC_PrtScr


;
; structures for dealing with hot keys
;
Hot_Key_Def STRUC
IFDEF DEBUG
	Hot_Key_Self_ptr    dd	?   ; to validate hot key handle
ENDIF
	Hot_Key_scan_code   db	?   ; scan code
	Hot_Key_last_VK_fl  db	?   ; flags from last virtual key
	Hot_Key_extended    db	?   ; 0 - normal, 1 - scan code is extended
	Hot_Key_call_mask   db	?
	Hot_Key_SS_Mask     dw	?   ; shift state mask
	Hot_Key_SS_Compare  dw	?   ; shift state compare
	Hot_Key_Local_Flag  dd	?   ; 0 - if global key
				    ; else a # between 1 and 31 to indicate
				    ;	that the key can be locally disabled
	Hot_Key_call_back   dd	?   ; call back routine
	Hot_Key_max_elapsed dd	?   ; maximum elapsed time for delayed
				    ; notification, used if PriorityNotify
				    ; set in Hot_Key_call_mask
	Hot_Key_ref_data    dd	?   ; reference data
Hot_Key_Def ENDS

Hot_Key_Last_VK equ word ptr Hot_Key_scan_code
Hot_Key_Down	equ Local_Key	    ; reuse flag bit in Hot_Key_call_mask

Not_Local_Key equ   0h		    ; default value for Hot_Key_Local_Flag


Hot_Key_Notify_Struc  STRUC
hkn_handle	    dd	?	    ; handle of hot key
hkn_posttime	    dd	?	    ; system time before Call_Priority_VM_Event
hkn_scancode	    db	?
hkn_notification    db	?	    ; Hot_Key_Pressed, Hot_Key_Released or
				    ; Hot_Key_Repeated
hkn_gbl_shift_state dw	?
Hot_Key_Notify_Struc  ENDS

hkn_code_and_notify equ word ptr hkn_scancode

;------------------------------------------------------------------------------
; equates for 8042 command table & command virtualization
;
VKD_not_defined 	equ 00000000b
VKD_Ignore_In_All_VMs	equ 01000000b
VKD_Do_In_Focus_VM	equ 10000000b
VKD_Do_In_All_VMs	equ 11000000b
VKD_Type_Mask		equ 11000000b
VKD_Cnt_Mask		equ 00011111b

VKD_Read_Cmd		equ 00000000b
VKD_Write_Cmd		equ 00100000b

VKD_NulTerminated	equ 1Fh 	; optional repeat count value

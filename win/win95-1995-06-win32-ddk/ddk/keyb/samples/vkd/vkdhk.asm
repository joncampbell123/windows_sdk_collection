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
TITLE vkdhk.asm -
;******************************************************************************
;
;   Title:	vkdhk.asm
;
;   Version:	1.00
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

	INCLUDE VKD.INC
	INCLUDE VKDSYS.INC
	INCLUDE OPTTEST.Inc
.LIST

VxD_DATA_SEG

EXTRN	VKD_CB_Offset:DWORD
EXTRN	VKD_flags:DWORD
EXTRN	VKD_gbl_shift_state:DWORD


PUBLIC	Hot_Key_List
PUBLIC	Hot_Key_Notify_List

Hot_Key_List		dd  ?		; hot key list
Hot_Key_Notify_List	dd  ?		; notification list
last_hot_key_handle	dd  0		; handle of last received hot key

VKD_last_key		db  0		; last hot key entered

VxD_DATA_ENDS


VxD_Code_SEG

EXTRN Update_Shift_State:NEAR
EXTRN Simulate_VM_INT9:NEAR


;******************************************************************************
;
;   Chk_Hot_Keys
;
;   DESCRIPTION:    check for scan code and global shift state match to detect
;		    a user entered hot key combination
;
;		    Monitoring hotkeys are handled directly, because reporting
;		    them back to VKD_VM_Server_Part_2 would cause us to enter
;		    the hotkey state, which is precisely what monitoring
;		    hotkeys were designed not to do!
;
;   ENTRY:	    AX is the virtual key to check
;		    ESI is ptr to VKD data in VM's CB
;
;   EXIT:	    Carry set, if this is a valid hot key combination
;		    EBX is hot key handle
;
;   USES:
;
;==============================================================================
BeginProc Chk_Hot_Keys

	VK_HK_Queue_Out "Chk_Hot_Keys key #AX"

	push	eax
	test	[VKD_gbl_shift_state], SS_Shift_mask + SS_Toggle_Dn_mask
					    ;Q: are any shift state keys down?
	jnz	short chkhk_1		    ;	Y:
	and	eax, (NOT SC_Break AND 0FFh) OR VK_Extended
					    ; clear all but Extended bit &
					    ; base scan code

	cmp	eax, Pause_HK		    ;Q: special single key hot key?
	jz	DEBFAR je_don_t_check	    ;	N: (chained short jumps)

chkhk_0:
	mov	eax, [esp]		    ; retrieve saved eax value
chkhk_1:
IFDEF DEBUG
	push	ebx
	mov	ebx, [esp+8]
	VK_HK_Queue_Out 'Chk_Hot_Keys #ax, called from ?ebx'
	pop	ebx
ENDIF
	test	al, SC_Break		    ;Q: key being released?
	jz	short continue_check
	TestMem [VKD_flags], VKDf_HK_hold   ;Q: already in hot key state?
je_don_t_check:
	je	DEBFAR don_t_check	    ;	N: don't enter hot key state
					    ;	   on just the release!

continue_check:
	pushad
	mov	edi, esi		    ; point edi at CB data
	mov	cx, ax			    ; save virtual key
	mov	esi, [Hot_Key_List]
	VMMCall List_Get_First
	jz	short no_hot_keys	    ; jump if no hot keys defined

;
; if extended, is all we need to know
; so leave VK_Extended bit in modifier byte and NON SC_Break bits in the
; scan code byte
;
	and	cx, VK_Extended + (0FFh XOR SC_Break)
.errnz (VK_Extended + (0FFh XOR SC_Break)) - 807Fh

chk_key:
	cmp	cl, [eax.Hot_Key_scan_code] ;Q: scan codes match?
	jne	short no_key		    ;	N:
	cmp	[eax.Hot_Key_extended], AllowExtended_B ;Q: ignore extended flag?
	je	short accept_key	    ;		    Y:
	cmp	ch, [eax.Hot_Key_extended]  ;Q: same ext status?
	jne	short no_key		    ;	N:
accept_key:
	mov	ebx, [VKD_gbl_shift_state]
	and	bx, [eax.Hot_Key_SS_Mask]
	cmp	bx, [eax.Hot_Key_SS_Compare] ;Q: correct shift state?
	je	short hot_key_fnd	    ;	Y:
no_key:
	VMMCall List_Get_Next		    ; EAX is next key definition
	jnz	chk_key
no_hot_keys:
	clc				    ; failed!
	jmp	short chk_exit

hot_key_fnd:
	VK_HK_Queue_Out "Chk_Hot_Keys Hot key detected #eax"

	mov	ebx, [eax.Hot_Key_Local_Flag]
	bt	[edi.disabled_hot_keys], ebx ;Q: key disabled?
	jnc	short hot_not_disabled	    ;	N: match found

	test	byte ptr [esp.Pushad_EAX], SC_Break ;Q: release hot key?
IFDEF DEBUG
	jnz	SHORT D00_handle_up_on_disabled
	VK_HK_Queue_Out "Chk_Hot_Keys Hot key disabled"
	jmp	no_key
D00_handle_up_on_disabled:
ELSE
	jz	no_key
ENDIF
	and	[eax.Hot_Key_call_mask], NOT Hot_Key_Down   ; flag as key up
	mov	bl, byte ptr [esp.Pushad_EAX]
	mov	[VKD_last_key], bl
	jmp	no_key


hot_not_disabled:
	TestMem	[eax.Hot_Key_call_mask], Monitor_Key ; monitoring only?
	jnz	Hot_Key_Activate_Monitor    ; Y: Handle internally
	stc
	mov	[esp.Pushad_EBX], eax	    ; return handle of hot key

chk_exit:
	popad
don_t_check:
	pop	eax
	ret

EndProc Chk_Hot_Keys

VxD_Code_Ends

VxD_Pageable_Code_Seg

;******************************************************************************
;
;   Hot_Key_Activate_Monitor
;
;   DESCRIPTION:    A special "monitoring" hotkey has been pressed.
;		    Right now, Ctrl+C is the only monitoring hotkey.
;
;   ENTRY:	    EAX = hot key handle
;		    ESP.Pushad_EAX = scan code pressed or released
;
;   EXIT:	    jmp no_key to re-enter main Chk_Hot_Keys loop
;
;   USES:	    EBX, ECX, EDX, EBP, Flags
;		    Preserves EAX, ESI, EDI
;
;==============================================================================
BeginProc Hot_Key_Activate_Monitor

	xchg	ebx, eax		; EBX = hot key handle
	mov	al, byte ptr [esp].Pushad_EAX[0] ; AL = scan code
	mov	ah, Hot_Key_Pressed + Hot_Key_Monitor
	btr	eax, 7			; Key release? (and clear bit)
	jnc	@F
	add	ah, Hot_Key_Released - Hot_Key_Pressed
@@:	call	Hot_Key_Notify		; Notify (or schedule if necessary)
	xchg	ebx, eax		; EAX = hot key handle
	jmp	no_key

EndProc Hot_Key_Activate_Monitor

;******************************************************************************
;
;   Hot_Key_Entered
;
;   DESCRIPTION:    set special holding flag
;
;   ENTRY:	    EBX hot key handle
;		    AL is scan code of key
;
;   EXIT:
;
;   USES:	    flags
;
;==============================================================================
BeginProc Hot_Key_Entered

	VK_HK_Queue_Out "Hot_Key_Entered handle #AL, #EBX"

	push	eax
	push	ecx
	mov	ecx, ebx
	xchg	[last_hot_key_handle], ecx  ; save hot key handle & get old
	or	ecx, ecx		    ;Q: was there an old one?
	jz	short no_complete_notify    ;	N:
	cmp	ecx, ebx		    ;Q: same hot key?
	je	short no_complete_notify    ;	Y: possibly auto-repeat
new_hk:
	test	[ecx.Hot_Key_call_mask], CallOnComplete ;Q: req complete notification?
	jz	short no_complete_notify    ;	N:
	push	ebx			    ;	Y: call call-back
	mov	ebx, ecx
	VK_HK_Queue_Out 'Hot_Key_Entered forced complete on #ebx'
	mov	al, [ebx.Hot_Key_scan_code]
	mov	ah, Hot_Key_Completed
	call	Hot_Key_Notify
	ClrFlag [VKD_flags], VKDf_HK_hold   ; release from hold state
	pop	ebx

no_complete_notify:
	pop	ecx
	SetFlag [VKD_flags], VKDf_HK_hold   ; flag special hot key state
	xor	ah, ah			    ; assume Hot_Key_Pressed
.errnz Hot_Key_Pressed
	test	al, SC_Break		    ;Q: key released?
	jz	short hk_pressed	    ;	N:
	and	[ebx.Hot_Key_call_mask], NOT Hot_Key_Down   ; flag as key up
	test	[ebx.Hot_Key_call_mask], CallOnRelease
	jz	short hk_update_last	    ; jump if don't need to call
	inc	ah			    ;	Y:  AH = Hot_Key_Released
.errnz Hot_Key_Released - 1
	jmp	short hk_call
hk_pressed:
	or	[ebx.Hot_Key_call_mask], Hot_Key_Down	    ; flag as key down
	cmp	al, [VKD_last_key]	    ;Q: same as last?
	jne	short hk_new		    ;	N:
	test	[ebx.Hot_Key_call_mask], CallOnRepeat
	jz	short hk_update_last	    ; jump if don't need to call
	mov	ah, Hot_Key_Repeated	    ;	Y: must be auto-repeat
	jmp	short hk_call
hk_new:
	test	[ebx.Hot_Key_call_mask], CallOnPress
	jnz	short hk_call		    ; jump if do need to call
hk_update_last:
	mov	[VKD_last_key], al
	jmp	short hk_exit

hk_call:
	mov	[VKD_last_key], al	    ; save the scan code
	VK_HK_Queue_Out 'Hot_Key_Entered notify for #ebx'
	call	Hot_Key_Notify

hk_exit:
	pop	eax
	ret

EndProc Hot_Key_Entered

;******************************************************************************
;
;   Hot_Key_Notify
;
;   DESCRIPTION:    Call hot key call back to notify of a hot key event.
;		    If PriorityNotify was requested when the hot key was
;		    defined, then a priority event is scheduled, so that
;		    the call back procedure will only be called when VM
;		    ints are enabled, and the critical section is unowned.
;
;   ENTRY:	    AL = scan code of key
;		    AH = 0, if key just pressed 	    (Hot_Key_Pressed)
;		       = 1, if key just released	    (Hot_Key_Released)
;		       = 2, if key is an auto-repeat press  (Hot_Key_Repeated)
;		       = 3, if key is completed 	    (Hot_Key_Completed)
;		    EBX = hot key handle
;
;   EXIT:	    nothing
;
;   USES:	    flags
;
;==============================================================================
BeginProc Hot_Key_Notify

	pushad
	xor	edi, edi
	mov	ecx, [VKD_gbl_shift_state]
	test	[ebx.Hot_Key_call_mask], PriorityNotify
	jz	short call_now

; allocate a list element to record the notification information

	push	eax			    ; save scan code & flags
	mov	esi, [Hot_Key_Notify_List]
	VMMCall List_Allocate		    ; EAX = new hot key node
	mov	edx, eax		    ; pass list handle as reference data
	mov	[edx.hkn_handle], ebx	    ; save handle
	VMMCall Get_System_Time
	mov	[edx.hkn_posttime], eax     ; save handle
	pop	eax
	mov	[edx.hkn_code_and_notify], ax ; save scancode & notification
						; flag
	mov	[edx.hkn_gbl_shift_state], cx

	mov	eax, edx		    ; queue notification
	VMMCall List_Attach_Tail
	bts	[VKD_flags], VKDf_HK_event_bit	;Q: priority event already scheduled?
	jc	short hkn_exit		    ;	    Y: don't schedule another


; Call_Priority_VM_Event, to ensure that VM ints are enabled and the critical
; section is unowned - the event processor will then handle the real
; notification call!

	mov	eax, Low_Pri_Device_Boost
	mov	ecx, PEF_Wait_For_STI OR PEF_Wait_Not_Crit ;;OR PEF_Always_Sched
					    ; wait for ints enabled and
					    ; critical section unowned

IFDEF Schedule_Priority_on_Owner_VM
	mov	ebx, [VKD_Kbd_Owner]	    ; do on keyboard owner VM

	TestMem [ebx.CB_VM_Status], <VMStat_Not_Executeable OR VMStat_Suspended>
	jz	short sch_for_owner	    ; jump if owner still executable

;
; set flag in notification field indicating that the event is scheduled for
; the SYS VM and schedule the event
;
	or	[edx.hkn_notification], Hot_Key_SysVM_Notify
ENDIF

	VMMCall Get_Sys_VM_Handle	    ; do on SYS VM handle, so VM can't
					    ; be suspended
sch_for_owner:
	mov	esi, OFFSET32 VKD_PriorityNotify

	VK_HK_Queue_Out "Hot_Key_Notify Pri event scheduled"

	VMMCall Call_Priority_VM_Event
	jmp	short hkn_exit

call_now:
; pass information to call back
;   AL = scan code of key
;   AH = 0, if key just pressed 	    (Hot_Key_Pressed)
;      = 1, if key just released	    (Hot_Key_Released)
;      = 2, if key is an auto-repeat press  (Hot_Key_Repeated)
;   EBX is hot key handle
;   ECX = global shift state
;   EDX is reference data
;   EDI = elapsed time for delayed notification  (milliseconds)
;	    (normally 0, but if PriorityNotify is specified
;	     then this value could be larger)
;
	VK_HK_Queue_Out "Hot_Key_Notify notify now"

	mov	edx, [ebx.Hot_Key_ref_data]
	call	[ebx.Hot_Key_call_back]
hkn_exit:
	popad
	ret

EndProc Hot_Key_Notify


;******************************************************************************
;
;   Hot_Key_Ended
;
;   DESCRIPTION:    make sure that the last hot key pressed sent a release
;		    message to the registered call-back and exit the hot key
;		    state
;
;   ENTRY:	    ESI points to VKD data in VM's CB
;
;   EXIT:	    nothing
;
;   USES:	    EAX, EBX and flags
;
;==============================================================================
BeginProc Hot_Key_Ended

	VK_HK_Queue_Out "Hot_Key_Ended"

	xor	ebx, ebx
	xchg	ebx, [last_hot_key_handle]
	or	ebx, ebx		    ;Q: already ended?
	jz	hke_exit		    ;	Y:

	mov	al, [VKD_last_key]
	test	al, SC_Break		    ;Q: hot key released?
	jnz	short release_seen	    ;	Y:
	and	[ebx.Hot_Key_call_mask], NOT Hot_Key_Down   ; flag as key up
	or	al, SC_Break		    ;	N: fake the release!
	call	Hot_Key_Entered
	mov	[last_hot_key_handle], 0    ; zero what Hot_Key_Entered set

release_seen:
	ClrFlag [VKD_flags], VKDf_HK_hold   ; release from hold state

	test	[ebx.Hot_Key_call_mask], CallOnComplete ;Q: req complete notification?
	jz	short complete_not_req	    ;	N:
	mov	al, [ebx.Hot_Key_scan_code] ;	Y: call call-back
	mov	ah, Hot_Key_Completed
	VK_HK_Queue_Out 'Hot_Key_Ended notify for #ebx'
	call	Hot_Key_Notify

complete_not_req:
	push	edx
	mov	edx, esi
	sub	edx, [VKD_CB_Offset]
	Assert_VM_Handle edx
	movzx	eax, [esi.loc_shift_state]
	mov	ebx, [VKD_gbl_shift_state]
	call	Update_Shift_State
	call	Simulate_VM_INT9	    ; simulate the first byte
	pop	edx
hke_exit:
	ret

EndProc Hot_Key_Ended

;******************************************************************************
;
;   VKD_PriorityNotify
;
;   DESCRIPTION:    Handle call-back notification as a result of a
;		    Call_Priority_VM_Event
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================
BeginProc VKD_PriorityNotify

	VK_HK_Queue_Out "VKD_PriorityNotify"

	ClrFlag [VKD_flags], VKDf_HK_event

pn_get_event:
	mov	esi, [Hot_Key_Notify_List]
	VMMCall List_Get_First
	jz	short pn_exit		    ; jump if queue empty
	mov	edx, eax

; remove record from list

	VMMCall List_Remove
	mov	ebx, [edx.hkn_handle]
	mov	edi, [edx.hkn_posttime]
	movzx	ecx, [edx.hkn_code_and_notify]
	movzx	edx, [edx.hkn_gbl_shift_state]
	VMMCall List_Deallocate
	VMMCall Get_System_Time
	xchg	eax, edi
	sub	edi, eax		    ; edi = elapsed time
	mov	eax, ecx		    ; eax = scan code & flags
	mov	ecx, edx		    ; ecx = global shift state
	mov	edx, [ebx.Hot_Key_max_elapsed]
	or	edx, edx		    ;Q: max time set?
	je	short Call_Hot_Key_CallBack ;	N:
	cmp	edi, edx		    ;Q: too much time?
	ja	pn_get_event		    ;	Y:

; pass information to call back
;   AL = scan code of key
;   AH = 0, if key just pressed 	    (Hot_Key_Pressed)
;      = 1, if key just released	    (Hot_Key_Released)
;      = 2, if key is an auto-repeat press  (Hot_Key_Repeated)
;   EBX is hot key handle
;   ECX = global shift state
;   EDX is reference data
;   EDI = elapsed time for delayed notification  (milliseconds)
;	    (normally 0, but if PriorityNotify is specified
;	     then this value could be larger)
;
Call_Hot_Key_CallBack:
	VK_HK_Queue_Out "VKD_PriorityNotify calling notify proc"

	mov	edx, [ebx.Hot_Key_ref_data]
	call	[ebx.Hot_Key_call_back]
	jmp	pn_get_event

pn_exit:
	ret

EndProc VKD_PriorityNotify

VxD_Pageable_Code_Ends

VxD_Code_Seg

IFDEF DebugHKs
BeginProc VKD_Show_Test_HKs
	Trace_Out "LIST OF CURRENT TEST HOT KEYS"
	Trace_Out "    ScrolLock+A this test"
	Trace_Out "  1 NumLock+A"
	Trace_Out "  2 RAlt+A"
	Trace_Out "  3 LAlt+RAlt+A"
	Trace_Out "  4 LShft+RShft"
	Trace_Out "  5 Shft+Ins_CP (Cursor Pad)"
	Trace_Out "  6 Shft+Del"
	Trace_Out "  7 RShft"
	Trace_Out "  8 ScrlLk"
	ret
EndProc VKD_Show_Test_HKs

; test hot key call-back notifications
;		    AL = scan code of key
;		    AH = 0, if key just pressed 	    (Hot_Key_Pressed)
;		       = 1, if key just released	    (Hot_Key_Released)
;		       = 2, if key is an auto-repeat press  (Hot_Key_Repeated)
;		       = 3, hot key state ended 	    (Hot_Key_Completed)
;		    EBX is hot key handle
;		    ECX = global shift state
;		    EDX is reference data
;		    EDI = elapsed time for delayed notification  (milliseconds)
;			    (normally 0, but if PriorityNotify is specified
;			     then this value could be larger)
BeginProc VKD_Test_HK
	Trace_Out "#edx  #ax"
	ret
EndProc VKD_Test_HK

BeginProc VKD_Test_Msg_Mode
	mov	eax, Begin_Message_Mode
	VMMCall Get_Cur_VM_Handle
	call	VKD_Control
	ret
EndProc VKD_Test_Msg_Mode

BeginProc VKD_Test_Reflect_HK
	Trace_Out "#edx  #ax"
	or	ah, ah
	jnz	short trhk_exit
	or	edx, edx
	jnz	short reflect_hk
	VMMCall Get_Cur_VM_Handle
	mov	eax, [tst_key1]
	or	eax, eax
	jz	short trhk_skip_1
	VxDCall VKD_Local_Enable_Hot_Key
trhk_skip_1:
	mov	eax, [tst_key2]
	or	eax, eax
	jz	short trhk_exit
	VxDCall VKD_Local_Enable_Hot_Key
	jmp	short trhk_exit

reflect_hk:
	mov	[edx], ebx
	mov	eax, ebx
	VMMCall Get_Cur_VM_Handle
	VxDCall VKD_Reflect_Hot_Key
	VxDCall VKD_Local_Disable_Hot_Key

trhk_exit:
	ret
EndProc VKD_Test_Reflect_HK

ENDIF

VxD_Code_ENDS
END

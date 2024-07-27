;===========================================================================
;
; HotKey.asm -- ALT-TAB and ALT-ESC context switch code
;
; 		$Author:   rmh  $
; 		$Revision:   1.8  $
; 		$Date:   05 Nov 1987  0:57:36  $
;
;==========================================================================


; 
; Constants
;
.xlist
include	cmacros.inc
include tty.inc
include	macro.inc
.list


; 
; Imported Functions
;
externNP	UpdateAppTitle	
externNP	NextDestWindow
externNP	PrevDestWindow
externNP	ExecuteMacro
externNP	FinishSwitchMacro

;
; Data Segement
;
sBegin	DATA

externW		DestWindow		; HotKey
externW		hTTYWnd
externW		WindowList
externW		OurWindow
externW		OldApState		; The old app state (TTYWIN)

sEnd	DATA


;
; Code Segment
;

sBegin	CODE
	assumes	DS, DATA
	assumes CS, CODE
	
externW		GrabReq			; Menu
externB		fSwitch
externW		fHotKey

;**
;
; ProcessHotKey - When entering in hotkey mode (Alt-Tab)
;
;	HotKey mode is left either when the user releases the Alt
; 	or he presses a key other than Tab.
;  	This function is only called in bad apps.
;
; ENTRY
;
; EXIT
;	fSwitch and bHotSwitch might be set. DestWindow will be
; 	updated.
;
; USES
;   	C convention
;
	public	ProcessHotKey
ProcessHotKey	PROC 	NEAR
	push	di
	push	si
    ;
    ; Main HotKey loop: check for a key in keyboard buffer
    ;
HKloop:
	mov	ah, 1			; GetStatus
	int	16h			; BIOS keyboard handler
	jz	HKchk			; If there isn't a key, check fHotkey
	xor	ax, ax			; But there is, swollow the key...
	int	16h
	jmp	HKretsw			;  and switch.
    ;
    ; fHotKey is maintained by the int09hander.
    ; It is cleared when the user releases Alt. 
    ;	
HKchk:
    	mov	bx, cs:[fHotKey]
    	jmp	cs:HKJmp[bx]
HKJmp	dw	HKretsw			; HK_CLEAR: Alt is released, switch
	dw	HKloop			; HK_MODE: No action, just loop
	dw	HKnext			; HK_NEXT:
	dw	HKprev			; HK_PREV:
	    	
    ;
    ;   Set DestWindow to the next window
    ;
HKnext:    
    	mov	ax, 1
    	call	SetDestTitle
    	cCall	NextDestWindow		
	jmp	HKSetTitle
    	
    ;
    ;	Set DestWindow to the previous window
    ;
HKprev:    
	mov	ax, 1
	call	SetDestTitle
	cCall	PrevDestWindow	
	
HKSetTitle:	
	xor	ax, ax
	call	SetDestTitle
	mov	cs:[fHotKey], HK_MODE
	jmp	HKLoop
			
    ;
    ; We want to switch: Don't switch back to ourselves and
    ;			 don't switch when the application is dead.
    ;
HKretsw:
	mov	ax, DestWindow		; Are we switching to ourselves?
	cmp	ax, OurWindow
	je	HKret			;  Yes, don't do anything
	cmp	OldApState, SF_TERMINATED; Are we terminated?
	je	HKclose			;   Yes: close 
	mov	cs:[fSwitch], SW_TABTYPE;   No: enter a tab type switch
	mov	ax, SwitchOutID		;   and execute the switch macro
	cCall	ExecuteMacro, <ax>
	or	ax, ax			; Execute OK?
	jz	HKret			;  Yes, just return
	mov	ax, SwitchOutID		;  No, pretend it did.
	call	FinishSwitchMacro	
	jmp	HKret
    ;
    ; The tried to tab out of dead app. Just, exit 
    ;
HKClose:
	mov	OldApState, SF_CLOSED	; ShowTerminate will do the rest.

HKret:	
    ;
    ; Return: DoMenu will take down the application title.
    ;
	pop	si
	pop	di	
	ret
ProcessHotKey	ENDP

;***
;
; SetDestTitle -- Sets the menu tiltle to the DestWindow title
;  
; ENTRY
;	AX  - UpdateAppTitle mode
;
; EXIT
;
; USES
;	All
;	
;
SetDestTitle	proc	near
	push	cs
	pop	es			;get cs in es
	lea	di, [GrabReq]		;get pointer to grab request sturcture
	mov	si, [DestWindow]
	shl	si, 1
	add	si, WindowList
	mov	si, [si]
	add	si, 4
	call	UpdateAppTitle
	ret
SetDestTitle	endp
		
	
cProc	GetFSwitch, <PUBLIC,NEAR>
cBegin
	mov	al, cs:[fSwitch]
	xor	ah, ah
cEnd

sEnd	CODE
end

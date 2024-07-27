;------------------------------ Module Header ------------------------------;
; Module Name: Timer interface procedures
;
; Created: ??-???-83
;
; Copyright (c) 1983, 1984, 1985, 1986, 1987  Microsoft Corporation
;
; History:
;  10-Jan-87 by ****  Adusted StackBase to be even
;   9-Jan-87 by ****  Patches to support HiTime.asm
;---------------------------------------------------------------------------;

	TITLE	Timer interface procedures

?DF=1
include system.inc
createSeg _TEXT,code,word,public,CODE

; Interrupt vector to use
;
vector	    equ 08h

sBegin	CODE	    ; Beginning of code segment
assumes CS,CODE

		PUBLIC	timerTable
timerTable	LABEL	BYTE
tiblock     <-1,0,0>
tiblock     <-1,0,0>
tiblock     <-1,0,0>
tiblock     <-1,0,0>
tiblock     <-1,0,0>
tiblock     <-1,0,0>
tiblock     <-1,0,0>
tiblock     <-1,0,0>
	    DW	    -1
	    DW	    -1

enabled 	DB  0		; 0 means int 8 hook not installed
				; 1 means int 8 hook installed
				; >1 means inside our int 8 hook


	public	StackBase
		EVEN		; Put the stack at a word boundary!
StackBase	DB  64	DUP (-1)

		PUBLIC	prevInt8Proc,lpTimerTable,prevSSSP,enabled,AT_DOS30
		PUBLIC	cms, cmsRound

		DB  128 DUP (?)
int8stack	LABEL	BYTE	; Stack to use inside our int 8 hook

prevInt8Proc	DD  0		; Previous int 8 interrupt handler
prevSSSP	DD  0		; Previous stack when inside our hook
cms		DD  0		; msec count.
cmsRound	DW  0		; for rounding off the msec count.

sEnd	CODE

sBegin	CODE	    ; Beginning of code segment
assumes CS,CODE

AT_DOS30	dw  1		; = 0 if running under DOS 3.0 on AT
lpTimerTable	DD  timerTable	; Far address of timer table

;--- timer hardware service -----------------------
;
noevent:
	jmp	CS:[prevInt8Proc]

	    PUBLIC  timer_int

;----------------------------- Private Function ----------------------------;
;
; Entry:	call	far ptr timer_int
;
; Returns:	nothing
;
; Registers Destroyed: none
;
; History:
;  09-Jan-87 by ****  Added hooks for the high resolution timer fns
;		      in hitime.asm
;  ??-???-?? by ????  Wrote it
;---------------------------------------------------------------------------;

timer_int   PROC    FAR
;
; Don't trash any registers.
;
	add	word ptr cs:[cms],(res_low / 1000)
	adc	word ptr cs:[cms+2],0
	add	cs:[cmsRound],(res_low - ((res_low / 1000) * 1000))
	cmp	cs:[cmsRound],1000
	jb	ti0
	sub	cs:[cmsRound],1000
	inc	word ptr cs:[cms]
	jnz	ti0
	inc	word ptr cs:[cms+2]
ti0:

	cmp	byte ptr cs:[AT_DOS30],0  ; if zero, then we are in an AT
	jz	NoEvent 		  ; running DOS 3.0 and doing a hard
					  ; disk operation, better go quick
	cmp	CS:[enabled],1
	jne	noevent
	inc	CS:[enabled]
	pushf
	call	CS:[prevInt8Proc]	; call previous Int 8 routine
	public	ti1
ti1:
	cli
	mov	word ptr CS:[prevSSSP+2],ss
	mov	word ptr CS:[prevSSSP],sp
	push	cs
	pop	ss
	mov	sp,codeOffset int8stack
	sti			    ; Allow interrupts
	push	ax

	mov	al,00001011b	; ask for 8259 status
	out	20H,al
	jmp	$+2
	in	al,20H		; get the status
	or	al,al
	jnz	TheEnd		; if other pending EOIs, just exit

	push	bp
	push	ds
	push	es
	push	bx
	push	cx
	push	dx
	push	si
	push	di

	xor	bp,bp		    ; No valid BP chain
	lds	si,CS:[lpTimerTable]
nextent:
	cld
	lodsw			    ; Get timer rate
	errnz	<tirate>
	inc	ax		    ; -1 means unused entry
	jnz	checkent	    ; no, check used entry
	lodsw			    ; yes, get timer count
	errnz	<2-ticount>
	inc	ax		    ; another -1 means end of table
	jz	lastent 	    ; yes, all done
	add	si,4		    ; o.w. skip to next entry
	jmp	nextent
checkent:
	dec	ax		    ; 0 means call at maximum rate
	jz	callent
	dec	word ptr DS:[si]    ; o.w. decrement rate counter
	errnz	<2-ticount>
	jz	callent 	    ; zero means timer has gone off
	add	si,6		    ; o.w. skip to next entry
	jmp	nextent
callent:
	mov	DS:[si],ax
	inc	si
	inc	si
	lea	ax,[si-4]	    ; Pass timer handle in AX
	errnz	<4-tiproc>
	call	dword ptr DS:[si]
	add	si,4
	jmp	nextent
lastent:
	pop	di
	pop	si
	pop	dx
	pop	cx
	pop	bx
	pop	es
	pop	ds
	pop	bp
TheEnd:
	pop	ax
	cli
	dec	CS:[enabled]
	mov	ss,word ptr CS:[prevSSSP+2]
	mov	sp,word ptr CS:[prevSSSP]
	public	ti2
ti2:
	iret
timer_int   ENDP


;============================================================================
; DWORD GetSystemMsecCount(void) - returns msec count.
;
LabelFP <PUBLIC, GetSystemMsecCount>
	mov	ax,word ptr cs:[cms]
	mov	dx,word ptr cs:[cms+2]
	retf

;
; EnableSystemTimers() - enable hardware timer interrupts
;

;----------------------------- Private Function ----------------------------;
;
; Entry:    cCall   far ptr EnableSystemTimers
;
; Returns:  nothing
;
; Registers Destroyed:	??
;
; History:
;  09-Jan-87 by ****  Patched to support hitime.asm
;  ??-???-?? by ????  Wrote it
;---------------------------------------------------------------------------;

cProc	EnableSystemTimers,<FAR,PUBLIC>
cBegin	nogen
; All done if just already enabled
	cmp	[enabled],0
	jne	edone

; Save away current timer interrupt vector value
	mov	ah,35h
	mov	al,vector
	int	21h
	mov	WORD PTR CS:[prevInt8Proc],bx
	mov	WORD PTR CS:[prevInt8Proc+2],es

; Setup timer interrupt vector to point to our interrupt routine
	mov	ah,25h
	mov	al,vector
	push	ds
	push	cs
	pop	ds
	mov	dx,codeOFFSET timer_int
	int	21h
	pop	ds
	mov	[enabled],1
edone:	ret
cEnd	nogen


;
; DisableSystemTimers() - disable system timer interrupts, restoring
; the previous timer interrupt handler.
;
cProc	DisableSystemTimers,<FAR,PUBLIC>
cBegin	nogen
; Do nothing if not enabled
	cmp	[enabled],0
	je	ddone
	mov	[enabled],0

; Restore the timer interrupt vector to point to previous value

	mov	ah,25h
	mov	al,vector
	push	ds
	lds	dx,CS:[prevInt8Proc]
	int	21h
	pop	ds
ddone:
	ret
cEnd	nogen

cProc	CreateSystemTimer,<PUBLIC,FAR>
ParmW	rate
ParmD	lpproc
cBegin
	lds	bx,[lpTimerTable]
	mov	ax,rate
	or	ax,ax
	jz	ctfirst
	mov	cx,1000 		; change msecs into ticks.
	mul	cx
	mov	cx,res_low
	div	cx
ctfirst:
	cli				; beginning of critical section
ctloop:
	cmp	DS:[bx].tirate,-1
	jne	ctnext
	cmp	DS:[bx].ticount,-1
	je	ctfail
	mov	cx,OFF_lpproc
	mov	dx,SEG_lpproc
	mov	word ptr DS:[bx].tiproc,cx
	mov	word ptr DS:[bx+2].tiproc,dx
	mov	DS:[bx].ticount,ax
	mov	DS:[bx].tirate,ax	   ; Set this last
	jmp	ctexit
ctnext:
	add	bx,SIZE tiblock
	jmp	ctloop
ctfail:
	xor	bx,bx
ctexit:
	sti				   ; end of critical section
	mov	ax,bx
	mov	cx,bx
cEnd

cProc	KillSystemTimer,<PUBLIC,FAR>,<di>
    parmW   htimer
cBegin
	les	di,[lpTimerTable]
	mov	ax,htimer
ktloop:
	cmp	ES:[di].tirate,-1
	jne	ktmatch
	cmp	ES:[di].ticount,-1
	jne	ktnext
	jmp	ktexit
ktmatch:
	cmp	di,ax
	jne	ktnext
	cld
	mov	ax,-1
	stosw
	not	ax
	stosw
	stosw
	stosw
	jmp	ktexit
ktnext:
	add	di,SIZE tiblock
	jmp	ktloop
ktexit:
	mov	cx,ax
cEnd
;
;  Sleazy routine called by user to see if its okay to take a long time
;  ( >2 msec ) to process an interrupt event.
;
LabelFP <PUBLIC, InquireLongInts>
	mov	ax,cs:[AT_DOS30]
	mov	cx,ax
	retf

sEnd	CODE		; End of code segment

END

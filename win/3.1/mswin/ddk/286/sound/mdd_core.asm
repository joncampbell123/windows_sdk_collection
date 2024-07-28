        TITLE   MDD_CORE - Interface for Multivoice Music Device Driver
?PLM = 1
?WIN = 1

XDDEF = 0

.xlist
include cmacros.inc
include sound.inc
.list

externA __WinFlags

;-----------------------------------------------------------------------
;
;                               Data
;
;-----------------------------------------------------------------------

sBegin DATA

PUBLIC	clk_tics
PUBLIC  int_length, in_tic_toc
PUBLIC	$music
PUBLIC  qcblock
PUBLIC	$sndtim
PUBLIC	tew_flags, tt_installed

qcblock que_ctrl_blk <> ;Voice 1 (Music)
clk_tics   DB	0	;Clock tic modulo counter
in_tic_toc DB   0       ;in tic_toc interrupt routine flag
                        ; 0 indicates not in tic_toc
                        ; 1 indicates in tic_toc
int_length DW	0	;Length of interstice (non 0 indicates we're playing notes)
$music	   DB	0	;music currently active or not byte
$sndtim    DW	0	;one word reserved for SNDTIM 0
tew_flags  DW	0	;Threshold Event Word Flags
			; bit 0 = 0 indicates voice 1 queue above threshold
			; bit 0 = 1 indicates voice 1 queue below threshold
tt_installed DB 0       ;Tic_Toc Interrupt handler active or not flag
                        ; 0 indicates tic_toc not installed
                        ; 1 indicates tic_toc installed
                        ; 2 indicates tic_toc installed but may be de-installed
globalB fReEnter,0	; used as a semaphore in DoBeep
globalW win_flags,__WinFlags

if ROM
globalD     rom_tod,0
endif

sEnd    DATA

sBegin  CODE

assumes CS,CODE
assumes DS,DATA
assumes ES,nothing


ife ROM
; Code Segment Variables

        PUBLIC  rom_tod
rom_tod DD	?	;vector to ROM time of day interrupt routine
endif


;-----------------------------------------------------------------------
;
;                   Interrupt Vectors for the IBM PC
;
;-----------------------------------------------------------------------
;
;  Before Installation of Music Device Driver:
;
;                   +-------------------+
;                   |                   |
;       INT_8 ----->|  ROM_Time_Of_Day  |
;                   |     18.2 / s      |
;                   |                   |
;                   +-------------------+
;
;  After Installation of Music Device Driver but before Music Plays:
;
;                   +-------------------+
;                   |                   |
;       INT_8 ----->|  ROM_Time_Of_Day  |
;                   |     18.2 / s      |
;       rom_tod --->|                   |
;                   |                   |
;                   +-------------------+
;
;  After Installation of Music Device Driver and as Music Plays:
;
;                   +-------------------+
;                   |                   |
;       INT_8 ----->|    MDD_tic_toc    |
;                   |     572.4 / s     |
;                   |                   |
;                   |              JMP -|--------+
;                   |                   |        |
;                   +-------------------+        |
;                                                |
;                   +-------------------+        |
;                   |                   |        |
;       rom_tod --->|  ROM_Time_Of_Day  |<-------+
;                   |     18.2 / s      |
;                   |                   |
;                   +-------------------+
;
;-----------------------------------------------------------------------


;-----------------------------------------------------------------------
;
; ini_que
;
; This routine is called whenever one of the
; queues is no longer required and can be
; initialized to the same condition as mdd_iniq.
;
; Each queue is initialized as follows:
;
;	  queue control 	  queue:
;		 block:
;			       +--------+
;			  +--->|  high	|
;	    quenote = 0   |    | memory |
;	    quetop -------+    |	|
;	    quenum = 0	       |	|
;	    queget ------+     |	|
;	    queput ----+ |     |	|
;		       | |     |  low	|
;	    queseg:0---+-+-+-->| memory |
;	    qthresh = p        +--------+
;
; Note:   This routine must not alter the threshold
;	  event detection value for this queue.
;
; Entry:
;	BX => queue control block
;
; Returns:
;	AX = 0
;
; Registers Modified:
;	none
;
; History:
;
;  Wed 14-Dec-1988 12:28:39  -by-  David N. Weise  [davidw]
;  Cleaned this sucker up!
;-----------------------------------------------------------------------

cProc	ini_que,<PUBLIC,NEAR>
cBegin nogen
	xor	ax,ax
	mov	[bx].queget,ax		; init get vector for this queue
	mov	[bx].queput,ax		; init put vector for this queue
	mov	[bx].quenote,ax 	; init number of notes
	mov	[bx].quenum,ax		; init number of bytes in queue
	ret
cEnd nogen


;-----------------------------------------------------------------------
;
; get_q_seg
;
; This routine gets the segment location of
; the queue for the given voice, and the address
; of the queue control block for this voice.
;
; Entry:
;	none
;
; Exit:
;	BX = qcblock
;	ES = [BX].queseg
;	DS:BX points to queue control block
;	ES:0000 points to this queue
;
; Registers Modified:
;	none
;
; History:
;
;  Wed 14-Dec-1988 12:28:39  -by-  David N. Weise  [davidw]
;  Cleaned this sucker up!
;-----------------------------------------------------------------------

        PUBLIC  get_q_seg

get_q_seg:
	mov	bx,OFFSET qcblock	; queue control block
	mov	es,[bx].queseg		; Set ES to the queue segment location.
	ret


;-----------------------------------------------------------------------
;
;       Name:           tic_toc
;
;       Purpose:        The timer interrupt, vectors here BEFORE updating the
;                       time of the day. This routine helps support the
;                       Music routines. Tic_toc keeps decrementing the duration
;                       count until it becomes zero at which point it will call
;                       next_sound to get the next sound or turn off the speaker.
;
;       2. TIC_TOC.... The TIMER interrupt vectors here before
;                       updating the time of the day. Normal
;                       frequency of the timer interrupt is 18.2
;                       times per second. This frequency is changed
;                       so that it interrupts at 572.4 times per
;                       second (exactly 32 times faster). This increase
;                       in speed gives better music performance. The
;                       basic philosophy in handling music is described
;                       below :
;
;               Music is composed of notes and a note consists of:
;               1. Frequency
;               2. Duration
;               3. Volume
;                       The frequency is loaded into the 8253 timer
;               chip or the TI chip as the case may be. The duration
;               is saved in some memory location and each time the
;               timer interrupts, we decrement the duration by 1 and
;               when the duration becomes zero we are all done. It is
;               important to note that the 8253 operates in the mode
;               wherein it reloads itself each time it counts down to
;               zero. In case of the PC Junior machine which supports the
;               TICHIP we have another parameter namely Volume which
;               gets loaded into the TICHIP at the same time as
;               frequency. Handling the TICHIP is rather tricky and
;               the PC Junior technical reference manual discusses it in
;               detail. Another good reference for the TI SOUND CHIP
;               is the "BYTE MAGAZINE, JULY 1982".
;               We have to remember one important thing, since we
;               change the frequency of the TIMER interrupt whenever
;               music is playing, we should see to it that the actual
;               timer interrupt (one which updates the time of the day)
;               gets called once every 32 times our timer interrupt
;               gets called. The figure 32 comes from the fact that
;               the TIMER is 32 times as fast as what it used to be.
;               Also when music stops playing we SHOULD change the
;               timer frequency back to its original rate.
;               The figure below illustrates how the TIMER interrupt
;               ( INT 8H ) is handled :
;
;                 Timer INT
;                  vectors
;                ------------        IF MUSIC      --------------
;               |     -------|------------------->| TIC_TOC      |
;                ------------    |     ACTIVE     |    Interrupt |
;               |    CS      |   |                |   service    |
;                ------------    |                |   routine for|
;                                |                |   handling   |
;                                |                |   music      |
;                                |                |     _________|______
;                                | ELSE IF         --------------       |
;                                | MUSIC NOT                            |
;                                | ACTIVE                               |
;                                |                                      |
;                                |                 ------------         |
;                                 --------------->| ROM Timer  |        |
;                                                 | interrupt  |<-------
;                                                 | service    |
;                                                 | routine    |
;                                                 | updates    |
;                                                 | time of    |
;                                                 | day.       |
;                                                 |            |
;                                                 | (F000:FEA5)|
;                                                  ------------
;
;               The routine TIC_TOC calls on the following routines:
;
;               1. next_sound.... This looks to see if any more entries
;                             are present in the sound queue and if so
;                             calls get_sound else shuts off voices.
;               2. get_sound.... This just pulls out an entry from the
;                             appropriate sound queue. It is evident
;                             that the notes are stored in a QUEUE.
;
;       Entry:          none
;
;       Exit:           none
;
;       Modified:       none
;
;-----------------------------------------------------------------------

        PUBLIC  tic_toc

tic_toc:
        PUSH    AX
        PUSH    BX
        PUSH    CX
        PUSH    DX
        PUSH    SI
        PUSH    DI
        PUSH    DS
	PUSH	ES
	MOV	AX,_DATA		;get music device driver's data seg
	MOV	DS,ax			;make ES same as DS!!!!!!!
        MOV     ES,AX                   ; necessary ?????????????
        MOV     in_tic_toc,1            ;we're in tic_toc now
        XOR     AX,AX
        MOV     SI,AX                   ;[SI] = voice Id*2
        CMP     $sndtim[SI],AX          ;has SND_TIM zeroed out?
        JZ      nxtone                  ;Brif so to get next sound
        DEC     $sndtim[SI]             ;SND_TIM - 1
        JNZ     clk_tic                 ;
nxtone:
        CALL    next_sound              ;get next sound
clk_tic:
        MOV     in_tic_toc,0            ;we're no longer in tic_toc
        DEC     clk_tics                ;clock tick -1
        AND     clk_tics,1FH            ;since 32 times as fast
        POP     ES
        POP     DS
        POP     DI
        POP     SI
        POP     DX
        POP     CX
        POP     BX
        JNZ     clktix                  ;do ROM clock INT once every 32 times
        POP     AX
IFE	debug
IF	ROM
	push	ds			; save ds
	push	ax
	mov	ax, _DATA		; setup ds
	mov	ds, ax
	pop	ax
	push	word ptr rom_tod[2]	; push address to "return to"
	push	word ptr rom_tod[0]
	push	bp
	mov	bp, sp
	mov	ds, [bp+6]		; restore ds
	pop	bp
	retf	2			;Jump and pop DS
ELSE
	JMP	CS:rom_tod		;Jump to ROM clock INT service routine
ENDIF
                                        ; which will perform an IRET
ELSE
        JMP     clkret
ENDIF                                   ;IFE debug
clktix:
        MOV     AL,eoi                  ;send End-of-Interrupt
        pause                           ;make sure instruction fetch has occurred
	out	intao,AL		;to 8259
                                        ; - allows pending interrupts to be processed
        POP     AX
clkret:
IFE debug
        IRET
ELSE
        RET
ENDIF ;IF debug


;-----------------------------------------------------------------------
;
; next_sound
;
; This routine supports the interrupt routine tic_toc. It
; either gets the next sound from the appropriate sound
; queue by calling get_sound or else it turns off the voice
; by calling endvce.
;
; Entry:
;	none
;
; Exit:
;	none
;
; Registers Modified:
;	BX,DX
;
; History:
;
;  Wed 14-Dec-1988 12:28:39  -by-  David N. Weise  [davidw]
;  Cleaned this sucker up!
;-----------------------------------------------------------------------

        PUBLIC  next_sound

next_sound:
	push	cx
	push	ax
	call	get_q_seg		; DS:BX points to queue contol block
	cmp	[bx].quenum,0		; is queue empty?
	jz	endvce			; Brif to turn off the voice
	call	get_sound		; DX = Duration, CX = Freq

; Output frequency into the 8253 timer.

	mov	al,cl			; LSB of freq
	out	timer2,al
	mov	al,ch
	pause				; make sure instruction fetch has occurred
	out	timer2,al		; MSB of frequency
	mov	$sndtim,dx		; $snd_tim = Duration
	jmp	short nxtret

endvce: call	stop_music

nxtret: pop	ax
	pop	cx
	ret


;-----------------------------------------------------------------------
;
; get_sound
;
; This routine supports next_sound.
;
; It also supports play event trapping by setting the play
; event flag whenever the number of notes in the usic
; background queue goes below the number specified in the
; ON PLAY(n)... statement, provided play trapping is enabled.
; get_sound makes several calls to queue_out to get notes out of the
; music queue.
;
; Entry:
;	DS:BX = address of appropriate music control block
;	ES:0 points to queue
;
; Exit:
;      CX = frequency in Hz
;      DX = duration in clock tics
;
; Registers Modified:
;	AX,BX,CX,DX
;
; History:
;
;  Wed 14-Dec-1988 12:28:39  -by-  David N. Weise  [davidw]
;  Cleaned this sucker up!
;-----------------------------------------------------------------------

        PUBLIC  get_sound

get_sound:

; check to see if we're in the middle of a note/interstice

	mov	dx,int_length		; interstice length
	or	dx,dx			; see if there's any interstice to play
	jnz	get_int 		; brif interstice - don't get next note

; get next note from queue

	push	si
	mov	si,[bx].queget
	mov	cx,es:[si].nb_freq
	mov	dx,es:[si].nb_interstice
	mov	int_length,dx
	mov	dx,es:[si].nb_dur
	add	si,SIZE note_block
	cmp	si,[bx].quetop
	jnz	nwrdqu
	xor	si,si
nwrdqu:
	mov	[bx].queget,si
	dec	[bx].quenum
	pop	si

; CX = frequency
; DX = note duration

	or	cx,cx			; 0 frequency indicates no note
	jz	not_audible		; brif if there's a note

; general queue maintenance:

	dec	[bx].quenote		; decrement # of notes in queue

; check for play events

	push	dx
	mov	dx,[bx].qthresh 	; get the user defined threshold
	cmp	[bx].quenote,dx 	; notes in queue = threshold ?
	jnz	no_event		; Brif not
	or	tew_flags,01		; set flag for this event
no_event:
	pop	dx

	jmp	short gets_exit 	; and return to next_sound

not_audible:
	mov	dx,int_length		; get the interstice length

get_int:

; return an interstice (or pause) note to next_sound
; DX = int_length = length of pause

	mov	int_length,0		; zero out this value for next time
	mov	cx,12h			; default frequency (1.193180/65535)

gets_exit:
	ret


;-----------------------------------------------------------------------
;
; start_music
;
; This routine starts the music.
;
; It does the following things:
;
; 1. Change the interrupt vector to point at our handler.
; 2. Modify timer2 to interrupt 32 times faster
; 3. Turn on the speaker and start timer2.
;
; Entry:
;	none
;
; Exit:
;	none
;
; Registers Modified:
;	none
;
; History:
;
;  Wed 14-Dec-1988 12:28:39  -by-  David N. Weise  [davidw]
;  Cleaned this sucker up and protect mode'ized it!
;-----------------------------------------------------------------------

        PUBLIC  start_music

start_music:
	push	ax			; These are not necessary ?????
	push	bx			;
	push	cx			;
	push	dx			;
	disable 			; needed ?????
	in	al,MSKREG		; get IMR into [AL]
	or	al,01H			; mask out timer interrupt
	pause
	out	MSKREG,al		; write mask to IMR
	cmp	$music,1		; is music currently on?
	jz	strtmxt 		; Brif so

; This code was added to fix a bug in the DoBeep routine.  We must let
; DoBeep know when the sound driver is actually making sound.  DoBeep
; will not screw with the speaker in this case.  The sound driver can,
; however do things while DoBeep has the speaker going.  This is not
; fatal and will simply result in some funny sounds.  Without this fix,
; DoBeep could go into continuous beep mode and never turn the speaker
; off. (Sat 10-Oct-1987 : bobgu)

; Set the flag that DoBeep looks at so that he won't touch the speaker
; if we are playing something (it's rude to interrupt a performance...)

	inc	fReEnter		; inc the semaphore.

	mov	$music,1		; else make music currently active

	push	ds
	push	cs
	pop	ds
IFE     debug
	setvec	clkint/4,tic_toc	; modify timer2 interrupt vector
ENDIF					; IFE debug
	pop	ds
	mov	tt_installed,1		; tic_toc now installed
	mov	ax,2048 		; modify timer2 to interrupt
	out	TIMER0,al		; at 32 times the
	mov	al,ah			; original
	pause				; make sure instruction fetch has occurred
	out	TIMER0,al		; rate
	mov	al,SQUARE		; else set timer2 in square
	pause				; make sure instruction fetch has occurred
	out	TMRCMD,al		; wave mode
	pause				; make sure instruction fetch has occurred
	in	al,SPEAKER		; turn on the
	or	al,SPKRON		; speaker
	pause				; make sure instruction fetch has occurred
	out	SPEAKER,al
strtmxt:
	pause				; make sure instruction fetch has occurred
	mov	clk_tics,0
	in	al,MSKREG		; get IMR into [AL]
	and	al,0FEh 		; unmask timer interrupt
	pause				; make sure instruction fetch has occurred
	out	MSKREG,al		; write mask to IMR
	enable				; interrupts
	pop	dx
	pop	cx
	pop	bx
	pop	ax
IF debug
more_tics:
	call	tic_toc
	jmp	short more_tics
ENDIF ;IF debug
	ret

;-----------------------------------------------------------------------
;
; stop_music
;
; This function stops the music playing and flushes
; all remaining notes from all the queues.
;
; Entry:
;	none
;
; Exit:
;	none
;
; Registers Modified:
;	none
;
; History:
;
;  Wed 14-Dec-1988 12:28:39  -by-  David N. Weise  [davidw]
;  Cleaned this sucker up and protect mode'ized it!
;-----------------------------------------------------------------------

        PUBLIC  stop_music

stop_music:

        disable
	push	ax
	in	al,MSKREG		; get IMR into [AL]
	or	al,01h			; mask out timer interrupt
	pause
	out	MSKREG,al		; write mask to IMR
	call	$sndoff
	in	al,MSKREG		; get IMR into [AL]
	and	al,0FEh 		; unmask timer interrupt
	pause
	out	MSKREG,al		; write mask to IMR
	enable				; interrupts
	pop	ax
	ret				; returns with no errors

;-----------------------------------------------------------------------
;
; $sndoff
;
; This routine stops music and flushes the music
; queue(s). It resets the timer interrupt vector,
; resets the timer count and clears the flags
; VOICE i and SND_TIM i.
;
; Entry:
;	none
;
; Exit:
;	none
;
; Registers Modified:
;	none
;  Wed 14-Dec-1988 11:33:37  -by-  David N. Weise  [davidw]
; Cleaned this up and protect mode'ized it.
;-----------------------------------------------------------------------

        PUBLIC  $sndoff

$sndoff:
        disable
	push	ax
	push	bx
	push	cx
	push	ds			; ES must be same as DS for the
	pop	es			; STOS instructions that may follow

; This code was added to fix a bug in the DoBeep routine.  We must let
; DoBeep know when the sound driver is actually making sound.  DoBeep
; will not screw with the speaker in this case.  The sound driver can,
; however do things while DoBeep has the speaker going.  This is not
; fatal and will simply result in some funny sounds.  Without this fix,
; DoBeep could go into continuous beep mode and never turn the speaker
; off. (Sat 10-Oct-1987 : bobgu)

; Clear the flag that DoBeep looks at so that he can do beeps again

	xor	ax,ax
	cmp	$music,al
	jz	around			; didn't come through start_music
	dec	fReEnter		; dec the semaphore.
	mov	$music,al		; music is currently OFF
around:
	mov	$sndtim,ax		; reset SND_TIM 0
	mov	bx,OFFSET qcblock
	call	ini_que 		; re-initialize first music queue

	in	al,SPEAKER
	and	al,NOT SPKRON		; turn off speaker
	pause
	out	SPEAKER,al

	pause
	mov	ax,1Dh
	out	TIMER2,al
        pause
	mov	al,ah
	out	TIMER2,al

; Fri 02-Oct-1987 10:27:38
;
; The following fix was implemented to fix a bug wherein this
; routine was called from the timer interrupt.	In this situation,
; we would not unhook ourselves from the timer interrupt, and end
; up only calling about every two seconds, instead of 18 times a
; second.

	test	win_flags,1
	jnz	use_dos

	disable 			; Just make sure, enable is performed
	push	es
	xor	ax,ax
	mov	tt_installed,al 	; Show de-installed
	mov	es,ax
	mov	ax,word ptr rom_tod[0]
	mov	word ptr es:[tod_int*4][0],ax
	mov	ax,word ptr rom_tod[2]
	mov	word ptr es:[tod_int*4][2],ax
	pop	es
	jmp	short restore_timer

use_dos:
	push	dx
	push	ds
	lds	dx,rom_tod
	mov	ax,2508h
	int	21h
	pop	ds
	pop	dx
	mov	tt_installed,0		; now de-installed

restore_timer:
	xor	ax,ax
	out	TIMER0,al		; restore timer2 count
	pause
	out	TIMER0,al
	pop	cx
	pop	bx
	pop	ax
	clc				; clear carry to indicate no error
        enable
	ret


;-----------------------------------------------------------------------
;
; queue_note
;
; This routine calls a number of device dependent
; routines. These device dependent routines check
; for the validity of frequency, and duration.
; They also output these quantities to the 8253
; timer chip.
;
; It either queues in the whole note or doesn't
; queue at all.
;
; The interstice parameter is the duration of the
; short rest that follows all staccato and normal
; notes.
;
; Both duration parameters are understood to be in
; units of 2.5 milliseconds.
;
; Pauses have 0 note duration and 0 duration. Frequency
; may be ignored in this case.
;
; Entry:
;	BX = Frequency	      ;what about fractional part ?????
;	CX = Duration of note
;	DX = Duration of interstice if any
;
; Exit:
;
; Error Return:
;	CF = 1 indicates one of the following:
;	AL = err_qfull	 - No room in the queue or
;	   = err_freq	 - Unsupported frequency or
;
; Registers Modified:
;	none
;
; History:
;
;  Wed 14-Dec-1988 12:28:39  -by-  David N. Weise  [davidw]
;  Cleaned this sucker up!
;-----------------------------------------------------------------------

        PUBLIC  queue_note

queue_note:
	disable 			; don't want a partial note in the queue
	push	bx			; frequency
	call	get_q_seg		; ES:0000 points to queue
					; DS:BX points to queue control block
	mov	ax,[bx].queget		; is there enough room for another note ?
	cmp	ax,[bx].queput
	jnz	got_space
	cmp	[bx].quenote,0
	jz	got_space
	mov	al,err_qfull
	stc
	jmp	short queue_full
got_space:
	pop	ax			; frequency
	or	ax,ax			; 0 frequency ?
	jz	queue_pause		; brif rest/pause
	or	cx,cx			; 0 note duration?
	jz	queue_pause		; brif rest/pause

; AX = frequency
; BX = pointer to queue control block
; CX = note duration
; DX = interstice duration

	call	check_freq		; check frequency in AX
	jc	freq_err		; brif frequency error
	call	check_duration		; check note duration
interstice:
	xchg	cx,dx			; and also interstice
	call	check_duration		; check interstice duration

; AX = frequency      (may be 0 if pause)
; BX = pointer to queue control block
; CX = interstice duration
; DX = note duration  (will be ignored if frequency 0)

	inc	[bx].quenote		; everything's valid - update note counter

	push	si
	mov	si,[bx].queput		; get put PTR

	call	freq_to_tics		; turn frequency in [AX] into timer tics
	mov	es:[si].nb_freq,ax
	mov	es:[si].nb_dur,dx
	mov	es:[si].nb_interstice,cx
	add	si,SIZE note_block
	cmp	si,[bx].quetop
	jnz	nwrque
	xor	si,si
nwrque:
	mov	[bx].queput,si
	inc	[bx].quenum
	pop	si
	jmp	short all_queued

queue_pause:

; DX = duration of interstice (only valid parameter)

	xor	ax,ax			; ensure 0 frequency
	dec	ax			; by using an inaudible frequency
	jmp	short interstice

all_queued:
	clc				; return no errors

freq_err:				; CF set and AL = err_freq
	push	bx			; dummy push

queue_full:				; CF set and AL = err_qfull
	pop	bx			; clean up stack
        enable
	ret


;-----------------------------------------------------------------------
;
; check_duration
;
; This routine adjusts the duration, DX, so that
; it matches the the 8253 timer chip.  It multiplies
; the given duration by 1.5.  If there is an overflow,
; check_duration sets DX to 65535.
;
; Entry:
;	CX = duration (1 = 2.5 millisecs)
;
; Exit:
;	CX = CX * 1.5 if timer2 in action
;
; Registers Modified:
;	none
;
;-----------------------------------------------------------------------

        PUBLIC  check_duration

check_duration:
	push	ax
	mov	ax,cx			; copy duration for adjustment
	shr	ax,1			; divide by 2
	add	cx,ax			; duration = 1.5 * original duration
	jnb	durok			; Brif no overflow
	mov	cx,65535d		; else use maximum duration
durok:	pop	ax
	ret


;-----------------------------------------------------------------------
;
; check_freq
;
; This routine checks to see if the frequency is
; in the valid range ( 37 to 32767 ).
; If it finds the frequency to be invalid it returns
; with PSW.C set and [AL] set to the error code 3.
;
; Entry:
;	AX = frequency
;
; Returns:
;	AX = frequency (unchanged)
;
; Error Returns:
;	CF = 1
;	AL = err_freq
;
; Registers Modified:
;	none
;
;-----------------------------------------------------------------------

        PUBLIC  check_freq

check_freq:
	cmp	ax,37d			; check for valid frequency
	jb	frqerr			; if invalid return
	cmp	ax,32767d		; error code
	jna	frqret			; else return with carry clear
frqerr:
	mov	ax,err_freq		; AL = error code for bad frequency
	stc				; set carry to indicate error
	ret
frqret:
	clc				; clear carry to indicate no error
	ret


;-----------------------------------------------------------------------
;
; freq_to_tics
;
; This routine converts the frequency in Hz to clock
; tics required by the 8253 timer chip, the tics are
; calculated using the formula :
;
;   tics = clock frequency / given frequency
;
; The clock frequency is 1.19MHz.
;
; Entry:
;	AX = given frequency
;
; Exit:
;	AX = tics
;
; Registers Modified:
;	none
;
;-----------------------------------------------------------------------

	PUBLIC	freq_to_tics

freq_to_tics:
	push	dx
	push	cx
	mov	cx,ax			; given freq
	mov	dx,12h			; clock freq =
	mov	ax,34DCh		;  1.193180 MHz
	div	cx			; count = clock / frequency
	pop	cx
	pop	dx
	ret


;-----------------------------------------------------------------------
;
; notes_in_queue
;
; This routine returns the number of notes remaining in a queue.
;  If tic_toc is not installed, then we should report 0 notes.
;  If tic_toc is installed, then we should report the number of notes
;   in the queue or 1, which ever is higher.
;
; Entry:
;	none
;
; Exit:
;	BX = number of notes in this queue
;
; Registers Modified:
;	none
;
; History:
;
;  Wed 14-Dec-1988 12:28:39  -by-  David N. Weise  [davidw]
;  Cleaned this sucker up!
;-----------------------------------------------------------------------

        PUBLIC  notes_in_queue

notes_in_queue:
	xor	bx,bx
	cmp	tt_installed,bl
	je	no_notes
	mov	bx,OFFSET qcblock	; queue control block
	mov	bx,[bx].quenote 	; get the # of remaining notes
	or	bx,bx			; any notes left
	jnz	notes_left
	inc	bx			; add one if tic_toc still installed
no_notes:
notes_left:
	ret


sEnd    CODE

        END

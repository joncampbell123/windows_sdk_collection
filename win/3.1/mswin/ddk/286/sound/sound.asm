
        TITLE   MDD_CINT - C Interface for Multivoice Music Device Driver
?PLM = 1
?WIN = 1

.xlist
include cmacros.inc
include sound.inc
.list

;-----------------------------------------------------------------------
;
;       Microsoft Multivoice Music Device Driver
;
;       C User Interface to OEM Machine Interface Module
;
;       (C) 1985 Microsoft Inc. - Last Revision March 5, 1985
;
;-----------------------------------------------------------------------
;
;       This module provides the OEM independent portion of code for
;       the Microsoft Multivoice Music and Sound Device Driver.
;       This device driver is intended for multi-tasking environments
;       such as Microsoft Windows etc.
;
;       This module interfaces the C user with the OEM dependent
;       portion of the Multivoice Music and Sound Device Driver.
;       OEMs must link machine specific code with this module to
;       create a device driver (or library) accessible to C programs.
;
;-----------------------------------------------------------------------
;
;       Code Model:   +-----------------------------------------------+
;                     |                                               |
;                     |  MDD_TEST: An Application                     |
;                     |                                               |
;                     +-----------------------------------------------+
;                     |                                               |
;                     |  MDD_CINT: C User Interface to                |
;                     |            OEM Machine Interface              |
;                     |                                               |
;                     +--------------------------+                    |
;                     |                          |                    |
;                     |  MDD_IBM:                |                    |
;                     |   IBM PC Specific Code   |                    |
;                     |                          |                    |
;                     +-------------+------------+--------+-----------+
;                     |             |                     |           |
;                     |  Hardware   |       MS-DOS        |  MDD_WIN  |
;                     |             |                     |           |
;                     +-------------+---------------------+-----------+
;
;-----------------------------------------------------------------------


;-----------------------------------------------------------------------
;
;                               Data
;
;-----------------------------------------------------------------------

sBegin DATA

globalW  pid,0          ;process id of task accessing Music Device Driver
                        ; 0 indicates that this device is not in use
                        ; non 0 indicates device open

PUBLIC  qlocinfo
qlocinfo q_loc_block <> ; queue 1

externW qcblock 	;queue control block
externW tew_flags	;Threshold Event Word Flags
			; bit 0 = 0 indicates voice 1 queue above threshold
			; bit 0 = 1 indicates voice 1 queue below threshold

if ROM
externD rom_tod
endif

sEnd   DATA

sBegin CODE

assumes CS,CODE
assumes DS,DATA

externFP GetCurrentTask
externFP GlobalAlloc
externFP GlobalRealloc
externFP GlobalFree
externFP Yield
externFP AllocCStoDSAlias
externFP FreeSelector

externNP start_music
externNP stop_music
externNP ini_que
externNP notes_in_queue
externNP queue_note

ife ROM
; Code Segment Variables

externD rom_tod 		;vector to ROM time of day interrupt routine
endif


checkpid:
	cCall	GetCurrentTask		; get process id in AX
	cmp	ax,pid			; is it the current process?
        ret


;-----------------------------------------------------------------------
;
; MyOpenSound()
;
; OpenSound will open access to the music device.
; While the play device is opened it can not be
; opened by another process.
;
; OpenSound will return the number of music voices
; available to the user application.
;
; Error codes are returned as negative numbers.
;
; OpenSound will provide the application with a default
; music queue size of 32 notes per voice, where each
; note is assumed to take 7 bytes.
;
; Entry:
;
; Returns:
;	AX = 1
;
; Error Returns:
;
;	AX = erdvna - device not available
;	   = erofm - out of memory
;
; Registers Modified:
;	BX,CX,DX,ES
;
; History:
;
;  Wed 14-Dec-1988 12:28:39  -by-  David N. Weise  [davidw]
; Cleaned this sucker up!
;-----------------------------------------------------------------------

	assumes ds,data
	assumes es,nothing

cProc	MyOpenSound,<PUBLIC,FAR>,<di,si>
cBegin
        mov     ax,pid
	or	ax,ax			; is there another process currently
	jnz	devinuse		;  accessing the music device driver?
	cCall	GetCurrentTask		; get process id
	mov	pid,ax			;   and store for future reference
	or	ax,ax			; is it NULL - if so then os failure
	jz	devinuse

; Save the old time of day interrupt.
; First get a code alias to record the old one.

ife ROM
	cCall	AllocCStoDSAlias,<cs>
	push	ax			; for the FreeSelector below
	push	ds
	mov	ds,ax
	assumes ds,code
endif

	mov	ax,3500h or tod_int
	int	21h
	mov	word ptr rom_tod[0],bx
	mov	word ptr rom_tod[2],es

ife ROM
	pop	ds
	assumes ds,data
	cCall	FreeSelector		; argument is already on the stack
endif

; Get memory from OS for 32 notes per music queue

	mov	ax,DEFAULT_SQ_SIZE	; enough room for 32 notes (32*6)
	cwd
	regptr	dxax,dx,ax
        cCall   GlobalAlloc,<dx,dxax>   ; global fixed object
        or      ax,ax
	jz	internal_error
	mov	cx,ax			; for later

; Now set up some defaults for the voice.

	push	ds
	pop	es
	mov	di,OFFSET qlocinfo	; point to the queue control block
	stosw				; segment location for this queue
	.errnz	0 - q_segment
	mov	ax,DEFAULT_SQ_SIZE	; size of this queue
	stosw
	.errnz	2 - q_size
	mov	al,0120D		; default tempo for this queue
	stosb
	.errnz	4 - q_tempo
	xor	ax,ax			; normal mode for this queue
	.errnz	NORMAL
	stosb
	.errnz	5 - q_mode_nls
	stosb
	.errnz	6 - q_pitch
	.errnz	NORMAL

	mov	bx,OFFSET qcblock
	mov	[bx].qthresh,ax 	; default threshold is 0
	mov	[bx].quetop,DEFAULT_SQ_SIZE
	mov	[bx].queseg,cx
	call	ini_que

; initialization complete

	mov	ax,1			;number of voices available to the user
	jmp	short opendone

internal_error:                         ;os error
	cCall	<FAR PTR CloseSound>	; free all memory assigned so far
	mov	ax,erofm		;out of memory error
	jmp	short opendone

devinuse:
	mov	ax,erdvna		;device unavailable (in use)

opendone:
cEnd


;-----------------------------------------------------------------------
;
; coStartSound()
;
; StartSound will start play in each voice queue.
; StartSound is not destructive and so may be called
; multiple times.
;
; Entry:
;
; Exit:
;	AX = 0
;
; Registers Modified:
;	BX,CX,DX
;
; History:
;
;  Wed 14-Dec-1988 12:28:39  -by-  David N. Weise  [davidw]
; Cleaned this sucker up!
;-----------------------------------------------------------------------

	assumes ds,data
	assumes es,nothing

cProc   StartSound,<PUBLIC,FAR>,<si,di>
cBegin
        call    checkpid
	jnz	sts_x
	mov	bx,OFFSET qcblock
	cmp	[bx].quenote,0		; any notes is queue?
	jz	sts_x
	call	start_music
sts_x:	xor	ax,ax			; return no errors
cEnd

;-----------------------------------------------------------------------
;
; StopSound()
;
; StopSound will stop playing all voice queues. All
; voice queues are flushed. The sound driver for
; each voice is turned off.
;
; Entry:
;
; Exit:
;	AX = 0
;
; Registers Modified:
;	BX,CX,DX
;
; History:
;
;  Wed 14-Dec-1988 12:28:39  -by-  David N. Weise  [davidw]
; Cleaned this sucker up!
;-----------------------------------------------------------------------

	assumes ds,data
	assumes es,nothing

cProc   StopSound,<PUBLIC,FAR>,<si,di>
cBegin
        call    checkpid
	jnz	ss_end
	call	stop_music
ss_end: xor	ax,ax			; Return no errors
cEnd


;-----------------------------------------------------------------------
;
; CloseSound()
;
; CloseSound will close access to device and allow
; another process to play music. All voice queues
; are flushed. Buffers allocated for voice queues
; will be freed. By definition, all music will
; stop when the when CloseSound is called, and hence
; StopSound need not be called.
;
; Entry:
;
; Exit:
;
; Registers Modified:
;	AX,BX,CX,DX
;
; History:
;
;  Wed 14-Dec-1988 12:28:39  -by-  David N. Weise  [davidw]
; Cleaned this sucker up!
;-----------------------------------------------------------------------

	assumes ds,data
	assumes es,nothing

cProc   CloseSound,<PUBLIC,FAR>,<si,di>
cBegin
        call    checkpid
        jnz     pclose_end              ; pid is not current one
        cCall   StopSound

	xor	ax,ax
	mov	pid,ax			; free up the device for others to use

; give back all the music/noise queue memory to OS

	mov	si,OFFSET qlocinfo	; first index into queue location control block
	xchg	ax,[si].q_size		; get the size of the memory
	or	ax,ax			; has any memory been assigned?
	jz	pclose_end		; brif no memory assigned
	cCall	GlobalFree,<[si].q_segment>
pclose_end:
cEnd

;-----------------------------------------------------------------------
;
; SetVoiceQueueSize(voice,cb)
;
; SetVoiceQueueSize will allocate cb bytes for the
; specified voice queue. The default queue size
; at OpenSound time is 32 notes per voice with 7 bytes
; per note. For a single voice system this is 224 bytes.
; In addition to the memory required for the music
; is the memory required for the music queue control
; blocks which is 14 bytes per voice.  The amount of queue
; space consumed varies. All queues are locked in memory.
; Allocation of queues can not be done while music is
; playing. SetVoiceQueueSize will return an error in this
; case.
;
; Note:   The user may alter the queue size of any queue
;	  which has no notes.
;
; Entry:
;	int voice;
;	int cb;
;
; Exit:
;	AX = erotm - out of memory
;	   = ermact - music active
;
; Registers Modified:
;	BX,CX,DX
;
; History:
;
;  Wed 14-Dec-1988 12:28:39  -by-  David N. Weise  [davidw]
; Cleaned this sucker up!
;-----------------------------------------------------------------------

	assumes ds,data
	assumes es,nothing

cProc   SetVoiceQueueSize,<PUBLIC,FAR>,<si,di>
        parmW   voice
        parmW   cb
cBegin
        call    checkpid
        jnz     qset_done
	cmp	voice,1
	jnz	qset_done

; test if music currently playing and report error accordingly

	cCall	<FAR PTR CountVoiceNotes>,<voice>
	or	ax,ax			; are there any notes?
	jnz	queue_not_empty 	; brif queue not empty

; deallocate the memory that is currently assigned to this voice

	mov	si,OFFSET qlocinfo	; first index into queue location control block
	xor	ax,ax
	xchg	ax,[si].q_segment	; current segment for this voice queue
	cCall	GlobalFree,<ax>

; save in queue location control table

	mov	cx,cb
	jcxz	cb_bytes		; if it's 0, then don't allocate os memory

; allocate the memory from the OS

	mov	ax,cx			; ask for a multiple of the
	xor	dx,dx			;  note block_size
	mov	cx,SIZE note_block
	div	cx
	mul	cx

	push	ax
        xor     dx,dx
	regptr	DXAX,dx,ax
	cCall	GlobalAlloc,<dx,dxax>	; fixed global
	mov	[si].q_segment,ax	; queue segment for this voice
	pop	cx
        or      ax,ax
	jz	mem_failure		; os memory failure
cb_bytes:

	mov	[si].q_size,cx		; queue size for this voice
	mov	bx,OFFSET qcblock
	mov	[bx].queseg,ax
	mov	[bx].quetop,cx
	call	ini_que

qset_done:
	xor	ax,ax			; return no errors
	jmp	short qset_exit

mem_failure:
	xor	ax,ax			; no bytes allowed - OS problem
	mov	[si].q_size,ax		; queue size for this voice
	mov	ax,erofm		; out of memory error
	jmp	short qset_exit

queue_not_empty:
	mov	ax,ermact		; music active error

qset_exit:
cEnd

;-----------------------------------------------------------------------
;
; SetVoiceNote
;
; SetVoiceNote will queue the note information in the
; specified voice queue. If not enough room is
; in the queue an error is returned. If voice is
; out of range then the call to SetVoicNote is ignored.
;
; Note:   Note values of 0 are interpreted as pauses.
;
; Note:   Note values are adjusted by the pitch wheel,
;	  which is set by pqaccent.
;
; Entry:
;	int   voice;
;	int   value;
;	int   length;
;	int   cdots;
;
; Exit:
;	AX = erqful - Queue full
;	   = erbdnt - Invalid note
;	   = erbdln - Invalid note length
;	   = erbdcc - Invalid cdot note count
;
; Registers Modified:
;	BX,CX,DX
;
; History:
;
;  Wed 14-Dec-1988 12:28:39  -by-  David N. Weise  [davidw]
; Cleaned this sucker up!
;-----------------------------------------------------------------------

	assumes ds,data
	assumes es,nothing

cProc   SetVoiceNote,<PUBLIC,FAR>,<si,di>
        parmW   voice
        parmW   value
	parmW	nlength
	parmW	cdots

	localW	full_dur
cBegin
        call    checkpid
        jnz     qn_0
	cmp	voice,1 		; only voice 1 supported
	jz	qn_1
qn_0:	jmp	qn_2			; return no error
qn_1:

; Get index into queue location / control table

	mov	si,OFFSET qlocinfo	;table with segment/size/volume/tempo/mode

; Pull the note information from the stack

	mov	bx,nlength
        mov     ax,value

; Calculate Duration:
;
;	AX = note value (which may be altered by the pitch wheel)
;	SI = index to queue location/control info for this voice
;	BX = 1/note_length
;
; Duration Parameters:
;
;	Note Length:	1 to 1/64
;
;	Tempo:		Number of Quarter Notes per Minute
;
;	Mode:		leggato  1/1 time
;			normal	 7/8 time
;			staccato 3/4 time
;
;	Cdot		length = length * (2-(1/2)^cdots)
;
; Duration is calculated as follows:
;
;	4/tempo = minutes/note
;
;	(60*4*mode*note_length*(2-(1/2)^cdot)) / tempo = seconds/note
;
;	(60*4*mode*note_length*(2-(1/2)^cdot)) / (tempo*.0025)
;
;		= # of 2.5 milliseconds / note
;
; Simplifying:
;
;	(96000*mode*(2-(1/2)^cdot)) / ((1/note_length)*tempo) = duration

	push	ax			; Save note value
	mov	dl,bl			; 1/note length = 1/(1..64)
	mov	al,[si].q_tempo 	; get tempo (beats per unit time)
	mul	dl			; AX = (1/note_length) * tempo
	mov	cx,ax			; CX = (1/note_length) * tempo
	mov	dx,1			; 96000 (4*60/.0025) = 17700 Hex
	mov	ax,7700h		; (special time constant)
	jcxz	qn_cont1		; zero duration (don't queue it)
	div	cx			; AX = DX:AX  =        96000
					;      -----	  ----------------
					;	CX	(1/note_length)*tempo)
	or	ax,ax			; If duration is zero, get out.
	mov	bx,cdots		; BX = number of dots (cdot)
	jnz	qn_cont2
qn_cont1:
	pop	ax			; pop bogus note value
	jmp	qn_exit 		; zero duration (don't queue it)
qn_cont2:

; Calculate the effects of dots next

	cmp	bx,bx			; check for any dots
	jz	no_dots 		;  brif no dots at all
					; AX = duration thus far
					; BX = number of dots (cdot)
	mov	cx,ax

; repeat the following for each dot

cdts:	shr	cx,1			; length * (1/2)
	dec	bx
	jnz	cdts			; repeat for each dot
	mov	bx,ax
	sub	bx,cx			; (length) - (length * (1/2))^cdots
	add	ax,bx			; (length * 2) - (length * (1/2))^cdots
	jc	bad_nl			; Invalid Note Length if overflow
no_dots:

; IF note value 0 then this is a complete rest note from the user.
;
; Therefore, we want to queue the rest with full note duration.
;
; AX = duration = (96000*(2-(1/2)^cdot)) / ((1/note_length)*tempo)
;
; Effects of Staccato / Legatto or Normal not calculated yet.
; (And they don't have to be !!!)

	pop	cx			; refresh note value
	or	cx,cx			; if the note value = 0 we have a rest
	jnz	qn_have_note		; queue rest only
	mov	dx,ax			; this is now the interstice duration
	xor	bx,bx			; with [CX] = 0 note duration
	jmp	short queue_it		; and [BX] = 0 frequency

qn_have_note:

; If note value non zero, we continue calculating real note duration.
; AX = duration = (96000*(2-(1/2)^cdot)) / ((1/note_length)*tempo)

	push	cx
	mov	full_dur,ax		; save full duration

; Now calculate the effects of mode (leggato/normal/staccato)

	mov	cl,[si].q_mode_nls	; Using scale for shift count
	cmp	cl,LOW leggato		;
	jz	note_val		; Brif Legatto (1/1 time)
	mov	bx,3			; Staccato multiplier (3/4)
	cmp	cl,LOW staccato 	;  (2)
	jz	mode_calc		; Brif Staccato
	mov	cl,bl			;  else Normal (7/8)
	mov	bl,7			;
mode_calc:
	mul	bx			; Duration *7 or *3
	shr	ax,cl			; Duration /8 or /4
	or	ax,ax			; Note duration of 0 shows roundoff
	jnz	note_val		;
	inc	ax			; If zero then make 1

note_val:
	mov	dx,ax			; Duration in DX

; DX = (96000*mode*(2-(1/2)^cdot)) / ((1/note_length)*tempo)

; Check out the note value

	pop	ax			; note value
	cmp	ax,84			; Must be 84 or less
	ja	bad_val 		; brif error

; adjust note value with default pitch

	add	al,[si].q_pitch 	; get the current pitch
	cmp	al,84d			; check for wrap around
	jbe	qn_cont5
	SUB	AL,84D			; perform modulus 84 arithmetic
qn_cont5:

; 7 octaves each with 12 notes:  C C# D D# E F F# G G# A A# B

	dec	ax			; we want note 1..12 not 0..11
	mov	bl,12			; 12 notes per octave
	div	bl			;  determine octave and note
	mov	bx,ax			; [BH:BL] = note:octave
	inc	bh			; we want note 1..12 not 0..11
	push	bx
	mov	bl,bh
	xor	bh,bh			; note now in BL
	shl	bl,1			;  double it (we're after a word)
	add	bx,OFFSET note_tbl-2	; address of note frequency table
	mov	ax,cs:[bx]		; get freq. of note BL for high octave
	pop	bx			; [BH:BL] = note:octave
	mov	cl,6			; highest octave
	sub	cl,bl			; 6-octave
	shr	ax,cl			; divide by 2 for each octave from 6
	adc	ax,0			; add carry appropriately to round up

; AX = frequency in Hz
; DX = duration in 2.5 millisecond intervals
;
; Now queue the note plus a pause if Normal or Staccato

	mov	bx,ax			; BX = frequency in Hz
	mov	cx,dx			; note duration
	mov	dx,full_dur		; full duration of note including interstice
	sub	dx,cx			; interstice duration
queue_it:
	call	queue_note
	jnc	qn_2

; Handle all error conditions:

	call	field_errors		; return error condition to user
	jmp	short qn_exit		; return appropriate error code

bad_nl: pop	ax			; pop bogus note value
bad_nl2:
	mov	ax,erbdln		; Invalid Note Length
	jmp	short qn_exit

bad_val:
	mov	ax,erbdnt		; Invalid Note
	jmp	short qn_exit

qn_2:	xor	ax,ax			; No error condition

qn_exit:
cEnd

; table of note frequencies:
; these are the frequencies in hertz of the top octave (6)
; divide these down by powers of two to get all other octaves

        PUBLIC  note_tbl
note_tbl:
	DW	4186			; C	 1
	DW	4435			; C#	 2
	DW	4699			; D	 3
	DW	4978			; D#	 4
	DW	5274			; E	 5
	DW	5588			; F	 6
	DW	5920			; F#	 7
	DW	6272			; G	 8
	DW	6645			; G#	 9
	DW	7040			; A	10
	DW	7459			; A#	11
	DW	7902			; B	12

;-----------------------------------------------------------------------
;
; SetVoiceSound(voice,sound)
;
; Directly programs the sound chip.
;
; SetVoiceSound will queue the parameters for the
; specified voice. Note that pqnote and pqsound
; should not be intermixed. The frequency value is
; defined as a long word. The most significant word
; is the integer part of the frequency. The least
; significant is the fractional part.
;
; Note:   This routine will only operate with music
;	  queues. Noise queues are ignored.
;
; Note:   Pauses are programmed using 0 frequency.
;
; Entry:
;	int voice;
;	int freqint;
;	int freqfrac;
;	int  duration;
;
; Exit:
;	AX = erqful - Queue full
;	   = erbdfq - Invalid frequency
;	   = erbddr - Invalid duration
;
; Registers Modified:
;	BX,CX,DX
;
; History:
;
;  Wed 14-Dec-1988 12:28:39  -by-  David N. Weise  [davidw]
; Cleaned this sucker up!
;-----------------------------------------------------------------------

	assumes ds,data
	assumes es,nothing

cProc   SetVoiceSound,<PUBLIC,FAR>,<si,di>
        parmW   voice
        parmD   freq
        parmW   duration
cBegin
        call    checkpid
        jnz     qs_end
	cmp	voice,1
	jnz	qs_end
        mov     bx,seg_freq
	mov	cx,duration		; duration of note
	xor	dx,dx			; 0 interstice duration
	or	bx,bx			; 0 frequency ?
	jnz	qs_it			; brif no rest
	xor	cx,cx			; 0 note duration
qs_it:	call	queue_note
	jnc	qs_end
	call	field_errors		; invalid freq/dur or full queue
	jmp	short qs_exit
qs_end: xor	ax,ax			; return no errors
qs_exit:
cEnd


;-----------------------------------------------------------------------
;
; SetVoiceAccent(voice, paccent)
;
; SetVoiceAccent will queue the accent information in the
; specified voice queue. If there is not enough room
; in the queue an error is returned. If voice is out
; of range SetVoiceAccent is ignored.  The effects
; of SetVoiceAccent last until the next SetVoiceAccent call.
; The SetVoiceAccent queue is not counted as a note.
;
; Note:   Pitch is interpreted as follows:
;	  There is a pitch wheel which varies from
;	  0 to 83 where 0 is the default value.
;	  Pitch is added to note value in SetVoiceNote
;	  using modulus 84 arithmetic. For example
;	  note value 20 + pitch 30 gives an
;	  effective note value of 50. Another
;	  example: note value 70 + pitch value 30
;	  gives an effective note value of 16.
;
; Note:   The volume can be set at the default level
;	  by using volume 65535.
;
; Entry:
;	int	voice;
;	int	tempo;	 (32 to 255)
;	int	volume;  (0 to 65535)
;	int	mode;	 (n,s,l)
;	int	pitch;	 (0 to 83)
;
; Exit:
;	AX = erqful - Queue full
;	   = erbdtp - Invalid tempo
;	   = erbdvl - Invalid volume
;	   = erbdmd - Invalid mode
;
; Registers Modified:
;	BX,CX,DX
;
; History:
;
;  Wed 14-Dec-1988 12:28:39  -by-  David N. Weise  [davidw]
; Cleaned this sucker up!
;-----------------------------------------------------------------------

	assumes ds,data
	assumes es,nothing

cProc   SetVoiceAccent,<PUBLIC,FAR>,<si,di>
        parmW   voice
        parmW   tempo
        parmW   volume
        parmW   mode
        parmW   pitch
cBegin
        call    checkpid
        jnz     qa_end
	cmp	voice,1
	jnz	qa_end

	mov	si,OFFSET qlocinfo
        mov     ax,tempo                ;tempo (32 to 255)
	mov	cx,mode 		;mode (normal/leggato/staccato)
	mov	dx,pitch		;pitch (0 to 84)

; Test and store these values

	or	ah,ah			;Test tempo <= 255
	jnz	bad_tempo		;Invalid Tempo
	cmp	al,32D			;Test tempo >= 32
	jb	bad_tempo		;Invalid Tempo
	mov	[si].q_tempo,al 	;Save the tempo for this voice
	cmp	cx,2			;Test mode 0,1,2
	ja	bad_mode		;Invalid Mode
	mov	[si].q_mode_nls,cl	;Save the mode for this voice
	cmp	dx,83D			;Test pitch wheel <=83
	ja	bad_pitch		;Invalid pitch
	mov	[si].q_pitch,dl 	;Save the pitch for this voice
qa_end:
	xor	ax,ax			;No error condition
	jmp	short qa_exit

bad_pitch:
	mov	ax,erbdpt		;Invalid Pitch
	jmp	short qa_exit
bad_mode:
	mov	ax,erbdmd		;Invalid mode
	jmp	short qa_exit
bad_tempo:
	mov	ax,erbdtp		;Invalid tempo

qa_exit:
cEnd


;-----------------------------------------------------------------------
;
; field_errors
;
; This routine checks for errors after calls
; to the OEM dependent routine mdd_play. This routine
; translates OEM interface errors into user
; interface errors
;
; Entry:
;	AX = error code passed by mdd_play
;	   = err_qfull	= 1 - No room in the queue for this request
;	   = err_freq	= 2 - Unsupported frequency
;	   = err_vol	= 3 - Unsupported volume index
;	   = err_eshape = 4 - Unsupported envelope shape
;	   = err_dur	= 5 - Unsupported envelope duration
;	   = err_noise	= 6 - Unsupported noise source
;
; Exit:
;	AX = error code passed to user
;	   = erqful - queue full
;	   = erbdfq - invalid frequency
;	   = erbdvl - invalid volume
;	   = erbdsh - invalid shape
;	   = erbddr - invalid duration
;	   = erbdsr - invalid source
;
; Registers Modified:
;	none
;
; History:
;
;  Wed 14-Dec-1988 12:28:39  -by-  David N. Weise  [davidw]
; Cleaned this sucker up!
;-----------------------------------------------------------------------

        PUBLIC  field_errors

field_errors:

	push	si
	xor	ah,ah			; strip off any junk
	dec	ax			; 0 based error
	shl	ax,1			; word pointer
	add	ax,OFFSET error_table	; into error table
	mov	si,ax
	mov	ax,cs:[si]		; get the translated error message
	pop	si
	ret

error_table:
	DW	erqful			; err_qfull  = erqful
	DW	erbdfq			; err_freq   = erbdfq
	DW	erbdvl			; err_vol    = erbdvl
	DW	erbdsh			; err_eshape = erbdsh
	DW	erbddr			; err_dur    = erbddr
	DW	erbdsr			; err_noise  = erbdsr



;-----------------------------------------------------------------------
;
; GetThresholdEvent()
;
; GetThresholdEvent returns a pointer to a flag word
; called the threshold event flag. The threshold
; event flag indicates the occurance of a threshold
; event. A threshold event is defined as the
; transition of a voice queue from n to n-1
; where n is the threshold level in notes.
; SetThresholdStatus should only be used to clear this flag.
;
; Entry:
;
; Exit:
;	AX = pointer to event flags
;
; Registers Modified:
;	BX,CX,DX
;
; History:
;
;  Wed 14-Dec-1988 12:28:39  -by-  David N. Weise  [davidw]
; Cleaned this sucker up!
;-----------------------------------------------------------------------

	assumes ds,data
	assumes es,nothing

cProc   GetThresholdEvent,<PUBLIC,FAR>,<si,di>
cBegin
        call    checkpid
        jz      pgp_0
        xor     ax,ax
        jmp     short pgp_1
pgp_0:	mov	ax,OFFSET tew_flags	; Threshold event word pointer
        mov     dx,ds
pgp_1:
cEnd

;-----------------------------------------------------------------------
;
; GetThresholdStatus()
;
; GetThresholdStatus will return a word containing the
; threshold status for each voice. A bit set means
; that that voice queue level is currently below
; threshold. Pgetstatus will clear the threshold
; event flag. The threshold event flag should be
; cleared only in this way.
;
; Entry:
;
; Exit:
;	AX = word
;
; Registers Modified:
;	BX,CX,DX
;
; History:
;
;  Wed 14-Dec-1988 12:28:39  -by-  David N. Weise  [davidw]
; Cleaned this sucker up!
;-----------------------------------------------------------------------

	assumes ds,data
	assumes es,nothing

cProc   GetThresholdStatus,<PUBLIC,FAR>,<si,di>
cBegin
        call    checkpid
	mov	ax,0			; don't change flags!
	jnz	gts_exit		; clear the event flags
	xchg	ax,tew_flags		; and return this to the user
gts_exit:
cEnd

;-----------------------------------------------------------------------
;
; SetVoiceThreshold(voice,cnote)
;
; SetVoiceThreshold sets the threshold level for a
; voice. When the number of notes remaining in the
; queue goes below cnote the threshold flag
; word is set. If the queue level is below cnote
; when Psetthreshold is called, the flag is not
; set. Pgetstatus should be called to verify this
; case.
;
; Note:   This routine operates with both the music
;	  and noise queues.
;
; Entry:
;	int voice;
;	int cnote;
;
; Exit:
;	AX = 0
;
; Registers Modified:
;	BX,CX,DX
;
; History:
;
;  Wed 14-Dec-1988 12:28:39  -by-  David N. Weise  [davidw]
; Cleaned this sucker up!
;-----------------------------------------------------------------------

	assumes ds,data
	assumes es,nothing

cProc   SetVoiceThreshold,<PUBLIC,FAR>,<si,di>
        parmW   voice
        parmW   cnote
cBegin
        call    checkpid
        jnz     qt_done
	cmp	voice,1
	jne	qt_done
	mov	ax,cnote
	mov	bx,OFFSET qcblock	; queue control block
	mov	[bx].qthresh,ax 	; save the threshold for this voice
qt_done:
	xor	ax,ax			; return no errors
cEnd

;-----------------------------------------------------------------------
;
; WaitSoundState
;
; WaitSoundState will not return until the driver enters
; the specified state.	State can be one of the following:
;  * Queue empty:   All voice queues are empty and sound drivers turned off.
;  * Threshold:     A voice queue has dropped below threshold, returns voice.
;  * All Threshold: All voices have reached threshold
;
; Entry:  state on stack - one of the following:
;	    0 = queueempty
;	    1 = threshold
;	    2 = allthreshold
;
; Returns:
;	AX = erbdst - Invalid state
;
; Registers modified:
;	BX,CX,DX
;
; History:
;
;  Wed 14-Dec-1988 12:28:39  -by-  David N. Weise  [davidw]
; Cleaned this sucker up!
;-----------------------------------------------------------------------

	assumes ds,data
	assumes es,nothing

cProc   WaitSoundState,<PUBLIC,FAR>,<si,di>
        parmW   state
cBegin
        call    checkpid
	jnz	pw_done 		; pid not current one, quit
        mov     ax,state
	or	ax,ax
	.errnz	queueempty
	jz	pw_queue_empty		; wait until all queues are empty

	dec	ax
	.errnz	threshold - 1
	jz	pw_threshold		; wait until one of the voice queues
					;  drops below the threshold
	dec	ax
	.errnz	allthreshold -2
	jz	pw_threshold		; all voices same as only voice

	mov	ax,erbdst		; Invalid state error
	jmp	short pw_exit

; wait until all queues are empty

pw_queue_empty:
        cCall   Yield
	call	notes_in_queue
	or	bx,bx			; are there any notes in this queue?
	jnz	pw_queue_empty		; nope - still notes in the queue
	jmp	short pw_done		; we're done waiting

; wait until one of the voice queues drops below the threshold
; Poll all voice queues until at least one voice is below the
; threshold. (Indicated by a set bit in flags word.)

pw_threshold:
        cCall   Yield
	cCall	GetThresholdStatus
	or	ax,ax			; test if any voice below threshold
	jz	pw_threshold		; none yet

pw_done:
	xor	ax,ax			; return no errors
pw_exit:
cEnd

;-----------------------------------------------------------------------
;
; CountVoiceNotes(voice)
;
; Returns number of notes in queue.
;
; CountVoiceNote returns the number of notes in the
; specified queue. Only those queue entries
; resulting from calls to pqnote are counted as
; notes. The queue may therefore contain many
; entries that are not counted as notes.
;
; Entry:
;	int voice;
;
; Exit:
;	AX = Number of notes in queue
;
; Registers Modified:
;	BX,CX,DX
;
; History:
;
;  Wed 14-Dec-1988 12:28:39  -by-  David N. Weise  [davidw]
; Cleaned this sucker up!
;-----------------------------------------------------------------------

	assumes ds,data
	assumes es,nothing

cProc   CountVoiceNotes,<PUBLIC,FAR>,<si,di>
        parmW   voice
cBegin
        call    checkpid
	mov	ax,0			; don't change flags
	jnz	pgcn_end
	cmp	voice,1
	jnz	pgcn_end
	call	notes_in_queue
	mov	ax,bx
pgcn_end:
cEnd

;-----------------------------------------------------------------------
;
; SyncAllVoices()
;
; Synchronize all voices by queueing a sync mark.
;
; Entry:
;
; Exit:
;	AX = 0
;
; Registers Modified:
;	none
;
;  Thu 08-Dec-1988 23:26:29  -by-  David N. Weise  [davidw]
; Neutered this because it ain't supported.
;-----------------------------------------------------------------------

	assumes ds,nothing
	assumes es,nothing

cProc	SyncAllVoices,<PUBLIC,FAR>
cBegin nogen
	xor	ax,ax
	ret
cEnd nogen


;-----------------------------------------------------------------------
;
; SetSoundNoise (source,duration)
;
; Makes noise.
;
; Entry:
;	int source;
;	int duration;
;
; Exit:
;	AX = 0
;
; Registers Modified:
;	none
;
;  Thu 08-Dec-1988 23:26:29  -by-  David N. Weise  [davidw]
; Neutered this because it ain't supported.
;-----------------------------------------------------------------------

	assumes ds,nothing
	assumes es,nothing

cProc	SetSoundNoise,<PUBLIC,FAR>
;	parmW	source
;	parmW	duration
cBegin nogen
	xor	ax,ax
	ret	4
cEnd nogen


;-----------------------------------------------------------------------
;
; SetVoiceEnvelope(voice, paccent)
;
; Queues envelope information.
;
; Entry:
;	int   voice;
;	int   shape;
;	int   repeat;
;
; Exit:
;	AX = 0
;
; Registers Modified:
;	none
;
;  Thu 08-Dec-1988 23:26:29  -by-  David N. Weise  [davidw]
; Neutered this because it ain't supported.
;-----------------------------------------------------------------------

	assumes ds,nothing
	assumes es,nothing

cProc	SetVoiceEnvelope,<PUBLIC,FAR>
;	parmW	voice
;	parmW	shape
;	parmW	repeat
cBegin nogen
	xor	ax,ax
	ret	6
cEnd nogen


sEnd    CODE

        END

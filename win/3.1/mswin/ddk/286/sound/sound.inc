
;-----------------------------------------------------------------------
;
;                               Structures
;
;-----------------------------------------------------------------------


; This queue structure is used by the MUSIC queues.

que_ctrl_blk    STRUC
	quenote DW	?	; number of notes in queue
	quetop	DW	?	; pointer to highest memory location of queue
				;  also queue length (in bytes)
	quenum	DW	?	; queue counter in bytes - # of bytes in queue
	queget	DW	?	; where to get next byte from queue
	queput	DW	?	; where to put next byte into queue
	queseg	DW	?	; segment of queue (queseg:0 is first byte)
	qthresh DW	?	; threshold determining queue event (set by user)
	quevol	DB	?	; default volume (may change as we're playing)
que_ctrl_blk    ENDS

q_loc_block	STRUC
        q_segment       DW      0
        q_size          DW      0
        q_tempo         DB      0
        q_mode_nls      DB      0
        q_pitch         DB      0
q_loc_block	ENDS

note_block	STRUC
	nb_freq 	DW	0
	nb_dur		DW	0
	nb_interstice	DW	0
note_block	ENDS


;-----------------------------------------------------------------------
;
;                               Constants
;
;-----------------------------------------------------------------------

DEFAULT_SQ_SIZE EQU	32 * 6

disable EQU	CLI
enable  EQU     STI
clkint	    = 20h	;X'20' clock interrupt vector
debug	    = 0 	;1=debug versions, 0=delivered versions
eoi	    = 20h	;X'20' 8259 end_of_interrupt ACK

err_qfull   = 1         ;No room in the queue for this request
err_freq    = 2         ;Unsupported frequency
err_vol     = 3         ;Unsupported volume index
err_eshape  = 4         ;Unsupported envelope shape
err_dur     = 5         ;Unsupported envelope duration
err_noise   = 6         ;Unsupported noise source

f_init      = 1         ;- initialize and return voices
f_q_size    = 2         ;- initialize queues with queue size
f_q_note    = 3         ;- queue a note or rest
f_volume    = 4         ;- queue volume change
f_envelope  = 5         ;- queue envelope change
f_start     = 6         ;- start music
f_stop      = 7         ;- stop music
f_sync      = 8         ;- synchronize voices
f_term      = 9         ;- terminate
f_ev_ptr    = 10        ;- get event flag pointer
f_flags     = 11        ;- get event flag and clear it
f_trap      = 12        ;- set trap level
f_noise     = 13        ;- queue noise
f_num_notes = 14        ;- get number of notes in queue

intao	    = 20h	;X'20' 8259 int control port
mskreg	    = 21h	;8259 interrupt mask register
speaker     = 61h	;speaker latch port address
spkron	    = 3 	;Speaker on mask
spkren	    = 10h	;speaker enable/disable mask
square	    = 0B6h	;timer 2 square wave - mode 3.
timer0	    = 40h	;timer 0 port address
timer2	    = 42h	;timer 2 port address
tmrcmd	    = 43h	;timer control port
tod_int     = 8h	;ROM time of day interrupt
type_note   = 1 	;first byte of a note in a queue

; error codes

erdvna  EQU     -1      ;/* device not available */
erofm   EQU     -2      ;/* out of memory */
ermact  EQU     -3      ;/* music active */
erqful  EQU     -4      ;/* queue full */
erbdnt  EQU     -5      ;/* invalid note */
erbdln  EQU     -6      ;/* invalid note length */
erbdcc  EQU     -7      ;/* invalid note count */
erbdtp  EQU     -8      ;/* invalid tempo */
erbdvl  EQU     -9      ;/* invalid volume */
erbdmd  EQU     -10     ;/* invalid mode */
erbdsh  EQU     -11     ;/* invalid shape */
erbdpt  EQU     -12     ;/* invalid pitch */
erbdfq  EQU     -13     ;/* invalid frequency */
erbddr  EQU     -14     ;/* invalid duration */
erbdsr  EQU     -15     ;/* invalid source */
erbdst  EQU     -16     ;/* invalid state */

; constants used to specify return condition for pwait

QUEUEEMPTY	EQU	0
THRESHOLD	EQU	1
ALLTHRESHOLD	EQU	2

; constants used to specify accent mode

NORMAL		EQU	0
LEGGATO 	EQU	1
STACCATO	EQU	2


;-----------------------------------------------------------------------
;
;                               Macros
;
;-----------------------------------------------------------------------

pause	MACRO				; macro to insure that an instruction
	jmp	$ + 2			; fetch occurs between IN and/or OUT
	ENDM				; instructions on the PC AT machine

savint  MACRO   savloc,intvec           ;
	mov	al,intvec/4		; AL = interrupt
	mov	ah,35h
	int	21h			; function 35h Get Interrupt Vector
					; returns ES:BX pointer to interrupt routine
	mov	word ptr savloc[0],bx	; savloc = offset
	mov	word ptr savloc[2],es	; savloc+2 = segment
	ENDM

setvec	MACRO	interrupt,offset_adr	; Set interrupt vector function call
	mov	al,interrupt		; AL interrupt number
	mov	dx,OFFSET offset_adr	; DS:DX interrupt handling routine
	mov	ah,25h			; Set Vector
	int	21h
	ENDM

rstvec	MACRO	interrupt,save_adr	; Restore interrupt from saved
	lds	dx,dword ptr save_adr	; DS:DX interrupt handling routine
	mov	al,interrupt		; AL interrupt number
	mov	ah,25h			; Set Vector
	int	21h
	ENDM

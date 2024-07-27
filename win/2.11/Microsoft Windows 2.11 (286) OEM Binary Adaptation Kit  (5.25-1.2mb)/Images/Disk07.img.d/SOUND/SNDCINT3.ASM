?PLM = 1
?WIN = 1

.xlist
include cmacros.inc
.list

sBegin DATA
;-----------------------------------------------------------------------
;
;                               Data
;
;-----------------------------------------------------------------------
externW  <full_dur>     ;full duration for notes
externB  <nmvoices>     ;number of music voices for this machine
externB  <nnvoices>     ;number of noise voices for this machine
externB  <ntvoices>     ;total number of voices for this machine
externW  <pid>          ;process id of task accessing Music Device Driver
                        ; 0 indicates that this device is not in use
                        ; non 0 indicates device open
externW  tew_ptr        ;Threshold event word pointer

sEnd   DATA

sBegin CODE

externNP    <mdd_play>
externNP    <checkpid>
externNP    <field_errors>

assumes CS,CODE
assumes DS,DATA

; /* error codes */

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

;-----------------------------------------------------------------------
;
;       Name:           pstart - start play
;
;       Call:           pstart()
;
;       Purpose:        Pstart will start play in each voice queue.
;                       Pstart is not destructive and so may be called
;                       multiple times.
;
;       Entry:          none
;
;       Exit:           none
;
;       Modified:       AX,BX,CX,DX
;
;-----------------------------------------------------------------------

cProc   StartSound,<PUBLIC,FAR>,<si,di>
cBegin
        call    checkpid
        jnz     pstart_end
        MOV     AL,f_start              ;Function: start music
        CALL    mdd_play                ; (this function never returns an error)
pstart_end:
        XOR     AX,AX                   ;return no errors
cEnd

;-----------------------------------------------------------------------
;
;       Name:           pstop - stop all queues
;
;       Call:           pstop()
;
;       Purpose:        Pstop will stop playing all voice queues. All
;                       voice queues are flushed. The sound driver for
;                       each voice is turned off.
;
;       Entry:          none
;
;       Exit:           none
;
;       Modified:       AX,BX,CX,DX
;
;-----------------------------------------------------------------------

cProc   StopSound,<PUBLIC,FAR>,<si,di>
cBegin
        call    checkpid
        jnz     pstop_end
        MOV     AL,f_stop               ;Function: Stop music / flush queues
        CALL    mdd_play                ; (this function never returns an error) ;
pstop_end:
        XOR     AX,AX                   ;return no errors
cEnd

;-----------------------------------------------------------------------
;
;       Name:           pgetpevt
;
;       Call:           int far *pgetpevt()
;
;       Purpose:        Pgetpevt returns a pointer to a flag word
;                       called the threshold event flag. The threshold
;                       event flag indicates the occurance of a threshold
;                       event. A threshold event is defined as the
;                       transition of a voice queue from n to n-1
;                       where n is the threshold level in notes.
;                       Pgetstatus should only be used to clear this flag.
;
;       Entry:          none
;
;       Exit:           [AX] = pointer to event flags
;
;       Modified:       BX,CX,DX
;
;-----------------------------------------------------------------------

cProc   GetThresholdEvent,<PUBLIC,FAR>,<si,di>
cBegin
        call    checkpid
        jz      pgp_0
        xor     ax,ax
        jmp     short pgp_1
pgp_0:
        MOV     AX,tew_ptr              ;Threshold event word pointer
        mov     dx,ds
pgp_1:
cEnd

;-----------------------------------------------------------------------
;
;       Name:           pgetstatus - return current threshold status
;
;       Call:           pgetstatus()
;
;       Purpose:        Pgetstatus will return a word containing the
;                       threshold status for each voice. A bit set means
;                       that that voice queue level is currently below
;                       threshold. Pgetstatus will clear the threshold
;                       event flag. The threshold event flag should be
;                       cleared only in this way.
;
;       Entry:          none
;
;       Exit:           AX = word
;
;       Modified:       BX,CX,DX
;
;-----------------------------------------------------------------------

cProc   GetThresholdStatus,<PUBLIC,FAR>,<si,di>
cBegin
        call    checkpid
        jz      pgs_0
        xor     ax,ax
        jmp     short pgs_1
pgs_0:
        MOV     AL,f_flags              ;function f_flags- poll events
        CALL    mdd_play                ; (this function never returns an error)
        MOV     AX,BX                   ;returns [BX] = flags when event occurs
                                        ; set bits indicate threshold event
pgs_1:
cEnd

;-----------------------------------------------------------------------
;
;       Name:           psetthreshold
;
;       Call:           Psetthreshold(voice, cnote);
;                       int voice, cnote;
;
;       Purpose:        Psetthreshold sets the threshold level for a
;                       voice. When the number of notes remaining in the
;                       queue goes below cnote the threshold flag
;                       word is set. If the queue level is below cnote
;                       when Psetthreshold is called, the flag is not
;                       set. Pgetstatus should be called to verify this
;                       case.
;
;                       Note:   This routine operates with both the music
;                               and noise queues.
;
;       Entry:          voice on stack
;                       cnote on stack
;
;       Exit:           None
;
;       Modified:       AX,BX,CX,DX
;
;-----------------------------------------------------------------------

cProc   SetVoiceThreshold,<PUBLIC,FAR>,<si,di>
        parmW   voice
        parmW   cnote
cBegin
        call    checkpid
        jnz     qt_done
        mov     ax,voice
        cmp     al,ntvoices
        ja      qt_done
        mov     bx,cnote
        DEC     AL                      ;0 based voice number
        ;
        ; mdd_play function f_trap (set trap threshold)
        ;
        ;       [AL] = f_trap (set threshold)
        ;       [AH] = voice (0 based)
        ;       [BX] = number of notes for threshold
        ;
        MOV     AH,AL                   ;voice (0 based)
        MOV     AL,f_trap               ;function set trap level
        CALL    mdd_play                ;set threshold
                                        ; (this function never returns an error)
qt_done:
        XOR     AX,AX                   ;return no errors
qt_exit:
cEnd

;-----------------------------------------------------------------------
;
;       Name:           pgetcnote - return number of notes in queue
;
;       Call:           pgetcnote(voice)
;                       int voice;
;
;       Purpose:        Pgetcnote returns the number of notes in the
;                       specified queue. Only those queue entries
;                       resulting from calls to pqnote are counted as
;                       notes. The queue my therefore contain many
;                       entries that are not counted as notes.
;
;                       Note:   This routine operates with both the music
;                               and noise queues.
;
;       Entry:          voice on stack
;
;       Exit:           [AX] = Number of notes in queue
;
;       Modified:       BX,CX,DX
;
;-----------------------------------------------------------------------

cProc   CountVoiceNotes,<PUBLIC,FAR>,<si,di>
        parmW   voice
cBegin
        call    checkpid
        jnz     pgcn0
        mov     ax,voice
        cmp     al,ntvoices
        jbe     pgcn1
pgcn0:
        xor     ax,ax
        jmp     short pgcn_end
pgcn1:
        DEC     AL                      ;0 based voice number
        ;
        ; mdd_play function f_num_notes
        ;
        ;       [AH] = 0 based voice
        ;       [AL] = f_num_notes
        ;
        MOV     AH,AL                   ;voice
        MOV     AL,f_num_notes          ;function
        CALL    mdd_play                ;Return [BX] = number of notes in the queue
                                        ; (this function never returns an error)
        MOV     AX,BX
pgcn_end:
cEnd

;-----------------------------------------------------------------------
;
;       Name:           psync - Synchronize all voices by queueing a sync mark
;
;       Call:           psync()
;
;       Purpose:        Psync will queue a sync mark in each queue. Upon
;                       encountering a sync mark in a voice queue, the
;                       voice is turned off until sync marks are
;                       encountered in all other queues. This forces
;                       syncronization between all voices.
;
;                       Also sync noise queue ??????? or pass voice ?????
;
;       Entry:          none
;
;       Exit:           AX = erqful - Queue full
;
;       Modified:       BX,CX,DX
;
;-----------------------------------------------------------------------

cProc   SyncAllVoices,<PUBLIC,FAR>,<si,di>
cBegin
        call    checkpid
        jnz     sync_end
        MOV     AH,ntvoices             ;move number of voices to AH
        MOV     AL,f_sync               ;Function: queue a sync mark
ps_loop:
        DEC     AH                      ;current voice number 0 relative
        PUSH    AX
        CALL    mdd_play                ;queue sync mark for voice AH
        JC      sync_error
        POP     AX
        OR      AH,AH                   ;any more voices to synchronize ?
        JNZ     ps_loop                 ;do it for all voices
sync_end:
        XOR     AX,AX                   ;return no errors
sync_exit:
cEnd

sync_error:
        CALL    field_errors            ;test for full queue
        POP     AX
        JMP     SHORT sync_exit

;-----------------------------------------------------------------------
;
;       Name:           pqnoise - queue noise to noise hardware
;
;       Call:           pqnoise(source, duration)
;                       short source, duration
;
;       Purpose:        Pqnoise will queue noise to the noise hardware.
;                       This may not be the same as the play hardware.
;                       Source is the noise source. Source is one of a set
;                       of predefined values. See play.h for valid values.
;                       This can be a constant frequency derived from the
;                       clock or from one of the voice queues. Which voice
;                       queue is hardware dependent. Duration is in clock
;                       tics.
;
;                       Note:   This routine assumes that the noise channel
;                               handle is 1 + number of music voices.
;
;                       Note:   This routine can only handle 1 noise voice.
;
;                       Note:   To set the volume for the noise channel use
;                               pqaccent.
;
;       Entry:          noise on stack as follows: source
;                                                  duration
;
;       Exit:           AX = erbdsr - Invalid source
;
;       Modified:       BX,CX,DX
;
;-----------------------------------------------------------------------

cProc   SetSoundNoise,<PUBLIC,FAR>,<si,di>
        parmW   source
        parmW   duration
cBegin
        call    checkpid
        jnz     qns_end
        mov     bx,duration
        mov     cx,source
        ;
        ; [CX] = source index
        ; [DX] = duration (in 2.5 ms intervals)
        ;
        MOV     AL,f_noise              ;Function - queue noise
        CALL    mdd_play
        JC      noise_errors            ;invalid source/volume/duration or full queue
qns_end:
        XOR     AX,AX                   ;return no errors
qns_exit:
cEnd

noise_errors:
        CALL    field_errors            ;return the appropriate error condition
        JMP     SHORT qns_exit

;-----------------------------------------------------------------------
;
;       Name:           pqsound - directly programs the sound chip
;
;       Call:           pqsound(voice, sound)
;                       int voice;
;                       int freqint;
;                       int freqfrac;
;                       int  duration
;
;       Purpose:        Pqsound will queue the parameters for the
;                       specified voice. Note that pqnote and pqsound
;                       should not be intermixed. The frequency value is
;                       defined as a long word. The most significant word
;                       is the integer part of the frequency. The least
;                       significant is the fractional part.
;
;                       Note:   This routine will only operate with music
;                               queues. Noise queues are ignored.
;
;                       Note:   Pauses are programmed using 0 frequency.
;
;       Entry:          voice on stack
;                       sound on stack as follows: frequency integer part
;                                                  frequency fractional part
;                                                  duration in clock ticks
;
;       Exit:           AX = erqful - Queue full
;                          = erbdfq - Invalid frequency
;                          = erbddr - Invalid duration
;
;       Modified:       BX,CX,DX
;
;-----------------------------------------------------------------------

cProc   SetVoiceSound,<PUBLIC,FAR>,<si,di>
        parmW   voice
        parmD   freq
        parmW   duration
cBegin
        call    checkpid
        jnz     qs_end
        mov     ax,voice
        cmp     al,ntvoices
        ja      qs_end
        mov     bx,seg_freq
        DEC     AL                      ;0 based voice number
        MOV     AH,AL                   ;voice into AH
        MOV     AL,f_q_note             ;queue a note function
        OR      BX,BX                   ;0 frequency ?
        JZ      qs_rest                 ;brif a rest
        MOV     CX,duration             ;duration of note
        XOR     DX,DX                   ;0 interstice duration
qs_it:
        CALL    mdd_play                ;queue the note
        JC      sound_errors            ;errors from OEM
qs_end:
        XOR     AX,AX                   ;return no errors
qs_exit:
cEnd

qs_rest:                                ;if a rest - keep interstice in DX
        XOR     CX,CX                   ;0 note duration
        JMP     SHORT qs_it

sound_errors:
        CALL    field_errors            ;invalid freq/dur or full queue
        JMP     SHORT qs_exit

;-----------------------------------------------------------------------
;
;       Name:           pqenvelope - queue envelope information
;
;       Call:           pqenvelope(voice, paccent)
;                       int   voice;
;                       int   shape;
;                       int   repeat;
;
;       Purpose:        Pqenvelope will queue the envelope information
;                       in the specified voice queue. If there is not
;                       enough room in the queue an error is returned.
;                       If voice is out of range pmusic is ignored. Space
;                       for a single sync mark is always left in the queue.
;                       The effects of pqenvelope last until the next
;                       pqenvelope call. A pqenvelope queue entry is
;                       not counted as a note.
;
;                       Note:   This routine will operate with both
;                               music and noise queues.
;
;       Entry:          voice on stack
;                       envelope on stack as follows: shape
;                                                     repeat
;
;       Exit:           AX = erqful - Queue full
;                          = erbdsh - Invalid shape
;                          = erbddr - Invalid duration
;
;       Modified:       BX,CX,DX
;
;-----------------------------------------------------------------------

cProc   SetVoiceEnvelope,<PUBLIC,FAR>,<si,di>
        parmW   voice
        parmW   shape
        parmW   repeat
cBegin
        call    checkpid
        jnz     env_end
        mov     ax,voice
        cmp     al,ntvoices
        ja      env_end
        mov     bx,shape
        mov     cx,repeat
        MOV     AH,AL                   ;voice
        DEC     AH                      ;0 based
        MOV     AL,f_envelope
        CALL    mdd_play
        JC      envelope_errors
env_end:
        XOR     AX,AX                   ;return no errors
envelope_exit:
cEnd

envelope_errors:
        CALL    field_errors            ;test for error conditions
        JMP     SHORT envelope_exit

sEnd    CODE

END

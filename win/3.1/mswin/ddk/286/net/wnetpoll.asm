;
;   WNETPOLL.ASM
;
;   Contains WNetWatchQueue() and WNetUnwatchQueue(), and supporting functions
;   and data
;
;   Copyright (C) 1989-1990 Microsoft Corp, all rights reserved.
;


memS=1			    ; small model
?PLM=1			    ; PASCAL calling conventions
?WIN=1			    ; Windows support

.xlist
include cmacros.inc
include wnet.inc
.list

externFP <PostMessage>	    ; necessary USER things
externFP <SetTimer>
externFP <KillTimer>

externFP <UTLIsSerialDevice>   ; in UTILS.ASM

;================================
;   Automatic data
;

sBegin data

globalW 	hwndSpooler,0,1 ; window handle of spooler

globalW 	cWatches,0,1	; number of watches

globalW 	rgfWatch,0,<4 * size WatchEntry>

globalW 	wElapse,30000,1 ; millisecond polling interval

globalW 	nIDEvent,0,1	; timer handle

lpfnTimer	dd  far ptr TimerProc

sEnd data

;================================
;   code segment
;

sBegin code

    assumes cs,code
    assumes ds,data
    assumes es,nothing
    assumes ss,nothing


;----------------------
;   TimerProc()
;
;   Called by DispatchMessage() when a WM_TIMER message is posted with
;   NULL hwnd and lParam = TimerProc.  Causes posting of an SP_QUEUECHANGE
;   message if there are watches on any queues
;
cProc	TimerProc, <FAR,PUBLIC>

    parmW   hwnd
    parmW   timermsg
    parmW   nID
    parmD   time

cBegin
    cmp     cWatches,0	    ; make sure we're watching in case there's an extra
    jz	    timer_exit	    ; message floating in

    push    hwndSpooler
    mov     ax,SP_QUEUECHANGED
    push    ax		    ; message
    sub     ax,ax
    dec     ax
    push    ax		    ; wParam = invalidate all queues (-1)
    push    ax		    ; lParam = unused
    push    ax
    cCall   PostMessage     ; tell the spooler

timer_exit:
cEnd

;----------------------
;   StartPolling()
;
;   Start the timer for the first spooler watch
;
cProc	StartPolling, <NEAR>

cBegin
    sub     ax,ax

    push    ax		    ; NULL hwnd (call callback, don't post)
    push    ax		    ; NULL nIDEvent
    push    wElapse	    ; millisecond delay

    push    word ptr lpfnTimer[2]   ; segment and...
    push    word ptr lpfnTimer[0]   ; ...offset of timer procedure

    cCall   SetTimer	    ; make a timer
    or	    ax,ax	    ; did we really get it?
    jnz     st_gotone

    mov     ax,WN_WINDOWS_ERROR
    jmp     short st_exit

st_gotone:
    mov     nIDEvent,ax
    sub     ax,ax	    ; return WN_SUCCESS

st_exit:
cEnd

;--------------------
;   StopPolling()
;
;   Stop watching the remote queues after the last watch is ended.
;   Halts posting of SP_QUEUECHANGED messages
;
cProc	StopPolling, <NEAR>

cBegin
    sub     ax,ax
    push    ax			; NULL hwnd
    push    nIDEvent		; timer ID
    cCall   KillTimer
    or	    ax,ax
    jnz     stopp_stopped

    mov     ax,WN_WINDOWS_ERROR
    jmp     short stopp_exit

stopp_stopped:
    sub     ax,ax

stopp_exit:
cEnd


;----------------------------
;   WNetWatchQueue
;
;   Causes SP_QUEUECHANGED messages to be posted every 30 seconds for
;   for this queue.  Marks it out in the flag array.
;
cProc	WNetWatchQueue, <FAR,PUBLIC>, <SI>

    parmW   hwnd		; spooler window handle
    parmD   lpszQueue		; local queue to watch
    parmD   lpszUser		; unused
    parmW   nQueue		; unused

cBegin
    mov     ax,hwnd
    mov     hwndSpooler,ax	; save spooler window handle

    push    seg_lpszQueue
    push    off_lpszQueue	; is the queue a local port?
    cCall   UTLIsSerialDevice

    or	    ax,ax		; NULL?
    jnz     wnwq_validport

    mov     ax,WN_BAD_QUEUE	; invalid queue
    jmp     short wnwq_exit

wnwq_validport:
    xchg    ax,si
    test    byte ptr [si].fWatch, PORT_WATCHED
    jz	    wnwq_notwatched

    mov     ax,WN_ALREADY_LOCKED    ; already being watched
    jmp     short wnwq_exit

wnwq_notwatched:
    cmp     cWatches,0
    jnz     wnwq_dontstarttimer
    cCall   StartPolling
    or	    ax,ax
    jnz     wnwq_exit		; if not WN_SUCCESS, return error

wnwq_dontstarttimer:
    inc     cWatches		; increment number of watchers
    or	    byte ptr [si].fWatch, PORT_WATCHED	 ; set watch flag for port

    sub     ax,ax		; return WN_SUCCESS

wnwq_exit:
cEnd

;----------------------------------------
;   WNetUnwatchQueue
;
;   Halts posting of SP_QUEUECHANGED messages for a particular queue
;
cProc	WNetUnwatchQueue, <FAR,PUBLIC>

    parmD   lpszQueue

cBegin

    push    seg_lpszQueue
    push    off_lpszQueue
    cCall   UTLIsSerialDevice	; is it an LPTx: port?
    or	    ax,ax
    jnz     wnuq_validport

    mov     ax,WN_BAD_QUEUE	; invalid queue name
    jmp     short wnuq_exit

wnuq_validport:
    xchg    ax,bx
    test    byte ptr [bx].fWatch, PORT_WATCHED
    jnz     wnuq_watched

    mov     ax,WN_ALREADY_LOCKED
    jmp     short wnuq_exit

wnuq_watched:
    and     byte ptr [bx].fWatch, not PORT_WATCHED  ; clear the flag
    dec     cWatches		; reduce the number of watchers
    jnz     wnuq_dontstoptimer	; if any left, keep polling
    cCall   StopPolling 	; return status from StopPolling
    jmp     short wnuq_exit

wnuq_dontstoptimer:
    sub     ax,ax		; return WN_SUCCESS

wnuq_exit:
cEnd

sEnd

end

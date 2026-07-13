                page    88,122
                title   INTR - Interrupt Handlers (2.0.1 Spec)

;****************************************************************************;
;*                                                                          *;
;*      (C) Copyright 1988,1989 by 3Com Corporation                         *;
;*                                                                          *;
;*      This IBM Token Ring NDIS Driver source is the property of Microsoft *;
;*      Corporation and is provided only as an example of an NDIS MAC       *;
;*      driver for use with Microsoft's OS/2 LAN Manager, DOS LAN Manager,  *;
;*      or LM/X products.                                                   *;
;*                                                                          *;
;****************************************************************************;

;------ INTR.ASM ------------------------------------------------------------;
;                                                                            ;
;       This file contains the interrupt handler.  A lot of the action       ;
;       takes place here because of the way the IBM Token Ring adapter is    ;
;       designed.  In many cases you may want to refer to the initial        ;
;       function call for documentation on how things work.                  ;
;                                                                            ;
;       09-21-88 Connery                                                     ;
;       Original version.                                                    ;
;                                                                            ;
;       01-04-89 Connery                                                     ;
;       Changed interrupt handler to always switch stacks (previously was    ;
;       conditional on nestlevel which meant that if somebody else got       ;
;       in between two nested interrupts we would use their stack).  Since   ;
;       we switch stacks back BEFORE post-processing, the nested interrupt   ;
;       can just always switch.                                              ;
;                                                                            ;
;       03-17-89 Connery                                                     ;
;       16/4 adapter support.  Fixed bug in transmit confirms which was      ;
;       causing an invalid protid to be passed up (ENQHEAD trashes AX).      ;
;       This would cause VECTOR to toss the confirmation away.  Change       ;
;       request confirmation logic to return CONFIRMATION_FAILURE on one of  ;
;       the open adapter parameter errors rather than the ubiquitous         ;
;       HARDWARE_FAILURE (ie.  txbuf too big for this speed or not enough    ;
;       room for all those rxbufs).                                          ;
;                                                                            ;
;       02-25-90 2.0a Connery                                                ;
;       Lift one of Microsoft's ideas to get rid of the switchback variable  ;
;       and the tests on it when we exit.  Instead, push an extra return     ;
;       address on the stack if we come in in real mode.  Also change the    ;
;       EOI sequence to always fall-thru by issuing a NOP to the PIC if      ;
;       necessary.  Since we're getting rid of local stack variables, put    ;
;       reasoning in the MAC data structure since its only used in the       ;
;       non-reentrant front portion of the interrupt handler.  And then use  ;
;       BP instead of the remaining stack variable.  Drop multiple adapter   ;
;       support.  Add FASTGDT code.  Added logic to return NETWORK_MAY_      ;
;       NOT_BE_CONNECTED on OpenAdapter.  Use new ENQ/DEQ macros.  Minor     ;
;       performance tweaks.  Logic to support ResetMAC.  Added logic to      ;
;       return TRANSMIT_ERROR or NO_SUCH_DESTINATION based on FS byte.       ;
;       Break statistics into ANY/ALL categories.                            ;
;                                                                            ;
;----------------------------------------------------------------------------;

.xlist
IFDEF OS2
include devhlp.inc
ENDIF
include includes.mac
.list
.lfcond
.sall

;------ Exported Interfaces -------------------------------------------------;

                public  AdapterClosed
                public  IntHandler
                public  NextTransmit
                public  OpenCompleted
                public  StatusIndicate

;------ Imported Interfaces -------------------------------------------------;

                DEXTRN  asbfrees:WORD
                DEXTRN  intsreal:WORD
                DEXTRN  intstotal:WORD
                DEXTRN  lastarb:BYTE
                DEXTRN  lastreason:WORD
                DEXTRN  lastsrb:BYTE
                DEXTRN  rxarbs:WORD
                DEXTRN  rxasbs:WORD
                DEXTRN  rxlaters:WORD
                DEXTRN  txarbs:WORD
                DEXTRN  txasbs:WORD
                DEXTRN  txlaters:WORD
                DEXTRN  txsrbs:WORD
                DEXTRN  txssbs:WORD
                DOSXTRN DDStkOff:WORD
                DOSXTRN DDStkSeg:WORD
                DOSXTRN DDTopStk:BYTE
                OS2XTRN devHelper:DWORD
                OS2XTRN segTX:WORD
                extrn   EOIs:WORD
                extrn   ICpending:BYTE
                extrn   IMRport:WORD
                extrn   Intpending:BYTE
                extrn   SRBinuse:BYTE
                extrn   acked:BYTE
                extrn   adapterBits:BYTE
                extrn   common:BYTE
                extrn   devHeader:WORD
                extrn   disableMask:BYTE
                extrn   enableMask:BYTE
                extrn   indications:BYTE
                extrn   nestLevel:BYTE
                extrn   oARB:WORD
                extrn   oASB:WORD
                extrn   oSRB:WORD
                extrn   oSSB:WORD
                extrn   offDepth:BYTE
                extrn   oldstate:BYTE
                extrn   pdispatch:BYTE
                extrn   protDS:WORD
                extrn   q_rqconfirm:BYTE
                extrn   q_rqdone:BYTE
                extrn   q_rqfree:BYTE
                extrn   q_rqtodo:BYTE
                extrn   q_txawaits:BYTE
                extrn   q_txconfirm:BYTE
                extrn   q_txdone:BYTE
                extrn   q_txfree:BYTE
                extrn   q_txtodo:BYTE
                extrn   rate:BYTE
                extrn   reasonMask:WORD
                extrn   reasoning:WORD
                extrn   receiving:BYTE
                extrn   releasePort:WORD
                extrn   retcode:WORD
                extrn   rxHead:WORD
                extrn   rxRelease:WORD
                extrn   rxSize:WORD
                extrn   segMMIO:WORD
                extrn   segSM:WORD
                extrn   startDepth:BYTE
                extrn   state:BYTE
                extrn   status:BYTE
                extrn   timerTicks:WORD
                extrn   txBuffer:WORD
                extrn   txCorrelate:BYTE
                extrn   txNeeded:BYTE

                extrn   FlushQueues:NEAR        ; from util
                extrn   NextRequest:NEAR        ; from req
                extrn   OpenAdapter:NEAR        ; from util
                extrn   ResetAdapter:NEAR       ; from util
IFDEF SNAP
                DEXTRN  SnapRx:NEAR             ; from recv
                DEXTRN  SnapTx:NEAR             ; from xmit
ENDIF

NEWPAGE <Code>
;------ Code ----------------------------------------------------------------;

                BEGIN_CODE

;------ Switch to Protect Mode ----------------------------------------------;
;
; entered in real mode.  switch to protect mode and push an extra return
; address on the stack to switch us back when we leave.
;
IFDEF OS2
                align   4
                public  int_toprotect
int_toprotect:
                DINC    intsreal
                mov     si,offset DGROUP:devHeader
                mov     dl,DevHlp_RealToProt
                call    devHelper

                push    cs
                push    offset CGROUP:int_toreal
                jmp     short int_enter
ENDIF

NEWPAGE <IntHandler - Interrupt Handler>
;------ IntHandler ----------------------------------------------------------;
;                                                                            ;
;       This routine fields an interrupt from the adapter.                   ;
;                                                                            ;
;       For the AT bus adapters once an interrupt is generated to the PC,    ;
;       another won't be until the Interrupt Release port is hit.  This      ;
;       port is ignored by the microchannel adapters so in addition we need  ;
;       to disable interrupts from the adapter at the start of the           ;
;       interrupt handler and reenable them just before hitting the          ;
;       Interrupt Release port.  Though these steps are not necessary for    ;
;       the AT bus adapters it was easier to incorporate them directly than  ;
;       to avoid them.                                                       ;
;                                                                            ;
;       Because of this enable/disable logic, it is not necessary to do      ;
;       an implicit IndicationOff/IndicationOn around the ReceiveLookahead   ;
;       call.  The IndicationOff has already been done at the start of the   ;
;       interrupt handler.  On return if the indications flag has been       ;
;       cleared, we leave offDepth set to one.  Then when we go to rearm     ;
;       the adapter, we reenable interrupts only if offDepth is zero.        ;
;                                                                            ;
;       Possible Optimizations:                                              ;
;                                                                            ;
;       1) It is possible to field many interrupts without mode switching    ;
;       if we are in the compatibility box.  Those interrupts which result   ;
;       in calls to the protocol need to switch, but many interrupts do not  ;
;       (a transmit proceeds through several interrupts and if the request   ;
;       handle is zero, none of these result in calls to the protocol).  To  ;
;       make the driver bimodal we need to make the selectors segMMIO and    ;
;       segSM bi-modal, by setting them to the appropriate values for the    ;
;       current mode.  Most of the code is then bimodal automatically.  The  ;
;       only tricky logic is in handling the transmit ARB.                   ;
;                                                                            ;
;       While the theory is that this will increase the system performance   ;
;       and network throughput while in the real mode box, early tests do    ;
;       not show any significant change.  Most interrupts are taken in       ;
;       protect mode even when running a tight loop in the real mode box.    ;
;       So for the moment since they significantly complicate things, the    ;
;       bimodal changes have been backed off.                                ;
;                                                                            ;
;       2) At the end of the interrupt handler just before returning check   ;
;       the ISRP to see if there is something else that has come up and      ;
;       take care of it rather than returning from the interrupt handler     ;
;       and coming back in.  Some care must be taken to avoid issuing extra  ;
;       EOIs and such but with a real protocol stack that imposes some       ;
;       delay between the rearming of the adapter and the end of the         ;
;       interrupt handler it seems like between 15% and 30% of the           ;
;       interrupts (depending on processor speed) could be looped around in  ;
;       this fashion.  What exactly the impact would be is not clear.  A     ;
;       couple of quick hacks at it introduced bugs which I didn't have      ;
;       time to work around.                                                 ;
;                                                                            ;
;----------------------------------------------------------------------------;

;------ Common Interrupt Handler --------------------------------------------;

                align   4
IntHandler      proc    far
;
; disable interrupts explicitly
;
                cli
;
; under DOS we must save the registers ourselves and set up DS to equal CS
; since there is only one segment (both handled by OS/2 for us)
;
IFDEF DOS
                PUSHR   ax,bx,cx,dx,si,di,es,ds,bp

                mov     ax,cs                           ; local data
                mov     ds,ax
;
; under DOS, switch stacks
;
                mov     DDStkSeg,ss                     ; save stack segment
                mov     DDStkOff,sp                     ; save stack pointer
                mov     ss,ax                           ; new stack segment
                mov     sp,offset DGROUP:DDTopStk       ; new stack pointer
ENDIF
;
; if in real mode, switch to protected mode (note that this test is not
; sufficient if "OS/3" uses the Protected Virtual 8086 mode).
;
IFDEF OS2
                DINC    intstotal

                smsw    ax                      ; ax <- machine status word
                and     ax,1                    ; protect mode bit (faster than shr)
                jz      int_toprotect
ENDIF
;
; increment the interrupt nest level
;
                public  int_enter
int_enter:
                inc     nestLevel
;
; clear the direction flag to conform with Microsoft C calling conventions
;
                cld
;
; set CX to the ISRP (byte swapped) and clear the bits which caused the
; interrupt.  it is important that we do this as soon as possible, otherwise
; the adapter might set a bit twice before we get around to clearing it.
;
; we use a memory location rather than a constant here so that we can set
; it to zero during reset, keeping the reset logic out of critical paths
;
                mov     es,segMMIO
                mov     cx,es:ISRP
                and     cx,reasonMask
IFDEF DEV
                mov     lastreason,cx
ENDIF

                not     cx
                mov     es:ISRP+ACARESET,cx
                not     cx
;
; save interrupt reasons locally.  we'll deal with them one at a time, highest
; priority first, masking off the bits until none are left.
;
                mov     reasoning,cx
;
; disable interrupts from the adapter through the PIC IMR
;
                inc     offDepth

                mov     dx,IMRport
                in      al,dx
                or      al,disableMask
                out     dx,al
;
; allow interrupt controller to acknowledge other system interrupts.  under
; DOS this is always done by playing directly with the PIC.  under OS/2 we
; use the DevHlp call unless FASTEOI is defined in which case we also do
; the EOI by playing with the PIC directly.  This is because the DevHlp call
; is horrendously inefficient (125 instructions or about 150usec on an 8MHz
; AT).  Defining FASTEOI makes a noticable performance difference.
;
IFDEF DOS
                mov     ax,EOIs                 ; commands to issue
                out     INT_2EOIR,al            ; command to master
                xchg    ah,al
                out     INT_1EOIR,al            ; command to slave
ELSEIFDEF FASTEOI
                mov     ax,EOIs                 ; commands to issue
                out     INT_2EOIR,al            ; command to master
                xchg    ah,al
                out     INT_1EOIR,al            ; command to slave
ELSE
                mov     al,specific.mc_interrupt.lobyte
                mov     dl,DevHlp_EOI
                call    devHelper
ENDIF

;------ Top of Reason Loop --------------------------------------------------;
;
; CX contains the reason(s) for the interrupt.  Determine the highest priority
; reason and take care of it (jump to appropriate label).
;
; ARB_REASON and SSB_REASON are really performance critical, with SRB_REASON
; also critical to some extent since it is used for Request Interrupt.
;
; It is required that ARB_REASON be tested before SRB_REASON just in case
; both would generate an indication to the protocol (Receive Data or Ring
; Status from the ARB and Interrupt Indication from the SRB).  Since the
; first might leave indications disabled and the Interrupt Indication can be
; suppressed if another indication has already been done, we check the ARB
; first and then only generate an Interrupt Indication if the Intpending
; flag is still set (cleared if another indication was done).
;
; During reset we occasionally get an interrupt for no (ie. 0) reason, perhaps
; due to some timing window.  If so, just ignore it.  However, this can also
; happen because we set reasonMask to 0 as part of ResetMAC.  So check for
; this condition as well.
;
                public  int_reasonloop
int_reasonloop:
                test    cx,ARB_REASON
                jnz     int_arb
                test    cx,SSB_REASON
                jnz     goto_int_ssb
                test    cx,SRB_REASON
                jnz     goto_int_srb
                test    cx,ASBFREE_REASON
                SJNZ    int_asbfree
                test    cx,TICK_REASON
                SJNZ    int_tick
                test    cx,CHECK_REASON
                SJNZ    int_check
                cmp     state,PRERESET
                je      goto_int_startreset
ifdef DEV
                test    cx,cx
                SJZ     int_rearm
;
; unknown reason for interrupt, probably an access error.  just ignore it
; and hope we survive (its most likely an access error, meaning we're dead).
;
                BPOINT  bad_int_reason_in_cx
endif
                jmp     int_rearm
;
; indirect jumps if too far away
;
                align   4
goto_int_ssb:   jmp     int_ssb
                align   4
goto_int_srb:   jmp     int_srb
goto_int_startreset: jmp int_startreset

;------ ARB -----------------------------------------------------------------;
;
; locate the ARB in ES:DI
;
                align   4
                public  int_arb
int_arb:
                and     reasoning,NOT ARB_REASON
                mov     es,segSM
                mov     di,oARB
;
; switch based on the command specified
;
                mov     al,es:[di].arb_command
IFDEF DEV
                mov     lastarb,al
ENDIF
                cmp     al,DIR_DATARECEIVED
                je      int_rxarb
                cmp     al,DIR_TRANSMITREQUEST
                SJE     int_txarb
                cmp     al,DIR_RINGSTATUS
                SJE     int_ringstatus
;
; no match, just release the ARB and ignore it.
;
                public  int_badarb
int_badarb:
                DINT3
                mov     es,segMMIO
                mov     byte ptr es:ISRAL+ACASET,ISRALARBFREE
                jmp     int_nextreason
;
; incoming data received with a zero packet filter.  ignore it.
;
; check for an invalid state.  if so, just release the ARB.
;
                public  int_rxarb_ignore
int_rxarb_ignore:
                test    di,di                   ; oARB = 0?
                jz      int_badarb
;
; set the receive buffer to be released
;
                mov     ax,es:[di].rxarb_buffer
                xchg    ah,al
                mov     rxRelease,ax
;
; release the ARB
;
                mov     es,segMMIO
                mov     byte ptr es:ISRAL+ACASET,ISRALARBFREE
;
; rejoin normal logic to release the receive buffers
;
                jmp     int_rxarb_asb

;------ Data Received -------------------------------------------------------;
;
; incoming data received.  the ARB specifies the buffer location for the first
; buffer containing the incoming frame.  there may be one or more buffers in
; the chain.  each has the rxbuf structure in its first 8 bytes, and then
; the actual frame data.
;
                align   4
                public  int_rxarb
int_rxarb:
                DINC    rxarbs
;
; if the packet filter is zero, ignore the received packet
;
                cmp     receiving,NO
                je      int_rxarb_ignore
;
; snap the received packet for debugging
;
IFDEF SNAP
                call    SnapRx
ENDIF
;
; set CX to the packet length, DX to the number of bytes in the first buffer,
; and DI to offset of the first buffer's data.  save the buffer offset and
; size for use in TransferData and in releasing the buffer.
;
                mov     ax,es:[di].rxarb_buffer
                xchg    ah,al
                mov     cx,es:[di].rxarb_framelen
                xchg    ch,cl
                mov     di,ax
                mov     rxHead,di
                mov     rxSize,cx
                mov     dx,es:[di].rxbuf_buflen
                xchg    dh,dl
                add     di,rxbuf_data
;
; release the ARB
;
                mov     es,segMMIO
                mov     byte ptr es:ISRAL+ACASET,ISRALARBFREE
;
; debug code to trap disktest errors when pattern is all A's.  assume an
; NBP header, so raw data starts at offset 36.  if the first four bytes
; are A's, then assume its disktest data and check the rest of the packet
; that we can see.  to keep this fast, we'll only check the first buffer
; of DX bytes in length.
;
ifdef ALLAS
                public  allas_test, allas_failure, allas_pops, allas_period
allas_test:
                cmp     dx,128
                jb      allas_period
                PUSHR   es,di,ax,cx
                mov     es,segSM
                cmp     word ptr es:[di][14h],003Ch     ; NBP Data packet?
                jne     allas_period
                mov     ax,4141h
                cmp     word ptr es:[di][24h],53FFh     ; SMB?
                jne     allas_raw?
                cmp     word ptr es:[di][26h],424Dh
                jne     allas_pops
                cmp     byte ptr es:[di][28h],0Bh       ; Write SMB?
                jne     allas_pops
                cmp     es:[di][54h],ax                 ; with AAAA's?
                jne     allas_pops
                cmp     es:[di][56h],ax                 ; with AAAA's?
                jne     allas_pops
                add     di,58h
                mov     cx,dx
                sub     cx,58h
                jmp     short allas_check
allas_raw?:
                cmp     es:[di][24h],ax
                jne     allas_pops
                cmp     es:[di][26h],ax
                jne     allas_pops
                add     di,28h
                mov     cx,dx
                sub     cx,28h
allas_check:
                shr     cx,1
        repe    scasw
                je      allas_pops
allas_failure:
                int 3
allas_pops:
                POPR    es,di,ax,cx
allas_period:
endif
;
; do an implicit IndicationOff.
;
; increment the IndicationOff depth.  we don't need to actually disable
; interrupts from the adapter since we did that earlier.
;
                inc     offDepth
;
; default indications flag to 0xFF
;
                mov     indications,0FFh
;
; update statistics on received packets, total bytes received, number of
; multicasts and broadcasts (multicasts includes broadcasts).
;
                INCANY  ms_total_recv
                ADDANY  ms_total_rbytes,cx
IFDEF ALLSTATS
                push    es
                mov     es,segSM
                test    byte ptr es:[di][DA][0],80h             ;broadcast bit?
                jz      @F
                INCALL  ms_mult_recv
                ADDALL  ms_mult_rbytes,cx
                test    byte ptr es:[di][DA][2],80h             ;broadcast?
                jz      @F
                INCALL  ms_broad_recv
                ADDALL  ms_broad_rbytes,cx
@@:             pop     es
ENDIF
;
; cancel any interrupt indication that is pending, since we are allowed to
; collapse it (and need to if the protocol is going to leave indications
; off) if any other indications are generated
;
                mov     Intpending,NO
;
; issue the ReceiveLookahead indication to the protocol.  enable interrupts
; around the call just in case the protocol spins its wheels a little.
;
                sti

                push    common.cc_ID
                push    cx
                push    dx
                push    segSM
                push    di
                push    ds
                pushc   <offset DGROUP:indications>,ax
                push    protDS
                call    pdispatch.pd_rxlookahead

                cli
;
; remember to issue an IndicationComplete to the protocol.
;
                mov     ICpending,YES
;
; if indications flag wasn't cleared, do an implicit IndicationOn by
; decrementing the IndicationOff depth back to zero so that interrupts from
; the adapter will be reenabled when we rearm it.
;
int_rxdone:
                cmp     indications,0
                jz      @F
                dec     offDepth
@@:
;
; set rxrelease to the lead buffer to be released
;
                mov     ax,rxHead
                mov     rxRelease,ax
;
; we want to send an ASB to the adapter to release the receive buffers.  if
; the ASB is available, then take it over.
;
                public  int_rxarb_asb
int_rxarb_asb:
                mov     es,segSM
                mov     di,oASB
                cmp     es:[di].asb_retcode,0FFh
                jne     int_rxlater

;------ Release Receive Buffer ----------------------------------------------;
;
; rxrelease points to the lead buffer in a chain of buffers which need to be
; released, and the ASB is available.  ES:DI points to the ASB.
;
; build an ASB to release the buffers.  set the retcode to zero so that any
; test against FFh (completed) will fail.  interrupts must remain disabled
; until we hit the ASB ready bit.
;
                public  int_rxrelease
int_rxrelease:
                xor     ax,ax
                xchg    ax,rxRelease
                xchg    ah,al

                mov     es:[di].asb_command,DIR_DATARECEIVED
                mov     es:[di].asb_retcode,0
                mov     es:[di].rxasb_stationid,0
                mov     es:[di].rxasb_buffer,ax
;
; hit the ASB ready bit to inform the adapter to process it and if a transmit
; is waiting to be started, make sure the adapter interrupts us when this
; ASB is finished processing.
;
                DINC    rxasbs
                mov     es,segMMIO
                cmp     q_txawaits.q_head,0
                jnz     int_rxparallel

                mov     byte ptr es:ISRAL+ACASET,ISRALASBRDY
                jmp     int_nextreason
int_rxparallel:
                mov     byte ptr es:ISRAL+ACASET,ISRALASBRDY+ISRALFREEASB
                jmp     int_nextreason

;------ Postpone Releasing Buffer -------------------------------------------;
;
; the ASB is in use.  make sure the adapter will interrupt us when the ASB is
; available.  the buffer chain starting with the buffer at rxrelease will be
; released when the ASB free interrupt comes in.  if the ASB became available
; since we checked, the adapter will still generate an ASB free interrupt.
;
                public  int_rxlater
int_rxlater:
                DINC    rxlaters
                mov     es,segMMIO
                mov     byte ptr es:ISRAL+ACASET,ISRALFREEASB
                jmp     int_nextreason

;------ Issue Pending Tx or Rq SRB ------------------------------------------;
;
; Issue the pending SRB, then return to the mainline tx ARB processing.  To
; optimize the somewhat common Tx SRB we take advantage of the fact that the
; last SRB was a transmit SRB to keep things simple.
;
                align   4
                public  int_txarb_ntx
int_txarb_ntx:
                DINC    txsrbs
                dec     txNeeded
                mov     es,segMMIO
                mov     byte ptr es:ISRAL+ACASET,ISRALSRBRDY
                mov     es,segSM
                jmp     short int_txarbdeq

                public  int_txarb_nrq
int_txarb_nrq:
                call    NextRequest
                jmp     short int_txarbdeq

;------ Save Tx Buffer For Later --------------------------------------------;
;
; There's nothing on the transmit todo queue, so just save the transmit buffer
; and its correlate for later use.  If the transmit buffer address has been
; prefetched then TransmitChain will take a slightly more efficient path.
;
IFDEF PREFETCH
                align   4
                public  int_txpostpone
int_txpostpone:
                mov     al,es:[di].arb_correlate
                mov     txCorrelate,al

                mov     ax,es:[di].txarb_buffer
                xchg    ah,al
                mov     txBuffer,ax
;
; release the ARB
;
                mov     es,segMMIO
                mov     byte ptr es:ISRAL+ACASET,ISRALARBFREE
                jmp     int_nextreason
ENDIF

;------ Transmit Data Request -----------------------------------------------;
;
; as a result of a transmit SRB from the PC, the adapter has generated
; a transmit data request ARB specifying what transmit buffer to use.  ES:DI
; points to the ARB containing the correlate and buffer offset in segSM.
;
                align   4
                public  int_txarb
int_txarb:
                DINC    txarbs
;
; check if there are any transmit SRBs that need issuing or any requests to
; start (pended because the SRB was in use).
;
                cmp     txNeeded,0
                jnz     int_txarb_ntx
                EMPTYQ? q_rqtodo
                jnz     int_txarb_nrq
;
; since we suppressed the SRB free interrupt when we issued the transmit SRB,
; this is where we realize the SRB is free once again.
;
                mov     SRBinuse,NO
;
; if the queue is empty, just save the txbuffer for later
;
                public  int_txarbdeq
int_txarbdeq:
IFDEF PREFETCH
                EMPTYQ? q_txtodo
                jz      int_txpostpone
ENDIF
;
; dequeue the first qentry on the tx todo queue into SI
;
                DEQ     q_txtodo
;
; enqueue it on the tx awaiting start queue
;
                ENQONLY q_txawaits
;
; copy the correlate to the qentry
;
                mov     al,es:[di].arb_correlate
                mov     [si].qt_correlate,al
;
; set DI to the transmit buffer offset and save for later use in BP
;
                mov     ax,es:[di].txarb_buffer
                xchg    ah,al
                mov     di,ax
                mov     bp,di
;
; release the ARB
;
                mov     es,segMMIO
                mov     byte ptr es:ISRAL+ACASET,ISRALARBFREE
;
; enable interrupts since we expect to be spending time in here...
;
                sti
;
; copy the immediate data if any.
;
                mov     es,segSM
                mov     cx,[si].qt_ibytes
                jcxz    int_txcopyblocks

                push    si
                add     si,qt_ibuffer
                shr     cx,1
        rep     movsw                           ; move words
                adc     cx,cx
        rep     movsb                           ; move remaining byte if odd
                pop     si
;
; check if there are any data blocks to copy
;
                align   4
                public  int_txcopyblocks
int_txcopyblocks:
                mov     cx,[si].qt_blocks
                jcxz    int_txcopydone
;
; set DX to the number of frame blocks in the buffer descriptor and DS:SI to
; the address of the first chain entry
;
                mov     dx,cx
                add     si,qt_chain
;
; for each chain entry in DS:SI, ES:DI is the current tx buffer address.
;
; set CX to the number of bytes in this chain entry
;
                align   4
                public  int_txchainloop
int_txchainloop:
                mov     cx,[si].cd_data_len

                push    ds
                push    si
;
; get the frame data block's virtual address in DS:SI.  under OS/2 we may
; have to convert the frame data block's physical address to a virtual one
; using the transmit GDT selector reserved at INIT time.  if FASTGDT is
; defined do this ourselves in-line (the devhelp calls are just too many
; instructions).
;
IFDEF OS2
                cmp     [si].cd_ptr_type,CT_GDT
                jne     @F
                lds     si,[si].cd_data_buff
                jmp     short int_txcopy
@@:
    IFDEF FASTGDT
                mov     al,[si].cd_data_buff.hiword.lobyte
                mov     bx,[si].cd_data_buff.loword
                lds     si,dword ptr segTX
                mov     ds:[si].segd_base15_0,bx
                mov     ds:[si].segd_base23_16,al
                mov     ds:[si].segd_lim15_0,cx ; CX-1 really, but screw it
                mov     ds,si
                xor     si,si
    ELSE
                push    dx
                mov     ax,[si].cd_data_buff.hiword
                mov     bx,[si].cd_data_buff.loword
                mov     si,segTX
                mov     dl,DevHlp_PhysToGDTSelector
                call    devHelper
                mov     ds,si
                xor     si,si
                pop     dx
    ENDIF
ELSE
                lds     si,[si].cd_data_buff
ENDIF
;
; copy CX bytes from DS:SI to ES:DI
;
                public  int_txcopy
int_txcopy:
                shr     cx,1
        rep     movsw                           ; move words
                adc     cx,cx
        rep     movsb                           ; move remaining byte if odd
;
; next frame data block (in DS:SI)
;
                pop     si
                pop     ds

                add     si,CDSIZE
                dec     dx
                jnz     int_txchainloop
;
; packet copied to transmit buffer.  disable interrupts again.
;
                align   4
                public  int_txcopydone
int_txcopydone:
                cli
;
; set SI to the qentry again and save the number of bytes copied
;
                mov     si,q_txawaits.q_head
                sub     di,bp
                mov     [si].qt_bytes,di
;
; snap the transmit data for debugging
;
IFDEF SNAP
                mov     di,bp
                mov     cx,[si].qt_bytes
                call    SnapTx
ENDIF
;
; update statistics on packets, total bytes transmitted
;
IFDEF ANYSTATS
                INCANY  ms_total_sent
                mov     cx,[si].qt_bytes
                ADDANY  ms_total_sbytes,cx
ENDIF
IFDEF ALLSTATS
                test    byte ptr es:[bp][DA][0],80h             ;broadcast bit?
                jz      @F
                INCALL  ms_mult_sent
                ADDALL  ms_mult_sbytes,cx
                test    byte ptr es:[bp][DA][2],80h             ;broadcast?
                jz      @F
                INCALL  ms_broad_sent
                ADDALL  ms_broad_sbytes,cx
@@:
ENDIF
;
; we want to send an ASB to the adapter to start the transmission.  if the
; ASB is available, then take it over.
;
                mov     di,oASB
                cmp     es:[di].asb_retcode,0FFh
                DJNE    int_txlater

;------ Start Transmitting --------------------------------------------------;
;
; the qentry on the tx awaiting start queue needs to be transmitted, and the
; ASB is available.  ES:DI points to the ASB.  interrupts must remain off
; until we hit the ASB ready bit.
;
; dequeue the qentry into SI
;
                public  int_txstart
int_txstart:
                DEQONLY q_txawaits
;
; if the handle is zero, no confirmation is needed.  just move the qentry in SI
; back to the free queue.  otherwise, enqueue it on the tx awaiting completion
; queue for when the tx SSB comes in.
;
                cmp     [si].qt_handle,0
                jnz     int_txpicky

                ENQHEAD q_txfree
;
; set up the ASB to start the transmission.  set the retcode to zero so that
; any test against FFh (completed) will fail.
;
                align   4
                public  int_txbuildasb
int_txbuildasb:
                mov     di,oASB
                mov     es:[di].asb_command,DIR_TRANSMIT
                mov     al,[si].qt_correlate
                mov     es:[di].asb_correlate,al
                mov     es:[di].asb_retcode,0
                mov     es:[di].txasb_stationid,0
                mov     ax,[si].qt_bytes
                xchg    ah,al
                mov     es:[di].txasb_framelen,ax
;
; hit the ASB ready bit to inform the adapter to process it, and if a receive
; buffer is waiting to be released, make sure the adapter will interrupt us
; when this ASB is finished processing.
;
                DINC    txasbs
                mov     es,segMMIO
                cmp     rxRelease,0
                jnz     int_txparallel

                mov     byte ptr es:ISRAL+ACASET,ISRALASBRDY
                jmp     int_nextreason
int_txparallel:
                mov     byte ptr es:ISRAL+ACASET,ISRALASBRDY+ISRALFREEASB
                jmp     int_nextreason

;------ Enqueue Transmit For Confirmation -----------------------------------;
;
; enqueue the qentry in SI on the tx awaiting confirmation queue, then rejoin
; the normal start transmit processing
;
                align   4
                public  int_txpicky
int_txpicky:
                ENQ     q_txdone,di
                jmp     short int_txbuildasb

;------ Postpone Starting Transmit ------------------------------------------;
;
; the ASB is in use (which happens very rarely).  make sure the adapter will
; interrupt us when the ASB is available.  the qentry is on the tx awaiting
; start queue and will be started off the ASB free interrupt.  if the ASB
; has become available since we checked, the adapter will still generate the
; ASB free interrupt.
;
                public  int_txlater
int_txlater:
                DINC    txlaters
                mov     es,segMMIO
                mov     byte ptr es:ISRAL+ACASET,ISRALFREEASB
                jmp     int_nextreason

;------ ASB Free ------------------------------------------------------------;
;
; earlier we set the FREEASB bit, requesting that the adapter inform us when
; the ASB was available (this is okay even if the ASB was actually available
; just then).  The ASB is now available.  The adapter has reset FREEASB.
;
                public  int_asbfree
int_asbfree:
                and     reasoning,NOT ASBFREE_REASON
                DINC    asbfrees
;
; just in case this interrupt came in with a transmit or receive ARB piggybacked
; with an ASB free interrupt (and they have issued an ASB already), make sure
; the ASB is still available before using it.
;
                mov     es,segSM
                mov     di,oASB
                cmp     es:[di].asb_retcode,0FFh
                SJNE    int_nextreason
;
; check if a transmission needs to be started or a receive buffer is waiting
; to be released (pended earlier because the ASB was in use).
;
                cmp     q_txawaits.q_head,0
                SJNZ    int_txstart
                cmp     rxRelease,0
                SJNZ    int_rxrelease
                jmp     int_nextreason

;------ Timer Tick ----------------------------------------------------------;
;
; increment the tick counter.  once the adapter is openned, this happens
; very rarely.  we might eventually want to perform a few sanity checks on
; the queue structures out of here.
;
                public  int_tick
int_tick:
                and     reasoning,NOT TICK_REASON
                inc     timerTicks
                jmp     int_nextreason
;
; too far away
;
goto_int_initialized:
                jmp     int_initialized
;
; OpenAdapter completed as part of RESETTING.  Set up the error code and
; join EndReset logic.
;
                public  int_openreset
int_openreset:
                xor     ax,ax
                cmp     es:[di].srb_retcode,0
                jz      @F
                mov     ax,HARDWARE_ERROR
                cmp     es:[di].srb_retcode,ERR_OPEN_NET
                jne     @F
                mov     ax,NETWORK_MAY_NOT_BE_CONNECTED
@@:             jmp     int_endreset

;------ SRB Response --------------------------------------------------------;
;
; the SRB has been processed and any return parameters are set.
;
                align   4
                public  int_srb
int_srb:
                and     reasoning,NOT SRB_REASON
;
; if the Intpending flag is set, then a status indication, opcode
; INTERRUPT_INDICATION needs to be generated to the protocol.  This flag is
; cleared by the generation of other indications (to suppress the optional
; interrupt indication when it may not be legal) but it may have been set
; since because of a still pending interrupt request.  so skip this step if
; indications are off.
;
                cmp     Intpending,YES
                jne     @F
                cmp     offDepth,1
                jne     @F

                mov     dx,INTERRUPT_INDICATION
                xor     ax,ax
                call    StatusIndicate
@@:
;
; set ES:DI to point to the SRB then check if the command needs any special
; handling before we generate the confirmation (most don't).
;
                mov     es,segSM
                mov     di,oSRB
;
; if the SRB offset is zero, it was zapped by ResetAdapter.  Just assume
; this is the initialization complete SRB.
;
                test    di,di
                jz      goto_int_initialized
;
; check for errors during development
;
IFDEF DEV
                mov     al,es:[di].srb_command
                mov     lastsrb,al

                mov     al,es:[di].srb_retcode
                cmp     al,0FFh
                je      @F
                test    al,al
                jz      @F
                BPOINT  bad_srb_retcode
@@:
ENDIF
;
; if this is an acknowledgement of an interrupt request SRB then we've already
; done all we had to do.  interrupt requests don't get queued.
;
                mov     al,es:[di].srb_command
                cmp     al,DIR_INTERRUPT
                DJE     int_srbnext
;
; transmit SRBs shouldn't normally be acknowledged since we don't latch
; FREESRB, but just in case we do see one, ignore it.  SRBinuse will be
; cleared when we get the tx ARB (which might come in before or together
; with this interrupt).
;
                cmp     al,DIR_TRANSMIT
                SJE     int_nextreason
;
; if this is an acknowledgement of an open adapter SRB then we need to do
; a few things before generating the acknowledgement.  check if we're
; resetting and take a different path...
;
                cmp     al,DIR_OPENADAPTER
                jne     @F
                call    OpenCompleted
                je      int_openreset
                jmp     short int_srbconfirm
;
; if this is an acknowledgement of a close adapter SRB the SRB is going
; to move back to its initial location again.
;
@@:             cmp     al,DIR_CLOSEADAPTER
                jne     int_srbconfirm
                call    AdapterClosed

;------ int_srbconfirm ------------------------------------------------------;
;
; the last SRB issued has been completed.  ES:DI is the address of the SRB.
;
; dequeue the first qentry off the request awaiting completion queue.  assuming
; requests are serviced in order, this should be the one we want.  if we should
; reach this point with the queue empty (ie. a bug), just skip this part.
;
                public  int_srbconfirm
int_srbconfirm:
                DEQ     q_rqdone,int_srbnext
;
; if the request handle is zero, no confirmation is necessary.
;
                cmp     [si].qr_handle,0
                jz      int_srbfreeq
;
; fill in the status based on the retcode in the SRB.  for Open Adapter we do
; some special case translations, otherwise if non-zero return HARDWARE_FAILURE
;
                mov     cl,es:[di].srb_retcode
                xor     ax,ax
                test    cl,cl
                jz      @F
                mov     ax,NETWORK_MAY_NOT_BE_CONNECTED
                cmp     cl,ERR_OPEN_NET
                je      @F
                mov     ax,HARDWARE_FAILURE
                cmp     cl,ERR_OPEN_LOW
                jb      @F
                cmp     cl,ERR_OPEN_HIGH
                ja      @F
                mov     ax,CONFIGURATION_FAILURE
@@:             mov     [si].qr_status,ax
;
; move the qentry to the confirmation queue so the interrupt handler will
; generate a confirmation out of its post-processing logic
;
                ENQ     q_rqconfirm,di
                jmp     short int_srbnext
;
; no confirmation necessary.  just move the qentry back to the free queue
;
int_srbfreeq:
                ENQ     q_rqfree,di

;------ int_srbnext ---------------------------------------------------------;
;
; check if there are any transmit SRBs that need issuing or any requests
; that need to be started.  Usually there aren't any.
;
                align   4
                public  int_srbnext
int_srbnext:
                cmp     txNeeded,0
                jnz     int_srb_ntx
                EMPTYQ? q_rqtodo
                jnz     int_srb_nrq
;
; clear the SRBinuse flag.
;
                mov     SRBinuse,NO
                jmp     int_nextreason

                align   4
                public  int_srb_ntx
int_srb_ntx:
                call    NextTransmit
                jmp     int_nextreason

                public  int_srb_nrq
int_srb_nrq:
                call    NextRequest
                jmp     int_nextreason

;------ Bad SSB -------------------------------------------------------------;
;
; no match, just release the SSB and ignore it (and hope we live).
;
                public  int_badssb
int_badssb:
                DINT3
                mov     es,segMMIO
                mov     byte ptr es:ISRAL+ACASET,ISRALSSBFREE
                jmp     int_nextreason

;------ SSB -----------------------------------------------------------------;
;
; locate the SSB in ES:DI
;
                align   4
                public  int_ssb
int_ssb:
                and     reasoning,NOT SSB_REASON
                mov     es,segSM
                mov     di,oSSB
;
; check the command code
;
                cmp     es:[di].ssb_command,DIR_TRANSMIT
                jne     int_badssb

;------ Transmit Completed --------------------------------------------------;
;
; Transmit Completed SSB received.  If the original transmit request used a
; non-zero request handle, then the queue entry to generate the confirmation
; for is on the head of the tx awaiting confirmation queue.  Otherwise just
; release the SSB.  We use the correlate to differentiate the two cases.
;
                public  int_txssb
int_txssb:
                DINC    txssbs
;
; get the transmit correlate into AL, the retcode into DL, and the FS byte
; (only valid if AL=22h) in DH.
;
                mov     al,es:[di].ssb_correlate
                mov     dl,es:[di].ssb_retcode
                mov     dh,es:[di].txssb_fstatus
;
; release the SSB
;
                mov     es,segMMIO
                mov     byte ptr es:ISRAL+ACASET,ISRALSSBFREE
;
; if there is nothing on the tx awaiting completion queue, or if the correlates
; don't match, assume this transmit was done with a zero request handle, so no
; confirmation is necessary.
;
                mov     di,offset DGROUP:q_txdone
                EMPTYQ? di
                jz      int_nextreason

                mov     si,[di].q_head
                cmp     al,[si].qt_correlate
                jne     int_nextreason
;
; we need to generate a confirmation.  dequeue the first entry on the transmit
; awaiting completion queue into SI
;
                DEQ
;
; check for any error on transmit.
;
                test    dl,dl
                jnz     int_txfailed
                mov     [si].qt_status,SUCCESS
;
; move the qentry to the confirmation queue.  the confirmation will be
; generated out of the unnested interrupt handler after the hardware has
; been rearmed and interrupts enabled, avoiding interrupt nesting.
;
                public  int_txconfirm
int_txconfirm:
                ENQ     q_txconfirm,di
                jmp     short int_nextreason
;
; an error on transmit.  if the error code indicates that we should check
; the FS byte, then set the correct error code for source-routing based
; on the FS byte.  otherwise just return HARDWARE_ERROR.  we're only going
; to maintain the hardware error transmitting counter on this path, rather
; than impacting the performance of the non-confirmation path where we
; currently don't check for errors.
;
                public  int_txfailed
int_txfailed:
                mov     ax,HARDWARE_ERROR
                cmp     dl,ERR_CHECK_FS
IFDEF ANYSTATS
                jne     int_txhardware
ELSE
                jne     @F
ENDIF
                mov     ax,NO_SUCH_DESTINATION
                test    dh,FS_RECOGNIZED
                jz      @F
                mov     ax,TRANSMIT_ERROR
@@:             mov     [si].qt_status,ax
                jmp     short int_txconfirm
IFDEF ANYSTATS
int_txhardware:
                INCANY  ms_hard_serr
                jmp     @B
ENDIF

;------ Next Reason ---------------------------------------------------------;
;
; Before rearming, if there are any more interrupt reasons left to deal with
; loop back around to the top once again...
;
                align   4
                public  int_another
int_another:
                jmp     int_reasonloop

                align   4
                public  int_nextreason
int_nextreason:
                mov     cx,reasoning
                test    cx,cx
                jnz     int_another

;------ Rearm Adapter -------------------------------------------------------;
;
; Unless indications are still disabled (as a result of returning from an
; indication to the protocol with the indications flag cleared) reenable
; interrupts from the adapter through the PIC IMR.  CLI in effect.
;
                public  int_rearm
int_rearm:
                dec     offDepth
                jnz     @F

                mov     dx,IMRport
                in      al,dx
                and     al,enableMask
                out     dx,al
@@:
;
; release the interrupt through the adapter (only affects Adapters I & II)
;
                mov     dx,releasePort
                out     dx,al
;
; switch back to the original stack. There is no stack supplied to the Protocol
; for indication completes, and confirmations.
;
IFDEF DOS
                mov     ss,DDStkSeg                     ; restore old SS
                mov     sp,DDStkOff                     ; restore old SP
ENDIF

;------ Post Processing -----------------------------------------------------;
;
; IndicationComplete, TransmitConfirm and RequestConfirm require interrupts
; to be enabled and the hardware rearmed before they are invoked.  Since the
; protocol can take its time handling these functions, we generate them only
; out of the first level interrupt to keep interrupt nesting to a minimum.
;
                cmp     nestLevel,1
                ja      int_exit
;
; Once we reenable interrupts ICpending might be set to YES again or another
; entry moved to the confirmation queue, so we loop around, checking them with
; interrupts disabled and only leaving if both ICpending is NO and the
; confirmation queues are empty.  be careful with the critical sections here.
;
; if ICpending is YES then we need to generate an Indication Complete to
; the protocol to allow it to do post processing.
;
; interrupts are disabled.
;
                public  int_postloop
int_postloop:
                cmp     ICpending,YES
                je      int_postic
;
; if there is anything on the transmit confirmation queue, then we need to
; generate a TransmitConfirm to the protocol
;
                EMPTYQ? q_txconfirm
                jnz     int_posttx
;
; if there is anything on the request confirmation queue, then we need to
; generate a RequestConfirm to the protocol
;
                EMPTYQ? q_rqconfirm
                jnz     int_postrq

;------ int_exit ------------------------------------------------------------;
;
; done.  decrement the interrupt nest level.  interrupts off.
;
                public  int_exit
int_exit:
                dec     nestLevel
;
; return from interrupt.
;
                public  int_return
int_return:
IFDEF OS2
;
; if we entered in real mode this will just return to int_toreal because of
; the extra return address on the stack.
;
                clc
                ret
;
; return to real mode, and really return from interrupt
;
                public  int_toreal
int_toreal:
                mov     si,offset DGROUP:devHeader
                mov     dl,DevHlp_ProtToReal
                call    devHelper
                clc
                ret
ELSE
;
; pop off registers that were saved at the interrupt routine entry on the
; existing stack
;
                POPR    ax,bx,cx,dx,si,di,es,ds,bp
                iret
ENDIF

;------ Indication Complete -------------------------------------------------;
;
; turn interrupts on and generate an IndicationComplete to the protocol.  then
; around the post-processing loop again.
;
; interrupts are disabled.
;
                align   4
                public  int_postic
int_postic:
                mov     ICpending,NO

                sti

                push    common.cc_ID
                push    protDS
                call    pdispatch.pd_indcomplete

                cli
                jmp     int_postloop

;------ Transmit Confirmation -----------------------------------------------;
;
; enable interrupts and generate a TransmitConfirm for the next qentry on
; the transmit confirmation queue.  make sure to move the qentry back to the
; free queue so it can be reused by another TransmitChain call from the
; confirmation handler if necessary.
;
                align   4
                public  int_posttx
int_posttx:
                DEQ     q_txconfirm
                ENQHEAD q_txfree                ; trashes AX

                mov     ax,[si].qt_protid       ; SI on freq but CLI as yet
                mov     cx,[si].qt_handle
                mov     dx,[si].qt_status

                sti

                push    ax
                push    common.cc_ID
                push    cx
                push    dx
                push    protDS
                call    pdispatch.pd_txconfirm

                cli
                jmp     int_postloop

;------ Request Confirmation ------------------------------------------------;
;
; enable interrupts and generate a RequestConfirm for the next qentry on
; the request confirmation queue
;
                public  int_postrq
int_postrq:
                DEQ     q_rqconfirm

                sti

                push    [si].qr_protid
                push    common.cc_ID
                push    [si].qr_handle
                push    [si].qr_status
                push    [si].qr_opcode
                push    protDS
                call    pdispatch.pd_reqconfirm

                cli
;
; move the qentry to the free queue and around the post-processing loop again
;
                ENQ     q_rqfree,di
                jmp     int_postloop

;------ Adapter Check -------------------------------------------------------;
;
; an Adapter Check has occured.  the reason is in shared memory at the offset
; currently in the Write Window Close register.  set AX to the reason.
;
                public  int_check
int_check:
                and     reasoning,NOT CHECK_REASON
                mov     ax,es:[WWCR]
                xchg    al,ah
                mov     di,ax
                mov     es,segSM
                mov     ax,es:[di]
                xchg    ah,al
;
; note that the adapter has closed on a hardware fault
;
                mov     dx,status.ms_flags.loword
                and     dx,NOT ST_MAC_IS_OPEN
                and     dx,NOT 7
                or      dx,ST_FAULT
                mov     status.ms_flags.loword,dx

                mov     state,DECEASED
;
; relocate the SRB
;
                call    AdapterClosed
;
; flush out any queued requests
;
                call    FlushQueues
;
; generate a status indication to the protocol, opcode ADAPTER_CHECK
;
                mov     dx,ADAPTER_CHECK
                call    StatusIndicate
                jmp     int_nextreason

;------ Ring Status ---------------------------------------------------------;
;
; Ring Status Change ARB.  We get one after Open Adapter, and other times
; when the ring status changes.  Set AX to the new status, and check if it
; indicates the adapter has closed.
;
                public  int_ringstatus
int_ringstatus:
                mov     ax,es:[di].ring_rstatus
                xchg    ah,al
                test    ax,0D00h
                jz      @F
;
; adapter closed.  modify status
;
                mov     state,DECEASED
                and     status.ms_flags.loword,NOT ST_MAC_IS_OPEN
;
; relocate the SRB
;
                call    AdapterClosed
;
; flush out any queued requests
;
                call    FlushQueues
;
; release the ARB
;
@@:             mov     es,segMMIO
                mov     byte ptr es:ISRAL+ACASET,ISRALARBFREE
;
; generate a status indication to the protocol, opcode RING_STATUS
;
                mov     dx,RING_STATUS
                call    StatusIndicate
                jmp     int_nextreason

;------ StartReset ----------------------------------------------------------;
;
; the interrupt handler has been entered in PRERESET state.  change state to
; RESETTING, and set reasonMask back to normal.
;
                public  int_startreset
int_startreset:
                mov     state,RESETTING
                mov     reasonMask,ISRPMASK
;
; generate the StartReset indication
;
                mov     dx,START_RESET
                xor     ax,ax
                call    StatusIndicate
;
; no indication complete is necessary at this point (it might get generated
; later during open adapter processing when it shouldn't).  we'll set this
; again when we generate the EndReset indication
;
                mov     ICpending,NO
;
; decrement the indication offDepth to account for the increment on entry
; to the interrupt handler, then save the current setting (0 or 1) from
; StartReset in startDepth.  NDIS allows the EndReset indication to be
; produced even if indications were disabled by StartReset, so we'll set
; offDepth to 0 for now, and add startDepth back in before EndReset.  this
; allows either StartReset or EndReset to disable other indications.
;
                dec     offDepth
                xor     al,al
                xchg    al,offDepth
                mov     startDepth,al
;
; we're now going to reset the adapter.  in order for it to work correctly,
; interrupts must be enabled during its processing, we must be able to enter
; the interrupt handler and respond to timer ticks, and so forth.  so we
; switch off the stack now (ResetAdapter enables interrupts thru PIC).
;
                dec     nestLevel               ; the one added by IntHandler
ifdef DOS
                mov     ss,DDStkSeg             ; switch off stack
                mov     sp,DDStkOff
endif
;
; now reset the adapter.
;
                call    ResetAdapter
;
; return from interrupt.
;
                jmp     int_return
;
; initialization completed with an error, and we were not resetting.  change
; state to DECEASED.
;
                public  int_init_fail
int_init_fail:
                mov     state,DECEASED
                jmp     int_nextreason
;
; initialization completed with no error, and we were not resetting.  change
; state to CLOSED.
;
int_init_done:
                mov     state,CLOSED
                jmp     int_nextreason

;------ Initialization Completed --------------------------------------------;
;
; The initialization complete SRB has come in from the adapter, as a response
; to a latching the reset port.  Note that this is not really a response to
; an SRB, but to the latching of the reset port.
;
; After a reset the SRB location is moved (in order to allow the larger than
; normal Open Adapter SRB to be issued).  We need to reload the location of
; the SRB from the Write Region Base (use ES:DI).
;
                public  int_initialized
int_initialized:
                mov     es,segMMIO
                mov     ax,es:WRBR
                xchg    al,ah
                mov     oSRB,ax
                mov     es,segSM
                mov     di,ax
;
; Save the bring up code and set the acked flag for anyone polling for
; completion of the reset.
;
                mov     ax,es:[di].init_bringup
                mov     retcode,ax
                mov     acked,YES
;
; Set the status field to indicate whether the hardware is operational or
; not.  AX has the bringup code.
;
                mov     dl,ST_OK
                test    ax,ax
                jz      @F
                mov     dl,ST_DIAG_ERR
@@:             mov     dh,status.ms_flags.loword.lobyte
                and     dh,NOT 7
                or      dh,dl
                mov     status.ms_flags.lobyte,dh
;
; Check for error.  If the reset failed and we were RESETTING, join EndReset
; logic with HARDWARE_ERROR.
;
                test    ax,ax
                jz      @F
                cmp     state,RESETTING
                jne     int_init_fail
                mov     ax,HARDWARE_ERROR
                jmp     short int_endreset
@@:
;
; If the adapter is jumpered for 16Mbps, set the rate to 16
;
                test    es:[di].init_status,INIT_16_MBPS
                jz      @F
                mov     rate,16
@@:
;
; some of the adapters with 64K of SRAM reserve the top 512 bytes which
; must for some reason be initialized by us.
;
                test    adapterBits,ZERO_TOP_512
                jz      @F
                mov     di,0FE00h
                mov     cx,512
                xor     al,al
        rep     stosb
@@:
;
; Turn off the adapter timer.
;
                mov     es,segMMIO
                mov     byte ptr es:TVRL,0h
                mov     byte ptr es:TCRH,TCRHENABLE+TCRHRUN+TCRHRELOAD
;
; If not resetting, continue as normal...
;
                cmp     state,RESETTING
                jne     int_init_done
;
; If the original state was OPENNED, then issue an OpenAdapter and continue
; with our processing.
;
                cmp     oldstate,OPENNED
                jne     @F
                call    OpenAdapter
                jmp     int_nextreason
@@:
;
; Change state to CLOSED and set AX to success.
;
                mov     state,CLOSED
                xor     ax,ax

;------ EndReset ------------------------------------------------------------;
;
; Reset is almost complete.  AX holds the error code.  Do some final cleanup
; and generate the EndReset indication.
;
; If AX indicates an error, then change state to DECEASED.
;
                public  int_endreset
int_endreset:
                test    ax,ax
                jz      @F
                mov     state,DECEASED
@@:
;
; Restore receiving from status.ms_filter
;
                cmp     status.ms_filter,0
                jz      @F
                mov     receiving,YES
@@:
;
; Generate the EndReset indication with the error code in AX.
;
; Before we do this, we add startDepth back into offDepth.  So if indications
; were disabled by StartReset, they will be disabled after EndReset returns.
;
                mov     cl,startDepth
                add     offDepth,cl

                mov     dx,END_RESET
                call    StatusIndicate
;
; Continue with our lives.
;
                jmp     int_nextreason
IntHandler      endp

BREAK <NextTransmit - Issue Pending Transmit SRB>
;------ NextTransmit --------------------------------------------------------;
;                                                                            ;
;       NextTransmit()                                                       ;
;                                                                            ;
;       May destroy AX,BX,CX,DX and flags.                                   ;
;                                                                            ;
;       Issue a pending transmit SRB to the adapter and decrement the        ;
;       count.  Set SRBinuse again.                                          ;
;                                                                            ;
;----------------------------------------------------------------------------;

                align   4
NextTransmit    proc    near
                PUSHR   di,es
;
; take over the SRB and decrement the count of tx SRBs pending
;
                mov     SRBinuse,YES
                dec     txNeeded
;
; set ES:DI to point to the SRB on the adapter
;
                mov     es,segSM
                mov     di,oSRB
;
; set the command code for TRANSMIT.FRAME and dummy out the station id
;
                mov     es:[di].srb_command,DIR_TRANSMIT
                mov     es:[di].txsrb_stationid,0
;
; hit the command ready bit
;
                DINC    txsrbs
                mov     es,segMMIO
                mov     byte ptr es:ISRAL+ACASET,ISRALSRBRDY

                POPR    di,es
                ret
NextTransmit    endp

BREAK <StatusIndicate - Generate Status Indication to Protocol>
;------ StatusIndicate ------------------------------------------------------;
;                                                                            ;
;       void StatusIndicate()                                                ;
;                                                                            ;
;       DX = opcode                                                          ;
;       AX = reason or 0                                                     ;
;                                                                            ;
;       Registers AX,BX,CX,DX,ES and flags may be destroyed.                 ;
;                                                                            ;
;       Generates a status indication to the protocol.                       ;
;                                                                            ;
;----------------------------------------------------------------------------;

                align   4
StatusIndicate  proc    near
;
; remember that an IndicationComplete needs to be done
;
                mov     ICpending,YES
;
; cancel any interrupt indication that is pending, since we are allowed to
; collapse it (and need to if the protocol is going to leave indications
; off) if any other indications are generated
;
                mov     Intpending,NO
;
; do an implicit IndicationOff.
;
; increment the IndicationOff depth.  we don't need to actually disable
; interrupts from the adapter since we did that earlier.
;
                inc     offDepth
;
; default indications flag to 0xFF
;
                mov     indications,0FFh
;
; issue a status indication to the protocol.  enable interrupts around the
; call just in case the protocol spins its wheels a little.
;
                sti

                push    common.cc_ID
                push    ax
                push    ds
                pushc   <offset DGROUP:indications>,ax
                push    dx
                push    protDS
                call    pdispatch.pd_status

                cli
;
; if indications flag wasn't cleared, do an implicit IndicationOn.
;
; decrement the IndicationOff depth back to zero so that interrupts from
; the adapter will be reenabled when we rearm it.
;
                cmp     indications,0
                jz      @F
                dec     offDepth
@@:
                ret
StatusIndicate  endp

BREAK <OpenCompleted - OpenAdapter completed>
;------ OpenCompleted -------------------------------------------------------;
;                                                                            ;
;       OpenCompleted()                                                      ;
;                                                                            ;
;       ES:DI = address of the completed SRB                                 ;
;                                                                            ;
;       Returns:                                                             ;
;       flags set for JE if RESETTING                                        ;
;                                                                            ;
;       May destroy AX,BX,CX,DX and flags.  All other registers preserved.   ;
;                                                                            ;
;       Called when the Open Adapter SRB is acknowledged, indicating that    ;
;       the adapter is now open.                                             ;
;                                                                            ;
;       The confirmation will be generated when we return, but we need to    ;
;       take care of a few specific things first, such as changing state if  ;
;       successful, and setting the offsets of the SRB et al from the        ;
;       return parameters.                                                   ;
;                                                                            ;
;----------------------------------------------------------------------------;

OpenCompleted   proc    near
;
; set CL to the current state before we change it
;
                mov     cl,state
;
; if the retcode indicates that the Open Adapter was successful, then change
; state to OPENNED and set the openned bit in the status field
;
                cmp     es:[di].srb_retcode,0
                jnz     open_exit

                mov     state,OPENNED
                or      status.ms_flags.loword,ST_MAC_IS_OPEN
;
; copy the offsets of the ASB, SRB, ARB, SSB from the Open Adapter Complete
; SRB since they change after the open completes (the Open Adapter SRB is much
; bigger than the normal SRBs).  remember that these are byte-swapped.
;
                mov     ax,es:[di].openc_asbaddr
                xchg    al,ah
                mov     oASB,ax
                mov     ax,es:[di].openc_srbaddr
                xchg    al,ah
                mov     oSRB,ax
                mov     ax,es:[di].openc_arbaddr
                xchg    al,ah
                mov     oARB,ax
                mov     ax,es:[di].openc_ssbaddr
                xchg    al,ah
                mov     oSSB,ax
;
; set txneeded to 1 instead of 0 so we'll get the transmit buffer back
; even if we haven't got anything to transmit just now...
;
IFDEF PREFETCH
                mov     txNeeded,1
ENDIF
;
; return with flags set for JE if the original state was RESETTING
;
open_exit:
                cmp     cl,RESETTING
                ret
OpenCompleted   endp

BREAK <AdapterClosed - Adapter Closed>
;------ AdapterClosed -------------------------------------------------------;
;                                                                            ;
;       AdapterClosed()                                                      ;
;                                                                            ;
;       May destroy AX and flags.  All other registers preserved.            ;
;                                                                            ;
;       Called when the Close Adapter SRB is acknowledged, or a ring status  ;
;       or adapter check occurs indicating that the adapter is now closed.   ;
;                                                                            ;
;       After the adapter is closed, the SRB location is moved back to its   ;
;       original reset location, and must be reloaded from the WRB.  This    ;
;       is to allow the larger than normal Open Adapter SRB to be issued.    ;
;                                                                            ;
;----------------------------------------------------------------------------;

AdapterClosed   proc    near
                PUSHR   es
;
; reload the SRB location from the WRB
;
                mov     es,segMMIO
                mov     ax,es:WRBR
                xchg    al,ah
                mov     oSRB,ax
;
; set the number of transmits needed to 0 (we'll get access errors from the
; adapter if these are processed after the adapter closes)
;
                mov     txNeeded,0

                POPR    es
                ret
AdapterClosed   endp

                END_CODE
                end

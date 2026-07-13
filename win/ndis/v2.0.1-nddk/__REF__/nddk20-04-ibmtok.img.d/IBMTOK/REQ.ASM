                page    88,122
                title   REQ - Request entry and subfunctions (2.0.1 Spec)

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

;------ REQ.ASM -------------------------------------------------------------;
;                                                                            ;
;       This file contains the Request entry point and its subfunctions.     ;
;                                                                            ;
;       09-21-88 Connery                                                     ;
;       Original version.                                                    ;
;                                                                            ;
;       03-17-89 Connery                                                     ;
;       OR earlyrelease option into open options supplied on Request to      ;
;       support the EARLYRELEASE keyword added for 16/4 support.             ;
;                                                                            ;
;       02-25-90 2.0a Connery                                                ;
;       Dropped multiple adapter support.  Use new ENQ/DEQ macros.  Added    ;
;       ResetMAC support.  Moved most of the OpenAdapter logic into a        ;
;       subroutine in UTIL.  Minor performance tweaks.                       ;
;                                                                            ;
;----------------------------------------------------------------------------;

.xlist
include includes.mac
.list
.lfcond
.sall

;------ Exported Interfaces -------------------------------------------------;

                public  Request
                public  NextRequest
                public  RqAddMulticast
                public  RqCloseAdapter
                public  RqDelMulticast
                public  RqOpenAdapter
                public  RqReadLog
                public  RqReset
                public  RqSetAddress
                public  RqSetFilter
                public  RqSetFunctional
                public  RqUnconditional
IFDEF ANYSTATS
                public  RqClearStats
ENDIF

                public  request_table

;------ Imported Interfaces -------------------------------------------------;

                extrn   FlushQueues:NEAR        ; from util
                extrn   OpenAdapter:NEAR        ; from util
                extrn   ResetAdapter:NEAR       ; from util

IFDEF ANYSTATS
                DOSXTRN timerCount:DWORD
                OS2XTRN infoSeg:WORD
ENDIF
                extrn   Intpending:BYTE
                extrn   SRBinuse:BYTE
                extrn   common:BYTE
                extrn   cs_ds:WORD
                extrn   multilist:BYTE
                extrn   oARB:WORD
                extrn   oASB:WORD
                extrn   oSRB:WORD
                extrn   oSSB:WORD
                extrn   oldstate:BYTE
                extrn   openSoft:WORD
                extrn   pdispatch:BYTE
                extrn   protDS:WORD
                extrn   q_rqdone:BYTE
                extrn   q_rqfree:BYTE
                extrn   q_rqtodo:BYTE
                extrn   reasonMask:WORD
                extrn   receiving:BYTE
                extrn   segMMIO:WORD
                extrn   segSM:WORD
                extrn   specific:BYTE
                extrn   state:BYTE
                extrn   status:BYTE
                extrn   txNeeded:BYTE

NEWPAGE <Dispatch Table>
;------ Dispatch Table ------------------------------------------------------;
;
; 0 offsets indicate unsupported functions.  Request Interrupt has a 0 offset
; but IS supported, it is just handled directly by the Request function
; to streamline things a bit.  Ditto ResetMAC, though for other reasons.
;
                BEGIN_DATA
                even
request_table   label   word
                dw      0                               ; 0 = illegal function
                dw      0                               ; 1 = diagnostics
                dw      offset CGROUP:RqReadLog         ; 2 = read error log
                dw      offset CGROUP:RqSetAddress      ; 3 = set address
                dw      offset CGROUP:RqOpenAdapter     ; 4 = open adapter
                dw      offset CGROUP:RqCloseAdapter    ; 5 = close adapter
                dw      0                               ; 6 = reset mac
                dw      offset CGROUP:RqSetFilter       ; 7 = set packet filter
                dw      offset CGROUP:RqAddMulticast    ; 8 = add multicast
                dw      offset CGROUP:RqDelMulticast    ; 9 = delete multicast
                dw      0                               ; 10 = update statistics
IFDEF ANYSTATS
                dw      offset CGROUP:RqClearStats      ; 11 = clear statistics
ELSE
                dw      0
ENDIF
                dw      0                               ; 12 = interrupt
                dw      offset CGROUP:RqSetFunctional   ; 13 = set functional
                dw      offset CGROUP:RqUnconditional   ; 14 = set lookahead
                END_DATA

                BEGIN_CODE

NEWPAGE <Request Entry>
;------ Request -------------------------------------------------------------;
;                                                                            ;
;       int far pascal Request(                                              ;
;                               WORD ProtID,                                 ;
;                               WORD Handle,                                 ;
;                               WORD Word,                                   ;
;                               DWORD Bufp,                                  ;
;                               WORD Opcode,                                 ;
;                               WORD MACDS      );                           ;
;                                                                            ;
;       This is the generic request entry into the MAC.  Various control     ;
;       functions can be requested by appropriate setting of Opcode.         ;
;                                                                            ;
;       This function must be completely reentrant and can be called at any  ;
;       time.  None of the functions are remotely performance critical,      ;
;       with the single exception of Interrupt.                              ;
;                                                                            ;
;       This function merely enqueues the request on the request todo queue  ;
;       and then calls NextRequest to start the next request if the          ;
;       SRBinuse flag indicates that the adapter is not busy.  If the        ;
;       adapter is busy, then NextRequest will be called out of the          ;
;       interrupt handler when the current request completes.                ;
;                                                                            ;
;       The Request Interrupt function is given special priority handling    ;
;       because of its performance critical nature.  Interrupt Request is    ;
;       handled without any queueing or dequeueing and always returns        ;
;       SUCCESS without any confirmation.  If possible, we generate an       ;
;       interrupt SRB to the adapter, which will generate an SRB ack         ;
;       interrupt eventually.  However, if the SRB is in use we just set     ;
;       the Intpending flag, which is checked when ANY SRB completes.  This  ;
;       way we can piggyback on top of another SRB if necessary.             ;
;                                                                            ;
;----------------------------------------------------------------------------;

Request         proc    far
                push    bp
                mov     bp,sp
                PUSHR   si,di,ds
;
; set up DS.
;
                mov     ds,cs:cs_ds

;------ Special Handling of Request Interrupt -------------------------------;
;
; check for Interrupt Request.  if so, we give it priority handling since
; it is performance critical.
;
                mov     di,[bp].rq_opcode
                cmp     di,INTERRUPT
                jne     rq_noncrit
;
; set Intpending so that we will generate an Interrupt Indication when the
; SRB ack interrupt comes in
;
                public  rq_interrupt
rq_interrupt:
                mov     Intpending,YES
;
; set AX to SUCCESS
;
                xor     ax,ax
;
; if the SRB is not in use, take it over.
;
                mov     dl,YES
                xchg    dl,SRBinuse
                test    dl,dl
                jnz     rq_piggyback
;
; set up for a DIR.INTERRUPT SRB to the adapter
;
                mov     es,segSM
                mov     di,oSRB
                mov     es:[di].srb_command,DIR_INTERRUPT
;
; hit the command ready bit and return SUCCESS.  make sure the adapter
; interrupts us when the SRB is free again (this only lasts for one SRB).
;
                mov     es,segMMIO
                mov     byte ptr es:ISRAL+ACASET,ISRALSRBRDY+ISRALFREESRB

;------ rq_exit -------------------------------------------------------------;
;
; done.  return code in AX
;
                public  rq_exit
rq_exit:
                sti
                POPR    si,di,ds
                pop     bp
                ret     RQRET

;------ rq_piggyback --------------------------------------------------------;
;
; the SRB was in use.  in case a tx SRB is in progress, make sure the adapter
; interrupts us when the SRB is free (it normally won't for transmits).  that
; way we can generate the interrupt indication out of the SRB free interrupt.
;
                public  rq_piggyback
rq_piggyback:
                mov     es,segMMIO
                mov     byte ptr es:ISRAL+ACASET,ISRALFREESRB
                jmp     rq_exit

;------ Non-Critical Request Handling ---------------------------------------;
;
; special case ResetMAC.  we will accept it even when the hardware has gone
; dead (that's the whole point) and don't want to delay it while commands on
; the queue get processed (they may not complete).
;
                public  rq_noncrit
rq_noncrit:
                cmp     di,RESET_MAC
                jne     rq_general

                public  rq_reset
rq_reset:
                call    RqReset
                jmp     rq_exit

;------ General Purpose Request Handling ------------------------------------;
;
; if a hardware fault has occured, reject all requests
;
                public  rq_general
rq_general:
                cmp     state,REJECTSTATE
                jae     rq_invalid
;
; check if function is supported (an in-range opcode and a non-zero offset in
; the dispatch table)
;
                cmp     di,MAX_REQUEST_OPCODE
                ja      rq_notsupported

                shl     di,1
                cmp     request_table[di],0
                jz      rq_notsupported
;
; grab a free request queue entry
;
                cli
                DEQ     q_rqfree,rq_unable
;
; copy parameters to request queue entry
;
                push    si

                lea     di,[si].qr_opcode
                mov     ax,ds
                mov     dx,ss
                mov     es,ax
                mov     ds,dx
                lea     si,[bp].rq_opcode
                mov     cx,(SIZE rqparms - STDSIZE) / 2
        rep     movsw

                mov     ds,ax
                pop     si
;
; enqueue last on request todo queue
;
                ENQ     q_rqtodo,di
;
; take over SRBinuse if we can.
;
                mov     al,YES
                xchg    al,SRBinuse
                test    al,al
                jnz     rq_confirm
;
; call NextRequest to start the next request
;
                call    NextRequest
;
; return to protocol
;
                public  rq_confirm
rq_confirm:
                mov     ax,REQUEST_QUEUED
                jmp     rq_exit
;
; function not supported
;
                public  rq_notsupported
rq_notsupported:
                mov     ax,NOT_SUPPORTED
                jmp     rq_exit
;
; invalid state
;
                public  rq_invalid
rq_invalid:
                mov     ax,INVALID_FUNCTION
                jmp     rq_exit
;
; out of resources.
;
                public  rq_unable
rq_unable:
                mov     ax,OUT_OF_RESOURCES
                jmp     rq_exit
Request         endp

NEWPAGE <NextRequest - Start Next Request>
;------ NextRequest ---------------------------------------------------------;
;                                                                            ;
;       void NextRequest()                                                   ;
;                                                                            ;
;       AX,BX,CX,DX and flags may be destroyed.                              ;
;                                                                            ;
;       This function is called to start the next request.  It may be        ;
;       called from the Request entry or out of the interrupt handler        ;
;       when the last request completes.  It dequeues the next request       ;
;       off the request dodo queue and starts it.  If the function will      ;
;       complete asynchronously, then we are done.  If it completes          ;
;       synchronously, then we generate a confirmation and see if there      ;
;       is anything more in the queue to start.                              ;
;                                                                            ;
;       This routine is entered with SRBinuse set and interrupts disabled.   ;
;       It returns with interrupts disabled.  On return, SRBinuse may or     ;
;       may not be set.                                                      ;
;                                                                            ;
;       This function handles most of the overhead in dealing with a         ;
;       given request.  The opcode-specific code is handled by subfunctions  ;
;       named Rq... which are called through a jump table.                   ;
;                                                                            ;
;       The subfunctions themselves handle very little other than their      ;
;       specified task.  Some functions perform their tasks synchronously,   ;
;       while others are asynchronous.  This routine generates confirmations ;
;       in both cases--for synchronous functions the confirmation is         ;
;       generated before the function returns, so that we can loop around    ;
;       to handle any other requests which were queued.                      ;
;                                                                            ;
;       A control function moves through various queues during its           ;
;       processing.  A free queue entry is filled in and then moved to       ;
;       the end of the todo queue.  The first entry on the todo queue        ;
;       is started.  If it finishes synchronously, then the confirmation     ;
;       is generated out of this function and the queue entry returns to     ;
;       the free queue.  If it finishes asynchronously, then the queue       ;
;       entry is moved to the done queue where we wait for an interrupt      ;
;       to come in.  When the interrupt confirming the completion of the     ;
;       subfunction comes in, the queue entry is moved to the confirmation   ;
;       queue.  The interrupt handler then generates confirmations for       ;
;       all of the entries on the confirmation queue one by one and moves    ;
;       them back to the free queue.                                         ;
;                                                                            ;
;       Be careful with the critical sections here.  Interrupts must be      ;
;       enabled when generating confirmations but we have to ensure that we  ;
;       don't miss anything that gets onto the todo queue while we have      ;
;       SRBinuse is still set.                                               ;
;                                                                            ;
;       The subfunctions are invoked with interrupts disabled, and for       ;
;       those which return REQUEST_QUEUED interrupts must be disabled once   ;
;       the command has been set in motion so that the request can be        ;
;       moved to the done queue before the interrupt comes in.  If a         ;
;       subfunction enables interrupts it must disable them before return.   ;
;                                                                            ;
;----------------------------------------------------------------------------;

NextRequest     proc    near
                PUSHR   si,di,es
;
; set SRBinuse (for when we're called on SRB free interrupt)
;
                mov     SRBinuse,YES
;
; dequeue first request todo entry.  interrupts are disabled.
;
nr_loop:
                DEQ     q_rqtodo,nr_empty
;
; call appropriate function through jump table.
;
                mov     bx,[si].qr_opcode
                shl     bx,1
                call    request_table[bx]
;
; if the function returns REQUEST_QUEUED then the function will complete
; asynchronously so just move the queue entry to the done queue and return
;
                cmp     ax,REQUEST_QUEUED
                jne     nr_synchronous

                ENQ     q_rqdone,di
                jmp     short nr_exit
;
; the function completed synchronously, generate a RequestConfirmation unless
; the handle was zero (indicating the protocol does not want confirmation).
;
; note that interrupts must be enabled when generating confirmations
;
nr_synchronous:
                cmp     [si].qr_handle,0
                jz      nr_free

                sti

                push    [si].qr_protid
                push    common.cc_ID
                push    [si].qr_handle
                push    ax
                push    [si].qr_opcode
                push    protDS
                call    pdispatch.pd_reqconfirm

                cli
;
; move the queue entry back to the free queue
;
nr_free:
                ENQ     q_rqfree,di
;
; since the function completed synchronously we won't be getting an interrupt
; so if there is anything left in the todo queue it has to be handled now
;
                jmp     nr_loop
;
; nothing left to process.  just reset SRBinuse and return.  this branch
; should only be taken if we performed a synchronous request and looped
; around and there was nothing in the todo queue.
;
nr_empty:
                mov     SRBinuse,NO
;
; disable interrupts and return
;
nr_exit:
                POPR    si,di,es
                ret
NextRequest     endp

NEWPAGE <RqUnconditional - Return Success>
;------ RqUnconditional -----------------------------------------------------;
;                                                                            ;
;       int RqUnconditional()                                                ;
;                                                                            ;
;       returns:                                                             ;
;                                                                            ;
;       AX = return code                                                     ;
;                                                                            ;
;       Registers AX,BX,CX,DX,ES may be destroyed by this function.          ;
;                                                                            ;
;       This function returns SUCCESS.  It can be used to handle those       ;
;       functions which don't really do anything in this driver (such as     ;
;       SetLookahead) but which should return SUCCESS anyway.                ;
;                                                                            ;
;       This function is performed synchronously.                            ;
;                                                                            ;
;----------------------------------------------------------------------------;

RqUnconditional proc    near
                xor     ax,ax
                ret
RqUnconditional endp

NEWPAGE <RqReset - Request ResetMAC Subfunction>
;------ RqReset -------------------------------------------------------------;
;                                                                            ;
;       int RqReset()                                                        ;
;                                                                            ;
;       returns:                                                             ;
;                                                                            ;
;       AX = return code                                                     ;
;                                                                            ;
;       Registers AX,BX,CX,DX,ES may be destroyed by this function.          ;
;                                                                            ;
;       This function handles Request ResetMAC.  Typically this will be      ;
;       issued when the adapter has indicated a hardware failure of some     ;
;       kind to the protocol.  The most common reason on the IBM Token Ring  ;
;       Adapter would be as a result of a Ring Status or Adapter Check       ;
;       which indicated that the adapter had removed itself from the         ;
;       network.  These can occur if the adapter is unplugged from the       ;
;       network for too long (typically about 30 seconds) or if the adapter  ;
;       is not being serviced quickly enough.                                ;
;                                                                            ;
;       This request can be issued with the adapter open or closed.          ;
;       However, if an OpenAdapter or CloseAdapter request is outstanding,   ;
;       it is undefined what state the MAC will end up in once the reset is  ;
;       completed.                                                           ;
;                                                                            ;
;       The request will be handled as follows:                              ;
;                                                                            ;
;       RqReset:                                                             ;
;                                                                            ;
;       1.  Save the current state and receiving flag.                       ;
;                                                                            ;
;       2.  Change state to PRERESET.  This will prevent any new requests    ;
;           from being accepted.  Set receiving to NO.  This will prevent    ;
;           any packets from being received for the moment.                  ;
;                                                                            ;
;       3.  Generate failed confirmations for all requests currently         ;
;           queued.  The queues must be emptied anyway, and we can't         ;
;           afford to wait for things which might not complete, and this     ;
;           will increase compatibility with stacks that do not yet          ;
;           handle reset (ie. a Vector configuration where some other        ;
;           stack issued the reset).                                         ;
;                                                                            ;
;       4.  Set reasonMask to 0.  This will cause the interrupt handler      ;
;           to mask off all the reasons for the interrupt and fall into      ;
;           a path not normally executed where we can put the test for       ;
;           reset.                                                           ;
;                                                                            ;
;       5.  Request an interrupt from the adapter.  We do this by turning    ;
;           on the adapter timer.  There were several other options but      ;
;           this was the simplest that worked most reliably.  Using the      ;
;           Request Interrupt logic fails if the adapter has disabled        ;
;           itself as a result of a failed initialization, and issuing a     ;
;           ResetAdapter from here may blow things in progress up.           ;
;                                                                            ;
;       6.  Return SUCCESS. This does not imply the Reset is done.  The      ;
;           interrupt will come in with indications enabled, so we can       ;
;           issue StartReset and EndReset indications from there.  Although  ;
;           the NDIS specification is not clear on this point, doing         ;
;           otherwise risks reentrancy problems in protocol stacks.          ;
;                                                                            ;
;       Interrupt Handler/No Reason for Interrupt/PRERESET:                  ;
;                                                                            ;
;       7.  Set state to RESETTING.                                          ;
;                                                                            ;
;       8.  Generate a StartReset indication.                                ;
;                                                                            ;
;       9.  Reset the adapter.                                               ;
;                                                                            ;
;       Interrupt Handler/Reset Completes:                                   ;
;                                                                            ;
;       10. If the adapter was originally open, issue an OpenAdapter         ;
;           command.  If not, skip to 10.                                    ;
;                                                                            ;
;       Interrupt Handler/OpenAdapter Completes:                             ;
;                                                                            ;
;       11. Restore the original state.                                      ;
;                                                                            ;
;       12. Reset the receiving flag.                                        ;
;                                                                            ;
;       13. Do whatever other cleanup is necessary.                          ;
;                                                                            ;
;       14. Generate the EndReset indication.                                ;
;                                                                            ;
;       15. Generate the IndicationComplete.                                 ;
;                                                                            ;
;       There are a couple of problems with the Draft 2.0.1 NDIS as far as   ;
;       ResetMAC is concerned.  It does not specify whether the StartReset   ;
;       and EndReset indications can occur with indications disabled (we     ;
;       assume they can't).  It has conflicting advice on what error code    ;
;       to use when blowing off queued requests (we use INVALID_FUNCTION).   ;
;       It doesn't discuss what happens if the reset fails (we pass up an    ;
;       error code on EndReset, and the adapter is closed at that point,     ;
;       meaning a subsequence ResetMAC would not open it).  It does not      ;
;       specify what happens if both StartReset and EndReset disable         ;
;       indications (we set offDepth to 2, and two calls to IndicationOn     ;
;       will be required).  There is no discussion of whether ResetMAC must  ;
;       be queued behind any other requests or not (we don't queue it).      ;
;       All of these are expected to be addressed when the next version of   ;
;       the 2.0 spec comes out.                                              ;
;                                                                            ;
;----------------------------------------------------------------------------;

RqReset         proc    near
                cli
;
; state must be closed, openned or deceased.  save the current state in
; oldstate.  change state to DECEASED to prevent new requests from being
; submitted.  we'll change state to PRERESET in a minute, but the FlushQueues
; routine will enable interrupts and we don't want to generate the StartReset
; indication until the queues are empty.
;
                mov     al,state

                cmp     al,DECEASED
                je      @F
                cmp     al,CLOSED
                je      @F
                cmp     al,OPENNED
                je      @F
                mov     ax,INVALID_FUNCTION
                ret
@@:             mov     oldstate,al
                mov     state,DECEASED
;
; set receiving to NO to prevent any packets from being received.  we'll reset
; it later from status.ms_filter.
;
                mov     receiving,NO
;
; blow off all requests currently on queues with errors.
;
                call    FlushQueues
;
; now that the queues are all empty, change state to PRERESET.  if we get an
; interrupt now, we'll start the reset from there.  set the reasonMask to 0
; so we can check for PRERESET out of non-critical logic.
;
                mov     state,PRERESET
                mov     reasonMask,0
;
; enable interrupts through the adapter in case they are disabled.
;
                mov     es,segMMIO
                mov     byte ptr es:ISRPH+ACASET,ISRPHENABLE+ISRPHNMI
;
; turn the timer on to get an interrupt
;
                mov     byte ptr es:TVRL,1
                mov     byte ptr es:TCRH,TCRHENABLE+TCRHRUN+TCRHRELOAD
;
; return success.
;
                xor     ax, ax
                ret
RqReset         endp

NEWPAGE <RqSetAddress - Request SetStationAddress Subfunction>
;------ RqSetAddress --------------------------------------------------------;
;                                                                            ;
;       int RqSetAddress()                                                   ;
;                                                                            ;
;       SI = queue entry for this request                                    ;
;                                                                            ;
;       where the queue entry fields are as follows:                         ;
;                                                                            ;
;           bufp        = address of buffer containing new station address   ;
;           word        = not used                                           ;
;                                                                            ;
;       returns:                                                             ;
;                                                                            ;
;       AX = return code                                                     ;
;                                                                            ;
;       Registers AX,BX,CX,DX,ES may be destroyed by this function.          ;
;                                                                            ;
;       This function handles Request SetStationAddress.  For an 802.5       ;
;       style adapter which requires an OpenAdapter call to function, this   ;
;       request is legal only if the adapter is in a closed state and the    ;
;       actual setting of the address will not take place until the          ;
;       OpenAdapter is done.  This function is performed synchronously.      ;
;                                                                            ;
;----------------------------------------------------------------------------;

RqSetAddress    proc    near
;
; ensure that the adapter is closed
;
                cmp     state,CLOSED
                jne     rqadr_invalid
;
; copy the address to the mac specific characteristics table for later use
;
                les     bx,[si].qr_bufp
                mov     ax,word ptr es:[bx][0]
                mov     word ptr specific.mc_caddr[0],ax
                mov     ax,word ptr es:[bx][2]
                mov     word ptr specific.mc_caddr[2],ax
                mov     ax,word ptr es:[bx][4]
                mov     word ptr specific.mc_caddr[4],ax

                xor     ax,ax
                ret
;
; invalid state
;
rqadr_invalid:
                mov     ax,INVALID_FUNCTION
                ret
RqSetAddress    endp

NEWPAGE <RqSetFilter - Request SetPacketFilter Subfunction>
;------ RqSetFilter ---------------------------------------------------------;
;                                                                            ;
;       int RqSetFilter()                                                    ;
;                                                                            ;
;       SI = queue entry for this request                                    ;
;                                                                            ;
;       where the queue entry fields are as follows:                         ;
;                                                                            ;
;           bufp        = not used                                           ;
;           word        = new packet filter                                  ;
;                                                                            ;
;       returns:                                                             ;
;                                                                            ;
;       AX = return code                                                     ;
;                                                                            ;
;       Registers AX,BX,CX,DX,ES may be destroyed by this function.          ;
;                                                                            ;
;       This function handles Request SetPacketFilter.  The IBM Token Ring   ;
;       adapters have no packet filter per se, so the only thing this        ;
;       function really does is to enable or disable the generation of       ;
;       receive indications.  If a zero packet filter is specified, any      ;
;       packets received will be discarded.  A non-zero packet filter        ;
;       allows normal reception once again.                                  ;
;                                                                            ;
;       When the driver is initially started, the packet filter defaults to  ;
;       zero.  ReceiveLookahead indications may occur once both OpenAdapter  ;
;       and SetPacketFilter (non-zero filter) have been done.  The           ;
;       SetPacketFilter can be done before or after OpenAdapter.             ;
;                                                                            ;
;       This function is performed synchronously.                            ;
;                                                                            ;
;----------------------------------------------------------------------------;

RqSetFilter     proc    near
;
; ensure that the adapter is openned or closed
;
                cmp     state,OPENNED
                je      rqflt_able
                cmp     state,CLOSED
                jne     rqflt_invalid
;
; check if any bits were specified which are not supported by this driver
;
rqflt_able:
                mov     ax,[si].qr_word
                test    ax,FM_PROMISCUOUS+FM_SRC_ROUTING
                jnz     rqflt_fail
;
; save the new packet filter in the mac specific status table
;
                mov     status.ms_filter,ax
;
; set receiving to YES or NO based on whether the packet filter is non-zero
;
                mov     dl,NO
                test    ax,ax
                jz      rqflt_setrcv
                mov     dl,YES
rqflt_setrcv:
                mov     receiving,dl
;
; done.  return SUCCESS.
;
                xor     ax,ax
                ret
;
; invalid state
;
rqflt_invalid:
                mov     ax,INVALID_FUNCTION
                ret
;
; general failure
;
rqflt_fail:
                mov     ax,GENERAL_FAILURE
                ret
RqSetFilter     endp

NEWPAGE <RqClearStats - Request ClearStatistics Subfunction>
;------ RqClearStats --------------------------------------------------------;
;                                                                            ;
;       int RqClearStats()                                                   ;
;                                                                            ;
;       SI = queue entry for this request                                    ;
;                                                                            ;
;       returns:                                                             ;
;                                                                            ;
;       AX = return code                                                     ;
;                                                                            ;
;       Registers AX,BX,CX,DX,ES may be destroyed by this function.          ;
;                                                                            ;
;       This function handles Request ClearStatistics.  It updates the       ;
;       date/time statistics were last cleared and clears out the stats      ;
;       which are being maintained.                                          ;
;                                                                            ;
;       This function is performed synchronously.                            ;
;                                                                            ;
;----------------------------------------------------------------------------;

IFDEF ANYSTATS
RqClearStats    proc    near
                pushr   si,di
;
; under OS/2 we can get the current date/time in the format we want from the
; global infoSeg.
;
IFDEF OS2
                mov     es,infoSeg
                mov     ax,es:[is_elapsed].loword
                mov     dx,es:[is_elapsed].hiword
                mov     status.ms_cdate.loword,ax
                mov     status.ms_cdate.hiword,dx
ENDIF
;
; under DOS we've been accumulating timer ticks since the last time clear
; was issued, so we can adjust the time after converting to seconds
;
; the timer tick handler is invoked 18.20651 times per second.  we use this
; as 36413/2000 = 18.2065.  probably good enough and it keeps the arithmetic
; halfway reasonable.
;
IFDEF DOS
                xor     ax,ax
                xchg    ax,timerCount.loword
                mov     cx,2000
                mul     cx
                mov     si,ax
                mov     di,dx
                xor     ax,ax
                xchg    ax,timerCount.hiword
                mul     cx
                add     ax,di
                adc     dx,0
                mov     cx,36413
                div     cx
                mov     di,ax
                mov     ax,si
                div     cx                      ; DI:AX is seconds since last clear
                add     status.ms_cdate.loword,ax
                adc     status.ms_cdate.hiword,di
ENDIF
;
; set AX to zero, both for the zeroing and to return success
;
                xor     ax,ax
;
; and clear the various statistics we maintain, without disturbing the
; others...
;
                mov     status.ms_total_recv.loword,ax
                mov     status.ms_total_recv.hiword,ax
                mov     status.ms_total_rbytes.loword,ax
                mov     status.ms_total_rbytes.hiword,ax
                mov     status.ms_total_sent.loword,ax
                mov     status.ms_total_sent.hiword,ax
                mov     status.ms_total_sbytes.loword,ax
                mov     status.ms_total_sbytes.hiword,ax
                mov     status.ms_hard_serr.loword,ax
                mov     status.ms_hard_serr.hiword,ax
IFDEF ALLSTATS
                mov     status.ms_mult_recv.loword,ax
                mov     status.ms_mult_recv.hiword,ax
                mov     status.ms_broad_recv.loword,ax
                mov     status.ms_broad_recv.hiword,ax
                mov     status.ms_mult_rbytes.loword,ax
                mov     status.ms_mult_rbytes.hiword,ax
                mov     status.ms_broad_rbytes.loword,ax
                mov     status.ms_broad_rbytes.hiword,ax
                mov     status.ms_mult_sent.loword,ax
                mov     status.ms_mult_sent.hiword,ax
                mov     status.ms_broad_sent.loword,ax
                mov     status.ms_broad_sent.hiword,ax
                mov     status.ms_mult_sbytes.loword,ax
                mov     status.ms_mult_sbytes.hiword,ax
                mov     status.ms_broad_sbytes.loword,ax
                mov     status.ms_broad_sbytes.hiword,ax
ENDIF

                popr    si,di
                ret
RqClearStats    endp
ENDIF

NEWPAGE <RqSetFunctional - Request SetFunctionalAddress Subfunction>
;------ RqSetFunctional -----------------------------------------------------;
;                                                                            ;
;       int RqSetFunctional()                                                ;
;                                                                            ;
;       SI = queue entry for this request                                    ;
;                                                                            ;
;       where the queue entry fields are as follows:                         ;
;                                                                            ;
;           bufp        = address of buffer containing functional address    ;
;           word        = not used                                           ;
;                                                                            ;
;       returns:                                                             ;
;                                                                            ;
;       AX = return code                                                     ;
;                                                                            ;
;       Registers AX,BX,CX,DX,ES may be destroyed by this function.          ;
;                                                                            ;
;       This function handles Request SetFunctionalAddress.  This function   ;
;       is performed asynchronously except for errors.                       ;
;                                                                            ;
;----------------------------------------------------------------------------;

RqSetFunctional proc    near
;
; ensure that the adapter is openned
;
                cmp     state,OPENNED
                jne     rqfnc_invalid
;
; ensure that the first bit isn't set
;
                les     bx,[si].qr_bufp
                test    byte ptr es:[bx][0],80h
                jnz     rqfnc_bad
;
; copy the address to the mac specific characteristics table (save in AX|DX)
;
                mov     ax,word ptr es:[bx][0]
                mov     dx,word ptr es:[bx][2]
                mov     word ptr specific.mc_faddr[0],ax
                mov     word ptr specific.mc_faddr[2],dx
;
; set up an SRB for DIR.SET.FUNCT.ADDRESS (from AX|DX)
;
                mov     es,segSM
                mov     bx,oSRB
                mov     es:[bx].srb_command,DIR_SETFUNCTIONAL
                mov     word ptr es:[bx].funct_address[0],ax
                mov     word ptr es:[bx].funct_address[2],dx
;
; hit the command ready bit
;
                mov     es,segMMIO
                mov     byte ptr es:ISRAL+ACASET,ISRALSRBRDY+ISRALFREESRB
;
; done.  return.
;
                mov     ax,REQUEST_QUEUED
                ret
;
; invalid state
;
rqfnc_invalid:
                mov     ax,INVALID_FUNCTION
                ret
;
; invalid functional address
;
rqfnc_bad:
                mov     ax,INVALID_PARAMETER
                ret
RqSetFunctional endp

NEWPAGE <RqAddMulticast - Request AddMulticastAddress Subfunction>
;------ RqAddMulticast ------------------------------------------------------;
;                                                                            ;
;       int RqAddMulticast()                                                 ;
;                                                                            ;
;       SI = queue entry for this request                                    ;
;                                                                            ;
;       where the queue entry fields are as follows:                         ;
;                                                                            ;
;               bufp = address of buffer containing group address            ;
;               word = not used                                              ;
;                                                                            ;
;       returns:                                                             ;
;                                                                            ;
;       AX = return code                                                     ;
;                                                                            ;
;       Registers AX,BX,CX,DX,ES may be destroyed by this function.          ;
;                                                                            ;
;       This function handles Request AddMulticastAddress.  The only         ;
;       'multicast' address which is supported by the IBM Token Ring         ;
;       adapters is the single group address, so things are somewhat         ;
;       simpler than they might seem.  If a group address has already been   ;
;       set, then the request is refused.  It will have to be deleted        ;
;       before a new group address can be set.  The group address set on     ;
;       the adapter is only the last 4 bytes of the 6 byte address.  The     ;
;       first two bytes are assumed to be 80 00 hex.                         ;
;                                                                            ;
;       This function is performed asynchronously except for errors.         ;
;                                                                            ;
;----------------------------------------------------------------------------;

RqAddMulticast  proc    near
;
; ensure that the adapter is openned
;
                cmp     state,OPENNED
                jne     rqadd_invalid
;
; check that there isn't already a group address
;
                cmp     multilist.ma_cur,0
                jne     rqadd_invalid
;
; ensure that the first two bytes of the address are correct
;
                les     bx,[si].qr_bufp
                mov     ax,word ptr es:[bx][0]
                cmp     ax,0080h
                jne     rqadd_bad
;
; save the group address in the multicast list table (save inn AX|DX)
;
                mov     multilist.ma_cur,1
                mov     word ptr multilist.ma_mbuff[0],ax
                mov     ax,word ptr es:[bx][2]
                mov     dx,word ptr es:[bx][4]
                mov     word ptr multilist.ma_mbuff[2],ax
                mov     word ptr multilist.ma_mbuff[4],dx
;
; set up an SRB for DIR.SET.GROUP.ADDRESS (from AX|DX)
;
                mov     es,segSM
                mov     bx,oSRB
                mov     es:[bx].srb_command,DIR_SETGROUP
                mov     word ptr es:[bx].group_address[0],ax
                mov     word ptr es:[bx].group_address[2],dx
;
; hit the command ready bit
;
                mov     es,segMMIO
                mov     byte ptr es:ISRAL+ACASET,ISRALSRBRDY+ISRALFREESRB
;
; done.  return.
;
                mov     ax,REQUEST_QUEUED
                ret
;
; invalid state
;
rqadd_invalid:
                mov     ax,INVALID_FUNCTION
                ret
;
; bad parameter
;
rqadd_bad:
                mov     ax,INVALID_PARAMETER
                ret
RqAddMulticast  endp

NEWPAGE <RqDelMulticast - Request DeleteMulticastAddress Subfunction>
;------ RqDelMulticast ------------------------------------------------------;
;                                                                            ;
;       int RqDelMulticast()                                                 ;
;                                                                            ;
;       SI = queue entry for this request                                    ;
;                                                                            ;
;       where the queue entry fields are as follows:                         ;
;                                                                            ;
;               bufp = address of buffer containing group address            ;
;               word = not used                                              ;
;                                                                            ;
;       returns:                                                             ;
;                                                                            ;
;       AX = return code                                                     ;
;                                                                            ;
;       Registers AX,BX,CX,DX,ES may be destroyed by this function.          ;
;                                                                            ;
;       This function handles Request DeleteMulticastAddress.  The only      ;
;       'multicast' address which is supported by the IBM Token Ring         ;
;       adapters is the single group address, so things are somewhat         ;
;       simpler than they might seem.  The address must match the one in     ;
;       the multicast list table or it will be refused.  A zero group        ;
;       address will then be set.                                            ;
;                                                                            ;
;       This function is performed asynchronously except for errors.         ;
;                                                                            ;
;----------------------------------------------------------------------------;

RqDelMulticast  proc    near
;
; ensure that the adapter is openned
;
                cmp     state,OPENNED
                jne     rqdel_invalid
;
; check that there is a group address in the table
;
                cmp     multilist.ma_cur,1
                jne     rqdel_invalid
;
; ensure that the group address specified matches the one in the table
;
                les     bx,[si].qr_bufp
                cmp     word ptr es:[bx][0],0080h
                jne     rqdel_bad
                mov     ax,word ptr es:[bx][2]
                cmp     ax,word ptr multilist.ma_mbuff[2]
                jne     rqdel_bad
                mov     ax,word ptr es:[bx][4]
                cmp     ax,word ptr multilist.ma_mbuff[4]
                jne     rqdel_bad
;
; delete the group address in the multicast list table
;
                mov     multilist.ma_cur,0
;
; set up an SRB for DIR.SET.GROUP.ADDRESS to set the group address to all zeroes
;
                mov     es,segSM
                mov     bx,oSRB
                mov     es:[bx].srb_command,DIR_SETGROUP
                mov     word ptr es:[bx].group_address[0],0
                mov     word ptr es:[bx].group_address[2],0
;
; hit the command ready bit
;
                mov     es,segMMIO
                mov     byte ptr es:ISRAL+ACASET,ISRALSRBRDY+ISRALFREESRB
;
; done.  return.
;
                mov     ax,REQUEST_QUEUED
                ret
;
; invalid state
;
rqdel_invalid:
                mov     ax,INVALID_FUNCTION
                ret
;
; bad parameter
;
rqdel_bad:
                mov     ax,INVALID_PARAMETER
                ret
RqDelMulticast  endp

NEWPAGE <RqReadLog - Request ReadErrorLog Subfunction>
;------ RqReadLog -----------------------------------------------------------;
;                                                                            ;
;       int RqReadLog()                                                      ;
;                                                                            ;
;       SI = queue entry for this request                                    ;
;                                                                            ;
;       where the queue entry fields are as follows:                         ;
;                                                                            ;
;           bufp        = address of buffer to hold error log                ;
;           word        = length of buffer                                   ;
;                                                                            ;
;       returns:                                                             ;
;                                                                            ;
;       AX = return code                                                     ;
;                                                                            ;
;       Registers AX,BX,CX,DX,ES may be destroyed by this function.          ;
;                                                                            ;
;       This function handles Request ReadErrorLog.  This function is        ;
;       performed asynchronously except for errors.                          ;
;                                                                            ;
;----------------------------------------------------------------------------;

RqReadLog       proc    near
;
; ensure that the adapter is openned
;
                cmp     state,OPENNED
                jne     rqlog_invalid
;
; ensure that there is sufficient space for the error log
;
                cmp     [si].qr_word,LOGSIZE
                jb      rqlog_fail
;
; set up an SRB for DIR.READ.LOG
;
                mov     es,segSM
                mov     bx,oSRB
                mov     es:[bx].srb_command,DIR_READLOG
;
; hit the command ready bit
;
                mov     es,segMMIO
                mov     byte ptr es:ISRAL+ACASET,ISRALSRBRDY+ISRALFREESRB
;
; done.  return.
;
                mov     ax,REQUEST_QUEUED
                ret
;
; invalid state
;
rqlog_invalid:
                mov     ax,INVALID_FUNCTION
                ret
;
; general failure
;
rqlog_fail:
                mov     ax,GENERAL_FAILURE
                ret
RqReadLog       endp

NEWPAGE <RqOpenAdapter - Request OpenAdapter Subfunction>
;------ RqOpenAdapter -------------------------------------------------------;
;                                                                            ;
;       int RqOpenAdapter()                                                  ;
;                                                                            ;
;       SI = queue entry for this request                                    ;
;                                                                            ;
;       where the queue entry fields are as follows:                         ;
;                                                                            ;
;           bufp        = not used                                           ;
;           word        = open options                                       ;
;                                                                            ;
;       returns:                                                             ;
;                                                                            ;
;       AX = return code                                                     ;
;                                                                            ;
;       Registers AX,BX,CX,DX,ES may be destroyed by this function.          ;
;                                                                            ;
;       This function handles Request OpenAdapter.  It causes the adapter    ;
;       to be physically inserted into the ring, and allows other commands   ;
;       to the adapter to be issued.                                         ;
;                                                                            ;
;       This function must be called after Bind, but before most other       ;
;       functions (only Request Interrupt, SetStationAddress and             ;
;       IndicationOn/Off are legal before the adapter is open).  A           ;
;       CloseAdapter will return the adapter to the same state as before     ;
;       the open.  SetPacketFilter is still required to enable incoming      ;
;       packets to generate indications to the protocol.                     ;
;                                                                            ;
;----------------------------------------------------------------------------;

RqOpenAdapter   proc    near
                PUSHR   si,di
;
; ensure that the adapter is closed (change state to open when the open
; adapter command completes)
;
                cmp     state,CLOSED
                jne     rqopn_invalid
;
; save the open options supplied on the call
;
                mov     ax,[si].qr_word
                mov     openSoft,ax
;
; open the adapter
;
                call    OpenAdapter
;
; done.  return.
;
                mov     ax,REQUEST_QUEUED
rqopn_exit:
                POPR    si,di
                ret
;
; invalid state
;
rqopn_invalid:
                mov     ax,INVALID_FUNCTION
                jmp     rqopn_exit
RqOpenAdapter   endp

NEWPAGE <RqCloseAdapter - Request CloseAdapter Subfunction>
;------ RqCloseAdapter ------------------------------------------------------;
;                                                                            ;
;       int RqCloseAdapter()                                                 ;
;                                                                            ;
;       SI = queue entry for this request                                    ;
;                                                                            ;
;       where the queue entry fields are as follows:                         ;
;                                                                            ;
;           bufp        = not used                                           ;
;           word        = not used                                           ;
;                                                                            ;
;       returns:                                                             ;
;                                                                            ;
;       AX = return code                                                     ;
;                                                                            ;
;       Registers AX,BX,CX,DX,ES may be destroyed by this function.          ;
;                                                                            ;
;       This function handles Request CloseAdapter.  This function is        ;
;       performed asynchronously except for errors.                          ;
;                                                                            ;
;       Note that CloseAdapter zeroes out the functional and group           ;
;       addresses just like TOKREUI.                                         ;
;                                                                            ;
;----------------------------------------------------------------------------;

RqCloseAdapter  proc    near
;
; ensure that the adapter is openned then change state to closed and clear the
; openned bit in the status flags
;
                cmp     state,OPENNED
                jne     rqcls_invalid
                mov     state,CLOSED
                and     status.ms_flags.loword,NOT ST_MAC_IS_OPEN
;
; zero out the functional and group addresses
;
                mov     word ptr specific.mc_faddr[0],0
                mov     word ptr specific.mc_faddr[2],0
                mov     multilist.ma_cur,0
;
; set the number of transmits needed to 0 (we'll get access errors from the
; adapter if these are processed after the adapter closes)
;
                mov     txNeeded,0
;
; set up an SRB for DIR.CLOSE.ADAPTER
;
                mov     es,segSM
                mov     bx,oSRB
                mov     es:[bx].srb_command,DIR_CLOSEADAPTER
;
; hit the command ready bit
;
                mov     es,segMMIO
                mov     byte ptr es:ISRAL+ACASET,ISRALSRBRDY+ISRALFREESRB
;
; done.  return.
;
                mov     ax,REQUEST_QUEUED
                ret
;
; invalid state
;
rqcls_invalid:
                mov     ax,INVALID_FUNCTION
                ret
RqCloseAdapter  endp

                END_CODE
                end

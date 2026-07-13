		PAGE  60, 132
		TITLE Token-Ring RIPL Boot ROM Driver - INT.ASM
;**********************************************************************;
;**********************************************************************;
;								       ;
; File Name:       INT.ASM            				       ;
;								       ;
; Program Name:    Token-Ring RIPL Boot ROM Device Driver              ;
;								       ;
; Companion Files: None						       ;
;								       ;
; Function:        This module contains the routines that handle       ;
;                  interrupts from the TMS380 based adapter.  This     ;
;                  module contains the immediate routines for handling ;
;                  interrupts, but there are several extended          ;
;                  routines, especially for receives, which are found  ;  
;                  in their specific module.                           ;
;								       ;
; History:             						       ;
;								       ;
;     03/20/89  Mike Witkowski - Original Version 0.0		       ;
;     02/14/90  Mike Witkowski - Reworked for RIPL                     ;
;								       ;
;**********************************************************************;
;**********************************************************************;

;-------------------------------------;
; Include all general companion files ;
;-------------------------------------;
 					   
		.XLIST
	       	INCLUDE includes.inc  	
		.LIST

PAGE_BREAK <External References>
;-----------------------------------;
; External Data/Variable References ;
;-----------------------------------;
		EXTRN	gbl_int_table:		WORD
		EXTRN	gbl_dynmem:		WORD

		EXTRN	gbl_stack_nest:		WORD
                EXTRN   gbl_dvrstk:             WORD
                EXTRN   gbl_dosstk_ss:          WORD
                EXTRN   gbl_dosstk_sp:          WORD
                EXTRN   MSG_statuserr:          BYTE
                EXTRN   MSG_beacon:             BYTE

;--------------------------;
; External Code References ;
;--------------------------;
		EXTRN	Rcv_Status:		NEAR
		EXTRN	IssueEOI:		NEAR
		EXTRN	DLCPostEvent:		NEAR
		EXTRN	DLCStatus:		NEAR

START_CODE

PAGE_BREAK <Adapter Interrupt Handler Routine Definition>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;  Routine Name:  Adapter Interrupt Handler Routine       	       ;
;                   					               ;
;  Description: The Adapter Interrupt Handler Routine is what is called;
;               when an interrupt is asserted from the attached TMS380 ;
;               based adapter.  This routine determines the interrupt  ;
;               reason code and acts accordingly.                      ;
;                                                                      ;
;  Input:       DS - our data segment                                  ;
;                                                                      ;
;  Output:      carry cleared if interrupt was ours                    ;
;								       ;
;  Calls:                                                              ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

                PUBLIC  interrupt
interrupt   	PROC	FAR
		cld
		cli

		PUSHREG ax, bx cx, dx, di, si, bp, ds, es

		mov	ds, cs:gbl_dynmem	; Point to dynamic memory
		xor	bx, bx                  ; Set pointer to ACB


;
;  Read the SIFINT register to determine if the interrupt was 
;  caused by the adapter.  
; 
		mov	dx, [bx].acb_sif_int	; Get SIFINT register
		in	ax, dx			;   contents
		mov	si, ax			; Save interrupt reason
                and     al, SIFINT_SYSINT       ; Isolate system int bit

                and     [bx].acb_flags, NOT INT_VALID 
                                                ; Set interrupt not valid        
                or      [bx].acb_flags, al      ; Set if int is valid
		test	si, SIFINT_SYSINT	; Is this valid int ?
                LJZ     int_no_process          ;   No, skip int

		xor	ax, ax			
		out	dx, ax			; Deactivate SINTR line

                call    IssueEOI                ; EOI the int controller


;
;  Now, look through the transmit lists and act on all those that 
;  have completed.  We look for all the lists that have completed
;  and call the DLCPostEvent routine for each complete Transmit request.
;  We will also clear the EOF list's CSTAT fields while we're at it. 
;
;  This is done here because we do not implement the frame complete bit
;  in the transmit list CSTATs.  This scanning code is a lot cheaper 
;  than having the additional interrupt overhead. 
;
int_xmits:
		inc	[bx].acb_intnest	; Inc interrupt level

		mov	di, [bx].acb_xmit_ahead		; Get oldest xmit
		or	di, di				; Are any active ?
		SJZ	int_ind_chk     	        ;  No, skip this
		
		test	[di].XMIT_CSTAT, XCSTAT_COMPLETE   ; Is frame cmplt ?	
		SJZ	int_ind_chk			   ;   No, skip this

		mov	[bx].acb_xmit_ahead, 0		; Clear active Q head

int_xmit_scan:
                mov     cx, di                          ; Copy SOF pointer
		mov	di, [di].XMIT_EOF		; Get EOF list/count
		and	di, 0fffch			; AND out list count
		mov	[di].XMIT_CSTAT, 0		; Clear its CSTAT

                PUSHREG si, di, bx              ; Save needed registers

                mov     di, cx                  ; Point to SOF list
		mov	[di].XMIT_CSTAT, 0	; Clear its CSTAT

		mov	ax, [di].XMIT_EOF	; Find EOF list
		and	ax, 0003h		; Isolate list count
		add	[bx].acb_xlist_cnt, ax	; Free the list count

		mov	si, [di].XMIT_ImmPtr	; Get the imm buf ptr
		or	si, si			; Was there one ?
		SJZ	@F      	    	;   No, skip this

		mov	cx, si		  	; Save imm buf address
		xchg	cx, [bx].acb_ibuf_free	; Place it onto imm
		mov	[si].IBUF_Ptr,	cx	;   buffer free queue

@@:
                les     si, [di].XMIT_ED        ; Point to ED

                mov     es:[si].ED_ComCode, SUCCESSFUL
                                                ; Set good return code
                push    ds                      ; Save dynamic data segment
                push    cs                      ; Set DS = CS
                pop     ds                      ;
                call    DLCPostEvent            ; Inform RIPL ROM DLC
                pop     ds                      ; Restore data segment

                POPREG  si, di, bx              ; Restore needed registers

		cmp	di, [bx].acb_xmit_atail		; This Active Q tail ?
		SJE	int_ind_chk			;   Yes, we're done
		mov	di, [di].XMIT_Next      	 ; Get next SOF list
		test	[di].XMIT_CSTAT, XCSTAT_COMPLETE ; Is new SOF cmplt ? 
		SJNZ	int_xmit_scan			 ;   Yes, scan it

		mov	[bx].acb_xmit_ahead, di		; Set new active head


int_ind_chk:
		inc	cs:gbl_stack_nest	; Increment stack nesting
		cmp	cs:gbl_stack_nest, 1	;   counter, if not first
		SJNE	int_nostkrplc		;   to use, don't replace it

		mov	cs:gbl_dosstk_ss, ss	 	; Swap stack to 
		mov	cs:gbl_dosstk_sp, sp		;   our own. 
                mov     sp, OFFSET CGROUP:gbl_dvrstk	;
		mov	ax, cs				;
		mov	ss, ax				;

int_nostkrplc:
                test    [bx].acb_flags, INT_VALID       ; Was int valid ?
                SJZ     int_exit                        ;   No, don't process

                call    Rcv_Status              ; Clean up receives


;
;  Determine the reason for the interrupt and call the handler routine
;  responsible for handling it. 
;  
		and	si, 000eh       	        ; Isolate reason code
		jmp	WORD PTR cs:gbl_int_table[si]   ; Jump to handler


		PAGE
;----------------------------------------------------------------------;
;  Now that the interrupt has been processed and an indication has     ;
;  possibly been made, it is time to clean up the adapter and call     ;
;  an indication complete or a confirmation routine in the upper       ;
;  protocol driver.  The first thing to perform, however, is see if an ;
;  indication needs to be generated from an outstanding interrupt re-  ;
;  quest.                                                              ;
;----------------------------------------------------------------------;		
MLABEL <int_exit>, %MNEAR, %MPUBLIC

;
;  If we're in DOS, then switch the stack back if there is no one 
;  else using it (stack nesting = 0). 
;
		dec	cs:gbl_stack_nest       ; Dec stack nesting level	
		SJNZ	int_nostkswtch		;   Jump if still in use

		mov	sp, cs:gbl_dosstk_sp    ; Restore the DOS stack
		mov	ax, cs:gbl_dosstk_ss	;	
 		mov	ss, ax			;

int_nostkswtch:

;
;  Issue an SSB Clear to the adapter.
;
int_complete:
                test    [bx].acb_flags, INT_VALID       ; Was int valid ?
                SJZ     int_leave                       ;   No, skip SSB clear
        
		mov	dx, [bx].acb_sif_int   	; Get SIFINT register
		mov	ax, SIFINT_SSBCLEAR	; Set SSB Clear request
		out	dx, ax			; Issue it to adapter


int_leave:
		dec	[bx].acb_intnest	; Dec interrupt nest level

int_leavenoinc:
		POPREG  ax, bx, cx, dx, di, si, bp, ds, es

		iret                            ; Return from interrupt


;
;  A spurrious interrupt was encountered.  Issue EOI and return.
;
int_no_process:
                call    IssueEOI                ; Issue EOI
                SJMP    int_leavenoinc          ; Exit interrupt

                
PAGE_BREAK <Command Completion Interrupt Handler Definition>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;  Routine Name:  Command Completion Interrupt Handler       	       ;
;                   					               ;
;  Description: The Command Completion Interrupt Handler is called by  ;
;               the adapter interrupt routine to handle command com-   ;
;               pletions.  This routine examines the SSB for the com-  ;
;               mand completion status and sets the GRB status field   ;
;               accordingly.                                           ;
;                                                                      ;
;  Input:       BX - pointer to MCB                                    ;
;                                                                      ;
;  Output:                                                             ;
;								       ;
;  Calls:                                                              ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

MLABEL <CommandHandler>, %MWORD, %MPUBLIC

;
;  First check the SSB command field for a Read Adapter Command.  If it
;  is, then skip everything because there is no GRB associated with it.
;
		cmp	[bx].acb_ssb.SSB_Cmd, TMS_READADP  ; Is it a read ?
		LJE	int_exit			   ;   Yes, exit int


;
;  Look at the GRB and update the status field.
;
                mov     di, dyn_GRB             ; Point to GRB

;
;  Now check the SSB for the command completion status.	 First check
;  if a Command Reject occurred.  If it did, this is really bad, since
;  this means the driver is at fault.  If there was no Command Reject,
;  we merely check the status field to see if the command completed 
;  successfully or not.	 We also return back to the main interrupt
;  handling return here.
;
		cmp	[bx].acb_ssb.SSB_Cmd, TMS_CMDREJECT  ; If cmd ok, 
		SJNE	int_cmd_ok			     ;   continue

		mov	[di].GRB_SCBPHi, GENERAL_FAILURE     ; If rejected,
		LJMP	int_exit			     ;   big problem


int_cmd_ok:
	   	cmp	[bx].acb_ssb.SSB_Status, SSB_GOOD    ; If status bad, 
		SJNE	int_cmd_bad			     ;   set error

		mov	[di].GRB_SCBPHi, SUCCESSFUL	     ; Set good code


;
;  Now check the command to see if it was an open command.  If it was, 
;  then issue a receive command to the adapter to enable receives. 
;  This is done by using the foward pointer of the first list to 
;  tell the adapter to start receiving on the second list. Therefore,
;  we must make sure to dequeue the first list so that the second list
;  is the first we process. (This was done so that a special MyMoto
;  field didn't have to be added to every list.)
;
		cmp	[bx].acb_ssb.SSB_Cmd, TMS_OPEN	; Was it open cmd ?
		LJNE	int_exit  			;   No, return

		mov	[bx].acb_state, AS_OPENED	; Set opened state

                push    di                              ; Save GRB pointer

		mov	di, [bx].acb_rcv_head		; Get first rcv list
		mov	ax, [di].RCV_Next  		; Get next rcv list
		mov	[bx].acb_rcv_head, ax		; Make it first now

		mov	[bx].acb_scb.SCB_Cmd, TMS_RECEIVE
							; Set receive command

		mov	ax, [di].RCV_FwdPtr.Mhi		; Load first receive 
		mov	[bx].acb_scb.SCB_Ptr.Mhi, ax	;   list's forward
		mov	ax, [di].RCV_FwdPtr.Mlo		;   pointer into SCB.
		mov	[bx].acb_scb.SCB_Ptr.Mlo, ax	;
				
		mov	dx, [bx].acb_sif_int		; Get SIFINT port @

		mov	ax, SIFINT_CMD OR SIFINT_SCBREQST  	
							; Set cmd & request

		out	dx, ax				; Inform adapter
		
		mov	ax, [bx].acb_xmit_head		; Get next xmit list
		mov	[bx].acb_xmit_whead, ax		; Set it up to xmit
		mov	[bx].acb_xmit_wtail, ax		; Set it up to xmit
                pop     di                              ; Restore GRB pointer
		LJMP	int_exit			; Return from int


int_cmd_bad:
		cmp	[bx].acb_ssb.SSB_Cmd, TMS_OPEN	; Was it open cmd ?
		SJNE	@F       			;   No, return error

		mov	[di].GRB_SCBPHi, GENERAL_FAILURE        ; Set error

		mov	[bx].acb_state, AS_INITIALIZED	; Set init state
		LJMP	int_exit			; Return from int

@@:					
		mov	[di].GRB_SCBPHi, INVALID_PARAMETER   ; Set error code
		LJMP	int_exit			     ; Return			


PAGE_BREAK <Ring Status Interrupt Handler Definition>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;  Routine Name:  Ring Status Interrupt Handler         	       ;
;                   					               ;
;  Description: The Ring Status Interrupt Handler is called by the     ;
;               adapter interrupt routine to handle ring status        ;
;               interrupts.  This routine examines the SSB for the     ;
;               ring status, and sets the adapter state based on the   ;
;               status received.                                       ;
;                                                                      ;
;  Input:       BX - pointer to MCB                                    ;
;                                                                      ;
;  Output:                                                             ;
;								       ;
;  Calls:                                                              ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

MLABEL <StatusHandler>, %MWORD, %MPUBLIC
	
;
;  Depending on what the status is, the adapter may be in a closed 
;  state.  If one of these statuses is detected, then set the 
;
		mov	ax, [bx].acb_ssb.SSB_Status
						; Get the status
                mov     cx, ax                  ; Copy it
                
                and     ax, NOT 0e020h          ; Is status good ?
                SJZ     sh_callstat             ;   Yes, tell DLC 

		test	ax, 000dh		; Is adapter closed ?
                SJZ     sh_chkbeacon            ;   No check for beacon

		mov	[bx].acb_state, AS_INITIALIZED
						; Set initialized state

                mov     ax, OFFSET CGROUP:MSG_statuserr
                                                ; Point to status error
                SJMP    sh_callstat             ; Tell DLC


sh_chkbeacon:
                test    ax, 0050h               ; Is ring beaconing ?
		LJZ	int_exit      		;   No, status OK

                mov     ax, OFFSET CGROUP:MSG_beacon
                                                ; Point to beacon message
                xor     cx, cx                  ; Set non-fatal error

sh_callstat:
                push    ds                      ; Save dynamic data segment
                push    bx                      ; Save ACB pointer
                push    cs                      ; Set driver data segment
                pop     ds                      ;
                call    DLCStatus               ; Call status routine                     		
                pop     bx                      ; Restore ACB pointer
                pop     ds                      ; Restore dynamic segment
                LJMP    int_exit                ; Exit interrupt
        

PAGE_BREAK <Adapter Check Interrupt Handler Definition>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;  Routine Name:  Adapter Check Interrupt Handler                      ;
;                   					               ;
;  Description: The Adapter Check Interrupt Handler is called by the   ;
;               adapter interrupt routine to handle adapter check      ;
;               interrupts.                                            ;
;                                                                      ;
;  Input:       BX - pointer to MCB                                    ;
;                                                                      ;
;  Output:                                                             ;
;								       ;
;  Calls:                                                              ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
                                                
MLABEL <CheckHandler>, %MWORD, %MPUBLIC

		mov	[bx].acb_state, AS_HARDERROR
						; Set hard error state
		
		LJMP	int_exit		; Return			


PAGE_BREAK <SCB Clear Interrupt Handler Definition>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;  Routine Name:  SCB Clear Interrupt Handler         	               ;
;                   					               ;
;  Description: The SCB Clear Interrupt Handler is called by the       ;
;               adapter interrupt routine to handle SCB Clear          ;
;               interrupts.  An SCB Clear is generated when the TRNDD  ;
;               wants to know when the SCB is free for use.  Either a  ;
;               GRB or a transmit command is pending on the SCB, so    ;
;               this routine checks each of these waiting queues to    ;
;               see which gets written to the SCB next.  This routine  ;
;               gives priority to transmit requests.                   ;
;                                                                      ;
;               Since receive processing also depends on SCB inter-    ;
;               rupts to kick off stalled receives, we always exit     ;
;               through the receive interrupt handler to let it clear  ;
;               itself out.                                            ; 
;                                                                      ;
;  Input:       BX - pointer to MCB                                    ;
;                                                                      ;
;  Output:      always exits through the Receive Interrupt Handler     ;
;								       ;
;  Calls:                                                              ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

MLABEL <SCBHandler>, %MWORD, %MPUBLIC

;
;  First clear the SCB Clear Pending flag. 
;
                and     [bx].acb_flags, NOT INT_SCBPENDING
                                                ; Clear SCB CLEAR pending


;
;  Check to see if this is an interrupt test. If it is, then 
;  change the state of the MCB to initialized. 
;
                cmp     [bx].acb_state, AS_INTTEST      ; Is this int test ?
                SJNE    int_notest                      ;   No, continue

                mov     [bx].acb_state, AS_INITIALIZED  ; Set inited state
                LJMP    int_exit                        ; Exit from interrupt


;
;  Now test to see if the SCB is currently in use (false SCB clear 
;  interrupt).  If it is then generate another. 
;
int_notest:
		mov	dx, [bx].acb_sif_int	; Get SIFINT port address
		cmp	[bx].acb_scb.SCB_Cmd, 0	; Is SCB free ? 
		LJNZ	int_issue_req		;   No, request another


;
;  Now check the transmit waiting queue to see if there are any
;  transmit requests pending.  If there are, then dequeue the entire
;  waiting list chain and build a transmit command SCB. 
;
		xor	di, di		   		; Dequeue the xmit
		xchg	di, [bx].acb_xmit_whead		;   waiting chain
		or	di, di			        ; Was anything there?
		SJZ	int_check_genq		        ;   No, check gen req

		cmp	[di].XMIT_CSTAT, 0		; Is cstat = 0
		SJZ	int_no_xq			;   Don't requeue it

		mov	[bx].acb_xmit_ahead, di		; Put waiting queue
		mov	ax, [bx].acb_xmit_wtail		;   elements onto
		mov	[bx].acb_xmit_atail, ax		;   active queue.


int_no_xq:			
		mov	[bx].acb_scb.SCB_Cmd, TMS_TRANSMIT   ; Set Xmit cmd

		mov	ax, [di].XMIT_MyMoto.Mhi	; Set list's motorola
		mov	[bx].acb_scb.SCB_Ptr.Mhi, ax    ;   address into the		
		mov	ax, [di].XMIT_MyMoto.Mlo	;   SCB.
		mov	[bx].acb_scb.SCB_Ptr.Mlo, ax	;	
		SJMP	int_issue_scb			; Issue SCB command


;
;  Now check the general request waiting queue.  If there is anything
;  waiting, then dequeue the head of the queue and build up an SCB.
;
int_check_genq:
 		mov	si, [bx].acb_req_next	        ; Get waiting queue
		or	si, si				; Anything there ?
		LJZ	int_exit			;  No, exit int

		xor	ax, ax          		; Clear waiting
		mov	[bx].acb_req_next, ax		;

		mov	ax, [si].GRB_SCBCmd	        ; Set SCB command
		mov	[bx].acb_scb.SCB_Cmd, ax	;   into SCB.

		mov	ax, [si].GRB_SCBPHi		; Set parm list's 
		mov	[bx].acb_scb.SCB_Ptr.Mhi, ax	;   motorola address
		mov	ax, [si].GRB_SCBPLo	        ;   into the SCB.
		mov	[bx].acb_scb.SCB_Ptr.Mlo, ax	;

int_issue_scb:
		mov	dx, [bx].acb_sif_int	; Get SIFINT port address
		mov	ax, SIFINT_CMD		; Set command interrupt
		out	dx, ax			; Inform adapter

		LJMP	int_exit		; Exit int


int_issue_req:
		mov	ax, SIFINT_SCBREQST	; Set SCB clear request
					 	
		out	dx, ax		        ; Inform adapter

		LJMP	int_exit		; Exit int handler


interrupt	ENDP


END_CODE
		END

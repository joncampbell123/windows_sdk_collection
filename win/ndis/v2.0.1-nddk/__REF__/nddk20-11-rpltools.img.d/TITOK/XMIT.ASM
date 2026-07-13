		PAGE  60, 132
		TITLE Token-Ring RIPL Boot ROM Driver - XMIT.ASM
;**********************************************************************;
;**********************************************************************;
;								       ;
; File Name:       XMIT.ASM            				       ;
;								       ;
; Program Name:    Token-Ring RIPL Boot ROM Device Driver              ;
;								       ;
; Companion Files: None						       ;
;								       ;
; Function:        This module contains the routines that handle       ;
;                  transmit requests from the upper boundry of the     ;
;                  TRNDD.  This entire module falls under the Upper    ;
;                  Boundry Request Handlers functional block of the    ;
;                  TRNDD functional specification.  This module in-    ;  
;                  cludes the definition of the following routines:    ;
;								       ;
;								       ;
;								       ;
; History:             						       ;
;								       ;
;     03/20/89  Mike Witkowski - Original Version 0.0		       ;
;     02/27/90  Mike Witkowski - Reworked for RIPL                     ;
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
		EXTRN	gbl_dynmem:		WORD
		EXTRN	gbl_RPLaddr:		BYTE
		EXTRN	gbl_xlist_table:	WORD

		
;--------------------------;
; External Code References ;
;--------------------------;
                EXTRN   VirttoPhys:             NEAR
		EXTRN	int_exit:		NEAR
		EXTRN	DLCPostEvent:		NEAR

START_CODE

PAGE_BREAK <NDITransmitPacket Routine Definition>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;  Routine Name:  NDITransmitPacket Routine                            ;
;                   					               ;
;  Description: The NDITransmitPacket Routine accepts all directed     ;
;               send requests from the RIPL ROM DLC code.  This        ;
;               routine determines the validity of the request and     ;
;               works with the adapter to carry it out.                ;
;                                                                      ;
;  Input:       ES:SI - pointer to ED                                  ;
;                                                                      ;
;  Output:      ED Completion Code field updated                       ;
;								       ;
;  Calls:       TransmitPacket                                         ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

                        PUBLIC  NDITransmitPacket
NDITransmitPacket        PROC	NEAR

;
;  Set the transmit type field in the ED and call the transmit routine.
;
                mov     es:[si].ED_XmitType, XT_DIRECTED  ; Set xmit type
                call    TransmitPacket                    ; Transmit frame
                ret                                       ; Return to caller

NDITransmitPacket        ENDP


PAGE_BREAK <NDITransmitRPL Routine Definition>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;  Routine Name:  NDITransmitRPL Routine                               ;
;                   					               ;
;  Description: The NDITransmitRPL Routine accepts all broadcast       ;
;               send requests from the RIPL ROM DLC code.  This        ;
;               routine determines the validity of the request and     ;
;               works with the adapter to carry it out.                ;
;                                                                      ;
;  Input:       ES:SI - pointer to ED                                  ;
;                                                                      ;
;  Output:      ED Completion Code field updated                       ;
;								       ;
;  Calls:       TransmitPacket                                         ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

                PUBLIC  NDITransmitRPL
NDITransmitRPL  PROC	NEAR

;
;  Set the transmit type field in the ED and call the transmit routine.
;
                mov     es:[si].ED_XmitType, XT_BROADCAST ; Set xmit type
                call    TransmitPacket                    ; Transmit frame
                ret                                       ; Return to caller

NDITransmitRPL  ENDP


PAGE_BREAK <TransmitPacket Routine Definition>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;  Routine Name:  TransmitPacket Routine                               ;
;                   					               ;
;  Description: The TransmitPacket Routine accepts all send requests   ;
;               from the RIPL ROM DLC code.  This routine determines   ;
;               the validity of the request and works with the adapter ;
;               to carry it out.                                       ;
;                                                                      ;
;  Input:       ES:SI - pointer to ED                                  ;
;                                                                      ;
;  Output:      ED Completion Code field updated                       ;
;								       ;
;  Calls:       VirttoPhys                                             ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

                PUBLIC  TransmitPacket
TransmitPacket  PROC	NEAR

                push    ds                      ; Save data segment

		pushf				; Save flags

		cli				; Disable interrupts

		mov	ds, cs:gbl_dynmem	; Point to dynamic memory
		xor	bx, bx                  ; Set pointer to ACB


;
;  First check to see if the adapter is in the proper state for trans-
;  mitting frames.
; 	
		mov	ax, INVALID_FUNCTION	  	; Set error code
		cmp	[bx].acb_state, AS_OPENED	; Is adapter opened ?
		LJNE	xmit_error			;   No, return error

				
;
;  Now, get the ED pointer to the transmit data buffer descriptor and 
;  verify the fields contained in it.
;
		mov	ax, INVALID_PARAMETER	; Set error code
	        mov	di, es:[si].ED_DCount	; Get data count
	        cmp	di, MAXTXBUFS		; Is it > max ?
		LJA	xmit_error		;   Yes, return error
		or	di, di			; Is total # of ptrs = 0 ? 
	      	LJZ	xmit_error		;   Yes, return error

		mov	ax, OUT_OF_RESOURCE	; Set out of resource error
		shl	di, 1			; Make count a word pointer

		mov	dx, cs:gbl_xlist_table[di] 
                                                ; Get # of lists required

		cmp	dx, [bx].acb_xlist_cnt	; Are there enough lists ?		
		LJA	xmit_error		;  No, return error

		xor	di, di			; Clear immediate pointer

		mov	di, [bx].acb_ibuf_free  ; Get buffer from free Q
		or	di, di	        	; Are there any left ?
		LJZ	xmit_error	        ;   No, return error

		push	ds			; Swap ES and DS
		push	es			; 
		pop	ds		        ; 
		pop	es		        ; 

		mov	ax, es:[di].IBUF_Ptr	   ; Dequeue the immediate 
		mov	es:[bx].acb_ibuf_free, ax  ;   buffer from the free Q


;
;  Place the Token-Ring MAC header into the immediate buffer.  Check
;  the transmit type field of the ED which was set up earlier.  If this
;  is a directed transmit request, load the passed destination address. 
;  If this is a transmit to the RPL Functional address, then copy in the 
;  known RPL Functional address as the destination. 
;
                push    di                      ; Save buffer pointer address
                lea     di, [di].size_IBUF      ; Point to buffer data
                mov     ax, 4010h               ; Set AC/FC bytes
                stosw                           ; Place them into buffer

                push    si                      ; Save ED pointer

                cmp     [si].ED_XmitType, XT_BROADCAST  ; If this broadcast ?
                SJE     xmit_rpl                        ; Yes, load RPL addr
                        
                lea     si, [si].ED_DestAddr    ; Point to destination address
                mov     cx, 3                   ; Set word count
                rep     movsw                   ; Copy destination address
                SJMP    xmit_popbuf             ; Continue

xmit_rpl:
                lea     si, gbl_RPLaddr         ; Point to RPL Functional addr
                mov     cx, 3                   ; Set word count
                rep     movs WORD PTR es:[di], WORD PTR cs:gbl_RPLaddr 
                                                ; Copy RPL Functional address

xmit_popbuf:
                pop     si                      ; Restore ED pointer
                pop     di                      ; Restore buffer pointer


;
;  Now dequeue the first list from the free pool.
;		
		sub	es:[bx].acb_xlist_cnt, dx   ; Dec # available lists
		mov	ax, es:[bx].acb_xmit_head   ; Get first free list
		push	ax			    ; Save current EOF list

		push	es:[di].IBUF_MyMoto.Mlo	    ; Save the phys buf addr
		push	es:[di].IBUF_MyMoto.Mhi	    ; Save the phys buf addr
		push	di		            ; Save imm buffer address

		mov	di, ax			; Set next free list
		mov	es:[di].XMIT_EOF, dx	; Set number of lists in EOF
		add	di, XMIT_Dptrs		; Point to list's data ptrs

                mov     bp, si                  ; Save ED pointer offset
		add	si, ED_Descript	        ; Point to xmit data dscrpt's

		mov	dh, BYTE PTR [si].ED_DCount - BYTE PTR ED_Descript
						; Set xmit descriptor count

		mov	dl, 3			; Set list pointer count
		xor	cx, cx			; Clear frame size

		mov	es:[di].XMIT_ImmPtr - WORD PTR XMIT_Dptrs, cx    
						; Clear imm ptr @ in list
				
		pop	ax			; Get immediate buffer ptr

		mov	es:[di].XMIT_ImmPtr - WORD PTR XMIT_Dptrs, ax    
						; Set immediate ptr @ in list

		mov	ax, 14                  ; Set immediate size = 14
		mov	cx, ax			; Set current frame size
		xchg	ah, al			; Put count in motorola form
		or	ax, DATA_NOT_LAST	; Set not last data count
		stosw				; Put it into the list
			
		pop	ax		 	; Get imm buf physical @(hi)
		stosw				; Put it into the list
		pop	ax			; Get imm buf physical @(lo)
		stosw				; Put it into the list

		dec	dl			; Decrement list ptr count    	


;
;  This is the main transmit data buffer descriptor to transmit list
;  descriptor conversion loop. 
;
xmit_cvrt_loop:
		mov	ax, [si].DLen   	; Get data length for ptr
		add	cx, ax			; Add it to total frame size

		xchg	ah, al			; Put it into motorola form
		or	ax, DATA_NOT_LAST	; Set "data not last" bit
		stosw				; Write it into list
									
                push    [si].DPointer.hi        ; Pass segment/offset pair
                push    [si].DPointer.lo        ;
                call    VirttoPhys              ; Convert it to motorola
                pop     ax                      ; Put it into the list
                stosw                           ;
                pop     ax                      ; 
                stosw                           ;

		dec	dl		     	; Is this last ptr in list ?
		SJNZ	xmit_next_cvt		;   No, skip this
		cmp	dh, 1			; Are ther any more data ?
		SJZ	xmit_cvt_done		;   No, we're all done
		mov	dl, 3			; Reset number of list ptrs

		mov	di, es:[di].XMIT_Next - WORD PTR XMIT_MyMoto
						; Get next free list 

		pop	ax			; Pop off current EOF list  			
		push	di			; Save current EOF list
		add	di, XMIT_Dptrs		; Point to list's data ptrs
		

xmit_next_cvt:
		add	si, size_descript      	; Get next data descriptor
		dec	dh			; Decrement # outstanding
		SJNZ	xmit_cvrt_loop		; Convert next descriptor


;
;  The conversion is completed.  Now it is time to fill in the 
;  remaining control and information fields in the lists before 
;  they are queued up to the adapter.  
;  
;  A special note about how the	EOF field in the list is handle should 
;  be made here.  The EOF field contains to parameters: the EOF list 
;  offset and the number of lists used by this transmit request.  Since 
;  we know that lists will always reside on a 4-byte boundry, the least 
;  significant 2 bits of the field will never be set.  And, since the 
;  highest number of lists is 3, we may use the 2 LSB's to store the list 
;  count. This is why we OR in the EOF pointer here.
;
xmit_cvt_done:
		push	es			; Swap ES and DS
		push	ds			; 
		pop	es	                ;
		pop	ds	                ;

		and	[di].XDP_Length - size_XDP, DATA_LAST
		pop	di
		mov	[di].XMIT_CSTAT, XCSTAT_EOF

		mov	si, [di].XMIT_Next	; Get next free list
		xchg	si, [bx].acb_xmit_head	; Set new free Q head
		or	[si].XMIT_EOF, di	; Set ptr to last list

		mov	[si].XMIT_ED.offp, bp	; Set ED pointer
		mov	[si].XMIT_ED.segp, es	; 

		or	[si].XMIT_CSTAT, XCSTAT_FINT	; Set frame interrupt

		xchg	ch, cl			; Put size in motorola form 
		mov	[si].XMIT_Fsize, cx		; Set list frame size 
		or	[si].XMIT_CSTAT, XCSTAT_LSOF	; Set start of frame
		

;
;  Now it is time to queue the lists up to the adapter to be transmitted.
;  This is done by merely issuing a Transmit valid to the adapter.
;
		mov	[bx].acb_xmit_atail, di ; Queue lists onto active Q
		cmp	[bx].acb_xmit_ahead, 0	; Is transmit active ? 
 		SJNZ	xmit_req		;   Yes, issue valid

		mov	[bx].acb_xmit_ahead, si	; Set new active Q head

xmit_req:
		mov	dx, [bx].acb_sif_int    ; Set SIFINT address
		mov	ax, SIFINT_XMTVALID	; Set request SCB code
		out	dx, ax			; Issue request to adapter

		mov	ax, REQUEST_QUEUED	; Set request queued


xmit_req_exit:
		popf                            ; Restore interrupts
                pop     ds                      ; Restore data segment
		ret	                 	; Return to caller


;
;  A transmit error occurred, call the post event routine to return 
;  the ED. 
;
xmit_error:
                mov     es:[si].ED_ComCode, al  ; Set return code
                push    ds                      ; Save dynamic data segment
                push    cs                      ; Set DS = CS
                pop     ds                      ;
                call    DLCPostEvent            ; Return the ED   
                pop     ds                      ; Restore data segment

                SJMP    xmit_req_exit           ; Return to caller                


TransmitPacket  ENDP


PAGE_BREAK <Transmit Status Interrupt Handler Definition>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;  Routine Name:  Transmit Status Interrupt Handler       	       ;
;                   					               ;
;  Description: The Transmit Status Interrupt Handler is called by     ;
;               the adapter interrupt routine to handle transmit       ;
;               statuses.  This routine examines the SSB for the       ;
;               transmit status and sets up the lists for the          ;
;               call to the Transmit Chain Request Confirmation        ;
;               routine.                                               ;
;                                                                      ;
;  Input:       BX - pointer to MCB                                    ;
;                                                                      ;
;  Output:                                                             ;
;								       ;
;  Calls:                                                              ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

Xmit_Status	PROC	NEAR

MLABEL <XmitHandler>, %MWORD, %MPUBLIC

;----------------------------------------------------------------------;
;  If a frame complete was generated, then merely exit.               ;
;----------------------------------------------------------------------;
		cmp	[bx].acb_ssb.SSB_Status, XSTAT_FRMCMPLT
		LJE	int_exit

;----------------------------------------------------------------------;
;  A list error must have occurred.  Dequeue the head of the active    ;
;  queue and queue it onto the confirm queue.  If there are still      ;
;  active lists, we must reissue a transmit command. 	               ;
;----------------------------------------------------------------------;
		xor	di, di
		xchg	di, [bx].acb_xmit_ahead	     ; Get list in error	
		mov	si, [di].XMIT_EOF	     ; Get frame's EOF list
		and	si, 0fffch		     ; AND out list count
                push    [si].XMIT_ED.segp            ; Save ED Address
                push    [si].XMIT_ED.offp            ;
		cmp	si, [bx].acb_xmit_atail	     ; Is this last active ?
		mov	si, [si].XMIT_Next 	     ;   Get next list 
		SJE	xmit_rexmit		     ;   Yes, restart xmits 
		mov	[bx].acb_xmit_ahead, si	     ; Set new active Q head


;
;  Now, retransmit any frames that are still active.
;
xmit_rexmit:
		mov	dx, [bx].acb_sif_int	     ; Load SIFINT port addr	
		cmp	[bx].acb_scb.SCB_Cmd, 0	     ; Is SCB busy ?
		SJNZ	xmit_qto_wait		     ;   Yes, queue to wait Q

		mov	[bx].acb_scb.SCB_Cmd, TMS_TRANSMIT   ; Set Xmit Cmd	

		mov	ax, [si].XMIT_MyMoto.Mhi     ; Get active head's 
		mov	[bx].acb_scb.SCB_Ptr.Mhi, ax ;   motorola address 				
		mov	ax, [si].XMIT_MyMoto.Mlo     ;   and load it into 
		mov	[bx].acb_scb.SCB_Ptr.Mlo, ax ;   the SCB.				
								
		mov	ax, SIFINT_CMD		     ; Set SIF Command Req
		out	dx, ax			     ; Inform adapter
		SJMP	xmit_sts_exit		     ; Exit int handler

					
xmit_qto_wait:
		xor	ax, ax			     ; Clear AX
		xchg	ax, [bx].acb_xmit_ahead	     ; Get active Q  
                or      ax, ax                       ; Is xmit active ?
                SJNZ    @f                           ;   Yes, queue to waiting
		mov	[bx].acb_xmit_whead, si	     ;   No, just queue head
		mov	[bx].acb_xmit_wtail, si	     ;
                SJMP    xmit_issue_wait              ; 
@@:
		xchg	ax, [bx].acb_xmit_whead	     ;   Place all active 
		xchg	ax, [bx].acb_xmit_atail	     ;	 members onto the
		mov	[bx].acb_xmit_wtail, ax	     ;	 waiting Queue

xmit_issue_wait:
		mov	ax, SIFINT_SCBREQST	     ; Set SIF SCB request	
		out	dx, ax			     ; Inform adapter


;
;  Now call the DLCPostEvent routine to queue the ED to the RIPL 
;  ROM's DLC code. 
;
xmit_sts_exit:
                pop     si                      ; Restore ED address
                pop     es                      ; 

                mov     es:[si].ED_ComCode, GENERAL_FAILURE
                                                ; Set error code

                push    ds                      ; Save dynamic data segment
                push    cs                      ; Set DS = CS
                pop     ds                      ;
                call    DLCPostEvent            ; Queue the ED to DLC
                pop     ds                      ; Restore data segment

		LJMP	int_exit		; Exit int handler

Xmit_Status	ENDP

END_CODE
		END

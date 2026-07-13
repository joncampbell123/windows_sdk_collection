		PAGE  60, 132
		TITLE Token-Ring RIPL Boot ROM Driver - RCV.ASM
;**********************************************************************;
;**********************************************************************;
;								       ;
; File Name:       RCV.ASM            				       ;
;								       ;
; Program Name:    Token-Ring RIPL Boot ROM Device Driver              ;
;								       ;
; Companion Files: None						       ;
;								       ;
; Function:        This module contains the routines that handle       ;
;                  the receive frame process.  This includes routines  ;
;                  called from the interrupt handler .                 ;
;                  This module includes the definition of the          ;
;                  following routines:                                 ;
;								       ;
;	  	   Rcv_Status - Handles receive status interrupts      ;
;								       ;
; History:             						       ;
;								       ;
;     03/20/89  Mike Witkowski - Original Version 0.0		       ;
;     03/14/90  Mike Witkowski - Reworked for RIPL                     ;
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


;--------------------------;
; External Code References ;
;--------------------------;
                EXTRN   DLCGetED:               NEAR
                EXTRN   DLCPostEvent:           NEAR

PAGE_BREAK <Receive Status Interrupt Handler Definition>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;  Routine Name:  Receive Status Interrupt Handler       	       ;
;                   					               ;
;  Description: The Receive Status Interrupt Handler is called by      ;
;               the adapter interrupt routine to handle receive        ;
;               statuses.  Since this version of the driver uses a     ;
;               circular queue of receive lists, this routine only     ;
;               needs to process receive frame complete interrupts.    ;
;               Also, this driver must guarantee that there are enough ;
;               receive buffer space allocated to receive the largest  ;
;               possible frame size that can be received.              ;
;                                                                      ;
;                                                                      ;
;  Input:       BX - pointer to MCB                                    ;
;                                                                      ;
;  Output:                                                             ;
;								       ;
;  Calls:                                                              ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

START_CODE
                PUBLIC  Rcv_Status
Rcv_Status	PROC	NEAR

                PUSHREG si                      ; Save int reason register

;
;  Enable the transfer flag and scan the receive lists to determine 
;  if there are any receive lists to be processed.  If so, dequeue 
;  the first receive data buffer and call the receive lookahead
;  routine. 
;
		mov	si, [bx].acb_rcv_head	; Get head of receive queue


;
;  The following is the actual receive scanning loop.
;
rcv_scan:
		mov	ax, [si].RCV_CSTAT	; Get copy of CSTAT	
		test	ax, RCSTAT_COMPLETE	; Is rcv list ready ?
		LJZ	rcv_int_exit	     	;   No, exit loop

		test	ax, RCSTAT_EOF 		; Is the EOF bit set ? 
		LJZ	rcv_list_purge		;   No, purge lists 	
		test	ax, RCSTAT_SOF 		; Is the SOF bit set ? 
		LJZ	rcv_list_purge		;   No, purge lists 	
                
		PUSHREG	bx, si  		; Save important regs
                
                push    ds                      ; Save dynamic data segment
                push    cs                      ; Set ROM data segment
                pop     ds                      ;
                call    DLCGetED                ; Get an ED (ES:SI)
                pop     ds                      ; Restore dynamic data segment
                mov     di, si                  ; Copy ED pointer

		POPREG	bx, si  	        ; Restore important regs

                SJZ     rcv_no_filter           ; If no ED, exit
                
		PUSHREG	bx, si  		; Save important regs

                call    xfer_request            ; Transfer Data in ED buffer
                mov     si, di                  ; Copy ED pointer

                push    ds                      ; Save dynamic data segment
                push    cs                      ; Set ROM data segment
                pop     ds                      ;
                call    DLCPostEvent            ; Queue the ED
                pop     ds                      ; Restore dynamic data segment
        
		POPREG	bx, si  	        ; Restore important regs
                

rcv_no_filter:
		mov	[si].RCV_CSTAT, RCSTAT_GO  ; Reset list's CSTAT field
		mov	si, [si].RCV_Next	   ; Get next list
		mov	[bx].acb_rcv_head, si	   ; Set new rcv queue head

		LJMP	rcv_scan	    	; Scan the next list

		PAGE
;
;  A receive that is longer that one list has been detected.  Search 
;  for the EOF list.  
; 
rcv_list_purge:
		test	[si].RCV_CSTAT, RCSTAT_EOF	; Is this EOF list ? 
		SJNZ	rcv_no_filter			;   Yes, purge done
		mov	[si].RCV_CSTAT, RCSTAT_GO	; Set list ready 
		mov	si, [si].RCV_Next		; Get next list
		SJMP	rcv_list_purge			; Continue purge


;
;  At this point, receive list scanning stopped because there are no more 
;  lists with data.  
;  
rcv_int_exit:
		mov	dx, [bx].acb_sif_int	; Get SIFINT port address
		mov	ax, SIFINT_RCVVALID	; Set receive valid
		out	dx, ax			; Inform adapter 

                POPREG  si                      ; Restore int reason
                ret                             ; Return to caller

Rcv_Status	ENDP



PAGE_BREAK <TransferData Routine Definition>
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;
;  Routine Name:  TransferData Routine                                 ;
;                   					               ;
;  Description: The TransferData Routine transfers data from the       ;
;               driver's internal receive buffers to the ED buffer    ;
;               passed to this routine.                                ;
;                                                                      ;
;  Input:       ES:DI - ED pointer                                    ;
;               SI    - receive list pointer                           ;
;                                                                      ;
;  Output:      none                                                   ;
;								       ;
;  Calls:       none                                                   ;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++;

xfer_request	PROC	NEAR

                PUSHREG es, di, si              ; Save needed regs

		mov	dx, es:[di].ED_DCount	; Get # of descrptrs
		lea	di, [di].ED_Descript	; Get buffer descrptr
		lea	ax, [si].RCV_Buf	; Get buffer address
		mov	cx, [si].RCV_Fsize	; Get frame size
		xchg	ch, cl			; Set intel format

                test    BYTE PTR [si].RCV_Buf.FF_SRC0, 80h
                                                ; Does frame contain routing ?
                SJNZ    @F                      ;   Yes, find MAC header len
                xor     si, si                  ; Set routing length
                SJMP    xr_setlen               ; Set buffer offset

@@:
		mov	si, [si].RCV_Buf.FF_RI	; Get routing field
                and     si, 001fh               ; Isolate length bits

xr_setlen:
                add     si, 14                  ; Set full MAC header length

		
;
;  Now it is time to transfer the data.  Set up the remaining parameters
;  for the main receive data loop.
;
                sub     cx, si                  ; Set remaining size          		
		add	si, ax			; Set move start addr
		xor	ax, ax			; Set total move cnt	


;
;  This is the main receive data loop.  Each iteration through the loop 
;  processes one transfer data descriptor. 
;
rcv_data_loop:
		push	dx			; Save number of descriptors
		push	ax			; Save total move count
		push	cx   			; Save remaining size

		mov	ax, es:[di].DLen	; Get decriptor len
		
		cmp	cx, ax			; Will remaining data fit ?
		SJBE	rcv_size_set		;   Yes, skip				   	
		mov	cx, ax			;   No, adjust move size

rcv_size_set:					
		push	cx			; Save move size
		push	es			; Save descripor pointer
		push	di			;

		les	di, es:[di].DPointer	; Load data pointer


rcv_move_data:
		OPTMOVE				; Move the data

		pop	di  			; Restore descripor address
		pop	es			;
                add     di, 6                   ; Point to next descriptor

		pop	dx			; Restore move count		
	     	pop	cx		        ; Restore remaining size
	     	pop	ax		        ; Restore total move count

		add	ax, dx			; Calculate total count
		sub	cx, dx			; Calculate remaining size 
		pop	dx			; Restore # of descriptors
		SJZ	rcv_move_done

		dec	dx		        ; Are we through ?
		SJNZ	rcv_data_loop		;   No, check next one		
		
;
;  At this point, the move data loop could have stopped for two reasons.
;  First, we could have transfered all the data to the upper protocol 
;  driver.  Second, we could have run out of data descriptors.  In either
;  case, register AX now contains the number of bytes moved, so set this
;  into the bytes copied variable that was passed to us. 
;
rcv_move_done:
                POPREG  es, di, si              ; Restore regs

                PUSHREG es, di, si              ; Save needed regs

                lea     si, [si].Rcv_Buf.FF_SRC0
                                                ; Point to source addr
                lea     di, [di].ED_DestAddr    ; Point to address field
                mov     cx, 3                   ; Set word count
                and     BYTE PTR [si], 7fh      ; Clear routing bit
                rep     movsw                   ; Copy destination address

                POPREG  es, di, si              ; Restore regs

                mov     es:[di].ED_ComCode, 0  ; Set good completion

		ret

xfer_request	ENDP

END_CODE
		END

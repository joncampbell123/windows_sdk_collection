page ,132
;                                                                       ;
;-----------------------------------------------------------------------;
;                                                                       ;
;                      Memory Management Routines			;
;                                                                       ;
;-----------------------------------------------------------------------;

include	cmacros.inc
include	galloc.inc
include	pdb.inc

page

sBegin	CODE
	assumes	DS, nothing
	assumes	CS, code

;
ExternFP	<GetCurrentPDB>

	public	alloc_partition
	public	free_memory
	public	free_partition
	public	modify_partition
	public	fix_all_partitions
	public	check_partition

CurPDB		dw	0		; temporary copy of current PDB


;======= GetMemoryBase =======================================================
;
; Get the top memory arena within the partition where the current PDB resides
;
; Entry:
;
;
; Exit:
;   AX=		Top memory arena (base)
;   CurPDB	Current task PDB
; Regs:
;   DS, AX, FLAGS
;
;=============================================================================


GetMemoryBase	Proc	NEAR

	push	DX
;
; Get currentPDB
;
	ccall	GetCurrentPDB
	mov	CS:[CurPDB], AX		; Save it temporarily
	mov	DS, AX
	mov	AX, word ptr DS:[PDB_GlobalHeap +2]
	dec	AX			; AX= Top memory base
;
	pop	DX
	ret

GetMemoryBase	EndP



page
;-----------------------------------------------------------------------;
; alloc_partition						        ;
; 								        ;
; Allocate a block of memory.					        ;
; 								        ;
; Arguments:							        ;
; 	BX = size in paragraphs					        ;
; Returns:							        ;
; 	AX:0 = new block					        ;
; Alters:							        ;
; 	nothing							        ;
; Calls:							        ;
; 								        ;
; History:							        ;
; 								        ;
;  Wed Jul 23, 1986 11:48:31a  -by-    Charles Whitmer    [cxw]		;
; Made it get critical inside since some people were calling it while	;
; not in critical sections!						;
; 									;
;  Mon Jun 10 10:38:56 1985    -by-    Charles Whitmer    [cxw]	        ;
; Took the guts of get_block to make this routine.		        ;
; 								        ;
;  Sun Feb 24 23:25:21 1985    -by-    Charles Whitmer    [cxw]	        ;
; Wrote it.							        ;
;-----------------------------------------------------------------------;



alloc_partition	proc	near

	push	ds
	push	es
	push	si
	cli
;
	call	GetMemoryBase		; AX= memory base
;
; get ready for the search

	mov	ds,ax
	xor	si,si			; SI = maximum block available
get_search_loop:

; check to see if the partition is valid

	call	check_partition
	jnc	arena_is_ok

get_error_return:
	sti
	pop	si
	pop	es
	pop	ds
	stc
	ret
arena_is_ok:

; make sure it's free

	cmp	DS:ga_owner,0
	jz	yes_its_free

try_the_next:
	cmp	DS:ga_sig,'Z'
	jnz	get_the_next
	mov	bx,si			; return maximum available
	mov	ax,8			; error: Insufficient memory
	jmp	get_error_return

get_the_next:
	inc	ax
	add	ax,DS:ga_size
	mov	ds,ax
	jmp	get_search_loop

yes_its_free:

; is it big enough?

	cmp	bx,DS:ga_size
	jbe	got_a_live_one
	cmp	si,DS:ga_size
	jae	try_the_next
	mov	si,DS:ga_size
	jmp	try_the_next

got_a_live_one:

; if it's exactly the right size, mark it as used

	jb	get_cut

get_is_done:
 	mov	si, CS:[CurPDB]
	mov	DS:ga_owner,si
	sti
	pop	si
	pop	es
	pop	ds
	inc	ax
	clc
	ret

; split block into two blocks

get_cut:
	mov	si,DS:ga_size
	mov	DS:ga_size,bx		; shrink the block
	sub	si,bx			; calculate remainder
	dec	si
	add	ax,bx
	inc	ax
	mov	es,ax
	mov	al,ds:ga_sig	; steal old identity
	mov	es:ga_sig,al
	mov	ds:ga_sig,'M'	; the found one is not last
	mov	es:ga_owner,0		; mark remainder as free
	mov	es:ga_size,si
	mov	ax,ds
	jmp	get_is_done

alloc_partition	endp




;-----------------------------------------------------------------------;
; free_memory								;
; 									;
; Frees up memory allocated by alloc_memory.				;
; 									;
; Arguments:								;
; 	ES = segment to be freed					;
; Returns:								;
; 	CF = 0   if segment freed OK					;
; 	CF = 1   if segment was not allocated by alloc_memory		;
; Alters:								;
; 	nothing								;
; Calls:								;
; 	nothing								;
; History:								;
; 									;
;  Tue Sep 17 05:22:50 1985    -by-    Wesley O. Rupel    [wor]		;
; Moved it here from memory.asm to allow it to fall into		;
; free_partition rather than calling it and returning.			;
; 									;
;  Fri Apr 12 11:21:24 1985    -by-    Charles Whitmer    [cxw]		;
; Changed return codes and made it transparent.				;
; 									;
;  Fri Feb 08 02:04:17 1985    -by-    Charles Whitmer    [cxw]		;
; Fixed bug in collapsing free partitions.				;
; 									;
;  Sat Jan 26 22:33:30 1985    -by-    Anthony C. Pisculli    [acp]	;
; Replaced mem_sems with critical section macros			;
; 									;
;  Fri Feb 08 02:06:43 1985    -by-    Charles Whitmer    [cxw]		;
; Wrote it long ago.							;
;-----------------------------------------------------------------------;



;-----------------------------------------------------------------------;
; free_partition						        ;
; 								        ;
; Free an allocated block of memory.				        ;
; 								        ;
; Arguments:							        ;
; 	ES = block to be returned				        ;
; Returns:							        ;
; 	nothing							        ;
; Alters:							        ;
; 	nothing							        ;
; Calls:							        ;
; 	fix_all_partitions					        ;
; 	check_partition						        ;
; History:							        ;
; 								        ;
;  Mon Jun 10 11:22:24 1985    -by-    Charles Whitmer    [cxw]	        ;
; Wrote it.							        ;
;-----------------------------------------------------------------------;



free_memory 	proc	near
free_partition	proc	near

	push	ES
	push	ax

; check the submitted block

	mov	AX, ES
	dec	ax
	call	check_partition
	jnc	block_is_ok
	mov	ax,9			; error: Invalid memory block

free_error_return:
	add	sp,2
	pop	ES
	stc
	ret

block_is_ok:

; free this block

	push	ds
	mov	ds,ax
	mov	DS:ga_owner,0
	call	GetMemoryBase		; AX= Top memory base
	pop	ds

; fix the arena and go home

	call	fix_all_partitions
	jc	free_error_return
	pop	ax
	pop	ES
	clc
	ret

free_partition	endp
free_memory endp


;-----------------------------------------------------------------------;
; modify_partition							;
; 									;
; Attempts to change the size of an existing memory partition.		;
; 									;
; Arguments:								;
; 	ES = block to modify						;
; 	BX = requested block size					;
; Returns:								;
; 	CF = 0	done							;
; 	CF = 1	error							;
; 	  AX = 7  memory blocks plastered				;
; 	  AX = 8  insufficient memory					;
; 	    BX = maximum available memory				;
; Alters:								;
; 	nothing								;
; Calls:								;
; 									;
; History:								;
; 									;
;  Mon Mar 03 03:00:27 1986    -by-    Nathan P. Myhrvold    [npm]	;
; 	Fixed a bug with growing the next to last partition.		;
; 									;
;  Tue Jun 18 23:10:54 1985    -by-    Oren Cheyette    [orc]		;
; 	Wrote it.							;
;-----------------------------------------------------------------------;


modify_partition	proc	near

	push	ds
	push	es
	push	cx
	push	dx
	mov	AX, ES
	dec	ax
	call	check_partition
	jnc	this_block_ok

modify_exit:
	pop	dx
	pop	cx
	pop	es
	pop	ds
	ret

this_block_ok:
	mov	ds,ax
	cmp	BX,DS:ga_size		; check size
	jnz	change_size
	clc
	jmp short modify_exit

; grow the partition

	cmp	DS:ga_sig,'Z'
	jnz	not_the_last

not_enough_memory1:
	mov	BX,DS:ga_size		; no more memory available
not_enough_memory:
	stc
	mov	ax,8
	jmp short modify_exit

change_size:
	jb	shrink
;
; if the current block is the last one, we return error #8 since we cannot
; satisfy the grow request.
;

	cmp	DS:ga_sig, 'Z'
	je	not_enough_memory1
	
not_the_last:
	mov	ax,ds
	add	ax,DS:ga_size
	inc	ax			; AX = next partition
	call	check_partition
	jc	modify_exit

	mov	es,ax
	cmp	es:ga_owner,0		; see if its free
	jnz	not_enough_memory1	; occupied
	mov	dx,es:ga_size
	inc	dx			; size of next partition
	mov	ax,DS:ga_size
	sub	BX,ax			; BX = amount to expand
	sub	dx,BX 			; enough room?
	jae	enough_memory
	add	BX,ax
	add	BX,dx
	jmp short not_enough_memory

; AX = original size of partition we're growing
; BX = amount to expand it by
; DX = (available space in next partition - amount to expand by)
; DS = partition we're growing
; ES = next partition

enough_memory:
	add	ax,BX			; AX = size of grown partition
	mov	DS:ga_size,ax		; set new size of grown partition

	cmp	es:ga_sig,'Z'	; was the next partition the last one?
	jnz	dont_replace_the_end
	or	dx,dx			; is there any memory left
	jnz	dont_replace_the_end	; in the last partition?
	mov	DS:ga_sig,'Z'		; make the current partition the last 
	jmp	short done_growing

dont_replace_the_end:
	or	dx,dx
	jz	done_growing		; took all remaining memory
	dec	dx			; size of shrunk partition
	inc	ax
	mov	CX,ds
	add	CX,ax
	mov	ds,CX			; DS = new location of shrunk partition
	mov	CL,es:ga_sig
	mov	DS:ga_owner,0		; not occupied
	mov	DS:ga_sig,CL
	mov	DS:ga_size,dx

done_growing:
	clc
	jmp 	modify_exit

shrink:
	mov	ax,ds
	mov	dx,DS:ga_size
	mov	DS:ga_size,BX		; new size of old partition
	mov	CX,dx
	sub	CX,BX			; size of new partition
	add	ax,BX
	inc	ax

	mov	es,ax			; location of new partition
	mov	al,DS:ga_sig
	mov	es:ga_sig,al		; inherit identifier
	mov	DS:ga_sig,'M'		; definitely not the last partition
	mov	es:ga_owner,0		; free
	dec	CX
	mov	es:ga_size,CX
	mov	ax,es
	call	fix_all_partitions
	clc
	jmp	modify_exit

modify_partition	endp



;-----------------------------------------------------------------------;
; fix_all_partitions						        ;
; 								        ;
; Checks the integrity of the memory partition list, and combines       ;
; together any consequetive free blocks.			        ;
; 								        ;
; Arguments:							        ;
; 	AX = base						        ;
; Returns:							        ;
; 	nothing							        ;
; Alters:							        ;
; 	nothing							        ;
; Calls:							        ;
; 	nothing							        ;
; History:							        ;
; 								        ;
;  Mon Jun 10 10:57:32 1985    -by-    Charles Whitmer    [cxw]	        ;
; Modified the old routine fix_all_blocks.			        ;
; 								        ;
;  Mon Feb 25 03:54:06 1985    -by-    Charles Whitmer    [cxw]	        ;
; Wrote it.							        ;
;-----------------------------------------------------------------------;


fix_all_partitions	proc	near
	push	ax
	push	es
	push	ds

	mov	ds,ax

; check base block for validity

	call	check_partition
	jnc	base_block_is_ok
fix_error_return:
	pop	ds
	pop	es
	add	sp,2
	stc
	ret

base_block_is_ok:

; check if this is the last block

fix_loop:
	cmp	DS:ga_sig,'Z'
	jnz	theres_more_to_go

fix_all_done:
	pop	ds
	pop	es
	pop	ax
	clc
	ret

theres_more_to_go:

; find and check next block

	mov	ax,ds
	inc	ax
	add	ax,DS:ga_size
	call	check_partition
	jc	fix_error_return
	mov	es,ax

; are both blocks free?

	cmp	ds:ga_owner,0
	jnz	fix_next
	cmp	es:ga_owner,0
	jz	both_blocks_are_free

fix_next:
	push	es
	pop	ds
	jmp	fix_loop

both_blocks_are_free:

; combine the blocks!

	mov	al,es:ga_sig
	mov	ds:ga_sig,al
	mov	ax,es:ga_size
	inc	ax
	add	ds:ga_size,ax
;	mov	es:ga_sig,0		; smash identifier
	jmp	fix_loop

fix_all_partitions	endp



;-----------------------------------------------------------------------;
; check_partition						        ;
; 								        ;
; Checks a record of the memory block list to see if its valid.	        ;
; 								        ;
; Arguments:							        ;
; 	AX = Partition						        ;
; Returns:							        ;
; 	CF = 0  if partition OK					        ;
; 	CF = 1  if partition not OK				        ;
; 	  AX = 7						        ;
; Alters:							        ;
; 	nothing							        ;
; Calls:							        ;
; 	nothing							        ;
; History:							        ;
; 								        ;
;  Mon Jun 10 10:53:15 1985    -by-    Charles Whitmer    [cxw]	        ;
; Wrote it.							        ;
;-----------------------------------------------------------------------;



check_partition	proc	near
	push	ds
	mov	ds,ax
	cmp	DS:ga_sig,'M'
	jz	partition_ok
	cmp	DS:ga_sig,'Z'
	jz	partition_ok
;	better_not_get_here	      ;????
	mov	ax,7		      ; error: Memory control blocks destroyed
	stc
	pop	ds
	ret

partition_ok:
	clc
	pop	ds
	ret
check_partition	endp

sEnd	CODE

	end

	PAGE	,132
		.286P

	TITLE	DSKCACHE - MS-DOS 5.0 disk drivers for IBMPC/AT
	NAME	DSKCACHE

;*****************************************************************************
;	MS-DOS 5.0 caching support for the hard disk code in the disk
;	driver. The file forms support for a caching facility that applies
;	to the hard disks in the system as part of the multi-tasking,
;	dual-mode disk device driver for DOS 5.0.
;
;	AUTHOR: Rajen J. Shah
;
;		Microsoft Corp.
;		Copyright (C) 1984
;
;*****************************************************************************

.xlist
	INCLUDE dosmac.inc		; Dos macros (<BREAK>, LJc)
	INCLUDE devhlp.inc		; Devhelp constants.
	INCLUDE abddrv.inc		; IoStruc and Request packet devs
	INCLUDE absegs.inc		; Code and Data segments
	.LFCOND 			; Show false conditionals
	.SALL				; Supress macro expansion
.list

	include atcache.inc		; Equates and structures for caching


;***********************************************************************
;
; Layout of the cache control segment is as follows
;
;   --------------------------------------------------
;   |	WORD   Offset to first buffer control struc  |
;   |	       (first_free_ptr) 		     |
;   --------------------------------------------------
;   |						     |
;   |						     |
;   |	Hash table entries			     |
;   |						     |
;   |						     |
;   --------------------------------------------------
;   |						     |
;   |						     |
;   |	Buffer control structures		     |
;   |						     |
;   |						     |
;   |						     |
;   |						     |
;   |						     |
;   |						     |
;   |						     |
;   |						     |
;   |						     |
;   --------------------------------------------------
;
;   (See the file dskcache.inc for the various structures).
;
;************************************************************************


	BREAK	<Data for disk driver>

BioData SEGMENT

	EXTRN	    DevHlpFunc:dword
	EXTRN	    Fixed:BYTE

	Cache_Seg	DD	0	    ; Physical address of cache memory
	CtrlStrucMemSiz DW	0	    ; Size of memory allocated for buffer
						; control information
; Define public so that they can be patched at INIT time to see the effect
; of different values.
Public CacheSizeL, CacheSizeH
	CacheSizeL	DW	0	    ; Low word for cache size in bytes
	CacheSizeH	DW	10H	    ; High word for cache size

Public NumHashTabEntries
	NumHashTabEntries   DW	32	    ; Number of buffer control linked lists
	NumBufInLList	EQU	4	    ; Starting buffer count per linked list
	NumSecInBuf	EQU	4	    ; Number of sectors cached in a buffer
	CutOffSecCnt	EQU	NumSecInBuf * 2 ; We do not cache READs if sector
						    ; count greater than this
	NumBufCntrl	EQU	NumBufInLList*Buf_Cntrl_Siz
	NumBytPerHash	EQU	Buf_Secsize*NumSecInBuf*NumBufInLList

Public HaveCache
	HaveCache	DB	0	    ; Non-zero if we have caching

BioData ENDS


Code	SEGMENT
    ASSUME	CS:BiosSeg,DS:BiosGroup,ES:NOTHING

; External routines
	EXTRN	PhysToVirtDSSI:NEAR
	EXTRN	PhysToVirtESDI:NEAR
	EXTRN	UN_PhysToVirt:NEAR


;****	CacheSector - Set up data structure for caching if necessary
;
;	Entry	DI -> BDS for the drive
;		SI -> Structure for the I/O
;
;	Exit	'C' if we have a cache hit
;		'C' clear if no cache hit
;
;	Uses	AX,BX,CX,DX,Flags
;
ASSUME	DS:BiosGroup,ES:NOTHING
Public CacheSector
CacheSector PROC
	Test	[HaveCache],-1
	ljz	csec9		    ; Caching is off
	and	[SI].Flags,NOT fCacheActive ; Zero out flag bit
	Test	[SI].Flags,fNoCache+fReset
	jz	csec1		    ; We are going to cache this one
csec0:
	jmp	csec10

csec1:
; Get a buffer for the caching operation
	mov	ax,word ptr [SI].First
	mov	dx,word ptr [SI].First+2
	mov	cl,[SI].PhysDrv
	CALL	GetABuffer	    ; AX:BX is physical address of a cache buffer
	test	[SI].Flags,fCacheActive
	jnz	short csec3	    ; We had a cache hit
	test	[SI].Flags,fNoCache
	jnz	csec0		    ; Do not cache if no buffer returned by GetABuffer
csec3:
	mov	word ptr [SI].CurrAddr,bx
	mov	word ptr [SI].XferAddr,bx
	mov	word ptr [SI].CurrAddr+2,ax
	mov	word ptr [SI].XferAddr+2,ax
; Get index into cache buffer for I/O to xfer area in request packet
	mov	dx,word ptr [SI].First
	ERRNZ <NumSecInBuf-4>	    ; Assume 4 sectors per buffer
	and	dx,3		    ; DX = sector index into buffer
	mov	[SI].XferSec,DL     ; Save sector index
; Convert new logical sector into Cylinder, Head, Sector
	test	[si].Flags,fWrite
	jnz	csec4		    ; Correctly set up by MapSector
	mov	ax,word ptr [SI].First
	mov	dx,word ptr [SI].First+2
	and	ax,NOT 3	    ; DX:AX = sector aligned on buffer boundary
	div	[DI].BPBtrksiz
	mov	[SI].Sector,dl
	xor	dx,dx
	div	[DI].BPBnhead
	mov	[SI].Head,DL
	mov	[SI].Cyl,AX
; Get minimum of sectors to read or sectors available in buffer
csec4:
	mov	ax,NumSecInBuf
	sub	al,[SI].XferSec
	cmp	ax,[SI].NumSectors
	jb	csec5		    ; Not whole buffer is needed
	mov	ax,[SI].NumSectors
csec5:
	mov	[SI].XferNumSec,ax
	test	[SI].Flags,fWrite
	jnz	csec6		    ; Only write out number of sectors requested
	mov	ax,NumSecInBuf
csec6:
	mov	[SI].NumSectors,ax
	mov	[SI].SecToGo,ax     ; Sectors to fill up buffer
	mov	ax,Buf_SecSize
; Set up transfer to be made between cache and request xfer area
	mul	[SI].XferNumSec
	mov	[SI].NumBytes,ax
	test	[SI].Flags,fWrite
	jz	csec7		    ; All done if it is not a WRITE
	mov	ax,Buf_SecSize
	xor	ch,ch
	mov	cl,[SI].XferSec
	mul	cx
	add	word ptr [si].CurrAddr,ax   ; Adjust transfer address for disk I/O
	adc	word ptr [si].CurrAddr+2,dx
csec7:
	jmp	csec21


;
; Caching is off. We turn on the fNoCache flag bit in Fixed.Flags
;
csec9:
	OR     Fixed.Flags,fNoCache
;
; We are not going to cache this portion of the request because there was an
; error encountered when we attempted it earlier. It may have been due to
; an attempt to I/O to a bad sector that is not part of the original request.
csec10:
; Set up data structure to handle I/O corresponding to the particular buffer
	mov	ax,word ptr [si].Addr
	mov	word ptr [si].CurrAddr,ax
	mov	ax,word ptr [si].Addr+2
	mov	word ptr [si].CurrAddr+2,ax
	mov	dx,word ptr [SI].First
	ERRNZ <NumSecInBuf-4>	    ; Assume 4 sectors per buffer
	and	dx,3		    ; dX = sector index into buffer
	mov	ax,NumSecInBuf
	sub	ax,dx		    ; ax = number of sectors to I/O for this buffer
	cmp	ax,[SI].NumSectors
	jb	csec12		    ; Not whole buffer is needed
	mov	ax,[SI].NumSectors
csec12:
	mov	[SI].XferNumSec,ax
	mov	[SI].NumSectors,ax
	mov	[SI].SecToGo,ax
; Set up correct transfer byte count based on sector size
	mov	ax,Buf_SecSize
	mul	[SI].XferNumSec
	mov	[SI].NumBytes,ax
csec15:
	clc
csec21:
	ret

CacheSector ENDP



;****	Make_MRU - Make the buffer the most recently used
;
;	Entry	DS:SI -> buffer control structure
;		DS points to the cache control segment
;
;	Exit	NONE
;
;	Uses	Flags
;
;
;	BUGBUG	This routine should also move the structure to the beginning
;		of the hash table linked list.
;
ASSUME	DS:NOTHING, ES:NOTHING
Public	Make_MRU
Procedure Make_MRU,NEAR
	SaveReg <DI,BX>
;
; Unlink the structure from the free list
;
	mov	di,[si].Prev_Free
	mov	bx,[si].Next_Free
	mov	[di].Next_Free,bx
	mov	[bx].Prev_Free,di
;
; Link at end of free list
;
	mov	di,word ptr ds:[first_free_ptr]
	mov	bx,[di].Prev_Free
	mov	[si].Next_Free,di
	mov	[si].Prev_Free,bx
	mov	[bx].Next_Free,si
	mov	[di].Prev_Free,si
;
; Unlink the structure from the hash linked list
;
	mov	di,[si].Prev_Ctrl
	mov	bx,[si].Next_Ctrl
	mov	[di].Next_Ctrl,bx
	or	bx,bx
	js	mm2			; We were at the end of the linked list
	mov	[bx].Prev_Ctrl,di
;
; Get the beginning of the hash linked list
;
mm2:
	mov	ax,word ptr [si].Buf_Sector
	mov	dx,word ptr [si].Buf_Sector+2
	CALL	GetHashEntry		; DS:DI -> Hash linked list
;
; Link it into hash linked list
;
	mov	bx,[di].Next_Ctrl
	mov	[si].Next_Ctrl,bx
	mov	[si].Prev_Ctrl,di
	mov	[di].Next_Ctrl,si
	or	bx,bx
	js	mm3			; We were at the end of the linked list
	mov	[bx].Prev_Ctrl,si
mm3:
	RestoreReg <BX,DI>
	ret
EndProc Make_MRU


;****	GetABuffer - Gets a buffer for the I/O
;
;	Entry	DX:AX logical sector for I/O
;		CL physical drive number for I/O
;		SI -> Structure for the I/O
;
;	Exit	AX:BX is the physical address of the buffer
;
;	Side Effects	Buffer is MRU
;			Buffer is set up in hash linked list
;			Buffer is marked as "in use"
;			Drive and logical sector used as signature for buffer
;			Fixed.Flags may have fNoCache bit set if no cache is to be used
;
;	Uses	Flags
;
ASSUME	DS:BiosGroup, ES:NOTHING
Public	GetABuffer
Procedure GetABuffer,NEAR
	SaveReg <DS,ES,DI,SI>
	PUSH	DS
	POP	ES
	SaveReg <ax,cx,dx>
	mov	ax,word ptr [Cache_Seg+2]
	mov	bx,word ptr [Cache_Seg]
	mov	cx,[CtrlStrucMemSiz]	; Size of control struc memory segment
	mov	si,SELECTOR3		;rel sel 3 in special pool for cache
	mov	dh,RESULTDSSI		;result to go in ds:si	(dh = 1)
	mov	dl,DevHlp_PhystoVirt
	Call	[DevHlpFunc]
;;	call	PhysToVirtDSSI
	RestoreReg <dx,cx,ax>		; Restore Sector # and drive #
ASSUME	DS:NOTHING, ES:BiosGroup
	CALL	Cache_Hit
	jc	gb3		    ; We have a cache hit
	test	Fixed.Flags,fWrite
	jnz	gb5		    ; Do not allocate buffer if no cache
					    ; hit for a WRITE operation
;
; In order to ensure that the buffer cache does not get overwritten before the
; data already read in is used up, we choose to not cache any READs that span
; two buffers or more. This effectively makes it a "read through" system for
; large transfers where there is no cache hit.
;
	mov	si,[CutOffSecCnt]
	cmp	Fixed.Count,si
	jge	gb5		    ; Do not cache READs that exceed limit
	CALL	GetFreeBuffer	    ; Clears carry
gb3:
	CALL	Make_MRU
	mov	ax,word ptr [si].Buffer_Ptr+2
	mov	bx,word ptr [si].Buffer_Ptr	; Return address of buffer
	jmp	short gb6

gb5:
	OR	Fixed.Flags,fNoCache	; We are not using the cache this time
gb6:						; Clears carry
	mov	dl,DevHlp_UnPhysToVirt
	call	es:[DevHlpFunc]
	RestoreReg <SI,DI,ES,DS>
	ret
EndProc GetABuffer


;****	GetFreeBuffer - Gets a buffer from the free list
;
;	Entry	DX:AX logical sector for I/O
;		CL physical drive number for I/O
;		DS points to the cache control segment
;		ES points to BiosGroup
;
;	Exit	DS:SI points to the buffer control structure
;
;	Uses	AX,BX,DI,SI,Flags
;
ASSUME	DS:NOTHING, ES:BiosGroup
Public	GetFreeBuffer
Procedure GetFreeBuffer,NEAR
	MOV	si,word ptr ds:[first_free_ptr]
	mov	bx,[si].Next_Free
	mov	word ptr ds:[first_free_ptr],bx ; Move free list pointer along
;
; Unlink structure from the hash linked list
;
	mov	di,[si].Prev_Ctrl
	mov	bx,[si].Next_Ctrl
	mov	[di].Next_Ctrl,bx
	or	bx,bx
	js	gfb5		    ; At end of list
	mov	[bx].Prev_Ctrl,di
;
;	Link into new hash linked list
;
gfb5:
	call	GetHashEntry	    ; DS:DI -> hash linked list, DX:AX = logical sector used
	mov	word ptr [si].Buf_Sector,ax
	mov	word ptr [si].Buf_Sector+2,dx
	mov	[si].Prev_Ctrl,di
	mov	bx,[di].Next_Ctrl
	mov	[si].Next_Ctrl,bx
	mov	[di].Next_Ctrl,si
	or	bx,bx
	js	gfb10		    ; We were inserting into an empty list
	mov	[bx].Prev_Ctrl,si
gfb10:
	mov	[si].Buf_Drive,cl
	or	[si].Buf_Flags,fBufUsed     ; Clears carry
	ret
EndProc GetFreeBuffer


;****	Cache_Hit - Determine if we have a cache hit or not
;
;	Entry	DX:AX logical sector for I/O
;		CL physical drive number for I/O
;		DS:SI -> Hash table in buffer control segment
;		ES = BiosGroup
;
;	Exit
;		If 'C' set, DS:SI points to the buffer control structure
;
;	Uses	DI,SI,Flags
;
ASSUME	DS:NOTHING, ES:BiosGroup
Public Cache_Hit
Procedure Cache_Hit,NEAR
	SaveReg <ax,dx>
	call	GetHashEntry		; DS:DI -> hash linked list, DX:AX = sector number used for hashing
ch2:
	cmp	[di].Next_Ctrl,-1
	jz	ch4			; We are at end of linked list
	mov	di,[di].Next_Ctrl
	test	[di].Buf_Flags,fBufUsed
	jz	ch2			; Buffer is unused
	cmp	word ptr [di].Buf_Sector,ax
	jnz	ch2			; No hit. Try next buffer in link.
	cmp	word ptr [di].Buf_Sector+2,dx
	jnz	ch2			; No hit. Try next buffer in link.
	cmp	[di].Buf_Drive,cl
	jnz	ch2			; No hit. Try next buffer in link.
	xchg	di,si			; DS:SI -> buffer control structure
	or	Fixed.Flags,fCacheActive    ; Say we have a cache active
	stc
	jmp	short ch5
ch4:
	clc
ch5:
	RestoreReg <dx,ax>
	ret
EndProc Cache_Hit


;****	GetHashEntry - Get the beginning of the hash linked list
;
;	Entry	DX:AX logical sector for I/O
;		DS points to cache control segment
;
;	Exit	DS:DI -> beginning of hash linked list
;		DX:AX	 sector number used, aligned along buffer lines
;
;	Uses	Flags
;
ASSUME	DS:NOTHING, ES:NOTHING
Public GetHashEntry
Procedure GetHashEntry,NEAR
	SaveReg <cx>
	ERRNZ <NumSecInBuf-4>	; Assume 4 sectors per buffer
	and	ax,NOT 3	; AX = sector number to use to get hash index
	push	ax
	push	dx
	mov	cx,ds:[size_hash_tab]
	dec	cx		; CX = divisor to get hash index
	div	cx		; DX = index into hash table
	xchg	ax,dx
	mov	cl,HashSiz
	mul	cl
	add	ax,free_list_siz; Add in size of [first_free_ptr] variable
	mov	di,ax		; DS:DI -> beginning of linked list
	pop	dx
	pop	ax		; DX:AX = sector used for determining hash index
	RestoreReg <cx>
	ret
EndProc GetHashEntry


;****	FreeCacheBuf - Free the buffer in the cache
;
;	Entry	DS:DI -> Fixed (data structure)
;		DS = BiosGroup
;
;	Exit	NONE
;
;	Uses	ES,AX,BX,CX,DX,DI,SI,Flags
;
ASSUME	DS:BiosGroup, ES:NOTHING
Public FreeCacheBuf
Procedure FreeCacheBuf, NEAR
;	PUSH	DS
	PUSH	DS
	PUSH	DS
	POP	ES
	mov	ax,word ptr [Cache_Seg+2]
	mov	bx,word ptr [Cache_Seg]
	mov	cx,[CtrlStrucMemSiz]	; Size of control struc memory segment
	mov	si,SELECTOR3		;rel sel 3 in special pool for cache
	mov	dh,RESULTDSSI		;result to go in ds:si	(dh = 1)
	mov	dl,DevHlp_PhystoVirt
	Call	[DevHlpFunc]
ASSUME	DS:NOTHING, ES:BiosGroup
	mov	ax,word ptr es:[DI].First
	mov	dx,word ptr es:[DI].First+2
	mov	cl,es:[DI].PhysDrv
	CALL	Cache_Hit		    ; DS:SI -> the buffer
	jnc	fcbuf10 		    ; Buffer not present in cache
	AND	[SI].Buf_Flags,NOT fBufUsed ; Mark buffer as unused
fcbuf10:
;	POP	DS
	mov	dl,DevHlp_UnPhysToVirt
	call	es:[DevHlpFunc]
;	call	UN_PhysToVirt
	POP	DS
	ret
EndProc FreeCacheBuf

Code ENDS

CodeInit segment

;****	Cache_Init - Initialise the system cache structures
;
;	Entry	es:bx = pointer to data packet containing
;		      -allocated cache memory size (4 bytes)
;		      -allocated cache memory physical address (4 bytes)
;		      -number of GDT descriptors allocated (1 byte)
;
;	Exit	'C' set if error
;
;	Uses	Flags
;
ASSUME DS:BiosGroup, ES:BiosGroup

CacheInit_Input     struc
  CA_Size	 dd    ?	       ;allocated cache memory size
  CA_BegAddr	 dd    ?	       ;cache memory starting physical address
  CA_GDTs	 db    ?	       ;number of GDT descriptors allocated
  CA_MsgID	 dw    ?	       ;cache message ID, 0 or msg id number
CacheInit_Input     ends

Public	Cache_Init
Procedure Cache_Init,NEAR

LocalVar    BufferLow,WORD
LocalVar    BufferHigh,WORD
LocalVar    BufCntrl,WORD
LocalVar    Previous_Free,WORD
LocalVar    Previous_Ctrl,WORD

	EnterProc		    ; Initialize stack frame
;;	int 3
	PUSHA
	PUSH	ES
	mov	ax,word ptr ES:[BX].CA_BegAddr
	mov	dx,word ptr ES:[BX].CA_BegAddr+2
	mov	word ptr [Cache_Seg],ax
	mov	word ptr [Cache_Seg]+2,dx   ; Save address of cache segment
	mov	ax,word ptr ES:[BX].CA_Size
	mov	dx,word ptr ES:[BX].CA_Size+2
	or	ax,ax
	jnz	ci2		    ; There is a cache allocated
	or	dx,dx
	ljz	ci10		    ; No cache allocated
ci2:
	mov	CacheSizeL,ax
	mov	CacheSizeH,dx	    ; Save cache size away
	CALL	SetUpCacheParms
;
; Set up other pointers to the cache areas
; [first_free_ptr] points to the first buffer control structure
;
	mov	[Previous_Free],-1
;
; Now proceed to initialize each linked list of buffers
; Set up DS to access the buffer control structure segment
;
	MOV	ax,word ptr [Cache_Seg+2]
	MOV	bx,word ptr [Cache_Seg]
	mov	cx,[CtrlStrucMemSiz]
	call	PhysToVirtESDI	    ; ES:DI -> cache control structure
	OR	DI,DI
;
; FAIL if the offset is not zero. The following code will not work correctly
;
	LJNZ	ci9

ASSUME	ES:NOTHING
	mov	cx,[BufCntrl]
	mov	word ptr es:[first_free_ptr],cx
	mov	ax,NumSecInBuf
	mov	word ptr es:[num_sec_in_buf],ax
	mov	ax,NumHashTabEntries
	mov	word ptr es:[size_hash_tab],ax
	mov	cx,NumHashTabEntries
L1:
	push	cx
	mov	ax,NumHashTabEntries
	sub	ax,cx		    ; AX = index into hash table
	mov	cx,HashSiz
	mul	cx		    ; AX = AX * size of hash table entry
	add	ax,free_list_siz    ; Add in size of [first_free_ptr] variable
	mov	si,ax		    ; SI offsets into hash table
	mov	di,[BufCntrl]	    ; DI -> buffer control structure
	mov	es:[si].Cntrl_Struc_Off,di
	mov	es:[si].Dummy,-1
	mov	Previous_Ctrl,si
; Now set up each control structure in the linked list
	mov	cx,NumBufInLList
L2:
	push	cx
	mov	ax,di
	add	ax,Buf_Cntrl_Siz
	mov	es:[di].Next_Ctrl,ax   ; Next entry in hash linked list
	mov	es:[di].Next_Free,ax   ; Next entry in free list
	mov	ax,[Previous_Ctrl]  ; Back link for control strucs
	mov	es:[di].Prev_Ctrl,ax
	mov	ax,[Previous_Free]  ; Back link for free list
	mov	es:[di].Prev_Free,ax
	mov	ax,[BufferHigh]
	mov	bx,[BufferLow]
	mov	word ptr es:[di].Buffer_Ptr,bx
	mov	word ptr es:[di].Buffer_Ptr+2,ax
	push	ax			    ; Save high word of buffer address
	mov	ax,NumSecInBuf
	mov	cx,Buf_SecSize
	mul	cx			    ; AX = Size of buffer
	add	bx,ax
	pop	ax
	adc	ax,0			    ; AX:BX is address of next buffer
	mov	[BufferHigh],ax
	mov	[BufferLow],bx
	mov	es:[di].Buf_Flags,0
	mov	[Previous_Ctrl],di
	mov	[Previous_Free],di
	mov	di,es:[di].Next_Ctrl
	pop	cx
	loop	L2

; Set up end of hash linked list correctly
	mov	[BufCntrl],di		    ; Set up next available structure
	mov	di,[Previous_Ctrl]	    ; Get last structure in linked list
	mov	es:[di].Next_Ctrl,-1
	pop	cx
	LOOP	L1

; Set up end of free linked list correctly
	mov	si,word ptr es:[first_free_ptr]
	mov	es:[si].Prev_Free,di
	mov	es:[di].Next_Free,si
; Set flag that caching is enabled
	mov	[HaveCache],1

ci9:
	Call	UN_PhysToVirt
ci10:
	POP	ES
	POPA
	LeaveProc
	ret

EndProc Cache_Init


;****	SetUpCacheParms - Set up cache parameters from cache size
;
;	Entry	DX:AX = cache size in bytes
;		ES:BX = parameter packet with cache information
;
;	Exit	The following data are set up:
;		    - NumHashTabEntries
;		    - BufferHigh
;		    - BufferLow = address of buffers
;		    - BufCntrl = offset of control structures in segment
;		    - CtrlStrucMemSize = size of memory needed for control strucs
;
;		BufferHigh and BufferLow set to the address of the buffers.
;
;	Uses	AX,CX,DX,Flags
;
ASSUME DS:BiosGroup, ES:BiosGroup
Procedure SetUpCacheParms,NEAR
	sub	ax,size free_list_entry
	sbb	dx,0			; DX:AX = cache size - free list entry
	mov	cx,HashSiz+NumBufCntrl+NumBytPerHash
	div	cx			; AX = number of hash table entries
	mov	[NumHashTabEntries],ax
	push	ax
	mov	cl,HashSiz
	mul	cl
	add	ax,free_list_siz
	mov	[BufCntrl],ax		; AX = offset of first control struc in segment
	pop	ax			; Number of hash table entries
	mov	cl,NumBufCntrl
	mul	cl
	add	ax,[BufCntrl]		; AX = size of cache for control structures
	mov	[CtrlStrucMemSiz],ax
	add	ax,[BufCntrl]
	mov	dx,word ptr [Cache_Seg]+2
	add	ax,word ptr [Cache_Seg]
	adc	dx,0			; DX:AX = start of buffers
	mov	[BufferLow],ax
	mov	[BufferHigh],dx
	ret
EndProc SetUpCacheParms

CodeInit ENDS

END

;
; disk.asm - low level disk primitives used by Format and Sys procedures
;

.xlist
include cmacros.inc
if1			; This module also used by SETUP
ifndef ?SETUP
    ?SETUP1 = 0
else
    ?SETUP1 = 1
endif
endif
include disk.inc
include file.inc
.list

	public	__acrtused
__acrtused = 1

sBegin	DATA
globalW curFATsec,-1
globalB curFATmode,0
externB _DPB
externB _bpbList
externW _cCluster
sEnd	DATA

sBegin	CODE
assumes CS,CODE
assumes DS,DATA

;
; SetDASD
;
cProc	SetDASD,<PUBLIC,NEAR>
	parmW	drive
	parmB	dasdvalue
cBegin
	mov	ah,17h
	mov	al,dasdvalue
	mov	dx,drive
	int	13h
cEnd


; GetDBT
cProc	GetDBT,<PUBLIC,NEAR>
cBegin	nogen
	push	ds
	xor	ax,ax
	mov	ds,ax
	mov	ax,ds:[78h]	; low word of lp to DBT
	mov	dx,ds:[7ah]	; high word of lp to DBT
	pop	ds
	ret
cEnd	nogen




;
; GetVersionNumber - returns the dos version number, bytes swapped
;		      i.e 3.2 will return 314h
;
cProc	GetVersionNumber,<PUBLIC,NEAR>
cBegin	nogen
	mov	ah,30h
	int	21h
	xchg	ah,al
	ret
cEnd	nogen

;
; GetDeviceParameters - fills the buffer pointed to by pDPB
;		    with the device parameters
;
cProc	GetDeviceParameters,<PUBLIC,NEAR>
	parmW	drive
	parmW	pDPB
cBegin
	mov	dx,pDPB
	mov	ax,440dh	; ioctl, generic ioctl
	mov	bx,drive
	inc	bx
	mov	cx,860h 	; (RAWIO shl 8) or GET_DEVICE_PARAMETERS
	int	21H
cEnd


;
; SetDeviceParameters - fills the buffer pointed to by pDPB
;		    with the device parameters
;
cProc	SetDeviceParameters,<PUBLIC,NEAR>
	parmW	drive
	parmW	pDPB
cBegin
	mov	dx,pDPB
	mov	ax,440dh	; ioctl, generic ioctl
	mov	bx,drive
	inc	bx
	mov	cx,840h 	;
	int	21H
cEnd


;
; GetCntFloppyDrives - Returns the number of floppies attached.
;
;   Inputs:	    none
;   Outputs:	    returns INT 11 version of number of drives attached
;   Side Effects:   none
;
cProc	GetCntFloppyDrives,<PUBLIC,NEAR>
cBegin	nogen
	int	11h
; Ignore low order bit as some "compatables" dont set it
	rol	al,1
	rol	al,1
	and	ax,3
	inc	ax
	ret
cEnd	nogen


; NormalizeDrive converts a 1-based drive into a 0-based drive.
;
;  Inputs:     drive   1-based (0=default) drive
;  Returns:    0-based drive (0=A)
;  Side effects:       none
;
cProc	NormalizeDrive,<PUBLIC,NEAR>
    parmB   drive
cBegin
    xor     ax,ax
    or	    al,drive
    jnz     normdone
    mov     ah,19h
    int     21h
    inc     ax
normdone:
    dec     ax
cEnd


;
; GetCurDisk() - returns the current disk drive number
;
;   Inputs:	none.
;   Returns:	0 = A:, 1 = B:, etc.
;
cProc	GetCurDisk,<PUBLIC,NEAR>
cBegin	nogen
	mov	ah,19h
	int	21h
	xor	ah,ah
	ret
cEnd	nogen


;
; SetCurDisk( dDisk ) - returns the current disk drive number
;
;   Inputs:	0-based drive number, 0 = A:, 1 = B:, etc.
;   Returns:	#logical drives
;
cProc	SetCurDisk,<PUBLIC,NEAR>
    parmB   dDisk
cBegin
	mov	dl,dDisk
	mov	ah,0Eh
	int	21h
	xor	ah,ah
cEnd


; GetClusterSize( drive ) - returns the size of a cluster, in bytes,
; for the specified drive.
;
;  Inputs:     drive   1-based
;  Returns:    0 if unsuccessful, or the number of bytes in a cluster
;  Side effects:    issues the getfreespace system call
;
cProc	GetClusterSize,<PUBLIC,NEAR>
    parmB   drive
cBegin
    mov     ah,36h
    mov     dl,drive
    int     21h
    inc     ax
    jz	    gcsdone
    dec     ax
    mul     cx
gcsdone:
cEnd


;
; DiskReset () flushes out the buffer cache.
;
;   Inputs:	none.
;   Returns:	none
;   Side effects:	DOS buffer cache is flushed and invalidated.

cProc	DiskReset,<PUBLIC,NEAR>
cBegin	nogen
	mov	ah,0Dh
	int	21h
	ret
cEnd	nogen


;
; SetDPB (drive, pBPB, pDPB) fakes up a DPB from the BPB for use by the DOS.
;
;   Inputs:	drive	1-based drive number
;		pBPB	pointer to source BPB
;		pDPB	pointer to destination
;   Returns:	nothing
;   Side effects:	none
;

cProc	SetDPB,<PUBLIC,NEAR>,<SI>
    ParmB   drive
    ParmW   pBPB
    ParmW   pDPB
cBegin
	mov	si,pBPB 		; point to BPB
	push	ds
	pop	es
	mov	cl,drive		; grab this before we stomp BP
	push	bp
	mov	bp,pDPB
	mov	byte ptr es:[bp],cl	; ES:BP = pDPB
	mov	ah,53h
	int	21h
	push	ax

	; Intensely stupid DOS 4.0 hack -
	; IBM has changed the "FAT_size" field in the DPB structure from a
	; byte to a word in DOS 4.0 causing us to misread any variables in
	; the structure after this field.  This super-sleazy hack rep moves
	; the rest of the structure up one byte, effectively converting "FAT_
	; size" back into a byte.  ChipA 22-Jun-1988

	call	GetVersionNumber	; DOS 4.0 or greater?
	cmp	ax,0400h
	jb	SDExit

	push	es
	pop	ds
	mov	si,bp			; Source at DPB.dpb_dir_sector
	add	si,17
	mov	di,si			; Destination up one
	dec	di
	mov	cx,16
	rep movsb

SDExit: pop	ax
	pop	bp
cEnd

;
; GetDPB (drive, pDPB) returns a copy of the DOS dpb for the specified drive
;
;   Inputs:	drive	1-based drive number
;		pDPB	DS pointer to destination
;   Returns:	0	if successful
;		<> 0	Otherwise
;   Side effects:	none
;
cProc	GetDPB,<PUBLIC,NEAR>,<DS,SI,DI>
    ParmB   drive
    ParmW   pDPB
cBegin
	mov	cx,ds
	mov	ah,32h			; Get_DPB system call
	mov	dl,drive
	int	21h

	cbw				; extend error in AL to AX
	or	ax,ax			; any error at all?
	jnz	gdpbdone		; yes, return immediately
	mov	si,bx
	mov	es,cx
	mov	di,pDPB
	mov	cx,SIZE DPB		; number of bytes
	cld
	rep	movsb

	; Intensely stupid DOS 4.0 hack -
	; IBM has changed the "FAT_size" field in the DPB structure from a
	; byte to a word in DOS 4.0 causing us to misread any variables in
	; the structure after this field.  This super-sleazy hack rep moves
	; the rest of the structure up one byte, effectively converting "FAT_
	; size" back into a byte.  ChipA 22-Jun-1988

	push	ax			; Preserve AX

	call	GetVersionNumber	; DOS 4.0 or greater?
	cmp	ax,0400h
	jb	gdpbdone

	push	es			; DS=ES
	pop	ds
	mov	si,pDPB 		; Source at DPB.dpb_dir_sector
	add	si,17
	mov	di,si			; Destination up one
	dec	di
	mov	cx,16
	rep movsb

gdpbdone:
	pop	ax
cEnd

;
; int25 (drive, buffer, count, sector) performs a direct sector read from the
; specified drive and sector for the specified count into the specified buffer.
;
;   Inputs:	drive	0-based drive number
;		buffer	long address of buffer
;		count	number of sectors to transfer
;		sector	beginning logical sector number
;   Returns:	0	if successful
;		-1	Otherwise
;   Side effects:	none

cProc	int25,<PUBLIC,NEAR>,<DS,SI,DI>
    ParmB   drive
    ParmD   buffer
    ParmW   count
    ParmW   sector
cBegin
	mov	al,drive
	lds	bx,buffer
	mov	cx,count
	mov	dx,sector
	push	bp
	int	25h
	pop	bx			; remove flags on stack
	pop	bp
	mov	dx,ax
	mov	ax,0			; initial return code is 0
	sbb	ax,0			; if carry set, return is now -1
cEnd

;
; int26 (drive, buffer, count, sector) performs a direct sector write to the
; specified drive and sector for the specified count from the specified buffer.
;
;   Inputs:	drive	0-based drive number
;		buffer	long address of buffer
;		count	number of sectors to transfer
;		sector	beginning logical sector number
;   Returns:	0	if successful
;		-1	Otherwise
;   Side effects:	none

cProc	int26,<PUBLIC,NEAR>,<DS,SI,DI>
    ParmB   drive
    ParmD   buffer
    ParmW   count
    ParmW   sector
cBegin
	mov	al,drive
	lds	bx,buffer
	mov	cx,count
	mov	dx,sector
	push	bp
	int	26h
	pop	bx			; remove flags on stack
	pop	bp
	mov	dx,ax
	mov	ax,0			; initial return code is 0
	sbb	ax,0			; if carry set, return is now -1
cEnd

;
; FormatTrackHead (drive, track, head, pTrack) formats and verifies a track.
; If either format or verify returns an error, FormatTrackHead gives up.  The
; track table is generated in the passed buffer.
;
;   Inputs:	drive	    0-based drive number for formatting
;		track	    number of track to be formatted
;		head	    head number of track to be formatted
;		cSec	    number of sectors on track
;		pTrack	    long pointer to available track buffer
;   Outputs:	0	    if format/verify succeeded
;		<> 0	    format or verify failed.
;   Side Effects:	    contents at pTrack are destroyed
;

cProc	FormatTrackHead,<PUBLIC,NEAR>,<DS,SI,DI>
    ParmB   drive
    ParmB   track
    ParmB   head
    ParmB   cSec
    ParmD   pTrack
    LocalD  pTrackTbl
    LocalB  dosver
    LocalB  retry
cBegin
	les	di,pTrack
	cld
	xor	cx,cx
	mov	cl,cSec

	push	ds			; Handle bogus BIOS code
	push	cx
	mov	ax,60h			; Save what is at 60:0
	mov	ds,ax
	xor	si,si
	shl	cx,1			; 2 words per sector
	rep	movsw
	pop	cx
	pop	ds
	xor	di,di			; Now build track table at 60:0
	mov	es,ax			; so int 13h DMA boundary check
					; logic will be happy
TrackTblBuild:
	mov	OFF_pTrackTbl,di
	mov	SEG_pTrackTbl,es
	mov	bx,0200h
	mov	al,byte ptr track
	mov	ah,byte ptr head
TrackBuildLoop:
	stosw				; store C,H
	inc	bl
	xchg	ax,bx
	stosw				; store R,N
	xchg	ax,bx
	loop	TrackBuildLoop

; Format the track, retrying 4 times
;
	mov	retry,4
FormatTrack:
	mov	ah,05
	mov	al,cSec
	les	bx,pTrackTbl
	mov	ch,byte ptr track
	mov	cl,1
	mov	dl,drive
	mov	dh,head

	int	13h

	mov	ax,0
	jc	DecRetry
;
; verify the track
;
	mov	ah,4
	mov	al,cSec
	les	bx,pTrackTbl
	mov	ch,byte ptr track
	mov	cl,1
	mov	dl,drive
	mov	dh,head

	int	13h

	mov	ax,0
	jnc	done
DecRetry:
	dec	retry
	jnz	FormatTrack
	mov	ax,-1
Done:
;
; Restore the contents of 60:0
;
	lds	si,pTrack
	les	di,pTrackTbl
	xor	cx,cx
	mov	cl,cSec
	shl	cx,1
	rep	movsw
Done1:
cEnd


;
; GetDriveType (drive) returns the type of drive supported by the ROM.
;
;   Inputs:	drive	    0-based drive number for int 13
;   Outputs:	-1	    if INT 13 returns carry
;		0	    if changeline not supported
;		1	    if changeline supported
;		2	    if wacko returns from int 13
;   Side Effects:	    if changeline supported, set dasdtype for drive
;			    to 1.3 meg in 1.3 meg drive
;

cProc	GetDriveType,<PUBLIC,NEAR>
    ParmB   drive
cBegin
	mov	dl,drive
	mov	ax,(15h shl 8) + 0FFh
	int	13h
;
; Stupid IBM has documented INCORRECTLY the readdasdtype call to INT 13.  The
; conditions at this point are:
;
;   Carry flag unknown
;   ah = 00	iff no diskette present 	=> 2 (wacko)
;   ah = 01	iff no changeline available	=> 0
;   ah = 02	iff changeline available	=> 1
;   ah = 03	iff hard disk			=> 2 (wacko)
;   ah = 15	iff not on AT			=> 0
;
	cmp	ah,1
	jb	wacko
	jz	zero
	cmp	ah,3
	jae	wacko
;
; Changeline present.  Put drive into hidensity mode.
;
GetDriveCall:
	mov	ah,17h
	mov	dl,drive
	mov	al,3
	int	13h
;
; Doubly stupid IBM spins the drive for this call and potentially returns the
; disk change error (mumble).  If we get carry set AND ah == 6, we retry the
; operation
;
	jnc	NoCarry
	cmp	ah,06h
	jnz	wacko
	jmp	GetDriveCall
NoCarry:
	mov	ax,1
	jmp	short doneDrive
wacko:
	mov	ax,2
	jmp	short doneDrive
zero:
	mov	ax,0
	jmp	short doneDrive
carry:
	mov	ax,-1
doneDrive:
cEnd

cProc	ReadVolumeID,<PUBLIC,NEAR>,<si,di>
    parmW   drive
    parmW   pBuf
    localV  fcb1,<SIZE FCB + SIZE XFCB>
    localV  fcb2,<SIZE FCB + SIZE XFCB>
cBegin
	push	ds
	push	ss
	pop	ds
	lea	di,fcb1
	mov	dx,di
	lea	di,[di].SIZE XFCB.FCB_name
	mov	byte ptr [di],0
	push	di
	mov	ah,1ah
	int	21h

	cCall	NormalizeDrive,<drive>
	push	ss
	pop	es
	lea	di,fcb2
	mov	dx,di
	mov	es:[di].XFCB_flag,0FFh
	mov	es:[di].XFCB_attr,A_VID
	add	di,SIZE XFCB
	cld
	errnz	<FCB_drive>
	inc	al
	stosb
	mov	al,'?'
	xor	cx,cx
	mov	cl,11
	rep	stosb

	mov	ah,11h
	int	21h

	pop	si
	mov	di,pBuf
	pop	es
	cld
	xor	bx,bx
	mov	cx,11
rvidloop:
	lodsb
	cmp	al,' '
	je	rvidnext
	mov	bx,di
rvidnext:
	stosb
	or	al,al
	jz	rviddone
	loop	rvidloop
	xor	ax,ax
	stosb
	or	bx,bx
	jz	rviddone
	lea	di,[bx+1]
	stosb
rviddone:
	push	es
	pop	ds
	mov	ax,di
	sub	ax,pBuf
	dec	ax
cEnd

cProc	WriteVolumeID,<PUBLIC,NEAR>,<si,di>
    parmW   pDPB
    parmW   dDst
    parmD   lpVID
    parmD   lpBuf
cBegin
	mov	bx,pDPB
	mov	ax,1
	cCall	int25,<dDst,lpBuf,ax,[bx].DPB_dir_sector>
	jnz	wviddone
	mov	al,0E5h
	errnz	<DIR_name>
	cld
	les	di,lpBuf		    ; Point to 1st directory entry
	stosb				    ; Mark it free
	add	di,SIZE DIR - 1 	    ; Point to 2nd directory entry
	stosb				    ; Mark it free
	add	di,SIZE DIR - 1 	    ; Point to 3rd directory entry
	push	ds
	lds	si,lpVID		    ; Copy volume name into it
	mov	cx,11
wvidloop:
	lodsb
	or	al,al
	jz	wvid1
	stosb
	loop	wvidloop
wvid1:
	jcxz	wvid2
	mov	al,' '                      ; Pad with blanks
	rep	stosb
wvid2:
	pop	ds
	mov	al,A_VID
	stosb				    ; Store volume ID attribute byte
	add	di,10			    ; skip padding
	mov	ah,2Ch
	int	21h			    ; Get the current time
	mov	ax,cx
	shl	al,1
	shl	al,1
	mov	cl,3
	shl	ax,cl
	or	al,dh
	stosw				    ; Store packed time

	mov	ah,2Ah			    ; Get current date
	int	21h
	sub	cx,1980 		    ; normalize year
	mov	ah,cl
	mov	cl,4
	shl	dh,cl
	mov	al,dh
	shl	ax,1
	or	al,dl
	stosw				    ; Store packed date

	xor	ax,ax
	stosw				    ; Zero first cluster number
	stosw				    ; ...and size
	stosw

	mov	bx,pDPB
	inc	ax
	cCall	int26,<dDst,lpBuf,ax,[bx].DPB_dir_sector>
wviddone:
cEnd

; WriteBootSec (dSrc, pDPB, lpBuf) copies the boot sector from one drive
; to another adjusting for differing media types
;
;  Inputs:	dSrc	    0-based drive of source boot sector
;		dDPB	    -> DPB of destination
;		pBPB	    pointer to BPB for destination drive
;			    (if zero then guesses which BPB to use)
;		lpBuf	    long pointer to buffer large enough to
;			    contain boot sector.
;  Returns:	0	    if successful
;		-1	    if unsuccessful
;
cProc	WriteBootSec,<PUBLIC,NEAR>,<si,di>
    parmW   dSrc
    parmW   dDst
    parmW   pBPB
    parmD   lpBuf
cBegin
	; Read boot sector from source drive
	xor	bx,bx
	lea	ax,[bx+1]
	cCall	int25,<dSrc,lpBuf,ax,bx>
	jnz	bsdone

	mov	si,pBPB
	or	si,si
	jnz	havebpb
	mov	ah,36h
	mov	dx,dDst
	inc	dl
	int	21h
	inc	ax
	jz	bsfail
	mov	si,dataOffset _bpbList	; Scan BPB list for match
	mov	bx,dataOffset _cCluster ; with associated cluster size
bsloop:
	mov	cx,word ptr [bx]
	jcxz	bsfail
	cmp	cx,dx
	je	havebpb
	add	si,SIZE BPB
	inc	bx
	inc	bx
	jmp	bsloop

bsfail:
	mov	ax,-1
	jmp	short bsdone

havebpb:
	; Copy BPB appropriate for dest. drive into boot sector
	les	di,lpBuf
	lea	di,[di].BOOT_BPB
	mov	cx,SIZE BPB
	cld
	rep	movsb

	; Store a zero in BOOT_bootdrive field
	; (i.e. always assume A: is the boot drive
	; so IBMBIO.COM will be happy)
	xor	ax,ax
	mov	di,OFF_lpBuf
	mov	es:[di].BOOT_phydrv,al	       ; 3.2 expects it here
	mov	es:[di].BOOT_bootdrive,al      ; < 3.2 expects it here

	mov	bx,1
	cCall	int26,<dDst,lpBuf,bx,ax>    ; Output correct boot sector
bsdone:
cEnd

; SetIndexSector - determine sector and offset-within-sector for a particular
; cluster.
;
; Inputs:   DI = -> DPB block
;	    CX = clustor number
;
; Outputs:  AX = sector number of fat sector containing fat entry
;	    DX = offset into fat sector of 1st bytes of fat entry for passed
;		 cluster number.
;
;
cProc	SetIndexSector,<PUBLIC,NEAR>
cBegin	nogen
	xor	dx,dx
	mov	ax,cx
	cmp	[di].DPB_max_cluster,4096-10	; 12 bit fat entries?
	jb	sis12bit		; Yes, go calculate sector/offset

; offset = (2 * cluster) % pDPB->sector_size
; sector = pDPB->first_FAT + (2 * cluster) / pDPB->sector_size
	shl	ax,1
	div	[di].DPB_sector_size	; No, 16 bit fat entries
	jmp	short sisdone
sis12bit:
; offset = (cluster + (cluster >> 1)) % pDPB->sector_size
; sector = pDPB->first_FAT + (cluster + (cluster >> 1)) / pDPB->sector_size
	shr	ax,1
	add	ax,cx
	div	[di].DPB_sector_size
sisdone:
	add	ax,[di].DPB_first_FAT
	ret
cEnd	nogen

cProc	OpenFAT,<PUBLIC,NEAR>
    parmB mode
cBegin
	mov	[curFATsec],-1
	mov	al,mode
	and	al,FAT_READ+FAT_WRITE
	mov	[curFATmode],al
cEnd

cProc	FlushFAT,<PUBLIC,NEAR>,<si,di>
    parmW   pDPB
    parmD   lpBuf
    localB  dosver
cBegin
	mov	ax,[curFATsec]
	inc	ax
	jz	ffdone
	test	[curFATmode],FAT_WRITE
	jz	ffdone
	mov	ah,30h
	int	21h
	mov	dosver,al
	mov	si,[curFATsec]
	mov	bx,pDPB
	xor	dx,dx
	mov	dl,[bx].DPB_drive
	mov	di,dx			; DI = drive number
	mov	dl,2			; DL = #sectors to write
	push	bx
	cCall	int26,<di,lpBuf,dx,si>
	pop	bx
	jnz	ffdone
	push	bx
	cmp	si,1			; Did we overwrite the media byte
	jne	ff2nd
	cmp	dosver,3		; Yes, is this DOS 3.X?
	jb	ff2nd			; Then no door lock support

	mov	cx,ds			; Yes, then mark media as invalid
	mov	ah,32h			; Get_DPB system call
	mov	dx,di
	inc	dx			; Get_DPB needs 1-based drive number
	int	21h
	push	ds
	pop	es
	mov	ds,cx
	cbw				; extend error in AL to AX
	or	ax,ax			; any error at all?
	jnz	ff2nd			; yes, return immediately
	mov	es:[bx].DPB_first_access,-1
ff2nd:
	pop	bx
	xor	dx,dx
	mov	dl,[bx].DPB_FAT_size
	add	si,dx
	mov	dl,2
	cCall	int26,<di,lpBuf,dx,si>
ffdone:
cEnd

; UnpackFAT returns the contents of a FAT entry.
;
; Inputs:     pDPB	  drive parameter block for drive
;	      lpBuf	  pointer to a sector buffer
;	      cluster	  cluster whose FAT contents are to be returned
; Returns:    -1	  if disk error or if contents >= pDPB->max_cluster
;	      xxx	  contents of cluster
; Side effects: 	  reads contents of FAT into lpBuf
;
cProc	UnpackFAT,<PUBLIC,NEAR>,<si,di>
    parmW   pDPB
    parmD   lpBuf
    parmW   cluster
cBegin
	mov	di,pDPB
	mov	cx,cluster
	cCall	SetIndexSector
	mov	si,dx
	cmp	[curFATsec],ax
	je	upfnoread
	push	ax
	cCall	FlushFAT,<di,lpBuf>
	pop	cx
	jnz	upfdone
	mov	al,[di].DPB_drive
	mov	dx,2
	push	cx
	cCall	int25,<ax,lpBuf,dx,cx>
	pop	cx
	jnz	upfdone
	mov	[curFATsec],cx
upfnoread:
	les	bx,lpBuf
	mov	ax,es:[bx+si]
	cmp	[di].DPB_max_cluster,4096-10	; 12 bit fat entries?
	jae	upfdone

	test	byte ptr cluster,1
	jz	upf1
	mov	cl,4
	shr	ax,cl
upf1:
	and	ax,0FFFh
upfdone:
	mov	dx,ax
	cmp	[di].DPB_max_cluster,ax
	jae	upfexit
	mov	ax,-1
upfexit:
cEnd

; PackFAT stuffs a particular value into a FAT entry
;
; Inputs:     pDPB	  pointer to DPB
;	      lpBuf	  pointer to a 2 sector area
;	      cluster	  cluster to be modified
;	      value	  value to be stuffed into cluster
; Outputs:    0 	  success
;	      <> 0	  error
; Side Effects: 	  lpBuf may be written to disk
;			  lpBuf may be read from disk
;

cProc	PackFAT,<PUBLIC,NEAR>,<si,di>
    parmW   pDPB
    parmD   lpBuf
    parmW   cluster
    parmW   value
cBegin
	mov	di,pDPB
	mov	cx,cluster
	cCall	SetIndexSector
	mov	si,dx
	cmp	[curFATsec],ax
	je	pfnoread
	push	ax
	cCall	FlushFAT,<di,lpBuf>
	pop	cx
	jnz	pfdone
	mov	al,[di].DPB_drive
	mov	dx,2
	push	cx
	cCall	int25,<ax,lpBuf,dx,cx>
	pop	cx
	jnz	pfdone
	mov	[curFATsec],cx
pfnoread:
	mov	ax,value
	xor	dx,dx
	cmp	[di].DPB_max_cluster,4096-10
	jae	pf2
	mov	cl,4
	mov	dh,0F0h
	test	byte ptr cluster,1
	jz	pf1
	rol	dx,cl
	shl	ax,cl
	jmp	short pf2
pf1:
	not	dx
	and	ax,dx
	not	dx
pf2:
	les	bx,lpBuf
	and	es:[bx+si],dx
	or	es:[bx+si],ax
	xor	ax,ax
pfdone:
cEnd

; (c-ralphp 6/14/88) Begin Modification

cProc	IsHPMachine,<PUBLIC,NEAR>
cBegin	nogen
	push	es
	mov	ax,0F000h
	mov	es,ax
	mov	ax,word ptr es:[0F8h]
	pop	es
	cmp	ax,'PH'
	je	IHMFoundIt
	xor	ax,ax
	jmp	IHMDone

IHMFoundIt:
	    mov     ax,1
IHMDone:
	    ret
cEnd	nogen

; (c-ralphp) End modification


if ?SETUP1

;
; This procedure should return zero if there is no harddisk in the
; system.  Otherwise it should return the upper case ASCII representation
; of the primary hard disk of the system.
;

cProc	GetHardDisk,<PUBLIC,NEAR>
cBegin	nogen
	push	si
	cCall	GetCntFloppyDrives
	mov	si,ax
	cmp	si,1		    ; Dont let A:/B: hack keep us from
	ja	ghdloop
	inc	si		    ; starting with C:
ghdloop:
	inc	si
	mov	dx,si
	mov	ah,36h
	int	21h
	inc	ax
	jz	ghddone
	mov	ax,si
	dec	ax
	add	al,'A'
ghddone:
	pop	si
	ret
cEnd	nogen


; IsFormatted - determines if passed drive is formatted.
;
; Inputs:     drive	  1-based drive number
;	      lpBuf	  pointer to a 2 sector area
; Outputs:    -1	  i/o error reading drive
;	      0 	  drive is not formatted
;	      1 	  drive is formatted, but contains no files.
;	      2 	  drive is formatted, and contains N files.
;
; Side Effects: 	  lpBuf may be read from disk
;
cProc	IsFormatted,<PUBLIC,NEAR>,<si,di>
    parmW   drive
    parmD   lpBuf
    localV  tbuf,6
cBegin
	xor	ax,ax			; Read boot sector from drive
	mov	cx,1
	mov	dx,drive
	dec	dl
	cCall	int25,<dx,lpBuf,cx,ax>
	jz	isf0
	cmp	dl,2
	je	diskerror
	jmp	short notformatted
isf0:
	les	bx,lpBuf
	cmp	byte ptr es:[bx],0E9h	; Boot sector start with a short
	je	isf1
	cmp	byte ptr es:[bx],0EBh	; or long jump?
	jne	notformatted		; No...not formatted then
isf1:					; Yes...assume formatted
	mov	di,dataOffset _DPB
	cCall	GetDPB,<drive,di>
	jnz	notformatted
	mov	ax,FAT_READ
	cCall	OpenFAT,<ax>
	xor	si,si
	cCall	UnpackFAT,<di,lpBuf,si>
	les	si,lpBuf
	cld
	lods	byte ptr es:[si]
	cmp	[di].DPB_media,al
	jne	notformatted
	lods	word ptr es:[si]
	inc	ax
	jnz	notformatted
	mov	si,2
clusloop:
	cCall	UnpackFAT,<di,lpBuf,si>
	or	dx,dx
	jz	nextcluster
	cmp	[di].DPB_max_cluster,4096-10
	jae	clus1
	cmp	dx,0FF7h
	jne	hasfiles
	jmp	short nextcluster
clus1:
	cmp	dx,0FFF7h
	jne	hasfiles
nextcluster:
	inc	si
	cmp	[di].DPB_max_cluster,si
	ja	clusloop
	mov	ax,1
	jmp	short isfdone
hasfiles:
	mov	ax,2
	jmp	short isfdone
diskerror:
	mov	ax,-1
	jmp	short isfdone
notformatted:
	xor	ax,ax
isfdone:
cEnd


; Erase disk - clears all fat entries and the root directory
;
; Inputs:     drive	  1-based drive number
;	      lpBuf	  pointer to a 2 sector area
; Outputs:    0 	  drive is erased
;	      -1	  error occurred
;
; Side Effects: 	  lpBuf may be read from disk
;
cProc	EraseDisk,<PUBLIC,NEAR>,<si,di>
    parmW   drive
    parmD   lpBuf
cBegin
	mov	ax,FAT_READ+FAT_WRITE
	cCall	OpenFAT,<ax>
	mov	di,dataOffset _DPB
	cCall	GetDPB,<drive,di>
	jnz	eddone
	mov	al,[di].DPB_drive
	mov	drive,ax
	mov	si,2
edloop:
	cCall	UnpackFAT,<di,lpBuf,si>
	or	dx,dx
	jz	ednext
	cmp	[di].DPB_max_cluster,4096-10
	jae	ed1
	cmp	dx,0FF7h
	je	ednext
	jmp	short edzero
ed1:
	cmp	dx,0FFF7h
	je	ednext
edzero:
	xor	dx,dx
	cCall	PackFAT,<di,lpBuf,si,dx>
	jnz	eddone
ednext:
	inc	si
	cmp	[di].DPB_max_cluster,si
	ja	edloop
	cCall	FlushFAT,<di,lpBuf>
	jnz	eddone
	xor	ax,ax
	mov	cx,[di].DPB_sector_size
	push	di
	les	di,lpBuf
	cld
	rep	stosb
	pop	di

	mov	si,[di].DPB_dir_sector
edloop1:
	xor	ax,ax
	cmp	si,[di].DPB_first_sector
	jae	eddone
	inc	ax
	cCall	int26,<drive,lpBuf,ax,si>
	jnz	eddone
	inc	si
	jmp	edloop1
eddone:
cEnd

endif

sEnd	CODE

end


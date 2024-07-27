;
; adjust.asm - Export drive number adjust routine
;

.xlist
include cmacros.inc
.list

sBegin  DATA

sEnd    DATA

sBegin  CODE
assumes CS,CODE
assumes DS,DATA

;
; AdjustDriveNum(drive)
;
; The purpose of this routine is to adjust the INT 13H drive number for
;    operations like format if needed. The input is the current number,
;    and the output is the "adjusted" number. The input numbers are based
;    on this assumption:
;
;	If there are N removeable floppy drives on the system, their INT 13
;	numbers go 0,1,2,3,....,n-1 starting with the drive with the lowest
;	(closest to A:) DOS drive letter.
;
;    The purpose of this routine is to make any adjustments needed due to
;    invalidities in this assumption.
;
; This particular routine makes this adjustment:
;
;	If((InputDrv > 0) && (INT 11H says this is a single drive system))
;		OutputDrv=InputDrv + 1;
;
;    This adjustment deals with the fact that Add in 3.5" disk drives on IBM
;    systems start at INT 13 drive #2 even if drive B: is not present.
;
cProc	AdjustDriveNum,<PUBLIC,FAR>,<SI,DI>
        parmW   drive
cBegin AdjustDriveNum
	mov	ax,drive
	or	ax,ax			; Drive 0?
	jz	AdjDn			; Yes
	mov	dx,ax
	int	11H
	xchg	ax,dx
	mov	cl,6
	shr	dx,cl
	and	dx,0000000000000011B	; Mask to # of disketts
	or	dx,dx			; Single drive system?
	jnz	AdjDn			; No
	inc	ax			; Adjust drive #
AdjDn:
cEnd AdjustDriveNum

sEnd    CODE

end

page    ,132
title   Control - Perform various GDI control functions
.286c

.xlist
	include CMACROS.INC
	include windefs.inc
        include 8514.INC
.list

sBegin  Data            
ife     GreyScale
externW         Palette                 ;in PALETTE.DAT
endif
externA		NbrofColours		;in palette.dat
sEnd    Data

subttl  Code Area
page +             
sBegin  Code
assumes cs,Code
assumes ds,Data

externFP        CursorExclude           ;in ROUTINES.ASM
externFP        CursorUnExclude         ;in ROUTINES.ASM

page +
cProc   Control,<FAR,PUBLIC>,<si,di>

        parmD   lpDevice
        parmW   Function
        parmD   lpInData
        parmD   lpOutData

cBegin
	mov	ax, Function
	cmp	ax, 5
	je	FunctionOkay
	cmp	ax, 8
	je	FunctionOkay
	jmp	short ExitControl	;this function is not supported, exit.
FunctionOkay:
	mov	cx, ax			;CX: Function
	mov	dx,ds			;save DS (= Data)
	assumes ds,nothing
	lds	si,lpInData		;if function is valid, point DS:SI at
					;passed colour index
	lodsw				;get the index into BX
	mov	bx,ax
	xor	ax,ax			;assume we have a bad func code passed
	cmp	cl, 5			;does he want to get the colour table
					;entry?
	je	GetColourTable		;yes! go do it
	cmp	cl, 8			;does he want to inquire on our
					;capabilities?
	je	InquireControlCaps	;yes, go do it
	jmp	short ExitControl	;nope, return error (AX = 0)

public  InquireControlCaps
InquireControlCaps:

;AX contains 0
;BX has the control number that he wants to inquire about:

	inc	ax			;assume we support the function
	cmp	bx,5			;is it a get colour table?
	je	ExitControl		;yes, return success!
	cmp	bx,8			;is it an Inquire Control Capability?
	je	ExitControl		;yes, return success!
	dec	ax			;otherwise, return 0 (failure)
	jmp	ExitControl		;return it and leave.

public  GetColourTable
GetColourTable:
	cmp	bx,NbrofColours
	je	short ExitControl

	cmp	seg_lpOutData, 0
	je	short ExitControl
	les	di,lpOutData
                                ;now ES:DI points to return data
if	GreyScale
	mov	al,bl		;get the colour index for RGB return
	mov	ah,al
	stosw
	stosb
	xor	ax,ax		;return a zero in high byte of RGB dword
	stosb
	inc	ax		;return a 1 (success) return code
else				;GreyScale
	mov	ds,dx		;get back DS (= Data)
	assumes ds,Data
	mov	si,DataOFFSET Palette
				;now DS:SI points to base of table
	add	si,bx		;point to colour table entry (there are 3 bytes
	add	si,bx		;per entry in PALETTE)
	add	si,bx
	movsw			;get the RED and the GREEN values
	movsb			;get the BLUE value
	xor	ax,ax		;and return a 0 in the high byte of RGB dword
	stosb
	inc	ax		;return a 1 success code
endif				;GreyScale


ExitControl:
cEnd    Control

sEnd    Code
end

;****************************************************************************
;*									    *
;*  HITEST.ASM -                                                            *
;*									    *
;*	High Memory Manager Testing Program				    *
;*									    *
;*	Used to verify the functionality of any High Memory Manager	    *
;*	Driver. 							    *
;*									    *
;****************************************************************************

	name	Test
        title   'HMM Test Program'

code    segment byte public 'CODE'

        assume  cs:code, ds:code, es:code

        org     100h

main    proc    near

        mov     ah,9h
	mov	dx,offset SignOn
	int	21h

	; Is an HMM Installed?
	mov	ax,1600h
	int	2Fh
	cmp	al,80h
	je	HMMIn
	mov	ah,9h
	mov	dx,offset AintThere
	int	21h
	int	20h			; Terminate

HMMIn:	mov	ah,9h
	mov	dx,offset FoundOne
	int	21h

	; Get the HMM's control entry point
	mov	ax,1610h
        int     2Fh
        mov     word ptr cs:[HMMEntryPt][0],bx
	mov	word ptr cs:[HMMEntryPt][2],es

	; Get the HMM's version number
	mov	ax,0
	call	cs:[HMMEntryPt]

	; Print it
	mov	bx,ax
	mov	ch,4
HexLoop:mov	cl,4
	rol	bx,cl
	mov	al,bl
	and	al,0Fh
	add	al,30h
	cmp	al,3Ah
	jl	Output
	add	al,07h
Output: mov	dl,al
	mov	ah,02h
	int	21h
	dec	ch
	jnz	HexLoop

        ;*------------------------------------------------------------------*
        ;*      Basic Function Check                                        *
        ;*------------------------------------------------------------------*

        xor     bx,bx
        push    cs
        pop     es
	mov	si,offset rgbCommands
cloop:  mov     bl,byte ptr es:[si]
        cmp     bl,'$'
	je	exit

        ; Print the command being executed
        mov     ch,bl
        shl     bx,1
	mov	dx,[rgszCommands+bx]
        mov     ah,9h
        int     21h

	cmp	ch,11
	je	isnop

	mov	ah,ch
	call	cs:[HMMEntryPt]

        ; Print the result
        or      ax,ax
        jz      Fail
        mov     dx,offset Success
        jmp     short PrintIt
Fail:   mov     dx,offset Failure
PrintIt:mov     ah,9
        int     21h

        ; Now print the state of the A20 Line
        mov     dx,offset A20Msg
        int     21h
        mov     ah,7h
        call    cs:[HMMEntryPt]
        or      ax,ax
        jz      NoA20
        mov     dx,offset A20On
        jmp     short PrntIt2
NoA20:  mov     dx,offset A20Off
PrntIt2:mov     ah,9h
        int     21h

isnop:	inc	si
	jmp	cloop

exit:   ret

main	endp

;*--------------------------------------------------------------------------*
;*	Data Area							    *
;*--------------------------------------------------------------------------*

cmdVersion  equ     0
cmdRequest  equ     1
cmdRelease  equ     2
cmdGEnable  equ     3
cmdGDisable equ     4
cmdTEnable  equ     5
cmdTDisable equ     6
cmdA20Query equ     7
cmdExtQuery equ     8
cmdExtAlloc equ     9
cmdExtFree  equ     10
cmdDivider  equ     11

rgbCommands db	    cmdDivider

	    ; Normal High Memory Area Test
	    db	    cmdRequest, cmdRelease, cmdDivider

	    ; Nested High Memory Area Test
	    db	    cmdRequest, cmdRequest, cmdRelease, cmdRelease, cmdDivider
	    db	    cmdDivider

	    ; Normal Global A20 Test
	    db	    cmdGEnable, cmdGDisable, cmdDivider

	    ; Normal Temporary A20 Test
	    db	    cmdTEnable, cmdTDisable, cmdDivider

	    ; Nested Global A20 Test
	    db	    cmdGEnable
	    db		cmdGEnable, cmdGDisable
	    db	    cmdGDisable, cmdDivider

	    ; Nested Temporary A20 Test
	    db	    cmdTEnable
	    db		cmdTEnable, cmdTDisable
	    db	    cmdTDisable, cmdDivider

	    ; Temporary A20s Nested within Global A20s Test
	    db	    cmdGEnable
	    db		cmdTEnable, cmdTDisable
	    db	    cmdGDisable, cmdDivider

	    ; Global A20s Nested within Temporary A20s Test
	    db	    cmdTEnable
	    db		cmdGEnable, cmdGDisable
	    db	    cmdTDisable, cmdDivider
            db      '$'

rgszCommands dw     szVer
	    dw	    szHighReq
	    dw	    szHighRel
	    dw	    szGEnable
	    dw	    szGDisable
	    dw	    szTEnable
	    dw	    szTDisable
	    dw	    szTestA20
	    dw	    szExtQuery
	    dw	    szExtAlloc
	    dw	    szExtFree
	    dw	    szDivider

szVer	    db	    13,10,'Version: $'
szHighReq   db	    13,10,'Request High Memory Area:  $'
szHighRel   db	    13,10,'Release High Memory Area:  $'
szGEnable   db	    13,10,'Globally Enable A20 Line:  $'
szGDisable  db	    13,10,'Globally Disable A20 Line: $'
szTEnable   db	    13,10,'Temp. Enable A20 Line:     $'
szTDisable  db	    13,10,'Temp. Disable A20 Line:    $'
szTestA20   db	    13,10,'See if the A20 Line is On: $'
szExtQuery  db	    13,10,'Extended Memory Free:      $'
szExtAlloc  db	    13,10,'Allocate Extended Memory:  $'
szExtFree   db	    13,10,'Deallocate Extended Memory:$'
szDivider   db	    13,10,'------------------------------------------------------$'

Success     db      'Succeeded$'
Failure     db	    'Failed   $'

A20Msg      db      ' - A20 Line is $'
A20On       db      'On$'
A20Off      db      'Off$'

SignOn      db      13,10,'High Memory Manager Test Program 1.0 - 4/23/88'
            db      13,10,'Copyright 1988, Microsoft Corp.'
	    db	    13,10,'$'

FoundOne    db	    13,10,'High Memory Manager is Installed - Version $'
AintThere   db	    13,10,'High Memory Manager not Installed.$'

HMMEntryPt  dd      ?

code    ends

        end     main

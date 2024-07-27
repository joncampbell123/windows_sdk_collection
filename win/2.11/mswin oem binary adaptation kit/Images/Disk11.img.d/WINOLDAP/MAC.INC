DOScall	MACRO	Function, Subfunction, Handle, Count
	mov	AH, Function
	ifnb	<Subfunction>
	mov	AL, Subfunction
	endif
	ifnb	<Handle>
	mov	BX, Handle
	endif
	ifnb	<Count>
	mov	CX, Count
	endif
	int	21H
ENDM
;
;
POPFF	MACRO
	Local	a
	jmp	$+3
a	label	near
	iret
	push	CS
	call	a
ENDM
;
;~~tqn 031587
;
EMSCall	MACRO	Function, Pages_requested, Pages_map, PID
	mov	AX, Function
	ifnb	<Pages_requested>
	mov	BX, Pages_requested
	endif
	ifnb	<Pages_map>
	mov	CX, Pages_map
	endif
	ifnb	<PID>
	mov	DX, PID
	endif
	int	67H
ENDM
;  
;~~

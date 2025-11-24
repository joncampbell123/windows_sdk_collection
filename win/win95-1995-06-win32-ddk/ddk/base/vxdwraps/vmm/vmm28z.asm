include local.inc

StartCDecl	List_Remove_First@4

		pop	edx		; Get return address
		mov	ecx, esi	; save esi
		pop	esi		; Get List
		VxDCall	List_Remove
		mov	esi, ecx
		sbb	eax, eax
		inc	eax
		jmp	edx

EndCDecl	List_Remove_First@4

END

Page    ,132
title   ~~      Macro Memory Management routines for Spud 
        .xlist
        include cmacros.inc
        include windows.inc
        include macro.inc       
	include tty.inc
        .list
;-----------Macro Memory Manager---------------------------
;
;Module 
;	Macromem.asm:      Code to initialize Macro memory 
;
;Programmer:
;	Ramana Murthy
;
;----------------------------------------------------------
sBegin  data

externB		pifBehavior
externW		OldApState
externW         MacroPara                    
externW         MacroSeg
externW         hMacroSeg
externW         MacroFirst              ;Offset to First Macro
externW         MacroLast               ;Offset to Last Macro
externW         FirstMacAddr            ;First Macro text offset 
externW         FromWithIn              ;Is AddMacro from Within or Update
GlobalW         MacroSegSize,0
GlobalW         MacroMemUsed,0          ;macro memory used up
MarkText        db      "[MARK]",0	;default macros
CopyText        db      "[COPY]",0
PasteText       db      "[PASTE]",0
ScrollText      db      "[SCROLL]",0
RdScreenText    db      0,0
CloseText       db      "[CLOSE]",0
SwitchText      db      "[SWITCH]",0
NullText        db      0,0
sEnd    data

sBegin  code                            
        assumes cs,code
        assumes ds,data

PUBLIC          MacroInitMem,AddDefaultMacros
PUBLIC		SetMacroSize

ExternFP        <GlobalAlloc, GlobalLock,GlobalUnlock>
ExternFP	<GlobalReAlloc>
ExternNP        <AddMacro,VerifyString>

;-----------MacroInitMem-----------------------------------
;
;       DESCRIPTION     
;               This routine allocates Macro memory from 
;               WinOldAp free memory and initializes the 
;               macro memory with default macros
;
;       ENTRY
;               None
;
;       EXIT
;               Initialized macro memory
;
;       USES    
;               AX,BX,CX,DX
;
;----------------------------------------------------------

MacroInitMem    PROC    NEAR

        push    SI
        push    DI
        push    ES
        push    DS
        pop     ES
        mov     AX,MacroPara            ;macro memory needed in paras
        mov     CX,4
        shl     AX,CL                   ;convert into bytes * 16
        mov     MacroSegSize,AX         ;Save it
        mov     CX,GMEM_MOVEABLE+GMEM_ZEROINIT  ;make it moveable
        xor     BX,BX
;        push    ES
;        push    DS
        RegPtr  MacSize,<BX,AX>         ;Global Alloc needs long size
        cCall   GlobalAlloc,<CX,MacSize>
	or	AX,AX			;is the call successful ?
	jz	MI_FAIL			;failed
        mov     hMacroSeg,AX            ;memory handle from GlobalAlloc
        cCall   GlobalLock,AX           ;get the absolute address of MacroMem
;        pop     DS
;        pop     ES
        mov     MacroSeg,DX
        mov     FirstMacAddr,AX
        mov     BX,MacroSegSize
        sub     BX,MacroInfoSize        ;to use only 2k no more
        mov     MacroLast,BX
        mov     MacroFirst,BX
        mov     ES,DX                     
        mov     ES:[BX].MacroID,LowMacroID
        call    AddDefaultMacros
        mov     AX,hMacroSeg            ;have to add default macros
        cCall   GlobalUnlock,AX 
        jmp	MI_RET
MI_FAIL:

	mov	hMacroSeg,0		;for safety already 0
	mov	MacroPara,0		;no need to allocate in bufseg
	mov	MacroSeg,0		;same as hMacroSeg
MI_RET:
        pop     ES
        pop     DI
        pop     SI
        ret


MacroInitMem    ENDP

;--------AddDefaultMacros----------------------------------
;
;        DESCRIPTION	  
;		Adds default macros as defined by DDE and
;		Window Menus
;
;        ENTRY
;		ES:MacroSeg
;		
;        EXIT
;		Macro Memory initialized with macros
;		
;        USES
;		AX,DX,BX,CX
;
;----------------------------------------------------------

AddDefaultMacros        PROC    NEAR
        push    ES
        push    DS
        push    SI
        push    DI

        mov     DX,DS
        mov     ES,DX

        mov     SI,OFFSET MarkText
        mov     AX,MarkID
        call    AddOneMacro

        mov     SI,OFFSET CopyText
        mov     AX,CopyID
        call    AddOneMacro

        mov     SI,OFFSET PasteText
        mov     AX,PasteID
        call    AddOneMacro

        mov     SI,OFFSET ScrollText
        mov     AX,ScrollID
        call    AddOneMacro

        mov     SI,OFFSET SwitchText
        mov     AX,SwitchID
        call    AddOneMacro

        mov     SI,OFFSET CloseText
        mov     AX,CloseID
        call    AddOneMacro

        mov     SI,OFFSET NullText
        mov     AX,NullID
        call    AddOneMacro

        mov     SI,OFFSET NullText
        mov     AX,SwitchOutID
        call    AddOneMacro

        mov     SI,OFFSET NullText
        mov     AX,ExitID
        call    AddOneMacro

        mov     SI,OFFSET NullText
        mov     AX,ExecuteID
        call    AddOneMacro

        pop     DI
        pop     SI
        pop     DS
        pop     ES

        ret
                  
AddDefaultMacros        ENDP


;--------AddOneMacro---------------------------------------
;
;        DESCRIPTION
;		This is called from AddDefaultMacros to add
;		just one macro by setting up SI and ES values
;		    
;        ENTRY
;		SI: Macro Text to be added
;
;        EXIT
;
;        USES
;		AX
;
;----------------------------------------------------------
AddOneMacro     PROC    NEAR

        sub     SI,SIZE MacroID
        sub     SI,SIZE MacroSize
        push    AX
        call    VerifyString
        pop     AX          
        mov     FromWithIn,0
        call    AddMacro
        ret

AddOneMacro     ENDP

;--------SetMacroMemSize-----------------------------------
;
;	 DESCRIPTION
;		Reallocates macro memory if required by the DDE
;		handler for BAD applications since for good apps
;		this can be increased dynamically.
;
;	 ENTRY
;		macrosize in C convention on the stack		
;
;	 EXIT					    
;		if successful AX <> 0
;
;	 USES
;		AX,BX,CX,DX
;----------------------------------------------------------

cProc	SetMacroSize,<PUBLIC,NEAR>,<SI,DI,ES,DS>
	parmW	macrsize	       

cBegin	SetMacroSize
	
	cmp	OldApState,SF_NOT_STARTED
	jnz	SMC99
	mov	AX,1				;if good app no errors
	test	pifbehavior,BADBITS
	jz	SMC99				;not for goodapps
	mov	AX,macrsize
	or	AX,AX				;check if zero
	jnz	SMC10
	mov	AX,1				;no errors
	jmp	SMC99
	
SMC10:	
	add 	AX,MacroSegSize			;macro segment size in paras
	add	AX,0Fh				;round to nearest para
	mov	CX,4
	shr	AX,CL				;convert to paras
	mov	DX,AX
	mov	CX,4
	shl	AX,CL						
	mov	CX,AX
	mov	BX,GMEM_MOVEABLE + GMEM_ZEROINIT
	push	CX
	push	DX	     
	xor	SI,SI
	regptr	MacMem,SI,AX	
	cCall	GlobalReAlloc,<hMacroSeg,MacMem,BX>
	pop	DX
	pop	CX
	or	AX,AX
	jz	SMC99
	mov	MacroSegSize,CX			;new size
	mov	MacroPara,DX			;new macro para
	mov	hMacroSeg,AX			; GMEM_MOVEABLE flag is used

SMC99:
	       
cEnd	SetMacroSize

sEnd    code
        end

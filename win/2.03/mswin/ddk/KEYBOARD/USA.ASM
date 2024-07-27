;***************************************************************************
;                                                                          *
;   Copyright (C) 1983,1984,1985 by Microsoft Inc.                         *
;                                                                          *
;***************************************************************************

        TITLE   USA - country specific code for U.S.A. keyboard

include keyboard.inc
;
; The INIT segment is discardable, as it can be executed only once no
; special care has been taken inside when calling internal subroutines.
; 
createSeg _INIT, INIT, BYTE, PUBLIC, CODE
sBegin INIT
assumes CS,INIT
assumes DS,DATA

externNP  <SetKbdType>
;
; KbInit, Initialize keyboard
;
cProc KbInit,<PUBLIC,FAR>
cBegin  nogen
	call	SetKbdType	; check keyboard type (enhanced or not)
	or	ax,ax
	mov	ax,0
	jnz	kbError		; return AX = 0 if error
	inc	ax
kbError:
	ret
cEnd    nogen
sEnd    INIT

 
sBegin  DATA            ; Beginning of data segment

externB fKbRt

globalB fAltGr,0                ; nothing to activate

labelB  AsciiTable              ;base,shift,control
        db      VK_CANCEL       ,3,3,3
        db      VK_BACK         ,8,8,127
        db      VK_TAB          ,9,9,-1
        db      VK_RETURN       ,13,13,10
        db      VK_ESCAPE       ,27,27,27
        db      VK_SPACE        ,' ',' ',' '
        db      VK_EXECUTE      ,13,13,10
        db      VK_0            ,'0',')',-1
        db      VK_1            ,'1','!',-1
        db      VK_2            ,'2','@',0   +80H       ; shift must be on
        db      VK_3            ,'3','#',-1
        db      VK_4            ,'4','$',-1
        db      VK_5            ,'5','%',-1
        db      VK_6            ,'6','^',30  +80H       ; shift must be on
        db      VK_7            ,'7','&',-1
        db      VK_8            ,'8','*',-1
        db      VK_9            ,'9','(',-1
        db      VK_A            ,'a','A',1
        db      VK_B            ,'b','B',2
        db      VK_C            ,'c','C',3
        db      VK_D            ,'d','D',4
        db      VK_E            ,'e','E',5
        db      VK_F            ,'f','F',6
        db      VK_G            ,'g','G',7
        db      VK_H            ,'h','H',8
        db      VK_I            ,'i','I',9
        db      VK_J            ,'j','J',10
        db      VK_K            ,'k','K',11
        db      VK_L            ,'l','L',12
        db      VK_M            ,'m','M',13
        db      VK_N            ,'n','N',14
        db      VK_O            ,'o','O',15
        db      VK_P            ,'p','P',16
        db      VK_Q            ,'q','Q',17
        db      VK_R            ,'r','R',18
        db      VK_S            ,'s','S',19
        db      VK_T            ,'t','T',20
        db      VK_U            ,'u','U',21
        db      VK_V            ,'v','V',22
        db      VK_W            ,'w','W',23
        db      VK_X            ,'x','X',24
        db      VK_Y            ,'y','Y',25
        db      VK_Z            ,'z','Z',26
        db      VK_NUMPAD0      ,'0','0',-1
        db      VK_NUMPAD1      ,'1','1',-1
        db      VK_NUMPAD2      ,'2','2',-1
        db      VK_NUMPAD3      ,'3','3',-1
        db      VK_NUMPAD4      ,'4','4',-1
        db      VK_NUMPAD5      ,'5','5',-1
        db      VK_NUMPAD6      ,'6','6',-1
        db      VK_NUMPAD7      ,'7','7',-1
        db      VK_NUMPAD8      ,'8','8',-1
        db      VK_NUMPAD9      ,'9','9',-1
        db      VK_DECIMAL      ,'.','.',-1
        db      VK_OEM_PLUS     ,'=','+',-1
        db      VK_OEM_COMMA    ,',','<',-1
        db      VK_OEM_MINUS    ,'-','_',31  +80H       ; shift must be on
        db      VK_OEM_PERIOD   ,'.','>',-1
        db      VK_OEM_1        ,';',':',-1
        db      VK_OEM_2        ,'/','?',-1
        db      VK_OEM_3        ,'`','~',-1
        db      VK_OEM_4        ,'[','{',27
        db      VK_OEM_5        ,'\','|',28
        db      VK_OEM_6        ,']','}',29
        db      VK_OEM_7        ,"'",'"',-1
        db      VK_MULTIPLY     ,'*','*',-1
        db      VK_SUBTRACT     ,'-','-',-1
        db      VK_ADD          ,'+','+',-1
	db      VK_DIVIDE       ,'/','/',-1 
labelB  VirtualDefault
        db      -1,-1,-1,-1


; Table to translate IBM/PC hardware scan code to MS-WIN virtual key codes
;
	PUBLIC  keyTranslationTable
labelB  keyTranslationTable
        DB      -1             ; Scan code 0 undefined
        DB      VK_ESCAPE      ; Esc
        DB      VK_1           ; 1
        DB      VK_2           ; 2
        DB      VK_3           ; 3
        DB      VK_4           ; 4
        DB      VK_5           ; 5
        DB      VK_6           ; 6
        DB      VK_7           ; 7
        DB      VK_8           ; 8
        DB      VK_9           ; 9
        DB      VK_0           ; 0
        DB      VK_OEM_MINUS   ; -
        DB      VK_OEM_PLUS    ; =
        DB      VK_BACK        ; backspace
        DB      VK_TAB         ; tab
        DB      VK_Q           ; q
        DB      VK_W           ; w
        DB      VK_E           ; e
        DB      VK_R           ; r
        DB      VK_T           ; t
        DB      VK_Y           ; y
        DB      VK_U           ; u
        DB      VK_I           ; i
        DB      VK_O           ; o
        DB      VK_P           ; p
        DB      VK_OEM_4       ; [
        DB      VK_OEM_6       ; ]
        DB      VK_RETURN      ; return
        DB      VK_CONTROL     ; Ctrl
        DB      VK_A           ; a
        DB      VK_S           ; s
        DB      VK_D           ; d
        DB      VK_F           ; f
        DB      VK_G           ; g
        DB      VK_H           ; h
        DB      VK_J           ; j
        DB      VK_K           ; k
        DB      VK_L           ; l
        DB      VK_OEM_1       ; ;
        DB      VK_OEM_7       ; '
        DB      VK_OEM_3       ; `
        DB      VK_SHIFT       ; left-shift
        DB      VK_OEM_5       ; \
        DB      VK_Z           ; z
        DB      VK_X           ; x
        DB      VK_C           ; c
        DB      VK_V           ; v
        DB      VK_B           ; b
        DB      VK_N           ; n
        DB      VK_M           ; m
        DB      VK_OEM_COMMA   ; ,
        DB      VK_OEM_PERIOD  ; .
        DB      VK_OEM_2       ; /
        DB      VK_SHIFT       ; right-shift
        DB      VK_MULTIPLY    ; PrtSc
        DB      VK_MENU        ; Alt
        DB      VK_SPACE       ; space
        DB      VK_CAPITAL     ; CapsLock
        DB      VK_F1          ; F1
        DB      VK_F2          ; F2
        DB      VK_F3          ; F3
        DB      VK_F4          ; F4
        DB      VK_F5          ; F5
        DB      VK_F6          ; F6
        DB      VK_F7          ; F7
        DB      VK_F8          ; F8
        DB      VK_F9          ; F9
        DB      VK_F10         ; F10
        DB      VK_OEM_NUMBER  ; NumLock
        DB      VK_OEM_SCROLL  ; ScrollLock
        DB      VK_HOME        ; numpad-7
        DB      VK_UP          ; numpad-8
        DB      VK_PRIOR       ; numpad-9
        DB      VK_SUBTRACT    ; numpad-minus
        DB      VK_LEFT        ; numpad-4
        DB      VK_CLEAR       ; numpad-5
        DB      VK_RIGHT       ; numpad-6
        DB      VK_ADD         ; numpad-plus
        DB      VK_END         ; numpad-1
        DB      VK_DOWN        ; numpad-2
        DB      VK_NEXT        ; numpad-3
        DB      VK_INSERT      ; numpad-0
        DB      VK_DELETE      ; numpad-period
;
; Extension for RT keyboard, we can leave these codes for the other
; keyboards, as they can't reach them no big deal.
;
        DB      -1             ; would be SysReq, but unused
	DB      -1             ; ???
        DB      -1             ; not used for U.S. RT keyboard
        DB      VK_F11         ; F11
        DB      VK_F12         ; F12

                PUBLIC KeyTransTblLen
KeyTransTblLen  = $ - KeyTranslationTable   ; count of keys

	PUBLIC  KeyNumTrans
labelB  KeyNumTrans
        DB      VK_NUMPAD7      ; numpad-7
        DB      VK_NUMPAD8      ; numpad-8
        DB      VK_NUMPAD9      ; numpad-9
        DB      VK_SUBTRACT     ; numpad-minus
        DB      VK_NUMPAD4      ; numpad-4
        DB      VK_NUMPAD5      ; numpad-5
        DB      VK_NUMPAD6      ; numpad-6
        DB      VK_ADD          ; numpad-plus
        DB      VK_NUMPAD1      ; numpad-1
        DB      VK_NUMPAD2      ; numpad-2
        DB      VK_NUMPAD3      ; numpad-3
        DB      VK_NUMPAD0      ; numpad-0
        DB      VK_DECIMAL      ; numpad-period

sEnd    DATA                    ; End of data segment

NoOemAnsiTables = 0
CtrlAltEscape   = 0
Do_dead_key     = 0             ; no dead keys for USA keyboard
include toascii.inc

;
; The NORES segment contains some code for WINOLDAP. It is LOADONCALL
;
createSeg _NORES, NORES, BYTE, PUBLIC, CODE

sBegin NORES
assumes CS,NORES
assumes DS,DATA

;
; This table maps OEM keys 0-127 into scan codes and Shift Modifiers
; it is indexed by the character value, there are 2 bytes per char.
;
; This table is IBM PC specific
;
;	First byte, Modifier flags (Shift state)
;		bit  7 - Insert state active
;		bit  6 - Caps Lock state active
;		bit  5 - Num Lock state active
;		bit  4 - Scroll Lock state active
;		bit  3 - Alt shift depressed
;		bit  2 - Ctrl shift depressed
;		bit  1 - left shift depressed	USE THIS ONE FOR SHIFTED CHARS
;		bit  0 - right shift depressed	DO NOT USE THIS ONE
;	    NOTE: Setting the Shift state to -1 (0FFH) is an indication that
;		there is no scan code translation for this key. This can be
;		used to indicate OEM keys which do not map to a Single scan code
;		(chars built from multi-key sequences such as dead-keys).
;		For entries of this type, the Scan code (second byte) is
;		irrelevant (setting it to -1 too is fine).
;	Second byte, Scan Code
;
KeyTable	label byte
	db	00001000B,52H	   ; NUL	0
	db	00000100B,1EH	   ; ^A
	db	00000100B,30H	   ; ^B
	db	00000100B,2EH	   ; ^C
	db	00000100B,20H	   ; ^D
	db	00000100B,12H	   ; ^E
	db	00000100B,21H	   ; ^F
	db	00000100B,22H	   ; ^G
	db	00000000B,0EH	   ; BS
	db	00000000B,0FH	   ; TAB
	db	00000100B,24H	   ; ^J 	10
	db	00000100B,25H	   ; ^K
	db	00000100B,26H	   ; ^L
	db	00000000B,1CH	   ; ENTER
	db	00000100B,31H	   ; ^N
	db	00000100B,18H	   ; ^O
	db	00000100B,19H	   ; ^P
	db	00000100B,10H	   ; ^Q
	db	00000100B,13H	   ; ^R
	db	00000100B,1FH	   ; ^S
	db	00000100B,14H	   ; ^T 	20
	db	00000100B,16H	   ; ^U
	db	00000100B,2FH	   ; ^V
	db	00000100B,11H	   ; ^W
	db	00000100B,2DH	   ; ^X
	db	00000100B,15H	   ; ^Y
	db	00000100B,2CH	   ; ^Z
	db	00000000B,01H	   ; ESC
	db	00000100B,2BH	   ; ^\
	db	00000100B,1BH	   ; ^]
	db	00000110B,07H	   ; ^^ 	30
	db	00000110B,0CH	   ; ^_
	db	00000000B,39H	   ; SPACE
	db	00000010B,02H	   ; !
	db	00000010B,28H	   ; "
	db	00000010B,04H	   ; #
	db	00000010B,05H	   ; $
	db	00000010B,06H	   ; %
	db	00000010B,08H	   ; &
	db	00000000B,28H	   ; '
	db	00000010B,0AH	   ; (		40
	db	00000010B,0BH	   ; )
	db	00000010B,09H	   ; *
	db	00000010B,0DH	   ; +
	db	00000000B,33H	   ; ,
	db	00000000B,0CH	   ; -
	db	00000000B,34H	   ; .
	db	00000000B,35H	   ; /
	db	00000000B,0BH	   ; 0
	db	00000000B,02H	   ; 1
	db	00000000B,03H	   ; 2		50
	db	00000000B,04H	   ; 3
	db	00000000B,05H	   ; 4
	db	00000000B,06H	   ; 5
	db	00000000B,07H	   ; 6
	db	00000000B,08H	   ; 7
	db	00000000B,09H	   ; 8
	db	00000000B,0AH	   ; 9
	db	00000010B,27H	   ; :
	db	00000000B,27H	   ; ;
	db	00000010B,33H	   ; <		60
	db	00000000B,0DH	   ; =
	db	00000010B,34H	   ; >
	db	00000010B,35H	   ; ?
	db	00000010B,03H	   ; @
	db	00000010B,1EH	   ; A
	db	00000010B,30H	   ; B
	db	00000010B,2EH	   ; C
	db	00000010B,20H	   ; D
	db	00000010B,12H	   ; E
	db	00000010B,21H	   ; F		70
	db	00000010B,22H	   ; G
	db	00000010B,23H	   ; H
	db	00000010B,17H	   ; I
	db	00000010B,24H	   ; J
	db	00000010B,25H	   ; K
	db	00000010B,26H	   ; L
	db	00000010B,32H	   ; M
	db	00000010B,31H	   ; N
	db	00000010B,18H	   ; O
	db	00000010B,19H	   ; P		80
	db	00000010B,10H	   ; Q
	db	00000010B,13H	   ; R
	db	00000010B,1FH	   ; S
	db	00000010B,14H	   ; T
	db	00000010B,16H	   ; U
	db	00000010B,2FH	   ; V
	db	00000010B,11H	   ; W
	db	00000010B,2DH	   ; X
	db	00000010B,15H	   ; Y
	db	00000010B,2CH	   ; Z		90
	db	00000000B,1AH	   ; [
	db	00000000B,2BH	   ; \
	db	00000000B,1BH	   ; ]
	db	00000010B,07H	   ; ^
	db	00000010B,0CH	   ; _
	db	00000000B,29H	   ; `
	db	00000000B,1EH	   ; a
	db	00000000B,30H	   ; b
	db	00000000B,2EH	   ; c
	db	00000000B,20H	   ; d		100
	db	00000000B,12H	   ; e
	db	00000000B,21H	   ; f
	db	00000000B,22H	   ; g
	db	00000000B,23H	   ; h
	db	00000000B,17H	   ; i
	db	00000000B,24H	   ; j
	db	00000000B,25H	   ; k
	db	00000000B,26H	   ; l
	db	00000000B,32H	   ; m
	db	00000000B,31H	   ; n		110
	db	00000000B,18H	   ; o
	db	00000000B,19H	   ; p
	db	00000000B,10H	   ; q
	db	00000000B,13H	   ; r
	db	00000000B,1FH	   ; s
	db	00000000B,14H	   ; t
	db	00000000B,16H	   ; u
	db	00000000B,2FH	   ; v
	db	00000000B,11H	   ; w
	db	00000000B,2DH	   ; x		120
	db	00000000B,15H	   ; y
	db	00000000B,2CH	   ; z
	db	00000010B,1AH	   ; {
	db	00000010B,2BH	   ; |
	db	00000010B,1BH	   ; }
	db	00000010B,29H	   ; ~
	db	00000000B,53H	   ; DEL	127


;
; OEMKeyScan( OEMKey )
;
; This routine maps OEM ASCII key codes < 128 to OEM scan codes and Shift
;    states for WINOLDAP. OUTPUT:
;
;	AX = Scan code
;	DX = Modifier (Shift state)
;
;	NOTE: RETURN OF DX=AX=-1 means there is no scan code translation for
;	   this key
;
; NOTE: If passsed a key >= 128 this routine returns -1
;
cProc OEMKeyScan,<PUBLIC,FAR>,<si,di>
    ParmB   OEMKey
cBegin OEMKeyScan
	xor	bx,bx
	mov	bl,OEMKey
	or	bl,bl		; Check for range
	js	NoKey		; Out of range, return -1
	xor	ax,ax		; AX = 0
	shl	bx,1		; Two bytes per entry in table
	mov	dx,word ptr cs:[bx.KeyTable]
	xchg	dh,al		; AL is scan code, DH = 0
	cmp	dl,-1		; Valid mapping?
	jnz	KeyDn		; yes
NoKey:
	xor	ax,ax		; Set -1 return
	dec	ax
	mov	dx,ax
KeyDn:
cEnd	OEMKeyScan

sEnd	NORES


end  Kbinit			; startup code

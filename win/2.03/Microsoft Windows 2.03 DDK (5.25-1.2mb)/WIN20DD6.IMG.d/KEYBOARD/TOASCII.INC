; This include module contains the ToAscii procedure.  It is included
; into the contry specific .ASM file for each country.


sBegin  DATA        ; Beginning of data segment

ifdef DOSHIFTLOCK
	externB    <fShiftLock>
endif

; scan codes for special keys

fCaps   equ     01000000b
fNum    equ     00100000b
fScroll equ     00010000b

KeyPadDigits    db      7,8,9,-1
                db      4,5,6,-1
                db      1,2,3
                db      0

fAnsi           db      0

sEnd    DATA        ; End of data segment

sBegin  CODE        ; Beginning of code segment
assumes CS,CODE
assumes DS,DATA

externNP <Dolights>

ife NoOemAnsiTables

; These are the standard tables for all countries except Denmark and
; Norway.

; The goal of this module is to translate strings from Ansi to Oem
; character set or the opposite. If there is no equivalent character
; we use the followings rules:
;  1) we put a similar character (e.g. character without accent)
;  2) if rule 1 is not applicable we put underscore"_".

AnsiToOemTable  label   byte
      db   0FFh     ; non breaking space (NBSP)
      db   0ADh     ; A1h  inverted point
      db   09Bh     ; A2h  cent
      db   09Ch     ; A3h  british pound
      db   00Fh     ; A4h  sun
      db   09Dh     ; A5h  yen
      db   07CH     ; A6h  vertical bar (cut)
      db   015h     ; A7h  section mark
      db   022h     ; A8h  umlaut
      db   063h     ; A9h  copyright sign
      db   0A6h     ; AAh  a underlined (superscript)
      db   0AEh     ; ABh  <<
      db   0AAh     ; ACh  logical NOT
      db   02Dh     ; ADh  syllabic hyphenation (SHY)
      db   072h     ; AEh  registered mark
      db   05Fh     ; AFh  top bar

      db   0F8h     ; B0h  degree
      db   0F1h     ; B1h  + underlined
      db   0FDh     ; B2h  2 (superscript)
      db   033h     ; B3h  3 (superscript)
      db   027h     ; B4h  acute
      db   0E6h     ; B5h  greek mu
      db   014h     ; B6h  paragraph sign
      db   0FAh     ; B7h  middle dot
      db   02Ch     ; B8h  cedilla
      db   031h     ; B9h  1 (superscript)
      db   0A7h     ; BAh  o underlined (superscript)
      db   0AFh     ; BBh  >>
      db   0ACh     ; BCh  1/4
      db   0ABh     ; BDh  1/2
      db   05Fh     ; BEh  3/4
      db   0A8h     ; BFh  inverted question mark

      db   041h     ; C0h  A grave uppercase
      db   041h     ; C1h  A acute uc
      db   041h     ; C2h  A circumflex uc
      db   041h     ; C3h  A tilde uc
      db   08Eh     ; C4h  A umlaut uc
      db   08Fh     ; C5h  A ring uc
      db   092h     ; C6h  AE uc
      db   080h     ; C7h  C cedilla uc
      db   045h     ; C8h  E grave uc
      db   090h     ; C9h  E acute uc
      db   045h     ; CAh  E circumflex uc
      db   045h     ; CBh  E umlaut uc
      db   049h     ; CCh  I grave uc
      db   049h     ; CDh  I acute uc
      db   049h     ; CEh  I circumflex uc
      db   049h     ; CFh  I umlaut uc

      db   044h     ; D0h  D striked uc
      db   0A5h     ; D1h  N tilde uc
      db   04Fh     ; D2h  O grave uc
      db   04Fh     ; D3h  O acute uc
      db   04Fh     ; D4h  O circumflex uc
      db   04Fh     ; D5h  O tilde uc
      db   099h     ; D6h  O umlaut uc
      db   05Fh     ; D7h  reserved
      db   04Fh     ; D8h  O slashed uc
      db   055h     ; D9h  U grave uc
      db   055h     ; DAh  U acute uc
      db   055h     ; DBh  U circumflex uc
      db   09Ah     ; DCh  U umlaut uc
      db   059h     ; DDh  Y acute uc
      db   05Fh     ; DEH  iceland Eth
      db   0E1h     ; DFh  german sharp S or greek beta


      db   085h     ; E0h  a grave lowercase
      db   0A0h     ; E1h  a acute lc
      db   083h     ; E2h  a circumflex lc
      db   061h     ; E3h  a tilde lc
      db   084h     ; E4h  a umlaut lc
      db   086h     ; E5h  a ring lc
      db   091h     ; E6h  ae lc
      db   087h     ; E7h  c cedilla lc
      db   08Ah     ; E8h  e grave lc
      db   082h     ; E9h  e acute lc
      db   088h     ; EAh  e circumflex lc
      db   089h     ; EBh  e umlaut lc
      db   08Dh     ; ECh  i grave lc
      db   0A1h     ; EDh  i acute lc
      db   08Ch     ; EEh  i circumflex lc
      db   08Bh     ; EFh  i umlaut lc

      db   064h     ; F0h  d striked lc
      db   0A4h     ; F1h  n tilde lc
      db   095h     ; F2h  o grave lc
      db   0A2h     ; F3h  o acute lc
      db   093h     ; F4h  o circumflex lc
      db   06Fh     ; F5h  o tilde lc
      db   094h     ; F6h  o umlaut lc
      db   05Fh     ; F7h  reserved
      db   06Fh     ; F8h  o slashed lc
      db   097h     ; F9h  u grave lc
      db   0A3h     ; FAh  u acute lc
      db   096h     ; FBh  u circumflex lc
      db   081h     ; FCh  u umlaut lc
      db   079h     ; FDh  y acute lc
      db   05Fh     ; FEH  iceland eth lc
      db   098h     ; FFh  y umlaut lc



OemToAnsiTable   label   byte
      db   0C7H     ; 80h  C cedilla
      db   0FCh     ; 81h  u umlaut
      db   0E9h     ; 82h  e acute
      db   0E2h     ; 83h  a circumflex
      db   0E4h     ; 84h  a umlaut
      db   0E0h     ; 85h  a grave
      db   0E5h     ; 86h  a ring
      db   0E7h     ; 87h  c cedilla
      db   0EAh     ; 88h  e circumflex
      db   0EBh     ; 89h  e umlaut
      db   0E8h     ; 8Ah  e grave
      db   0EFh     ; 8Bh  i umlaut
      db   0EEh     ; 8Ch  i circumflex
      db   0ECh     ; 8Dh  i grave
      db   0C4h     ; 8Eh  A umlaut
      db   0C5h     ; 8Fh  A ring

      db   0C9h     ; 90h  E acute
      db   0E6h     ; 91h  ae
      db   0C6h     ; 92h  AE
      db   0F4h     ; 93h  o circumflex
      db   0F6h     ; 94h  o umlaut
      db   0F2h     ; 95h  o grave
      db   0FBh     ; 96h  u circumflex
      db   0F9h     ; 97h  u grave
      db   0FFh     ; 98h  y umlaut
      db   0D6h     ; 99h  O umlaut
      db   0DCh     ; 9Ah  U umlaut
      db   0A2h     ; 9Bh  cent
      db   0A3h     ; 9Ch  british pound
      db   0A5h     ; 9Dh  yen
      db   070h     ; 9Eh  Pesetas
      db   066h     ; 9Fh  florin (dutch)

      db   0E1h     ; A0h  a acute
      db   0EDh     ; A1h  i acute
      db   0F3h     ; A2h  o acute
      db   0FAh     ; A3h  u acute
      db   0F1h     ; A4h  n tilde
      db   0D1h     ; A5h  N tilde
      db   0AAh     ; A6h  a underlined superscript
      db   0BAh     ; A7h  o underlined superscript
      db   0BFh     ; A8h  inverted question mark
      db   05Fh     ; A9h  left top corner
      db   0ACh     ; AAh  right top corner
      db   0BDh     ; ABh  1/2
      db   0BCh     ; ACh  1/4
      db   0A1h     ; ADh  inverted point
      db   0ABh     ; AEh  <<
      db   0BBh     ; AFh  >>

      db   ' '      ; B0h  here begins semigraphic characters
      db   ' '      ; B1h
      db   ' '      ; B2h
      db   '|'      ; B3h
      db   '+'      ; B4h
      db   '+'      ; B5h
      db   '+'      ; B6h
      db   '+'      ; B7h
      db   '+'      ; B8h
      db   '+'      ; B9h
      db   '|'      ; BAh
      db   '+'      ; BBh
      db   '+'      ; BCh
      db   '+'      ; BDh
      db   '+'      ; BEh
      db   '+'      ; BFh

      db   '+'      ; C0h
      db   '+'      ; C1h
      db   '+'      ; C2h
      db   '+'      ; C3h
      db   0ADh     ; C4h
      db   '+'      ; C5h
      db   '+'      ; C6h
      db   '+'      ; C7h
      db   '+'      ; C8h
      db   '+'      ; C9h
      db   '+'      ; CAh
      db   '+'      ; CBh
      db   '+'      ; CCh
      db   '='      ; CDh
      db   '+'      ; CEh
      db   '+'      ; CFh

      db   '+'      ; D0h
      db   '+'      ; D1h
      db   '+'      ; D2h
      db   '+'      ; D3h
      db   '+'      ; D4h
      db   '+'      ; D5h
      db   '+'      ; D6h
      db   '+'      ; D7h
      db   '+'      ; D8h
      db   '+'      ; D9h
      db   '+'      ; DAh
      db   ' '      ; DBh
      db   ' '      ; DCh
      db   ' '      ; DDh
      db   ' '      ; DEh
      db   ' '      ; DFh  end of semigraphic characters

      db   05Fh     ; E0h  alpha
      db   0DFh     ; E1h  german sharp S or greek beta
      db   05Fh     ; E2h  lambda
      db   0B6h     ; E3h  pi
      db   05Fh     ; E4h  sigma uc
      db   05Fh     ; E5h  sigma lc
      db   0B5h     ; E6h  mu
      db   05Fh     ; E7h  tau
      db   05Fh     ; E8h  phi uc
      db   05Fh     ; E9h  teta
      db   05Fh     ; EAh  omega
      db   05Fh     ; EBh  delta
      db   05Fh     ; ECh  infinite
      db   05Fh     ; EDh  math empty set or phi lc
      db   05Fh     ; EEh  math own sign
      db   05Fh     ; EFh  math include sign

      db   05Fh     ; F0h  math equivalence sign
      db   0B1h     ; F1h  + underlined
      db   05Fh     ; F2h  greater equal
      db   05Fh     ; F3h  less equal
      db   05Fh     ; F4h  math integral upper part
      db   05Fh     ; F5h  math integral lower part
      db   05Fh     ; F6h  math divide
      db   05Fh     ; F7h  math approximately (~)
      db   0B0h     ; F8h  degree
      db   0B7h     ; F9h  period accent (bold)
      db   0B7h     ; FAh  period accent
      db   05Fh     ; FBh  math root
      db   06Eh     ; FCh  n superscript
      db   0B2h     ; FDh  2 superscript
      db   0A8h     ; FEh  black box (or umlaut)
      db   0A0h     ; FFh  blank

endif

lstrsetup:
        pop     dx
        mov     bx,sp

        push    ds
        push    si
        push    di

        lds     si,ss:[bx+8]
        les     di,ss:[bx+4]
        cld
        mov     ah,-1
        jmp     dx

lstrfinish  proc    far
        cwd                     ; set ax = to hi bit of ah
        mov     ax,dx
        pop     di
        pop     si
        pop     ds
        ret     8
lstrfinish  endp


;
; AnsiToOem( pSrc, pDst ) - Translates the string at pSrc into
; the string at pDst.  pSrc == pDst is legal.
;
;  On Return AX = 0 if some of the translations were not exact
;

cProc   AnsiToOem,<PUBLIC,FAR>
;   ParmD   pSrc
;   ParmD   pDst
cBegin  nogen
        call    lstrsetup
        mov     bx,codeOffset AnsiToOemTable-160
ao1:    lodsb
        cmp     al,160                  ; 160 = first difference of ...
        jb      ao2                     ; ...a printable character
        xlat    AnsiToOemTable
        and     ah,al
ao2:    stosb
        or      al,al
        jnz     ao1
        jmp     lstrfinish
cEnd    nogen


;
; OemToAnsi( pSrc, pDst ) - Translates the string at pSrc into
; the string at pDst.  pSrc == pDst is legal.
;
;  On Return AX = 0 if some of the translations were not exact
;
cProc   OemToAnsi,<PUBLIC,FAR>
;   ParmD   pSrc
;   ParmD   pDst
cBegin  nogen
        call    lstrsetup
        mov     bx,codeOffset OemToAnsiTable-128
oa1:    lodsb
        cmp     al,128
        jb      oa2
        xlat    OemToAnsiTable
        and     ah,al
oa2:    stosb
        or      al,al
        jnz     oa1
        jmp     lstrfinish
cEnd    nogen


;
; Procedure to provide the default ASCII translation for an MS-WIN
; virtual key code.
;

; Added feature if Do_dead_key is non-zero:

; 1) Dead key function (acute and grave)
; 2) Caps Lock for special cases (ie umlaut characters)
;
; CtrlAltEscape flag, if set to 1, allows to get some US ASCII codes ([{]}\|)

; Every country Xlation routine sharing these kind of features (and no more)
; uses the same assembly code. These countries are the following:
;  Denmark, Finland, France, Germany, Norway, Spain, Sweden.

cProc   ToAscii,<PUBLIC,FAR>,<si,di>
    ParmW   VirtKey
    ParmW   Scancode
    ParmD   pKeyState
    ParmD   pState
    ParmW   KeyFlags
cBegin  ToAscii
        mov     ax,VirtKey          ; get the virtual key
        mov     dx,ax
        mov     bx,Scancode         ; get the scancode
        or      bx,bx
        jge     toasc1              ; Make, continue
        cmp     al,VK_MENU          ; Break, is it the Alt key?
        jnz     ToAsc0              ; No, continue

; We are in the Alt Break condition, in this case we provide the
; value generated (if any) on the numeric keypad. If this value
; was entered preceeded by a 0 the value will be considered
; Ansi, either it will be IBM character set.

        les     di,pState           ; Yes, get state block pointer
        xor     ax,ax
        xchg    al,[fAnsi]          ; clear ANSI flag, get state
        cmp     byte ptr es:[di],ah ; Anything in buffer
        jz      ToAsc0              ; No, ignore this key
        or      al,al
        jnz     ToAscA
        regptr  esdi,es,di
        cCall   OemToAnsi,<esdi,esdi>
ToAscA: jmp     ToAsc6

ToAsc0: xor     ax,ax
        jmp     ToAscX

; Detect Alt (must be alone)

ToAsc1:
        les     si,pKeyState    ; get pointer to key state
        mov     ch,80H

        test    es:[si+VK_MENU],ch              ; Is alt key down?
        jz      ToAsc2b                         ; jump if not
        test    es:[si+VK_CONTROL],ch           ; is Control down?
        jnz     ToAsc2                          ; yes, dont do it
        test    es:[si+VK_SHIFT],ch             ; is Shift key down?
        jnz     ToAsc2b                         ; yes, dont do it
        test    byte ptr KeyFlags,1             ; is menu displayed
        jnz     ToAsc2b

; Alt is down but not Control or Shift, if now user uses the numeric
; keypad we'll compose a number.

        cmp     bl,83                           ; is it on keypad?
        jae     ToAsc2b                         ; no...
        sub     bl,71
        jb      ToAsc2b
        xor     bh,bh
        mov     bh,[bx+KeyPadDigits]
        cmp     bh,-1                           ; no...
        jz      ToAsc2b

        les     di,pState       ; Get pointer to state block
        mov     al,10
        mov     bl,es:[di]
        mul     bl
        add     al,bh           ; Add in this digit
        mov     es:[di],al
        cmp     al,bl
        jnz     ToAsc0
        mov     [fAnsi],1       ; is started with zero, then do ANSI
        jmp     ToAsc0

; we know here Ctrl Alt keys are down, depending on the country we'll
; scan a Ctrl-Alt table to get U.S. standard characters.

ToAsc2:
if CtrlAltEscape
        mov     [CtrlAlt_default],al   ; prepare to scan table
        mov     bx,dataOffset CtrlAlt_table-3
ToAsc2a:
        add     bx,3
        cmp     [bx],al                ; look for valid code (+Ctrl Alt)
        jnz     ToAsc2a
        mov     ax,[bx+1]              ; code found, test for Shift
        test    es:[si+VK_SHIFT],ch
        jz      ToAsc3z
        xchg    al,ah                  ; if Shift xchange code
        jmp     short ToAsc3z
ToAsc2b:
else
ToAsc2a:
ToAsc2b:
endif

; Here takes place the standard Virtual key -> ANSI translation
; We take care of Control and Shift conditions.

        mov     [VirtualDefault],al
        mov     bx,dataOffset AsciiTable-4
ToAsc3:
        add     bx,4
        cmp     [bx],al
        jnz     ToAsc3
        test    es:[si+VK_CONTROL],ch           ; is control key down?
        jz      ToAsc3a                         ; no, forget it.
        mov     al,[bx+3]                       ; yes, it's a control key
        cmp     al,-1                           ; if -1, can't be
        jz      ToAsc3zz
        or      al,al                           ; is shift required?
        jns     ToAsc3z                         ; no, return the key
        and     al,01111111b                    ; clear hi bit
        test    es:[si+VK_SHIFT],ch
        jz      ToAsc3zz
ToAsc3z:
        jmp     ToAsc4z                         ; Ctrl code got.
ToAsc3zz:
	jmp     Toasc0                          ; Crtl code was invalid
ToAsc3a:
        mov     ax,[bx+1]       ; get translation for base and shifted
        test    es:[si+VK_SHIFT],ch
        jz      ToAsc4
        xchg    al,ah
ToAsc4:

; CapsLock stuff, all countries playing with dead keys need also to
; do special things with CapsLock (ie: some accents character are
; also affected by CapsLock

        test    byte ptr es:[si+VK_CAPITAL],1
        jz      ToAsc4a
        cmp     dl,VK_A
        jb      ToAsc3d
        cmp     dl,VK_Z
        ja      ToAsc3d
        xchg    al,ah
        jmp     short ToAsc4a
ToAsc3d:
if Do_dead_key
;   now, we need to look for extra characters affected by Caps Lock
; 
        mov     bx,dataOffset capital_table-1
ToAsc3y:
        inc     bx
	cmp     byte ptr [bx],-1   ; end of table?
	jz      ToAsc4a            ; skip if yes
        cmp     [bx],dl            ; test if virtual key is affected
        jnz     ToAsc3y            ; no, continue to scan table
        xchg    al,ah              ; yes, reverse the codes
endif

; Dead key stuff, concerns all the countries where accentuated characters
; are composed in 2 keystrokes (accent + character).
; The accent alone is got by typing Accent + Space, however there is an
; exception for the german keyboard (see its specific file).
; If the character doesn't accept this accent (no combination), ToAscii
; will return the accent AND the character.
; 
ToAsc4a:
if Do_dead_key
	mov     bx,dataOFFSET dead_keys-1
ToAsc4aa:
	inc	bx
	mov	ah,[bx]            ; get valid dead key code
	cmp	ah,-1              ; end of table test
	jz	ToAsc4b            ; skip if was not dead key
	cmp	al,ah
	jnz	ToAsc4aa           ; scan table until end or found
        mov     [dead_key],al      ; store dead key as a flag
        les     di,pState
        stosb                      ; store it also into pState
        mov     ax,-1              ; with negative count (means dead key)
        jmp     ToAscX
ToAsc4b:
        mov     ah,[dead_key]
        or      ah,ah           ; is it a dead key already stored ?
        jz      ToAsc4z         ; no, jump
        mov     [dead_key_default],al ; yes, prepare to scan table
        mov     bx,dataOffset dead_key_table-5
ToAsc4c:
        add     bx,5
        cmp     [bx],al         ; look for valid entry
        jnz     ToAsc4c
        cmp     byte ptr [bx+2],-1 ;is it valid ? (any valid has at least acute)
        jz      ToAsc4zb        ; no, jump
        mov     byte ptr [dead_key],0 ; yes, reset dead key flag
        cmp     ah,grave        ; is it grave accent ?
        jnz     ToAsc4d         ; no
        mov     al,[bx+1]       ; yes
        jmp     short ToAsc4f
ToAsc4d:
        cmp     ah,acute        ; is it acute accent ?
        jnz     ToAsc4e         ; no
        mov     al,[bx+2]       ; yes
        jmp     short ToAsc4f
ToAsc4e:
        cmp     ah,circumflex   ; is it circumflex ?
        mov     al,[bx+3]
        jz      ToAsc4f         ; yes, jump
        mov     al,[bx+4]       ; no, so it is umlaut

; Here, we need to make a weird thing, in fact "y umlaut" has
; the code 0FFh that means also -1 (used to indicate no Xlation).
; So we'll isolate this situation to allow this code to be sent.

;  Here we know we have umlaut, so just test if character= 'y'

        cmp     byte ptr[bx],'y'
        jnz     ToAsc4f         ; no, it is not 'y umlaut'
        mov     al,0FFh         ; yes , store this code
        jmp     short ToAsc5
ToAsc4f:
        cmp     al,-1           ; is the new code valid ?
        jnz     ToAsc5          ; yes, jump
        mov     al,[bx]         ; no, get back original code
        jmp     short ToAsc4zc  ; and store them

ToAsc4zb:
        cmp     al,-1           ; is it a valid code ?
        jz     ToAsc4za         ; no, just make DoLights
ToAsc4zc:
        mov     byte ptr [dead_key],0 ; reset dead key flag
        les     di,pState
        push    ax
        mov     al,ah
        xor     ah,ah
        stosw
        pop     ax
        xor     ah,ah
        stosw                   ; store accent and 2nd code
        mov     ax,2            ; length= 2
        jmp     short ToAscX
endif

; we have a normal plain Ansi code, however if this code is invalid (-1)
; we just set the lights, else we return the result.

ToAsc4z:
        xor     ah,ah
        cmp     al,-1
        jnz     ToAsc5
ToAsc4za:
        cCall    DoLights,pKeyState    ; turn on/off lights
        jmp     ToAsc0

ToAsc5:
        les     di,pState       ; Get pointer to state block
        stosb
ToAsc6:
        mov     ax,1            ; Return length
ToAscX:

; Here with result in AX and possibly the state block

cEnd    ToAscii


sEnd    CODE            ; End of code segment

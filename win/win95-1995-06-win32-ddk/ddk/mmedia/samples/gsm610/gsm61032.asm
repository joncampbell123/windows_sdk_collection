;*==========================================================================;
;*
;*  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
;*  ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED
;*  TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR
;*  A PARTICULAR PURPOSE.
;*
;*  Copyright (C) 1993 - 1995 Microsoft Corporation. All Rights Reserved.
;*
;*--------------------------------------------------------------------------;
;*
;*  gsm61032.asm
;*
;*  Description:
;*      This file contains 32-bit assembly routines for some of the most
;*      computationally intensive routines.  All of these routines have a
;*      counterpart in gsm610.c; please refer to that file for a more
;*      comprehensive explanation of the algorithms used.
;*
;*==========================================================================;

;
;   Note we're use the 'C' calling convention cuz these functions are
;   prototyped as extern 'C' in the C code.
;
.386
.MODEL FLAT, C

    .xlist

    include gsm610.inc
    
    .list

;=======================================================;
.CODE

;-------------------------------------------------------;
;
;   external tables
;       
    EXTERN DLB:WORD
    EXTERN QLB:WORD
    
;-------------------------------------------------------;
;
; encodeLTPAnalysis
;
; Entry:
;       PSTREAMINSTANCE psi
;       LPINT       d
;       LPINT       pNc
;       LPINT       pbc
;
; Returns:
;       void
;
; Registers Preserved:
;       TBD
;
; Registers Destroyed:
;       TBD
;
; Calls:
;   None
;
;-----------------------------------------------------------------------;

encodeLTPAnalysis PROC PUBLIC USES ebx esi edi, \
    psi:PTR STREAMINSTANCE,     \
    d:PTR WORD,         \
    pNc:PTR WORD,       \
    pbc:PTR WORD


    LOCAL   wt[40]:WORD
    LOCAL   lambda:WORD
    LOCAL   l_max:DWORD
    LOCAL   Nc:WORD
    LOCAL   Scal:WORD
    LOCAL   l_power:DWORD
    LOCAL   R:WORD
    LOCAL   S:WORD

    ;----
    ; Search for optimum scaling of d[0..39]
    ;----
    
    ;
    ; Loop thru all d[] looking for max abs value
    ;

    ; for i=0 to 39, (40 iterations)
    mov     ecx,40
    
    mov     esi,d               ; esi -> d[i]

    xor     edx,edx             ; dx will get max abs value
loopi:
    lodsw                       ; get next d[i]
    ; Take absolute value of d[i]
    test    ax,8000h            ; is this negative?
    jz      @F                  ;   no
    neg     ax                  ; negate
    jns     @F                  ; still negative? (if was -32768)
    mov     ax,7FFFh            ;   yes, make max positive value
@@:                             ; ax = abs( d[i] )
    ; See if this is new max
    cmp     ax,dx
    jb      @F
    mov     dx,ax
@@:                             ; dx has max thru this iteration
    loop    loopi

    ;
    ; Determine amount of down scaling
    ;
    ; If I'm interpreting the algorithm in the GSM 6.10
    ; spec correctly, then our goal is to scal d[] such that
    ; there are no values with magnitude >= 512.
    ; So, we find the first highest order '1' bit in dmax
    ; and from this determine how many right shifts are
    ; necessary to get this down to bit 8.
    ;
    bsr     cx,dx                   ; cx gets highest bit position = '1'
    jz      short NoScal            ; dmax must've been == 0
    sub     cl,8                    ; shifts required to get down to bit 8
    jnc     short GotScal
NoScal:
    xor     cx,cx
GotScal:                            ; cl has num rt shfts to apply to d[]
    mov     Scal,cx
    ;
    ; At this point, CL is number of shifts to apply
    ; to each member of d[].  Scaled values are stored in wt[].
    ;
    ; esi -> d[],  edi -> wt[]
    mov     esi,d
    lea     edi,wt[0]
    ; edx will count thru 40 iterations to process all of d[]
    mov     edx,40
loopScald:
    lodsw
    sar     ax,cl
    stosw
    dec     edx
    jnz     loopScald
    
    ;-----
    ; Search for max cross correlation and coding of LTP lag
    ;-----

    ; init l_max, Nc
    xor     eax,eax
    mov     l_max,eax
    mov     Nc,40
    ; make ptr to dp[120+0-40] in edi (this is 1st element of dp[] used)
    mov     ebx,psi
    mov     edi,(120+0-40)*2
    lea     edi,(tagSTREAMINSTANCE ptr [ebx]).dp[edi]
    ; make ptr to wt[0] in [si]
    lea     esi,wt[0]
    ; for lambda=40 to 120, (81 iterations).  our counter lambda will count
    ; down from 81 to make it easier to update the counter.  so the real
    ; lambda is 121 minus our counter
    mov     lambda,120-(40-1)
looplambda:
    xor     edx,edx             ; edx will accumulate the sum
    ; for k=0 to 39, (40 iterations)
    mov     ecx,40
loopk:
    ; compute l_mult( wt[k], dp[120+k-lambda] )
    lodsw                       ; eax gets wt[k], also si++ for next wt[k]
    cwde
    movsx   ebx,word ptr [edi]  ; ebx gets dp[120+k-lambda]
    imul    eax,ebx             ; eax has result of l_mult(...)
    add     edx,eax             ; accumulate sum in edx
    add     edi,2               ; next dp[120+k-lambda]
    loop    loopk               ; next k

    sub     edi,(40*2)+(1*2)    ; reset di to pnt to dp[120+0-(++lambda)]
    sub     esi,40*2            ; reset si to pnt to wt[0]

    ; the new max?
    cmp     edx,l_max
    jle     @F
    ; store the max and the associated lambda
    mov     l_max,edx           ; the max up thru this iteration of loopk
    mov     eax,121             ; to figure out the real lambda from
    sub     ax,lambda           ;   our counter we need to do 121-lambda.
    mov     Nc,ax               ; save lambda
@@:
    dec     lambda
    jnz     looplambda
    shl     l_max,1             ; cuz l_mult() should always dbl result

    ;
    ; Rescale l_max
    ;
    mov     ecx,6
    sub     cx,Scal
    sar     l_max,cl

    ;----
    ; Init working array wt[0..39] to scaled copy of lagged dp[]
    ;----

    ; make ptr to dp[120+0-Nc] in esi.  this is first element of dp[]
    ; that we use in the loop
    mov     ebx,psi
    movsx   esi,Nc
    neg     esi
    add     esi,120
    shl     esi,1
    lea     esi,(tagSTREAMINSTANCE ptr[ebx]).dp[esi]
    ; make ptr to wt[0] in edi.  (again for use in loop)
    lea     edi,wt
    ; we will shift all elements of dp[] by 3 to obtain wt[]
    mov     ecx,3
    ; 40 elements in dp[] and wt[]
    mov     edx,40
loopScaldp:
    lodsw
    sar     ax,cl
    stosw
    dec     edx
    jnz     loopScaldp

    ;
    ; Return Nc to caller thru pNc
    ;
    mov     edi,pNc
    mov     ax,Nc
    stosw

    ;----
    ; Compute power of dp[] (now scaled in and lagged wt[])
    ;----
    
    ; make ptr to wt[0] in esi
    lea     esi,wt
    ; for 40 elements of wt[]
    mov     ecx,40
    xor     eax,eax         ; eax will get wt[i]
    xor     edx,edx         ; edx will accumulate sum
loopPower:
    lodsw
    cwde
    imul    eax,eax
    add     edx,eax
    loop    loopPower
    shl     edx,1           ; cuz l_mult() should dbl result
    mov     l_power,edx     ; save the power
    
    ;----
    ; Normalization of l_max and l_power
    ;----
    mov     edi,pbc
    
    cmp     l_max,0
    jg      @F
    xor     eax,eax         ; *pbc = 0
    stosw
    jmp     GetOuttaHere
@@:
    mov     eax,l_max
    cmp     eax,l_power
    jl      @F
    mov     eax,3           ; *pbc = 3
    stosw
    jmp     GetOuttaHere
@@:
    ; norm(l_power)
    mov     eax,l_power     ; I think l_power will definitely be >0 here
    bsr     ecx,eax
    mov     eax,30
    sub     eax,ecx         ; AX <- num left shifts to get bit to bit 14
    jnc     @F
    xor     eax,eax
@@:
    mov     cl,al
    mov     eax,l_max
    shl     eax,cl
    push    eax
    add     sp,2
    pop     ax
    mov     R,ax
    mov     eax,l_power
    shl     eax,cl
    push    eax
    add     sp,2
    pop     ax
    mov     S,ax
    ; EDI still pnts to bc (*pbc)
    xor     esi,esi             ; for esi = 0 to 3
loopCodeLTPGain:
    mov     [edi],si            ; *pbc = (LOWORD(esi))
    cmp     esi,3
    je      GetOuttaHere
    mov     ax,S
    shl     esi,1
    imul    DLB[esi]
    shr     esi,1
    shld    dx,ax,1
    cmp     R,dx
    jle     GetOuttaHere
    inc     esi
    jmp     loopCodeLTPGain     ; next esi
    
    
GetOuttaHere:
    ret
        
encodeLTPAnalysis EndP

;-------------------------------------------------------;
;
; Compd
;
; Entry:
;       PSTREAMINSTANCE psi
;       LPINT       rp
;       LPINT       s
;       LPINT       d
;       UINT        k_start
;       UINT        k_end
;
; Returns:
;       void
;
; Registers Preserved:
;       TBD
;
; Registers Destroyed:
;       TBD
;
; Calls:
;   None
;
;-----------------------------------------------------------------------;

Compd PROC PUBLIC USES ebx esi edi, \
    psi:PTR STREAMINSTANCE,     \
    rp:PTR WORD,        \
    s:PTR WORD,         \
    d:PTR WORD,         \
    k_start:WORD,           \
    k_end:WORD

    LOCAL   k:WORD
    LOCAL   sav:WORD
    LOCAL   da:WORD
    LOCAL   temp:WORD

    ; k will count down til we get a carry
    mov     ax,k_end
    sub     ax,k_start
    mov     k,ax

    movzx   ebx,k_start         ; k = k_start
    shl     ebx,1               ; to index integers

    ; point d at d[k_start] and bx at s[k_start]
    add     d,ebx
    add     ebx,s

    ; edi -> u[0]
    mov     edi,psi
    lea     edi,(tagSTREAMINSTANCE ptr[edi]).u
    ; esi -> rp[1]
    mov     esi,rp
    add     esi,2
    
loopk:
    ; sav = da = s[k]
    mov     ax,[ebx]
    mov     da,ax
    mov     sav,ax

    mov     ecx,8               ; for i=1 to 8
    push    esi
    push    edi
loopi:  
    mov     ax,[esi]            ; rp[i]
    imul    da
    add     ax,4000h            ; rounding
    adc     dx,0
    shld    dx,ax,1             ; DX = mult_r( rp[i], da )
    mov     ax,[edi]            ; u[i-1]
    add     ax,dx               ; temp = add(u[i-1], mult_r(...))
    jno     @F
    mov     ax,7FFFh
    js      @F
    mov     ax,8000h
@@:
    mov     temp,ax             ; temp

    mov     ax,[esi]            ; rp[i]
    imul    word ptr [edi]      ;  times u[i-1]
    add     ax,4000h            ; rounding
    adc     dx,0
    shld    dx,ax,1             ; DX = mult_r(...)
    add     da,dx               ; di = add(di, mult_r(...))
    jno     @F
    mov     da,7FFFh
    js      @F
    mov     da,8000h
@@:

    mov     ax,sav              ; u[i-1] = sav
    stosw                       ; also di++ to get next u[i-1]
    add     esi,2               ; si++ for next rp[i]

    mov     ax,temp             ; sav = temp
    mov     sav,ax

    loop    loopi               ; next i

    mov     esi,d
    mov     dx,da
    mov     word ptr [esi],dx   ; d[k] = di

    add     d,2                 ; next d[k]
    add     ebx,2               ; next s[k]

    pop     edi                 ; reset to u[0]
    pop     esi                 ; reset to rp[1]

    sub     k,1                 ; next k (remember, DEC won't update CY!)
    jnc     loopk
    
    ret

Compd endp

;-------------------------------------------------------;
;
; Compsr
;
; Entry:
;       PSTREAMINSTANCE psi
;       LPINT       wt
;       LPINT       rrp
;       UINT        k_start
;       UINT        k_end
;       LPINT       sr
;
; Returns:
;       void
;
; Registers Preserved:
;       TBD
;
; Registers Destroyed:
;       TBD
;
; Calls:
;   None
;
;-----------------------------------------------------------------------;

Compsr PROC PUBLIC USES ebx esi edi, \
    psi:PTR STREAMINSTANCE, \
    wt:PTR WORD,    \
    rrp:PTR WORD,   \
    k_start:WORD,       \
    k_end:WORD,         \
    sr:PTR WORD

    LOCAL   k:WORD
    LOCAL   sri:WORD

    ; setup ptr to s[k_start], ds:[sr] -> sr[k_start]
    movzx   ebx,k_start
    shl     ebx,1           ; scale for integer index
    add     sr,ebx

    ; setup ptr to wt[k_start], [ebx] -> wt[k_start]
    add     ebx,wt

    ; setup ptr to rrp[8], esi -> rrp[8]
    mov     esi,8*2
    add     esi,rrp

    ; setup ptr to v[7], edi -> v[7]
    mov     edi,psi
    lea     edi,(tagSTREAMINSTANCE ptr[edi]).v[7*2]

    ; let k count down till carry, for k=k_start to k_end
    mov     ax,k_end
    sub     ax,k_start
    mov     k,ax

loopk:
    ; sri = wt[k]
    mov     ax,[ebx]
    mov     sri,ax

    ; 8 iterations of inner loop, for i=1 to 8
    mov     ecx,8

loopi:
    ; compute mult_r( rrp[9-i], v[8-i] )
    mov     ax,[esi]            ; rrp[9-i]
    imul    word ptr [edi]      ; times v[8-i]
    add     ax,4000h            ; for rounding
    adc     dx,0
    shld    dx,ax,1             ; dx is our result
    ; sri = sub( sri, mult_r(...) )
    mov     ax,sri
    sub     ax,dx
    jno     @F
    mov     ax,7FFFh
    js      @F
    mov     ax,8000h
@@: mov     sri,ax
    ; compute mult_r( rrp[9-i], sri )
    imul    word ptr [esi]
    add     ax,4000h
    adc     dx,0
    shld    dx,ax,1             ; dx is result
    ; add( v[8-1], mult_r(...) )
    mov     ax,[edi]
    add     ax,dx
    jno     @F
    mov     ax,7FFFh
    js      @F
    mov     ax,8000h
@@:                             ; ax is result
    ; v[9-i] = add(...)
    mov     [edi+2],ax

    ; next i
    sub     esi,2           ; next rrp[9-i]
    sub     edi,2           ; next v[8-i]
    loop    loopi

    ; restore ptrs to rrp[8] and v[7]
    add     edi,16
    add     esi,16

    ; sr[k] = sri
    mov     ax,sri
    mov     edx,sr
    xchg    edx,ebx
    mov     word ptr [ebx],ax
    xchg    ebx,edx
    ; v[0] = sri
    mov     [edi-7*2],ax

    ; next k
    add     sr,2            ; next sr[k]
    add     ebx,2           ; next wt[k]
    sub     k,1
    jnc     loopk

    ret

Compsr endp

;-------------------------------------------------------;
;
; CompACF
;
; Entry:
;       PSTREAMINSTANCE psi
;       LPINT       s
;       LPLONG      l_ACF
;
; Returns:
;       void
;
; Registers Preserved:
;       TBD
;
; Registers Destroyed:
;       TBD
;
; Calls:
;   None
;
;-----------------------------------------------------------------------;

CompACF PROC PUBLIC USES ebx esi edi, \
    s:PTR WORD,         \
    l_ACF:PTR DWORD

    LOCAL   smax:WORD
    LOCAL   scalauto:WORD
    LOCAL   k:WORD

    ;----
    ; Dynamic scaling of array s[0..159]
    ;----

    ;
    ; Search for the maximum
    ;
    
    ; point ds:si at s[], init smax, prepare loop counter
    mov     esi,s
    mov     smax,0
    mov     ecx,160
loopsmax:
    ; get next s[k] and take absolute value
    lodsw
    test    ax,8000h        ; is it negative?
    jz      @F
    neg     ax              ; negate to make positive
    test    ax,8000h        ; still negative (if was -32768)
    jz      @F
    mov     ax,7FFFh        ; make max positive
@@:                         ; now ax has abs(s[k])
    ; see if this is our new smax
    cmp     ax,smax
    jbe     @F
    mov     smax,ax
@@:
    ; next k
    loop    loopsmax

    ;----
    ; computation of scaling factor, scaling of array s
    ;
    ; if I'm interpreting the spec correctly, the goal here is to
    ; scale array s[] such that all values are less than 800h in
    ; magnitude.  when we scale the array, we round up.
    ;----

    ; determine scaling factor scalauto
    mov     scalauto,0          ; if smax==0 then scalauto:=0
    mov     ax,smax
    bsr     cx,ax               ; CX := bit position of highest '1'
    jz      Gotscalauto         ; smax must have been zero if we jump
    sub     cx,10               ; rt shfts to get that highest '1' to bit 10
    jc      Gotscalauto         ; jump if highest '1' was already below bit 10
    mov     scalauto,cx         ; save out scale factor
Gotscalauto:

    ; scale the array s[] with rounding
    mov     cx,scalauto         ; rt shfts to apply to s[]
    jcxz    skipscals           ; forget it if scal is 0
    mov     esi,s               ; pnt ds:si at s[]
    mov     edi,s               ; also pnt es:di at s[]
    mov     edx,160             ; 160 elements in s[]
loopscals:
    lodsw                       ; get next s[k]
    sar     ax,cl               ; scale s[k]
    adc     ax,0                ; round up
    stosw                       ; save new s[k]
    dec     edx                 ; next k
    jnz     loopscals
skipscals:

    ;----
    ; compute l_ACF[]
    ;----

    mov     edi,l_ACF           ; pnt es:di at l_ACF[k]
    
    mov     k,0
loopACFstart:
    cmp     k,8
    ja      loopACFend
    
    mov     ecx,160             ; cx counts loop i=k to 159
    sub     cx,k

    mov     ebx,s               ; pnt ebx at s[i-k]
    movzx   esi,k               ; pnt esi at s[i]
    shl     esi,1
    add     esi,ebx

    push    edi
    xor     edi,edi             ; init l_ACF[k]
loopACF:
    ; compute l_mult(s[i], s[i-k])
    lodsw                       ; eax = s[i]
    cwde
    movsx   edx,word ptr [ebx]
    add     ebx,2
    imul    eax,edx
    ; accumulate results of l_mult(...) in l_ACF[k]
    add     edi,eax
    loop    loopACF
    shl     edi,1               ; cuz l_mult should be doing << 1 on result
    mov     eax,edi
    pop     edi
    mov     dword ptr es:[edi],eax  ; store l_ACF[k]


    add     edi,4               ; next l_ACF[k]
    add     k,1                 ; next k
    jmp     loopACFstart
loopACFend:

    ;----
    ; rescaling of array s[]
    ;----
    movzx   ecx,scalauto
    jcxz    skiprescals

    mov     esi,s
    mov     edi,s
    
    mov     edx,160

looprescal:
    lodsw
    shl     ax,cl
    stosw
    dec     edx
    jnz     looprescal
skiprescals:

    ;----
    ;
    ;----
    ret

CompACF endp

;-------------------------------------------------------;
;
; decodePostProc
;
; Entry:
;       PSTREAMINSTANCE psi
;       LPINT       sr
;       LPINT       srop
;
; Returns:
;       void
;
; Registers Preserved:
;       TBD
;
; Registers Destroyed:
;       TBD
;
; Calls:
;   None
;
;-----------------------------------------------------------------------;

decodePostproc PROC PUBLIC USES ebx esi edi, \
    psi:PTR STREAMINSTANCE,         \
    sr:PTR WORD,            \
    srop:PTR WORD

    ; get psi-msr
    mov     esi,psi
    mov     bx,(tagSTREAMINSTANCE ptr[esi]).msr

    ; make ptrs to sr[] and srop[]
    mov     esi,sr
    mov     edi,srop

    mov     ecx,160         ; 160 elements in sr[] and srop[]
loopk:
    ; compute mult_r (psi->msr, 28180)
    mov     ax,28180
    imul    bx
    add     ax,4000h
    adc     dx,0
    shld    dx,ax,1
    ; add( sr[k], mult_r(..) )
    lodsw                   ; get sr[k]
    add     ax,dx
    jno     @F
    mov     ax,7FFFh
    js      @F
    inc     ax
@@:
    ; this is new msr, put back in bx
    mov     bx,ax
    ; upscaling
    shl     ax,1
    jno     @F
    mov     ax,7FFFh
    js      @F
    inc     ax
@@:
    ; truncation
    and     ax,0FFF8h
    ; save in srop[k]
    stosw
    
    ; next k
    loop    loopk

    ; store new msr
    mov     esi,psi
    mov     (tagSTREAMINSTANCE ptr [esi]).msr,bx

    ;----
    ;
    ;----
    ret

decodePostProc endp

;-------------------------------------------------------;
;
; decodeLTP
;
; Entry:
;       PSTREAMINSTANCE psi
;       int         bcr
;       int         Ncr
;       LPINT       erp
;
; Returns:
;       void
;
; Registers Preserved:
;       TBD
;
; Registers Destroyed:
;       TBD
;
; Calls:
;   None
;
;-----------------------------------------------------------------------;

decodeLTP PROC PUBLIC USES ebx esi edi, \
    psi:PTR STREAMINSTANCE, \
    bcr:WORD,           \
    Ncr:WORD,           \
    erp:PTR WORD

    ;
    ; make ptr to psi in ebx
    ;
    mov     ebx,psi

    ;----
    ; check limits of Ncr
    ;----
    mov     ax,Ncr
    cmp     ax,40
    jl      getnrp
    cmp     ax,120
    jbe     dontgetnrp
getnrp:
    mov     ax,(tagSTREAMINSTANCE ptr [ebx]).nrp
dontgetnrp:
    ; save limited Ncr
    mov     (tagSTREAMINSTANCE ptr [ebx]).nrp,ax
    mov     Ncr,ax
    
    ;----
    ; decoding of LTP gain bcr
    ;----
    movzx   esi,bcr
    shl     esi,1
    mov     ax,QLB[esi]
    mov     bcr,ax

    ;----
    ; computation of reconstructed short term residual signal drp[0..39]
    ;----

    ; make ptr to drp[120] in edi
    lea     edi,(tagSTREAMINSTANCE ptr [ebx]).drp[120*2]

    ; make ptr to drp[120-Nr] in ebx
    mov     ebx,edi
    movzx   eax,Ncr
    sub     ebx,eax
    sub     ebx,eax

    ; make ptr to erp[] in esi
    mov     esi,erp

    ; cx will count loop for k=0 to 39
    mov     ecx,40

loopk:
    ; compute mult_r( brp, drp[120+k-Nr] )
    mov     ax,bcr
    imul    word ptr [ebx]
    add     ebx,2           ; next drp[120+k-Nr]
    add     ax,4000h
    adc     dx,0
    shld    dx,ax,1
    ; compute add( erp[k], mult_r(..) )
    lodsw                   ; get next erp[k]
    add     ax,dx
    jno     @F
    mov     ax,7FFFh
    js      @F
    inc     ax
@@:
    ; drp[120+k] = add(..)
    stosw               ; store next drp[120+k]

    ; next k
    loop    loopk

    ;----
    ; update of the short term reconstructed residual signal drp[-120..-1]
    ;----

    ; copy drp[120-80+k] to drp[120-120+k]

    ; make ptrs to each starting point in array drp
    mov     ebx,psi
    lea     esi,(tagSTREAMINSTANCE ptr[ebx]).drp[(120- 80)*2]
    lea     edi,(tagSTREAMINSTANCE ptr[ebx]).drp[(120-120)*2]
    ; copy 120 words
    mov     ecx,120/2
    rep     movsd

    ;----
    ;
    ;----
    ret

decodeLTP endp

END

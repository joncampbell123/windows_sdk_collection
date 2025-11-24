;****************************************************************************
;                                                                           *
; THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
; KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
; IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
; PURPOSE.                                                                  *
;                                                                           *
; Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
;                                                                           *
;****************************************************************************

;*  ENTRY.ASM
;*
        .386
	.MODEL	flat

        INCLUDE winbase.inc
        INCLUDE winerror.inc

;extern dwDosSpoolPortMask :DWORD
;extern dwDosSpoolPortFlag :DWORD
CHECK_LPT   EQU     00000001H
CHECK_COM   EQU     00000002H

        .CODE

;--------------------------Public-Routine-------------------------------;
; BOOL IsLptOrCom(LPSTR)
;
; It takes a DWORD which is actually the first 4 bytes of a string.
; For example,
; LPT1 is 0x3154504c '1','T','P','L'
; COM1 is 0x314d4f43 '1','M','O','C'
; lpt1 is 0x31747046 '1','t','p','l'
; com1 is 0x316d6f63 '1','m','o','c'
;
; And it checks if the DWORD is between LPT1 and LPT9 or between COM1 and COM9.
;
; Return: TRUE if it is in LPT1 - LPT9 or COM1 - COM9. FALSE otherwise.
;
; Warnings:
;
; Effects:
;
; History:
;       ccteng, 10/26/94
;
;-----------------------------------------------------------------------;
;BOOL
CheckLptOrCom PROC APIENTRY USES esi,
        lpName:LPBYTE,
        dwFlag:DWORD

        mov     esi,lpName

        invoke  lstrlen,esi

        cmp     eax,5
        jne     short CheckLptOrCom_Not_5

        ; check if the last character is ':' if strlen == 5

        cmp     byte ptr [esi+4],':'
        jne     short ReturnFalse
        jmp     short CheckLptOrCom_Guts

CheckLptOrCom_Not_5:
        ; check if strlen == 4 if strlen != 5

        cmp     eax,4
        jne     short ReturnFalse

CheckLptOrCom_Guts:
        mov     eax,dword ptr [esi]

        ; LPT1 is 0x3154504c '1TPL'
        ; COM1 is 0x314d4f43 '1MOC'
        ; lpt1 is 0x3174706c '1tpl'
        ; com1 is 0x316d6f63 '1moc'

        or      eax,00202020H   ;convert to lower case
        mov     cl,8
        rol     eax,cl

        mov     ecx,dwFlag
        test    ecx,CHECK_LPT
        jz      short CheckCom

        ; lpt1 is now 0x74706c31 'tpl1'
        ; com1 is now 0x6d6f6331 'moc1'

        cmp     eax,74706c31H
        jb      short CheckCom

        cmp     eax,74706c39H
        ja      short CheckCom

        ; it's LPTx or (lptx)

        jmp     short ReturnTrue

CheckCom:
        test    ecx,CHECK_COM
        jz      short ReturnFalse

        cmp     eax,6d6f6331H
        jb      short ReturnFalse

        cmp     eax,6d6f6339H
        ja      short ReturnFalse

        ; it's COMx or (comx)

ReturnTrue:
        mov     eax,1
        jmp     short ReturnAx

ReturnFalse:
        xor     eax,eax

ReturnAx:
        ret

CheckLptOrCom ENDP


IF 0

IsLptOrCom PROC APIENTRY USES esi,
        lpName:LPBYTE

        mov     esi,lpName

        invoke  lstrlen,esi

        cmp     eax,5
        jne     short IsLptOrCom_Not_5

        ; check if the last character is ':' if strlen == 5

        cmp     byte ptr [esi+4],':'
        jne     short ReturnFalse
        jmp     short IsLptOrCom_Guts

IsLptOrCom_Not_5:

        ; check if strlen == 4 if strlen != 5

        cmp     eax,4
        jne     short ReturnFalse

IsLptOrCom_Guts:
        mov     eax,dword ptr [esi]

        ; LPT1 is 0x3154504c '1TPL'
        ; COM1 is 0x314d4f43 '1MOC'
        ; lpt1 is 0x3174706c '1tpl'
        ; com1 is 0x316d6f63 '1moc'

        or      eax,00202020H   ;convert to lower case
        mov     cl,8
        rol     eax,cl

        ; lpt1 is now 0x74706c31 'tpl1'
        ; com1 is now 0x6d6f6331 'moc1'

        cmp     eax,74706c31H
        jb      short CheckCom

        cmp     eax,74706c39H
        ja      short CheckCom
        jmp     short ReturnTrue

CheckCom:
        cmp     eax,6d6f6331H
        jb      short ReturnFalse

        cmp     eax,6d6f6339H
        ja      short ReturnFalse

ReturnTrue:
        mov     eax,1
        jmp     short ReturnAx

ReturnFalse:
        xor     eax,eax

ReturnAx:
        ret

IsLptOrCom ENDP


IsLpt PROC APIENTRY USES esi,
        lpName:LPBYTE

        mov     esi,lpName

        invoke  lstrlen,esi

        cmp     eax,5
        jne     short IsLpt_Not_5

        ; check if the last character is ':' if strlen == 5

        cmp     byte ptr [esi+4],':'
        jne     short ReturnFalse
        jmp     short IsLpt_Guts

IsLpt_Not_5:

        ; check if strlen == 4 if strlen != 5

        cmp     eax,4
        jne     short ReturnFalse

IsLpt_Guts:
        mov     eax,dword ptr [esi]

        ; LPT1 is 0x3154504c '1TPL'
        ; lpt1 is 0x3174706c '1tpl'

        or      eax,00202020H   ;convert to lower case
        mov     cl,8
        rol     eax,cl

        ; lpt1 is now 0x74706c31 'tpl1'

        cmp     eax,74706c31H
        jb      short ReturnFalse

        cmp     eax,74706c39H
        ja      short ReturnFalse

ReturnTrue:
        mov     eax,1
        jmp     short ReturnAx

ReturnFalse:
        xor     eax,eax

ReturnAx:
        ret

IsLpt ENDP


IsCom PROC APIENTRY USES esi,
        lpName:LPBYTE

        mov     esi,lpName

        invoke  lstrlen,esi

        cmp     eax,5
        jne     short IsCom_Not_5

        ; check if the last character is ':' if strlen == 5

        cmp     byte ptr [esi+4],':'
        jne     short ReturnFalse
        jmp     short IsCom_Guts

IsCom_Not_5:

        ; check if strlen == 4 if strlen != 5

        cmp     eax,4
        jne     short ReturnFalse

IsCom_Guts:
        mov     eax,dword ptr [esi]

        ; COM1 is 0x314d4f43 '1MOC'
        ; com1 is 0x316d6f63 '1moc'

        or      eax,00202020H   ;convert to lower case
        mov     cl,8
        rol     eax,cl

        ; com1 is now 0x6d6f6331 'moc1'

        cmp     eax,6d6f6331H
        jb      short ReturnFalse

        cmp     eax,6d6f6339H
        ja      short ReturnFalse

ReturnTrue:
        mov     eax,1
        jmp     short ReturnAx

ReturnFalse:
        xor     eax,eax

ReturnAx:
        ret

IsCom ENDP

ENDIF


;---------------------------------------------------------
; Stub0
;
; Parameters:
;
; Returns:
;
; Uses regs:
;---------------------------------------------------------
;DWORD
Stub0 PROC APIENTRY

        ret

Stub0 ENDP


        END

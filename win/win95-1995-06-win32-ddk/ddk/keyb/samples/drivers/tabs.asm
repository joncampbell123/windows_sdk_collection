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

        page    ,132
; TABS.ASM -- Translation tables module for keyboard driver
;
;***************************************************************************
;
; This file now contains the multilingual table switcher bootstrap, Inquire
; and NewTable functions.
;
;***************************************************************************

if1
%out .. Tabs.Asm
endif

include keyboard.inc
include vkwin.inc
include vkoem.inc
include lcid.inc

externFP	GlobalReAlloc
externFP	GetPrivateProfileString
externFP	GetProcAddress         
externFP	GetPrivateProfileInt   
externFP	OpenFile
externFP	AllocCStoDSAlias
externFP	FreeSelector
externFP	LocalAlloc
externFP	LocalFree
externFP	LocalReAlloc
externFP	GlobalAlloc
externFP	LocalInit
externFP	GetModuleHandle
ExternFP	RegOpenKey
ExternFP	RegEnumKey
ExternFP	RegCloseKey
ExternFP	RegQueryValue
ExternFP        FindResource
ExternFP        LoadResource
ExternFP        LockResource
ExternFP        FreeResource
ExternFP        GlobalSize
externFP	GlobalFree
ExternFP	GetSystemDirectory
ExternFP	_LOPEN
ExternFP	_LCLOSE
ExternFP	_LREAD
ExternFP	_LLSEEK
externFP	lstrcat
externFP	GlobalSmartPageUnlock
externFP	GlobalUnFix
externFP	GlobalUnWire
externFP	GlobalWire
externFP	GlobalFix
externFP	GlobalSmartPageLock
externFP	InquireEx
externFP	SetCurrentLocale

ExternA         sizeofLCIDIB                               ;init.asm
EXTRN keybd_int:FAR

BUFFSIZE        equ     12                              
PATHLEN         equ     128

	regptr csax, cs, ax
        regptr csbx, cs, bx                      
        regptr cscx, cs, cx              
        regptr csdi, cs, di                      
        regptr csdx, cs, dx              
        regptr cssi, cs, si                      
        regptr dsdi, ds, di              
        regptr dssi, ds, si              
        regptr dxbx, dx, bx
        regptr esdi, es, di
        regptr ssax, ss, ax
        regptr ssbx, ss, bx
        regptr sscx, ss, cx
        regptr sssi, ss, si
        regptr ssdi, ss, di


;*****************************************************************************
sBegin DATA

    assumes ds,DATA

;;;;;;;;;externW __acrtused

;***;org     0
;***;
;***;rsrvptrs        label   word
;***;        public  rsrvptrs                ; needed for linker!
;***;        dd      0                       ; (FAR *)NULL
;***;
;***;        dw      5                       ; what does this mean????????
;***;        dw      6 dup (0)               ; heap/atom/sstop/ssmin/ssbum
;***;
;***;org     10H

Vers    db      'WinKB4.00 '            ; 10 bytes long!

        even

;***************************************************************************
;
;       Some variables indicating system/keyboard type.
;
;***************************************************************************

globalB	PCType          0	; determines if it's AT or not.
globalB	PCTypeHigh      0       ; secondary ID byte
globalB	KeyType         0       ; 1..6 -- INIT to ZERO!!
globalB	IsEri           0
globalB	IsOli           0       ; NZ if Olivetti/AT&T ROM BIOS
globalB	OliType  	0       ; NZ if Olivetti-protocol kbd.
                                ; for M24 or AT&T 6300.
                                ; (= SubType in SYSTEM.INI)

globalD LightsAddr,     0       ; old int16h address
globalB	TableType	4
globalB	fKeyType	0	; This has special features
                         	; like...
                         	; kbAltGr: rhs alt = CtrlAlt
                         	; kbShiftLoc: shift not capslo
globalD lpfnLoadString  0

externB enabled

;***************************************************************************
;
;       Variables for NewTab()
;

wCount          dw      0                     	; byte count from OEMANSI.BIN

NAMESIZE        equ     63                    	; size of filename buffer
sNameBuf        db      (NAMESIZE + 1) dup (0)	; filename buffer.
public sNameBuf

externW         iqdNumFunc                    	; number of function keys in
                                              	; KBINFO structure.
NumKeys label byte                            	; Table of no. of virtual
        db      0, 10, 12, 10, 12, 10, 24     	; function keys, indexed by
        ;           1   2   3   4   5   6     	; <-- KeyType.

externD		CurrentLocale
externD         SystemLocale                    ; base lcid value
externW         pSystemLocale                   ; base lcid pointer
externW		pCurrentLocale
externW         pDataEntry
externW         pLCIDList
externW         nKeyboards
ExternW 	sizeLCIDTab                   	
ExternW 	NumLCIDBuffers                	
externW		hlibKbd

externb		inquireData
ExternD         bios_proc                       ; these are in DS for ROM
externB         old_enhanced
ExternD         nmi_vector
sEnd DATA


;*****************************************************************************
;
sBegin CODE

externB CodePage        ; 258 bytes starting here is overwritten.

globalB	bDataUnlocked, 0

sEnd CODE


;***************************************************************************
; This is all in a PRELOAD DISCARDABLE segment.
;
; Anything that needs to stay around should be saved in the DATA segment.
; Temporary variables and constants can be in the NEWTAB segment.
;

createSeg _NEWTAB, NEWTAB, BYTE, PUBLIC, CODE
sBegin NEWTAB
assumes CS,NEWTAB
assumes DS,DATA
.386p


;***************************************************************************
;
; Constants for NewTable(),  in the code segment 'NEWTAB'.

; These are names in SYSTEM.INI
sSysIni         db      "system.ini",0          ; name of file.
sKeyboard       db      "keyboard",0            ; [keyboard] section.
sType           db      "type", 0               ; type 1..6
sSubType        db      "subtype", 0            ; For AT&T/Oli kbd's, mainly
sDllName        db      "layout",0              ; Name of keyboard DLL file.
sOemName        db      "oemansi.bin",0         ; name of Oem/Ansi binary file
sNULL           db      0                       ; NULL string for default

;***************************************************************************
;
;       NewTable()
;
;       Change keyboard tables, if a keyboard table DLL is defined in
;       SYSTEM.INI and the function GetKbdTable() exists and returns
;       successfully.
;
;       This function is passed no parameters by the caller -- it obtains
;       the following from SYSTEM.INI:
;
;             [keyboard]
;               TYPE = 4                        ; 1..6.  4 is enhanced kbd.
;               SUBTYPE = 0                     ; 0 for all but Olivetti
;                                               ; 8086 systems & AT&T 6300+
;               KEYBOARD.DLL = kbdus.dll        ; name of DLL file
;               OEMANSI.BIN = XLATNO.BIN        ; oem/ansi tables file
;
;       The module name of the DLL is expected to be the root of the DLL's
;       file name.  In any event, the module name must be different for
;       each keyboard-table DLL!
;
;***************************************************************************

;
; For now there is a backwards compatability hack, the kbd file can be loaded
; as a table.
;
; Note: this will NOT allow switching back to the US table in the driver
;
cProc   NewTable,<LOADDS, PASCAL, PUBLIC,FAR>,<si,di>
                                                ; LOCAL variables on stack:
cBegin  
	xor	ax, ax
cEnd


;*****************************************************************************

;
; This function unlocks the page if nessacary when doing local allocs/reallocs
;
; this will move the dgroup around, but will only leave a hole in memory if
; the keyboards are added/deleted after the system boots
;
; this function MUST enter with ds == dgroup
;
; exit, ax == handle or zero

cProc KbdLocalAlloc, <PUBLIC, NEAR>

	parmW	wHandle
	parmW	wFlags
	parmW	wSize

	localW	theHandle
	localW	selCStoDS
cBegin
	mov	cx, wHandle
	jcxz	@F

	cCall	LocalReAlloc, <cx, wSize, wFlags>
	or	ax, ax
	jnz	KbdLA_done
	jmp	KLA_DoTheStuff
@@:
	cCall	LocalAlloc, <wFlags, wSize>
	or	ax, ax				; did it work without unlock
	jnz	KbdLA_done			; yep!, bye bye

KLA_DoTheStuff:
	mov	theHandle, ax			; save NULL handle

        mov     ax, _TEXT			; lock the flag
        cCall   AllocCStoDSAlias, <ax>
	mov	selCStoDS, ax
	mov	es, ax
	sti
	mov	es:[bDataUnLocked], 1
	cli

	cCall	GlobalSmartPageUnlock, <ds>
	cCall	GlobalUnFix, <ds>
	cCall	GlobalUnWire, <ds>

	cCall	GlobalSize, <ds>
	add	ax, wSize
GM_MOVEIT equ (GMEM_ZEROINIT or GMEM_MOVEABLE)
	cCall	GlobalReAlloc, <ds, 0, ax, GM_MOVEIT>

ifdef DEBUG
	or	ax, ax
	jnz	@F
	int	3
@@:
endif
	mov	cx, wHandle
	jcxz	@F				; alloc call

	cCall	LocalReAlloc, <cx, wSize, wFlags>
	jmp	KLA_GotHandle
@@:
	cCall	LocalAlloc, <wFlags,wSize>	; do the smegging call
KLA_GotHandle:
	mov	theHandle, ax
KLA_ReWire:
	cCall	GlobalWire, <ds>
	cCall	GlobalFix, <ds>
	cCall	GlobalSmartPageLock, <ds>
	
	mov	es, selCStoDS
	sti
	mov	es:[bDataUnLocked], 0
	cli
        cCall   FreeSelector, <es>

	mov	ax, theHandle
KbdLA_Done:
cEnd

;
; ALWAYS rewire for localfrees this ensures we are going to have enough memory
; if someone wants to play with some later.
;
cProc	KbdLocalFree, <PUBLIC, NEAR>
	parmW	theHandle

	localW	selCStoDS
cBegin
;**;        mov     ax, _TEXT			; lock the flag
;**;        cCall   AllocCStoDSAlias, <ax>
;**;	mov	selCStoDS, ax
;**;	mov	es, ax
;**;	sti
;**;	mov	es:[bDataUnLocked], 1
;**;	cli
;**;
;**;	cCall	GlobalSmartPageUnlock, <ds>
;**;	cCall	GlobalUnFix, <ds>
;**;	cCall	GlobalUnWire, <ds>
;**;	or	ax, ax
;**;	jz	KLF_ReWire

	cCall	LocalFree, <theHandle>

KLF_ReWire:
;**;	cCall	GlobalWire, <ds>
;**;	cCall	GlobalUnWire, <ds>
;**;	cCall	GlobalSmartPageLock, <ds>
;**;	
;**;	mov	es, selCStoDS
;**;	sti
;**;	mov	es:[bDataUnLocked], 1
;**;	cli
;**;        cCall   FreeSelector, <es>
cEnd

;***************************************************************************
;       GetSysFileName()
;
;       This is a local function whose purpose is to get a filename
;       from the [keyboard] section of SYSTEM.INI
;       It returns the filename in DATA:sNameBuf[] and the length
;       in AX.  It's a NEAR PASCAL function.
;       The only parameter is lpKey, the key name in [keyboard].
;       The default is ALWAYS a null string.
;***************************************************************************

cProc   GetSysFileName,<PUBLIC,NEAR>

        parmD   lpKey

cBegin
        ; load up and define the parameters to GetPrivateProfileString()

        mov     bx, NEWTABoffset sKeyboard	; app. name = "keyboard"
        mov     cx, NEWTABoffset sNULL		; default is NULL string
        mov     di, DATAoffset sNameBuf		; output string
        mov     dx, NEWTABoffset sSysIni	; file name = "SYSTEM.INI"
        mov     ax, NAMESIZE                	; how big our buffer is..

        cCall   GetPrivateProfileString,<csbx, lpKey, cscx, dsdi, ax, csdx>
cEnd

;***************************************************************************
;       LoadOemAnsiXlat()
;
;       Loads the Xlat???.bin file if it exists in the system.ini file.
;***************************************************************************

ifdef EXPORTEDXLAT
;
; do we need this as an export???
; davidms
;
cProc   LoadOemAnsiXlat,<LOADDS, PUBLIC,FAR,PASCAL> 
else
cProc   LoadOemAnsiXlat,<PUBLIC,NEAR,PASCAL> 
endif
        localW  selCS2DS
        localV  OFStruc,%(size OPENSTRUC)       ; buffer for OPENFILE struc

cBegin  
        ;
        ; Get the oem/ansi data file's path from SYSTEM.INI
        ;
        mov     di,NEWTABoffset sOemName        ;looking for "OEMANSI.BIN=" 
        cCall   GetSysFileName,<csdi>           ;in SYSTEM.INI
        cmp     ax,3                            ;look at filename length
        jb      LOAXExit                        ;if < 3, name is bogus

        mov     si,DATAoffset sNameBuf          ;file name from "OEMANSI.BIN= "
        regptr  lpFileName,ds,si                ;far pointer to file name
        lea     di,OFStruc                      ;buffer for open-file structure.
        regptr  lpReOpen,ss,di                  ;far ptr to open-file struct.
        mov     ax, OF_READ                     ;style
        cCall   OpenFile,<lpFileName, lpReOpen, ax>     ;open the file.
        cmp     ax, 0                           ;check handle
        jl      LOAXExit                        ;if -ve, we didn't open it.
if1
        %out ROM WARNING: Impure Code, _TEXT must RAM loaded!!!
        %out _TEXT must be fixed in real mode!!!
endif

        push    ax                      ; Save file handle
        mov     ax, _TEXT
        cCall   AllocCStoDSAlias, <ax>

        pop     bx                      ; restore file handle
        or      ax, ax

        jz      LOAXClose2

        mov     selCS2DS, ax

        mov     wCount,0                ; clear byte count, then read from file.
        mov     dx,DATAoffset wCount
        mov     cx,2                    ; 1st 2 bytes of file is count
        mov     ah,3fh
        int     21h
        jc      LOAXClose               ; if error, give up and close file
        cmp     ax,2                    ; must have read 2 bytes
        jne     LOAXClose
        mov     cx,wCount               ; load byte count for rest of file
        cmp     cx,322                  ; for extended ansi .bins
        je      SHORT LOAXNew           ; .. will be 322 for new bin
        cmp     cx,258                  ; .. must be 258
        jne     LOAXClose               ; old (not extended ansi)
                                        ; CX is now byte count for next read..
                                        ; BX is still handle
LOAXNew:
        mov     dx,offset CodePage      ; read starting at Code Page entry
        mov     ax,selCS2DS             ; get writable alias for CS
        push    ds                      ; save DS
        mov     ds,ax                   ; set DS = CS alias
        mov     ah,3fh                  ; now read rest of file
        int     21h                     ; do it..
        pop     ds

LOAXClose:
        mov     si,bx                   ; save file handle
        cCall   FreeSelector, <selCS2DS>
        mov     bx,si

LOAXClose2:
        mov     ah,3eh                  ; close file, bx = handle, still.
        int     21h

LOAXExit:
cEnd    LoadOemAnsiXlat

;***************************************************************************
;
;       GetKeyboardInfo()
;
;       Reads the subtype keyboard information stored in the system.ini file
;
;       [damianf]       Multilingual Chicago
;
;***************************************************************************

cProc   GetKeyboardInfo <LOADDS, FAR,PUBLIC,PASCAL>

        localW  selCS2DS

cBegin 
        mov     selCS2DS, 0

        cmp     KeyType,0
        jz      SetDefaultKeyType
        cmp     KeyType,6
        ja      SetDefaultKeyType
        mov     KeyType, 4

jKeyTypeFound:
        jmp     KeyTypeFound

SetDefaultKeyType:
        ; Set default keyboard type, in case entry isn't in WIN.INI.
                                                ; IBM compatible 
                                                ; Probably IBM-compatible:
        mov     KeyType,1                       ; First set to XT keyboard.
        cmp     PCType,IBMATID                  ; If it's an AT-type system,
        jne     NotAT
        mov     KeyType,3                       ; default to type 3.
        push    ds
        mov     ax,RAMBIOS
        mov     ds,ax
    assumes DS,RAMBIOS
        test    byte ptr [KB_type],10h          ; is Enhanced keyboard flag set?
        pop     ds
    assumes DS,DATA
        jz      notEnhanced                     ; skip if not
        mov     KeyType,4                       ; it's an enhanced keyboard
notEnhanced:

NotAT:

        ; Get keyboard table type from WIN.INI.
        mov     si,NEWTABoffset sKeyboard       ; lpAppName = "keyboard"
        mov     di,NEWTABoffset sType   	; lpKeyName = "type"
        mov     bx,NEWTABoffset sSysIni 	; lpFile = "SYSTEM.INI"
        xor     ah,ah
        mov     al,KeyType                      ; nDefault = KeyType
        cCall   GetPrivateProfileInt,<cssi,csdi,ax,csbx>
        or      al,al                           ; was returned value 0?
        jnz     KeyTypeNZ
        mov     al,KeyType                      ; yes. restore orig. value.
KeyTypeNZ:

        cmp     al,4                            ; only allow 5, 6 for Nokia

        jbe     TypeInRange

TypeInRange:
        mov     KeyType,al                      ; save result

        ; Get Olivetti/AT&T keyboard type from WIN.INI (should be 0
        ; if the system is NOT an Olivetti system: M24 series
        ; or AT&T 6300 series).  This value is used in the case of
        ; 80286 systems to determine whether the M24 LED protocol
        ; should be used IF olikbd.drv is accidentally installed.
KeyTypeFound:

        ; Set number of function keys
        mov     bl, KeyType                     ; get keyboard type
        xor     bh,bh                           ;  (1..6)
        mov     bl, NumKeys[bx]                 ; look up
        mov     iqdNumFunc, bx                  ; save in KBINFO
cEnd GetKeyboardInfo


;****************************************************************************
; LCIDtoDW
; Converts Character LCIDs (00000409) to LCID Double Words (0x00000409)
; returns LCID in eax
;****************************************************************************

szNum1  db      "0123456789ABCDEF"
szNum2  db      "0123456789abcdef"

LabelFP <PUBLIC, LCIDtoDW>
.386
        pop     eax             ; Pop FAR return address
        pop     cx              ; offset of string
        pop     dx              ; segment of string
        push    eax             ; replace FAR return address
        push    ds              
        push    si
        push    edi

        mov     si,     cx      ; get ofs
        mov     ds,     dx      ; get seg

        xor     eax,    eax
        mov     ebx,    eax
        mov     edx,    eax

        lea     di,     szNum2
        ror     edi,    16
        lea     di,     szNum1

        push    cs
        pop     es
IR1:
        lodsb
        or      al,     al
        jz      IR99

        mov     dx,     di
        mov     cx,     16
        repnz   scasb
        jnz     IR2                     ; not in first list, try other one
        sub     di,     dx
        dec     di                      ; di is now the value
        xchg    di,     dx              ; dx now value, di back again
        shl     ebx,    4               ; bx = old value * 16
        add     ebx,    edx
        jmp short IR1
IR2:
        ror     edi,    16
        mov     dx,     di
        mov     cx,     16
        repnz   scasb 
        jnz     IR3                     ; not in either list, error.

        sub     di,     dx
        dec     di                      ; di is now the value
        xchg    dx,     di
        ror     edi,    16
IR4:
        shl     ebx,    4               ; bx = old value * 16
        add     ebx,    edx
        jmp short IR1
IR3:
        ;
        ; error!
        ;
        xor     ebx,    ebx
        dec     ebx                     ; return -1
IR99:
        mov     eax,    ebx             ; eax is now the value.
        pop     edi
        pop     si
        pop     ds              

        retf


;***************************************************************************
;***************************************************************************

;Read-only init strings.
; dont worry too much about wasting a bit of space here. this is a discardable
; segment that will 90% of the time thrown away after boot and never reloaded.
; (its only reloaded to add tables, and then trashed again).
;
sUser           db 'USER',0
szSystemLocale  db 'SYSTEM\currentcontrolset\control\nls\locale\systemlocale',0
ifdef JAPAN
szRegValue      db 'New106Keyboard',0
szRegHotKey     db 'Control Panel\Keyboard',0
endif
        farptr lpszUSER,cs,<NEWTABOFFSET sUser>

cProc	LoadTheWorld, <FAR, PUBLIC>

	localD	hkey
	localW	nAllKeyboards			; all the ones in the reg
	localV	szBuffer, 16
cBegin
;----------------------------- Set up interrupts -----------------------------
        mov     ax,3500h or vector
        int     21h                             ; vector is in ES:BX
        mov     word ptr [bios_proc][0], bx
        mov     word ptr [bios_proc][2], es

        ;
        ; Save away current keyboard call vector value
        ;
        mov     ax,3516H
        int     21h

        mov     WORD PTR [LightsAddr],bx
        mov     WORD PTR [LightsAddr+2],es

        ;
        ; Setup keyboard interrupt vector to point to our interrupt routine
        ;
        assumes ds, nothing
        push    ds                              ; save DS
        mov     ax, _TEXT
        mov     ds, ax
        mov     ax,2500h or vector
        mov     dx,codeOFFSET keybd_int
        int     21h                             ; set the vector
        pop     ds                              ; restore DS
        assumes ds, DATA

;-------------------------- Get LoadString address ---------------------------
;
;Get address of LoadString API from User. Used by GetKeynameText
;
        cCall   GetModuleHandle, <lpszUser>
	or	ax, ax
	jz	LTW_NoUserYet

	push	ax
	pushd	176			; LoadString
        cCall   GetProcAddress
	mov	cx, ax
	or	cx, dx
	jnz	@F
LTW_NoUserYet:
	xor	ax, ax
	jmp	LTW_Done
@@:
        mov     lpfnLoadString.sel, dx
        mov     lpfnLoadString.off, ax
	
LTW_ResolvedUser:
;-------------------------- Fetch the Info items -----------------------------

        cCall   GetKeyboardInfo
        cCall   LoadOemAnsiXlat                 

;----------------------- find the base system locale -------------------------
;
;        farptr  HKEY_LOCAL_MACHINE 8000h,0002h
;	lea	bx, hkey
;	mov	eax, 15
;	mov	ss:[bx], eax
;	lea	ax, szSystemLocale
;	lea	cx, szBuffer
;	cCall	RegQueryValue, <HKEY_LOCAL_MACHINE, csax, sscx, ssbx>
;	mov	cx, ax
;	mov	eax, 0ffff0409H                 ; assume something is wrong..
;.errnz ERROR_SUCCESS
;	or	cx, dx				; check the high word as well
;	jnz	@f				; non-zero, bug, keep default
;
;	lea	ax, szBuffer
;	cCall	LCIDtoDW, <ssax>
;@@:
;	mov	SystemLocale, eax
;
;-------------------------- Load the Resource Layout -------------------------
        cmp     nKeyboards, 0
        jne     @F
        call	LoadBaseKeyTable		; ax = address of US table
	or	ax, ax
	mov	pSystemLocale, ax
	mov	pCurrentLocale, ax

	mov	bx, pLCIDlist
	LMHtoP	bx
	mov	ecx, SystemLocale
	mov	[bx].localeID, ecx
	mov	[bx].pKeyData, ax		; handle to base layout
	inc	nKeyboards			; we now have one

	mov	bx, ax
	call	SetCurrentLocale
@@:
;---------------------- BACKWARD HACK - LOAD OLD TABLE -----------------------
;;;;;;;	call	NewTable			; may or may not work
;------------------------------- Enable BIOS ---------------------------------
;
; If Enhanced (type 4) keyboard has been detected, we'll let the BIOS think 
; we have a standard keyboard. This is to facilitate handling of PrintScreen 
; and Pause keys by the BIOS. This stuff must be done AFTER the NewTable() 
; call, because this clears the RT keyboard flag!
;
        cmp     [KeyType],4             ; is it RT keyboard ?
        jnz     ena10                   ; if not, skip
        push    ds
        mov     ax,RAMBIOS
        mov     ds,ax
        assumes ds,RAMBIOS

        mov     al, byte ptr[KB_type]   ; get old 40h:96h (enhanced flag)
        and     byte ptr[KB_type],0efH  ; clear bit 4

        pop     ds
        assumes ds,DATA
        mov     old_enhanced, al        ; save old enhanced flag byte (which 
                                        ; may or may not have bit 4 set!)
ena10:

; Setup nmi_vector (for SysReq key handling) and CSAlias.
; Need to alias CODE because of protect mode.

ifdef NEWNMI
        call    GetNmi
else
        Public GetNmi
GetNmi:
        mov     ax,3502h
        int     21h
        mov     word ptr [nmi_vector][0],bx
        mov     word ptr [nmi_vector][2],es
        Public GotNmi
GotNmi:
endif

;-------------------------- Register we are enabled --------------------------
        mov     [enabled],-1
LTW_Done:
cEnd 


;***************************************************************************
;
; Inquire( pKBINFO ) - copies information about the keyboard hardware into
; the area pointer to by the long pointer argument.  Returns a count of the
; number of bytes copied.
;
; The Windows kernel calls this to copy information to its 'KeyInfo' data
; structure.
;
;***************************************************************************

LabelFP <PUBLIC, Inquire>

	pop	eax		; return address
	pop	ebx		; lpBuffer
	pushW	INQEX_KBDINFO
	push	ebx
	pushW	0
	push	eax
	jmp	InquireEx	; jump into _TEXT segment - keep this here
				; out the way


;***************************************************************************
; InquireEx - For Chicago Multilingual  [damianf]
;
; InquireEx( pKBINFO,lpnLCIDs ) - copies information about the keyboard hardware into
; the area pointer to by the long pointer argument.  Returns a count of the
; number of bytes copied.
;
; lpnLCIDs Stores number of keyboard in the system here 
;
; The Windows kernel calls this to copy information to its 'KeyInfo' data
; structure.
;
; Copyright (C) Microsoft Corp. 1993
;
;
; this will be called twice from USER
;       1. pKBINFO set
;          lpnLCIDS is a pointer
;          lpLCID is NULL
;
;          NewTableEx will be called, and the number of LCIDs returned to 
;          USER in lpnLCIDS
;
;       2. pKBINFO is NULL
;          lpnLCIDS is a word value (seg == 0)
;          lpLCID is a pointer
;          The buffer will be filled in.
;
;***************************************************************************

InqJumpTab 	dw offset inqKbdInfo
		dw offset inqNLcid
		dw offset inqLpLcid
		dw offset inqKeyboardType
		dw offset inqKbdSubType
		dw offset inqNFuncKeys

cProc   RareInquireEx, <PASCAL, PUBLIC,FAR>
	parmW	wCode
	parmD	lpBuffer
	parmW	nCount
cBegin nogen
        push    esi
        push    di
;------------------------- jump to correct action ----------------------------
	mov	bx, wCode
	dec	bx			; make zero based
	cmp	bx, INQEX_NFUNCKEYS	; last one we nuderstand here
	jae	InqEx_Error

	add	bx, bx
	jmp	cs:[bx].InqJumpTab
InqEx_Error:
	xor	ax, ax
	jmp	InqEx_Done
;-----------------------------------------------------------------------------
inqKeyboardType:	
	mov	ax, 4
	jmp	InqEx_Done

inqKbdSubType:
	xor	ax, ax
	jmp	InqEx_Done

inqNFuncKeys:
	mov	ax, 12
	jmp	InqEx_Done
;----------------------------- Fill In KbdInfo -------------------------------
inqKbdInfo:
; fill in the KBInfo structure that Inquire calls for. 
;
        les     di, lpBuffer            ;Get far ptr of dest area
	mov	ax, es			; make sure we want it!
	or	ax, di
	jz	@F
        mov     si,dataOFFSET inquireData       ;Get source
        mov     ax,size KBINFO          ;Get number of bytes to move
        mov     cx,ax                   ;(Return byte count in AX)
        rep     movsb                   ;Move the bytes
	jmp	InqEx_Done

;------------------------------- return NLCID --------------------------------
inqNLcid:
	mov	ax, nKeyboards
        or      ax, ax                  ; do we have any yet?
        jnz     InqEx_Done              ; ... yes!
        inc     ax                      ; ... no, so Enable() hasn't arrived
	jmp	InqEx_Done

;----------------------------- Fill In LCID list -----------------------------
inqLpLCID:
	les	di, lpBuffer
	assumes es, nothing

        mov     ax, nKeyboards
        or      ax, ax                  ; do we have any yet?
        jnz     @F                      ; ... yes, cool
        mov     eax, 0FFFF0409H         ; ... no, this is the driver default
        stosd                           ; ... save it
        mov     ax, 1                   ; ... and say there is only one
        jmp     InqEx_Done
@@:
        mov     bx, nCount
        sub     ax, bx
        sbb     cx, cx
        and     ax, cx
        add     ax, bx                      	;Lowest left in ax
        mov     cx, ax

	mov	si, pLCIDList			;Get LCID list Table
	LMHtoP	si
@@:
	movsd					; move the layout ID across

sizeofLCID_NODWORD = sizeofLCIDIB - 4
if sizeofLCID_NODWORD
	add	si, sizeofLCID_NODWORD		; move up struct if ness
endif
	loop	@B
;	jmp	InqEx_Done			; ax has # copied
;-----------------------------------------------------------------------------
InqEx_Done:
        pop     di
        pop     esi
cEnd


;***************************************************************************

;
; Entry: 
;	bx = HANDLE of block to fixup.
;	ds = selector of block
;
FixupKbdTable	proc near
	public FixupKbdTable

	push	di
	push	ds
	pop	es

        add     [bx+STATE_LIST], bx
        add     [bx+TOASC_STATES], bx
        add     [bx+STATETABLES], bx
        add     [bx+VKEY_LISTS], bx
        add     [bx+VKEY_LIST_LENS], bx
        add     [bx+VK_STATES], bx
        add     [bx+SCANTOIDX], bx
        add     [bx+VKEYTOIDX], bx
        add     [bx+VKEYTOANSI], bx
        add     [bx+CAPSTABLE], bx
ifdef JAPAN
        add     [bx+KANA_NORMAL], bx
endif
        mov     cx, [bx+DEAD_KEYS]
        jcxz    @F
        add     cx, bx
        mov     [bx+DEAD_KEYS], cx
@@:
        mov     cx, [bx+DEAD_KEYTRANS]
        jcxz    @F
        add     cx, bx
        mov     [bx+DEAD_KEYTRANS], cx
@@:
        mov     cx, [bx+LIG_KEYS]
        jcxz    @F
        add     cx, bx
        mov     [bx+LIG_KEYS], cx
@@:
        mov     di, [bx+STATETABLES]
        mov     cx, [bx+NSTATES]
        inc     cx
@@:
        mov     ax, [di]
        add     ax, bx
        stosw
        loop    @B

        mov     di, [bx+VKEY_LISTS]
        mov     cx, [bx+NSTATES]
        inc     cx
@@:
        mov     ax, [di]
        add     ax, bx
        stosw
        loop    @B
	pop	di

	ret
FixupKbdTable	endp


;***************************************************************************
;
; NewTableEx(LCID, bAdd)

; 	params: 
;		lpszFileName : name of kbd file to load, or NULL.
;		layout: layout id/lang id
; 
;	if lpszFileName is NON-NULL, then the layout is to be loaded, and
;	associated with the language specified in layout.  If it is NULL then
;	the layout is to be removed from the list.
;
;	If the new layout has the language of the system locale, and the
;	HI word of the keyboard ID (SystemLocale) is zero, then this layout
;	is set to be the default layout, and will be stored in local memory.
;	Otherwise its loaded into global memory.
;
;	If the language of the layout exists, it will replace the layout 
;	already in use.
;
;	Note, we now fail the call if a LocalReAlloc is needed and it fails.
;	As the base table is preloaded this is safe.
;
; 	NO registry checking is done here. It is assumed that layout has been
;	set up correctly for lpszFile. Adding/Removal of layouts is based on
;	this.
;
;***************************************************************************

cProc NewTableEx,<LOADDS, PASCAL, PUBLIC,FAR>,<di,si,ds>
	parmD	lpszFile
	parmD	layout

	localW	hfile
	localW	nSystemDir
	localW	NewNumKeyboards
	localW	wLayoutToKill

	localV	myKbdHeader, sizeofKBDHEADER

        localV  szSystemDir, PATHLEN
cBegin
	xor	ax, ax
	mov	wLayoutToKill, ax
	mov	NewNumKeyboards, ax

	mov	ecx, lpszFile
	or	ecx, ecx
	jnz	NTE_LoadLayout
	call	NTE_RemoveLayout
NTE_Done1:
	jmp	NTE_Done
;------------------- find the language in the table. -------------------------
;
; find the location of the language in the table. 
;
NTE_LoadLayout:
	mov	eax, layout
	mov	si, pLCIDList
	LMHtoP	si
	mov	cx, nKeyboards
        jcxz    NTE_ANewSlot                    ; empty buffer only at start
@@:
	cmp	ax, [si].LocaleId.off		; if we find the lang, can we 
	je	NTE_CheckSlotData		; kill the data?

	add	si, sizeofLCIDIB
	loop	@B
;------------------------ Ensure table big enough ----------------------------
;
; Fell off the end of the table search, so didn't find language, put new 
; language in next free slot at end. If there is not enough room in the table, 
; make some!
;
	mov	ax, NumLCIDBuffers
	cmp	ax, nKeyboards
	ja	NTE_SlotsOK

	inc	ax
	inc	ax				; make 2 new slots
	mov	di, ax
	mov	dx, sizeofLCIDIB
	mul	dx				; ax now sizeof(newslots+)
	Arg	<pLCIDList, LMEM_MOVEABLE, ax>
	cCall	KbdLocalAlloc
	or	ax, ax
	jnz	@F
	xor	ax, ax
	jmp	NTE_Done
@@:
ifdef DEBUG
	cmp	ax, pLCIDList			; LocalReAlloc should always
	je	@f				; return the same handle.
	int	3
@@:
endif
	mov	NumLCIDBuffers, di		; register new size
NTE_SlotsOk:
;--------------------------- Data Present Check ------------------------------
;
; check to see if data already in use. If so dont bother loading the file
; again.
	mov	si, pLCIDList
	LMHtoP	si				; repoint to table
	mov	bx, si				; save for later
	mov	ax, layout.sel			; get layout ID
	mov	cx, nKeyboards
@@:
	cmp	ax, [si].LocaleId.sel
	je	NTE_FixupSlotOnly		; we already use this layout!
	add	si, sizeofLCIDIB
	loop	@B
NTE_ANewSlot:
	inc	NewNumKeyboards			; we have a new language
	jmp	NTE_FoundSlot			; si at end, layout NOT 
						; already in use
NTE_FixupSlotOnly:
;
; layout already in use. In this case, simply insert the data into the slot
; and we're done
;
; si points to existing language which uses this layout
;
	mov	dx, nKeyboards
	mov	ax, sizeofLCIDIB
	mul	dx
	add	bx, ax				; bx now points to end
	mov	eax, layout
	mov	[bx].LocaleID, eax

	mov	ax, [si].pKeyData
	mov	[bx].pKeyData, ax

	inc	nKeyboards
	mov	ax, 1
	jmp	NTE_Done
;---------------------------- Remove Data section ----------------------------
NTE_CheckSlotData:
;
; the language already exists, we are using the same slot again.  We need to
; check that this is the only place using this data, and if so free it so 
; that we dont have bogus data in one of the heap's
;
	mov	ax, [si].LocaleID.sel		; get the layout id
	mov	bx, pLCIDList
	LMHtoP	bx
	mov	cx, nKeyboards
	xor	dx, dx				; dx is # of slots using this
NTE_CheckToKill:
	cmp	[bx].LocaleID.sel, ax
	jne	@F
	inc	dx
@@:
	add	bx, sizeofLCIDIB
	loop	NTE_CheckToKill

	cmp	dx, 1
	ja	@F

	mov	ax, [si].pKeyData
	mov	wLayoutToKill, ax
@@:
;------------------------ Load Data and fill in slot -------------------------
NTE_FoundSlot:
;
; si is location of free slot. 
;
        mov     ax, PATHLEN			; get the system directory
	lea	di, szSystemDir
        cCall   GetSystemDirectory, <ssdi, ax>
	or	ax,ax
	jnz	@F
NTE_Failed2:
	xor	ax, ax				; ensure everyone here fails
	jmp	NTE_Done
@@:
	lea	bx, szSystemDir			; tack the filename on the end
	add	bx, ax
        mov     ax, '\'
	mov	ss:[bx], ax
	cCall	lstrcat, <ssdi, lpszFile>

        cCall   _LOPEN, <ssdi, OF_READ>		; open the file
        cmp     ax, 0ffffh
	jz	NTE_Failed2

        mov     HFile,ax                        ;save file handle
                                                ;_LSEEK to end to get file len
	
	xor	di, di
	lea	cx, myKbdHeader	   
	cCall	_lread, <ax, sscx, sizeofKBDHEADER>
	cmp	ax, sizeofKBDHEADER
	je	@F

NTE_OpenFail:
        cCall   _LCLOSE,<HFile>                 ;Close it and get the next

	or	di, di				; did we already allocate
	jz	NTE_Failed2			; ... a block? 
	cCall	LocalFree, <di>
	jmp	NTE_Failed2

@@:
	mov	ax, myKbdHeader.kbdhMagic	; check the magic number
	cmp	ax, 'SD'
	jne	NTE_OpenFail

	mov	eax, myKbdHeader.kbdhLayout	; move to the data
	cCall	_llseek, <HFile, eax, 0>

	mov	ax, si				; ax is address
	mov	si, pLCIDList			; si is base handle
	LMHtoP	si				; si is base pointer
	sub	ax, si				; ax is offset
	mov	si, ax				; save offset

	mov	di, myKbdHeader.kbdhSize
	Arg	<0, LMEM_FIXED, di>		; di is size required
	cCall	KbdLocalAlloc
	or	ax, ax
	jz	NTE_Done			; stack exit will restore DS

	xchg	ax, di				; ax = size, di = handle

	cCall	_lread, <hfile, dsdi, ax>
	cmp	ax, myKbdHeader.kbdhSize
	jne	NTE_OpenFail
        cCall   _LCLOSE,<HFile>                 ;_LCLOSE the file we're done 

;----------------------------- fix up slot data ------------------------------
	mov	ax, si				; ax is offset
	mov	si, PLCIDList			; si is base handle
	LMHtoP	si				; si is base pointer
	add	si, ax				; si is pointer

	mov	eax, layout
	mov	[si].LocaleID, eax
	mov	[si].pKeyData, di

	cmp	ax, word ptr SystemLocale
	jne	@F
	mov	pSystemLocale, di		; This is the one trap uses
	mov	pCurrentLocale, di
	mov	SystemLocale, eax
	mov	CurrentLocale, eax

	mov	bx, di				; set the flags for this 
	call	SetCurrentLocale
@@:
	mov	cx, wLayoutToKill
	jcxz	@F
	cCall	KbdLocalFree, <cx>		; clear it up.
@@:
	mov	ax, NewNumKeyboards
	add	nKeyboards, ax			; could have been zero 

        mov     bx, di                          ; still in layout dgroup
        call    FixupKbdTable
;-----------------------------------------------------------------------------
        mov     ax, 1				; Return TRUE
NTE_Done:
cEnd 

;*****************************************************************************
;----------------------------- Kill off a table ------------------------------

;
; removal section.  Uses the same stack frame as NewTableEx. This can be 
; called by NewTableEx in BOTH cases of it being called.

NTE_RemoveLayout	proc near
	public	NTE_RemoveLayout

;
; loop round looking for the layout in the list. If we have it, then remove
; the layout from it's heap and then compact the list and free up the last
; slot
;
        mov     eax, layout
        mov     di, pLCIDList
        LMHtoP  di
        mov     bx, di                          ; need this later
        mov     cx, nKeyboards
NTE_NextToKill:
        cmp     eax, [di].LocaleID
        je      NTE_FoundKiller
        add     di, sizeofLCIDIB
        loop    NTE_NextToKill
@@:
	xor	ax, ax
	ret					; didn't find it.

;--------- found language, check layout in use somewhere else ----------------
NTE_FoundKiller:
;
;
; di = location of block to kill
; bx = base of slot table
;
        cmp     eax, SystemLocale		; CANT KILL SYSTEMLOCALE!!!!
        je      @b

        xor     dx, dx
        mov     cx, nKeyboards                  ; search for layout
        mov     ax, [di].pKeyData               
        mov     si, bx                          ; save pointer
NTE_FoundKillLayout:
        cmp     ax, [bx].pKeyData
        jne     @F
        inc     dx
@@:
        add     bx, sizeofLCIDIB
        loop    NTE_FoundKillLayout

;----------------------- shuffle list if nessacary ---------------------------
;
; do shuffle now, before any calls out to LocalFree etc. This means that
; the addresses we have are legal.
;
; ax = handle of layout
; bx = end of slot list. (i.e. 1st free address).
; dx = # of times the layout is used.
; si = base of slot table 
; di = location of block to kill
;
	mov	si, di				
	add	si, sizeofLCIDIB		; si now 1st byte to more from
	cmp	si, bx
	je	@F				; nothing to move
	mov	cx, bx			
	sub	cx, si				; # of bytes
	push	ds
	pop	es
	rep	movsb
@@:
        cmp     dx, 1                           ; cant free if used by
        ja      @F                              ; more than one layout

        cCall   KbdLocalFree, <ax>                 ; kill off the block
@@:
	dec	nKeyboards
        mov     ax, 1
        ret
NTE_RemoveLayout	endp


;****************************************************************************
; LoadBaseKeyTable
; Load a keyboard resource containing the US keyboard
; This MAY be a last ditched attempt to keep the driver working
; if this fails then it's - 
;
;       Burn Baby Burn Let's all gather round and warm ourselves by the fire
;
; Plan - We'll load & lock the resource, get size alloc mem and copying it to
; memory. Free the resource, do the internal fix-ups if necessary. 
;
; If all this is done put it in an LCIDIB block.
;
; Otherwise light a big fire !!
;
;****************************************************************************

szDefaultID db	"FFFF0409",0

FarPtr lpszKeyboard, cs, <offset sKeyboard>
FarPtr lpszLayout, cs, <offset sDllName>
FarPtr lpszNULL, cs, <offset sNULL>
FarPtr lpszSystemIni, cs, <offset sSysIni>
FarPtr lpszDefaultID, cs, <offset szDefaultID>

cProc   LoadBaseKeyTable, <PUBLIC,NEAR,PASCAL>, <si,di>

	localD  lpResTable
	localW  HUSKTable
	localW  pUSTable

	localV	szDummyFile, 256
cBegin  
	xor	ax, ax
	mov	pUSTable, ax			; assume error!
	mov	hUSKTable, ax

	lea	si, szDummyFile
	Arg	<lpszKeyboard, lpszLayout, lpszNULL, sssi, 256, lpszSystemIni>
	cCall	GetPrivateProfileString
	or	ax, ax
	jz	NoFailsafeLayout

	cCall	NewTableEx, <sssi, lpszDefaultID>
	or	ax, ax
	jz	NoFailsafeLayout

	mov	eax, 0FFFF0409H
	mov	SystemLocale, eax
	mov	CurrentLocale, eax

	mov	si, pLCIDlist
	LMHtoP	si
	mov	ax, [si].pKeyData
        mov     pUSTable, ax                    ; Save memory value
        mov     pSystemLocale, ax               ; assume no other locales
        mov     pCurrentLocale, ax              ; assume no other locales
	jmp	LUSKT_Complete

NoFailsafeLayout:
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
	FarPtr  LPRCDATA, 0, RT_RCDATA
	FarPtr  LPUS_KEYBOARD_TABLE, 0, US_KEYBOARD_TABLE
	cCall	FindResource, <hlibKbd, LPUS_KEYBOARD_TABLE, LPRCDATA>
        or      ax,     ax
        jz      LUSKT_Done

        cCall   LoadResource,<hlibKbd,ax>
        or      ax,     ax
        jz      LUSKT_Done

        mov     HUSKTable,ax                    ;save handle

        cCall   LockResource,<ax>               ;lock down and load resource
        mov     lpResTable.off, ax               ;save address
	mov	di, ax
        mov     lpResTable.sel, dx               ;of resource
	mov	es, dx

	or	ax, dx
        jz      LUSKT_Done

;------------------- Size the file and alloc a local block -------------------

	mov	di, es:[di].kbdhSize
        cCall	KbdLocalAlloc, <0,LMEM_FIXED, di>
        or      ax,ax
        jz      LUSKT_Done

        mov     pUSTable, ax                    ; Save memory value
        mov     pSystemLocale, ax               ; assume no other locales
        mov     pCurrentLocale, ax              ; assume no other locales

;--------------------- make resource local and free global -------------------
        mov     cx, di                          ; size in bytes
	mov	di, ax				; pointer
        push    ds
        pop     es
	lds	si, lpResTable
	add	si, word ptr [si].kbdhLayout	; move to layout data
        shr     cx, 1
        rep     movsw
        jnc     @F	 
        movsb
@@:     
	push	es
	pop     ds                              ;restore ds

;-------------------------- fixup the table offsets --------------------------
	mov	bx, pUSTable			; point to list
	call	FixupKbdTable

LUSKT_Done:
	mov	cx, HUSKTable
	jcxz	@F
	cCall	GlobalFree, <cx>
@@:
	mov	ax, pUSTable
LUSKT_Complete:
cEnd    

.286p

;*****************************************************************************

sEnd NEWTAB


if2
%out  .. end Tabs.Asm
%out
endif

end

page		60, 132
title		Device mode dialog
;===============================================================================
; Filename	MSNETDEV.ASM
; Copyright	(C) 1989 by Research Machines Ltd.
;		(C) 1989-1990 by Microsoft Corporation
;===============================================================================
; REVISIONS:	15/03/1989	Initial version
;		23/03/1989	Change to segment name / file name
;		23/03/1989	Update to spec 0.58
;		31/03/1989	Tidied up comments.
;		19/03/1989	Add pretty DeviceMode dialog box
;		1989-1990	Made it work...
;===============================================================================

		memM	equ	1			; Middle memory model
		?WIN	=	1			; Windows prolog/epilog
		?PLM	=	1			; Pascal calling convention

		.xlist
include 	cmacros.inc
include 	windows.inc
include 	wnet.inc
		.list

IDDEVMODE	equ	100
IDWARNING	equ	101
IDABOUT 	equ	102
IDD_STARTUPWARN equ	200
IDD_STARTUPRESTORE  equ 201
IDD_HELP	equ	204
IDD_ABOUT	equ	205
; This comes from cphelp.h 
IDH_CHILD_NETWORK equ   5010
		.sall

;===============================================================================
; ============= EXTERNAL FUNCTIONS =============================================
;===============================================================================

externFP	DialogBox
externFP	GetDlgItem
externFP	IsDlgButtonChecked
externFP	CheckDlgButton
externFP	EndDialog
externFP	GetProfileInt
externFP	WriteProfileString
externFP	LockSegment
externFP	UnlockSegment
externFP	WinHelp

;===============================================================================
; ============= DATA SEGMENT ===================================================
;===============================================================================

sBegin		DATA

externW 	hLibraryModule

globalD 	lpfnDevmode,<far ptr DeviceModeDialog>, 1
globalD 	lpfnAbout, <far ptr AboutDlgProc>, 1

globalB 	szSection,<'Windows',0>, 1
globalB 	szTag,<'NetWarn',0>, 1
globalB 	szNetSection, <'Network',0>, 1
globalB 	szRestore, <'Restore',0>, 1
globalB		szHelpFile, <'control.hlp',0>, 1
sEnd		DATA

;===============================================================================
; ============= CODE SEGMENT ===================================================
;===============================================================================

createSeg	_DEV, DEVCODE, BYTE, PUBLIC, CODE
sBegin		DEVCODE
		assumes CS, DEVCODE
		assumes DS, DATA


cProc	IsWarningEnabled, <NEAR, PUBLIC>
cBegin
    cCall   LockSegment,<DS>

    push    ds
    lea     bx, szSection
    push    bx

    push    ds
    lea     bx, szTag
    push    bx

    mov     ax, 1
    push    ax

    call    GetProfileInt

    push    ax

    cCall   UnlockSegment, <DS>

    pop     ax
cEnd

cProc	IsRestoreEnabled, <NEAR, PUBLIC>
cBegin
    cCall   LockSegment,<DS>

    push    ds
    lea     bx, szNetSection
    push    bx

    push    ds
    lea     bx, szRestore
    push    bx

    mov     ax, 1
    push    ax

    call    GetProfileInt

    push    ax

    cCall   UnlockSegment, <DS>

    pop     ax
cEnd


;===============================================================================
; ============= EXPORTED FUNCTIONS =============================================
;===============================================================================

;===============================================================================
subttl		AboutDlgProc
page
;===============================================================================
;
; DESCRIPTION .
; ENTRY .......
; EXIT ........
; COMMENTS ....
;
;===============================================================================

cProc		AboutDlgProc, <FAR, PUBLIC>

    parmW   hDlg
    parmW   wMsg
    parmW   wParam
    parmD   lParam

cBegin
    sub     ax, ax
    cmp     wMsg, WM_COMMAND
    jnz     adp_exit
    cmp     wParam, IDOK
    jz	    adp_bye
    cmp     wParam, IDCANCEL
    jnz     adp_exit
adp_bye:
    push    hDlg
    push    ax
    call    EndDialog
adp_exit:
cEnd

;===============================================================================
subttl		DeviceModeDialog
page
;===============================================================================
;
; DESCRIPTION .
; ENTRY .......
; EXIT ........
; COMMENTS ....
;
;===============================================================================

cProc		DeviceModeDialog, <FAR, PUBLIC>

    parmW   hDlg
    parmW   wMsg
    parmW   wParam
    parmD   lParam

    localW  wNoWarn
cBegin

;---------------------------------------------------------------
; Dispatch to appropriate code
;---------------------------------------------------------------

    cmp     ( wMsg ), WM_INITDIALOG
    je	    WMInitDialog

    cmp     ( wMsg ), WM_COMMAND
    je	    WMCommand

    jmp     NotProcessed

;---------------------------------------------------------------
; Process WM_INITDIALOG
;---------------------------------------------------------------

WMInitDialog:
    call    IsWarningEnabled
    cmp     ax, 1				    ; do a !ax, iow
    sbb     ax, ax
    neg     ax
    mov     cx, IDD_STARTUPWARN
    cCall   CheckDlgButton, <hDlg, cx, ax>
    call    IsRestoreEnabled
    mov     cx, IDD_STARTUPRESTORE
    cCall   CheckDlgButton, <hDlg, cx, ax>
    jmp     Processed

;---------------------------------------------------------------
; Process WM_COMMAND
;---------------------------------------------------------------

WMCommand:
    cmp     wParam, IDOK
    je	    WMIDOK

    cmp     wParam, IDCANCEL
    je	    WMIDCANCEL

    cmp     wParam, IDD_ABOUT
    jne     @F
    jmp     AboutDialog
@@:
    cmp     wParam, IDD_HELP
    jne     @F
    jmp     InvokeHelp
@@:
    jmp     NotProcessed


;---------------------------------------------------------------
; Process WM_COMMAND IDOK
;---------------------------------------------------------------
WMIDOK:
    ;	    lock our ds so string don't move
    cCall   LockSegment, <ds>

    ;	    is the button for not warning set?
    push    hDlg
    mov     ax, IDD_STARTUPWARN
    push    ax
    call    IsDlgButtonChecked

    cmp     ax, 1		    ; is ax >= 1?
    sbb     ax, ax		    ; 0 if ==, -1 if >= 1
    neg     ax			    ; ==> 0 or 1
    add     al, '0'		    ; convert to '1' or '0'

    mov     wNoWarn, ax 	    ; put in memory

    lea     bx, szSection
    push    ds
    push    bx

    lea     bx, szTag
    push    ds
    push    bx

    lea     bx, wNoWarn
    push    ss
    push    bx

    call    WriteProfileString


    ;	    is the button for restoring connections set?
    push    hDlg
    mov     ax, IDD_STARTUPRESTORE
    push    ax
    call    IsDlgButtonChecked
    mov     bx, '0'
    or	    ax, ax
    jz	    @F
    inc     bx
@@:

    mov     wNoWarn, bx 	    ; put in memory

    lea     bx, szNetSection
    push    ds
    push    bx

    lea     bx, szRestore
    push    ds
    push    bx

    lea     bx, wNoWarn
    push    ss
    push    bx

    call    WriteProfileString

    cCall   UnlockSegment, <ds>

    ; FALL THRU

;---------------------------------------------------------------
; Process WM_COMMAND IDCANCEL
;---------------------------------------------------------------
WMIDCANCEL:
    sub     ax,ax
    Arg     hDlg
    Arg     ax
    cCall   EndDialog

    jmp     short Processed

;---------------------------------------------------------------
; Put up the about dialog box
;---------------------------------------------------------------

AboutDialog:
    RegPtr  lpszDialog, dx, ax

    mov     ax, IDD_ABOUT
    cwd

    Arg     hLibraryModule
    Arg     lpszDialog
    Arg     hDlg
    Arg     lpfnAbout
    cCall   DialogBox

    jmp     short Processed

;---------------------------------------------------------------
; Put up help for this dialog box
;---------------------------------------------------------------

InvokeHelp:
    lea     bx, szHelpFile
    RegPtr  lpszHelpFile,ds,bx
    cCall   WinHelp,<hDlg,lpszHelpFile,HELP_CONTEXT,0,IDH_CHILD_NETWORK>
    jmp     short Processed

;---------------------------------------------------------------
; Exit without having processed message
;---------------------------------------------------------------

NotProcessed:
    sub     ax,ax
    jmp     short DeviceModeDialogExit

;---------------------------------------------------------------
; Exit after processing message
;---------------------------------------------------------------

Processed:
    mov     ax, TRUE

;---------------------------------------------------------------
; Exit one way or another
;---------------------------------------------------------------

DeviceModeDialogExit:

cEnd

;===============================================================================
subttl		WNetDeviceMode
page
;===============================================================================
;
; DESCRIPTION . Allow access to network functions not supported by WinNet
;		Display version / copyright messages
; ENTRY ....... hWnd is a handle to the window for dialog boxes etc
; EXIT ........
; COMMENTS ....
;
;===============================================================================

cProc		WNetDeviceMode, <FAR, PUBLIC>

		parmW	hWnd
cBegin

;---------------------------------------------------------------
; Display / process dialog box
;---------------------------------------------------------------

    RegPtr  lpszDialog, dx, ax

    mov     ax,IDDEVMODE
    cwd

    Arg     hLibraryModule			    ; hInst
    Arg     lpszDialog
    Arg     hWnd				    ; hWndParent
    Arg     lpfnDevmode
    cCall   DialogBox				    ; Modal Dialog Box

;---------------------------------------------------------------
; Return to caller
;---------------------------------------------------------------

     or      ax,ax
     jz      WnetDeviceModeExit

     mov     ax,WN_WINDOWS_ERROR

WNetDeviceModeExit:

cEnd

;===============================================================================
subttl		PostWarning
page
;===============================================================================
;
; DESCRIPTION . If the NetWarn flag is enabled, warn the user that his net
;		software is not running.
; ENTRY .......
; EXIT ........
; COMMENTS .... Using same dlg function as device mode for now
;
;===============================================================================

cProc		PostWarning, <FAR, PUBLIC>
cBegin

;---------------------------------------------------------------
; Display / process dialog box
;---------------------------------------------------------------

    call    IsWarningEnabled
    or	    ax,ax
    jz	    PW_Exit

    RegPtr  lpszDialog, dx, ax

    mov     ax,IDWARNING
    cwd
    sub     cx, cx

    Arg     hLibraryModule			    ; hInst
    Arg     lpszDialog
    Arg     cx					    ; hWndParent
    Arg     lpfnDevmode
    cCall   DialogBox				    ; Modal Dialog Box

PW_Exit:

cEnd

;===============================================================================
; ============= END OF MSNETDEV ================================================
;===============================================================================

sEnd		DEVCODE
		end


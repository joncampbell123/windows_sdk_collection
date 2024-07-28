;
;    (C) Copyright MICROSOFT Corp. 1986-1990
;
;    ************* MICROSOFT CONFIDENTIAL ******************
;

;
; Masks and Bits defs for StatusFlags
;
Terminate	equ	00000000000000000000000000000001B
TerminateBit	equ	0		  ; == 0 if app running, else app is done
fExclApp	equ	00000000000000000000000000000010B
fExclAppBit	equ	1		  ; != 0 if app is an Exclusive VM
fBackground	equ	00000000000000000000000000000100B
fBackgroundBit	equ	2		  ; != 0 if background execution
fWindowed	equ	00000000000000000000000000001000B
fWindowedBit	equ	3		  ; != 0 if app is in a Window
Suspended	equ	00000000000000000000000000010000B
SuspendedBit	equ	4		  ; == 0 if app running, else app is suspended
TFocus		equ	00000000000000000000000000100000B
TFocusBit	equ	5		  ; != 0 if app has focus when terminated
Destroyed	equ	00000000000000000000000001000000B
DestroyedBit	equ	6		  ; == 0 if app VM exists, else is destroyed
Created 	equ	00000000000000000000000010000000B
CreatedBit	equ	7		  ; == 0 if app VM has not been created yet
fFirstEvnt	equ	00000000000000000000000100000000B
fFirstEvntBit	equ	8		  ; == 0 if app has not gotten First Event Flag
fPasting	equ	00000000000000000000001000000000B
fPastingBit	equ	9		  ; != 0 if app is pasting
fFlashWnd	equ	00000000000000000000010000000000B
fFlashWndBit	equ	10		  ; != 0 if app is flash windowing
fDialog 	equ	00000000000000000000100000000000B
fDialogBit	equ	11		  ; != 0 if app is dialoging
fActive 	equ	00000000000000000001000000000000B
fActiveBit	equ	12		  ; != 0 if app is the active window
fScroll 	equ	00000000000000000010000000000000B
fScrollBit	equ	13		  ; != 0 if app in scroll mode
fJustCanKey	equ	00000000000000000100000000000000B
fJustCanKeyBit	equ	14		  ; != 0 if just did a key cancel or process
fOwnKbd 	equ	00000000000000001000000000000000B
fOwnKbdBit	equ	15		  ; != 0 if app owns the keyboard
fFocus		equ	00000000000000010000000000000000B
fFocusBit	equ	16		  ; != 0 if app has focus when running
fVValid 	equ	00000000000000100000000000000000B
fVValidBit	equ	17		  ; != 0 if app has valid video state
fSelect 	equ	00000000000001000000000000000000B
fSelectBit	equ	18		  ; != 0 if app is in selection mode
fGrbProb	equ	00000000000010000000000000000000B
fGrbProbBit	equ	19		  ; != 0 if app is to "Wake up"
fHTrack 	equ	00000000000100000000000000000000B
fHTrackBit	equ	20		  ; != 0 if app is to track cursor Horiz
fMaxWndSwIfAc	equ	00000000001000000000000000000000B
fMaxWndSwIfAcBit equ	21		  ; != 0 if switch from full screen to window if active
fCompMax	equ	00000000010000000000000000000000B
fCompMaxBit	equ	22		  ; != 0 if in compute max code
fMaxWndSwtch	equ	00000000100000000000000000000000B
fMaxWndSwtchBit equ	23		  ; != 0 if switch from full-screen to window implied
fSizeReent	equ	00000001000000000000000000000000B
fSizeReentBit	equ	24		  ; != 0 if size code is busy
fBadPaintSem	equ	00000010000000000000000000000000B
fBadPaintSemBit equ	25		  ; != 0 if doing a bad paint dialog
fSwitchBack	equ	00000100000000000000000000000000B
fSwitchBackBit	equ	26		  ; != 0 if ALT-TAB/ESC in progress
fIgnoreMouse	equ	00001000000000000000000000000000B
fIgnoreMouseBit equ	27		  ; != 0 if next mouse button MSG is to be ignored
fWndSwtch	equ	00010000000000000000000000000000B
fWndSwtchBit	equ	28		  ; != 0 if switch from full-screen to window implied
fRestFgrnd	equ	00100000000000000000000000000000B
fRestFgrndBit	equ	29		  ; != 0 if paste needs to restore fgrnd
fLockSusp	equ	01000000000000000000000000000000B
fLockSuspBit	equ	30		  ; != 0 if Locked suspended while Window
fDoResume	equ	10000000000000000000000000000000B
fDoResumeBit	equ	31		  ; != 0 if app needs first resume
;
; This mask is used to isolate the status bits shared with the VM_Descriptor
;
DescMask	equ	fWindowed + fExclApp + fBackground
DescSHFT	equ	1

;
; Masks and Bits defs for StatusFlags2
;
fDoingSizeMove	    equ 00000000000000000000000000000001B
fDoingSizeMoveBit   equ 0		  ; != 0 if app in middle of size or move
fDoingAltSpace	    equ 00000000000000000000000000000010B
fDoingAltSpaceBit   equ 1		  ; != 0 if app in middle of ALT-SPACE
fShowMainWnd	    equ 00000000000000000000000000000100B
fShowMainWndBit     equ 2		  ; != 0 if initial ShowWindow has been done
fInMenuMd	    equ 00000000000000000000000000001000B
fInMenuMdBit	    equ 3		  ; != 0 if in menu mode
fCtrlReset	    equ 00000000000000000000000000010000B
fCtrlResetBit	    equ 4		  ; != 0 if in process of restting control dlg state
fEverFocus	    equ 00000000000000000000000000100000B
fEverFocusBit	    equ 5		  ; != 0 if in process of restting control dlg state
fIconProb	    equ 00000000000000000000000001000000B
fIconProbBit	    equ 6		  ; != 0 if had trouble making ICON
fWkDirDDE	    equ 00000000000000000000000010000000B
fWkDirDDEBit	    equ 7		  ; != 0 if working dir set via DDE
fTitleDDE	    equ 00000000000000000000000100000000B
fTitleDDEBit	    equ 8		  ; != 0 if title set via DDE
fDoneCmpct	    equ 00000000000000000000001000000000B
fDoneCmpctBit	    equ 9		  ; != 0 if compacted for create VM
fBadPaintRep	    equ 00000000000000000000010000000000B
fBadPaintRepBit     equ 10		  ; != 0 if bad paint reported
fScreenSem	    equ 00000000000000000000100000000000B
fScreenSemBit	    equ 11		  ; != 0 if screen sem reported
fWinMouse	    equ 00000000000000000001000000000000B
fWinMouseBit	    equ 12		  ; == 0 if mouse is for selection
fChangedFont	    equ 00000000000000000010000000000000B
fChangedFontBit	    equ 13		  ; != 0 if fonts changed for current paint
fOldMouseWay	    equ 00000000000000000100000000000000B
fOldMouseWayBit     equ 14		  ; != 0 if mouse usage is 3.1 style
fHasCalledGrb       equ 00000000000000001000000000000000B
fHasCalledGrbBit    equ 15		  ; != 0 if grabber has been called for pnt info
fDoneHkProbDlg	    equ 00000000000000010000000000000000B
fDoneHkProbDlgBit   equ 16		  ; != 0 if have put up msgBadHtKy

;
; Masks and Bits defs for GlobalFlags in cdata
;
SwitcherSet	equ	0000000000000001B
SwitcherSetBit	equ	0		  ; != 0 if changing activation on switch
FirstALTB	equ	0000000000000010B
FirstALTBBit	equ	1		  ; == 0 if this is first ALT-TAB/ESC ever in session
HaveCmdEnvSz	equ	0000000000000100B
HaveCmdEnvSzBit equ	2		  ; != 0 if CmdEnvSz var in cdata set.
NoPaging	equ	0000000000001000B
NoPagingBit	equ	3		  ; != 0 if paging is off

;
; Masks and Bits defs for PfW386Flags in PIF file and pifW386Flags
;
fEnableClose		equ	00000000000000000000000000000001b
fEnableCloseBit 	equ	0		; Enable close at all times
fNewBackgrnd		equ	00000000000000000000000000000010b
fNewBackgrndBit 	equ	1		; New location of background bit
fNewExclusive		equ	00000000000000000000000000000100b
fNewExclusiveBit	equ	2		; New location of exclusive bit
;** fNewFullScr is also defined in FONTDLG.C!!!
fNewFullScr		equ	00000000000000000000000000001000b
fNewFullScrBit		equ	3		; New loc of full screen bit

fALTTABdis		equ	00000000000000000000000000100000b
fALTTABdisBit		equ	5		; disable ALT-TAB
fALTESCdis		equ	00000000000000000000000001000000b
fALTESCdisBit		equ	6		; disable ALT-ESCAPE
fALTSPACEdis		equ	00000000000000000000000010000000b
fALTSPACEdisBit 	equ	7		; disable ALT-SPACE
fALTENTERdis		equ	00000000000000000000000100000000b
fALTENTERdisBit 	equ	8		; disable ALT-ENTER
fALTPRTSCdis		equ	00000000000000000000001000000000b
fALTPRTSCdisBit 	equ	9		; disable ALT-PRTSC
fPRTSCdis		equ	00000000000000000000010000000000b
fPRTSCdisBit		equ	10		; disable PRTSC
fCTRLESCdis		equ	00000000000000000000100000000000b
fCTRLESCdisBit		equ	11		; disable CTRL-ESC
fPollingDetect		equ	00000000000000000001000000000000b
fPollingDetectBit	equ	12		; Auto Idle detect
fNoHMA			equ	00000000000000000010000000000000b
fNoHMABit		equ	13		; disable HMA use for VM
fPifHasHotKey		equ	00000000000000000100000000000000b
fPifHasHotKeyBit	equ	14		; Has a Hot Key
fEMSLocked		equ	00000000000000001000000000000000b
fEMSLockedBit		equ	15		; EMS is locked
fXMSLocked		equ	00000000000000010000000000000000b
fXMSLockedBit		equ	16		; XMS is locked
fINT16Paste		equ	00000000000000100000000000000000b
fINT16PasteBit		equ	17		; Paste via int 16
fVMLocked		equ	00000000000001000000000000000000b
fVMLockedBit		equ	18		; VM is to be Always LOCKED.

;
; This mask is used to isolate the status bits shared with the VM_Descriptor
;
PifDescMask equ fALTTABdis + fALTESCdis + fALTSPACEdis + fALTENTERdis +\
		fALTPRTSCdis + fPRTSCdis + fCTRLESCdis + fPollingDetect +\
		fNoHMA + fPifHasHotKey + fEMSLocked + fXMSLocked +\
		fINT16Paste + fVMLocked
;
; Bits of VD_Flags2
;
fVDDMask		equ	00000000000000001111111111111111b
fVDDMinBit		equ	0
fVDDMaxBit		equ	15

fVidTxtEmulate		equ	00000000000000000000000000000001b
fVidTxtEmulateBit	equ	0
fVidNoTrpTxt		equ	00000000000000000000000000000010b
fVidNoTrpTxtBit 	equ	1
fVidNoTrpLRGrfx 	equ	00000000000000000000000000000100b
fVidNoTrpLRGrfxBit	equ	2
fVidNoTrpHRGrfx 	equ	00000000000000000000000000001000b
fVidNoTrpHRGrfxBit	equ	3
fVidTextMd		equ	00000000000000000000000000010000b
fVidTextMdBit		equ	4
fVidLowRsGrfxMd 	equ	00000000000000000000000000100000b
fVidLowRsGrfxMdBit	equ	5
fVidHghRsGrfxMd 	equ	00000000000000000000000001000000b
fVidHghRsGrfxMdBit	equ	6
fVidRetainAllo		equ	00000000000000000000000010000000b
fVidRetainAlloBit	equ	7

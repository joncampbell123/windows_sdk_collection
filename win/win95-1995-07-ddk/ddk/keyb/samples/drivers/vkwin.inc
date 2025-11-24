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

;;; VKOEM.INC ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
if1
%out Microsoft VKWIN.INC
endif
; ======================================================================
;  Virtual Keys, Standard Set < 80H
; ======================================================================

VK_LBUTTON	= 01H
VK_RBUTTON	= 02H
VK_CANCEL	= 03H

; 4..7 undefined

VK_BACK		= 08H
VK_TAB		= 09H

; 0ah .. 0bh undefined

VK_CLEAR	= 0cH
VK_RETURN	= 0dH

VK_SHIFT	= 10H
VK_CONTROL	= 11H
VK_MENU		= 12H
VK_PAUSE	= 13H
VK_CAPITAL	= 14H
		; 15h..1ah

VK_ESCAPE       = 1bH
                ; 1ch..1fh

VK_SPACE	= 20H
VK_PRIOR	= 21H	; page up
VK_NEXT		= 22H	; page down
VK_END		= 23H
VK_HOME		= 24H
VK_LEFT		= 25H
VK_UP		= 26H
VK_RIGHT	= 27H
VK_DOWN		= 28H
VK_SELECT	= 29H
VK_PRINT	= 2aH	; only used by Nokia..
VK_EXECUTE	= 2bH
VK_SNAPSHOT	= 2ch	; Printscreen key..

VK_INSERT	= 2dH
VK_DELETE	= 2eH
VK_HELP		= 2fH
VK_0		= 30H
VK_1		= 31H
VK_2		= 32H
VK_3		= 33H
VK_4		= 34H
VK_5		= 35H
VK_6		= 36H
VK_7		= 37H
VK_8		= 38H
VK_9		= 39H
		; 40h
VK_A		= 41H
VK_B		= 42H
VK_C		= 43H
VK_D		= 44H
VK_E		= 45H
VK_F		= 46H
VK_G		= 47H
VK_H		= 48H
VK_I		= 49H
VK_J		= 4AH
VK_K		= 4BH
VK_L		= 4CH
VK_M		= 4DH
VK_N		= 4EH
VK_O		= 4FH
VK_P		= 50H
VK_Q		= 51H
VK_R		= 52H
VK_S		= 53H
VK_T		= 54H
VK_U		= 55H
VK_V		= 56H
VK_W		= 57H
VK_X		= 58H
VK_Y		= 59H
VK_Z		= 5AH

VK_LWIN		= 05bH
VK_RWIN		= 05cH
VK_APPS		= 05dH

		; 5eh..5fh

VK_NUMPAD0	= 60H
VK_NUMPAD1	= 61H
VK_NUMPAD2	= 62H
VK_NUMPAD3	= 63H
VK_NUMPAD4	= 64H
VK_NUMPAD5	= 65H
VK_NUMPAD6	= 66H
VK_NUMPAD7	= 67H
VK_NUMPAD8	= 68H
VK_NUMPAD9	= 69H
VK_MULTIPLY	= 6AH
VK_ADD		= 6BH
VK_SEPARATER	= 6CH
VK_SUBTRACT	= 6DH
VK_DECIMAL	= 6EH
VK_DIVIDE	= 6FH

VK_F1		= 70H
VK_F2		= 71H
VK_F3		= 72H
VK_F4		= 73H
VK_F5		= 74H
VK_F6		= 75H
VK_F7		= 76H
VK_F8		= 77H
VK_F9		= 78H
VK_F10		= 79H
VK_F11		= 7aH
VK_F12		= 7bH
VK_F13		= 7cH
VK_F14		= 7dH
VK_F15		= 7eH
VK_F16		= 7fH

VK_NUMLOCK	= 90H


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

PAGE 58,132
;******************************************************************************
TITLE vkdmsg.asm -
;******************************************************************************
;
;   Title:	vkdmsg.asm - strings for international conversion
;
;   Version:	1.00
;
;==============================================================================
.386

	INCLUDE VMM.INC
CREATE_MESSAGES EQU VMM_TRUE
        INCLUDE MSGMACRO.INC
        INCLUDE VKDMSG.INC


VxD_IDATA_SEG

PUBLIC Kbd_Boost_Time
PUBLIC Kbd_Alt_Delay
PUBLIC Kbd_Paste_TimeOut
PUBLIC Kbd_Paste_Pause_Ini
PUBLIC Kbd_CR_Paste_Pause_Ini
PUBLIC Kbd_SlowPaste_Delay
PUBLIC Kbd_AltPaste_Delay
PUBLIC Kbd_PasteBuf_Delay
PUBLIC Kbd_MCA_Passwd_Ini
PUBLIC Kbd_8042_ReadCmd_Ini
PUBLIC Kbd_8042_WriteCmd_Ini

IFDEF Support_Reboot
PUBLIC Kbd_Reboot_Ini
ENDIF

BeginMsg
;
; VKD INI switch equates
;
Kbd_Boost_Time		db 'KeyBoostTime', 0
;   Amount of extra execution time given to a VM after each key stroke.
;   Specified in seconds.  Default 0.001

Kbd_Alt_Delay		db 'AltKeyDelay', 0
;   Amount of time to delay after each ALT key is simulated into a VM.	This
;   is mainly for updating the ALT state after VM switching for applications
;   which base menu mode on an ALT state.  Default .005

Kbd_Paste_TimeOut	db 'KeyPasteTimeout', 0
;   Amount of time to wait to see if a VM will allow for INT 16 pasting
;   Specified in seconds.  Default 1.000

Kbd_Paste_Pause_Ini	db 'KeyPasteSkipCount', 0
;   Number of read status INT 16 calls which should return buffer empty status
;   before another character is pasted.  Default 2

Kbd_CR_Paste_Pause_Ini	db 'KeyPasteCRSkipCount', 0
;   Number of read status INT 16 calls which should return buffer empty status
;   before another character is pasted after a RETURN is pasted.  Default 10

Kbd_SlowPaste_Delay	db 'KeyPasteDelay', 0
;   Delay time between each key stroke when slow (hardware) pasting is required.
;   The delay allows programs to keep up with the pasting, because it slows it
;   down so that it is similar to being humanly typed.
;   Specified in seconds.  Default 0.003

Kbd_AltPaste_Delay	db 'AltPasteDelay', 0
;   Extra delay alter ALT keys are pasted when slow pasting
;   Specified in seconds.  Default 0.025

Kbd_PasteBuf_Delay	db 'KeyBufferDelay', 0
;   Extra delay after detecting that the DOS keyboard buffer is almost full
;   Specified in seconds.  Default 0.200

Kbd_MCA_Passwd_Ini	db 'KYBDPASSWD', 0
;   set to TRUE, if VKD should support PS/2 8042 commands that implement
;   password security.	This only works on 8042 keyboard controllers which
;   are compatible with the PS/2.
;
;   default FALSE, if not PS/2
;	    TRUE, if PS/2

Kbd_8042_ReadCmd_Ini	db '8042READCMD', 0
;   define an 8042 command which returns data through port 60h
;
;   SYTAX:  8042ReadCmd =cmdnum [, [cnt | -] [, None | Focus | All]]
;
;	the first parameter is "cmdnum" which is the 8042 command number in hex
;	the 2nd parameter is optional and can be blank, a "-" or a count in
;	   decimal, if blank, then count = 0 (no data is returned thru 60h)
;	   if "-", then a variable # of bytes is returned terminated with a nul
;	   byte, if a count is given, then it must be less than 32 and it
;	   indicates the number of bytes which will be returned thru port 60h
;	the 3rd parameter is also optional and specifies in which VM's the
;	   command is legal (passed to the physical 8042).  Possible values
;	   are:  None, Focus, All.  Only the first letter is checked.  NONE
;	   means that the command will be ignored in all VM's, it will return
;	   the specified number of nul bytes (1 nul, if count = '-').  FOCUS
;	   means that the command will be ignored in all VM's except the
;	   current keyboard focus VM.  ALL means that the command is legal
;	   in all VM's.
;
;	i.e.	8042ReadCmd=A4,1	defines the PS/2 test password cmd
;					for all VM's
;		8042ReadCmd=A5,1,F	defines Compaq special status read
;					for focus VM

Kbd_8042_WriteCmd_Ini	db '8042WRITECMD', 0
;   define an 8042 command which requires additional data to be written to
;   port 60h
;
;   SYTAX:  8042WriteCmd=cmdnum [, [cnt | -] [, None | Focus | All]]
;
;	the first parameter is "cmdnum" which is the 8042 command number in hex
;	the 2nd parameter is optional and can be blank, a "-" or a count in
;	   decimal, if blank, then count = 0 (no data is required to be written
;	   to 60h) if "-", then a variable # of bytes can be written to 60h
;	   followed by a num byte, if a count is given, then it must be less
;	   than 32 and it indicates the number of bytes which are required
;	   to be written to port 60h
;	the 3rd parameter is also optional and specifies in which VM's the
;	   command is legal (passed to the physical 8042).  Possible values
;	   are:  None, Focus, All.  Only the first letter is checked.  NONE
;	   means that the command will be ignored in all VM's, the specified
;	   count of bytes written to 60h will also be ignored.	FOCUS
;	   means that the command will be ignored in all VM's except the
;	   current keyboard focus VM.  ALL means that the command is legal
;	   in all VM's.
;
;	i.e.	8042WriteCmd=A5,-,F	defines PS/2 load password cmd for
;					focus VM
;		8042WriteCmd=A6,0,F	defines PS/2 enable security cmd
;					for focus VM

IFDEF Support_Reboot
Kbd_Reboot_Ini		db 'KYBDREBOOT', 0
;   Set to FALSE, if VKD should not attempt to reboot in response to
;   Ctrl+Alt+Del.  Default is TRUE - attempt to reboot
ENDIF

PUBLIC Key_Delay_Ini

Key_Delay_Ini	     db  'KEYIDLEDELAY', 0
;   Set to 0.5, if none.  Ignore all idle calls until specified # of
;   seconds pass after simulating keys into a VM.  Improves perceived response
;   time, because it prevents pre-paging while typing.	This also fixes the
;   bug caused by the special ALT key processing which the HP 486 does.


ifdef JAPAN
;   Some of Japanese 106 keyboard needs Shift down state to generate WM_CAPTAL
;   When VKD want to post 3A scan code of CAPS-Lock key down, we need to 
;   simulate Shift+[3Ah] Scan Code].
;   To distinguish the used keyboard, we need to see win.ini [keyboard] sect.
;   
;   [Keyboard] section of WIN.INI
;   
;   type=<value>
;    7: Japanese Keyboard
;
;   subtype=<value>
;    0: 101 keyboard
;    1: AX keyboard
;    2: 106 keyboard
;    3: IBM-5576 002/003 keyboard
;    4: IBM-5576 001 keyboard
;
PUBLIC Kbd_Sec_Ini
Kbd_Sec_Ini	     db  'KEYBOARD', 0
PUBLIC Key_Type_Ini
Key_Type_Ini	     db  'TYPE', 0
PUBLIC Key_SubType_Ini
Key_SubType_Ini	     db  'SUBTYPE', 0
endif


EndMsg

VxD_IDATA_ENDS

END

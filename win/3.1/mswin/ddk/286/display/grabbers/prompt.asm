
		page	,132
		%out	PromptGrab
		name	PROMPT
		title	Copyright (c) Hewlett-Packard Co. 1985-1987


IFDEF	GRAB_VER_2

;
; NAME
;	PROMPT.ASM
;
; DESCRIPTION
;	This file contains the (now defunct) original grabber entrypoint
;	PromptGrab.
;
; AUTHOR
;	Jim Christy
;
; HISTORY
;	1.00	060187	jhc	Epoch
;       1.01    080989   ac     IFDEFed out portions which have changed or are
;                               not needed under windows 3.0 winoldaps.


_TEXT		segment byte	public	'CODE'
		assume	cs:_TEXT


		public	PromptGrab
		subttl	PromptGrab


;
; PromptGrab - scan screen and display restart message if screen is nonblank
;
;	This entrypoint is no longer called by Winoldap 2.0 since the new
;	Winoldap uses menus and titlebars (similar to GoodOldAp operation) to
;	let the user close the inactive application.  It exists here as a stub
;	to prevent older Winoldaps and WIN.COM from crashing if they attempt
;	to call this function.	WIN.COM calls the loads and calls the grabber
;	during execution of an UglyOldAp.
;
; ENTRY
;	ds	=  cs
; EXIT
;	none
; USES
;	none
;
PromptGrab	proc	far
		assume	ds:_TEXT
		ret
PromptGrab	endp


_TEXT		ends
		end

ENDIF	;GRAB_VER_2

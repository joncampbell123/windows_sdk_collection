
		page	,132
		%out	PromptGrab
		name	PROMPT
		title	Copyright (C) by Microsoft Inc. 1985-1987


_TEXT		segment byte	public	'CODE'
		assume	cs:_TEXT


		public	PromptGrab
		subttl	PromptGrab


;
; PromptGrab - scan screen and display restart message if screen is nonblank
;
;	This entrypoint is no longer called by Winoldap 2.0 since the new
;	Winoldap uses menus and titlebars (similar to GoodOldAp operation)
;	to let the user close the inactive application.  It exists here as
;	a stub to prevent older Winoldaps from crashing if they attempt to
;	call this function.
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


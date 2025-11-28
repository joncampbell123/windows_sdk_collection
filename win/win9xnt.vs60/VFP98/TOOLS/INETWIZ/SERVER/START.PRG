LOCAL llStart,lnSeconds

CD\server
DEFINE BAR 13 OF _mwizards PROMPT "\<WWW Search Page Wizard"
ON SELECTION BAR 13 OF _mwizards do c:\server\wwwpage.app
ACTIVATE SCREEN
CLEAR
ERASE exit.nul
llStart=.T.
_screen.AddObject('tmrExitServerCheck','ExitServerCheck')
DO WHILE .T.
	IF CHRSAW() OR INKEY('HM')>0
		EXIT
	ENDIF
	IF NOT EMPTY(SYS(2000,'server.app'))
		llStart=.F.
		ACTIVATE SCREEN
		CLEAR
		ERASE exit.nul
		CLEAR PROGRAM
		DO server.app
		IF EMPTY(SYS(2000,'exit.nul'))
			EXIT
		ENDIF
		ERASE exit.nul
		ERASE server.app
		lnSeconds=SECONDS()
		DO WHILE ABS(SECONDS()-lnSeconds)<2
		ENDDO
		CLEAR
		? 'Press any key to exit to the Command window...'
	ENDIF
ENDDO
_screen.RemoveObject('tmrExitServerCheck')
ACTIVATE SCREEN
CLEAR
RETURN



DEFINE CLASS ExitServerCheck AS Timer

	Interval=1000
	
	FUNCTION Timer
	IF NOT EMPTY(SYS(2000,'exit.nul'))
		_screen.ActiveForm.Release()
	ENDIF
	ENDFUNC

ENDDEFINE





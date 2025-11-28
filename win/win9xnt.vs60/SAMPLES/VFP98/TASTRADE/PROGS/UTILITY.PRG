*-- (c) Microsoft Corporation 1995

*-- General purpose utility functions independent of any classes
*-- for better performance and accessibility

#INCLUDE "INCLUDE\TASTRADE.H"

************************************
FUNCTION IsTag (tcTagName, tcAlias)
  *-- Receives a tag name and an alias (which is optional) as
  *-- parameters and returns .T. if the tag name exists in the
  *-- alias. If no alias is passed, the current alias is assumed.
  LOCAL llIsTag, ;
        lcTagFound

  IF PARAMETERS() < 2
    tcAlias = ALIAS()
  ENDIF
  
  IF EMPTY(tcAlias)
    RETURN .F.
  ENDIF

  llIsTag = .F.
  tcTagName = UPPER(ALLTRIM(tcTagName))

  lnTagNum = 1
  lcTagFound = TAG(lnTagNum, tcAlias)
  DO WHILE !EMPTY(lcTagFound)
    IF UPPER(ALLTRIM(lcTagFound)) == tcTagName
      llIsTag = .T.
      EXIT
    ENDIF
    lnTagNum = lnTagNum + 1
    lcTagFound = TAG(lnTagNum, tcAlias)
  ENDDO

  RETURN llIsTag
ENDFUNC

FUNCTION NotYet()
  *-- Used during construction of Tastrade to indicate those
  *-- parts of the application that were not yet completed.
  =MESSAGEBOX(NOTYET_LOC, MB_ICONINFORMATION)
  RETURN
ENDFUNC

FUNCTION FileSize(tcFileName)
  *-- Returns the size of a file. SET COMPATIBLE must be ON for
  *-- FSIZE() to return the size of a file. Otherwise, it returns
  *-- the size of a field.
  LOCAL lcSetCompatible, lnFileSize

  lcSetCompatible = SET('COMPATIBLE')
  SET COMPATIBLE ON
  lnFileSize = FSIZE(tcFileName)
  SET COMPATIBLE &lcSetCompatible
  RETURN lnFileSize
ENDFUNC

FUNCTION FormIsObject()
  *-- Return .T. if the active form is of type "O" and its baseclass
  *-- is "Form". 
  RETURN (TYPE("_screen.activeform") == "O" AND ;
          UPPER(_screen.ActiveForm.BaseClass) = "FORM")
ENDFUNC

FUNCTION ToolBarEnabled
  *- Return value of Toolbar object
  PARAMETER oObject
  LOCAL oToolObj
  oToolObj = "oApp.oToolBar." + oobject + ".enabled"
  IF TYPE(oToolObj) # "L"
    RETURN .F. 
  ELSE
    RETURN EVAL(oToolObj)
  ENDIF
ENDFUNC

FUNCTION OnShutdown()
  *-- Custom message called via the ON SHUTDOWN command to indicate
  *-- that the user must exit Tastrade before exiting Visual Foxpro.
  =MESSAGEBOX(CANNOTQUIT_LOC, ;
              MB_ICONEXCLAMATION, ;
              TASTRADE_LOC)
ENDFUNC
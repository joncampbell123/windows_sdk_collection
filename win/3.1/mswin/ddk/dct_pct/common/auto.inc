'$include: 'mfdll.inc'


CONST PRINTERSINI = "C:\CUSTOM\PRINTERS.INI"
CONST TESTSINI    = "C:\T2\T2TESTS.INI"



DECLARE FUNCTION GetProfileString LIB "KERNEL" (a$, k$, d$, b$, l% ) AS INTEGER


'******************************************************************************
'**********************   G E T   I N I   S T R I N G   ***********************
'******************************************************************************

DECLARE FUNCTION GetINIString$( a$, k$, fini$ )

FUNCTION GetINIString$( a$, k$, fINI$ )
  DIM buf$, rc%
  buf$ = String$(128," ")
  rc%  = GetPrivateProfileString( a$, k$, "", buf$, len(buf$), fINI$ )
  GetINIString$ = mid$(buf$,1,rc%)
END FUNCTION


'******************************************************************************
'****************************   G E T   A P P    ******************************
'******************************************************************************

DECLARE FUNCTION GetApp$( a$, section$, index% )

FUNCTION GetApp$( a$, section$, index% )
  DIM App$
  app$ = "app" + ltrim$(rtrim$(str$(index%)))
  GetApp$ = GetINIString( section$, app$, a$ )
END FUNCTION  


'******************************************************************************
'************************   G E T   D O C U M E N T   *************************
'******************************************************************************

DECLARE FUNCTION GetDoc$( d$, section$, index% )

FUNCTION GetDoc$( d$, section$, index% )
  DIM Document$
  document$ = "doc" + ltrim$(rtrim$(str$(index%)))
  GetDoc$ = GetINIString( section$, document$, d$ )
END FUNCTION  


'******************************************************************************
'************************   G E T   P R I N T E R   ***************************
'******************************************************************************

DECLARE FUNCTION GetPrinter$( f$, section$, index% )

FUNCTION GetPrinter$( f$, section$, index% )
  DIM Model$
  model$ = "model"+ltrim$(rtrim$(str$(index%)))
  GetPrinter$ = GetINIString( section$, model$, f$ )
END FUNCTION


'******************************************************************************
'************************   U S E   P R I N T E R    **************************
'******************************************************************************

DECLARE SUB UsePrinter( p$ )

SUB UsePrinter( p$ )
  DIM rc%
  DIM s$

  s$ = GetINIString( p$, "share", PRINTERSINI )
  if s$ = "" then
    print "Error, printer not in INI: '"; p$; "'"
    end
  endif

  rc% = WNetCancelConnection( "LPT1:", 0 )
  rc% = WNetAddConnection( s$, "", "LPT1:" ) 
  IF rc% <> 0 THEN
    print "Error connecting to printer: '"; s$; "' - rc% = "; rc%
    END
  END IF

END SUB


'******************************************************************************
'*****************   S E T   D E F A U L T   P R I N T E R   ******************
'******************************************************************************
'
' This routine changes the default printer to Model$ on LPT1. First it
' looks up the model in the 'PrinterPorts' section of WIN.INI and gets
' the driver name. Then it constructs the default printer string and
' enters it into the 'Windows' section of WIN.INI under key name 'device'.
'

DECLARE SUB SetDefaultPrinter( Model$ )

SUB SetDefaultPrinter( Model$ )
  DIM buf$, drv$
  DIM rc%, i1%


  buf$ = "                                                              "
  rc% = GetProfileString( "PrinterPorts", Model$, chr$(0), buf$, len(buf$) )
  if rc% = 0 then
    print "Error getting printer string"
    end
  end if
  
  i1% = instr( buf$, "," )

  drv$  = mid$( buf$, 1, i1%-1 )
  
  Dev$ = Model$ + "," + drv$ + ",LPT1:"
  if WriteProfileString( "Windows", "device", Dev$ ) = 0 then
    print "**** Error setting default printer: '"; Dev$; "'"
    end
  end if

  UsePrinter Model$

END SUB


'******************************************************************************
'*****************   G E T   D E F A U L T   P R I N T E R   ******************
'******************************************************************************

DECLARE FUNCTION GetDefaultPrinter() AS STRING

FUNCTION GetDefaultPrinter() AS STRING
  DIM buf$, Model$, Port$
  DIM rc%, i1%, i2%, i3%

  buf$ = "                                                              "
  rc% = GetProfileString( "Windows", "device", chr$(0), buf$, len(buf$) )
  if rc% = 0 then
    print "Error getting default printer string"
    end
  end if
  
  i1% = instr( buf$, "," )
  i2% = instr( i1%+1, buf$, "," )
 
  Model$ = mid$( buf$, 1, i1%-1 )
  Port$  = mid$( buf$, i2%+1 )
  
  GetDefaultPrinter = Model$ + " on " + Port$

END FUNCTION


'******************************************************************************
'****************************   R U N   A P P   *******************************
'******************************************************************************
'
' This routine notes the current directory, changes to the requested
' starting directory and runs the given command, then changes back to
' the original starting directory.
'

DECLARE SUB RunApp( ExeName$, StartDir$ )

SUB RunApp( ExeName$, StartDir$ )
  DIM d$
  d$ = CurDir$                      ' Note Original Directory
  if MID$(StartDir$,2,1) = ":" then ' Is Drive Specified?
    ChDrive StartDir$               '   Change to Starting Drive
  endif
  ChDir   StartDir$                 ' Change to Starting Directory
  Run ExeName$, NOWAIT              ' Run Command Asynchronously
  ChDrive d$                        ' Change Back to Original Drive
  ChDir   d$                        ' Change Back to Original Directory
END SUB


'******************************************************************************
'*********************   P R I N T   T H E   S C R E E N   ********************
'******************************************************************************

DECLARE SUB PrintTheScreen( a$ )

SUB PrintTheScreen( a$ )
  p$ = GetINIString( a$, "PrintScreen", TESTSINI )
  if p$ = "ON" then
    Print "Printing the screen"
    PrintScreen "HP LaserJet Series II", "LPT3:"
    Print "Finished printing the screen"
  endif
END SUB


'******************************************************************************
'*****************************   B U S Y   ************************************
'******************************************************************************

DECLARE SUB Busy()

SUB Busy()                      ' just a little easier way to
  While IsCursor( MFC_WAIT )    ' use Mike's IsCursor function
  Wend
END SUB


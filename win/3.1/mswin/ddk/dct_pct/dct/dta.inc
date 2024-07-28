'-------------------------------------------------------------------------
'
' NAME     : DTA.INC
' DESC     : Contains common routines used by DTA-PEN.WTD, DTA-BRU.WTD, 
'            DTA-PRIM.WTD, DTA-BKMO.WTD.
' DATE     : 10-03-91
' 
'-------------------------------------------------------------------------

'$IFNDEF SMOKE
'$DEFINE PROMPT
'$ENDIF

'$include: 'mfdll.inc'
'$include: 'wtdutil.inc'
'$IFDEF CTSHELL
   '$include: 'hct.inc'
'$ENDIF

CONST TESTAPP  = "dta"                         ' name of app w/o extension
CONST WINTITLE = "Display Test Application"    ' window title
CONST LOG      = "dta-test.log"                ' name of log file
CONST MAXREP   = 5        ' repeat this many times when script says several
CONST DISP_EXT = ".zzz"                        ' all display tests will 
                                               '  write results to this 
                                               '  extension.

GLOBAL  hDTA AS INTEGER                        ' handle to DTA window

DECLARE SUB InitApp
DECLARE SUB EndScript
DECLARE SUB EndTest (szTestName AS STRING)
DECLARE SUB SelectOptionMenu (menu$)
DECLARE SUB DrawObject
DECLARE SUB PrintPause (szMsg AS STRING)
DECLARE SUB ResetDTA
DECLARE SUB SelectComboBoxItem (nIndex%)
DECLARE SUB StartScript
DECLARE SUB StartTest (szTestName AS STRING, nSection AS INTEGER, lNum AS LONG)
DECLARE SUB TestHeader (testName$)

'--------------------------------------------------------------------------
'--------------------------------------------------------------------------

SUB selectOptionMenu (menu$)
   ' win30 doesn't do selectMenuItem() !@&@#!!!

   SendKeys "%o" + menu$   ' SelectMenuItem "Options" SelectMenuItem menu$
END SUB

'--------------------------------------------------------------------------

SUB drawObject
   '$IFDEF PROMPT
      hWnd% = GetActiveWindow ()
      SendKeys  "{F3}"                      ' draw the shape      
      WHILE hwnd% = GetActiveWindow ()          
         ' wait for prompt window to pop up...
      WEND
  
      WHILE hwnd% <> GetActiveWindow ()         
         ' wait for user to answer prompt...
      WEND
   '$ELSE
       SendKeys  "{F3}"                      ' draw the shape              
   '$ENDIF
   
'   Inc nVar                               ' add one to variation counter
END SUB

'-------------------------------------------------------------------------

SUB EndScript
   Print "In EndScript..."
   '$IFDEF CTSHELL
      lRetCode = SendMessage (hDTA, 10003, 0, 0)            
      HctScriptEnd RS_PASSED      
   '$ENDIF
   PrintPause "End of " + SCRIPT + " Test"   
   SelectWindow WINTITLE     ' make sure we're closing the right app...
   SendKeys "%{F4}"          ' terminate app ("I'LL BE BACK!!")
END SUB

'-------------------------------------------------------------------------

SUB EndTest (szTestName AS STRING)

   PrintPause "End of " + szTestName + " Test"
END SUB

'--------------------------------------------------------------------------

SUB SelectComboBoxItem (nIndex%)
   ' allocate buffer
   szComboText$ = "                                                    " 
   GetComboBoxItemText nIndex%, szComboText$, 20
   ClickComboBoxItem szComboText$
END SUB

'--------------------------------------------------------------------------

SUB InitApp
   IF EXISTS (TESTAPP + ".exe") = 0 THEN
'$IFNDEF CTSHELL
      LogPause SCRIPT + ": Unable to initialize [" + TESTAPP + ".exe]"
'$ELSE
      Pause TEST_NAME + ": Unable to initialize [" + TESTAPP + ".exe]"
'$ENDIF
      END                ' terminate test script
   ELSE 
      RUN TESTAPP + " " + TEST_NAME, NOWAIT
'      WAIT 2                          'wait for window to be registered
      SelectWindow   WINTITLE          ' select testapp's window

      IF WindowExists ("Logging Options") THEN
         SelectWindow "Logging Options"
         ' This is the debug options - only comes up first time to 
         '   create a DTA section in WIN.INI
         SendKeys "%f"                 ' choose log to file
         ClickButton "OK"              ' close debug options dlg
      ENDIF

      hDTA = GetActiveWindow()         ' save the handle to DTA window
      
      selectOptionMenu "n"             ' select points dlg
      SendKeys "%1{tab}50{tab}50"      ' turn off random checkboxes
      SendKeys "%2{tab}200{tab}200"    ' turn off random checkboxes      
      SendKeys "%3%4"                  ' turn these off too
      SendKeys "%k"                    ' ClickButton "O&K"

      '$IFDEF PROMPT
         ' turn on smart prompting
         SelectOptionMenu "s"          ' SelectMenuItem "Smart Prompting" 
      '$ENDIF
   END IF
END SUB

'-------------------------------------------------------------------------

SUB PrintPause (szMsg AS STRING)
   '$IFDEF PROMPT 
      Pause szMsg
   '$ENDIF
   Print szMsg
END SUB

'-------------------------------------------------------------------------

SUB ResetDTA
   SendKeys "%ot%od"    ' options: repeat last, default settings
END SUB

'-------------------------------------------------------------------------

SUB StartTest (szTestName AS STRING, nSection AS INTEGER, lNum AS LONG)
   DIM lRetCode AS LONG

   lRetCode = SendMessage (hDTA, 10001, nSection, lNum)
   PrintPause "Start Of " + szTestName + " Test"
END SUB

'-------------------------------------------------------------------------

SUB StartScript
   '$IFDEF CTSHELL
      HctScriptBegin  TESTAPP
   '$ELSE
      ViewPort ON
      ViewPort CLEAR
   '$ENDIF

   Print "In StartScript..."
   PrintPause "Start of " + SCRIPT + " Test"
END SUB


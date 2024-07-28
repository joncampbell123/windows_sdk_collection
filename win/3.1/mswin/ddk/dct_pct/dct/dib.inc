'$DEFINE PROMPT

CONST TESTAPP     = "DibView"      ' window title of exe
CONST TEST_PATH   = "dibview"     ' path to exe 
CONST DIBPATH     = "."            ' bmp files in dibview child dir
   
CONST FILE_FORMAT = "Dibview Bitmap Format Tests"    ' test subroutine id's for DIBTest 
CONST SCROLL      = "Dibview Bitmap Scroll Tests"    ' test subroutine id's for DIBTest 
CONST STRETCH     = "Dibview Bitmap Stretch Tests"   ' test subroutine id's for DIBTest       
CONST PALETTE     = "Dibview Palette Tests"          ' test subroutine id's for DIBTest       

GLOBAL hWndDibview    AS INTEGER

DECLARE FUNCTION DibError% (szFileFormat$) 
DECLARE SUB      DIBTest (DIBTestId$)
DECLARE SUB      EndScript (szScriptName AS STRING)
DECLARE SUB      InitApp (szTestId AS STRING)
DECLARE FUNCTION LoadFiles%
DECLARE SUB      ShowDibFile (szDibFile$)
DECLARE SUB      StartScript (szScriptName AS STRING)
DECLARE SUB      StretchScroll (szTestId$, szFileName$)

'-------------------------------------------------------------------------
'-------------------------------------------------------------------------

FUNCTION DibError% (szMisc$) 
   ' RETURNS: TRUE  if error detected
   '          FALSE if no error detected
   ' this error occurs when DIBVIEW is unable to show a DIB.  Could
   '   be there isn't enough memory to create the device dependent 
   '   bitmap used to display the dib on screen, or the DIB file was
   '   bad.  In either case, inform the user, move on to next DIB

   DIM szErrorMsg AS STRING

   IF (ButtonExists ("OK") > 0) THEN   ' error displaying DDB!
      ClickButton "OK"                 ' close error box
      szErrorMsg = "* DIBVIEW ERROR: " + szMisc$
      Print szErrorMsg   
      DibError% = TRUE
   ELSE
      DibError% = FALSE
   ENDIF
END FUNCTION

'------------------------------------------------------------------------=

SUB DIBTest (DIBTestId$)
   ' Displays successive DIB files (from directory specified by DIBPATH) then
   '   calls subroutine identified by DIBTestId$.  After subroutine is 
   '   called, the DIB file is closed.

   DIM szDataFile AS STRING

   Print "----- Start of " + DIBTestId$ + " -----"

   IF (LoadFiles% = FALSE) THEN
      Print "No appropriate dib files here!"
   ELSE
      FOR szDataFile IN FILELIST SORTED BY NAME
         ShowDibFile szDataFile

            ' check to see if we opened the file before testing it

         IF (DibError% (DIBTestId$) = FALSE) THEN
            Print ">>> Using DIB [" + szDataFile + "]"
            '$IFDEF DIB_SCRL
                StretchScroll DIBTestId$, szDataFile
            '$ELSEIFDEF DIB_STRE
                StretchScroll DIBTestId$, szDataFile
            '$ELSEIFDEF DIB_FORM
               FileFormatTest szDataFile
            '$ELSE
               Pause "OOPS! SUB DIBTest: Invalid parameter"
               END     ' terminate wattdrvr script
            '$ENDIF
            SendKeys   "^{F4}"             ' close MDI child
         ENDIF
         Print " "
      NEXT szDataFile
   ENDIF
   
   Print "----- End of " + DIBTestId$ + " Test -----"   
END SUB

'-------------------------------------------------------------------------

SUB endScript (szScriptName AS STRING)
   DIM lRet AS LONG

   '$IFDEF CTSHELL
      lRet = SendMessage (hWndDibview, 10001, 0, 0)      
   '$ENDIF
   '$IFDEF PROMPT
      Pause "End Of " + szScriptName
   '$ENDIF
   
   SelectWindow TESTAPP     ' make sure we're closing the right app...
   SendKeys "%{F4}"         ' terminate app ("I'LL BE BACK!!")
   Print "End of " + szScriptName
END SUB

'-------------------------------------------------------------------------

SUB InitApp (szTestID AS STRING)
   DIM szRunLine AS STRING
   
   Print "In InitApp [" + szTestID + "] -----------------"
   ChDir TEST_PATH                       
   fullName$ = TESTAPP + ".EXE"
   IF EXISTS (fullName$) = 0 THEN
      Print fullName$ + " does not exist."
      Pause fullName$ + " does not exist."
      Print "Script aborting."
      END   ' abort script
   ELSE 
      szRunLine = fullName$ + " /s" + chr$ (34) + szTestID + chr$ (34)
      Print "   Running [" + szRunLine + "]"
      RUN szRunLine, NOWAIT
      WAIT 1                           ' wait for window to be registered
      MaximizeWindow                        
      SelectWindow TESTAPP             ' select application to be tested
      hWndDibview = GetActiveWindow()  ' save handle to DibView
      Randomize Timer                  ' get seed      
   ENDIF    
END SUB

'-------------------------------------------------------------------------

FUNCTION LoadFiles%
   ' Fills the FileList structure used by DIBtest subroutine.
   ' Returns TRUE or FALSE indicating if an appropriate number of files
   '   are in the FileList structure.

      SETFILE DIBPATH + "\SAMCAT.DIB", ON
      SETFILE DIBPATH + "\OREGON2.RLE", ON
      SETFILE DIBPATH + "\LTHOUSE1.DIB", ON
      SETFILE DIBPATH + "\KRIS1.RLE", ON
      IF (CountSetFile% = 4) THEN
         LoadFiles% = TRUE
      ELSE
         LoadFiles% = FALSE
      ENDIF
END FUNCTION

'-------------------------------------------------------------------------

SUB ShowDibFile (szDibFile$)
   SendKeys "%fo%n"           ' "File"  "Open"  "File Name"
   SetEditText szDibFile$     ' insert filename
   ClickButton "OK"           ' show dib file
END SUB

'-------------------------------------------------------------------------

SUB StartScript (szScriptName AS STRING)
   ViewPort Off
   ViewPort CLEAR
'   echo on
   '$IFDEF PROMPT
      Pause "Start Of " + szScriptName
      Print "!!! Prompt Test Mode: User Verification Active !!!"
   '$ELSE
      Print "!!! Stress Test Mode: No user verification !!!"
   '$ENDIF
   Print "Start of " + LOG + " Test"
END SUB

'-------------------------------------------------------------------------

SUB StretchScroll (szTestId$, szFileName$)
   MAX_DIB_TYPE = 3              ' display formats + 1 (zero based arrays)                                    

   DIM nResults        AS INTEGER
   DIM nDisplayDIBType AS INTEGER
   DIM DisplayFormat$ (3)
   DisplayFormat$ (0) = "Use DIBs (to device)"
   DisplayFormat$ (1) = "Use DDBs"
   DisplayFormat$ (2) = "Use DIBs (to memory)"
   
   FOR nDisplayDIBType = 0 TO MAX_DIB_TYPE - 1
      SendKeys       "%o"     '   SelectMenuItem "Options"  
      ClickButton DisplayFormat$ (nDisplayDIBType)
      SELECT CASE szTestId$   
         CASE STRETCH
            SetButtonState "Stretch To Window", TRUE
         CASE SCROLL
            SetButtonState "Stretch To Window", FALSE
         CASE ELSE
            ' check for invalid parameter
            Print "OOPS! SUB stretchScroll: Invalid parameter"
            END     ' terminate wattdrvr script
      END SELECT
      ClickButton    "OK"

      '$IFDEF DIB_STRE 
         StretchDIB 
      '$ELSEIFDEF DIB_SCRL
         ScrollDIB 
      '$ENDIF

      IF (DibError% (DisplayFormat$ (nDisplayDIBType)) = FALSE) THEN
         SendKeys   "%-x"      ' maximize child window
         lRet = SendMessage (hWndDibview, 10000, 0, 0)
         WHILE ((ButtonExists("No")) OR (ButtonExists("Done")))
             WAIT 1
         WEND
         SendKeys   "%-r"      ' restore window for next stretch session
      ENDIF
   NEXT nDisplayDIBType
END SUB

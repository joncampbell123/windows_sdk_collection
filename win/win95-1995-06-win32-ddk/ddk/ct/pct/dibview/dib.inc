'============================================================================
'
' DIB.INC (3.0)
'
' Contains common routines used by DIBVIEW DCT automation script.
'
'
' DEFINES
'   CTSHELL    If defined, script will send messages to the CT shell and/or
'              application being automated so that pass/fail information
'              will be generated.
'
'   NO_PROMPT  Default action is to prompt the user for verification.  If
'              someone just wants to blaze through, define this symbol
'              and we won't stop.  We also won't send any msgs back to the
'              CT shell for verification...
'
'   COMPORT    Enables msgs to be printed on the comport using the ECHO ON
'              statement.
'
' History:
'  07/11/94 - davidkl - started port to 32-bit test 3.0
'  07/13/94 - davidkl - created history section, worked on localization issues
'  08/02/94 - davidkl - increased deadlock timeout in StartScript()
'  12/07/94 - davidkl - deadlock timeout increased to 5 min (3.0a default)
'  01/10/95 - davidkl - changed to read env. var for data path if msdct flag is set
'  01/17/95 - davidkl - house cleaning
'  01/27/95 - davidkl - added hct logging
'============================================================================

'$IFNDEF _DIB_INC
'$DEFINE _DIB_INC       'don't include data more than once

'----------------------------------------------------------------------------
CONST WINTITLE    = "DibView"                        ' window title of exe
CONST TEST_PATH   = "..\..\common\dibview"           ' path to exe
CONST DIBPATH     = "..\..\common\dibview"           ' path to bmp files


CONST RECURSE     = TRUE
CONST WM_REPORT_CARD = 10001     ' msg id's to send to dibview.
CONST WM_ASK_USER    = 10000
CONST CMD_SEP        = ","       ' separates items in Command$

CONST MAX_DEADLOCK_TIMEOUT = 300000 'how long to wait before deadlock error msg (5 min)

CONST FILE_FORMAT = "Dibview Bitmap Format Tests"    ' test subroutine id's for DIBTest
CONST SCROLL      = "Dibview Bitmap Scroll Tests"    ' test subroutine id's for DIBTest
CONST STRETCH     = "Dibview Bitmap Stretch Tests"   ' test subroutine id's for DIBTest
CONST PALETTE     = "Dibview Palette Tests"          ' test subroutine id's for DIBTest
CONST FILE_PRINT  = "Dibview Bitmap Printing Tests"  ' test subroutine id's for DIBTest

'$IFNDEF NO_PROMPT
   CONST MAX_REP     = 10                            ' how many times to repeat something
'$ELSE
   CONST MAX_REP     = 30
'$ENDIF

'++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
'To localize for a particular language:
'  Change these text strings to their equivalent in the desired language
CONST BAD_TEXT              = "=   BAD  "
CONST BMP_LOOK_FINE         = "     Does the bitmap display look fine?"
CONST BOGUS_TEXT            = "=   BOGUS"
CONST CANCEL_TEXT           = "Cancel"
CONST CANNOT_FIND_WND       = "Can't find DibView window, script aborting"
CONST CONTINUE_TEST         = ":  continue with test"
CONST DIBTEST_ABORT         = "DIBTest aborting - can't find dib files!"
CONST DIBVIEW_ERR           = "* DIBVIEW ERROR: "
CONST DOES_NOT_EXIST        = " does not exist."
CONST DT_INVALID_PARM       = "OOPS! SUB DIBTest: Invalid parameter"
CONST GOOD_TEXT             = "=   GOOD"
CONST NO_TEXT               = "No"
CONST REPORT_AND_CONTINUE   = ":  create report and continue with test"
CONST REPORT_AND_TERMINATE  = ":  create report and terminate the test"
CONST RET_FAIL_TO_CT        = " will return failure to the CT shell because "
CONST RET_PASS_TO_CT        = " will return pass to the CT shell because "
CONST SCRIPT_ABORT          = "Script aborting."
CONST TESTS_FAILED          = " test(s) failed,  and "
CONST TESTS_PASSED          = " test(s) passed."
CONST TESTS_PASSED_NO_FAIL  = " test(s) passed and NONE failed"
CONST USER_ABORT            = "Script Aborted By User"
CONST YES_TEXT              = "Yes"


'----------------------------------------------------------------------------

DECLARE SUB      PC_CloseApp
DECLARE SUB      ConfirmDisplay (szWinTitle$)
DECLARE SUB      DIBTest      (DIBTestId$)
DECLARE SUB      EndScript    (szScriptName AS STRING)
DECLARE SUB      InitApp      (szTestId AS STRING)
DECLARE SUB      ShowDibFile  (szDibFile$)
DECLARE SUB      StartScript  (szScriptName AS STRING)
DECLARE SUB      StretchScroll(szTestId$, szFileName$)
DECLARE FUNCTION DibError%    (szFileFormat$)
DECLARE FUNCTION LoadFiles%
DECLARE FUNCTION SetupBitmapSource (szPath AS STRING) AS STRING

'$define TESTCTRL
'$define TESTSCRN
'$include 'mstest.inc'
'$include 'winapi.inc'


''$include 'dib_ct.inc'    '7/11/94 - davidkl - this line replaces ct_util.inc
'$include 'util_30.inc'                 ' handy utility functions

'$ifdef CTSHELL
   '$include 'player.inc'                  ' play controls utility.
   On ERROR GOTO ErrorHandler              ' In ABORT.INC
'$endif CTSHELL

GLOBAL ghControls    AS INTEGER         ' handle to play controls window.
GLOBAL ghWndDibview  AS INTEGER         ' handle to dibview window.
GLOBAL gszHomeDir    AS STRING          ' where to make temp files.
GLOBAL gszNetDrive   AS STRING          ' drive letter of net drive holding bitmaps
GLOBAL gnNumBitmaps  AS INTEGER         ' how many bitmaps in the FILELIST
GLOBAL gszFailCount  AS INTEGER         ' keeps track of how many tests failed
GLOBAL gszPassCount  AS INTEGER         ' keeps track of how many tests passed
GLOBAL gszCancelFlag AS INTEGER         ' Flag to indicate that user cancelled script

'-------------------------------------------------------------------------
'-------------------------------------------------------------------------

SUB PC_CloseApp

   DIM hWnd AS INTEGER, nRet AS INTEGER

   '$ifdef CTSHELL
      nRet = CloseControls()
   '$endif

   'this code doesn't seem to work correctly if script is aborted
   hWnd = WFndWnd (WINTITLE, FW_PART OR FW_FOCUS)
   WMenu MNU_FILE
   WMenu MITEM_EXIT

   '$IFDEF CTSHELL AND NO_PROMPT
      HctScriptEnd RS_PASSED
   '$ENDIF

   '$ifdef CTSHELL
      IF ERR = ERR_USER_ABORT then
         print USER_ABORT               'log to viewport user aborted script
         HctLog USER_ABORT           ' log abort to hct.log

         HctScriptEnd RS_FAILED      ' WAA! This doesn't work.  I'll need to send a msg to dibview and tell it to fail.
      ELSE
      ENDIF
   '$endif

   if len (gszNetDrive) > 0 then
      nRet = NetDisconnect (gszNetDrive)
   endif
END SUB

'-------------------------------------------------------------------------

SUB ConfirmDisplay (szWinTitle AS STRING)

   DIM nRespond AS INTEGER, nRet AS INTEGER
   DIM Msg AS STRING, nTab AS STRING


   '$IFDEF CTSHELL AND NOT NO_PROMPT
      nTab = Chr$ (9)
      Msg =  BMP_LOOK_FINE + Chr$ (13) +  Chr$ (13)
      Msg = Msg + YES_TEXT + nTab + GOOD_TEXT + nTab + CONTINUE_TEST + Chr$ (13)
      Msg = Msg + NO_TEXT + nTab + BAD_TEXT + nTab + REPORT_AND_CONTINUE + Chr$ (13)
      Msg = Msg + CANCEL_TEXT + nTab + BOGUS_TEXT + nTab + REPORT_AND_TERMINATE + Chr$ (13) + Chr$ (13)
      MessageBeep &H20
      nRespond = MessageBox (GetActiveWindow (), msg$, szWinTitle$, MB_YESNOCANCEL)
      IF (nRespond = IDNO) THEN
         Inc gszFailCount
         HctLog "User Confirmed FAILURE."
'currently not supported in Win32
'         nRet = CreateProblemReport (GetActiveWindow())
      ELSEIF (nRespond = IDCANCEL) THEN
         Inc gszFailCount
         HctLog "User Cancelled... FAILURE."
'currently not supported in Win32
'         nRet = CreateProblemReport (GetActiveWindow())
         gszCancelFlag = 1
      ELSE
         Inc gszPassCount
      ENDIF
   '$ENDIF
END SUB

'-------------------------------------------------------------------------

FUNCTION DibError% (szMisc$)
   ' RETURNS: TRUE  if error detected
   '          FALSE if no error detected
   ' this error occurs when DIBVIEW is unable to show a DIB.  Could
   '   be there isn't enough memory to create the device dependent
   '   bitmap used to display the dib on screen, or the DIB file was
   '   bad.  In either case, inform the user, move on to next DIB

   DIM szErrorMsg AS STRING

   IF WButtonExists (OK_BUTTON)  THEN   ' error displaying DDB!
      WButtonClick OK_BUTTON            ' close error box
      szErrorMsg = DIBVIEW_ERR + szMisc$
      Print szErrorMsg
      HctLog szErrorMsg
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

   DIM szDataFile AS STRING, nCount AS INTEGER

   nCount = 0

   Print "----- Start of " + DIBTestId$ + " -----"
   IF (LoadFiles% = FALSE) THEN
      print DIBTEST_ABORT
      HctLog DIBTEST_ABORT
   ELSE
      FOR szDataFile IN FILELIST SORTED BY NAME
         ShowDibFile szDataFile
         inc nCount

            ' check to see if we opened the file before testing it

         IF (DibError% (DIBTestId$) = FALSE) THEN
            print ">>> [" + SCRIPT + "]  Bitmap: " + trimStr$ (nCount) + "/" + trimStr$ (gnNumBitmaps) + "  [" + szDataFile + "]"
            '$IFDEF DIB_SCRL
                StretchScroll DIBTestId$, szDataFile
            '$ELSEIFDEF DIB_STRE
                StretchScroll DIBTestId$, szDataFile
            '$ELSEIFDEF DIB_FORM
                FileFormatTest szDataFile
            '$ELSE
               Pause DT_INVALID_PARM  'invalid parameter
               END     ' terminate testdrvr script
            '$ENDIF
            DoKeys   "^{F4}"             ' close MDI child
         ENDIF
         Print " "
         IF gszCancelFlag <> 0 THEN
            EXIT FOR
         ENDIF
      NEXT szDataFile
   ENDIF

   Print "----- End of " + DIBTestId$ + " Test -----"
END SUB

'-------------------------------------------------------------------------

SUB EndScript (szScriptName AS STRING)

   DIM lRet AS LONG, nRet AS INTEGER
   DIM nMsg AS STRING

   '$IFDEF CTSHELL AND NOT NO_PROMPT
      IF gszFailCount <> 0 THEN
         nMsg = WINTITLE + RET_FAIL_TO_CT + Chr$ (13)
         nMsg = nMsg +  TrimStr$ (gszFailCount) + TESTS_FAILED + TrimStr$ (gszPassCount) +  TESTS_PASSED
         PrintPause nMsg
         HctLog nMsg
         HctScriptEnd RS_FAILED
      ELSE
         nMsg = WINTITLE + RET_PASS_TO_CT
         nMsg = nMsg + Chr$ (13) + TrimStr$ (gszPassCount) + TESTS_PASSED_NO_FAIL
         PrintPause nMsg
         HctLog nMsg
         HctScriptEnd RS_PASSED
      ENDIF
   '$ENDIF

   PC_CloseApp
   PrintPause "End of " + szScriptName
END SUB

'-------------------------------------------------------------------------

SUB InitApp (szTestID AS STRING)

   DIM szRunLine AS STRING, szFullName AS STRING, hWnd AS INTEGER
   DIM wFlags AS INTEGER

   Print "In InitApp [" + szTestID + "] -----------------"
   ChDir TEST_PATH
   szFullName = WINTITLE + "2" + ".EXE"     'launch dibview2.exe
   IF EXISTS (szFullName) = 0 THEN
      pause szFullName + DOES_NOT_EXIST
      HctLog szFullName + DOES_NOT_EXIST
      Print SCRIPT_ABORT
      HctLog SCRIPT_ABORT
      END
   ELSE
      '
      ' the /s"section name" used by dibview as section name for HCT logs.
      '
      szRunLine = szFullName + " /s" + chr$ (34) + szTestID + chr$ (34)
      Print "   Running [" + szRunLine + "]"
      RUN szRunLine, NOWAIT
      SLEEP 5                         ' wait for window to be registered
      wFlags = FW_ALL or FW_NOCASE or FW_FOCUS or FW_PART or FW_MAXIMIZE
      hwnd = WFndWnd(WINTITLE, wFlags) ' select application to be tested
      if hwnd = 0 then
         pause CANNOT_FIND_WND
         PC_CloseApp
         end
      endif

      ghWndDibview = hwnd              ' save handle to DibView
      Randomize Timer                  ' get seed
   ENDIF
END SUB

'-------------------------------------------------------------------------


FUNCTION SetupBitmapSource (szPath AS STRING) AS STRING

   '
   ' Determine whether or not the path to the bitmaps is a UNC network
   ' path, or a directory on a fixed disk.
   '
   ' UNC Network things look like \\server\share,dir\subdir1\bitmapdir.
   ' Fixed disk paths look like c:\dir\subdir\bitmapdir.
   '
   ' Returns a path to the bitmaps without trailing backslash:
   '    y:\bitmaps     z:
   '

   DIM szTemp AS STRING, szNetShare AS STRING, nPos AS INTEGER

   if (instr (szPath, "\\") > 0) then
      ' See if there's subdir information associated with the net share
      nPos = instr (szPath, CMD_SEP)
      if nPos = 0 then
         szNetShare = szPath
      else
         szNetShare = mid$ (szPath, 1, nPos - 1)
      endif

      gszNetDrive = MakeNetDrive (szNetShare)
      if len (gszNetDrive) < 1 then
         pause "Could not establish net connection to " + szNetShare
         PC_CloseApp
         end
      endif

      szTemp = gszNetDrive
      ' find where the path to bitmaps start on server
      if (nPos > 1) then
         szTemp = szTemp + "\" + mid$ (szPath, nPos + 1)
      endif
   else
      szTemp = szPath
   endif

   SetupBitmapSource = szTemp
END FUNCTION

'-------------------------------------------------------------------------

FUNCTION LoadFiles%
   ' Fills the FileList structure used by DIBtest subroutine.
   ' Returns TRUE or FALSE indicating if an appropriate number of files
   '   are in the FileList structure.

   DebugOut "-=> Creating FileList <=-"

   DIM szBitmapPath AS STRING
   dim szMsdctVar  as string
   dim szDibPathVar as string

   'init szMsdctVar and szDibPathVar
   szMsdctVar  = string$(255,0)
   szDibPathVar = string$(255,0)

   '$IFDEF NO_PROMPT
      if (len (Command$) > 0) then
         'check to see if msdct env. variable exists
         if (GetEnvironmentVariable("MSDCT", szMsDctVar, 255) > 0) then
            DebugOut ">> Found MSDCT! <<"
            'get the value of dibpath
            if (GetEnvironmentVariable("DIBPATH", szDibPathVar, 255) > 0) then
               szBitmapPath = SetupBitmapSource(szDibPathVar)
            else
               'dibpath var. does not exist, exit script
               szBitmapPath = SetupBitmapSource (Command$)
               end RS_NOT_RUN
            endif
         else
            szBitmapPath = SetupBitmapSource (Command$)
         endif

         DebugOut ">> Bitmap Source = " + szBitmapPath + " <<"
         HctLog ">> Bitmap Source = " + szBitmapPath + " <<"

         FILELIST ADD szBitmapPath + "\*.DIB", , RECURSE
         FILELIST ADD szBitmapPath + "\*.BMP", , RECURSE
         FILELIST ADD szBitmapPath + "\*.RLE", , RECURSE

      else
   '$ENDIF
         FILELIST ADD DIBPATH + "\SAMCAT.DIB"
         'FILELIST ADD DIBPATH + "\OREGON2.RLE"
         'FILELIST ADD DIBPATH + "\LTHOUSE1.DIB"
         'FILELIST ADD DIBPATH + "\KRIS1.RLE"
   '$IFDEF NO_PROMPT
      endif
   '$ENDIF


   gnNumBitmaps = CountSetFile%

   '$IFDEF NO_PROMPT
      if (gnNumBitmaps > 0) then
   '$ELSE
      IF (gnNumBitmaps = 1) THEN
   '$ENDIF
      LoadFiles% = TRUE
   ELSE
      pause "YIKES!  Can't find all bitmaps at '" + DIBPATH + "'"
      LoadFiles% = FALSE
   ENDIF
END FUNCTION

'-------------------------------------------------------------------------

SUB ShowDibFile (szDibFile$)

   WMenu MNU_FILE                               'Select "File" Menu
   WMenu MITEM_OPEN                             'Select "Open" Menu Item
   DoKeys "%n"                                  'Select "File Name:" Field
   DoKeys szDibFile$                            ' insert filename
   DebugOut "Opened File : " + szDibFile
   HctLog "Opened File : " + szDibFile
   'WButtonClick OK_BUTTON                       ' show dib file
   'why press enter and not click OK?
   Dokeys "{enter}"
END SUB

'-------------------------------------------------------------------------

SUB StartScript (szScriptName AS STRING)

   DIM szTitle AS STRING

   SetDeadlockTimeout MAX_DEADLOCK_TIMEOUT  'set timeout for deadlock error

   gszPassCount  = 0                 ' initialize counter for passed tests
   gszFailCount  = 0                 ' initialize counter for failed tests
   gszCancelFlag = 0                 ' set flag to 0 (= Script not cancelled)
   '$IFDEF CTSHELL
      TM_SetEchoState                ' Set echo state from testmode$ Echo=
   '$ENDIF

   '$IFDEF NO_PROMPT
      szTitle = SCRIPT + " <Smoke>"
   '$ELSE
      szTitle = SCRIPT + " <Verify>"
   '$ENDIF

   '$IFDEF CTSHELL
      ghControls = UseControls (szTitle)
      HctScriptBegin WINTITLE
      gszHomeDir = STRING$(255,0)
      GetTempPath 255, gszHomeDir
   '$ENDIF

   gszNetDrive = ""                          ' initialize to no net drive

   PrintPause "Start of " + szScriptName
END SUB

'-------------------------------------------------------------------------

SUB StretchScroll (szTestId$, szFileName$)

   DIM MAX_DIB_TYPE AS INTEGER, lRet AS LONG
   DIM szStretchBox AS STRING, nResults AS INTEGER, nDisplayDIBType AS INTEGER
   STATIC DisplayFormat$ (3)
   DIM ghWndBmp AS INTEGER, ClientR AS RECT, BmpR AS RECT

   MAX_DIB_TYPE = 3              ' display formats + 1 (zero based arrays)

   DisplayFormat$ (0) = "D"  '  "Use &DDBs"
   DisplayFormat$ (1) = "e"  '  "Use DIBs (to d&evice)"
   DisplayFormat$ (2) = "m"  '  "Use DIBs (to &memory)"

   szStretchBox = "&Stretch To Window"          ' check box control label

   ghWndBmp = WFndWnd (szFileName$, FW_CHILDOK OR FW_PART OR FW_NOCASE)   ' Get handle for bitmap window
   if (ghWndBmp = 0) then
      pause "Can't find child window for bitmap [" + szFileName$ + "]"
      PC_CloseApp
      end
   endif

   GetClientRect ghWndDibview, ClientR          ' Get rectangle coords for dibview window
   GetClientRect ghWndBmp, BmpR                 ' Get rectangle coords for Bitmap window

   FOR nDisplayDIBType = 0 TO MAX_DIB_TYPE - 1
      DoKeys       "%o"     '   SelectMenuItem "Options"
      DoKeys       DisplayFormat$ (nDisplayDIBType)
      'WOptionClick DisplayFormat$ (nDisplayDIBType)
      SELECT CASE szTestId$
         CASE STRETCH
            if WCheckState (szStretchBox) = UNCHECKED then
               WCheckCheck szStretchBox
            endif
         CASE SCROLL
            IF WCheckState (szStretchBox) = CHECKED THEN
               WCheckUnCheck szStretchBox
            ENDIF
         CASE ELSE
            ' check for invalid parameter
            Print "OOPS! SUB stretchScroll: Invalid parameter"
            END     ' terminate testdrvr script
      END SELECT

'? why press enter and not click ok?
      DoKeys "{enter}"
      ' WButtonClick    OK_BUTTON
      '$IFDEF DIB_STRE
         StretchDIB ghWndBmp, ClientR.wright, ClientR.wbottom
      '$ELSEIFDEF DIB_SCRL
         ScrollDIB nDisplayDIBType, ghWndBmp, BmpR.wright, BmpR.wbottom, ClientR.wright, ClientR.wbottom
      '$ENDIF

      IF (DibError% (DisplayFormat$ (nDisplayDIBType)) = FALSE) THEN
         DoKeys   "%-x"                            ' maximize child window
         DoKeys   "%-r"                            ' restore window for next stretch session
      ENDIF
      IF gszCancelFlag <> 0 THEN
         EXIT FOR
      ENDIF
   NEXT nDisplayDIBType
END SUB

'----------------------------------------------------------------------------
'$ENDIF _DIB_INC

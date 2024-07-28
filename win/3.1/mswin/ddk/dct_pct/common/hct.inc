'$IFNDEF CTINC_DONE
'skip over this whole file if it has already been included
'$DEFINE CTINC_DONE

GLOBAL SZ_EOL$ 
SZ_EOL= CHR$(13) + CHR$(10)

'$IFNDEF WTDUTIL
'$include: 'winplay.inc'
Declare Function ExitWindows Lib "User" (dwReserved&,wReturnCode) As Integer
'$ENDIF

Declare Function GetWindowsDirectory Lib "Kernel" (lpBuffer AS ANY,  nSize As Integer) As Integer

DECLARE SUB HctScriptBegin(szName$)
DECLARE SUB HctScriptEnd  (iresult%)
DECLARE SUB HctRawLog    	LIB "HCTLIB.DLL" (psz$)
DECLARE SUB _HctLog  (psz$)
DECLARE SUB HctLog (psz$)

SUB _HctLog (psz$)
'$IFNDEF WTDUTIL
   x=MyBox( EL_WARN, "Hey! Don't call function _HctLog anymore, use HctLog")
'$ENDIF
   HctLog(psz)
END SUB

SUB HctLog (psz$)
   HctRawLog(psz + SZ_EOL)
END SUB


'$IFDEF USE_INSTRUCT
'$include: 'instruct.inc'

'$ENDIF         'IFDEF USE_INSTRUCT



DECLARE FUNCTION ChooseResult  LIB "HCTLIB.DLL" (testname$, i% ) AS INTEGER
DECLARE FUNCTION CreateProblemReport  LIB "HCTLIB.DLL" (hwndOwner AS INTEGER) AS INTEGER
DECLARE FUNCTION CreateProblemReportEx LIB "HCTLIB.DLL" (hwndOwner AS INTEGER, szTestCase AS STRING, szProbDesc AS STRING) AS INTEGER

DECLARE FUNCTION HctTestResult LIB "HCTLIB.DLL" (szTestName$) AS INTEGER

DECLARE SUB HctTestBegin   	LIB "HCTLIB.DLL" (n% , szTestName$)
DECLARE SUB HctTestDone       LIB "HCTLIB.DLL" (n%)
DECLARE FUNCTION HctChangeLogIfNoShell LIB "HCTLIB.DLL"  (szTestName$, szReserved$, dwOptions&) AS INTEGER
'Execute function in hct dir
Declare Function HctRun    	Lib "HCTLIB.DLL" (lpCmdLine As String, nCmdShow As Integer) As Integer

DECLARE FUNCTION HctGetHomeDir  LIB "HCTLIB.DLL" () AS LONG
DECLARE FUNCTION HctGetExecDir  LIB "HCTLIB.DLL" (lpBuf AS ANY, cBytes%) AS INTEGER

DECLARE FUNCTION StringEmpty  LIB "HCTLIB.DLL" (psz$) AS INTEGER
DECLARE SUB HCTErrorBox    	LIB "HCTLIB.DLL" (n%,psz$)
DECLARE FUNCTION CpuType   	LIB "HCTLIB.DLL" () AS INTEGER

'************* Global variables
GLOBAL ctDir AS STRING
GLOBAL winDir AS STRING


'******** Initialization Code

    Dim FixStr As String * 144
    k% = HctGetExecDir(VARPTR(FixStr),Len(FixStr))
    Print "fix string is "; FixStr
    ctDir = RemoveNulls(FixStr)
    Print "ct directory is "; ctDir
    k% = GetWindowsDirectory(VARPTR(FixStr),Len(FixStr))
    Print "fix string is "; FixStr
    winDir = RemoveNulls(FixStr)
    Print "win directory is "; winDir
    winDir = winDir + "\"



'$IFDEF USE_INSTRUCT
   __hwInstruct = CreateInstruct(0,  ctDir + "instruct.txt")
'$ENDIF         'IFDEF USE_INSTRUCT


'******** End Initialization Code










CONST RS_NOT_RUN=0
CONST RS_RUNNING=1
CONST RS_FAILED=2
CONST RS_PASSED=3
CONST RS_CANT_RUN=4
CONST RS_UNKNOWN=5


SUB HctScriptEnd  (iresult%)
    '$IFDEF USE_INSTRUCT
        IF __hwInstruct <> 0 THEN
            DestroyInstruct(__hwInstruct)
        ENDIF        
    '$ENDIF

     HctTestDone(iresult%)
END SUB


SUB HctScriptBegin(szName$)


    
    HctTestBegin  0 , szName$
     
END SUB


'$ENDIF         'IFNDEF CTINC_DONE
               

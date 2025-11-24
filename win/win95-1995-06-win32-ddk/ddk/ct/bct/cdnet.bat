REM CD Network Test Batch File
REM  %1 = Drive letter for net connection to CD Share
REM  %2 = Directory to copy compare
REM  %3 = Local drive letter to copy to
REM  EXAMPLE: CDNET N DIR C
REM           Create \DIR on C:, XCOPY the contents from N:\DIR to
REM               C:\DIR the run TCOMP to compare N:\DIR to C:\DIR.
REM
REM
MD %3:\%2
XCOPY %1:\%2 %3:\%2 /s /e /v
TCOMP %1:\%2 %3:\%2 > CDNET.LOG

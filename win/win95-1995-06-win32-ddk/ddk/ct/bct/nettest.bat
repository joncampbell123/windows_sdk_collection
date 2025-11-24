REM Network R/W Test Batch File
REM  %1 = Drive letter for R/W drive share
REM  %2 = Directory to copy compare
REM  %3 = Local drive letter to copy to
REM  %4 = Directory name to create on R/W drive share 
REM  EXAMPLE: NETTEST D DIR C NEWDIR
REM           Create \DIR on C:, XCOPY the contents from D:\DIR to
REM               C:\DIR the run TCOMP to compare D:\DIR to C:\DIR.
REM			Then we XCOPY the local copy back to the net drive
REM			\NEWDIR and compare that with C:\DIR.
MD %3:\%2
XCOPY %1:\%2 %3:\%2 /s /e /v
TCOMP %1:\%2 %3:\%2 > NETTEST.LOG
MD %1:\%4
XCOPY %3:\%2 %1:\%4 /s /e /v
TCOMP %3:\%2 %1:\%4 >> NETTEST.LOG



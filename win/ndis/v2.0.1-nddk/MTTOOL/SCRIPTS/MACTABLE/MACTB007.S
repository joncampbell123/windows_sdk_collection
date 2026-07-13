;*******************************************************************************
;*   FILE: mactb007.s - test open/close table bits
;*
;*   Description: check if supported
;*		  if not supported check for open bit set
;*                open adapter
;*                see if open bit in ss table is set
;*                close adapter
;*                see if open bit in ss table is clear
;*
;*   Date: 6-04-90
;*
;*   Pre-conditions: adapter is closed
;*
;*   Post-conditions: adapter is closed
;*
;*******************************************************************************

*** check if Open/CloseAdatper is supported
  -CheckMacTable SC 41 m 08
  -JFAIL NotSupported

  -LOG 3 open/close adapter is supported

;open adapter
  -OpenAdapter,SUCCESS
  -JPASS PASS

  -LOG 3 open adapter didn't return success
  -LOG 3 SQLKEY:MACTB007.1 FAIL
  -JUMP Abort

:PASS
  -LOG 3 open adapter returned success
  -LOG 3 SQLKEY:MACTB007.1 PASS

;check open bit in ss table
  -CheckMacTable ss 06 m 10
  -JPASS PASS

  -LOG 3 open bit in service specific status table not set
  -LOG 3 SQLKEY:MACTB007.2 FAIL
  -JUMP End

:PASS
  -LOG 3 open bit in service specific status table set
  -LOG 3 SQLKEY:MACTB007.2 PASS

;close the adapter
  -CloseAdapter,SUCCESS
  -JPASS PASS

  -LOG 3 close adapter didn't return success
  -LOG 3 SQLKEY:MACTB007.3 FAIL
  -JUMP Abort

:PASS
  -LOG 3 close adapter returned success

;check the open bit in ss table
;if it is set, that is a failure
  -CheckMacTable ss 06 e 0f
  -JPASS PASS

  -LOG 3 open bit is set, it should be clear
  -LOG 3 SQLKEY:MACTB007.4 FAIL
  -JUMP Abort

:PASS
  -LOG 3 open bit is clear
  -LOG 3 SQLKEY:MACTB007.4 PASS
  -JUMP End

:Abort
  -LOG 3 Aborting MACTB007 because state of MAC is uncertain
  -SCRIPT Abort Abort
  -SetFailure
  -JUMP End2

:NotSupported
  -LOG 3 open/close adapter is not supported
  -LOG 3 check that open bit is set.
  -CheckMacTable SS 06 e 1f
  -JFAIL Fail
  -LOG 3 Open bit is set.
  -LOG 3 SQLKEY:MACTB007.5 PASS
  -JUMP End

:Fail
  -LOG 3 Open bit was not set.
  -LOG 3 SQLKEY:MACTB007.5 FAIL

:End
  -CloseAdapter
  -Wait 2

:End2


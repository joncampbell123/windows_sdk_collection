;*******************************************************************************
;*   FILE: mactb006.s - test software settable station address
;*
;*   Description: check if supported
;*		  set address
;*                check if it was changed
;*
;*   Date: 5-30-90
;*
;*   Pre-conditions: adapter is closed
;*                   station address is set to permanent address
;*
;*   Post-conditions: adapter is closed
;*		      station address is set to 44 55 44 55 44 55
;*
;*******************************************************************************

*** Check the Service Specific Characteristics Table to see if Stnadr is
*** supported for this MAC.
  -CheckMacTable SC 40 m 10
  -JFAIL NotSupported

  -LOG 3 software settable station address is supported
*** set station address
  -SetStationAddress 44 55 44 55 44 55,SUCCESS
  -JPASS PASS
  -LOG 3 Set Station Address didn't return success
  -LOG 3 SQLKEY:MACTB006.1 FAIL
  -JUMP End

:PASS
  -LOG 3 Set Station Address returned success
  -LOG 3 SQLKEY:MACTB006.1 PASS

*** Check the Service Specific Characteristics Table to see if OpenAdapter
*** is supported by this MAC.
  -CheckMacTable SC 41 m 08
  -JFAIL Stnadr

  -OpenAdapter,SUCCESS
  -JFAIL OpenFail
  -LOG 3 OpenAdapter was successful.

:Stnadr
;see if the address was changed in the table
  -CheckMacTable SC 24 e 44 55 44 55 44 55
  -JPASS PASS
  -LOG 3 Station address was not changed correctly
  -LOG 3 SQLKEY:MACTB006.2 FAIL
  -JUMP End

:PASS
  -LOG 3 Station address was changed correctly
  -LOG 3 SQLKEY:MACTB006.2 PASS
  -JUMP End

:OpenFail
  -LOG 3 OpenAdapter was not successful.

:Abort
  -LOG 3 Aborting MACTB006 because state of MAC is uncertain
  -SCRIPT Abort Abort
  -SetFailure
  -JUMP End2

:NotSupported
  -LOG 3 software settable station address is not supported
  -LOG 3 SQLKEY:MACTB006.3 PASS

:End
  -CloseAdapter
  -Wait 2

:End2


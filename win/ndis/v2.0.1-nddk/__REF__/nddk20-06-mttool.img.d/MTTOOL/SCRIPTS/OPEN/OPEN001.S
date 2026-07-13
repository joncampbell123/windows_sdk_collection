;*****************************************************************************
;*
;*   File: OPEN001.S - Script file to test the OpenAdapter primitive.
;*
;*   Description: If OpenAdapter is supported verify its operation under
;*                normal conditions.
;*                Tests include:
;*                   initial open succeeds
;*                   second open returns INVALID_FUNCTION
;*                   close/open
;*                   close/open/close/open
;*
;*   Date: 6/11/90
;*
;*   Pre-conditions: Adapter is closed.
;*
;*   Post-conditions: Adapter is closed.
;*
;*****************************************************************************

  -LOG 3 Starting OpenAdapter Tests


*** Check the Service Specific Characteristics Table to see if the
*** OpenAdapter command is supported by this MAC.
  -CheckMacTable SC 41 m 08
  -JFAIL NotSupported

  -OpenAdapter,SUCCESS
  -JPASS FirstOpenPass

   Quit script because state of MAC is uncertain.
  -LOG 3 First OpenAdapter failed.  Exiting test.
  -LOG 3 SQLKEY:OPEN001.1 FAIL
  -JUMP abort

:FirstOpenPass
  -LOG 3 OpenAdapter returned SUCCESS.
  -LOG 3 SQLKEY:OPEN001.1 PASS

*** Second OpenAdapter command should return INVALID_FUNCTION.
  -LOG 3 Testing that OpenAdapter while open returns INVALID_FUNCTION.
  -OpenAdapter,INVALID_FUNCTION
  -JFAIL SecondOpenFailed

  -LOG 3 Second OpenAdapter returned INVALID_FUNCTION as expected.
  -LOG 3 SQLKEY:OPEN001.2 PASS
  -JUMP CloseOpenTest

:SecondOpenFailed
  -LOG 3 Second OpenAdapter did not return INVALID_FUNCTION.
  -LOG 3 SQLKEY:OPEN001.2 FAIL

*** Close and then Open; expect SUCCESS.
:CloseOpenTest
  -LOG 3 Testing Close and then Open.
  -CloseAdapter,SUCCESS
  -JFAIL CloseOpenFail1

  -LOG 3 CloseAdapter returned SUCCESS.

   Adapter should be closed so OpenAdapter should succeed.
  -OpenAdapter,SUCCESS
  -JFAIL CloseOpenFail2

  -LOG 3 OpenAdapter after CloseAdapter returned SUCCESS as expected.
  -LOG 3 SQLKEY:OPEN001.3 PASS
  -JUMP CloseOpenTest2

   Quit script because state of MAC is uncertain.
:CloseOpenFail1
  -LOG 3 CloseAdapter after OpenAdapter failed.  Exiting test.
  -LOG 3 SQLKEY:OPEN001.3 FAIL
  -JUMP end

   Quit script because state of MAC is uncertain.
:CloseOpenFail2
  -LOG 3 OpenAdapter after CloseAdapter failed.  Exiting test.
  -LOG 3 SQLKEY:OPEN001.3 FAIL
  -JUMP end

*** Close/open/close/open and expect SUCCESS
:CloseOpenTest2
  -LOG 3 Testing close/open/close/open.
  -CloseAdapter,SUCCESS
  -JFAIL CloseOpen2Fail1

  -LOG 3 First CloseAdapter returned SUCCESS as expected.

  -OpenAdapter,SUCCESS
  -JFAIL CloseOpen2Fail2

  -LOG 3 First OpenAdapter returned SUCCESS as expected.

  -CloseAdapter,SUCCESS
  -JFAIL CloseOpen2Fail3

  -LOG 3 Second CloseAdapter returned SUCCESS as expected.

  -OpenAdapter,SUCCESS
  -JFAIL CloseOpen2Fail4

  -LOG 3 Second OpenAdapter returned SUCCESS as expected.
  -LOG 3 SQLKEY:OPEN001.4 PASS
  -JUMP end

   Quit script because state of MAC is uncertain.
:CloseOpen2Fail1
  -LOG 3 First CloseAdapter in cl/op/cl/op failed.  Exiting script.
  -LOG 3 SQLKEY:OPEN001.4 FAIL
  -JUMP end

   Quit script because state of MAC is uncertain.
:CloseOpen2Fail2
  -LOG 3 First OpenAdapter in cl/op/cl/op failed.  Exiting script.
  -LOG 3 SQLKEY:OPEN001.4 FAIL
  -JUMP end

   Quit script because state of MAC is uncertain.
:CloseOpen2Fail3
  -LOG 3 Second CloseAdapter in cl/op/cl/op failed.  Exiting script.
  -LOG 3 SQLKEY:OPEN001.4 FAIL
  -JUMP end

   Quit script because state of MAC is uncertain.
:CloseOpen2Fail4
  -LOG 3 Second OpenAdapter in cl/op/cl/op failed.  Exiting script.
  -LOG 3 SQLKEY:OPEN001.4 FAIL
  -JUMP end

*** NOT_SUPPORTED error handler.
:NotSupported
   Specifically test for NOT_SUPPORTED before reporting that it is not
   supported
  -OpenAdapter,NOT_SUPPORTED
  -JFAIL NotSupportedFail

  -LOG 3 OpenAdapter is NOT_SUPPORTED for this MAC.
  -LOG 3 SQLKEY:OPEN001.5 PASS
  -JUMP end

:NotSupportedFail
  -LOG 3 ERROR: Service Specific Characteristics Table indicates that
  -LOG 3        OpenAdapter is not supported but OpenAdapter did not
  -LOG 3	return NOT_SUPPORTED.
  -LOG 3 SQLKEY:OPEN001.5 FAIL
  -JUMP end

:abort
  -LOG 3 Aborting open001 because state of MAC is uncertain.
  -Script ABORT ABORT
  -SetFailure
  -JUMP End2

:end
  -LOG 3 Finished OpenAdapter Tests
  -SetPacketFilter 0,SUCCESS
  -CloseAdapter
  -WAIT 2

:End2



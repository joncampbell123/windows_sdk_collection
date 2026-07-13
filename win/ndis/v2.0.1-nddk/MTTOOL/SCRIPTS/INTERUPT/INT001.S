*****************************************************************************
*
*   File: INT001.S - Script file to test the Interrupt primitive.
*
*   Description: If Interrupt is supported verify its operation under
*                normal conditions.
*                Tests include:
*                   Interrupt returns SUCCESS
*                   Interrupt indication occurred
*
*   Date: 3/2/90
*
*   Pre-conditions: Adapter is closed.
*
*   Post-conditions: Adapter is closed.
*
*****************************************************************************

  -LOG 3 Starting Interrupt Tests

*** Check the Service Specific Characteristics Table to see if the
*** OpenAdapter command is supported by this MAC.
  -LOG 3 Check if OpenAdapter is supported.
  -CheckMacTable SC 41 m 08
  -JFAIL Interrupt

  -OpenAdapter,SUCCESS
  -JPASS Interrupt

   Quit script because state of MAC is uncertain.
  -LOG 3 OpenAdapter failed.  Exiting test.
  -JUMP abort

:Interrupt
  Check if Interrupt is supported.
  -CheckMacTable SC 41 m 10
  -JFAIL NotSupported

  -CheckIntNum X
  -Interrupt,SUCCESS
  -JFAIL IntFail

  -LOG 3 Interrupt returned SUCCESS.
  -LOG 3 SQLKEY:INT001.1 PASS

  Check that there is an indication. 
  -CheckIntNum X
  -SetStatInd 4
  -Interrupt
  -WAIT 5
  -ChkStatInd
  -JFAIL IntFail2

  -LOG 3 Interrupt indication occurred.
  -LOG 3 SQLKEY:INT001.2 PASS
  -JUMP end

:IntFail
  -LOG 3 Interrupt did not return SUCCESS.
  -LOG 3 SQLKEY:INT001.1 FAIL
  -JUMP end

:IntFail2
  -LOG 3 Interrupt indication did not occur.
  -LOG 3 SQLKEY:INT001.2 FAIL
  -JUMP end

*** NOT_SUPPORTED error handler.
:NotSupported
   Specifically test for NOT_SUPPORTED before reporting that it is not
   supported
  -Interrupt,NOT_SUPPORTED
  -JFAIL NotSupportedFail

  -LOG 3 Interrupt is NOT_SUPPORTED for this MAC.
  -LOG 3 SQLKEY:INT001.3 PASS
  -JUMP end

:NotSupportedFail
  -LOG 3 ERROR: Service Specific Characteristics Table indicates that
  -LOG 3        Interrupt is not supported but Interrupt did not
  -LOG 3        return NOT_SUPPORTED.
  -LOG 3 SQLKEY:INT001.3 FAIL
  -JUMP end

:abort
  -LOG 3 Aborting INT001 because state of MAC is uncertain.
  -SCRIPT ABORT ABORT
  -JUMP end2

:end
  -LOG 3 Finished Interrupt Tests

*** Check the Service Specific Characteristics Table to see if the
*** CloseAdapter command is supported by this MAC.
  -LOG 3 Check if CloseAdapter is supported.
  -CheckMacTable SC 41 m 08
  -JFAIL end2

  -CloseAdapter,SUCCESS
  -JPASS end2

   Quit script because state of MAC is uncertain.
  -LOG 3 CloseAdapter failed.  Exiting test.
  -LOG 3 Aborting INT001 because state of MAC is uncertain.
  -SetFailure
  -SCRIPT ABORT ABORT

:end2

;*****************************************************************************
;*
;*   File: UPDST001.S - Script file to test the UpdateStatistics (US)
;*                      primitive.
;*
;*   Description: Verify operation of UpdateStatistics under normal
;*                conditions.
;*                Tests include:
;*                   - If US IS NOT supported, check for NOT_SUPPORTED return
;*                   - If US IS supported:
;*                         - Check for SUCCESS return
;*                         - Verification of the updated statistics in the
;*                           MAC table are checked in the Mactable tests
;*                           (** this test is done in the MACTABLE tests **)
;*
;*
;*   Date: 6/5/90
;*
;*   Pre-conditions: Adapter is closed.
;*
;*   Post-conditions: Adapter is closed.
;*
;*****************************************************************************

  -LOG 3 Starting the UpdateStatistics Tests

  -LOG 3 If this MAC supports OpenAdapter exec. OA
*** Check the Service Specific Characteristics Table to see if the
*** OpenAdapter command is supported by this MAC.
  -CheckMacTable SC 41 m 08
  -JFAIL Begin_Test

  -OpenAdapter,SUCCESS
  -JPASS Begin_Test

   Quit script because state of MAC is uncertain.
  -LOG 3 OpenAdapter failed.  Exiting test.
  -JUMP abort

:Begin_Test
*** Execute UpdateStatistics, to check for NOT_SUPPORTED or SUCCESS
  -UpdateStatistics
  -JPASS Chk_SUCCESS

  -LOG 3 UpdateStatistics returned other than SUCCESS.  This primitive
  -LOG 3 may not be supported by the MAC - now check for NOT_SUPPORTED

  -WAIT 01
  -CheckIntNum X
  -UpdateStatistics,NOT_SUPPORTED
  -JPASS USNS_Pass
  Check for successful to make sure we originally did not set 
  REQUEST_QUEUED. 
  -UpdateStatistics,SUCCESS
  -JPASS Chk_SUCCESS
  -JUMP USNS_Fail

:USNS_Pass
  -LOG 3 UpdateStatistics returned NOT_SUPPORTED. 
  -LOG 3 SQLKEY:UPDST001.1 PASS
  -JUMP end
:USNS_Fail
  -LOG 3 UpdateStatistics did not return SUCCESS or NOT_SUPPORTED. 
  -LOG 3 SQLKEY:UPDST001.1 FAIL
  -JUMP end

:Chk_SUCCESS
  -WAIT 01
  -CheckIntNum X
  -UpdateStatistics,SUCCESS
  -JFAIL USS_Fail
  -LOG 3 UpdateStatistics returned SUCCESS. 
  -LOG 3 SQLKEY:UPDST001.2 PASS
  -JUMP end
:USS_Fail
  -LOG 3 UpdateStatistics did not return SUCCESS or NOT_SUPPORTED. 
  -LOG 3 SQLKEY:UPDST001.2 FAIL
  -JUMP end

:abort
  -LOG 3 Aborting UPDST001 because state of MAC is uncertain.
  -SCRIPT ABORT ABORT
  -SetFailure
  -JUMP end2

:end
  -LOG 3 Finished UpdateStatistics Tests
*** Check the Service Specific Characteristics Table to see if the
*** CloseAdapter command is supported by this MAC.
  -LOG 3 Check if CloseAdapter is supported.
  -CheckMacTable SC 41 m 08
  -JFAIL end2

  -CloseAdapter,SUCCESS
  -JPASS end2

   Quit script because state of MAC is uncertain.
  -LOG 3 CloseAdapter failed.  Exiting test.
  -LOG 3 Aborting UPDST001 because state of MAC is uncertain.
  -SCRIPT ABORT ABORT

:end2


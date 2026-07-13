;*****************************************************************************
;*
;*   File: INIDG001.S - Script file to test the InitiateDiagnostics (IDG)
;*                      primitive.
;*
;*   Description: Verify operation of InitiateDiagnostics under normal
;*                conditions.
;*                Tests include:
;*                   - If IDG IS NOT supported, check for NOT_SUPPORTED return
;*                   - If IDG IS supported:
;*                         - Check for SUCCESS return
;*                         - Check the following in the MAC's service-specific
;*                           status table:
;*                               - Before IDG is issued, the Date/time field
;*                                 should be 0xFFFFFFFF.  Check this
;*                               - After IDG is issued, the Date/time field
;*                                 should be updated.  For now, this will be
;*                                 checked to not be 0xFFFFFFFF
;*                               - Make sure the packet filter setting is
;*                                 retained after an IDG
;*
;*
;*
;*   Date: 6/1/90
;*
;*   Pre-conditions: Adapter is closed.
;*
;*   Post-conditions: Adapter is closed.
;*
;*****************************************************************************

  -LOG 3 Starting the InitiateDiagnostics tests

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
  -LOG 3 Starting InitiateDiagnostics Tests
*** Check the Service Specific Characteristics Table to see if
*** InitiateDiagnostics is supported by this MAC.
  -CheckMacTable SC 40 m 40
  -JPASS IDG_Supported

  -LOG 3 Check for NOT_SUPPORTED
  -InitiateDiagnostics,NOT_SUPPORTED
  -JFAIL NS_Fail
  -LOG 3 Initiate Diagnostics is not supported.
  -LOG 3 SQLKEY:INIDG001.1 PASS
  -JUMP end
:NS_Fail
  -LOG 3 SQLKEY:INIDG001.1 FAIL
  -LOG 3 WARNING: Service Specific Characteristics Table indicates that
  -LOG 3	  InitiateDiagnostics is not supported but
  -LOG 3	  InitiateDiagnostics did not return NOT_SUPPORTED.
  -JUMP end

:IDG_Supported
*** Check for 0xFFFFFFFF in the Date/time field of the MAC's srv. specific
*** status table
  -CheckMacTable SS 02 e FF FF FF FF
  -JFAIL NegField_Fail
  -LOG 3 Initiate Diagnostics started with time/date at 0xFFFFFFFF.
  -LOG 3 SQLKEY:INIDG001.2 PASS
  -JUMP Do_IDG2
:NegField_Fail
  -LOG 3 Date/time field not 0xFFFFFFFF in MAC SS table
  -LOG 3 SQLKEY:INIDG001.2 FAIL

:Do_IDG2
*** Need to set the packet filter to a known value to insure its setting
*** remains after the following IDG
  -SetPacketFilter 3,SUCCESS
*** No use continuing if we can't do this
  -JFAIL abort

*** Execute InitiateDiagnostics, expect SUCCESS
  -InitiateDiagnostics,SUCCESS
  -JFAIL IDGFail
  -LOG 3 Initiate Diagnostics returned SUCCESS.
  -LOG 3 SQLKEY:INIDG001.3 PASS
  -JUMP Do_IDG3
:IDGFail
  -LOG 3 InitiateDiagnostics returned other than SUCCESS.  Exit test
  -LOG 3 since state of the MAC is now uncertain
  -LOG 3 SQLKEY:INIDG001.3 FAIL
  -JUMP end

:Do_IDG3
*** Date/time field should now contain a valid date/time
*** This field should now NOT be 0xFFFFFFFF
  -CheckMacTable SS 02 e FF FF FF FF
  -JPASS NegField_Pass
  -LOG 3 Initiate Diagnostics time/date was updated after call.
  -LOG 3 SQLKEY:INIDG001.4 PASS
  -JUMP Do_IDG4
:NegField_Pass
  -LOG 3 Date/time field is still 0xFFFFFFFF in MAC SS status table
  -LOG 3 when it should contain a valid date/time
  -LOG 3 SQLKEY:INIDG001.4 FAIL

:Do_IDG4
*** Insure that the previous packet filter setting of 3 is retained after
*** an IDG
  -CheckMacTable SS 0A e 03 00
  -JFAIL FilterTst_Fail
  -LOG 3 Filter was retained after Initiate Diagnostics call.
  -LOG 3 SQLKEY:INIDG001.5 PASS
  -JUMP end
:FilterTst_Fail
  -LOG 3 The filter has changed after the InitiateDiagnostics call
  -LOG 3 SQLKEY:INIDG001.5 FAIL
  -JUMP end

:abort
  -LOG 3 Aborting INIDG001 because state of MAC is uncertain.
  -SCRIPT ABORT ABORT
  -SetFailure
  -JUMP end2

:end
  -LOG 3 Finished InitiateDiagnostics Tests
  -SetPacketFilter 0,SUCCESS

*** Check the Service Specific Characteristics Table to see if the
*** CloseAdapter command is supported by this MAC.
  -LOG 3 Check if CloseAdapter is supported.
  -CheckMacTable SC 41 m 08
  -JFAIL end2

  -CloseAdapter,SUCCESS
  -JPASS end2

   Quit script because state of MAC is uncertain.
  -LOG 3 CloseAdapter failed.  Exiting test.
  -LOG 3 Aborting INIDG001 because state of MAC is uncertain.
  -SCRIPT ABORT ABORT

:end2


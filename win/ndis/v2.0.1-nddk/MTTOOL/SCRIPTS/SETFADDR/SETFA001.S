;*****************************************************************************
;*
;*   File: SETFA001.S - Script file to test the SetFunctionalAddress (SFA)
;*                      primitive.
;*
;*   Description: Verify operation of SetFunctionalAddress under normal
;*                conditions.
;*                Tests include:
;*                   - If SFA IS NOT supported, check for NOT_SUPPORTED return
;*                   - If SFA IS supported:
;*                         - Check for SUCCESS return
;*                         - Check for the functional address in the MAC's
;*                           characteristics table
;*                         - Have slave transmit to newly added FA, then check
;*                           for a local receive indication
;*                         - Have slave transmit to a non-existent FA, then
;*                           insure no receive indication occurred
;*
;*
;*   Date: 6/4/90
;*
;*   Pre-conditions: Adapter is closed.
;*                   Slave computer is ready.
;*
;*   Post-conditions: Adapter is closed.
;*
;*****************************************************************************

  -LOG 3 Starting SetFunctionalAddress Tests

*** Check the Service Specific Characteristics Table to see if the
*** OpenAdapter command is supported by this MAC.
  -LOG 3 Check if OpenAdapter is supported.
  -CheckMacTable SC 41 m 08
  -JFAIL GetSlave

  -OpenAdapter,SUCCESS
  -JPASS GetSlave

   Quit script because state of MAC is uncertain.
  -LOG 3 OpenAdapter failed.  Exiting test.
  -JUMP abort

*** Identify slave computer.
:GetSlave
   Need to be able to see slave's response.
  -SetPacketFilter 1,SUCCESS
  -JFAIL SlaveFail

  -IDSLAVE,SUCCESS
  -JPASS SetFunctional

  -LOG 3 Unable to identify slave computer.  Exiting script.
  -JUMP abort

:SlaveFail
  -LOG 3 Unable to set packet filter to 1 or identify slave.  Exiting script.
  -JUMP abort

:SetFunctional
  -LOG 3 Starting SetFunctionalAddress Tests
  -LOG 3 Check the Service Specific Characteristics Table to see if
  -LOG 3 Functional/Group addresses are supported by this MAC.
  -CheckMacTable SC 40 m 04
  -JPASS DoSetFA

  -LOG 3 Check for NOT_SUPPORTED
  -SetFunctionalAddress 04 03 02 01,NOT_SUPPORTED
  -JFAIL NS_Fail
  -LOG 3 SetFunctionalAddress returned NOT_SUPPORTED as expected. 
  -LOG 3 SQLKEY:SETFA001.1 PASS
  -JUMP end
:NS_Fail
  -LOG 3 SQLKEY:SETFA001.1 FAIL
  -LOG 3 ERROR: Service Specific Characteristics Table indicates that
  -LOG 3        SetFunctionalAddress is not supported but SetFunctionalAddress
  -LOG 3        did not return NOT_SUPPORTED.
  -JUMP end

*** Execute SetFunctionalAddress, expect SUCCESS
:DoSetFA
  -SetFunctionalAddress 04 03 02 01,SUCCESS
  -JFAIL SetFAFail1
  -LOG 3 SetFunctionalAddress returned SUCCESS as expected. 
  -LOG 3 SQLKEY:SETFA001.2 PASS
  -JUMP SetFATst2
:SetFAFail1
  -LOG 3 SetFunctionalAddress of 04 03 02 01 failed - this could be
  -LOG 3 wierdness if the NDI purports to support SFA so abort
  -LOG 3 SQLKEY:SETFA001.2 FAIL
  -JUMP abort

:SetFATst2
*** Check for the functional address in the MAC's characteristics table
  -CheckMACTable SC 34 e 04 03 02 01
  -JFAIL FA_CheckFail
  -LOG 3 Functional Address set in MAC's tables as expected. 
  -LOG 3 SQLKEY:SETFA001.3 PASS
  -JUMP SetFATst3
:FA_CheckFail
  -LOG 3 Unable to verify functional address in the MAC's SC table
  -LOG 3 SQLKEY:SETFA001.3 FAIL

:SetFATst3
  -SetPacketFilter 3,SUCCESS
  -JFAIL RecFail1

  -CheckIntNum X

  Check for token ring
  -CheckMacTable SC 2 e 38 30 32 2e 35
  -JFAIL EtherFC

  Tell slave to send to token ring Functional Address.
  -MRECV 1 100 100 0 0 2 c0 00 00 00 02 00,SUCCESS
  -JFAIL RecFail2
  -JUMP SentFC

:EtherFC
  Tell slave to send to ethernet Functional Address.
  -MRECV 1 100 100 0 0 2 01 00 00 00 02 00,SUCCESS
  -JFAIL RecFail2

:SentFC
  -WAIT 01
  -CheckIntNum 1,SUCCESS
  -JFAIL RecFail3

  -LOG 3 Frame received to newly added functional address.
  -LOG 3 SQLKEY:SETFA001.4 PASS
  -JUMP SetFATst4

:RecFail1
  -LOG 3 Unable to set the packet filter.  Exiting test.
  -LOG 3 SQLKEY:SETFA001.4 FAIL
  -JUMP abort

:RecFail2
  -LOG 3 Send to Functional Address failed.  Exiting test.
  -LOG 3 SQLKEY:SETFA001.4 FAIL
  -JUMP abort

:RecFail3
  -LOG 3 No indication received for functional packet.  One expected.
  -LOG 3 SQLKEY:SETFA001.4 FAIL

:SetFATst4
  -LOG 3 Testing that slave's trans. to a FA other than ours does NOT
  -LOG 3 generate a receive indication
   Have slave send a direct packet to an addr. not matching ours.
  -CheckIntNum X

  Check for token ring
  -CheckMacTable SC 2 e 38 30 32 2e 35
  -JFAIL EtherFC

  Tell slave to send to token ring Functional Address.
  -MRECV 1 100 100 0 0 2 c0 00 10 FC 09 06,SUCCESS
  -JFAIL RecFail2
  -JUMP SentFC

:EtherFC
  Tell slave to send to ethernet Functional Address.
  -MRECV 1 100 100 0 0 2 01 00 10 FC 09 06,SUCCESS
  -JFAIL RecFail2

:SentFC
  -WAIT 01
  -CheckIntNum 0,SUCCESS
  -JFAIL RecFail3

  -LOG 3 No indication received for functional packet.  
  -LOG 3 SQLKEY:SETFA001.5 PASS
  -JUMP end

:RecFail2
  -LOG 3 Send to Functional Address failed.  Exiting test.
  -LOG 3 SQLKEY:SETFA001.5 FAIL
  -JUMP abort

:RecFail3
  -LOG 3 Frame received to newly added functional address.  None expected.
  -LOG 3 SQLKEY:SETFA001.5 FAIL
  -JUMP end

:abort
  -LOG 3 State of MAC unknown.  Aborting tests.
  -Script ABORT ABORT
  -SetFailure
  -JUMP end2

:end
  -LOG 3 Finished SetFunctionalAddress Tests
  -SetPacketFilter 0,SUCCESS
  -SetFunctionalAddress 00 00 00 00
  -WAIT 2

*** Check the Service Specific Characteristics Table to see if the
*** CloseAdapter command is supported by this MAC.
  -LOG 3 Check if CloseAdapter is supported.
  -CheckMacTable SC 41 m 08
  -JFAIL end2

  -CloseAdapter,SUCCESS
  -JPASS end2

   Quit script because state of MAC is uncertain.
  -LOG 3 CloseAdapter failed.  Exiting test.
  -LOG 3 Aborting SETFA001 because state of MAC is uncertain.
  -SCRIPT ABORT ABORT

:end2

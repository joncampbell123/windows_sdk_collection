;*****************************************************************************
;*
;*   File: STN001.S - Script file to test the SetStationAddress primitive.
;*
;*   Description: Verify operation of SetStationAddress under normal
;*                conditions.
;*                Tests include:
;*                   Change station address, expect SUCCESS.
;*                   Test that packets to new address are received.
;*                   Set to different address.
;*                   Set to same address.
;*
;*   Date: 6/11/90
;*
;*   Pre-conditions: Adapter is closed.
;*                   Slave computer is ready.
;*
;*   Post-conditions: Adapter is closed.
;*                    Station address cannot be restored.
;*
;*****************************************************************************

  -LOG 3 Starting SetStationAddress Tests

 Check if station address is software settable.
 -LOG 3 Check that SetStationAddress is supported.
 -CheckMacTable SC 40 m 10
 -JFAIL NotSupported

*** Change station address while adapter is closed.
*** Check Service-Specific Characteristics table to verify change.
  -LOG 3 SetStationAddress while adapter is closed.
  -SetStationAddress 46 05 04 03 02 01,SUCCESS
  -JFAIL Fail1

  -LOG 3 SetStationAddress worked while adapter was closed.

  -LOG 3 Checking table for new address.
  -CheckMacTable SC 24 e 46 05 04 03 02 01
  -JFAIL Fail2

  -LOG 3 Table reflects new address.
  -LOG 3 SQLKEY:STN001.1 PASS
  -JUMP Receive

:Fail1
  -LOG 3 SetStationAddress did not work while station was closed.
  -LOG 3 SQLKEY:STN001.1 FAIL
  -JUMP end

:Fail2
  -LOG 3 Table does not show new station address.  Exiting test.
  -LOG 3 SQLKEY:STN001.1 FAIL
  -JUMP end

*** Have slave send to the new address.
:Receive
  -LOG 3 Check if OpenAdapter is supported.
  -CheckMacTable SC 41 m 08
  -JFAIL GetSlave

  -OpenAdapter,SUCCESS
  -JPASS GetSlave

   Quit script because state of MAC is uncertain.
  -LOG 3 OpenAdapter failed.  Exiting test.
  -JUMP abort

  Identify slave computer.
:GetSlave
   Need to be able to see slave's response.
  -SetPacketFilter 1,SUCCESS
  -JFAIL SlaveFail

  -IDSLAVE,SUCCESS
  -JPASS DoIt

  -LOG 3 Unable to identify slave computer.  Exiting test.
  -JUMP abort

:SlaveFail
  -LOG 3 Unable to set packet filter to 1 to identify slave.  Exiting test.
  -JUMP abort

:DoIt
  -CheckIntNum X

  -MRECV 1 64 100 64 100 0,SUCCESS
  -JFAIL RecFail1

  -WAIT 01
  -CheckIntNum 1,SUCCESS
  -JFAIL RecFail2

  -LOG 3 One indication received from slave as expected.
  -LOG 3 SQLKEY:STN001.2 PASS
  -JUMP NewAddress

:RecFail1
  -LOG 3 Unable to tell slave to send a packet.  Exiting test.
  -LOG 3 SQLKEY:STN001.2 FAIL
  -JUMP abort

:RecFail2
  -LOG 3 One indication not received from slave.
  -LOG 3 SQLKEY:STN001.2 FAIL

*** Set station address to different address.
:NewAddress
  -LOG 3 Check if CloseAdapter is supported.
  -CheckMacTable SC 41 m 08
  -JFAIL New1

  -CloseAdapter,SUCCESS
  -JFAIL NewFail1

:New1
  -LOG 3 SetStationAddress to new address.
  -SetStationAddress 44 05 05 05 05 05,SUCCESS
  -JFAIL NewFail2

  -LOG 3 SetStationAddress returned SUCCESS.

  -LOG 3 Checking table for new address.
  -CheckMacTable SC 24 e 44 05 05 05 05 05
  -JFAIL NewFail3

  -LOG 3 Table reflects new address.
  -LOG 3 SQLKEY:STN001.3 PASS
  -JUMP SameAddress

:NewFail1
  -LOG 3 CloseAdapter is supported, but it failed.  Exiting test.
  -JUMP abort

:NewFail2
  -LOG 3 SetStationAddress did not return SUCCESS.  Exiting test.
  -LOG 3 SQLKEY:STN001.3 FAIL
  -JUMP abort

:NewFail3
  -LOG 3 Table does not show new station address.
  -LOG 3 SQLKEY:STN001.3 FAIL

*** SetStationAddress to current address, expect SUCCESS.
:SameAddress
  -LOG 3 SetStationAddress to same address.
  -SetStationAddress 44 05 05 05 05 05,SUCCESS
  -JFAIL SameFail

  -LOG 3 SetStationAddress returned SUCCESS.
  -LOG 3 SQLKEY:STN001.4 PASS
  -JUMP NegTest

:SameFail
  -LOG 3 SetStationAddress did not return SUCCESS.
  -LOG 3 SQLKEY:STN001.4 FAIL

*** Set Station Address with a Multicast, Broadcast and Functional
*** address and check for INVALID_PARAMETER.
:NegTest
  Check for Token Ring
  -CheckMacTable SC 2 e 38 30 32 2e 35
  -JFAIL EtherMC

  Set Station Address with multicast address (Token Ring)
  -LOG 3 Set Stnadr to multicast addr for Token Ring
  -SetStationAddress 80 00 01 02 03 04,INVALID_PARAMETER
  -JFAIL NegFail1

  -LOG 3 Set Stnadr returned an INVALID_PARAMETER
  -LOG 3 SQLKEY:STN001.5 PASS
  -JUMP NegTest2

:EtherMC
  Set Station Address with multicast address (Ethernet)
  -LOG 3 Set Stnadr to multicast addr for Ethernet
  -SetStationAddress 01 02 03 04 05 06,INVALID_PARAMETER
  -JFAIL NegFail1

  -LOG 3 Set Stnadr returned an INVALID_PARAMETER
  -LOG 3 SQLKEY:STN001.5 PASS
  -JUMP NegTest2

:NegFail1
  -LOG 3 Set Stnadr returned other than INVALID_PARAMETER
  -LOG 3 SQLKEY:STN001.5 FAIL

:NegTest2
  Set Station address to a broadcast address
  -LOG 3 Set Station to a broadcast address
  -SetStationAddress ff ff ff ff ff ff,INVALID_PARAMETER
  -JFAIL NegFail2

  -LOG 3 Set Stnadr returned an INVALID_PARAMETER
  -LOG 3 SQLKEY:STN001.6 PASS
  -JUMP NegTest3

:NegFail2
  -LOG 3 Set Stnadr returned other than INVALID_PARAMETER
  -LOG 3 SQLKEY:STN001.6 FAIL

:NegTest3
  ;check for Token Ring
  -CheckMacTable SC 2 e 38 30 32 2e 35
  -JFAIL EtherMC

  Set Station address to a functional address
  -LOG 3 Set Station to a functional address
  -SetStationAddress C0 00 00 00 ff 00,INVALID_PARAMETER
  -JFAIL NegFail3

  -LOG 3 Set Stnadr returned an INVALID_PARAMETER
  -LOG 3 SQLKEY:STN001.7 PASS
  -JUMP end

:NegFail3
  -LOG 3 Set Stnadr returned other than INVALID_PARAMETER
  -LOG 3 SQLKEY:STN001.7 FAIL
  -JUMP end

:EtherMC
  -LOG 3 SQLKEY:STN001.7 PASS
  -JUMP end

*** NOT_SUPPORTED error handler.
*** When SetStationAddress is not supported, the return should be
*** INVALID_FUNCTION.
:NotSupported
   Specifically test for NOT_SUPPORTED before reporting that it is not
   supported
  -SetStationAddress 46 05 04 03 02 01,NOT_SUPPORTED
  -JFAIL NotSupportedFail

  -LOG 3 SetStationAddress is not supported for this MAC.
  -LOG 3 SQLKEY:STN001.8 PASS
  -JUMP end

:NotSupportedFail
  -LOG 3 ERROR: Service Specific Characteristics Table indicates that
  -LOG 3        SetStationAddress is not supported but SetStationAddress
  -LOG 3	did not return NOT_SUPPORTED
  -LOG 3 SQLKEY:STN001.8 FAIL
  -JUMP end

:abort
  -LOG 3 Aborting STN001 because state of MAC is uncertain.
  -SetFailure
  -SCRIPT ABORT ABORT

:end
  -LOG 3 Finished SetStationAddress Tests




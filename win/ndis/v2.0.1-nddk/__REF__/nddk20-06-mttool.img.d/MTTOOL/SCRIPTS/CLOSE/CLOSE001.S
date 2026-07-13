;*****************************************************************************
;*
;*   File: CLOSE001.S - Script file to test the CloseAdapter primitive.
;*
;*   Description: If CloseAdapter is supported verify its operation under
;*                normal conditions.
;*                Tests include:
;*                   close after open succeeds
;*		     second close returns INVALID_FUNCTION
;*                   open/close/open/close
;*                   attempt send and receive while closed
;*                   verify that functional and multicast addresses are reset
;*		     verify that filter, and set station addr do not reset
;*
;*
;*   Date: 6-11-90
;*
;*   Pre-conditions: Adapter is closed.
;*
;*   Post-conditions: Adapter is closed.
;*
;*****************************************************************************

  -LOG 3 Starting CloseAdapter Tests


*** Check the Service Specific Characteristics Table to see if the
*** CloseAdapter command is supported by this MAC.
   Check that CloseAdapter is supported.
  -CheckMacTable SC 41 m 08
  -JFAIL NotSupported

  -OpenAdapter,SUCCESS
  -JPASS FirstClose

   Quit script because state of MAC is uncertain.
  -LOG 3 First OpenAdapter failed.  Exiting test.
  -JUMP abort

:FirstClose
  -CloseAdapter,SUCCESS
  -JFAIL FirstCloseFail

  -LOG 3 First CloseAdapter returned SUCCESS.
  -LOG 3 SQLKEY:CLOSE001.1 PASS
  -JUMP SecondClose

:FirstCloseFail
   Quit script because state of MAC is uncertain.
  -LOG 3 First CloseAdapter failed.  Exiting test.
  -LOG 3 SQLKEY:CLOSE001.1 FAIL
  -JUMP abort


*** Second CloseAdapter command should return INVALID_FUNCTION.
:SecondClose
  -LOG 3 Testing that CloseAdapter while closed returns INVALID_FUNCTION.
  -CloseAdapter,INVALID_FUNCTION
  -JFAIL SecondCloseFailed

  -LOG 3 Second Closedapter returned INVALID_FUNCTION as expected.
  -LOG 3 SQLKEY:CLOSE001.2 PASS
  -JUMP OpenCloseTest

:SecondCloseFailed
  -LOG 3 Second CloseAdapter did not return INVALID_FUNCTION.
  -LOG 3 SQLKEY:CLOSE001.2 FAIL

*** Open/close/open/close and expect SUCCESS
:OpenCloseTest
  -LOG 3 Testing open/close/open/close.
  -OpenAdapter,SUCCESS
  -JFAIL OpenCloseFail1

  -LOG 3 First OpenAdapter returned SUCCESS as expected.

  -CloseAdapter,SUCCESS
  -JFAIL OpenCloseFail2

  -LOG 3 First CloseAdapter returned SUCCESS as expected.

  -OpenAdapter,SUCCESS
  -JFAIL OpenCloseFail3

  -LOG 3 Second OpenAdapter returned SUCCESS as expected.

  -CloseAdapter,SUCCESS
  -JFAIL OpenCloseFail4

  -LOG 3 Second CloseAdapter returned SUCCESS as expected.
  -LOG 3 SQLKEY:CLOSE001.3 PASS
  -JUMP SendRecv

   Quit script because state of MAC is uncertain.
:OpenCloseFail1
  -LOG 3 First OpenAdapter in op/cl/op/cl failed.  Exiting script.
  -LOG 3 SQLKEY:CLOSE001.3 FAIL
  -JUMP end

   Quit script because state of MAC is uncertain.
:OpenCloseFail2
  -LOG 3 First CloseAdapter in op/cl/op/cl failed.  Exiting script.
  -LOG 3 SQLKEY:CLOSE001.3 FAIL
  -JUMP end

   Quit script because state of MAC is uncertain.
:OpenCloseFail3
  -LOG 3 Second OpenAdapter in op/cl/op/cl failed.  Exiting script.
  -LOG 3 SQLKEY:CLOSE001.3 FAIL
  -JUMP end

   Quit script because state of MAC is uncertain.
:OpenCloseFail4
  -LOG 3 Second CloseAdapter in op/cl/op/cl failed.  Exiting script.
  -LOG 3 SQLKEY:CLOSE001.3 FAIL
  -JUMP end

*** Attempt to send and receive while adapter is closed.
:SendRecv
  -LOG 3 Testing send and receive while adapter is closed.
  -TRAN,INVALID_FUNCTION
  -JFAIL SendWorked

  -LOG 3 TransmitChain did not work while adapter was closed, as expected.
  -LOG 3 SQLKEY:CLOSE001.4 PASS
  -JUMP TryRecv

:SendWorked
  -LOG 3 TransmitChain worked while adapter was closed.
  -LOG 3 SQLKEY:CLOSE001.4 FAIL

:TryRecv


*** Verify that functional and multicast addresses are reset after
*** CloseAdapter.
:Addresses
  -LOG 3 Testing that functional address is reset by CloseAdapter.
   Check that functional addresses are supported.
  -CheckMacTable SC 40 m 04
  -JFAIL FncAddrFail1

  -OpenAdapter,SUCCESS
  -JFAIL FncAddrFail2

  -SetFunctionalAddress 01 02 03 04,SUCCESS
  -JFAIL FncAddrFail3

  -LOG 3 SetFunctionalAddress returned SUCCESS.

  -CloseAdapter,SUCCESS
  -JFAIL FncAddrFail4

  -LOG 3 CloseAdapter returned SUCCESS as expected.


   Check that functional address was reset.
  -CheckMacTable SC 34 e 00 00 00 00
  -JFAIL FncAddrFail5

  -LOG 3 Functional address was cleared by CloseAdapter.
  -LOG 3 SQLKEY:CLOSE001.5 PASS
  -JUMP MultiCastAddress

:FncAddrFail1
  -LOG 3 Functional address is not supported.
  -LOG 3 SQLKEY:CLOSE001.5 PASS
  -JUMP MultiCastAddress

   Quit script because state of MAC is uncertain.
:FncAddrFail2
  -LOG 3 OpenAdapter in functional address test failed.  Exiting script.
  -LOG 3 SQLKEY:CLOSE001.5 FAIL
  -JUMP end

:FncAddrFail3
  -LOG 3 SetFunctionalAddress failed.
  -LOG 3 SQLKEY:CLOSE001.5 FAIL

  -CloseAdapter,SUCCESS
  -JPASS MultiCastAddress  ;Fall through if CloseAdapter fails

   Quit script because state of MAC is uncertain.
:FncAddrFail4
  -LOG 3 CloseAdapter in functional address test failed.  Exiting script.
  -LOG 3 SQLKEY:CLOSE001.5 FAIL
  -JUMP end

:FncAddrFail5
  -LOG 3 CloseAdapter did not reset functional address.
  -LOG 3 SQLKEY:CLOSE001.5 FAIL

*
:MultiCastAddress
  -LOG 3 Testing that multicast address is reset by CloseAdapter.
   Check that multicast addresses are supported.
  -CheckMacTable SC 40 m 06
  -JFAIL MultiCastFail1

  -OpenAdapter,SUCCESS
  -JFAIL MultiCastFail2

  Check for token ring
  -CheckMacTable SC 2 e 38 30 32 2e 35
  -JFAIL EtherMC

  Add token ring Multicast Address.
  -AddMulticastAddress 80 00 03 04 05 06,SUCCESS
  -JFAIL MultiCastFail3
  -JUMP AddMCPass

:EtherMC
  Add ethernet Multicast Address.
  -AddMulticastAddress 01 02 03 04 05 06,SUCCESS
  -JFAIL MultiCastFail3

:AddMCPass
  -LOG 3 AddMulticastAddress returned SUCCESS.

  -CloseAdapter,SUCCESS
  -JFAIL MultiCastFail4

  -LOG 3 CloseAdapter returned SUCCESS as expected.

  -OpenAdapter,SUCCESS
  -JFAIL MultiCastFail2

  Check Multicast Address List to ensure multicast address is deleted.
  -LOG 3 Check Multicast Address List
  -CheckMacTable MC 2 e 00 00
  -JFAIL MultiCastFail5

  Test that the Multicast Address was cleared from firmware.
  ***Identify slave computer

:GetSlave
  Need to be able to see slave's response.
  -SetPacketFilter 1,SUCCESS
  -JFAIL MultiCastFail6

  -IDSLAVE,SUCCESS
  -JPASS Receive

  -LOG 3 Unable to identify slave computer. Exiting Script.
  -JUMP abort

:Receive
***Send to a Multicast Address that should have been deleted through
   CloseAdapter.
  -LOG 3 Send to a Multicast Address
  -CheckIntNum X

  Check for Token Ring
  -CheckMacTable SC 2 e 38 30 32 2e 35
  -JFAIL EtherMC

  Tell Slave to send to token ring multicast address.
  -MRECV 1 100 100 0 0 2 c0 00 83 04 05 06,SUCCESS
  -JFAIL MultiCastFail7
  -JUMP SentMC

:EtherMC
  Tell Slave to send to ethernet Multicast Address.
  -MRECV 1 100 100 0 0 2 01 02 03 04 05 06,SUCCESS
  -JFAIL MultiCastFail7

:SentMC
  -WAIT 01
  -CheckIntNum 0,SUCCESS
  JFAIL MultiCastFail8

  Issue a delmc to show Multicast Address was cleared.

  Check for token ring
  -CheckMacTable SC 2 e 38 30 32 2e 35
  -JFAIL EtherMC

  Delete of multicast address should fail (token ring)
  -DelMultiCastAddress 80 00 03 04 05 06,INVALID_PARAMETER
  -JFAIL MultiCastFail9
  -JUMP MultiCastPass

:EtherMC
  Delete of multicast address should fail (ethernet)
  -DelMultiCastAddress 01 02 03 04 05 06,INVALID_PARAMETER
  -JFAIL MultiCastFail9

:MultiCastPass
  -LOG 3 MultiCastAddress was cleared by CloseAdapter.
  -LOG 3 SQLKEY:CLOSE001.6 PASS
  -JUMP Filter

:MultiCastFail1
  -LOG 3 MultiCastAddresses/Grp Functional are not supported.
  -LOG 3 SQLKEY:CLOSE001.6 PASS
  -JUMP Filter

   Quit script because state of MAC is uncertain.
:MultiCastFail2
  -LOG 3 OpenAdapter in MultiCastAddress test failed.  Exiting script.
  -LOG 3 SQLKEY:CLOSE001.6 FAIL
  -JUMP end

:MultiCastFail3
  -LOG 3 AddMultiCastAddress failed.
  -LOG 3 SQLKEY:CLOSE001.6 FAIL
  -JUMP Filter

   Quit script because state of MAC is uncertain.
:MultiCastFail4
  -LOG 3 CloseAdapter in multicast address test failed.  Exiting script.
  -LOG 3 SQLKEY:CLOSE001.6 FAIL
  -JUMP end

:MultiCastFail5
  -LOG 3 Multicast Addr List does not show number of Addresses as 0.
  -LOG 3 SQLKEY:CLOSE001.6 FAIL
  -JUMP Filter

:MultiCastFail6
  -LOG 3 Unable to set Packet Filter to 1 to identify slave. Exit script.
  -LOG 3 SQLKEY:CLOSE001.6 FAIL
  -JUMP abort

:MultiCastFail7
  -LOG 3 Send to Multicast Address failed. Exiting Test.
  -LOG 3 SQLKEY:CLOSE001.6 FAIL
  -JUMP abort

:MultiCastFail8
  -LOG 3 Indication was received for Multicast Packet. None Expected.
  -LOG 3 SQLKEY:Close001.6 FAIL
  -JUMP Filter

:MultiCastFail9
  -LOG 3 CloseAdapter did not reset MultiCastAddress.
  -LOG 3 SQLKEY:CLOSE001.6 FAIL

*** Verify that Packet Filter does not reset to zero when CloseAdapter is
    issued.
:Filter

  -LOG 3 Set Packet Filter to 3.
  -Filter 3,SUCCESS
  -JFAIL FilterFail1

  -LOG 3 Set Packet Filter returned a SUCCESS.

  -LOG 3 Issue CloseAdapter to ensure Packet Filter remains set to 3.
  -CloseAdapter,SUCCESS
  -JFAIL FilterFail2

  -LOG 3 CloseAdapter returned SUCCESS.

***Check the Service-Specific Status Table to see if the Packet Filter
   is still set to 3.
  -CheckMacTable ss 0A e 03 00
  -JFAIL FilterFail3

  -LOG 3 Filter retained.
  -LOG 3 SQLKEY:CLOSE001.7 PASS
  -JUMP Stnadr

:FilterFail1
  -LOG 3 Set Packet Filter had a return code other than SUCCESS.
  -LOG 3 SQLKEY:CLOSE001.7 FAIL
  -JUMP Stnadr

   Quit script because state of MAC is uncertain.
:FilterFail2
  -LOG 3 CloseAdapter failed. Exiting Script.
  -LOG 3 SQLKEY:CLOSE001.7 FAIL
  -JUMP end

:FilterFail3
  -LOG 3 Packet Filter had a Filter value other than 3.
  -LOG 3 SQLKEY:CLOSE001.7 FAIL
  -JUMP Stnadr


***Verify that Set Station Address is not reset when CloseAdapter is
   issued.

:Stnadr

***Check if station address is software settable.
  -LOG 3 Check that SetStationAddress is supported.
  -CheckMacTable sc 40 m 10
  -JFAIL StnFail1

***Issue station address while adapter is closed.
  -LOG 3 SetStationAddress while adapter is closed.
  -SetStationAddress 44 44 44 44 44 44,SUCCESS
  -JFAIL StnFail2

  -LOG 3 SetStationAddress worked while adapter was closed.

  -LOG 3 Checking table for new address.
  -CheckMacTable sc 24 e 44 44 44 44 44 44
  -JFAIL StnFail3

  -LOG 3 Table shows new address.

  -LOG 3 Issue an OpenAdapter to set the current address to new stnadr.
  -OpenAdapter,SUCCESS
  -JFAIL StnFail4

  -LOG 3 OpenAdapter returned a SUCCESS.

***Issue a CloseAdapter to ensure that the stnadr was not reset
  -CloseAdapter,SUCCESS
  -JFAIL StnFail5

  -LOG 3 CloseAdapter returned a SUCCESS.

  -LOG 3 Check table to ensure new address was retained.
  -CheckMacTable sc 24 e 44 44 44 44 44 44
  -JFAIL StnFail6

  -LOG 3 CloseAdapter did not reset the Station Address.
  -LOG 3 SQLKEY:CLOSE001.8 PASS
  -JUMP end

:StnFail1
  -LOG 3 Set Station Address was NOT_SUPPORTED.
  -LOG 3 SQLKEY:CLOSE001.8 PASS
  -JUMP end

:StnFail2
  -LOG 3 Unable to set Stnadr while adapter was closed.
  -LOG 3 SQLKEY:CLOSE001.8 FAIL
  -JUMP end

:StnFail3
  -LOG 3 New Station Address was not present in table.
  -LOG 3 SQLKEY:CLOSE001.8 FAIL
  -JUMP end

:StnFail4
  -LOG 3 OpenAdapter in SetStationAddress test failed. Exiting script.
  -LOG 3 SQLKEY:CLOSE001.8 FAIL
  -JUMP end

:StnFail5
  -LOG 3 CloseAdapter in SetStationAddress test failed. Exiting script.
  -LOG 3 SQLKEY:CLOSE001.8 FAIL
  -JUMP end

:StnFail6
  -LOG 3 CloseAdapter reset Stnadr. Not expected.
  -LOG 3 SQLKEY:CLOSE001.8 FAIL
  -JUMP end

*** NOT_SUPPORTED error handler.
:NotSupported
   Specifically test for NOT_SUPPORTED before reporting that it is not
   supported
  -CloseAdapter,NOT_SUPPORTED
  -JFAIL NotSupportedFail

  -LOG 3 CloseAdapter is NOT_SUPPORTED for this MAC.
  -LOG 3 SQLKEY:CLOSE001.9 PASS
  -JUMP end

:NotSupportedFail
  -LOG 3 ERROR: Service Specific Characteristics Table indicates that
  -LOG 3        CloseAdapter is not supported but CloseAdapter did not
  -LOG 3	return NOT_SUPPORTED.
  -LOG 3 SQLKEY:CLOSE001.9 FAIL
  -JUMP end

:abort
  -LOG 3 Aborting CLOSE001 because state of MAC is uncertain.
  -Script ABORT ABORT
  -SetFailure

:end
  -LOG 3 Finished CloseAdapter Tests
  -SetPacketFilter 0,SUCCESS






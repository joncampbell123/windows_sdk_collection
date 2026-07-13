;*****************************************************************************
;*
;*   File: SETPF002.S - Script file to test the SetPacketFilter primitive.
;*
;*   Description: Verify operation of SetPacketFilter under normal conditions.
;*                Tests include:
;*                   Set filter to 3 and attempt to stress MAC by sending
;*                   various sizes of:
;*                     broadcast packets
;*                     direct packets
;*                     multicast packets
;*                   Set filter to illegal filter values to verify 
;*                   INVALID_PARAMETER return code. 
;*
;*   Date: 6/5/90
;*
;*   Pre-conditions: Adapter is closed.
;*                   Slave computer is ready.
;*
;*   Post-conditions: Adapter is closed.
;*
;*****************************************************************************

  -LOG 3 Starting SetPacketFilter Tests

*** Check the Service Specific Characteristics Table to see if the
*** OpenAdapter command is supported by this MAC.
  -LOG 3 Check if OpenAdapter is supported
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
  -JPASS AddMulticast

  -LOG 3 Unable to identify slave computer.  Exiting script.
  -JUMP abort

:SlaveFail
  -LOG 3 Unable to set packet filter to 1 to identify slave.
  -LOG 3 Exiting script.
  -JUMP abort

*** Set up multicast address for testing purposes.
:AddMulticast
  -LOG 3 Check that multicast addresses are supported.

  Check multicast and group bits.
  -CheckMacTable SC 40 m 06
  -JFAIL Filter3

  Check for token ring.
  -CheckMacTable SC 2 e 38 30 32 2e 35
  -JFAIL AddEtherMC

  -AddMulticastAddress 80 00 03 04 05 06,SUCCESS
  -JPASS Filter3
  -JUMP AddMCFail

:AddEtherMC
  -AddMulticastAddress 01 02 03 04 05 06,SUCCESS
  -JPASS Filter3

:AddMCFail
  -LOG 3 Unable to add multicast address.

*** Set packet filter to 3.  Attempt to stress the MAC by having slave send
*** multiple direct, broadcast and multicast packets.
:Filter3
  -LOG 3 Testing stress with filter of 3.
  -SetPacketFilter 3,SUCCESS
  -JPASS Filter3OK

  -LOG 3 SetPacketFilter 3 did not return SUCCESS.
  -LOG 3 SQLKEY:SETPF002.1 FAIL
  -JUMP end

:Filter3OK
  -LOG 3 SetPacketFilter 3 returned SUCCESS as expected.

  -LOG 3 Testing 1000 direct packets of 64 bytes.
   Have slave send 1000 64 byte directed packets.
  -ClearStatistics
  -WAIT 01
  -CheckIntNum X
  -MRECV 1000 64 64 0 0 0,SUCCESS
  -JFAIL Direct64Fail1

  -LOG 3 Slave sending 1000 64 byte direct packets.

  -WAIT 05
  Update statistics, check number of frames received.
  -UpdateStatistics
  -WAIT 01
  -CheckIntNum X

  -LOG 3 Checking statistics table for correct number of packets.
  -CheckMacTable SS 14 e FF FF FF FF
  -JPASS Direct64Fail2
  -CheckMacTable SS 14 e E8 03 00 00
  -JFAIL Direct64Fail3

  -LOG 3 Received 1000 64 byte direct packets as expected.
  -LOG 3 SQLKEY:SETPF002.1 PASS
  -JUMP Direct1500

:Direct64Fail1
  -LOG 3 Unable to tell slave to send direct packets.  Exiting script.
  -LOG 3 SQLKEY:SETPF002.1 FAIL
  -JUMP abort

:Direct64Fail2
  -LOG 3 Statistics not supported.  Cannot verify number of packets.
  -LOG 3 SQLKEY:SETPF002.1 PASS
  -JUMP Direct1500

:Direct64Fail3
  -LOG 3 Did not receive 1000 64 byte direct packets.
  -LOG 3 SQLKEY:SETPF002.1 FAIL

:Direct1500
  -LOG 3 Testing 1000 direct packets of 1500 bytes.
   Have slave send 1000 1500 byte directed packets.
  -ClearStatistics
  -WAIT 01
  -CheckIntNum X
  -MRECV 1000 1500 1500 0 0 0,SUCCESS
  -JFAIL Direct1500Fail1

  -LOG 3 Slave sending 1000 1500 byte direct packets.

  -WAIT 05
  Update statistics, check number of frames received.
  -UpdateStatistics
  -WAIT 01
  -CheckIntNum X

  -LOG 3 Checking statistics table for correct number of packets.
  -CheckMacTable SS 14 e FF FF FF FF
  -JPASS Direct1500Fail2
  -CheckMacTable SS 14 e E8 03 00 00
  -JFAIL Direct1500Fail3

  -LOG 3 Received 1000 1500 byte direct packets as expected.
  -LOG 3 SQLKEY:SETPF002.2 PASS
  -JUMP Broadcast64

:Direct1500Fail1
  -LOG 3 Unable to tell slave to send direct packets.  Exiting script.
  -LOG 3 SQLKEY:SETPF002.2 FAIL
  -JUMP abort

:Direct1500Fail2
  -LOG 3 Statistics not supported.  Cannot verify number of packets.
  -LOG 3 SQLKEY:SETPF002.2 PASS
  -JUMP Broadcast64

:Direct1500Fail3
  -LOG 3 Did not receive 1000 1500 byte direct packets.
  -LOG 3 SQLKEY:SETPF002.2 FAIL

*
:Broadcast64
   First check that broadcast is supported.
  -CheckMacTable SC 40 m 01
  -JFAIL Broadcast64Fail1

  -LOG 3 Testing 1000 broadcast packets of 64 bytes.
   Have slave send 1000 64 byte broadcast packets.
  -ClearStatistics
  -WAIT 01
  -CheckIntNum X
  -MRECV 1000 64 64 0 0 1,SUCCESS
  -JFAIL Broadcast64Fail2

  -LOG 3 Slave sending 1000 64 byte broadcast packets.

  -WAIT 05
  Update statistics, check number of frames received.
  -UpdateStatistics
  -WAIT 01
  -CheckIntNum X

  -LOG 3 Checking statistics table for correct number of broadcast packets.
  -CheckMacTable SS 28 e FF FF FF FF
  -JPASS Broadcast64Fail3
  -CheckMacTable SS 28 e E8 03 00 00
  -JFAIL Broadcast64Fail4

  -LOG 3 Received 1000 64 byte broadcast packets as expected.
  -LOG 3 SQLKEY:SETPF002.3 PASS
  -JUMP Broadcast1500

:Broadcast64Fail1
  -LOG 3 Broadcast mode not supported.
  -JUMP Multicast64

:Broadcast64Fail2
  -LOG 3 Unable to tell slave to send broadcast packets.  Exiting script.
  -LOG 3 SQLKEY:SETPF002.3 FAIL
  -JUMP abort

:Broadcast64Fail3
  -LOG 3 Statistics not supported.  Cannot verify number of packets.
  -LOG 3 SQLKEY:SETPF002.3 PASS
  -JUMP Broadcast1500

:Broadcast64Fail4
  -LOG 3 Did not receive 1000 64 byte broadcast packets.
  -LOG 3 SQLKEY:SETPF002.3 FAIL

:Broadcast1500
  -LOG 3 Testing 1000 broadcast packets of 1500 bytes.
   Have slave send 1000 1500 byte broadcast packets.
  -ClearStatistics
  -WAIT 01
  -CheckIntNum X
  -MRECV 1000 1500 1500 0 0 1,SUCCESS
  -JFAIL Broadcast1500Fail1

  -LOG 3 Slave sending 1000 1500 byte broadcast packets.

  -WAIT 05
  Update statistics, check number of frames received.
  -UpdateStatistics
  -WAIT 01
  -CheckIntNum X

  -LOG 3 Checking statistics table for correct number of broadcast packets.
  -CheckMacTable SS 28 e FF FF FF FF
  -JPASS Broadcast1500Fail2
  -CheckMacTable SS 28 e E8 03 00 00
  -JFAIL Broadcast1500Fail3

  -LOG 3 Received 1000 1500 byte broadcast packets as expected.
  -LOG 3 SQLKEY:SETPF002.4 PASS
  -JUMP Multicast64


:Broadcast1500Fail1
  -LOG 3 Unable to tell slave to send broadcast packets.  Exiting script.
  -LOG 3 SQLKEY:SETPF002.4 FAIL
  -JUMP abort

:Broadcast1500Fail2
  -LOG 3 Statistics not supported.  Cannot verify number of packets.
  -LOG 3 SQLKEY:SETPF002.4 PASS
  -JUMP Multicast64

:Broadcast1500Fail3
  -LOG 3 Did not receive 1000 1500 byte broadcast packets.
  -LOG 3 SQLKEY:SETPF002.4 FAIL


:Multicast64
   Check that multicast addresses are supported.
  -CheckMacTable SC 40 m 06
  -JFAIL Multicast64Fail1

  -LOG 3 Testing 1000 multicast packets of 64 bytes.
   Have slave send 1000 64 byte multicast packets.
  -ClearStatistics
  -WAIT 01
  -CheckIntNum X

  Check for token ring.
  -CheckMacTable SC 2 e 38 30 32 2e 35
  -JFAIL EtherMC

  -MRECV 1000 64 64 0 0 2 C0 00 83 04 05 06,SUCCESS
  -JFAIL Multicast64Fail2
  -JUMP MCContinue

:EtherMC
  -MRECV 1000 64 64 0 0 2 01 02 03 04 05 06,SUCCESS
  -JFAIL Multicast64Fail2

:MCContinue
  -LOG 3 Slave sending 1000 64 byte multicast packets.

  -WAIT 05
  Update statistics, check number of frames received.
  -UpdateStatistics
  -WAIT 01
  -CheckIntNum X

  -LOG 3 Checking statistics table for correct number of multicast packets.
  -CheckMacTable SS 24 e FF FF FF FF
  -JPASS Multicast64Fail3
  -CheckMacTable SS 24 e E8 03 00 00
  -JFAIL Multicast64Fail4

  -LOG 3 Received 1000 64 byte multicast packets as expected.
  -LOG 3 SQLKEY:SETPF002.5 PASS
  -JUMP Multicast1500

:Multicast64Fail1
  -LOG 3 Multicast addresses not supported.
  -JUMP BadFilter

:Multicast64Fail2
  -LOG 3 Unable to tell slave to send multicast packets.  Exiting script.
  -LOG 3 SQLKEY:SETPF002.5 FAIL
  -JUMP abort

:Multicast64Fail3
  -LOG 3 Statistics not supported.  Cannot verify number of packets.
  -LOG 3 SQLKEY:SETPF002.5 PASS
  -JUMP Multicast1500

:Multicast64Fail4
  -LOG 3 Did not receive 1000 64 byte multicast packets.
  -LOG 3 SQLKEY:SETPF002.5 FAIL

:Multicast1500
  -LOG 3 Testing 1000 multicast packets of 1500 bytes.
   Have slave send 1000 1500 byte multicast packets.
  -ClearStatistics
  -WAIT 01
  -CheckIntNum X

  Check for token ring
  -CheckMacTable SC 2 e 38 30 32 2e 35
  -JFAIL EtherMC

  -MRECV 1000 1500 1500 0 0 2 C0 00 83 04 05 06,SUCCESS
  -JFAIL Multicast1500Fail1
  -JUMP MCContinue

:EtherMC
  -MRECV 1000 1500 1500 0 0 2 01 02 03 04 05 06,SUCCESS
  -JFAIL Multicast1500Fail1

:MCContinue
  -LOG 3 Slave sending 1000 1500 byte multicast packets.

  -WAIT 10
  Update statistics, check number of frames received.
  -UpdateStatistics
  -WAIT 01
  -CheckIntNum X

  -LOG 3 Checking statistics table for correct number of multicast packets.
  -CheckMacTable SS 24 e FF FF FF FF
  -JPASS Multicast1500Fail2
  -CheckMacTable SS 24 e E8 03 00 00
  -JFAIL Multicast1500Fail3

  -LOG 3 Received 1000 1500 byte multicast packets as expected.
  -LOG 3 SQLKEY:SETPF002.6 PASS
  -JUMP BadFilter

:Multicast1500Fail1
  -LOG 3 Unable to tell slave to send multicast packets.  Exiting script.
  -LOG 3 SQLKEY:SETPF002.6 FAIL
  -JUMP abort

:Multicast1500Fail2
  -LOG 3 Statistics not supported.  Cannot verify number of packets.
  -LOG 3 SQLKEY:SETPF002.6 PASS
  -JUMP BadFilter

:Multicast1500Fail3
  -LOG 3 Did not receive 1000 1500 byte multicast packets.
  -LOG 3 SQLKEY:SETPF002.6 FAIL

:BadFilter
*** Bad Filter Tests.  Test that setting bits 15 through 4 return 
*** INVALID_PARAMETER from the driver.
  -LOG 3 Test setting filter to 0x8000.
  -SetPacketFilter 8000,INVALID_PARAMETER
  -JFAIL FilterFail
  -LOG 3 Test setting filter to 0x4000.
  -SetPacketFilter 4000,INVALID_PARAMETER
  -JFAIL FilterFail
  -LOG 3 Test setting filter to 0x2000.
  -SetPacketFilter 2000,INVALID_PARAMETER
  -JFAIL FilterFail
  -LOG 3 Test setting filter to 0x1000.
  -SetPacketFilter 1000,INVALID_PARAMETER
  -JFAIL FilterFail
  -LOG 3 Test setting filter to 0x0800.
  -SetPacketFilter 0800,INVALID_PARAMETER
  -JFAIL FilterFail
  -LOG 3 Test setting filter to 0x0400.
  -SetPacketFilter 0400,INVALID_PARAMETER
  -JFAIL FilterFail
  -LOG 3 Test setting filter to 0x0200.
  -SetPacketFilter 0200,INVALID_PARAMETER
  -JFAIL FilterFail
  -LOG 3 Test setting filter to 0x0100.
  -SetPacketFilter 0010,INVALID_PARAMETER
  -JFAIL FilterFail
  -LOG 3 Test setting filter to 0x0080.
  -SetPacketFilter 0080,INVALID_PARAMETER
  -JFAIL FilterFail
  -LOG 3 Test setting filter to 0x0040.
  -SetPacketFilter 0040,INVALID_PARAMETER
  -JFAIL FilterFail
  -LOG 3 Test setting filter to 0x0020.
  -SetPacketFilter 0020,INVALID_PARAMETER
  -JFAIL FilterFail
  -LOG 3 Test setting filter to 0x0010.
  -SetPacketFilter 0010,INVALID_PARAMETER
  -JFAIL FilterFail
  Check id source routing bridge supported.
  -CheckMacTable SC 41 m 20
  -JPASS FilterOK
  -LOG 3 Test setting filter to 0x0008.
  -SetPacketFilter 0008,INVALID_PARAMETER
  -JPASS FilterOK

:FilterFail
  -LOG 3 SetPacketFilter did not return INVALID_PARAMETER as expected.
  -LOG 3 SQLKEY:SETPF002.7 FAIL
  -JUMP end

:FilterOK
  -LOG 3 SetPacketFilter returned INVALID_PARAMETER as expected.
  -LOG 3 SQLKEY:SETPF002.7 PASS
  -JUMP end

:abort
  -LOG 3 Aborting SETPF002 because state of MAC is uncertain.
  -SCRIPT ABORT ABORT
  -SetFailure
  -JUMP end2

:end
  -LOG 3 Remove Multicast address.
  Check if multicast supported.
  -CheckMacTable SC 40 m 02
  -JPASS DelMulticast
  Check if group supported.
  -CheckMacTable SC 40 m 04
  -JFAIL CleanupFilter

:DelMulticast
  Check for token ring
  -CheckMacTable SC 2 e 38 30 32 2e 35
  -JFAIL EtherMC

  -DelMulticastAddress 80 00 03 04 05 06,SUCCESS
  -JFAIL abort2
  -JUMP CleanupFilter

:EtherMC
  -DelMulticastAddress 01 02 03 04 05 06,SUCCESS

:CleanupFilter
  -SetPacketFilter 0,SUCCESS

  -LOG 3 Finished SetPacketFilter Tests

*** Check the Service Specific Characteristics Table to see if the
*** CloseAdapter command is supported by this MAC.
  -LOG 3 Check if CloseAdapter is supported.
  -CheckMacTable SC 41 m 08
  -JFAIL end2

  -CloseAdapter,SUCCESS
  -JPASS end2

   Quit script because state of MAC is uncertain.
  -LOG 3 CloseAdapter failed.  Exiting test.

:abort2
  -LOG 3 Aborting SETPF002 because state of MAC is uncertain.
  -SCRIPT ABORT ABORT

:end2


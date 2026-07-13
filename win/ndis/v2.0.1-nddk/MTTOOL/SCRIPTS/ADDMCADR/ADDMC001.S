;*****************************************************************************
;*
;*   File: ADDMC001.S - Script file to test the AddMulticastAddress primitive.
;*
;*   Description: Verify operation of AddMulticastAddress under normal
;*                conditions.
;*                Tests include:
;*                   Add address, expect SUCCESS.
;*                   Attempt to add a previously added MulticastAddress.
;*                   Attempt to add an invalid MulticastAddress.
;*                   Receive MulticastAddress tests
;*                       Send to added Multicast Address, should receive it.
;*                       Sends to invalid MC Address, no receive indications.
;*
;*   Date: 6/1/90
;*
;*   Pre-conditions: Adapter is closed.
;*                   Slave computer is ready.
;*
;*   Post-conditions: Adapter is closed.
;*
;*****************************************************************************

  -LOG 3 Starting AddMulticastAddress Tests

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
  -JPASS AddMulticast

  -LOG 3 Unable to identify slave computer.  Exiting script.
  -JUMP abort

:SlaveFail
  -LOG 3 Unable to set packet filter to 1 or identify slave.  Exiting script.
  -JUMP abort

*** Attempt to add a Multicast Address.
:AddMulticast
  -LOG 3 Check that Multicast Addresses are supported.

  Check multicast bit.
  -CheckMacTable SC 40 m 02
  -JPASS AddMulticast

  Check functional/group bit, if set, assume support for group 
  through the use of multicast commands.
  -CheckMacTable SC 40 m 04
  -JFAIL NotSupported
  
:AddMulticast
  Check both the multicast and the functional/group bits and make sure
  both are not set simultaneously.
  -CheckMacTable SC 40 m 02
  -JFAIL AddMulticast2
  -CheckMacTable SC 40 m 04
  -JFAIL AddMulticast2

  -LOG 3 Multicast and Functional/Group bits set.  This is illegal.
  -LOG 3 SQLKEY:ADDMC001.0 FAIL
  -JUMP AddMulticast3

:AddMulticast2
  -LOG 3 Only the Multicast or Functional/Group bit set.  This is correct.
  -LOG 3 SQLKEY:ADDMC001.0 PASS

:AddMulticast3
  -LOG 3 Adding Multicast Address.

  Check for token ring
  -CheckMacTable SC 2 e 38 30 32 2e 35
  -JFAIL EtherMC

  Add token ring Multicast Address.
  -AddMulticastAddress 80 00 03 04 05 06,SUCCESS
  -JFAIL AddMCFail
  -JUMP AddMCPass

:EtherMC
  Add ethernet Multicast Address.
  -AddMulticastAddress 01 02 03 04 05 06,SUCCESS
  -JFAIL AddMCFail

:AddMCPass
  -LOG 3 Added Multicast Address.
  -LOG 3 SQLKEY:ADDMC001.1 PASS
  -JUMP ReAdd

:AddMCFail
  -LOG 3 Unable to add Multicast Address.  Exiting test.
  -LOG 3 SQLKEY:ADDMC001.1 FAIL
  -JUMP abort

*** Attempt to added previously added Multicast Address.
:ReAdd
  Check if the driver can accept more than one multicast.  If not,
  skip this test.         
  -CheckMacTable MC 0 e 01 00
  -JPASS Receive

  -LOG 3 Attempting to add previously added Multicast Address.

  Check for token ring
  -CheckMacTable SC 2 e 38 30 32 2e 35
  -JFAIL EtherMC

  Re-add token ring Multicast Address.
  -AddMulticastAddress 80 00 03 04 05 06,INVALID_PARAMETER
  -JFAIL ReAddMCFail
  -JUMP ReAddMCPass

:EtherMC
  Re-add ethernet Multicast Address.
  -AddMulticastAddress 01 02 03 04 05 06,INVALID_PARAMETER
  -JFAIL ReAddMCFail

:ReAddMCPass
  -LOG 3 Re-add of Multicast Address returned INVALID_PARAMETER as expected.
  -LOG 3 SQLKEY:ADDMC001.2 PASS
  -JUMP Receive

:ReAddMCFail
  -LOG 3 Re-add of Multicast Address did not return INVALID_PARAMETER.
  -LOG 3 SQLKEY:ADDMC001.2 FAIL

*** Send to added Multicast Address and expect to receive it.
:Receive
  -LOG 3 Attempting to send to previously added Multicast Address.

  -SetPacketFilter 3,SUCCESS
  -JFAIL RecFail1

  -CheckIntNum X

  Check for token ring
  -CheckMacTable SC 2 e 38 30 32 2e 35
  -JFAIL EtherMC

  Tell slave to send to token ring Multicast Address.
  -MRECV 1 100 100 0 0 2 c0 00 83 04 05 06,SUCCESS
  -JFAIL RecFail2
  -JUMP SentMC

:EtherMC
  Tell slave to send to ethernet Multicast Address.
  -MRECV 1 100 100 0 0 2 01 02 03 04 05 06,SUCCESS
  -JFAIL RecFail2

:SentMC
  -WAIT 01
  -CheckIntNum 1,SUCCESS
  -JFAIL RecFail3

  -LOG 3 One indication received from multicast packet as expected.
  -LOG 3 SQLKEY:ADDMC001.3 PASS
  -JUMP RecDir

:RecFail1
  -LOG 3 Unable to set the packet filter.  Exiting test.
  -LOG 3 SQLKEY:ADDMC001.3 FAIL
  -JUMP abort

:RecFail2
  -LOG 3 Send to Multicast Address failed.  Exiting test.
  -LOG 3 SQLKEY:ADDMC001.3 FAIL
  -JUMP abort

:RecFail3
  -LOG 3 No indication received for multicast packet.  One expected.
  -LOG 3 SQLKEY:ADDMC001.3 FAIL

*** Send to Directed Address and don't expect to see it.
:RecDir
  -LOG 3 Attempting to send to Directed Address.
  -CheckIntNum X

  Tell slave to send to Directed Address.
  -MRECV 1 100 100 0 0 2 00 01 02 03 04 05,SUCCESS
  -JFAIL DirFail1

  -WAIT 01
  -CheckIntNum 0,SUCCESS
  -JFAIL DirFail2

  -LOG 3 No indications received from directed packet as expected.
  -LOG 3 SQLKEY:ADDMC001.4 PASS
  -JUMP RecUnsetMC

:DirFail1
  -LOG 3 Send to Directed Address failed.  Exiting test.
  -LOG 3 SQLKEY:ADDMC001.4 FAIL
  -JUMP abort

:DirFail2
  -LOG 3 Indications received for directed packet.  None expected.
  -LOG 3 SQLKEY:ADDMC001.4 FAIL
  -JUMP end


*** Send to unset Multicast Address and don't expect to see it.
:RecUnsetMC
  -LOG 3 Attempting to send to unset Multicast Address.
  -CheckIntNum X

  Check for token ring
  -CheckMacTable SC 2 e 38 30 32 2e 35
  -JFAIL EtherMC

  Tell slave to send to unset Multicast Address.
  -MRECV 1 100 100 0 0 2 c0 00 82 03 04 05,SUCCESS
  -JFAIL UnsetFail1
  -JUMP UnsetWait

:EtherMC
  Tell slave to send to unset Multicast Address.
  -MRECV 1 100 100 0 0 2 01 02 03 04 05 07,SUCCESS
  -JFAIL UnsetFail1

:UnsetWait
  -WAIT 01
  -CheckIntNum 0,SUCCESS
  -JFAIL UnsetFail2

  -LOG 3 No indications received from multicast packet as expected.
  -LOG 3 SQLKEY:ADDMC001.5 PASS
  -JUMP RecBrdcst

:UnsetFail1
  -LOG 3 Send to Multicast Address failed.  Exiting test.
  -LOG 3 SQLKEY:ADDMC001.5 FAIL
  -JUMP abort

:UnsetFail2
  -LOG 3 Indications received for multicast packet.  None expected.
  -LOG 3 SQLKEY:ADDMC001.5 FAIL
  -JUMP end


*** Send to Broadcast Address and don't expect to see it.
:RecBrdcst
  -LOG 3 Attempting to send to Broadcast Address.
  -CheckIntNum X

  Don't allow broadcasts to be received. 
  -SetPacketFilter 1,SUCCESS
  -JFAIL BcsFail3

  Tell slave to send to Broadcast Address.
  -MRECV 1 100 100 0 0 2 ff ff ff ff ff ff,SUCCESS
  -JFAIL BcsFail1

  -WAIT 01
  -CheckIntNum 0,SUCCESS
  -JFAIL BcsFail2

  -LOG 3 No indications received from broadcast packet as expected.
  -LOG 3 SQLKEY:ADDMC001.6 PASS
  -JUMP RecFunc

:BcsFail1
  -LOG 3 Send to Broadcast Address failed.  Exiting test.
  -LOG 3 SQLKEY:ADDMC001.6 FAIL
  -JUMP abort

:BcsFail2
  -LOG 3 Indications received for broadcast packet.  None expected.
  -LOG 3 SQLKEY:ADDMC001.6 FAIL
  -JUMP end

:BcsFail3
  -LOG 3 Unable to set the packet filter.  Exiting test.
  -LOG 3 SQLKEY:ADDMC001.6 FAIL
  -JUMP abort


*** Send to Functional Address and don't expect to see it.
:RecFunc
  See if driver supports functional addresses.
  -CheckMacTable SC 40 m 04
  -JFAIL end

  -LOG 3 Attempting to send to Functional Address.
  -CheckIntNum X

  Tell slave to send to Functional Address.
  -MRECV 1 100 100 0 0 2 c0 00 03 04 05 06,SUCCESS
  -JFAIL FunFail1

  -WAIT 01
  -CheckIntNum 0,SUCCESS
  -JFAIL FunFail2

  -LOG 3 No indications received from functional packet as expected.
  -LOG 3 SQLKEY:ADDMC001.7 PASS
  -JUMP end

:FunFail1
  -LOG 3 Send to Functional Address failed.  Exiting test.
  -LOG 3 SQLKEY:ADDMC001.7 FAIL
  -JUMP abort

:FunFail2
  -LOG 3 Indications received for functional packet.  None expected.
  -LOG 3 SQLKEY:ADDMC001.7 FAIL
  -JUMP end


*** NOT_SUPPORTED error handler.
:NotSupported
   Specifically test for NOT_SUPPORTED before reporting that it is not
   supported
  -AddMulticastAddress 01 02 03 04 05 06,NOT_SUPPORTED
  -JFAIL NotSupportedFail

  -LOG 3 AddMulticastAddress is NOT_SUPPORTED for this MAC.
  -LOG 3 SQLKEY:ADDMC001.8 PASS
  -JUMP end2

:NotSupportedFail
  -LOG 3 ERROR: Service Specific Characteristics Table indicates that
  -LOG 3        AddMulticastAddress is not supported but AddMulticastAddress
  -LOG 3        did not return NOT_SUPPORTED.
  -LOG 3 SQLKEY:ADDMC001.8 FAIL
  -JUMP end2

:abort
  -LOG 3 Aborting ADDMC001 because state of MAC is uncertain.
  -SCRIPT ABORT ABORT
  -SetFailure
  -JUMP end3

:end
  Check for token ring
  -CheckMacTable SC 2 e 38 30 32 2e 35
  -JFAIL EtherMC
  -DelMulticastAddress 80 00 03 04 05 06,SUCCESS
  -JUMP Invalid

:EtherMC
  -DelMulticastAddress 01 02 03 04 05 06,SUCCESS

*** Attempt to add an invalid Multicast Address.
:Invalid
  -LOG 3 Attempting to add an invalid Multicast Address

  Add invalid Multicast Address.
  -AddMulticastAddress 00 01 02 03 04 05,INVALID_PARAMETER
  -JFAIL InvalidMCFail

  -LOG 3 Invalid Multicast Address returned INVALID_PARAMETER as expected.
  -LOG 3 SQLKEY:ADDMC001.9 PASS
  -JUMP end2

:InvalidMCFail
  -LOG 3 Invalid Multicast Address did not return INVALID_PARAMETER.
  -LOG 3 SQLKEY:ADDMC001.9 FAIL

:end2
  -LOG 3 Finished AddMulticastAddress Tests

*** Check the Service Specific Characteristics Table to see if the
*** CloseAdapter command is supported by this MAC.
  -LOG 3 Check if CloseAdapter is supported.
  -CheckMacTable SC 41 m 08
  -JFAIL end3

  -CloseAdapter,SUCCESS
  -JPASS end3

   Quit script because state of MAC is uncertain.
  -LOG 3 CloseAdapter failed.  Exiting test.
  -LOG 3 Aborting ADDMC001 because state of MAC is uncertain.
  -SCRIPT ABORT ABORT

:end3

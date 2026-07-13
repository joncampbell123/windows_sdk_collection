;*****************************************************************************
;*
;*   File: DELMC001.S - Script file to test DeleteMulticastAddress primitive.
;*
;*   Description: Verify operation of DeleteMulticastAddress under normal
;*                conditions.
;*                Tests include:
;*                   Delete unadded address, expect INVALID_FUNCTION.
;*                   Add/Delete MulticastAddress.
;*                   Verify that packets to previously deleted Multicast
;*                     Address are not received.
;*                   Add Two Multicast Address.
;*                   Delete First Multicast Address.
;*                   Verify packet can be recieved by second 
;*                     Multicast Address.
;*
;*   Date: 6/1/90
;*
;*   Pre-conditions: Adapter is closed.
;*                   Slave computer is ready.
;*
;*   Post-conditions: Adapter is closed.
;*
;*****************************************************************************

  -LOG 3 Starting DeleteMulticastAddress Tests

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
  -JPASS DelMulticast

  -LOG 3 Unable to identify slave computer.  Exiting script.
  -JUMP abort

:SlaveFail
  -LOG 3 Unable to set packet filter to 1 to identify slave.
  -LOG 3 Exiting script.
  -JUMP abort

*** Delete unadded Multicast Address expecting INVALID_PARAMETER.
:DelMulticast
  -LOG 3 Check that Multicast Addresses are supported.
  Check multicast bit.
  -CheckMacTable SC 40 m 02
  -JPASS DelMulticast
  -JFAIL NotSupported

  Check functional/group bit, if set, assume support for group 
  through the use of multicast commands.
  -CheckMacTable SC 40 m 04
  -JFAIL NotSupported
  
:DelMulticast
  -LOG 3 Deleting unadded Multicast Address.

  Check for token ring
  -CheckMacTable SC 2 e 38 30 32 2e 35
  -JFAIL EtherMC

  Delete token ring Multicast Address.
  -DelMulticastAddress 80 00 03 04 05 06,INVALID_PARAMETER
  -JFAIL DelMCFail
  -JUMP DelMCPass

:EtherMC
  Delete ethernet Multicast Address.
  -DelMulticastAddress 01 02 03 04 05 06,INVALID_PARAMETER
  -JFAIL DelMCFail

:DelMCPass
  -LOG 3 Delete of unadded Multicast Address returned INVALID_PARAMETER.
  -LOG 3 SQLKEY:DELMC001.1 PASS
  -JUMP AddDel

:DelMCFail
  -LOG 3 Delete of unadded Multicast Address did not return INVALID_PARAMETER.
  -LOG 3 SQLKEY:DELMC001.1 FAIL

*** Add and then delete a Multicast Address
:AddDel
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
  -LOG 3 SQLKEY:DELMC001.2 PASS
  -JUMP Delete

:AddMCFail
  -LOG 3 Unable to add Multicast Address.  Exiting test.
  -LOG 3 SQLKEY:DELMC001.2 FAIL
  -JUMP abort

:Delete
  -LOG 3 Deleting Multicast Address.

  Check for token ring
  -CheckMacTable SC 2 e 38 30 32 2e 35
  -JFAIL EtherMC

  Delete token ring Multicast Address.
  -DelMulticastAddress 80 00 03 04 05 06,SUCCESS
  -JFAIL DelMCFail
  -JUMP DelMCPass

:EtherMC
  Delete ethernet Multicast Address.
  -DelMulticastAddress 01 02 03 04 05 06,SUCCESS
  -JFAIL DelMCFail

:DelMCPass
  -LOG 3 Deleted Multicast Address.
  -LOG 3 SQLKEY:DELMC001.3 PASS
  -JUMP RecMC

:DelMCFail
  -LOG 3 Unable to delete Multicast Address.
  -LOG 3 SQLKEY:DELMC001.3 FAIL
  -JUMP abort

*** Tell slave to send to added/deleted Multicast Address.
*** Do not expect to see anything.
:RecMC
  -LOG 3 Attempting to receive on previously deleted Multicast Address.
  -CheckIntNum X

  Check for token ring
  -CheckMacTable SC 2 e 38 30 32 2e 35
  -JFAIL EtherMC

  Tell slave to send to token ring Multicast Address.
  -MRECV 1 100 100 0 0 2 C0 00 83 04 05 06,SUCCESS
  -JFAIL RecFail1
  -JUMP SentMC

:EtherMC
  Tell slave to send to ethernet Multicast Address.
  -MRECV 1 100 100 0 0 2 01 02 03 04 05 06,SUCCESS
  -JFAIL RecFail1

:SentMC
  -WAIT 01
  -CheckIntNum 0,SUCCESS
  -JFAIL RecFail2

  -LOG 3 No indications received from multicast packet as expected.
  -LOG 3 SQLKEY:DELMC001.4 PASS
  -JUMP AddTwo

:RecFail1
  -LOG 3 Send to Multicast Address failed.  Exiting test.
  -LOG 3 SQLKEY:DELMC001.4 FAIL
  -JUMP abort

:RecFail2
  -LOG 3 Indications received for multicast packet.  None expected.
  -LOG 3 SQLKEY:DELMC001.4 FAIL

*** Try to add two Multicast Addresses, then delete the first and send
*** to the second to make sure the proper addresses was deleted.
:AddTwo

  Check to see if driver supports more than one multicast. If not, 
  skip this test. 
  -CheckMacTable MC 0 e 00 00
  -JPASS end
  -CheckMacTable MC 0 e 01 00
  -JPASS end

  -LOG 3 Adding two multicasts. 

  Check for token ring
  -CheckMacTable SC 2 e 38 30 32 2e 35
  -JFAIL EtherMC

  Add token ring Multicast Address.
  -AddMulticastAddress 80 00 03 04 05 06,SUCCESS
  -JFAIL AddMCFail
  -AddMulticastAddress 80 00 06 05 04 03,SUCCESS
  -JFAIL AddMCFail
  -JUMP AddMCPass

:EtherMC
  Add ethernet Multicast Address.
  -AddMulticastAddress 01 02 03 04 05 06,SUCCESS
  -JFAIL AddMCFail
  -AddMulticastAddress 01 02 06 05 04 03,SUCCESS
  -JFAIL AddMCFail

:AddMCPass
  -LOG 3 Added Multicast Address.
  -JUMP DeleteFirst

:AddMCFail
  -LOG 3 Unable to add Multicast Address.  Exiting test.
  -LOG 3 SQLKEY:DELMC001.5 FAIL
  -JUMP abort

:DeleteFirst
  -LOG 3 Deleting first multicast.

  Check for token ring
  -CheckMacTable SC 2 e 38 30 32 2e 35
  -JFAIL EtherMC

  Delete token ring Multicast Address.
  -DelMulticastAddress 80 00 03 04 05 06,SUCCESS
  -JFAIL DelMCFail
  -JUMP DelMCPass

:EtherMC
  Delete ethernet Multicast Address.
  -DelMulticastAddress 01 02 03 04 05 06,SUCCESS
  -JFAIL DelMCFail

:DelMCPass
  -LOG 3 Deleted Multicast Address.
  -JUMP RecSecondMC

:DelMCFail
  -LOG 3 Unable to delete Multicast Address.
  -LOG 3 SQLKEY:DELMC001.5 FAIL
  -JUMP abort

:RecSecondMC
  -LOG 3 Attempting to receive on second Multicast Address.
  -CheckIntNum X

  Check for token ring
  -CheckMacTable SC 2 e 38 30 32 2e 35
  -JFAIL EtherMC

  Tell slave to send to token ring Multicast Address.
  -MRECV 1 100 100 0 0 2 C0 00 86 05 04 03,SUCCESS
  -JFAIL RecFail1
  -JUMP SentMC

:EtherMC
  Tell slave to send to ethernet Multicast Address.
  -MRECV 1 100 100 0 0 2 01 02 06 05 04 03,SUCCESS
  -JFAIL RecFail1

:SentMC
  -WAIT 01
  -CheckIntNum 1,SUCCESS
  -JFAIL RecFail2

  -LOG 3 Indications received from multicast packet as expected.
  -LOG 3 SQLKEY:DELMC001.5 PASS
  -JUMP MulCleanUp

:RecFail1
  -LOG 3 Send to Multicast Address failed.  Exiting test.
  -LOG 3 SQLKEY:DELMC001.5 FAIL
  -JUMP abort

:RecFail2
  -LOG 3 No indications received for multicast packet.  One Expected.
  -LOG 3 SQLKEY:DELMC001.5 FAIL

:MulCleanUp
  Check for token ring
  -CheckMacTable SC 2 e 38 30 32 2e 35
  -JFAIL EtherMC

  Delete token ring Multicast Address.
  -DelMulticastAddress 80 00 06 05 04 03,SUCCESS
  -JFAIL MulCleanAbort
  -JUMP end

:EtherMC
  Delete ethernet Multicast Address.
  -DelMulticastAddress 01 02 06 05 04 03,SUCCESS
  -JFAIL MulCleanAbort
  -JUMP end

:MulCleanAbort
  -LOG 3 Unable to delete Multicast Address.
  -JUMP abort


*** NOT_SUPPORTED error handler.
:NotSupported
   Specifically test for NOT_SUPPORTED before reporting that it is not
   supported

  Check for token ring
  -CheckMacTable SC 2 e 38 30 32 2e 35
  -JFAIL EtherMC

  Delete token ring Multicast Address.
  -DelMulticastAddress 80 00 03 04 05 06,NOT_SUPPORTED
  -JFAIL NotSupportedFail
  -JUMP NotSupPass

:EtherMC
  Delete ethernet Multicast Address.
  -DelMulticastAddress 01 02 03 04 05 06,NOT_SUPPORTED
  -JFAIL NotSupportedFail

:NotSupPass
  -LOG 3 DeleteMulticastAddress is NOT_SUPPORTED for this MAC.
  -LOG 3 SQLKEY:DELMC001.6 PASS
  -JUMP end

:NotSupportedFail
  -LOG 3 ERROR: Service Specific Characteristics Table indicates that
  -LOG 3        DeleteMulticastAddress is not supported but
  -LOG 3        DeleteMulticastAddress did not return NOT_SUPPORTED.
  -LOG 3 SQLKEY:DELMC001.6 FAIL
  -JUMP end

:abort
  -LOG 3 Aborting DELMC001 because state of MAC is uncertain.
  -SCRIPT ABORT ABORT
  -SetFailure
  -JUMP end2

:end
  -LOG 3 Finished DeleteMulticastAddress Tests

*** Check the Service Specific Characteristics Table to see if the
*** CloseAdapter command is supported by this MAC.
  -LOG 3 Check if CloseAdapter is supported.
  -CheckMacTable SC 41 m 08
  -JFAIL end2

  -CloseAdapter,SUCCESS
  -JPASS end2

   Quit script because state of MAC is uncertain.
  -LOG 3 CloseAdapter failed.  Exiting test.
  -LOG 3 Aborting DELMC001 because state of MAC is uncertain.
  -SCRIPT ABORT ABORT

:end2




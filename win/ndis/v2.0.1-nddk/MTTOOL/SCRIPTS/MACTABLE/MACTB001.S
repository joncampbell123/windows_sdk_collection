********************************************************************************
;*   FILE: Mactb001.s - Check the broadcast supported bit.
;*
;*   Description: If broadcast supported
;*		     Set packet filter to accept broadcast
;*                   Have master send 256 broadcast frames of 256 bytes
;*		     Make sure the master receives them if Loopback is
;*		     supported.
;*		     Check if Loopback is supported
;*                   Set up slave
;*                   Have slave send 256 broadcast frames of 256 bytes
;*                   Make sure the master receives them
;*                   Clean up
;*
;*   Date: 6-11-90
;*
;*   Pre-conditions: Adapter is closed
;*                   Slave exists
;*
;*   Post-conditions: Adapter is closed
;*
;*******************************************************************************


*** Check if broadcast is supported
  -CheckMacTable SC 40 m 01
  -JFAIL NotSupported

*** Set packet filter to receive broadcast frames
  -SetPacketFilter 2,SUCCESS
  -JFAIL FilterFail

  -LOG 3 Set Packet Filter returned SUCCESS.
  -LOG 3 SQLKEY:MACTB001.1 PASS
  -JUMP BcstTest

:FilterFail
  -LOG 3 Set packet filter failed. Exiting script.
  -LOG 3 SQLKEY:MACTB001.1 FAIL
  -JUMP End

:BcstTest
***Check the Service-Specific characteristics Table to see if the
***OpenAdapter command is supported by this MAC.
  -LOG 3 Check if OpenAdapter is supported.
  -CheckMacTable SC 41 m 08
  -JFAIL Bcst1

  -OpenAdapter,SUCCESS
  -JPASS Bcst1

   Quit script because state of the MAC is uncertain.
  -LOG 3 OpenAdapter in Broadcast test failed. Exiting script.
  -JUMP End

:Bcst1
  -ClearStatistics
  -WAIT 1
  -CheckIntNum X

*** 256 frames, 256 bytes min, 256 bytes max
*** 0 delay min, 0 delay max, broadcast
  -MTRAN 256 256 256 0 0 1,SUCCESS
  -JPASS Bcst2

  -LOG 3 broadcast send failed
  -LOG 3 SQLKEY:MACTB001.2 FAIL
  -JUMP End

:Bcst2
  -LOG 3 broadcast send passed
  -LOG 3 SQLKEY:MACTB001.2 PASS

  -UpdateStatistics
  -WAIT 1
  -CheckIntNum X

  Check to see if LoopBack is supported.
  -CheckMacTable SC 40 m 80
  -JFAIL Txmt

  -LOG 3 LoopBack is supported.

*** check that the statistics in the status table were updated
*** total frames received should be 256, 100h
  -CheckMacTable SS 14 e 00 01 00 00 | FF FF FF FF
  -JPASS Recv1

  -LOG 3 Total frames received not correct
  -LOG 3 SQLKEY:MACTB001.3 FAIL
  -JUMP Continue1

:Recv1
  -LOG 3 Total frames received was correct
  -LOG 3 SQLKEY:MACTB001.3 PASS

:Continue1
*** total bytes received should be 65536, 10000h
  -CheckMacTable SS 1C e 00 00 01 00 | FF FF FF FF
  -JPASS Recv2

  -LOG 3 Total bytes received not correct
  -LOG 3 SQLKEY:MACTB001.4 FAIL
  -JUMP Continue2

:Recv2
  -LOG 3 Total bytes received was correct
  -LOG 3 SQLKEY:MACTB001.4 PASS

:Continue2
*** broadcast frames received should be 256, 100h
  -CheckMacTable SS 28 e 00 01 00 00| FF FF FF FF
  -JPASS Recv3

  -LOG 3 Total broadcast frames received not correct
  -LOG 3 SQLKEY:MACTB001.5 FAIL
  -JUMP Continue3

:Recv3
  -LOG 3 Total broadcast frames received was correct
  -LOG 3 SQLKEY:MACTB001.5 PASS

:Continue3
*** broadcast bytes received should be 65536, 10000h
  -CheckMacTable SS 3C e 00 00 01 00 | FF FF FF FF
  -JPASS Recv4

  -LOG 3 Total broadcast bytes received not correct
  -LOG 3 SQLKEY:MACTB001.6 FAIL
  -JUMP Txmt

:Recv4
  -LOG 3 Total broadcast bytes received was correct
  -LOG 3 SQLKEY:MACTB001.6 PASS

:Txmt
*** total frames transmitted should be 256, 100h
  -CheckMacTable SS 44 e 00 01 00 00 | FF FF FF FF
  -JPASS Txmt1

  -LOG 3 Total frames transmitted not correct
  -LOG 3 SQLKEY:MACTB001.7 FAIL
  -JUMP Continue4

:Txmt1
  -LOG 3 Total frames transmitted was correct
  -LOG 3 SQLKEY:MACTB001.7 PASS

:Continue4
*** total bytes transmitted should be 65536, 10000h
  -CheckMacTable SS 48 e 00 00 01 00 | FF FF FF FF
  -JPASS Txmt2

  -LOG 3 Total bytes transmitted not correct
  -LOG 3 SQLKEY:MACTB001.8 FAIL
  -JUMP Continue5

:Txmt2
  -LOG 3 Total bytes transmitted was correct
  -LOG 3 SQLKEY:MACTB001.8 PASS

:Continue5
*** broadcast frames transmitted should be 256, 100h
  -CheckMacTable SS 50 e 00 01 00 00 | FF FF FF FF
  -JPASS Txmt3

  -LOG 3 Total broadcast frames transmitted not correct
  -LOG 3 SQLKEY:MACTB001.9 FAIL
  -JUMP Continue6

:Txmt3
  -LOG 3 Total broadcast frames transmitted was correct
  -LOG 3 SQLKEY:MACTB001.9 PASS

:Continue6
*** broadcast bytes transmitted should be 65536, 10000h
  -CheckMacTable SS 54 e 00 00 01 00 | FF FF FF FF
  -JPASS Txmt4

  -LOG 3 Total broadcast bytes transmitted not correct
  -LOG 3 SQLKEY:MACTB001.10 FAIL
  -JUMP Continue7

:Txmt4
  -LOG 3 Total broadcast bytes transmitted was correct
  -LOG 3 SQLKEY:MACTB001.10 PASS

:Continue7
*** now do the same with a broadcast from the slave
*** set packet filter such that we can identify the slave
  -SetPacketFilter 3,SUCCESS
  -JPASS CallSlave

  -LOG 3 Set packet filter failed. Exiting script.
  -JUMP Abort

:CallSlave
  -IDSLAVE,SUCCESS
  -JPASS MrcvTest

  -LOG 3 slave mode section failed
  -JUMP Abort

:MrcvTest
  -LOG 3 slave mode section started

  -ClearStatistics
  -WAIT 1
  -CheckIntNum X

*** 256 frames, 256 bytes min, 256 bytes max
*** 0 delay min, 0 delay max, broadcast
  -MRECV 256 256 256 0 0 1,SUCCESS
  -JPASS Mrcv2

  -LOG 3 broadcast send from slave failed
  -LOG 3 SQLKEY:MACTB001.11 FAIL
  -JUMP End

:Mrcv2
  -LOG 3 broadcast send from slave passed
  -LOG 3 SQLKEY:MACTB001.11 PASS
  -WAIT 5
*** wait for broadcast to finish

  -UpdateStatistics
  -WAIT 1
  -CheckIntNum X

*** check that the statistics in the status table were updated
*** total frames received should be 256, 100h
  -CheckMacTable SS 14 e 00 01 00 00 | FF FF FF FF
  -JPASS Mrcv4

  -LOG 3 Total frames received not correct
  -LOG 3 SQLKEY:MACTB001.12 FAIL
  -JUMP Continue8

:Mrcv4
  -LOG 3 Total frames received was correct
  -LOG 3 SQLKEY:MACTB001.12 PASS

:Continue8
*** total bytes received should be 65536, 10000h
  -CheckMacTable SS 1C e 00 00 01 00 | FF FF FF FF
  -JPASS Mrcv5

  -LOG 3 Total bytes received not correct
  -LOG 3 SQLKEY:MACTB001.13 FAIL
  -JUMP Continue9

:Mrcv5
  -LOG 3 Total bytes received was correct
  -LOG 3 SQLKEY:MACTB001.13 PASS

:Continue9
*** broadcast frames received should be 256, 100h
  -CheckMacTable SS 28 e 00 01 00 00 | FF FF FF FF
  -JPASS Mrcv6

  -LOG 3 Total broadcast frames received not correct
  -LOG 3 SQLKEY:MACTB001.14 FAIL
  -JUMP Continue10

:Mrcv6
  -LOG 3 Total broadcast frames received was correct
  -LOG 3 SQLKEY:MACTB001.14 PASS

:Continue10
*** broadcast bytes received should be 65536, 10000h
  -CheckMacTable SS 3C e 00 00 01 00 | FF FF FF FF
  -JPASS Mrcv7

  -LOG 3 Total broadcast bytes received not correct
  -LOG 3 SQLKEY:MACTB001.15 FAIL
  -JUMP Continue11

:Mrcv7
  -LOG 3 Total broadcast bytes received was correct
  -LOG 3 SQLKEY:MACTB001.15 PASS

:Continue11
  -JUMP End

:NotSupported
  -LOG 3 broadcast is not supported
  -JUMP End

:Abort
  -LOG 3 Aborting MACTB001 because state of MAC is uncertain
  -SCRIPT ABORT ABORT
  -SetFailure
  -JUMP End2

:End
  -CloseAdapter
  -Wait 2

:End2


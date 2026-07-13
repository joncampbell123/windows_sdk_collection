;*******************************************************************************
;*   FILE: Mactb002.s - Check the multicast supported bit.
;*
;*   Description: If multicast supported
;*		     Make sure multicast list is empty
;*                   Add multicast address 01 ab bc cd de ef
;*                   Check that it was added
;*                   Have master send 256 multicast frames of 256 bytes
;*                      to 01 ab bc cd de ef
;*                   Make sure the master receives them
;*                   Set up slave
;*                   Have slave send 256 multicast frames of 256 bytes
;*                      to 01 ab bc cd de ef
;*                   Make sure the master receives them
;*                   Clean up
;*
;*   Date: 6-11-90
;*
;*   Pre-conditions: Adapter is closed
;*                   Slave exists
;*                   multicast address list is empty
;*
;*   Post-conditions: Adapter is closed
;*                    multicast address list is empty
;*
;*******************************************************************************


*** check that multicast is supported
  -CheckMacTable SC 40 m 06
  -JFAIL NotSupported

*** Check the Service Specific Characteristics Table to see if the OpenAdapter
*** command is supported by this MAC.
  -CheckMacTable SC 41 m 08
  -JFAIL Multi1

  -OpenAdapter,SUCCESS
  -JFAIL MultiFail1

  -LOG 3 OpenAdapter passed open test.

:Multi1
*** check that the multicast address list is empty
  -CheckMacTable MC 02 e 00 00
  -JPASS Multi2
  -LOG 3 multicast address list must be empty
  -JUMP Abort

:MultiFail1
  -LOG 3 OpenAdapter failed the multicast test. Exiting script.
  -JUMP Abort

:Multi2
*** set packet filter to accept multicast addresses
  -SetPacketFilter 1,SUCCESS
  -JPASS Multi3

  -LOG 3 Set packet filter failed
  -JUMP End

:Multi3
  -LOG 3 Set packet filter succeeded

;check for token ring
  -CheckMacTable sc 2 e 38 30 32 2e 35
  -JFAIL EtherNet

*** add multicast address 80 00 bc cd de ef
  -AddMulticastAddress 80 00 bc cd de ef,SUCCESS
  -JPASS Multi4

  -LOG 3 Add multicast address failed
  -LOG 3 SQLKEY:MACTB002.1 FAIL
  -JUMP End

:EtherNet
*** add multicast address 01 ab bc cd de ef
  -AddMulticastAddress 01 ab bc cd de ef,SUCCESS
  -JPASS Multi4

  -LOG 3 Add multicast address failed
  -LOG 3 SQLKEY:MACTB002.1 FAIL
  -JUMP End

:Multi4
  -LOG 3 Add multicast address passed
  -LOG 3 SQLKEY:MACTB002.1 PASS


*** check multicast address list
  -CheckMacTable MC 02 e 01 00
  -JPASS Multi5

  -LOG 3 number of multicast addresses not updated
  -LOG 3 SQLKEY:MACTB002.2 FAIL
  -JUMP Continue

:Multi5
  -LOG 3 number of multicast addresses updated
  -LOG 3 SQLKEY:MACTB002.2 PASS

:Continue
;check for token ring
  -CheckMacTable sc 2 e 38 30 32 2e 35
  -JFAIL EtherNet

*** check that address is in multicast address list
  -CheckMacTable MC 04 e 80 00 bc cd de ef
  -JPASS Multi6

  -LOG 3 multicast address not added to list
  -LOG 3 SQLKEY:MACTB002.3 FAIL
  -JUMP Continue

:EtherNet
*** check that address is in multicast address list
  -CheckMacTable MC 04 e 01 ab bc cd de ef
  -JPASS Multi6

  -LOG 3 multicast address not added to list
  -LOG 3 SQLKEY:MACTB002.3 FAIL
  -JUMP Continue

:Multi6
  -LOG 3 multicast address added to list
  -LOG 3 SQLKEY:MACTB002.3 PASS

:Continue
*** clear statistics table
  -ClearStatistics
  -WAIT 1
  -CheckIntNum X

;check for token ring
  -CheckMacTable sc 2 e 38 30 32 2e 35
  -JFAIL EtherNet

*** send 256 frames of 256 bytes each to address 80 00 bc cd de ef
  -MTRAN 256 256 256 0 0 2 c0 00 bc cd de ef,SUCCESS
  -JPASS Multi7

  -LOG 3 transmit failed
  -LOG 3 SQLKEY:MACTB002.4 FAIL
  -JUMP SlaveStuff

:EtherNet
*** send 256 frames of 256 bytes each to address 01 ab bc cd de ef
  -MTRAN 256 256 256 0 0 2 01 ab bc cd de ef,SUCCESS
  -JPASS Multi7

  -LOG 3 transmit failed
  -LOG 3 SQLKEY:MACTB002.4 FAIL
  -JUMP SlaveStuff

:Multi7
  -LOG 3 transmit succeeded
  -LOG 3 SQLKEY:MACTB002.4 PASS

*** make sure master updates multicast frames transmitted
  -UpdateStatistics
  -WAIT 1
  -CheckIntNum X

  -CheckMacTable SS 4c e 00 01 00 00 | FF FF FF FF
  -JPASS Multi8

  -LOG 3 multicast frames transmitted statistic not updated
  -LOG 3 SQLKEY:MACTB002.5 FAIL
  -JUMP Continue

:Multi8
  -LOG 3 multicast frames transmitted statistic updated
  -LOG 3 SQLKEY:MACTB002.5 PASS

:Continue
*** make sure master updates multicast bytes transmitted
  -CheckMacTable SS 58 e 00 00 01 00 | FF FF FF FF
  -JPASS Multi9

  -LOG 3 multicast bytes transmitted statistic not updated
  -LOG 3 SQLKEY:MACTB002.6 FAIL
  -JUMP LoopChk

:Multi9
  -LOG 3 multicast bytes transmitted statistic updated
  -LOG 3 SQLKEY:MACTB002.6 PASS

:LoopChk
*** Check to see if loopback is supported.
  -LOG 3 Check for Loopback support.
  -CheckMacTable SC 40 m 80
  -JFAIL SlaveStuff

:Continue
*** make sure master updates multicast frames received
  -CheckMacTable SS 24 e 00 01 00 00 | FF FF FF FF
  -JPASS Multi10

  -LOG 3 multicast frames received statistic not updated
  -LOG 3 SQLKEY:MACTB002.7 FAIL
  -JUMP Continue

:Multi10
  -LOG 3 multicast frames received statistic updated
  -LOG 3 SQLKEY:MACTB002.7 PASS

:Continue
*** make sure master updates multicast bytes received
  -CheckMacTable SS 38 e 00 00 01 00 | FF FF FF FF
  -JPASS Multi11
  -LOG 3 multicast bytes received statistic not updated
  -LOG 3 SQLKEY:MACTB002.8 FAIL
  -JUMP SlaveStuff

:Multi11
  -LOG 3 multicast bytes received statistic updated
  -LOG 3 SQLKEY:MACTB002.8 PASS

:SlaveStuff
*** set up slave mode
  -IDSLAVE,SUCCESS
  -JPASS Pass

  -LOG 3 slave mode failed
  -JUMP Abort

:Pass
  -LOG 3 slave mode succeeded

*** clear statistics table
  -ClearStatistics
  -WAIT 1
  -CheckIntNum X

;check for token ring
  -CheckMacTable sc 2 e 38 30 32 2e 35
  -JFAIL EtherNet

*** have slave send 256 frames of 256 bytes to c0 00 bc cd de ef
  -MRECV 256 256 256 0 0 2 c0 00 bc cd de ef,SUCCESS
  -JPASS Multi12

  -LOG 3 Attempted send by slave failed
  -LOG 3 SQLKEY:MACTB002.9 FAIL
  -JUMP End

:EtherNet
*** have slave send 256 frames of 256 bytes to 01 ab bc cd de ef
  -MRECV 256 256 256 0 0 2 01 ab bc cd de ef,SUCCESS
  -JPASS Multi12

  -LOG 3 attempted send by slave failed
  -LOG 3 SQLKEY:MACTB002.9 FAIL
  -JUMP End

:Multi12
  -LOG 3 attempted send by slave succeeded
  -LOG 3 SQLKEY:MACTB002.9 PASS

*** wait to be sure it got here
  -WAIT 5

*** Update statistics table
  -UpdateStatistics
  -WAIT 1
  -CheckIntNum X

:Multi13
*** make sure master updates multicast frames received
  -CheckMacTable SS 24 e 00 01 00 00 | FF FF FF FF
  -JPASS Multi14

  -LOG 3 multicast frames received statistic not updated
  -LOG 3 SQLKEY:MACTB002.10 FAIL
  -JUMP Continue

:Multi14
  -LOG 3 multicast frames received statistic updated
  -LOG 3 SQLKEY:MACTB002.10 PASS

:Continue
*** make sure master updates multicast bytes received
  -CheckMacTable SS 38 e 00 00 01 00 | FF FF FF FF
  -JPASS Multi15

  -LOG 3 multicast bytes received statistic not updated
  -LOG 3 SQLKEY:MACTB002.11 FAIL
  -JUMP End

:Multi15
  -LOG 3 multicast bytes received statistic updated
  -LOG 3 SQLKEY:MACTB002.11 PASS
  -JUMP End

:NotSupported
  -LOG 3 multicast not supported
  -JUMP close

:Abort
  -LOG 3 test aborted
  -Script abort abort
  -SetFailure
  -JUMP End2

:End
;check for token ring
  -CheckMacTable sc 2 e 38 30 32 2e 35
  -JFAIL EtherNet
  -DelMulticastAddress 80 00 bc cd de ef
  -JUMP close

:EtherNet
  -DelMulticastAddress 01 ab bc cd de ef

:close
  -SetPacketFilter 0
  -CloseAdapter
  -WAIT 2

:End2


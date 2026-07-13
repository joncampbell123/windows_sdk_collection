;*******************************************************************************
;*   FILE: mactb004.s - checks promiscuous mode
;*
;*   Description: Check that promiscuous mode is supported
;*		  Set up Slave
;*		  Have Slave send 1 frame of 100 bytes to 00 00 c3 d4 e5 f6
;*		  Check that master received it
;*		  Have Slave send 256 frames of 256 bytes
;*		  Check that the statistics are updated
;*
;*   Date: 5-30-90
;*
;*   Pre-conditions: adapter is closed
;*
;*   Post-conditions: adapter is closed
;*		      filter is 0
;*
;*******************************************************************************


*** Check that promiscuous mode is supported
  -CheckMacTable SC 40 m 08
  -JFAIL NotSupported
  -LOG 3 promiscuous mode is supported

*** Check the Service Specific Characteristics Table to see if the
*** OpenAdapter is supported by this MAC.
  -CheckMacTable SC 41 m 08
  -JFAIL SetFilter

  -OpenAdapter,SUCCESS
  -JFAIL Testfail

  -LOG 3 adapter opened successfully

:SetFilter
*** set filter to 4, promiscuous mode
  -SetPacketFilter 4,SUCCESS
  -JPASS Pass
  -LOG 3 cannot set promiscuous mode
  -LOG 3 SQLKEY:MACTB004.1 FAIL
  -JUMP End

:Testfail
  -LOG 3 Open Adapter failed. Exiting Script.
  -JUMP abort

:Pass
  -LOG 3 promiscuous mode set
  -LOG 3 SQLKEY:MACTB004.1 PASS

*** Identify slave computer.
  -IDSLAVE,SUCCESS
  -JPASS Pass

  -LOG 3 Unable to identify slave computer. Exiting script.
  -JUMP abort

:Pass
*** Tell slave to send to address 00 00 c3 d4 e5 f6
  -CheckIntNum x
  -MRECV 1 100 100 0 0 2 00 00 c3 d4 e5 f6
  -CheckIntNum 1,SUCCESS
  -JPASS Pass
  -LOG 3 Attempted send by slave Failed.
  -LOG 3 SQLKEY:MACTB004.2 FAIL
  -JUMP Continue

:Pass
  -LOG 3 received the data
  -LOG 3 SQLKEY:MACTB004.2 PASS

:Continue
  -LOG 3 Issue a ClearStatistics.
  -ClearStatistics
  -WAIT 1

*** Tell slave to send 256 frames with 256 bytes to 00 00 c3 d4 e5 f6
  -CheckIntNum X
  -MRECV 256 256 256 0 0 2 00 00 c3 d4 e5 f6
  -JPASS Pass
  -LOG 3 Attempted send by slave Failed
  -LOG 3 SQLKEY:MACTB004.3 FAIL
  -JUMP End

:Pass
  -LOG 3 received the data
  -LOG 3 SQLKEY:MACTB004.3 PASS

*** make sure master updates frames received
  -CheckMacTable SS 24 e 00 01 00 00 | FF FF FF FF
  -JPASS Pass
  -LOG 3 promiscuous frames received but statistics not updated
  -LOG 3 SQLKEY:MACTB004.4 FAIL
  -JUMP End

:Pass
  -LOG 3 promiscuous frames received and statistics updated
  -LOG 3 SQLKEY:MACTB004.4 PASS
  -JUMP End

:NotSupported
  -LOG 3 promiscuous mode is not supported
  -LOG 3 SQLKEY:MACTB004.5 PASS
  -JUMP End

:abort
-LOG 3 Aborting MACTB004 because state of MAC is uncertain
-SCRIPT ABORT ABORT
-JUMP End2

:End
  -CloseAdapter
  -Wait 2
  -SetPacketFilter 0

:End2


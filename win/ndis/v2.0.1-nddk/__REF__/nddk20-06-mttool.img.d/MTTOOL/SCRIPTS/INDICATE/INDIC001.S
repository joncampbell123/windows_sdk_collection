;*******************************************************************************
;*   FILE: indic001.s - checks indication on/off
;*
;*   Description: open adapter
;*                Indication off
;*                Indication off
;*                Indication off
;*                Transmit broadcast
;*                    Check for 0 indication
;*                Indication on
;*                Transmit broadcast
;*                    Check for 0 indication
;*                Indication on
;*                Indication on
;*                Transmit broadcast
;*                    Check for 1 indication
;*                Indication off
;*                Reset mac
;*                Transmit broadcast
;*                   Check for 1 indication
;*                close adapter
;*
;*   Date: 6/1/90
;*
;*   Pre-conditions: adapter is closed
;*                   indications are on
;*
;*   Post-conditions: adapter is closed
;*                    indications are on
;*
;*******************************************************************************

  -LOG 3 Starting Indication On/Off Tests

*** Check the Service Specific Characteristics Table to see if the
*** OpenAdapter command is supported by this MAC.
  -LOG 3 Check if OpenAdapter is supported.
  -CheckMacTable SC 41 m 08
  -JFAIL GetSlave

  -OpenAdapter,SUCCESS
  -JPASS GetSlave

   Quit script because state of MAC is uncertain.
  -LOG 3 OpenAdapter failed.  Exiting test.
  -JUMP Abort

*** Identify slave computer.
:GetSlave
   Need to be able to see slave's response.
  -SetPacketFilter 1,SUCCESS
  -JFAIL SlaveFail

  -IDSLAVE,SUCCESS
  -JPASS SetFilter

  -LOG 3 Unable to identify slave computer.  Exiting script.
  -JUMP Abort

:SlaveFail
  -LOG 3 Unable to set packet filter to 1 or identify slave.
  -LOG 3 Exiting script.
  -JUMP Abort

*** Set filter to receive all frames. 
:SetFilter
  -LOG 3 Set filter to receive broadcast frames
  -SetPacketFilter 3,SUCCESS
  -JFAIL Abort

;clear indications
  -CheckIntNum X

  -IndicationOff,SUCCESS
  -JPASS PASS
  -LOG 3 Indication off failed
  -LOG 3 SQLKEY:INDIC001.1 FAIL
  -JUMP Abort

:PASS
  -LOG 3 Indication off passed
  -LOG 3 SQLKEY:INDIC001.1 PASS
  -IndicationOff,SUCCESS
  -JPASS PASS
  -LOG 3 Indication off failed
  -LOG 3 SQLKEY:INDIC001.2 FAIL
  -JUMP Abort

:PASS
  -LOG 3 Indication off passed
  -LOG 3 SQLKEY:INDIC001.2 PASS
  -IndicationOff,SUCCESS
  -JPASS PASS
  -LOG 3 Indication off failed
  -LOG 3 SQLKEY:INDIC001.3 FAIL
  -JUMP Abort

:PASS
  -LOG 3 Indication off passed
  -LOG 3 SQLKEY:INDIC001.3 PASS
  -MRECV 1 64 64 0 0 1,SUCCESS
  -JPASS PASS

  -LOG 3 Slave transmit request failed.
  -LOG 3 SQLKEY:INDIC001.4 FAIL
  -JUMP Abort

:PASS
  -LOG 3 Slave transmit completed successfully.
  -CheckIntNum 0
  -JPASS PASS
  -LOG 3 indications received
  -LOG 3 SQLKEY:INDIC001.4 FAIL
  -JUMP Abort

:PASS
  -LOG 3 0 indications received as expected
  -LOG 3 SQLKEY:INDIC001.4 PASS
  -IndicationOn,SUCCESS
  -JPASS PASS
  -LOG 3 Indication on failed
  -LOG 3 SQLKEY:INDIC001.5 FAIL
  -JUMP Abort

:PASS
  -LOG 3 Indication on succeeded
  -LOG 3 SQLKEY:INDIC001.5 PASS
  -MRECV 1 64 64 0 0 1,SUCCESS
  -JPASS PASS

  -LOG 3 Slave transmit request failed.
  -LOG 3 SQLKEY:INDIC001.6 FAIL
  -JUMP Abort

:PASS
  -LOG 3 Slave transmit completed successfully.
  -CheckIntNum 0
  -JPASS PASS
  -LOG 3 indications received
  -LOG 3 SQLKEY:INDIC001.6 FAIL
  -JUMP Abort

:PASS
  -LOG 3 0 indications received as expected
  -LOG 3 SQLKEY:INDIC001.6 PASS
  -IndicationOn,SUCCESS
  -JPASS PASS
  -LOG 3 Indication on didn't succeed
  -LOG 3 SQLKEY:INDIC001.7 FAIL
  -JUMP Abort

:PASS
  -LOG 3 Indication on succeeded
  -LOG 3 SQLKEY:INDIC001.7 PASS
  -IndicationOn,SUCCESS
  -JPASS PASS
  -LOG 3 Indication on didn't succeed
  -LOG 3 SQLKEY:INDIC001.8 FAIL
  -JUMP Abort

:PASS
  -LOG 3 Indication on succeeded
  -LOG 3 SQLKEY:INDIC001.8 PASS
  -CheckIntNum X                       ;Clear interrupts
  -MRECV 1 64 64 0 0 1,SUCCESS
  -JPASS PASS

  -LOG 3 Slave transmit request failed.
  -LOG 3 SQLKEY:INDIC001.9 FAIL
  -JUMP Abort

:PASS
  -LOG 3 Slave transmit completed successfully.
  -CheckIntNum 1
  -JPASS PASS
  -LOG 3 0 indications received
  -LOG 3 SQLKEY:INDIC001.9 FAIL
  -JUMP Abort

:PASS
  -LOG 3 1 Indication received
  -LOG 3 SQLKEY:INDIC001.9 PASS

*** Check the Service Specific Characteristics Table to see if the
*** ResetMAC command is supported by this MAC.
  -LOG 3 Check if ResetMAC is supported.
  -CheckMacTable SC 41 m 04
  -JFAIL SkipReset

  -IndicationOff,SUCCESS
  -JPASS PASS
  -LOG 3 Indication off failed
  -LOG 3 SQLKEY:INDIC001.10 FAIL
  -JUMP Abort

:PASS
  -LOG 3 Indication off succeeded
  -LOG 3 SQLKEY:INDIC001.10 PASS
  -ResetMAC,SUCCESS
  -JPASS PASS

   Quit script because state of MAC is uncertain.
  -LOG 3 Reset mac failed.  Exiting Test.
  -LOG 3 SQLKEY:INDIC001.11 FAIL
  -JUMP Abort

:PASS
  -LOG 3 Reset mac passed
  -LOG 3 SQLKEY:INDIC001.11 PASS

  ;In Mttool, Request timeout value is increased for Openadapter and Reset.
  ;Issue a request to lower the timeout value.
  -SetPacketFilter 3

  -CheckIntNum X                       ;Clear interrupts
  -MRECV 1 64 64 0 0 1,SUCCESS
  -JPASS PASS

  -LOG 3 Slave transmit request failed.
  -LOG 3 SQLKEY:INDIC001.12 FAIL
  -JUMP Abort

:PASS
  -LOG 3 Slave transmit completed successfully.
  -CheckIntNum 1
  -JPASS PASS
  -LOG 3 No indications received, reset didn't turn indications on.
  -LOG 3 SQLKEY:INDIC001.12 FAIL
  -JUMP Abort

:PASS
  -LOG 3 One Indication received, reset turned indications on.
  -LOG 3 SQLKEY:INDIC001.12 PASS

*** This part of the test script tests that the proper indications 
*** opcodes are used by the driver for the reset indications.  
  -CheckIntNum X
  -SetStatInd 3 5 
  -ResetMAC
  -WAIT 15
  -ChkStatInd    
  -JPASS PASS

  -LOG 3 Incorrect reset indication op codes received.
  -LOG 3 SQLKEY:INDIC001.13 FAIL
  -JUMP Abort

:PASS
  -LOG 3 1 Correct reset indication op codes received.
  -LOG 3 SQLKEY:INDIC001.13 PASS

*** This part of the test script tests that the proper indications 
*** opcodes are used by the driver for the interrupt indications.  
:SkipReset
  Check if Interrupt is supported.
  -CheckMacTable SC 41 m 10
  -JPASS IntSupp

  -LOG 3 Interrupt not supported.
  -JUMP End

:IntSupp
  -CheckIntNum X
  -SetStatInd 4
  -Interrupt
  -WAIT 5
  -ChkStatInd
  -JPASS PASS

  -LOG 3 Incorrect interrupt indication op code received.
  -LOG 3 SQLKEY:INDIC001.14 FAIL
  -JUMP Abort

:PASS
  -LOG 3 1 Correct interrupt indication op code received.
  -LOG 3 SQLKEY:INDIC001.14 PASS
  -JUMP End

:Abort
  -script Abort Abort
  -SetFailure
  -JUMP End2

:End
  -LOG 3 Finished Indication On/Off Tests

  -SetPacketFilter 0,SUCCESS

*** Check the Service Specific Characteristics Table to see if the
*** CloseAdapter command is supported by this MAC.
  -LOG 3 Check if CloseAdapter is supported.
  -CheckMacTable SC 41 m 08
  -JFAIL End2

  -CloseAdapter,SUCCESS
  -JPASS End2

   Quit script because state of MAC is uncertain.
  -LOG 3 CloseAdapter failed.  Exiting test.
  -LOG 3 Aborting INDIC001 because state of MAC is uncertain.
  -SCRIPT ABORT ABORT

:End2


;*******************************************************************************
;*
;*   File: SLOOK001.S - Script file to test the SetLookahead primitive.
;*
;*   Description: Verify SetLookahead operation under normal conditions.
;*		  Tests include:
;*		     - Setting valid length succeeds
;*		     - Setting length lower than first setlookahead value.
;*		       This should SUCCEED and the LOOKAHEAD minimum value
;*		       should not change.
;*		     - Setting length = 256 succeeds (required upper boundary)
;*		     - Setting length > 256 either succeeds or returns
;*		       INVALID_PARAMETER.
;*		     - Setting valid length with ReqHandle = 0 succeeds
;*		     - For each of the above lengths that receive a SUCCESS,
;*		       a received frame will be checked for a valid
;*		       Lookahead size.	This assumes that the driver supports
;*		       Broadcast Frames and that it has some method
;*		       of transmitting and receiving a frame (i.e. Either
;*		       loopback is supported or another station
;*		       is setup to echo back frames.)
;*
;*   Date: 1/31/90
;*
;*   Pre-conditions: Adapter is closed.
;*		     Loopback is supported or another station exists
;*		     on the network which is in echo mode.
;*		     If slave is used, the master and slave systems have
;*		     already been connected.
;*
;*   Post-conditions: Adapter is closed.
;*		      Txbuf size will be reset to 64 bytes.
;*		      The lookahead value will be reset to 64 bytes
;*		      Turn ReqHandle mode OFF
;*
;*   Changes:	      06-01-90, Carol F.,
;*			 - Took out GDT, Register Preservation, and Handle = 0
;*			 - Added tests for preservation of the lookahead value
;*			   after a close and a reset request. (These tests were
;*			   taken out of their respective script files because
;*			   of the necessity of using sstate 1 opposed to MRECV.
;*			 - Changed frame sent to a broadcast destination instead
;*			   of the address set by SetStationAddress.
;*			 - Changed first setlookahead value from 64 to 128.
;*			   Also changed frame size to 128.
;*			 - Added CheckIntNum x after SlookGTRSupported1
;*
;*******************************************************************************


;*** Test for Broadcast support and loopback support.
;*** If the driver can send and recieve a specified frame, validate
;*** the received frame's lookahead size.  If not just do the SetLookahead
;*** commands and validate the expected return code.

;***
;*** CHECK FOR BROADCAST AND LOOKBACK SUPPORT
;***

-CheckMACTable SC 40 m 01	 ;Is Broadcast supported?
-JFAIL SlookOnly		 ;  No, Only look at return codes.


;    Driver supports Broadcast

;    Is the driver a token ring driver?
-CheckMACTable SC 2 e 38 30 32 2E 35
-JPASS TokenRingType1


;    The driver is an ETHERNET type

-Buf i FF FF FF FF FF FF FF FF FF FF FF FF    ;Works for both loopback and nonloopback
-JUMP SlookWithXmit


;    The driver is a TOKEN RING type

:TokenRingType1
-Buf i 00 40 FF FF FF FF FF FF


;***
;*** TEST SETLOOKAHEAD COMMANDS AND RECEIVED LOOKAHEAD VALUES.
;***

:SlookWithXmit


;    Set frame size = 80h, Filter for directed frames
-Bufsz 40 40
-filter 3

;    Is OpenAdapter supported?	If so, issue an open
-CheckMACTable SC 40 m 00 08
-JFAIL SlookValid1
-OpenAdapter,SUCCESS
-JPASS SlookValid1

; Quit script because state of MAC is uncertain.
-LOG 3 OpenAdapter failed.  Exiting test.
-JUMP abort



;*** SLOOK001.1  -  SetLookahead with Receive Checking
;*** Try a SetLookahead command with a valid length.  If this fails, assume
;    the SetLookahead command for this driver does not work.

:SlookValid1

;    Is loopback supported? If not turn on slave's echo mode
-CheckMACTable SC 40 m 80
-JPASS LoopBackSup

-IDSLAVE,SUCCESS
-JPASS TurnOnEcho

-LOG 3 Unable to identify slave computer.  Exiting script.
-JUMP abort

:TurnOnEcho
;Turn slave's echo mode ON
-sstate 1
		
:LoopBackSup
-checkintnum x
-SetLookahead 128,SUCCESS	    ;Note, 128 is decimal
-JPASS SlookCont1

-LOG 3 SetLookahead with valid length did not return SUCCESS.
-LOG 3 SetLookahead cannot be tested
-LOG 3 SQLKEY:SLOOK001.1 FAIL
-JUMP SlookFinished


:SlookCont1
-LOG 3 SetLookahead with valid length returned SUCCESS.

;    Test Received lookahead length  (Lookahead length = framesize)
-TRANSMITCHAIN 3 80
-JFAIL Transmit1Fail

-LOG 3 Received Lookahead length was VALID.
-LOG 3 SQLKEY:SLOOK001.1 PASS
-JUMP SlookLowerTest1

:Transmit1Fail
-LOG 3 Received Lookahead length was INVALID.
-LOG 3 SQLKEY:SLOOK001.1 FAIL



;*** SLOOK001.2  -  SetLookahead with Receive Checking
;*** Try a SetLookahead command with length lower than first SetLookahead (128).

:SlookLowerTest1

-SetLookahead 64,SUCCESS
-JFAIL SlookLowerFail1

-LOG 3 SetLookahead with lower value returned SUCCESS.

;    Test Received lookahead length
-Bufsz 40 40 40
-TRANSMITCHAIN 3 C0
-JFAIL Transmit2Fail

-LOG 3 Received Lookahead length was VALID.
-LOG 3 SQLKEY:SLOOK001.2 PASS
-JUMP SlookUpperTest1

:Transmit2Fail
-LOG 3 Received Lookahead length was INVALID.
-LOG 3 SQLKEY:SLOOK001.2 FAIL
-JUMP SlookUpperTest1

:SlookLowerFail1
-LOG 3 SetLookahead with lower value did not return SUCCESS.
-LOG 3 SQLKEY:SLOOK001.2 FAIL



;*** SLOOK001.3  -  SetLookahead with Receive Checking
;*** Try a SetLookahead command with length = 256.

:SlookUpperTest1

;    Set frame size = 512h
-Bufsz 40 c0 80 80

-SetLookahead 256,SUCCESS
-JFAIL SlookUpperFail1

-LOG 3 SetLookahead with 256 returned SUCCESS.

;    Test Received lookahead length
-TRANSMITCHAIN 3 200
-JFAIL Transmit3Fail

-LOG 3 Received Lookahead length was VALID.
-LOG 3 SQLKEY:SLOOK001.3 PASS
-JUMP SlookGreaterTest1

:Transmit3Fail
-LOG 3 Received Lookahead length was INVALID.
-LOG 3 SQLKEY:SLOOK001.3 FAIL
-JUMP SlookGreaterTest1

:SlookUpperFail1
-LOG 3 SetLookahead with 256 did not return SUCCESS.
-LOG 3 SQLKEY:SLOOK001.3 FAIL



;*** SLOOK001.4  -  SetLookahead with Receive Checking
;*** Try a SetLookahead command with length > 256.

:SlookGreaterTest1

-Bufsz 40 c0 80 80 80			;NEW - if it is supported transmit a frame

-SetLookahead 512
-JPASS SlookGTRSupported1

-CheckIntNum x
-SetLookahead 512,INVALID_PARAMETER
-JFAIL SlookQueued1

-LOG 3 SetLookahead with length > 256 returned INVALID_PARAMETER
-LOG 3 SQLKEY:SLOOK001.4 PASS
-JUMP SlookClose1

:SlookQueued1
-SetLookahead 512,SUCCESS
-JFAIL SlookGTRFail1

-LOG 3 Disregard Error above.  SetLookahead used REQUEST_QUEUED method
-JUMP SlookGTRSupported1

:SlookGTRFail1
-LOG 3 SetLookahead with length > 256 did not return SUCCESS
-LOG 3						  or INVALID_PARAMETER.
-LOG 3 SQLKEY:SLOOK001.4 FAIL
-JUMP SlookClose1

:SlookGTRSupported1

-LOG 3 SetLookahead with length > 256 returned SUCCESS.

;    Test Received lookahead length
-CheckIntNum x
-TRANSMITCHAIN 3 280
-JFAIL Transmit4Fail

-LOG 3 Received Lookahead length was VALID.
-LOG 3 SQLKEY:SLOOK001.4 PASS
-JUMP SlookClose1

:Transmit4Fail
-LOG 3 Received Lookahead length was INVALID.
-LOG 3 SQLKEY:SLOOK001.4 FAIL


;*** SLOOK001.5  -  SetLookahead preserved during a CloseAdapter
;*** Make sure Lookahead value is preserved after a CloseAdapter request.

:SlookClose1


;    Is CloseAdapter supported?  If not, cannot test
-CheckMACTable SC 40 m 00 08
-JFAIL CloseNotSupp
-CloseAdapter,SUCCESS
-JPASS CloseCheck1

-LOG 3 CloseAdapter Failed.  Exiting Test.
-JUMP abort

:CloseCheck1
-LOG 3 Close was Successful. Check received lookahead value.
-OpenAdapter,SUCCESS
-JPASS CloseCont

-LOG 3 OpenAdapter Failed.  Exiting Test.
-JUMP abort

:CloseCont
-TRANSMITCHAIN 3 280
-JFAIL Transmit5Fail

-LOG 3 Received Lookahead length was VALID.
-LOG 3 SQLKEY:SLOOK001.5 PASS
-JUMP SlookReset1

:Transmit5Fail
-LOG 3 Received Lookahead length was INVALID.
-LOG 3 SQLKEY:SLOOK001.5 FAIL
-JUMP SlookReset1

:CloseNotSupp
-LOG 3 CloseAdapter Not Supported.  Cannot test perservation.
-LOG 3 SQLKEY:SLOOK001.5 PASS



;*** SLOOK001.6  -  SetLookahead preserved during a Reset
;*** Make sure Lookahead value is preserved after a Reset request.

:SlookReset1


;    Is ResetMac supported?  If not, cannot test
-CheckMACTable SC 40 m 00 04
-JFAIL ResetNotSupp
-ResetMac,SUCCESS
-JPASS ResetCheck1

-LOG 3 Reset Failed. Jump to End.
-LOG 3 SQLKEY:SLOOK001.6 FAIL
-JUMP SlookEnd1

:ResetCheck1
-LOG 3 Reset was Successful. Check received lookahead value.
-TRANSMITCHAIN 3 280
-JFAIL Transmit6Fail

-LOG 3 Received Lookahead length was VALID.
-LOG 3 SQLKEY:SLOOK001.6 PASS
-JUMP SlookEnd1

:Transmit6Fail
-LOG 3 Received Lookahead length was INVALID.
-LOG 3 SQLKEY:SLOOK001.6 FAIL
-JUMP SlookEnd1

:ResetNotSupp
-LOG 3 ResetMac Not Supported.	Cannot test perservation.
-LOG 3 SQLKEY:SLOOK001.6 PASS


;*** FINISHED  -  SetLookahead with Receive Checking

:SlookEnd1

-Checkintnum x

;    Is loopback supported? If not turn off slave's echo mode
-CheckMACTable SC 40 m 80
-JPASS LoopBackSup
;Turn slave's echo mode OFF
-sstate 0		

:LoopBackSup
-Filter 0
-JUMP SlookFinished





;***
;*** TEST ONLY SETLOOKAHEAD COMMANDS.
;***

:SlookOnly


;    Is OpenAdapter supported?	If so, issue an open
-CheckMACTable SC 40 m 00 08
-JFAIL SlookValid2
-OpenAdapter,SUCCESS
-JPASS SlookValid2

-LOG 3 OpenAdapter Failed.  Exiting Test
-JUMP abort

;*** SLOOK001.7  -  SetLookahead Only
;*** Try a SetLookahead command with a valid length.  If this fails, assume
;    the SetLookahead command for this driver does not work.

:SlookValid2

-SetLookahead 128,SUCCESS
-JPASS SlookCont1

-LOG 3 SetLookahead with valid length did not return SUCCESS.
-LOG 3 SetLookahead cannot be tested
-LOG 3 SQLKEY:SLOOK001.7 FAIL
-JUMP SlookFinished

:SlookCont1
-LOG 3 SetLookahead with valid length returned SUCCESS.
-LOG 3 SQLKEY:SLOOK001.7 PASS



;*** SLOOK001.8  -  SetLookahead Only
;*** Try a SetLookahead command with length lower than first Setlookahead (128).

-SetLookahead 64,SUCCESS
-JFAIL SlookLowerFail2

-LOG 3 SetLookahead with lower value returned SUCCESS.
-LOG 3 SQLKEY:SLOOK001.8 PASS
-JUMP SlookUpperTest2

:SlookLowerFail2
-LOG 3 SetLookahead with lower value did not return SUCCESS.
-LOG 3 SQLKEY:SLOOK001.8 FAIL



;*** SLOOK001.9  -  SetLookahead Only
;*** Try a SetLookahead command with length = 256.

:SlookUpperTest2

-SetLookahead 256,SUCCESS
-JFAIL SlookUpperFail2

-LOG 3 SetLookahead with 256 returned SUCCESS.
-LOG 3 SQLKEY:SLOOK001.9 PASS
-JUMP SlookGreaterTest2

:SlookUpperFail2
-LOG 3 SetLookahead with 256 did not return SUCCESS.
-LOG 3 SQLKEY:SLOOK001.9 FAIL


;*** SLOOK001.10  -  SetLookahead Only
;*** Try a SetLookahead command with length > 256.

:SlookGreaterTest2

-SetLookahead 512
-JPASS SlookGTRSupported2

-CheckIntNum x
-SetLookahead 512,INVALID_PARAMETER
-JFAIL SlookQueued2

-LOG 3 SetLookahead with length > 256 returned INVALID_PARAMETER
-LOG 3 SQLKEY:SLOOK001.10 PASS
-JUMP SlookEnd2

:SlookQueued2
-SetLookahead 512,SUCCESS
-JFAIL SlookGTRFail2

-LOG 3 Disregard Error above.  SetLookahead used REQUEST_QUEUED method.
-JUMP SlookGTRSupported2

:SlookGTRFail2
-LOG 3 SetLookahead with length > 256 did not return SUCCESS
-LOG 3						  or INVALID_PARAMETER.
-LOG 3 SQLKEY:SLOOK001.10 FAIL
-JUMP SlookEnd2

:SlookGTRSupported2
-LOG 3 SetLookahead with length > 256 returned SUCCESS.
-LOG 3 SQLKEY:SLOOK001.10 PASS

;*** FINISHED  -  SetLookahead Only

:SlookEnd2

-Checkintnum x
-JUMP SlookFinished


:abort
-LOG 3 Aborting Slook001 because state of MAC is uncertain
-SCRIPT ABORT ABORT
-SetFailure
-JUMP SlookFinished2


;***
;***TEST CASE CLEAN UP
;***

:SlookFinished

;   If Open was issued, issue a close.
-CheckMACTable SC 40 m 00 08
-JFAIL SlookFinished2
-CloseAdapter,SUCCESS
-JPASS SlookFinished2

-LOG 3 Aborting Slook001 because state of MAC is uncertain
-SCRIPT ABORT ABORT

:SlookFinished2
-Bufsz 40




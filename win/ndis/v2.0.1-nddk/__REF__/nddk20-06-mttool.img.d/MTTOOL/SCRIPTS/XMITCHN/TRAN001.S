;*******************************************************************************
;*
;*   File: TRAN001.S - Script file to test the transmitting capabilities
;*		       of the driver.
;*
;*   Description: Verify Transmitter operation under normal conditions.
;*		  Tests include:
;*		     - Transmit with ReqHandle = value
;*		     - Transmit with no immediate data
;*		     - Transmit with immediate data of 1 byte
;*		     - Transmit with immediate data of 20 bytes (decimal)
;*		     - Transmit with immediate data of 64 bytes (decimal)
;*		     - Transmit with 0 Data Blocks
;*		     - Transmit with 1 Data Block
;*		     - Transmit with 8 Data Blocks
;*		     - Transmit with 9 Data Blocks, expect INVALID_PARAMETER
;*		     - Transmit and expect to receive a return code of
;*		       NO_SUCH_DESTINATION if it is supported. (MUST BE TRAN)
;*		     - Transmit with no data and expect INVALID_PARAMETER
;*		       (MUST BE TRAN)
;*		     - Transmit with immediate data > 64 and expect
;*		       INVALID_PARAMETER   (MUST BE TRAN)
;*		     - Transmit when closed if supported and expect
;*		       INVALID_FUNCTION    (MUST BE TRAN)
;*
;*   Date: 2/01/90
;*
;*   Pre-conditions: Adapter is closed.
;*		     ReqHandle mode is OFF
;*
;*   Post-conditions: Adapter is closed.
;*		      Txbuf size will be reset to 64 bytes.
;*		      Turn ReqHandle mode OFF
;*
;*   Changes:	      06-06-90, Carol F.,
;*		      - Took out GDT, Register Preservation, and Handle = 0
;*		      - Added a test for 9 Data Blocks, expect INVALID_PARAMETER
;*		      - Uncommented the NO_SUCH_DESTINATION test
;*		      - Changed Destination address to Broadcast address to avoid
;*			getting a NO_SUCH_DESTINATION for all tranmits.
;*		      - Changed transmits checking return code from a
;*			TRANSMITCHAIN command to a TRAN command.  The
;*			TRANSMITCHAIN command is a script command which does not
;*			check return codes.  TRAN is a Mttool command which
;*			passes a return code.
;*
;*******************************************************************************

;***
;*** TEST CASE SETUP
;***

;    Set frame size = 128,  Set filters such that no frames are received
-Bufsz 40 40
-Filter 0
-CheckIntNum x

;    Test for OpenAdatper support.
-CheckMACTable SC 40 m 00 08
-JFAIL CheckMACType
-OpenAdapter,SUCCESS
-JPASS CheckMACType

;Quit script because state of MAC is uncertain.
-LOG 3 OpenAdapter failed.  Exiting test.
-JUMP abort

:CheckMACType
;    Is the driver a token ring driver?
-CheckMACTable SC 2 e 38 30 32 2E 35
-JPASS TokenRingType1


;    The driver is an ETHERNET type

-Buf i FF FF FF FF FF FF
-JUMP ValidReqTest

;    The driver is a TOKEN RING type

:TokenRingType1
-Buf i 00 40 FF FF FF FF FF FF




;***
;*** MAIN BODY OF TEST CASE
;***


:ValidReqTest

;*** TRAN001.1	-  ReqHandle = valid value
;*** Transmit with a valid ReqHandle and expect a TransmitConfirm Indication

-TransmitChain 1	   ;Expect only a transmitconfirm indication
-JFAIL ValidReqFail

-LOG 3 TransmitChain with valid ReqHandle value PASSED.
-LOG 3 SQLKEY:TRAN001.1 PASS
-JUMP NoImmedTest

:ValidReqFail
-LOG 3 TransmitChain with valid ReqHandle value FAILED
-LOG 3 SQLKEY:TRAN001.1 FAIL




:NoImmedTest

;*** TRAN001.2	-  No Immediate Data
;*** Transmit with No Immdediate Data and expect a TransmitConfirm Indication

-Bufsz 0 40
-TransmitChain 1
-JFAIL NoImmedFail

-LOG 3 TransmitChain with No Immediate Data  PASSED.
-LOG 3 SQLKEY:TRAN001.2 PASS
-JUMP Immed1Test

:NoImmedFail
-LOG 3 TransmitChain with No Immediate Data  FAILED.
-LOG 3 SQLKEY:TRAN001.2 FAIL



:Immed1Test

;*** TRAN001.3	-  Immediate Data is 1 byte in length
;*** Transmit with Immdediate Data = 1 byte and expect a
;    TransmitConfirm Indication

-Bufsz 1 3F
-TransmitChain 1
-JFAIL Immed1Fail

-LOG 3 TransmitChain Immediate Data = 1 byte  PASSED.
-LOG 3 SQLKEY:TRAN001.3 PASS
-JUMP Immed20Test

:Immed1Fail
-LOG 3 TransmitChain Immediate Data = 1 byte  FAILED.
-LOG 3 SQLKEY:TRAN001.3 FAIL



:Immed20Test

;*** TRAN001.4	-  Immediate Data is 20 bytes in length
;*** Transmit with Immdediate Data = 20 bytes and expect a
;    TransmitConfirm Indication

-Bufsz 14
-TransmitChain 1
-JFAIL Immed20Fail

-LOG 3 TransmitChain Immediate Data = 20 bytes	PASSED.
-LOG 3 SQLKEY:TRAN001.4 PASS
-JUMP Immed64Test

:Immed20Fail
-LOG 3 TransmitChain Immediate Data = 20 bytes	FAILED.
-LOG 3 SQLKEY:TRAN001.4 FAIL



:Immed64Test

;*** TRAN001.5	-  Immediate Data is 64 bytes in length
;*** Transmit with Immdediate Data = 64 bytes and expect a
;    TransmitConfirm Indication

-Bufsz 40
-TransmitChain 1
-JFAIL Immed64Fail

-LOG 3 TransmitChain Immediate Data = 64 bytes	PASSED.
-LOG 3 SQLKEY:TRAN001.5 PASS
-JUMP Data0Test

:Immed64Fail
-LOG 3 TransmitChain Immediate Data = 64 bytes	FAILED.
-LOG 3 SQLKEY:TRAN001.5 FAIL


:Data0Test

;*** TRAN001.6	-  0 Data Blocks
;*** Transmit with no Data Blocks and expect a TransmitConfirm Indication

-Bufsz 40
-TransmitChain 1
-JFAIL Data0Fail

-LOG 3 TransmitChain 0 Data Blocks  PASSED.
-LOG 3 SQLKEY:TRAN001.6 PASS
-JUMP Data1Test

:Data0Fail
-LOG 3 TransmitChain 0 Data Blocks  FAILED.
-LOG 3 SQLKEY:TRAN001.6 FAIL



:Data1Test

;*** TRAN001.7	-  1 Data Block
;*** Transmit with 1 Data Block and expect a TransmitConfirm Indication

-Bufsz 40 40
-TransmitChain 1
-JFAIL Data1Fail

-LOG 3 TransmitChain 1 Data Block  PASSED.
-LOG 3 SQLKEY:TRAN001.7 PASS
-JUMP Data8Test

:Data1Fail
-LOG 3 TransmitChain 1 Data Block  FAILED.
-LOG 3 SQLKEY:TRAN001.7 FAIL



:Data8Test

;*** TRAN001.8	-  8 Data Blocks
;*** Transmit with 8 Data Blocks and expect a TransmitConfirm Indication

-Bufsz 40 20 20 20 20 20 20 20 20
-TransmitChain 1
-JFAIL Data8Fail

-LOG 3 TransmitChain 8 Data Blocks  PASSED.
-LOG 3 SQLKEY:TRAN001.8 PASS
-JUMP Data9Test

:Data8Fail
-LOG 3 TransmitChain 8 Data Blocks  FAILED.
-LOG 3 SQLKEY:TRAN001.8 FAIL



:Data9Test

;*** TRAN001.9	-  9 Data Blocks
;*** Transmit with 9 Data Blocks and expect INVALID_PARAMETER

-Bufsz 40 20 20 20 20 20 20 20 20 20
-Tran,INVALID_PARAMETER 		;MUST BE TRAN
-JFAIL Data9Fail

-LOG 3 TransmitChain 9 Data Blocks  PASSED.
-LOG 3 SQLKEY:TRAN001.9 PASS
-JUMP NoSuchDes

:Data9Fail
-LOG 3 TransmitChain 9 Data Blocks  FAILED.
-LOG 3 SQLKEY:TRAN001.9 FAIL



:NoSuchDes

;*** TRAN001.10  -  NO_SUCH_DESTINATION return code
;*** Transmit to a non-existent destination and receive a NO_SUCH_DESTINATION

-Bufsz 40

;    Is the driver a token ring driver?
-CheckMACTable SC 2 e 38 30 32 2E 35
-JFAIL NotSupported

;    Driver is Token Ring

-Buf i 00 40 44 44 55 55 77 77
-Tran,NO_SUCH_DESTINATION		    ;MUST BE TRAN
-JFAIL NoSuchDesFail

-LOG 3 TransmitChain to non-existent des. returned NO_SUCH_DESTINATION.
-LOG 3 SQLKEY:TRAN001.10 PASS
-Buf i 00 40 FF FF FF FF FF FF
-JUMP InvalidParm1

:NoSuchDesFail
-LOG 3 Tran to non-existent des. did not return NO_SUCH_DESTINATION.
-LOG 3 SQLKEY:TRAN001.10 FAIL
-Buf i 00 40 FF FF FF FF FF FF
-JUMP InvalidParm1

:NotSupported
-LOG 3 Driver does not support NO_SUCH_DESTINATION return code
-LOG 3 SQLKEY:TRAN001.10 PASS



:InvalidParm1

;*** TRAN001.11  -  INVALID_PARAMETER return code
;*** Transmit a 0 byte frame and expect INVALID_PARAMETER

-Bufsz 0
-Tran,INVALID_PARAMETER 	;MUST BE TRAN
-JFAIL InvalidParm1Fail

-LOG 3 TransmitChain with 0 byte frame returned INVALID_PARAMETER.
-LOG 3 SQLKEY:TRAN001.11 PASS
-JUMP InvalidParm2

:InvalidParm1Fail
-LOG 3 TransmitChain with 0 byte frame did not return INVALID_PARAMETER.
-LOG 3 SQLKEY:TRAN001.11 FAIL



:InvalidParm2

;*** TRAN001.12  -  INVALID_PARAMETER return code
;*** Transmit immediate data > 64 and expect INVALID_PARAMETER

-Bufsz 45 40
-Tran,INVALID_PARAMETER 	;MUST BE TRAN
-JFAIL InvalidParm2Fail

-LOG 3 TransmitChain with immediate data > 64 returned INVALID_PARAMETER.
-LOG 3 SQLKEY:TRAN001.12 PASS
-JUMP InvalidFunction

:InvalidParm2Fail
-LOG 3 TransmitChain with immed data > 64 did not return INVALID_PARAMETER.
-LOG 3 SQLKEY:TRAN001.12 FAIL



:InvalidFunction

;*** TRAN001.13  -  INVALID_FUNCTION return code
;*** Transmit when adapter is closed and expect INVALID_FUNCTION

-Bufsz 40 40

;    Does the driver support close?
-CheckMACTable SC 40 m 00 08
-JFAIL CloseNotSup

-CloseAdapter,SUCCESS
-JPASS CheckInvalid

;Quit because state of MAC is uncertain
-LOG 3 CloseAdapter failed.  Exiting test.
-JUMP abort

:CheckInvalid
-Tran,INVALID_FUNCTION	    ;MUST BE TRAN
-JFAIL InvalidFunFail

-LOG 3 TransmitChain when closed returned INVALID_FUNCTION.
-LOG 3 SQLKEY:TRAN001.13 PASS
-JUMP Finished

:InvalidFunFail
-LOG 3 TransmitChain when closed did not return INVALID_FUNCTION.
-LOG 3 SQLKEY:TRAN001.13 FAIL
-JUMP Finished

:CloseNotSup
-LOG 3 INVALID_FUNCTION cannot be tested.  CloseAdapter is not supported.
-LOG 3 SQLKEY:TRAN001.13 PASS
-JUMP Finished


:abort
-LOG 3 Aborting TRAN001 because state of MAC is uncertain
-SCRIPT ABORT ABORT
-SetFailure


;*** FINISHED  -  TransmitChain

:Finished

;***
;***TEST CASE CLEAN UP
;***

-Bufsz 40


;*******************************************************************************
;*
;*   File: RESET001.S - Script file to test the ResetMAC primitive.
;*
;*   Description: Verify ResetMAC operation under normal conditions.
;*		  Tests include:
;*		     - If ResetMAC is not supported, Test for return code
;*		       of NOT SUPPORTED and then exit the test case.
;*		       If Reset is supported, execute the following test cases.
;*		     - Issue ResetMAC command from closed state and verify it
;*		       succeeds.  If SetStationAddress is supported, issue
;*		       one before the ResetMAC and after the reset, confirm
;*		       that the SetStationAddress still exists.  Make sure
;*		       the ResetMAC did not open a closed adapter by issuing
;*		       an open adapter.  If a station address was set, transmit
;*		       a frame to it and verify it receives.
;*		     - Issue ResetMAC command from an open state and verify it
;*		       succeeds.  Make sure the ResetMAC command left the
;*		       adapter in an open state by transmitting and frame to
;*		       a non-existent destination and expect only a
;*		       transmitconfirm indication
;*		     - Issue a SetPacketFilter command and then issue a ResetMAC.
;*		       The ResetMAC should succeed and the filter's value should
;*		       be preserved.
;*		     - Issue a SetFunctionalAddress command and then issue a
;*		       ResetMAC. The ResetMAC should succeed and the Functional
;*		       Address should be preserved. (This test is Only valid
;*		       for Token Ring adapters.)
;*		     - Issue a AddMulticastAddress command and then issue a
;*		       ResetMAC. The ResetMAC should succeed and the Multicast
;*		       Address should be preserved.
;*		     - Issue multiple AddMulticastAddress commands and then
;*		       issue a ResetMAC. The ResetMAC should succeed and the
;*		       Multicast Addresses should be preserved.  (This test is
;*		       only valid for ethernet.)
;*		     - Verify ResetMAC turns Indications ON
;*		     - Verify Start and End status indications occur after a
;*		       ResetMAC. (Note, End status indications are only
;*		       generated for drivers 2.0 and up). Also, if the driver
;*		       is Token Ring, another station needs to be opened on the
;*		       network.  Otherwise, RingStatus indications could be
;*		       received and the interrupt count will be off.
;*		     - Verify if a transmitchain command is issued during a
;*		       reset, it will fail with INVALID_FUNCTION.
;*
;*   Date: 2/02/90
;*
;*   Pre-conditions: Adapter is closed.
;*		     Another system is in Slave Mode.
;*
;*   Post-conditions: Adapter is closed.
;*		      Filter will be reset to 0.
;*		      TxBuf is reset to 40.
;*
;*   Changes:	      06-06-90, Carol F.,
;*		      - Removed GDT, Register Preservation, and Handle = 0.
;*		      - Removed Setlookahead checking and added it to the
;*			SetLookAhead script.
;*		      - Changed transmitting and receiving frames through
;*			sstate 1 to MRECV's.
;*		      - Uncommented Indication checking (Start and End Reset)
;*		      - Corrected Multicast addresses for Ethernet.
;*		      - Corrected checking for INVALID_FUNCTION when transmitting
;*			During a Reset.  If the Reset returned a SUCCESS
;*			Immediately (Not Request Queued Method), the test should
;*			skip around the transmit because the Reset is probably
;*			already through.
;*
;*******************************************************************************

;    The following open/close insures we bind with the tool
;    Does the driver support OpenAdapter?
-CheckMACTable SC 40 m 00 08
-JFAIL OANotSupp
-OpenAdapter,SUCCESS
-JFAIL Trouble
-CloseAdapter,SUCCESS
-JPASS OANotSupp

:Trouble
-LOG 3 This MAC supports open/close adapter and one or more of these
-LOG 3 operations failed?  We had better bail out of the test then
-JUMP Abort

:OANotSupp


;***  IS RESETMAC SUPPORTED?

-CheckMACTable SC 40 m 00 04
-JFAIL ResetNotSup

-LOG 3 ResetMAC is Supported.


;***
;***   RESETMAC SUPPORTED
;***


:ResetCloseTest

;*** Reset001.1 - ResetMAC when closed and preservation of SetStationAddress
;*** Issue a setstationaddress, then issue a ResetMAC.	Check for SUCCESS and
;    that the SetstationAddress is preserved.  Also, make sure the reset did
;    not open a closed adapter by issuing an OpenAdapter.  If a station address
;    was set, make sure a frame can be received to this address.

;    Is SetStationAddress supported?
-CheckMACTable SC 40 m 10
-JFAIL SetStnNotSup


;    Driver supports SetStationAddress

-LOG 3 Setting Station Address = 44 55 66 44 55 66.

-SetStationAddress 44 55 66 44 55 66,SUCCESS
-JFAIL SetStationFail

;    Is the station address in the Mac characteristics table?
-CheckMACTable SC 24 e 44 55 66 44 55 66
-JFAIL SetStnNotSet


:IssueReset1
-ResetMAC,SUCCESS
-JFAIL ResetCloseFail

-LOG 3 ResetMAC returned SUCCESS.

;    Is the station address still in the Mac characteristics table?
-CheckMACTable SC 24 e 44 55 66 44 55 66
-JFAIL ResetCloseFail2

-LOG 3 Station address set was preserved.

;    Does the driver support OpenAdapter?
-CheckMACTable SC 40 m 00 08
-JFAIL RCVStnAddr
-OpenAdapter,SUCCESS
-JFAIL OpenedIt

:RCVStnAddr
-Bufsz 40
-SetPacketFilter 3
-IDSLAVE,SUCCESS		;Identify slave
-JPASS RcvCheck

-LOG 3 Unable to identify slave computer.  Exiting script.
-JUMP Abort

:RcvCheck
-CheckIntNum X
-MRECV 1 100 100 0 0 0,SUCCESS
-CheckIntNum 1
-JFAIL RCVStnAddrFail

-LOG 3 Frame was received on Station address.
-LOG 3 SQLKEY:RESET001.1 PASS
-JUMP ResetOpenTest

:RCVStnAddrFail
-LOG 3 Frame was not received on Station address.
-LOG 3 SQLKEY:RESET001.1 FAIL
-JUMP ResetOpenTest


:ResetCloseFail2
-LOG 3 Station address set was not preserved.
-LOG 3 SQLKEY:RESET001.1 FAIL
-JUMP OpenIt


; *** Set station not supported or not working

:SetStnNotSet
-LOG 3 Set Station Address was not reflected in stats, Only do ResetMAC.
-JUMP SetStnNotSup

:SetStationFail
-LOG 3 Set Station Address Failed, Only do ResetMAC.

:SetStnNotSup
-ResetMAC,SUCCESS
-JFAIL ResetCloseFail

-LOG 3 ResetMAC returned SUCCESS.

-CheckMACTable SC 40 m 00 08
-JFAIL AlreadyOpen
-OpenAdapter,SUCCESS
-JFAIL OpenedIt

:AlreadyOpen
-LOG 3 SQLKEY:RESET001.1 PASS
-JUMP ResetOpenTest

:OpenedIt
-LOG 3 ResetMAC opened a closed Adapter.
-LOG 3 SQLKEY:RESET001.1 FAIL
-JUMP ResetOpenTest

:ResetCloseFail
-LOG 3 ResetMAC did not return SUCCESS.
-LOG 3 SQLKEY:RESET001.1 FAIL

:OpenIt
-CheckMACTable SC 40 m 00 08
-JFAIL ResetOpenTest
-OpenAdapter,SUCCESS


:ResetOpenTest

;*** Reset001.2 - ResetMAC when adapter is open.
;*** Issue a ResetMAC and check for SUCCESS.  Make sure the reset left the
;    adapter in an open state by transmitting a frame to an non-existent
;    destination and only expecting a transmitconfirm indication.

-SetPacketFilter 3
-IDSLAVE,SUCCESS		;Identify slave
-JPASS Continue

-LOG 3 Unable to identify slave computer.  Exiting script.
-JUMP Abort

:Continue
-ResetMAC,SUCCESS
-JFAIL ResetOpenFail

-LOG 3 ResetMAC returned SUCCESS.

:IssueTran2
-Filter 3
-CheckIntNum X
-MRECV 1 100 100 0 0 0,SUCCESS
-CheckIntNum 1
-JFAIL ResetOpenFail2

-LOG 3 Adapter was left in an open state.
-LOG 3 SQLKEY:RESET001.2 PASS
-JUMP ResetFilterTest

:ResetOpenFail2
-LOG 3 Adapter was not left in an open state.
-LOG 3 SQLKEY:RESET001.2 FAIL
-JUMP ResetFilterTest

:ResetOpenFail
-LOG 3 ResetMAC did not return SUCCESS.
-LOG 3 SQLKEY:RESET001.2 FAIL




:ResetFilterTest

;*** Reset001.3 - Check filter preservation during a reset
;*** Issue a SetPacketFilter and then issue a ResetMAC command.  The ResetMAC
;    should succeed and the filter should be preserved.


-SetPacketFilter 3,SUCCESS
-CheckMACTable SS a e 03 00
-JFAIL SetFilterFail

-ResetMAC,SUCCESS
-JFAIL ResetFilterFail

-LOG 3 ResetMAC returned SUCCESS.

-CheckMACTable SS a e 03 00
-JFAIL ResetFilterFail2

-LOG 3 Filter was preserved.
-LOG 3 SQLKEY:RESET001.3 PASS
-JUMP ResetFunTest

:ResetFilterFail2
-LOG 3 Filter was not preserved.
-LOG 3 SQLKEY:RESET001.3 FAIL
-JUMP ResetFunTest

:ResetFilterFail
-LOG 3 ResetMAC did not return SUCCESS.
-LOG 3 SQLKEY:RESET001.3 FAIL
-JUMP ResetFunTest

:SetFilterFail
-LOG 3 Filter was not reflected in the stats.  Preservation cannot be tested
-LOG 3 SQLKEY:RESET001.3 FAIL


:ResetFunTest

;*** Reset001.4 - Check Functional Address preservation during a reset
;*** Issue a SetFunctionalAddress and then issue a ResetMAC command.  The
;    ResetMAC should succeed.  The preservation of the Functional Address should
;    be confirmed by transmitting and receiving a frame to the functional
;    address.

;    Is SetFunctionalAddress supported?
-CheckMACTable SC 40 m 04
-JFAIL SetFunNotSup


-LOG 3 Setting Functional Address = 01 02 03 04.

-SetFunctionalAddress 01 02 03 04,SUCCESS
-JFAIL ResetFunFail

;    Is the Functional address in the stats?
-CheckMACTable SC 34 e 01 02 03 04
-JFAIL ResetFunFail2

-ResetMAC,SUCCESS
-JFAIL ResetFunFail3

-LOG 3 ResetMAC returned SUCCESS.

;    Is the Fucntional address in the stats?
-CheckMACTable SC 34 e 01 02 03 04
-JFAIL ResetFunFail4
-LOG 3 Functional Address was preserved in the stats.

-SetPacketFilter 3

-CheckIntNum X
-MRECV 1 100 100 0 0 2 C0 00 00 02 00 00,SUCCESS
-CheckIntNum 1
-JFAIL ResetFunFail5

-LOG 3 Frame received.	Functional Address was preserved.
-LOG 3 SQLKEY:RESET001.4 PASS
-JUMP ResetFunClean

:ResetFunFail5
-LOG 3 Functional Address not preserved or frame was not received.
-JUMP Failure5

:ResetFunFail4
-LOG 3 Functional Address not preserved in the stats.
-JUMP Failure5

:ResetFunFail3
-LOG 3 ResetMAC did not return SUCCESS.
-JUMP Failure5

:ResetFunFail2
-LOG 3 Functional Address was not updated in the stats.
-JUMP Failure5

:ResetFunFail
-LOG 3 Failure setting functional address

:Failure5
-LOG 3 SQLKEY:RESET001.4 FAIL

:ResetFunClean
-SetFunctionalAddress 00 00 00 00
-JUMP ResetMCTest


;*** Set Functional Address not supported
:SetFunNotSup
-LOG 3 SetFunctionalAddr is not supported.  Perservation cannot be checked.
-LOG 3 SQLKEY:RESET001.4 PASS




:ResetMCTest

;*** Reset001.5 - Check Multicast Address preservation during a reset
;*** Issue a AddMulticastAddress and then issue a ResetMAC command.  The
;    ResetMAC should succeed.  The preservation of the Multicast Address should
;    be confirmed by transmitting and receiving a frame to the multicast
;    address.

;    Is AddMulticastAddress supported?	Check for either Multicast bit or
;    functional/group bit being set.
-CheckMACTable SC 40 m 06
-JFAIL SetMCNotSup

;    Is the driver a token ring driver?
-CheckMACTable SC 2 e 38 30 32 2E 35
-JPASS TokenRingType6


;***  ETHERNET

-LOG 3 Setting MulticastAddress = 01 bb cc dd ee ff.

-AddMulticastAddress 01 bb cc dd ee ff,SUCCESS
-JFAIL SetMCFail

;    Is the Multicast address in the stats?
-CheckMACTable MC 2 e 01 00,SUCCESS
-JFAIL ResetMCFail2
-CheckMACTable MC 4 e 01 bb cc dd ee ff
-JFAIL ResetMCFail2

-ResetMAC,SUCCESS
-JFAIL ResetMCFail3

-LOG 3 ResetMAC returned SUCCESS.

;    Is the Multicast address in the stats?
-CheckMACTable MC 2 e 01 00,SUCCESS
-JFAIL ResetMCFail4
-CheckMACTable MC 4 e 01 bb cc dd ee ff
-JFAIL ResetMCFail4

-JUMP IssueTran6



;*** TOKEN RING


;    The driver is a TOKEN RING type

:TokenRingType6

-LOG 3 Setting MulticastAddress = 80 00 01 02 03 04.

-AddMulticastAddress 80 00 01 02 03 04,SUCCESS
-JFAIL SetMCFail

;    Is the Multicast address in the stats?
-CheckMACTable MC 2 e 01 00,SUCCESS
-JFAIL ResetMCFail2
-CheckMACTable MC 4 e 80 00 01 02 03 04
-JFAIL ResetMCFail2


-ResetMAC,SUCCESS
-JFAIL ResetMCFail3

-LOG 3 ResetMAC returned SUCCESS.

;    Is the Multicast address in the stats?
-CheckMACTable MC 2 e 01 00,SUCCESS
-JFAIL ResetMCFail4
-CheckMACTable MC 4 e 80 00 01 02 03 04
-JFAIL ResetMCFail4


;*** Issue transmit for both Ethernet types and token ring types
:IssueTran6

-LOG 3 Multicast Address was preserved in the stats.
-SetPacketFilter 3

-CheckIntNum X
;    Is the driver a token ring driver?
-CheckMACTable SC 2 e 38 30 32 2E 35
-JPASS TranTokenRing
-MRECV 1 100 100 0 0 2 01 bb cc dd ee ff,SUCCESS
-CheckIntNum 1
-JFAIL ResetMCFail5
-JUMP TranPass

:TranTokenRing
-MRECV 1 100 100 0 0 2 C0 00 81 02 03 04,SUCCESS
-CheckIntNum 1
-JFAIL ResetMCFail5

:TranPass
-LOG 3 Frame received.	 Multicast Address was preserved.
-LOG 3 SQLKEY:RESET001.5 PASS
-JUMP ResetMCClean

:ResetMCFail5
-LOG 3 Multicast Address not preserved or frame was not received.
-JUMP Failure6

:ResetMCFail4
-LOG 3 Multicast Address not preserved in the stats.
-JUMP Failure6

:ResetMCFail3
-LOG 3 ResetMAC did not return SUCCESS.
-JUMP Failure6

:ResetMCFail2
-LOG 3 Multicast Address was not updated in the stats correctly.

:Failure6
-LOG 3 SQLKEY:RESET001.5 FAIL
-JUMP ResetMCClean

;*** Set Multicast Address Failed
:SetMCFail
-LOG 3 AddMulticastAddr Failed.   Preservation cannot be checked.
-LOG 3 SQLKEY:RESET001.5 FAIL

:ResetMCClean
;    Is the driver a token ring driver?
-CheckMACTable SC 2 e 38 30 32 2E 35
-JPASS TokenRingType6b

;*** Ethernet
-DelMulticastAddress 01 bb cc dd ee ff
-JUMP ResetMulMCTest

;*** Token Ring
:TokenRingType6b
-DelMulticastAddress 80 00 01 02 03 04
-JUMP ResetMulMCTest


;*** Multicast Addressing not supported
:SetMCNotSup
-LOG 3 AddMulticastAddr is not supported.  Perservation cannot be checked.
-LOG 3 SQLKEY:RESET001.5 PASS




:ResetMulMCTest

;*** Reset001.6 - Check Multiple Multicast Address preservations during a reset
;*** Issue 2 AddMulticastAddress commands and then issue a ResetMAC command.
;    The ResetMAC should succeed.  The preservation of the Multicast Addresses
;    should be confirmed by transmitting and receiving a frame to the addesses.

;    Is AddMulticastAddress supported?
-CheckMACTable SC 40 m 06
-JFAIL SetMulMCNotSup

-CheckMACTable MC 0 e 00 00
-JPASS MulMCNotSup2
-CheckMACTable MC 0 e 01 00
-JPASS MulMCNotSup2

;***  Multiple multicasts are supported

;    Is the driver a token ring driver?  If not, must be Ethernet
-CheckMACTable SC 2 e 38 30 32 2E 35
-JFAIL EtherType

;Token Ring type

-LOG 3 Setting MulticastAddress1 = 80 00 0a ff ee dd.
-AddMulticastAddress 80 00 0a ff ee dd,SUCCESS
-JFAIL AddFail7a
-LOG 3 Setting MulticastAddress2 = 80 00 0d cc ff ee.
-AddMulticastAddress 80 00 0d cc ff ee,SUCCESS
-JPASS CheckMulMC1

-DelMulticastAddress 80 00 0d cc ff ee,SUCCESS
:AddFail7a
-DelMulticastAddress 80 00 0a ff ee dd,SUCCESS
-JUMP SetMulMCFail


:CheckMulMC1
;    Are the Multicast addresses in the stats?
-CheckMACTable MC 2 e 02 00,SUCCESS
-JFAIL ResetMulMCFail2
-CheckMACTable MC 4 e 80 00 0a ff ee dd
-JFAIL ResetMulMCFail2
-CheckMACTable MC 14 e 80 00 0d cc ff ee
-JFAIL ResetMulMCFail2


-ResetMAC,SUCCESS
-JFAIL ResetMulMCFail3

-LOG 3 ResetMAC returned SUCCESS.

;    Are the Multicast addresses in the stats?
-CheckMACTable MC 2 e 02 00,SUCCESS
-JFAIL ResetMulMCFail4
-CheckMACTable MC 4 e 80 00 0a ff ee dd
-JFAIL ResetMulMCFail4
-CheckMACTable MC 14 e 80 00 0d cc ff ee
-JFAIL ResetMulMCFail4
-LOG 3 Multicast Addresses were preserved in the stats.

;    Transmit to Multicast address  = 80 00 0a ff ee dd
-CheckIntNum X
-MRECV 1 100 100 0 0 2 C0 00 8a ff ee dd,SUCCESS
-CheckIntNum 1
-JFAIL ResetMulMCFail5

;    Transmit to Multicast address  = 01 aa dd cc ff ee
-CheckIntNum X
-MRECV 1 100 100 0 0 2 C0 00 8d cc ff ee,SUCCESS
-CheckIntNum 1
-JFAIL ResetMulMCFail5
-JUMP ResetMulPass


:EtherType

-LOG 3 Setting MulticastAddress1 = 01 bb aa ff ee dd.
-AddMulticastAddress 01 bb aa ff ee dd,SUCCESS
-JFAIL AddFail7b
-LOG 3 Setting MulticastAddress2 = 01 aa dd cc ff ee.
-AddMulticastAddress 01 aa dd cc ff ee,SUCCESS
-JPASS CheckMulMC

-DelMulticastAddress 01 aa dd cc ff ee,SUCCESS
:AddFail7b
-DelMulticastAddress 01 bb aa ff ee dd,SUCCESS
-JUMP SetMulMCFail


:CheckMulMC
;    Are the Multicast addresses in the stats?
-CheckMACTable MC 2 e 02 00,SUCCESS
-JFAIL ResetMulMCFail2
-CheckMACTable MC 4 e 01 bb aa ff ee dd
-JFAIL ResetMulMCFail2
-CheckMACTable MC 14 e 01 aa dd cc ff ee
-JFAIL ResetMulMCFail2


-ResetMAC,SUCCESS
-JFAIL ResetMulMCFail3

-LOG 3 ResetMAC returned SUCCESS.

;    Are the Multicast addresses in the stats?
-CheckMACTable MC 2 e 02 00,SUCCESS
-JFAIL ResetMulMCFail4
-CheckMACTable MC 4 e 01 bb aa ff ee dd
-JFAIL ResetMulMCFail4
-CheckMACTable MC 14 e 01 aa dd cc ff ee
-JFAIL ResetMulMCFail4
-LOG 3 Multicast Addresses were preserved in the stats.

;    Transmit to Multicast address  = 01 bb aa ff ee dd
-CheckIntNum X
-MRECV 1 100 100 0 0 2 01 bb aa ff ee dd,SUCCESS
-CheckIntNum 1
-JFAIL ResetMulMCFail5

;    Transmit to Multicast address  = 01 aa dd cc ff ee
-CheckIntNum X
-MRECV 1 100 100 0 0 2 01 aa dd cc ff ee,SUCCESS
-CheckIntNum 1
-JFAIL ResetMulMCFail5


;***  Logs for both Token Ring and Ethernet

:ResetMulPass
-LOG 3 Frames received.  Multicast Addresses were preserved.
-LOG 3 SQLKEY:RESET001.6 PASS
-JUMP ResetMulMCClean

:ResetMulMCFail5
-LOG 3 A Multicast Address was not preserved or a frame was not received.
-JUMP Failure7

:ResetMulMCFail4
-LOG 3 Multicast Addresses were not preserved in the stats.
-JUMP Failure7

:ResetMulMCFail3
-LOG 3 ResetMAC did not return SUCCESS.
-JUMP Failure7

:ResetMulMCFail2
-LOG 3 Multicast Addresses were not updated in the stats correctly.

:Failure7
-LOG 3 SQLKEY:RESET001.6 FAIL

:ResetMulMCClean
;    Is the driver a token ring driver?  If not, must be Ethernet
-CheckMACTable SC 2 e 38 30 32 2E 35
-JFAIL EtherType

-DelMulticastAddress 80 00 0d cc ff ee,SUCCESS
-DelMulticastAddress 80 00 0a ff ee dd,SUCCESS
-JUMP ResetIONTest

:EtherType
-DelMulticastAddress 01 aa dd cc ff ee,SUCCESS
-DelMulticastAddress 01 bb aa ff ee dd,SUCCESS
-JUMP ResetIONTest

;*** Set Multicast Address Failed
:SetMulMCFail
-LOG 3 AddMulticastAddr Failed.   Preservation cannot be checked.
-LOG 3 SQLKEY:RESET001.6 FAIL
-JUMP ResetIONTest

:MulMCNotSup2
-LOG 3 Multiple Multicasts are not supported.  Preservation cannot be checked.
-LOG 3 SQLKEY:RESET001.6 PASS
-JUMP ResetIONTest

;*** Multicast Addressing not supported
:SetMulMCNotSup
-LOG 3 AddMulticastAddr is not supported.  Perservation cannot be checked.
-LOG 3 SQLKEY:RESET001.6 PASS




:ResetIONTest

;*** Reset001.7 - Check that ResetMAC leaves driver with indications on.
;*** Issue an IndicationsOFF command. Then issue a ResetMAC command.  It
;    should succeed and indications should be ON.


-LOG 3 RESET001.7 assumes for TOKEN RING that there is another node inserted
-LOG 3 onto the ring.  This is necessary because of the Ring Status indication
-LOG 3 for the first station on the ring.


-IndicationOFF,SUCCESS
-JFAIL InoffFail

-ResetMAC,SUCCESS
-JFAIL ResetIONFail

-LOG 3 ResetMAC returned SUCCESS.

-Bufsz 40
-SetPacketFilter 3
-CheckIntNum X
-MRECV 1 100 100 0 0 0,SUCCESS
-CheckIntNum 1
-JFAIL ResetIONFail2

-LOG 3 Indications were turned ON.
-LOG 3 SQLKEY:RESET001.7 PASS
-JUMP ResetStatusTest

:ResetIONFail2
-LOG 3 Indications were not turned ON.
-LOG 3 SQLKEY:RESET001.7 FAIL
-IndicationOn,SUCCESS
-JUMP ResetStatusTest

:ResetIONFail
-LOG 3 ResetMAC did not return SUCCESS.
-LOG 3 SQLKEY:RESET001.7 FAIL
-IndicationOn,SUCCESS
-JUMP ResetStatusTest

:IndoffFail
-LOG 3 IndicationOff command failed.  Indications ON cannot be checked.
-LOG 3 SQLKEY:RESET001.7 FAIL


:ResetStatusTest

;*** Reset001.8 - Check that Start and End reset statuses are received.
;*** Issue a ResetMAC and check for Start and End reset status indications.

;-LOG 1 THIS TEST SECTION WILL TAKE A COUPLE OF MINUTES
-CheckIntNum x

-SetStatInd 3 5 		;NEW,	This command sets up an array in
				;Mttool. The parmeters of this command are the
				;Opcode values of the expected status
				;indications in the expected order.
-ResetMAC
-Wait 15
-ChkStatInd			;NEW,	This command checks the array set up
				;in the setstatind command with the status
				;indications that were received.
-JFAIL TestStatusFail

-LOG 3 All statuses were received.
-LOG 3 SQLKEY:RESET001.8 PASS
-JUMP ResetINVALIDTest

:TestStatusFail
-LOG 3 Number of expected Indications did not matched what was received.
-LOG 3 SQLKEY:RESET001.8 FAIL



:ResetINVALIDTest

;*** Reset001.9 - Expect a INVALID_FUNCTION return code.
;*** Issue a ResetMAC and then issue a Tran to a non-existent destination.
;    This should produce a INVALID_FUNCTION return code for the TRAN.

;*** Setup buffer to transmit
;    Is the driver a token ring driver?
-CheckMACTable SC 2 e 38 30 32 2E 35
-JPASS TokenRingType11

;    The driver is an ETHERNET type
-Buf i 44 44 55 55 77 77
-JUMP IssueReset11

;    The driver is a TOKEN RING type
:TokenRingType11
-Buf i 00 40 44 44 55 55 77 77

:IssueReset11
-ResetMAC
-JPASS NotReqQueued
-TRAN,INVALID_FUNCTION
-JFAIL ResetINVALIDFail

-LOG 3 TransmitChain returned INVALID_FUNCTION.
-LOG 3 SQLKEY:RESET001.9 PASS
-JUMP Finished1

:NotReqQueued
-LOG 3 ResetMac does not use REQUEST_QUEUED method.
-LOG 3 SQLKEY:RESET001.9 PASS
-JUMP Finished2

:ResetINVALIDFail
-LOG 3 TransmitChain did not return INVALID_FUNCTION.
-LOG 3 SQLKEY:RESET001.9 FAIL

:Finished1
-Wait 15

:Finished2
-Filter 0
-CheckIntNum X
;    Does the driver support OpenAdapter?
-CheckMACTable SC 40 m 00 08
-JFAIL Finished
-CloseAdapter,SUCCESS
-JPASS Finished

-LOG 3 CloseAdapter failed.  Exiting test.
-JUMP Abort


;***
;***   RESETMAC NOT SUPPORTED
;***

:ResetNotSup

-LOG 3 ResetMAC is Not Supported.
-ResetMAC,NOT_SUPPORTED
-JFAIL ResetNotSupFail

-LOG 3 ResetMAC returned NOT_SUPPORTED.
-LOG 3 SQLKEY:RESET001.10 PASS
-JUMP Finished

:ResetNotSupFail
-LOG 3 ResetMAC did not return NOT_SUPPORTED.
-LOG 3 SQLKEY:RESET001.10 FAIL
-JUMP Finished

:Abort
-LOG 3 Aborting ResetMAC Reset001 script
-Script abort abort
-SetFailure


:Finished


;*** FINISHED  -  ReSetMAC Test Case

-Bufsz 40


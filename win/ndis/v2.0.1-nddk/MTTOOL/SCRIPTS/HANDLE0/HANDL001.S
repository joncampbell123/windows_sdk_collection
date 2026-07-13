;*******************************************************************************
;*
;*   File: HANDL001.S - Script file to test the primitives with a zero Request
;*			Handle.
;*
;*   Description: Verify all operations under normal conditions when request
;*		  handle equals zero.
;*		  Tests include:
;*		     - Issue a SetPacketFilter request and check for SUCCESS
;*		       or REQUEST_QUEUED.  Then check that filter was set.
;*		     - Issue a TransmitChain and expect no indications.
;*		     - If Multicast/Group addresses are supported, issue a
;*		       AddMulticastAddress and check for SUCCESS or
;*		       REQUEST_QUEUED.	Then check that Multicast was added.
;*		       If not supported, check for NOT_SUPPORTED.
;*		     - If Multicast/Group addresses are supported, issue a
;*		       DelMulticastAddress and check for SUCCESS or
;*		       REQUEST_QUEUED.	Then check that Multicast was deleted.
;*		       If not supported, check for NOT_SUPPORTED.
;*		     - Issue ClearStatistics and check for SUCCESS,
;*		       REQUEST_QUEUED, or NOT_SUPPORTED
;*		     - Issue UpdateStatistics and check for SUCCESS,
;*		       REQUEST_QUEUED, or NOT_SUPPORTED
;*		     - Issue IndicationOff and check for SUCCESS or
;*		       REQUEST_QUEUED.	Issue a Transmit and do not expect an
;*		       indication.
;*		     - Issue IndicationOn and check for SUCCESS or
;*		       REQUEST_QUEUED.	Should get the Indication from above.
;*		     - Issue InitiateDiagnostics and check for SUCCESS,
;*		       REQUEST_QUEUED or NOT_SUPPORTED.
;*		     - Issue Interrupt and check for SUCCESS, REQUEST_QUEUED,
;*		       or NOT_SUPPORTED.
;*		     - If ReadErrorLog is supported, issue a ReadErrorLog request
;*		       and check for SUCCESS or REQUEST_QUEUED.  If it is not
;*		       supported, check for NOT_SUPPORTED.
;*		     - If Functional addresses are supported issue a
;*		       SetFunctionalAddress and check for a SUCCESS or
;*		       REQUEST_QUEUED.	If not supported check for NOT_SUPPORTED
;*		     - Issue a SetLookAhead request and check for SUCCESS or
;*		       REQUEST_QUEUED.
;*		     - If CloseAdapter is supported issue the request and check
;*		       for a SUCCESS or REQUEST_QUEUED. If not supported check
;*		       for NOT_SUPPORTED.
;*		     - If SetStationAddress is supported issue the request and
;*		       check for a SUCCESS or REQUEST_QUEUED.  If not supported
;*		       check for NOT_SUPPORTED.
;*		     - If OpenAdapter is supported issue the request and check
;*		       for a SUCCESS or REQUEST_QUEUED. If not supported check
;*		       for NOT_SUPPORTED.
;*		     - If ResetMAC is supported issue the request and check
;*		       for a SUCCESS or REQUEST_QUEUED. If not supported check
;*		       for NOT_SUPPORTED.
;*
;*   Date: 06/07/90
;*
;*   Pre-conditions: Adapter is closed.
;*		     Another system is in Slave Mode.
;*
;*   Post-conditions: Adapter is closed.
;*		      Filter will be reset to 0.
;*
;*******************************************************************************


-CheckIntNum X

;Is OpenAdapter supported?
-CheckMACTable SC 40 m 00 08
-JFAIL GetSlave

-OpenAdapter,SUCCESS
-JPASS GetSlave

; Quit script because state of MAC is uncertain.
-LOG 2 OpenAdapter failed.  Exiting test.
-JUMP abort

:GetSlave
; Need to be able to see slave's response.
-SetPacketFilter 1,SUCCESS
-JFAIL SlaveFail

-IDSLAVE,SUCCESS
-JPASS ToggleOn

-LOG 3 Unable to identify slave computer.  Exiting script.
-JUMP abort

:SlaveFail
-LOG 3 Unable to set packet filter to 1 to identify slave.  Exiting script.
-JUMP abort


; Toggle Request Handle to zero
:ToggleOn
-ToggleReqHandle

;***
;*** MAIN BODY OF TEST CASE
;***

;*** HANDL001.1  -  SetPacketFilter
;*** Set Filter to 3 and check that filter was truly set

-SetPacketFilter 3
-JPASS CheckFilter

;SetPacketFilter Did Not Return SUCCESS.  Check for REQUEST_QUEUED.
-CheckIntNum X
-SetPacketFilter 3,REQUEST_QUEUED
-JPASS CheckFilter

-LOG 3 SetPacketFilter did not return SUCCESS or REQUEST_QUEUED.
-JUMP FilterFailed

:CheckFilter
-LOG 3 SetPacketFilter Returned SUCCESS or REQUEST_QUEUED.
-CheckMACTable SS 0a e 03 00
-JPASS FilterPassed

-LOG 3 Filter was not set properly

:FilterFailed
-LOG 3 SQLKEY:HANDL001.1 FAIL
-ToggleReqHandle
-SetPacketFilter 3
-ToggleReqHandle
-JUMP TranTest

:FilterPassed
-LOG 3 Filter was set to three.
-LOG 3 SQLKEY:HANDL001.1 PASS



:TranTest

;*** HANDL001.2  -  Transmit
;*** Transmit a frame to non-existent destination and expect no indications


;Check for Token Ring
-CheckMacTable SC 2 e 38 30 32 2e 35
-JFAIL EtherTran

-Buf i 00 40 44 34 43 34 43 34
-JUMP TranTest2

:EtherTran
-Buf i 44 34 43 34 43 34


:TranTest2
-CheckIntNum X
;expect no indications
-TransmitChain 0
-JFAIL TranFailed

-LOG 3 TransmitConfirm Indication was 0  PASSED.
-LOG 3 SQLKEY:HANDL001.2 PASS
-JUMP AddMCTest

:TranFailed
-LOG 3 TransmitChain with ReqHandle = 0  FAILED.
-LOG 3 SQLKEY:HANDL001.2 FAIL



:AddMCTest

;*** HANDL001.3  -  AddMulticastAddress
;*** Add a multicast address and expect a SUCCESS, REQUEST_QUEUED or NOT_SUPPORTED

;Is Multicast/Group supported?
-CheckMACTable SC 40 m 06
-JFAIL MultiNotSupp

;Multicast/Group supported

;Check for Token Ring
-CheckMacTable SC 2 e 38 30 32 2e 35
-JFAIL EtherMulti

-AddMulticastAddress 80 00 01 02 03 04
-JPASS AddCheck

;Add multicast Address did not return SUCCESS could it have been REQUEST_QUEUED
-CheckIntNum X
-Wait 1
-CheckMACTable MC 2 e 01 00
-JFAIL AddCheckFail

-ToggleReqHandle
-DelMulticastAddress 80 00 01 02 03 04
-ToggleReqHandle
-Wait 1
-CheckIntNum X
-AddMulticastAddress 80 00 01 02 03 04,REQUEST_QUEUED
-JPASS AddCheck

-LOG 3 AddMulticastAddress did not return a SUCCESS or REQUEST_QUEUED
-JUMP AddFailed


:EtherMulti

-AddMulticastAddress 01 02 03 04 05 06
-JPASS AddCheck

;Add multicast Address did not return SUCCESS could it have been REQUEST_QUEUED
-CheckIntNum X
-Wait 1
-CheckMACTable MC 2 e 01 00
-JFAIL AddCheckFail

-ToggleReqHandle
-DelMulticastAddress 01 02 03 04 05 06
-ToggleReqHandle
-Wait 1
-CheckIntNum X
-AddMulticastAddress 01 02 03 04 05 06,REQUEST_QUEUED
-JPASS AddCheck

-LOG 3 AddMulticastAddress did not return a SUCCESS or REQUEST_QUEUED
-JUMP AddFailed



:AddCheck
-LOG 3 AddMulticastAddress returned a SUCCESS or REQUEST_QUEUED
-Wait 1
-CheckMACTable MC 2 e 01 00
-JFAIL AddCheckFail

-LOG 3 Multicast Address added correctly
-LOG 3 SQLKEY:HANDL001.3 PASS
-JUMP DelMCTestSupp

:AddCheckFail
-LOG 3 Multicast Address was not added correctly
:AddFailed
-LOG 3 SQLKEY:HANDL001.3 FAIL
-JUMP DelMCTestSupp



:MultiNotSupp

;Check for Token Ring
-CheckMacTable SC 2 e 38 30 32 2e 35
-JFAIL EtherMulti

-AddMulticastAddress 80 00 01 02 03 04,NOT_SUPPORTED
-JFAIL NotSuppFailed
-JUMP AddMultiPass2

:EtherMulti
-AddMulticastAddress 01 02 03 04 05 06,NOT_SUPPORTED
-JFAIL NotSuppFailed

:AddMultiPass2
-LOG 3 AddMulticastAddress returned NOT_SUPPORTED
-LOG 3 SQLKEY:HANDL001.3 PASS
-JUMP DelMCTestNSupp

:NotSuppFailed
-LOG 3 AddMulticastAddress did not return NOT_SUPPORTED
-LOG 3 SQLKEY:HANDL001.3 FAIL
-JUMP DelMCTestNSupp





;*** HANDL001.4  -  DelMulticastAddress
;*** Delete multicast address and expect a SUCCESS, REQUEST_QUEUED or NOT_SUPPORTED


;Delete Multicast address is supported

:DelMCTestSupp
-CheckIntNum X
;Check for Token Ring
-CheckMacTable SC 2 e 38 30 32 2e 35
-JFAIL EtherDelMulti

-DelMulticastAddress 80 00 01 02 03 04
-JPASS DelCheck

;Del multicast Address did not return SUCCESS could it have been REQUEST_QUEUED
-CheckIntNum X
-Wait 1
-CheckMACTable MC 2 e 00 00
-JFAIL DelCheckFail

-ToggleReqHandle
-AddMulticastAddress 80 00 01 02 03 04
-ToggleReqHandle
-Wait 1
-CheckIntNum X
-DelMulticastAddress 80 00 01 02 03 04,REQUEST_QUEUED
-JPASS DelCheck

-LOG 3 DelMulticastAddress did not return a SUCCESS or REQUEST_QUEUED
-JUMP DelFailed


:EtherDelMulti

-DelMulticastAddress 01 02 03 04 05 06
-JPASS DelCheck

;Del multicast Address did not return SUCCESS could it have been REQUEST_QUEUED
-CheckIntNum X
-Wait 1
-CheckMACTable MC 2 e 00 00
-JFAIL DelCheckFail

-ToggleReqHandle
-AddMulticastAddress 01 02 03 04 05 06
-ToggleReqHandle
-Wait 1
-CheckIntNum X
-DelMulticastAddress 01 02 03 04 05 06,REQUEST_QUEUED
-JPASS DelCheck

-LOG 3 DelMulticastAddress did not return a SUCCESS or REQUEST_QUEUED
-JUMP DelFailed



:DelCheck
-LOG 3 DelMulticastAddress returned a SUCCESS or REQUEST_QUEUED
-Wait 1
-CheckMACTable MC 2 e 00 00
-JFAIL DelCheckFail

-LOG 3 Multicast Address deleted correctly
-LOG 3 SQLKEY:HANDL001.4 PASS
-JUMP ClrStatTest

:DelCheckFail
-LOG 3 Multicast Address was not deleted correctly
:DelFailed
-LOG 3 SQLKEY:HANDL001.4 FAIL
-JUMP ClrStatTest



;Delete Multicast address is not supported


:DelMCTestNSupp
;Check for Token Ring
-CheckIntNum X
-CheckMacTable SC 2 e 38 30 32 2e 35
-JFAIL EtherDelMulti2

-DelMulticastAddress 80 00 01 02 03 04,NOT_SUPPORTED
-JFAIL NotSuppFailed
-JUMP DelMultiPass2

:EtherDelMulti2
-DelMulticastAddress 01 02 03 04 05 06,NOT_SUPPORTED
-JFAIL NotSuppFailed

:DelMultiPass2
-LOG 3 DelMulticastAddress returned NOT_SUPPORTED
-LOG 3 SQLKEY:HANDL001.4 PASS
-JUMP ClrStatTest

:NotSuppFailed
-LOG 3 DelMulticastAddress did not return NOT_SUPPORTED
-LOG 3 SQLKEY:HANDL001.4 FAIL



:ClrStatTest

;*** HANDL001.5  -  ClearStatistics
;*** Issue ClearStatistics and expect a SUCCESS, REQUEST_QUEUED or NOT_SUPPORTED

-CheckIntNum X
-ClearStatistics
-JPASS ClrStatSuccess

;ClearStatistics did not return SUCCESS check for NOT_SUPPORTED first
-CheckIntNum X
-ClearStatistics,NOT_SUPPORTED
-JPASS ClrStatNSupp

-ClearStatistics,REQUEST_QUEUED
-JPASS ClrStatReqQud

-LOG 3 ClrStat did not return SUCCESS, REQUEST_QUEUED or NOT_SUPPORTED
-LOG 3 SQLKEY:HANDL001.5 FAIL
-JUMP UpStatTest

:ClrStatSuccess
-LOG 3 ClearStatistics returned SUCCESS
-JUMP ClrStatPass

:ClrStatNSupp
-LOG 3 ClearStatistics returned NOT_SUPPORTED
-JUMP ClrStatPass

:ClrStatReqQud
-Log 3 ClrStat returned REQUEST_QUEUED.  Disreguard Script Error

:ClrStatPass
-LOG 3 SQLKEY:HANDL001.5 PASS


:UpStatTest

;*** HANDL001.6  -  UpdateStatistics
;*** Issue UpdateStatistics and expect a SUCCESS, REQUEST_QUEUED or
;    NOT_SUPPORTED

-CheckIntNum X
-UpdateStatistics
-JPASS UpStatSuccess

;UpdateStatistics did not return SUCCESS check for NOT_SUPPORTED first
-CheckIntNum X
-UpdateStatistics,NOT_SUPPORTED
-JPASS UpStatNSupp

-UpdateStatistics,REQUEST_QUEUED
-JPASS UpStatReqQud

-LOG 3 UpdateStat did not return SUCCESS, REQUEST_QUEUED or NOT_SUPPORTED
-LOG 3 SQLKEY:HANDL001.6 FAIL
-JUMP IOFFTest

:UpStatSuccess
-LOG 3 UpdateStatistics returned SUCCESS
-JUMP UpStatPass

:UpStatNSupp
-LOG 3 UpdateStatistics returned NOT_SUPPORTED
-JUMP UpStatPass

:UpStatReqQud
-Log 3 UpdateStat returned REQUEST_QUEUED.  Disreguard Script Error

:UpStatPass
-LOG 3 SQLKEY:HANDL001.6 PASS



:IOFFTest

;*** HANDL001.7  -  IndicationOff
;*** Issue IndicationOff and expect a SUCCESS or REQUEST_QUEUED.  Transmit
;    a frame an expect no indications.


-CheckIntNum X
-IndicationOff
-JPASS IOFFCheck

;Indication OFF did not return SUCCESS.  Did it return REQUEST_QUEUED
-Wait 1
-ToggleReqHandle
-CheckIntNum X
-IndicationOn,SUCCESS
-ToggleReqHandle
-Wait 1
-CheckIntNum X
-IndicationOff,REQUEST_QUEUED
-JPASS IOFFCheck

-LOG 3 IndicationOff did not return SUCCESS or REQUEST_QUEUED
-JUMP IOffFailed

:IOFFCheck
-LOG 3 IndicationOff returned SUCCESS or REQUEST_QUEUED
-CheckIntNum X
-MRECV 1 100 100 0 0 0,SUCCESS
-CheckIntNum 0
-JFAIL IOffFailed1

-LOG 3 IndicationOff turned indications off.
-LOG 3 SQLKEY:HANDL001.7 PASS
-JUMP IOnTest

:IOffFailed1
-LOG 3 IndicatioOff did not turn indications off.

:IOffFailed
-LOG 3 SQLKEY:HANDL001.7 FAIL

;Turn Handle = 0 off and Issue a IOff so that indications will be off
;for the next test.
-ToggleReqHandle
-CheckIntNum X
-IndicationOff,SUCCESS
-ToggleReqHandle



:IOnTest

;*** HANDL001.8  -  IndicationOn
;*** Issue IndicationOn and expect a SUCCESS or REQUEST_QUEUED.  Transmit a
;    frame and expect an indication.


-CheckIntNum X
-IndicationOn
-JPASS IOnCheck

;Indication On did not return SUCCESS.	Did it return REQUEST_QUEUED
-Wait 1
-ToggleReqHandle
-CheckIntNum X
-IndicationOff,SUCCESS
-ToggleReqHandle
-Wait 1
-CheckIntNum X
-IndicationOn,REQUEST_QUEUED
-JPASS IOnCheck

-LOG 3 IndicationOn did not return SUCCESS or REQUEST_QUEUED
-JUMP IOnFailed

:IOnCheck
-LOG 3 IndicationOn returned SUCCESS or REQUEST_QUEUED
-CheckIntNum X
-MRECV 1 100 100 0 0 0,SUCCESS
-CheckIntNum 1
-JFAIL IOnFailed1

-LOG 3 IndicationOn turned indications on.
-LOG 3 SQLKEY:HANDL001.8 PASS
-JUMP DiagTest

:IOnFailed1
-LOG 3 IndicationOn did not turn indications on.

:IOnFailed
-LOG 3 SQLKEY:HANDL001.8 FAIL

;Turn Handle = 0 off and Issue a IOn so that indications will be on
;for the next test.
-ToggleReqHandle
-CheckIntNum X
-IndicationOn,SUCCESS
-ToggleReqHandle


:DiagTest

;*** HANDL001.9  -  InitiateDiagnostics
;*** Issue InitiateDiagnostics and expect a SUCCESS, REQUEST_QUEUED or
;*** NOT_SUPPORTED.


-CheckIntNum X

;Is InitiateDiagnostics supported?
-CheckMacTable SC 40 m 40
-JFAIL DiagNotSupp

;Diags supported check for SUCCESS or REQUEST_QUEUED.

-InitiateDiagnostics
-JPASS DiagPassed

;Diags did not return SUCCESS.	Did it return REQUEST_QUEUED
-Wait 1
-CheckIntNum X
-InitiateDiagnostics,REQUEST_QUEUED
-JPASS DiagPassed

-LOG 3 InitiateDiagnostics did not return SUCCESS or REQUEST_QUEUED
-LOG 3 SQLKEY:HANDL001.9 FAIL
-JUMP IntTest

:DiagPassed
-LOG 3 InitiateDiagnostics returned SUCCESS or REQUEST_QUEUED
-LOG 3 SQLKEY:HANDL001.9 PASS
-JUMP IntTest


;InitiateDiagnostics NOT SUPPORTED. Check for NOT_SUPPORTED.
:DiagNotSupp
-InitiateDiagnostics,NOT_SUPPORTED
-JPASS DiagPassed2

-LOG 3 InitiateDiagnostics did not return NOT_SUPPORTED
-LOG 3 SQLKEY:HANDL001.9 FAIL
-JUMP IntTest

:DiagPassed2
-LOG 3 InitiateDiagnostics returned NOT_SUPPORTED
-LOG 3 SQLKEY:HANDL001.9 PASS


:IntTest

;*** HANDL001.10  -  Interrupt
;*** Issue Interrupt and expect a SUCCESS, REQUEST_QUEUED or NOT_SUPPORTED.


-CheckIntNum X

;Is interrupt supported?
-CheckMacTable SC 41 m 10
-JFAIL IntNotSupp

;Interupt supported check for SUCCESS or REQUEST_QUEUED.

-Interrupt
-JPASS IntPassed

;Interrupt did not return SUCCESS.  Did it return REQUEST_QUEUED
-Wait 1
-CheckIntNum X
-Interrupt,REQUEST_QUEUED
-JPASS IntPassed

-LOG 3 Interrupt did not return SUCCESS or REQUEST_QUEUED
-LOG 3 SQLKEY:HANDL001.10 FAIL
-JUMP RdErrTest

:IntPassed
-LOG 3 Interrupt returned SUCCESS or REQUEST_QUEUED
-LOG 3 SQLKEY:HANDL001.10 PASS
-JUMP RdErrTest


;Interupt NOT SUPPORTED. Check for NOT_SUPPORTED.
:IntNotSupp
-Interrupt,NOT_SUPPORTED
-JPASS IntPassed2

-LOG 3 Interrupt did not return NOT_SUPPORTED
-LOG 3 SQLKEY:HANDL001.10 FAIL
-JUMP RdErrTest

:IntPassed2
-LOG 3 Interrupt returned NOT_SUPPORTED
-LOG 3 SQLKEY:HANDL001.10 PASS



:RdErrTest

;*** HANDL001.11  -  ReadErrorLog
;*** Issue ReadErrorLog and expect a SUCCESS, REQUEST_QUEUED or NOT_SUPPORTED.


-CheckIntNum X

;Is this Token Ring?
-CheckMacTable SC 2 e 38 30 32 2e 35
-JFAIL EtherRdErr

;Token Ring check for SUCCESS first.

-ReadErrorLog
-JPASS RdErrPassed

;ReadErrorLog did not return SUCCESS.  Did it return REQUEST_QUEUED
-Wait 1
-CheckIntNum X
-ReadErrorLog,REQUEST_QUEUED
-JPASS RdErrPassed

;Did not return SUCCESS or REQUEST_QUEUED. How about NOT_SUPPORTED?

-Wait 1
-CheckIntNum X
-ReadErrorLog,NOT_SUPPORTED
-JPASS RdErrPassed2
-JUMP RdErrFailed



:EtherRdErr


;Ethernet type check for NOT_SUPPORTED before testing for REQUEST_QUEUED

-ReadErrorLog
-JPASS RdErrPassed

;ReadErrorLog did not return SUCCESS.  Did it return NOT_SUPPORTED
-Wait 1
-CheckIntNum X
-ReadErrorLog,NOT_SUPPORTED
-JPASS RdErrPassed0

;Did not return SUCCESS or NOT_SUPPORTED. How about REQUEST_QUEUED?

-Wait 1
-CheckIntNum X
-ReadErrorLog,REQUEST_QUEUED
-JFAIL RdErrFailed

-LOG 3 ReadErrorLog returned REQUEST_QUEUED.  Disreguard script error.
-JUMP RdErrPassed3

:RdErrFailed
-LOG 3 ReadErrorLog did not return SUCCESS, REQUEST_QUEUED or NOT_SUPPORTED
-LOG 3 SQLKEY:HANDL001.11 FAIL
-JUMP FuncTest

:RdErrPassed0
-LOG 3 ReadErrorLog returned NOT_SUPPORTED
-JUMP RdErrPassed3

:RdErrPassed
-LOG 3 ReadErrorLog returned SUCCESS or REQUEST_QUEUED
-JUMP RdErrPassed3

:RdErrPassed2
-LOG 3 ReadErrorLog returned NOT_SUPPORTED.  Disreguard script error.

:RdErrPassed3
-LOG 3 SQLKEY:HANDL001.11 PASS


:FuncTest

;*** HANDL001.12  -  SetFunctionalAddress
;*** Issue SetFunctionalAddress expect a SUCCESS, REQUEST_QUEUED or
;*** NOT_SUPPORTED.


-CheckIntNum X

;Is functional addresses supported?
-CheckMacTable SC 40 m 04
-JFAIL FuncNotSupp

;Functional addresses supported check for SUCCESS or REQUEST_QUEUED.

-SetFunctionalAddress 01 02 03 04
-JPASS FuncCheck

;SetFunctionalAddress did not return SUCCESS.  Did it return REQUEST_QUEUED
-Wait 1
-CheckIntNum X
-SetFunctionalAddress 01 02 03 04,REQUEST_QUEUED
-JPASS FuncCheck

-LOG 3 SetFunctionalAddress did not return SUCCESS or REQUEST_QUEUED
-JUMP FuncFailed

:FuncCheck
-LOG 3 SetFunctionalAddress returned SUCCESS or REQUEST_QUEUED
-CheckMacTable SC 34 e 01 02 03 04
-JPASS FuncPassed

-LOG 3 Functional Address was not added correctly
-JUMP FuncFailed

:FuncPassed
-LOG 3 Functional Address added correctly
-LOG 3 SQLKEY:HANDL001.12 PASS
-SetFunctionalAddress 00 00 00 00
-JUMP SlookTest

:FuncFailed
-LOG 3 SQLKEY:HANDL001.12 FAIL
-SetFunctionalAddress 00 00 00 00
-JUMP SlookTest

:FuncNotSupp
;SetFunctionalAddress NOT SUPPORTED. Check for NOT_SUPPORTED.

-SetFunctionalAddress 01 02 03 04,NOT_SUPPORTED
-JPASS FuncPassed2

-LOG 3 SetFunctionalAddress did not return NOT_SUPPORTED
-LOG 3 SQLKEY:HANDL001.12 FAIL
-JUMP SlookTest

:FuncPassed2
-LOG 3 SetFunctionalAddress returned NOT_SUPPORTED
-LOG 3 SQLKEY:HANDL001.12 PASS



:SlookTest

;*** HANDL001.13  -  SetLookAhead
;*** Issue SetLookAhead and expect a SUCCESS or REQUEST_QUEUED.


-CheckIntNum X

-SetLookAhead 128
-JPASS SlookPassed

;SetLookAhead did not return SUCCESS.  Did it return REQUEST_QUEUED
-Wait 1
-CheckIntNum X
-SetLookAhead 128,REQUEST_QUEUED
-JPASS SlookPassed

-LOG 3 SetLookAhead did not return SUCCESS or REQUEST_QUEUED
-LOG 3 SQLKEY:HANDL001.13 FAIL
-JUMP CloseTest

:SlookPassed
-LOG 3 SetLookAhead returned SUCCESS or REQUEST_QUEUED
-LOG 3 SQLKEY:HANDL001.13 PASS


:CloseTest

;*** HANDL001.14  -  CloseAdapter
;*** Issue CloseAdapter and expect a SUCCESS, REQUEST_QUEUED or NOT_SUPPORTED


-CheckIntNum X

;Is CloseAdapter supported?
-CheckMacTable SC 41 m 08
-JFAIL ClsNotSupp

;Close Supported

-CloseAdapter
-JPASS CloseCheck

;Close did not return SUCCESS.	Did it return REQUEST_QUEUED
-Wait 2
-CheckIntNum X
-ToggleReqHandle
-OpenAdapter,SUCCESS
-ToggleReqHandle
-CloseAdapter,REQUEST_QUEUED
-JPASS CloseCheck

-LOG 3 CloseAdapter did not return SUCCESS or REQUEST_QUEUED
-Wait 2
-JUMP CloseFailed

:CloseCheck
-Wait 2
-CheckMacTable SS 06 e 0f
-JPASS ClosePassed

-LOG 3 Adapter is still in Open state
-JUMP CloseFailed


:ClosePassed
-LOG 3 Adapter is Closed
-LOG 3 SQLKEY:HANDL001.14 PASS
-JUMP SetStnTest

:CloseFailed
-LOG 3 SQLKEY:HANDL001.14 FAIL
-ToggleReqHandle
-CloseAdapter
-Wait 2
-ToggleReqHandle
-JUMP SetStnTest


;Close Not Supported

:ClsNotSupp

-CloseAdapter,NOT_SUPPORTED
-JPASS ClosePassed2

-LOG 3 CloseAdapter did not return NOT_SUPPORTED
-LOG 3 SQLKEY:HANDL001.14 FAIL
-JUMP SetStnTest

:ClosePassed2
-LOG 3 CloseAdapter returned NOT_SUPPORTED
-LOG 3 SQLKEY:HANDL001.14 PASS


:SetStnTest

;*** HANDL001.15  -  SetStationAddress
;*** Issue SetStationAddress and expect a SUCCESS, REQUEST_QUEUED or
;*** NOT_SUPPORTED


-CheckIntNum X

;Is SetStationAddress supported?
-CheckMacTable SC 40 m 10
-JFAIL SetStnNotSupp

;SetStation supported

-SetStationAddress 44 55 77 44 55 77
-JPASS StnCheck

;SetStationAddress did not return SUCCESS.  Did it return REQUEST_QUEUED
-Wait 1
-CheckIntNum X
-SetStationAddress 44 55 77 44 55 77,REQUEST_QUEUED
-JPASS StnCheck

-LOG 3 SetStationAddress did not return SUCCESS or REQUEST_QUEUED
-JUMP SetStnFailed

:StnCheck
-CheckMacTable SC 24 e 44 55 77 44 55 77
-JPASS SetStnPassed

-LOG 3 Station Address was not set properly
-JUMP SetStnFailed

:SetStnPassed
-LOG 3 Station Address was set correctly
-LOG 3 SQLKEY:HANDL001.15 PASS
-JUMP OpenTest

:SetStnFailed
-LOG 3 SQLKEY:HANDL001.15 FAIL
-JUMP OpenTest


;SetStationAddress not Supported

:SetStnNotSupp

-SetStationAddress 44 55 77 44 55 77,NOT_SUPPORTED
-JPASS SetStnPassed2

-LOG 3 SetStationAddress did not return NOT_SUPPORTED
-LOG 3 SQLKEY:HANDL001.15 FAIL
-JUMP OpenTest

:SetStnPassed2
-LOG 3 SetStationAddress returned NOT_SUPPORTED
-LOG 3 SQLKEY:HANDL001.15 PASS



:OpenTest

;*** HANDL001.16  -  OpenAdapter
;*** Issue OpenAdapter and expect a SUCCESS, REQUEST_QUEUED or NOT_SUPPORTED


-CheckIntNum X

;Is OpenAdapter supported?
-CheckMacTable SC 41 m 08
-JFAIL OpnNotSupp

;Open Supported

-OpenAdapter
-JPASS OpenCheck

;Open did not return SUCCESS.  Did it return REQUEST_QUEUED
-Wait 15
-CheckIntNum X
-ToggleReqHandle
-CloseAdapter,SUCCESS
-ToggleReqHandle
-OpenAdapter,REQUEST_QUEUED
-JPASS OpenCheck

-LOG 3 OpenAdapter did not return SUCCESS or REQUEST_QUEUED
-Wait 15
-JUMP OpenFailed

:OpenCheck
-Wait 15
-LOG 3 OpenAdapter returned SUCCESS or REQUEST_QUEUED
-CheckMacTable SS 06 e 1f
-JPASS OpenPassed

-LOG 3 Adapter is still in Close state
-JUMP OpenFailed


:OpenPassed
-LOG 3 Adapter is Open
-LOG 3 SQLKEY:HANDL001.16 PASS
-JUMP ResetTest

:OpenFailed
-LOG 3 SQLKEY:HANDL001.16 FAIL
-ToggleReqHandle
-OpenAdapter
-Wait 15
-ToggleReqHandle
-JUMP ResetTest


;Open Not Supported

:OpnNotSupp

-OpenAdapter,NOT_SUPPORTED
-JPASS OpenPassed2

-LOG 3 OpenAdapter did not return NOT_SUPPORTED
-LOG 3 SQLKEY:HANDL001.16 FAIL
-JUMP ResetTest

:OpenPassed2
-LOG 3 OpenAdapter returned NOT_SUPPORTED
-LOG 3 SQLKEY:HANDL001.16 PASS



:ResetTest

;*** HANDL001.17  -  ResetMac
;*** Issue ResetMac and expect a SUCCESS, REQUEST_QUEUED or NOT_SUPPORTED


-CheckIntNum X

;Is ResetMac supported?
-CheckMacTable SC 41 m 04
-JFAIL RSNotSupp

;Reset Supported

-ResetMac
-JPASS RSPassed

;Reset did not return SUCCESS.	Did it return REQUEST_QUEUED
-Wait 15
-CheckIntNum X
-ResetMac,REQUEST_QUEUED
-JPASS RSPassed

-LOG 3 ResetMac did not return SUCCESS or REQUEST_QUEUED
-LOG 3 SQLKEY:HANDL001.17 FAIL
-Wait 15
-JUMP EndTest

:RSPassed
-LOG 3 ResetMac returned SUCCESS or REQUEST_QUEUED
-LOG 3 SQLKEY:HANDL001.17 PASS
-Wait 15
-JUMP EndTest


;Reset Not Supported

:RSNotSupp

-ResetMac,NOT_SUPPORTED
-JPASS RSPassed2

-LOG 3 ResetMac did not return NOT_SUPPORTED
-LOG 3 SQLKEY:HANDL001.17 FAIL
-JUMP EndTest

:RSPassed2
-LOG 3 ResetMac returned NOT_SUPPORTED
-LOG 3 SQLKEY:HANDL001.17 PASS
-JUMP EndTest


:abort
-LOG 3 Aborting HANDL001 because state of MAC is uncertain
-SCRIPT ABORT ABORT
-SetFailure
-JUMP End


:EndTest

-ToggleReqHandle
-CheckIntNum X

;Is CloseAdapter supported?
-CheckMacTable SC 41 m 08
-JFAIL End

-CloseAdapter,SUCCESS
-JPASS End

-LOG 3 Aborting HANDL001 because state of MAC is uncertain
-SCRIPT ABORT ABORT

:End


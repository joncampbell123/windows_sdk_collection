;*****************************************************************************
;*
;*   File: CLRST001.S - Script file to test the ClearStatistics (CS)
;*                      primitive.
;*
;*   Description: Verify operation of ClearStatistics under normal
;*                conditions.
;*                Tests include:
;*                   - If CS IS NOT supported, check for NOT_SUPPORTED return
;*                   - If CS IS supported:
;*                         - Check for SUCCESS return
;*                         - Make sure the MAC's statistics counters are
;*                           reset to zero
;*
;*
;*   Date: 6/5/90
;*
;*   Pre-conditions: Adapter is closed.
;*
;*   Post-conditions: Adapter is closed.
;*
;*****************************************************************************

  -LOG 3 Starting ClearStatistics Tests

  -LOG 3 If this MAC supports OpenAdapter exec. OA
*** Check the Service Specific Characteristics Table to see if the
*** OpenAdapter command is supported by this MAC.
  -CheckMacTable SC 41 m 08
  -JFAIL Begin_Test

  -OpenAdapter,SUCCESS
  -JPASS Begin_Test

   Quit script because state of MAC is uncertain.
  -LOG 3 OpenAdapter failed.  Exiting test.
  -JUMP abort

:Begin_Test
  -LOG 3 Starting ClearStatistics Tests
*** Execute ClearStatistics, check for SUCCESS or NOT_SUPPORTED
  -ClearStatistics
  -JPASS Stats_Check

  -LOG 3 ClearStatistics may not be supported by the MAC - check
  -LOG 3 for NOT_SUPPORTED
  -CheckIntNum X
  -ClearStatistics,NOT_SUPPORTED
  -JPASS NS_Pass
  Do this to make sure ClearStatistics did not originally return 
  REQUEST_QUEUED. 
  -ClearStatistics,SUCCESS
  -JFAIL NS_Fail
  -JUMP Stats_Check

:NS_Pass
  -LOG 3 SQLKEY:CLRST001.1 PASS
  -JUMP end

:NS_Fail
  -LOG 3 WARNING:  ClearStatistics apparently is not supported by
  -LOG 3           this MAC but did not return NOT_SUPPORTED
  -LOG 3 SQLKEY:CLRST001.1 FAIL
  -JUMP end

:Stats_Check
*** Now explicitly check for SUCCESS return - we don't care if we call twice
  -CheckIntNum X
  -ClearStatistics,SUCCESS
  -JFAIL Chk_SUCCFail
  -LOG 3 SQLKEY:CLRST001.2 PASS
  -JUMP Start_Stats
:Chk_SUCCFail
  -LOG 3 ClearStatistics did not return SUCCESS
  -LOG 3 SQLKEY:CLRST001.2 FAIL
  -JUMP end

:Start_Stats
*** The rest of this test checks the MAC's SS table fields
  -LOG 3 Date/Time when ClearStatistics was issued should not be 0xFFFFFFFF
  -CheckMacTable SS 10 e FF FF FF FF
  -JPASS CSDT_Fail
  -LOG 3 SQLKEY:CLRST001.3 PASS
  -JUMP Chk_TotFrms
:CSDT_Fail
  -LOG 3 Date/Time last CS was issued is 0xFFFFFFFF
  -LOG 3 SQLKEY:CLRST001.3 FAIL
  -LOG 3 If this field is still negative assume statistics are not supported
  -JUMP end

:Chk_TotFrms
*** Total frames received should be 0
  -CheckMacTable SS 14 e 00 00 00 00 | FF FF FF FF
  -JFAIL TotFrms_Fail
  -LOG 3 SQLKEY:CLRST001.4 PASS
  -JUMP Chk_CRCErr

:TotFrms_Fail
  -LOG 3 Total frames received is nonzero and non -1. 
  -LOG 3 SQLKEY:CLRST001.4 FAIL

:Chk_CRCErr
*** Frames with CRC error should be 0
  -CheckMacTable SS 18 e 00 00 00 00 | FF FF FF FF
  -JFAIL CRCErr_Fail
  -LOG 3 SQLKEY:CLRST001.5 PASS
  -JUMP Chk_TotBRecv

:CRCErr_Fail
  -LOG 3 Frames with CRC error is nonzero and non -1. 
  -LOG 3 SQLKEY:CLRST001.5 FAIL

:Chk_TotBRecv
*** total bytes received should be 0
  -CheckMacTable SS 1C e 00 00 00 00 | FF FF FF FF
  -JFAIL ChkBR_Fail
  -LOG 3 SQLKEY:CLRST001.6 PASS
  -JUMP Chk_FrmsDisc

:ChkBR_Fail
  -LOG 3 Total bytes received is nonzero and non -1.
  -LOG 3 SQLKEY:CLRST001.6 FAIL

:Chk_FrmsDisc
*** Frames discarded should be zero
  -CheckMacTable SS 20 e 00 00 00 00 | FF FF FF FF
  -JFAIL FrmsDisc_Fail
  -LOG 3 SQLKEY:CLRST001.7 PASS
  -JUMP Chk_MRecv

:FrmsDisc_Fail
  -LOG 3 Frames discarded is nonzero and non -1. 
  -LOG 3 SQLKEY:CLRST001.7 FAIL

:Chk_MRecv
*** Multicast frames received should be 0
  -CheckMacTable SS 24 e 00 00 00 00 | FF FF FF FF 
  -JFAIL MRecv_Fail
  -LOG 3 SQLKEY:CLRST001.8 PASS
  -JUMP Chk_BRecv

:MRecv_Fail
  -LOG 3 Total multicast frames received is nonzero and non -1.
  -LOG 3 SQLKEY:CLRST001.8 FAIL

:Chk_BRecv
*** Broadcast frames received should 0
  -CheckMacTable SS 28 e 00 00 00 00 | FF FF FF FF
  -JFAIL BRecv_Fail
  -LOG 3 SQLKEY:CLRST001.9 PASS
  -JUMP Chk_FrmRErr

:BRecv_Fail
  -LOG 3 Total broadcast frames is nonzero and non -1.
  -LOG 3 SQLKEY:CLRST001.9 FAIL

:Chk_FrmRErr
*** Frames received with errors should be 0
  -CheckMacTable SS 2C e 00 00 00 00 | FF FF FF FF
  -JFAIL FrmRErr_Fail
  -LOG 3 SQLKEY:CLRST001.10 PASS
  -JUMP Chk_FrExMaxSiz

:FrmRErr_Fail
  -LOG 3 Total frames received is nonzero and non -1.
  -LOG 3 SQLKEY:CLRST001.10 FAIL

:Chk_FrExMaxSiz
*** Frames exceeding maximum size should be 0
  -CheckMacTable SS 30 e 00 00 00 00 | FF FF FF FF
  -JFAIL FrExMaxSiz_Fail
  -LOG 3 SQLKEY:CLRST001.11 PASS
  -JUMP Chk_FrExMinSiz

:FrExMaxSiz_Fail
  -LOG 3 Frames exceeding maximum size is nonzero and non -1.
  -LOG 3 SQLKEY:CLRST001.11 FAIL

:Chk_FrExMinSiz
*** Frames smaller than minimum size should be 0
  -CheckMacTable SS 34 e 00 00 00 00 | FF FF FF FF
  -JFAIL FrExMinSiz_Fail
  -LOG 3 SQLKEY:CLRST001.12 PASS
  -JUMP Chk_MCbrecv

:FrExMinSiz_Fail
  -LOG 3 Frames smaller than minimum size is nonzero and non -1.
  -LOG 3 SQLKEY:CLRST001.12 FAIL

:Chk_MCbrecv
*** Multicast bytes received should be 0
  -CheckMacTable SS 38 e 00 00 00 00 | FF FF FF FF
  -JFAIL MCbrecv_Fail
  -LOG 3 SQLKEY:CLRST001.13 PASS
  -JUMP Chk_BCbrecv

:MCbrecv_Fail
  -LOG 3 Multicast bytes received is nonzero and non -1.
  -LOG 3 SQLKEY:CLRST001.13 FAIL

:Chk_BCbrecv
*** Broadcast bytes received should be 0
  -CheckMacTable SS 3C e 00 00 00 00 | FF FF FF FF
  -JFAIL BCbrecv_Fail
  -LOG 3 SQLKEY:CLRST001.14 PASS
  -JUMP Chk_FrmsDisc

:BCbrecv_Fail
  -LOG 3 Broadcast bytes received is nonzero and non -1.
  -LOG 3 SQLKEY:CLRST001.14 FAIL

:Chk_FrmsDisc
*** Frames discarded should be 0
  -CheckMacTable SS 40 e 00 00 00 00 | FF FF FF FF
  -JFAIL FrmsDisc_Fail
  -LOG 3 SQLKEY:CLRST001.15 PASS
  -JUMP Chk_TotFXmit

:FrmsDisc_Fail
  -LOG 3 Frames discarded is nonzero and non -1.
  -LOG 3 SQLKEY:CLRST001.15 FAIL

:Chk_TotFXmit
***  Total frames transmitted should be 0
  -CheckMacTable SS 44 e 00 00 00 00 | FF FF FF FF
  -JFAIL TotFXmit_Fail
  -LOG 3 SQLKEY:CLRST001.16 PASS
  -JUMP Chk_TotBXmit

:TotFXmit_Fail
  -LOG 3 Total frames transmitted is nonzero and non -1.
  -LOG 3 SQLKEY:CLRST001.16 FAIL

:Chk_TotBXmit
***  Total bytes transmitted should be 0
  -CheckMacTable SS 48 e 00 00 00 00 | FF FF FF FF
  -JFAIL TotBXmit_Fail
  -LOG 3 SQLKEY:CLRST001.17 PASS
  -JUMP Chk_MCFrmXmit

:TotBXmit_Fail
  -LOG 3 Total bytes transmitted is nonzero and non -1.
  -LOG 3 SQLKEY:CLRST001.17 FAIL

:Chk_MCFrmXmit
***  Multicast frames transmitted should be 0
  -CheckMacTable SS 4C e 00 00 00 00 | FF FF FF FF
  -JFAIL MCFrmXmit_Fail
  -LOG 3 SQLKEY:CLRST001.18 PASS
  -JUMP Chk_BCFrmXmit

:MCFrmXmit_Fail
  -LOG 3 Multicast frames transmitted is nonzero and non -1.
  -LOG 3 SQLKEY:CLRST001.18 FAIL

:Chk_BCFrmXmit
***  Broadcast frames transmitted should be 0
  -CheckMacTable SS 50 e 00 00 00 00 | FF FF FF FF
  -JFAIL BCFrmXmit_Fail
  -LOG 3 SQLKEY:CLRST001.19 PASS
  -JUMP Chk_BCBXmit

:BCFrmXmit_Fail
  -LOG 3 Broadcast frames transmitted is nonzero and non -1.
  -LOG 3 SQLKEY:CLRST001.19 FAIL

:Chk_BCBXmit
***  Broadcast bytes transmitted should be 0
  -CheckMacTable SS 54 e 00 00 00 00 | FF FF FF FF
  -JFAIL BCBXmit_Fail
  -LOG 3 SQLKEY:CLRST001.20 PASS
  -JUMP Chk_MCBXmit

:BCBXmit_Fail
  -LOG 3 Broadcast bytes transmitted is nonzero and non -1.
  -LOG 3 SQLKEY:CLRST001.20 FAIL

:Chk_MCBXmit
***  Multicast bytes transmitted should be 0
  -CheckMacTable SS 58 e 00 00 00 00 | FF FF FF FF
  -JFAIL MCBXmit_Fail
  -LOG 3 SQLKEY:CLRST001.21 PASS
  -JUMP Chk_FNXmitTO

:MCBXmit_Fail
  -LOG 3 Multicast bytes transmitted is nonzero and non -1.
  -LOG 3 SQLKEY:CLRST001.21 FAIL

:Chk_FNXmitTO
***  Frames not transmitted due to time out should be 0
  -CheckMacTable SS 5C e 00 00 00 00 | FF FF FF FF
  -JFAIL FNXmitTO_Fail
  -LOG 3 SQLKEY:CLRST001.22 PASS
  -JUMP Chk_FNXmitHE

:FNXmitTO_Fail
  -LOG 3 Frames not transmitted due to time out is nonzero and non -1.
  -LOG 3 SQLKEY:CLRST001.22 FAIL

:Chk_FNXmitHE
***  Frames not transmitted due to hardware error should be 0
  -CheckMacTable SS 60 e 00 00 00 00 | FF FF FF FF
  -JFAIL FNXmitHE_Fail
  -LOG 3 SQLKEY:CLRST001.23 PASS
  -JUMP ChkAllNS

:FNXmitHE_Fail
  -LOG 3 Frames not transmitted due to hardware error is nonzero and non -1.
  -LOG 3 SQLKEY:CLRST001.23 FAIL


*** Check to see if all the counters are -1.  If so, then this is an
*** error because clear stats is returning SUCCESS. 
:ChkAllNS
*** Total frames received should be non -1.
  -CheckMacTable SS 14 e FF FF FF FF
  -JFAIL AllNS_Pass
*** Frames with CRC error should be non -1.
  -CheckMacTable SS 18 e FF FF FF FF
  -JFAIL AllNS_Pass
*** total bytes received should be non -1.
  -CheckMacTable SS 1C e FF FF FF FF
  -JFAIL AllNS_Pass
*** Frames discarded should be zero
  -CheckMacTable SS 20 e FF FF FF FF
  -JFAIL AllNS_Pass
*** Multicast frames received should be non -1.
  -CheckMacTable SS 24 e FF FF FF FF 
  -JFAIL AllNS_Pass
*** Broadcast frames received should 0
  -CheckMacTable SS 28 e FF FF FF FF
  -JFAIL AllNS_Pass
*** Frames received with errors should be non -1.
  -CheckMacTable SS 2C e FF FF FF FF
  -JFAIL AllNS_Pass
*** Frames exceeding maximum size should be non -1.
  -CheckMacTable SS 30 e FF FF FF FF
  -JFAIL AllNS_Pass
*** Frames smaller than minimum size should be non -1.
  -CheckMacTable SS 34 e FF FF FF FF
  -JFAIL AllNS_Pass
*** Multicast bytes received should be non -1.
  -CheckMacTable SS 38 e FF FF FF FF
  -JFAIL AllNS_Pass
*** Broadcast bytes received should be non -1.
  -CheckMacTable SS 3C e FF FF FF FF
  -JFAIL AllNS_Pass
*** Frames discarded should be non -1.
  -CheckMacTable SS 40 e FF FF FF FF
  -JFAIL AllNS_Pass
***  Total frames transmitted should be non -1.
  -CheckMacTable SS 44 e FF FF FF FF
  -JFAIL AllNS_Pass
***  Total bytes transmitted should be non -1.
  -CheckMacTable SS 48 e FF FF FF FF
  -JFAIL AllNS_Pass
***  Multicast frames transmitted should be non -1.
  -CheckMacTable SS 4C e FF FF FF FF
  -JFAIL AllNS_Pass
***  Broadcast frames transmitted should be non -1.
  -CheckMacTable SS 50 e FF FF FF FF
  -JFAIL AllNS_Pass
***  Broadcast bytes transmitted should be non -1.
  -CheckMacTable SS 54 e FF FF FF FF
  -JFAIL AllNS_Pass
***  Multicast bytes transmitted should be non -1.
  -CheckMacTable SS 58 e FF FF FF FF
  -JFAIL AllNS_Pass
***  Frames not transmitted due to time out should be non -1.
  -CheckMacTable SS 5C e FF FF FF FF
  -JFAIL AllNS_Pass
***  Frames not transmitted due to hardware error should be non -1.
  -CheckMacTable SS 60 e FF FF FF FF
  -JFAIL AllNS_Pass

  -LOG 3 All clear stats command returned SUCCESS, yet all
  -LOG 3 counters are -1.
  -LOG 3 SQLKEY:CLRST001.24 FAIL
  -JUMP end

:AllNS_Pass
  -LOG 3 All clear stats command returned SUCCESS, yet at least some
  -LOG 3 counters are supported.
  -LOG 3 SQLKEY:CLRST001.24 PASS
  -JUMP end


:abort
  -LOG 3 Aborting CLTST001 because state of MAC is uncertain.
  -SCRIPT ABORT ABORT
  -SetFailure
  -JUMP end2

:end
  -LOG 3 Finished ClearStatistics Tests

*** Check the Service Specific Characteristics Table to see if the
*** CloseAdapter command is supported by this MAC.
  -LOG 3 Check if CloseAdapter is supported.
  -CheckMacTable SC 41 m 08
  -JFAIL end2

  -CloseAdapter,SUCCESS
  -JPASS end2

   Quit script because state of MAC is uncertain.
  -LOG 3 CloseAdapter failed.  Exiting test.
  -LOG 3 Aborting CLRST001 because state of MAC is uncertain.
  -SCRIPT ABORT ABORT

:end2


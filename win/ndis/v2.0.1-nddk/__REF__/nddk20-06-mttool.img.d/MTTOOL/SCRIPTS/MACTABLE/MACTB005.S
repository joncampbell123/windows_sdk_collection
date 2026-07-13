;*******************************************************************************
;*   FILE: mactb005.s - test loopback
;*
;*   Description: Check if loopback supported
;*		  Issue MTRAN to the driver itself and check indications
;*		  Issue MTRAN in the Not_Supported test to ensure no indications
;*
;*
;*   Date: 5-30-90
;*
;*   Pre-conditions:
;*	 Adapter is closed
;*
;*   Post-conditions:
;*	 Adapter closed/Set Filter 0.
;*
;*******************************************************************************


*** check service specific characteristics table to check if loopback is
*** supported by this MAC.
  -CheckMacTable SC 40 m 80
  -JFAIL NotSupported
  -LOG 3 loopback supported

*** Check to see if Loopback is truely supported, by issuing a MTRAN.
*** Check Service Specific Characteristics Table for OpenAdapter support
*** for this MAC.
  -CheckMacTable SC 41 m 08
  -JFAIL Filter

  -LOG 3 OpenAdapter is supported.
  -OpenAdapter,SUCCESS
  -JFAIL OpenFail
  -LOG 3 OpenAdapter was successful.

  :Filter
  Set Packet Filter to receive directed packet.
  -LOG 3 Set Packet Filter.
  -SetPacketFilter 1,SUCCESS
  -JFAIL FilterFail1
  -LOG 3 Packet filter was set to 1 successfully.

  -LOG 3 Check for indications of received frame.
  -CheckIntNum X
  -MTRAN 1 100 100 0 0 0
  -CheckIntNum 1,SUCCESS
  -JFAIL LoopFail1
  -LOG 3 Received indication for frame sent to self.
  -LOG 3 SQLKEY:MACTB005.1 PASS
  -JUMP End

:OpenFail
-LOG 3 OpenAdapter was not successful. Exiting script
-JUMP abort

:FilterFail1
  -LOG 3 Set Packet Filter failed.  Exiting script.
  -JUMP abort

:NotSupported
  -LOG 3 loopback not supported
*** Ensure that loopback is not supported by issuing a MTRAN and checking
*** for zero indications received.
  -CheckIntNum X
  -MTRAN 1 64 100 0 0 0
  -CheckIntNum 0,SUCCESS
  -JFAIL LoopFail2
  -LOG 3 Did not receive indication for frame sent to self.
  -LOG 3 SQLKEY:MACTB005.2 PASS
  -JUMP End

:LoopFail1
  -LOG 3 No indications of a received frame.
  -LOG 3 SQLKEY:MACTB005.1 FAIL
  -JUMP End

:LoopFail2
  -LOG 3 Expecting zero indications. Received other than zero indications.
  -LOG 3 SQLKEY:MACTB005.2 FAIL
  -JUMP End

:abort
-LOG 3 Aborting MACTB005 because state of MAC is uncertain
-SCRIPT ABORT ABORT
-JUMP End2

:End
-SetPacketFilter 0
-CloseAdapter
-Wait 2

:End2



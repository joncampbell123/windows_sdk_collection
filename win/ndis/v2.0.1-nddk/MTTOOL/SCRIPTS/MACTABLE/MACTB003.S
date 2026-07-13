;*******************************************************************************
;*   FILE: mactb003.s - checks the functional address field and the functional
;*                      address supported bit
;*
;*   Description: check the supported bit
;*		  set the functional address
;*		  make sure it is changed
;*		  set up slave
;*		  have slave send 256 frames at 256 bytes to fncadr
;*		  master receives frames and updates counters
;*		  have slave send 256 frames at 256 bytes to grpadr
;*		  show that master does not confuse grpadr with fncadr
;*
;*   Date: 6-04-90
;*
;*   Pre-conditions: adapter closed
;*
;*   Post-conditions: adapter closed
;*		      functional address set to 00 00 00 00
;*
;*******************************************************************************


*** check that functional addressing is supported
  -CheckMacTable SC 40 m 04
  -JFAIL NotSupported
  -LOG 3 functional addressing is supported

*** Check the Service Specific Characteristics table to see if OpenAdapter
*** is supported for this MAC.
  -CheckMacTable SC 41 m 08
  -JFAIL Func1

  -OpenAdapter,SUCCESS
  -JFAIL FuncFail1
  -LOG 3 OpenAdapter was successful

:Func1
*** set packet filter to accept functional addressess
  ;set packet filter to 1
  -SetPacketFilter 1,SUCCESS
  -JFAIL FuncFail2
  -LOG 3 Set Packet Filter returned a success

:Func2
*** set the functional address to 00 00 FF 00
  -SetFunctionalAddress 00 00 FF 00,SUCCESS
  -JPASS Func3
  -LOG 3 set functional address command failed
  -LOG 3 SQLKEY:MACTB003.1 FAIL
  -JUMP End

:FuncFail1
  -LOG 3 OpenAdapter failed open test. Exiting script.
  -JUMP Abort

:FuncFail2
  -LOG 3 Set Packet Filter did not return a success
  -JUMP Abort

:Func3
  -LOG 3 set functional address command passed
  -LOG 3 SQLKEY:MACTB003.1 PASS

*** check that the functional address was changed
  -CheckMacTable SC 34 e 00 00 FF 00
  -JPASS Func4
  -LOG 3 functional address not changed in SS table
  -LOG 3 SQLKEY:MACTB003.2 FAIL
  -JUMP SlaveStuff

:Func4
  -LOG 3 functional address changed in SS table
  -LOG 3 SQLKEY:MACTB003.2 PASS

*** set up slave mode
:SlaveStuff
  -IDSLAVE,SUCCESS
  -JPASS Func5
  -LOG 3 slave mode failed
  -JUMP Abort

:Func5
  -LOG 3 slave mode succeeded

*** clear statistics table
  -ClearStatistics
  -WAIT 1
  -CheckIntNum X

*** have slave send 256 frames of 256 bytes to c0 00 00 00 ff 00
  -MRECV 256 256 256 0 0 2 c0 00 00 00 ff 00,SUCCESS
  -JPASS Func6
  -LOG 3 attempted send by slave failed
  -LOG 3 SQLKEY:MACTB003.3 FAIL
  -JUMP End

:Func6
  -LOG 3 attempted send by slave succeeded
  -LOG 3 SQLKEY:MACTB003.3 PASS

*** wait to be sure it got there
  -WAIT 5
*** Update statistics table
  -UpdateStatistics
  -WAIT 1
  -CheckIntNum X

*** make sure master updates functional frames received
  -CheckMacTable SS 24 e 00 01 00 00 | FF FF FF FF
  -JPASS Pass
  -LOG 3 functional frames received but statistics not updated
  -LOG 3 SQLKEY:MACTB003.4 FAIL
  -JUMP Func7

:Pass
  -LOG 3 functional frames received and statistics updated
  -LOG 3 SQLKEY:MACTB003.4 PASS

:Func7
*** clear statistics table
  -ClearStatistics
  -WAIT 1
  -CheckIntNum X

*** have slave send 256 frames of 256 bytes to c0 00 80 00 ff 00
  -MRECV 256 256 256 0 0 2 c0 00 80 00 ff 00,SUCCESS
  -JPASS Pass
  -LOG 3 attempted send by slave failed
  -LOG 3 SQLKEY:MACTB003.5 FAIL
  -JUMP End

:Pass
  -LOG 3 attempted send by slave succeeded
  -LOG 3 SQLKEY:MACTB003.5 PASS

*** slave sent a group address to master, indications should be 0
  -CheckIntNum 0,SUCCESS
  -JPASS Pass
  -LOG 3 Indications showed other than zero
  -LOG 3 SQLKEY:MACTB003.6 FAIL
  -JUMP End

:Pass
  -LOG 3 No indications were received by master as expected
  -LOG 3 SQLKEY:MACTB003.6 PASS
  -JUMP End

:NotSupported
  -LOG 3 Will assume functional addressing is not supported
  -SetFunctionalAddress 00 00 FF 00,NOT_SUPPORTED
  -JPASS Pass
  -LOG 3 SQLKEY:MACTB003.7 FAIL
  -JUMP End

:Pass
  -LOG 3 Set Functional Address is not supported
  -LOG 3 SQLKEY:MACTB003.7 PASS
  -JUMP End

:Abort
  -LOG 3 test aborted
  -Script abort abort
  -SetFailure
  -JUMP End2

:End
  -SetFunctionalAddress 00 00 00 00
  -CloseAdapter
  -WAIT 2

:End2


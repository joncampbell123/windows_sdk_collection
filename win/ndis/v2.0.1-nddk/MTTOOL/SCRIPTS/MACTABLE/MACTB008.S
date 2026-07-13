;*******************************************************************************
;*   FILE: mactb008.s - test packet filter bit mask
;*
;*   Description: set packet filters 0,1,2,3,7 and 8
;*		  make sure they are set in the ss table
;*
;*   Date: 6-11-90
;*
;*   Pre-conditions: adapter is closed
;*
;*   Post-conditions: adapter is closed
;*		      packet filter is set to 0
;*
;*******************************************************************************

:Filter0
;set packet filter to 0
  -SetPacketFilter 0,SUCCESS
  -JPASS PASS

  -LOG 3 Set packet filter didn't return success
  -LOG 3 SQLKEY:MACTB008.1 FAIL
  -JUMP Filter1

:PASS
  -LOG 3 set packet filter returned success

;check filter field in ss table
  -CheckMacTable ss 0A e 00 00
  -JPASS PASS

  -LOG 3 filter field in service specific status table not correct
  -LOG 3 SQLKEY:MACTB008.1 FAIL
  -JUMP Filter1

:PASS
  -LOG 3 filter field in service specific status table is correct
  -LOG 3 SQLKEY:MACTB008.1 PASS

:Filter1
;set packet filter to 1
  -SetPacketFilter 1,SUCCESS
  -JPASS PASS

  -LOG 3 set packet filter returned failure
  -LOG 3 SQLKEY:MACTB008.2 FAIL
  -JUMP Filter2

:PASS
  -LOG 3 Set Packet Filter was successful.

;check filter field in ss table
  -CheckMacTable SS 0A e 01 00
  -JPASS PASS

  -LOG 3 filter field in ss table not correct.
  -LOG 3 SQLKEY:MACTB008.2 FAIL
  -JUMP Filter2

:PASS
  -LOG 3 filter field in ss table was correct.
  -LOG 3 SQLKEY:MACTB008.2 PASS

:Filter2
;set packet filter to 2
  -SetPacketFilter 2,SUCCESS
  -JPASS PASS

  -LOG 3 Set Packet filter did'nt return a success.
  -LOG 3 SQLKEY:MACTB008.3 FAIL
  -JUMP Filter3

:PASS
  -LOG 3 Set Packet filter returned a success.

;check filter field in ss table
  -CheckMacTable SS 0A e 02 00
  -JPASS PASS

  -LOG 3 filter field in ss table was not correct.
  -LOG 3 SQLKEY:MACTB008.3 FAIL
  -JUMP Filter3

:PASS
  -LOG 3 filter field in ss table was correct.
  -LOG 3 SQLKEY:MACTB008.3 PASS

:Filter3
;set packet filter to 3
  -SetPacketFilter 3,SUCCESS
  -JPASS PASS

  -LOG 3 Set Packet Filter did not return a success
  -LOG 3 SQLKEY:MACTB008.4 FAIL
  -JUMP Filter7

:PASS
  -LOG 3 Set Packet Filter returned a success.

;check filter field in ss table
  -CheckMacTable SS 0A e 03 00
  -JPASS PASS

  -LOG 3 filter field in ss table was not correct
  -LOG 3 SQLKEY:MACTB008.4 FAIL
  -JUMP Filter7

:PASS
  -LOG 3 filter field in ss table was correct
  -LOG 3 SQLKEY:MACTB008.4 PASS

:Filter7
  -LOG 3 Check to see if promiscuous mode is supported
  -CheckMacTable SC 40 m 08
  -JFAIL ModeSppt

;set packet filter to 7
  -SetPacketFilter 7,SUCCESS
  -JPASS PASS

  -LOG 3 Set Packet Filter returned other than a success
  -JUMP Filter8

:PASS
  -LOG 3 Set Packet Filter returned a success

;check filter field in ss table
  -CheckMacTable SS 0A e 07 00
  -JPASS PASS

  -LOG 3 filter field in ss table was not correct
  -LOG 3 SQLKEY:MACTB008.5 FAIL
  -JUMP Filter8

:PASS
  -LOG 3 filter field in ss table was correct
  -LOG 3 SQLKEY:MACTB008.5 PASS
  -JUMP Filter8

:ModeSppt
  -LOG 3 Promiscuous Mode not supported

:Filter8
  ;check for source routing bridge support
  -CheckMacTable SC 41 m 20
  -JPASS SetFilter

  -LOG 3 Source routing bridge support is not supported
  -JUMP End

:SetFilter
  -LOG 3 Source routing bridge support is supported

  ;set packet filter to 8
  -SetPacketFilter 8,SUCCESS
  -JPASS PASS

  -LOG 3 Set Packet Filter did not return a success
  -LOG 3 SQLKEY:MACTB008.6 FAIL
  -JUMP End

:PASS
  -LOG 3 Set Packet Filter returned a success

  ;check filter field in ss table
  -CheckMacTable SS 0A e 08 00
  -JPASS PASS

  -LOG 3 filter field in ss table was not correct
  -LOG 3 SQLKEY:MACTB008.6 FAIL
  -JUMP End

:PASS
  -LOG 3 filter field in ss table was correct
  -LOG 3 SQLKEY:MACTB008.6 PASS

:End
  -SetPacketFilter 0
  -CloseAdapter
  -Wait 2


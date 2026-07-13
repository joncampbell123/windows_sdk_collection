;*****************************************************************************
;*
;*   File: SETPF001.S - Script file to test the SetPacketFilter primitive.
;*
;*   Description: Verify operation of SetPacketFilter under normal conditions.
;*                Tests include:
;*                   Set to 0
;*                     Should get no receive indications if slave sends:
;*                       broadcast packet
;*                       direct packet to ourselves
;*                       direct packet to somebody else
;*                       multicast packet
;*                       functional packet
;*                   Set to 1
;*                     Should get receive indications if slave sends:
;*                       direct packet to ourselves
;*                       multicast packet
;*                       functional packet
;*                     Should get no receive indications if slave sends:
;*                       direct packet to somebody else
;*                       broadcast packet
;*                   Set to 3
;*                     Should get receive indications if slave sends:
;*                       broadcast packet
;*                       direct packet to ourselves
;*                       multicast packet
;*                       functional packet
;*                     Should get no receive indications if slave sends:
;*                       direct packet to somebody else
;*                   Set to 4
;*                     Should get receive indications if slave sends:
;*                       broadcast packet
;*                       direct packet to ourselves
;*                       direct packet to somebody else
;*                       multicast packet
;*                       functional packet
;*
;*   Date: 6/5/90
;*
;*   Pre-conditions: Adapter is closed.
;*                   Slave computer is ready.
;*
;*   Post-conditions: Adapter is closed.
;*
;*****************************************************************************

  -LOG 3 Starting SetPacketFilter Tests

*** Check the Service Specific Characteristics Table to see if the
*** OpenAdapter command is supported by this MAC.
  -LOG 3 Check if OpenAdapter is supported.
  -CheckMacTable SC 41 m 08
  -JFAIL GetSlave

  -OpenAdapter,SUCCESS
  -JPASS GetSlave

   Quit script because state of MAC is uncertain.
  -LOG 3 OpenAdapter failed.  Exiting test.
  -JUMP abort

*** Identify slave computer.
:GetSlave
   Need to be able to see slave's response.
  -SetPacketFilter 1,SUCCESS
  -JFAIL SlaveFail

  -IDSLAVE,SUCCESS
  -JPASS AddMulticast

  -LOG 3 Unable to identify slave computer.  Exiting script.
  -JUMP abort

:SlaveFail
  -LOG 3 Unable to set packet filter to 1 to identify slave.
  -LOG 3 Exiting script.
  -JUMP abort

*** Set up multicast address for testing purposes.
:AddMulticast
  -LOG 3 Check that multicast or group addresses are supported.
  -CheckMacTable SC 40 m 06
  -JFAIL AddFunctional

:SetMulticast
  Check for token ring
  -CheckMacTable SC 2 e 38 30 32 2e 35
  -JFAIL EtherMC

  -AddMulticastAddress 80 00 03 04 05 06,SUCCESS
  -JPASS AddFunctional
  -JUMP AddMCFail

:EtherMC
  -AddMulticastAddress 01 02 03 04 05 06,SUCCESS
  -JPASS AddFunctional

:AddMCFail
  -LOG 3 Unable to add multicast address. Aborting tests.
  -JUMP abort

*** Set up functional address for testing purposes.
:AddFunctional
  -LOG 3 Check that functional addresses are supported.
  -CheckMacTable SC 40 m 04
  -JFAIL Filter0

  -SetFunctionalAddress 02 04 06 08,SUCCESS
  -JPASS Filter0

  -LOG 3 Unable to add functional address.  Aborting tests.
  -JUMP abort

*** Set packet filter to 0.  Test that direct, broadcast and multicast
*** packets do not generate a receive indication.
:Filter0
  -LOG 3 Testing SetPacketFilter with filter of 0.
  -SetPacketFilter 0,SUCCESS
  -JPASS Filter0OK

  -LOG 3 SetPacketFilter 0 did not return SUCCESS.
  -LOG 3 SQLKEY:SETPF001.1 FAIL
  -JUMP Filter1

:Filter0OK
  -LOG 3 SetPacketFilter 0 returned SUCCESS as expected.
  -LOG 3 SQLKEY:SETPF001.1 PASS

  -LOG 3 Testing that a direct packet sent to our address does not 
  -LOG 3 generate indications.
   Have slave send a directed packet.
  -CheckIntNum X
  -MRECV 1 100 100 0 0 0,SUCCESS
  -JFAIL Direct0Fail1

  -WAIT 01
  -CheckIntNum 0,SUCCESS
  -JFAIL Direct0Fail2

  -LOG 3 No indications received from direct packet as expected.
  -LOG 3 SQLKEY:SETPF001.2 PASS
  -JUMP Directedb0

:Direct0Fail1
  -LOG 3 Unable to tell slave to send direct packet.  Exiting script.
  -LOG 3 SQLKEY:SETPF001.2 FAIL
  -JUMP abort

:Direct0Fail2
  -LOG 3 Indication received for direct packet.  None expected.
  -LOG 3 SQLKEY:SETPF001.2 FAIL

:Directedb0
  -LOG 3 Testing that a direct packet sent ambiguous address does not 
  -LOG 3 generate indications.
   Have slave send a directed packet.
  -CheckIntNum X
  -MRECV 1 100 100 0 0 2 44 55 66 66 55 44,SUCCESS
  -JFAIL Direct0Fail1

  -WAIT 01
  -CheckIntNum 0,SUCCESS
  -JFAIL Direct0Fail2

  -LOG 3 No indications received from direct packet as expected.
  -LOG 3 SQLKEY:SETPF001.3 PASS
  -JUMP Broadcast0

:Direct0Fail1
  -LOG 3 Unable to tell slave to send direct packet.  Exiting script.
  -LOG 3 SQLKEY:SETPF001.3 FAIL
  -JUMP abort

:Direct0Fail2
  -LOG 3 Indication received for direct packet.  None expected.
  -LOG 3 SQLKEY:SETPF001.3 FAIL

:Broadcast0
  -LOG 3 Testing that a broadcast packet does not generate indications.
   First check that broadcast is supported.
  -CheckMacTable SC 40 m 01
  -JPASS RecBrdcast

  -LOG 3 Broadcast is not supported.

:RecBrdcast
   Have slave send a broadcast packet.
  -CheckIntNum X
  -MRECV 1 100 100 0 0 1,SUCCESS
  -JFAIL Broadcast0Fail2

  -WAIT 01
  -CheckIntNum 0,SUCCESS
  -JFAIL Broadcast0Fail3

  -LOG 3 No indications received from broadcast packet as expected.
  -LOG 3 SQLKEY:SETPF001.4 PASS
  -JUMP Multicast0

:Broadcast0Fail2
  -LOG 3 Unable to tell slave to send broadcast packet.  Exiting script.
  -LOG 3 SQLKEY:SETPF001.4 FAIL
  -JUMP abort

:Broadcast0Fail3
  -LOG 3 Indication received for broadcast packet.  None expected.
  -LOG 3 SQLKEY:SETPF001.4 FAIL

:Multicast0
  -LOG 3 Testing that a multicast packet does not generate indications.
  First check that multicast addresses are supported.
  -CheckMacTable SC 40 m 06
  -JPASS RecMulticast

  -LOG 3 Multicast addresses or group addresses are not supported.

:RecMulticast
  Have slave send packet to multicast address.
  -CheckIntNum X
  Check for token ring
  -CheckMacTable SC 2 e 38 30 32 2e 35
  -JFAIL EtherMC

  -MRECV 1 100 100 0 0 2 C0 00 83 04 05 06,SUCCESS
  -JFAIL Multicast0Fail2
  -JUMP MCContinue

:EtherMC
  -MRECV 1 100 100 0 0 2 01 02 03 04 05 06,SUCCESS
  -JFAIL Multicast0Fail2

:MCContinue
  -WAIT 01
  -CheckIntNum 0,SUCCESS
  -JFAIL Multicast0Fail3

  -LOG 3 No indications received from multicast packet as expected.
  -LOG 3 SQLKEY:SETPF001.5 PASS
  -JUMP Functional0

:Multicast0Fail2
  -LOG 3 Unable to tell slave to send multicast packet.  Exiting script.
  -LOG 3 SQLKEY:SETPF001.5 FAIL
  -JUMP abort

:Multicast0Fail3
  -LOG 3 Indication received for multicast packet.  None expected.
  -LOG 3 SQLKEY:SETPF001.5 FAIL

:Functional0
  -LOG 3 Testing that a functional packet does not generate indications.
  First check that functional addresses are supported.
  -CheckMacTable SC 40 m 04
  -JPASS RecFunctional

  -LOG 3 Functional addresses are not supported.

:RecFunctional
  Have slave send packet to functional address.
  -CheckIntNum X
  Check for token ring
  -CheckMacTable SC 2 e 38 30 32 2e 35
  -JFAIL EtherFC

  -MRECV 1 100 100 0 0 2 C0 00 00 00 04 00,SUCCESS
  -JFAIL Functional0Fail2
  -JUMP FCContinue

:EtherFC
  -MRECV 1 100 100 0 0 2 03 00 00 00 04 00,SUCCESS
  -JFAIL Functional0Fail2

:FCContinue
  -WAIT 01
  -CheckIntNum 0,SUCCESS
  -JFAIL Functional0Fail3

  -LOG 3 No indications received from functional packet as expected.
  -LOG 3 SQLKEY:SETPF001.6 PASS
  -JUMP Filter1

:Functional0Fail2
  -LOG 3 Unable to tell slave to send multicast packet.  Exiting script.
  -LOG 3 SQLKEY:SETPF001.6 FAIL
  -JUMP abort

:Functional0Fail3
  -LOG 3 Indication received for multicast packet.  None expected.
  -LOG 3 SQLKEY:SETPF001.6 FAIL

*** Set packet filter to 1.  Test that direct, multicast, and functional
*** packets generate a receive indication.  Test that broadcast
*** packets do not generate a receive indication.
:Filter1
  -LOG 3 Testing SetPacketFilter with filter of 1.
  -SetPacketFilter 1,SUCCESS
  -JPASS Filter1OK

  -LOG 3 SetPacketFilter 1 did not return SUCCESS.
  -LOG 3 SQLKEY:SETPF001.7 FAIL
  -JUMP Filter2

:Filter1OK
  -LOG 3 SetPacketFilter 1 returned SUCCESS as expected.
  -LOG 3 SQLKEY:SETPF001.7 PASS

  -LOG 3 Testing that a direct packet generates an indication.
   Have slave send a directed packet.
  -CheckIntNum X
  -MRECV 1 100 100 0 0 0,SUCCESS
  -JFAIL Direct1Fail1

  -WAIT 01
  -CheckIntNum 1,SUCCESS
  -JFAIL Direct1Fail2

  -LOG 3 Indication received from direct packet as expected.
  -LOG 3 SQLKEY:SETPF001.8 PASS
  -JUMP Directedb1

:Direct1Fail1
  -LOG 3 Unable to tell slave to send direct packet.  Exiting script.
  -LOG 3 SQLKEY:SETPF001.8 FAIL
  -JUMP abort

:Direct1Fail2
  -LOG 3 No indication received for direct packet.  One expected.
  -LOG 3 SQLKEY:SETPF001.8 FAIL

:Directedb1
  -LOG 3 Testing that a direct packet sent ambiguous address does not 
  -LOG 3 generate indications.
   Have slave send a directed packet.
  -CheckIntNum X
  -MRECV 1 100 100 0 0 2 44 55 66 66 55 44,SUCCESS
  -JFAIL Direct1Fail1

  -WAIT 01
  -CheckIntNum 0,SUCCESS
  -JFAIL Direct1Fail2

  -LOG 3 No indications received from direct packet as expected.
  -LOG 3 SQLKEY:SETPF001.9 PASS
  -JUMP Broadcast1

:Direct1Fail1
  -LOG 3 Unable to tell slave to send direct packet.  Exiting script.
  -LOG 3 SQLKEY:SETPF001.9 FAIL
  -JUMP abort

:Direct1Fail2
  -LOG 3 Indication received for direct packet.  None expected.
  -LOG 3 SQLKEY:SETPF001.9 FAIL

:Broadcast1
  -LOG 3 Testing that a broadcast packet does not generate indications.
   First check that broadcast is supported.
  -CheckMacTable SC 40 m 01
  -JPASS RecBrdcast

  -LOG 3 Broadcast is not supported.

:RecBrdcast
   Have slave send a broadcast packet.
  -CheckIntNum X
  -MRECV 1 100 100 0 0 1,SUCCESS
  -JFAIL Broadcast1Fail2

  -WAIT 01
  -CheckIntNum 0,SUCCESS
  -JFAIL Broadcast1Fail3

  -LOG 3 No indications received from broadcast packet as expected.
  -LOG 3 SQLKEY:SETPF001.10 PASS
  -JUMP Multicast1

:Broadcast1Fail2
  -LOG 3 Unable to tell slave to send broadcast packet.  Exiting script.
  -LOG 3 SQLKEY:SETPF001.10 FAIL
  -JUMP abort

:Broadcast1Fail3
  -LOG 3 Indication received for broadcast packet.  None expected.
  -LOG 3 SQLKEY:SETPF001.10 FAIL

:Multicast1
  -LOG 3 Testing that a multicast packet generates an indication.
  First check that multicast addresses are supported.
  -CheckMacTable SC 40 m 06
  -JFAIL Multicast1Fail1

  Have slave send packet to multicast address.
  -CheckIntNum X
  Check for token ring
  -CheckMacTable SC 2 e 38 30 32 2e 35
  -JFAIL EtherMC

  -MRECV 1 100 100 0 0 2 C0 00 83 04 05 06,SUCCESS
  -JFAIL Multicast1Fail2
  -JUMP MCContinue

:EtherMC
  -MRECV 1 100 100 0 0 2 01 02 03 04 05 06,SUCCESS
  -JFAIL Multicast1Fail2

:MCContinue
  -WAIT 01
  -CheckIntNum 1,SUCCESS
  -JFAIL Multicast1Fail3

  -LOG 3 One indication received from multicast packet as expected.
  -LOG 3 SQLKEY:SETPF001.11 PASS
  -JUMP Functional1

:Multicast1Fail1
  -LOG 3 Multicast addresses are not supported.
  -LOG 3 SQLKEY:SETPF001.11 PASS
  -JUMP Functional1

:Multicast1Fail2
  -LOG 3 Unable to tell slave to send multicast packet.
  -LOG 3 Exiting script.
  -LOG 3 SQLKEY:SETPF001.11 FAIL
  -JUMP abort

:Multicast1Fail3
  -LOG 3 No indication received for multicast packet.  One expected.
  -LOG 3 SQLKEY:SETPF001.11 FAIL

:Functional1
  -LOG 3 Testing that a functional packet generates an indication.
  First check that functional addresses are supported.
  -CheckMacTable SC 40 m 04
  -JFAIL Functional1Fail1

  Have slave send packet to functional address.
  -CheckIntNum X
  Check for token ring
  -CheckMacTable SC 2 e 38 30 32 2e 35
  -JFAIL EtherFC

  -MRECV 1 100 100 0 0 2 C0 00 00 00 04 00,SUCCESS
  -JFAIL Functional1Fail2
  -JUMP FCContinue

:EtherFC
  -MRECV 1 100 100 0 0 2 03 00 00 00 04 00,SUCCESS
  -JFAIL Functional1Fail2

:FCContinue
  -WAIT 01
  -CheckIntNum 1,SUCCESS
  -JFAIL Functional1Fail3

  -LOG 3 One indication received from functional packet as expected.
  -LOG 3 SQLKEY:SETPF001.12 PASS
  -JUMP Filter2

:Functional1Fail1
  -LOG 3 Functional addresses are not supported.
  -LOG 3 SQLKEY:SETPF001.12 PASS
  -JUMP Filter2

:Functional1Fail2
  -LOG 3 Unable to tell slave to send functional packet.  Exiting script.
  -LOG 3 SQLKEY:SETPF001.12 FAIL
  -JUMP abort

:Functional1Fail3
  -LOG 3 No indication received for functional packet.  One expected.
  -LOG 3 SQLKEY:SETPF001.12 FAIL

*** Set packet filter to 2.  Test that direct, multicast, and functional
*** packets do not generate a receive indication. Test that broadcast 
*** packets do generate receive indications. 
:Filter2
  -LOG 3 Testing SetPacketFilter with filter of 2.
  -SetPacketFilter 2,SUCCESS
  -JPASS Filter2OK

  -LOG 3 SetPacketFilter 2 did not return SUCCESS.
  -LOG 3 SQLKEY:SETPF001.13 FAIL
  -JUMP Filter3

:Filter2OK
  -LOG 3 SetPacketFilter 2 returned SUCCESS as expected.
  -LOG 3 SQLKEY:SETPF001.13 PASS

  -LOG 3 Testing that a direct packet sent to out address does not 
  -LOG 3 generate indications.
   Have slave send a directed packet.
  -CheckIntNum X
  -MRECV 1 100 100 0 0 0,SUCCESS
  -JFAIL Direct2Fail1

  -WAIT 01
  -CheckIntNum 0,SUCCESS
  -JFAIL Direct2Fail2

  -LOG 3 No indications received from direct packet as expected.
  -LOG 3 SQLKEY:SETPF001.14 PASS
  -JUMP Directedb2

:Direct2Fail1
  -LOG 3 Unable to tell slave to send direct packet.  Exiting script.
  -LOG 3 SQLKEY:SETPF001.14 FAIL
  -JUMP abort

:Direct2Fail2
  -LOG 3 Indication received for direct packet.  None expected.
  -LOG 3 SQLKEY:SETPF001.14 FAIL

:Directedb2
  -LOG 3 Testing that a direct packet sent ambiguous address does not 
  -LOG 3 generate indications.
   Have slave send a directed packet.
  -CheckIntNum X
  -MRECV 1 100 100 0 0 2 44 55 66 66 55 44,SUCCESS
  -JFAIL Direct2Fail1

  -WAIT 01
  -CheckIntNum 0,SUCCESS
  -JFAIL Direct2Fail2

  -LOG 3 No indications received from direct packet as expected.
  -LOG 3 SQLKEY:SETPF001.15 PASS
  -JUMP Broadcast2

:Direct2Fail1
  -LOG 3 Unable to tell slave to send direct packet.  Exiting script.
  -LOG 3 SQLKEY:SETPF001.15 FAIL
  -JUMP abort

:Direct2Fail2
  -LOG 3 Indication received for direct packet.  None expected.
  -LOG 3 SQLKEY:SETPF001.15 FAIL

:Broadcast2
  -LOG 3 Testing that a broadcast packet generates an indication.
   First check that broadcast is supported.
  -CheckMacTable SC 40 m 01
  -JFAIL Broadcast2Fail1

   Have slave send a broadcast packet.
  -CheckIntNum X
  -MRECV 1 100 100 0 0 1,SUCCESS
  -JFAIL Broadcast2Fail2

  -WAIT 01
  -CheckIntNum 1,SUCCESS
  -JFAIL Broadcast2Fail3

  -LOG 3 One indication received from broadcast packet as expected.
  -LOG 3 SQLKEY:SETPF001.16 PASS
  -JUMP Multicast2

:Broadcast2Fail1
  -LOG 3 Broadcast is not supported.
  -LOG 3 SQLKEY:SETPF001.16 PASS
  -JUMP Multicast2

:Broadcast2Fail2
  -LOG 3 Unable to tell slave to send broadcast packet.  Exiting script.
  -LOG 3 SQLKEY:SETPF001.16 FAIL
  -JUMP abort

:Broadcast2Fail3
  -LOG 3 No indication received for broadcast packet.  One expected.
  -LOG 3 SQLKEY:SETPF001.16 FAIL

:Multicast2
  -LOG 3 Testing that a multicast packet does not generate indications.
  First check that multicast addresses are supported.
  -CheckMacTable SC 40 m 06
  -JPASS RecMulticast

  -LOG 3 Multicast addresses or group addresses are not supported.

:RecMulticast
  Have slave send packet to multicast address.
  -CheckIntNum X
  Check for token ring
  -CheckMacTable SC 2 e 38 30 32 2e 35
  -JFAIL EtherMC

  -MRECV 1 100 100 0 0 2 C0 00 83 04 05 06,SUCCESS
  -JFAIL Multicast2Fail2
  -JUMP MCContinue

:EtherMC
  -MRECV 1 100 100 0 0 2 01 02 03 04 05 06,SUCCESS
  -JFAIL Multicast2Fail2

:MCContinue
  -WAIT 01
  -CheckIntNum 0,SUCCESS
  -JFAIL Multicast2Fail3

  -LOG 3 No indications received from multicast packet as expected.
  -LOG 3 SQLKEY:SETPF001.17 PASS
  -JUMP Functional2

:Multicast2Fail2
  -LOG 3 Unable to tell slave to send multicast packet.  Exiting script.
  -LOG 3 SQLKEY:SETPF001.17 FAIL
  -JUMP abort

:Multicast2Fail3
  -LOG 3 Indication received for multicast packet.  None expected.
  -LOG 3 SQLKEY:SETPF001.17 FAIL

:Functional2
  -LOG 3 Testing that a functional packet does not generate indications.
  First check that functional addresses are supported.
  -CheckMacTable SC 40 m 04
  -JPASS RecFunctional

  -LOG 3 Functional addresses are not supported.

:RecFunctional
  Have slave send packet to functional address.
  -CheckIntNum X
  Check for token ring
  -CheckMacTable SC 2 e 38 30 32 2e 35
  -JFAIL EtherFC

  -MRECV 1 100 100 0 0 2 C0 00 00 00 04 00,SUCCESS
  -JFAIL Functional2Fail2
  -JUMP FCContinue

:EtherFC
  -MRECV 1 100 100 0 0 2 03 00 00 00 04 00,SUCCESS
  -JFAIL Functional2Fail2

:FCContinue
  -WAIT 01
  -CheckIntNum 0,SUCCESS
  -JFAIL Functional2Fail3

  -LOG 3 No indications received from functional packet as expected.
  -LOG 3 SQLKEY:SETPF001.18 PASS
  -JUMP Filter3

:Functional2Fail2
  -LOG 3 Unable to tell slave to send multicast packet.  Exiting script.
  -LOG 3 SQLKEY:SETPF001.18 FAIL
  -JUMP abort

:Functional2Fail3
  -LOG 3 Indication received for multicast packet.  None expected.
  -LOG 3 SQLKEY:SETPF001.18 FAIL

*** Set packet filter to 3.  Test that direct, broadcast and multicast
*** packets generate a receive indication.
:Filter3
  -LOG 3 Testing SetPacketFilter with filter of 3.
  -SetPacketFilter 3,SUCCESS
  -JPASS Filter3OK

  -LOG 3 SetPacketFilter 3 did not return SUCCESS.  Exiting script.
  -LOG 3 SQLKEY:SETPF001.19 FAIL
  -JUMP Filter4

:Filter3OK
  -LOG 3 SetPacketFilter 3 returned SUCCESS as expected.
  -LOG 3 SQLKEY:SETPF001.19 PASS

  -LOG 3 Testing that a direct packet generates an indication.
   Have slave send a directed packet.
  -CheckIntNum X
  -MRECV 1 100 100 0 0 0,SUCCESS
  -JFAIL Direct3Fail1

  -WAIT 01
  -CheckIntNum 1,SUCCESS
  -JFAIL Direct3Fail2

  -LOG 3 One indication received from direct packet as expected.
  -LOG 3 SQLKEY:SETPF001.20 PASS
  -JUMP Directedb3

:Direct3Fail1
  -LOG 3 Unable to tell slave to send direct packet.  Exiting script.
  -LOG 3 SQLKEY:SETPF001.20 FAIL
  -JUMP abort

:Direct3Fail2
  -LOG 3 No indication received for direct packet.  One expected.
  -LOG 3 SQLKEY:SETPF001.20 FAIL

:Directedb3
  -LOG 3 Testing that a direct packet sent ambiguous address does not 
  -LOG 3 generate indications.
   Have slave send a directed packet.
  -CheckIntNum X
  -MRECV 1 100 100 0 0 2 44 55 66 66 55 44,SUCCESS
  -JFAIL Direct3Fail1

  -WAIT 01
  -CheckIntNum 0,SUCCESS
  -JFAIL Direct3Fail2

  -LOG 3 No indications received from direct packet as expected.
  -LOG 3 SQLKEY:SETPF001.21 PASS
  -JUMP Broadcast3

:Direct3Fail1
  -LOG 3 Unable to tell slave to send direct packet.  Exiting script.
  -LOG 3 SQLKEY:SETPF001.21 FAIL
  -JUMP abort

:Direct3Fail2
  -LOG 3 Indication received for direct packet.  None expected.
  -LOG 3 SQLKEY:SETPF001.21 FAIL

:Broadcast3
  -LOG 3 Testing that a broadcast packet generates an indication.
   First check that broadcast is supported.
  -CheckMacTable SC 40 m 01
  -JFAIL Broadcast3Fail1

   Have slave send a broadcast packet.
  -CheckIntNum X
  -MRECV 1 100 100 0 0 1,SUCCESS
  -JFAIL Broadcast3Fail2

  -WAIT 01
  -CheckIntNum 1,SUCCESS
  -JFAIL Broadcast3Fail3

  -LOG 3 One indication received from broadcast packet as expected.
  -LOG 3 SQLKEY:SETPF001.22 PASS
  -JUMP Multicast3

:Broadcast3Fail1
  -LOG 3 Broadcast is not supported.
  -LOG 3 SQLKEY:SETPF001.22 PASS
  -JUMP Multicast3

:Broadcast3Fail2
  -LOG 3 Unable to tell slave to send broadcast packet.  Exiting script.
  -LOG 3 SQLKEY:SETPF001.22 FAIL
  -JUMP abort

:Broadcast3Fail3
  -LOG 3 No indication received for broadcast packet.  One expected.
  -LOG 3 SQLKEY:SETPF001.22 FAIL

:Multicast3
  -LOG 3 Testing that a multicast packet generates an indication.
   First check that multicast addresses are supported.
  -CheckMacTable SC 40 m 06
  -JFAIL Multicast3Fail1

   Have slave send packet to multicast address.
  -CheckIntNum X
  Check for token ring
  -CheckMacTable SC 2 e 38 30 32 2e 35
  -JFAIL EtherMC

  -MRECV 1 100 100 0 0 2 C0 00 83 04 05 06,SUCCESS
  -JFAIL Multicast3Fail2
  -JUMP MCContinue

:EtherMC
  -MRECV 1 100 100 0 0 2 01 02 03 04 05 06,SUCCESS
  -JFAIL Multicast3Fail2

:MCContinue
  -WAIT 01
  -CheckIntNum 1,SUCCESS
  -JFAIL Multicast3Fail3

  -LOG 3 One indication received from multicast packet as expected.
  -LOG 3 SQLKEY:SETPF001.23 PASS
  -JUMP Functional3

:Multicast3Fail1
  -LOG 3 Multicast addresses are not supported.
  -LOG 3 SQLKEY:SETPF001.23 PASS
  -JUMP Functional3

:Multicast3Fail2
  -LOG 3 Unable to tell slave to send multicast packet.  Exiting script.
  -LOG 3 SQLKEY:SETPF001.23 FAIL
  -JUMP abort

:Multicast3Fail3
  -LOG 3 No indication received for multicast packet.  One expected.
  -LOG 3 SQLKEY:SETPF001.23 FAIL

:Functional3
  -LOG 3 Testing that a functional packet generates an indication.
  First check that functional addresses are supported.
  -CheckMacTable SC 40 m 04
  -JFAIL Functional3Fail1

  Have slave send packet to functional address.
  -CheckIntNum X
  Check for token ring
  -CheckMacTable SC 2 e 38 30 32 2e 35
  -JFAIL EtherFC

  -MRECV 1 100 100 0 0 2 C0 00 00 00 04 00,SUCCESS
  -JFAIL Functional3Fail2
  -JUMP FCContinue

:EtherFC
  -MRECV 1 100 100 0 0 2 03 00 00 00 04 00,SUCCESS
  -JFAIL Functional3Fail2

:FCContinue
  -WAIT 01
  -CheckIntNum 1,SUCCESS
  -JFAIL Functional3Fail3

  -LOG 3 One indication received from functional packet as expected.
  -LOG 3 SQLKEY:SETPF001.24 PASS
  -JUMP Filter4

:Functional3Fail1
  -LOG 3 Functional addresses are not supported.
  -LOG 3 SQLKEY:SETPF001.24 PASS
  -JUMP Filter4

:Functional3Fail2
  -LOG 3 Unable to tell slave to send functional packet.  Exiting script.
  -LOG 3 SQLKEY:SETPF001.24 FAIL
  -JUMP abort

:Functional3Fail3
  -LOG 3 No indication received for functional packet.  One expected.
  -LOG 3 SQLKEY:SETPF001.24 FAIL

*** Set packet filter to 4, Promisuous mode.  Test that direct, 
*** broadcast and multicast packets generate a receive indication.
:Filter4
  -LOG 3 Test if Promicuous mode supported.
  -CheckMacTable SC 40 m 08
  -JPASS SetFilter4

  -LOG 3 Promicuous mode supported not supported.
  -JUMP end

:SetFilter4
  -LOG 3 Testing SetPacketFilter with filter of 4.
  -SetPacketFilter 4,SUCCESS
  -JPASS Filter4OK

  -LOG 3 SetPacketFilter 4 did not return SUCCESS.  Exiting script.
  -LOG 3 SQLKEY:SETPF001.25 FAIL
  -JUMP end

:Filter4OK
  -LOG 3 SetPacketFilter 4 returned SUCCESS as expected.
  -LOG 3 SQLKEY:SETPF001.25 PASS

  -LOG 3 Testing that a direct packet generates an indication.
   Have slave send a directed packet.
  -CheckIntNum X
  -MRECV 1 100 100 0 0 0,SUCCESS
  -JFAIL Direct4Fail1

  -WAIT 01
  -CheckIntNum 1,SUCCESS
  -JFAIL Direct4Fail2

  -LOG 3 One indication received from direct packet as expected.
  -LOG 3 SQLKEY:SETPF001.26 PASS
  -JUMP Directedb4

:Direct4Fail1
  -LOG 3 Unable to tell slave to send direct packet.  Exiting script.
  -LOG 3 SQLKEY:SETPF001.26 FAIL
  -JUMP abort

:Direct4Fail2
  -LOG 3 No indication received for direct packet.  One expected.
  -LOG 3 SQLKEY:SETPF001.26 FAIL

:Directedb4
  -LOG 3 Testing that a direct packet sent ambiguous address generates 
  -LOG 3 indications.
   Have slave send a directed packet.
  -CheckIntNum X
  -MRECV 1 100 100 0 0 2 44 55 66 66 55 44,SUCCESS
  -JFAIL Direct4Fail1

  -WAIT 01
  -CheckIntNum 1,SUCCESS
  -JFAIL Direct4Fail2

  -LOG 3 An indication received from direct packet as expected.
  -LOG 3 SQLKEY:SETPF001.27 PASS
  -JUMP Broadcast4

:Direct4Fail1
  -LOG 3 Unable to tell slave to send direct packet.  Exiting script.
  -LOG 3 SQLKEY:SETPF001.27 FAIL
  -JUMP abort

:Direct4Fail2
  -LOG 3 No indication received for direct packet.  One expected.
  -LOG 3 SQLKEY:SETPF001.27 FAIL

:Broadcast4
  -LOG 3 Testing that a broadcast packet generates an indication.
   First check that broadcast is supported.
  -CheckMacTable SC 40 m 01
  -JFAIL Broadcast4Fail1

   Have slave send a broadcast packet.
  -CheckIntNum X
  -MRECV 1 100 100 0 0 1,SUCCESS
  -JFAIL Broadcast4Fail2

  -WAIT 01
  -CheckIntNum 1,SUCCESS
  -JFAIL Broadcast4Fail3

  -LOG 3 One indication received from broadcast packet as expected.
  -LOG 3 SQLKEY:SETPF001.28 PASS
  -JUMP Multicast4

:Broadcast4Fail1
  -LOG 3 Broadcast is not supported.
  -LOG 3 SQLKEY:SETPF001.28 PASS
  -JUMP Multicast4

:Broadcast4Fail2
  -LOG 3 Unable to tell slave to send broadcast packet.  Exiting script.
  -LOG 3 SQLKEY:SETPF001.28 FAIL
  -JUMP abort

:Broadcast4Fail3
  -LOG 3 No indication received for broadcast packet.  One expected.
  -LOG 3 SQLKEY:SETPF001.28 FAIL

:Multicast4
  -LOG 3 Testing that a multicast packet generates an indication.
   First check that multicast addresses are supported.
  -CheckMacTable SC 40 m 06
  -JFAIL Multicast4Fail1

   Have slave send packet to multicast address.
  -CheckIntNum X
  Check for token ring
  -CheckMacTable SC 2 e 38 30 32 2e 35
  -JFAIL EtherMC

  -MRECV 1 100 100 0 0 2 C0 00 83 04 05 06,SUCCESS
  -JFAIL Multicast4Fail2
  -JUMP MCContinue

:EtherMC
  -MRECV 1 100 100 0 0 2 01 02 03 04 05 06,SUCCESS
  -JFAIL Multicast4Fail2

:MCContinue
  -WAIT 01
  -CheckIntNum 1,SUCCESS
  -JFAIL Multicast4Fail3

  -LOG 3 One indication received from multicast packet as expected.
  -LOG 3 SQLKEY:SETPF001.29 PASS
  -JUMP Functional4

:Multicast4Fail1
  -LOG 3 Multicast addresses are not supported.
  -LOG 3 SQLKEY:SETPF001.29 PASS
  -JUMP Functional4

:Multicast4Fail2
  -LOG 3 Unable to tell slave to send multicast packet.  Exiting script.
  -LOG 3 SQLKEY:SETPF001.29 FAIL
  -JUMP abort

:Multicast4Fail3
  -LOG 3 No indication received for multicast packet.  One expected.
  -LOG 3 SQLKEY:SETPF001.29 FAIL

:Functional4
  -LOG 3 Testing that a functional packet generates an indication.
  First check that functional addresses are supported.
  -CheckMacTable SC 40 m 04
  -JFAIL Functional4Fail1

  Have slave send packet to functional address.
  -CheckIntNum X
  Check for token ring
  -CheckMacTable SC 2 e 38 30 32 2e 35
  -JFAIL EtherFC

  -MRECV 1 100 100 0 0 2 C0 00 00 00 04 00,SUCCESS
  -JFAIL Functional4Fail2
  -JUMP FCContinue

:EtherFC
  -MRECV 1 100 100 0 0 2 03 00 00 00 04 00,SUCCESS
  -JFAIL Functional4Fail2

:FCContinue
  -WAIT 01
  -CheckIntNum 1,SUCCESS
  -JFAIL Functional4Fail3

  -LOG 3 One indication received from functional packet as expected.
  -LOG 3 SQLKEY:SETPF001.30 PASS
  -JUMP end

:Functional4Fail1
  -LOG 3 Functional addresses are not supported.
  -LOG 3 SQLKEY:SETPF001.30 PASS
  -JUMP end

:Functional4Fail2
  -LOG 3 Unable to tell slave to send functional packet.  Exiting script.
  -LOG 3 SQLKEY:SETPF001.30 FAIL
  -JUMP abort

:Functional4Fail3
  -LOG 3 No indication received for functional packet.  One expected.
  -LOG 3 SQLKEY:SETPF001.30 FAIL
  -JUMP end

:abort
  -LOG 3 Aborting SETPF001 because state of MAC is uncertain.
  -SCRIPT ABORT ABORT
  -SetFailure
  -JUMP end2

:end
  -LOG 3 Remove Multicast address.
  Check if multicast supported.
  -CheckMacTable SC 40 m 02
  -JPASS DelMulticast
  Check if group supported.
  -CheckMacTable SC 40 m 04
  -JFAIL ChkFunctional

:DelMulticast
  Check for token ring
  -CheckMacTable SC 2 e 38 30 32 2e 35
  -JFAIL EtherMC

  -DelMulticastAddress 80 00 03 04 05 06,SUCCESS
  -JFAIL abort2
  -JUMP CleanupFilter

:EtherMC
  -DelMulticastAddress 01 02 03 04 05 06,SUCCESS

:ChkFunctional
  -LOG 3 Remove Functional address.
  Check if functional supported.
  -CheckMacTable SC 40 m 04
  -JFAIL CleanupFilter

  -SetFunctionalAddress 00 00 00 00,SUCCESS
  -JFAIL abort2

:CleanupFilter
  -SetPacketFilter 0,SUCCESS

  -LOG 3 Finished SetPacketFilter Tests

*** Check the Service Specific Characteristics Table to see if the
*** CloseAdapter command is supported by this MAC.
  -LOG 3 Check if CloseAdapter is supported.
  -CheckMacTable SC 41 m 08
  -JFAIL end2

  -CloseAdapter,SUCCESS
  -JPASS end2

   Quit script because state of MAC is uncertain.
  -LOG 3 CloseAdapter failed.  Exiting test.

:abort2
  -LOG 3 Aborting SETPF001 because state of MAC is uncertain.
  -SCRIPT ABORT ABORT

:end2


#	-SECTION_START-( 5.00 ) -SECTION_DESC-( "Stress Tests" )
##
## TITLE: 2.1.2.2.5 Stress Tests (1M/2C/1O)
##
## 1 Machine - 2 Card - 1 Open Instance on the card.  These tests
## will open the test card once and the trusted card once and run
## run a Stress client on the trusted card and a Stress server on the
## test card.

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#	-SECTION_START-( 5.01 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 5.01 )

# Then open the card.

#	-SECTION_START-( 5.02 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 5.02 )

#	-SECTION_START-( 5.03 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED
#	-SECTION_END-( 5.03 )

########################################################

# Wait for the server side to tell us its ready

#	-SECTION_START-( 5.04 )
Pause                                           +
    OpenInstance=1                              +
    RemoteAddress=%TP_REM_TRUSTED_CARD_ADDRESS% +
    TestSignature=212251
#	-SECTION_END-( 5.04 )

# and start the Stress client.

#	-SECTION_START-( 5.05 )
Stress                                          +
    OpenInstance=1                              +
    MemberType=TP_CLIENT                        +
    PacketType=CYCLICAL                         +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    PacketMakeUp=RAND                           +
    ResponseType=FULL_RESPONSE                  +
    Iterations=10                               + # 10 times MaxFrameSize packets sent.
    Packets=-1                                  +
    DelayType=FIXEDDELAY                        +
    DelayLength=0                               +
    WindowEnabled=TRUE                          +
    DataChecking=TRUE                           +
    PacketPool=FALSE
#	-SECTION_END-( 5.05 )

########################################################

# Then wait for the stress test to end,

#	-SECTION_START-( 5.06 )
WaitStress                                      +
    OpenInstance=1
#	-SECTION_END-( 5.06 )

# once the Stress test has finished, tell the server side to
# end the stress test also.

#	-SECTION_START-( 5.07 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED
#	-SECTION_END-( 5.07 )

#	-SECTION_START-( 5.08 )
Go                                              +
    OpenInstance=1                              +
    RemoteAddress=%TP_REM_TRUSTED_CARD_ADDRESS% +
    TestSignature=212252
#	-SECTION_END-( 5.08 )

########################################################

# Finally close the adapter, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 5.09 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 5.09 )

#	-SECTION_START-( 5.10 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 5.10 )

#	-SECTION_END-( 5.00 )

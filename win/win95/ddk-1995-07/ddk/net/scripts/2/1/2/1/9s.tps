#	-SECTION_START-( 9.00 ) -SECTION_DESC-( "Stress Tests" )
##
## TITLE: 2.1.2.1.9 Stress Tests (1M/2C/1O)
##
## 1 Machine - 2 Card - 1 Open Instance on the card.  These tests
## will open the test card once and the trusted card once and run
## run a Stress client on the test card and a Stress server on the
## trusted card.

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#	-SECTION_START-( 9.01 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 9.01 )

# Then open the card.

#	-SECTION_START-( 9.02 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TRUSTED_CARD%
#	-SECTION_END-( 9.02 )

#	-SECTION_START-( 9.03 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED
#	-SECTION_END-( 9.03 )

########################################################

# Wait for the server side to tell us its ready

#	-SECTION_START-( 9.04 )
Pause                                           +
    OpenInstance=1                              +
    RemoteAddress=%TP_REM_TEST_CARD_ADDRESS%    +
    TestSignature=212191
#	-SECTION_END-( 9.04 )

# and start the Stress client.

#	-SECTION_START-( 9.05 )
Stress                                          +
    OpenInstance=1                              +
    MemberType=TP_CLIENT                        +
    PacketType=RANDOMSIZE                       +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    PacketMakeUp=RAND                           +
    ResponseType=FULL_RESPONSE                  +
    Iterations=-1                               +
    Packets=10000                               +
    DelayType=FIXEDDELAY                        +
    DelayLength=0                               +
    WindowEnabled=FALSE                         +
    DataChecking=TRUE                           +
    PacketPool=TRUE
#	-SECTION_END-( 9.05 )

########################################################

# Then wait for the stress test to end,

#	-SECTION_START-( 9.06 )
WaitStress                                      +
    OpenInstance=1
#	-SECTION_END-( 9.06 )

# once the Stress test has finished, tell the server side to
# end the stress test also.

#	-SECTION_START-( 9.07 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED
#	-SECTION_END-( 9.07 )

#	-SECTION_START-( 9.08 )
Go                                              +
    OpenInstance=1                              +
    RemoteAddress=%TP_REM_TEST_CARD_ADDRESS%    +
    TestSignature=212192
#	-SECTION_END-( 9.08 )

########################################################

# Finally close the adapter, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 9.09 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 9.09 )

#	-SECTION_START-( 9.10 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 9.10 )

#	-SECTION_END-( 9.00 )

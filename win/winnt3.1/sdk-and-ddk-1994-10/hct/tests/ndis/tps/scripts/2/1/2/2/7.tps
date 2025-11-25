#	-SECTION_START-( 7.00 ) -SECTION_DESC-( "Stress Tests" )
##
## TITLE: 2.1.2.2.7 Stress Tests (1M/2C/1O)
##
## 1 Machine - 2 Card - 1 Open Instance on the card.  These tests
## will open the test card once and the trusted card once and run
## run a Stress client on the trusted card and a Stress server on the
## test card.

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#	-SECTION_START-( 7.01 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 7.01 )

# Then open the card.

#	-SECTION_START-( 7.02 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 7.02 )

#	-SECTION_START-( 7.03 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED
#	-SECTION_END-( 7.03 )

########################################################

# Wait for the server side to tell us its ready

#	-SECTION_START-( 7.04 )
Pause                                           +
    OpenInstance=1                              +
    RemoteAddress=%TP_REM_TRUSTED_CARD_ADDRESS% +
    TestSignature=212271
#	-SECTION_END-( 7.04 )

# and start the Stress client.

#	-SECTION_START-( 7.05 )
Stress                                          +
    OpenInstance=1                              +
    MemberType=TP_CLIENT                        +
    PacketType=RANDOMSIZE                       +
    PacketSize=256                              +
    PacketMakeUp=SMALL                          +
    ResponseType=ACK_10_TIMES                   +
    Iterations=-1                               +
    Packets=10000                               +
    DelayType=FIXEDDELAY                        +
    DelayLength=0                               +
    WindowEnabled=TRUE                          +
    DataChecking=TRUE                           +
    PacketPool=FALSE
#	-SECTION_END-( 7.05 )

########################################################

# Then wait for the stress test to end,

#	-SECTION_START-( 7.06 )
WaitStress                                      +
    OpenInstance=1
#	-SECTION_END-( 7.06 )

# once the Stress test has finished, tell the server side to
# end the stress test also.

#	-SECTION_START-( 7.07 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED
#	-SECTION_END-( 7.07 )

#	-SECTION_START-( 7.08 )
Go                                              +
    OpenInstance=1                              +
    RemoteAddress=%TP_REM_TRUSTED_CARD_ADDRESS% +
    TestSignature=212272
#	-SECTION_END-( 7.08 )

########################################################

# Finally close the adapter, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 7.09 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 7.09 )

#	-SECTION_START-( 7.10 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 7.10 )

#	-SECTION_END-( 7.00 )

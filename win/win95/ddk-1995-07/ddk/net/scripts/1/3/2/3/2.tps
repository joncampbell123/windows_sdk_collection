#	-SECTION_START-( 2.00 ) -SECTION_DESC-( "Stress Tests" )
##
## TITLE: 1.3.2.3.2 Stress Tests (1M/2C/1O)
##
## 1 Machine - 2 Card - 1 Open Instance on the card.  These tests
## will open the test card twice and run a Stress client on one of
## the open instances and a Stress server on the other

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#	-SECTION_START-( 2.01 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 2.01 )

#	-SECTION_START-( 2.02 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 2.02 )

# Then open the card.

#	-SECTION_START-( 2.03 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 2.03 )

#	-SECTION_START-( 2.04 )
Open                                            +
    OpenInstance=2                              +
    AdapterName=%TP_TRUSTED_CARD%
#	-SECTION_END-( 2.04 )

########################################################

# Then start the Stress Server,

#	-SECTION_START-( 2.05 )
StressServer                                    +
     OpenInstance=2
#	-SECTION_END-( 2.05 )

# And the Stress client.

#	-SECTION_START-( 2.06 )
Stress                                          +
    OpenInstance=1                              +
    MemberType=BOTH                             +
    PacketType=RANDOMSIZE                       +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    PacketMakeUp=ONES                           +
    ResponseType=ACK                            +
    Iterations=-1                               +
    Packets=10000                               +
    DelayType=FIXEDDELAY                        +
    DelayLength=0                               +
    WindowEnabled=TRUE                          +
    DataChecking=TRUE                           +
    PacketPool=FALSE
#	-SECTION_END-( 2.06 )

########################################################

# Then wait for the stress test to end,

#	-SECTION_START-( 2.07 )
WaitStress                                      +
    OpenInstance=1
#	-SECTION_END-( 2.07 )

# And stop the Stress Server

#	-SECTION_START-( 2.08 )
EndStress                                       +
    OpenInstance=2
#	-SECTION_END-( 2.08 )

########################################################

# Finally close the adapter, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 2.09 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 2.09 )

#	-SECTION_START-( 2.10 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 2.10 )

#	-SECTION_START-( 2.11 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 2.11 )

#	-SECTION_START-( 2.12 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 2.12 )

#	-SECTION_END-( 2.00 )

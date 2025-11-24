#	-SECTION_START-( 7.00 ) -SECTION_DESC-( "Stress Tests" )
##
## TITLE: 1.3.2.2.7 Stress Tests (1M/2C/1O)
##
## 1 Machine - 2 Card - 1 Open Instance on the card.  These tests
## will open the test card once and the trusted card once and run
## run a Stress client on the test card and a Stress server on the
## trusted card.

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#	-SECTION_START-( 7.01 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 7.01 )

#	-SECTION_START-( 7.02 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 7.02 )

# Then open the card.

#	-SECTION_START-( 7.03 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 7.03 )

#	-SECTION_START-( 7.04 )
Open                                            +
    OpenInstance=2                              +
    AdapterName=%TP_TRUSTED_CARD%
#	-SECTION_END-( 7.04 )

########################################################

# Then start the Stress Server,

#	-SECTION_START-( 7.05 )
StressServer                                    +
     OpenInstance=2
#	-SECTION_END-( 7.05 )

# And the Stress client.

#	-SECTION_START-( 7.06 )
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
#	-SECTION_END-( 7.06 )

########################################################

# Then wait for the stress test to end,

#	-SECTION_START-( 7.07 )
WaitStress                                      +
    OpenInstance=1
#	-SECTION_END-( 7.07 )

# And stop the Stress Server

#	-SECTION_START-( 7.08 )
EndStress                                       +
    OpenInstance=2
#	-SECTION_END-( 7.08 )

########################################################

# Finally close the adapter, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 7.09 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 7.09 )

#	-SECTION_START-( 7.10 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 7.10 )

#	-SECTION_START-( 7.11 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 7.11 )

#	-SECTION_START-( 7.12 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 7.12 )

#	-SECTION_END-( 7.00 )

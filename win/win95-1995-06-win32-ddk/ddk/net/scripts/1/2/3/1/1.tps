#	-SECTION_START-( 1.00 ) -SECTION_DESC-( "Stress Tests" )
##
## TITLE: 1.2.3.1.1 Stress Tests (1M/1C/2O)
##
## 1 Machine - 1 Card - 2 Open Instances on the card.  These tests
## will open the test card twice and run a Stress client on one of
## the open instances and a Stress server on the other

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#   -SECTION_START-()
#	-SECTION_START-( 1.01 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 1.01 )
#   -SECTION_END-()

#   -SECTION_START-()
#	-SECTION_START-( 1.02 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 1.02 )
#   -SECTION_END-()

# Then open the card.

#   -SECTION_START-()
#	-SECTION_START-( 1.03 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 1.03 )
#   -SECTION_END-()

#	-SECTION_START-( 1.04 )
Open                                            +
    OpenInstance=2                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 1.04 )

########################################################

# Then start the Stress Server,

#	-SECTION_START-( 1.05 )
StressServer                                    +
     OpenInstance=2
#	-SECTION_END-( 1.05 )

# And the Stress client.

#	-SECTION_START-( 1.06 )
Stress                                          +
    OpenInstance=1                              +
    MemberType=TP_CLIENT                        +
    PacketType=RANDOMSIZE                       +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    PacketMakeUp=ZEROS                          +
    ResponseType=ACK                            +
    Iterations=-1                               +
    Packets=10000                               +
    DelayType=FIXEDDELAY                        +
    DelayLength=0                               +
    WindowEnabled=TRUE                          +
    DataChecking=TRUE                           +
    PacketPool=FALSE
#	-SECTION_END-( 1.06 )

########################################################

# Then wait for the stress test to end,

#	-SECTION_START-( 1.07 )
WaitStress                                      +
    OpenInstance=1
#	-SECTION_END-( 1.07 )

# And stop the Stress Server

#	-SECTION_START-( 1.08 )
EndStress                                       +
    OpenInstance=2
#	-SECTION_END-( 1.08 )

########################################################

# Finally close the adapter, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 1.09 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 1.09 )

#	-SECTION_START-( 1.10 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 1.10 )

#	-SECTION_START-( 1.11 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 1.11 )

#   -SECTION_START-()
#	-SECTION_START-( 1.12 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 1.12 )

#	-SECTION_END-( 1.00 )

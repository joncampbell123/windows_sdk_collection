#	-SECTION_START-( 9.00 ) -SECTION_DESC-( "Stress Tests" )
##
## TITLE: 1.2.3.1.9 Stress Tests (1M/1C/2O)
##
## 1 Machine - 1 Card - 2 Open Instances on the card.  These tests
## will open the test card twice and run a Stress client on one of
## the open instances and a Stress server on the other

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#	-SECTION_START-( 9.01 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 9.01 )

#	-SECTION_START-( 9.02 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 9.02 )

# Then open the card.

#	-SECTION_START-( 9.03 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 9.03 )

#	-SECTION_START-( 9.04 )
Open                                            +
    OpenInstance=2                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 9.04 )

########################################################

# Then start the Stress Server,

#	-SECTION_START-( 9.05 )
StressServer                                    +
     OpenInstance=2
#	-SECTION_END-( 9.05 )

# And the Stress client.

#	-SECTION_START-( 9.06 )
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
#	-SECTION_END-( 9.06 )

########################################################

# Then wait for the stress test to end,

#	-SECTION_START-( 9.07 )
WaitStress                                      +
    OpenInstance=1
#	-SECTION_END-( 9.07 )

# And stop the Stress Server

#	-SECTION_START-( 9.08 )
EndStress                                       +
    OpenInstance=2
#	-SECTION_END-( 9.08 )

########################################################

# Finally close the adapter, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 9.09 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 9.09 )

#	-SECTION_START-( 9.10 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 9.10 )

#	-SECTION_START-( 9.11 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 9.11 )

#	-SECTION_START-( 9.12 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 9.12 )

#	-SECTION_END-( 9.00 )

#	-SECTION_START-( 5.00 ) -SECTION_DESC-( "Stress Tests" )
##
## TITLE: 1.2.3.2.5 Stress Tests (1M/1C/2O)
##
## 1 Machine - 1 Card - 2 Open Instances on the card.  These tests
## will open the test card twice and run a Stress client on one of
## the open instances and a Stress server on the other

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#	-SECTION_START-( 5.01 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 5.01 )

#	-SECTION_START-( 5.02 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 5.02 )

# Then open the card.

#	-SECTION_START-( 5.03 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 5.03 )

#	-SECTION_START-( 5.04 )
Open                                            +
    OpenInstance=2                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 5.04 )

########################################################

# Then start the Stress Server,

#	-SECTION_START-( 5.05 )
StressServer                                    +
     OpenInstance=2
#	-SECTION_END-( 5.05 )

# And the Stress client.

#	-SECTION_START-( 5.06 )
Stress                                          +
    OpenInstance=1                              +
    MemberType=BOTH                             +
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
#	-SECTION_END-( 5.06 )

########################################################

# Then wait for the stress test to end,

#	-SECTION_START-( 5.07 )
WaitStress                                      +
    OpenInstance=1
#	-SECTION_END-( 5.07 )

# And stop the Stress Server

#	-SECTION_START-( 5.08 )
EndStress                                       +
    OpenInstance=2
#	-SECTION_END-( 5.08 )

########################################################

# Finally close the adapter, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 5.09 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 5.09 )

#	-SECTION_START-( 5.10 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 5.10 )

#	-SECTION_START-( 5.11 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 5.11 )

#	-SECTION_START-( 5.12 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 5.12 )

#	-SECTION_END-( 5.00 )

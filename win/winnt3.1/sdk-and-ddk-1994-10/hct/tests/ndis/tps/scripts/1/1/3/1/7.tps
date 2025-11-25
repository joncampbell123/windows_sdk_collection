#   -SECTION_START-( 7.0 ) -SECTION_DESC-( "Stress Tests" )
##
## TITLE: 1.1.3.1.7 Stress Tests (1M/1C/1O)
##
## 1 Machine - 1 Card - 1 Open Instances on the card.  These tests
## will open the test card once and run both a Stress client and
## a Stress server on the same OpenInstance.

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#   -SECTION_START-( 7.01 )
GetEvents                                       +
    OpenInstance=1
#   -SECTION_END-  ( 7.01 )

# Then open the card.

#   -SECTION_START-( 7.02 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%
#   -SECTION_END-  ( 7.02 )

########################################################

# And the Stress client.

#   -SECTION_START-( 7.03 )
Stress                                          +
    OpenInstance=1                              +
    MemberType=BOTH                             +
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
#   -SECTION_END-  ( 7.03 )

########################################################

# Then wait for the stress test to end,

#   -SECTION_START-( 7.04 )
WaitStress                                      +
    OpenInstance=1
#   -SECTION_END-  ( 7.04 )

########################################################

# Finally close the adapter, and again dump the Event
# Queue for any unexpected events.

#   -SECTION_START-( 7.05 )
GetEvents                                       +
    OpenInstance=1
#   -SECTION_END-  ( 7.05 )

#   -SECTION_START-( 7.06 )
Close                                           +
    OpenInstance=1
#   -SECTION_END-  ( 7.06 )

#   -SECTION_END-  ( 7.0 )

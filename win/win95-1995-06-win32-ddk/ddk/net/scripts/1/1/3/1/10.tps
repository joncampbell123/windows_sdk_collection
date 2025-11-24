#   -SECTION_START-( 10.0 ) -SECTION_DESC-( "Stress Tests" )
##
## TITLE: 1.1.3.1.10 Stress Tests (1M/1C/1O)
##
## 1 Machine - 1 Card - 1 Open Instances on the card.  These tests
## will open the test card once and run both a Stress client and
## a Stress server on the same OpenInstance.

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#   -SECTION_START-( 10.01 )
GetEvents                                       +
    OpenInstance=1
#   -SECTION_END-  ( 10.01 )

# Then open the card.

#   -SECTION_START-( 10.02 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%
#   -SECTION_END-  ( 10.02 )

########################################################

# And the Stress client.

#   -SECTION_START-( 10.03 )
Stress                                          +
    OpenInstance=1                              +
    MemberType=BOTH                             +
    PacketType=FIXEDSIZE                        +
    PacketSize=60                               +
    PacketMakeUp=RAND                           +
    ResponseType=FULL_RESPONSE                  +
    Iterations=-1                               +
    Packets=10000                               +
    DelayType=FIXEDDELAY                        +
    DelayLength=0                               +
    WindowEnabled=FALSE                         +
    DataChecking=TRUE                           +
    PacketPool=TRUE
#   -SECTION_END-  ( 10.03 )

########################################################

# Then wait for the stress test to end,

#   -SECTION_START-( 10.04 )
WaitStress                                      +
    OpenInstance=1
#   -SECTION_END-  ( 10.04 )

########################################################

# Finally close the adapter, and again dump the Event
# Queue for any unexpected events.

#   -SECTION_START-( 10.05 )
GetEvents                                       +
    OpenInstance=1
#   -SECTION_END-  ( 10.05 )

#   -SECTION_START-( 10.06 )
Close                                           +
    OpenInstance=1
#   -SECTION_END-  ( 10.06 )

#   -SECTION_END-  ( 10.0 )

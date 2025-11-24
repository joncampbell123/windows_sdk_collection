#   -SECTION_START-( 1.0 )  -SECTION_DESC-( "Send Directed" )
##
## TITLE: 1.1.2.1.1 Send Directed (1M/1C/1O)
##
## 1 Machine - 1 Card - 1 Open Instance on the card.  These tests
## will have the test card send 10 DIRECTED packets to a random
## directed network address to verify that packets may be sent.
##

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#   -SECTION_START-( 1.01 )
GetEvents                                       +
    OpenInstance=1
#   -SECTION_END-  ( 1.01 )

# then open the card.

#   -SECTION_START-( 1.02 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%
#   -SECTION_END-  ( 1.02 )

########################################################

# Send 40 byte packets.

#   -SECTION_START-( 1.03 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_RANDOM_ADDRESS%      +
    PacketSize=40                               +
    Number=10

WaitSend                                        +
    OpenInstance=1
#   -SECTION_END-  ( 1.03 )

########################################################

# Send 512 byte packets.

#   -SECTION_START-( 1.04 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_RANDOM_ADDRESS%      +
    PacketSize=512                              +
    Number=10

WaitSend                                        +
    OpenInstance=1
#   -SECTION_END-  ( 1.04 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#   -SECTION_START-( 1.05 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_RANDOM_ADDRESS%      +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10

WaitSend                                        +
    OpenInstance=1
#   -SECTION_END-  ( 1.05 )

########################################################

# Finally close the adapter, and again dump the Event
# Queue for any unexpected events.

#   -SECTION_START-( 1.06 )
Close                                           +
    OpenInstance=1
#   -SECTION_END-  ( 1.06 )

#   -SECTION_START-( 1.07 )
GetEvents                                       +
    OpenInstance=1
#   -SECTION_END-  ( 1.07 )

#   -SECTION_END-  ( 1.0 )

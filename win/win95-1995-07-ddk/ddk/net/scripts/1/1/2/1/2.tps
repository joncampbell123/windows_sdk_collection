#   -SECTION_START-( 2.0 )  -SECTION_DESC-( "Send Broadcast" )
##
## TITLE: 1.1.2.1.2 Send Broadcast (1M/1C/1O)
##
## 1 Machine - 1 Card - 1 Open Instance on the card.  These tests
## will have the test card send 10 BROADCAST packets to a random
## directed network address to verify that packets may be sent.
##

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#   -SECTION_START-( 2.01 )
GetEvents                                       +
    OpenInstance=1
#   -SECTION_END-  ( 2.01 )

# then open the card.

#   -SECTION_START-( 2.02 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%
#   -SECTION_END-  ( 2.02 )

########################################################

# Send 40 byte packets.

#   -SECTION_START-( 2.03 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_BROADCAST_ADDRESS%   +
    PacketSize=40                               +
    Number=10

WaitSend                                        +
    OpenInstance=1
#   -SECTION_END-  ( 2.03 )

########################################################

# Send 512 byte packets.

#   -SECTION_START-( 2.04 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_BROADCAST_ADDRESS%   +
    PacketSize=512                              +
    Number=10

WaitSend                                        +
    OpenInstance=1
#   -SECTION_END-  ( 2.04 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#   -SECTION_START-( 2.05 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_BROADCAST_ADDRESS%   +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10

WaitSend                                        +
    OpenInstance=1
#   -SECTION_END-  ( 2.05 )

########################################################

# Finally close the adapter, and again dump the Event
# Queue for any unexpected events.

#   -SECTION_START-( 2.06 )
Close                                           +
    OpenInstance=1
#   -SECTION_END-  ( 2.06 )

#   -SECTION_START-( 2.07 )
GetEvents                                       +
    OpenInstance=1
#   -SECTION_END-  ( 2.07 )

#   -SECTION_END-  ( 2.0 )

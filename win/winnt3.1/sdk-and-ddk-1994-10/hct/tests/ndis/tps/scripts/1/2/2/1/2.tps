#	-SECTION_START-( 2.00 ) -SECTION_DESC-( "Send Broadcast" )
##
## TITLE: 1.2.2.1.2 Send Broadcast (1M/1C/2O)
##
## 1 Machine - 1 Card - 2 Open Instances on the card.  These tests
## will have the test card send 10 BROADCAST packets to a random
## directed network address to verify that packets may be sent.
##

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

# then open the card.

#	-SECTION_START-( 2.03 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 2.03 )

#	-SECTION_START-( 2.04 )
Open                                            +
    OpenInstance=2                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 2.04 )

########################################################

# Send 40 byte packets.

#	-SECTION_START-( 2.05 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_BROADCAST_ADDRESS%   +
    PacketSize=40                               +
    Number=10
#	-SECTION_END-( 2.05 )

#	-SECTION_START-( 2.06 )
Send                                            +
    OpenInstance=2                              +
    DestinationAddress=%TP_BROADCAST_ADDRESS%   +
    PacketSize=40                               +
    Number=10
#	-SECTION_END-( 2.06 )

#	-SECTION_START-( 2.07 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 2.07 )

#	-SECTION_START-( 2.08 )
WaitSend                                        +
    OpenInstance=2
#	-SECTION_END-( 2.08 )

########################################################

# Send 512 byte packets.

#	-SECTION_START-( 2.09 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_BROADCAST_ADDRESS%   +
    PacketSize=512                              +
    Number=10
#	-SECTION_END-( 2.09 )

#	-SECTION_START-( 2.10 )
Send                                            +
    OpenInstance=2                              +
    DestinationAddress=%TP_BROADCAST_ADDRESS%   +
    PacketSize=512                              +
    Number=10
#	-SECTION_END-( 2.10 )

#	-SECTION_START-( 2.11 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 2.11 )

#	-SECTION_START-( 2.12 )
WaitSend                                        +
    OpenInstance=2
#	-SECTION_END-( 2.12 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#	-SECTION_START-( 2.13 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_BROADCAST_ADDRESS%   +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10
#	-SECTION_END-( 2.13 )

#	-SECTION_START-( 2.14 )
Send                                            +
    OpenInstance=2                              +
    DestinationAddress=%TP_BROADCAST_ADDRESS%   +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10
#	-SECTION_END-( 2.14 )

#	-SECTION_START-( 2.15 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 2.15 )

#	-SECTION_START-( 2.16 )
WaitSend                                        +
    OpenInstance=2
#	-SECTION_END-( 2.16 )

########################################################

# Finally close the adapter, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 2.17 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 2.17 )

#	-SECTION_START-( 2.18 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 2.18 )

#	-SECTION_START-( 2.19 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 2.19 )

#	-SECTION_START-( 2.20 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 2.20 )

#	-SECTION_END-( 2.00 )

#	-SECTION_START-( 3.00 ) -SECTION_DESC-( "Send Multicast" )
##
## TITLE: 1.2.2.1.3 Send Multicast (1M/1C/2O)
##
## 1 Machine - 1 Card - 2 Open Instances on the card.  These tests
## will have the test card send 10 MULTICAST packets to a random
## directed network address to verify that packets may be sent.
##

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#	-SECTION_START-( 3.01 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 3.01 )

#	-SECTION_START-( 3.02 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 3.02 )

# then open the card.

#	-SECTION_START-( 3.03 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 3.03 )

#	-SECTION_START-( 3.04 )
Open                                            +
    OpenInstance=2                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 3.04 )

########################################################

# Send 40 byte packets.

#	-SECTION_START-( 3.05 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_MULTICAST_ADDRESS%   +
    PacketSize=40                               +
    Number=10
#	-SECTION_END-( 3.05 )

#	-SECTION_START-( 3.06 )
Send                                            +
    OpenInstance=2                              +
    DestinationAddress=%TP_MULTICAST_ADDRESS%   +
    PacketSize=40                               +
    Number=10
#	-SECTION_END-( 3.06 )

#	-SECTION_START-( 3.07 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 3.07 )

#	-SECTION_START-( 3.08 )
WaitSend                                        +
    OpenInstance=2
#	-SECTION_END-( 3.08 )

########################################################

# Send 512 byte packets.

#	-SECTION_START-( 3.09 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_MULTICAST_ADDRESS%   +
    PacketSize=512                              +
    Number=10
#	-SECTION_END-( 3.09 )

#	-SECTION_START-( 3.10 )
Send                                            +
    OpenInstance=2                              +
    DestinationAddress=%TP_MULTICAST_ADDRESS%   +
    PacketSize=512                              +
    Number=10
#	-SECTION_END-( 3.10 )

#	-SECTION_START-( 3.11 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 3.11 )

#	-SECTION_START-( 3.12 )
WaitSend                                        +
    OpenInstance=2
#	-SECTION_END-( 3.12 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#	-SECTION_START-( 3.13 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_MULTICAST_ADDRESS%   +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10
#	-SECTION_END-( 3.13 )

#	-SECTION_START-( 3.14 )
Send                                            +
    OpenInstance=2                              +
    DestinationAddress=%TP_MULTICAST_ADDRESS%   +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10
#	-SECTION_END-( 3.14 )

#	-SECTION_START-( 3.15 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 3.15 )

#	-SECTION_START-( 3.16 )
WaitSend                                        +
    OpenInstance=2
#	-SECTION_END-( 3.16 )

########################################################

# Finally close the adapter, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 3.17 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 3.17 )

#	-SECTION_START-( 3.18 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 3.18 )

#	-SECTION_START-( 3.19 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 3.19 )

#	-SECTION_START-( 3.20 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 3.20 )

#	-SECTION_END-( 3.00 )

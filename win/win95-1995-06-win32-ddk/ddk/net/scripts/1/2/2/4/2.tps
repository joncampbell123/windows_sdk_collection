#	-SECTION_START-( 2.00 ) -SECTION_DESC-( "Send Multicast, Receive Broadcast" )
##
## TITLE: 1.2.2.4.2 Send Multicast, Receive Broadcast (1M/1C/2O)
##
## 1 Machine - 1 Card - 2 Open Instances on the card.  These tests
## will have the test card send 10 MULTICAST packets to another open
## on the test card.  The test card will have its packetfilter
## No packets should be received by the test card.
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

# And open and setup the cards.

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

#	-SECTION_START-( 2.05 )
SetPacketFilter                                 +
    OpenInstance=2                              +
    PacketFilter=BROADCAST
#	-SECTION_END-( 2.05 )

# Now set the test card to receive packets,

#	-SECTION_START-( 2.06 )
Receive                                         +
    OpenInstance=2
#	-SECTION_END-( 2.06 )

########################################################

# Send 40 byte packets.

#	-SECTION_START-( 2.07 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_MULTICAST_ADDRESS%   +
    PacketSize=40                               +
    Number=10
#	-SECTION_END-( 2.07 )

#	-SECTION_START-( 2.08 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 2.08 )

########################################################

# Send 512 byte packets.

#	-SECTION_START-( 2.09 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_MULTICAST_ADDRESS%   +
    PacketSize=512                              +
    Number=10
#	-SECTION_END-( 2.09 )

#	-SECTION_START-( 2.10 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 2.10 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#	-SECTION_START-( 2.11 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_MULTICAST_ADDRESS%   +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10
#	-SECTION_END-( 2.11 )

#	-SECTION_START-( 2.12 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 2.12 )

########################################################

# Then stop the receive and dump the statistics.

#	-SECTION_START-( 2.13 )
StopReceive                                     +
    OpenInstance=2
#	-SECTION_END-( 2.13 )

########################################################

# Finally close the adapters, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 2.14 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 2.14 )

#	-SECTION_START-( 2.15 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 2.15 )

#	-SECTION_START-( 2.16 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 2.16 )

#	-SECTION_START-( 2.17 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 2.17 )

#	-SECTION_END-( 2.00 )

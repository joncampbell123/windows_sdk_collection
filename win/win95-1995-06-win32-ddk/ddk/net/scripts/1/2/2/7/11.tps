#	-SECTION_START-( 8.00 ) -SECTION_DESC-( "Send Broadcast, Receive Directed & Broadcast & Functional" )
##
## TITLE: 1.2.2.7.11 Send Broadcast, Receive Directed & Broadcast
## TITLE:            & Functional (1M/1C/2O)
##
## 1 Machine - 1 Card - 2 Open Instances on the card.  These tests
## will have the test card send 10 BROADCAST packets to another open
## on the test card.  The test card will have its packetfilter
## set to receive DIRECTED, BROADCAST and FUNCTIONAL packets.  Packets
## of size 40 bytes, 512 bytes, and MAX_FRAME_SIZE bytes will be sent.
##
## All packets should be received by the test card.
##

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#	-SECTION_START-( 8.01 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 8.01 )

#	-SECTION_START-( 8.02 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 8.02 )

# And open and setup the cards.

#	-SECTION_START-( 8.03 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 8.03 )

#	-SECTION_START-( 8.04 )
Open                                            +
    OpenInstance=2                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 8.04 )

#	-SECTION_START-( 8.05 )
SetPacketFilter                                 +
    OpenInstance=2                              +
    PacketFilter=DIRECTED|BROADCAST|FUNCTIONAL
#	-SECTION_END-( 8.05 )

# Now set the test card to receive packets,

#	-SECTION_START-( 8.06 )
Receive                                         +
    OpenInstance=2
#	-SECTION_END-( 8.06 )

########################################################

# Send 40 byte packets.

#	-SECTION_START-( 8.07 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_BROADCAST_ADDRESS%   +
    PacketSize=40                               +
    Number=10
#	-SECTION_END-( 8.07 )

#	-SECTION_START-( 8.08 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 8.08 )

########################################################

# Send 512 byte packets.

#	-SECTION_START-( 8.09 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_BROADCAST_ADDRESS%   +
    PacketSize=512                              +
    Number=10
#	-SECTION_END-( 8.09 )

#	-SECTION_START-( 8.10 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 8.10 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#	-SECTION_START-( 8.11 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_BROADCAST_ADDRESS%   +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10
#	-SECTION_END-( 8.11 )

#	-SECTION_START-( 8.12 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 8.12 )

########################################################

# Then stop the receive and dump the statistics.

#	-SECTION_START-( 8.13 )
StopReceive                                     +
    OpenInstance=2
#	-SECTION_END-( 8.13 )

########################################################

# Finally close the adapters, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 8.14 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 8.14 )

#	-SECTION_START-( 8.15 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 8.15 )

#	-SECTION_START-( 8.16 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 8.16 )

#	-SECTION_START-( 8.17 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 8.17 )

#	-SECTION_END-( 8.00 )

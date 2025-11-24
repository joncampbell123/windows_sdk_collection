#	-SECTION_START-( 3.00 ) -SECTION_DESC-( "Send Directed, Receive Directed & Source Routing" ) -OPTIONALS-( SOURCEROUTING )
##
## TITLE: 1.2.2.6.4 Send Directed, Receive Directed & Source Routing (1M/1C/2O)
##
## 1 Machine - 1 Card - 2 Open Instances on the card.  These tests
## will have the test card send 10 DIRECTED packets to another open
## on the test card.  The test card will have its packetfilter
## set to receive DIRECTED and SOURCEROUTING packets.  Packets of size
## 40 bytes, 512 bytes, and MAX_FRAME_SIZE bytes will be sent.
##
## All packets should be received by the test card.
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

# And open and setup the cards.

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

#	-SECTION_START-( 3.05 )
SetPacketFilter                                 +
    OpenInstance=2                              +
    PacketFilter=DIRECTED|SOURCEROUTING
#	-SECTION_END-( 3.05 )

# Now set the test card to receive packets,

#	-SECTION_START-( 3.06 )
Receive                                         +
    OpenInstance=2
#	-SECTION_END-( 3.06 )

########################################################

# Send 40 byte packets.

#	-SECTION_START-( 3.07 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_TEST_CARD_ADDRESS%   +
    PacketSize=40                               +
    Number=10
#	-SECTION_END-( 3.07 )

#	-SECTION_START-( 3.08 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 3.08 )

########################################################

# Send 512 byte packets.

#	-SECTION_START-( 3.09 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_TEST_CARD_ADDRESS%   +
    PacketSize=512                              +
    Number=10
#	-SECTION_END-( 3.09 )

#	-SECTION_START-( 3.10 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 3.10 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#	-SECTION_START-( 3.11 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_TEST_CARD_ADDRESS%   +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10
#	-SECTION_END-( 3.11 )

#	-SECTION_START-( 3.12 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 3.12 )

########################################################

# Then stop the receive and dump the statistics.

#	-SECTION_START-( 3.13 )
StopReceive                                     +
    OpenInstance=2
#	-SECTION_END-( 3.13 )

########################################################

# Finally close the adapters, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 3.14 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 3.14 )

#	-SECTION_START-( 3.15 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 3.15 )

#	-SECTION_START-( 3.16 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 3.16 )

#	-SECTION_START-( 3.17 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 3.17 )

#	-SECTION_END-( 3.00 )

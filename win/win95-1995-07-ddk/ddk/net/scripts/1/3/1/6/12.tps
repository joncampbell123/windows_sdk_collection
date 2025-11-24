#	-SECTION_START-( 9.00 ) -SECTION_DESC-( "Send Directed, Receive Directed & Broadcast & Functional & Source Routing and Mac Frame" ) -OPTIONALS-( SOURCEROUTING,MACFRAME )
##
## TITLE: 1.3.1.6.12 Send Directed, Receive Directed & Broadcast & Functional
## TITLE:            & Source Routing and Mac Frame (1M/2C/1O)
##
## 1 Machine - 2 Cards - 1 Open Instance on each card.  These tests
## will have a trusted card send 10 DIRECTED packets to the card
## being tested.  The card being tested will have its packetfilter
## set to receive DIRECTED, BROADCAST and FUNCTIONAL packets.  Packets
## of size 40 bytes, 512 bytes, and MAX_FRAME_SIZE bytes will be sent.
##
## All packets should be received by the test card.
##

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#	-SECTION_START-( 9.01 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 9.01 )

#	-SECTION_START-( 9.02 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 9.02 )

# And open and setup the cards.

#	-SECTION_START-( 9.03 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TRUSTED_CARD%
#	-SECTION_END-( 9.03 )

#	-SECTION_START-( 9.04 )
Open                                            +
    OpenInstance=2                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 9.04 )

#	-SECTION_START-( 9.05 )
SetPacketFilter                                 +
    OpenInstance=2                              +
    PacketFilter=DIRECTED|BROADCAST|FUNCTIONAL|SOURCEROUTING|MACFRAME
#	-SECTION_END-( 9.05 )

# Now set the test card to receive packets,

#	-SECTION_START-( 9.06 )
Receive                                         +
    OpenInstance=2
#	-SECTION_END-( 9.06 )

########################################################

# Send 40 byte packets.

#	-SECTION_START-( 9.07 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_TEST_CARD_ADDRESS%   +
    PacketSize=40                               +
    Number=10
#	-SECTION_END-( 9.07 )

#	-SECTION_START-( 9.08 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 9.08 )

########################################################

# Send 512 byte packets.

#	-SECTION_START-( 9.09 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_TEST_CARD_ADDRESS%   +
    PacketSize=512                              +
    Number=10
#	-SECTION_END-( 9.09 )

#	-SECTION_START-( 9.10 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 9.10 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#	-SECTION_START-( 9.11 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_TEST_CARD_ADDRESS%   +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10
#	-SECTION_END-( 9.11 )

#	-SECTION_START-( 9.12 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 9.12 )

########################################################

# Then stop the receive and dump the statistics.

#	-SECTION_START-( 9.13 )
StopReceive                                     +
    OpenInstance=2
#	-SECTION_END-( 9.13 )

########################################################

# Finally close the adapters, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 9.14 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 9.14 )

#	-SECTION_START-( 9.15 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 9.15 )

#	-SECTION_START-( 9.16 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 9.16 )

#	-SECTION_START-( 9.17 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 9.17 )

#	-SECTION_END-( 9.00 )

#	-SECTION_START-( 6.00 ) -SECTION_DESC-( "Send Directed, Receive Broadcast & Source Routing" ) -OPTIONALS-( SOURCEROUTING )
##
## TITLE: 1.2.2.6.8 Send Directed, Receive Broadcast & Source Routing (1M/1C/2O)
##
## 1 Machine - 1 Card - 2 Open Instances on the card.  These tests
## will have the test card send 10 DIRECTED packets to another open
## on the test card.  The test card will have its packetfilter
## set to receive BROADCAST and SOURCEROUTING packets.  Packets of size
## 40 bytes, 512 bytes, and MAX_FRAME_SIZE bytes will be sent.
##
## No packets should be received by the test card.
##

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#	-SECTION_START-( 6.01 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 6.01 )

#	-SECTION_START-( 6.02 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 6.02 )

# And open and setup the cards.

#	-SECTION_START-( 6.03 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 6.03 )

#	-SECTION_START-( 6.04 )
Open                                            +
    OpenInstance=2                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 6.04 )

#	-SECTION_START-( 6.05 )
SetPacketFilter                                 +
    OpenInstance=2                              +
    PacketFilter=BROADCAST|SOURCEROUTING
#	-SECTION_END-( 6.05 )

# Now set the test card to receive packets,

#	-SECTION_START-( 6.06 )
Receive                                         +
    OpenInstance=2
#	-SECTION_END-( 6.06 )

########################################################

# Send 40 byte packets.

#	-SECTION_START-( 6.07 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_TEST_CARD_ADDRESS%   +
    PacketSize=40                               +
    Number=10
#	-SECTION_END-( 6.07 )

#	-SECTION_START-( 6.08 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 6.08 )

########################################################

# Send 512 byte packets.

#	-SECTION_START-( 6.09 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_TEST_CARD_ADDRESS%   +
    PacketSize=512                              +
    Number=10
#	-SECTION_END-( 6.09 )

#	-SECTION_START-( 6.10 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 6.10 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#	-SECTION_START-( 6.11 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_TEST_CARD_ADDRESS%   +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10
#	-SECTION_END-( 6.11 )

#	-SECTION_START-( 6.12 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 6.12 )

########################################################

# Then stop the receive and dump the statistics.

#	-SECTION_START-( 6.13 )
StopReceive                                     +
    OpenInstance=2
#	-SECTION_END-( 6.13 )

########################################################

# Finally close the adapters, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 6.14 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 6.14 )

#	-SECTION_START-( 6.15 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 6.15 )

#	-SECTION_START-( 6.16 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 6.16 )

#	-SECTION_START-( 6.17 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 6.17 )

#	-SECTION_END-( 6.00 )

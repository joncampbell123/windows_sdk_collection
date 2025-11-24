#	-SECTION_START-( 7.00 ) -SECTION_DESC-( "Send Broadcast, Receive Broadcast & Source Routing & Resend Directed" ) -OPTIONALS-( SOURCEROUTING )
##
## TITLE: 1.3.1.15.11 Send Broadcast, Receive Broadcast & Source Routing
## TITLE:             & Resend Directed (1M/2C/1O)
##
## 1 Machine - 2 Cards - 1 Open Instance on each card.  These tests
## will have a trusted card send 10 BROADCAST packets to the card
## being tested.  The card being tested will have its packetfilter
## set to receive BROADCAST & SOURCEROUTING packets.  Each packet
## will contain a resend packet the test card will resend.  Packets
## of size 80 bytes, 512 bytes, and MAX_FRAME_SIZE bytes will be sent.
##
## All SEND packets should be received by the test card.
##

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#	-SECTION_START-( 7.01 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 7.01 )

#	-SECTION_START-( 7.02 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 7.02 )

# And open and setup the cards.

#	-SECTION_START-( 7.03 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TRUSTED_CARD%
#	-SECTION_END-( 7.03 )

#	-SECTION_START-( 7.04 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED
#	-SECTION_END-( 7.04 )

#	-SECTION_START-( 7.05 )
Open                                            +
    OpenInstance=2                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 7.05 )

#	-SECTION_START-( 7.06 )
SetPacketFilter                                 +
    OpenInstance=2                              +
    PacketFilter=BROADCAST|SOURCEROUTING
#	-SECTION_END-( 7.06 )

# Now set the trusted and test cards to receive packets.

#	-SECTION_START-( 7.07 )
Receive                                         +
    OpenInstance=1
#	-SECTION_END-( 7.07 )

#	-SECTION_START-( 7.08 )
Receive                                         +
    OpenInstance=2
#	-SECTION_END-( 7.08 )

########################################################

# Send 80 byte packets.

#	-SECTION_START-( 7.09 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_BROADCAST_ADDRESS%   +
    PacketSize=80                               +
    Number=10                                   +
    ResendAddress=%TP_TRUSTED_CARD_ADDRESS%
#	-SECTION_END-( 7.09 )

#	-SECTION_START-( 7.10 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 7.10 )

########################################################

# Send 512 byte packets.

#	-SECTION_START-( 7.11 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_BROADCAST_ADDRESS%   +
    PacketSize=512                              +
    Number=10                                   +
    ResendAddress=%TP_TRUSTED_CARD_ADDRESS%
#	-SECTION_END-( 7.11 )

#	-SECTION_START-( 7.12 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 7.12 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#	-SECTION_START-( 7.13 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_BROADCAST_ADDRESS%   +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10                                   +
    ResendAddress=%TP_TRUSTED_CARD_ADDRESS%
#	-SECTION_END-( 7.13 )

#	-SECTION_START-( 7.14 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 7.14 )

########################################################

# Then stop the receives and dump the statistics.

#	-SECTION_START-( 7.15 )
StopReceive                                     +
    OpenInstance=1
#	-SECTION_END-( 7.15 )

#	-SECTION_START-( 7.16 )
StopReceive                                     +
    OpenInstance=2
#	-SECTION_END-( 7.16 )

########################################################

# Finally close the adapters, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 7.17 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 7.17 )

#	-SECTION_START-( 7.18 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 7.18 )

#	-SECTION_START-( 7.19 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 7.19 )

#	-SECTION_START-( 7.20 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 7.20 )

#	-SECTION_END-( 7.00 )

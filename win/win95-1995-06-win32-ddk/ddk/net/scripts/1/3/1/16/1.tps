#	-SECTION_START-( 1.00 ) -SECTION_DESC-( "Send Multicast, Receive Multicast & Directed & Resend Directed" )
##
## TITLE: 1.3.1.16.1 Send Multicast, Receive Multicast & Directed
## TITLE:            & Resend Directed (1M/2C/1O)
##
## 1 Machine - 2 Cards - 1 Open Instance on each card.  These tests
## will have a trusted card send 10 MULTICAST packets to the card
## being tested.  The card being tested will have its packetfilter
## set to receive DIRECTED & MULTICAST packets.  Each packet will
## contain a resend packet the test card will resend.  Packets of
## size 80 bytes, 512 bytes, and MAX_FRAME_SIZE bytes will be sent.
##
## All SEND packets should be received by the test card.
##

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#	-SECTION_START-( 1.01 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 1.01 )

#	-SECTION_START-( 1.02 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 1.02 )

# And open and setup the cards.

#	-SECTION_START-( 1.03 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TRUSTED_CARD%
#	-SECTION_END-( 1.03 )

#	-SECTION_START-( 1.04 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED
#	-SECTION_END-( 1.04 )

#	-SECTION_START-( 1.05 )
Open                                            +
    OpenInstance=2                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 1.05 )

#	-SECTION_START-( 1.06 )
SetPacketFilter                                 +
    OpenInstance=2                              +
    PacketFilter=MULTICAST|DIRECTED
#	-SECTION_END-( 1.06 )

#	-SECTION_START-( 1.07 )
AddMulticastAddress                             +
    OpenInstance=2                              +
    MulticastAddress=%TP_MULTICAST_ADDRESS%
#	-SECTION_END-( 1.07 )

########################################################

# Now set the trusted and test cards to receive packets.

#	-SECTION_START-( 1.08 )
Receive                                         +
    OpenInstance=1
#	-SECTION_END-( 1.08 )

#	-SECTION_START-( 1.09 )
Receive                                         +
    OpenInstance=2
#	-SECTION_END-( 1.09 )

########################################################

# Send 80 byte packets.

#	-SECTION_START-( 1.10 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_MULTICAST_ADDRESS%   +
    PacketSize=80                               +
    Number=10                                   +
    ResendAddress=%TP_TRUSTED_CARD_ADDRESS%
#	-SECTION_END-( 1.10 )

#	-SECTION_START-( 1.11 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 1.11 )

########################################################

# Send 512 byte packets.

#	-SECTION_START-( 1.12 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_MULTICAST_ADDRESS%   +
    PacketSize=512                              +
    Number=10                                   +
    ResendAddress=%TP_TRUSTED_CARD_ADDRESS%
#	-SECTION_END-( 1.12 )

#	-SECTION_START-( 1.13 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 1.13 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#	-SECTION_START-( 1.14 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_MULTICAST_ADDRESS%   +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10                                   +
    ResendAddress=%TP_TRUSTED_CARD_ADDRESS%
#	-SECTION_END-( 1.14 )

#	-SECTION_START-( 1.15 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 1.15 )

########################################################

# Then stop the receives and dump the statistics.

#	-SECTION_START-( 1.16 )
StopReceive                                     +
    OpenInstance=1
#	-SECTION_END-( 1.16 )

#	-SECTION_START-( 1.17 )
StopReceive                                     +
    OpenInstance=2
#	-SECTION_END-( 1.17 )

########################################################

# Finally close the adapters, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 1.18 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 1.18 )

#	-SECTION_START-( 1.19 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 1.19 )

#	-SECTION_START-( 1.20 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 1.20 )

#	-SECTION_START-( 1.21 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 1.21 )

#	-SECTION_END-( 1.00 )

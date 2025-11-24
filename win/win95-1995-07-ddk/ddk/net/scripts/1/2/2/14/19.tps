#	-SECTION_START-( 9.00 ) -SECTION_DESC-( "Send Directed, Receive Directed & Broadcast & Multicast & Resend Multicast" )
##
## TITLE: 1.2.2.14.19 Send Directed, Receive Directed & Broadcast &
## TITLE:             Multicast & Resend Multicast (1M/1C/2O)
##
## 1 Machine - 1 Card - 2 Open Instances on the card.  These tests
## will have the test card send 10 DIRECTED packets to another open
## on the test card.  The test card will have its packetfilter
## set to receive DIRECTED & BROADCAST & MULTICAST packets.  Each
## packet will contain a resend packet the test card will resend.
## Packets of size 80 bytes, 512 bytes, and MAX_FRAME_SIZE bytes
## will be sent.
##
## All SEND packets should be received by the test card.
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
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 9.03 )

#	-SECTION_START-( 9.04 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=MULTICAST
#	-SECTION_END-( 9.04 )

#	-SECTION_START-( 9.05 )
AddMulticastAddress                             +
    OpenInstance=1                              +
    MulticastAddress=%TP_MULTICAST_ADDRESS%
#	-SECTION_END-( 9.05 )

#	-SECTION_START-( 9.06 )
Open                                            +
    OpenInstance=2                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 9.06 )

#	-SECTION_START-( 9.07 )
SetPacketFilter                                 +
    OpenInstance=2                              +
    PacketFilter=DIRECTED|BROADCAST|MULTICAST
#	-SECTION_END-( 9.07 )

#	-SECTION_START-( 9.08 )
AddMulticastAddress                             +
    OpenInstance=2				+
    MulticastAddress=%TP_MULTICAST_ADDRESS%
#	-SECTION_END-( 9.08 )

# Now set the trusted and test cards to receive packets.

#	-SECTION_START-( 9.09 )
Receive                                         +
    OpenInstance=1
#	-SECTION_END-( 9.09 )

#	-SECTION_START-( 9.10 )
Receive                                         +
    OpenInstance=2
#	-SECTION_END-( 9.10 )

########################################################

# Send 80 byte packets.

#	-SECTION_START-( 9.11 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_TEST_CARD_ADDRESS%   +
    PacketSize=80                               +
    Number=10                                   +
    ResendAddress=%TP_MULTICAST_ADDRESS%
#	-SECTION_END-( 9.11 )

#	-SECTION_START-( 9.12 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 9.12 )

########################################################

# Send 512 byte packets.

#	-SECTION_START-( 9.13 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_TEST_CARD_ADDRESS%   +
    PacketSize=512                              +
    Number=10                                   +
    ResendAddress=%TP_MULTICAST_ADDRESS%
#	-SECTION_END-( 9.13 )

#	-SECTION_START-( 9.14 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 9.14 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#	-SECTION_START-( 9.15 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_TEST_CARD_ADDRESS%   +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10                                   +
    ResendAddress=%TP_MULTICAST_ADDRESS%
#	-SECTION_END-( 9.15 )

#	-SECTION_START-( 9.16 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 9.16 )

########################################################

# Then stop the receives and dump the statistics.

#	-SECTION_START-( 9.17 )
StopReceive                                     +
    OpenInstance=1
#	-SECTION_END-( 9.17 )

#	-SECTION_START-( 9.18 )
StopReceive                                     +
    OpenInstance=2
#	-SECTION_END-( 9.18 )

########################################################

# Finally close the adapters, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 9.19 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 9.19 )

#	-SECTION_START-( 9.20 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 9.20 )

#	-SECTION_START-( 9.21 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 9.21 )

#	-SECTION_START-( 9.22 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 9.22 )

#	-SECTION_END-( 9.00 )

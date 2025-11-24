#	-SECTION_START-( 3.00 ) -SECTION_DESC-( "Send Directed, Receive Directed & Resend Multicast" )
##
## TITLE: 1.2.2.10.3 Send Directed, Receive Directed & Resend
## TITLE:           Multicast (1M/1C/2O)
##
## 1 Machine - 1 Card - 2 Open Instances on the card.  These tests
## will have the test card send 10 DIRECTED packets to another open
## on the test card.  The test card will have its packetfilter
## set to receive DIRECTED packets.  Each packet will contain
## a resend packet the test card will resend.  Packets of size
## 80 bytes, 512 bytes, and MAX_FRAME_SIZE bytes will be sent.
##
## All SEND packets should be received by the test card.
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
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=MULTICAST
#	-SECTION_END-( 3.04 )

#	-SECTION_START-( 3.05 )
AddMulticastAddress                             +
    OpenInstance=1                              +
    MulticastAddress=%TP_MULTICAST_ADDRESS%
#	-SECTION_END-( 3.05 )

#	-SECTION_START-( 3.06 )
Open                                            +
    OpenInstance=2                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 3.06 )

#	-SECTION_START-( 3.07 )
SetPacketFilter                                 +
    OpenInstance=2                              +
    PacketFilter=DIRECTED
#	-SECTION_END-( 3.07 )

# Now set the trusted and test cards to receive packets.

#	-SECTION_START-( 3.08 )
Receive                                         +
    OpenInstance=1
#	-SECTION_END-( 3.08 )

#	-SECTION_START-( 3.09 )
Receive                                         +
    OpenInstance=2
#	-SECTION_END-( 3.09 )

########################################################

# Send 80 byte packets.

#	-SECTION_START-( 3.10 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_TEST_CARD_ADDRESS%   +
    PacketSize=80                               +
    Number=10                                   +
    ResendAddress=%TP_MULTICAST_ADDRESS%
#	-SECTION_END-( 3.10 )

#	-SECTION_START-( 3.11 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 3.11 )

########################################################

# Send 512 byte packets.

#	-SECTION_START-( 3.12 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_TEST_CARD_ADDRESS%   +
    PacketSize=512                              +
    Number=10                                   +
    ResendAddress=%TP_MULTICAST_ADDRESS%
#	-SECTION_END-( 3.12 )

#	-SECTION_START-( 3.13 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 3.13 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#	-SECTION_START-( 3.14 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_TEST_CARD_ADDRESS%   +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10                                   +
    ResendAddress=%TP_MULTICAST_ADDRESS%
#	-SECTION_END-( 3.14 )

#	-SECTION_START-( 3.15 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 3.15 )

########################################################

# Then stop the receives and dump the statistics.

#	-SECTION_START-( 3.16 )
StopReceive                                     +
    OpenInstance=1
#	-SECTION_END-( 3.16 )

#	-SECTION_START-( 3.17 )
StopReceive                                     +
    OpenInstance=2
#	-SECTION_END-( 3.17 )

########################################################

# Finally close the adapters, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 3.18 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 3.18 )

#	-SECTION_START-( 3.19 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 3.19 )

#	-SECTION_START-( 3.20 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 3.20 )

#	-SECTION_START-( 3.21 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 3.21 )

#	-SECTION_END-( 3.00 )

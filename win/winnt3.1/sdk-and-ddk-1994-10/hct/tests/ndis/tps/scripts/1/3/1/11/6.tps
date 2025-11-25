#	-SECTION_START-( 5.00 ) -SECTION_DESC-( "Send Broadcast, Receive Promiscuous & Resend Broadcast" ) -OPTIONALS-( PROMISCUOUS )
##
## TITLE: 1.3.1.11.6 Send Broadcast, Receive Promiscuous & Resend
## TITLE:            Broadcast (1M/2C/1O)
##
## 1 Machine - 2 Cards - 1 Open Instance on each card.  These tests
## will have a trusted card send 10 BROADCAST packets to the card
## being tested.  The card being tested will have its packetfilter
## set to receive PROMISCUOUS packets.  Each packet will contain
## a resend packet the test card will resend.  Packets of size
## 80 bytes, 512 bytes, and MAX_FRAME_SIZE bytes will be sent.
##
## All packets should be received by the test card.
##

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#	-SECTION_START-( 5.01 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 5.01 )

#	-SECTION_START-( 5.02 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 5.02 )

# And open and setup the cards.

#	-SECTION_START-( 5.03 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TRUSTED_CARD%
#	-SECTION_END-( 5.03 )

#	-SECTION_START-( 5.04 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=BROADCAST
#	-SECTION_END-( 5.04 )

#	-SECTION_START-( 5.05 )
Open                                            +
    OpenInstance=2                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 5.05 )

#	-SECTION_START-( 5.06 )
SetPacketFilter                                 +
    OpenInstance=2                              +
    PacketFilter=PROMISCUOUS
#	-SECTION_END-( 5.06 )

# Now set the trusted and test cards to receive packets.

#	-SECTION_START-( 5.07 )
Receive                                         +
    OpenInstance=1
#	-SECTION_END-( 5.07 )

#	-SECTION_START-( 5.08 )
Receive                                         +
    OpenInstance=2
#	-SECTION_END-( 5.08 )

########################################################

# Send 80 byte packets.

#	-SECTION_START-( 5.09 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_BROADCAST_ADDRESS%   +
    PacketSize=80                               +
    Number=10                                   +
    ResendAddress=%TP_BROADCAST_ADDRESS%
#	-SECTION_END-( 5.09 )

#	-SECTION_START-( 5.10 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 5.10 )

########################################################

# Send 512 byte packets.

#	-SECTION_START-( 5.11 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_BROADCAST_ADDRESS%   +
    PacketSize=512                              +
    Number=10                                   +
    ResendAddress=%TP_BROADCAST_ADDRESS%
#	-SECTION_END-( 5.11 )

#	-SECTION_START-( 5.12 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 5.12 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#	-SECTION_START-( 5.13 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_BROADCAST_ADDRESS%   +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10                                   +
    ResendAddress=%TP_BROADCAST_ADDRESS%
#	-SECTION_END-( 5.13 )

#	-SECTION_START-( 5.14 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 5.14 )

########################################################

# Then stop the receives and dump the statistics.

#	-SECTION_START-( 5.15 )
StopReceive                                     +
    OpenInstance=1
#	-SECTION_END-( 5.15 )

#	-SECTION_START-( 5.16 )
StopReceive                                     +
    OpenInstance=2
#	-SECTION_END-( 5.16 )

########################################################

# Finally close the adapters, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 5.17 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 5.17 )

#	-SECTION_START-( 5.18 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 5.18 )

#	-SECTION_START-( 5.19 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 5.19 )

#	-SECTION_START-( 5.20 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 5.20 )

#	-SECTION_END-( 5.00 )

#  -SECTION_START-( 17.00 ) -SECTION_DESC-( "Send Broadcast, Receive Directed & Broadcast & Functional & Source Routing & Mac Frame & Resend Broadcast" ) -OPTIONALS-( SOURCEROUTING,MACFRAME )
##
## TITLE: 1.3.1.15.24 Send Broadcast, Receive Directed & Broadcast &
## TITLE:             Functional & Source Routing & Mac Frame & Resend
## TITLE:             Broadcast (1M/2C/1O)
##
## 1 Machine - 2 Cards - 1 Open Instance on each card.  These tests
## will have a trusted card send 10 BROADCAST packets to the card
## being tested.  The card being tested will have its packetfilter
## set to receive DIRECTED & BROADCAST & FUNCTIONAL & SOURCEROUTING
## & MACFRAME packets.  Each packet will contain a resend packet
## the test card will resend.  Packets of size 80 bytes, 512 bytes,
## and MAX_FRAME_SIZE bytes will be sent.
##
## All SEND packets should be received by the test card.
##

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#	-SECTION_START-( 17.01 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 17.01 )

#	-SECTION_START-( 17.02 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 17.02 )

# And open and setup the cards.

#	-SECTION_START-( 17.03 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TRUSTED_CARD%
#	-SECTION_END-( 17.03 )

#	-SECTION_START-( 17.04 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=BROADCAST
#	-SECTION_END-( 17.04 )

#	-SECTION_START-( 17.05 )
Open                                            +
    OpenInstance=2                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 17.05 )

#	-SECTION_START-( 17.06 )
SetPacketFilter                                 +
    OpenInstance=2                              +
    PacketFilter=DIRECTED|BROADCAST|FUNCTIONAL|SOURCEROUTING|MACFRAME
#	-SECTION_END-( 17.06 )

#	-SECTION_START-( 17.07 )
SetFunctionalAddress                            +
    OpenInstance=2                              +
    FunctionalAddress=%TP_FUNCTIONAL_ADDRESS%
#	-SECTION_END-( 17.07 )

# Now set the trusted and test cards to receive packets.

#	-SECTION_START-( 17.08 )
Receive                                         +
    OpenInstance=1
#	-SECTION_END-( 17.08 )

#	-SECTION_START-( 17.09 )
Receive                                         +
    OpenInstance=2
#	-SECTION_END-( 17.09 )

########################################################

# Send 80 byte packets.

#	-SECTION_START-( 17.10 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_BROADCAST_ADDRESS%   +
    PacketSize=80                               +
    Number=10                                   +
    ResendAddress=%TP_BROADCAST_ADDRESS%
#	-SECTION_END-( 17.10 )

#	-SECTION_START-( 17.11 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 17.11 )

########################################################

# Send 512 byte packets.

#	-SECTION_START-( 17.12 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_BROADCAST_ADDRESS%   +
    PacketSize=512                              +
    Number=10                                   +
    ResendAddress=%TP_BROADCAST_ADDRESS%
#	-SECTION_END-( 17.12 )

#	-SECTION_START-( 17.13 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 17.13 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#	-SECTION_START-( 17.14 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_BROADCAST_ADDRESS%   +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10                                   +
    ResendAddress=%TP_BROADCAST_ADDRESS%
#	-SECTION_END-( 17.14 )

#	-SECTION_START-( 17.15 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 17.15 )

########################################################

# Then stop the receives and dump the statistics.

#	-SECTION_START-( 17.16 )
StopReceive                                     +
    OpenInstance=1
#	-SECTION_END-( 17.16 )

#	-SECTION_START-( 17.17 )
StopReceive                                     +
    OpenInstance=2
#	-SECTION_END-( 17.17 )

########################################################

# Finally close the adapters, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 17.18 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 17.18 )

#	-SECTION_START-( 17.19 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 17.19 )

#	-SECTION_START-( 17.20 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 17.20 )

#	-SECTION_START-( 17.21 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 17.21 )

#	-SECTION_END-( 17.00 )

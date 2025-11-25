#	-SECTION_START-( 14.00 ) -SECTION_DESC-( "Send Broadcast, Receive Directed & Broadcast & Functional & Resend Broadcast" )
##
## TITLE: 1.2.2.15.21 Send Broadcast, Receive Directed & Broadcast &
## TITLE:             Functional & Resend Broadcast (1M/1C/2O)
##
## 1 Machine - 1 Card - 2 Open Instances on the card.  These tests
## will have the test card send 10 BROADCAST packets to another open
## on the test card.  The test card will have its packetfilter
## set to receive DIRECTED & BROADCAST & FUNCTIONAL packets.  Each
## packet will contain a resend packet the test card will resend.
## Packets of size 80 bytes, 512 bytes, and MAX_FRAME_SIZE bytes
## will be sent.
##
## All SEND packets should be received by the test card.
##

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#	-SECTION_START-( 14.01 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 14.01 )

#	-SECTION_START-( 14.02 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 14.02 )

# And open and setup the cards.

#	-SECTION_START-( 14.03 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 14.03 )

#	-SECTION_START-( 14.04 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=BROADCAST
#	-SECTION_END-( 14.04 )

#	-SECTION_START-( 14.05 )
Open                                            +
    OpenInstance=2                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 14.05 )

#	-SECTION_START-( 14.06 )
SetPacketFilter                                 +
    OpenInstance=2                              +
    PacketFilter=DIRECTED|BROADCAST|FUNCTIONAL
#	-SECTION_END-( 14.06 )

#	-SECTION_START-( 14.07 )
SetFunctionalAddress                            +
    OpenInstance=2                              +
    FunctionalAddress=%TP_FUNCTIONAL_ADDRESS%
#	-SECTION_END-( 14.07 )

# Now set the trusted and test cards to receive packets.

#	-SECTION_START-( 14.08 )
Receive                                         +
    OpenInstance=1
#	-SECTION_END-( 14.08 )

#	-SECTION_START-( 14.09 )
Receive                                         +
    OpenInstance=2
#	-SECTION_END-( 14.09 )

########################################################

# Send 80 byte packets.

#	-SECTION_START-( 14.10 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_BROADCAST_ADDRESS%   +
    PacketSize=80                               +
    Number=10                                   +
    ResendAddress=%TP_BROADCAST_ADDRESS%
#	-SECTION_END-( 14.10 )

#	-SECTION_START-( 14.11 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 14.11 )

########################################################

# Send 512 byte packets.

#	-SECTION_START-( 14.12 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_BROADCAST_ADDRESS%   +
    PacketSize=512                              +
    Number=10                                   +
    ResendAddress=%TP_BROADCAST_ADDRESS%
#	-SECTION_END-( 14.12 )

#	-SECTION_START-( 14.13 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 14.13 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#	-SECTION_START-( 14.14 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_BROADCAST_ADDRESS%   +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10                                   +
    ResendAddress=%TP_BROADCAST_ADDRESS%
#	-SECTION_END-( 14.14 )

#	-SECTION_START-( 14.15 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 14.15 )

########################################################

# Then stop the receives and dump the statistics.

#	-SECTION_START-( 14.16 )
StopReceive                                     +
    OpenInstance=1
#	-SECTION_END-( 14.16 )

#	-SECTION_START-( 14.17 )
StopReceive                                     +
    OpenInstance=2
#	-SECTION_END-( 14.17 )

########################################################

# Finally close the adapters, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 14.18 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 14.18 )

#	-SECTION_START-( 14.19 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 14.19 )

#	-SECTION_START-( 14.20 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 14.20 )

#	-SECTION_START-( 14.21 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 14.21 )

#	-SECTION_END-( 14.00 )

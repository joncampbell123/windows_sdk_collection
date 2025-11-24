#	-SECTION_START-( 5.00 ) -SECTION_DESC-( "Send Broadcast, Receive Broadcast & Functional & Resend Broadcast" )
##
## TITLE: 1.2.2.15.9 Send Broadcast, Receive Broadcast & Functional
## TITLE:            & Resend Broadcast (1M/1C/2O)
##
## 1 Machine - 1 Card - 2 Open Instances on the card.  These tests
## will have the test card send 10 BROADCAST packets to another open
## on the test card.  The test card will have its packetfilter
## set to receive BROADCAST & FUNCTIONAL packets.  Each packet will
## contain a resend packet the test card will resend.  Packets of
## size 80 bytes, 512 bytes, and MAX_FRAME_SIZE bytes will be sent.
##
## All SEND packets should be received by the test card.
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
    AdapterName=%TP_TEST_CARD%
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
    PacketFilter=BROADCAST|FUNCTIONAL
#	-SECTION_END-( 5.06 )

#	-SECTION_START-( 5.07 )
SetFunctionalAddress                            +
    OpenInstance=2                              +
    FunctionalAddress=%TP_FUNCTIONAL_ADDRESS%
#	-SECTION_END-( 5.07 )

# Now set the trusted and test cards to receive packets.

#	-SECTION_START-( 5.08 )
Receive                                         +
    OpenInstance=1
#	-SECTION_END-( 5.08 )

#	-SECTION_START-( 5.09 )
Receive                                         +
    OpenInstance=2
#	-SECTION_END-( 5.09 )

########################################################

# Send 80 byte packets.

#	-SECTION_START-( 5.10 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_BROADCAST_ADDRESS%   +
    PacketSize=80                               +
    Number=10                                   +
    ResendAddress=%TP_BROADCAST_ADDRESS%
#	-SECTION_END-( 5.10 )

#	-SECTION_START-( 5.11 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 5.11 )

########################################################

# Send 512 byte packets.

#	-SECTION_START-( 5.12 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_BROADCAST_ADDRESS%   +
    PacketSize=512                              +
    Number=10                                   +
    ResendAddress=%TP_BROADCAST_ADDRESS%
#	-SECTION_END-( 5.12 )

#	-SECTION_START-( 5.13 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 5.13 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#	-SECTION_START-( 5.14 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_BROADCAST_ADDRESS%   +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10                                   +
    ResendAddress=%TP_BROADCAST_ADDRESS%
#	-SECTION_END-( 5.14 )

#	-SECTION_START-( 5.15 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 5.15 )

########################################################

# Then stop the receives and dump the statistics.

#	-SECTION_START-( 5.16 )
StopReceive                                     +
    OpenInstance=1
#	-SECTION_END-( 5.16 )

#	-SECTION_START-( 5.17 )
StopReceive                                     +
    OpenInstance=2
#	-SECTION_END-( 5.17 )

########################################################

# Finally close the adapters, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 5.18 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 5.18 )

#	-SECTION_START-( 5.19 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 5.19 )

#	-SECTION_START-( 5.20 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 5.20 )

#	-SECTION_START-( 5.21 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 5.21 )

#	-SECTION_END-( 5.00 )

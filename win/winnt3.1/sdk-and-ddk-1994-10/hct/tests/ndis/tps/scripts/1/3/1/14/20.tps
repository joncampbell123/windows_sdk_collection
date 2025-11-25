#	-SECTION_START-( 13.00 ) -SECTION_DESC-( "Send Directed, Receive Directed & Broadcast & Functional & Resend Directed" )
##
## TITLE: 1.3.1.14.20 Send Directed, Receive Directed & Broadcast &
## TITLE:             Functional & Resend Directed (1M/2C/1O)
##
## 1 Machine - 2 Cards - 1 Open Instance on each card.  These tests
## will have a trusted card send 10 DIRECTED packets to the card
## being tested.  The card being tested will have its packetfilter
## set to receive DIRECTED & BROADCAST & FUNCTIONAL packets.  Each
## packet will contain a resend packet the test card will resend.
## Packets of size 80 bytes, 512 bytes, and MAX_FRAME_SIZE bytes
## will be sent.
##
## All SEND packets should be received by the test card.
##

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#	-SECTION_START-( 13.01 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 13.01 )

#	-SECTION_START-( 13.02 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 13.02 )

# And open and setup the cards.

#	-SECTION_START-( 13.03 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TRUSTED_CARD%
#	-SECTION_END-( 13.03 )

#	-SECTION_START-( 13.04 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED
#	-SECTION_END-( 13.04 )

#	-SECTION_START-( 13.05 )
Open                                            +
    OpenInstance=2                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 13.05 )

#	-SECTION_START-( 13.06 )
SetPacketFilter                                 +
    OpenInstance=2                              +
    PacketFilter=DIRECTED|BROADCAST|FUNCTIONAL
#	-SECTION_END-( 13.06 )

#	-SECTION_START-( 13.07 )
SetFunctionalAddress                            +
    OpenInstance=2                              +
    FunctionalAddress=%TP_FUNCTIONAL_ADDRESS%
#	-SECTION_END-( 13.07 )

# Now set the trusted and test cards to receive packets.

#	-SECTION_START-( 13.08 )
Receive                                         +
    OpenInstance=1
#	-SECTION_END-( 13.08 )

#	-SECTION_START-( 13.09 )
Receive                                         +
    OpenInstance=2
#	-SECTION_END-( 13.09 )

########################################################

# Send 80 byte packets.

#	-SECTION_START-( 13.10 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_TEST_CARD_ADDRESS%   +
    PacketSize=80                               +
    Number=10                                   +
    ResendAddress=%TP_TRUSTED_CARD_ADDRESS%
#	-SECTION_END-( 13.10 )

#	-SECTION_START-( 13.11 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 13.11 )

########################################################

# Send 512 byte packets.

#	-SECTION_START-( 13.12 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_TEST_CARD_ADDRESS%   +
    PacketSize=512                              +
    Number=10                                   +
    ResendAddress=%TP_TRUSTED_CARD_ADDRESS%
#	-SECTION_END-( 13.12 )

#	-SECTION_START-( 13.13 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 13.13 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#	-SECTION_START-( 13.14 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_TEST_CARD_ADDRESS%   +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10                                   +
    ResendAddress=%TP_TRUSTED_CARD_ADDRESS%
#	-SECTION_END-( 13.14 )

#	-SECTION_START-( 13.15 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 13.15 )

########################################################

# Then stop the receives and dump the statistics.

#	-SECTION_START-( 13.16 )
StopReceive                                     +
    OpenInstance=1
#	-SECTION_END-( 13.16 )

#	-SECTION_START-( 13.17 )
StopReceive                                     +
    OpenInstance=2
#	-SECTION_END-( 13.17 )

########################################################

# Finally close the adapters, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 13.18 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 13.18 )

#	-SECTION_START-( 13.19 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 13.19 )

#	-SECTION_START-( 13.20 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 13.20 )

#	-SECTION_START-( 13.21 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 13.21 )

#	-SECTION_END-( 13.00 )

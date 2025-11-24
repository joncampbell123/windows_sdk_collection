#	-SECTION_START-( 15.00 ) -SECTION_DESC-( "Send Directed, Receive Directed & Broadcast & Functional & Resend Functional" )
##
## TITLE: 1.3.1.14.22 Send Directed, Receive Directed & Broadcast &
## TITLE:             Functional & Resend Functional (1M/2C/1O)
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

#	-SECTION_START-( 15.01 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 15.01 )

#	-SECTION_START-( 15.02 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 15.02 )

# And open and setup the cards.

#	-SECTION_START-( 15.03 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TRUSTED_CARD%
#	-SECTION_END-( 15.03 )

#	-SECTION_START-( 15.04 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=FUNCTIONAL
#	-SECTION_END-( 15.04 )

#	-SECTION_START-( 15.05 )
SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=%TP_FUNCTIONAL_ADDRESS%
#	-SECTION_END-( 15.05 )

#	-SECTION_START-( 15.06 )
Open                                            +
    OpenInstance=2                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 15.06 )

#	-SECTION_START-( 15.07 )
SetPacketFilter                                 +
    OpenInstance=2                              +
    PacketFilter=DIRECTED|BROADCAST|FUNCTIONAL
#	-SECTION_END-( 15.07 )

#	-SECTION_START-( 15.08 )
SetFunctionalAddress                            +
    OpenInstance=2                              +
    FunctionalAddress=%TP_FUNCTIONAL_ADDRESS%
#	-SECTION_END-( 15.08 )

# Now set the trusted and test cards to receive packets.

#	-SECTION_START-( 15.09 )
Receive                                         +
    OpenInstance=1
#	-SECTION_END-( 15.09 )

#	-SECTION_START-( 15.10 )
Receive                                         +
    OpenInstance=2
#	-SECTION_END-( 15.10 )

########################################################

# Send 80 byte packets.

#	-SECTION_START-( 15.11 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_TEST_CARD_ADDRESS%   +
    PacketSize=80                               +
    Number=10                                   +
    ResendAddress=C0-00-%TP_FUNCTIONAL_ADDRESS%
#	-SECTION_END-( 15.11 )

#	-SECTION_START-( 15.12 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 15.12 )

########################################################

# Send 512 byte packets.

#	-SECTION_START-( 15.13 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_TEST_CARD_ADDRESS%   +
    PacketSize=512                              +
    Number=10                                   +
    ResendAddress=C0-00-%TP_FUNCTIONAL_ADDRESS%
#	-SECTION_END-( 15.13 )

#	-SECTION_START-( 15.14 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 15.14 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#	-SECTION_START-( 15.15 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_TEST_CARD_ADDRESS%   +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10                                   +
    ResendAddress=C0-00-%TP_FUNCTIONAL_ADDRESS%
#	-SECTION_END-( 15.15 )

#	-SECTION_START-( 15.16 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 15.16 )

########################################################

# Then stop the receives and dump the statistics.

#	-SECTION_START-( 15.17 )
StopReceive                                     +
    OpenInstance=1
#	-SECTION_END-( 15.17 )

#	-SECTION_START-( 15.18 )
StopReceive                                     +
    OpenInstance=2
#	-SECTION_END-( 15.18 )

########################################################

# Finally close the adapters, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 15.19 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 15.19 )

#	-SECTION_START-( 15.20 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 15.20 )

#	-SECTION_START-( 15.21 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 15.21 )

#	-SECTION_START-( 15.22 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 15.22 )

#	-SECTION_END-( 15.00 )

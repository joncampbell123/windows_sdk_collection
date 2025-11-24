#	-SECTION_START-( 9.00 ) -SECTION_DESC-( "Send Directed, Receive Directed & Source Routing & Resend Functional" ) -OPTIONALS-( SOURCEROUTING )
##
## TITLE: 1.3.1.14.13 Send Directed, Receive Directed & Source Routing
## TITLE:             & Resend Functional (1M/2C/1O)
##
## 1 Machine - 2 Cards - 1 Open Instance on each card.  These tests
## will have a trusted card send 10 DIRECTED packets to the card
## being tested.  The card being tested will have its packetfilter
## set to receive DIRECTED & SOURCEROUTING packets.  Each packet
## will contain a resend packet the test card will resend.
## Packets of size 80 bytes, 512 bytes, and MAX_FRAME_SIZE
## bytes will be sent.
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
    AdapterName=%TP_TRUSTED_CARD%
#	-SECTION_END-( 9.03 )

#	-SECTION_START-( 9.04 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=FUNCTIONAL
#	-SECTION_END-( 9.04 )

#	-SECTION_START-( 9.05 )
SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=%TP_FUNCTIONAL_ADDRESS%
#	-SECTION_END-( 9.05 )

#	-SECTION_START-( 9.06 )
Open                                            +
    OpenInstance=2                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 9.06 )

#	-SECTION_START-( 9.07 )
SetPacketFilter                                 +
    OpenInstance=2                              +
    PacketFilter=DIRECTED|SOURCEROUTING
#	-SECTION_END-( 9.07 )

# Now set the trusted and test cards to receive packets.

#	-SECTION_START-( 9.08 )
Receive                                         +
    OpenInstance=1
#	-SECTION_END-( 9.08 )

#	-SECTION_START-( 9.09 )
Receive                                         +
    OpenInstance=2
#	-SECTION_END-( 9.09 )

########################################################

# Send 80 byte packets.

#	-SECTION_START-( 9.10 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_TEST_CARD_ADDRESS%   +
    PacketSize=80                               +
    Number=10                                   +
    ResendAddress=C0-00-%TP_FUNCTIONAL_ADDRESS%
#	-SECTION_END-( 9.10 )

#	-SECTION_START-( 9.11 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 9.11 )

########################################################

# Send 512 byte packets.

#	-SECTION_START-( 9.12 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_TEST_CARD_ADDRESS%   +
    PacketSize=512                              +
    Number=10                                   +
    ResendAddress=C0-00-%TP_FUNCTIONAL_ADDRESS%
#	-SECTION_END-( 9.12 )

#	-SECTION_START-( 9.13 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 9.13 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#	-SECTION_START-( 9.14 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_TEST_CARD_ADDRESS%   +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10                                   +
    ResendAddress=C0-00-%TP_FUNCTIONAL_ADDRESS%
#	-SECTION_END-( 9.14 )

#	-SECTION_START-( 9.15 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 9.15 )

########################################################

# Then stop the receives and dump the statistics.

#	-SECTION_START-( 9.16 )
StopReceive                                     +
    OpenInstance=1
#	-SECTION_END-( 9.16 )

#	-SECTION_START-( 9.17 )
StopReceive                                     +
    OpenInstance=2
#	-SECTION_END-( 9.17 )

########################################################

# Finally close the adapters, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 9.18 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 9.18 )

#	-SECTION_START-( 9.19 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 9.19 )

#	-SECTION_START-( 9.20 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 9.20 )

#	-SECTION_START-( 9.21 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 9.21 )

#	-SECTION_END-( 9.00 )

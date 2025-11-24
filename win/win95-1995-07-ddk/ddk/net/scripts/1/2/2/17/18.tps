#   -SECTION_START-( 18.00 ) -SECTION_DESC-( "Send Functional, Receive Functional & Directed & Broadcast & Source Routing & Mac Frame & Resend Directed" ) -OPTIONALS-( SOURCEROUTING,MACFRAME )
##
## TITLE: 1.2.2.17.18 Send Functional, Receive Functional & Directed &
## TITLE:             Broadcast & Source Routing & Mac Frame & Resend
## TITLE:             Directed (1M/1C/2O)
##
## 1 Machine - 1 Card - 2 Open Instances on the card.  These tests
## will have the test card send 10 FUNCTIONAL packets to another open
## on the test card.  The test card will have its packetfilter
## set to receive FUNCTIONAL & DIRECTED & BROADCAST & SOURCEROUTING
## & MACFRAME packets.  Each packet will contain a resend packet the
## test card will resend.  Packets of size 80 bytes, 512 bytes, and
## MAX_FRAME_SIZE bytes will be sent.
##
## All SEND packets should be received by the test card.
##

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#	-SECTION_START-( 18.01 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 18.01 )

#	-SECTION_START-( 18.02 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 18.02 )

# And open and setup the cards.

#	-SECTION_START-( 18.03 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 18.03 )

#	-SECTION_START-( 18.04 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=FUNCTIONAL
#	-SECTION_END-( 18.04 )

#	-SECTION_START-( 18.05 )
SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=%TP_FUNCTIONAL_ADDRESS%
#	-SECTION_END-( 18.05 )

#	-SECTION_START-( 18.06 )
Open                                            +
    OpenInstance=2                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 18.06 )

#	-SECTION_START-( 18.07 )
SetPacketFilter                                 +
    OpenInstance=2                              +
    PacketFilter=FUNCTIONAL|DIRECTED|BROADCAST|SOURCEROUTING|MACFRAME
#	-SECTION_END-( 18.07 )

#	-SECTION_START-( 18.08 )
SetFunctionalAddress                            +
    OpenInstance=2                              +
    FunctionalAddress=%TP_FUNCTIONAL_ADDRESS%
#	-SECTION_END-( 18.08 )

########################################################

# Now set the trusted and test cards to receive packets.

#	-SECTION_START-( 18.09 )
Receive                                         +
    OpenInstance=1
#	-SECTION_END-( 18.09 )

#	-SECTION_START-( 18.10 )
Receive                                         +
    OpenInstance=2
#	-SECTION_END-( 18.10 )

########################################################

# Send 80 byte packets.

#	-SECTION_START-( 18.11 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=80                               +
    Number=10                                   +
    ResendAddress=%TP_TEST_CARD_ADDRESS%
#	-SECTION_END-( 18.11 )

#	-SECTION_START-( 18.12 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 18.12 )

########################################################

# Send 512 byte packets.

#	-SECTION_START-( 18.13 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=512                              +
    Number=10                                   +
    ResendAddress=%TP_TEST_CARD_ADDRESS%
#	-SECTION_END-( 18.13 )

#	-SECTION_START-( 18.14 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 18.14 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#	-SECTION_START-( 18.15 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10                                   +
    ResendAddress=%TP_TEST_CARD_ADDRESS%
#	-SECTION_END-( 18.15 )

#	-SECTION_START-( 18.16 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 18.16 )

########################################################

# Then stop the receives and dump the statistics.

#	-SECTION_START-( 18.17 )
StopReceive                                     +
    OpenInstance=1
#	-SECTION_END-( 18.17 )

#	-SECTION_START-( 18.18 )
StopReceive                                     +
    OpenInstance=2
#	-SECTION_END-( 18.18 )

########################################################

# Finally close the adapters, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 18.19 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 18.19 )

#	-SECTION_START-( 18.20 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 18.20 )

#	-SECTION_START-( 18.21 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 18.21 )

#	-SECTION_START-( 18.22 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 18.22 )

#	-SECTION_END-( 18.00 )

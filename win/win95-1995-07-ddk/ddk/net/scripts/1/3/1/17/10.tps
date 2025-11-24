#	-SECTION_START-( 10.00 ) -SECTION_DESC-( "Send Functional, Receive Functional & Mac Frame & Resend Directed" ) -OPTIONALS-( MACFRAME )
##
## TITLE: 1.3.1.17.10 Send Functional, Receive Functional & Mac Frame
## TITLE:             & Resend Directed (1M/2C/1O)
##
## 1 Machine - 2 Cards - 1 Open Instance on each card.  These tests
## will have a trusted card send 10 FUNCTIONAL packets to the card
## being tested.  The card being tested will have its packetfilter
## set to receive FUNCTIONAL & MACFRAME packets.  Each packet will
## contain a resend packet the test card will resend.  Packets of
## size 80 bytes, 512 bytes, and MAX_FRAME_SIZE bytes will be sent.
##
## All SEND packets should be received by the test card.
##

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#	-SECTION_START-( 10.01 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 10.01 )

#	-SECTION_START-( 10.02 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 10.02 )

# And open and setup the cards.

#	-SECTION_START-( 10.03 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TRUSTED_CARD%
#	-SECTION_END-( 10.03 )

#	-SECTION_START-( 10.04 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED
#	-SECTION_END-( 10.04 )

#	-SECTION_START-( 10.05 )
Open                                            +
    OpenInstance=2                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 10.05 )

#	-SECTION_START-( 10.06 )
SetPacketFilter                                 +
    OpenInstance=2                              +
    PacketFilter=FUNCTIONAL|MACFRAME
#	-SECTION_END-( 10.06 )

#	-SECTION_START-( 10.07 )
SetFunctionalAddress                            +
    OpenInstance=2                              +
    FunctionalAddress=%TP_FUNCTIONAL_ADDRESS%
#	-SECTION_END-( 10.07 )

########################################################

# Now set the trusted and test cards to receive packets.

#	-SECTION_START-( 10.08 )
Receive                                         +
    OpenInstance=1
#	-SECTION_END-( 10.08 )

#	-SECTION_START-( 10.09 )
Receive                                         +
    OpenInstance=2
#	-SECTION_END-( 10.09 )

########################################################

# Send 80 byte packets.

#	-SECTION_START-( 10.10 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=80                               +
    Number=10                                   +
    ResendAddress=%TP_TRUSTED_CARD_ADDRESS%
#	-SECTION_END-( 10.10 )

#	-SECTION_START-( 10.11 )
WaitSend                                        + OpenInstance=1
#	-SECTION_END-( 10.11 )

########################################################

# Send 512 byte packets.

#	-SECTION_START-( 10.12 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=512                              +
    Number=10                                   +
    ResendAddress=%TP_TRUSTED_CARD_ADDRESS%
#	-SECTION_END-( 10.12 )

#	-SECTION_START-( 10.13 )
WaitSend                                        + OpenInstance=1
#	-SECTION_END-( 10.13 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#	-SECTION_START-( 10.14 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10                                   +
    ResendAddress=%TP_TRUSTED_CARD_ADDRESS%
#	-SECTION_END-( 10.14 )

#	-SECTION_START-( 10.15 )
WaitSend                                        + OpenInstance=1
#	-SECTION_END-( 10.15 )

########################################################

# Then stop the receives and dump the statistics.

#	-SECTION_START-( 10.16 )
StopReceive                                     +
    OpenInstance=1
#	-SECTION_END-( 10.16 )

#	-SECTION_START-( 10.17 )
StopReceive                                     +
    OpenInstance=2
#	-SECTION_END-( 10.17 )

########################################################

# Finally close the adapters, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 10.18 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 10.18 )

#	-SECTION_START-( 10.19 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 10.19 )

#	-SECTION_START-( 10.20 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 10.20 )

#	-SECTION_START-( 10.21 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 10.21 )

#	-SECTION_END-( 10.00 )

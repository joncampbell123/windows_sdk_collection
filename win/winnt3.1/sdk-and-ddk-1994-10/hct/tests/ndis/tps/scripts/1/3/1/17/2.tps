#	-SECTION_START-( 2.00 ) -SECTION_DESC-( "Send Functional, Receive Functional & Directed & Resend Broadcast" )
##
## TITLE: 1.3.1.17.2 Send Functional, Receive Functional & Directed
## TITLE:            & Resend Broadcast (1M/2C/1O)
##
## 1 Machine - 2 Cards - 1 Open Instance on each card.  These tests
## will have a trusted card send 10 FUNCTIONAL packets to the card
## being tested.  The card being tested will have its packetfilter
## set to receive FUNCTIONAL & BROADCAST packets.  Each packet will
## contain a resend packet the test card will resend.  Packets of
## size 80 bytes, 512 bytes, and MAX_FRAME_SIZE bytes will be sent.
##
## All SEND packets should be received by the test card.
##

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#	-SECTION_START-( 2.01 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 2.01 )

#	-SECTION_START-( 2.02 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 2.02 )

# And open and setup the cards.

#	-SECTION_START-( 2.03 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TRUSTED_CARD%
#	-SECTION_END-( 2.03 )

#	-SECTION_START-( 2.04 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=BROADCAST
#	-SECTION_END-( 2.04 )

#	-SECTION_START-( 2.05 )
Open                                            +
    OpenInstance=2                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 2.05 )

#	-SECTION_START-( 2.06 )
SetPacketFilter                                 +
    OpenInstance=2                              +
    PacketFilter=FUNCTIONAL|DIRECTED
#	-SECTION_END-( 2.06 )

#	-SECTION_START-( 2.07 )
SetFunctionalAddress                            +
    OpenInstance=2                              +
    FunctionalAddress=%TP_FUNCTIONAL_ADDRESS%
#	-SECTION_END-( 2.07 )

# Now set the trusted and test cards to receive packets.

#	-SECTION_START-( 2.08 )
Receive                                         +
    OpenInstance=1
#	-SECTION_END-( 2.08 )

#	-SECTION_START-( 2.09 )
Receive                                         +
    OpenInstance=2
#	-SECTION_END-( 2.09 )

########################################################

# Send 80 byte packets.

#	-SECTION_START-( 2.10 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=80                               +
    Number=10                                   +
    ResendAddress=%TP_BROADCAST_ADDRESS%
#	-SECTION_END-( 2.10 )

#	-SECTION_START-( 2.11 )
WaitSend                                        + OpenInstance=1
#	-SECTION_END-( 2.11 )

########################################################

# Send 512 byte packets.

#	-SECTION_START-( 2.12 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=512                              +
    Number=10                                   +
    ResendAddress=%TP_BROADCAST_ADDRESS%
#	-SECTION_END-( 2.12 )

#	-SECTION_START-( 2.13 )
WaitSend                                        + OpenInstance=1
#	-SECTION_END-( 2.13 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#	-SECTION_START-( 2.14 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10                                   +
    ResendAddress=%TP_BROADCAST_ADDRESS%
#	-SECTION_END-( 2.14 )

#	-SECTION_START-( 2.15 )
WaitSend                                        + OpenInstance=1
#	-SECTION_END-( 2.15 )

########################################################

# Then stop the receives and dump the statistics.

#	-SECTION_START-( 2.16 )
StopReceive                                     +
    OpenInstance=1
#	-SECTION_END-( 2.16 )

#	-SECTION_START-( 2.17 )
StopReceive                                     +
    OpenInstance=2
#	-SECTION_END-( 2.17 )

########################################################

# Finally close the adapters, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 2.18 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 2.18 )

#	-SECTION_START-( 2.19 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 2.19 )

#	-SECTION_START-( 2.20 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 2.20 )

#	-SECTION_START-( 2.21 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 2.21 )

#	-SECTION_END-( 2.00 )

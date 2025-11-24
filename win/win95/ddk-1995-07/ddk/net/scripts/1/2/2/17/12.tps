#	-SECTION_START-( 12.00 ) -SECTION_DESC-( "Send Functional, Receive Functional & Mac Frame & Resend Functional" ) -OPTIONALS-( MACFRAME )
##
## TITLE: 1.2.2.17.12 Send Functional, Receive Functional & Mac Frame
## TITLE:             & Resend Functional (1M/1C/2O)
##
## 1 Machine - 1 Card - 2 Open Instances on the card.  These tests
## will have the test card send 10 FUNCTIONAL packets to another open
## on the test card.  The test card will have its packetfilter
## set to receive FUNCTIONAL packets.  Each packet will contain
## a resend packet the test card will resend.  Packets of size
## 80 bytes, 512 bytes, and MAX_FRAME_SIZE bytes will be sent.
##
## All SEND packets should be received by the test card.
##

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#	-SECTION_START-( 12.01 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 12.01 )

#	-SECTION_START-( 12.02 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 12.02 )

# And open and setup the cards.

#	-SECTION_START-( 12.03 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 12.03 )

#	-SECTION_START-( 12.04 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=FUNCTIONAL
#	-SECTION_END-( 12.04 )

#	-SECTION_START-( 12.05 )
SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=%TP_FUNCTIONAL_ADDRESS%
#	-SECTION_END-( 12.05 )

#	-SECTION_START-( 12.06 )
Open                                            +
    OpenInstance=2                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 12.06 )

#	-SECTION_START-( 12.07 )
SetPacketFilter                                 +
    OpenInstance=2                              +
    PacketFilter=FUNCTIONAL|MACFRAME
#	-SECTION_END-( 12.07 )

#	-SECTION_START-( 12.08 )
SetFunctionalAddress                            +
    OpenInstance=2                              +
    FunctionalAddress=%TP_FUNCTIONAL_ADDRESS%
#	-SECTION_END-( 12.08 )

########################################################

# Now set the trusted and test cards to receive packets.

#	-SECTION_START-( 12.09 )
Receive                                         +
    OpenInstance=1
#	-SECTION_END-( 12.09 )

#	-SECTION_START-( 12.10 )
Receive                                         +
    OpenInstance=2
#	-SECTION_END-( 12.10 )

########################################################

# Send 80 byte packets.

#	-SECTION_START-( 12.11 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=80                               +
    Number=10                                   +
    ResendAddress=C0-00-%TP_FUNCTIONAL_ADDRESS%
#	-SECTION_END-( 12.11 )

#	-SECTION_START-( 12.12 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 12.12 )

########################################################

# Send 512 byte packets.

#	-SECTION_START-( 12.13 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=512                              +
    Number=10                                   +
    ResendAddress=C0-00-%TP_FUNCTIONAL_ADDRESS%
#	-SECTION_END-( 12.13 )

#	-SECTION_START-( 12.14 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 12.14 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#	-SECTION_START-( 12.15 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10                                   +
    ResendAddress=C0-00-%TP_FUNCTIONAL_ADDRESS%
#	-SECTION_END-( 12.15 )

#	-SECTION_START-( 12.16 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 12.16 )

########################################################

# Then stop the receives and dump the statistics.

#	-SECTION_START-( 12.17 )
StopReceive                                     +
    OpenInstance=1
#	-SECTION_END-( 12.17 )

#	-SECTION_START-( 12.18 )
StopReceive                                     +
    OpenInstance=2
#	-SECTION_END-( 12.18 )

########################################################

# Finally close the adapters, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 12.19 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 12.19 )

#	-SECTION_START-( 12.20 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 12.20 )

#	-SECTION_START-( 12.21 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 12.21 )

#	-SECTION_START-( 12.22 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 12.22 )

#	-SECTION_END-( 12.00 )

#	-SECTION_START-( 9.00 ) -SECTION_DESC-( "Send Functional, Receive & Functional & Mac Frame & Resend Functional-server-side" ) -OPTIONALS-( MACFRAME )
##
## TITLE: 2.1.1.16.12 Send Functional, Receive & Functional & Mac Frame
## TITLE:             & Resend Functional (2M/1C/1O) server-side
##
## 2 Machine - 1 Card - 1 Open Instance on the card.  These tests
## will have the trusted card on one machine send 10 FUNCTIONAL packets
## to the card being tested on another machine.  The test card will
## have its packetfilter set to receive FUNCTIONAL & MACFRAME packets.
## Each packet will contain a resend packet the test card will resend.
## Packets of size 80 bytes, 512 bytes, and MAX_FRAME_SIZE bytes will
## be sent.
##
## The server side of the test is run on the machine with the test
## card installed.
##
## All SEND packets should be received by the test card.
##

Disable MACFRAME

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#	-SECTION_START-( 9.01 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 9.01 )

# And open and setup the card.

#	-SECTION_START-( 9.02 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TRUSTED_CARD%
#	-SECTION_END-( 9.02 )

#	-SECTION_START-( 9.03 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|FUNCTIONAL
#	-SECTION_END-( 9.03 )

#	-SECTION_START-( 9.04 )
SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=%TP_FUNCTIONAL_ADDRESS%
#	-SECTION_END-( 9.04 )

# Now set the trusted card to receive packets.

#	-SECTION_START-( 9.05 )
Receive                                         +
    OpenInstance=1
#	-SECTION_END-( 9.05 )

########################################################

# Now wait for the client side to start the test

#	-SECTION_START-( 9.06 )
Pause                                           +
    OpenInstance=1                              +
    RemoteAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    TestSignature=21116121
#	-SECTION_END-( 9.06 )

########################################################

# Then start the test,

# Send 80 byte packets.

#	-SECTION_START-( 9.07 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=80                               +
    Number=10                                   +
    ResendAddress=C0-00-%TP_FUNCTIONAL_ADDRESS%
#	-SECTION_END-( 9.07 )

#	-SECTION_START-( 9.08 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 9.08 )

########################################################

# Send 512 byte packets.

#	-SECTION_START-( 9.09 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=512                              +
    Number=10                                   +
    ResendAddress=C0-00-%TP_FUNCTIONAL_ADDRESS%
#	-SECTION_END-( 9.09 )

#	-SECTION_START-( 9.10 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 9.10 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#	-SECTION_START-( 9.11 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10                                   +
    ResendAddress=C0-00-%TP_FUNCTIONAL_ADDRESS%
#	-SECTION_END-( 9.11 )

#	-SECTION_START-( 9.12 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 9.12 )

########################################################

# Now tell the client side to clean up.

#	-SECTION_START-( 9.13 )
Go                                              +
    OpenInstance=1                              +
    RemoteAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    TestSignature=21116122
#	-SECTION_END-( 9.13 )

########################################################

# Then stop the receive and dump the statistics.

#	-SECTION_START-( 9.14 )
StopReceive                                     +
    OpenInstance=1
#	-SECTION_END-( 9.14 )

########################################################

# Finally close the adapters, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 9.15 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 9.15 )

#	-SECTION_START-( 9.16 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 9.16 )

Enable

#	-SECTION_END-( 9.00 )

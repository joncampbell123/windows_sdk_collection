#	-SECTION_START-( 2.00 ) -SECTION_DESC-( "Send Functional, Receive Functional & Resend Broadcast-server-side" )
##
## TITLE: 2.1.1.12.2 Send Functional, Receive Functional & Resend
## TITLE:            Broadcast (2M/1C/1O) server-side
##
## 2 Machine - 1 Card - 1 Open Instance on the card.  These tests
## will have the trusted card on one machine send 10 FUNCTIONAL packets
## to the card being tested on another machine.  The test card will
## have its packetfilter set to receive FUNCTIONAL packets.  Each packet
## will contain a resend packet the test card will resend.  Packets
## of size 80 bytes, 512 bytes, and MAX_FRAME_SIZE bytes will be
## sent.
##
## The server side of the test is run on the machine with the trusted
## card installed.
##
## All SEND packets should be received by the test card.
##

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#	-SECTION_START-( 2.01 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 2.01 )

# And open and setup the cards.

#	-SECTION_START-( 2.02 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TRUSTED_CARD%
#	-SECTION_END-( 2.02 )

#	-SECTION_START-( 2.03 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|BROADCAST
#	-SECTION_END-( 2.03 )

# Now set the trusted and test cards to receive packets.

#	-SECTION_START-( 2.04 )
Receive                                         +
    OpenInstance=1
#	-SECTION_END-( 2.04 )

########################################################

# Now wait for the server side to start the test

#	-SECTION_START-( 2.05 )
Pause                                           +
    OpenInstance=1                              +
    RemoteAddress=C0-00-%TP_FUNCTIONAL_ADDRESS%       +
    TestSignature=2111221
#	-SECTION_END-( 2.05 )

########################################################

# Then start the test,

# Send 80 byte packets.

#	-SECTION_START-( 2.06 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=80                               +
    Number=10                                   +
    ResendAddress=%TP_BROADCAST_ADDRESS%
#	-SECTION_END-( 2.06 )

#	-SECTION_START-( 2.07 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 2.07 )

########################################################

# Send 512 byte packets.

#	-SECTION_START-( 2.08 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=512                              +
    Number=10                                   +
    ResendAddress=%TP_BROADCAST_ADDRESS%
#	-SECTION_END-( 2.08 )

#	-SECTION_START-( 2.09 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 2.09 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#	-SECTION_START-( 2.10 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10                                   +
    ResendAddress=%TP_BROADCAST_ADDRESS%
#	-SECTION_END-( 2.10 )

#	-SECTION_START-( 2.11 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 2.11 )

########################################################

# Now tell the server side to clean up.

#	-SECTION_START-( 2.12 )
Go                                              +
    OpenInstance=1                              +
    RemoteAddress=C0-00-%TP_FUNCTIONAL_ADDRESS%       +
    TestSignature=2111222
#	-SECTION_END-( 2.12 )

########################################################

# Then stop the receive and dump the statistics.

#	-SECTION_START-( 2.13 )
StopReceive                                     +
    OpenInstance=1
#	-SECTION_END-( 2.13 )

########################################################

# Finally close the adapters, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 2.14 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 2.14 )

#	-SECTION_START-( 2.15 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 2.15 )

#	-SECTION_END-( 2.00 )

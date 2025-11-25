#	-SECTION_START-( 6.00 ) -SECTION_DESC-( "Send Broadcast, Receive Broadcast & Functional & Resend Functional-server-side" )
##
## TITLE: 2.1.1.14.10 Send Broadcast, Receive Broadcast & Functional & Resend
## TITLE:             Functional (2M/1C/1O) server-side
##
## 2 Machine - 1 Card - 1 Open Instance on the card.  These tests
## will have the trusted card on one machine send 10 BROADCAST packets
## to the card being tested on another machine.  The test card will
## have its packetfilter set to receive BROADCAST & FUNCTIONAL packets.
## Each packet will contain a resend packet the test card will resend.
## Packets of size 80 bytes, 512 bytes, and MAX_FRAME_SIZE bytes will
## be sent.
##
## The server side of the test is run on the machine with the trusted
## card installed.
##
## All SEND packets should be received by the test card.
##

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#	-SECTION_START-( 6.01 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 6.01 )

# And open and setup the cards.

#	-SECTION_START-( 6.02 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TRUSTED_CARD%
#	-SECTION_END-( 6.02 )

#	-SECTION_START-( 6.03 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|FUNCTIONAL
#	-SECTION_END-( 6.03 )

#	-SECTION_START-( 6.04 )
SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=%TP_FUNCTIONAL_ADDRESS%
#	-SECTION_END-( 6.04 )

# Now set the trusted and test cards to receive packets.

#	-SECTION_START-( 6.05 )
Receive                                         +
    OpenInstance=1
#	-SECTION_END-( 6.05 )

########################################################

# Now wait for the server side to start the test

#	-SECTION_START-( 6.06 )
Pause                                           +
    OpenInstance=1                              +
    RemoteAddress=%TP_BROADCAST_ADDRESS%    +
    TestSignature=21114101
#	-SECTION_END-( 6.06 )

########################################################

# Then start the test,

# Send 80 byte packets.

#	-SECTION_START-( 6.07 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_BROADCAST_ADDRESS%   +
    PacketSize=80                               +
    Number=10                                   +
    ResendAddress=C0-00-%TP_FUNCTIONAL_ADDRESS%
#	-SECTION_END-( 6.07 )

#	-SECTION_START-( 6.08 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 6.08 )

########################################################

# Send 512 byte packets.

#	-SECTION_START-( 6.09 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_BROADCAST_ADDRESS%   +
    PacketSize=512                              +
    Number=10                                   +
    ResendAddress=C0-00-%TP_FUNCTIONAL_ADDRESS%
#	-SECTION_END-( 6.09 )

#	-SECTION_START-( 6.10 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 6.10 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#	-SECTION_START-( 6.11 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_BROADCAST_ADDRESS%   +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10                                   +
    ResendAddress=C0-00-%TP_FUNCTIONAL_ADDRESS%
#	-SECTION_END-( 6.11 )

#	-SECTION_START-( 6.12 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 6.12 )

########################################################

# Now tell the server side to clean up.

#	-SECTION_START-( 6.13 )
Go                                              +
    OpenInstance=1                              +
    RemoteAddress=%TP_BROADCAST_ADDRESS%    +
    TestSignature=21114102
#	-SECTION_END-( 6.13 )

########################################################

# Then stop the receive and dump the statistics.

#	-SECTION_START-( 6.14 )
StopReceive                                     +
    OpenInstance=1
#	-SECTION_END-( 6.14 )

########################################################

# Finally close the adapters, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 6.15 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 6.15 )

#	-SECTION_START-( 6.16 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 6.16 )

#	-SECTION_END-( 6.00 )

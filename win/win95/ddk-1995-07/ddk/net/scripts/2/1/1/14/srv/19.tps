#	-SECTION_START-( 9.00 ) -SECTION_DESC-( "Send Broadcast, Receive Directed & Broadcast & Multicast & Resend Multicast-server-side" )
##
## TITLE: 2.1.1.14.19 Send Broadcast, Receive Directed & Broadcast &
## TITLE:             Multicast & Resend Multicast (2M/1C/1O) server-side
##
## 2 Machine - 1 Card - 1 Open Instance on the card.  These tests
## will have the trusted card on one machine send 10 BROADCAST packets
## to the card being tested on another machine.  The test card will
## have its packetfilter set to receive BROADCAST & MULTICAST packets.
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

#	-SECTION_START-( 9.01 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 9.01 )

# And open and setup the cards.

#	-SECTION_START-( 9.02 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TRUSTED_CARD%
#	-SECTION_END-( 9.02 )

#	-SECTION_START-( 9.03 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|MULTICAST
#	-SECTION_END-( 9.03 )

#	-SECTION_START-( 9.04 )
AddMulticastAddress                             +
    OpenInstance=1                              +
    MulticastAddress=%TP_MULTICAST_ADDRESS%
#	-SECTION_END-( 9.04 )

# Now set the trusted and test cards to receive packets.

#	-SECTION_START-( 9.05 )
Receive                                         +
    OpenInstance=1
#	-SECTION_END-( 9.05 )

########################################################

# Now wait for the server side to start the test

#	-SECTION_START-( 9.06 )
Pause                                           +
    OpenInstance=1                              +
    RemoteAddress=%TP_REM_TEST_CARD_ADDRESS%    +
    TestSignature=21114191
#	-SECTION_END-( 9.06 )

########################################################

# Then start the test,

# Send 80 byte packets.

#	-SECTION_START-( 9.07 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_BROADCAST_ADDRESS%   +
    PacketSize=80                               +
    Number=10                                   +
    ResendAddress=%TP_MULTICAST_ADDRESS%
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
    DestinationAddress=%TP_BROADCAST_ADDRESS%   +
    PacketSize=512                              +
    Number=10                                   +
    ResendAddress=%TP_MULTICAST_ADDRESS%
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
    DestinationAddress=%TP_BROADCAST_ADDRESS%   +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10                                   +
    ResendAddress=%TP_MULTICAST_ADDRESS%
#	-SECTION_END-( 9.11 )

#	-SECTION_START-( 9.12 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 9.12 )

########################################################

# Now tell the server side to clean up.

#	-SECTION_START-( 9.13 )
Go                                              +
    OpenInstance=1                              +
    RemoteAddress=%TP_REM_TEST_CARD_ADDRESS%    +
    TestSignature=21114192
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

#	-SECTION_END-( 9.00 )

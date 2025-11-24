#	-SECTION_START-( 8.00 ) -SECTION_DESC-( "Send Directed, Receive Directed & Mac Frame & Resend Broadcast-server-side" ) -OPTIONALS-( MACFRAME )
##
## TITLE: 2.1.1.13.15 Send Directed, Receive Directed & Mac Frame &
## TITLE:             Resend Broadcast (2M/1C/1O) server-side
##
## 2 Machine - 1 Card - 1 Open Instance on the card.  These tests
## will have the trusted card on one machine send 10 DIRECTED packets
## to the card being tested on another machine.  The test card will
## have its packetfilter set to receive DIRECTED & MAC FRAME packets.
## Each packet will contain a resend packet the test card will resend.
## Packets of size 80 bytes, 512 bytes, and MAX_FRAME_SIZE bytes will
## be sent.
##
## The server side of the test is run on the machine with the trusted
## card installed.
##
## All SEND packets should be received by the test card.
##

Disable MACFRAME

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#	-SECTION_START-( 8.01 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 8.01 )

# And open and setup the cards.

#	-SECTION_START-( 8.02 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TRUSTED_CARD%
#	-SECTION_END-( 8.02 )

#	-SECTION_START-( 8.03 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|BROADCAST
#	-SECTION_END-( 8.03 )

# Now set the trusted and test cards to receive packets.

#	-SECTION_START-( 8.04 )
Receive                                         +
    OpenInstance=1
#	-SECTION_END-( 8.04 )

########################################################

# Now wait for the server side to start the test

#	-SECTION_START-( 8.05 )
Pause                                           +
    OpenInstance=1                              +
    RemoteAddress=%TP_REM_TEST_CARD_ADDRESS%    +
    TestSignature=21113151
#	-SECTION_END-( 8.05 )

########################################################

# Then start the test,

# Send 80 byte packets.

#	-SECTION_START-( 8.06 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_REM_TEST_CARD_ADDRESS% +
    PacketSize=80                               +
    Number=10                                   +
    ResendAddress=%TP_BROADCAST_ADDRESS%
#	-SECTION_END-( 8.06 )

#	-SECTION_START-( 8.07 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 8.07 )

########################################################

# Send 512 byte packets.

#	-SECTION_START-( 8.08 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_REM_TEST_CARD_ADDRESS% +
    PacketSize=512                              +
    Number=10                                   +
    ResendAddress=%TP_BROADCAST_ADDRESS%
#	-SECTION_END-( 8.08 )

#	-SECTION_START-( 8.09 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 8.09 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#	-SECTION_START-( 8.10 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_REM_TEST_CARD_ADDRESS% +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10                                   +
    ResendAddress=%TP_BROADCAST_ADDRESS%
#	-SECTION_END-( 8.10 )

#	-SECTION_START-( 8.11 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 8.11 )

########################################################

# Now tell the server side to clean up.

#	-SECTION_START-( 8.12 )
Go                                              +
    OpenInstance=1                              +
    RemoteAddress=%TP_REM_TEST_CARD_ADDRESS%    +
    TestSignature=21113152
#	-SECTION_END-( 8.12 )

########################################################

# Then stop the receive and dump the statistics.

#	-SECTION_START-( 8.13 )
StopReceive                                     +
    OpenInstance=1
#	-SECTION_END-( 8.13 )

########################################################

# Finally close the adapters, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 8.14 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 8.14 )

#	-SECTION_START-( 8.15 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 8.15 )

Enable

#	-SECTION_END-( 8.00 )

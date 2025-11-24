#	-SECTION_START-( 3.00 ) -SECTION_DESC-( "Send Broadcast, Receive Promiscuous-server-side" ) -OPTIONALS-( PROMISCUOUS )
##
## TITLE: 2.1.1.2.3 Send Broadcast, Receive Promiscuous (2M/1C/1O) server-side
##
## 2 Machine - 1 Card - 1 Open Instance on the card.  These tests
## will have the trusted card on one machine send 10 BROADCAST packets
## to the card being tested on another machine.  The test card will
## have its packetfilter set to receive PROMISCUOUS packets.  Packets
## of size 40 bytes, 512 bytes, and MAX_FRAME_SIZE bytes will be
## sent.
##
## The server side of the test is run on the machine with the trusted
## card installed.
##
## No packets should be received by the test card.
##

Disable PROMISCUOUS

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#	-SECTION_START-( 3.01 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 3.01 )

# And open and setup the cards.

#	-SECTION_START-( 3.02 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TRUSTED_CARD%
#	-SECTION_END-( 3.02 )

#	-SECTION_START-( 3.03 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED
#	-SECTION_END-( 3.03 )

########################################################

# Now wait for the server side to start the test

#	-SECTION_START-( 3.04 )
Pause                                           +
    OpenInstance=1                              +
    RemoteAddress=%TP_REM_TEST_CARD_ADDRESS%    +
    TestSignature=211231
#	-SECTION_END-( 3.04 )

########################################################

# Then start the test,

# Send 40 byte packets.

#	-SECTION_START-( 3.05 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_BROADCAST_ADDRESS%   +
    PacketSize=40                               +
    Number=10
#	-SECTION_END-( 3.05 )

#	-SECTION_START-( 3.06 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 3.06 )

########################################################

# Send 512 byte packets.

#	-SECTION_START-( 3.07 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_BROADCAST_ADDRESS%   +
    PacketSize=512                              +
    Number=10
#	-SECTION_END-( 3.07 )

#	-SECTION_START-( 3.08 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 3.08 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#	-SECTION_START-( 3.09 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_BROADCAST_ADDRESS%   +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10
#	-SECTION_END-( 3.09 )

#	-SECTION_START-( 3.10 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 3.10 )

########################################################

# Now tell the server side to clean up.

#	-SECTION_START-( 3.11 )
Go                                              +
    OpenInstance=1                              +
    RemoteAddress=%TP_REM_TEST_CARD_ADDRESS%    +
    TestSignature=211232
#	-SECTION_END-( 3.11 )

########################################################

# Finally close the adapter, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 3.12 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 3.12 )

#	-SECTION_START-( 3.13 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 3.13 )

Enable

#	-SECTION_END-( 3.00 )

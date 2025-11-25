#	-SECTION_START-( 7.00 ) -SECTION_DESC-( "Send Directed, Receive Directed & Broadcast & Multicast & Resend Directed-server-side" )
##
## TITLE: 2.1.1.13.17 Send Directed, Receive Directed & Broadcast &
## TITLE:             Multicast & Resend Directed (2M/1C/1O) server-side
##
## 2 Machine - 1 Card - 1 Open Instance on the card.  These tests
## will have the trusted card on one machine send 10 DIRECTED packets
## to the card being tested on another machine.  The test card will
## have its packetfilter set to receive DIRECTED & BROADCAST & MULTICAST
## packets. Each packet will contain a resend packet the test card will
## resend.  Packets of size 80 bytes, 512 bytes, and MAX_FRAME_SIZE bytes
## will be sent.
##
## The server side of the test is run on the machine with the trusted
## card installed.
##
## All SEND packets should be received by the test card.
##

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#	-SECTION_START-( 7.01 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 7.01 )

# And open and setup the cards.

#	-SECTION_START-( 7.02 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TRUSTED_CARD%
#	-SECTION_END-( 7.02 )

#	-SECTION_START-( 7.03 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED
#	-SECTION_END-( 7.03 )

# Now set the trusted and test cards to receive packets.

#	-SECTION_START-( 7.04 )
Receive                                         +
    OpenInstance=1
#	-SECTION_END-( 7.04 )

########################################################

# Now wait for the server side to start the test

#	-SECTION_START-( 7.05 )
Pause                                           +
    OpenInstance=1                              +
    RemoteAddress=%TP_REM_TEST_CARD_ADDRESS%    +
    TestSignature=21113171
#	-SECTION_END-( 7.05 )

########################################################

# Then start the test,

# Send 80 byte packets.

#	-SECTION_START-( 7.06 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_REM_TEST_CARD_ADDRESS% +
    PacketSize=80                               +
    Number=10                                   +
    ResendAddress=%TP_TRUSTED_CARD_ADDRESS%
#	-SECTION_END-( 7.06 )

#	-SECTION_START-( 7.07 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 7.07 )

########################################################

# Send 512 byte packets.

#	-SECTION_START-( 7.08 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_REM_TEST_CARD_ADDRESS% +
    PacketSize=512                              +
    Number=10                                   +
    ResendAddress=%TP_TRUSTED_CARD_ADDRESS%
#	-SECTION_END-( 7.08 )

#	-SECTION_START-( 7.09 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 7.09 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#	-SECTION_START-( 7.10 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_REM_TEST_CARD_ADDRESS% +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10                                   +
    ResendAddress=%TP_TRUSTED_CARD_ADDRESS%
#	-SECTION_END-( 7.10 )

#	-SECTION_START-( 7.11 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 7.11 )

########################################################

# Now tell the server side to clean up.

#	-SECTION_START-( 7.12 )
Go                                              +
    OpenInstance=1                              +
    RemoteAddress=%TP_REM_TEST_CARD_ADDRESS%    +
    TestSignature=21113172
#	-SECTION_END-( 7.12 )

########################################################

# Then stop the receive and dump the statistics.

#	-SECTION_START-( 7.13 )
StopReceive                                     +
    OpenInstance=1
#	-SECTION_END-( 7.13 )

########################################################

# Finally close the adapters, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 7.14 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 7.14 )

#	-SECTION_START-( 7.15 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 7.15 )

#	-SECTION_END-( 7.00 )

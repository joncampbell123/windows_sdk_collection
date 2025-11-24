#	-SECTION_START-( 1.00 ) -SECTION_DESC-( "Send Functional, Receive Directed & Broadcast & Functional-server-side" )
##
## TITLE: 2.1.1.8.8 Send Functional, Receive Directed & Broadcast &
## TITLE:           Functional (2M/1C/1O) server-side
##
## 2 Machine - 1 Card - 1 Open Instance on the card.  These tests
## will have the trusted card on one machine send 10 FUNCTIONAL packets
## to the card being tested on another machine.  The test card will
## have its packetfilter set to receive DIRECTED & BROADCAST & FUNCTIONAL
## packets.  Packets of size 40 bytes, 512 bytes, and MAX_FRAME_SIZE
## bytes will be sent.
##
## The server side of the test is run on the machine with the trusted
## card installed.
##
## All SEND packets should be received by the test card.
##

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#	-SECTION_START-( 1.01 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 1.01 )

# And open and setup the cards.

#	-SECTION_START-( 1.02 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TRUSTED_CARD%
#	-SECTION_END-( 1.02 )

#	-SECTION_START-( 1.03 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED
#	-SECTION_END-( 1.03 )

########################################################

# Now wait for the server side to start the test

#	-SECTION_START-( 1.04 )
Pause                                           +
    OpenInstance=1                              +
    RemoteAddress=%TP_REM_TEST_CARD_ADDRESS%    +
    TestSignature=211881
#	-SECTION_END-( 1.04 )

########################################################

# Then start the test,

# Send 40 byte packets.

#	-SECTION_START-( 1.05 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=40                               +
    Number=10
#	-SECTION_END-( 1.05 )

#	-SECTION_START-( 1.06 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 1.06 )

########################################################

# Send 512 byte packets.

#	-SECTION_START-( 1.07 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=512                              +
    Number=10
#	-SECTION_END-( 1.07 )

#	-SECTION_START-( 1.08 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 1.08 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#	-SECTION_START-( 1.09 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10
#	-SECTION_END-( 1.09 )

#	-SECTION_START-( 1.10 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 1.10 )

########################################################

# Now tell the server side to clean up.

#	-SECTION_START-( 1.11 )
Go                                              +
    OpenInstance=1                              +
    RemoteAddress=%TP_REM_TEST_CARD_ADDRESS%    +
    TestSignature=211882
#	-SECTION_END-( 1.11 )

########################################################

# Finally close the adapter, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 1.12 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 1.12 )

#	-SECTION_START-( 1.13 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 1.13 )

#	-SECTION_END-( 1.00 )

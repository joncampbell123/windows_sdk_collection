#	-SECTION_START-( 5.00 ) -SECTION_DESC-( "Send Functional, Receive All Functional-server-side" ) -OPTIONALS-( ALLFUNCTIONAL )
##
## TITLE: 2.1.1.4.5 Send Functional, Receive All Functional (2M/1C/1O)
## TITLE:           server-side
##
## 2 Machine - 1 Card - 1 Open Instance on the card.  These tests
## will have the trusted card on one machine send 10 FUNCTIONAL packets
## to the card being tested on another machine.  The test card will
## have its packetfilter set to receive ALLFUNCTIONAL packets.  Packets
## of size 40 bytes, 512 bytes, and MAX_FRAME_SIZE bytes will be
## sent.
##
## The server side of the test is run on the machine with the trusted
## card installed.
##
## All SEND packets should be received by the test card.
##

Disable ALLFUNCTIONAL

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#	-SECTION_START-( 5.01 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 5.01 )

# And open and setup the cards.

#	-SECTION_START-( 5.02 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TRUSTED_CARD%
#	-SECTION_END-( 5.02 )

#	-SECTION_START-( 5.03 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED
#	-SECTION_END-( 5.03 )

########################################################

# Now wait for the server side to start the test

#	-SECTION_START-( 5.04 )
Pause                                           +
    OpenInstance=1                              +
    RemoteAddress=C0-00-%TP_FUNCTIONAL_ADDRESS%       +
    TestSignature=211451
#	-SECTION_END-( 5.04 )

########################################################

# Then start the test,

# Send 40 byte packets.

#	-SECTION_START-( 5.05 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=40                               +
    Number=10
#	-SECTION_END-( 5.05 )

#	-SECTION_START-( 5.06 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 5.06 )

########################################################

# Send 512 byte packets.

#	-SECTION_START-( 5.07 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=512                              +
    Number=10
#	-SECTION_END-( 5.07 )

#	-SECTION_START-( 5.08 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 5.08 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#	-SECTION_START-( 5.09 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10
#	-SECTION_END-( 5.09 )

#	-SECTION_START-( 5.10 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 5.10 )

########################################################

# Now tell the server side to clean up.

#	-SECTION_START-( 5.11 )
Go                                              +
    OpenInstance=1                              +
    RemoteAddress=C0-00-%TP_FUNCTIONAL_ADDRESS%       +
    TestSignature=211452
#	-SECTION_END-( 5.11 )

########################################################

# Finally close the adapter, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 5.12 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 5.12 )

#	-SECTION_START-( 5.13 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 5.13 )

Enable

#	-SECTION_END-( 5.00 )

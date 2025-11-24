#	-SECTION_START-( 9.00 ) -SECTION_DESC-( "Send Broadcast, Receive Broadcast & Mac Frames Resend Functional-client-side" ) -OPTIONALS-( MACFRAME )
##
## TITLE: 2.1.1.14.16 Send Broadcast, Receive Broadcast & Mac Frames
## TITLE:             Resend Functional (2M/1C/1O) client-side
##
## 2 Machine - 1 Card - 1 Open Instance on the card.  These tests
## will have the trusted card on one machine send 10 BROADCAST packets
## to the card being tested on another machine.  The test card will
## have its packetfilter set to receive BROADCAST & MACFRAME packets.
## Each packet will contain a resend packet the test card will resend.
## Packets of size 80 bytes, 512 bytes, and MAX_FRAME_SIZE bytes will be
## sent.
##
## The client side of the test is run on the machine with the test
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
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 9.02 )

#	-SECTION_START-( 9.03 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=BROADCAST|MACFRAME
#	-SECTION_END-( 9.03 )

########################################################

# Now set the test card to receive packets,

#	-SECTION_START-( 9.04 )
Receive                                         +
    OpenInstance=1
#	-SECTION_END-( 9.04 )

# tell the client side to start the test

#	-SECTION_START-( 9.05 )
Go                                              +
    OpenInstance=1                              +
    RemoteAddress=%TP_REM_TRUSTED_CARD_ADDRESS% +
    TestSignature=21114161
#	-SECTION_END-( 9.05 )

# and wait for the client side to finish

#	-SECTION_START-( 9.06 )
Pause                                           +
    OpenInstance=1                              +
    RemoteAddress=%TP_REM_TRUSTED_CARD_ADDRESS% +
    TestSignature=21114162
#	-SECTION_END-( 9.06 )

########################################################

# Then stop the receive and dump the statistics.

#	-SECTION_START-( 9.07 )
StopReceive                                     +
    OpenInstance=1
#	-SECTION_END-( 9.07 )

########################################################

# Finally close the adapter, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 9.08 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 9.08 )

#	-SECTION_START-( 9.09 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 9.09 )

Enable

#	-SECTION_END-( 9.00 )

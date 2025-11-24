#	-SECTION_START-( 8.00 ) -SECTION_DESC-( "Send Functional, Receive & Functional & Mac Frames & Resend Broadcast-client-side" ) -OPTIONALS-( MACFRAME )
##
## TITLE: 2.1.1.16.11 Send Functional, Receive & Functional & Mac Frames
## TITLE:             & Resend Broadcast (2M/1C/1O) client-side
##
## 2 Machine - 1 Card - 1 Open Instance on the card.  These tests
## will have the trusted card on one machine send 10 FUNCTIONAL packets
## to the card being tested on another machine.  The test card will
## have its packetfilter set to receive FUNCTIONAL & MACFRAME packets.
## Each packet will contain a resend packet the test card will resend.
## Packets of size 80 bytes, 512 bytes, and MAX_FRAME_SIZE bytes will
## be sent.
##
## The client side of the test is run on the machine with the test
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

# And open and setup the card.

#	-SECTION_START-( 8.02 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 8.02 )

#	-SECTION_START-( 8.03 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=FUNCTIONAL|MACFRAME
#	-SECTION_END-( 8.03 )

#	-SECTION_START-( 8.04 )
SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=%TP_FUNCTIONAL_ADDRESS%
#	-SECTION_END-( 8.04 )

########################################################

# Now set the test card to receive packets,

#	-SECTION_START-( 8.05 )
Receive                                         +
    OpenInstance=1
#	-SECTION_END-( 8.05 )

# tell the client side to start the test

#	-SECTION_START-( 8.06 )
Go                                              +
    OpenInstance=1                              +
    RemoteAddress=%TP_REM_TRUSTED_CARD_ADDRESS% +
    TestSignature=21116111
#	-SECTION_END-( 8.06 )

# and wait for the client side to finish

#	-SECTION_START-( 8.07 )
Pause                                           +
    OpenInstance=1                              +
    RemoteAddress=%TP_REM_TRUSTED_CARD_ADDRESS% +
    TestSignature=21116112
#	-SECTION_END-( 8.07 )

########################################################

# Then stop the receive and dump the statistics.

#	-SECTION_START-( 8.08 )
StopReceive                                     +
    OpenInstance=1
#	-SECTION_END-( 8.08 )

########################################################

# Finally close the adapter, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 8.09 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 8.09 )

#	-SECTION_START-( 8.10 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 8.10 )

Enable

#	-SECTION_END-( 8.00 )

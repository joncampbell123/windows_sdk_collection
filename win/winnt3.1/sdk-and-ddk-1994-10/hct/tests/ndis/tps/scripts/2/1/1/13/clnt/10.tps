#	-SECTION_START-( 6.00 ) -SECTION_DESC-( "Send Directed, Receive Directed & Functional & Resend Functional-client-side" )
##
## TITLE: 2.1.1.13.10 Send Directed, Receive Directed & Functional &
## TITLE:             Resend Functional (2M/1C/1O) client-side
##
## 2 Machine - 1 Card - 1 Open Instance on the card.  These tests
## will have the trusted card on one machine send 10 DIRECTED packets
## to the card being tested on another machine.  The test card will
## have its packetfilter set to receive DIRECTED & FUNCTIONAL packets.
## Each packet will contain a resend packet the test card will resend.
## Packets of size 80 bytes, 512 bytes, and MAX_FRAME_SIZE bytes will
## be sent.
##
## The client side of the test is run on the machine with the test
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

# And open and setup the card.

#	-SECTION_START-( 6.02 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%
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

########################################################

# Now set the test card to receive packets,

#	-SECTION_START-( 6.05 )
Receive                                         +
    OpenInstance=1
#	-SECTION_END-( 6.05 )

# tell the client side to start the test

#	-SECTION_START-( 6.06 )
Go                                              +
    OpenInstance=1                              +
    RemoteAddress=%TP_REM_TRUSTED_CARD_ADDRESS% +
    TestSignature=21113101
#	-SECTION_END-( 6.06 )

# and wait for the client side to finish

#	-SECTION_START-( 6.07 )
Pause                                           +
    OpenInstance=1                              +
    RemoteAddress=%TP_REM_TRUSTED_CARD_ADDRESS% +
    TestSignature=21113102
#	-SECTION_END-( 6.07 )

########################################################

# Then stop the receive and dump the statistics.

#	-SECTION_START-( 6.08 )
StopReceive                                     +
    OpenInstance=1
#	-SECTION_END-( 6.08 )

########################################################

# Finally close the adapter, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 6.09 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 6.09 )

#	-SECTION_START-( 6.10 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 6.10 )

#	-SECTION_END-( 6.00 )

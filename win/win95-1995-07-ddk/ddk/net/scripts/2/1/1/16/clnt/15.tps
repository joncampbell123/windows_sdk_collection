#	-SECTION_START-( 12.00 ) -SECTION_DESC-( "Send Functional, Receive Directed & Functional & Broadcast & Resend Functional-client-side" )
##
## TITLE: 2.1.1.16.15 Send Functional, Receive Directed & Functional &
## TITLE              Broadcast & Resend Functional (2M/1C/1O) client-side
##
## 2 Machine - 1 Card - 1 Open Instance on the card.  These tests
## will have the trusted card on one machine send 10 FUNCTIONAL packets
## to the card being tested on another machine.  The test card will
## have its packetfilter set to receive DIRECTED & FUNCTIONAL & BROADCAST
## packets.  Each packet will contain a resend packet the test card will
## resend.  Packets of size 80 bytes, 512 bytes, and MAX_FRAME_SIZE bytes
## will be sent.
##
## The client side of the test is run on the machine with the test
## card installed.
##
## All SEND packets should be received by the test card.
##

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#	-SECTION_START-( 12.01 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 12.01 )

# And open and setup the card.

#	-SECTION_START-( 12.02 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 12.02 )

#	-SECTION_START-( 12.03 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|FUNCTIONAL|BROADCAST
#	-SECTION_END-( 12.03 )

#	-SECTION_START-( 12.04 )
SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=%TP_FUNCTIONAL_ADDRESS%
#	-SECTION_END-( 12.04 )

########################################################

# Now set the test card to receive packets,

#	-SECTION_START-( 12.05 )
Receive                                         +
    OpenInstance=1
#	-SECTION_END-( 12.05 )

# tell the client side to start the test

#	-SECTION_START-( 12.06 )
Go                                              +
    OpenInstance=1                              +
    RemoteAddress=%TP_REM_TRUSTED_CARD_ADDRESS% +
    TestSignature=21116151
#	-SECTION_END-( 12.06 )

# and wait for the client side to finish

#	-SECTION_START-( 12.07 )
Pause                                           +
    OpenInstance=1                              +
    RemoteAddress=%TP_REM_TRUSTED_CARD_ADDRESS% +
    TestSignature=21116152
#	-SECTION_END-( 12.07 )

########################################################

# Then stop the receive and dump the statistics.

#	-SECTION_START-( 12.08 )
StopReceive                                     +
    OpenInstance=1
#	-SECTION_END-( 12.08 )

########################################################

# Finally close the adapter, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 12.09 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 12.09 )

#	-SECTION_START-( 12.10 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 12.10 )

#	-SECTION_END-( 12.00 )

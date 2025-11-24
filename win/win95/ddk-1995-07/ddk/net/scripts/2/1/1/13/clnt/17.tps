#	-SECTION_START-( 7.00 ) -SECTION_DESC-( "Send Directed, Receive Directed & Broadcast & Multicast & Resend Directed-client-side" )
##
## TITLE: 2.1.1.13.17 Send Directed, Receive Directed & Broadcast &
## TITLE:             Multicast & Resend Directed (2M/1C/1O) client-side
##
## 2 Machine - 1 Card - 1 Open Instance on the card.  These tests
## will have the trusted card on one machine send 10 DIRECTED packets
## to the card being tested on another machine.  The test card will
## have its packetfilter set to receive DIRECTED & BROADCAST & FUNCTIONAL
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

#	-SECTION_START-( 7.01 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 7.01 )

# And open and setup the card.

#	-SECTION_START-( 7.02 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 7.02 )

#	-SECTION_START-( 7.03 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|BROADCAST|MULTICAST
#	-SECTION_END-( 7.03 )

#	-SECTION_START-( 7.04 )
AddMulticastAddress                             +
    OpenInstance=1                              +
    MulticastAddress=%TP_MULTICAST_ADDRESS%
#	-SECTION_END-( 7.04 )

########################################################

# Now set the test card to receive packets,

#	-SECTION_START-( 7.05 )
Receive                                         +
    OpenInstance=1
#	-SECTION_END-( 7.05 )

# tell the client side to start the test

#	-SECTION_START-( 7.06 )
Go                                              +
    OpenInstance=1                              +
    RemoteAddress=%TP_REM_TRUSTED_CARD_ADDRESS% +
    TestSignature=21113171
#	-SECTION_END-( 7.06 )

# and wait for the client side to finish

#	-SECTION_START-( 7.07 )
Pause                                           +
    OpenInstance=1                              +
    RemoteAddress=%TP_REM_TRUSTED_CARD_ADDRESS% +
    TestSignature=21113172
#	-SECTION_END-( 7.07 )

########################################################

# Then stop the receive and dump the statistics.

#	-SECTION_START-( 7.08 )
StopReceive                                     +
    OpenInstance=1
#	-SECTION_END-( 7.08 )

########################################################

# Finally close the adapter, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 7.09 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 7.09 )

#	-SECTION_START-( 7.10 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 7.10 )

#	-SECTION_END-( 7.00 )

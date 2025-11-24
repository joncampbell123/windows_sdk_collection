#	-SECTION_START-( 4.00 ) -SECTION_DESC-( "Send Broadcast, Receive Directed & Broadcast & Multicast-client-side" )
##
## TITLE: 2.1.1.6.10 Send Broadcast, Receive Directed & Broadcast &
## TITLE:            Multicast (2M/1C/1O) client-side
##
## 2 Machine - 1 Card - 1 Open Instance on the card.  These tests
## will have the trusted card on one machine send 10 BROADCAST packets
## to the card being tested on another machine.  The test card will
## have its packetfilter set to receive DIRECTED & BROADCAST & MULTICAST
## packets.  Packets of size 40 bytes, 512 bytes, and MAX_FRAME_SIZE
## bytes will be sent.
##
## The client side of the test is run on the machine with the test
## card installed.
##
## All SEND packets should be received by the test card.
##

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#	-SECTION_START-( 4.01 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 4.01 )

# And open and setup the card.

#	-SECTION_START-( 4.02 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 4.02 )

#	-SECTION_START-( 4.03 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|BROADCAST|MULTICAST
#	-SECTION_END-( 4.03 )

#	-SECTION_START-( 4.04 )
AddMulticastAddress                             +
    OpenInstance=1                              +
    MulticastAddress=%TP_MULTICAST_ADDRESS%
#	-SECTION_END-( 4.04 )

########################################################

# Now set the test card to receive packets,

#	-SECTION_START-( 4.05 )
Receive                                         +
    OpenInstance=1
#	-SECTION_END-( 4.05 )

# tell the client side to start the test

#	-SECTION_START-( 4.06 )
Go                                              +
    OpenInstance=1                              +
    RemoteAddress=%TP_REM_TRUSTED_CARD_ADDRESS% +
    TestSignature=2116101
#	-SECTION_END-( 4.06 )

# and wait for the client side to finish

#	-SECTION_START-( 4.07 )
Pause                                           +
    OpenInstance=1                              +
    RemoteAddress=%TP_REM_TRUSTED_CARD_ADDRESS% +
    TestSignature=2116102
#	-SECTION_END-( 4.07 )

########################################################

# Then stop the receives and dump the statistics.

#	-SECTION_START-( 4.08 )
StopReceive                                     +
    OpenInstance=1
#	-SECTION_END-( 4.08 )

########################################################

# Finally close the adapters, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 4.09 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 4.09 )

#	-SECTION_START-( 4.10 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 4.10 )

#	-SECTION_END-( 4.00 )

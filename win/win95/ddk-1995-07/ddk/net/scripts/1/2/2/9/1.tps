#	-SECTION_START-( 1.00 ) -SECTION_DESC-( "Send Functional, Receive Directed & Broadcast" )
##
## TITLE: 1.2.2.9.1 Send Functional, Receive Directed & Broadcast (1M/1C/2O)
##
## 1 Machine - 1 Card - 2 Open Instances on the card.  These tests
## will have the test card send 10 FUNCTIONAL packets to another open
## on the test card.  The test card will have its packetfilter
## set to receive DIRECTED and BROADCAST packets.  Packets of size
## 40 bytes, 512 bytes, and MAX_FRAME_SIZE bytes will be sent.
##
## No packets should be received by the test card.
##

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#	-SECTION_START-( 1.01 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 1.01 )

#	-SECTION_START-( 1.02 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 1.02 )

# And open and setup the cards.

#	-SECTION_START-( 1.03 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 1.03 )

#	-SECTION_START-( 1.04 )
Open                                            +
    OpenInstance=2                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 1.04 )

#	-SECTION_START-( 1.05 )
SetPacketFilter                                 +
    OpenInstance=2                              +
    PacketFilter=DIRECTED|BROADCAST
#	-SECTION_END-( 1.05 )

# Now set the test card to receive packets,

#	-SECTION_START-( 1.06 )
Receive                                         +
    OpenInstance=2
#	-SECTION_END-( 1.06 )

########################################################

# Send 40 byte packets.

#	-SECTION_START-( 1.07 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=40                               +
    Number=10
#	-SECTION_END-( 1.07 )

#	-SECTION_START-( 1.08 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 1.08 )

########################################################

# Send 512 byte packets.

#	-SECTION_START-( 1.09 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=512                              +
    Number=10
#	-SECTION_END-( 1.09 )

#	-SECTION_START-( 1.10 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 1.10 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#	-SECTION_START-( 1.11 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10
#	-SECTION_END-( 1.11 )

#	-SECTION_START-( 1.12 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 1.12 )

########################################################

# Then stop the receive and dump the statistics.

#	-SECTION_START-( 1.13 )
StopReceive                                     +
    OpenInstance=2
#	-SECTION_END-( 1.13 )

########################################################

# Finally close the adapters, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 1.14 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 1.14 )

#	-SECTION_START-( 1.15 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 1.15 )

#	-SECTION_START-( 1.16 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 1.16 )

#	-SECTION_START-( 1.17 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 1.17 )

#	-SECTION_END-( 1.00 )

#	-SECTION_START-( 2.00 ) -SECTION_DESC-( "Send Functional, Receive Directed & Functional" )
##
## TITLE: 1.2.2.9.2 Send Functional, Receive Directed & Functional (1M/1C/2O)
##
## 1 Machine - 1 Card - 2 Open Instances on the card.  These tests
## will have the test card send 10 FUNCTIONAL packets to another open
## on the test card.  The test card will have its packetfilter
## set to receive DIRECTED and FUNCTIONAL packets.  Packets of size
## 40 bytes, 512 bytes, and MAX_FRAME_SIZE bytes will be sent.
##
## All packets should be received by the test card when the proper
## functional address is set, and No packets should be received when
## no address is set, or the wrong address is set.
##

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#	-SECTION_START-( 2.01 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 2.01 )

#	-SECTION_START-( 2.02 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 2.02 )

# And open and setup the cards.

#	-SECTION_START-( 2.03 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 2.03 )

#	-SECTION_START-( 2.04 )
Open                                            +
    OpenInstance=2                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 2.04 )

#	-SECTION_START-( 2.05 )
SetPacketFilter                                 +
    OpenInstance=2                              +
    PacketFilter=DIRECTED|FUNCTIONAL
#	-SECTION_END-( 2.05 )

########################################################

# Now set the test card to receive packets,

#	-SECTION_START-( 2.06 )
Receive                                         +
    OpenInstance=2
#	-SECTION_END-( 2.06 )

########################################################

# With the no functional address set no packets
# should be received.

########################################################

# Send 40 byte packets.

#	-SECTION_START-( 2.07 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=40                               +
    Number=10
#	-SECTION_END-( 2.07 )

#	-SECTION_START-( 2.08 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 2.08 )

########################################################

# Send 512 byte packets.

#	-SECTION_START-( 2.09 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=512                              +
    Number=10
#	-SECTION_END-( 2.09 )

#	-SECTION_START-( 2.10 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 2.10 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#	-SECTION_START-( 2.11 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10
#	-SECTION_END-( 2.11 )

#	-SECTION_START-( 2.12 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 2.12 )

########################################################

# Then stop the receive and dump the statistics.

#	-SECTION_START-( 2.13 )
StopReceive                                     +
    OpenInstance=2
#	-SECTION_END-( 2.13 )

#	-SECTION_START-( 2.14 )
Receive                                         +
    OpenInstance=2
#	-SECTION_END-( 2.14 )

########################################################

# With the correct functional address set all packets
# should be received.

#	-SECTION_START-( 2.15 )
SetFunctionalAddress                            +
    OpenInstance=2                              +
    FunctionalAddress=%TP_FUNCTIONAL_ADDRESS%
#	-SECTION_END-( 2.15 )

########################################################

# Send 40 byte packets.

#	-SECTION_START-( 2.16 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=40                               +
    Number=10
#	-SECTION_END-( 2.16 )

#	-SECTION_START-( 2.17 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 2.17 )

########################################################

# Send 512 byte packets.

#	-SECTION_START-( 2.18 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=512                              +
    Number=10
#	-SECTION_END-( 2.18 )

#	-SECTION_START-( 2.19 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 2.19 )

########################################################

# send "MAXIMUM FRAME SIZE" byte packets.

#	-SECTION_START-( 2.20 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10
#	-SECTION_END-( 2.20 )

#	-SECTION_START-( 2.21 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 2.21 )

########################################################

# Then stop the receive and dump the statistics.

#	-SECTION_START-( 2.22 )
StopReceive                                     +
    OpenInstance=2
#	-SECTION_END-( 2.22 )

#	-SECTION_START-( 2.23 )
Receive                                         +
    OpenInstance=2
#	-SECTION_END-( 2.23 )

########################################################

# With the incorrect functional address set no packets
# should be received.

#	-SECTION_START-( 2.24 )
SetFunctionalAddress                            +
    OpenInstance=2                              +
    FunctionalAddress=%TP_FUNCTIONAL_ADDRESS2%
#	-SECTION_END-( 2.24 )

########################################################

# Send 40 byte packets.

#	-SECTION_START-( 2.25 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=40                               +
    Number=10
#	-SECTION_END-( 2.25 )

#	-SECTION_START-( 2.26 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 2.26 )

########################################################

# Send 512 byte packets.

#	-SECTION_START-( 2.27 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=512                              +
    Number=10
#	-SECTION_END-( 2.27 )

#	-SECTION_START-( 2.28 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 2.28 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#	-SECTION_START-( 2.29 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10
#	-SECTION_END-( 2.29 )

#	-SECTION_START-( 2.30 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 2.30 )

########################################################

# Then stop the receive and dump the statistics.

#	-SECTION_START-( 2.31 )
StopReceive                                     +
    OpenInstance=2
#	-SECTION_END-( 2.31 )

########################################################

# Finally close the adapters, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 2.32 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 2.32 )

#	-SECTION_START-( 2.33 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 2.33 )

#	-SECTION_START-( 2.34 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 2.34 )

#	-SECTION_START-( 2.35 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 2.35 )

#	-SECTION_END-( 2.00 )

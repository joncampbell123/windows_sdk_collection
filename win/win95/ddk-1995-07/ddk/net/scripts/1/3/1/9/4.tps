#	-SECTION_START-( 4.00 ) -SECTION_DESC-( "Send Functional, Receive Directed & Broadcast & Functional Frame" )
##
## TITLE: 1.3.1.9.4 Send Functional, Receive Directed & Broadcast
## TITLE:           & Functional Frame (1M/2C/1O)
##
## 1 Machine - 2 Cards - 1 Open Instance on each card.  These tests
## will have a trusted card send 10 FUNCTIONAL packets to the card
## being tested.  The card being tested will have its packetfilter
## set to receive DIRECTED, BROADCAST and FUNCTIONAL packets.  Packets
## of size 40 bytes, 512 bytes, and MAX_FRAME_SIZE bytes will be sent.
##
## All packets should be received by the test card when the proper
## functional address is set, and No packets should be received when
## no address is set, or the wrong address is set.
##

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#	-SECTION_START-( 4.01 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 4.01 )

#	-SECTION_START-( 4.02 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 4.02 )

# And open and setup the cards.

#	-SECTION_START-( 4.03 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TRUSTED_CARD%
#	-SECTION_END-( 4.03 )

#	-SECTION_START-( 4.04 )
Open                                            +
    OpenInstance=2                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 4.04 )

#	-SECTION_START-( 4.05 )
SetPacketFilter                                 +
    OpenInstance=2                              +
    PacketFilter=DIRECTED|BROADCAST|FUNCTIONAL
#	-SECTION_END-( 4.05 )

########################################################

# Now set the test card to receive packets,

#	-SECTION_START-( 4.06 )
Receive                                         +
    OpenInstance=2
#	-SECTION_END-( 4.06 )

########################################################

# With the no functional address set no packets
# should be received.

########################################################

# Send 40 byte packets.

#	-SECTION_START-( 4.07 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=40                               +
    Number=10
#	-SECTION_END-( 4.07 )

#	-SECTION_START-( 4.08 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 4.08 )

########################################################

# Send 512 byte packets.

#	-SECTION_START-( 4.09 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=512                              +
    Number=10
#	-SECTION_END-( 4.09 )

#	-SECTION_START-( 4.10 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 4.10 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#	-SECTION_START-( 4.11 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10
#	-SECTION_END-( 4.11 )

#	-SECTION_START-( 4.12 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 4.12 )

########################################################

# Then stop the receive and dump the statistics.

#	-SECTION_START-( 4.13 )
StopReceive                                     +
    OpenInstance=2
#	-SECTION_END-( 4.13 )

#	-SECTION_START-( 4.14 )
Receive                                         +
    OpenInstance=2
#	-SECTION_END-( 4.14 )

########################################################

# With the correct functional address set all packets
# should be received.

#	-SECTION_START-( 4.15 )
SetFunctionalAddress                            +
    OpenInstance=2                              +
    FunctionalAddress=%TP_FUNCTIONAL_ADDRESS%
#	-SECTION_END-( 4.15 )

########################################################

# Send 40 byte packets.

#	-SECTION_START-( 4.16 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=40                               +
    Number=10
#	-SECTION_END-( 4.16 )

#	-SECTION_START-( 4.17 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 4.17 )

########################################################

# Send 512 byte packets.

#	-SECTION_START-( 4.18 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=512                              +
    Number=10
#	-SECTION_END-( 4.18 )

#	-SECTION_START-( 4.19 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 4.19 )

########################################################

# send "MAXIMUM FRAME SIZE" byte packets.

#	-SECTION_START-( 4.20 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10
#	-SECTION_END-( 4.20 )

#	-SECTION_START-( 4.21 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 4.21 )

########################################################

# Then stop the receive and dump the statistics.

#	-SECTION_START-( 4.22 )
StopReceive                                     +
    OpenInstance=2
#	-SECTION_END-( 4.22 )

#	-SECTION_START-( 4.23 )
Receive                                         +
    OpenInstance=2
#	-SECTION_END-( 4.23 )

########################################################

# With the incorrect functional address set no packets
# should be received.

#	-SECTION_START-( 4.24 )
SetFunctionalAddress                            +
    OpenInstance=2                              +
    FunctionalAddress=%TP_FUNCTIONAL_ADDRESS2%
#	-SECTION_END-( 4.24 )

########################################################

# Send 40 byte packets.

#	-SECTION_START-( 4.25 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=40                               +
    Number=10
#	-SECTION_END-( 4.25 )

#	-SECTION_START-( 4.26 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 4.26 )

########################################################

# Send 512 byte packets.

#	-SECTION_START-( 4.27 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=512                              +
    Number=10
#	-SECTION_END-( 4.27 )

#	-SECTION_START-( 4.28 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 4.28 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#	-SECTION_START-( 4.29 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10
#	-SECTION_END-( 4.29 )

#	-SECTION_START-( 4.30 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 4.30 )

########################################################

# Then stop the receive and dump the statistics.

#	-SECTION_START-( 4.31 )
StopReceive                                     +
    OpenInstance=2
#	-SECTION_END-( 4.31 )

#	-SECTION_START-( 4.32 )
Receive                                         +
    OpenInstance=2
#	-SECTION_END-( 4.32 )

########################################################

# Finally close the adapters, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 4.33 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 4.33 )

#	-SECTION_START-( 4.34 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 4.34 )

#	-SECTION_START-( 4.35 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 4.35 )

#	-SECTION_START-( 4.36 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 4.36 )

#	-SECTION_END-( 4.00 )

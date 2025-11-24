#   -SECTION_START-( 1.0 ) -SECTION_DESC-( "Send Functional, Receive Directed Broadcast & Functional" )
##
## TITLE: 1.1.2.9.1 Send Functional, Receive Directed & Broadcast
## TITLE:           & Functional (1M/1C/1O)
##
## 1 Machine - 1 Card - 1 Open Instance on the card.  These tests
## will have the test card send 10 FUNCTIONAL packets to itself on the
## same open on the test card.  The test card will have its packetfilter
## set to receive DIRECTED, BROADCAST and FUNCTIONAL packets.  Packets
## of size 40 bytes, 512 bytes, and MAX_FRAME_SIZE bytes will be sent.
##
## All packets should be received by the test card when the proper
## functional address is set, and No packets should be received when
## no address is set, or the wrong address is set.
##

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#   -SECTION_START-( 1.01 )
GetEvents                                       +
    OpenInstance=1
#   -SECTION_END-  ( 1.01 )

# And open and setup the card.

#   -SECTION_START-( 1.02 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%
#   -SECTION_END-  ( 1.02 )

#   -SECTION_START-( 1.03 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|BROADCAST|FUNCTIONAL
#   -SECTION_END-  ( 1.03 )

########################################################

# Now set the test card to receive packets,

#   -SECTION_START-( 1.04 )
Receive                                         +
    OpenInstance=1
#   -SECTION_END-  ( 1.04 )

########################################################

# With the no functional address set no packets
# should be received.

########################################################

# Send 40 byte packets.

#   -SECTION_START-( 1.05 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=40                               +
    Number=10
#   -SECTION_END-  ( 1.05 )

#   -SECTION_START-( 1.06 )
WaitSend                                        +
    OpenInstance=1
#   -SECTION_END-  ( 1.06 )

########################################################

# Send 512 byte packets.

#   -SECTION_START-( 1.07 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=512                              +
    Number=10
#   -SECTION_END-  ( 1.07 )

#   -SECTION_START-( 1.08 )
WaitSend                                        +
    OpenInstance=1
#   -SECTION_END-  ( 1.08 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#   -SECTION_START-( 1.09 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10
#   -SECTION_END-  ( 1.09 )

#   -SECTION_START-( 1.10 )
WaitSend                                        +
    OpenInstance=1
#   -SECTION_END-  ( 1.10 )

########################################################

# Then stop the receive and dump the statistics.

#   -SECTION_START-( 1.11 )
StopReceive                                     +
    OpenInstance=1
#   -SECTION_END-  ( 1.11 )

#   -SECTION_START-( 1.12 )
Receive                                         +
    OpenInstance=1
#   -SECTION_END-  ( 1.12 )

########################################################

# With the correct functional address set all packets
# should be received.

#   -SECTION_START-( 1.13 )
SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=%TP_FUNCTIONAL_ADDRESS%
#   -SECTION_END-  ( 1.13 )

########################################################

# Send 40 byte packets.

#   -SECTION_START-( 1.14 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=40                               +
    Number=10
#   -SECTION_END-  ( 1.14 )

#   -SECTION_START-( 1.15 )
WaitSend                                        +
    OpenInstance=1
#   -SECTION_END-  ( 1.15 )

########################################################

# Send 512 byte packets.

#   -SECTION_START-( 1.16 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=512                              +
    Number=10
#   -SECTION_END-  ( 1.16 )

#   -SECTION_START-( 1.17 )
WaitSend                                        +
    OpenInstance=1
#   -SECTION_END-  ( 1.17 )

########################################################

# send "MAXIMUM FRAME SIZE" byte packets.

#   -SECTION_START-( 1.18 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10
#   -SECTION_END-  ( 1.18 )

#   -SECTION_START-( 1.19 )
WaitSend                                        +
    OpenInstance=1
#   -SECTION_END-  ( 1.19 )

########################################################

# Then stop the receive and dump the statistics.

#   -SECTION_START-( 1.20 )
StopReceive                                     +
    OpenInstance=1
#   -SECTION_END-  ( 1.20 )

#   -SECTION_START-( 1.21 )
Receive                                         +
    OpenInstance=1
#   -SECTION_END-  ( 1.21 )

########################################################

# With the incorrect functional address set no packets
# should be received.

#   -SECTION_START-( 1.22 )
SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=%TP_FUNCTIONAL_ADDRESS2%
#   -SECTION_END-  ( 1.22 )

########################################################

# Send 40 byte packets.

#   -SECTION_START-( 1.23 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=40                               +
    Number=10
#   -SECTION_END-  ( 1.23 )

#   -SECTION_START-( 1.24 )
WaitSend                                        +
    OpenInstance=1
#   -SECTION_END-  ( 1.24 )

########################################################

# Send 512 byte packets.

#   -SECTION_START-( 1.25 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=512                              +
    Number=10
#   -SECTION_END-  ( 1.25 )

#   -SECTION_START-( 1.26 )
WaitSend                                        +
    OpenInstance=1
#   -SECTION_END-  ( 1.26 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#   -SECTION_START-( 1.27 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10
#   -SECTION_END-  ( 1.27 )

#   -SECTION_START-( 1.28 )
WaitSend                                        +
    OpenInstance=1
#   -SECTION_END-  ( 1.28 )

########################################################

# Then stop the receive and dump the statistics.

#   -SECTION_START-( 1.29 )
StopReceive                                     +
    OpenInstance=1
#   -SECTION_END-  ( 1.29 )

########################################################

# Finally close the adapter, and again dump the Event
# Queue for any unexpected events.

#   -SECTION_START-( 1.30 )
Close                                           +
    OpenInstance=1
#   -SECTION_END-  ( 1.30 )

#   -SECTION_START-( 1.31 )
GetEvents                                       +
    OpenInstance=1
#   -SECTION_END-  ( 1.31 )

#   -SECTION_END-  ( 1.0 )

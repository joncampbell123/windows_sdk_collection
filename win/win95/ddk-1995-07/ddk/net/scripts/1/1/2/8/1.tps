#   -SECTION_START-( 1.0 ) -SECTION_DESC-( "Send Multicast, Receive Directed Broadcast & Multicast" )
##
## TITLE: 1.1.2.8.1 Send Multicast, Receive Directed & Broadcast
## TITLE:           & Multicast Frame (1M/1C/1O)
##
## 1 Machine - 1 Card - 1 Open Instance on the card.  These tests
## will have the test card send 10 MULTICAST packets to itself on the
## same open on the test card.  The test card will have its packetfilter
## set to receive DIRECTED, BROADCAST and MULITCAST packets.  Packets
## of size 40 bytes, 512 bytes, and MAX_FRAME_SIZE bytes will be sent.
##
## All packets should be received by the test card when the proper
## multicast address is set, and No packets should be received when
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
    PacketFilter=DIRECTED|BROADCAST|MULTICAST
#   -SECTION_END-  ( 1.03 )

########################################################

# Now set the test card to receive packets,

#   -SECTION_START-( 1.04 )
Receive                                         +
    OpenInstance=1
#   -SECTION_END-  ( 1.04 )

########################################################

# With the correct multicast address set all packets
# should be received.

#   -SECTION_START-( 1.05 )
AddMulticastAddress                             +
    OpenInstance=1                              +
    MulticastAddress=%TP_MULTICAST_ADDRESS%
#   -SECTION_END-  ( 1.05 )

########################################################

# Send 40 byte packets.

#   -SECTION_START-( 1.06 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_MULTICAST_ADDRESS%   +
    PacketSize=40                               +
    Number=10
#   -SECTION_END-  ( 1.06 )

#   -SECTION_START-( 1.07 )
WaitSend                                        +
    OpenInstance=1
#   -SECTION_END-  ( 1.07 )

########################################################

# Send 512 byte packets.

#   -SECTION_START-( 1.08 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_MULTICAST_ADDRESS%   +
    PacketSize=512                              +
    Number=10
#   -SECTION_END-  ( 1.08 )

#   -SECTION_START-( 1.09 )
WaitSend                                        +
    OpenInstance=1
#   -SECTION_END-  ( 1.09 )

########################################################

# send "MAXIMUM FRAME SIZE" byte packets.

#   -SECTION_START-( 1.10 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_MULTICAST_ADDRESS%   +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10
#   -SECTION_END-  ( 1.10 )

#   -SECTION_START-( 1.11 )
WaitSend                                        +
    OpenInstance=1
#   -SECTION_END-  ( 1.11 )

########################################################

# Then stop the receive and dump the statistics.

#   -SECTION_START-( 1.12 )
StopReceive                                     +
    OpenInstance=1
#   -SECTION_END-  ( 1.12 )

#   -SECTION_START-( 1.13 )
Receive                                         +
    OpenInstance=1
#   -SECTION_END-  ( 1.13 )

########################################################

# With the incorrect multicast address set no packets
# should be received.

#   -SECTION_START-( 1.14 )
DeleteMulticastAddress                          +
    OpenInstance=1                              +
    MulticastAddress=%TP_MULTICAST_ADDRESS%
#   -SECTION_END-  ( 1.14 )

#   -SECTION_START-( 1.15 )
AddMulticastAddress                             +
    OpenInstance=1                              +
    MulticastAddress=%TP_MULTICAST_ADDRESS2%
#   -SECTION_END-  ( 1.15 )

########################################################

# Send 40 byte packets.

#   -SECTION_START-( 1.16 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_MULTICAST_ADDRESS%   +
    PacketSize=40                               +
    Number=10
#   -SECTION_END-  ( 1.16 )

#   -SECTION_START-( 1.17 )
WaitSend                                        +
    OpenInstance=1
#   -SECTION_END-  ( 1.17 )

########################################################

# Send 512 byte packets.

#   -SECTION_START-( 1.18 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_MULTICAST_ADDRESS%   +
    PacketSize=512                              +
    Number=10
#   -SECTION_END-  ( 1.18 )

#   -SECTION_START-( 1.19 )
WaitSend                                        +
    OpenInstance=1
#   -SECTION_END-  ( 1.19 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#   -SECTION_START-( 1.20 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_MULTICAST_ADDRESS%   +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10
#   -SECTION_END-  ( 1.20 )

#   -SECTION_START-( 1.21 )
WaitSend                                        +
    OpenInstance=1
#   -SECTION_END-  ( 1.21 )

########################################################

# Then stop the receive and dump the statistics.

#   -SECTION_START-( 1.22 )
StopReceive                                     +
    OpenInstance=1
#   -SECTION_END-  ( 1.22 )

#   -SECTION_START-( 1.23 )
Receive                                         +
    OpenInstance=1
#   -SECTION_END-  ( 1.23 )

########################################################

# With the no multicast address set no packets
# should be received.

#   -SECTION_START-( 1.24 )
DeleteMulticastAddress                          +
    OpenInstance=1                              +
    MulticastAddress=%TP_MULTICAST_ADDRESS2%
#   -SECTION_END-  ( 1.24 )

########################################################

# Send 40 byte packets.

#   -SECTION_START-( 1.25 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_MULTICAST_ADDRESS%   +
    PacketSize=40                               +
    Number=10
#   -SECTION_END-  ( 1.25 )

#   -SECTION_START-( 1.26 )
WaitSend                                        +
    OpenInstance=1
#   -SECTION_END-  ( 1.26 )

########################################################

# Send 512 byte packets.

#   -SECTION_START-( 1.27 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_MULTICAST_ADDRESS%   +
    PacketSize=512                              +
    Number=10
#   -SECTION_END-  ( 1.27 )

#   -SECTION_START-( 1.28 )
WaitSend                                        +
    OpenInstance=1
#   -SECTION_END-  ( 1.28 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#   -SECTION_START-( 1.29 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_MULTICAST_ADDRESS%   +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10
#   -SECTION_END-  ( 1.29 )

#   -SECTION_START-( 1.30 )
WaitSend                                        +
    OpenInstance=1
#   -SECTION_END-  ( 1.30 )

########################################################

# Then stop the receive and dump the statistics.

#   -SECTION_START-( 1.31 )
StopReceive                                     +
    OpenInstance=1
#   -SECTION_END-  ( 1.31 )

########################################################

# Finally close the adapter, and again dump the Event
# Queue for any unexpected events.

#   -SECTION_START-( 1.32 )
Close                                           +
    OpenInstance=1
#   -SECTION_END-  ( 1.32 )

#   -SECTION_START-( 1.33 )
GetEvents                                       +
    OpenInstance=1
#   -SECTION_END-  ( 1.33 )

#   -SECTION_END-  ( 1.0 )

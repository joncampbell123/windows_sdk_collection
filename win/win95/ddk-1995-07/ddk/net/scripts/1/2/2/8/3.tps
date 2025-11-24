#	-SECTION_START-( 3.00 ) -SECTION_DESC-( "Send Multicast, Receive Broadcast & Multicast" )
##
## TITLE: 1.2.2.8.3 Send Multicast, Receive Broadcast & Multicast (1M/1C/2O)
##
## 1 Machine - 1 Card - 2 Open Instances on the card.  These tests
## will have the test card send 10 MULTICAST packets to another open
## on the test card.  The test card will have its packetfilter
## set to receive BROADCAST and MULTICAST packets.  Packets of size
## 40 bytes, 512 bytes, and MAX_FRAME_SIZE bytes will be sent.
##
## All packets should be received by the test card when the proper
## multicast address is set, and No packets should be received when
## no address is set, or the wrong address is set.
##

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#	-SECTION_START-( 3.01 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 3.01 )

#	-SECTION_START-( 3.02 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 3.02 )

# And open and setup the cards.

#	-SECTION_START-( 3.03 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 3.03 )

#	-SECTION_START-( 3.04 )
Open                                            +
    OpenInstance=2                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 3.04 )

#	-SECTION_START-( 3.05 )
SetPacketFilter                                 +
    OpenInstance=2                              +
    PacketFilter=BROADCAST|MULTICAST
#	-SECTION_END-( 3.05 )

########################################################

# Now set the test card to receive packets,

#	-SECTION_START-( 3.06 )
Receive                                         +
    OpenInstance=2
#	-SECTION_END-( 3.06 )

########################################################

# With the correct multicast address set all packets
# should be received.

#	-SECTION_START-( 3.07 )
AddMulticastAddress                             +
    OpenInstance=2                              +
    MulticastAddress=%TP_MULTICAST_ADDRESS%
#	-SECTION_END-( 3.07 )

########################################################

# Send 40 byte packets.

#	-SECTION_START-( 3.08 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_MULTICAST_ADDRESS%   +
    PacketSize=40                               +
    Number=10
#	-SECTION_END-( 3.08 )

#	-SECTION_START-( 3.09 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 3.09 )

########################################################

# Send 512 byte packets.

#	-SECTION_START-( 3.10 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_MULTICAST_ADDRESS%   +
    PacketSize=512                              +
    Number=10
#	-SECTION_END-( 3.10 )

#	-SECTION_START-( 3.11 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 3.11 )

########################################################

# send "MAXIMUM FRAME SIZE" byte packets.

#	-SECTION_START-( 3.12 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_MULTICAST_ADDRESS%   +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10
#	-SECTION_END-( 3.12 )

#	-SECTION_START-( 3.13 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 3.13 )

########################################################

# Then stop the receive and dump the statistics.

#	-SECTION_START-( 3.14 )
StopReceive                                     +
    OpenInstance=2
#	-SECTION_END-( 3.14 )

#	-SECTION_START-( 3.15 )
Receive                                         +
    OpenInstance=2
#	-SECTION_END-( 3.15 )

########################################################

# With the incorrect multicast address set no packets
# should be received.

#	-SECTION_START-( 3.16 )
DeleteMulticastAddress                          +
    OpenInstance=2                              +
    MulticastAddress=%TP_MULTICAST_ADDRESS%
#	-SECTION_END-( 3.16 )

#	-SECTION_START-( 3.17 )
AddMulticastAddress                             +
    OpenInstance=2                              +
    MulticastAddress=%TP_MULTICAST_ADDRESS2%
#	-SECTION_END-( 3.17 )

########################################################

# Send 40 byte packets.

#	-SECTION_START-( 3.18 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_MULTICAST_ADDRESS%   +
    PacketSize=40                               +
    Number=10
#	-SECTION_END-( 3.18 )

#	-SECTION_START-( 3.19 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 3.19 )

########################################################

# Send 512 byte packets.

#	-SECTION_START-( 3.20 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_MULTICAST_ADDRESS%   +
    PacketSize=512                              +
    Number=10
#	-SECTION_END-( 3.20 )

#	-SECTION_START-( 3.21 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 3.21 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#	-SECTION_START-( 3.22 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_MULTICAST_ADDRESS%   +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10
#	-SECTION_END-( 3.22 )

#	-SECTION_START-( 3.23 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 3.23 )

########################################################

# Then stop the receive and dump the statistics.

#	-SECTION_START-( 3.24 )
StopReceive                                     +
    OpenInstance=2
#	-SECTION_END-( 3.24 )

#	-SECTION_START-( 3.25 )
Receive                                         +
    OpenInstance=2
#	-SECTION_END-( 3.25 )

########################################################

# With the no multicast address set no packets
# should be received.

#	-SECTION_START-( 3.26 )
DeleteMulticastAddress                          +
    OpenInstance=2                              +
    MulticastAddress=%TP_MULTICAST_ADDRESS2%
#	-SECTION_END-( 3.26 )

########################################################

# Send 40 byte packets.

#	-SECTION_START-( 3.27 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_MULTICAST_ADDRESS%   +
    PacketSize=40                               +
    Number=10
#	-SECTION_END-( 3.27 )

#	-SECTION_START-( 3.28 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 3.28 )

########################################################

# Send 512 byte packets.

#	-SECTION_START-( 3.29 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_MULTICAST_ADDRESS%   +
    PacketSize=512                              +
    Number=10
#	-SECTION_END-( 3.29 )

#	-SECTION_START-( 3.30 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 3.30 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#	-SECTION_START-( 3.31 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_MULTICAST_ADDRESS%   +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10
#	-SECTION_END-( 3.31 )

#	-SECTION_START-( 3.32 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 3.32 )

########################################################

# Then stop the receive and dump the statistics.

#	-SECTION_START-( 3.33 )
StopReceive                                     +
    OpenInstance=2
#	-SECTION_END-( 3.33 )

########################################################

# Finally close the adapters, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 3.34 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 3.34 )

#	-SECTION_START-( 3.35 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 3.35 )

#	-SECTION_START-( 3.36 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 3.36 )

#	-SECTION_START-( 3.37 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 3.37 )

#	-SECTION_END-( 3.00 )

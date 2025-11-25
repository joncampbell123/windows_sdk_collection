#	-SECTION_START-( 4.00 ) -SECTION_DESC-( "Send Multicast, Receive Multicast" )
##
## TITLE: 1.3.1.4.4 Send Multicast, Receive Multicast (1M/2C/1O)
##
## 1 Machine - 2 Cards - 1 Open Instance on each card.  These tests
## will have a trusted card send 10 MULTICAST packets to the card
## being tested.  The card being tested will have its packetfilter
## set to receive MULICAST packets.  Packets of size 40 bytes,
## 512 bytes, and MAX_FRAME_SIZE bytes will be sent.
##
## All packets should be received by the test card when the proper
## multicast address is set, and No packets should be received when
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
    PacketFilter=MULTICAST
#	-SECTION_END-( 4.05 )

########################################################

# Now set the test card to receive packets,

#	-SECTION_START-( 4.06 )
Receive                                         +
    OpenInstance=2
#	-SECTION_END-( 4.06 )

########################################################

# With the correct multicast address set all packets
# should be received.

#	-SECTION_START-( 4.07 )
AddMulticastAddress                             +
    OpenInstance=2                              +
    MulticastAddress=%TP_MULTICAST_ADDRESS%
#	-SECTION_END-( 4.07 )

########################################################

# Send 40 byte packets.

#	-SECTION_START-( 4.08 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_MULTICAST_ADDRESS%   +
    PacketSize=40                               +
    Number=10
#	-SECTION_END-( 4.08 )

#	-SECTION_START-( 4.09 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 4.09 )

########################################################

# Send 512 byte packets.

#	-SECTION_START-( 4.10 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_MULTICAST_ADDRESS%   +
    PacketSize=512                              +
    Number=10
#	-SECTION_END-( 4.10 )

#	-SECTION_START-( 4.11 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 4.11 )

########################################################

# send "MAXIMUM FRAME SIZE" byte packets.

#	-SECTION_START-( 4.12 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_MULTICAST_ADDRESS%   +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10
#	-SECTION_END-( 4.12 )

#	-SECTION_START-( 4.13 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 4.13 )

########################################################

# Then stop the receive and dump the statistics.

#	-SECTION_START-( 4.14 )
StopReceive                                     +
    OpenInstance=2
#	-SECTION_END-( 4.14 )

#	-SECTION_START-( 4.15 )
Receive                                         +
    OpenInstance=2
#	-SECTION_END-( 4.15 )

########################################################

# With the incorrect multicast address set no packets
# should be received.

#	-SECTION_START-( 4.16 )
DeleteMulticastAddress                          +
    OpenInstance=2                              +
    MulticastAddress=%TP_MULTICAST_ADDRESS%
#	-SECTION_END-( 4.16 )

#	-SECTION_START-( 4.17 )
AddMulticastAddress                             +
    OpenInstance=2                              +
    MulticastAddress=%TP_MULTICAST_ADDRESS2%
#	-SECTION_END-( 4.17 )

########################################################

# Send 40 byte packets.

#	-SECTION_START-( 4.18 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_MULTICAST_ADDRESS%   +
    PacketSize=40                               +
    Number=10
#	-SECTION_END-( 4.18 )

#	-SECTION_START-( 4.19 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 4.19 )

########################################################

# Send 512 byte packets.

#	-SECTION_START-( 4.20 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_MULTICAST_ADDRESS%   +
    PacketSize=512                              +
    Number=10
#	-SECTION_END-( 4.20 )

#	-SECTION_START-( 4.21 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 4.21 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#	-SECTION_START-( 4.22 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_MULTICAST_ADDRESS%   +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10
#	-SECTION_END-( 4.22 )

#	-SECTION_START-( 4.23 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 4.23 )

########################################################

# Then stop the receive and dump the statistics.

#	-SECTION_START-( 4.24 )
StopReceive                                     +
    OpenInstance=2
#	-SECTION_END-( 4.24 )

#	-SECTION_START-( 4.25 )
Receive                                         +
    OpenInstance=2
#	-SECTION_END-( 4.25 )

########################################################

# With the no multicast address set no packets
# should be received.

#	-SECTION_START-( 4.26 )
DeleteMulticastAddress                          +
    OpenInstance=2                              +
    MulticastAddress=%TP_MULTICAST_ADDRESS2%
#	-SECTION_END-( 4.26 )

########################################################

# Send 40 byte packets.

#	-SECTION_START-( 4.27 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_MULTICAST_ADDRESS%   +
    PacketSize=40                               +
    Number=10
#	-SECTION_END-( 4.27 )

#	-SECTION_START-( 4.28 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 4.28 )

########################################################

# Send 512 byte packets.

#	-SECTION_START-( 4.29 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_MULTICAST_ADDRESS%   +
    PacketSize=512                              +
    Number=10
#	-SECTION_END-( 4.29 )

#	-SECTION_START-( 4.30 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 4.30 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#	-SECTION_START-( 4.31 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_MULTICAST_ADDRESS%   +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10
#	-SECTION_END-( 4.31 )

#	-SECTION_START-( 4.32 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 4.32 )

########################################################

# Then stop the receive and dump the statistics.

#	-SECTION_START-( 4.33 )
StopReceive                                     +
    OpenInstance=2
#	-SECTION_END-( 4.33 )

########################################################

# Finally close the adapters, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 4.34 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 4.34 )

#	-SECTION_START-( 4.35 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 4.35 )

#	-SECTION_START-( 4.36 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 4.36 )

#	-SECTION_START-( 4.37 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 4.37 )

#	-SECTION_END-( 4.00 )

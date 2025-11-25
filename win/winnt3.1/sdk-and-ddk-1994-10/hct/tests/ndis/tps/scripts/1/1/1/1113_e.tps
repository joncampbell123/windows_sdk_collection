#       -SECTION_START-( 1.0 ) -SECTION_DESC-( "Set Packet Filter Tests" )
##
## TITLE: 1.1.1.1113_E Set Packet Filter Tests (1M/1C/10)
##
## 1 Machine - 1 Card - 1 Open Instance on the Card.  These tests
## will verify the ability to set and query the all packetfilter
## on an adapter.
##

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#   -SECTION_START-( 1.01 )
GetEvents                                       +
    OpenInstance=1
#   -SECTION_END-  ( 1.01 )

# and open the adapter

#   -SECTION_START-( 1.02 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%
#   -SECTION_END-  ( 1.02 )

########################################################

# set the packet filter to be DIRECTED

#   -SECTION_START-( 1.03 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#   -SECTION_END-  ( 1.03 )

########################################################

# set the packet filter to be MULTICAST

#   -SECTION_START-( 1.04 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=MULTICAST

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#   -SECTION_END-  ( 1.04 )

########################################################

# set the packet filter to be BROADCAST

#   -SECTION_START-( 1.05 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=BROADCAST

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#   -SECTION_END-  ( 1.05 )

########################################################

# set the packet filter to be ALLMULTICAST

#   -SECTION_START-( 1.06 ) -OPTIONALS-( ALLMULTICAST )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=ALLMULTICAST

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#   -SECTION_END-  ( 1.06 )

########################################################

# set the packet filter to be PROMISCUOUS

#   -SECTION_START-( 1.07 ) -OPTIONALS-( PROMISCUOUS )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=PROMISCUOUS

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#   -SECTION_END-  ( 1.07 )

########################################################

# set the packet filter to be DIRECTED|MULTICAST

#   -SECTION_START-( 1.08 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|MULTICAST

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#   -SECTION_END-( 1.08 )

########################################################

# set the packet filter to be DIRECTED|BROADCAST

#   -SECTION_START-( 1.09 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|BROADCAST

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#   -SECTION_END-  ( 1.09 )

########################################################

# set the packet filter to be DIRECTED|ALLMULTICAST

#   -SECTION_START-( 1.10 ) -OPTIONALS-( ALLMULTICAST )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|ALLMULTICAST

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#   -SECTION_END-  ( 1.10 )

########################################################

# set the packet filter to be DIRECTED|PROMISCUOUS

#   -SECTION_START-( 1.11 ) -OPTIONALS-( PROMISCUOUS )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|PROMISCUOUS

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#   -SECTION_END-  ( 1.11 )

########################################################

# set the packet filter to be MULTICAST|BROADCAST

#   -SECTION_START-( 1.12 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=MULTICAST|BROADCAST

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#   -SECTION_END-( 1.12 )

########################################################

# set the packet filter to be MULTICAST|ALLMULTICAST

#   -SECTION_START-( 1.13 ) -OPTIONALS-( ALLMULTICAST )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=MULTICAST|ALLMULTICAST

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#   -SECTION_END-  ( 1.13 )

########################################################

# set the packet filter to be MULTICAST|PROMISCUOUS

#   -SECTION_START-( 1.14 ) -OPTIONALS-( PROMISCUOUS )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=MULTICAST|PROMISCUOUS

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#   -SECTION_END-  ( 1.14 )

########################################################

# set the packet filter to be BROADCAST|PROMISCUOUS

#   -SECTION_START-( 1.15 ) -OPTIONALS-( PROMISCUOUS )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=BROADCAST|PROMISCUOUS

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#   -SECTION_END-  ( 1.15 )

########################################################

# set the packet filter to be DIRECTED|BROADCAST|PROMISCUOUS

#   -SECTION_START-( 1.16 ) -OPTIONALS-( PROMISCUOUS )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|BROADCAST|PROMISCUOUS

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#   -SECTION_END-  ( 1.16 )

########################################################

# set the packet filter to be BROADCAST|ALLMULTICAST

#   -SECTION_START-( 1.17 ) -OPTIONALS-( ALLMULTICAST )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=BROADCAST|ALLMULTICAST

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#   -SECTION_END-  ( 1.17 )

########################################################

# set the packet filter to be BROADCAST|PROMISCUOUS

#   -SECTION_START-( 1.18 ) -OPTIONALS-( PROMISCUOUS )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=BROADCAST|PROMISCUOUS

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#   -SECTION_END-  ( 1.18 )

########################################################

# set the packet filter to be ALLMULTICAST|PROMISCUOUS

#   -SECTION_START-( 1.19 ) -OPTIONALS-( PROMISCUOUS,ALLMULTICAST )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=ALLMULTICAST|PROMISCUOUS

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#   -SECTION_END-  ( 1.19 )

########################################################

# set the packet filter to be DIRECTED|MULTICAST|BROADCAST

#   -SECTION_START-( 1.20 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|BROADCAST|MULTICAST

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#   -SECTION_END-  ( 1.20 )

########################################################

# set the packet filter to be DIRECTED|MULTICAST|ALLMULTICAST

#   -SECTION_START-( 1.21 ) -OPTIONALS-( ALLMULTICAST )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|MULTICAST|ALLMULTICAST

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#   -SECTION_END-  ( 1.21 )

########################################################

# set the packet filter to be DIRECTED|MULTICAST|PROMISCUOUS

#   -SECTION_START-( 1.22 ) -OPTIONALS-( PROMISCUOUS )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|MULTICAST|PROMISCUOUS

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#   -SECTION_END-  ( 1.22 )

########################################################

# set the packet filter to be DIRECTED|BROADCAST|ALLMULTICAST

#   -SECTION_START-( 1.23 ) -OPTIONALS-( ALLMULTICAST )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|BROADCAST|ALLMULTICAST

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#   -SECTION_END-  ( 1.23 )

########################################################

# set the packet filter to be DIRECTED|BROADCAST|PROMISCUOUS

#   -SECTION_START-( 1.24 ) -OPTIONALS-( PROMISCUOUS )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|BROADCAST|PROMISCUOUS

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#   -SECTION_END-  ( 1.24 )

########################################################

# set the packet filter to be DIRECTED|ALLMULTICAST|PROMISCUOUS

#   -SECTION_START-( 1.25 ) -OPTIONALS-( PROMISCUOUS,ALLMULTICAST )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|ALLMULTICAST|PROMISCUOUS

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#   -SECTION_END-  ( 1.25 )

########################################################

# set the packet filter to be MULTICAST|BROADCAST|ALLMULTICAST

#   -SECTION_START-( 1.26 ) -OPTIONALS-( ALLMULTICAST )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=MULTICAST|BROADCAST|ALLMULTICAST

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#   -SECTION_END-  ( 1.26 )

########################################################

# set the packet filter to be MULTICAST|PROMISCUOUS|BROADCAST

#   -SECTION_START-( 1.27 ) -OPTIONALS-( PROMISCUOUS )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=BROADCAST|PROMISCUOUS|MULTICAST

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#   -SECTION_END-  ( 1.27 )

########################################################

# set the packet filter to be MULTICAST|ALLMULTICAST|PROMISCUOUS

#   -SECTION_START-( 1.28 ) -OPTIONALS-( PROMISCUOUS,ALLMULTICAST )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=MULTICAST|ALLMULTICAST|PROMISCUOUS

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#   -SECTION_END-  ( 1.28 )

########################################################

# set the packet filter to be ALLMULTICAST|BROADCAST|PROMISCUOUS

#   -SECTION_START-( 1.29 ) -OPTIONALS-( PROMISCUOUS,ALLMULTICAST )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=ALLMULTICAST|BROADCAST|PROMISCUOUS

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#   -SECTION_END-  ( 1.29 )

########################################################

# set the packet filter to be DIRECTED|MULTICAST|BROADCAST|ALLMULTICAST

#   -SECTION_START-( 1.30 ) -OPTIONALS-( ALLMULTICAST )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|MULTICAST|BROADCAST|ALLMULTICAST

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#   -SECTION_END-  ( 1.30 )

########################################################

# set the packet filter to be DIRECTED|MULTICAST|BROADCAST|PROMISCUOUS

#   -SECTION_START-( 1.31 ) -OPTIONALS-( PROMISCUOUS )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|MULTICAST|BROADCAST|PROMISCUOUS

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#   -SECTION_END-  ( 1.31 )

########################################################

# set the packet filter to be DIRECTED|MULTICAST|ALLMULTICAST|PROMISCUOUS

#   -SECTION_START-( 1.32 ) -OPTIONALS-( ALLMULTICAST,PROMISCUOUS )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|MULTICAST|ALLMULTICAST|PROMISCUOUS

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#   -SECTION_END-  ( 1.32 )

########################################################

# set the packet filter to be DIRECTED|ALLMULTICAST|BROADCAST|PROMISCUOUS

#   -SECTION_START-( 1.33 ) -OPTIONALS-( PROMISCUOUS,ALLMULTICAST )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|ALLMULTICAST|BROADCAST|PROMISCUOUS

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#   -SECTION_END-  ( 1.33 )

########################################################

# set the packet filter to be MULTICAST|ALLMULTICAST|BROADCAST|PROMISCUOUS

#   -SECTION_START-( 1.34 ) -OPTIONALS-( PROMISCUOUS,ALLMULTICAST )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=MULTICAST|ALLMULTICAST|BROADCAST|PROMISCUOUS

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#   -SECTION_END-  ( 1.34 )

########################################################

# set the packet filter to be DIRECTED|BROADCAST|PROMISCUOUS|MULTICAST|ALLMULTICAST

#   -SECTION_START-( 1.35 ) -OPTIONALS-( PROMISCUOUS,ALLMULTICAST )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|BROADCAST|PROMISCUOUS|MULTICAST|ALLMULTICAST

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#   -SECTION_END-  ( 1.35 )

########################################################

# finally close the adapter and dump the Event Queue for
# any unexpected events.

#   -SECTION_START-( 1.36 )
Close                                           +
    OpenInstance=1
#   -SECTION_END-  ( 1.36 )

# finally dump the Event Queue for any unexpected events.

#   -SECTION_START-( 1.37 )
GetEvents                                       +
    OpenInstance=1
#   -SECTION_END-  ( 1.37 )

#   -SECTION_END-  ( 1.0 )

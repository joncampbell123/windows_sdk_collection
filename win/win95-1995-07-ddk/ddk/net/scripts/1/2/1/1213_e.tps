#	-SECTION_START-( 1.00 ) -SECTION_DESC-( "Set Packet Filter Tests" )
##
## TITLE: 1.2.1.1213_E Set Packet Filter Tests (1M/1C/20)
##
## 1 Machine - 1 Card - 2 Open Instances on the Card.  These tests
## will verify the ability to set and query the packetfilter
## from multiple opens on the adapter.
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

# and open the adapter

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

########################################################

# set the packet filter to be DIRECTED

#	-SECTION_START-( 1.05 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED

SetPacketFilter                                 +
    OpenInstance=2                              +
    PacketFilter=DIRECTED

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#	-SECTION_END-( 1.05 )

########################################################

# set the packet filter to be BROADCAST

#	-SECTION_START-( 1.06 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=BROADCAST

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#	-SECTION_END-( 1.06 )

########################################################

# set the packet filter to be PROMISCUOUS

#	-SECTION_START-( 1.07 ) -OPTIONALS-( PROMISCUOUS )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=PROMISCUOUS

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#	-SECTION_END-( 1.07 )

########################################################

# set the packet filter to be DIRECTED|BROADCAST

#	-SECTION_START-( 1.08 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|BROADCAST

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#	-SECTION_END-( 1.08 )

########################################################

# set the packet filter to be DIRECTED|PROMISCUOUS

#	-SECTION_START-( 1.09 ) -OPTIONALS-( PROMISCUOUS )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|PROMISCUOUS

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#	-SECTION_END-( 1.09 )

########################################################

# set the packet filter to be BROADCAST|PROMISCUOUS

#	-SECTION_START-( 1.10 ) -OPTIONALS-( PROMISCUOUS )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=BROADCAST|PROMISCUOUS

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#	-SECTION_END-( 1.10 )

########################################################

# set the packet filter to be DIRECTED|BROADCAST|PROMISCUOUS

#	-SECTION_START-( 1.11 ) -OPTIONALS-( PROMISCUOUS )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|BROADCAST|PROMISCUOUS

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#	-SECTION_END-( 1.11 )

########################################################

# set the packet filter to be MULTICAST

#	-SECTION_START-( 1.12 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=MULTICAST

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#	-SECTION_END-( 1.12 )

########################################################


# set the packet filter to be DIRECTED|MULTICAST

#	-SECTION_START-( 1.13 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|MULTICAST

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#	-SECTION_END-( 1.13 )

########################################################

# set the packet filter to be DIRECTED|ALLMULTICAST

#	-SECTION_START-( 1.14 ) -OPTIONALS-( ALLMULTICAST )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|ALLMULTICAST

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#	-SECTION_END-( 1.14 )

########################################################

# set the packet filter to be BROADCAST|MULTICAST

#	-SECTION_START-( 1.15 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=BROADCAST|MULTICAST

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#	-SECTION_END-( 1.15 )

########################################################

# set the packet filter to be BROADCAST|ALLMULTICAST

#	-SECTION_START-( 1.16 ) -OPTIONALS-( ALLMULTICAST )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=BROADCAST|ALLMULTICAST

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#	-SECTION_END-( 1.16 )

########################################################

# set the packet filter to be PROMISCUOUS|MULTICAST

#	-SECTION_START-( 1.17 ) -OPTIONALS-( PROMISCUOUS )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=PROMISCUOUS|MULTICAST

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#	-SECTION_END-( 1.17 )

########################################################

# set the packet filter to be PROMISCUOUS|ALLMULTICAST

#	-SECTION_START-( 1.18 ) -OPTIONALS-( PROMISCUOUS,ALLMULTICAST )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=PROMISCUOUS|ALLMULTICAST

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#	-SECTION_END-( 1.18 )

########################################################

# set the packet filter to be MULTICAST|ALLMULTICAST

#	-SECTION_START-( 1.19 ) -OPTIONALS-( ALLMULTICAST )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=MULTICAST|ALLMULTICAST

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#	-SECTION_END-( 1.19 )

########################################################

# set the packet filter to be DIRECTED|BROADCAST|MULTICAST

#	-SECTION_START-( 1.20 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|BROADCAST|MULTICAST

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#	-SECTION_END-( 1.20 )

########################################################

# set the packet filter to be DIRECTED|BROADCAST|ALLMULTICAST

#	-SECTION_START-( 1.21 ) -OPTIONALS-( ALLMULTICAST )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|BROADCAST|ALLMULTICAST

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#	-SECTION_END-( 1.21 )

########################################################

# set the packet filter to be DIRECTED|PROMISCUOUS|MULTICAST

#	-SECTION_START-( 1.22 ) -OPTIONALS-( PROMISCUOUS )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|PROMISCUOUS|MULTICAST

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#	-SECTION_END-( 1.22 )

########################################################

# set the packet filter to be DIRECTED|PROMISCUOUS|ALLMULTICAST

#	-SECTION_START-( 1.23 ) -OPTIONALS-( PROMISCUOUS,ALLMULTICAST )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|PROMISCUOUS|ALLMULTICAST

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#	-SECTION_END-( 1.23 )

########################################################

# set the packet filter to be BROADCAST|PROMISCUOUS|MULTICAST

#	-SECTION_START-( 1.24 ) -OPTIONALS-( PROMISCUOUS )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=BROADCAST|PROMISCUOUS|MULTICAST

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#	-SECTION_END-( 1.24 )

########################################################

# set the packet filter to be BROADCAST|PROMISCUOUS|ALLMULTICAST

#	-SECTION_START-( 1.25 ) -OPTIONALS-( PROMISCUOUS,ALLMULTICAST )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=BROADCAST|PROMISCUOUS|ALLMULTICAST

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#	-SECTION_END-( 1.25 )

########################################################

# set the packet filter to be DIRECTED|BROADCAST|PROMISCUOUS|MULTICAST

#	-SECTION_START-( 1.26 ) -OPTIONALS-( PROMISCUOUS )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|BROADCAST|PROMISCUOUS|MULTICAST

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#	-SECTION_END-( 1.26 )

########################################################

# set the packet filter to be DIRECTED|BROADCAST|PROMISCUOUS|ALLMULTICAST

#	-SECTION_START-( 1.27 ) -OPTIONALS-( PROMISCUOUS,ALLMULTICAST )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|BROADCAST|PROMISCUOUS|ALLMULTICAST

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#	-SECTION_END-( 1.27 )

########################################################

# set the packet filter to be DIRECTED|BROADCAST|PROMISCUOUS|MULTICAST|ALLMULTICAST

#	-SECTION_START-( 1.28 ) -OPTIONALS-( PROMISCUOUS,ALLMULTICAST )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|BROADCAST|PROMISCUOUS|MULTICAST|ALLMULTICAST

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#	-SECTION_END-( 1.28 )

########################################################

# set the packet filter to be ALLMULTICAST

#	-SECTION_START-( 1.29 ) -OPTIONALS-( ALLMULTICAST )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=ALLMULTICAST

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#	-SECTION_END-( 1.29 )

########################################################

# set the packet filter to be DIRECTED|PROMISCUOUS

#	-SECTION_START-( 1.30 ) -OPTIONALS-( PROMISCUOUS )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|PROMISCUOUS

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#	-SECTION_END-( 1.30 )

########################################################

# finally close the adapter and dump the Event Queue for
# any unexpected events.

#	-SECTION_START-( 1.31 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 1.31)

#	-SECTION_START-( 1.32 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 1.32 )

# finally dump the Event Queue for any unexpected events.

#	-SECTION_START-( 1.33 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 1.33 )

#	-SECTION_START-( 1.34 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 1.34 )

#	-SECTION_END-( 1.00 )

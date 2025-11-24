#	-SECTION_START-( 1.00 ) -SECTION_DESC-( "Set Packet Filter Tests" )
##
## TITLE: 1.2.1.1213_TR Set Packet Filter Tests (1M/1C/20)
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

# set the packet filter to be FUNCTIONAL

#	-SECTION_START-( 1.08 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=FUNCTIONAL

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

# set the packet filter to be ALLFUNCTIONAL

#	-SECTION_START-( 1.09 ) -OPTIONALS-( ALLFUNCTIONAL )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=ALLFUNCTIONAL

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

# set the packet filter to be GROUP

#	-SECTION_START-( 1.10 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=GROUP

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

# set the packet filter to be SOURCEROUTING

#	-SECTION_START-( 1.11 ) -OPTIONALS-( SOURCEROUTING )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=SOURCEROUTING

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

# set the packet filter to be MACFRAME

#	-SECTION_START-( 1.12 ) -OPTIONALS-( MACFRAME )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=MACFRAME

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

# set the packet filter to be DIRECTED|BROADCAST

#	-SECTION_START-( 1.13 )
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
#	-SECTION_END-( 1.13 )

########################################################

# set the packet filter to be DIRECTED|FUNCTIONAL

#	-SECTION_START-( 1.14 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|FUNCTIONAL

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

# set the packet filter to be DIRECTED|GROUP

#	-SECTION_START-( 1.15 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|GROUP

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

# set the packet filter to be DIRECTED|ALLFUNCTIONAL

#	-SECTION_START-( 1.16 ) -OPTIONALS-( ALLFUNCTIONAL )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|ALLFUNCTIONAL

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

# set the packet filter to be DIRECTED|SOURCEROUTING

#	-SECTION_START-( 1.17 ) -OPTIONALS-( SOURCEROUTING )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|SOURCEROUTING

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

# set the packet filter to be DIRECTED|MACFRAME

#	-SECTION_START-( 1.18 ) -OPTIONALS-( MACFRAME )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|MACFRAME

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

# set the packet filter to be BROADCAST|FUNCTIONAL

#	-SECTION_START-( 1.19 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=BROADCAST|FUNCTIONAL

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

# set the packet filter to be BROADCAST|GROUP

#	-SECTION_START-( 1.20 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=BROADCAST|GROUP

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

# set the packet filter to be BROADCAST|ALLFUNCTIONAL

#	-SECTION_START-( 1.21 ) -OPTIONALS-( ALLFUNCTIONAL )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=BROADCAST|ALLFUNCTIONAL

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

# set the packet filter to be BROADCAST|SOURCEROUTING

#	-SECTION_START-( 1.22 ) -OPTIONALS-( SOURCEROUTING )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=BROADCAST|SOURCEROUTING

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

# set the packet filter to be BROADCAST|MACFRAME

#	-SECTION_START-( 1.23 ) -OPTIONALS-( MACFRAME )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=BROADCAST|MACFRAME

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

# set the packet filter to be PROMISCUOUS|FUNCTIONAL

#	-SECTION_START-( 1.24 ) -OPTIONALS-( PROMISCUOUS )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=PROMISCUOUS|FUNCTIONAL

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

readscript \tps\scripts\1\2\1\12135_tr.tps
########################################################

# finally close the adapter and dump the Event Queue for
# any unexpected events.

#	-SECTION_START-( 1.38 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 1.38 )

#	-SECTION_START-( 1.39 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 1.39 )

# finally dump the Event Queue for any unexpected events.

#	-SECTION_START-( 1.40 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 1.40 )

#	-SECTION_START-( 1.41 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 1.41 )

#	-SECTION_END-( 1.00 )

########################################################

# set the packet filter to be PROMISCUOUS|ALLFUNCTIONAL

#	-SECTION_START-( 1.25 ) -OPTIONALS-( PROMISCUOUS,ALLFUNCTIONAL )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=PROMISCUOUS|ALLFUNCTIONAL

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

# set the packet filter to be PROMISCUOUS|GROUP

#	-SECTION_START-( 1.26 ) -OPTIONALS-( PROMISCUOUS )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=PROMISCUOUS|GROUP

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

# set the packet filter to be PROMISCUOUS|SOURCEROUTING

#	-SECTION_START-( 1.27 ) -OPTIONALS-( PROMISCUOUS, SOURCEROUTING )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=PROMISCUOUS|SOURCEROUTING

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

# set the packet filter to be PROMISCUOUS|MACFRAME

#	-SECTION_START-( 1.28 ) -OPTIONALS-( PROMISCUOUS,MACFRAME )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=PROMISCUOUS|MACFRAME

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

# set the packet filter to be FUNCTIONAL|ALLFUNCTIONAL

#	-SECTION_START-( 1.29 ) -OPTIONALS-( ALLFUNCTIONAL )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=FUNCTIONAL|ALLFUNCTIONAL

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

# set the packet filter to be FUNCTIONAL|GROUP

#	-SECTION_START-( 1.30 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=FUNCTIONAL|GROUP

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

# set the packet filter to be DIRECTED|BROADCAST|FUNCTIONAL

#	-SECTION_START-( 1.31 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|BROADCAST|FUNCTIONAL

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
#	-SECTION_END-( 1.31 )

########################################################

# set the packet filter to be DIRECTED|BROADCAST|GROUP

#	-SECTION_START-( 1.32 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|BROADCAST|GROUP

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
#	-SECTION_END-( 1.32 )

########################################################

# set the packet filter to be
# DIRECTED|BROADCAST|ALLFUNCTIONAL|SOURCEROUTING|MACFRAME

#	-SECTION_START-( 1.33 ) -OPTIONALS-( SOURCEROUTING,MACFRAME,ALLFUNCTIONAL )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|BROADCAST|ALLFUNCTIONAL|SOURCEROUTING|MACFRAME

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
#	-SECTION_END-( 1.33 )

########################################################

# set the packet filter to be DIRECTED|FUNCTIONAL|ALLFUNCTIONAL

#	-SECTION_START-( 1.34 ) -OPTIONALS-( ALLFUNCTIONAL )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|FUNCTIONAL|ALLFUNCTIONAL

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
#	-SECTION_END-( 1.34 )

########################################################

# set the packet filter to be DIRECTED|FUNCTIONAL|GROUP

#	-SECTION_START-( 1.35 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|FUNCTIONAL|GROUP

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
#	-SECTION_END-( 1.35 )

########################################################

# set the packet filter to be DIRECTED|FUNCTIONAL|SOURCEROUTING|MACFRAME

#	-SECTION_START-( 1.36 ) -OPTIONALS-( SOURCEROUTING,MACFRAME )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|FUNCTIONAL|SOURCEROUTING|MACFRAME

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
#	-SECTION_END-( 1.36 )

########################################################

# set the packet filter to be DIRECTED|BROADCAST|FUNCTIONAL|GROUP|
#                             SOURCEROUTING|MACFRAME

#	-SECTION_START-( 1.37 ) -OPTIONALS-( SOURCEROUTING,MACFRAME )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|BROADCAST|FUNCTIONAL|GROUP|SOURCEROUTING|MACFRAME

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
#	-SECTION_END-( 1.37 )



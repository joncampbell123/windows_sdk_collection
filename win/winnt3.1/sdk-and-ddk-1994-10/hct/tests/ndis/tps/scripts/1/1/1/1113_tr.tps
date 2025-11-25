#       -SECTION_START-( 1.0 ) -SECTION_DESC-( "Set Packet Filter Tests" )
##
## TITLE: 1.1.1.1113_TR Set Packet Filter Tests (1M/1C/10)
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

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#   -SECTION_END-  ( 1.03 )

########################################################

# set the packet filter to be BROADCAST

#   -SECTION_START-( 1.04 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=BROADCAST

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

# set the packet filter to be PROMISCUOUS

#   -SECTION_START-( 1.05 ) -OPTIONALS-( PROMISCUOUS )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=PROMISCUOUS

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

# set the packet filter to be FUNCTIONAL

#   -SECTION_START-( 1.06 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=FUNCTIONAL

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

# set the packet filter to be ALLFUNCTIONAL

#   -SECTION_START-( 1.07 ) -OPTIONALS-( ALLFUNCTIONAL )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=ALLFUNCTIONAL

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

# set the packet filter to be GROUP

#   -SECTION_START-( 1.08 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=GROUP

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#   -SECTION_END-  ( 1.08 )

########################################################

# set the packet filter to be SOURCEROUTING

#   -SECTION_START-( 1.09 ) -OPTIONALS-( SOURCEROUTING )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=SOURCEROUTING

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

# set the packet filter to be MACFRAME

#   -SECTION_START-( 1.10 )  -OPTIONALS-( MACFRAME )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=MACFRAME

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

# set the packet filter to be DIRECTED|BROADCAST

#   -SECTION_START-( 1.11 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|BROADCAST

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#   -SECTION_END-( 1.11 )

########################################################

# set the packet filter to be DIRECTED|FUNCTIONAL

#   -SECTION_START-( 1.12 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|FUNCTIONAL

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

# set the packet filter to be DIRECTED|GROUP

#   -SECTION_START-( 1.13 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|GROUP

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

# set the packet filter to be DIRECTED|ALLFUNCTIONAL

#   -SECTION_START-( 1.14 ) -OPTIONALS-( ALLFUNCTIONAL )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|ALLFUNCTIONAL

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#   -SECTION_END-( 1.14 )

########################################################

# set the packet filter to be DIRECTED|SOURCEROUTING

#   -SECTION_START-( 1.15 ) -OPTIONALS-( SOURCEROUTING )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|SOURCEROUTING

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER

SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=NONE

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER
#   -SECTION_END-( 1.15 )

########################################################

# set the packet filter to be DIRECTED|MACFRAME

#   -SECTION_START-( 1.16 ) -OPTIONALS-( MACFRAME )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|MACFRAME

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

# set the packet filter to be BROADCAST|FUNCTIONAL

#   -SECTION_START-( 1.17 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=BROADCAST|FUNCTIONAL

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

# set the packet filter to be BROADCAST|GROUP

#   -SECTION_START-( 1.18 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=BROADCAST|GROUP

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

# set the packet filter to be BROADCAST|ALLFUNCTIONAL

#   -SECTION_START-( 1.19 ) -OPTIONALS-( ALLFUNCTIONAL )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=BROADCAST|ALLFUNCTIONAL

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

# set the packet filter to be BROADCAST|SOURCEROUTING

#   -SECTION_START-( 1.20 ) -OPTIONALS-( SOURCEROUTING )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=BROADCAST|SOURCEROUTING

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

# set the packet filter to be BROADCAST|MACFRAME

#   -SECTION_START-( 1.21 ) -OPTIONALS-( MACFRAME )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=BROADCAST|MACFRAME

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

# set the packet filter to be PROMISCUOUS|FUNCTIONAL

#   -SECTION_START-( 1.22 ) -OPTIONALS-( PROMISCUOUS )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=PROMISCUOUS|FUNCTIONAL

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

# set the packet filter to be PROMISCUOUS|ALLFUNCTIONAL

#   -SECTION_START-( 1.23 ) -OPTIONALS-( PROMISCUOUS,ALLFUNCTIONAL )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=PROMISCUOUS|ALLFUNCTIONAL

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

# set the packet filter to be PROMISCUOUS|GROUP

#   -SECTION_START-( 1.24 ) -OPTIONALS-( PROMISCUOUS )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=PROMISCUOUS|GROUP

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

# set the packet filter to be PROMISCUOUS|SOURCEROUTING

#   -SECTION_START-( 1.25 ) -OPTIONALS-( PROMISCUOUS,SOURCEROUTING )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=PROMISCUOUS|SOURCEROUTING

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

# set the packet filter to be PROMISCUOUS|MACFRAME

#   -SECTION_START-( 1.26 ) -OPTIONALS-( PROMISCUOUS,MACFRAME )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=PROMISCUOUS|MACFRAME

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

# set the packet filter to be FUNCTIONAL|ALLFUNCTIONAL

#   -SECTION_START-( 1.27 ) -OPTIONALS-( ALLFUNCTIONAL )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=FUNCTIONAL|ALLFUNCTIONAL

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

# set the packet filter to be FUNCTIONAL|GROUP

#   -SECTION_START-( 1.28 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=FUNCTIONAL|GROUP

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

# set the packet filter to be DIRECTED|BROADCAST|FUNCTIONAL

#   -SECTION_START-( 1.29 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|BROADCAST|FUNCTIONAL

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

# set the packet filter to be DIRECTED|BROADCAST|GROUP

#   -SECTION_START-( 1.30 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|BROADCAST|GROUP

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

# set the packet filter to be
# DIRECTED|BROADCAST|ALLFUNCTIONAL|SOURCEROUTING|MACFRAME

#   -SECTION_START-( 1.31 ) -OPTIONALS-( ALLFUNCTIONAL,SOURCEROUTING,MACFRAME )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|BROADCAST|ALLFUNCTIONAL|SOURCEROUTING|MACFRAME

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

# set the packet filter to be DIRECTED|FUNCTIONAL|ALLFUNCTIONAL

#   -SECTION_START-( 1.32 ) -OPTIONALS-( ALLFUNCTIONAL )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|FUNCTIONAL|ALLFUNCTIONAL

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

# set the packet filter to be DIRECTED|FUNCTIONAL|GROUP

#   -SECTION_START-( 1.33 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|FUNCTIONAL|GROUP

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

# set the packet filter to be DIRECTED|FUNCTIONAL|SOURCEROUTING|MACFRAME

#   -SECTION_START-( 1.34 ) -OPTIONALS-( SOURCEROUTING,MACFRAME )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|FUNCTIONAL|SOURCEROUTING|MACFRAME

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

# set the packet filter to be DIRECTED|BROADCAST|FUNCTIONAL|GROUP|
#                             SOURCEROUTING|MACFRAME

#   -SECTION_START-( 1.35 ) -OPTIONALS-( SOURCEROUTING,MACFRAME )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|BROADCAST|FUNCTIONAL|GROUP|SOURCEROUTING|MACFRAME

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

#       -SECTION_START-( 1.0 ) -SECTION_DESC-( "Query Information Tests" )
##
## TITLE: 1.1.1.1112_E Query Information Tests (1M/1C/10)
##
## 1 Machine - 1 Card - 1 Open Instance on the Card.  These tests
## will verify the ability to query the all OIDs from an adapter.

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#       -SECTION_START-( 1.01 )
GetEvents                                       +
    OpenInstance=1
#       -SECTION_END-  ( 1.01 )

# and open the adapter

#       -SECTION_START-( 1.02 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%
#       -SECTION_END-  ( 1.02 )

########################################################

# General Operational Characteristics

#       -SECTION_START-( 1.03 ) -TOKEN_MATCH-( 4,0 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_SUPPORTED_LIST                   # 0x00010101
#       -SECTION_END-  ( 1.03 )

#       -SECTION_START-( 1.04 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_HARDWARE_STATUS                      # 0x00010102
#       -SECTION_END-  ( 1.04 )

#       -SECTION_START-( 1.05 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_MEDIA_SUPPORTED                 # 0x00010103
#       -SECTION_END-  ( 1.05 )

#       -SECTION_START-( 1.06 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_MEDIA_IN_USE                    # 0x00010104
#       -SECTION_END-  ( 1.06 )

#       -SECTION_START-( 1.07 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_MAXIMUM_LOOKAHEAD                    # 0x00010105
#       -SECTION_END-  ( 1.07 )

#       -SECTION_START-( 1.08 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_MAXIMUM_FRAME_SIZE                   # 0x00010106
#       -SECTION_END-  ( 1.08 )

#       -SECTION_START-( 1.09 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_LINK_SPEED                           # 0x00010107
#       -SECTION_END-  ( 1.09 )

#       -SECTION_START-( 1.10 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_TRANSMIT_BUFFER_SPACE                # 0x00010108
#       -SECTION_END-  ( 1.10 )

#       -SECTION_START-( 1.11 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_RECEIVE_BUFFER_SPACE                 # 0x00010109
#       -SECTION_END-  ( 1.11 )

#       -SECTION_START-( 1.12 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_TRANSMIT_BLOCK_SIZE                  # 0x0001010A
#       -SECTION_END-  ( 1.12 )

#       -SECTION_START-( 1.13 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_RECEIVE_BLOCK_SIZE                   # 0x0001010B
#       -SECTION_END-  ( 1.13 )

#       -SECTION_START-( 1.14 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_VENDOR_ID                            # 0x0001010C
#       -SECTION_END-  ( 1.14 )

#       -SECTION_START-( 1.15 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_VENDOR_DESCRIPTION                   # 0x0001010D
#       -SECTION_END-  ( 1.15 )

#       -SECTION_START-( 1.16 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER                # 0x0001010E
#       -SECTION_END-  ( 1.16 )

#       -SECTION_START-( 1.17 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F
#       -SECTION_END-  ( 1.17 )

#       -SECTION_START-( 1.18 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_DRIVER_VERSION                       # 0x00010110
#       -SECTION_END-  ( 1.18 )

#       -SECTION_START-( 1.19 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_MAXIMUM_TOTAL_SIZE                   # 0x00010111
#       -SECTION_END-  ( 1.19 )

#       -SECTION_START-( 1.20 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_MAC_OPTIONS                          # 0x00010112
#       -SECTION_END-  ( 1.20 )

########################################################

# 802.3 Objects

# 802.3 Operational Characteristics

#       -SECTION_START-( 1.21 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_3_PERMANENT_ADDRESS              # 0x01010101
#       -SECTION_END-  ( 1.21 )

#       -SECTION_START-( 1.22 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_3_CURRENT_ADDRESS                # 0x01010102
#       -SECTION_END-  ( 1.22 )

#       -SECTION_START-( 1.23 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_3_MULTICAST_LIST               # 0x01010103
#       -SECTION_END-  ( 1.23 )

#       -SECTION_START-( 1.24 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_3_MAXIMUM_LIST_SIZE          # 0x01010104
#       -SECTION_END-  ( 1.24 )

########################################################

# 802.3 Statistics - Optional

disable QUERYSTATISTICS
#       -SECTION_START-( 1.25 ) -OPTIONALS-( QUERYSTATISTICS )
#       -SECTION_START-( 1.2501 )
QueryStatistics                                +
    DeviceName=%TP_TEST_CARD%                 +
    OID=OID_802_3_XMIT_UNDERRUN                    # 0x01020204
#       -SECTION_END-  ( 1.2501 )
#
#       -SECTION_START-( 1.2502 )
QueryStatistics                                +
    DeviceName=%TP_TEST_CARD%                  +
    OID=OID_802_3_XMIT_HEARTBEAT_FAILURE           # 0x01020205
#       -SECTION_END-  ( 1.2502 )
#
#       -SECTION_START-( 1.2503 )
QueryStatistics                               +
    DeviceName=%TP_TEST_CARD%                 +
    OID=OID_802_3_XMIT_TIMES_CRS_LOST              # 0x01020206
#       -SECTION_END-  ( 1.2503 )

#       -SECTION_START-( 1.2504 )
QueryStatistics                                +
    DeviceName=%TP_TEST_CARD%                  +
    OID=OID_802_3_XMIT_LATE_COLLISIONS             # 0x01020207
#       -SECTION_END-  ( 1.2504 )
#       -SECTION_END-  ( 1.25 )
enable

########################################################

# finally close the adapters and dump the Event Queue for
# any unexpected events.

#       -SECTION_START-( 1.26 )
Close                                           +
    OpenInstance=1
#       -SECTION_END-  ( 1.26 )

# and dump the Event Queue for any unexpected events.

#       -SECTION_START-( 1.27 )
GetEvents                                       +
    OpenInstance=1
#       -SECTION_END-  ( 1.27 )

#       -SECTION_END-  ( 1.0 )

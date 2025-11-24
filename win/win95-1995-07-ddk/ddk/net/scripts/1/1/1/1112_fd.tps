#       -SECTION_START-( 1.0 ) -SECTION_DESC-( "Query Information Tests" )
##
## TITLE: 1.1.1.1112_FD Query Information Tests (1M/1C/10)
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

# General Oberational Characteristics

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

# FDDI Objects

# FDDI Operational Characteristics

#       -SECTION_START-( 1.21 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_FDDI_LONG_PERMANENT_ADDR                 # 0x03010101
#       -SECTION_END-  ( 1.21 )

#       -SECTION_START-( 1.22 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_FDDI_LONG_CURRENT_ADDR                   # 0x03010102
#       -SECTION_END-  ( 1.22 )

#       -SECTION_START-( 1.23 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_FDDI_LONG_MULTICAST_LIST                 # 0x03010103
#       -SECTION_END-  ( 1.23 )

#       -SECTION_START-( 1.24 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_FDDI_LONG_MAX_LIST_SIZE                  # 0x03010104
#       -SECTION_END-  ( 1.24 )

#       -SECTION_START-( 1.25 ) -OPTIONALS-()
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_FDDI_SHORT_PERMANENT_ADDR                # 0x03010105
#       -SECTION_END-  ( 1.25 )

#       -SECTION_START-( 1.26 ) -OPTIONALS-()
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_FDDI_SHORT_CURRENT_ADDR                  # 0x03010106
#       -SECTION_END-  ( 1.26 )

#       -SECTION_START-( 1.27 ) -OPTIONALS-()
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_FDDI_SHORT_MULTICAST_LIST                # 0x03010107
#       -SECTION_END-  ( 1.27 )

#       -SECTION_START-( 1.28 ) -OPTIONALS-()
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_FDDI_SHORT_MAX_LIST_SIZE                 # 0x03010108
#       -SECTION_END-  ( 1.28 )

########################################################

# FDDI Statistics - Mandatory

disable QUERYSTATISTICS
#       -SECTION_START-( 1.29 ) -OPTIONALS-( QUERYSTATISTICS )
QueryStatistics                                 +
    DeviceName=%TP_TEST_CARD%                   +
    OID=OID_FDDI_ATTACHMENT_TYPE            # 0x03020101
#       -SECTION_END-  ( 1.29 )

#       -SECTION_START-( 1.30 ) -OPTIONALS-( QUERYSTATISTICS )
QueryStatistics                                 +
    DeviceName=%TP_TEST_CARD%                   +
    OID=OID_FDDI_UPSTREAM_NODE_LONG         # 0x03020102
#       -SECTION_END-  ( 1.30 )

#       -SECTION_START-( 1.31 ) -OPTIONALS-( QUERYSTATISTICS )
QueryStatistics                                 +
    DeviceName=%TP_TEST_CARD%                   +
    OID=OID_FDDI_DOWNSTREAM_NODE_LONG       # 0x03020103
#       -SECTION_END-  ( 1.31 )

#       -SECTION_START-( 1.32 ) -OPTIONALS-( QUERYSTATISTICS )
QueryStatistics                                 +
    DeviceName=%TP_TEST_CARD%                   +
    OID=OID_FDDI_FRAME_ERRORS               # 0x03020104
#       -SECTION_END-  ( 1.32 )

#       -SECTION_START-( 1.33 ) -OPTIONALS-( QUERYSTATISTICS )
QueryStatistics                                 +
    DeviceName=%TP_TEST_CARD%                   +
    OID=OID_FDDI_FRAMES_LOST                # 0x03020105
#       -SECTION_END-  ( 1.33 )

#       -SECTION_START-( 1.34 ) -OPTIONALS-( QUERYSTATISTICS )
QueryStatistics                                 +
    DeviceName=%TP_TEST_CARD%                   +
    OID=OID_FDDI_RING_MGT_STATE             # 0x03020106
#       -SECTION_END-  ( 1.34 )

#       -SECTION_START-( 1.35 ) -OPTIONALS-( QUERYSTATISTICS )
QueryStatistics                                 +
    DeviceName=%TP_TEST_CARD%                   +
    OID=OID_FDDI_LCT_FAILURES               # 0x03020107
#       -SECTION_END-  ( 1.35 )

#       -SECTION_START-( 1.36 ) -OPTIONALS-( QUERYSTATISTICS )
QueryStatistics                                 +
    DeviceName=%TP_TEST_CARD%                   +
    OID=OID_FDDI_LEM_REJECTS                # 0x03020108
#       -SECTION_END-  ( 1.36 )

#       -SECTION_START-( 1.37 ) -OPTIONALS-( QUERYSTATISTICS )
QueryStatistics                                 +
    DeviceName=%TP_TEST_CARD%                   +
    OID=OID_FDDI_LCONNECTION_STATE          # 0x03020109
#       -SECTION_END-  ( 1.37 )
enable

########################################################

# finally close the adapters and dump the Event Queue for
# any unexpected events.

#       -SECTION_START-( 1.38 )
Close                                           +
    OpenInstance=1
#       -SECTION_END-  ( 1.38 )

# and dump the Event Queue for any unexpected events.

#       -SECTION_START-( 1.39 )
GetEvents                                       +
    OpenInstance=1
#       -SECTION_END-  ( 1.39 )

#       -SECTION_END-  ( 1.0 )

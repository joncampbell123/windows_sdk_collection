#	-SECTION_START-( 1.00 ) -SECTION_DESC-( "Query Information Tests" )
##
## TITLE: 1.2.1.1212_TR Query Information Tests (1M/1C/20)
##
## 1 Machine - 1 Card - 1 Open Instance on the Card.  These tests
## will verify the ability to query the all OIDs from multiple
## opens on the adapter.
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

# General Oberational Characteristics

#	-SECTION_START-( 1.05 ) -TOKEN_MATCH-( 4,0 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_SUPPORTED_LIST                   # 0x00010101
#	-SECTION_END-( 1.05 )

#	-SECTION_START-( 1.06 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_HARDWARE_STATUS                      # 0x00010102
#	-SECTION_END-( 1.06 )

#	-SECTION_START-( 1.07 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_MEDIA_SUPPORTED                 # 0x00010103
#	-SECTION_END-( 1.07 )

#	-SECTION_START-( 1.08 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_MEDIA_IN_USE                    # 0x00010104
#	-SECTION_END-( 1.08 )

#	-SECTION_START-( 1.09 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_MAXIMUM_LOOKAHEAD                    # 0x00010105
#	-SECTION_END-( 1.09 )

#	-SECTION_START-( 1.10 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_MAXIMUM_FRAME_SIZE                   # 0x00010106
#	-SECTION_END-( 1.10 )

#	-SECTION_START-( 1.11 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_LINK_SPEED                           # 0x00010107
#	-SECTION_END-( 1.11 )

#	-SECTION_START-( 1.12 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_TRANSMIT_BUFFER_SPACE                # 0x00010108
#	-SECTION_END-( 1.12 )

#	-SECTION_START-( 1.13 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_RECEIVE_BUFFER_SPACE                 # 0x00010109
#	-SECTION_END-( 1.13 )

#	-SECTION_START-( 1.14 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_TRANSMIT_BLOCK_SIZE                  # 0x0001010A
#	-SECTION_END-( 1.14 )

#	-SECTION_START-( 1.15 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_RECEIVE_BLOCK_SIZE                   # 0x0001010B
#	-SECTION_END-( 1.15 )

#	-SECTION_START-( 1.16 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_VENDOR_ID                            # 0x0001010C
#	-SECTION_END-( 1.16 )

#	-SECTION_START-( 1.17 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_DRIVER_VERSION                       # 0x0001010D
#	-SECTION_END-( 1.17 )

#	-SECTION_START-( 1.18 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER                # 0x0001010E
#	-SECTION_END-( 1.18 )

#	-SECTION_START-( 1.19 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F
#	-SECTION_END-( 1.19 )

########################################################

# 802.5 Objects

# 802.5 Operational Characteristics

#	-SECTION_START-( 1.20 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_PERMANENT_ADDRESS              # 0x02010101
#	-SECTION_END-( 1.20 )

#	-SECTION_START-( 1.21 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_ADDRESS                # 0x02010102
#	-SECTION_END-( 1.21 )

#	-SECTION_START-( 1.22 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL           # 0x02010103
#	-SECTION_END-( 1.22 )

#	-SECTION_START-( 1.23 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x02010104
#	-SECTION_END-( 1.23 )

disable QUERYSTATISTICS
#	-SECTION_START-( 1.24 ) -OPTIONALS-( QUERYSTATISTICS )
QueryStatistics                                 +
    DeviceName=%TP_TEST_CARD%                   +
    OID=OID_802_5_LAST_OPEN_STATUS                     # 0x02010105
#	-SECTION_END-( 1.24 )
enable

########################################################

#	-SECTION_START-( 1.25 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 1.25 )

#	-SECTION_START-( 1.26 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 1.26 )

########################################################

# General Oberational Characteristics

#	-SECTION_START-( 1.27 ) -TOKEN_MATCH-( 4,0 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_SUPPORTED_LIST                   # 0x00010101
#	-SECTION_END-( 1.27 )

#	-SECTION_START-( 1.28 ) -TOKEN_MATCH-( 4,0 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_SUPPORTED_LIST                   # 0x00010101
#	-SECTION_END-( 1.28 )

#	-SECTION_START-( 1.29 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_HARDWARE_STATUS                      # 0x00010102
#	-SECTION_END-( 1.29 )

#	-SECTION_START-( 1.30 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_HARDWARE_STATUS                      # 0x00010102
#	-SECTION_END-( 1.30 )

#	-SECTION_START-( 1.31 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_MEDIA_SUPPORTED                 # 0x00010103
#	-SECTION_END-( 1.31 )

#	-SECTION_START-( 1.32 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_MEDIA_SUPPORTED                 # 0x00010103
#	-SECTION_END-( 1.32 )

#	-SECTION_START-( 1.33 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_MEDIA_IN_USE                    # 0x00010104
#	-SECTION_END-( 1.33 )

#	-SECTION_START-( 1.34 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_MEDIA_IN_USE                    # 0x00010104
#	-SECTION_END-( 1.34 )

#	-SECTION_START-( 1.35 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_MAXIMUM_LOOKAHEAD                    # 0x00010105
#	-SECTION_END-( 1.35 )

#	-SECTION_START-( 1.36 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_MAXIMUM_LOOKAHEAD                    # 0x00010105
#	-SECTION_END-( 1.36 )

#	-SECTION_START-( 1.37 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_MAXIMUM_FRAME_SIZE                   # 0x00010106
#	-SECTION_END-( 1.37 )

#	-SECTION_START-( 1.38 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_MAXIMUM_FRAME_SIZE                   # 0x00010106
#	-SECTION_END-( 1.38 )

#	-SECTION_START-( 1.39 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_LINK_SPEED                           # 0x00010107
#	-SECTION_END-( 1.39 )

#	-SECTION_START-( 1.40 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_LINK_SPEED                           # 0x00010107
#	-SECTION_END-( 1.40 )

#	-SECTION_START-( 1.41 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_TRANSMIT_BUFFER_SPACE                # 0x00010108
#	-SECTION_END-( 1.41 )

#	-SECTION_START-( 1.42 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_TRANSMIT_BUFFER_SPACE                # 0x00010108
#	-SECTION_END-( 1.42 )

#	-SECTION_START-( 1.43 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_RECEIVE_BUFFER_SPACE                 # 0x00010109
#	-SECTION_END-( 1.43 )

#	-SECTION_START-( 1.44 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_RECEIVE_BUFFER_SPACE                 # 0x00010109
#	-SECTION_END-( 1.44 )

#	-SECTION_START-( 1.45 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_TRANSMIT_BLOCK_SIZE                  # 0x0001010A
#	-SECTION_END-( 1.45 )

#	-SECTION_START-( 1.46 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_TRANSMIT_BLOCK_SIZE                  # 0x0001010A
#	-SECTION_END-( 1.46 )

#	-SECTION_START-( 1.47 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_RECEIVE_BLOCK_SIZE                   # 0x0001010B
#	-SECTION_END-( 1.47 )

#	-SECTION_START-( 1.48 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_RECEIVE_BLOCK_SIZE                   # 0x0001010B
#	-SECTION_END-( 1.48 )

#	-SECTION_START-( 1.49 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_VENDOR_ID                            # 0x0001010C
#	-SECTION_END-( 1.49 )

#	-SECTION_START-( 1.50 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_VENDOR_ID                            # 0x0001010C
#	-SECTION_END-( 1.50 )

#	-SECTION_START-( 1.51 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_DRIVER_VERSION                       # 0x0001010D
#	-SECTION_END-( 1.51 )

#	-SECTION_START-( 1.52 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_DRIVER_VERSION                       # 0x0001010D
#	-SECTION_END-( 1.52 )

#	-SECTION_START-( 1.53 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER                # 0x0001010E
#	-SECTION_END-( 1.53 )

#	-SECTION_START-( 1.54 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_PACKET_FILTER                # 0x0001010E
#	-SECTION_END-( 1.54 )

#	-SECTION_START-( 1.55 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F
#	-SECTION_END-( 1.55 )

#	-SECTION_START-( 1.56 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F
#	-SECTION_END-( 1.56 )

########################################################

# 802.5 Objects

# 802.5 Operational Characteristics

#	-SECTION_START-( 1.57 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_PERMANENT_ADDRESS              # 0x02010101
#	-SECTION_END-( 1.57 )

#	-SECTION_START-( 1.58 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_PERMANENT_ADDRESS              # 0x02010101
#	-SECTION_END-( 1.58 )

#	-SECTION_START-( 1.59 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_ADDRESS                # 0x02010102
#	-SECTION_END-( 1.59 )

#	-SECTION_START-( 1.60 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_ADDRESS                # 0x02010102
#	-SECTION_END-( 1.60 )

#	-SECTION_START-( 1.61 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL           # 0x02010103
#	-SECTION_END-( 1.61 )

#	-SECTION_START-( 1.62 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL           # 0x02010103
#	-SECTION_END-( 1.62 )

#	-SECTION_START-( 1.63 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x02010104
#	-SECTION_END-( 1.63 )

#	-SECTION_START-( 1.64 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x02010104
#	-SECTION_END-( 1.64 )

disable QUERYSTATISTICS
#	-SECTION_START-( 1.65 ) -OPTIONALS-( QUERYSTATISTICS )
QueryStatistics                                 +
    DeviceName=%TP_TEST_CARD%                   +
    OID=OID_802_5_LAST_OPEN_STATUS                     # 0x02010105
#	-SECTION_END-( 1.65 )

#	-SECTION_START-( 1.66 ) -OPTIONALS-( QUERYSTATISTICS )
QueryStatistics                                 +
    DeviceName=%TP_TEST_CARD%                   +
    OID=OID_802_5_LAST_OPEN_STATUS                     # 0x02010105
#	-SECTION_END-( 1.66 )
enable

########################################################

# finally close the adapters and dump the Event Queue for
# any unexpected events.

#	-SECTION_START-( 1.67 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 1.67 )

#	-SECTION_START-( 1.68 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 1.68 )

# finally dump the Event Queue for any unexpected events.

#	-SECTION_START-( 1.69 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 1.69 )

#	-SECTION_START-( 1.70 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 1.70 )

#	-SECTION_END-( 1.00 )

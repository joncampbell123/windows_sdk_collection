#	-SECTION_START-( 1.00 ) -SECTION_DESC-( "Set Lookahead Size Tests" )
##
## TITLE: 1.2.1.1214 Set Lookahead Size Tests (1M/1C/20)
##
## 1 Machine - 1 Card - 2 Open Instances on the Card.  These tests
## will verify the ability to set and query the lookahead size
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

# and open the adapters

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

#	-SECTION_START-( 1.05 )
SetLookAheadSize                                +
    OpenInstance=1                              +
    LookAhead=1
#	-SECTION_END-( 1.05 )

#	-SECTION_START-( 1.06 )
SetLookAheadSize                                +
    OpenInstance=2                              +
    LookAhead=1
#	-SECTION_END-( 1.06 )

#	-SECTION_START-( 1.07 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F
#	-SECTION_END-( 1.07 )

#	-SECTION_START-( 1.08 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F
#	-SECTION_END-( 1.08 )

#	-SECTION_START-( 1.09 )
SetLookAheadSize                                +
    OpenInstance=1                              +
    LookAhead=2
#	-SECTION_END-( 1.09 )

#	-SECTION_START-( 1.10 )
SetLookAheadSize                                +
    OpenInstance=2                              +
    LookAhead=2
#	-SECTION_END-( 1.10 )

#	-SECTION_START-( 1.11 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F
#	-SECTION_END-( 1.11 )

#	-SECTION_START-( 1.12 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F
#	-SECTION_END-( 1.12 )

#	-SECTION_START-( 1.13 )
SetLookAheadSize                                +
    OpenInstance=1                              +
    LookAhead=4
#	-SECTION_END-( 1.13 )

#	-SECTION_START-( 1.14 )
SetLookAheadSize                                +
    OpenInstance=2                              +
    LookAhead=4
#	-SECTION_END-( 1.14 )

#	-SECTION_START-( 1.15 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F
#	-SECTION_END-( 1.15 )

#	-SECTION_START-( 1.16 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F
#	-SECTION_END-( 1.16 )

#	-SECTION_START-( 1.17 )
SetLookAheadSize                                +
    OpenInstance=1                              +
    LookAhead=8
#	-SECTION_END-( 1.17 )

#	-SECTION_START-( 1.18 )
SetLookAheadSize                                +
    OpenInstance=2                              +
    LookAhead=8
#	-SECTION_END-( 1.18 )

#	-SECTION_START-( 1.19 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F
#	-SECTION_END-( 1.19 )

#	-SECTION_START-( 1.20 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F
#	-SECTION_END-( 1.20 )

#	-SECTION_START-( 1.21 )
SetLookAheadSize                                +
    OpenInstance=1                              +
    LookAhead=16
#	-SECTION_END-( 1.21 )

#	-SECTION_START-( 1.22 )
SetLookAheadSize                                +
    OpenInstance=2                              +
    LookAhead=16
#	-SECTION_END-( 1.22 )

#	-SECTION_START-( 1.23 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F
#	-SECTION_END-( 1.23 )

#	-SECTION_START-( 1.24 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F
#	-SECTION_END-( 1.24 )

#	-SECTION_START-( 1.25 )
SetLookAheadSize                                +
    OpenInstance=1                              +
    LookAhead=32
#	-SECTION_END-( 1.25 )

#	-SECTION_START-( 1.26 )
SetLookAheadSize                                +
    OpenInstance=2                              +
    LookAhead=32
#	-SECTION_END-( 1.26 )

#	-SECTION_START-( 1.27 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F
#	-SECTION_END-( 1.27 )

#	-SECTION_START-( 1.28 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F
#	-SECTION_END-( 1.28 )

#	-SECTION_START-( 1.29 )
SetLookAheadSize                                +
    OpenInstance=1                              +
    LookAhead=64
#	-SECTION_END-( 1.29 )

#	-SECTION_START-( 1.30 )
SetLookAheadSize                                +
    OpenInstance=2                              +
    LookAhead=64
#	-SECTION_END-( 1.30 )

#	-SECTION_START-( 1.31 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F
#	-SECTION_END-( 1.31 )

#	-SECTION_START-( 1.32 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F
#	-SECTION_END-( 1.32 )

#	-SECTION_START-( 1.33 )
SetLookAheadSize                                +
    OpenInstance=1                              +
    LookAhead=128
#	-SECTION_END-( 1.33 )

#	-SECTION_START-( 1.34 )
SetLookAheadSize                                +
    OpenInstance=2                              +
    LookAhead=128
#	-SECTION_END-( 1.34 )

#	-SECTION_START-( 1.35 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F
#	-SECTION_END-( 1.35 )

#	-SECTION_START-( 1.36 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F
#	-SECTION_END-( 1.36 )

#	-SECTION_START-( 1.37 )
SetLookAheadSize                                +
    OpenInstance=1                              +
    LookAhead=%TP_MAX_LOOKAHEAD_SIZE%
#	-SECTION_END-( 1.37 )

#	-SECTION_START-( 1.38 )
SetLookAheadSize                                +
    OpenInstance=2                              +
    LookAhead=%TP_MAX_LOOKAHEAD_SIZE%
#	-SECTION_END-( 1.38 )

#	-SECTION_START-( 1.39 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F
#	-SECTION_END-( 1.39 )

#	-SECTION_START-( 1.40 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F
#	-SECTION_END-( 1.40 )

########################################################

#	-SECTION_START-( 1.41 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 1.41 )

#	-SECTION_START-( 1.42 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 1.42 )

########################################################


#	-SECTION_START-( 1.43 )
SetLookAheadSize                                +
    OpenInstance=1                              +
    LookAhead=1
#	-SECTION_END-( 1.43 )

#	-SECTION_START-( 1.44 )
SetLookAheadSize                                +
    OpenInstance=2                              +
    LookAhead=%TP_MAX_LOOKAHEAD_SIZE%
#	-SECTION_END-( 1.44 )

#	-SECTION_START-( 1.45 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F
#	-SECTION_END-( 1.45 )

#	-SECTION_START-( 1.46 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F
#	-SECTION_END-( 1.46 )

#	-SECTION_START-( 1.47 )
SetLookAheadSize                                +
    OpenInstance=1                              +
    LookAhead=2
#	-SECTION_END-( 1.47 )

#	-SECTION_START-( 1.48 )
SetLookAheadSize                                +
    OpenInstance=2                              +
    LookAhead=128
#	-SECTION_END-( 1.48 )

#	-SECTION_START-( 1.49 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F
#	-SECTION_END-( 1.49 )

#	-SECTION_START-( 1.50 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F
#	-SECTION_END-( 1.50 )

#	-SECTION_START-( 1.51 )
SetLookAheadSize                                +
    OpenInstance=1                              +
    LookAhead=4
#	-SECTION_END-( 1.51 )

#	-SECTION_START-( 1.52 )
SetLookAheadSize                                +
    OpenInstance=2                              +
    LookAhead=64
#	-SECTION_END-( 1.52 )

#	-SECTION_START-( 1.53 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F
#	-SECTION_END-( 1.53 )

#	-SECTION_START-( 1.54 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F
#	-SECTION_END-( 1.54 )

#	-SECTION_START-( 1.55 )
SetLookAheadSize                                +
    OpenInstance=1                              +
    LookAhead=8
#	-SECTION_END-( 1.55 )

#	-SECTION_START-( 1.56 )
SetLookAheadSize                                +
    OpenInstance=2                              +
    LookAhead=32
#	-SECTION_END-( 1.56 )

#	-SECTION_START-( 1.57 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F
#	-SECTION_END-( 1.57 )

#	-SECTION_START-( 1.58 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F
#	-SECTION_END-( 1.58 )

#	-SECTION_START-( 1.59 )
SetLookAheadSize                                +
    OpenInstance=1                              +
    LookAhead=16
#	-SECTION_END-( 1.59 )

#	-SECTION_START-( 1.60 )
SetLookAheadSize                                +
    OpenInstance=2                              +
    LookAhead=16
#	-SECTION_END-( 1.60 )

#	-SECTION_START-( 1.61 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F
#	-SECTION_END-( 1.61 )

#	-SECTION_START-( 1.62 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F
#	-SECTION_END-( 1.62 )

#	-SECTION_START-( 1.63 )
SetLookAheadSize                                +
    OpenInstance=1                              +
    LookAhead=32
#	-SECTION_END-( 1.63 )

#	-SECTION_START-( 1.64 )
SetLookAheadSize                                +
    OpenInstance=2                              +
    LookAhead=8
#	-SECTION_END-( 1.64 )

#	-SECTION_START-( 1.65 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F
#	-SECTION_END-( 1.65 )

#	-SECTION_START-( 1.66 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F
#	-SECTION_END-( 1.66 )

#	-SECTION_START-( 1.67 )
SetLookAheadSize                                +
    OpenInstance=1                              +
    LookAhead=64
#	-SECTION_END-( 1.67 )

#	-SECTION_START-( 1.68 )
SetLookAheadSize                                +
    OpenInstance=2                              +
    LookAhead=4
#	-SECTION_END-( 1.68 )

#	-SECTION_START-( 1.69 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F
#	-SECTION_END-( 1.69 )

#	-SECTION_START-( 1.70 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F
#	-SECTION_END-( 1.70 )

#	-SECTION_START-( 1.71 )
SetLookAheadSize                                +
    OpenInstance=1                              +
    LookAhead=128
#	-SECTION_END-( 1.71 )

#	-SECTION_START-( 1.72 )
SetLookAheadSize                                +
    OpenInstance=2                              +
    LookAhead=2
#	-SECTION_END-( 1.72 )

#	-SECTION_START-( 1.73 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F
#	-SECTION_END-( 1.73 )

#	-SECTION_START-( 1.74 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F
#	-SECTION_END-( 1.74 )

#	-SECTION_START-( 1.75 )
SetLookAheadSize                                +
    OpenInstance=1                              +
    LookAhead=%TP_MAX_LOOKAHEAD_SIZE%
#	-SECTION_END-( 1.75 )

#	-SECTION_START-( 1.76 )
SetLookAheadSize                                +
    OpenInstance=2                              +
    LookAhead=1
#	-SECTION_END-( 1.76 )

#	-SECTION_START-( 1.77 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F
#	-SECTION_END-( 1.77 )

#	-SECTION_START-( 1.78 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F
#	-SECTION_END-( 1.78 )

########################################################

# finally close the adapters and dump the Event Queue for
# any unexpected events.

#	-SECTION_START-( 1.79 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 1.79 )

#	-SECTION_START-( 1.80 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 1.80 )

# finally dump the Event Queue for any unexpected events.

#	-SECTION_START-( 1.81 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 1.81 )

#	-SECTION_START-( 1.82 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 1.82 )

#	-SECTION_END-( 1.00 )

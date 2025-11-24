#       -SECTION_START-( 1.0 ) -SECTION_DESC-( "Set Lookahead Size Tests" )
##
## TITLE: 1.1.1.1114 Set Lookahead Size Tests (1M/1C/10)
##
## 1 Machine - 1 Card - 1 Open Instance on the Card.  These tests
## will verify the ability to set and query the lookahead size
## from an adapter.

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

#   -SECTION_START-( 1.03 )
SetLookAheadSize                                +
    OpenInstance=1                              +
    LookAhead=1

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F


SetLookAheadSize                                +
    OpenInstance=1                              +
    LookAhead=2

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F

SetLookAheadSize                                +
    OpenInstance=1                              +
    LookAhead=4

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F

SetLookAheadSize                                +
    OpenInstance=1                              +
    LookAhead=8

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F

SetLookAheadSize                                +
    OpenInstance=1                              +
    LookAhead=16

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F

SetLookAheadSize                                +
    OpenInstance=1                              +
    LookAhead=32

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F

SetLookAheadSize                                +
    OpenInstance=1                              +
    LookAhead=64

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F

SetLookAheadSize                                +
    OpenInstance=1                              +
    LookAhead=128

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F

SetLookAheadSize                                +
    OpenInstance=1                              +
    LookAhead=%TP_MAX_LOOKAHEAD_SIZE%

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_GEN_CURRENT_LOOKAHEAD                    # 0x0001010F
#   -SECTION_END-  ( 1.03 )

########################################################

# finally close the adapter and dump the Event Queue for
# any unexpected events.

#   -SECTION_START-( 1.04 )
Close                                           +
    OpenInstance=1
#   -SECTION_END-  ( 1.04 )

# finally dump the Event Queue for any unexpected events.

#   -SECTION_START-( 1.05 )
GetEvents                                       +
    OpenInstance=1
#   -SECTION_END-  ( 1.05 )

#   -SECTION_END-  ( 1.0 )

#	-SECTION_START-( 1.00 ) -SECTION_DESC-( "Open Close Adapter Tests" )
##
## TITLE: 1.2.1.1211 Open Close Adapter Tests (1M/1C/20)
##
## 1 Machine - 1 Card - 2 Open Instances on the Card.  These tests
## will verify the ability to open and close an adapter multiple
## times.
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

#	-SECTION_START-( 1.03 )
GetEvents                                       +
    OpenInstance=3
#	-SECTION_END-( 1.03 )

#	-SECTION_START-( 1.04 )
GetEvents                                       +
    OpenInstance=4
#	-SECTION_END-( 1.04 )

########################################################

# repeatedly open and close two opens on the card, 10 times.

#   -SECTION_START-( 1.04 )
#	-SECTION_START-( 1.05 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 1.05 )
#   -SECTION_END-  ( 1.04 )

#	-SECTION_START-( 1.06 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 1.06 )

#	-SECTION_START-( 1.07 )
Open                                            +
    OpenInstance=2                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 1.07 )

#	-SECTION_START-( 1.08 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 1.08 )

#	-SECTION_START-( 1.09 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 1.09 )

#	-SECTION_START-( 1.10 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 1.10 )

#	-SECTION_START-( 1.11 )
Open                                            +
    OpenInstance=2                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 1.11 )

#	-SECTION_START-( 1.12 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 1.12 )

#	-SECTION_START-( 1.13 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 1.13 )

#	-SECTION_START-( 1.14 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 1.14 )

#	-SECTION_START-( 1.15 )
Open                                            +
    OpenInstance=2                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 1.15 )

#	-SECTION_START-( 1.16 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 1.16 )

#	-SECTION_START-( 1.17 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 1.17 )

#	-SECTION_START-( 1.18 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 1.18 )

#	-SECTION_START-( 1.19 )
Open                                            +
    OpenInstance=2                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 1.19 )

#	-SECTION_START-( 1.20 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 1.20 )

#	-SECTION_START-( 1.21 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 1.21 )

#	-SECTION_START-( 1.22 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 1.22 )

#	-SECTION_START-( 1.23 )
Open                                            +
    OpenInstance=2                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 1.23 )

#	-SECTION_START-( 1.24 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 1.24 )

#	-SECTION_START-( 1.25 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 1.25 )

#	-SECTION_START-( 1.26 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 1.26 )

#	-SECTION_START-( 1.27 )
Open                                            +
    OpenInstance=2                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 1.27 )

#	-SECTION_START-( 1.28 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 1.28 )

#	-SECTION_START-( 1.29 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 1.29 )

#	-SECTION_START-( 1.30 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 1.30 )

#	-SECTION_START-( 1.31 )
Open                                            +
    OpenInstance=2                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 1.31 )

#	-SECTION_START-( 1.32 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 1.32 )

#	-SECTION_START-( 1.33 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 1.33 )

#	-SECTION_START-( 1.34 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 1.34 )

#	-SECTION_START-( 1.35 )
Open                                            +
    OpenInstance=2                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 1.35 )

#	-SECTION_START-( 1.36 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 1.36 )

#	-SECTION_START-( 1.37 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 1.37 )

#	-SECTION_START-( 1.38 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 1.38 )

#	-SECTION_START-( 1.39 )
Open                                            +
    OpenInstance=2                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 1.39 )

#	-SECTION_START-( 1.40 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 1.40 )

#	-SECTION_START-( 1.41 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 1.41 )

#	-SECTION_START-( 1.42 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 1.42 )

#	-SECTION_START-( 1.43 )
Open                                            +
    OpenInstance=2                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 1.43 )

#	-SECTION_START-( 1.44 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 1.44 )

########################################################

# dump the Event Queue for any unexpected events.

#	-SECTION_START-( 1.45 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 1.45 )

#	-SECTION_START-( 1.46 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 1.46 )

########################################################

# open the card four times representing 4 different protocols
# opening the card at once.

#	-SECTION_START-( 1.47 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 1.47 )

#	-SECTION_START-( 1.48 )
Open                                            +
    OpenInstance=2                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 1.48 )

#	-SECTION_START-( 1.49 )
Open                                            +
    OpenInstance=3                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 1.49 )

#	-SECTION_START-( 1.50 )
Open                                            +
    OpenInstance=4                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 1.50 )

## then close each of the open instances

#	-SECTION_START-( 1.51 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 1.51 )

#	-SECTION_START-( 1.52 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 1.52 )

#	-SECTION_START-( 1.53 )
Close                                           +
    OpenInstance=3
#	-SECTION_END-( 1.53 )

#	-SECTION_START-( 1.54 )
Close                                           +
    OpenInstance=4
#	-SECTION_END-( 1.54 )

########################################################

# again dump the Event Queue for any unexpected events.

#	-SECTION_START-( 1.55 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 1.55 )

#	-SECTION_END-( 1.00 )

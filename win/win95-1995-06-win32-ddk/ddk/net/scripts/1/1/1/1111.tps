#       -SECTION_START-( 1.0 ) -SECTION_DESC-( "Open Close Adapter Tests" )
##
## TITLE: 1.1.1.1111 Open Close Adapter Tests (1M/1C/10)
##
## 1 Machine - 1 Card - 1 Open Instance on the Card.  These tests
## will verify the ability to open and close an adapter.
##

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#       -SECTION_START-( 1.01 )
GetEvents                                       +
    OpenInstance=1
#       -SECTION_END-  ( 1.01 )

#       -SECTION_START-( 1.02 )
GetEvents                                       +
    OpenInstance=2
#       -SECTION_END-  ( 1.02 )

#       -SECTION_START-( 1.03 )
GetEvents                                       +
    OpenInstance=3
#       -SECTION_END-  ( 1.03 )

#       -SECTION_START-( 1.04 )
GetEvents                                       +
    OpenInstance=4
#       -SECTION_END-  ( 1.04 )

########################################################

# repeatedly open and close the card, 10 times.

#       -SECTION_START-( 1.05 ) -SECTION_DESC-( "Repeatedly open&close the card, 10 times" )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%

Close                                           +
    OpenInstance=1

Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%

Close                                           +
    OpenInstance=1

Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%

Close                                           +
    OpenInstance=1

Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%

Close                                           +
    OpenInstance=1

Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%

Close                                           +
    OpenInstance=1

Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%

Close                                           +
    OpenInstance=1

Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%

Close                                           +
    OpenInstance=1

Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%

Close                                           +
    OpenInstance=1

Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%

Close                                           +
    OpenInstance=1

Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%

Close                                           +
    OpenInstance=1
#       -SECTION_END-  ( 1.05 )

########################################################

# dump the Event Queue for any unexpected events.

#       -SECTION_START-( 1.06 )
GetEvents                                       +
    OpenInstance=1
#       -SECTION_END-  ( 1.06 )

########################################################

# open the card four times representing 4 different protocols
# opening the card at once.

#       -SECTION_START-( 1.07 ) -SECTION_DESC-( "Open the card four times at once" )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%

Open                                            +
    OpenInstance=2                              +
    AdapterName=%TP_TEST_CARD%

Open                                            +
    OpenInstance=3                              +
    AdapterName=%TP_TEST_CARD%

Open                                            +
    OpenInstance=4                              +
    AdapterName=%TP_TEST_CARD%

## then close each of the open instances

Close                                           +
    OpenInstance=1

Close                                           +
    OpenInstance=2

Close                                           +
    OpenInstance=3

Close                                           +
    OpenInstance=4
#       -SECTION_END-  ( 1.07 )

########################################################

# again dump the Event Queue for any unexpected events.

#       -SECTION_START-( 1.08 )
GetEvents                                       +
    OpenInstance=1
#       -SECTION_END-  ( 1.08 )

#       -SECTION_START-( 1.09 )
GetEvents                                       +
    OpenInstance=2
#       -SECTION_END-  ( 1.09 )

#       -SECTION_START-( 1.10 )
GetEvents                                       +
    OpenInstance=3
#       -SECTION_END-  ( 1.10 )

#       -SECTION_START-( 1.11 )
GetEvents                                       +
    OpenInstance=4
#       -SECTION_END-  ( 1.11 )

#       -SECTION_END-  ( 1.0 )

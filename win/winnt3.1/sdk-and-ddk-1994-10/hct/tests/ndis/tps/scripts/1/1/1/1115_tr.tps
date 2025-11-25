#       -SECTION_START-( 1.0 ) -SECTION_DESC-( "Set Functional Address Tests" )
##
## TITLE: 1.1.1.1115_TR Set Functional Address Tests (1M/1C/10)
##
## 1 Machine - 1 Card - 1 Open Instance on the Card.  These tests
## will verify the ability to set, query and delete a functional
## address on the adapter.

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

# Add and delete the same address multiple times.

#   -SECTION_START-( 1.03 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=01-02-03-04

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=00-00-00-00

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=01-02-03-04

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=00-00-00-00

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=01-02-03-04

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=00-00-00-00

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=01-02-03-04

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=00-00-00-00

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=01-02-03-04

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=00-00-00-00

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=01-02-03-04

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=00-00-00-00

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=01-02-03-04

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=00-00-00-00

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=01-02-03-04

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=00-00-00-00

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=01-02-03-04

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=00-00-00-00

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=01-02-03-04

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=00-00-00-00

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=01-02-03-04

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=00-00-00-00

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=01-02-03-04

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=00-00-00-00

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=01-02-03-04

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=00-00-00-00

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=01-02-03-04

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=00-00-00-00

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=01-02-03-04

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=00-00-00-00

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=01-02-03-04

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=00-00-00-00

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#   -SECTION_END-  ( 1.03 )

########################################################

# Add and delete the different addresses.

#   -SECTION_START-( 1.04 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=01-02-03-00

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=00-00-00-00

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=01-02-03-01

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=00-00-00-00

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=01-02-03-02

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=00-00-00-00

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=01-02-03-03

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=00-00-00-00

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=01-02-03-04

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=00-00-00-00

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=01-02-03-05

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=00-00-00-00

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=01-02-03-06

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=00-00-00-00

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=01-02-03-07

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=00-00-00-00

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=01-02-03-08

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=00-00-00-00

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=01-02-03-09

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=00-00-00-00

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=01-02-03-0A

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=00-00-00-00

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=01-02-03-0B

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=00-00-00-00

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=01-02-03-0C

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=00-00-00-00

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=01-02-03-0D

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=00-00-00-00

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=01-02-03-0E

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=00-00-00-00

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=01-02-03-0F

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103

SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=00-00-00-00

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#   -SECTION_END-  ( 1.04 )

########################################################

# finally close the adapter and dump the Event Queue for
# any unexpected events.

#   -SECTION_START-( 1.05 )
Close                                           +
    OpenInstance=1
#   -SECTION_END-  ( 1.05 )

# finally dump the Event Queue for any unexpected events.

#   -SECTION_START-( 1.06 )
GetEvents                                       +
    OpenInstance=1
#   -SECTION_END-  ( 1.06 )

#   -SECTION_END-  ( 1.0 )

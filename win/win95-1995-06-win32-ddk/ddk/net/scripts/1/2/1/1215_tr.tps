#	-SECTION_START-( 1.00 ) -SECTION_DESC-( "Set Functional Address Tests" )
##
## TITLE: 1.2.1.1215_TR Set Functional Address Tests (1M/1C/20)
##
## 1 Machine - 1 Card - 2 Open Instances on the Card.  These tests
## will verify the ability to set, query and delete a functional
## address from multiple opens on the adapter.
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

# Add and delete the same address multiple times.

#	-SECTION_START-( 1.05 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.05 )

#	-SECTION_START-( 1.06 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.06 )

#	-SECTION_START-( 1.07 )
SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=01-02-03-04
#	-SECTION_END-( 1.07 )

#	-SECTION_START-( 1.08 )
SetFunctionalAddress                            +
    OpenInstance=2                              +
    FunctionalAddress=01-02-03-14
#	-SECTION_END-( 1.08 )

#	-SECTION_START-( 1.09 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.09 )

#	-SECTION_START-( 1.10 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.10 )

#	-SECTION_START-( 1.11 )
SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=00-00-00-00
#	-SECTION_END-( 1.11 )

#	-SECTION_START-( 1.12 )
SetFunctionalAddress                            +
    OpenInstance=2                              +
    FunctionalAddress=00-00-00-00
#	-SECTION_END-( 1.12 )

#	-SECTION_START-( 1.13 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.13 )

#	-SECTION_START-( 1.14 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.14 )

#	-SECTION_START-( 1.15 )
SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=01-02-03-04
#	-SECTION_END-( 1.15 )

#	-SECTION_START-( 1.16 )
SetFunctionalAddress                            +
    OpenInstance=2                              +
    FunctionalAddress=01-02-03-14
#	-SECTION_END-( 1.16 )

#	-SECTION_START-( 1.17 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.17 )

#	-SECTION_START-( 1.18 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.18 )

#	-SECTION_START-( 1.19 )
SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=00-00-00-00
#	-SECTION_END-( 1.19 )

#	-SECTION_START-( 1.20 )
SetFunctionalAddress                            +
    OpenInstance=2                              +
    FunctionalAddress=00-00-00-00
#	-SECTION_END-( 1.20 )

#	-SECTION_START-( 1.21 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.21 )

#	-SECTION_START-( 1.22 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.22 )

#	-SECTION_START-( 1.23 )
SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=01-02-03-04
#	-SECTION_END-( 1.23 )

#	-SECTION_START-( 1.24 )
SetFunctionalAddress                            +
    OpenInstance=2                              +
    FunctionalAddress=01-02-03-14
#	-SECTION_END-( 1.24 )

#	-SECTION_START-( 1.25 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.25 )

#	-SECTION_START-( 1.26 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.26 )

#	-SECTION_START-( 1.27 )
SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=00-00-00-00
#	-SECTION_END-( 1.27 )

#	-SECTION_START-( 1.28 )
SetFunctionalAddress                            +
    OpenInstance=2                              +
    FunctionalAddress=00-00-00-00
#	-SECTION_END-( 1.28 )

#	-SECTION_START-( 1.29 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.29 )

#	-SECTION_START-( 1.30 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.30 )

#	-SECTION_START-( 1.31 )
SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=01-02-03-04
#	-SECTION_END-( 1.31 )

#	-SECTION_START-( 1.32 )
SetFunctionalAddress                            +
    OpenInstance=2                              +
    FunctionalAddress=01-02-03-14
#	-SECTION_END-( 1.32 )

#	-SECTION_START-( 1.33 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.33 )

#	-SECTION_START-( 1.34 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.34 )

#	-SECTION_START-( 1.35 )
SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=00-00-00-00
#	-SECTION_END-( 1.35 )

#	-SECTION_START-( 1.36 )
SetFunctionalAddress                            +
    OpenInstance=2                              +
    FunctionalAddress=00-00-00-00
#	-SECTION_END-( 1.36 )

#	-SECTION_START-( 1.37 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.37 )

#	-SECTION_START-( 1.38 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.38 )

#	-SECTION_START-( 1.39 )
SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=01-02-03-04
#	-SECTION_END-( 1.39 )

#	-SECTION_START-( 1.40 )
SetFunctionalAddress                            +
    OpenInstance=2                              +
    FunctionalAddress=01-02-03-14
#	-SECTION_END-( 1.40 )

#	-SECTION_START-( 1.41 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.41 )

#	-SECTION_START-( 1.42 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.42 )

#	-SECTION_START-( 1.43 )
SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=00-00-00-00
#	-SECTION_END-( 1.43 )

#	-SECTION_START-( 1.44 )
SetFunctionalAddress                            +
    OpenInstance=2                              +
    FunctionalAddress=00-00-00-00
#	-SECTION_END-( 1.44 )

#	-SECTION_START-( 1.45 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.45 )

#	-SECTION_START-( 1.46 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.46 )

#	-SECTION_START-( 1.47 )
SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=01-02-03-04
#	-SECTION_END-( 1.47 )

#	-SECTION_START-( 1.48 )
SetFunctionalAddress                            +
    OpenInstance=2                              +
    FunctionalAddress=01-02-03-14
#	-SECTION_END-( 1.48 )

#	-SECTION_START-( 1.49 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.49 )

#	-SECTION_START-( 1.50 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.50 )

#	-SECTION_START-( 1.51 )
SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=00-00-00-00
#	-SECTION_END-( 1.51 )

#	-SECTION_START-( 1.52 )
SetFunctionalAddress                            +
    OpenInstance=2                              +
    FunctionalAddress=00-00-00-00
#	-SECTION_END-( 1.52 )

#	-SECTION_START-( 1.53 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.53 )

#	-SECTION_START-( 1.54 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.54 )

#	-SECTION_START-( 1.55 )
SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=01-02-03-04
#	-SECTION_END-( 1.55 )

#	-SECTION_START-( 1.56 )
SetFunctionalAddress                            +
    OpenInstance=2                              +
    FunctionalAddress=01-02-03-14
#	-SECTION_END-( 1.56 )

#	-SECTION_START-( 1.57 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.57 )

#	-SECTION_START-( 1.58 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.58 )

#	-SECTION_START-( 1.59 )
SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=00-00-00-00
#	-SECTION_END-( 1.59 )

#	-SECTION_START-( 1.60 )
SetFunctionalAddress                            +
    OpenInstance=2                              +
    FunctionalAddress=00-00-00-00
#	-SECTION_END-( 1.60 )

#	-SECTION_START-( 1.61 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.61 )

#	-SECTION_START-( 1.62 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.62 )

#	-SECTION_START-( 1.63 )
SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=01-02-03-04
#	-SECTION_END-( 1.63 )

#	-SECTION_START-( 1.64 )
SetFunctionalAddress                            +
    OpenInstance=2                              +
    FunctionalAddress=01-02-03-14
#	-SECTION_END-( 1.64 )

#	-SECTION_START-( 1.65 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.65 )

#	-SECTION_START-( 1.66 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.66 )

#	-SECTION_START-( 1.67 )
SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=00-00-00-00
#	-SECTION_END-( 1.67 )

#	-SECTION_START-( 1.68 )
SetFunctionalAddress                            +
    OpenInstance=2                              +
    FunctionalAddress=00-00-00-00
#	-SECTION_END-( 1.68 )

#	-SECTION_START-( 1.69 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.69 )

#	-SECTION_START-( 1.70 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.70 )

#	-SECTION_START-( 1.71 )
SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=01-02-03-04
#	-SECTION_END-( 1.71 )

#	-SECTION_START-( 1.72 )
SetFunctionalAddress                            +
    OpenInstance=2                              +
    FunctionalAddress=01-02-03-14
#	-SECTION_END-( 1.72 )

#	-SECTION_START-( 1.73 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.73 )

#	-SECTION_START-( 1.74 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.74 )

#	-SECTION_START-( 1.75 )
SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=00-00-00-00
#	-SECTION_END-( 1.75 )

#	-SECTION_START-( 1.76 )
SetFunctionalAddress                            +
    OpenInstance=2                              +
    FunctionalAddress=00-00-00-00
#	-SECTION_END-( 1.76 )

#	-SECTION_START-( 1.77 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.77 )

#	-SECTION_START-( 1.78 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.78 )

#	-SECTION_START-( 1.79 )
SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=01-02-03-04
#	-SECTION_END-( 1.79 )

#	-SECTION_START-( 1.80 )
SetFunctionalAddress                            +
    OpenInstance=2                              +
    FunctionalAddress=01-02-03-14
#	-SECTION_END-( 1.80 )

#	-SECTION_START-( 1.81 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.81 )

#	-SECTION_START-( 1.82 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.82 )

#	-SECTION_START-( 1.83 )
SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=00-00-00-00
#	-SECTION_END-( 1.83 )

#	-SECTION_START-( 1.84 )
SetFunctionalAddress                            +
    OpenInstance=2                              +
    FunctionalAddress=00-00-00-00
#	-SECTION_END-( 1.84 )

#	-SECTION_START-( 1.85 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.85 )

#	-SECTION_START-( 1.86 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.86 )

#	-SECTION_START-( 1.87 )
SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=01-02-03-04
#	-SECTION_END-( 1.87 )

#	-SECTION_START-( 1.88 )
SetFunctionalAddress                            +
    OpenInstance=2                              +
    FunctionalAddress=01-02-03-14
#	-SECTION_END-( 1.88 )

#	-SECTION_START-( 1.89 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.89 )

#	-SECTION_START-( 1.90 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.90 )

#	-SECTION_START-( 1.91 )
SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=00-00-00-00
#	-SECTION_END-( 1.91 )

#	-SECTION_START-( 1.92 )
SetFunctionalAddress                            +
    OpenInstance=2                              +
    FunctionalAddress=00-00-00-00
#	-SECTION_END-( 1.92 )

#	-SECTION_START-( 1.93 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.93 )

#	-SECTION_START-( 1.94 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.94 )

#	-SECTION_START-( 1.95 )
SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=01-02-03-04
#	-SECTION_END-( 1.95 )

#	-SECTION_START-( 1.96 )
SetFunctionalAddress                            +
    OpenInstance=2                              +
    FunctionalAddress=01-02-03-14
#	-SECTION_END-( 1.96 )

#	-SECTION_START-( 1.97 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.97 )

#	-SECTION_START-( 1.98 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.98 )

#	-SECTION_START-( 1.99 )
SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=00-00-00-00
#	-SECTION_END-( 1.99 )

#	-SECTION_START-( 1.100 )
SetFunctionalAddress                            +
    OpenInstance=2                              +
    FunctionalAddress=00-00-00-00
#	-SECTION_END-( 1.100 )

#	-SECTION_START-( 1.101 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.101 )

#	-SECTION_START-( 1.102 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.102 )

#	-SECTION_START-( 1.103 )
SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=01-02-03-04
#	-SECTION_END-( 1.103 )

#	-SECTION_START-( 1.104 )
SetFunctionalAddress                            +
    OpenInstance=2                              +
    FunctionalAddress=01-02-03-14
#	-SECTION_END-( 1.104 )

#	-SECTION_START-( 1.105 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.105 )

#	-SECTION_START-( 1.106 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.106 )

#	-SECTION_START-( 1.107 )
SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=00-00-00-00
#	-SECTION_END-( 1.107 )

#	-SECTION_START-( 1.108 )
SetFunctionalAddress                            +
    OpenInstance=2                              +
    FunctionalAddress=00-00-00-00
#	-SECTION_END-( 1.108 )

#	-SECTION_START-( 1.109 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.109 )

#	-SECTION_START-( 1.110 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.110 )

#	-SECTION_START-( 1.111 )
SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=01-02-03-04
#	-SECTION_END-( 1.111 )

#	-SECTION_START-( 1.112 )
SetFunctionalAddress                            +
    OpenInstance=2                              +
    FunctionalAddress=01-02-03-14
#	-SECTION_END-( 1.112 )

#	-SECTION_START-( 1.113 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.113 )

#	-SECTION_START-( 1.114 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.114 )

#	-SECTION_START-( 1.115 )
SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=00-00-00-00
#	-SECTION_END-( 1.115 )

#	-SECTION_START-( 1.116 )
SetFunctionalAddress                            +
    OpenInstance=2                              +
    FunctionalAddress=00-00-00-00
#	-SECTION_END-( 1.116 )

#	-SECTION_START-( 1.117 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.117 )

#	-SECTION_START-( 1.118 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.118 )

#	-SECTION_START-( 1.119 )
SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=01-02-03-04
#	-SECTION_END-( 1.119 )

#	-SECTION_START-( 1.120 )
SetFunctionalAddress                            +
    OpenInstance=2                              +
    FunctionalAddress=01-02-03-14
#	-SECTION_END-( 1.120 )

#	-SECTION_START-( 1.121 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.121 )

#	-SECTION_START-( 1.122 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.122 )

#	-SECTION_START-( 1.123 )
SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=00-00-00-00
#	-SECTION_END-( 1.123 )

#	-SECTION_START-( 1.124 )
SetFunctionalAddress                            +
    OpenInstance=2                              +
    FunctionalAddress=00-00-00-00
#	-SECTION_END-( 1.124 )

#	-SECTION_START-( 1.125 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.125 )

#	-SECTION_START-( 1.126 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.126 )

#	-SECTION_START-( 1.127 )
SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=01-02-03-04
#	-SECTION_END-( 1.127 )

#	-SECTION_START-( 1.128 )
SetFunctionalAddress                            +
    OpenInstance=2                              +
    FunctionalAddress=01-02-03-14
#	-SECTION_END-( 1.128 )

#	-SECTION_START-( 1.129 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.129 )

#	-SECTION_START-( 1.130 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.130 )

#	-SECTION_START-( 1.131 )
SetFunctionalAddress                            +
    OpenInstance=1                              +
    FunctionalAddress=00-00-00-00
#	-SECTION_END-( 1.131 )

#	-SECTION_START-( 1.132 )
SetFunctionalAddress                            +
    OpenInstance=2                              +
    FunctionalAddress=00-00-00-00
#	-SECTION_END-( 1.132 )

#	-SECTION_START-( 1.133 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.133 )

#	-SECTION_START-( 1.134 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_FUNCTIONAL                # 0x2010103
#	-SECTION_END-( 1.134 )

readscript \tps\scripts\1\2\1\12155_tr.tps
########################################################

# finally close the adapter and dump the Event Queue for
# any unexpected events.

#	-SECTION_START-( 1.265 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 1.265 )

#	-SECTION_START-( 1.266 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 1.266 )

# finally dump the Event Queue for any unexpected events.

#	-SECTION_START-( 1.267 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 1.267 )

#	-SECTION_START-( 1.268 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 1.268 )

#	-SECTION_END-( 1.00 )

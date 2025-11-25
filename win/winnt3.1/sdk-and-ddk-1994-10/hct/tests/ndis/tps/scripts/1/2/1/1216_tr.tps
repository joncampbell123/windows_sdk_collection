#	-SECTION_START-( 1.00 ) -SECTION_DESC-( "Set Group Address Tests" )
##
## TITLE: 1.2.1.1216_TR Set Group Address Tests (1M/1C/20)
##
## 1 Machine - 1 Card - 2 Open Instances on the Card.  These tests
## will verify the ability to set, query and delete a group
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

# Set the cards group address to the NULL state

SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=00-00-00-00

SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.04 )

########################################################

# Add and delete the same address multiple times.

#	-SECTION_START-( 1.05 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.05 )

#	-SECTION_START-( 1.06 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.06 )

#	-SECTION_START-( 1.07 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=81-02-03-04
#	-SECTION_END-( 1.07 )

#	-SECTION_START-( 1.08 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-14
#	-SECTION_END-( 1.08 )

#	-SECTION_START-( 1.09 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.09 )

#	-SECTION_START-( 1.10 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104

SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-04

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.10 )

#	-SECTION_START-( 1.11 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.11 )

#	-SECTION_START-( 1.12 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.12 )

#	-SECTION_START-( 1.13 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.13 )

#	-SECTION_START-( 1.14 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.14 )

#	-SECTION_START-( 1.15 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=81-02-03-04
#	-SECTION_END-( 1.15 )

#	-SECTION_START-( 1.16 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-14
#	-SECTION_END-( 1.16 )

#	-SECTION_START-( 1.17 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.17 )

#	-SECTION_START-( 1.18 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104

SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-04

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.18 )

#	-SECTION_START-( 1.19 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.19 )

#	-SECTION_START-( 1.20 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.20 )

#	-SECTION_START-( 1.21 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.21 )

#	-SECTION_START-( 1.22 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.22 )

#	-SECTION_START-( 1.23 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=81-02-03-04
#	-SECTION_END-( 1.23 )

#	-SECTION_START-( 1.24 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-14
#	-SECTION_END-( 1.24 )

#	-SECTION_START-( 1.25 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.25 )

#	-SECTION_START-( 1.26 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104

SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-04

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.26 )

#	-SECTION_START-( 1.27 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.27 )

#	-SECTION_START-( 1.28 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.28 )

#	-SECTION_START-( 1.29 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.29 )

#	-SECTION_START-( 1.30 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.30 )

#	-SECTION_START-( 1.31 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=81-02-03-04
#	-SECTION_END-( 1.31 )

#	-SECTION_START-( 1.32 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-14
#	-SECTION_END-( 1.32 )

#	-SECTION_START-( 1.33 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.33 )

#	-SECTION_START-( 1.34 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104

SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-04

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.34 )

#	-SECTION_START-( 1.35 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.35 )

#	-SECTION_START-( 1.36 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.36 )

#	-SECTION_START-( 1.37 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.37 )

#	-SECTION_START-( 1.38 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.38 )

#	-SECTION_START-( 1.39 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=81-02-03-04
#	-SECTION_END-( 1.39 )

#	-SECTION_START-( 1.40 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-14
#	-SECTION_END-( 1.40 )

#	-SECTION_START-( 1.41 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.41 )

#	-SECTION_START-( 1.42 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104

SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-04

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.42 )

#	-SECTION_START-( 1.43 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.43 )

#	-SECTION_START-( 1.44 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.44 )

#	-SECTION_START-( 1.45 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.45 )

#	-SECTION_START-( 1.46 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.46 )

#	-SECTION_START-( 1.47 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=81-02-03-04
#	-SECTION_END-( 1.47 )

#	-SECTION_START-( 1.48 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-14
#	-SECTION_END-( 1.48 )

#	-SECTION_START-( 1.49 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.49 )

#	-SECTION_START-( 1.50 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104

SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-04

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.50 )

#	-SECTION_START-( 1.51 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.51 )

#	-SECTION_START-( 1.52 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.52 )

#	-SECTION_START-( 1.53 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.53 )

#	-SECTION_START-( 1.54 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.54 )

#	-SECTION_START-( 1.55 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=81-02-03-04
#	-SECTION_END-( 1.55 )

#	-SECTION_START-( 1.56 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-14
#	-SECTION_END-( 1.56 )

#	-SECTION_START-( 1.57 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.57 )

#	-SECTION_START-( 1.58 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104

SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-04

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.58 )

#	-SECTION_START-( 1.59 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.59 )

#	-SECTION_START-( 1.60 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.60 )

#	-SECTION_START-( 1.61 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.61 )

#	-SECTION_START-( 1.62 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.62 )

#	-SECTION_START-( 1.63 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=81-02-03-04
#	-SECTION_END-( 1.63 )

#	-SECTION_START-( 1.64 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-14
#	-SECTION_END-( 1.64 )

#	-SECTION_START-( 1.65 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.65 )

#	-SECTION_START-( 1.66 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104

SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-04

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.66 )

#	-SECTION_START-( 1.67 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.67 )

#	-SECTION_START-( 1.68 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.68 )

#	-SECTION_START-( 1.69 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.69 )

#	-SECTION_START-( 1.70 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.70 )

#	-SECTION_START-( 1.71 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=81-02-03-04
#	-SECTION_END-( 1.71 )

#	-SECTION_START-( 1.72 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-14
#	-SECTION_END-( 1.72 )

#	-SECTION_START-( 1.73 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.73 )

#	-SECTION_START-( 1.74 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104

SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-04

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.74 )

#	-SECTION_START-( 1.75 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.75 )

#	-SECTION_START-( 1.76 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.76 )

#	-SECTION_START-( 1.77 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.77 )

#	-SECTION_START-( 1.78 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.78 )

#	-SECTION_START-( 1.79 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=81-02-03-04
#	-SECTION_END-( 1.79 )

#	-SECTION_START-( 1.80 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-14
#	-SECTION_END-( 1.80 )

#	-SECTION_START-( 1.81 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.81 )

#	-SECTION_START-( 1.82 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104

SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-04

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.82 )

#	-SECTION_START-( 1.83 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.83 )

#	-SECTION_START-( 1.84 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.84 )

#	-SECTION_START-( 1.85 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.85 )

#	-SECTION_START-( 1.86 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.86 )

#	-SECTION_START-( 1.87 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=81-02-03-04
#	-SECTION_END-( 1.87 )

#	-SECTION_START-( 1.88 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-14
#	-SECTION_END-( 1.88 )

#	-SECTION_START-( 1.89 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.89 )

#	-SECTION_START-( 1.90 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104

SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-04

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.90 )

#	-SECTION_START-( 1.91 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.91 )

#	-SECTION_START-( 1.92 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.92 )

#	-SECTION_START-( 1.93 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.93 )

#	-SECTION_START-( 1.94 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.94 )

#	-SECTION_START-( 1.95 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=81-02-03-04
#	-SECTION_END-( 1.95 )

#	-SECTION_START-( 1.96 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-14
#	-SECTION_END-( 1.96 )

#	-SECTION_START-( 1.97 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.97 )

#	-SECTION_START-( 1.98 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104

SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-04

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.98 )

#	-SECTION_START-( 1.99 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.99 )

#	-SECTION_START-( 1.100 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.100 )

#	-SECTION_START-( 1.101 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.101 )

#	-SECTION_START-( 1.102 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.102 )

#	-SECTION_START-( 1.103 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=81-02-03-04
#	-SECTION_END-( 1.103 )

#	-SECTION_START-( 1.104 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-14
#	-SECTION_END-( 1.104 )

#	-SECTION_START-( 1.105 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.105 )

#	-SECTION_START-( 1.106 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104

SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-04

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.106 )

#	-SECTION_START-( 1.107 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.107 )

#	-SECTION_START-( 1.108 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.108 )

#	-SECTION_START-( 1.109 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.109 )

#	-SECTION_START-( 1.110 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.110 )

#	-SECTION_START-( 1.111 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=81-02-03-04
#	-SECTION_END-( 1.111 )

#	-SECTION_START-( 1.112 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-14
#	-SECTION_END-( 1.112 )

#	-SECTION_START-( 1.113 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.113 )

#	-SECTION_START-( 1.114 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104

SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-04

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.114 )

#	-SECTION_START-( 1.115 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.115 )

#	-SECTION_START-( 1.116 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.116 )

#	-SECTION_START-( 1.117 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.117 )

#	-SECTION_START-( 1.118 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.118 )

#	-SECTION_START-( 1.119 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=81-02-03-04
#	-SECTION_END-( 1.119 )

#	-SECTION_START-( 1.120 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-14
#	-SECTION_END-( 1.120 )

#	-SECTION_START-( 1.121 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.121 )

#	-SECTION_START-( 1.122 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104

SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-04

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.122 )

#	-SECTION_START-( 1.123 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.123 )

#	-SECTION_START-( 1.124 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.124 )

#	-SECTION_START-( 1.125 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.125 )

#	-SECTION_START-( 1.126 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.126 )

#	-SECTION_START-( 1.127 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=81-02-03-04
#	-SECTION_END-( 1.127 )

#	-SECTION_START-( 1.128 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-14
#	-SECTION_END-( 1.128 )

#	-SECTION_START-( 1.129 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.129 )

#	-SECTION_START-( 1.130 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104

SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-04

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.130 )

#	-SECTION_START-( 1.131 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.131 )

#	-SECTION_START-( 1.132 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.132 )

#	-SECTION_START-( 1.133 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.133 )

#	-SECTION_START-( 1.134 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.134 )


########################################################

# Add and delete the different addresses.
# Note that the group addresses will be nulled in
# a different order

#	-SECTION_START-( 1.135 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=81-02-03-00
#	-SECTION_END-( 1.135 )

#	-SECTION_START-( 1.136 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-10
#	-SECTION_END-( 1.136 )

#	-SECTION_START-( 1.137 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.137 )

#	-SECTION_START-( 1.138 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104

SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-00

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.138 )

#	-SECTION_START-( 1.139 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.139 )

#	-SECTION_START-( 1.140 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.140 )

#	-SECTION_START-( 1.141 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.141 )

#	-SECTION_START-( 1.142 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.142 )

#	-SECTION_START-( 1.143 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=81-02-03-01
#	-SECTION_END-( 1.143 )

#	-SECTION_START-( 1.144 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-11
#	-SECTION_END-( 1.144 )

#	-SECTION_START-( 1.145 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.145 )

#	-SECTION_START-( 1.146 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104

SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-01

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.146 )

#	-SECTION_START-( 1.147 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.147 )

#	-SECTION_START-( 1.148 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.148 )

#	-SECTION_START-( 1.149 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.149 )

#	-SECTION_START-( 1.150 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.150 )

#	-SECTION_START-( 1.151 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=81-02-03-02
#	-SECTION_END-( 1.151 )

#	-SECTION_START-( 1.152 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-12
#	-SECTION_END-( 1.152 )

#	-SECTION_START-( 1.153 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.153 )

#	-SECTION_START-( 1.154 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104

SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-02

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.154 )

#	-SECTION_START-( 1.155 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.155 )

#	-SECTION_START-( 1.156 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.156 )

#	-SECTION_START-( 1.157 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.157 )

#	-SECTION_START-( 1.158 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.158 )

#	-SECTION_START-( 1.159 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=81-02-03-03
#	-SECTION_END-( 1.159 )

#	-SECTION_START-( 1.160 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-13
#	-SECTION_END-( 1.160 )

#	-SECTION_START-( 1.161 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.161 )

#	-SECTION_START-( 1.162 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104

SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-03

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.162 )

#	-SECTION_START-( 1.163 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.163 )

#	-SECTION_START-( 1.164 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.164 )

#	-SECTION_START-( 1.165 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.165 )

#	-SECTION_START-( 1.166 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.166 )

#	-SECTION_START-( 1.167 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=81-02-03-04
#	-SECTION_END-( 1.167 )

#	-SECTION_START-( 1.168 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-14
#	-SECTION_END-( 1.168 )

#	-SECTION_START-( 1.169 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.169 )

#	-SECTION_START-( 1.170 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104

SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-04

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.170 )

#	-SECTION_START-( 1.171 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.171 )

#	-SECTION_START-( 1.172 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.172 )

#	-SECTION_START-( 1.173 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.173 )

#	-SECTION_START-( 1.174 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.174 )

#	-SECTION_START-( 1.175 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=81-02-03-05
#	-SECTION_END-( 1.175 )

#	-SECTION_START-( 1.176 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-15
#	-SECTION_END-( 1.176 )

#	-SECTION_START-( 1.177 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.177 )

#	-SECTION_START-( 1.178 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104

SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-05

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.178 )

#	-SECTION_START-( 1.179 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.179 )

#	-SECTION_START-( 1.180 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.180 )

#	-SECTION_START-( 1.181 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.181 )

#	-SECTION_START-( 1.182 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.182 )

#	-SECTION_START-( 1.183 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=81-02-03-06
#	-SECTION_END-( 1.183 )

#	-SECTION_START-( 1.184 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-16
#	-SECTION_END-( 1.184 )

#	-SECTION_START-( 1.185 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.185 )

#	-SECTION_START-( 1.186 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104

SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-06

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.186 )

#	-SECTION_START-( 1.187 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.187 )

#	-SECTION_START-( 1.188 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.188 )

#	-SECTION_START-( 1.189 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.189 )

#	-SECTION_START-( 1.190 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.190 )

#	-SECTION_START-( 1.191 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=81-02-03-07
#	-SECTION_END-( 1.191 )

#	-SECTION_START-( 1.192 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-17
#	-SECTION_END-( 1.192 )

#	-SECTION_START-( 1.193 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.193 )

#	-SECTION_START-( 1.194 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104

SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-07

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.194 )

#	-SECTION_START-( 1.195 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.195 )

#	-SECTION_START-( 1.196 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.196 )

#	-SECTION_START-( 1.197 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.197 )

#	-SECTION_START-( 1.198 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.198 )

#	-SECTION_START-( 1.199 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=81-02-03-08
#	-SECTION_END-( 1.199 )

#	-SECTION_START-( 1.200 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-18
#	-SECTION_END-( 1.200 )

#	-SECTION_START-( 1.201 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.201 )

#	-SECTION_START-( 1.202 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104

SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-08

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.202 )

#	-SECTION_START-( 1.203 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.203 )

#	-SECTION_START-( 1.204 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.204 )

#	-SECTION_START-( 1.205 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.205 )

#	-SECTION_START-( 1.206 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.206 )

#	-SECTION_START-( 1.207 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=81-02-03-09
#	-SECTION_END-( 1.207 )

#	-SECTION_START-( 1.208 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-19
#	-SECTION_END-( 1.208 )

#	-SECTION_START-( 1.209 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.209 )

#	-SECTION_START-( 1.210 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104

SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-09

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.210 )

#	-SECTION_START-( 1.211 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.211 )

#	-SECTION_START-( 1.212 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.212 )

#	-SECTION_START-( 1.213 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.213 )

#	-SECTION_START-( 1.214 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.214 )

#	-SECTION_START-( 1.215 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=81-02-03-0A
#	-SECTION_END-( 1.215 )

#	-SECTION_START-( 1.216 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-1A
#	-SECTION_END-( 1.216 )

#	-SECTION_START-( 1.217 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.217 )

#	-SECTION_START-( 1.218 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104

SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-0A

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.218 )

#	-SECTION_START-( 1.219 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.219 )

#	-SECTION_START-( 1.220 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.220 )

#	-SECTION_START-( 1.221 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.221 )

#	-SECTION_START-( 1.222 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.222 )

#	-SECTION_START-( 1.223 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=81-02-03-0B
#	-SECTION_END-( 1.223 )

#	-SECTION_START-( 1.224 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-1B
#	-SECTION_END-( 1.224 )

#	-SECTION_START-( 1.225 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.225 )

#	-SECTION_START-( 1.226 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104

SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-0B

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.226 )

#	-SECTION_START-( 1.227 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.227 )

#	-SECTION_START-( 1.228 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.228 )

#	-SECTION_START-( 1.229 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.229 )

#	-SECTION_START-( 1.230 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.230 )

#	-SECTION_START-( 1.231 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=81-02-03-0C
#	-SECTION_END-( 1.231 )

#	-SECTION_START-( 1.232 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-1C
#	-SECTION_END-( 1.232 )

#	-SECTION_START-( 1.233 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.233 )

#	-SECTION_START-( 1.234 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104

SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-0C

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.234 )

#	-SECTION_START-( 1.235 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.235 )

#	-SECTION_START-( 1.236 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.236 )

#	-SECTION_START-( 1.237 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.237 )

#	-SECTION_START-( 1.238 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.238 )

#	-SECTION_START-( 1.239 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=81-02-03-0D
#	-SECTION_END-( 1.239 )

#	-SECTION_START-( 1.240 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-1D
#	-SECTION_END-( 1.240 )

#	-SECTION_START-( 1.241 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.241 )

#	-SECTION_START-( 1.242 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104

SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-0D

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.242 )

#	-SECTION_START-( 1.243 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.243 )

#	-SECTION_START-( 1.244 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.244 )

#	-SECTION_START-( 1.245 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.245 )

#	-SECTION_START-( 1.246 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.246 )

#	-SECTION_START-( 1.247 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=81-02-03-0E
#	-SECTION_END-( 1.247 )

#	-SECTION_START-( 1.248 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-1E
#	-SECTION_END-( 1.248 )

#	-SECTION_START-( 1.249 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.249 )

#	-SECTION_START-( 1.250 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104

SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-0E

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.250 )

#	-SECTION_START-( 1.251 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.251 )

#	-SECTION_START-( 1.252 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.252 )

#	-SECTION_START-( 1.253 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.253 )

#	-SECTION_START-( 1.254 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.254 )

#	-SECTION_START-( 1.255 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=81-02-03-0F
#	-SECTION_END-( 1.255 )

#	-SECTION_START-( 1.256 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-1F
#	-SECTION_END-( 1.256 )

#	-SECTION_START-( 1.257 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.257 )

#	-SECTION_START-( 1.258 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104

SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=81-02-03-0F

QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.258 )

#	-SECTION_START-( 1.259 )
SetGroupAddress                                 +
    OpenInstance=2                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.259 )

#	-SECTION_START-( 1.260 )
SetGroupAddress                                 +
    OpenInstance=1                              +
    GroupAddress=00-00-00-00
#	-SECTION_END-( 1.260 )

#	-SECTION_START-( 1.261 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.261 )

#	-SECTION_START-( 1.262 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_802_5_CURRENT_GROUP                # 0x2010104
#	-SECTION_END-( 1.262 )

########################################################

# finally close the adapter and dump the Event Queue for
# any unexpected events.

#	-SECTION_START-( 1.263 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 1.263 )

#	-SECTION_START-( 1.264 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 1.264 )

# finally dump the Event Queue for any unexpected events.

#	-SECTION_START-( 1.265 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 1.265 )

#	-SECTION_START-( 1.266 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 1.266 )

#	-SECTION_END-( 1.00 )

#       -SECTION_START-( 1.0 ) -SECTION_DESC-( "Add Multicast Address Tests" )
##
## TITLE: 1.1.1.1115_FD Add Multicast Address Tests (1M/1C/10)
##
## 1 Machine - 1 Card - 1 Open Instance on the Card.  These tests
## will verify the ability to add and delete multicast address from
## list on the adapter.

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
    OID=OID_FDDI_LONG_MULTICAST_LIST                # 0x03010103

AddMulticastAddress                             +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-06

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_FDDI_LONG_MULTICAST_LIST                # 0x03010103

DeleteMulticastAddress                          +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-06

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_FDDI_LONG_MULTICAST_LIST                # 0x03010103

AddMulticastAddress                             +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-06

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_FDDI_LONG_MULTICAST_LIST                # 0x03010103

DeleteMulticastAddress                          +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-06

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_FDDI_LONG_MULTICAST_LIST                # 0x03010103

AddMulticastAddress                             +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-06

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_FDDI_LONG_MULTICAST_LIST                # 0x03010103

DeleteMulticastAddress                          +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-06

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_FDDI_LONG_MULTICAST_LIST                # 0x03010103

AddMulticastAddress                             +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-06

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_FDDI_LONG_MULTICAST_LIST                # 0x03010103

DeleteMulticastAddress                          +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-06

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_FDDI_LONG_MULTICAST_LIST                # 0x03010103

AddMulticastAddress                             +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-06

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_FDDI_LONG_MULTICAST_LIST                # 0x03010103

DeleteMulticastAddress                          +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-06

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_FDDI_LONG_MULTICAST_LIST                # 0x03010103

AddMulticastAddress                             +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-06

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_FDDI_LONG_MULTICAST_LIST                # 0x03010103

DeleteMulticastAddress                          +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-06

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_FDDI_LONG_MULTICAST_LIST                # 0x03010103

AddMulticastAddress                             +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-06

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_FDDI_LONG_MULTICAST_LIST                # 0x03010103

DeleteMulticastAddress                          +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-06

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_FDDI_LONG_MULTICAST_LIST                # 0x03010103

AddMulticastAddress                             +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-06

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_FDDI_LONG_MULTICAST_LIST                # 0x03010103

DeleteMulticastAddress                          +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-06

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_FDDI_LONG_MULTICAST_LIST                # 0x03010103

AddMulticastAddress                             +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-06

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_FDDI_LONG_MULTICAST_LIST                # 0x03010103

DeleteMulticastAddress                          +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-06

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_FDDI_LONG_MULTICAST_LIST                # 0x03010103

AddMulticastAddress                             +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-06

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_FDDI_LONG_MULTICAST_LIST                # 0x03010103

DeleteMulticastAddress                          +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-06

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_FDDI_LONG_MULTICAST_LIST                # 0x03010103

AddMulticastAddress                             +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-06

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_FDDI_LONG_MULTICAST_LIST                # 0x03010103

DeleteMulticastAddress                          +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-06

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_FDDI_LONG_MULTICAST_LIST                # 0x03010103

AddMulticastAddress                             +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-06

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_FDDI_LONG_MULTICAST_LIST                # 0x03010103

DeleteMulticastAddress                          +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-06

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_FDDI_LONG_MULTICAST_LIST                # 0x03010103

AddMulticastAddress                             +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-06

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_FDDI_LONG_MULTICAST_LIST                # 0x03010103

DeleteMulticastAddress                          +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-06

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_FDDI_LONG_MULTICAST_LIST                # 0x03010103

AddMulticastAddress                             +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-06

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_FDDI_LONG_MULTICAST_LIST                # 0x03010103

DeleteMulticastAddress                          +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-06

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_FDDI_LONG_MULTICAST_LIST                # 0x03010103

AddMulticastAddress                             +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-06

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_FDDI_LONG_MULTICAST_LIST                # 0x03010103

DeleteMulticastAddress                          +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-06

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_FDDI_LONG_MULTICAST_LIST                # 0x03010103

AddMulticastAddress                             +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-06

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_FDDI_LONG_MULTICAST_LIST                # 0x03010103

DeleteMulticastAddress                          +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-06

QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_FDDI_LONG_MULTICAST_LIST                # 0x03010103
#   -SECTION_END-  ( 1.03 )

########################################################

# Now Add multiple multicast address

#   -SECTION_START-( 1.04 )
AddMulticastAddress                             +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-00

AddMulticastAddress                             +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-01

AddMulticastAddress                             +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-02

AddMulticastAddress                             +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-03

AddMulticastAddress                             +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-04

AddMulticastAddress                             +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-05

AddMulticastAddress                             +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-06

AddMulticastAddress                             +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-07

AddMulticastAddress                             +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-08

AddMulticastAddress                             +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-09
#   -SECTION_END-( 1.04 )

#   -SECTION_START-( 1.05 ) -TOKEN_MATCH-( 4,0 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_FDDI_LONG_MULTICAST_LIST                # 0x03010103
#   -SECTION_END-( 1.05 )

#   -SECTION_START-( 1.06 )
DeleteMulticastAddress                          +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-00

DeleteMulticastAddress                          +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-01

DeleteMulticastAddress                          +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-02

DeleteMulticastAddress                          +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-03

DeleteMulticastAddress                          +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-04

DeleteMulticastAddress                          +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-05

DeleteMulticastAddress                          +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-06

DeleteMulticastAddress                          +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-07

DeleteMulticastAddress                          +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-08

DeleteMulticastAddress                          +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-09
#   -SECTION_END-( 1.06 )

#   -SECTION_START-( 1.07 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_FDDI_LONG_MULTICAST_LIST                # 0x03010103
#   -SECTION_END-  ( 1.07 )

########################################################

# finally close the adapter and dump the Event Queue for
# any unexpected events.

#   -SECTION_START-( 1.08 )
Close                                           +
    OpenInstance=1
#   -SECTION_END-  ( 1.08 )

# finally dump the Event Queue for any unexpected events.

#   -SECTION_START-( 1.09 )
GetEvents                                       +
    OpenInstance=1
#   -SECTION_END-  ( 1.09 )

#   -SECTION_END-  ( 1.0 )

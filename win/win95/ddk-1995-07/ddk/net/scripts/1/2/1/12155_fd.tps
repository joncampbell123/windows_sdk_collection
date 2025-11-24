########################################################

# Now Add multiple multicast address

#	-SECTION_START-( 1.135 )
AddMulticastAddress                             +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-00
#	-SECTION_END-( 1.135 )

#	-SECTION_START-( 1.136 )
AddMulticastAddress                             +
    OpenInstance=2                              +
    MulticastAddress=11-02-03-04-05-00
#	-SECTION_END-( 1.136 )

#	-SECTION_START-( 1.137 )
AddMulticastAddress                             +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-01
#	-SECTION_END-( 1.137 )

#	-SECTION_START-( 1.138 )
AddMulticastAddress                             +
    OpenInstance=2                              +
    MulticastAddress=11-02-03-04-05-01
#	-SECTION_END-( 1.138 )

#	-SECTION_START-( 1.139 )
AddMulticastAddress                             +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-02
#	-SECTION_END-( 1.139 )

#	-SECTION_START-( 1.140 )
AddMulticastAddress                             +
    OpenInstance=2                              +
    MulticastAddress=11-02-03-04-05-02
#	-SECTION_END-( 1.140 )

#	-SECTION_START-( 1.141 )
AddMulticastAddress                             +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-03
#	-SECTION_END-( 1.141 )

#	-SECTION_START-( 1.142 )
AddMulticastAddress                             +
    OpenInstance=2                              +
    MulticastAddress=11-02-03-04-05-03
#	-SECTION_END-( 1.142 )

#	-SECTION_START-( 1.143 )
AddMulticastAddress                             +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-04
#	-SECTION_END-( 1.143 )

#	-SECTION_START-( 1.144 )
AddMulticastAddress                             +
    OpenInstance=2                              +
    MulticastAddress=11-02-03-04-05-04
#	-SECTION_END-( 1.144 )

#	-SECTION_START-( 1.145 )
AddMulticastAddress                             +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-05
#	-SECTION_END-( 1.145 )

#	-SECTION_START-( 1.146 )
AddMulticastAddress                             +
    OpenInstance=2                              +
    MulticastAddress=11-02-03-04-05-05
#	-SECTION_END-( 1.146 )

#	-SECTION_START-( 1.147 )
AddMulticastAddress                             +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-06
#	-SECTION_END-( 1.147 )

#	-SECTION_START-( 1.148 )
AddMulticastAddress                             +
    OpenInstance=2                              +
    MulticastAddress=11-02-03-04-05-06
#	-SECTION_END-( 1.148 )

#	-SECTION_START-( 1.149 ) -TOKEN_MATCH-( 4,0 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_FDDI_LONG_MULTICAST_LIST                # 0x03010103
#	-SECTION_END-( 1.149 )

#	-SECTION_START-( 1.150 ) -TOKEN_MATCH-( 4,0 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_FDDI_LONG_MULTICAST_LIST                # 0x03010103
#	-SECTION_END-( 1.150 )

#	-SECTION_START-( 1.151 )
DeleteMulticastAddress                          +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-00
#	-SECTION_END-( 1.151 )

#	-SECTION_START-( 1.152 )
DeleteMulticastAddress                          +
    OpenInstance=2                              +
    MulticastAddress=11-02-03-04-05-00
#	-SECTION_END-( 1.152 )

#	-SECTION_START-( 1.153 )
DeleteMulticastAddress                          +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-01
#	-SECTION_END-( 1.153 )

#	-SECTION_START-( 1.154 )
DeleteMulticastAddress                          +
    OpenInstance=2                              +
    MulticastAddress=11-02-03-04-05-01
#	-SECTION_END-( 1.154 )

#	-SECTION_START-( 1.155 )
DeleteMulticastAddress                          +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-02
#	-SECTION_END-( 1.155 )

#	-SECTION_START-( 1.156 )
DeleteMulticastAddress                          +
    OpenInstance=2                              +
    MulticastAddress=11-02-03-04-05-02
#	-SECTION_END-( 1.156 )

#	-SECTION_START-( 1.157 )
DeleteMulticastAddress                          +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-03
#	-SECTION_END-( 1.157 )

#	-SECTION_START-( 1.158 )
DeleteMulticastAddress                          +
    OpenInstance=2                              +
    MulticastAddress=11-02-03-04-05-03
#	-SECTION_END-( 1.158 )

#	-SECTION_START-( 1.159 )
DeleteMulticastAddress                          +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-04
#	-SECTION_END-( 1.159 )

#	-SECTION_START-( 1.160 )
DeleteMulticastAddress                          +
    OpenInstance=2                              +
    MulticastAddress=11-02-03-04-05-04
#	-SECTION_END-( 1.160 )

#	-SECTION_START-( 1.161 )
DeleteMulticastAddress                          +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-05
#	-SECTION_END-( 1.161 )

#	-SECTION_START-( 1.162 )
DeleteMulticastAddress                          +
    OpenInstance=2                              +
    MulticastAddress=11-02-03-04-05-05
#	-SECTION_END-( 1.162 )

#	-SECTION_START-( 1.163 )
DeleteMulticastAddress                          +
    OpenInstance=1                              +
    MulticastAddress=01-02-03-04-05-06
#	-SECTION_END-( 1.163 )

#	-SECTION_START-( 1.164 )
DeleteMulticastAddress                          +
    OpenInstance=2                              +
    MulticastAddress=11-02-03-04-05-06
#	-SECTION_END-( 1.164 )

#	-SECTION_START-( 1.165 )
QueryInformation                                +
    OpenInstance=1                              +
    OID=OID_FDDI_LONG_MULTICAST_LIST                # 0x03010103
#	-SECTION_END-( 1.165 )

#	-SECTION_START-( 1.166 )
QueryInformation                                +
    OpenInstance=2                              +
    OID=OID_FDDI_LONG_MULTICAST_LIST                # 0x03010103
#	-SECTION_END-( 1.166 )



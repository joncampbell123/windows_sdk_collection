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



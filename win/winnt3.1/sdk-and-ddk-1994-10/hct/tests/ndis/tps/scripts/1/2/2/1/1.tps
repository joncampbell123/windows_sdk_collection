#	-SECTION_START-( 1.00 ) -SECTION_DESC-( "Send Directed" )
##
## TITLE: 1.2.2.1.1 Send Directed (1M/1C/2O)
##
## 1 Machine - 1 Card - 2 Open Instances on the card.  These tests
## will have the test card send 10 DIRECTED packets to a random
## directed network address to verify that packets may be sent.
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

# then open the card.

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

# Send 40 byte packets.

#	-SECTION_START-( 1.05 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_RANDOM_ADDRESS%      +
    PacketSize=40                               +
    Number=10
#	-SECTION_END-( 1.05 )

#	-SECTION_START-( 1.06 )
Send                                            +
    OpenInstance=2                              +
    DestinationAddress=%TP_RANDOM_ADDRESS%      +
    PacketSize=40                               +
    Number=10
#	-SECTION_END-( 1.06 )

#	-SECTION_START-( 1.07 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 1.07 )

#	-SECTION_START-( 1.08 )
WaitSend                                        +
    OpenInstance=2
#	-SECTION_END-( 1.08 )

########################################################

# Send 512 byte packets.

#	-SECTION_START-( 1.09 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_RANDOM_ADDRESS%      +
    PacketSize=512                              +
    Number=10
#	-SECTION_END-( 1.09 )

#	-SECTION_START-( 1.10 )
Send                                            +
    OpenInstance=2                              +
    DestinationAddress=%TP_RANDOM_ADDRESS%      +
    PacketSize=512                              +
    Number=10
#	-SECTION_END-( 1.10 )

#	-SECTION_START-( 1.11 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 1.11 )

#	-SECTION_START-( 1.12 )
WaitSend                                        +
    OpenInstance=2
#	-SECTION_END-( 1.12 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#	-SECTION_START-( 1.13 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_RANDOM_ADDRESS%      +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10
#	-SECTION_END-( 1.13 )

#	-SECTION_START-( 1.14 )
Send                                            +
    OpenInstance=2                              +
    DestinationAddress=%TP_RANDOM_ADDRESS%      +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10
#	-SECTION_END-( 1.14 )

#	-SECTION_START-( 1.15 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 1.15 )

#	-SECTION_START-( 1.16 )
WaitSend                                        +
    OpenInstance=2
#	-SECTION_END-( 1.16 )

########################################################

# Finally close the adapter, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 1.17 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 1.17 )

#	-SECTION_START-( 1.18 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 1.18 )

#	-SECTION_START-( 1.19 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 1.19 )

#	-SECTION_START-( 1.20 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 1.20 )

#	-SECTION_END-( 1.00 )

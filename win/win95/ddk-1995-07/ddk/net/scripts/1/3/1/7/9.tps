#	-SECTION_START-( 7.00 ) -SECTION_DESC-( "Send Broadcast, Receive Broadcast & Mac Frame" ) -OPTIONALS-( MACFRAME )
##
## TITLE: 1.3.1.7.9 Send Broadcast, Receive Broadcast & Mac Frame (1M/2C/1O)
##
## 1 Machine - 2 Cards - 1 Open Instance on each card.  These tests
## will have a trusted card send 10 BROADCAST packets to the card
## being tested.  The card being tested will have its packetfilter
## set to receive BROADCAST and MACFRAME packets.  Packets of size
## 40 bytes, 512 bytes, and MAX_FRAME_SIZE bytes will be sent.
##
## All packets should be received by the test card.
##

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#	-SECTION_START-( 7.01 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 7.01 )

#	-SECTION_START-( 7.02 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 7.02 )

# And open and setup the cards.

#	-SECTION_START-( 7.03 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TRUSTED_CARD%
#	-SECTION_END-( 7.03 )

#	-SECTION_START-( 7.04 )
Open                                            +
    OpenInstance=2                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 7.04 )

#	-SECTION_START-( 7.05 )
SetPacketFilter                                 +
    OpenInstance=2                              +
    PacketFilter=BROADCAST|MACFRAME
#	-SECTION_END-( 7.05 )

# Now set the test card to receive packets,

#	-SECTION_START-( 7.06 )
Receive                                         +
    OpenInstance=2
#	-SECTION_END-( 7.06 )

########################################################

# Send 40 byte packets.

#	-SECTION_START-( 7.07 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_BROADCAST_ADDRESS%   +
    PacketSize=40                               +
    Number=10
#	-SECTION_END-( 7.07 )

#	-SECTION_START-( 7.08 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 7.08 )

########################################################

# Send 512 byte packets.

#	-SECTION_START-( 7.09 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_BROADCAST_ADDRESS%   +
    PacketSize=512                              +
    Number=10
#	-SECTION_END-( 7.09 )

#	-SECTION_START-( 7.10 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 7.10 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#	-SECTION_START-( 7.11 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_BROADCAST_ADDRESS%   +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10
#	-SECTION_END-( 7.11 )

#	-SECTION_START-( 7.12 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 7.12 )

########################################################

# Then stop the receive and dump the statistics.

#	-SECTION_START-( 7.13 )
StopReceive                                     +
    OpenInstance=2
#	-SECTION_END-( 7.13 )

########################################################

# Finally close the adapters, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 7.14 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 7.14 )

#	-SECTION_START-( 7.15 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 7.15 )

#	-SECTION_START-( 7.16 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 7.16 )

#	-SECTION_START-( 7.17 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 7.17 )

#	-SECTION_END-( 7.00 )

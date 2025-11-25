#   -SECTION_START-( 1.0 ) -SECTION_DESC-( "Send Directed, Receive Directed Broadcast & Functional" )
##
## TITLE: 1.1.2.6.2 Send Directed, Receive Directed Broadcast & Functional (1M/1C/1O)
##
## 1 Machine - 1 Card - 1 Open Instance on the card.  These tests
## will have the test card send 10 DIRECTED packets to itself on the
## same open on the test card.  The test card will have its packetfilter
## set to receive DIRECTED BROADCAST and FUNCTIONAL packets.  Packets
## of size 40 bytes, 512 bytes, and MAX_FRAME_SIZE bytes will be sent.
##
## No packets should be received by the test card.
##

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#   -SECTION_START-( 1.01 )
GetEvents                                       +
    OpenInstance=1
#   -SECTION_END-  ( 1.01 )

# And open and setup the card.

#   -SECTION_START-( 1.02 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TEST_CARD%
#   -SECTION_END-  ( 1.02 )

#   -SECTION_START-( 1.03 )
SetPacketFilter                                 +
    OpenInstance=1                              +
    PacketFilter=DIRECTED|BROADCAST|FUNCTIONAL
#   -SECTION_END-  ( 1.03 )

# Now set the test card to receive packets,

#   -SECTION_START-( 1.04 )
Receive                                         +
    OpenInstance=1
#   -SECTION_END-  ( 1.04 )

########################################################

# Send 40 byte packets.

#   -SECTION_START-( 1.05 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_TEST_CARD_ADDRESS%   +
    PacketSize=40                               +
    Number=10
#   -SECTION_END-  ( 1.05 )

#   -SECTION_START-( 1.06 )
WaitSend                                        +
    OpenInstance=1
#   -SECTION_END-  ( 1.06 )

########################################################

# Send 512 byte packets.

#   -SECTION_START-( 1.07 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_TEST_CARD_ADDRESS%   +
    PacketSize=512                              +
    Number=10
#   -SECTION_END-  ( 1.07 )

#   -SECTION_START-( 1.08 )
WaitSend                                        +
    OpenInstance=1
#   -SECTION_END-  ( 1.08 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#   -SECTION_START-( 1.09 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=%TP_TEST_CARD_ADDRESS%   +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10
#   -SECTION_END-  ( 1.09 )

#   -SECTION_START-( 1.10 )
WaitSend                                        +
    OpenInstance=1
#   -SECTION_END-  ( 1.10 )

########################################################

# Then stop the receive and dump the statistics.

#   -SECTION_START-( 1.11 )
StopReceive                                     +
    OpenInstance=1
#   -SECTION_END-  ( 1.11 )

########################################################

# Finally close the adapter, and again dump the Event
# Queue for any unexpected events.

#   -SECTION_START-( 1.12 )
Close                                           +
    OpenInstance=1
#   -SECTION_END-  ( 1.12 )

#   -SECTION_START-( 1.13 )
GetEvents                                       +
    OpenInstance=1
#   -SECTION_END-  ( 1.13 )

#   -SECTION_END-  ( 1.0 )

#	-SECTION_START-( 6.00 ) -SECTION_DESC-( "Send Functional, Receive Source Routing" ) -OPTIONALS-( SOURCEROUTING )
##
## TITLE: 1.3.1.5.6 Send Functional, Receive Source Routing (1M/2C/1O)
##
## 1 Machine - 2 Cards - 1 Open Instance on each card.  These tests
## will have a trusted card send 10 FUNCTIONAL packets to the card
## being tested.  The card being tested will have its packetfilter
## set to receive SOURCEROUTING packets.  Packets of size 40 bytes,
## 512 bytes, and MAX_FRAME_SIZE bytes will be sent.
##
## No packets should be received by the test card.
##

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#	-SECTION_START-( 6.01 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 6.01 )

#	-SECTION_START-( 6.02 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 6.02 )

# And open and setup the cards.

#	-SECTION_START-( 6.03 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TRUSTED_CARD%
#	-SECTION_END-( 6.03 )

#	-SECTION_START-( 6.04 )
Open                                            +
    OpenInstance=2                              +
    AdapterName=%TP_TEST_CARD%
#	-SECTION_END-( 6.04 )

#	-SECTION_START-( 6.05 )
SetPacketFilter                                 +
    OpenInstance=2                              +
    PacketFilter=SOURCEROUTING
#	-SECTION_END-( 6.05 )

# Now set the test card to receive packets,

#	-SECTION_START-( 6.06 )
Receive                                         +
    OpenInstance=2
#	-SECTION_END-( 6.06 )

########################################################

# Send 40 byte packets.

#	-SECTION_START-( 6.07 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=40                               +
    Number=10
#	-SECTION_END-( 6.07 )

#	-SECTION_START-( 6.08 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 6.08 )

########################################################

# Send 512 byte packets.

#	-SECTION_START-( 6.09 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=512                              +
    Number=10
#	-SECTION_END-( 6.09 )

#	-SECTION_START-( 6.10 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 6.10 )

########################################################

# Send "MAXIMUM FRAME SIZE" byte packets.

#	-SECTION_START-( 6.11 )
Send                                            +
    OpenInstance=1                              +
    DestinationAddress=C0-00-%TP_FUNCTIONAL_ADDRESS% +
    PacketSize=%TP_MAX_FRAME_SIZE%              +
    Number=10
#	-SECTION_END-( 6.11 )

#	-SECTION_START-( 6.12 )
WaitSend                                        +
    OpenInstance=1
#	-SECTION_END-( 6.12 )

########################################################

# Then stop the receive and dump the statistics.

#	-SECTION_START-( 6.13 )
StopReceive                                     +
    OpenInstance=2
#	-SECTION_END-( 6.13 )

########################################################

# Finally close the adapters, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 6.14 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 6.14 )

#	-SECTION_START-( 6.15 )
Close                                           +
    OpenInstance=2
#	-SECTION_END-( 6.15 )

#	-SECTION_START-( 6.16 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 6.16 )

#	-SECTION_START-( 6.17 )
GetEvents                                       +
    OpenInstance=2
#	-SECTION_END-( 6.17 )

#	-SECTION_END-( 6.00 )

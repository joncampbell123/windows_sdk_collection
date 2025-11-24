#	-SECTION_START-( 3.00 ) -SECTION_DESC-( "Stress Tests" )
##
## TITLE: 2.1.2.2.3 Stress Tests (1M/2C/1O)
##
## 1 Machine - 2 Card - 1 Open Instance on the card.  These tests
## will open the test card once and the trusted card once and run
## run a Stress client on the trusted card and a Stress server on the
## test card.

# First dump any outstanding unexpected events to clear
# the event queue.  This "should" be empty here.

#	-SECTION_START-( 3.01 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 3.01 )

# Then open the card.

#	-SECTION_START-( 3.02 )
Open                                            +
    OpenInstance=1                              +
    AdapterName=%TP_TRUSTED_CARD%
#	-SECTION_END-( 3.02 )

########################################################

# Then start the Stress Server,

#	-SECTION_START-( 3.03 )
StressServer                                    +
     OpenInstance=1
#	-SECTION_END-( 3.03 )

# And tell the client we are ready to act as a Stress server.

#	-SECTION_START-( 3.04 )
Go                                              +
    OpenInstance=1                              +
    RemoteAddress=%TP_REM_TEST_CARD_ADDRESS%    +
    TestSignature=212231
#	-SECTION_END-( 3.04 )

########################################################

# Then wait for the stress test to end,

#	-SECTION_START-( 3.05 )
Pause                                           +
    OpenInstance=1                              +
    RemoteAddress=%TP_REM_TEST_CARD_ADDRESS%    +
    TestSignature=212232
#	-SECTION_END-( 3.05 )

# And stop the Stress Server

#	-SECTION_START-( 3.06 )
EndStress                                       +
    OpenInstance=1
#	-SECTION_END-( 3.06 )

########################################################

# Finally close the adapter, and again dump the Event
# Queue for any unexpected events.

#	-SECTION_START-( 3.07 )
GetEvents                                       +
    OpenInstance=1
#	-SECTION_END-( 3.07 )

#	-SECTION_START-( 3.08 )
Close                                           +
    OpenInstance=1
#	-SECTION_END-( 3.08 )

#	-SECTION_END-( 3.00 )

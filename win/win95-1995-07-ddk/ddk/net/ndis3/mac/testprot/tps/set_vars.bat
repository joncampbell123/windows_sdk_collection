@echo off
rem Set the name of a local "TEST" card
rem
rem example: set TP_TEST_CARD=ms$testcard
rem
set TP_TEST_CARD=

echo setting vars for %tp_test_card%

rem Set the "TEST" card address
rem
rem Format XX-XX-XX-XX-XX-XX
rem
rem example: set TP_TEST_CARD_ADDRESS=01-23-45-67-89-AC
rem
set TP_TEST_CARD_ADDRESS=

rem Set the name of a local "TRUSTED" card
rem
rem example: set TP_TRUSTED_CARD=ms$trustedcard
rem
set TP_TRUSTED_CARD=

rem Set the "TRUSTED" card address
rem
rem Format XX-XX-XX-XX-XX-XX
rem
rem example: set TP_TRUSTED_CARD_ADDRESS=01-23-45-67-89-AC
rem
set TP_TRUSTED_CARD_ADDRESS=

rem For ethernet or fiber media set two multicast address to be
rem used during the tests.  These address must match those
rem being used on both the test machine and the server-side
rem machine for the two machine tests.
rem
rem Format XX-XX-XX-XX-XX-XX
rem
rem Note: The Least Significant Bit in the first byte of the
rem       address must be set to 1.
rem
rem example: set TP_MULTICAST_ADDRESS=01-02-03-04-05-00
rem
rem set TP_MULTICAST_ADDRESS=07-07-07-07-07-07
rem set TP_MULTICAST_ADDRESS2=07-07-07-07-07-08

rem For tokenring media set two functional address to be
rem used during the tests.  These address must match those
rem being used on both the test machine and the server-side
rem machine for the two machine tests.  Only the last 4 bytes
rem are required/allowed.
rem
rem Format XX-XX-XX-XX
rem
rem Note: The Most Significant Bit in the Most Significant Byte
rem       for the functional address inidicator must be set to 0
rem       Further, please note that the second functional addres
rem       must not be an inclusive OR of the first address. The
rem       second functional address if anded bitwise with the first
rem       must result in 0
rem
rem example: set TP_FUNCTIONAL_ADDRESS=0C-02-03-04
rem          set TP_FUNCTIONAL_ADDRESS=00-00-00-40
rem
rem set TP_FUNCTIONAL_ADDRESS=0C-02-03-04
rem set TP_FUNCTIONAL_ADDRESS2=00-00-00-40

rem unREMark a broadcast address for the selected media to be
rem used during the tests.
rem
rem Format XX-XX-XX-XX-XX-XX
rem
rem For ethernet use: FF-FF-FF-FF-FF-FF
rem
rem set TP_BROADCAST_ADDRESS=FF-FF-FF-FF-FF-FF
rem
rem For tokenring use: C0-00-FF-FF-FF-FF
rem
rem set TP_BROADCAST_ADDRESS=C0-00-FF-FF-FF-FF

rem Set a random unique address to be used during the tests.
rem
rem Format XX-XX-XX-XX-XX-XX
rem
rem example ethernet  use: 00-23-45-67-89-AC
rem                         
rem                         The two LSB's are zero
rem
rem example tokenring use: 00-01-02-03-04-05
rem                        
rem                        The two MSB's are zero
rem
rem set TP_RANDOM_ADDRESS=

rem Set the remote address of the opposing test or trusted card
rem
rem Format XX-XX-XX-XX-XX-XX
rem
rem On the server-side machine set the remote TEST cards address
rem
rem set TP_REM_TEST_CARD_ADDRESS=
rem
rem On the test machine set the remote TRUSTED or server-side cards
rem address
rem
rem set TP_REM_TRUSTED_CARD_ADDRESS=

rem Set the Maximum frame size for the "test" card
rem
rem This can be found by querying information from the adapter for
rem the oid OID_GEN_MAXIMUM_TOTAL_SIZE.
rem
rem PLEASE NOTE : DO NOT SET THIS SIZE TO OID_GEN_MAXIMUM_FRAME_SIZE
rem               AS THIS VARIABLE REFLECTS THE TRUE MAXIMUM PACKET
rem               SIZE WHICH IS THE HEADER AND DATA PORTIONS
rem
rem set TP_MAX_FRAME_SIZE=

rem Set the Maximum Lookahead size the adapter will support.
rem
rem This can be found by querying information from the adapter for
rem the oid OID_GEN_MAXIMUM_LOOKAHEAD
rem
set TP_MAX_LOOKAHEAD_SIZE=256

rem
rem Set or unset the Environment variables for the difference file to use
rem If your card does not support that particular filter capability, simply
rem remove the 1 in front of the environment variable
rem
set PROMISCUOUS=1
set MACFRAME=1
set SOURCEROUTING=1
set ALLFUNCTIONAL=1
set ALLMULTICAST=1

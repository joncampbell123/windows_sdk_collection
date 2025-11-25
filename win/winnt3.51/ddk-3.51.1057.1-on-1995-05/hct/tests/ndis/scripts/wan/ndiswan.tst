#------------------------------------------
# File:  ndiswan.tst
#
# This is the master NdisWAN Test Tool script.  It calls all the
# functional scripts and stress scripts
#
# Script globals are defined here.  Whenever they are set
# as the result of a command, they must be enclosed in the
# "global" "end_global" pair, or else the script might create
# a local variable with the same name.
#
# Adapted in most part from ndis.tst
#-------------------------------------------

variation

global
#--------------------------------------
# Flag to indicate strict reporting for WAN cases
#--------------------------------------
$G_Strict = 0

#--------------------------------------
# the following global variables describe the test card
#--------------------------------------
$G_TestCard = "bogus"
$G_MesgCard = "bogus"
$G_Media = 0
$G_Filters = 0
$G_MaxLookahead = 0
$G_MaxTotalSize = 0
$G_TestAddress = array uchar 6
$G_TestAddress[0] = (uchar) 0x00
$G_TestAddress[1] = (uchar) 0x00
$G_TestAddress[2] = (uchar) 0x00
$G_TestAddress[3] = (uchar) 0x00
$G_TestAddress[4] = (uchar) 0x00
$G_TestAddress[5] = (uchar) 0x00

#--------------------------------------
# this global variable is only used for arcnet cards.
# it determines whether it is opened in Arcnet878_2 mode (if FALSE)
# or encapsulated ethernet mode (if TRUE)
#--------------------------------------
$G_OpenFlag = TRUE

#--------------------------------------
# the following global variables describe the trusted card
#--------------------------------------
$G_TrustedCard = "bogus"
$G_TrustedTotalSize = 0
$G_TrustedAddress = array uchar 6
$G_TrustedAddress[0] = (uchar) 0x00
$G_TrustedAddress[1] = (uchar) 0x00
$G_TrustedAddress[2] = (uchar) 0x00
$G_TrustedAddress[3] = (uchar) 0x00
$G_TrustedAddress[4] = (uchar) 0x00
$G_TrustedAddress[5] = (uchar) 0x00

#--------------------------------------
# this global variable is the open instance used for sending
# commands to the server
#--------------------------------------
$G_ServerOpen = 0

#--------------------------------------
# this global variable determine the number of packets that are
# sent in all the send tests
#--------------------------------------
$G_PacketsToSend = 10

end_global

debuglevel  1

#----------------------------------------
# find test card driver, and set associated global variables
#----------------------------------------
call  scripts\getwtest

#------------------------------------------
# run 2 machine, 2 card tests -- note:  uses single server
#------------------------------------------

variation

#
# enable wan connectivity
#
enablewan client

#
# initialize the connection params
#
$G_NumWanPorts = conninit "isdn" "9,5561393" "64000" "0"

global
$G_ServerOpen = openserver $G_MesgCard $G_OpenFlag
end_global

if ($G_ServerOpen != 0)
{
   call scripts\initwcrd.tst     # initialize the wan card - replaces gettest.tst

   global
   $ClientOpen = open $G_TestCard $G_OpenFlag
   end_global

   if ($ClientOpen == 0)
   {
      block_variation
      print "BLOCKED:  unable to open test card"
      goto endvar0
   }

   call scripts\2m2c_01.tst         # send directed/receive various tests..
   call scripts\2m2c_02.tst         # send broadcast/receive various tests...

   if ( ($G_Filters & MULTICAST) != NONE)
   {
      call scripts\2m2c_03a.tst     # send multicast/receive various tests..
   }
   elseif ( ($G_Filters & FUNCTIONAL) != NONE)
   {
      call scripts\2m2c_03b.tst     # send functional/receive various tests..
      call scripts\2m2c_03c.tst     # send group/receive various tests..
   }
   call scripts\2m2c_04.tst         # send random/receive various tests..

   call scripts\2m2c_05.tst         # send directed/receive/resend tests
   call scripts\2m2c_06.tst         # send broadcast/receive/resend tests

   if ( ($G_Filters & MULTICAST) != NONE)
   {
      call scripts\2m2c_07a.tst     # send multicast/receive/resend tests..
   }
   elseif ( ($G_Filters & FUNCTIONAL) != NONE)
   {
      call scripts\2m2c_07b.tst     # send functional/receive/resend tests..
      call scripts\2m2c_07c.tst     # send group/receive/resend tests..
   }
   #call scripts\str_2m1.tst         # 2 machine stress tests..
   #call scripts\str_2m2.tst
   #call scripts\str_2m3.tst

   closeserver $G_ServerOpen TRUE
}

#-------------------------------------------------------------------
# end of file ndiswan.tst
#-------------------------------------------------------------------



#------------------------------------------
# File:  nd2mach.tst
#
# This script does just the 2 machine portion
# of the HCT tests.  Note that it does only the
# functional and stress tests--it does NOT do the
# performance tests.
#
# Script globals are defined here.  Whenever they are set
# as the result of a command, they must be enclosed in the
# "global" "end_global" pair, or else the script might create
# a local variable with the same name.
#-------------------------------------------

variation

global

#--------------------------------------
# the following global variables describe the test card
#--------------------------------------
$G_TestCard = "bogus"
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
$G_OpenFlag = FALSE

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


debuglevel  0

#----------------------------------------
# find test and trusted card, and set associated global variables
#----------------------------------------
call scripts\gettest.tst         # get name of test card (required)
call scripts\gettrust.tst        # get name of trusted card (optional)

#------------------------------------------
# run 2 machine, 2 card tests -- note:  uses single server
#------------------------------------------

variation

global
if ($G_TrustedTotalSize != 0)    # will be non-zero ONLY if it finds a trusted card..
{
   $G_ServerOpen = openserver $G_TrustedCard $G_OpenFlag
}
else
{
   $G_ServerOpen = openserver $G_TestCard $G_OpenFlag
}
end_global

if ($G_ServerOpen != 0)
{
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
   call scripts\str_2m1.tst         # 2 machine stress tests..
   call scripts\str_2m2.tst
   call scripts\str_2m3.tst

   closeserver $G_ServerOpen TRUE
}

#---------------------------------------------------------------
# if ethernet, tokenring, fddi, are done now
# if arcnet, we just finished with arcnet878_2 mode, now
# need to do encapsulated ethernet...
#---------------------------------------------------------------

if ($G_OpenFlag == FALSE)
{
   variation

   global
   $G_OpenFlag = TRUE
   end_global

   call scripts\gettest.tst
   call scripts\gettrust.tst        # get name of trusted card (in same machine as test card)

   #------------------------------------------
   # run 2 machine, 2 card tests for encapsulated ethernet
   #------------------------------------------

   variation

   global
   if ($G_TrustedTotalSize != 0)    # will be non-zero ONLY if it finds a trusted card..
   {
      $G_ServerOpen = openserver $G_TrustedCard $G_OpenFlag
   }
   else
   {
      $G_ServerOpen = openserver $G_TestCard $G_OpenFlag
   }
   end_global

   if ($G_ServerOpen != 0)
   {
      call scripts\2m2c_01e.tst     # send directed/receive various tests..
      call scripts\2m2c_02e.tst     # send broadcast/receive various tests...
      call scripts\2m2c_03e.tst     # send multicast/receive various tests..
      call scripts\2m2c_04e.tst     # send random/receive various tests..
      call scripts\2m2c_05e.tst     # send directed/receive/resend tests
      call scripts\2m2c_06e.tst     # send broadcast/receive/resend tests
      call scripts\2m2c_07e.tst     # send multicast/receive/resend tests..
      call scripts\str_2m1e.tst     # 2 machine stress tests..
      call scripts\str_2m2e.tst
      call scripts\str_2m3e.tst

      closeserver $G_ServerOpen TRUE
   }
}

end

#-------------------------------------------------------------------
# end of file nd2mach.tst
#-------------------------------------------------------------------


#------------------------------------------
# File:  ndis.tst
#
# This is the master HCT script.  It calls all the
# functional scripts and stress scripts
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

#-------------------------------------------
# run 1 machine, 1 card, 1 open instance tests
#-------------------------------------------

call scripts\1m1c_01.tst         # open/close tests
call scripts\1m1c_02.tst         # queryinfo tests
call scripts\1m1c_03.tst         # querystats tests
call scripts\1m1c_04.tst         # setpacketfilter tests
call scripts\1m1c_05.tst         # setlookahead tests
if ( ($G_Filters & MULTICAST) != NONE)
{
   call scripts\1m1c_06a.tst     # set multicast address tests
}
elseif ( ($G_Filters & FUNCTIONAL) != NONE)
{
   call scripts\1m1c_06b.tst     # set functional/group address
}

call scripts\1m1c_07.tst         # send tests
call scripts\1m1c_08.tst         # send directed/receive various tests..
call scripts\1m1c_09.tst         # send broadcast/receive various tests...
if ( ($G_Filters & MULTICAST) != NONE)
{
   call scripts\1m1c_10a.tst     # send multicast/receive various tests..
}
elseif ( ($G_Filters & FUNCTIONAL) != NONE)
{
   call scripts\1m1c_10b.tst     # send functional/receive various tests..
   call scripts\1m1c_10c.tst     # send group/receive various tests..
}
call scripts\1m1c_11.tst         # send random/receive various tests..

call scripts\1m1c_12.tst         # send directed/receive/resend tests
call scripts\1m1c_13.tst         # send broadcast/receive/resend tests
if ( ($G_Filters & MULTICAST) != NONE)
{
   call scripts\1m1c_14a.tst     # send multicast/receive/resend tests..
}
elseif ( ($G_Filters & FUNCTIONAL) != NONE)
{
   call scripts\1m1c_14b.tst     # send functional/receive/resend tests..
   call scripts\1m1c_14c.tst     # send group/receive/resend tests..
}
if ( ($G_Filters & PROMISCUOUS) != NONE)
{
   call scripts\1m1c_15.tst      # send random/receive/resend tests..
}

call scripts\str_1c1.tst         # 1 card, 1 open stress tests

#------------------------------------------
# run 1 machine, 1 card, 2 open instances tests
#------------------------------------------

call scripts\1m2o_01.tst         # queryinfo tests
call scripts\1m2o_02.tst         # querystats tests
call scripts\1m2o_03.tst         # setpacketfilter tests
call scripts\1m2o_04.tst         # setlookahead tests
if ( ($G_Filters & MULTICAST) != NONE)
{
   call scripts\1m2o_05a.tst     # set multicast address tests
}
elseif ( ($G_Filters & FUNCTIONAL) != NONE)
{
   call scripts\1m2o_05b.tst     # set functional address tests
   call scripts\1m2o_05c.tst     # set group address tests
}

call scripts\1m2o_06.tst         # send tests
call scripts\1m2o_07.tst         # send directed/receive various tests..
call scripts\1m2o_08.tst         # send broadcast/receive various tests...
if ( ($G_Filters & MULTICAST) != NONE)
{
   call scripts\1m2o_09a.tst     # send multicast/receive various tests..
}
elseif ( ($G_Filters & FUNCTIONAL) != NONE)
{
   call scripts\1m2o_09b.tst     # send functional/receive various tests..
   call scripts\1m2o_09c.tst     # send group/receive various tests..
}
call scripts\1m2o_10.tst         # send random/receive various tests..

call scripts\1m2o_11.tst         # send directed/receive/resend tests
call scripts\1m2o_12.tst         # send broadcast/receive/resend tests
if ( ($G_Filters & MULTICAST) != NONE)
{
   call scripts\1m2o_13a.tst     # send multicast/receive/resend tests..
}
elseif ( ($G_Filters & FUNCTIONAL) != NONE)
{
   call scripts\1m2o_13b.tst     # send functional/receive/resend tests..
   call scripts\1m2o_13c.tst     # send group/receive/resend tests..
}

call scripts\str_2o1.tst         # 1 card, 2 open stress tests
call scripts\str_2o2.tst

#------------------------------------------
# run 1 machine, 2 card tests
#------------------------------------------

variation

if ($G_TrustedTotalSize != 0)    # will be non-zero ONLY if it finds a trusted card..
{                                # otherwise all these tests will be skipped
   call scripts\1m2c_01.tst      # send directed/receive various tests..
   call scripts\1m2c_02.tst      # send broadcast/receive various tests...
   if ( ($G_Filters & MULTICAST) != NONE)
   {
      call scripts\1m2c_03a.tst  # send multicast/receive various tests..
   }
   elseif ( ($G_Filters & FUNCTIONAL) != NONE)
   {
      call scripts\1m2c_03b.tst  # send functional/receive various tests..
      call scripts\1m2c_03c.tst  # send group/receive various tests..
   }
   call scripts\1m2c_04.tst      # send random/receive various tests..

   call scripts\1m2c_05.tst      # send directed/receive/resend tests
   call scripts\1m2c_06.tst      # send broadcast/receive/resend tests
   if ( ($G_Filters & MULTICAST) != NONE)
   {
      call scripts\1m2c_07a.tst  # send multicast/receive/resend tests..
      call scripts\1m2c_08a.tst  # receive with multiple multicast addresses
   }
   elseif ( ($G_Filters & FUNCTIONAL) != NONE)
   {
      call scripts\1m2c_07b.tst  # send functional/receive/resend tests..
      call scripts\1m2c_07c.tst  # send group/receive/resend tests..
   }
   call scripts\1m2c_09.tst      # rcv directed on multiple open instances
   call scripts\1m2c_10.tst      # rcv broadcast on multiple open instances
   if ( ($G_Filters & MULTICAST) != NONE)
   {
      call scripts\1m2c_11a.tst  # rcv multicast on multiple open instances
   }
   elseif ( ($G_Filters & FUNCTIONAL) != NONE)
   {
      call scripts\1m2c_11b.tst  # rcv functional on multiple open instances
      call scripts\1m2c_11c.tst  # rcv group on multiple open instances
   }
   call scripts\1m2c_12.tst      # rcv random on multiple open instances 

   call scripts\str_2c1.tst      # 1 machine, 2 card stress tests
   call scripts\str_2c2.tst
   call scripts\str_2c3.tst
}

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

#   call scripts\perfprof.tst         # performance profile test
#   call scripts\perf_dir.tst         # directed performance tests
#   call scripts\perf_brd.tst         # broadcast performance tests
#   if ( ($G_Filters & MULTICAST) != NONE)
#   {
#      call scripts\perf_mlt.tst      # multicast performance tests
#   }
#   if ( ($G_Filters & FUNCTIONAL) != NONE)
#   {
#      call scripts\perf_fnc.tst      # functional performance tests
#   }
#   if ( ($G_Filters & PROMISCUOUS) != NONE)
#   {
#      call scripts\perf_pro.tst      # promiscous performance tests
#   }

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

   #-------------------------------------------
   # run 1 machine, 1 card, 1 open instance tests for encapsulated ethernet
   #-------------------------------------------

   call scripts\1m1c_01e.tst        # open/close tests
   call scripts\1m1c_02e.tst        # queryinfo tests
   call scripts\1m1c_03e.tst        # querystats tests
   call scripts\1m1c_04e.tst        # setpacketfilter tests
   call scripts\1m1c_05e.tst        # setlookahead tests
   call scripts\1m1c_06e.tst        # set multicast address tests
   call scripts\1m1c_07e.tst        # send tests
   call scripts\1m1c_08e.tst        # send directed/receive various tests..
   call scripts\1m1c_09e.tst        # send broadcast/receive various tests...
   call scripts\1m1c_10e.tst        # send multicast/receive various tests..
   call scripts\1m1c_11e.tst        # send random/receive various tests..
   call scripts\1m1c_12e.tst        # send directed/receive/resend tests
   call scripts\1m1c_13e.tst        # send broadcast/receive/resend tests
   call scripts\1m1c_14e.tst        # send multicast/receive/resend tests..
   if ( ($G_Filters & PROMISCUOUS) != NONE)
   {
      call scripts\1m1c_15e.tst     # send random/receive/resend tests..
   }
   call scripts\str_1c1e.tst        # 1 card, 1 open stress tests

   #------------------------------------------
   # run 1 machine, 1 card, 2 open instances tests for encapsulated ethernet
   #------------------------------------------

   call scripts\1m2o_01e.tst        # queryinfo tests
   call scripts\1m2o_02e.tst        # querystats tests
   call scripts\1m2o_03e.tst        # setpacketfilter tests
   call scripts\1m2o_04e.tst        # setlookahead tests
   call scripts\1m2o_05e.tst        # set multicast address tests
   call scripts\1m2o_06e.tst        # send tests
   call scripts\1m2o_07e.tst        # send directed/receive various tests..
   call scripts\1m2o_08e.tst        # send broadcast/receive various tests...
   call scripts\1m2o_09e.tst        # send multicast/receive various tests..
   call scripts\1m2o_10e.tst        # send random/receive various tests..
   call scripts\1m2o_11e.tst        # send directed/receive/resend tests
   call scripts\1m2o_12e.tst        # send broadcast/receive/resend tests
   call scripts\1m2o_13e.tst        # send multicast/receive/resend tests..
   call scripts\str_2o1e.tst       # 1 card, 2 open stress tests
   call scripts\str_2o2e.tst

   #------------------------------------------
   # run 1 machine, 2 card tests for encapsulated ethernet
   #------------------------------------------

   variation

   if ($G_TrustedTotalSize != 0)    # will be non-zero ONLY if it finds a trusted card..
   {                                # otherwise all these tests will be skipped
      call scripts\1m2c_01e.tst     # send directed/receive various tests..
      call scripts\1m2c_02e.tst     # send broadcast/receive various tests...
      call scripts\1m2c_03e.tst     # send multicast/receive various tests..
      call scripts\1m2c_04e.tst     # send random/receive various tests..
      call scripts\1m2c_05e.tst     # send directed/receive/resend tests
      call scripts\1m2c_06e.tst     # send broadcast/receive/resend tests
      call scripts\1m2c_07e.tst     # send multicast/receive/resend tests..
      call scripts\1m2c_08e.tst     # receive on multiple multicast addresses
      call scripts\1m2c_09e.tst     # receive directed on multiple opens
      call scripts\1m2c_10e.tst     # receive broadcast on multiple opens
      call scripts\1m2c_11e.tst     # receive multicast on multiple opens
      call scripts\1m2c_12e.tst     # receive random on multiple opens
      call scripts\str_2c1e.tst     # 1 machine, 2 card stress tests
      call scripts\str_2c2e.tst
      call scripts\str_2c3e.tst
   }

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

#      call scripts\perfdire.tst     # performance tests..
#      call scripts\perfbrde.tst
#      call scripts\perfmlte.tst

      closeserver $G_ServerOpen TRUE
   }
}

end

#-------------------------------------------------------------------
# end of file ndis.tst
#-------------------------------------------------------------------


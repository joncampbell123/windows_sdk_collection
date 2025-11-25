#------------------------------------------
# File:  stress.tst
#
# This is the master stress script.
# It calls all the stress scripts
#
# Script globals are defined here.  Whenever they are set
# as the result of a command, they must be enclosed in the
# "global" "end_global" pair, or else the script might create
# a local variable with the same name.
#
# For WAN adapted from stress.tst
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
end_global


debuglevel  0

#----------------------------------------
# find test card driver, and set associated global variables
#----------------------------------------
##      call  scripts\gettest
##
##      call scripts\str_1c1.tst         # 1 card, 1 open stress tests
##
##      call scripts\str_2o1.tst         # 1 card, 2 open stress tests
##      call scripts\str_2o2.tst
##
##      variation
##
##      call scripts\gettrust.tst        # get name of trusted card (in same machine as test card)
##      if ($G_TrustedTotalSize != 0)    # will be non-zero ONLY if it finds a trusted card..
##      {
##         call scripts\str_2c1.tst      # 1 machine, 2 card stress tests
##         call scripts\str_2c2.tst
##         call scripts\str_2c3.tst
##      }
##
variation

        ##      global
        ##      $G_ServerOpen = openserver $G_TestCard $G_OpenFlag
        ##      end_global
        ##      if ($G_ServerOpen != 0)
        ##      {
   call scripts\str_2m1.tst         # 2 machine stress tests..
   call scripts\str_2m2.tst
   call scripts\str_2m3.tst

   closeserver $G_ServerOpen TRUE
##      }

#---------------------------------------------------------------
# if ethernet, tokenring, fddi, are done now
# if arcnet, we just finished with arcnet878_2 mode, now
# need to do encapsulated ethernet...
#---------------------------------------------------------------

##      if ($G_OpenFlag == FALSE)
##      {
##         variation
##
##         global
##         $G_OpenFlag = TRUE
##         end_global
##
##         call  scripts\gettest
##
##         call scripts\str_1c1e.tst        # 1 card, 1 open stress tests
##
##         call scripts\str_2o1e.tst        # 1 card, 2 open stress tests
##         call scripts\str_2o2e.tst
##
##         call scripts\gettrust.tst        # get name of trusted card (in same machine as test card)
##         if ($G_TrustedTotalSize != 0)    # will be non-zero ONLY if it finds a trusted card..
##         {
##            call scripts\str_2c1e.tst     # 1 machine, 2 card stress tests
##            call scripts\str_2c2e.tst
##            call scripts\str_2c3e.tst
##         }
##
##         variation
##
##         global
##         $G_ServerOpen = openserver $G_TestCard $G_OpenFlag
##         end_global
##         if ($G_ServerOpen != 0)
##         {
##            call scripts\str_2m1e.tst     # 2 machine stress tests..
##            call scripts\str_2m2e.tst
##            call scripts\str_2m3e.tst
##
##            closeserver $G_ServerOpen TRUE
##         }
##      }

end

#-------------------------------------------------------------------
# end of file stress.tst
#-------------------------------------------------------------------




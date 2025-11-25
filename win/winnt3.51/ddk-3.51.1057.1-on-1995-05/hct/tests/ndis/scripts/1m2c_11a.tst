#######################################################
# FILE:  1m2c_11a.tst
#
# These tests verify the ability to receive packets sent
# from a second card on the same machine, with various
# packet filters set on different open instances
#######################################################

friendly_script_name "Multiple Open Receive Multicast Tests"

print "TITLE:  Test ability to receive multicast packets with multiple different opens\n"

#------------------------------------------------------
# variation:  open up cards, prepare for tests...
#------------------------------------------------------

variation

$SendOpen = open $G_TrustedCard $G_OpenFlag
if ($SendOpen == 0)
{
   block_variation
   print "BLOCKED:  unable to open trusted card"
   goto endvar0
}
#

$RcvOpen = array ulong 8

$Count = 0
while ($Count < 8)
{
   $RcvOpen[$Count] = (ulong)0
   $Count = $Count + 1
}

#
# now open the eight different open instances...
#
$Count = 0
while ($Count < 8)
{
   $RcvOpen[$Count] = open $G_TestCard $G_OpenFlag
   if ($RcvOpen[$Count] == 0)
   {
      block_variation
      print "BLOCKED:  unable to open test card"
      goto endvar1
   }
   $Count = $Count + 1
}

$MaximumPacketSize = $G_MaxTotalSize
if ($G_TrustedTotalSize < $MaximumPacketSize)
{
   $MaximumPacketSize = $G_TrustedTotalSize
}


$GoodMultAddr = array uchar 6
$GoodMultAddr[0] = (uchar)0x01
$GoodMultAddr[1] = (uchar)0x02
$GoodMultAddr[2] = (uchar)0x03
$GoodMultAddr[3] = (uchar)0x04
$GoodMultAddr[4] = (uchar)0x05
$GoodMultAddr[5] = (uchar)0x06

$BadMultAddr = array uchar 6
$BadMultAddr[0] = (uchar)0x01
$BadMultAddr[1] = (uchar)0x02
$BadMultAddr[2] = (uchar)0x03
$BadMultAddr[3] = (uchar)0x04
$BadMultAddr[4] = (uchar)0x05
$BadMultAddr[5] = (uchar)0x00

#
# Set up the filter array...
#
$Filters = array long 24

$Filters[0]  = NONE
$Filters[1]  = DIRECTED
$Filters[2]  = BROADCAST
$Filters[3]  = DIRECTED | BROADCAST
$Filters[4]  = MULTICAST
$Filters[5]  = MULTICAST
$Filters[6]  = MULTICAST | DIRECTED
$Filters[7]  = MULTICAST | DIRECTED
$Filters[8]  = MULTICAST | BROADCAST
$Filters[9]  = MULTICAST | BROADCAST
$Filters[10] = MULTICAST | DIRECTED | BROADCAST
$Filters[11] = MULTICAST | DIRECTED | BROADCAST

if ($G_Filters & ALLMULTICAST)
{
   $Filters[12]  = ALLMULTICAST
   $Filters[13]  = ALLMULTICAST | DIRECTED
   $Filters[14]  = ALLMULTICAST | BROADCAST
   $Filters[15]  = ALLMULTICAST | MULTICAST
   $Filters[16]  = ALLMULTICAST | MULTICAST

   if ($G_Filters & PROMISCUOUS)
   {
      $Filters[17] = PROMISCUOUS
      $Filters[18] = PROMISCUOUS | DIRECTED
      $Filters[19] = PROMISCUOUS | BROADCAST
      $Filters[20] = PROMISCUOUS | MULTICAST
      $Filters[21] = PROMISCUOUS | MULTICAST
      $Filters[22] = PROMISCUOUS | MULTICAST | DIRECTED | BROADCAST
      $Filters[23] = PROMISCUOUS | MULTICAST | DIRECTED | BROADCAST
      $MaxLoops = 17
   }
   else
   {
      $MaxLoops = 10
   }
}

else     # no allmulticast
{
   if ($G_Filters & PROMISCUOUS)
   {
      $Filters[12] = PROMISCUOUS
      $Filters[13] = PROMISCUOUS | DIRECTED
      $Filters[14] = PROMISCUOUS | BROADCAST
      $Filters[15] = PROMISCUOUS | MULTICAST
      $Filters[16] = PROMISCUOUS | MULTICAST
      $Filters[17] = PROMISCUOUS | DIRECTED | BROADCAST | MULTICAST
      $Filters[18] = PROMISCUOUS | DIRECTED | BROADCAST | MULTICAST
      $MaxLoops = 12
   }
   else
   {
      $MaxLoops = 5
   }
}

$PacketsSent = 0
$Received = 0
$Resent = 0
$Transferred = 0
$NumPackets = $G_PacketsToSend
$MinPacketSize = 40

#------------------------------------------------
# variation:   send directed packets of various sizes with
#              the packet filter set to each of the legal
#              settings for this card/driver
#------------------------------------------------

$Count = 0
print "\n***Loop thru supported packet filters***\n"

while ($Count < $MaxLoops)
{
   print "\nLOOPTOP-LOOPTOP-LOOPTOP-LOOPTOP-LOOPTOP-LOOPTOP-LOOPTOP-LOOPTOP-LOOPTOP-LOOPTOP\n"

   #
   # first set the packet filters for the test card open instances
   #
   print " "
   variation
   $Index = 0
   while ($Index < 8)
   {
      $CurFilter = $Filters[$Count + $Index]
      $result = setpacketfilter $RcvOpen[$Index] $CurFilter
      if ($result != TEST_SUCCESSFUL)
      {
         fail_variation
         print "FAILED:  unable to set packet filter"
      }
      if ($CurFilter & MULTICAST)
      {
         if (($Index + $Count) & 0x01)
         {
            $result = addmulticast $RcvOpen[$Index] $GoodMultAddr
         }
         else
         {
            $result = addmulticast $RcvOpen[$Index] $BadMultAddr
         }
         if ($result != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILED:  unable to set multicast address"
         }
      }
      $Index = $Index + 1
   }

   $PacketSize = $MinPacketSize
   while (TRUE)
   {
      print " "
      variation

      #
      # start receives on all test open instances
      #
      $Index = 0
      while ($Index < 8)
      {
         $result = startreceive $RcvOpen[$Index]
         if ($result != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILURE:  receive failed to start"
         }
         $Index = $Index + 1
      }

      #
      # send packets, check send results
      #

      $result = send $SendOpen $GoodMultAddr $PacketSize $NumPackets
      if ($result != TEST_SUCCESSFUL)
      {
         fail_variation
         print "FAILURE:  send failed to start"
      }
      else
      {
         $result = waitsend $SendOpen
         if ($result != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILURE:  waitsend failed"
         }
         else
         {
            $result = getsendresults $SendOpen $PacketsSent
            if ($result != TEST_SUCCESSFUL)
            {
               fail_variation
               print "FAILURE:  unable to get send results"
            }
            elseif ($PacketsSent != $NumPackets)
            {
               fail_variation
               print "FAILURE:  should have sent" $NumPackets "packets"
            }
         }
      }

      print " "
      variation

      #
      # the test card will receive packets when the filter is set right
      #
      $Index = 0
      while ($Index < 8)
      {
         $CurFilter = $Filters[$Count + $Index]

         $result = stopreceive $RcvOpen[$Index]
         if ($result != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILURE:  stopreceive failure"
         }
         else
         {
            $result = getreceiveresults $RcvOpen[$Index] $Received $Resent $Transferred
            if ($result == TEST_FAILED)
            {
               fail_variation
            }
            elseif ($result == TEST_WARNED)
            {
               warn_variation
            }

            if ($Resent != 0)
            {
               fail_variation
               print "FAILURE:  no packets should have been resent"
            }

            $ShouldRcv = FALSE
            if ( ($CurFilter & (ALLMULTICAST + PROMISCUOUS)) != 0)
            {
               $ShouldRcv = TRUE
            }
            elseif (($CurFilter & MULTICAST) && (($Index + $Count) & 0x01))
            {
               $ShouldRcv = TRUE
            }

            if ($ShouldRcv)
            {
               if ($Received != $NumPackets)
               {
                  fail_variation
                  print "FAILURE:  should have received" $NumPackets "packets"
               }
               if ($Transferred != $NumPackets)
               {
                  fail_variation
                  print "FAILURE:  should have transferred" $NumPackets "packets"
               }
            }
            else
            {
               if ($Received != 0)
               {
                  fail_variation
                  print "FAILURE:  no packets should have been received!"
               }
               if ($Transferred != 0)
               {
                  fail_variation
                  print "FAILURE:  no packets should have been transferred!"
               }
            }
         }

         $Index = $Index + 1
      }
      #
      # prepare for next send
      #
      if ($PacketSize == $MaximumPacketSize)
      {
         break
      }

      elseif ($PacketSize == $MinPacketSize)
      {
         $PacketSize = ($MaximumPacketSize + $MinPacketSize) / 2
         if ( ($PacketSize & 0x0001) == 0)
         {
            $PacketSize = $PacketSize + 1
         }
      }
      else
      {
         $PacketSize = $MaximumPacketSize
      }
   }
   #
   # clean up multicast if necessary
   #
   print " "
   variation
   $Index = 0
   while ($Index < 8)
   {
      $CurFilter = $Filters[$Count + $Index]
      if ($CurFilter & MULTICAST)
      {
         if (($Index + $Count) & 0x01)
         {
            $result = deletemulticast $RcvOpen[$Index] $GoodMultAddr
         }
         else
         {
            $result = deletemulticast $RcvOpen[$Index] $BadMultAddr
         }
         if ($result != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILED:  unable to delete multicast address"
         }
      }
      $Index = $Index + 1
   }
   $Count = $Count + 1
}


#---------------------------------------------
# cleanup
#---------------------------------------------

print " "
variation
$result = getevents $SendOpen
if ($result == TEST_FAILED)
{
   fail_variation
   print "FAILURE:  unexpected events occurred."
}
elseif ($result == TEST_WARNED)
{
   warn_variation
   print "WARNING:  unexpected events occurred."
}


print " "
variation
$Index = 0
while ($Index < 8)
{
   $result = getevents $RcvOpen[$Index]
   if ($result == TEST_FAILED)
   {
      fail_variation
      print "FAILURE:  unexpected events occurred."
   }
   elseif ($result == TEST_WARNED)
   {
      warn_variation
      print "WARNING:  unexpected events occurred."
   }
   $Index = $Index + 1
}


:endvar1

#
# now cose the eight different open instances on the test card
#
$Count = 0
while ($Count < 8)
{
   if ($RcvOpen[$Count] != 0)
   {
      $result = close $RcvOpen[$Count]
      if ($result != TEST_SUCCESSFUL)
      {
         fail_variation
         print "FAILURE:  close failed."
      }
   }
   $Count = $Count + 1
}
#
# and close the trusted card
#
$result = close $SendOpen
if ($result != TEST_SUCCESSFUL)
{
   fail_variation
   print "FAILURE:  close failed."
}

:endvar0

end

############################################################
# end of file 1m2c_11a.tst
############################################################


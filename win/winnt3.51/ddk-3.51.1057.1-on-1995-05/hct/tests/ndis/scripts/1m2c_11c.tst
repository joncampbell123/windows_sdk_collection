#######################################################
# FILE:  1m2c_11c.tst
#
# These tests verify the ability to receive packets sent
# from a second card on the same machine, with various
# packet filters set on different open instances
#######################################################

friendly_script_name "Multiple Open Receive Group Tests"

print "TITLE:  Test ability to receive broadcast packets with multiple different opens\n"

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

#
# Set up the filter array...
#
$Filters = array long 16
$SendAddr = array uchar 6

$Filters[0] = NONE
$Filters[1] = DIRECTED
$Filters[2] = BROADCAST
$Filters[3] = DIRECTED | BROADCAST

if ($G_Media == MEDIUM_ARCNET)
{
   $Filters[4]  = NONE
   $Filters[5]  = DIRECTED
   $Filters[6]  = BROADCAST
   $Filters[7]  = DIRECTED | BROADCAST
   $Filters[8]  = NONE
   $Filters[9]  = DIRECTED
   $Filters[10] = BROADCAST
   $Filters[11] = DIRECTED | BROADCAST
   $Filters[12] = NONE
   $Filters[13] = DIRECTED
   $Filters[14] = BROADCAST
   $Filters[15] = DIRECTED | BROADCAST

   $SendAddr[0] = (uchar)0x00
   $SendAddr[1] = (uchar)0x00
   $SendAddr[2] = (uchar)0x00
   $SendAddr[3] = (uchar)0x00
   $SendAddr[4] = (uchar)0x00
   $SendAddr[5] = (uchar)0x00
}

elseif ($G_Media == MEDIUM_TOKENRING)
{
   $FunctAddr = array uchar 4
   $FunctAddr[0] = (uchar)0x01
   $FunctAddr[1] = (uchar)0x02
   $FunctAddr[2] = (uchar)0x03
   $FunctAddr[3] = (uchar)0x04

   $GroupAddr = array uchar 4
   $GroupAddr[0] = (uchar)0x81
   $GroupAddr[1] = (uchar)0x02
   $GroupAddr[2] = (uchar)0x03
   $GroupAddr[3] = (uchar)0x04

   $NullAddr  = array uchar 4
   $NullAddr[0] = (uchar)0x00
   $NullAddr[1] = (uchar)0x00
   $NullAddr[2] = (uchar)0x00
   $NullAddr[3] = (uchar)0x00

   $SendAddr[0] = (uchar)0xc0
   $SendAddr[1] = (uchar)0x00
   $SendAddr[2] = (uchar)0xff
   $SendAddr[3] = (uchar)0xff
   $SendAddr[4] = (uchar)0xff
   $SendAddr[5] = (uchar)0xff

   $Filters[4]  = FUNCTIONAL
   $Filters[5]  = FUNCTIONAL | DIRECTED
   $Filters[6]  = FUNCTIONAL | BROADCAST
   $Filters[7]  = GROUP
   $Filters[8]  = GROUP | DIRECTED
   $Filters[9]  = GROUP | BROADCAST
   $Filters[10] = GROUP | FUNCTIONAL


   if ($G_Filters & PROMISCUOUS)
   {
      $Filters[11] = PROMISCUOUS
      $Filters[12] = PROMISCUOUS | DIRECTED
      $Filters[13] = PROMISCUOUS | BROADCAST
      $Filters[14] = PROMISCUOUS | FUNCTIONAL
      $Filters[15] = PROMISCUOUS | GROUP
   }
   else
   {
      $Filters[11] = NONE
      $Filters[12] = DIRECTED
      $Filters[13] = BROADCAST
      $Filters[14] = FUNCTIONAL
      $Filters[15] = GROUP
   }
}

else                 #if $G_Media == MEDIUM_ETHERNET || $G_Media == MEDIUM_FDDI
{
   $MultAddr = array uchar 6
   $MultAddr[0] = (uchar)0x01
   $MultAddr[1] = (uchar)0x02
   $MultAddr[2] = (uchar)0x03
   $MultAddr[3] = (uchar)0x04
   $MultAddr[4] = (uchar)0x05
   $MultAddr[5] = (uchar)0x06

   $SendAddr[0] = (uchar)0xff
   $SendAddr[1] = (uchar)0xff
   $SendAddr[2] = (uchar)0xff
   $SendAddr[3] = (uchar)0xff
   $SendAddr[4] = (uchar)0xff
   $SendAddr[5] = (uchar)0xff

   $Filters[4]  = MULTICAST
   $Filters[5]  = MULTICAST | DIRECTED
   $Filters[6]  = MULTICAST | BROADCAST
   if ($G_Filters & ALLMULTICAST)
   {
      $Filters[7]  = ALLMULTICAST
      $Filters[8]  = ALLMULTICAST | DIRECTED
      $Filters[9]  = ALLMULTICAST | BROADCAST
      $Filters[10] = ALLMULTICAST | MULTICAST

      if ($G_Filters & PROMISCUOUS)
      {
         $Filters[11] = PROMISCUOUS
         $Filters[12] = PROMISCUOUS | DIRECTED
         $Filters[13] = PROMISCUOUS | BROADCAST
         $Filters[14] = PROMISCUOUS | MULTICAST
         $Filters[15] = PROMISCUOUS | ALLMULTICAST
      }
      else
      {
         $Filters[11] = NONE
         $Filters[12] = DIRECTED
         $Filters[13] = BROADCAST
         $Filters[14] = MULTICAST
         $Filters[15] = ALLMULTICAST
      }
   }

   else     # no allmulticast
   {
      $Filters[7] = NONE
      if ($G_Filters & PROMISCUOUS)
      {
         $Filters[8]  = PROMISCUOUS
         $Filters[9]  = PROMISCUOUS | DIRECTED
         $Filters[10] = PROMISCUOUS | BROADCAST
         $Filters[11] = PROMISCUOUS | MULTICAST
      }
      else
      {
         $Filters[8]  = NONE
         $Filters[9]  = DIRECTED
         $Filters[10] = BROADCAST
         $Filters[11] = MULTICAST
      }
      $Filters[12]  = DIRECTED | BROADCAST
      $Filters[13]  = DIRECTED | MULTICAST
      $Filters[14]  = BROADCAST | MULTICAST
      $Filters[15]  = NONE
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

while ($Count < 9)
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
         $result = addmulticast $RcvOpen[$Index] $MultAddr
         if ($result != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILED:  unable to set multicast address"
         }
      }
      if ($CurFilter & FUNCTIONAL)
      {
         $result = setfunctional $RcvOpen[$Index] $FunctAddr
         if ($result != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILED:  unable to set functional address"
         }
      }
      if ($CurFilter & GROUP)
      {
         $result = setgroup $RcvOpen[$Index] $GroupAddr
         if ($result != TEST_SUCCESSFUL)
         {
            print "Warning:  failed to set group address"
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

      $result = send $SendOpen $SendAddr $PacketSize $NumPackets
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
            if ( ($CurFilter & (BROADCAST + PROMISCUOUS)) != 0)
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
   #clean up multicast/functional/group if necessary
   #
   print " "
   variation
   $Index = 0
   while ($Index < 8)
   {
      $CurFilter = $Filters[$Count + $Index]
      if ($CurFilter & MULTICAST)
      {
         $result = deletemulticast $RcvOpen[$Index] $MultAddr
         if ($result != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILED:  unable to delete multicast address"
         }
      }
      if ($CurFilter & FUNCTIONAL)
      {
         $result = setfunctional $RcvOpen[$Index] $NullAddr
         if ($result != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILED:  unable to clear functional address"
         }
      }
      if ($CurFilter & GROUP)
      {
         setgroup $RcvOpen[$Index] $NullAddr
         print "Warning:  failed to clear group address"
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
# end of file 1m2c_11c.tst
############################################################


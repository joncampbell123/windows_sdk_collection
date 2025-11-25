#######################################################
# FILE:  1m2c_05.tst
#
# These tests verify the ability to send, receive, and resend self-directed
# packets between two different cards on one machine
#######################################################

friendly_script_name "Two Card Send/Receive/Resend Directed Tests"

print "TITLE:  Test send, receive, and resend of directed packets with two cards on one machine\n"

#------------------------------------------------------
# variation:  open up cards, prepare for tests...
#------------------------------------------------------

variation

$OpenOne = open $G_TrustedCard $G_OpenFlag
if ($OpenOne == 0)
{
   block_variation
   print "BLOCKED:  unable to open trusted card"
   goto endvar0
}

$OpenTwo = open $G_TestCard $G_OpenFlag
if ($OpenTwo == 0)
{
   block_variation
   print "BLOCKED:  unable to open test card"
   close $OpenOne
   goto endvar0
}

$MaximumPacketSize = $G_MaxTotalSize
if ($G_TrustedTotalSize < $MaximumPacketSize)
{
   $MaximumPacketSize = $G_TrustedTotalSize
}

$ResendAddr     = array uchar 6
$MulticastAddr  = array uchar 6
$BroadcastAddr  = array uchar 6
$FunctionalAddr = array uchar 4
$GroupAddr      = array uchar 4
$NullAddr       = array uchar 4

$MulticastAddr[0]  = (uchar)0x01
$MulticastAddr[1]  = (uchar)0x02
$MulticastAddr[2]  = (uchar)0x03
$MulticastAddr[3]  = (uchar)0x04
$MulticastAddr[4]  = (uchar)0x05
$MulticastAddr[5]  = (uchar)0x06

$FunctionalAddr[0] = (uchar)0x01
$FunctionalAddr[1] = (uchar)0x02
$FunctionalAddr[2] = (uchar)0x03
$FunctionalAddr[3] = (uchar)0x04

$GroupAddr[0]      = (uchar)0x81
$GroupAddr[1]      = (uchar)0x02
$GroupAddr[2]      = (uchar)0x03
$GroupAddr[3]      = (uchar)0x04

$NullAddr[0]       = (uchar)0x00
$NullAddr[1]       = (uchar)0x00
$NullAddr[2]       = (uchar)0x00
$NullAddr[3]       = (uchar)0x00

if ($G_Media == MEDIUM_TOKENRING)
{
   $BroadcastAddr[0] = (uchar)0xc0
   $BroadcastAddr[1] = (uchar)0x00
   $BroadcastAddr[2] = (uchar)0xff
   $BroadcastAddr[3] = (uchar)0xff
   $BroadcastAddr[4] = (uchar)0xff
   $BroadcastAddr[5] = (uchar)0xff
}
elseif ($G_Media == MEDIUM_ARCNET)
{
   $BroadcastAddr[0] = (uchar)0x00
   $BroadcastAddr[1] = (uchar)0x00
   $BroadcastAddr[2] = (uchar)0x00
   $BroadcastAddr[3] = (uchar)0x00
   $BroadcastAddr[4] = (uchar)0x00
   $BroadcastAddr[5] = (uchar)0x00
}
else              # MEDIUM_ETHERNET or MEDIUM_FDDI
{
   $BroadcastAddr[0] = (uchar)0xff
   $BroadcastAddr[1] = (uchar)0xff
   $BroadcastAddr[2] = (uchar)0xff
   $BroadcastAddr[3] = (uchar)0xff
   $BroadcastAddr[4] = (uchar)0xff
   $BroadcastAddr[5] = (uchar)0xff
}


$Filters = array long 8

$Filters[0]  = DIRECTED
$Filters[1]  = BROADCAST
$NumFilters  = 2
$MaxTests    = 2

# Ethernet/FDDI

if ($G_Filters & MULTICAST)
{
   $Filters[$NumFilters] = MULTICAST
   $NumFilters = $NumFilters + 1
   $MaxTests = $MaxTests * 2
   $ResendTypes = $NumFilters
   if ($G_Filters & ALLMULTICAST)
   {
      $Filters[$NumFilters] = ALLMULTICAST
      $NumFilters = $NumFilters + 1
      $MaxTests = $MaxTests * 2
   }
}

# Token Ring

elseif ($G_Filters & FUNCTIONAL)
{
   $Filters[$NumFilters] = FUNCTIONAL
   $NumFilters = $NumFilters + 1
   $Filters[$NumFilters] = GROUP
   $NumFilters = $NumFilters + 1
   $MaxTests = $MaxTests * 4
   $ResendTypes = $NumFilters

   $TempFilter = NONE
   if ($G_Filters & ALLFUNCTIONAL)
   {
      $TempFilter = ALLFUNCTIONAL
   }
   if ($G_Filters & MACFRAME)
   {
      $TempFilter = $TempFilter | MACFRAME
   }
   if ($G_Filters & SOURCEROUTING)
   {
      $TempFilter = $TempFilter | SOURCEROUTING
   }
   if ($TempFilter != NONE)
   {
      $Filters[$NumFilters] = $TempFilter
      $NumFilters = $NumFilters + 1
      $MaxTests = $MaxTests * 2
   }
}


# Arcnet

else
{
   $ResendTypes = $NumFilters
}


$PacketsSent = 0
$Received = 0
$Resent = 0
$Transferred = 0
$NumPackets = $G_PacketsToSend
$MinPacketSize = 64


#------------------------------------------------
# variation:   send directed packets of various sizes with
#              the packet filter set to each of the legal
#              settings for this card/driver
#------------------------------------------------

#
# outer loop:  pass 0--base filter is DIRECTED
#              pass 1--base filter is PROMISCUOUS
#

$Pass = 0
while ($Pass < 2)
{
   #
   # middle loop--vary resend address and receive filter type for open instance 1
   #
   $Rcount = 0
   while ($Rcount < $ResendTypes)
   {
      print "\n----------------------------------------------------------------------------\n"

      $OneResendExp = 0
      $OneReceiveExp = $NumPackets

      $FilterOne = $Filters[$Rcount]
      if ($FilterOne == DIRECTED)
      {
         $ResendAddr[0] = $G_TrustedAddress[0]
         $ResendAddr[1] = $G_TrustedAddress[1]
         $ResendAddr[2] = $G_TrustedAddress[2]
         $ResendAddr[3] = $G_TrustedAddress[3]
         $ResendAddr[4] = $G_TrustedAddress[4]
         $ResendAddr[5] = $G_TrustedAddress[5]
         print "\n***Send directed, receive various, resend Directed***\n"
      }
      elseif ($FilterOne == BROADCAST)
      {
         $ResendAddr[0] = $BroadcastAddr[0]
         $ResendAddr[1] = $BroadcastAddr[1]
         $ResendAddr[2] = $BroadcastAddr[2]
         $ResendAddr[3] = $BroadcastAddr[3]
         $ResendAddr[4] = $BroadcastAddr[4]
         $ResendAddr[5] = $BroadcastAddr[5]
         print "\n***Send Directed, receive various, resend Broadcast***\n"
      }
      elseif ($FilterOne == MULTICAST)
      {
         $ResendAddr[0] = $MulticastAddr[0]
         $ResendAddr[1] = $MulticastAddr[1]
         $ResendAddr[2] = $MulticastAddr[2]
         $ResendAddr[3] = $MulticastAddr[3]
         $ResendAddr[4] = $MulticastAddr[4]
         $ResendAddr[5] = $MulticastAddr[5]
         $result = addmulticast $OpenOne $MulticastAddr
         if ($result != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILURE:  unable to set multicast address"
         }
         print "\n***Send Directed, receive various, resend Multicast***\n"
      }
      elseif ($FilterOne == FUNCTIONAL)
      {
         $ResendAddr[0] = (uchar)0xC0
         $ResendAddr[1] = (uchar)0x00
         $ResendAddr[2] = $FunctionalAddr[0]
         $ResendAddr[3] = $FunctionalAddr[1]
         $ResendAddr[4] = $FunctionalAddr[2]
         $ResendAddr[5] = $FunctionalAddr[3]
         $result = setfunctional $OpenOne $FunctionalAddr
         if ($result != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILURE:  unable to set functional address"
         }
         print "\n***Send Directed, receive various, resend Functional***\n"
      }
      elseif ($FilterOne == GROUP)
      {
         $ResendAddr[0] = (uchar)0xC0
         $ResendAddr[1] = (uchar)0x00
         $ResendAddr[2] = $GroupAddr[0]
         $ResendAddr[3] = $GroupAddr[1]
         $ResendAddr[4] = $GroupAddr[2]
         $ResendAddr[5] = $GroupAddr[3]
         $result = setgroup $OpenOne $GroupAddr
         if ($result != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILURE:  unable to set group address"
         }
         print "\n***Send Directed, receive various, resend Group***\n"
      }

      $result = setpacketfilter $OpenOne $FilterOne
      if ($result != TEST_SUCCESSFUL)
      {
         fail_variation
         print "FAILURE:  unable to set packet filter"
      }

      #
      # inner loop--vary receive filter for open instance 2
      #

      print "\n**Loop thru the supported packet filters**\n"

      $Count = 0
      while ($Count < $MaxTests)
      {
         print "\nLOOPTOP-LOOPTOP-LOOPTOP-LOOPTOP-LOOPTOP-LOOPTOP-LOOPTOP-LOOPTOP-LOOPTOP\n"

         #
         # first set the packet filter
         #
         if ($Pass == 0)
         {
            $CurFilter = DIRECTED
         }
         else
         {
            $CurFilter = PROMISCUOUS
         }
         if ($Count & 0x01)
         {
            $CurFilter = $CurFilter | $Filters[1]
         }
         if ($Count & 0x02)
         {
            $CurFilter = $CurFilter | $Filters[2]
         }
         if ($Count & 0x04)
         {
            $CurFilter = $CurFilter | $Filters[3]
         }
         if ($Count & 0x08)
         {
            $CurFilter = $CurFilter | $Filters[4]
         }

         $result = setpacketfilter $OpenTwo $CurFilter
         if ($result == TEST_SUCCESSFUL)
         {
            if ($CurFilter & MULTICAST)
            {
               $result = addmulticast $OpenTwo $MulticastAddr
               if ($result != TEST_SUCCESSFUL)
               {
                  fail_variation
                  print "FAILURE:  unable to set multicast address"
               }
            }
            if ($CurFilter & FUNCTIONAL)
            {
               $result = setfunctional $OpenTwo $FunctionalAddr
               if ($result != TEST_SUCCESSFUL)
               {
                  fail_variation
                  print "FAILURE:  unable to set functional address"
               }
            }
            if ($CurFilter & GROUP)
            {
               $result = setgroup $OpenTwo $GroupAddr
               if ($result != TEST_SUCCESSFUL)
               {
                  fail_variation
                  print "FAILURE:  unable to set group address"
               }
            }

            $TwoResendExp  = $NumPackets
            $TwoReceiveExp = $NumPackets
            if ($Pass == 0x01)
            {
               $TwoReceiveExp = $NumPackets * 2
            }
            elseif ($FilterOne == BROADCAST)
            {
               if ($CurFilter & BROADCAST)
               {
                  $TwoReceiveExp = $NumPackets * 2
               }
            }
            elseif ($FilterOne == MULTICAST)
            {
               if ($CurFilter & (MULTICAST | ALLMULTICAST))
               {
                  $TwoReceiveExp = $NumPackets * 2
               }
            }
            elseif ($FilterOne == FUNCTIONAL)
            {
               if ($CurFilter & (FUNCTIONAL | ALLFUNCTIONAL))
               {
                  $TwoReceiveExp = $NumPackets * 2
               }
            }
            elseif ($FilterOne == GROUP)
            {
               if ($CurFilter & GROUP)
               {
                  $TwoReceiveExp = $NumPackets * 2
               }
            }

            $PacketSize = $MinPacketSize

            #
            # innermost loop:  do the actual test on various packet sizes
            #
            while (TRUE)
            {
               print " "
               variation

               #
               # start receive, send packets, check send results, then check
               # receive results
               #
               $result = startreceive $OpenOne
               if ($result != TEST_SUCCESSFUL)
               {
                  fail_variation
                  print "FAILURE:  receive failed to start"
               }
               $result = startreceive $OpenTwo
               if ($result != TEST_SUCCESSFUL)
               {
                  fail_variation
                  print "FAILURE:  receive failed to start"
               }

               $result = send $OpenOne $G_TestAddress $PacketSize $NumPackets $ResendAddr
               if ($result != TEST_SUCCESSFUL)
               {
                  fail_variation
                  print "FAILURE:  send failed to start"
               }
               else
               {
                  $result = waitsend $OpenOne
                  if ($result != TEST_SUCCESSFUL)
                  {
                     fail_variation
                     print "FAILURE:  waitsend failed"
                  }
                  else
                  {
                     $result = getsendresults $OpenOne $PacketsSent
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

               #
               # get receive results from "sending" open instance
               #
               $result = stopreceive $OpenOne
               if ($result != TEST_SUCCESSFUL)
               {
                  fail_variation
                  print "FAILURE:  stopreceive failure"
               }
               else
               {
                  $result = getreceiveresults $OpenOne $Received $Resent $Transferred
                  if ($result == TEST_FAILED)
                  {
                     fail_variation
                  }
                  elseif ($result == TEST_WARNED)
                  {
                     warn_variation
                  }

                  if ($Resent != $OneResendExp)
                  {
                     fail_variation
                     print "FAILURE:  should have resent" $OneResendExp "packets"
                  }
                  if ($Received != $OneReceiveExp)
                  {
                     fail_variation
                     print "FAILURE:  should have received" $OneReceiveExp "packets"
                  }
                  if ($Transferred != $OneReceiveExp)
                  {
                     fail_variation
                     print "FAILURE:  should have transferred" $OneReceiveExp "packets"
                  }
               }

               #
               # get receive results from "receiving" open instance
               #
               $result = stopreceive $OpenTwo
               if ($result != TEST_SUCCESSFUL)
               {
                  fail_variation
                  print "FAILURE:  stopreceive failure"
               }
               else
               {
                  $result = getreceiveresults $OpenTwo $Received $Resent $Transferred
                  if ($result == TEST_FAILED)
                  {
                     fail_variation
                  }
                  elseif ($result == TEST_WARNED)
                  {
                     warn_variation
                  }

                  if ($Resent != $TwoResendExp)
                  {
                     fail_variation
                     print "FAILURE:  should have resent" $TwoResendExp "packets"
                  }
                  if ($Received != $TwoReceiveExp)
                  {
                     fail_variation
                     print "FAILURE:  should have received" $TwoReceiveExp "packets"
                  }
                  if ($Transferred != $TwoReceiveExp)
                  {
                     fail_variation
                     print "FAILURE:  should have transferred" $TwoReceiveExp "packets"
                  }
               }

               #
               # set up size for next batch of packets
               #

               if ($PacketSize == $MaximumPacketSize)
               {
                  break
               }
               elseif ($PacketSize == $MinPacketSize)
               {
                  if ($Pass == 0)
                  {
                     if ($CurFilter & $Filters[1])
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
                  else
                  {
                     break
                  }
               }

               else
               {
                  $PacketSize = $MaximumPacketSize
               }

            }
            #
            # end of innermost loop
            #
            if ($CurFilter & MULTICAST)
            {
               $result = deletemulticast $OpenTwo $MulticastAddr
               if ($result != TEST_SUCCESSFUL)
               {
                  warn_variation
                  print "WARNING:  unable to clear multicast address"
               }
            }
            if ($CurFilter & FUNCTIONAL)
            {
               $result = setfunctional $OpenTwo $NullAddr
               if ($result != TEST_SUCCESSFUL)
               {
                  warn_variation
                  print "WARNING:  unable to clear functional address"
               }
            }
            if ($CurFilter & GROUP)
            {
               $result = setgroup $OpenTwo $NullAddr
               if ($result != TEST_SUCCESSFUL)
               {
                  warn_variation
                  print "WARNING:  unable to clear group address"
               }
            }

         }
         else
         {
            block_variation
            print "BLOCKED:  unable to set packet filter"
         }

         $Count = $Count + 1
      }

      #
      # end of inner loop
      #
      $result = setpacketfilter $OpenOne NONE
      if ($result != TEST_SUCCESSFUL)
      {
         fail_variation
         print "FAILURE:  unable to set packet filter to NONE"

      }
      if ($FilterOne == MULTICAST)
      {
         $result = deletemulticast $OpenOne $MulticastAddr
         if ($result != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILURE:  unable to delete multicast address"
         }
      }
      elseif ($FilterOne == FUNCTIONAL)
      {
         $result = setfunctional $OpenOne $NullAddr
         if ($result != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILURE:  unable to clear functional address"
         }
      }
      elseif ($FilterOne == GROUP)
      {
         $result = setgroup $OpenOne $NullAddr
         if ($result != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILURE:  unable to clear group address"
         }
      }
      $Rcount = $Rcount + 1
   }

   #
   # end of middle loop
   #
   if ($G_Filters & PROMISCUOUS)
   {
      $Pass = $Pass + 1
   }
   else
   {
      break
   }
}



#---------------------------------------------
# cleanup
#---------------------------------------------

print " "
variation
$result = getevents $OpenOne
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

$result = close $OpenOne
if ($result != TEST_SUCCESSFUL)
{
   fail_variation
   print "FAILURE:  close failed."
}

print " "
variation
$result = getevents $OpenTwo
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

$result = close $OpenTwo
if ($result != TEST_SUCCESSFUL)
{
   fail_variation
   print "FAILURE:  close failed."
}

:endvar0

end

############################################################
# end of file 1m2c_05.tst
############################################################


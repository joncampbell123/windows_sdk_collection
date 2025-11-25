#######################################################
# FILE:  2m2c_07a.tst
#
# These tests verify the ability to send, receive, and resend multicast-addressed
# packets between two different cards on different machine
#######################################################

friendly_script_name "Two Machine Send/Receive/Resend Multicast Tests"

print "TITLE:  Test send, receive, and resend with multicast packets between two different cards\n"

#------------------------------------------------------
# variation:  open up cards, prepare for tests...
#------------------------------------------------------

variation

$ClientOpen = open $G_TestCard $G_OpenFlag
if ($ClientOpen == 0)
{
   block_variation
   print "BLOCKED:  unable to open test card"
   goto endvar0
}


$MaximumPacketSize = 0
queryinfo $G_ServerOpen OID_GEN_MAXIMUM_TOTAL_SIZE $MaximumPacketSize
if ($G_MaxTotalSize < $MaximumPacketSize)
{
   $MaximumPacketSize = $G_MaxTotalSize
}

$ServerAddress = array uchar 6

if ($G_Media == MEDIUM_ETHERNET)
{
   queryinfo $G_ServerOpen OID_802_3_PERMANENT_ADDRESS $ServerAddress
}
elseif ($G_Media == MEDIUM_FDDI)
{
   queryinfo $G_ServerOpen OID_FDDI_LONG_PERMANENT_ADDR $ServerAddress
}
else
{
   block_variation
   print "BLOCKED:  invalid media type"
   close $ClientOpen
   goto endvar0
}

$ResendAddr     = array uchar 6
$MulticastAddr  = array uchar 6
$BroadcastAddr  = array uchar 6

$MulticastAddr[0]  = (uchar)0x01
$MulticastAddr[1]  = (uchar)0x02
$MulticastAddr[2]  = (uchar)0x03
$MulticastAddr[3]  = (uchar)0x04
$MulticastAddr[4]  = (uchar)0x05
$MulticastAddr[5]  = (uchar)0x06

$BroadcastAddr[0] = (uchar)0xff
$BroadcastAddr[1] = (uchar)0xff
$BroadcastAddr[2] = (uchar)0xff
$BroadcastAddr[3] = (uchar)0xff
$BroadcastAddr[4] = (uchar)0xff
$BroadcastAddr[5] = (uchar)0xff


$Filters = array long 8

$Filters[0]  = MULTICAST
$Filters[1]  = DIRECTED
$Filters[2]  = BROADCAST
$NumFilters  = 3
$MaxTests    = 4
$ResendTypes = $NumFilters

if ($G_Filters & ALLMULTICAST)
{
   $Filters[3] = ALLMULTICAST
   $NumFilters = 4
   $MaxTests = 8
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
# outer loop:  pass 0--base filter is MULTICAST
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
      print "\n------------------------------------------------------------------------------\n"

      $OneResendExp = 0
      $OneReceiveExp = $NumPackets

      $FilterOne = $Filters[$Rcount]
      if ($FilterOne == DIRECTED)
      {
         $ResendAddr[0] = $ServerAddress[0]
         $ResendAddr[1] = $ServerAddress[1]
         $ResendAddr[2] = $ServerAddress[2]
         $ResendAddr[3] = $ServerAddress[3]
         $ResendAddr[4] = $ServerAddress[4]
         $ResendAddr[5] = $ServerAddress[5]
         print "\n***Send Multicast, receive various, resend Directed***\n"
      }
      elseif ($FilterOne == BROADCAST)
      {
         $ResendAddr[0] = $BroadcastAddr[0]
         $ResendAddr[1] = $BroadcastAddr[1]
         $ResendAddr[2] = $BroadcastAddr[2]
         $ResendAddr[3] = $BroadcastAddr[3]
         $ResendAddr[4] = $BroadcastAddr[4]
         $ResendAddr[5] = $BroadcastAddr[5]
         print "\n***Send Multicast, receive various, resend Broadcast***\n"
      }
      elseif ($FilterOne == MULTICAST)
      {
         $ResendAddr[0] = $MulticastAddr[0]
         $ResendAddr[1] = $MulticastAddr[1]
         $ResendAddr[2] = $MulticastAddr[2]
         $ResendAddr[3] = $MulticastAddr[3]
         $ResendAddr[4] = $MulticastAddr[4]
         $ResendAddr[5] = $MulticastAddr[5]
         $result = addmulticast $G_ServerOpen $MulticastAddr
         if ($result != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILURE:  unable to set multicast address"
         }
         $OneResendExp  = $NumPackets
         $OneReceiveExp = $NumPackets * 3
         print "\n***Send Multicast, receive various, resend Multicast***\n"
      }

      $result = setpacketfilter $G_ServerOpen $FilterOne
      if ($result != TEST_SUCCESSFUL)
      {
         fail_variation
         print "FAILURE:  unable to set packet filter"
      }

      #
      # inner loop--vary receive filter for open instance 2
      #

      $Count = 0
      while ($Count < $MaxTests)
      {
         print "\nLOOPTOP-LOOPTOP-LOOPTOP-LOOPTOP-LOOPTOP-LOOPTOP-LOOPTOP-LOOPTOP-LOOPTOP\n"

         #
         # first set the packet filter
         #
         if ($Pass == 0)
         {
            $CurFilter = MULTICAST
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

         $result = setpacketfilter $ClientOpen $CurFilter
         if ($result == TEST_SUCCESSFUL)
         {
            if ($CurFilter & MULTICAST)
            {
               $result = addmulticast $ClientOpen $MulticastAddr
               if ($result != TEST_SUCCESSFUL)
               {
                  fail_variation
                  print "FAILURE:  unable to set multicast address"
               }
            }

            $TwoResendExp  = $NumPackets
            $TwoReceiveExp = $NumPackets
            if ($FilterOne == MULTICAST)
            {
               $TwoReceiveExp = $NumPackets * 3
            }
            elseif ($Pass == 0x01)
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
               $result = startreceive $G_ServerOpen
               if ($result != TEST_SUCCESSFUL)
               {
                  fail_variation
                  print "FAILURE:  receive failed to start"
               }
               $result = startreceive $ClientOpen
               if ($result != TEST_SUCCESSFUL)
               {
                  fail_variation
                  print "FAILURE:  receive failed to start"
               }

               $result = send $G_ServerOpen $MulticastAddr $PacketSize $NumPackets $ResendAddr
               if ($result != TEST_SUCCESSFUL)
               {
                  fail_variation
                  print "FAILURE:  send failed to start"
               }
               else
               {
                  $result = waitsend $G_ServerOpen
                  if ($result != TEST_SUCCESSFUL)
                  {
                     fail_variation
                     print "FAILURE:  waitsend failed"
                  }
                  else
                  {
                     $result = getsendresults $G_ServerOpen $PacketsSent
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
               $result = stopreceive $G_ServerOpen
               if ($result != TEST_SUCCESSFUL)
               {
                  fail_variation
                  print "FAILURE:  stopreceive failure"
               }
               else
               {
                  $result = getreceiveresults $G_ServerOpen $Received $Resent $Transferred
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
               $result = stopreceive $ClientOpen
               if ($result != TEST_SUCCESSFUL)
               {
                  fail_variation
                  print "FAILURE:  stopreceive failure"
               }
               else
               {
                  $result = getreceiveresults $ClientOpen $Received $Resent $Transferred
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
               $result = deletemulticast $ClientOpen $MulticastAddr
               if ($result != TEST_SUCCESSFUL)
               {
                  warn_variation
                  print "WARNING:  unable to clear multicast address"
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
      $result = setpacketfilter $G_ServerOpen NONE
      if ($result != TEST_SUCCESSFUL)
      {
         fail_variation
         print "FAILURE:  unable to set packet filter to NONE"

      }
      if ($FilterOne == MULTICAST)
      {
         $result = deletemulticast $G_ServerOpen $MulticastAddr
         if ($result != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILURE:  unable to delete multicast address"
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
$result = getevents $G_ServerOpen
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
$result = getevents $ClientOpen
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

$result = close $ClientOpen
if ($result != TEST_SUCCESSFUL)
{
   fail_variation
   print "FAILURE:  close failed."
}

:endvar0

end

############################################################
# end of file 2m2c_07a.tst
############################################################


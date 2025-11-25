#######################################################
# FILE:  1m2o_13c.tst
#
# These tests verify the ability to send, receive, and resend group-addressed
# packets between two different open instances..
#######################################################

friendly_script_name "Two Open Send/Receive/Resend Group Tests"

print "TITLE:  Test send, receive, and resend with group addresses and two open instances\n"

#------------------------------------------------------
# variation:  open up cards, prepare for tests...
#------------------------------------------------------

variation

$OpenOne = open $G_TestCard $G_OpenFlag
if ($OpenOne == 0)
{
   block_variation
   print "BLOCKED:  unable to open card"
   goto endvar0
}

$OpenTwo = open $G_TestCard $G_OpenFlag
if ($OpenTwo == 0)
{
   block_variation
   print "BLOCKED:  unable to open card a second time"
   close $OpenOne
   goto endvar0
}

$ResendAddr     = array uchar 6
$SendAddr       = array uchar 6
$BroadcastAddr  = array uchar 6
$FunctionalAddr = array uchar 4
$GroupAddr      = array uchar 4
$NullAddr       = array uchar 4

$SendAddr[0]       = (uchar)0xC0
$SendAddr[1]       = (uchar)0x00
$SendAddr[2]       = (uchar)0x81
$SendAddr[3]       = (uchar)0x02
$SendAddr[4]       = (uchar)0x03
$SendAddr[5]       = (uchar)0x04

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

$BroadcastAddr[0] = (uchar)0xc0
$BroadcastAddr[1] = (uchar)0x00
$BroadcastAddr[2] = (uchar)0xff
$BroadcastAddr[3] = (uchar)0xff
$BroadcastAddr[4] = (uchar)0xff
$BroadcastAddr[5] = (uchar)0xff


$Filters = array long 8

$Filters[0]  = GROUP
$Filters[1]  = DIRECTED
$Filters[2]  = BROADCAST
$Filters[3]  = FUNCTIONAL
$NumFilters  = 4
$MaxTests    = 8

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
   $Filters[4] = $TempFilter
   $NumFilters = 5
   $MaxTests = 16
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
# outer loop:  pass 0--base filter is GROUP
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
         $ResendAddr[0] = $G_TestAddress[0]
         $ResendAddr[1] = $G_TestAddress[1]
         $ResendAddr[2] = $G_TestAddress[2]
         $ResendAddr[3] = $G_TestAddress[3]
         $ResendAddr[4] = $G_TestAddress[4]
         $ResendAddr[5] = $G_TestAddress[5]
         print "\n***Send Group, receive various, resend Directed***\n"
      }
      elseif ($FilterOne == BROADCAST)
      {
         $ResendAddr[0] = $BroadcastAddr[0]
         $ResendAddr[1] = $BroadcastAddr[1]
         $ResendAddr[2] = $BroadcastAddr[2]
         $ResendAddr[3] = $BroadcastAddr[3]
         $ResendAddr[4] = $BroadcastAddr[4]
         $ResendAddr[5] = $BroadcastAddr[5]
         print "\n***Send Group, receive various, resend Broadcast***\n"
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
         print "\n***Send Group, receive various, resend Functional***\n"
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
         $OneResendExp  = $NumPackets
         $OneReceiveExp = $NumPackets * 3
         print "\n***Send group, receive various, resend Group***\n"
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

      print "\n**Loop thru supported packet filters**\n"

      $Count = 0
      while ($Count < $MaxTests)
      {
         print "\nLOOPTOP-LOOPTOP-LOOPTOP-LOOPTOP-LOOPTOP-LOOPTOP-LOOPTOP-LOOPTOP-LOOPTOP\n"

         #
         # first set the packet filter
         #
         if ($Pass == 0)
         {
            $CurFilter = GROUP
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
            if ($FilterOne == GROUP)
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
            elseif ($FilterOne == FUNCTIONAL)
            {
               if ($CurFilter & (FUNCTIONAL | ALLFUNCTIONAL))
               {
                  $TwoReceiveExp = $NumPackets * 2
               }
            }
            elseif ($FilterOne == DIRECTED)
            {
               if ($CurFilter & DIRECTED)
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

               $result = send $OpenOne $SendAddr $PacketSize $NumPackets $ResendAddr
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
               if ($PacketSize == $G_MaxTotalSize)
               {
                  break
               }
               elseif ($PacketSize == $MinPacketSize)
               {
                  if ($Pass == 0)
                  {
                     if ($CurFilter & $Filters[1])
                     {
                        $PacketSize = ($G_MaxTotalSize + $MinPacketSize) / 2
                        if ( ($PacketSize & 0x0001) == 0)
                        {
                           $PacketSize = $PacketSize + 1
                        }
                     }
                     else
                     {
                        $PacketSize = $G_MaxTotalSize
                     }
                  }
                  else
                  {
                     break
                  }
               }
               else
               {
                  $PacketSize = $G_MaxTotalSize
               }
            }
            #
            # end of innermost loop
            #
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
            print "BLOCKED:  failed to set packet filter"
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
      if ($FilterOne == FUNCTIONAL)
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
# end of file 1m2o_13c.tst
############################################################


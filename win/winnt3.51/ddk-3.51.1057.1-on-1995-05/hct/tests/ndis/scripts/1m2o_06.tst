#######################################################
# FILE:  1m2o_06.tst
#
# These tests verify the ability to send
# from two different open instances..
#######################################################

friendly_script_name "Two Open Simple Send Tests"

print "TITLE:  Test ability to send from different open instances\n"

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



#---------------------------------------------------
# variation -- (pass 0) send to a random ("directed") address
#              (pass 1) send to the broadcast address
#              (pass 2) send to functional or multicast address
#              (pass 3) send to group address
#---------------------------------------------------

$DestAddr = array uchar 6
$DestAddr[0] = (uchar)0x00
$DestAddr[1] = (uchar)0x02
$DestAddr[2] = (uchar)0x04
$DestAddr[3] = (uchar)0x06
$DestAddr[4] = (uchar)0x08
$DestAddr[5] = (uchar)0x0a

$MinPacketSize = 40
$PacketSize  = $MinPacketSize
$NumPackets = $G_PacketsToSend
$PacketsSent = 0

print "\n***Test sending to a (random) directed address***\n"

$Pass = 0
while (TRUE)
{
   while (TRUE)
   {
      print " "
      variation

      $result1 = send $OpenOne $DestAddr $PacketSize $NumPackets
      $result2 = send $OpenTwo $DestAddr $PacketSize $NumPackets

      if ($result1 != TEST_SUCCESSFUL)
      {
         fail_variation
         print "FAILURE:  send on 1st open failed to start"
      }
      else
      {
         $result1 = waitsend $OpenOne
         if ($result1 != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILURE:  waitsend on 1st open failed"
         }
         else
         {
            $result1 = getsendresults $OpenOne $PacketsSent
            if ($result1 != TEST_SUCCESSFUL)
            {
               fail_variation
               print "FAILURE:  unable to get send results for 1st open"
            }
            elseif ($PacketsSent != $NumPackets)
            {
               fail_variation
               print "FAILURE:  should have sent" $NumPackets "packets"
            }
         }
      }

      if ($result2 != TEST_SUCCESSFUL)
      {
         fail_variation
         print "FAILURE:  send on 2nd open failed to start"
      }
      else
      {
         $result2 = waitsend $OpenTwo
         if ($result2 != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILURE:  waitsend on 2nd open failed"
         }
         else
         {
            $result2 = getsendresults $OpenTwo $PacketsSent
            if ($result2 != TEST_SUCCESSFUL)
            {
               fail_variation
               print "FAILURE:  unable to get send results for 2nd open"
            }
            elseif ($PacketsSent != $NumPackets)
            {
               fail_variation
               print "FAILURE:  should have sent" $NumPackets "packets"
            }
         }
      }

      if ($PacketSize == $G_MaxTotalSize)
      {
         break
      }
      elseif ($PacketSize == $MinPacketSize)
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

   #
   # set up for next test..
   #
   $PacketSize  = $MinPacketSize
   if ($Pass == 0)         # just finished random directed address
   {                       # set up for sending to broadcast address
      $Pass = 1
      if ($G_Media == MEDIUM_TOKENRING)
      {
         $DestAddr[0] = (uchar)0xc0
         $DestAddr[1] = (uchar)0x00
         $DestAddr[2] = (uchar)0xff
         $DestAddr[3] = (uchar)0xff
         $DestAddr[4] = (uchar)0xff
         $DestAddr[5] = (uchar)0xff
      }
      elseif ($G_Media == MEDIUM_ARCNET)
      {
         $DestAddr[0] = (uchar)0x00
         $DestAddr[1] = (uchar)0x00
         $DestAddr[2] = (uchar)0x00
         $DestAddr[3] = (uchar)0x00
         $DestAddr[4] = (uchar)0x00
         $DestAddr[5] = (uchar)0x00
      }
      else                 # MEDIUM_ETHERNET or MEDIUM_FDDI
      {
         $DestAddr[0] = (uchar)0xff
         $DestAddr[1] = (uchar)0xff
         $DestAddr[2] = (uchar)0xff
         $DestAddr[3] = (uchar)0xff
         $DestAddr[4] = (uchar)0xff
         $DestAddr[5] = (uchar)0xff
      }
      print "\n***Test sending to the broadcast address***\n"
   }

   elseif ($Pass == 1)     # just finished broadcast address
   {                       # set up for sending to functional or multicast address
      $Pass = 2
      if ($G_Media == MEDIUM_TOKENRING)
      {
         $DestAddr[0] = (uchar)0xc0
         $DestAddr[1] = (uchar)0x00
         $DestAddr[2] = (uchar)0x01
         $DestAddr[3] = (uchar)0x02
         $DestAddr[4] = (uchar)0x03
         $DestAddr[5] = (uchar)0x04
         print "\n***Test sending to a functional address***\n"
      }
      elseif ($G_Media == MEDIUM_ARCNET)
      {
         break
      }
      else                 # MEDIUM_ETHERNET or MEDIUM_FDDI
      {
         $DestAddr[0] = (uchar)0x01
         $DestAddr[1] = (uchar)0x02
         $DestAddr[2] = (uchar)0x03
         $DestAddr[3] = (uchar)0x04
         $DestAddr[4] = (uchar)0x05
         $DestAddr[5] = (uchar)0x06
         print "\n***Test sending to a multicast address***\n"
      }

   }

   elseif ($Pass == 2)     # just finished multicast or functional address
   {                       # set up for sending to group address
      $Pass = 3
      if ($G_Media == MEDIUM_TOKENRING)
      {
         $DestAddr[0] = (uchar)0xc0
         $DestAddr[1] = (uchar)0x00
         $DestAddr[2] = (uchar)0x81
         $DestAddr[3] = (uchar)0x02
         $DestAddr[4] = (uchar)0x03
         $DestAddr[5] = (uchar)0x04
         print "\n***Test sending to a group address***\n"
      }
      else
      {
         break
      }
   }

   else                    # just finished group address
   {                       # we are all done!
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
# end of file 1m2o_06.tst
############################################################


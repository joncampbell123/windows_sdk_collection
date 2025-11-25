###############################################################
# File 1m2c_08a.tst
#
# This script tests the ability to receive on multiple multicast
# addresses
###############################################################


friendly_script_name "Two Card Multiple Multicast Address Receive Tests"

print "TITLE:  Test receive on multiple multicast addresses\n"

#------------------------------------------------------
# variation:   set up for the tests..
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

if ($G_Media == MEDIUM_ETHERNET)
{
   $ListCode = OID_802_3_MULTICAST_LIST
   $SizeCode = OID_802_3_MAXIMUM_LIST_SIZE
}
elseif ($G_Media == MEDIUM_FDDI)
{
   $ListCode = OID_FDDI_LONG_MULTICAST_LIST
   $SizeCode = OID_FDDI_LONG_MAX_LIST_SIZE
}
else
{
   fail_variation
   print "FAILURE:  card not FDDI or Ethernet!"
   close $OpenOne
   close $OpenTwo
   goto endvar0
}


$MulticastAddress  = array uchar 6
$MulticastAddress[0] = (uchar)0x01
$MulticastAddress[1] = (uchar)0x02
$MulticastAddress[2] = (uchar)0x03
$MulticastAddress[3] = (uchar)0x04
$MulticastAddress[4] = (uchar)0x05
$MulticastAddress[5] = (uchar)0x00

$SendAddress = array uchar 6
$SendAddress[0] = (uchar)0x01
$SendAddress[1] = (uchar)0x02
$SendAddress[2] = (uchar)0x03
$SendAddress[3] = (uchar)0x04
$SendAddress[4] = (uchar)0x05
$SendAddress[5] = (uchar)0x00

$PacketSize = 40
$NumPackets = $G_PacketsToSend
$PacketsSent = 0
$Received = 0
$Resent = 0
$Transferred = 0


$MaxList = 0

$result = queryinfo $OpenTwo $SizeCode $MaxList
if ($result != TEST_SUCCESSFUL)
{
   fail_variation
   print "FAILURE:  unable to get maximum multicast list size"
   close $OpenOne
   close $OpenTwo
   goto endvar0
}
elseif ($MaxList == 0)
{
   fail_variation
   print "FAILURE:  list size may not equal zero"
   close $OpenOne
   close $OpenTwo
   goto endvar0
}
elseif ($MaxList > 64)
{
   warn_variation
   print "WARNING: list size greater than 64.  Only 64 will be tested"
}


$result = setpacketfilter $OpenTwo MULTICAST
if ($result != TEST_SUCCESSFUL)
{
   fail_variation
   print "FAILURE:  unable to set filter to MULTICAST"
   close $OpenOne
   close $OpenTwo
   goto endvar0
}

#---------------------------------------------------
# variation:   add multicast addresses, testing that we can receive on
#              all added addresses (and not on ones not added yet)
#---------------------------------------------------

print "\n***Add and test receive on multicast addresses***\n"


$Count = 0

while ($Count < $MaxList)
{
   print " "
   variation
   $MulticastAddress[5] = (uchar)$Count
   $result = addmulticast $OpenTwo $MulticastAddress
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE: unable to add multicast address"
      $MaxList = $Count
   }
   else
   {
      # first, test for good receives on all set multicast addresses

      $Index = 0
      while ($Index <= $Count)
      {
         print " "
         $result = startreceive $OpenTwo
         if ($result != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILURE:  receive failed to start"
         }
         $SendAddress[5] = (uchar)$Index

         $result = send $OpenOne $SendAddress $PacketSize $NumPackets
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

         # now check receives -- should get these..

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

            if ($Resent != 0)
            {
               fail_variation
               print "FAILURE:  no packets should have been resent"
            }
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

         $Index = $Index + 1
      }

      # now tests for no receives on multicast addresses not yet set

      if ($Index < $MaxList)
      {
         print " "
         $result = startreceive $OpenTwo
         if ($result != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILURE:  receive failed to start"
         }

         while ($Index < $MaxList)
         {
            $SendAddress[5] = (uchar)$Index

            $result = send $OpenOne $SendAddress $PacketSize $NumPackets
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
            $Index = $Index + 1
         }

         # now check receives -- should get none..

         print " "
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

            if (($Resent != 0) || ($Received != 0) || ($Transferred != 0))
            {
               fail_variation
               print "FAILURE:  no packets should have been received, transferred, or resent"
            }
         }
      }
   }

   $Count = $Count + 1
}

#--------------------------------------------------------
# variation:   try to delete these multicast addresses, checking to make sure
#              driver receives on all active ones (and only on active ones)
#--------------------------------------------------------

print "\n***Delete and check receives on multicast addresses***\n"



$Count = 0

while ($Count < $MaxList)
{
   print " "
   variation
   $MulticastAddress[5] = (uchar)$Count
   $result = deletemulticast $OpenTwo $MulticastAddress
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE: unable to delete multicast address"
   }
   else
   {
      # first, test for no receives on all deleted multicast addresses

      $Index = 0
      print " "
      $result = startreceive $OpenTwo
      if ($result != TEST_SUCCESSFUL)
      {
         fail_variation
         print "FAILURE:  receive failed to start"
      }

      while ($Index <= $Count)
      {
         $SendAddress[5] = (uchar)$Index

         $result = send $OpenOne $SendAddress $PacketSize $NumPackets
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

         $Index = $Index + 1
      }

      # now check receives -- should have none..

      print " "
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

         if (($Resent != 0) || ($Received != 0) || ($Transferred != 0))
         {
            fail_variation
            print "FAILURE:  no packets should have been received, transferred, or resent"
         }
      }

      # now check addresses that are still active

      while ($Index < $MaxList)
      {
         print " "
         $result = startreceive $OpenTwo
         if ($result != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILURE:  receive failed to start"
         }
         $SendAddress[5] = (uchar)$Index

         $result = send $OpenOne $SendAddress $PacketSize $NumPackets
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

         # now check receives -- should get these..

         print " "
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

            if ($Resent != 0)
            {
               fail_variation
               print "FAILURE:  no packets should have been resent"
            }
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

         $Index = $Index + 1
      }
   }
   $Count = $Count + 1
}


#------------------------------------------------------------
# cleanup
#------------------------------------------------------------


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
# end of file 1m1c_08a.tst
############################################################



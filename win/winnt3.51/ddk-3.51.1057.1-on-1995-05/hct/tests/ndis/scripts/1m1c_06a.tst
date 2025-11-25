###############################################################
# File 1m1c_06a.tst
#
# This script tests the ability to set multicast addresses
###############################################################


friendly_script_name "Single Open SetMulticastList Tests"

print "TITLE:  Test setting multicast addresses\n"

#------------------------------------------------------
# variation:   set up for the tests..
#------------------------------------------------------

variation

$OpenInstance = open $G_TestCard $G_OpenFlag
if ($OpenInstance == 0)
{
   block_variation
   print "BLOCKED:  Unable to open card"
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
   close $OpenInstance
   goto endvar0
}


$AddressList = array uchar 384    # 64*6
$ListLength = 0
$NewAddress  = array uchar 6
$MaxList = 0

$result = queryinfo $OpenInstance $SizeCode $MaxList
if ($result != TEST_SUCCESSFUL)
{
   fail_variation
   print "FAILURE:  unable to get maximum multicast list size"
   close $OpenInstance
   goto endvar0
}
elseif ($MaxList == 0)
{
   fail_variation
   print "FAILURE:  list size may not equal zero"
   close $OpenInstance
   goto endvar0
}
elseif ($MaxList > 64)
{
   warn_variation
   print "WARNING: list size greater than 64.  Only 64 will be tested"
}


#---------------------------------------------------
# variation:   add/delete a multicast address multiple times (25)
#---------------------------------------------------

$NewAddress[0] = (uchar)0x01
$NewAddress[1] = (uchar)0x02
$NewAddress[2] = (uchar)0x03
$NewAddress[3] = (uchar)0x04
$NewAddress[4] = (uchar)0x05
$NewAddress[5] = (uchar)0x06

print "\n***Add and delete a multicast address multiple times***\n"
variation

$result = queryinfo $OpenInstance $ListCode $AddressList $ListLength
if ($result != TEST_SUCCESSFUL)
{
   fail_variation
   print "FAILURE:  query for multicast list failed"
}
elseif ($ListLength != 0)
{
   fail_variation
   print "FAILURE:  multicast list should be empty."
}


$Count = 0
while ($Count < 25)
{
   print " "
   variation

   $result = addmulticast $OpenInstance $NewAddress
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE: unable to add multicast address"
   }

   $result = queryinfo $OpenInstance $ListCode $AddressList $ListLength
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  query for multicast list failed"
   }
   elseif ($ListLength != 1)
   {
      fail_variation
      print "FAILURE:  multicast list should only have 1 entry"
   }
   else
   {
      $Index = 0
      while ($Index < 6)
      {
         if ($NewAddress[$Index] != $AddressList[$Index])
         {
            fail_variation
            print "FAILURE:  incorrect multicast address returned"
            break
         }
         $Index = $Index + 1
      }
   }
   print " "
   variation

   $result = deletemulticast $OpenInstance $NewAddress
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE: unable to delete multicast address"
   }
   $result = queryinfo $OpenInstance $ListCode $AddressList $ListLength
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  query for multicast list failed"
   }
   elseif ($ListLength != 0)
   {
      fail_variation
      print "FAILURE:  multicast list should be empty."
   }

   $Count = $Count + 1
}


#--------------------------------------------------------
# variation:   try to add as many different multicast addresses as the
#              driver says it supports -- then try to delete them again..
#--------------------------------------------------------
# NOTE:  need to add check for multicast addresses--make sure they are correct

$NewAddress[0] = (uchar)0x01
$NewAddress[1] = (uchar)0x02
$NewAddress[2] = (uchar)0x03
$NewAddress[3] = (uchar)0x04
$NewAddress[4] = (uchar)0x05
$NewAddress[5] = (uchar)0x00

print "\n***Add as many multicast addresses as driver says it supports***\n"

$Count = 0
while ($Count < $MaxList)
{
   print " "
   variation

   $NewAddress[5] = (uchar)$Count

   $result = addmulticast $OpenInstance $NewAddress
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE: unable to add multicast address"
   }

   $result = queryinfo $OpenInstance $ListCode $AddressList $ListLength
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  query for multicast list failed"
   }
   elseif ($ListLength != ($Count+1))
   {
      fail_variation
      print "FAILURE:  multicast list should have " ($Count+1) " entries"
   }

   $Count = $Count + 1
}


print "\n***Now delete all these added multicast addresses***\n"

$Count = 0
while ($Count < $MaxList)
{
   print " "
   variation

   $NewAddress[5] = (uchar)$Count

   $result = deletemulticast $OpenInstance $NewAddress
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE: unable to delete multicast address"
   }

   $result = queryinfo $OpenInstance $ListCode $AddressList $ListLength
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  query for multicast list failed"
   }
   elseif ($ListLength != ($MaxList - ($Count+1)))
   {
      fail_variation
      print "FAILURE:  multicast list should have " ($MaxList-($Count+1)) "entries"
   }

   $Count = $Count + 1
}

#------------------------------------------------------------
# cleanup
#------------------------------------------------------------


print " "
variation
$result = getevents $OpenInstance
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

$result = close $OpenInstance
if ($result != TEST_SUCCESSFUL)
{
   fail_variation
   print "FAILURE:  close failed."
}

:endvar0

end

############################################################
# end of file 1m1c_06a.tst
############################################################



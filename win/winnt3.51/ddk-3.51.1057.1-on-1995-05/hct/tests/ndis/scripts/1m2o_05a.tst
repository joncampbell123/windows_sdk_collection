#######################################################
# FILE:  1m2o_05a.tst
#
# These tests verify the ability to set the multicast addresses
# from two different open instances..
#######################################################

friendly_script_name "Two Open SetMulticastList Tests"

print "TITLE:  Test ability to set multicast addresses on different open instances\n"

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
   goto endvar1
}


$AddressList = array uchar 384    # 64*6
$ListLength  = 0
$NewAddr1    = array uchar 6
$NewAddr2    = array uchar 6
$MaxList     = 0


$result = queryinfo $OpenOne $SizeCode $MaxList
if ($result != TEST_SUCCESSFUL)
{
   fail_variation
   print "FAILURE:  unable to get maximum multicast list size"
   goto endvar1
}
$MaxList = $MaxList / 2

if ($MaxList == 0)
{
   fail_variation
   print "FAILURE:  list size may not equal zero"
   goto endvar1
}
elseif ($MaxList > 64)
{
   warn_variation
   print "WARNING: list size greater than 64.  64 will be used"
   $MaxList = 64
}


#---------------------------------------------------
# variation:   add/delete a multicast address multiple times (25)
#---------------------------------------------------

$NewAddr1[0] = (uchar)0x01
$NewAddr1[1] = (uchar)0x02
$NewAddr1[2] = (uchar)0x03
$NewAddr1[3] = (uchar)0x04
$NewAddr1[4] = (uchar)0x05
$NewAddr1[5] = (uchar)0x06

$NewAddr2[0] = (uchar)0x11
$NewAddr2[1] = (uchar)0x02
$NewAddr2[2] = (uchar)0x03
$NewAddr2[3] = (uchar)0x04
$NewAddr2[4] = (uchar)0x05
$NewAddr2[5] = (uchar)0x06


print " "
variation
$result = queryinfo $OpenOne $ListCode $AddressList $ListLength
if ($result != TEST_SUCCESSFUL)
{
   fail_variation
   print "FAILURE:  query for open 1 multicast list failed"
}
elseif ($ListLength != 0)
{
   fail_variation
   print "FAILURE:  open 1 multicast list should be empty."
}

$result = queryinfo $OpenTwo $ListCode $AddressList $ListLength
if ($result != TEST_SUCCESSFUL)
{
   fail_variation
   print "FAILURE:  query for open 2 multicast list failed"
}
elseif ($ListLength != 0)
{
   fail_variation
   print "FAILURE:  open 2 multicast list should be empty."
}


print "\n***Repeatedly add and delete a multicast address on each open instance***\n"

$Count = 0
while ($Count < 25)
{
   print " "
   variation

   $result = addmulticast $OpenOne $NewAddr1
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE: unable to add multicast address"
   }

   $result = addmulticast $OpenTwo $NewAddr2
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE: unable to add multicast address"
   }

   $result = queryinfo $OpenOne $ListCode $AddressList $ListLength
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  query for open 1 multicast list failed"
   }
   elseif ($ListLength != 1)
   {
      fail_variation
      print "FAILURE:  open 1 multicast list should have 1 entry"
   }
   else
   {
      $Index = 0
      while ($Index < 6)
      {
         if ($NewAddr1[$Index] != $AddressList[$Index])
         {
            fail_variation
            print "FAILURE:  incorrect multicast address returned"
            break
         }
         $Index = $Index + 1
      }
   }

   $result = queryinfo $OpenTwo $ListCode $AddressList $ListLength
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  query for open 2 multicast list failed"
   }
   elseif ($ListLength != 1)
   {
      fail_variation
      print "FAILURE:  open 2 multicast list should have 1 entry"
   }
   else
   {
      $Index = 0
      while ($Index < 6)
      {
         if ($NewAddr2[$Index] != $AddressList[$Index])
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

   if ( ($Count & 0x01) == 0x01)
   {
      $result = deletemulticast $OpenOne $NewAddr1
   }
   else
   {
      $result = deletemulticast $OpenTwo $NewAddr2
   }
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE: unable to delete multicast address"
   }

   if ( ($Count & 0x01) == 0x01)
   {
      $result = deletemulticast $OpenTwo $NewAddr2
   }
   else
   {
      $result = deletemulticast $OpenOne $NewAddr1
   }
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE: unable to delete multicast address"
   }

   $result = queryinfo $OpenOne $ListCode $AddressList $ListLength
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  query for open 1 multicast list failed"
   }
   elseif ($ListLength != 0)
   {
      fail_variation
      print "FAILURE:  open 1 multicast list should be empty."
   }

   $result = queryinfo $OpenTwo $ListCode $AddressList $ListLength
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  query for open 2 multicast list failed"
   }
   elseif ($ListLength != 0)
   {
      fail_variation
      print "FAILURE:  open 2 multicast list should be empty."
   }

   $Count = $Count + 1
}


#--------------------------------------------------------
# variation:   try to add as many different multicast addresses as the
#              driver says it supports -- then try to delete them again..
#--------------------------------------------------------
# NOTE:  need to add check that correct multicast addresses are returned

$NewAddr1[0] = (uchar)0x01
$NewAddr1[1] = (uchar)0x02
$NewAddr1[2] = (uchar)0x03
$NewAddr1[3] = (uchar)0x04
$NewAddr1[4] = (uchar)0x05
$NewAddr1[5] = (uchar)0x00

$NewAddr2[0] = (uchar)0x11
$NewAddr2[1] = (uchar)0x02
$NewAddr2[2] = (uchar)0x03
$NewAddr2[3] = (uchar)0x04
$NewAddr2[4] = (uchar)0x05
$NewAddr2[5] = (uchar)0x00

print "\n***Add half of the maximum list size multicast addresses to each open instance***\n"

$Count = 0
while ($Count < $MaxList)
{
   print " "
   variation

   $NewAddr1[5] = (uchar)$Count
   $NewAddr2[5] = (uchar)$Count

   if ( ($Count & 0x01) == 0x01 )
   {
      $result = addmulticast $OpenOne $NewAddr1
   }
   else
   {
      $result = addmulticast $OpenTwo $NewAddr2
   }
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE: unable to add multicast address"
   }

   if ( ($Count & 0x01) == 0x01 )
   {
      $result = addmulticast $OpenTwo $NewAddr2
   }
   else
   {
      $result = addmulticast $OpenOne $NewAddr1
   }
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE: unable to add multicast address"
   }

   print " "
   variation

   $result = queryinfo $OpenOne $ListCode $AddressList $ListLength
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  query for open 1 multicast list failed"
   }
   elseif ($ListLength != ($Count+1))
   {
      fail_variation
      print "FAILURE:  open 1 multicast list should have" ($Count+1) "entries"
   }

   $result = queryinfo $OpenTwo $ListCode $AddressList $ListLength
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  query for open 2 multicast list failed"
   }
   elseif ($ListLength != ($Count+1))
   {
      fail_variation
      print "FAILURE:  open 2 multicast list should have" ($Count+1) "entries"
   }

   $Count = $Count + 1
}


print "\n***Now try to delete all these addresses***\n"

$Count = 0
while ($Count < $MaxList)
{
   print " "
   variation

   $NewAddr1[5] = (uchar)$Count
   $NewAddr2[5] = (uchar)$Count

   if ( ($Count & 0x01) == 0x01)
   {
      $result = deletemulticast $OpenTwo $NewAddr2
   }
   else
   {
      $result = deletemulticast $OpenOne $NewAddr1
   }
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE: unable to delete multicast address"
   }

   if ( ($Count & 0x01) == 0x01)
   {
      $result = deletemulticast $OpenOne $NewAddr1
   }
   else
   {
      $result = deletemulticast $OpenTwo $NewAddr2
   }
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE: unable to delete multicast address"
   }

   $result = queryinfo $OpenOne $ListCode $AddressList $ListLength
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  query for open 1 multicast list failed"
   }
   elseif ($ListLength != ($MaxList - ($Count+1)))
   {
      fail_variation
      print "FAILURE:  open 1 multicast list should have" ($MaxList-($Count+1)) "entries"
   }

   $result = queryinfo $OpenTwo $ListCode $AddressList $ListLength
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  query for open 2 multicast list failed"
   }
   elseif ($ListLength != ($MaxList - ($Count+1)))
   {
      fail_variation
      print "FAILURE:  open 2 multicast list should have" ($MaxList-($Count+1)) "entries"
   }

   $Count = $Count + 1
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
# end of file 1m2o_05a.tst
############################################################


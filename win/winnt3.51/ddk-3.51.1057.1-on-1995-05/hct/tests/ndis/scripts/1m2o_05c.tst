#######################################################
# FILE:  1m2o_05c.tst
#
# These tests verify the ability to set the group addresses
# from two different open instances..
#######################################################

friendly_script_name "Two Open SetGroupAddress Tests"

print "TITLE:  Test ability to set group address from different open instances\n"

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


$NewGroup1 = array uchar 4
$NewGroup2 = array uchar 4
$NullGroup = array uchar 4
$CurGroup1 = array uchar 4
$CurGroup2 = array uchar 4

$NullGroup[0] = (uchar)0x00
$NullGroup[1] = (uchar)0x00
$NullGroup[2] = (uchar)0x00
$NullGroup[3] = (uchar)0x00

$NewGroup1[0] = (uchar)0x81
$NewGroup1[1] = (uchar)0x02
$NewGroup1[2] = (uchar)0x03
$NewGroup1[3] = (uchar)0x04

$NewGroup2[0] = (uchar)0x81
$NewGroup2[1] = (uchar)0x02
$NewGroup2[2] = (uchar)0x03
$NewGroup2[3] = (uchar)0x14



print " "
variation

$result = queryinfo $OpenOne OID_802_5_CURRENT_GROUP $CurGroup1
if ($result != TEST_SUCCESSFUL)
{
   fail_variation
   print "FAILURE:  unable to query group address on 1st open"
}
$result = queryinfo $OpenTwo OID_802_5_CURRENT_GROUP $CurGroup2
if ($result != TEST_SUCCESSFUL)
{
   fail_variation
   print "FAILURE:  unable to query group address on 2nd open"
}

$Index = 0
while ($Index < 4)
{
   if (($CurGroup1[$Index] != $NullGroup[$Index]) || ($CurGroup2[$Index] != $NullGroup[$Index]))
   {
      fail_variation
      print "FAILURE:  incorrect group address"
      break
   }
   $Index = $Index + 1
}


#---------------------------------------------------------
# variation:   set/clear the same group address multiple times
#---------------------------------------------------------

print "\n***Repeatedly set/clear the same group address***\n"

$Count = 0
while ($Count < 32)
{
   print " "
   variation

   if ( ($Count & 0x01) == 0x01)
   {
      $result1 = setgroup $OpenOne $NewGroup1
      $result2 = setgroup $OpenTwo $NewGroup1
   }
   else
   {
      $result2 = setgroup $OpenTwo $NewGroup1
      $result1 = setgroup $OpenOne $NewGroup1
   }
   if ($result1 != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to set group address for 1st open"
   }
   if ($result2 != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to set group address for 2nd open"
   }

   $result = queryinfo $OpenOne OID_802_5_CURRENT_GROUP $CurGroup1
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to query group address on 1st open"
   }
   $result = queryinfo $OpenTwo OID_802_5_CURRENT_GROUP $CurGroup2
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to query group address on 2nd open"
   }

   $Index = 0
   while ($Index < 4)
   {
      if ($CurGroup1[$Index] != $NewGroup1[$Index])
      {
         fail_variation
         print "FAILURE:  incorrect group address from 1st open"
         break
      }
      if ($CurGroup2[$Index] != $NewGroup1[$Index])
      {
         fail_variation
         print "FAILURE:  incorrect group address from 2nd open"
         break
      }
      $Index = $Index + 1
   }

   print " "
   variation

   if ( ($Count & 0x02) == 0x02)
   {
      $result1 = setgroup $OpenOne $NullGroup
      $result2 = setgroup $OpenTwo $NullGroup
   }
   else
   {
      $result2 = setgroup $OpenTwo $NullGroup
      $result1 = setgroup $OpenOne $NullGroup
   }
   if ($result1 != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to clear group address for 1st open"
   }
   if ($result2 != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to clear group address for 2nd open"
   }

   $result = queryinfo $OpenOne OID_802_5_CURRENT_GROUP $CurGroup1
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to query group address on 1st open"
   }

   $result = queryinfo $OpenTwo OID_802_5_CURRENT_GROUP $CurGroup2
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to query group address on 2nd open"
   }

   $Index = 0
   while ($Index < 4)
   {
      if ($CurGroup1[$Index] != $NullGroup[$Index])
      {
         fail_variation
         print "FAILURE:  incorrect group address from 1st open"
         break
      }
      if ($CurGroup2[$Index] != $NullGroup[$Index])
      {
         fail_variation
         print "FAILURE:  incorrect group address from 2nd open"
         break
      }
      $Index = $Index + 1
   }


   $Count = $Count + 1
}




#---------------------------------------------------------
# variation:   try to set to different group addresses.
#              First should succeed, second should fail...
#---------------------------------------------------------

print "\n***Try to set different group addresses on each open instance***\n"

$Count = 0
while ($Count < 32)
{
   print " "
   variation

   if ( ($Count & 0x01) == 0x01)
   {
      $result1 = setgroup $OpenOne $NewGroup1
#      $result = queryinfo $OpenTwo OID_802_5_CURRENT_GROUP
      $result2 = setgroup $OpenTwo $NewGroup2
   }
   else
   {
      $result1 = setgroup $OpenTwo $NewGroup2
#      $result = queryinfo $OpenOne OID_802_5_CURRENT_GROUP
      $result2 = setgroup $OpenOne $NewGroup1
   }
   if ($result1 != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to set first group address"
   }
   if ($result2 == TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  should not be able to set a second group address"
   }

   $result = queryinfo $OpenOne OID_802_5_CURRENT_GROUP $CurGroup1
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to query group address on 1st open"
   }
   $result = queryinfo $OpenTwo OID_802_5_CURRENT_GROUP $CurGroup2
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to query group address on 2nd open"
   }

   #
   # check group address on first open instance we tried to set
   # it should have succeeded
   #
   $Index = 0
   while ($Index < 4)
   {
      if ( ($Count & 0x01) == 0x01)
      {
         if ($CurGroup1[$Index] != $NewGroup1[$Index])
         {
            fail_variation
            print "FAILURE:  incorrect group address from 1st open"
            break
         }
      }
      else
      {
         if ($CurGroup2[$Index] != $NewGroup2[$Index])
         {
            fail_variation
            print "FAILURE:  incorrect group address from 2nd open"
            break
         }
      }
      $Index = $Index + 1
   }

   #
   # check group address on second open instance we tried to set
   # it should have failed.  The value returned should be either the same
   # as for the successfully set open instance, or the NULL address
   #
   $Index = 0
   $NullError = FALSE
   $AddrError = FALSE
   while ($Index < 4)
   {
      if ( ($Count & 0x01) == 0x01)
      {
         if ($CurGroup2[$Index] != $NewGroup1[$Index])
         {
            $AddrError = TRUE
         }
         if ($CurGroup2[$Index] != $NullGroup[$Index])
         {
            $NullError = TRUE
         }
         if ($NullError && $AddrError)
         {
            fail_variation
            print "FAILURE:  incorrect group address from 2nd open"
            break
         }
      }
      else
      {
         if ($CurGroup1[$Index] != $NewGroup2[$Index])
         {
            $AddrError = TRUE
         }
         if ($CurGroup1[$Index] != $NullGroup[$Index])
         {
            $NullError = TRUE
         }
         if ($NullError && $AddrError)
         {

            fail_variation
            print "FAILURE:  incorrect group address from 1st open"
            break
         }
      }
      $Index = $Index + 1
   }

   print " "
   variation

#
# try to clear the group addresses address
#
   if ( ($Count & 0x02) == 0x02)
   {
      $result1 = setgroup $OpenOne $NullGroup
#      $result = queryinfo $OpenTwo OID_802_5_CURRENT_GROUP
      $result2 = setgroup $OpenTwo $NullGroup

      if ( ($Count & 0x01) == 0x01)
      {
         if ($result1 != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILURE:  unable to clear group address for 1st open"
         }
      }
      else
      {
         if ($result1 == TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILURE:  should not have been able to clear group address for 1nd open"
         }
      }
      if ($result2 != TEST_SUCCESSFUL)
      {
         fail_variation
         print "FAILURE:  unable to clear group address for 2nd open"
      }
   }


   else
   {
      $result2 = setgroup $OpenTwo $NullGroup
#      $result = queryinfo $OpenOne OID_802_5_CURRENT_GROUP
      $result1 = setgroup $OpenOne $NullGroup

      if (($Count & 0x01 == 0))
      {
         if ($result2 != TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILURE:  unable to clear group address for 2nd open"
         }
      }
      else
      {
         if ($result2 == TEST_SUCCESSFUL)
         {
            fail_variation
            print "FAILURE:  should not have been able to clear group address for 2nd open"
         }
      }
      if ($result1 != TEST_SUCCESSFUL)
      {
         fail_variation
         print "FAILURE:  unable to clear group address for 1st open"
      }
   }


   $result = queryinfo $OpenOne OID_802_5_CURRENT_GROUP $CurGroup1
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to query group address on 1st open"
   }

   $result = queryinfo $OpenTwo OID_802_5_CURRENT_GROUP $CurGroup2
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to query group address on 2nd open"
   }

   $Index = 0
   while ($Index < 4)
   {
      if ($CurGroup1[$Index] != $NullGroup[$Index])
      {
         fail_variation
         print "FAILURE:  incorrect group address from 1st open"
         break
      }
      if ($CurGroup2[$Index] != $NullGroup[$Index])
      {
         fail_variation
         print "FAILURE:  incorrect group address from 2nd open"
         break
      }
      $Index = $Index + 1
   }

   $Count = $Count + 1
}

#---------------------------------------------
# variation:  set both to same group address,
#             then try to change...
#---------------------------------------------

print "\n***Set same group address on both open instances, then try to change one***\n"

$Count = 0
while ($Count < 32)
{
   print " "
   variation

   $NewGroup1[3] = (uchar)$Count
   $NewGroup2[3] = (uchar)($Count + 0x40)

   $result1 = setgroup $OpenOne $NewGroup1
   $result2 = setgroup $OpenTwo $NewGroup1
   if ($result1 != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to set group address for 1st open"
   }
   if ($result2 != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to set group address for 2nd open"
   }

   $result = queryinfo $OpenOne OID_802_5_CURRENT_GROUP $CurGroup1
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to query group address on 1st open"
   }
   $result = queryinfo $OpenTwo OID_802_5_CURRENT_GROUP $CurGroup2
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to query group address on 2nd open"
   }

   $Index = 0
   while ($Index < 4)
   {
      if ($CurGroup1[$Index] != $NewGroup1[$Index])
      {
         fail_variation
         print "FAILURE:  incorrect group address from 1st open"
         break
      }
      if ($CurGroup2[$Index] != $NewGroup1[$Index])
      {
         fail_variation
         print "FAILURE:  incorrect group address from 2nd open"
         break
      }
      $Index = $Index + 1
   }

   print " "
   variation

   $result1 = setgroup $OpenOne $NewGroup2
   $result2 = setgroup $OpenTwo $NewGroup2
   if ( ($result1 == TEST_SUCCESSFUL) || ($result2 == TEST_SUCCESSFUL))
   {
      fail_variation
      print "FAILURE:  May not change in-use group address"
   }

   print " "
   variation

   if ( ($Count & 0x01) == 0x01)
   {
      $result1 = setgroup $OpenOne $NullGroup
   }
   else
   {
      $result1 = setgroup $OpenTwo $NullGroup
   }
   if ($result1 != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to clear group address"
   }

   if ( ($Count & 0x02) == 0x02)
   {
      $result1 = setgroup $OpenOne $NewGroup2
   }
   else
   {
      $result1 = setgroup $OpenTwo $NewGroup2
   }
   if ( (($Count & 0x03) == 0x02) || (($Count & 0x03) == 0x01))
   {
      if ($result1 != TEST_SUCCESSFUL)
      {
         fail_variation
         print "FAILURE:  failed to set group address"
      }
   }
   else
   {
      if ($result1 == TEST_SUCCESSFUL)
      {
         fail_variation
         print "FAILURE:  May not change an in-use group address"
      }
   }

   print " "
   variation

   if (($Count & 0x01) == 0x01)
   {
      $result2 = setgroup $OpenTwo $NullGroup
      $result1 = setgroup $OpenOne $NullGroup
   }
   else
   {
      $result1 = setgroup $OpenOne $NullGroup
      $result2 = setgroup $OpenTwo $NullGroup
   }

   if ($result1 != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to clear group address for 1st open"
   }
   if ($result2 != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to clear group address for 2nd open"
   }

   $result = queryinfo $OpenOne OID_802_5_CURRENT_GROUP $CurGroup1
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to query group address on 1st open"
   }

   $result = queryinfo $OpenTwo OID_802_5_CURRENT_GROUP $CurGroup2
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to query group address on 2nd open"
   }

   $Index = 0
   while ($Index < 4)
   {
      if ($CurGroup1[$Index] != $NullGroup[$Index])
      {
         fail_variation
         print "FAILURE:  incorrect group address from 1st open"
         break
      }
      if ($CurGroup2[$Index] != $NullGroup[$Index])
      {
         fail_variation
         print "FAILURE:  incorrect group address from 2nd open"
         break
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
# end of file 1m2o_05c.tst
############################################################


###############################################################
# File 1m1c_06b.tst
#
# This script tests the ability to set functional and group addresses
###############################################################


friendly_script_name "Single Open SetFunctionalAddress Tests"

print "TITLE:  Tests ability to set functional address\n"

#------------------------------------------------------
# variation:   set up for the tests
#------------------------------------------------------

variation

$OpenInstance = open $G_TestCard $G_OpenFlag
if ($OpenInstance == 0)
{
   block_variation
   print "BLOCKED:  Unable to open card"
   goto endvar0
}


$NewFunctional  = array uchar 4
$NullFunctional = array uchar 4
$CurFunctional  = array uchar 4

$NullFunctional[0] = (uchar)0x00
$NullFunctional[1] = (uchar)0x00
$NullFunctional[2] = (uchar)0x00
$NullFunctional[3] = (uchar)0x00

$NewFunctional[0] = (uchar)0x01
$NewFunctional[1] = (uchar)0x02
$NewFunctional[2] = (uchar)0x03
$NewFunctional[3] = (uchar)0x04

print " "
variation

$result = queryinfo $OpenInstance OID_802_5_CURRENT_FUNCTIONAL $CurFunctional
if ($result != TEST_SUCCESSFUL)
{
   fail_variation
   print "FAILURE:  unable to query current functional address"
}
else
{
   $Index = 0
   while ($Index < 4)
   {
      if ($CurFunctional[$Index] != $NullFunctional[$Index])
      {
         fail_variation
         print "FAILURE:  incorrect functional address returned"
         break
      }
      $Index = $Index + 1
   }
}

#---------------------------------------------------------
# variation:   set/clear the same functional address multiple times
#---------------------------------------------------------

print "\n***Set/clear a functional address multiple times***\n"

$Count = 0
while ($Count < 25)
{
   print " "
   variation

   $result = setfunctional $OpenInstance $NewFunctional
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to set functional address"
   }

   $result = queryinfo $OpenInstance OID_802_5_CURRENT_FUNCTIONAL $CurFunctional
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to query current functional address"
   }
   else
   {
      $Index = 0
      while ($Index < 4)
      {
         if ($CurFunctional[$Index] != $NewFunctional[$Index])
         {
            fail_variation
            print "FAILURE:  incorrect functional address returned"
            break
         }
         $Index = $Index + 1
      }
   }
   print " "
   variation

   $result = setfunctional $OpenInstance $NullFunctional
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to clear functional address"
   }

   $result = queryinfo $OpenInstance OID_802_5_CURRENT_FUNCTIONAL $CurFunctional
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to query current functional address"
   }
   else
   {
      $Index = 0
      while ($Index < 4)
      {
         if ($CurFunctional[$Index] != $NullFunctional[$Index])
         {
            fail_variation
            print "FAILURE:  incorrect functional address returned"
            break
         }
         $Index = $Index + 1
      }
   }


   $Count = $Count + 1
}


#----------------------------------------------------
# variation:   set/clear different functional addresses
#----------------------------------------------------

print "\n***Set/clear a series of functional addresses***\n"

$Count = 0
while ($Count < 32)
{
   print " "
   variation

   $NewFunctional[3] = (uchar)$Count
   $result = setfunctional $OpenInstance $NewFunctional
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to set functional address"
   }

   $result = queryinfo $OpenInstance OID_802_5_CURRENT_FUNCTIONAL $CurFunctional
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to query current functional address"
   }
   else
   {
      $Index = 0
      while ($Index < 4)
      {
         if ($CurFunctional[$Index] != $NewFunctional[$Index])
         {
            fail_variation
            print "FAILURE:  incorrect functional address returned"
            break
         }
         $Index = $Index + 1
      }
   }

   print " "
   variation

   $result = setfunctional $OpenInstance $NullFunctional
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to clear functional address"
   }

   $result = queryinfo $OpenInstance OID_802_5_CURRENT_FUNCTIONAL $CurFunctional
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to query current functional address"
   }
   else
   {
      $Index = 0
      while ($Index < 4)
      {
         if ($CurFunctional[$Index] != $NullFunctional[$Index])
         {
            fail_variation
            print "FAILURE:  incorrect functional address returned"
            break
         }
         $Index = $Index + 1
      }
   }


   $Count = $Count + 1
}

#
# now do same with group addresses
#

#---------------------------------------------------------
# variation:   set up for group address tests
#---------------------------------------------------------

friendly_script_name "Single Open SetGroupAddress Tests"

print "\n\nTITLE:  Test ability to set group address\n"

$NewFunctional[0] = (uchar)0x81
$NewFunctional[1] = (uchar)0x02
$NewFunctional[2] = (uchar)0x03
$NewFunctional[3] = (uchar)0x04

variation

$result = queryinfo $OpenInstance OID_802_5_CURRENT_GROUP $CurFunctional
if ($result != TEST_SUCCESSFUL)
{
   fail_variation
   print "FAILURE:  unable to query current group address"
}
else
{
   $Index = 0
   while ($Index < 4)
   {
      if ($CurFunctional[$Index] != $NullFunctional[$Index])
      {
         fail_variation
         print "FAILURE:  incorrect group address returned"
         break
      }
      $Index = $Index + 1
   }
}

#---------------------------------------------------------
# variation:   set/clear the same group address multiple times
#---------------------------------------------------------

print "\n***Set/clear a group address multiple times***\n"

$Count = 0
while ($Count < 25)
{
   print " "
   variation

   $result = setgroup $OpenInstance $NewFunctional
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to set group address"
   }

   $result = queryinfo $OpenInstance OID_802_5_CURRENT_GROUP $CurFunctional
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to query current group address"
   }
   else
   {
      $Index = 0
      while ($Index < 4)
      {
         if ($CurFunctional[$Index] != $NewFunctional[$Index])
         {
            fail_variation
            print "FAILURE:  incorrect group address returned"
            break
         }
         $Index = $Index + 1
      }
   }

   print " "
   variation

   $result = setgroup $OpenInstance $NullFunctional
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to clear group address"
   }

   $result = queryinfo $OpenInstance OID_802_5_CURRENT_GROUP $CurFunctional
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to query current group address"
   }
   else
   {
      $Index = 0
      while ($Index < 4)
      {
         if ($CurFunctional[$Index] != $NullFunctional[$Index])
         {
            fail_variation
            print "FAILURE:  incorrect group address returned"
            break
         }
         $Index = $Index + 1
      }
   }


   $Count = $Count + 1
}

#----------------------------------------------------
# variation:   set/clear different group addresses
#----------------------------------------------------

print "\n***Set/clear a series of group addresses***\n"

$Count = 0
while ($Count < 32)
{
   print " "
   variation

   $NewFunctional[3] = (uchar)$Count
   $result = setgroup $OpenInstance $NewFunctional
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to set group address"
   }

   $result = queryinfo $OpenInstance OID_802_5_CURRENT_GROUP $CurFunctional
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to query current group address"
   }
   else
   {
      $Index = 0
      while ($Index < 4)
      {
         if ($CurFunctional[$Index] != $NewFunctional[$Index])
         {
            fail_variation
            print "FAILURE:  incorrect group address returned"
            break
         }
         $Index = $Index + 1
      }
   }
   print " "
   variation

   $result = setgroup $OpenInstance $NullFunctional
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to clear group address"
   }

   $result = queryinfo $OpenInstance OID_802_5_CURRENT_GROUP $CurFunctional
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to query current group address"
   }
   else
   {
      $Index = 0
      while ($Index < 4)
      {
         if ($CurFunctional[$Index] != $NullFunctional[$Index])
         {
            fail_variation
            print "FAILURE:  incorrect group address returned"
            break
         }
         $Index = $Index + 1
      }
   }


   $Count = $Count + 1
}


#----------------------------------------------------
# cleanup
#----------------------------------------------------

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
# end of file 1m1c_06b.tst
############################################################



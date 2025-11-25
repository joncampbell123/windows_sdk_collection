#######################################################
# FILE:  1m2o_05b.tst
#
# These tests verify the ability to set the functional and group addresses
# from two different open instances..
#######################################################

friendly_script_name "Two Open SetFunctionalAddress Tests"

print "TITLE:  Test ability to set the functional address from different open instances\n"

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


$NewFunct1 = array uchar 4
$NewFunct2 = array uchar 4
$NullFunct = array uchar 4
$CurFunct1 = array uchar 4
$CurFunct2 = array uchar 4

$NullFunct[0] = (uchar)0x00
$NullFunct[1] = (uchar)0x00
$NullFunct[2] = (uchar)0x00
$NullFunct[3] = (uchar)0x00

$NewFunct1[0] = (uchar)0x01
$NewFunct1[1] = (uchar)0x02
$NewFunct1[2] = (uchar)0x03
$NewFunct1[3] = (uchar)0x04

$NewFunct2[0] = (uchar)0x01
$NewFunct2[1] = (uchar)0x02
$NewFunct2[2] = (uchar)0x03
$NewFunct2[3] = (uchar)0x14



print " "
variation

$result = queryinfo $OpenOne OID_802_5_CURRENT_FUNCTIONAL $CurFunct1
if ($result != TEST_SUCCESSFUL)
{
   fail_variation
   print "FAILURE:  unable to query open 1 functional address"
}
$result = queryinfo $OpenTwo OID_802_5_CURRENT_FUNCTIONAL $CurFunct2
if ($result != TEST_SUCCESSFUL)
{
   fail_variation
   print "FAILURE:  unable to query open 2 functional address"
}

$Index = 0
while ($Index < 4)
{
   if (($CurFunct1[$Index] != $NullFunct[$Index]) || ($CurFunct2[$Index] != $NullFunct[$Index]))
   {
      fail_variation
      print "FAILURE:  incorrect functional address"
      break
   }
   $Index = $Index + 1
}

#---------------------------------------------------------
# variation:   set/unset the same functional address multiple times
#---------------------------------------------------------

print "\n***Repeatedly set/clear a single functional address on each open instance***\n"
$Count = 0
while ($Count < 25)
{
   print " "
   variation

   if ( ($Count & 0x01) == 0x01)
   {
      $result = setfunctional $OpenOne $NewFunct1
   }
   else
   {
      $result = setfunctional $OpenTwo $NewFunct2
   }
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to set functional address"
   }

   if ( ($Count & 0x01) == 0x01)
   {
      $result = setfunctional $OpenTwo $NewFunct2
   }
   else
   {
      $result = setfunctional $OpenOne $NewFunct1
   }
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to set functional address"
   }

   $result = queryinfo $OpenOne OID_802_5_CURRENT_FUNCTIONAL $CurFunct1
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to query open 1 functional address"
   }
   $result = queryinfo $OpenTwo OID_802_5_CURRENT_FUNCTIONAL $CurFunct2
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to query open 2 functional address"
   }

   $Index = 0
   while ($Index < 4)
   {
      if ($CurFunct1[$Index] != $NewFunct1[$Index])
      {
         fail_variation
         print "FAILURE:  incorrect open 1 functional address"
         break
      }
      if ($CurFunct2[$Index] != $NewFunct2[$Index])
      {
         fail_variation
         print "FAILURE:  incorrect open 2 functional address"
         break
      }
      $Index = $Index + 1
   }

   print " "
   variation

   $result = setfunctional $OpenOne $NullFunct
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to clear open 1 functional address"
   }

   $result = setfunctional $OpenTwo $NullFunct
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to clear open 2 functional address"
   }

   $result = queryinfo $OpenOne OID_802_5_CURRENT_FUNCTIONAL $CurFunct1
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to query open 1 functional address"
   }

   $result = queryinfo $OpenTwo OID_802_5_CURRENT_FUNCTIONAL $CurFunct2
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to query open 2 functional address"
   }

   $Index = 0
   while ($Index < 4)
   {
      if ($CurFunct1[$Index] != $NullFunct[$Index])
      {
         fail_variation
         print "FAILURE:  incorrect open 1 functional address"
         break
      }
      if ($CurFunct2[$Index] != $NullFunct[$Index])
      {
         fail_variation
         print "FAILURE:  incorrect open 2 functional address"
         break
      }
      $Index = $Index + 1
   }

   $Count = $Count + 1
}


#----------------------------------------------------
# variation:   set/clear different functional addresses
#----------------------------------------------------

print "\n***Set/clear different functional addresses on two open instances***\n"

$Count = 0
while ($Count < 16)
{
   print " "
   variation

   $NewFunct1[3] = (uchar)$Count
   $NewFunct2[3] = (uchar)($Count + 0x10)

   $result = setfunctional $OpenOne $NewFunct1
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to set open 1 functional address"
   }

   $result = setfunctional $OpenTwo $NewFunct2
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to set open 2 functional address"
   }

   $result = queryinfo $OpenOne OID_802_5_CURRENT_FUNCTIONAL $CurFunct1
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to query open 1 functional address"
   }
   $result = queryinfo $OpenTwo OID_802_5_CURRENT_FUNCTIONAL $CurFunct2
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to query open 2 functional address"
   }

   $Index = 0
   while ($Index < 4)
   {
      if ($CurFunct1[$Index] != $NewFunct1[$Index])
      {
         fail_variation
         print "FAILURE:  incorrect open 1 functional address"
         break
      }
      if ($CurFunct2[$Index] != $NewFunct2[$Index])
      {
         fail_variation
         print "FAILURE:  incorrect open 2 functional address"
         break
      }
      $Index = $Index + 1
   }

   print " "
   variation

   if ( ($Count & 0x01) == 0x01)
   {
      $result = setfunctional $OpenOne $NullFunct
   }
   else
   {
      $result = setfunctional $OpenTwo $NullFunct
   }
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to clear functional address"
   }

   if ( ($Count & 0x01) == 0x01)
   {
      $result = setfunctional $OpenTwo $NullFunct
   }
   else
   {
      $result = setfunctional $OpenOne $NullFunct
   }
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to clear functional address"
   }

   $result = queryinfo $OpenOne OID_802_5_CURRENT_FUNCTIONAL $CurFunct1
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to query open 1 functional address"
   }
   $result = queryinfo $OpenTwo OID_802_5_CURRENT_FUNCTIONAL $CurFunct2
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to query open 2 functional address"
   }

   $Index = 0
   while ($Index < 4)
   {
      if ($CurFunct1[$Index] != $NullFunct[$Index])
      {
         fail_variation
         print "FAILURE:  incorrect open 1 functional address"
         break
      }
      if ($CurFunct2[$Index] != $NullFunct[$Index])
      {
         fail_variation
         print "FAILURE:  incorrect open 2 functional address"
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
# end of file 1m2o_05b.tst
############################################################


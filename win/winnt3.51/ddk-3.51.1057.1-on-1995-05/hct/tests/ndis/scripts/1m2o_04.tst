#######################################################
# FILE:  1m2o_04.tst
#
# These tests verify the ability to set the lookahead size
# from two different open instances..
#######################################################

friendly_script_name "Two Open SetLookAhead Tests"

print "TITLE:  Test ability to independently set lookahead size from different open instances\n"

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

#--------------------------------------------------
# variation:   set lookahead to same value for both open instances
#--------------------------------------------------


$CurLookahead  = 1
$NewLookahead1 = 0
$NewLookahead2 = 0
$MaxLookahead  = 1

print "\n***Set lookahead to same value on two open instances***"
while (TRUE)
{
   print " "
   variation

   $result = setlookahead $OpenOne $CurLookahead
   if ($CurLookahead > $G_MaxLookahead)
   {
      if ($result == TEST_SUCCESSFUL)
      {
         fail_variation
         print "FAILURE:  should not be able to set lookahead to" $CurLookahead
      }
   }
   else
   {
      if ($result != TEST_SUCCESSFUL)
      {
         fail_variation
         print "FAILURE:  unable to set lookahead to" $CurLookahead
      }
   }

   $result = setlookahead $OpenTwo $CurLookahead
   if ($CurLookahead > $G_MaxLookahead)
   {
      if ($result == TEST_SUCCESSFUL)
      {
         fail_variation
         print "FAILURE:  should not be able to set lookahead to" $CurLookahead
      }
   }
   else
   {
      if ($result != TEST_SUCCESSFUL)
      {
         fail_variation
         print "FAILURE:  unable to set lookahead to" $CurLookahead
      }
   }

   print " "
   variation

   $result = queryinfo $OpenOne OID_GEN_CURRENT_LOOKAHEAD $NewLookahead1
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to query current open 1 lookahead."
   }

   $result = queryinfo $OpenTwo OID_GEN_CURRENT_LOOKAHEAD $NewLookahead2
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to query current open 2 lookahead."
   }

   if ($CurLookahead <= $G_MaxLookahead)
   {
      if ($NewLookahead1 < $CurLookahead)
      {
         fail_variation
         print "FAILURE: open 1 queryinfo returned" $NewLookahead1
      }

      if ($NewLookahead2 < $CurLookahead)
      {
         fail_variation
         print "FAILURE: open 2 queryinfo returned" $NewLookahead2
      }

      if ($CurLookahead < $G_MaxLookahead)
      {
         $CurLookahead = $CurLookahead * 2
         $MaxLookahead = $MaxLookahead * 2
         if ($CurLookahead == $G_MaxLookahead)
         {
            $MaxLookahead = $G_MaxLookahead * 2
         }
         elseif ($CurLookahead > $G_MaxLookahead)
         {
            $CurLookahead = $G_MaxLookahead
         }
      }
      else     # $CurLookahead == $G_MaxLookahead
      {
         $CurLookahead = $MaxLookahead
      }
   }

   else           #$CurLookahead > G_MaxLookahead
   {
      if ($NewLookahead1 != $G_MaxLookahead)
      {
         fail_variation
         print "FAILURE: open 1 queryinfo returned" $NewLookahead1
      }

      if ($NewLookahead2 != $G_MaxLookahead)
      {
         fail_variation
         print "FAILURE: open 2 queryinfo returned" $NewLookahead2
      }
      break
   }
}

#---------------------------------------------
# variation:   set two open instances to different lookaheads
#---------------------------------------------

$CurLookahead1 = 1
$CurLookahead2 = $MaxLookahead

print "\n***Set lookahead to different values on two open instances***"
while (TRUE)
{
   print " "
   variation

   $result = setlookahead $OpenOne $CurLookahead1
   if ($CurLookahead1 > $G_MaxLookahead)
   {
      if ($result == TEST_SUCCESSFUL)
      {
         fail_variation
         print "FAILURE:  should not be able to set lookahead to" $CurLookahead1
      }
   }
   else
   {
      if ($result != TEST_SUCCESSFUL)
      {
         fail_variation
         print "FAILURE:  unable to set lookahead to" $CurLookahead1
      }
   }

   $result = setlookahead $OpenTwo $CurLookahead2
   if ($CurLookahead2 > $G_MaxLookahead)
   {
      if ($result == TEST_SUCCESSFUL)
      {
         fail_variation
         print "FAILURE:  should not be able to set lookahead to" $CurLookahead2
      }
   }
   else
   {
      if ($result != TEST_SUCCESSFUL)
      {
         fail_variation
         print "FAILURE:  unable to set lookahead to" $CurLookahead2
      }
   }

   print " "
   variation

   $result = queryinfo $OpenOne OID_GEN_CURRENT_LOOKAHEAD $NewLookahead1
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to query current open 1 lookahead."
   }

   $result = queryinfo $OpenTwo OID_GEN_CURRENT_LOOKAHEAD $NewLookahead2
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to query current open 2 lookahead."
   }

   if ($CurLookahead1 <= $G_MaxLookahead)
   {
      if ($NewLookahead1 < $CurLookahead1)
      {
         fail_variation
         print "FAILURE: open 1 queryinfo returned" $NewLookahead1
      }

      if ($CurLookahead1 < $G_MaxLookahead)
      {
         $CurLookahead1 = $CurLookahead1 * 2
         if ($CurLookahead1 > $G_MaxLookahead)
         {
            $CurLookahead1 = $G_MaxLookahead
         }
      }
      else     # $CurLookahead1 == $G_MaxLookahead
      {
         $CurLookahead1 = $MaxLookahead
      }
   }

   else           #$CurLookahead > G_MaxLookahead
   {
      if ($NewLookahead1 != $G_MaxLookahead)
      {
         fail_variation
         print "FAILURE: open 1 queryinfo returned" $NewLookahead1
      }
   }

   if ($CurLookahead2 == $MaxLookahead)
   {
      if ($NewLookahead2 != $G_MaxLookahead)
      {
         fail_variation
         print "FAILURE: open 2 queryinfo returned" $NewLookahead2
      }

      $CurLookahead2 = $G_MaxLookahead
   }
   elseif ($CurLookahead2 == $G_MaxLookahead)
   {
      if ($NewLookahead2 != $G_MaxLookahead)
      {
         fail_variation
         print "FAILURE: open 2 queryinfo returned" $NewLookahead2
      }

      $CurLookahead2 = $MaxLookahead / 2
      if ($CurLookahead2 == $G_MaxLookahead)
      {
         $CurLookahead2 = $CurLookahead2 / 2
      }
   }
   else
   {
      if ($NewLookahead2 < $CurLookahead2)
      {
         fail_variation
         print "FAILURE: open 2 queryinfo returned" $NewLookahead2
      }

      if ($CurLookahead2 == 1)
      {
         break
      }
      $CurLookahead2 = $CurLookahead2 / 2
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
# end of file 1m2o_04.tst
############################################################


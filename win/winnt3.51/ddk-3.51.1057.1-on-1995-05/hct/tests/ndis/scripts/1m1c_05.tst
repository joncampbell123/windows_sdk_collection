###############################################################
# File 1m1c_05.tst
#
# This script tests the ability to set and query the lookahead size
###############################################################


friendly_script_name "Single Open SetLookAhead Tests"

print "TITLE:  Test ability to set and query the lookahead size\n"

#------------------------------------------------------
# variation:   open up card, prepare for tests...
#------------------------------------------------------

variation

$OpenInstance = open $G_TestCard $G_OpenFlag
if ($OpenInstance == 0)
{
   block_variation
   print "BLOCKED:  Unable to open card"
   goto endvar0
}


#-----------------------------------------------------
# variation:   check setting lookahead to less than maxlookahead
#-----------------------------------------------------

$CurLookahead = 1
$NewLookahead = 0

print "\n***Testing full range of legal lookahead values***"

while (TRUE)
{
   print " "
   variation

   $result = setlookahead $OpenInstance $CurLookahead
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to set lookahead to " $CurLookahead
   }
   $result = queryinfo $OpenInstance OID_GEN_CURRENT_LOOKAHEAD $NewLookahead
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  unable to query current lookahead."
   }
   elseif ($NewLookahead < $CurLookahead)
   {
      fail_variation
      print "FAILURE: queryinfo returned " $NewLookahead
   }
   elseif ($NewLookahead > $CurLookahead)
   {
      warn_variation
      print "WARNING: queryinfo returned " $NewLookahead
   }
   $CurLookahead = $CurLookahead * 2
   if ($CurLookahead >= $G_MaxLookahead)
   {
      break
   }
}

#----------------------------------------------------
# variation:   check setting lookahead to the maximum
#----------------------------------------------------

print " "
variation

$result = setlookahead $OpenInstance $G_MaxLookahead
if ($result != TEST_SUCCESSFUL)
{
   fail_variation
   print "FAILURE:  unable to set lookahead to MaximumLookahead."
}
$result = queryinfo $OpenInstance OID_GEN_CURRENT_LOOKAHEAD $NewLookahead
if ($result != TEST_SUCCESSFUL)
{
   fail_variation
   print "FAILURE:  unable to query current lookahead."
}
elseif ($NewLookahead != $G_MaxLookahead)
{
   fail_variation
   print "FAILURE: queryinfo returned " $NewLookahead
}


#----------------------------------------------------
# variation:   check on setting lookahead to the maximum+1
#----------------------------------------------------

print "\n***Testing out-of-range lookahead values***\n"
variation

$CurLookahead = $G_MaxLookahead + 1

$result = setlookahead $OpenInstance $CurLookahead
if ($result == TEST_SUCCESSFUL)
{
   fail_variation
   print "FAILURE:  should not be able to set lookahead to " $CurLookahead
}
$result = queryinfo $OpenInstance OID_GEN_CURRENT_LOOKAHEAD $NewLookahead
if ($result != TEST_SUCCESSFUL)
{
   fail_variation
   print "FAILURE:  unable to query current lookahead."
}
elseif ($NewLookahead != $G_MaxLookahead)
{
   fail_variation
   print "FAILURE: queryinfo returned " $NewLookahead
}

#---------------------------------------------
# cleanup
#---------------------------------------------

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
# end of file 1m1c_05.tst
############################################################



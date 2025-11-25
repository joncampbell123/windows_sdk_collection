#########################################################
# 1m1c_01.tst
#
# Open/close tests.  These tests will verify the ability
# to open and close an adapter.
#########################################################

friendly_script_name "OpenAdapter and CloseAdapter Tests"

print "TITLE: NdisOpenAdapter and NdisCloseAdapter tests\n"

# -------------------------------------------------------
# variation:  repeatedly open and close the card, 25 times.
#--------------------------------------------------------

print "***Repeatedly open and close the card***"

$count = 0
while ($count < 25)
{
   print " "
   variation

   $OpenInstance = open $G_TestCard $G_OpenFlag
   if ($OpenInstance == 0)
   {
      fail_variation
      print "FAILURE:  Unable to open test card"
      goto endvar1
   }
   $result = getevents $OpenInstance
   if ($result == TEST_FAILED)
   {
      fail_variation
      print "FAILURE:  Unexpected events recorded"
      close $OpenInstance
      goto endvar1
   }
   elseif ($result == TEST_WARNED)
   {
      warn_variation
      print "WARNING:  Unexpected events recorded"
   }
   $result = close $OpenInstance
   if ($result != TEST_SUCCESSFUL)
   {
      fail_variation
      print "FAILURE:  Unable to close test card"
      goto endvar1
   }
   $count = $count + 1
}

:endvar1

#--------------------------------------------------------
# variation:  Open the card eight times representing 8
#             different protocols opening the card at once.
#--------------------------------------------------------

$oi_array = array uint 8
$index = 0

print "\n***Open card eight times, then close each open instance***"

while ($index < 10)
{
   print " "
   variation

   $count = 0
   while ($count < 8)
   {
      $OpenInstance = open $G_TestCard $G_OpenFlag
      if ($OpenInstance == 0)
      {
         fail variation
         print "FAILURE:  Unable to open test card."
         goto endvar2
      }
      $oi_array[$count] = $OpenInstance
      $count = $count + 1
   }

   print " "
   variation

   $i = 0
   while ($i < $count)
   {
      $results = getevents $oi_array[$i]
      if ($result == TEST_FAILED)
      {
         fail_variation
         print "FAILURE:  Unexpected events recorded"
      }
      elseif ($result == TEST_WARNED)
      {
         warn_variation
         print "WARNING:  Unexpected events recorded"
      }

      $results = close $oi_array[$i]
      if ($result != TEST_SUCCESSFUL)
      {
         fail_variation
         print "FAILURE:  Unable to close test card"
      }
      $i = $i + 1
   }

   $index = $index + 1
}

# branch to here if need to do cleanup after a failure

:endvar2
if ($index < 10)
{
   $i = 0
   while ($i < $count)
   {
      getevents $oi_array[$i]
      close $oi_array[$i]
      $i = $i + 1
   }
}

end

##################################################
# end of file 1m1c_01.tst
##################################################



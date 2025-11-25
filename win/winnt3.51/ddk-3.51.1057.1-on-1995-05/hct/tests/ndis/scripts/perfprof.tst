#------------------------------------------
# File:  perfprof.tst
#
# performance profile tests
# currently done only with directed addresses
#
#-------------------------------------------

friendly_script_name "Performance Profile Tests"

variation

#---------------------------
# first, open up the test card...
#---------------------------


$OpenInstance = open $G_TestCard $G_OpenFlag
if ($OpenInstance == 0)
{
   block_variation
   print "BLOCKED:  unable to open test card"
   goto perfend
}

#---------------------------
# set up some local variables we need
#---------------------------


$SendAddr  = array uchar 6
$Speed    = 0

$result = queryinfo $OpenInstance OID_GEN_LINK_SPEED $Speed
if ($result != TEST_SUCCESSFUL)
{
   block_variation
   print "BLOCKED:  unable to get link speed"
   close $OpenInstance
   goto perfend
}

#
# set the lookahead to a standard value
#
$result = setlookahead $OpenInstance 32
if ($result != TEST_SUCCESSFUL)
{
   print "WARNING:  unable to set lookahead"
}

setpacketfilter $OpenInstance NONE

#
# set number of packets to send in each test
#

if ($G_Media == MEDIUM_ETHERNET)
{
   if ($Speed == 1000000)     # 100MBit ethernet
   {
      $NumMin = 600000
      $NumMed = 400000
      $NumMax = 200000
   }
   elseif ($Speed == 100000)  # normal 10MBit ethernet
   {
      $NumMin = 60000
      $NumMed = 40000
      $NumMax = 20000
   }
   else                       # must be encapsulated ethernet
   {
      $NumMin = 15000
      $NumMed = 10000
      $NumMax =  5000
   }
}

elseif ($G_Media == MEDIUM_TOKENRING)
{
   if ($Speed == 160000)   # 16MBit token ring
   {
      $NumMin = 45000
      $NumMed = 30000
      $NumMax = 15000
   }
   else                    # 4MBit token ring
   {
      $NumMin = 15000
      $NumMed = 10000
      $NumMax =  5000
   }
}

elseif ($G_Media == MEDIUM_FDDI) # 100MBit
{
   $NumMin = 600000
   $NumMed = 400000
   $NumMax = 200000
}

elseif ($G_Media == MEDIUM_ARCNET)
{
   if ($Speed == 1000000)     # 100MBit TCNS arcnet
   {
      $NumMin = 600000
      $NumMed = 400000
      $NumMax = 200000
   }
   else                       # normal 2MBit arcnet
   {
      $NumMin = 15000
      $NumMed = 10000
      $NumMax =  5000
   }
}

else
{
   print "TEST ABORTED:  Unrecognized media type.\n"
   close $OpenInstance
   goto perfend
}

#
# set size of packets to use
#
$SizeInc = ($G_MaxTotalSize - 64) / 15
$SizeMin = $G_MaxTotalSize - (15 * $SizeInc)
$SizeMax  = $G_MaxTotalSize
$SizeBreak1 = 512
$SizeBreak2 = 1024

#
# first, do send-only test.  Send random_directed packets to oblivion
#

print "\nSend random-directed packets\n\n"
$SendAddr[0] = (uchar) 0x00
$SendAddr[1] = (uchar) 0x02
$SendAddr[2] = (uchar) 0x04
$SendAddr[3] = (uchar) 0x06
$SendAddr[4] = (uchar) 0x08
$SendAddr[5] = (uchar) 0x0A

$Size = $SizeMin
while($Size <= $SizeMax)
{
   if ($Size <= $SizeBreak1)
   {
      $Num = $NumMin
   }
   elseif ($Size <= $SizeBreak2)
   {
      $Num = $NumMed
   }
   else
   {
      $Num = $NumMax
   }

   variation
   performance $OpenInstance $SendAddr $Size $Num 0 PERFORM_SEND
   waitperformance $OpenInstance
   getperformanceresults $OpenInstance

   $Size = $Size + $SizeInc
}

#
# now, do the directed receive tests
#
print "\nTest reception of directed packets\n"

setpacketfilter $OpenInstance DIRECTED

$Size = $SizeMin
while($Size <= $SizeMax)
{
   if ($Size <= $SizeBreak1)
   {
      $Num = $NumMin
   }
   elseif ($Size <= $SizeBreak2)
   {
      $Num = $NumMed
   }
   else
   {
      $Num = $NumMax
   }

   $Mode = PERFORM_RECEIVE
   variation
   performance $OpenInstance $G_TestAddress $Size $Num 0 $Mode $G_ServerOpen
   waitperformance $OpenInstance
   getperformanceresults $OpenInstance
   sleep 5

   $Mode = PERFORM_RECEIVE + PERFORM_COPY_RECEIVES
   variation
   performance $OpenInstance $G_TestAddress $Size $Num 0 $Mode $G_ServerOpen
   waitperformance $OpenInstance
   getperformanceresults $OpenInstance
   sleep 5

   $Size = $Size + $SizeInc
}

close $OpenInstance

:perfend

end

#-------------------------------------------------------------------
# end of file perfprof.tst
#-------------------------------------------------------------------




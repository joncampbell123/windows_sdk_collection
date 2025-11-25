#------------------------------------------
# File:  perf_pro.tst
#
# test performance of card when receive in promiscuous mode..
#
#-------------------------------------------

friendly_script_name "Promiscuous Performance Tests"

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

setpacketfilter $OpenInstance DIRECTED

#
# set number of packets to send in each test
#
if ($G_Media == MEDIUM_ETHERNET)
{
   if ($Speed == 1000000)     # 100MBit ethernet
   {
      $NumMin = 2000000
      $NumMed =  500000
      $NumMax =  250000
   }
   elseif ($Speed == 100000)  # normal 10MBit ethernet
   {
      $NumMin = 200000
      $NumMed =  50000
      $NumMax =  25000
   }
   else                       # must be encapsulated ethernet
   {
      $NumMin =  40000
      $NumMed =  10000
      $NumMax =   5000
   }
}

elseif ($G_Media == MEDIUM_TOKENRING)
{
   if ($Speed == 160000)   # 16MBit token ring
   {
      $NumMin = 160000
      $NumMed =  80000
      $NumMax =  16000
   }
   else                    # 4MBit token ring
   {
      $NumMin =  40000
      $NumMed =  20000
      $NumMax =   4000
   }
}

elseif ($G_Media == MEDIUM_FDDI) # 100MBit
{
   $NumMin = 2000000
   $NumMed =  500000
   $NumMax =  250000
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
$SizeMin  = 64
$SizeMed  = 512
$SizeMax  = $G_MaxTotalSize
if ($SizeMed > $SizeMax)
{
   $SizeMed = ($SizeMin + $SizeMax) / 2
}

#
# now, do the promiscuous-mode receive tests..directed
#
$SendAddr[0] = (uchar) 0x00
$SendAddr[1] = (uchar) 0x02
$SendAddr[2] = (uchar) 0x04
$SendAddr[3] = (uchar) 0x06
$SendAddr[4] = (uchar) 0x08
$SendAddr[5] = (uchar) 0x0A
setpacketfilter $OpenInstance PROMISCUOUS
print "\nTest promiscious mode reception\n"

#
# test 4 different speeds with tiny packets
#
$Delay = 0
while ($Delay < 14)
{
   $Mode = PERFORM_RECEIVE
   variation
   performance $OpenInstance $SendAddr $SizeMin $NumMin $Delay $Mode $G_ServerOpen
   waitperformance $OpenInstance
   getperformanceresults $OpenInstance
   sleep 10

   $Mode = PERFORM_RECEIVE + PERFORM_COPY_RECEIVES
   variation
   performance $OpenInstance $SendAddr $SizeMin $NumMin $Delay $Mode $G_ServerOpen
   waitperformance $OpenInstance
   getperformanceresults $OpenInstance
   sleep 10

   $Delay = $Delay + 4
}

#
# just top speed with medium and big packets
#
$Mode = PERFORM_RECEIVE
variation
performance $OpenInstance $SendAddr $SizeMed $NumMed 0 $Mode $G_ServerOpen
waitperformance $OpenInstance
getperformanceresults $OpenInstance
sleep 10

$Mode = PERFORM_RECEIVE + PERFORM_COPY_RECEIVES
variation
performance $OpenInstance $SendAddr $SizeMed $NumMed 0 $Mode $G_ServerOpen
waitperformance $OpenInstance
getperformanceresults $OpenInstance
sleep 10

$Mode = PERFORM_RECEIVE
variation
performance $OpenInstance $SendAddr $SizeMax $NumMax 0 $Mode $G_ServerOpen
waitperformance $OpenInstance
getperformanceresults $OpenInstance
sleep 10

$Mode = PERFORM_RECEIVE + PERFORM_COPY_RECEIVES
variation
performance $OpenInstance $SendAddr $SizeMax $NumMax 0 $Mode $G_ServerOpen
waitperformance $OpenInstance
getperformanceresults $OpenInstance
sleep 10

close $OpenInstance

:perfend

end

#-------------------------------------------------------------------
# end of file perf_pro.tst
#-------------------------------------------------------------------




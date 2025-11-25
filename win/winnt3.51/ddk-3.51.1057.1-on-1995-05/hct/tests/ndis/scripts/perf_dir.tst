#------------------------------------------
# File:  perf_dir.tst
#
# performance tests with directed addresses
#
#-------------------------------------------

friendly_script_name "Directed Performance Tests"

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

elseif ($G_Media == MEDIUM_ARCNET)
{
   if ($Speed == 1000000)     # 100MBit TCNS arcnet
   {
      $NumMin = 2000000
      $NumMed =  500000
      $NumMax =  250000
   }
   else                       # normal 2MBit arcnet
   {
      $NumMin =  40000
      $NumMed =  10000
      $NumMax =   5000
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
$SizeMin  = 64
$SizeMed  = 512
$SizeMax  = $G_MaxTotalSize
if ($SizeMed > $SizeMax)
{
   $SizeMed = ($SizeMin + $SizeMax) / 2
}

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

#
# send only performance test
#
variation
performance $OpenInstance $SendAddr $SizeMin $NumMin 0 PERFORM_SEND
waitperformance $OpenInstance
getperformanceresults $OpenInstance

variation
performance $OpenInstance $SendAddr $SizeMed $NumMed 0 PERFORM_SEND
waitperformance $OpenInstance
getperformanceresults $OpenInstance

variation
performance $OpenInstance $SendAddr $SizeMax $NumMax 0 PERFORM_SEND
waitperformance $OpenInstance
getperformanceresults $OpenInstance


#
# similar test--send directed to server card
#
print "\nSend directed to server\n"
setpacketfilter $OpenInstance DIRECTED

#
# send performance test
#
variation
performance $OpenInstance $G_TestAddress $SizeMin $NumMin 0 PERFORM_SEND $G_ServerOpen
waitperformance $OpenInstance
getperformanceresults $OpenInstance

variation
performance $OpenInstance $G_TestAddress $SizeMed $NumMed 0 PERFORM_SEND $G_ServerOpen
waitperformance $OpenInstance
getperformanceresults $OpenInstance

variation
performance $OpenInstance $G_TestAddress $SizeMax $NumMax 0 PERFORM_SEND $G_ServerOpen
waitperformance $OpenInstance
getperformanceresults $OpenInstance

#
# now, do the directed receive tests
#
print "\nTest reception of directed packets\n"

#
# test 4 different speeds with tiny packets
#
$Delay = 0
while ($Delay < 14)
{
   $Mode = PERFORM_RECEIVE
   variation
   performance $OpenInstance $G_TestAddress $SizeMin $NumMin $Delay $Mode $G_ServerOpen
   waitperformance $OpenInstance
   getperformanceresults $OpenInstance
   sleep 10

   $Mode = PERFORM_RECEIVE + PERFORM_COPY_RECEIVES
   variation
   performance $OpenInstance $G_TestAddress $SizeMin $NumMin $Delay $Mode $G_ServerOpen
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
performance $OpenInstance $G_TestAddress $SizeMed $NumMed 0 $Mode $G_ServerOpen
waitperformance $OpenInstance
getperformanceresults $OpenInstance
sleep 10

$Mode = PERFORM_RECEIVE + PERFORM_COPY_RECEIVES
variation
performance $OpenInstance $G_TestAddress $SizeMed $NumMed 0 $Mode $G_ServerOpen
waitperformance $OpenInstance
getperformanceresults $OpenInstance
sleep 10

$Mode = PERFORM_RECEIVE
variation
performance $OpenInstance $G_TestAddress $SizeMax $NumMax 0 $Mode $G_ServerOpen
waitperformance $OpenInstance
getperformanceresults $OpenInstance
sleep 10

$Mode = PERFORM_RECEIVE + PERFORM_COPY_RECEIVES
variation
performance $OpenInstance $G_TestAddress $SizeMax $NumMax 0 $Mode $G_ServerOpen
waitperformance $OpenInstance
getperformanceresults $OpenInstance
sleep 10

#
# finally, do the misc performance tests
#

$Count = 0
while ($Count < 3)
{
   if ($Count == 0)
   {
      print "\nSend-with-ack tests\n"
      $Mode = PERFORM_SEND_WITH_ACK
      $TempMin = $NumMin / 2
      $TempMed = $NumMed / 2
      $TempMax = $NumMax / 2
   }
   elseif ($Count == 1)
   {
      print "\nReq-and-receive tests\n"
      $TempMin = $NumMin / 2
      $TempMed = $NumMed / 2
      $TempMax = $NumMax / 2
      $Mode = PERFORM_REQ_AND_RECEIVE
   }
   elseif ($Count == 2)
   {
      print "\nTwo-way send-and-receive tests\n"
      $TempMin = $NumMin
      $TempMed = $NumMed
      $TempMax = $NumMax
      $Mode = PERFORM_SEND_AND_RECEIVE
   }

   variation
   performance $OpenInstance $G_TestAddress $SizeMin $TempMin 0 $Mode $G_ServerOpen
   waitperformance $OpenInstance
   getperformanceresults $OpenInstance
   sleep 10

   variation
   performance $OpenInstance $G_TestAddress $SizeMed $TempMed 0 $Mode $G_ServerOpen
   waitperformance $OpenInstance
   getperformanceresults $OpenInstance
   sleep 10

   variation
   performance $OpenInstance $G_TestAddress $SizeMax $TempMax 0 $Mode $G_ServerOpen
   waitperformance $OpenInstance
   getperformanceresults $OpenInstance
   sleep 10

   $Count = $Count + 1
}


close $OpenInstance

:perfend

end

#-------------------------------------------------------------------
# end of file perf_dir.tst
#-------------------------------------------------------------------




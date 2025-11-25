#------------------------------------------
# File:  perf_brd.tst
#
# Performance test with broadcast-addressed packets
#
#-------------------------------------------

friendly_script_name "Broadcast Performance Tests"

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


$BcastAddr = array uchar 6
$Speed    = 0

if ($G_Media == MEDIUM_TOKENRING)
{
   $BcastAddr[0] = (uchar)0xc0
   $BcastAddr[1] = (uchar)0x00
   $BcastAddr[2] = (uchar)0xff
   $BcastAddr[3] = (uchar)0xff
   $BcastAddr[4] = (uchar)0xff
   $BcastAddr[5] = (uchar)0xff
}
elseif ($G_Media == MEDIUM_ARCNET)
{
   $BcastAddr[0] = (uchar)0x00
   $BcastAddr[1] = (uchar)0x00
   $BcastAddr[2] = (uchar)0x00
   $BcastAddr[3] = (uchar)0x00
   $BcastAddr[4] = (uchar)0x00
   $BcastAddr[5] = (uchar)0x00
}
else              # MEDIUM_ETHERNET or MEDIUM_FDDI
{
   $BcastAddr[0] = (uchar)0xff
   $BcastAddr[1] = (uchar)0xff
   $BcastAddr[2] = (uchar)0xff
   $BcastAddr[3] = (uchar)0xff
   $BcastAddr[4] = (uchar)0xff
   $BcastAddr[5] = (uchar)0xff
}

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
# first, do send-only test.  Send broadcast packets to oblivion
#

#
# set up address to send to.
#
print "\nSend broadcast packets\n\n"

#
# send only performance test
#
variation
performance $OpenInstance $BcastAddr $SizeMin $NumMin 0 PERFORM_SEND
waitperformance $OpenInstance
getperformanceresults $OpenInstance

variation
performance $OpenInstance $BcastAddr $SizeMed $NumMed 0 PERFORM_SEND
waitperformance $OpenInstance
getperformanceresults $OpenInstance

variation
performance $OpenInstance $BcastAddr $SizeMax $NumMax 0 PERFORM_SEND
waitperformance $OpenInstance
getperformanceresults $OpenInstance


#
# now, do the broadcast receive tests
#
print "\nTest reception of broadcast packets\n"
setpacketfilter $OpenInstance BROADCAST

#
# test 4 different speeds with tiny packets
#
$Delay = 0
while ($Delay < 14)
{
   $Mode = PERFORM_RECEIVE
   variation
   performance $OpenInstance $BcastAddr $SizeMin $NumMin $Delay $Mode $G_ServerOpen
   waitperformance $OpenInstance
   getperformanceresults $OpenInstance
   sleep 10

   $Mode = PERFORM_RECEIVE + PERFORM_COPY_RECEIVES
   variation
   performance $OpenInstance $BcastAddr $SizeMin $NumMin $Delay $Mode $G_ServerOpen
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
performance $OpenInstance $BcastAddr $SizeMed $NumMed 0 $Mode $G_ServerOpen
waitperformance $OpenInstance
getperformanceresults $OpenInstance
sleep 10

$Mode = PERFORM_RECEIVE + PERFORM_COPY_RECEIVES
variation
performance $OpenInstance $BcastAddr $SizeMed $NumMed 0 $Mode $G_ServerOpen
waitperformance $OpenInstance
getperformanceresults $OpenInstance
sleep 10

$Mode = PERFORM_RECEIVE
variation
performance $OpenInstance $BcastAddr $SizeMax $NumMax 0 $Mode $G_ServerOpen
waitperformance $OpenInstance
getperformanceresults $OpenInstance
sleep 10

$Mode = PERFORM_RECEIVE + PERFORM_COPY_RECEIVES
variation
performance $OpenInstance $BcastAddr $SizeMax $NumMax 0 $Mode $G_ServerOpen
waitperformance $OpenInstance
getperformanceresults $OpenInstance
sleep 10


close $OpenInstance

:perfend

end

#-------------------------------------------------------------------
# end of file perf_brd.tst
#-------------------------------------------------------------------




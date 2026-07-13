
#this program is for comoparing the multinode performance data
#it accepts any number of .rpt files and combines them into one report
#and prints * against the max value

BEGIN { py = 0; i = 1; j = 0;ok = 0;  fname = "mulcmp.out";}

/----/ { if (ok == 0)   ok = 1;
	 else  ok = 0; 
       }

{ if ((ok == 1) && (NF > 3 )) {
	W[i++] = $(NF-2);
	} 
}


END { print "i = " i;
    for (j = 1; j <= 93; j++) {
 	printf("%7d\t%7d\t%7d\t%7d\n",W[j],W[j+93],W[j+186],W[j+279]) >> fname;
    }
}

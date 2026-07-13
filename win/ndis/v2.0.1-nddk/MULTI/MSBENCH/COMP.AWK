
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
	e = " "; f = " "; g = " "; h = " ";
	if ((W[j] >=  W[j+93]) && (W[j] >=  W[j+186]) && (W[j] >= W[j+279])) 
		{e = "*";}
	else { 
        if ((W[j+93] >= W[j]) && (W[j+93] >= W[j+186]) && (W[j+93] >= W[j+279]))  		{f = "*";}
	else  {
	if ((W[j+186] >= W[j]) && (W[j+186] >= W[j+93]) && (W[j+186] >= W[j+279]))
		{g = "*";} 
	else 	{h = "*";}
	}
	}
 #	printf("%7d%5s%7d%5s%7d\n",W[j],"     ",W[j+93],"     ",W[j+186]) >> fname;
 	printf("%7d%1s%4s%7d%1s%4s%7d%1s%4s%7d%1s\n",W[j],e,"    ",W[j+93],f,"    ",W[j+186],g,"    ",W[j+279],h) >> fname;
    }
}

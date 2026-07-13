#this program takes input from perf?.out and rearrages it to
# get all the read /write numbers in sequence. It also deletes
# extra BPS data
BEGIN { i = 1; j = 1; k = 1; l = 1; } 
{ w[i++] = $(NF);}
END {  k = 1;
      while (j++ <= 4) { 
	 for (l = 1; l <= 7; l++){ 
	    print w[k++];
	 }
	 i = 27 - (3*j); 
	 print w[k+i]"\n"w[k+i+1] "\n" w[k+i+2] "\n" w[k+i+3];
     }
     for (j = 52; j <=100; j++)
	  print w[j];
}

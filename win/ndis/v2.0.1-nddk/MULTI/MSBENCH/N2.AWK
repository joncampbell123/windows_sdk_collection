BEGIN { f1 = 0; f2 = 0; f3 = 0;i = 0;j = 0; OP = " "; 
	print "\t\t","Multinode performance data for";}
{ if (NR == 1) 	printf("\t\t%6s\n",$1);}
/Server/ {print "\t\t"$2"\t"$3;}
#/START/ { f1 = 1; }
#/STOP/ 	{ f1 = 0;}
/----/ { if (f1 == 0) {f1 = 1;} 
	 else  { f1 = 0;}
}	
{ if ((f1 == 1) && (NF > 3)) {
	if (OP != $(NF-6))  {
	   printf("%5s\t%4s\t%7s\t%7s\t%7s\t%7s\t%7s\t%7s\n","Opern","Mode","BufSize","Wksta-1","Wksta-2","Wksta-4","Wksta-6","Wksta-12");
	   OP = $(NF-6);
	} 
	 printf("%5s\t%2s\t%2s\t%7d\t%7d\t%7d\t%7d\t%7d\n",$(NF-6),$(NF-5),$1,$(NF-4),$(NF-3),$(NF-2),$(NF-1),$NF);
   }
}

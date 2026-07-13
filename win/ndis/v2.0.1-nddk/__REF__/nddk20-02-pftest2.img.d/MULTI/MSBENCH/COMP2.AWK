
BEGIN { flg = 0;ot = 0;} 
/Multi-node/ { ot = 1;}
/----/ { if (flg == 0)  flg = 1
	 else flg = 0;}
{ if (ot == 1) {
	if ((flg == 1) && (NF > 1)) {
	    print $(NF) $(NF-1) $(NF-2);
	    }
	else { print $0;}
 }
}

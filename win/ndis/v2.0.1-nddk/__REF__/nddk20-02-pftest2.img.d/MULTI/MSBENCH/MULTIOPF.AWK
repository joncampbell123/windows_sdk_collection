#program for the old pf output format
# This program is for filtering multi-node performance data
# it creates .out files containing average BPS
BEGIN { op = " "; mode = " "; buf = 0; fs = " "; bps = 0;py = 0;}
/---/ { bps = 0; buf = 0; fs = " "; mode = " "; op = " ";py = 0;}
/file:/ { mode = substr($4,1,2); fs = $(NF-3);}
/rec size/ { buf = $(NF); }
/bytes\/sec/ {  bps = $(NF - 1);
		if (NF == 12) {op = $2;} 
	         else {op = $2$3;}
 	    print buf " " fs " " op " " mode " " bps; py = 0; 
	    bps = 0; buf = 0; fs = " "; mode = " "; op = " ";
	    }

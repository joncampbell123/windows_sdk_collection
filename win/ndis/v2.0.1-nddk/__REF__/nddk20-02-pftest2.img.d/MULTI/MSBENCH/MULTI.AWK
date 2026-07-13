# This program is for filtering multi-node performance data
# it creates .out files containing average BPS
BEGIN { op = " "; mode = " "; buf = 0; fs = " "; bps = 0;py = 0;}
/---/ { bps = 0; buf = 0; fs = " "; mode = " "; op = " ";py = 0;}
/file:/ { mode = substr($4,1,2); fs = $(NF);}
/rec size/ { buf = $(NF); }
/AvgBPS/ { py = 1 }

{ if ((py == 1) && ($(NF) == 0)) {
		py = 2; bps = $(NF - 2); if (NF == 11) {op = $1;} 
				         else {op = $1$2;}
	    }
}

{ if (py == 2) { print buf " " fs " " op " " mode " " bps; py = 0; }
}

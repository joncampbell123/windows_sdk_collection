# this program is for creating multinode performance report
# it combines the output of multi1.awk (pfw.out) and multi.tmp
# which is a report template
BEGIN { fname = "multi.rpt";flg = 0;ot = 0;k = 1;l = 1;
	print " " > fname;} 
{if ((ot == 0) && (NF > 0)) W[k++] = $0;}
/Multi-node/ { ot = 1;}
/----/ { if (flg == 0)  flg = 1
	 else flg = 0;}
{ if (ot == 1) {
	if ((flg == 1) && (NF > 1)) {
		print $0,"\t",W[l++] >> fname; }
	else { print $0 >> fname;}
 }
}

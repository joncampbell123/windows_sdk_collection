BEGIN {i= 1; fname = "pfw.out";print " " > fname;}
{ W[i++] = $(NF);} 
END { for (j = 1; j <= 93; j++) {
#       print W[j]"\t" W[j+93]"\t" W[j+186]"\t" W[j+279]"\t" W[j+372] >> fname;
 	printf("%6d\t%6d\t%6d\t%6d\t%6d\n",W[j],W[j+93],W[j+186],W[j+279],W[j+372]) >> fname;
    }
}

rem This file creates a report for multinode performance tests
del multi.rpt
awk -f multi.awk %1\multi\output\perf1.12\*.out > perf12.out
awk -f strip.awk perf12.out > pf12.out
awk -f multi.awk %1\multi\output\perf1.6\*.out > perf6.out
awk -f strip.awk perf6.out > pf6.out
awk -f multi.awk %1\multi\output\perf1.4\*.out > perf4.out
awk -f strip.awk perf4.out > pf4.out
awk -f multi.awk %1\multi\output\perf1.2\*.out > perf2.out
awk -f strip.awk perf2.out > pf2.out
awk -f multi.awk %1\multi\output\perf1.1\*.out > perf1.out
awk -f strip.awk perf1.out > pf1.out
awk -f multi1.awk pf1.out pf2.out pf4.out pf6.out pf12.out
awk -f multi2.awk pfw.out report.tmp
del pf??.out
del perf??.out
rename multi.rpt %2.rpt
awk -f n2.awk %2.rpt > %2.xls
copy %2.rpt %1\multi\output\%2.rpt
copy %2.xls %1\multi\output\%2.xls
echo report and Excel worksheet are placed in %2.rpt and %2.xls respectively

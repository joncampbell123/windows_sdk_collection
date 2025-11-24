;
; File:         CDPerf.pro
;
; Purpose:      Choice Profile for CD Perf Test for the HCT.
;

[File]
Input=*.avi
Output=.\CDPerf.log
OutAppend=1

[Parameter]
TransferRate=300
UsePacing=0
UsePriming=1
PrimingData=600
DestroyCache=1

[Block]
Block1=1
Block2=2
Block3=4
Block4=8
Block5=16
Block6=32
Block7=64
Block8=128
Block9=256
Block10=512
Selection=0x3FE

[Work]
Work1=0
Work2=10
Work3=20
Work4=30
Work5=40
Work6=50
Work7=60
Work8=70
Work9=80
Work10=90
Selection=0x3FF

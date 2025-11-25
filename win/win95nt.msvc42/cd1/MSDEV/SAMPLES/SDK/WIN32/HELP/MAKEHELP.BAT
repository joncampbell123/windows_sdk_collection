@if exist sample.err del sample.err > nul
@start /w hcrtf -x sample.hpj
@if exist sample.hp type sample.hp

##
## TITLE: NDIS 3.0 Conformance Test Suite
##
## Run the NDIS 3.0 Conformance Test Suite using the
## TokenRing specific - client side scripts.
##

###############################################

##
## Run the One Machine One Card One Open Tests.
##

ReadScript \tps\scripts\1\1\1\111_tr.tps
ReadScript \tps\scripts\1\1\2\112_tr.tps
ReadScript \tps\scripts\1\1\3\113.tps

##
## Run the One Machine One Card Two Open Tests.
##

ReadScript \tps\scripts\1\2\1\121_tr.tps
ReadScript \tps\scripts\1\2\2\122_tr.tps
ReadScript \tps\scripts\1\2\3\123.tps

##
## Run the One Machine Two Card One Open Tests.
##

ReadScript \tps\scripts\1\3\1\131_tr.tps
ReadScript \tps\scripts\1\3\2\132.tps

##
## Run the Two Machine One Card One Open Tests.
##

ReadScript \tps\scripts\2\1\1\211_tr.tps
ReadScript \tps\scripts\2\1\2\212.tps


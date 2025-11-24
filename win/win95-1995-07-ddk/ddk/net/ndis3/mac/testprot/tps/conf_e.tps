##
## TITLE: NDIS 3.0 Conformance Test Suite
##
## Run the NDIS 3.0 Conformance Test Suite using the
## Ethernet specific - client side scripts.
##

###############################################

##
## Run the One Machine One Card One Open Tests.
##

ReadScript \tps\scripts\1\1\1\111_e.tps
ReadScript \tps\scripts\1\1\2\112_e.tps
ReadScript \tps\scripts\1\1\3\113.tps

##
## Run the One Machine One Card Two Open Tests.
##

ReadScript \tps\scripts\1\2\1\121_e.tps
ReadScript \tps\scripts\1\2\2\122_e.tps
ReadScript \tps\scripts\1\2\3\123.tps

##
## Run the One Machine Two Card One Open Tests.
##

ReadScript \tps\scripts\1\3\1\131_e.tps
ReadScript \tps\scripts\1\3\2\132.tps

##
## Run the Two Machine One Card One Open Tests.
##

ReadScript \tps\scripts\2\1\1\211_e.tps
ReadScript \tps\scripts\2\1\2\212.tps


#!/bin/bash

# string matching is not as easy as it seems
# what about utf8 strings , or other encodings?
# what about the degree of acceptable fuzziness

# this one succeeds for some reason
#./vcf_weaksearch_test -f cardfiles/multiple_example.vcf -s "Spieg"

#  name:Bald;Proctor 
./vcf_weaksearch_test -f cardfiles/multiple_example.vcf -s "Proct"
#./vcf_weaksearch_test -f cardfiles/multiple_example.vcf -s "Bald"

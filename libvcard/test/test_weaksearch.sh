#!/bin/bash

# string matching is not as easy as it seems
# what about utf8 strings , or other encodings?
# what about the degree of acceptable fuzziness

# this one succeeds for some reason
# pwd

fail=0
./vcf_weaksearch_test -f test/cardfiles/multiple_example.vcf -s "Spieg" > test/weaksearch.out

#printf "x found: ${x}\n\n"
if cmp -l test/test_weaksearch_result test/weaksearch.out
then
    :
else
    printf "weaksearch test failed\n"
    fail=1
fi
exit $fail
#  name:Bald;Proctor 
# ./vcf_weaksearch_test -f cardfiles/multiple_example.vcf -s "Proct"
# ./vcf_weaksearch_test -f cardfiles/multiple_example.vcf -s "Bald"


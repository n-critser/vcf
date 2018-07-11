#!/bin/bash

# res=$(./vcf_weaksearch_test -f cardfiles/ham_vcard_example.vcf -s "Andreas")
# printf "$res\n"

# fails to find the match leonard;Andy
# need regex search !!!!!!!!!
#res=$(./vcf_weaksearch_test -f cardfiles/all_Contacts_example.vcf -s "Andy")
#printf "$res\n"

#printf "res = %d",$res
# if [ $res eq 2 ];
# then
#     printf "res = %d\n",$res
# else
#     printf "EXPECTED 2 BUT GOT %d\n",$res
# fi

# res=$(./vcf_weaksearch_test -f cardfiles/ham_vcard_example.vcf -s "Eieland;A")
# printf "$res\n"

# if [ $res eq 2 ];
# then
#     printf "res = %d\n",$res
# else
#     printf "EXPECTED 2 BUT GOT %d\n",$res

# fi


#./vcf_weaksearch_test < cardfiles/all_Contacts_example.vcf


# string matching is not as easy as it seems
# what about utf8 strings , or other encodings?
# what about the degree of acceptable fuzziness

# this one succeeds for some reason
./vcf_weaksearch_test -f cardfiles/ham_vcard_example.vcf -s "andy"

# this one fails -- only diff is capital A vs small a 
./vcf_weaksearch_test -f cardfiles/ham_vcard_example.vcf -s "Andy"
./vcf_weaksearch_test -f cardfiles/ham_vcard_example.vcf -s "d;And"
./vcf_weaksearch_test -f cardfiles/ham_vcard_example.vcf -s ";"

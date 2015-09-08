#!/bin/bash

#/./derrick -m -r ~/PcapData/smtp.pcap -b 256 -d bin > smtp_merge_32.log
#./derrick -m -r ~/PcapData/Pure/pure_http.pcap -b 256 -d bin > http_merge_256.log
#./derrick -m -r ~/PcapData/Pure/pure_http.pcap -b 256 -d bin > http_merge_32.log
#./derrick -m -r ~/PcapData/Pure/pure_http.pcap -b 256 -d bin > http_merge_16.log
#./derrick -r ~/PcapData/Pure/pure_http.pcap -d bin > http_no_merge_16.bin
#./derrick -l http_no_merge_16_encode.gz  -r ~/PcapData/Pure/pure_http.pcap 

# -m : means that perform tcp and ip normalization
# -d $2 : ascii/encoded/hex/bin
# -n $1 : the number of bytes for every flow payload need to truncate.

#./derrick -m -n $1 -r ~/pcap/pure_http.pcap -d $2 > ~/samples/http_samples/http_merge_${1}.${2}

#./derrick -m -n $1 -r ~/pcap/pure_http.pcap -d $2 -o ~/samples/http_samples/16/http_sample
#./derrick -m -n $1 -r ~/pcap/pure_http.pcap -d $2 -o ~/samples/http_samples/128/http_sample
./derrick -m -n $1 -r ~/pcap/pure_http.pcap -d $2 -o ~/samples/http_samples/64/http_sample



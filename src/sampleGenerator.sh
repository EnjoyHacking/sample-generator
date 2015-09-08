#/./derrick -m -r ~/PcapData/smtp.pcap -b 256 -d bin > smtp_merge_32.log
#./derrick -m -r ~/PcapData/Pure/pure_http.pcap -b 256 -d bin > http_merge_256.log
#./derrick -m -r ~/PcapData/Pure/pure_http.pcap -b 256 -d bin > http_merge_32.log
#./derrick -m -r ~/PcapData/Pure/pure_http.pcap -b 256 -d bin > http_merge_16.log
#./derrick -r ~/PcapData/Pure/pure_http.pcap -d bin > http_no_merge_16.bin
./derrick -l http_no_merge_16_encode.gz  -r ~/PcapData/Pure/pure_http.pcap 

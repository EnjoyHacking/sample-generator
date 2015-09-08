#!/bin/sh
awk '{for(i=5;i<=NF;i++) {if(i != NF) {printf("%s ",$i);} else {printf("%s\n", $i);} }}' $1 

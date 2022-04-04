#!/usr/bin/env bash

#usage ./run_compile.sh ./config_1_2.txt

CUR_DIR=$(pwd)
AlveoLinkPath=$(awk 'sub(/AlveoLink.*/, "")' <<< "$CUR_DIR")AlveoLink

#make manager
cd $AlveoLinkPath/kernel/sw
make clean TARGET=hw
make TARGET=hw
cd $CUR_DIR

while IFS=' ' read -r hostname ipAddr xclbin devId
do
    msg="hostname: $hostname, ipAddr: $ipAddr, xclbin: $xclbin, devID: $devId"
    echo "$msg"
    ssh -f $hostname  "cd $AlveoLinkPath/tests/kernel/sync_udp_test; bash compile.sh $hostname $AlveoLinkPath"
    sleep 1
done <"$1"

#!/usr/bin/env bash

#usage ./run_compile.sh ./config_1_2.txt

ip_file="./ip.txt"
rm -f $ip_file

CUR_DIR=$(pwd)
AlveoLinkPath=$(awk 'sub(/AlveoLink.*/, "")' <<< "$CUR_DIR")AlveoLink
if [ "$AlveoLinkPath" == "AlveoLink" ]; then
    AlveoLinkPath=$AL_PATH
fi

while IFS=' ' read -r hostname ipAddr xclbin devId
do
    msg="hostname: $hostname, ipAddr: $ipAddr, xclbin: $xclbin, devID: $devId"
    echo "$msg"
    echo "$ipAddr" >> $ip_file
    ssh -f $hostname  "cd $AlveoLinkPath/network/udp/scripts; bash compile.sh $hostname $AlveoLinkPath"
    sleep 1
done <"$1"

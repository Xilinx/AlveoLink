#!/usr/bin/env bash

#usage ./run_compile.sh ./config_1_2.txt

CUR_DIR=$(pwd)
AlveoLinkPath=$(awk 'sub(/AlveoLink.*/, "")' <<< "$CUR_DIR")AlveoLink

while IFS=' ' read -r hostname ipAddr xclbin devId
do
    msg="hostname: $hostname, ipAddr: $ipAddr, xclbin: $xclbin, devID: $devId"
    echo "$msg"
    if [[ $xclbin =~ .*Manager.* ]]; then
        echo "it's manager"
        ssh -f $hostname  "cd $AlveoLinkPath/kernel/builds/udp; bash compile.sh $hostname"
    else
        ssh -f $hostname  "cd $AlveoLinkPath/tests/kernel/sync_adapter_udp; bash compile.sh $hostname"
    fi
    sleep 1
done <"$1"

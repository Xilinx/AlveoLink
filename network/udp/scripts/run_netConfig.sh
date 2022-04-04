#!/usr/bin/env bash

#usage ./run_netConfig.sh ./config_2_3.txt ./ip.txt
CUR_DIR=$(pwd)
AlveoLinkPath=$(awk 'sub(/AlveoLink.*/, "")' <<< "$CUR_DIR")AlveoLink

socket_files=()

while IFS=' ' read -r hostname ipAddr xclbin devId
do
    socket_files+=("$CUR_DIR/${hostname}_${devId}_sockets.txt")
    rm -rf "${hostname}_${devId}_sockets.txt"
done <"$1"

while IFS=' ' read -r hostname ipAddr xclbin devId
do
    msg="hostname: $hostname, ipAddr: $ipAddr, xclbin: $xclbin, devID: $devId"
    echo "$msg"
    ssh -f $hostname  "cd $CUR_DIR;  bash $AlveoLinkPath/network/udp/scripts/netConfig.sh $hostname $AlveoLinkPath $xclbin $devId $2 $ipAddr"
    sleep 1
done <"$1"

for socket_file in ${socket_files[@]}
do
    while [ ! -f $socket_file ]; do sleep 2; done
done

while IFS=' ' read -r hostname ipAddr xclbin devId
do
    msg="hostname: $hostname, ipAddr: $ipAddr, xclbin: $xclbin, devID: $devId"
    echo "$msg"
    ssh -f $hostname  "cd $AlveoLinkPath/network/udp/scripts;  bash arpdisc.sh $hostname $AlveoLinkPath $xclbin $devId"
    sleep 1
done <"$1"

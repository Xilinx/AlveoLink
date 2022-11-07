#!/usr/bin/env bash

#usage ./run_compile.sh numKernel filepath hopnumber batchsize

CUR_DIR=$(pwd)
AL_PATH=$(awk 'sub(/AlveoLink.*/, "")' <<< "$CUR_DIR")AlveoLink

ssh -f host01  "cd $AL_PATH/tests/kernel/sync_dv/tm_managers; bash tm_managers.sh host01 1 $1"

ssh -f host03  "cd $AL_PATH/tests/kernel/sync_dv/tm_managers; bash tm_managers.sh host03 0 $1"

sleep 1

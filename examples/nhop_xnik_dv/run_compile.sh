#!/usr/bin/env bash

#usage ./run_compile.sh

CUR_DIR=$(pwd)
AL_PATH=$CUR_DIR/../../AlveoLink

ssh -f host01  "cd $CUR_DIR; bash compile.sh host01"
sleep 1

ssh -f host03  "cd $CUR_DIR; bash compile.sh host03"
sleep 1

ssh -f host04  "cd $CUR_DIR; bash compile.sh host04"
sleep 1

ssh -f host03  "cd $AL_PATH/tests/kernel/sync_dv/tm_managers; bash compile.sh host03"
sleep 1

ssh -f host01  "cd $AL_PATH/tests/kernel/sync_dv/tm_managers; bash compile.sh host01"
sleep 1

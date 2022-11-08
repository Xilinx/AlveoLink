#!/usr/bin/env bash

#usage ./run_compile.sh numKernel filepath hopnumber batchsize

CUR_DIR=$(pwd)
AL_PATH=$CUR_DIR/../../AlveoLink

ssh -f host04  "cd $CUR_DIR; bash nhop.sh host04 1 2 0 $1 $2 $3 $4"
sleep 1

ssh -f host04  "cd $CUR_DIR; bash nhop.sh host04 0 2 2 $1 $2 $3 $4"
sleep 1

ssh -f host03  "cd $CUR_DIR; bash nhop.sh host03 1 1 4 $1 $2 $3 $4"
sleep 1

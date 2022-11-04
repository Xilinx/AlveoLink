#!/usr/bin/env bash

#usage ./run_compile.sh numKernel filepath hopnumber batchsize

CUR_DIR=$(pwd)
graphanalyticsPath=$(awk 'sub(/graphanalytics.*/, "")' <<< "$CUR_DIR")graphanalytics
AL_PATH=$graphanalyticsPath/../AlveoLink

ssh -f xsj-dxgradb04  "cd $graphanalyticsPath/L2/nhop_xnik_dv; bash nhop.sh xsj-dxgradb04 1 2 0 $1 $2 $3 $4"
sleep 1

ssh -f xsj-dxgradb04  "cd $graphanalyticsPath/L2/nhop_xnik_dv; bash nhop.sh xsj-dxgradb04 0 2 2 $1 $2 $3 $4"
sleep 1

ssh -f xsj-dxgradb03  "cd $graphanalyticsPath/L2/nhop_xnik_dv; bash nhop.sh xsj-dxgradb03 1 1 4 $1 $2 $3 $4"
sleep 1

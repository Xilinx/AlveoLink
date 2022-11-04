#!/usr/bin/env bash

#usage ./run_compile.sh

CUR_DIR=$(pwd)
graphanalyticsPath=$(awk 'sub(/graphanalytics.*/, "")' <<< "$CUR_DIR")graphanalytics
AL_PATH=$graphanalyticsPath/../AlveoLink

ssh -f xsj-dxgradb01  "cd $graphanalyticsPath/L2/nhop_xnik_dv; bash compile.sh xsj-dxgradb01"
sleep 1

ssh -f xsj-dxgradb03  "cd $graphanalyticsPath/L2/nhop_xnik_dv; bash compile.sh xsj-dxgradb03"
sleep 1

ssh -f xsj-dxgradb04  "cd $graphanalyticsPath/L2/nhop_xnik_dv; bash compile.sh xsj-dxgradb04"
sleep 1

ssh -f xsj-dxgradb03  "cd $AL_PATH/tests/kernel/sync_dv/tm_managers; bash compile.sh xsj-dxgradb03"
sleep 1

ssh -f xsj-dxgradb01  "cd $AL_PATH/tests/kernel/sync_dv/tm_managers; bash compile.sh xsj-dxgradb01"
sleep 1

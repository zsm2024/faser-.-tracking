#!/bin/bash

runno=$1
num=$2
filesPerJob=$3
ptag=$4

WD=$PWD

cd /afs/cern.ch/user/d/dfellers/faser
source setup.sh
cd build
source x86_64-centos7-gcc11-opt/setup.sh
cd $WD
echo "Starting analysis"
analyzeRun.py $runno $num $filesPerJob $ptag
cp Data-tuple*.root /eos/project/f/faser-commissioning/DeionsNtuples/$runno/

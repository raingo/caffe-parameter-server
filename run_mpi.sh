#!/usr/bin/env sh

NumberOfProcesses=$1
script=$2
LockingFlag=$3

dist_dir=distribute
export LD_LIBRARY_PATH=$dist_dir/deps:$LD_LIBRARY_PATH
export PATH=$dist_dir/deps:$PATH
export DYLD_LIBRARY_PATH=$dist_dir/deps:$DYLD_LIBRARY_PATH

while true
do
    $script $NumberOfProcesses $LockingFlag
    err=$?
    if [ $err == "0" ]
    then
        break
    fi
    break
done

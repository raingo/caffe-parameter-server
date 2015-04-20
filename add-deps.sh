#!/bin/sh
# vim ft=sh

dist_dir=$1

BIN=$dist_dir/bin/caffe.bin
BIN_NAME=`basename $BIN`
scp $BIN cycle3:
ssh cycle3 "ldd $BIN_NAME | grep not | awk '{print \$1}'" > $dist_dir/.deps
mkdir -p $dist_dir/deps

for dep in `cat $dist_dir/.deps`
do
    ldd $BIN | grep $dep | awk '{print $3}' | xargs -L 1 -I {} cp {} $dist_dir/deps
done

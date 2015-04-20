#!/bin/sh
# vim ft=sh

node=$1
if [ -z "$node" ]
then
    echo usage $0 node
    exit
fi

#dist_dir=cycle3:/home/vax1/scratch/yli-data/caffe-dist
dist_dir=$node:/localdisk/yli-data/caffe-dist

rsync -arv distribute $dist_dir

#mkl
#rsync -arv /u/qyou/intel/mkl/lib/intel64/*.so $dist_dir/distribute/deps
#rsync -arv /u/qyou/intel/composer_xe_2013_sp1.2.144/compiler/lib/intel64/*.so $dist_dir/distribute/deps

rsync -arv examples/cifar10 $dist_dir/examples
rsync -arv examples/mnist $dist_dir/examples
rsync -arv mpi_env.csh $dist_dir
rsync -arv run_mpi.sh $dist_dir
rsync -arv hostfile $dist_dir
rsync -arv run_exp.sh $dist_dir
rsync -arv /u/yli/mpich/x86_64 cycle3:mpich/

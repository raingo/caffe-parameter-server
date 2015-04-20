#!/bin/sh
# vim ft=sh

mkdir -p logs
rm -f logs/*
scripts=(./examples/mnist/train_lenet.sh ./examples/cifar10/train_cifar10.sh)
#scripts=(./examples/cifar10/train_cifar10.sh)
#scripts=(./examples/mnist/train_lenet.sh)
n_procs=(24 20 16 12 8 4 2 1)
#n_procs=(2)
#n_procs=(24)

for n_proc in ${n_procs[@]}
do
    for script in ${scripts[@]}
    do
        log_name=`basename $script`-$n_proc-lock
        echo $log_name
        ./run_mpi.sh $n_proc $script -locking 2>&1 | tee logs/$log_name

        log_name=`basename $script`-$n_proc-unlock
        echo $log_name
        ./run_mpi.sh $n_proc $script 2>&1 | tee logs/$log_name
    done
done

echo $0 `hostname` | mail -s "done" yli

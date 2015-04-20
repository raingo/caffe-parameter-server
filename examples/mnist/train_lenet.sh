#!/usr/bin/env sh
dist_dir=distribute
mpirun -f hostfile -np $1 ./$dist_dir/bin/caffe.bin train --solver=examples/mnist/lenet_solver_mpi.prototxt $2

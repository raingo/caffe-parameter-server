#!/usr/bin/env sh

dist_dir=distribute
mpirun -f hostfile -np $1 ./$dist_dir/bin/caffe.bin train --solver=examples/cifar10/cifar10_quick_step_solver.prototxt $2

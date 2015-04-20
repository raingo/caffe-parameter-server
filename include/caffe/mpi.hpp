#ifndef HEADER_MPI
#define HEADER_MPI

namespace caffe {

extern int MPI_N_Proc;
extern int MPI_My_Rank;

template <typename Dtype>
    void update_mpi(int sz, int param_id, const Dtype *diff, Dtype *data);

}

#endif

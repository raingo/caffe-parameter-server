#ifndef HEADER_PARAMSERVER
#define HEADER_PARAMSERVER

#include <mpi.h>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <vector>
#include <glog/logging.h>

//caffe_axpy should be defined before including ParamServer.hpp
//template <typename Dtype>
//void caffe_axpy(const int N, const Dtype alpha, const Dtype* X,
//        Dtype* Y);

namespace caffe {

    /// Copy from boost MPI
    template<typename T> MPI_Datatype get_mpi_datatype(const T&v)
    {
        throw "Function not yet implemented.";
    }

#define BOOST_MPI_DATATYPE(CppType, MPIType)                      \
    template<>                                                              \
    inline MPI_Datatype                                                     \
    get_mpi_datatype< CppType >(const CppType &v) { return MPIType; }

    BOOST_MPI_DATATYPE(float, MPI_FLOAT);
    BOOST_MPI_DATATYPE(double, MPI_DOUBLE);

    template<typename ParamType>
        class ParamServer {
            typedef typename ParamType::element_type Dtype;

        public:
            ParamServer() {rank_ = -1;}

            void init(int *argc, char ***argv)
            {
                int provided;
                MPI_Init_thread(argc, argv, MPI_THREAD_MULTIPLE, &provided);
                CHECK_EQ(provided, MPI_THREAD_MULTIPLE) << "require multi thread MPI support";
                MPI_Comm_rank(MPI_COMM_WORLD, &rank_);
            }

            void sleep()
            {
                boost::this_thread::sleep(boost::posix_time::milliseconds(100));
            }

            void sync()
            {
                DLOG(INFO) << "syncing";
                MPI_Barrier(MPI_COMM_WORLD);
                DLOG(INFO) << "synced";
            }
            virtual ~ParamServer ()
            {
                if (listen_thr)
                {
                    int dummy = 1;
                    MPI_Send(&dummy, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
                    listen_thr -> join();
                }

                if (rank_ != -1)
                    MPI_Finalize();
            }

            void set_params(const std::vector<boost::shared_ptr<ParamType> > &params)
            {
                params_ = params;
            }

            void update(int i)
            {
                static int update_counters = 0;
                MPI_Status status;
                boost::shared_ptr<ParamType> ptr = params_[i];
                DLOG(INFO) << "updating " << i << " from " << rank_;
                if (rank_ == 0)
                {
                    if (locking_)
                        update_lock[i].lock();
                    caffe_axpy<Dtype>(ptr -> count(), Dtype(-1), ptr -> cpu_diff(), ptr -> mutable_cpu_data());
                    if (locking_)
                        update_lock[i].unlock();

                    DLOG(INFO) << "updated";
                    ++update_counters;

                    if (update_counters % 100000 == 0)
                        LOG(INFO) << "update_counters " << update_counters;
                }
                else
                {
                    DLOG(INFO) << rank_ << " sending " << i;
                    MPI_Send((void*)ptr -> cpu_diff(), ptr -> count(), get_mpi_datatype<Dtype>(Dtype(0.0)), 0, i, MPI_COMM_WORLD);
                    DLOG(INFO) << rank_ << " sent " << i;

                    MPI_Recv((void*)ptr -> mutable_cpu_data(), ptr -> count(), get_mpi_datatype<Dtype>(Dtype(0.0)), 0, i, MPI_COMM_WORLD, &status);
                    DLOG(INFO) << rank_ << " received " << i;
                }
            }

            void listen(bool locking = false)
            {
                if (rank_ == 0)
                {
                    locking_ = locking;
                    if (locking_)
                        update_lock.reset(new boost::mutex[params_.size()]);
                    listen_thr.reset(new boost::thread(&ParamServer::do_listen, this));
                }
            }

            int rank()
            {
                return rank_;
            }

        private:
            int rank_;
            boost::shared_ptr<boost::thread> listen_thr;
            boost::shared_array<boost::mutex> update_lock;
            bool locking_;
            std::vector<boost::shared_ptr<ParamType> > params_;

            void handle_message(int param_id, int node_id)
            {
                MPI_Status status;
                boost::shared_ptr<ParamType> ptr = params_[param_id];
                MPI_Recv((void*)ptr -> mutable_cpu_diff(), ptr -> count(), get_mpi_datatype<Dtype>(Dtype(0.0)), node_id, param_id, MPI_COMM_WORLD, &status);

                DLOG(INFO) << "got " << param_id << " at " << node_id;

                update(param_id);

                MPI_Send((void*)ptr -> cpu_data(), ptr -> count(), get_mpi_datatype<Dtype>(Dtype(0.0)), node_id, param_id, MPI_COMM_WORLD);

                DLOG(INFO) << "done with " << param_id << " at " << node_id;
            }


            void do_listen()
            {
                while(true)
                {
                    MPI_Status status;


                    //int MPI_Probe(int source, int tag, MPI_Comm comm, MPI_Status *status)
                    MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                    DLOG(INFO) << "got " << status.MPI_TAG << " at " << status.MPI_SOURCE;

                    if (status.MPI_SOURCE == rank_)
                    {
                        int dummy;
                        MPI_Recv(&dummy, 1, MPI_INT, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, &status);
                        break;
                    }

                    handle_message(status.MPI_TAG, status.MPI_SOURCE);
                    //boost::thread t = boost::thread(&ParamServer::handle_message, this, status.MPI_SOURCE, status.MPI_TAG);
                    //t.detach();
                }
            }
        };
}


#endif

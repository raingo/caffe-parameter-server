# Set the default home directories for MPI and Cilk
setenv MPI_HOME /u/yli/mpich/x86

if (`uname -i` == "x86_64") then
setenv MPI_HOME /u/yli/mpich/x86_64
endif

# set MPI binary and library paths
setenv PATH ${MPI_HOME}/bin:${PATH}

if ( "$?LD_LIBRARY_PATH" ) then
setenv LD_LIBRARY_PATH ${MPI_HOME}/lib:${LD_LIBRARY_PATH}
else
setenv LD_LIBRARY_PATH ${MPI_HOME}/lib
endif

setenv MPICH_USE_SHLIB yes

#include <stdio.h>
#include <stdlib.h>

#ifndef _EXTERN_C_
#ifdef __cplusplus
#define _EXTERN_C_ extern "C"
#else /* __cplusplus */
#define _EXTERN_C_
#endif /* __cplusplus */
#endif /* _EXTERN_C_ */

#include <omp.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>        
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>           /* For O_* constants */

#include <libunwind.h> // libunwind-version backtrace
#include "papi_wrap.h"

// This is a communicator that will contain the first 48 out of
// every 64 ranks in the applicatPAPI failed to read countersion.
//static MPI_Comm world48;
// static int rank, size, fd;
// static volatile int *addr;
// static int *addr_nonvol;
// static int cnt_send;


// backtrace info
extern VertexType vertex_type;
extern void *backtrace_buffer[];
extern int trace_size;

inline void get_backtrace()
{
    if (vertex_type!=VertexType::Function)
    // todo: revert his
    // trace_size = unw_backtrace(backtrace_buffer, (vertex_type==VertexType::CallSite)?4:10);
    trace_size = unw_backtrace(backtrace_buffer, 10);
}


_EXTERN_C_ int my_Init(int *argc, char ***argv) { 
    int _wrap_py_return_val = 0;
{
    get_backtrace();
    papi_update(1, 0, 0, 0, nullptr);
//    if (rank==0)
  //  {
    //    fprintf(stderr,"Vapro library loaded\n");
    //}
}    return _wrap_py_return_val;
}

_EXTERN_C_ int my_Barrier(/*MPI_Comm comm*/) { 
    int _wrap_py_return_val = 0;
{
    get_backtrace();
    papi_update(0, 23, 0, 0, nullptr);
    papi_update(1, 23, 0, 0, nullptr);
}    return _wrap_py_return_val;
}

_EXTERN_C_ double my_Wtime() { 
    double _wrap_py_return_val = 0;
{
    get_backtrace();
    papi_update(0, 359, 0, 0, nullptr);
    papi_update(1, 359, 0, 0, nullptr);
}    return _wrap_py_return_val;
}


_EXTERN_C_ int my_Finalize() { 
    int _wrap_py_return_val = 0;
{
    get_backtrace();
    papi_update(0, 1, 0, 0, nullptr);
	#pragma omp parallel
    {
        int total = omp_get_num_procs();
		int thread_id = omp_get_thread_num();
		fprintf(stderr, "done_thread_id%d\n", thread_id);
		print_graph(thread_id, total);
		//papi_destroy(thread_id);
		//PAPI_cleanup_evnetset(eventset[thread_id]);
		//PAPI_destroy_eventset(&eventset[thread_id]);
	}
}	return _wrap_py_return_val;
}



#include <stdio.h>
#include <stdlib.h>

#ifndef _EXTERN_C_
#ifdef __cplusplus
#define _EXTERN_C_ extern "C"
#else /* __cplusplus */
#define _EXTERN_C_
#endif /* __cplusplus */
#endif /* _EXTERN_C_ */

#include <dlfcn.h>
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
#include <climits>
#include <libunwind.h> // libunwind-version backtrace
#include "papi_wrap.h"
//#include <libgomp.h>
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
/*
extern struct gomp_team *gomp_new_team (unsigned);
extern void gomp_team_start (void (*) (void *), void *, unsigned,
			     unsigned, struct gomp_team *,
			     struct gomp_taskgroup *);
extern void gomp_team_end (void);
*/
inline void get_backtrace()
{
    if (vertex_type!=VertexType::Function)
    // todo: revert his
    // trace_size = unw_backtrace(backtrace_buffer, (vertex_type==VertexType::CallSite)?4:10);
    trace_size = unw_backtrace(backtrace_buffer, 10);
}

//void GOMP_parallel_end(void)
//{
//    static void (*GOMP_parallel_end_p) (void);
//    char* error;
//
//    fprintf(stdout, "GOMP_parallel_end begin\n");
//
//    if (!GOMP_parallel_end_p) {
//        *(void**)(&GOMP_parallel_end_p) = dlsym(RTLD_NEXT, "GOMP_parallel_end");
//        if ((error = dlerror()) != NULL) {
//            fputs(error, stderr);
//            exit(1);
//        }
//    }
//
//    GOMP_parallel_end_p();
//
//    fprintf(stdout, "GOMP_parallel_end end\n");
//}
//
//void GOMP_parallel_start(void (*fn) (void *), void *data, unsigned num_threads)
//{
//    static void (*GOMP_parallel_start_p) (void (*fn) (void *),
//            void *data, unsigned num_threads);
//    char* error;
//
//    if (!GOMP_parallel_start_p) {
//        *(void**)(&GOMP_parallel_start_p) = dlsym(RTLD_NEXT, "GOMP_parallel_start");
//        if ((error = dlerror()) != NULL) {
//            fputs(error, stderr);
//            exit(1);
//        }
//    }
//
//    fprintf(stdout, "GOMP_parallel_start begin\n");
//
//    GOMP_parallel_start_p(fn, data, num_threads);
//
//    fprintf(stdout, "GOMP_parallel_start end\n");
//}

_EXTERN_C_ void GOMP_parallel (void (*fn)(void *), void *data, unsigned num_threads, unsigned int flags) 
{
    static void (*GOMP_parallel_p) (void (*fn) (void *),
            void *data, unsigned num_threads);
    char* error;

    if (!GOMP_parallel_p) {
        *(void**)(&GOMP_parallel_p) = dlsym(RTLD_NEXT, "GOMP_parallel");
        if ((error = dlerror()) != NULL) {
            fputs(error, stderr);
            exit(1);
        }
    }

    fprintf(stdout, "GOMP_parallel_start begin\n");

    GOMP_parallel_p(fn, data, num_threads);

    fprintf(stdout, "GOMP_parallel_start end\n");
}

_EXTERN_C_ int my_Init(int *argc, char ***argv) { 
    int _wrap_py_return_val = 0;
{
    get_backtrace();
    papi_update(1, 0, 0, 0, nullptr);
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



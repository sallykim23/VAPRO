#include "pmc_wrapper.h"
#include <omp.h>
#include <papi.h>
#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include "papi_wrap.h"
// ====== papi ======
extern unsigned long long init_time;
extern unsigned long long last_time;
//extern int rank;
extern bool init_flag;
extern VertexType vertex_type;
#ifdef USE_PAPI
int eventset[100];
#define CPU_FREQ 2.3e9

static __inline__ unsigned long long rdtsc()
{
	unsigned hi, lo;
	__asm__ __volatile__("rdtsc"
			: "=a"(lo), "=d"(hi));
	return ((unsigned long long)lo) | (((unsigned long long)hi) << 32);
}


void papi_start_counters()
{
	int retval = PAPI_library_init(PAPI_VER_CURRENT);
	if (retval != PAPI_VER_CURRENT) {
		    fprintf(stderr, "Error inititalizing PAPI! %s\n", PAPI_strerror(retval));
	}

    int num_threads = omp_get_num_procs();
	fprintf(stderr,"numer of thread is %d\n",num_threads);
	for (int i = 0; i < num_threads; i++) {
		eventset[i] = PAPI_NULL;
	}
	
	#pragma omp parallel
	{
        retval = PAPI_thread_init(pthread_self);
        if ( retval != PAPI_OK ){
			fprintf(stderr, "Error thread init\n");
        }

		int thread_id = omp_get_thread_num();
		
        fprintf(stderr,"thread_id:%d\n", thread_id);
	
        retval = PAPI_create_eventset(&eventset[thread_id]);		
		if (retval != PAPI_OK) {
			fprintf(stderr,"Error creating eventset! %s\n", PAPI_strerror(retval));
		}

		retval = PAPI_add_named_event(eventset[thread_id], "PAPI_TOT_INS");
		if (retval != PAPI_OK) {
			fprintf(stderr, "Error adding event: %s\n", PAPI_strerror(retval));
		}
		
        PAPI_reset(eventset[thread_id]);
    	if ((retval = PAPI_start(eventset[thread_id])) != PAPI_OK){
            fprintf(stderr, "PAPI failed to start counters: %s\n", PAPI_strerror(retval));
        	exit(1);
    	}
	}		
}
void papi_init()
{
    int ret;
    init_time = rdtsc();
    if (PAPI_num_counters() < CNT_PAPI_EVENTS)
    {
        fprintf(stderr, "No hardware counters here, or PAPI not supported.\n");
        exit(1);
    }
    //if ((ret = PAPI_set_domain(PAPI_DOM_ALL)) != PAPI_OK)
    if ((ret = PAPI_set_domain(PAPI_DOM_USER | PAPI_DOM_KERNEL)) != PAPI_OK)
    //if ((ret = PAPI_set_domain(PAPI_DOM_USER)) != PAPI_OK)
    {
        fprintf(stderr, "PAPI failed to set_domain: %s\n", PAPI_strerror(ret));
        exit(1);
    }

    // get VERTEX_GRAIN
    char *vertex_path_str = getenv("VERTEX_GRAIN");
    int vertex_path_int = (vertex_path_str == 0) ? VertexType::CallSite : atoi(vertex_path_str);
    if (mpi_rank == 0)
        fprintf(stderr, "Papi inited: VERTEX_GRAIN=%d\n", vertex_path_int);
    assert(!(vertex_path_int < 0 || vertex_path_int > 2));
    if (vertex_path_int < 0 || vertex_path_int > 2)
    {
        fprintf(stderr, "vertex path environment variable error");
        exit(-1);
    }
    else
        vertex_type = (VertexType)vertex_path_int;
    if (mpi_rank == 0)
    {
        fprintf(stderr, "vertex type: %d\n", (int)vertex_type);
    }

     init_flag = true;
     papi_start_counters();
     PAPI_reset(eventset[0]);
}

DataType papi_get_data(ULL cur_time)
{
    int num_threads = omp_get_num_procs();
    fprintf(stderr,"current num_thread:%d\n", num_threads);
    long long* count= (long long *) malloc (num_threads * sizeof(long long));
    
    for (int i = 0; i < num_threads; i++){
	    count[i] = 0;
    }

    int ret,thread_id;
	
    #pragma omp parallel private(thread_id, ret)
    {
        thread_id = omp_get_thread_num();      
        fprintf(stderr,"read_thread_id:%d\n", thread_id); 
        //start time of the thread
        //double start_time = omp_get_wtime();
    
        if ((ret = PAPI_read(eventset[thread_id], &count[thread_id])) != PAPI_OK){
            fprintf(stderr, "PAPI failed to read counters: %s\n", PAPI_strerror(ret));
            exit(1);
        }

        //ending time of the thread
        //double end_time = omp_get_wtime();
        
        //count[num_threads + thread_id] = end_time * 1000000000;//end timestamp
		//count[2*num_threads + thread_id] = (end_time - start_time) * 1000000000;//elapsed t
	    //printf("timestamp:%f s\n", end_time);
	    //printf("elapsedtime:%f s\n", end_time - start_time);

        PAPI_reset(eventset[thread_id]);
	}

    DataType retv;
    retv.set_papi_data(num_threads, count);
    retv.elapsed = cur_time - last_time; // append wall time to locate this record
    retv.timestamp = cur_time;
    last_time = cur_time;
    return retv;
}

void papi_destroy(int thread_id){
	PAPI_cleanup_eventset(eventset[thread_id]);
	PAPI_destroy_eventset(&eventset[thread_id]);
}
#endif

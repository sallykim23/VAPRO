#include <vector>
#include <papi.h>
#include "papi_wrap.h"
//#define USE_JEVENTS
// #define USE_ASSEMBLY_RDPMC
#define USE_PAPI
//#define USE_RUSAGE

#ifdef USE_JEVENTS

void pmc_enable_real();
std::vector<unsigned long long> pmc_read_real();

#endif

#ifdef USE_PAPI
void papi_start_counters();
void papi_init();
DataType papi_get_data(ULL cur_time);
void papi_finalize();
void papi_destroy(int numthread);
#endif

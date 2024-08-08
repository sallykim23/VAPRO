#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <syscall.h>
#include <unistd.h>
#include <vector>
#include <linux/perf_event.h>
#include <sys/ioctl.h>
#include <cassert>
#include "jevents/rdpmc.h"
#include "jevents/jevents.h"
#include "jevents/perf-iter.h"
#include "papi_wrap.h"

#include <signal.h>
#include <sys/time.h>

using namespace std;
using ULL = unsigned long long;

rdpmc_ctx ctx_rdpmc[CNT_PAPI_EVENTS];

volatile ULL global_ret[CNT_PAPI_EVENTS] = { 0, };

// high-performance clock
static __inline__ unsigned long long rdtsc()
{
    unsigned hi, lo;
    __asm__ __volatile__("rdtsc"
                         : "=a"(lo), "=d"(hi));
    return ((unsigned long long)lo) | (((unsigned long long)hi) << 32);
}

const int CNT_PAPI_EVENTS_ALWAYS = 3;
const int CNT_PAPI_EVENTS_TOGGLE_1 = 2;
const int CNT_PAPI_EVENTS_TOGGLE_2 = 2;
const int CNT_PAPI_EVENTS_TOGGLE_3 = 2;
const int CNT_PAPI_EVENTS_TOGGLE_4 = 1;

const char *e_always[CNT_PAPI_EVENTS_ALWAYS] = {"cpu/instructions/",  // Fixed
                   "cpu_clk_unhalted.thread",         // Fixed
                   "cpu_clk_unhalted.ref_tsc"         // Fixed, For core utilization
                   };

const char *e_toggle_1[CNT_PAPI_EVENTS_TOGGLE_1] = {
                   "idq_uops_not_delivered.core",
                   "uops_retired.retire_slots"
                   };

const char *e_toggle_2[CNT_PAPI_EVENTS_TOGGLE_2] = {
                   "uops_issued.any",
                   "int_misc.recovery_cycles"
                   };

const char *e_toggle_3[CNT_PAPI_EVENTS_TOGGLE_3] = {
                   "cycle_activity.stalls_mem_any",
                   "cycle_activity.stalls_l1d_miss"
                   };

const char *e_toggle_4[CNT_PAPI_EVENTS_TOGGLE_4] = {
                   "cycle_activity.stalls_l2_miss"
                   };

int num_toggling_iterations = 0;

void handler(int sig) {
    static int action = 0;

    printf("current (%d): ", action);
    for (int i = 0; i < CNT_PAPI_EVENTS; ++i) {
        printf(" %llu (%llu) ", global_ret[i], &(global_ret[i]));
    }
    printf("\n");

    try {
        if (action == 0) {
            for (int i = 0; i < CNT_PAPI_EVENTS_TOGGLE_1; ++i) {
                int idx = 0;
                if (i == 0)
                    idx = 1;
                else if (i == 1)
                    idx = 2;
                global_ret[idx] += rdpmc_read(ctx_rdpmc+CNT_PAPI_EVENTS_ALWAYS+i);
            }

            for (int i = 0; i < CNT_PAPI_EVENTS_TOGGLE_1; ++i)
                rdpmc_close(ctx_rdpmc+CNT_PAPI_EVENTS_ALWAYS+i);

            assert(sizeof(e_toggle_2) / sizeof(*e_toggle_2) == CNT_PAPI_EVENTS_TOGGLE_2);
            perf_event_attr attr;
            for (int i = 0; i < CNT_PAPI_EVENTS_TOGGLE_2; ++i)
            {
                if (resolve_event(e_toggle_2[i], &attr) < 0) {
                    fprintf(stderr, "Event %d %s\n", i, e_toggle_2[i]);
                    assert(!"No event");
                }
                if (rdpmc_open_attr(&attr, ctx_rdpmc + CNT_PAPI_EVENTS_ALWAYS + i, ctx_rdpmc + 0) < 0) {
                    throw 400;
                    assert(!"Cannot open rdpmc");
                }
            }

        } else if (action == 1) {

            for (int i = 0; i < CNT_PAPI_EVENTS_TOGGLE_2; ++i) {
                int idx = 0;
                if (i == 0)
                    idx = 3;
                else if (i == 1)
                    idx = 4;
                global_ret[idx] += rdpmc_read(ctx_rdpmc+CNT_PAPI_EVENTS_ALWAYS+i);
            }

            for (int i = 0; i < CNT_PAPI_EVENTS_TOGGLE_2; ++i)
                rdpmc_close(ctx_rdpmc+CNT_PAPI_EVENTS_ALWAYS+i);

            assert(sizeof(e_toggle_3) / sizeof(*e_toggle_3) == CNT_PAPI_EVENTS_TOGGLE_3);
            perf_event_attr attr;
            for (int i = 0; i < CNT_PAPI_EVENTS_TOGGLE_3; ++i)
            {
                if (resolve_event(e_toggle_3[i], &attr) < 0) {
                    fprintf(stderr, "Event %d %s\n", i, e_toggle_3[i]);
                    assert(!"No event");
                }
                if (rdpmc_open_attr(&attr, ctx_rdpmc + CNT_PAPI_EVENTS_ALWAYS + i, ctx_rdpmc + 0) < 0) {
                    throw 400;
                    assert(!"Cannot open rdpmc");
                }
            }

        } else if (action == 2) {

            for (int i = 0; i < CNT_PAPI_EVENTS_TOGGLE_3; ++i) {
                int idx = 0;
                if (i == 0)
                    idx = 7;
                else if (i == 1)
                    idx = 8;
                global_ret[idx] += rdpmc_read(ctx_rdpmc+CNT_PAPI_EVENTS_ALWAYS+i);
            }

            for (int i = 0; i < CNT_PAPI_EVENTS_TOGGLE_3; ++i)
                rdpmc_close(ctx_rdpmc+CNT_PAPI_EVENTS_ALWAYS+i);

            assert(sizeof(e_toggle_4) / sizeof(*e_toggle_4) == CNT_PAPI_EVENTS_TOGGLE_4);
            perf_event_attr attr;
            for (int i = 0; i < CNT_PAPI_EVENTS_TOGGLE_4; ++i)
            {
                if (resolve_event(e_toggle_4[i], &attr) < 0) {
                    fprintf(stderr, "Event %d %s\n", i, e_toggle_4[i]);
                    assert(!"No event");
                }
                if (rdpmc_open_attr(&attr, ctx_rdpmc + CNT_PAPI_EVENTS_ALWAYS + i, ctx_rdpmc + 0) < 0) {
                    throw 400;
                    assert(!"Cannot open rdpmc");
                }
            }

        } else if (action == 3) {

            for (int i = 0; i < CNT_PAPI_EVENTS_TOGGLE_4; ++i) {
                int idx = 0;
                if (i == 0)
                    idx = 9;
                global_ret[idx] += rdpmc_read(ctx_rdpmc+CNT_PAPI_EVENTS_ALWAYS+i);
            }

            for (int i = 0; i < CNT_PAPI_EVENTS_TOGGLE_4; ++i)
                rdpmc_close(ctx_rdpmc+CNT_PAPI_EVENTS_ALWAYS+i);

            assert(sizeof(e_toggle_1) / sizeof(*e_toggle_1) == CNT_PAPI_EVENTS_TOGGLE_1);
            perf_event_attr attr;
            for (int i = 0; i < CNT_PAPI_EVENTS_TOGGLE_1; ++i)
            {
                if (resolve_event(e_toggle_1[i], &attr) < 0) {
                    fprintf(stderr, "Event %d %s\n", i, e_toggle_1[i]);
                    assert(!"No event");
                }
                if (rdpmc_open_attr(&attr, ctx_rdpmc + CNT_PAPI_EVENTS_ALWAYS + i, ctx_rdpmc + 0) < 0) {
                    throw 400;
                    assert(!"Cannot open rdpmc");
                }
            }
        }

    } catch (int num) {
        printf("something happend (%d)\n", num);
    }

    action += 1;
    if (action == 4) {
        num_toggling_iterations += 1;
        action = 0;
    }
}

void pmc_enable_real()
{
    printf("PMC_ENABLE_REAL (%d)\n", getpid());

    static int handler_init = 0;
    if (PMC_TOGGLING && !handler_init) {
        signal (SIGALRM, handler);

        struct itimerval tv;
        tv.it_value.tv_sec = 0; //0;
        tv.it_value.tv_usec = 200000; //100000; //100000;
        tv.it_interval.tv_sec = 0;
        tv.it_interval.tv_usec = 200000; //10000;

        if (setitimer(ITIMER_REAL, &tv, NULL) == -1) {
            assert("Failed to set timer.\n");
            //exit(0);
        }

        handler_init = 1;
    }

    num_toggling_iterations = 0;

    assert(sizeof(e_always) / sizeof(*e_always) == CNT_PAPI_EVENTS_ALWAYS);
    perf_event_attr attr;
    for (int i = 0; i < CNT_PAPI_EVENTS_ALWAYS; ++i)
    {
        if (resolve_event(e_always[i], &attr) < 0) {
            fprintf(stderr, "Event %d %s\n", i, e_always[i]);
            assert(!"No event");
        }
        if (rdpmc_open_attr(&attr, ctx_rdpmc + i, (i == 0) ? nullptr : ctx_rdpmc + 0) < 0)
            assert(!"Cannot open rdpmc");
    }
    assert(sizeof(e_toggle_1) / sizeof(*e_toggle_1) == CNT_PAPI_EVENTS_TOGGLE_1);
    for (int i = 0; i < CNT_PAPI_EVENTS_TOGGLE_1; ++i)
    {
        if (resolve_event(e_toggle_1[i], &attr) < 0) {
            fprintf(stderr, "Event %d %s\n", i, e_toggle_1[i]);
            assert(!"No event");
        }
        if (rdpmc_open_attr(&attr, ctx_rdpmc + CNT_PAPI_EVENTS_ALWAYS + i, ctx_rdpmc + 0) < 0)
            assert(!"Cannot open rdpmc");
    }
    printf("pmc_enable_real\n");
}

vector<ULL> pmc_read_real()
{
    if (PMC_TOGGLING)
        for (int i = 0; i < CNT_PAPI_EVENTS; ++i)
            if (i != 0 && i != 5 && i != 6)
                global_ret[i] *= num_toggling_iterations;

    for (int i = 0; i < CNT_PAPI_EVENTS_ALWAYS; ++i) {
        int idx = 0;
        if (i  == 0)
            idx = 0;
        else if (i == 1)
            idx = 5;
        else if (i == 2)
            idx = 6;
        global_ret[idx] = rdpmc_read(ctx_rdpmc+i);
    }

    printf("pmc_read_real pid %d\n", getpid());
    for (int i = 0; i < CNT_PAPI_EVENTS; ++i) {
        printf(" %llu", global_ret[i]);
    }
    printf("\n");

    vector<ULL> ret;
    for (int i = 0; i < CNT_PAPI_EVENTS; ++i) {
        ret.emplace_back(global_ret[i]);
    }

    for (int i = 0; i < CNT_PAPI_EVENTS; ++i) {
        global_ret[i] = 0;
    }
    num_toggling_iterations = 0;

    return ret;
}

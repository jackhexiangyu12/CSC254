#include "papi.h"
#include "set.hpp"
#include <iostream>
#include <string>
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <omp.h>
#include <cstdlib>
#include <functional>
#include <cassert>
#include <fstream>

template <typename SimpleSetImpl>
void test_simple_set(std::string fname, std::function<SimpleSetImpl()> createFn) {
    #pragma omp critical
    std::cout << "Running test for " << fname << " on thread #" << PAPI_thread_id() << std::endl;

    int eventset = PAPI_NULL;
    int ret = PAPI_create_eventset(&eventset);
    if (ret != PAPI_OK) {
        std::cerr << "PAPI could not create eventset!" << std::endl;
        exit(-1);
    }
    ret = PAPI_add_event(eventset, PAPI_TOT_INS);
    if (ret != PAPI_OK) {
        std::cerr << "PAPI could not add to eventset!" << std::endl;
        exit(-1);
    }

    std::ofstream fout(fname, std::ofstream::out | std::ofstream::trunc);

    long long insertInstrs, removeInstrs, containInstrs = 0;
    PAPI_start(eventset);
    
    std::srand(time(nullptr));

    int M = 1;
    int N = 1024 * 64;
    for (int n = M; n <= N; n += 10) {    
        SimpleSetImpl R = createFn();    

        // Warmup...
        for(int i = 0; i < 10; i++) {
            R += i;
            R -= i;
        }

        // Insertion
        PAPI_reset(eventset);
        for(int i = 0; i < n; i++) {
            R += i;
        }
        PAPI_read(eventset, &insertInstrs);

        // Contains
        PAPI_reset(eventset);
        for (int i = 0; i < n; i++) {
            assert(R.contains(i));
        }
        PAPI_read(eventset, &containInstrs);

        // Remove
        PAPI_reset(eventset);
        for (int i = 0; i < n; i++) {
            R -= i;
        }
        PAPI_read(eventset, &removeInstrs);

        // Skip the first 1% as it appears to cause some weird outliers in the data, likely due to warmup of cache.
        // Weird thing is that we create a new object in each iteration of the loop, yet it is consistently bad.
        if (n < (N * 0.01)) continue;
        fout << n << ", " << insertInstrs / n << ", " << containInstrs / n << ", " << removeInstrs / n << std::endl;
    }
    
    PAPI_stop(eventset, &insertInstrs);
}

template <typename RangeSetImpl>
void test_range_set(std::string fname, std::function<RangeSetImpl()> createFn) {
    #pragma omp critical
    std::cout << "Running test for " << fname << " on thread #" << PAPI_thread_id() << std::endl;

    int eventset = PAPI_NULL;
    int ret = PAPI_create_eventset(&eventset);
    if (ret != PAPI_OK) {
        std::cerr << "PAPI could not create eventset!" << std::endl;
        exit(-1);
    }
    ret = PAPI_add_event(eventset, PAPI_TOT_INS);
    if (ret != PAPI_OK) {
        std::cerr << "PAPI could not add to eventset!" << std::endl;
        exit(-1);
    }

    std::ofstream fout(fname, std::ofstream::out | std::ofstream::trunc);
    long long insertInstrs, removeInstrs, containInstrs = 0;
    PAPI_start(eventset);
    
    std::srand(time(nullptr));

    int M = 1;
    int N = 1024;
    for (int n = M; n <= N; n += 1) {    
        RangeSetImpl R = createFn();    

        // Warmup...
        for(int i = 0; i < 10; i++) {
            R += range<int>(i, true, i, true);
            R -= range<int>(i, true, i, true);
        }

        // Insertion
        PAPI_reset(eventset);
        for(int i = 0; i < n; i++) {
            R += range<int>(i * 64, true, (i + 1) * 64, false);
        }
        PAPI_read(eventset, &insertInstrs);

        // Contains
        PAPI_reset(eventset);
        for (int i = 0; i < n; i++) {
            // Midpoint between ranges inserted above
            assert(R.contains((int) (i * 64 + (i + 1) * 64) / 2));
        }
        PAPI_read(eventset, &containInstrs);

        // Remove
        PAPI_reset(eventset);
        for (int i = 0; i < n; i++) {
            R -= range<int>(i * 64, true, (i + 1) * 64, false);
        }
        PAPI_read(eventset, &removeInstrs);

        // Skip the first 1% as it appears to cause some weird outliers in the data, likely due to warmup of cache.
        // Weird thing is that we create a new object in each iteration of the loop, yet it is consistently bad.
        if (n < (N * 0.01)) continue;
        fout << n << ", " << insertInstrs / n << ", " << containInstrs / n << ", " << removeInstrs / n << std::endl;
    }
    
    PAPI_stop(eventset, &insertInstrs);
}

int main(void) {
    PAPI_library_init(PAPI_VER_CURRENT);

    int ret = PAPI_thread_init([] (void) -> unsigned long { return omp_get_thread_num(); });
    if (ret != PAPI_OK) {
        std::cerr << "PAPI could not initialize thread support!" << std::endl;
        exit(-1);
    }
    // Bound the time for running these tests by running them in parallel.
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            PAPI_register_thread();
            test_simple_set<std_simple_set<int>>("std_simple_set.dat", [] () { return std_simple_set<int>(); });
        }
        // #pragma omp section
        // {
        //     PAPI_register_thread();
        //     test_range_set<std_range_set<int>>("std_range_set.dat", [] () { return std_range_set<int>(); });
        // }
        // #pragma omp section
        // {
        //     PAPI_register_thread();
        //     test_simple_set<carray_simple_set<int>>("carray_simple_set.dat", [] () { return carray_simple_set<int>(0, 1024*64); });
        // }
        // #pragma omp section
        // {
        //     PAPI_register_thread();
        //     test_simple_set<hashed_simple_set<int>>("hashed_simple_set.dat", [] () { return hashed_simple_set<int>(1024*64); });
        // }
        #pragma omp section
        {
            PAPI_register_thread();
            test_simple_set<bin_search_simple_set<int>>("bin_search_simple_set.dat", [] () { return bin_search_simple_set<int>(1024*64); });
        }
        // #pragma omp section
        // {
        //     PAPI_register_thread();
        //     test_range_set<carray_range_set<int>>("carray_range_set.dat", [] () { return carray_range_set<int>(0, 1024*64); });
        // }
        // #pragma omp section
        // {
        //     PAPI_register_thread();
        //     test_range_set<hashed_range_set<int>>("hashed_range_set.dat", [] () { return hashed_range_set<int>(1024*64); });
        // }
        // #pragma omp section
        // {
        //     PAPI_register_thread();
        //     test_range_set<bin_search_range_set<int>>("bin_search_range_set.dat", [] () { return bin_search_range_set<int>(1024*64); });
        // }
    }
}
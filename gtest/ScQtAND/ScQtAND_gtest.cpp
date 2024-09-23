/** @file NeuRipes_gtest.cpp
 *  @brief The main file for the test program for testing
 * SystemC-implemented NeuRipes modules using Google-test
 *
 *  @author János Végh (jvegh)
 *  @bug No known bugs.
 */
// All systemc modules should include systemc.h header file
#include "gtest/gtest.h"
#include <systemc>
#include "sctestbench_simple.h"

//#define MAKE_TIME_BENCHMARKING  // uncomment to measure the time with benchmarking macros
//#include "MacroTimeBenchmarking.h"    // Must be after the define to have its effect
#define MAX_CLOCK_CYCLES 10000
using namespace std; using namespace sc_core;
//string simulation_name =  string(PROJECT_NAME) + "_BASIC" + " V" + string(PROJECT_VERSION) + " SystemC/gtest unit testing";

int sc_main(int argc, char* argv[]) {
    // turn off warnings about IEEE 1666 deprecated feature
    sc_core::sc_report_handler::set_actions( "/IEEE_Std_1666/deprecated",
                                               sc_core::SC_DO_NOTHING );
    sc_set_time_resolution(1,SC_US);
/*        chrono::steady_clock::time_point
    m_running_time; // A work variable, do not touch outside the macros
        std::chrono::duration< int64_t, nano>
    m_part_time, m_sum_time;    // Work variables, contain actual benchmark duration and their sum
    BENCHMARK_TIME_RESET(&m_running_time,&m_part_time,&m_sum_time);
    BENCHMARK_TIME_BEGIN(&m_running_time,&m_part_time);
*/
    testing::InitGoogleTest(&argc, argv);
/*    BENCHMARK_TIME_END(&m_running_time,&m_part_time,&m_sum_time);
    std::cerr << "Gtest for " << simulation_name << " inited in "
              << m_part_time.count()/1000 << " usecs" << std::endl;

//    sc_set_time_resolution(SCTIME_RESOLUTION);
    //Do whatever setup here you will need for your tests here
    //!! all SC-related object and connections must be established befor calling sc_start

    BENCHMARK_TIME_BEGIN(&m_running_time,&m_part_time);
    */
    SimpleTB_t   InitGTest("gtest_bus"); // Set up SystemC related testing
/*    GTestModule_bus InitGTest("gtest_bus"); // Set up SystemC related testing
    BENCHMARK_TIME_END(&m_running_time,&m_part_time,&m_sum_time);
    std::cerr << "Testbench for " << simulation_name << " prepared in "
              << m_part_time.count()/1000/1000 << " msecs" << std::endl;
    //Run the Simulation for "MAX_CLOCK_CYCLES  nanosecnds"
    BENCHMARK_TIME_BEGIN(&m_running_time,&m_part_time);
    */
       sc_start(MAX_CLOCK_CYCLES ,SC_US);
/*    BENCHMARK_TIME_END(&m_running_time,&m_part_time,&m_sum_time);
    std::cerr << "Tests for " << simulation_name << " run in "
              << m_part_time.count()/1000/1000 << " msecs" << std::endl;
    // Return here when no more events remained
    if (not sc_end_of_simulation_invoked()) sc_stop(); //< invoke end_of_simulation
    std::cerr  << "Exiting " << simulation_name << " total time "
               << m_sum_time.count()/1000/1000 << " msecs" << std::endl;
*/    return(0);
}


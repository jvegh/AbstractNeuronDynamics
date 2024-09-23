/** @file AP_Conceptual.cpp
 *  @brief The main file for the demo to generate a conceptual action potential
 *  SystemC-based program
 *
 *  @author János Végh (jvegh)
 *  @bug No known bugs.
 */
// All systemc modules should include systemc.h header file
#include <systemc>

//#define MAKE_TIME_BENCHMARKING  // uncomment to measure the time with benchmarking macros
//#include "MacroTimeBenchmarking.h"    // Must be after the define to have its effect
#define MAX_CLOCK_CYCLES 10000
using namespace std; using namespace sc_core;
//string simulation_name =  string(PROJECT_NAME) + "_BASIC" + " V" + string(PROJECT_VERSION) + " SystemC/gtest unit testing";

int sc_main(int argc, char* argv[]) {
    // turn off warnings about IEEE 1666 deprecated feature
    sc_core::sc_report_handler::set_actions( "/IEEE_Std_1666/deprecated",
                                               sc_core::SC_DO_NOTHING );
       sc_start(MAX_CLOCK_CYCLES ,SC_US);
    return(0);
}


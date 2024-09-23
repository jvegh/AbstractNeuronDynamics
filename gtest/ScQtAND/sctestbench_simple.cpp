/** @file sctestbench.cpp
 *  @ingroup GENCOMP_MODULE_TEST
 *  @brief  The abstract framework for testing SystemC modules
 */
/*
 *  @author János Végh (jvegh)
 *  @bug No known bugs.
*/
#include <systemc>
#include "gtest/gtest.h"
using namespace sc_core;
using std::cout;
using std::endl;
//#include <chrono>
#include "sctestbench_simple.h"
using namespace std;

void SimpleTB_t::test_thread()
{
    ErrorCode = RUN_ALL_TESTS();
    if (not sc_end_of_simulation_invoked())
        sc_stop(); //< invoke end_of_simulation
}//end testbench::test_thread


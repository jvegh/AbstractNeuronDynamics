#ifndef TESTBENCH_H
#define TESTBENCH_H
#include <systemc>
using namespace sc_core;

SC_MODULE(SimpleTB_t)
{
  // Constructor
public:
  SC_HAS_PROCESS(SimpleTB_t);
    public:
  SimpleTB_t( sc_module_name nm) : sc_module(nm)
  {
    SC_THREAD(test_thread);
  }

  int32_t ErrorCode_Get(){    return ErrorCode;}
  protected:
   void test_thread();
  int32_t ErrorCode;
};

#endif

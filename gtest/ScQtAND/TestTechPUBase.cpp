#include <gtest/gtest.h>
#include "scGenComp_PU_TechTest.h"

/*
 *  @author János Végh (jvegh)
 *  @bug No known bugs.
 */

// This section configures debug and log printing; must be located AFTER the other includes
//#define SUPPRESS_LOGGING // Suppress all log messages
//#define DEBUG_EVENTS    ///< Print event debug messages  for this module
//#define DEBUG_PRINTS    ///< Print general debug messages for this module
// Those defines must be located before 'DebugMacros.h", and are undefined in that file
#include "DebugMacros.h"


scGenComp_PU_TechTEST::
    scGenComp_PU_TechTEST(sc_core::sc_module_name nm   // Module name
                               ,int32_t No ):  // Heartbeat time
    scGenComp_PU_Tech (nm,No)

{   // Needed to avoid using SystemC specific syntax
    typedef scGenComp_PU_Tech  SC_CURRENT_USER_MODULE;
    // This routine will be called after initalizations but before starting simulation
    //   SC_THREAD(Initialize_method);
    //    sensitive << Demo_Event;
    //dont_initialize(); // A simple method for starting the demo

    // ** Do not reimplement any of the xxx_method functions
    // until you know what you are doing
};

scGenComp_PU_TechTEST::
    ~scGenComp_PU_TechTEST(void){}



// For demonstration purposes, drives generic 'Tech' PU through all states
// Prepare events for the demo unit; runs before the other 'method's
// at 120 ms finishes 'Processing'
void scGenComp_PU_TechTEST::
    Initialize_Do()
{
    //    InputCurrent_Set(5); // Use 5 nA constant input current

    // Set up which events are to be monitored
    // group and module observing are enabled by default
    ObservingBit_Set(gcob_ObserveInput, true);
    ObservingBit_Set(gcob_ObserveInitialize, true);
    ObservingBit_Set(gcob_ObserveProcessingBegin, true);
    ObservingBit_Set(gcob_ObserveProcessingBegin, true);
    ObservingBit_Set(gcob_ObserveHeartbeat, true);
    //    wait(313,SC_US);

    //    EVENT_GenComp.Initialize.notify(SC_ZERO_TIME);
    //    Heartbeat_Set(TECH_DEMO_HEARTBEAT_TIME); // Just to speed up demo
    HeartbeatTime_Set(sc_core::sc_time(256,SC_US));
    DEBUG_SC_PRINT_LOCAL("Heartbeat is set to: "  << sc_time_String_Get(mHeartbeat, SC_TIME_UNIT_DEFAULT)<< " " << SC_TIME_UNIT[SC_TIME_UNIT_DEFAULT]
                                                 << ", in " << HeartbeatDivisions_Get() << " divisions");
    //??    EVENT_GenComp.InputReceived.notify(20,SC_MS);
    // Not really needed: done in constructor
    //??            DEBUG_SC_PRINT("Will issue 'InputReceived' @ 10 ms");
    //??    wait(10,SC_MS);
    EVENT_GenComp.InputReceived.notify(313,SC_US);
    DEBUG_SC_EVENT("DEMO_DRIVER SENT #0 EVENT_GenComp.InputReceived  @0,313 (+000) us");
    DEBUG_SC_EVENT("DEMO_DRIVER XPCT EVENT_GenComp.ProcessingBegin");
    // Receiving an input, also starts 'Processing'
    // The BPU starts to receive spikes
    //    wait();
    EVENT_GenComp.InputReceived.notify(200,SC_US);
    DEBUG_SC_EVENT("DEMO_DRIVER SENT #1 EVENT_GenComp.InputReceived  @0,313 (+200) us");
    //    wait(500,SC_US);
    EVENT_GenComp.InputReceived.notify(500,SC_US);
    DEBUG_SC_EVENT("DEMO_DRIVER SENT #2 InputReceived  @0,313 (+700) us");
    // Now we sent 3 spikes
    // At local time 900, the membrane threshold potential exceeded
    //    wait(200,SC_US);
    // Now we are at @10,900 (900) us
    /*            EVENT_GenComp.Synchronize.notify(SC_ZERO_TIME);
            DEBUG_SC_EVENT("DEMO_DRIVER SENT EVENT_GenComp.Synchronized");
            DEBUG_SC_PRINT("Another 'Syncronized' @10,900 (+900 us");
*/
    // The BPU starts to receive spikes
    //    wait(50,SC_US);
    DEBUG_SC_EVENT("Now processing must be over");
}

scGenComp_PU_TechTEST TPU("myTech", 2);

// A new test class  of these is created for each test
class TEST_TechPU : public testing::Test
{
public:
    virtual void SetUp()
    {
        DEBUG_PRINT("TechTest started");
     }

    virtual void TearDown()
    {
        DEBUG_PRINT("TechTest terminated");
    }
};

/**
 * Tests the generic Technical processing unit
 */
TEST_F(TEST_TechPU, Basic)
{

    // We begin testing independently
    TPU.EVENT_GenComp.Initialize.notify(SC_ZERO_TIME); // Be sure we are reset
    wait(SC_ZERO_TIME);
//    wait(BPU.EVENT_GenComp.Initialize);
    //
    TPU.HeartbeatTime_Set(sc_core::sc_time(128,SC_US));
    TPU.HeartbeatDivisions_Set(8);
    EXPECT_EQ( gcsm_Ready, TPU.StateFlag_Get());  // The unit is initialized to 'Ready' state
}


#include <gtest/gtest.h>
#include "NeuronDemoTest.h"


#include "DebugMacros.h"

NeuronDemoTEST::
    NeuronDemoTEST(sc_core::sc_module_name nm ):  // Module name
   NeuronDemo (nm)
{ /// Just an empty constructor
};

NeuronDemoTEST::
     ~NeuronDemoTEST(void){}

NeuronDemoTEST NPU("NeuronDemo");

// A new test class  of these is created for each test
class TEST_NeuronDemoPU : public testing::Test
{
public:
    virtual void SetUp()
    {
        DEBUG_SC_PRINT(NPU.name(),">>>Begin test");
    }

    virtual void TearDown()
    {
        while(sc_pending_activity())
            wait(sc_time_to_pending_activity());
        DEBUG_SC_PRINT(DPU.name(),"<<<End test");
    }
};

#define PU_InitialDelayTime1 sc_core::sc_time(63,SC_US)
#define PU_InitialDelayTime sc_core::sc_time(100,SC_US)
#define PU_SynchronizeTime1 sc_core::sc_time(80,SC_US) // During "Relaxing", before initial
#define PU_SynchronizeTime2 sc_core::sc_time(120,SC_US) // During "Delivering"
#define PU_InputTime1       sc_core::sc_time(191,SC_US) // Normal input
#define PU_InputTime2       sc_core::sc_time(197,SC_US) // Normal input
#define PU_InputTime3       sc_core::sc_time(205,SC_US) // Normal input
#define PU_InputTime4       sc_core::sc_time(213,SC_US) // Normal input

/*
 * Generate a single Action Potential-like membrane voltage, with simulated threshold exceeding
 * Imitates chargeup and discharge
 */
TEST_F(TEST_NeuronDemoPU, SingleAP)
{
    sc_core::sc_time BaseTime = sc_core::sc_time_stamp();
    EXPECT_EQ(0,NPU.MembraneRelativePotential_Get());
    // We begin testing independently; no initialization at the beginning
    NPU.EVENT_GenComp.InputReceived.notify(PU_InputTime1);
            DEBUG_SC_EVENT(NPU.name(),"Schd 'InputReceived' @" <<sc_time_String_Get(PU_InputTime1-BaseTime));
    wait(NPU.EVENT_GenComp.InputReceived);
            EXPECT_EQ(0+MembraneInputIncrease,NPU.MembraneRelativePotential_Get());
    NPU.EVENT_GenComp.InputReceived.notify(PU_InputTime2-PU_InputTime1);
            DEBUG_SC_EVENT(NPU.name(),"Schd 'InputReceived' @" <<sc_time_String_Get(PU_InputTime2));
    wait(NPU.EVENT_GenComp.InputReceived);
            EXPECT_EQ(0+2*MembraneInputIncrease,NPU.MembraneRelativePotential_Get());
    NPU.EVENT_GenComp.InputReceived.notify(PU_InputTime3-PU_InputTime2);
            DEBUG_SC_EVENT(NPU.name(),"Schd 'InputReceived' @" <<sc_time_String_Get(PU_InputTime3));
    wait(NPU.EVENT_GenComp.InputReceived);
            // After 3 inputs plus one discharge
            EXPECT_EQ(0+3*MembraneInputIncrease-MembraneRelaxDischarge,NPU.MembraneRelativePotential_Get());
    NPU.EVENT_GenComp.InputReceived.notify(PU_InputTime4-PU_InputTime3);
            DEBUG_SC_EVENT(NPU.name(),"Schd 'InputReceived' @" <<sc_time_String_Get(PU_InputTime4));
    wait(NPU.EVENT_GenComp.InputReceived);
            EXPECT_EQ(0+4*MembraneInputIncrease-2*MembraneRelaxDischarge,NPU.MembraneRelativePotential_Get());

    EXPECT_EQ(PU_InputTime4,sc_core::sc_time_stamp());
     wait(NPU.EVENT_GenComp.DeliveringBegin);
    EXPECT_EQ(4,NPU.NoOfInputsReceived_Get());
    EXPECT_EQ(PU_InputTime1+3*HEARTBEAT_TIME_DEFAULT,sc_core::sc_time_stamp());
    EXPECT_TRUE(NPU.MembraneRelativePotential_Get()>ThresholdPotential);

    EXPECT_EQ( gcsm_Delivering, NPU.StageFlag_Get());  // The unit is relaxing; initially
     wait(NPU.EVENT_GenComp.DeliveringEnd);

    NPU.EVENT_GenComp.InputReceived.notify(5*HEARTBEAT_TIME_DEFAULT);
     DEBUG_SC_EVENT(NPU.name(),"Schd 'InputReceived' @" <<sc_time_String_Get(PU_InputTime1-BaseTime));
     // 6 heartbeats later there appears a new input in the    /**

    wait(NPU.EVENT_GenComp.RelaxingBegin);
    EXPECT_EQ(sc_time_stamp(),NPU.scRelaxingBeginTime_Get());
    EXPECT_EQ(NPU.MembraneRelativePotential_Get(),ThresholdPotential-1);
     // Check if discharge in relaxing works
    // Check if memory works (receiving input in non-RestingMembranePotential works
    wait(NPU.EVENT_GenComp.InputReceived);
//    wait(NPU.EVENT_GenComp.ComputingBegin); Just imitated
    // Idle period will only change at 'ComputingBegin'
    EXPECT_EQ(sc_time(PU_InputTime1),NPU.scIdlePeriod_Get());
    NPU.EVENT_GenComp.InputReceived.notify(6*HEARTBEAT_TIME_DEFAULT);
    DEBUG_SC_EVENT(NPU.name(),"Schd 'InputReceived' @" <<sc_time_String_Get(PU_InputTime1-BaseTime));
    // A new input at non-resting potential received
    EXPECT_EQ(NPU.MembraneRelativePotential_Get(),ThresholdPotential+6 -5-6);
    wait(50*HEARTBEAT_TIME_DEFAULT);
    // A self-consistency check
    EXPECT_EQ(sc_time(30,SC_US),NPU.scResultPeriod_Get());
    EXPECT_EQ(sc_time(1.281,SC_MS),NPU.scRelaxingBeginTime_Get());
    // Check if relaxing completed
    EXPECT_EQ(NPU.MembraneRelativePotential_Get(),0);
}

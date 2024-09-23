#include <gtest/gtest.h>
#include "NeuronPhysicalTest.h"


#include "DebugMacros.h"

NeuronPhysicalTEST::
    NeuronPhysicalTEST(sc_core::sc_module_name nm ):  // Module name
   NeuronPhysical (nm)
{ /// Just an empty constructor
};

NeuronPhysicalTEST::
     ~NeuronPhysicalTEST(void){}

NeuronPhysicalTEST NPPU("NeuronPhysical");

// A new test class  of these is created for each test
class TEST_NeuronPhysicalPU : public testing::Test
{
public:
    virtual void SetUp()
    {
        DEBUG_SC_PRINT(NPPU.name(),">>>Begin test");
    }

    virtual void TearDown()
    {
        while(sc_pending_activity())
            wait(sc_time_to_pending_activity());
        DEBUG_SC_PRINT(NPPU.name(),"<<<End test");
    }
};

#define PU_InitialDelayTime1 sc_core::sc_time(63,SC_US)
#define PU_InitialDelayTime sc_core::sc_time(100,SC_US)
#define PU_SynchronizeTime1 sc_core::sc_time(80,SC_US) // During "Relaxing", before initial
#define PU_SynchronizeTime2 sc_core::sc_time(120,SC_US) // During "Delivering"
#define PU_InputTime1       sc_core::sc_time(190,SC_US) // Normal input
#define PU_InputTime2       sc_core::sc_time(197,SC_US) // Normal input
#define PU_InputTime3       sc_core::sc_time(205,SC_US) // Normal input
#define PU_InputTime4       sc_core::sc_time(213,SC_US) // Normal input

/*
 * Generate a single Action Potential-like membrane voltage, with simulated threshold exceeding
 * Imitates chargeup and discharge
 */
TEST_F(TEST_NeuronPhysicalPU, SingleAP)
{
    sc_core::sc_time BaseTime = sc_core::sc_time_stamp();
    EXPECT_EQ(0,NPPU.MembraneRelativePotential_Get());
    // We begin testing independently; no initialization at the beginning
    NPPU.EVENT_GenComp.InputReceived.notify(sc_time(8,SC_US));
            DEBUG_SC_EVENT(NPPU.name(),"Schd 'InputReceived' @" <<sc_time_String_Get(sc_time(8,SC_US)+BaseTime));
    wait(NPPU.EVENT_GenComp.InputReceived);
    NPPU.EVENT_GenComp.InputReceived.notify(sc_time(19,SC_US));
            DEBUG_SC_EVENT(NPPU.name(),"Schd 'InputReceived' @" <<sc_time_String_Get(sc_time(17,SC_US)+BaseTime));
    wait(NPPU.EVENT_GenComp.InputReceived);
            NPPU.EVENT_GenComp.InputReceived.notify(sc_time(17,SC_US));
            DEBUG_SC_EVENT(NPPU.name(),"Schd 'InputReceived' @" <<sc_time_String_Get(sc_time(24,SC_US)+BaseTime));
    wait(NPPU.EVENT_GenComp.InputReceived);
/*            EXPECT_EQ(0+MembraneInputIncrease,NPPU.RelativeMembranePotential_Get());
    NPPU.EVENT_GenComp.InputReceived.notify(PU_InputTime2-PU_InputTime1);
            DEBUG_SC_EVENT(NPPU.name(),"Schd 'InputReceived' @" <<sc_time_String_Get(PU_InputTime2));
    wait(NPPU.EVENT_GenComp.InputReceived);
            EXPECT_EQ(0+2*MembraneInputIncrease,NPPU.RelativeMembranePotential_Get());
    NPPU.EVENT_GenComp.InputReceived.notify(PU_InputTime3-PU_InputTime2);
            DEBUG_SC_EVENT(NPPU.name(),"Schd 'InputReceived' @" <<sc_time_String_Get(PU_InputTime3));
    wait(NPPU.EVENT_GenComp.InputReceived);
            // After 3 inputs plus one discharge
            EXPECT_EQ(0+3*MembraneInputIncrease-MembraneRelaxDischarge,NPPU.RelativeMembranePotential_Get());
    NPPU.EVENT_GenComp.InputReceived.notify(PU_InputTime4-PU_InputTime3);
            DEBUG_SC_EVENT(NPPU.name(),"Schd 'InputReceived' @" <<sc_time_String_Get(PU_InputTime4));
    wait(NPPU.EVENT_GenComp.InputReceived);
            EXPECT_EQ(0+4*MembraneInputIncrease-2*MembraneRelaxDischarge,NPPU.RelativeMembranePotential_Get());

    EXPECT_EQ(PU_InputTime4,sc_core::sc_time_stamp());
     wait(NPPU.EVENT_GenComp.DeliveringBegin);
    EXPECT_EQ(4,NPPU.NoOfInputsReceived_Get());
    EXPECT_EQ(PU_InputTime1+3*HEARTBEAT_TIME_DEFAULT,sc_core::sc_time_stamp());
    EXPECT_TRUE(NPPU.RelativeMembranePotential_Get()>ThresholdPotential);

    EXPECT_EQ( gcsm_Delivering, NPPU.StateFlag_Get());  // The unit is relaxing; initially
     wait(NPPU.EVENT_GenComp.DeliveringEnd);

    NPPU.EVENT_GenComp.InputReceived.notify(3*HEARTBEAT_TIME_DEFAULT);
     // 3 heartbeats later there appears a new input in the
    wait(NPPU.EVENT_GenComp.RelaxingBegin);
    EXPECT_EQ(sc_time_stamp(),NPPU.scRelaxingBeginTime_Get());
    EXPECT_EQ(NPPU.RelativeMembranePotential_Get(),-2*ThresholdPotential);
     // Check if discharge in relaxing works
    // Check if memory works (receiving input in non-RestingMembranePotential works
    wait(NPPU.EVENT_GenComp.InputReceived);
//    wait(NPPU.EVENT_GenComp.ComputingBegin); Just imitated
    // Idle period will only change at 'ComputingBegin'
    EXPECT_EQ(sc_time(PU_InputTime1),NPPU.scIdlePeriod_Get());
    // A new input at non-resting potential received
    EXPECT_EQ(NPPU.RelativeMembranePotential_Get(),-2*ThresholdPotential+3*MembraneRelaxDischarge + MembraneInputIncrease);
    wait(50*HEARTBEAT_TIME_DEFAULT);
    // A self-consistency check
    EXPECT_EQ(sc_time(30,SC_US),NPPU.scResultPeriod_Get());
    EXPECT_EQ(sc_time(1.38,SC_MS),NPPU.scRelaxingBeginTime_Get());
    // Check if relaxing completed
    EXPECT_EQ(NPPU.RelativeMembranePotential_Get(),0);
*/
}

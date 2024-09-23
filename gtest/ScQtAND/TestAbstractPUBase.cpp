#include <gtest/gtest.h>
#include "scGenComp_PU_AbstractTest.h"


/*
 *  @author János Végh (jvegh)
 *  @bug No known bugs.
 */

// This section configures debug and log printing; must be located AFTER the other includes
//#define SUPPRESS_LOGGING // Suppress all log messages
#define DEBUG_EVENTS    ///< Print event debug messages  for this module
//#define DEBUG_PRINTS    ///< Print general debug messages for this module
// Those defines must be located before 'DebugMacros.h", and are undefined in that file
#include "DebugMacros.h"

#define PU_ComputingPeriod sc_core::sc_time(40,SC_US)
#define PU_DeliveringPeriod sc_core::sc_time(60,SC_US)

#define PU_InitialDelayTime1 sc_core::sc_time(63,SC_US)
#define PU_InputTime2        sc_core::sc_time(87,SC_US) // Normal input
#define PU_InitialDelayTime  sc_core::sc_time(100,SC_US)
#define PU_SynchronizeTime1  sc_core::sc_time(80,SC_US) // During "Relaxing", before initial
#define PU_SynchronizeTime2  sc_core::sc_time(120,SC_US) // During "Delivering"
#define PU_InputTime1        sc_core::sc_time(190,SC_US) // Normal input
#define PU_SynchronizeTime3  sc_core::sc_time(200,SC_US) // During "Computing"
#define PU_FailedTime        PU_SynchronizeTime3+PU_DeliveringPeriod-sc_core::sc_time(15,SC_US) // After finishing test 'Synchronize'

scGenComp_PU_AbstractTEST::
    scGenComp_PU_AbstractTEST(sc_core::sc_module_name nm
                              ,sc_core::sc_time FixedComputingTime
                              ,sc_core::sc_time FixedDeliveringTime
                              ,bool ExternSynchronMode
                              // Module name
//                        ,sc_core::sc_time HeartBeat
                              ):  // Heartbeat time
   scGenComp_PU_Abstract (nm
                          ,FixedComputingTime
                          ,FixedDeliveringTime
                          ,ExternSynchronMode
                          )
{   // Needed to using SystemC-specific syntax
//    typedef scGenComp_PU_Abstract  SC_CURRENT_USER_MODULE;
};

scGenComp_PU_AbstractTEST::
     ~scGenComp_PU_AbstractTEST(void){}




/** @class	TestAbstractNeuron
 * @brief	Tests the operation of the objects for generalized computing
 * 
 */

scGenComp_PU_AbstractTEST APU("APU"
                               ); // For testing the non-timed, asynchron operation
scGenComp_PU_AbstractTEST TAPU("TAPU"
                              ,PU_ComputingPeriod
                              ,PU_DeliveringPeriod
                              ,false
                              ); // For testing timed, asynchron operation
scGenComp_PU_AbstractTEST CAPU("CAPU"
                               ,PU_ComputingPeriod
                               ,PU_DeliveringPeriod
                               ,true
                               );   // For testing clocked operation
/** Tests the generic abstract processing unit
 */

// A new test class  of these is created for each test
class TEST_AbstractPU : public testing::Test
{
public:
    virtual void SetUp()
    {
        DEBUG_SC_PRINT(APU.name(),">>>Begin test");
    }

    virtual void TearDown()
    {
        while(sc_pending_activity())
            wait(sc_time_to_pending_activity());
        DEBUG_SC_PRINT(APU.name(),"<<<End test");
    }
};

/*
 * Test if the states follow each other as expected
 *
 * !!! Only one instance of the same event can be in the queue
 * !!! The 'wait' test have been removed because the 'SC_ZERO_TIME' waiting may change events' order
 * // No practical importance. See the CirculatingTimed test instead
 */

TEST_F(TEST_AbstractPU, CirculatingUnTimed)
{
    sc_core::sc_time BaseTime = sc_time_stamp();
    // We begin testing independently
    // @63: initialize
    APU.EVENT_GenComp.Initialize.notify(PU_InitialDelayTime1); // Be sure we are reset
    DEBUG_SC_EVENT(APU.name(),"Schd 'Initialize' @" << sc_time_String_Get(PU_InitialDelayTime1));
     // We begin testing independently; no initialization at the beginning
    wait(APU.EVENT_GenComp.Initialize);
    // We can schedule 'InputReceived' only now: 'Initialize' cancels all events
    APU.EVENT_GenComp.InputReceived.notify(PU_InputTime2-PU_InitialDelayTime1);
    DEBUG_SC_EVENT(APU.name(),"Schd 'InputReceived' @" <<sc_time_String_Get(PU_InputTime2));
    EXPECT_EQ(PU_InitialDelayTime1-BaseTime,sc_core::sc_time_stamp());
    EXPECT_EQ(gcsm_Relaxing, APU.StateFlag_Get());  // The unit is 'Relaxing'
     // The time of last computation was the initialization time
    wait(APU.EVENT_GenComp.InputReceived);
    EXPECT_EQ(PU_InputTime2,sc_core::sc_time_stamp()-BaseTime);
    // After receiving input, the unit passes to 'Computing'
    EXPECT_EQ(gcsm_Computing, APU.StateFlag_Get());  // The unit is computing

    EXPECT_EQ(APU.scLocalTimeBase_Get(), sc_core::sc_time_stamp() );
    EXPECT_EQ(PU_InputTime2, sc_core::sc_time_stamp() );
//    wait(APU.EVENT_GenComp.ComputingBegin );
//    wait(APU.EVENT_GenComp.Heartbeat);
   EXPECT_EQ(PU_InputTime2+SC_ZERO_TIME, sc_core::sc_time_stamp() ); // Does not need time
    // Now computing completed, delivering not started
 //   wait(APU.EVENT_GenComp.ComputingEnd | APU.EVENT_GenComp.Heartbeat);
 //   wait(APU.EVENT_GenComp.DeliveringBegin| APU.EVENT_GenComp.Heartbeat);
//    wait(APU.EVENT_GenComp.Heartbeat);
    EXPECT_EQ(PU_InputTime2+SC_ZERO_TIME, sc_core::sc_time_stamp()); // Does not need time
 //   EXPECT_EQ( gcsm_Delivering, APU.StateFlag_Get());  // The unit is relaxing
    EXPECT_EQ(APU.scIdlePeriod_Get(), PU_InputTime2);
    EXPECT_EQ(APU.scResultPeriod_Get(),  SC_ZERO_TIME); // The time of last computation was the initialization time
    EXPECT_EQ(APU.scDeliveringTimeBegin_Get()+PU_InputTime2,   sc_core::sc_time_stamp()); // The time of last computation was the initialization time
 //   EXPECT_EQ( gcsm_Delivering, APU.StateFlag_Get());  // The unit is 'Delivering'
//    wait(APU.EVENT_GenComp.DeliveringEnd | APU.EVENT_GenComp.Heartbeat);
    EXPECT_EQ(APU.scResultPeriod_Get(), SC_ZERO_TIME );
//    wait(APU.EVENT_GenComp.RelaxingBegin | APU.EVENT_GenComp.Heartbeat);
     EXPECT_EQ(APU.scRelaxingTimeBegin_Get()+PU_InputTime2,    sc_core::sc_time_stamp()); // The time of last computation was the initialization time
//    EXPECT_EQ( gcsm_Relaxing, APU.StateFlag_Get());  // The unit is relaxing
//    EXPECT_EQ(APU.scOperatingPeriod_Get(), SC_ZERO_TIME );
    // Now computing completed, delivering completed
    EXPECT_EQ(APU.scResultPeriod_Get(),  SC_ZERO_TIME);
    EXPECT_EQ(APU.scIdlePeriod_Get(), PU_InputTime2);
//    EXPECT_EQ(APU.OperationCounter_Get(), 1);
}

/*
 * Test if the states follow each other as expected
 */
TEST_F(TEST_AbstractPU, CirculatingTimed)
{
    sc_core::sc_time BaseTime = sc_time_stamp();
    // We begin testing independently
    // @63: initialize
    TAPU.EVENT_GenComp.Initialize.notify(PU_InitialDelayTime1); // Be sure we are reset
    DEBUG_SC_EVENT(TAPU.name(),"Schd 'Initialize' @" << sc_time_String_Get(BaseTime+PU_InitialDelayTime1));
         // We begin testing independently; no initialization at the beginning
    wait(TAPU.EVENT_GenComp.Initialize);
    // We can schedule 'InputReceived' only here
    TAPU.EVENT_GenComp.InputReceived.notify(PU_InputTime2-PU_InitialDelayTime1);
    DEBUG_SC_EVENT(TAPU.name(),"Schd 'InputReceived' @" <<sc_time_String_Get(PU_InputTime2+BaseTime));
    EXPECT_EQ(PU_InitialDelayTime1+BaseTime,sc_core::sc_time_stamp());
    EXPECT_EQ(gcsm_Relaxing, TAPU.StateFlag_Get());  // The unit is 'Relaxing'
        // The time of last computation was the initialization time
    wait(TAPU.EVENT_GenComp.InputReceived);
    EXPECT_EQ(PU_InputTime2+BaseTime,sc_time_stamp());
    // After receiving input, the unit passes to 'Computing'
    EXPECT_EQ(gcsm_Computing, TAPU.StateFlag_Get());  // The unit is computing

    EXPECT_EQ(TAPU.scLocalTimeBase_Get(), sc_core::sc_time_stamp() );
//    wait(TAPU.EVENT_GenComp.ComputingBegin); // No, uses implicit 'ComputingBegin'
    wait(TAPU.EVENT_GenComp.ComputingEnd); //
    EXPECT_EQ(PU_InputTime2+PU_ComputingPeriod, sc_core::sc_time_stamp()-BaseTime );
    EXPECT_EQ(PU_ComputingPeriod,TAPU.scResultPeriod_Get());
    wait(TAPU.EVENT_GenComp.DeliveringEnd); //
    EXPECT_EQ(PU_InputTime2+PU_ComputingPeriod+PU_DeliveringPeriod, sc_core::sc_time_stamp()-BaseTime );
    wait(TAPU.EVENT_GenComp.RelaxingBegin); //
    EXPECT_EQ(PU_ComputingPeriod+PU_DeliveringPeriod,TAPU.scProcessingPeriod_Get());
    // Now computing completed, delivering completed
    EXPECT_EQ(TAPU.scResultPeriod_Get(),  PU_ComputingPeriod);
    EXPECT_EQ(TAPU.scIdlePeriod_Get(), PU_InputTime2+BaseTime);
    EXPECT_EQ(TAPU.OperationCounter_Get(), 1);
    EXPECT_EQ(TAPU.scLocalTimeBase_Get(), sc_time_stamp()-TAPU.scProcessingPeriod_Get());
    EXPECT_EQ(TAPU.scRelaxingTimeBegin_Get(),sc_time_stamp());  // The point we can continue from
}// of CirculatingTimed



/*
 * Test if synchronization and failing works
 *
 * Issues XXX.notify() events, and checks for the right alignment of events and execution times
 */

TEST_F(TEST_AbstractPU, FailingTimed)
{
    // We begin testing independently; no initialization at the beginning
    sc_core::sc_time BaseTime = sc_time_stamp();

    // Attempt in "Relaxing"
    TAPU.EVENT_GenComp.Synchronize.notify(PU_SynchronizeTime1); // Attempt in "Relaxing"
    DEBUG_SC_EVENT(TAPU.name(),"Schd 'Synchronize' @" << sc_time_String_Get(BaseTime+PU_SynchronizeTime1));
    wait(TAPU.EVENT_GenComp.Synchronize);
    EXPECT_EQ(PU_SynchronizeTime1, sc_time_stamp()-BaseTime);
    EXPECT_EQ(gcsm_Delivering, TAPU.StateFlag_Get());  // The unit is 'Delivering'

    // Attempt in "Delivering"
    TAPU.EVENT_GenComp.InputReceived.notify(HEARTBEAT_TIME_DEFAULT/2);
    DEBUG_SC_EVENT(TAPU.name(),"Schd 'InputReceived' @" << sc_time_String_Get(BaseTime+PU_SynchronizeTime1+HEARTBEAT_TIME_DEFAULT/2));
    wait(TAPU.EVENT_GenComp.InputReceived);
    EXPECT_EQ(PU_SynchronizeTime1+HEARTBEAT_TIME_DEFAULT/2+BaseTime, sc_time_stamp());
    EXPECT_EQ(gcsm_Delivering, TAPU.StateFlag_Get());

    // Attempt in "Computing"
    TAPU.EVENT_GenComp.InputReceived.notify(PU_InputTime1-PU_SynchronizeTime1-HEARTBEAT_TIME_DEFAULT/2);
    DEBUG_SC_EVENT(TAPU.name(),"Schd 'InputRecieved' @" << sc_time_String_Get(PU_InputTime1));
    wait(TAPU.EVENT_GenComp.InputReceived);
    EXPECT_EQ(PU_InputTime1+BaseTime, sc_time_stamp());
    EXPECT_EQ(gcsm_Computing, TAPU.StateFlag_Get());
    TAPU.EVENT_GenComp.Synchronize.notify(HEARTBEAT_TIME_DEFAULT/2); // Attempt in "Relaxing"
    DEBUG_SC_EVENT(TAPU.name(),"Schd 'Synchronize' @" << sc_time_String_Get(BaseTime+PU_InputTime1));
    wait(TAPU.EVENT_GenComp.Synchronize);
    EXPECT_EQ(BaseTime+PU_InputTime1+HEARTBEAT_TIME_DEFAULT/2, sc_time_stamp());
    EXPECT_EQ(gcsm_Delivering, TAPU.StateFlag_Get());
    // received 'Synchronize' in 'Computing', begin immediate 'Delivering'
    DEBUG_SC_EVENT(TAPU.name(),"Schd 'DeliveringEnd' @" << sc_time_String_Get(PU_DeliveringPeriod));
    wait(TAPU.EVENT_GenComp.DeliveringEnd);
    wait(TAPU.EVENT_GenComp.RelaxingBegin);
    EXPECT_EQ(BaseTime+PU_InputTime1+HEARTBEAT_TIME_DEFAULT/2+PU_DeliveringPeriod, sc_time_stamp());
    EXPECT_EQ(gcsm_Relaxing, TAPU.StateFlag_Get());

    // Fail in any state
    TAPU.EVENT_GenComp.Failed.notify(PU_SynchronizeTime3-sc_time_stamp()); // Attempt in "Relaxing"
    DEBUG_SC_EVENT(TAPU.name(),"Schd 'Failed' @" << sc_time_String_Get(PU_SynchronizeTime3-sc_time_stamp()));
    wait(TAPU.EVENT_GenComp.Failed);
    EXPECT_EQ(BaseTime+PU_SynchronizeTime3+PU_DeliveringPeriod-HEARTBEAT_TIME_DEFAULT/2, sc_time_stamp());
    EXPECT_EQ(gcsm_Relaxing, TAPU.StateFlag_Get());
 }


#include <gtest/gtest.h>
#include "scGenComp_PU_BioTest.h"

/*
 *  @author János Végh (jvegh)
 *  @bug No known bugs.
 */

// This section configures debug and log printing; must be located AFTER the other includes
//#define SUPPRESS_LOGGING // Suppress all log messages
#define DEBUG_EVENTS    ///< Print event debug messages  for this module
#define DEBUG_PRINTS    ///< Print general debug messages for this module
// Those defines must be located before 'DebugMacros.h", and are undefined in that file
#include "DebugMacros.h"

#define PU_ComputingPeriod sc_core::sc_time(40,SC_US)
#define PU_DeliveringPeriod sc_core::sc_time(60,SC_US)

#define PU_InitialDelayTime1 sc_core::sc_time(63,SC_US)
#define PU_InputTime2       sc_core::sc_time(87,SC_US) // Normal input
#define PU_InitialDelayTime sc_core::sc_time(100,SC_US)
#define PU_SynchronizeTime1 sc_core::sc_time(80,SC_US) // During "Relaxing", before initial
#define PU_SynchronizeTime2 sc_core::sc_time(120,SC_US) // During "Delivering"
#define PU_InputTime1       sc_core::sc_time(190,SC_US) // Normal input
#define PU_SynchronizeTime3 sc_core::sc_time(200,SC_US) // During "Computing"
#define PU_FailedTime       PU_SynchronizeTime3+PU_DeliveringPeriod-sc_core::sc_time(15,SC_US) // After finishing test 'Synchronize'


/**
 * @brief scGenComp_PU_BioDemo::scGenComp_PU_BioDemo
 * @param nm Module name
 *
 * This class demonstrates how a biological neuron can be handled as a biological processing unit
 */
scGenComp_PU_BioTEST::
    scGenComp_PU_BioTEST(sc_core::sc_module_name nm   // Module name
                         ):  // Heartbeat time
    scGenComp_PU_Bio (nm
                     )
{   // Needed to avoid using SystemC specific syntax
    typedef scGenComp_PU_BioTEST  SC_CURRENT_USER_MODULE;
//    void ComputingBegin_Do(void );
//    void DeliveringBegin_Do(void);
};

scGenComp_PU_BioTEST::
    ~scGenComp_PU_BioTEST(void){}


    // This routine makes actual input processing, although most of the job is done in Process and Heartbeat
void scGenComp_PU_BioTEST::
    InputReceived_Do(void)
{
    EVENT_GenComp.ComputingBegin.notify(SC_ZERO_TIME); // If we have enough inputs, start processing
    DEBUG_SC_EVENT(name(),"SENT 'ComputingBegin in "<< GenCompStatesString[mStateFlag] << "'");
}
void scGenComp_PU_BioTEST::
    ComputingBegin_Do()
{
    EVENT_GenComp.ComputingEnd.notify(PU_ComputingPeriod); // Just for testing: issue a 'ProcessingEnd' immediately
    DEBUG_SC_PRINT(name(),"SENT 'ComputingEnd in "<< GenCompStatesString[mStateFlag] << "'");
}

void scGenComp_PU_BioTEST::
    DeliveringBegin_Do()
{
    EVENT_GenComp.DeliveringEnd.notify(PU_DeliveringPeriod); // If we have enough inputs, start processing
    DEBUG_SC_PRINT(name(),"SENT 'DeliveringEnd in "<< GenCompStatesString[mStateFlag] << "'");
}


/** @class	TestBioNeuron
 * @brief	Tests the operation of the objects for generalized bio computing
 *
 */


scGenComp_PU_BioTEST BPU("myBioTest");

// A new test class  of these is created for each test
class TEST_BioPU : public testing::Test
{
public:
    virtual void SetUp()
    {
        DEBUG_SC_PRINT(BPU.name(),">>>Begin test");
    }

    virtual void TearDown()
    {
        while(sc_pending_activity())
            wait(sc_time_to_pending_activity());
        DEBUG_SC_PRINT(BPU.name(),"<<<End test");
    }
};

/*
 * Test if the states follow each other as expected
 */
TEST_F(TEST_BioPU, Circulating)
{
/*    // @63: initialize
    BPU.EVENT_GenComp.Initialize.notify(PU_InitialDelayTime1); // Be sure we are reset
    BPU.EVENT_GenComp.InputReceived.notify(PU_InputTime2);
        // We begin testing independently; no initialization at the beginning
    sc_core::sc_time BaseTime = sc_time_stamp();
    wait(BPU.EVENT_GenComp.Initialize);
    EXPECT_EQ(gcsm_Relaxing, BPU.StateFlag_Get());  // The unit is 'Relaxing'
        // The time of last computation was the initialization time
    wait(BPU.EVENT_GenComp.InputReceived);
    wait(BPU.EVENT_GenComp.ComputingBegin);
    EXPECT_EQ(gcsm_Computing, BPU.StateFlag_Get());  // The unit is computing

    EXPECT_EQ(BPU.scLocalTimeBase_Get(), sc_core::sc_time_stamp() );
    EXPECT_EQ(PU_InputTime2 + BaseTime , sc_core::sc_time_stamp());
    wait(BPU.EVENT_GenComp.ComputingEnd);
    EXPECT_EQ(PU_InputTime2+PU_ComputingPeriod+BaseTime, sc_core::sc_time_stamp() );
    // Now computing completed, delivering not started
    wait(BPU.EVENT_GenComp.DeliveringBegin);
    EXPECT_EQ(PU_InputTime2+PU_ComputingPeriod+BaseTime, sc_core::sc_time_stamp()); // Does not need time
    EXPECT_EQ( gcsm_Delivering, BPU.StateFlag_Get());  // The unit is relaxing
    EXPECT_EQ(BPU.scIdlePeriod_Get()-BaseTime, PU_InputTime2);
    EXPECT_EQ(BPU.scResultPeriod_Get(),  PU_ComputingPeriod); // The time of last computation was the initialization time
    EXPECT_EQ(BPU.scDeliveringTimeBegin_Get(),  sc_core::sc_time_stamp()); // The time of last computation was the initialization time
    EXPECT_EQ( gcsm_Delivering, BPU.StateFlag_Get());  // The unit is 'Delivering'
    wait(BPU.EVENT_GenComp.DeliveringEnd);
    EXPECT_EQ(BPU.scResultPeriod_Get(), PU_ComputingPeriod );
    wait(BPU.EVENT_GenComp.RelaxingBegin);
    EXPECT_EQ(BPU.scRelaxingTimeBegin_Get(),  sc_core::sc_time_stamp()); // The time of last computation was the initialization time
    EXPECT_EQ(BPU.scProcessingPeriod_Get(), PU_ComputingPeriod + PU_DeliveringPeriod );
    // Now computing completed, delivering completed
    EXPECT_EQ(BPU.scResultPeriod_Get(),  PU_ComputingPeriod);
    EXPECT_EQ(BaseTime+PU_InputTime2+PU_ComputingPeriod+PU_DeliveringPeriod, sc_core::sc_time_stamp()); // We are in time
    EXPECT_EQ(PU_ComputingPeriod+PU_DeliveringPeriod, BPU.scProcessingPeriod_Get()); // The new period is correct
    EXPECT_EQ(BPU.scResultPeriod_Get(),  PU_ComputingPeriod);
    EXPECT_EQ( gcsm_Relaxing, BPU.StateFlag_Get());  // The unit is relaxing
*/
    sc_core::sc_time BaseTime = sc_time_stamp();
    // We begin testing independently
    // @63: initialize
    BPU.EVENT_GenComp.Initialize.notify(PU_InitialDelayTime1); // Be sure we are reset
    DEBUG_SC_EVENT(BPU.name(),"Schd 'Initialize' @" << sc_time_String_Get(BaseTime+PU_InitialDelayTime1));
        // We begin testing independently; no initialization at the beginning
    wait(BPU.EVENT_GenComp.Initialize);
    // We can schedule 'InputReceived' only here
    BPU.EVENT_GenComp.InputReceived.notify(PU_InputTime2-PU_InitialDelayTime1);
    DEBUG_SC_EVENT(BPU.name(),"Schd 'InputReceived' @" <<sc_time_String_Get(PU_InputTime2+BaseTime));
    EXPECT_EQ(PU_InitialDelayTime1+BaseTime,sc_core::sc_time_stamp());
    EXPECT_EQ(gcsm_Relaxing, BPU.StateFlag_Get());  // The unit is 'Relaxing'
        // The time of last computation was the initialization time
    wait(BPU.EVENT_GenComp.InputReceived);
    EXPECT_EQ(PU_InputTime2+BaseTime,sc_time_stamp());
    // After receiving input, the unit passes to 'Computing'
    EXPECT_EQ(gcsm_Computing, BPU.StateFlag_Get());  // The unit is computing

    EXPECT_EQ(BPU.scLocalTimeBase_Get(), sc_core::sc_time_stamp() );
    //    wait(BPU.EVENT_GenComp.ComputingBegin); // No, uses implicit 'ComputingBegin'
    wait(BPU.EVENT_GenComp.ComputingEnd); //
    EXPECT_EQ(PU_InputTime2+PU_ComputingPeriod, sc_core::sc_time_stamp()-BaseTime );
    EXPECT_EQ(PU_ComputingPeriod,BPU.scResultPeriod_Get());
    wait(BPU.EVENT_GenComp.DeliveringEnd); //
    EXPECT_EQ(PU_InputTime2+PU_ComputingPeriod+PU_DeliveringPeriod, sc_core::sc_time_stamp()-BaseTime );
    wait(BPU.EVENT_GenComp.RelaxingBegin); //
    EXPECT_EQ(PU_ComputingPeriod+PU_DeliveringPeriod,BPU.scProcessingPeriod_Get());
    // Now computing completed, delivering completed
    EXPECT_EQ(BPU.scResultPeriod_Get(),  PU_ComputingPeriod);
    EXPECT_EQ(BPU.scIdlePeriod_Get(), PU_InputTime2+BaseTime);
    EXPECT_EQ(BPU.OperationCounter_Get(), 1);
    EXPECT_EQ(BPU.scLocalTimeBase_Get(), sc_time_stamp()-BPU.scProcessingPeriod_Get());
    EXPECT_EQ(BPU.scRelaxingTimeBegin_Get(),sc_time_stamp());  // The point we can continue from

}
/*
 * Test if synchronization and failing works
 *
 * Issues XXX.notify() events, and checks for the right alignment of events and execution times
 */

TEST_F(TEST_BioPU, Synchronizing)
{
    // We begin testing independently; no initialization at the beginning
    // @80 Synchronize in 'Relaxing'; goes to 'Delivering'
    BPU.EVENT_GenComp.Synchronize.notify(PU_SynchronizeTime1); // Attempt in "Relaxing"
    DEBUG_SC_EVENT(BPU.name(),"RCVD 'Synchronize in 'Test'");
    //@100 Initialize in 'Delivering'; remains in 'Delivering'
    BPU.EVENT_GenComp.Initialize.notify(PU_InitialDelayTime); // Attempt in "Processing"
    DEBUG_SC_EVENT(BPU.name(),"RCVD 'Initialize in 'Test'");
    BPU.EVENT_GenComp.Synchronize.notify(PU_SynchronizeTime2); // Attempt in "Processing"
    DEBUG_SC_EVENT(BPU.name(),"RCVD 'Synchronize in 'Test'");
    BPU.EVENT_GenComp.InputReceived.notify(PU_InputTime1);
    DEBUG_SC_EVENT(BPU.name(),"RCVD 'InputReceived in 'Test'");
    BPU.EVENT_GenComp.Failed.notify(PU_FailedTime);
    DEBUG_SC_EVENT(BPU.name(),"RCVD 'Failed in 'Test'");
    sc_core::sc_time BaseTime = sc_time_stamp();

    // @80 Synchronize in 'Relaxing'; goes to 'Delivering'
    //@120 Synchronize in 'Delivering'; Neglected, remains in 'Delivering'
    //@190 Input in 'Relaxing'; Passes to 'Processing'
    //@200 Synchronize in 'Computing'; remains 'Computing'
    // Initially, the unit is in 'Relaxing'
    EXPECT_EQ( gcsm_Relaxing, BPU.StateFlag_Get());  // The unit is relaxing; initially

    // @80 Synchronize in 'Relaxing'; goes to 'Delivering'
    wait(BPU.EVENT_GenComp.Synchronize);
    EXPECT_EQ(PU_SynchronizeTime1,  sc_core::sc_time_stamp()-BaseTime);  // Synchronize request in 'Relaxing' acknowledged
    // and goes to 'Delivering'
    wait(BPU.EVENT_GenComp.DeliveringBegin);
    // Now the synchronize request arrived
    EXPECT_EQ( BPU.StateFlag_Get(), gcsm_Delivering);  // The unit is delivering; before initializing
    wait(BPU.EVENT_GenComp.Initialize);
    // OK, we initialized at @100
    EXPECT_EQ( BPU.StateFlag_Get(), gcsm_Relaxing);  // The unit is delivering; before initializing
    //@120 Synchronize in 'Relaxing'; Neglected, remains in 'Delivering'; delivery finished
    EXPECT_EQ(PU_InitialDelayTime+BaseTime,  sc_core::sc_time_stamp());
    EXPECT_EQ( BPU.StateFlag_Get(), gcsm_Relaxing);  // The unit is delivering; before initializing

    //@190 Input in 'Relaxing'; passes 'Computing'
    wait(BPU.EVENT_GenComp.InputReceived);
    wait(BPU.EVENT_GenComp.ComputingBegin); // A consequence of 'input'
    // This 'Synchronize' can be issued only after the previous one has been accepted!
    BPU.EVENT_GenComp.Synchronize.notify(PU_SynchronizeTime3-PU_InputTime1); // Attempt in "Delivering"
    EXPECT_EQ(PU_InputTime1+BaseTime,  sc_core::sc_time_stamp());  // Synchronize request in 'Relaxing' acknowledged
    EXPECT_EQ( BPU.StateFlag_Get(), gcsm_Computing);  // The unit is computing; before initializing

    //@200 Synchronize in 'Computing'; goes to 'Delivering'
    wait(BPU.EVENT_GenComp.Synchronize);
    EXPECT_EQ(PU_SynchronizeTime3+BaseTime,  sc_core::sc_time_stamp());  // Synchronize request in 'Relaxing' acknowledged
    EXPECT_EQ( BPU.StateFlag_Get(), gcsm_Computing);  // The unit is computing; before initializing
    // @245: Now test "Failing" event
    wait(BPU.EVENT_GenComp.Failed);
    EXPECT_EQ(PU_FailedTime+BaseTime,  sc_core::sc_time_stamp());  // Synchronize request in 'Relaxing' acknowledged
    // @260: cancelled 'ComputingEnd'
    /*  wait(BPU.EVENT_GenComp.DeliveringEnd); // Triggered by 'Synchronize'
   wait(BPU.EVENT_GenComp.RelaxingBegin); // Cancelled by 'Synchronize'
   EXPECT_EQ(PU_SynchronizeTime3+PU_DeliveringPeriod-BaseTime,  sc_core::sc_time_stamp());  // Synchronize request in 'Relaxing' acknowledged
   EXPECT_EQ( BPU.StateFlag_Get(), gcsm_Relaxing);  // The unit is computing; before initializing
*/

    wait(BPU.EVENT_GenComp.Initialize);
    EXPECT_EQ(PU_FailedTime+BaseTime,  sc_core::sc_time_stamp());  // Synchronize request in 'Relaxing' acknowledged
    EXPECT_EQ(BPU.StateFlag_Get(), gcsm_Relaxing);  // The unit is computing; before initializing
}

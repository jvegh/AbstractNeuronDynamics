/** @file scGenComp_PU_Abstract.cpp
 *  @ingroup GENCOMP_MODULE_PROCESS
 *  @brief  The abstract processing unit for generalized computing
 */
/*
 *  @author János Végh (jvegh)
 *  @bug No known bugs.
*/

/*
 *  If theses macros are not defined, no code is generated;
        the variables, however, must be defined (although they will be
        optimized out as unused ones).
        Alternatively, the macros may be protected with "\#ifdef MAKE_TIME_BENCHMARKING".
        The macros have source-module scope. All variables must be passed by reference.
*/

// Use this define in the derived module
#define MAKE_TIME_BENCHMARKING  // comment out if you do not want to benchmark clock time
#define SC_MAKE_TIME_BENCHMARKING  // comment out if you do not want to benchmark SystemC time

#include "scGenComp_PU_Abstract.h"

// This section configures debug and log printing
//#define SUPPRESS_LOGGING // Suppress all log messages
//#define DEBUG_EVENTS    ///< Print event debug event for this module
//#define DEBUG_PRINTS    ///< Print general debug messages for this module
// Those defines must be located before 'DebugMacros.h", and are undefined in that file
#include "DebugMacros.h"

string GenCompObserveStrings[]{"Group","Module","ComputingBegin","ComputingEnd",
                               "DeliveringBegin","DeliveringEnd","Heartbeat",
                               "Input","Initialize","RelaxingBegin","ValueChanged"};
string GenCompStagesString[]{"Sleeping", "Relaxing", "Computing", "Delivering"};

/*sc_core::sc_time PU_ComputingPeriod  = sc_core::sc_time(40,SC_US);
sc_core::sc_time PU_DeliveringPeriod = sc_core::sc_time(60,SC_US);
*/

    scGenComp_PU_Abstract::
scGenComp_PU_Abstract(sc_core::sc_module_name nm
                          ,sc_core::sc_time(FixedComputingTime)
                          ,sc_core::sc_time(FixedDeliveringTime)
                          ,bool CentralClockMode
                          ): sc_core::sc_module( nm)
    ,mStageFlag(gcsm_Relaxing)
    ,mLocalTimeBase(sc_core::sc_time_stamp())   /// Remember the beginning of the operation
    ,mRelaxingBeginTime(sc_core::sc_time_stamp())  /// End of the previous operation
    ,mOperationCounter(0)               /// Count performed operations
    ,mFixedComputingTime(FixedComputingTime)
    ,mFixedDeliveringTime(FixedDeliveringTime)
    ,mInputsReceived(0)
    ,m_Heartbeat_time_default(sc_core::sc_time(HEARTBEAT_TIME_DEFAULT,SC_NS))
    ,mCentralClockMode(CentralClockMode)
  {
    m_Heartbeat_time = m_Heartbeat_time_default;
    mObservedBits[gcob_ObserveGroup] = true;   // Enable this module for its group observing by default
    mObservedBits[gcob_ObserveModule] = true;   // Enable module observing by default

    Tracing_Initialize();
    // Make sure if not using variable computing time with a central clock
    assert(!((SC_ZERO_TIME == FixedComputingTime) && CentralClockMode));
    // *** The stuff below in the constructor are SystemC specific, do not touch!
    typedef scGenComp_PU_Abstract SC_CURRENT_USER_MODULE; // Needed if some routines are listed in this constructor
    // Operating
    SC_METHOD(DeliveringBegin_method);
    sensitive << EVENT_GenComp.DeliveringBegin;
    dont_initialize();
    SC_METHOD(DeliveringEnd_method);
    sensitive << EVENT_GenComp.DeliveringEnd;
    dont_initialize();
    SC_METHOD(Initialize_method);
    sensitive << EVENT_GenComp.Initialize;
//    dont_initialize();
    SC_METHOD(InputReceived_method);
    sensitive << EVENT_GenComp.InputReceived;
    dont_initialize();
    SC_METHOD(ComputingBegin_method);
    sensitive << EVENT_GenComp.ComputingBegin;
    dont_initialize();
    SC_METHOD(ComputingEnd_method);
    sensitive << EVENT_GenComp.ComputingEnd;
    dont_initialize();
    SC_METHOD(RelaxingBegin_method);
    sensitive << EVENT_GenComp.RelaxingBegin;
    dont_initialize();
    SC_METHOD(Heartbeat_method);
    sensitive << EVENT_GenComp.Heartbeat;
    dont_initialize();
    SC_METHOD(scClockUp_method);
    sensitive << EVENT_GenComp.ClockUp;
    dont_initialize();
    SC_METHOD(scClockDown_method);
    sensitive << EVENT_GenComp.ClockDown;
    dont_initialize();
 #ifdef USE_PU_HWSLEEPING
    // Power handling
    SC_METHOD(Sleep_method);
    sensitive << EVENT_GenComp.Sleep;
    dont_initialize();  
    SC_METHOD(Wakeup_method);
    sensitive << EVENT_GenComp.Wakeup;
    dont_initialize();
#endif// USE_PU_HWSLEEPING

    SC_METHOD(Failed_method);
    sensitive << EVENT_GenComp.Failed;
    dont_initialize();
    SC_METHOD(Synchronize_method);
    sensitive << EVENT_GenComp.Synchronize;
    dont_initialize();
 }

scGenComp_PU_Abstract::
~scGenComp_PU_Abstract(void)
{
    sc_close_vcd_trace_file(m_tracefile);
}

void scGenComp_PU_Abstract::
    Tracing_Initialize()
{
    m_tracefile = sc_create_vcd_trace_file(name());
}
// Cancel all possible events in the air
void scGenComp_PU_Abstract::
    CancelEvents(void)
{
    EVENT_GenComp.DeliveringBegin.cancel();
    EVENT_GenComp.DeliveringEnd.cancel();
    EVENT_GenComp.ComputingBegin.cancel();
    EVENT_GenComp.ComputingEnd.cancel();
    EVENT_GenComp.RelaxingBegin.cancel();
  /*    EVENT_GenComp.TransmittingBegin.cancel();
    EVENT_GenComp.TransmittingEnd.cancel();
    Implement later
*/
    EVENT_GenComp.Failed.cancel();
    EVENT_GenComp.Heartbeat.cancel();
    EVENT_GenComp.InputReceived.cancel();
    EVENT_GenComp.Synchronize.cancel();
                DEBUG_SC_EVENT(name(),"Cncl 'all events' in " << GenCompStagesString[mStageFlag] << "'");
}

//  Receive rising edge of an external clock and issue 'ComputingBegin'
void scGenComp_PU_Abstract::
    scClockUp_method(void)
{
    // Imitate 'ComputingBegin'; for central clock
            assert(mCentralClockMode);
    ComputingBegin_method();    // Imitate receiving 'ComputingBegin'
}

//  Receive falling edge of an external clock and issue 'ComputingEnd'
void scGenComp_PU_Abstract::
    scClockDown_method(void)
{
    // Imitate 'ComputingEnd'; for central clock
            assert(mCentralClockMode);
    ComputingEnd_method();    // Imitate receiving 'ComputingEnd'
}

// Called when the state 'processing' begins
void scGenComp_PU_Abstract::
    ComputingBegin_method()
{
        // Previous, the stage must have been 'Relaxing'
            #ifdef BENCHMARK_TIME_ACTIVE
                BENCHMARK_TIME_END(&t_Relaxing,&x_Relaxing,&s_Relaxing);
                TimeDuration_Relaxing_Print();
            #endif // BENCHMARK_TIME_ACTIVE
            #ifdef SC_BENCHMARK_TIME_ACTIVE
                SC_BENCHMARK_TIME_END(&SC_t_Relaxing,&SC_x_Relaxing,&SC_s_Relaxing);
                SC_TimeDuration_Relaxing_Print();
            #endif // SC_BENCHMARK_TIME_ACTIVE
                // And now begin the new computing
            #ifdef BENCHMARK_TIME_ACTIVE
                t_Computing =chrono::steady_clock::now();
            #endif // BENCHMARK_TIME_ACTIVE
            #ifdef SC_BENCHMARK_TIME_ACTIVE
                SC_t_Computing = sc_core::sc_time_stamp();
            #endif // SC_BENCHMARK_TIME_ACTIVE
    // Set the smallest heartbeat at the beginning of computing
    HeartbeatTime_Set(m_Heartbeat_time_resolution);
                // HeartbeatTime_Set(m_Heartbeat_time_default);
            assert(gcsm_Relaxing == StageFlag_Get()); // Must be set in stage 'Relaxing'
            assert(HaveEnoughInputs()); // And must receive input before computing
            DEBUG_SC_EVENT(name(),"RCVD 'ComputingBegin' in stage '" << GenCompStagesString[mStageFlag] << "'");
    mIdlePeriod = sc_core::sc_time_stamp() - mRelaxingBeginTime; // Remember idle time before this processing
    mLocalTimeBase = sc_core::sc_time_stamp();  // The clock is synchronized to the beginning of processing
    StageFlag_Set(gcsm_Computing);   // Pass to "Computing"
        //    Inputs.clear();
    ObserverNotify(gcob_ObserveComputingBegin);
    ComputingBegin_Do();         // Now perform the activity in the derived classes
    if(SC_ZERO_TIME==mFixedComputingTime)
    {   // We use variable time and heartbeat
        EVENT_GenComp.Heartbeat.notify(SC_ZERO_TIME);  // Use a minimum time
/*        m_FirstHeartbeatInStage = true;  // Mark we need correction
        EVENT_GenComp.Heartbeat.notify(m_Heartbeat_time_resolution);  // Use a minimum time
                DEBUG_SC_EVENT_LOCAL(scLocalTime_Get(),name(),"Schd 'Heartbeat' @"  << sc_time_String_Get(sc_time_stamp()+m_Heartbeat_time_default) << " in stage '"<< GenCompStagesString[mStageFlag]);
        m_t = scLocalTime_Get().to_seconds()*1000.;
*/
//        Heartbeat_Computing_Do();    // Make the calculation for 0th HB, still in 'Relaxing' mode
//                DEBUG_SC_EVENT_LOCAL(scLocalTime_Get(),name(),"Calculated in stage '"<< GenCompStagesString[mStageFlag] << "'");
     }
    else
    {
        EVENT_GenComp.ComputingEnd.notify(mFixedComputingTime);
                DEBUG_SC_EVENT_LOCAL(scLocalTime_Get(),name(),"Schd 'ComputingEnd' @" << sc_time_String_Get(sc_time_stamp()+mFixedComputingTime) << " in stage '"<< GenCompStagesString[mStageFlag] << "'" );
    }
}


// Called when the state 'processing' ends
void scGenComp_PU_Abstract::
    ComputingEnd_method()
{
            #ifdef BENCHMARK_TIME_ACTIVE
                BENCHMARK_TIME_END(&t_Computing,&x_Computing,&s_Computing);
                TimeDuration_Computing_Get();
            #endif // BENCHMARK_TIME_ACTIVE
            #ifdef SC_BENCHMARK_TIME_ACTIVE
                SC_BENCHMARK_TIME_END(&SC_t_Computing,&SC_x_Computing,&SC_s_Computing);
                SC_TimeDuration_Computing_Get();
            #endif // SC_BENCHMARK_TIME_ACTIVE
    //?? Set the smallest heartbeat at the beginning of delivering
    //?? HeartbeatTime_Set(m_Heartbeat_time_resolution);
    //HeartbeatTime_Set(m_Heartbeat_time_default);
    if(gcsm_Relaxing == StageFlag_Get())
    {   // Computing failed; omit delivering
                DEBUG_SC_EVENT_LOCAL(scLocalTime_Get(),name(),"RCVD 'ComputingEnd' in '" << GenCompStagesString[mStageFlag] << "': computing failed");
        StageFlag_Set(gcsm_Delivering); // Imitate normal computing end
        EVENT_GenComp.RelaxingBegin.notify(SC_ZERO_TIME);
                DEBUG_SC_EVENT_LOCAL(scLocalTime_Get(),name(),"SENT 'RelaxingBegin' in " << GenCompStagesString[mStageFlag] << "'");
    }
    else
    {  // Computing succeeded
 //       m_t = scLocalTime_Get().to_seconds()*1000.;
                DEBUG_SC_EVENT_LOCAL(scLocalTime_Get(),name(),"RCVD 'ComputingEnd' in '" << GenCompStagesString[mStageFlag] << "'");
                assert(gcsm_Computing == StageFlag_Get()); // Must be set by 'ComputingEnd'
        ComputingEnd_Do();          // Now perform the activity in the derived classes
        ObserverNotify(gcob_ObserveComputingEnd);
        mResultPeriod = scLocalTime_Get();
        EVENT_GenComp.DeliveringBegin.notify(SC_ZERO_TIME);
                DEBUG_SC_EVENT_LOCAL(scLocalTime_Get(),name(),"SENT 'DeliveringBegin' in '" << GenCompStagesString[mStageFlag]<< "'");
    }
}

// Called when the stage 'delivering' begins
void scGenComp_PU_Abstract::
    DeliveringBegin_method()
{
            #ifdef BENCHMARK_TIME_ACTIVE
                BENCHMARK_TIME_BEGIN(&t_Delivering,&x_Delivering);    // Begin the benchmarking here
            #endif // BENCHMARK_TIME_ACTIVE
            #ifdef SC_BENCHMARK_TIME_ACTIVE
                SC_BENCHMARK_TIME_BEGIN(&SC_t_Delivering,&SC_x_Delivering);    // Begin the benchmarking here
            #endif //SC_ BENCHMARK_TIME_ACTIVE
    // Set the smallest heartbeat at the beginning of computing
    HeartbeatTime_Set(m_Heartbeat_time_resolution);
        // HeartbeatTime_Set(m_Heartbeat_time_default);
                assert(gcsm_Computing == StageFlag_Get()); // Must be set by 'ComputingEnd'
                DEBUG_SC_EVENT_LOCAL(scLocalTime_Get(),name(),"RCVD 'DeliveringBegin in " << GenCompStagesString[mStageFlag]<< "'");
                mDeliveringBeginTime = scLocalTime_Get();
    StageFlag_Set(gcsm_Delivering);   // Pass to "Delivering"
    ObserverNotify(gcob_ObserveDeliveringBegin);
    if(mFixedComputingTime==SC_ZERO_TIME)
    {   // We use variable time
        EVENT_GenComp.Heartbeat.notify(SC_ZERO_TIME);  // Make an immediete calculation
                DEBUG_SC_EVENT_LOCAL(scLocalTime_Get(),name(),"Calculated in stage '"<< GenCompStagesString[mStageFlag] << "'");
 /*       DEBUG_SC_EVENT_LOCAL(scLocalTime_Get(),name(),"Schd 'Heartbeat' @" << sc_time_String_Get(sc_time_stamp()+m_Heartbeat_time_resolution) << " in stage '"<< GenCompStagesString[mStageFlag]
                                                                             << "'" );
        m_FirstHeartbeatInStage = true;  // Mark we need correction
        EVENT_GenComp.Heartbeat.notify(m_Heartbeat_time_resolution);  // Use a minimum time
                    DEBUG_SC_EVENT_LOCAL(scLocalTime_Get(),name(),"Schd 'Heartbeat' @" << sc_time_String_Get(sc_time_stamp()+m_Heartbeat_time_resolution) << " in stage '"<< GenCompStagesString[mStageFlag]
                                                                                << "'" );
*/
        m_t = scLocalTime_Get().to_seconds()*1000.;
        DeliveringBegin_Do();;  // Make the calculation for 0th HB
    }
    else
    {
        EVENT_GenComp.DeliveringEnd.notify(mFixedDeliveringTime);
        DEBUG_SC_EVENT_LOCAL(scLocalTime_Get(),name(),"Schd 'DeliveringEnd' in '"<< GenCompStagesString[mStageFlag] << "' @" << sc_time_String_Get(sc_time_stamp()+mFixedDeliveringTime));
    }
}


void scGenComp_PU_Abstract::
    DeliveringEnd_method()
{
            #ifdef BENCHMARK_TIME_ACTIVE
                BENCHMARK_TIME_END(&t_Delivering,&x_Delivering,&s_Delivering);
                TimeDuration_Delivering_Get();
            #endif // BENCHMARK_TIME_ACTIVE
            #ifdef SC_BENCHMARK_TIME_ACTIVE
                SC_BENCHMARK_TIME_END(&SC_t_Delivering,&SC_x_Delivering,&SC_s_Delivering);
                SC_TimeDuration_Delivering_Get();
            #endif // SC_BENCHMARK_TIME_ACTIVE
//    HeartbeatTime_Set(m_Heartbeat_time_default);
//    m_t = scLocalTime_Get().to_seconds()*1000.;
                DEBUG_SC_EVENT_LOCAL(scLocalTime_Get(),name(),"RCVD 'DeliveringEnd' in stage " << GenCompStagesString[mStageFlag] << "'");
                assert(StageFlag_Get() == gcsm_Delivering); // Must be set by 'DeliveringBegin'
    DeliveringEnd_Do();
    ++mOperationCounter;        // This operation finished
    ObserverNotify(gcob_ObserveDeliveringEnd);  // Inform observer
    EVENT_GenComp.RelaxingBegin.notify(SC_ZERO_TIME);
                DEBUG_SC_EVENT_LOCAL(scLocalTime_Get(),name(),"SENT 'RelaxingBegin' in stage " << GenCompStagesString[mStageFlag] << "'");
 }


void scGenComp_PU_Abstract::
    Failed_method(void)
{
                DEBUG_SC_EVENT(name(),"RCVD 'Failed  in " << GenCompStagesString[mStageFlag] << "'");
                DEBUG_SC_EVENT(name(),"Implicit 'Initialize'  in " << GenCompStagesString[mStageFlag] << "'");
    Initialize_method();
    Failed_Do();
}

// Adjusts the heartbeat time (integration step size)
void scGenComp_PU_Abstract::
    Heartbeat_Adjust(void)
{   // By default, make nothing. See the bio case
    m_t = scLocalTime_Get().to_seconds()*1000.; // Set the time for the calculation, in usec

}

// Called when the state 'processing' ends
void scGenComp_PU_Abstract::
    Heartbeat_method()
{
                DEBUG_SC_EVENT_LOCAL(scLocalTime_Get(),name(),"RCVD 'Heartbeat' in stage '"<< GenCompStagesString[mStageFlag] << "'");
    ObserverNotify(gcob_ObserveHeartbeat);
    Heartbeat_Adjust(); // The derived classes may need to adjust the integration time step
    bool StopHeartbeating = false;
    switch(StageFlag_Get())
    {
        case gcsm_Computing:
        {
            Heartbeat_Computing_Do();  // The voltage gradient may skip
            if((StopHeartbeating = Heartbeat_Computing_Stop()))
            {
                // Set the smallest heartbeat for delivering
//                HeartbeatTime_Set(m_Heartbeat_time_resolution);
                EVENT_GenComp.ComputingEnd.notify(SC_ZERO_TIME);
                    DEBUG_SC_EVENT_LOCAL(scLocalTime_Get(),name(),"SENT 'ComputingEnd' in stage '" << GenCompStagesString[mStageFlag] << "'");
            }
            break;
        }
        case gcsm_Delivering:
        {
            Heartbeat_Delivering_Do(); // The voltage gradient may skip
            if((StopHeartbeating = Heartbeat_Delivering_Stop()))
            {
                // Set the smallest heartbeat for the relaxing
//                HeartbeatTime_Set(m_Heartbeat_time_resolution);
                EVENT_GenComp.DeliveringEnd.notify(SC_ZERO_TIME);
                    DEBUG_SC_EVENT_LOCAL(scLocalTime_Get(),name(),"SENT 'DeliveringEnd' in 302 " << GenCompStagesString[mStageFlag] << "'");
            }
            break;
        }
        case gcsm_Relaxing:
        {
            Heartbeat_Relaxing_Do();
            if((StopHeartbeating = Heartbeat_Relaxing_Stop()))
            {
                // Set the default heartbeat for the new input
                HeartbeatTime_Set(m_Heartbeat_time_default);
                m_Relaxing_Stopped = true;
                    DEBUG_SC_EVENT_LOCAL(scLocalTime_Get(),name(),"Relaxing finished in '" << GenCompStagesString[mStageFlag]
                                                                                   << "' @" << sc_time_String_Get(sc_time_stamp()));
            }
            break;
        }
        default: ; assert(0); break; // do nothing
    }
    OutputItem(); // Deliver the result of heartbeat calculation
    if(!StopHeartbeating)
    {   // Send the heartbeat again
/*??        sc_core::sc_time NewScheduledHeartbeat = m_Heartbeat_time;
        if(m_FirstHeartbeatInStage)
            NewScheduledHeartbeat -= m_Heartbeat_time_resolution;
        m_FirstHeartbeatInStage = false;  // Mark we do NOT need correction any more
*/
            EVENT_GenComp.Heartbeat.notify(m_Heartbeat_time); // Continue heartbeating
                DEBUG_SC_EVENT_LOCAL(scLocalTime_Get(),name(),"Schd 'Heartbeat' " <<
                                    "' @" << sc_time_String_Get(sc_time_stamp()+m_Heartbeat_time)
                                    << " in stage '" << GenCompStagesString[mStageFlag]);
    }
 }

// Puts the PU in its default state
// Usually triggered by EVENT_GenComp.Initialize
// but called from the contructor, too
void scGenComp_PU_Abstract::
    Initialize_method(void)
{
            #ifdef BENCHMARK_TIME_ACTIVE
                BENCHMARK_TIME_RESET(&t_Computing,&x_Computing,&s_Computing); // Reset at the very begining, say in the constructor
                BENCHMARK_TIME_RESET(&t_Delivering,&x_Delivering,&s_Delivering); // Reset at the very begining, say in the constructor
                BENCHMARK_TIME_RESET(&t_Relaxing,&x_Relaxing,&s_Relaxing); // Reset at the very begining, say in the constructor
            #endif // BENCHMARK_TIME_ACTIVE
            #ifdef SC_BENCHMARK_TIME_ACTIVE
                SC_BENCHMARK_TIME_RESET(&SC_t_Computing,&SC_x_Computing,&SC_s_Computing); // Reset at the very begining, say in the constructor
                SC_BENCHMARK_TIME_RESET(&SC_t_Delivering,&SC_x_Delivering,&SC_s_Delivering); // Reset at the very begining, say in the constructor
                SC_BENCHMARK_TIME_RESET(&SC_t_Relaxing,&SC_x_Relaxing,&SC_s_Relaxing); // Reset at the very begining, say in the constructor
            #endif // SC_BENCHMARK_TIME_ACTIVE
                DEBUG_SC_EVENT(name(),"RCVD 'Initialize' in stage '" << GenCompStagesString[mStageFlag] << "'");
    ObserverNotify(gcob_ObserveInitialize);
    StageFlag_Set(gcsm_Relaxing);   // Pass to "Relaxing"
    mLocalTimeBase = sc_time_stamp();
    mOperationCounter = 0;    // Count the actions the unit makes
    mInputsReceived = 0;     // We did not receive any input yet
    HeartbeatTime_Set(m_Heartbeat_time_default);
//??    m_FirstHeartbeatInStage = false;
    CancelEvents();
    EVENT_GenComp.DeliveringEnd.cancel();
    EVENT_GenComp.ComputingEnd.cancel();
    Initialize_Do();
}


void scGenComp_PU_Abstract::
    InputReceived_method(void)
{
    if(gcsm_Delivering == mStageFlag)
    {   // Omit input in delivery phase
                DEBUG_SC_EVENT_LOCAL(scLocalTime_Get(),name(),"Omit 'InputReceived' in '"<< GenCompStagesString[mStageFlag] << "'");
        return;
    }
                DEBUG_SC_EVENT(name(),"RCVD 'InputReceived' #" <<mInputsReceived  << " in stage '" << GenCompStagesString[mStageFlag] << "'");
    ObserverNotify(gcob_ObserveInput);
    // Set the smallest heartbeat for the new input
    HeartbeatTime_Set(m_Heartbeat_time_resolution);
    InputReceived_Do();
    ++mInputsReceived;  // Now we have a valid input
    if(SC_ZERO_TIME == mFixedComputingTime)
    {// we are asynchronous
        if(!HaveEnoughInputs()) return;
        // Imitate 'ComputingBegin'; in bio PUs for the first 'InputReceived' only
        if(!(mInputsReceived>1))
        {
                    DEBUG_SC_EVENT(name(), "After 'InputReceived' in stage '" << GenCompStagesString[mStageFlag] << "', imitate 'ComputingBegin'");
            ComputingBegin_method();    // Imitate receiving 'ComputingBegin'
        }
    }
    else
    {   // We use fixed computing time
        if(mCentralClockMode) return;   // Some clock will trigger computing
        StageFlag_Set(gcsm_Computing);
    }
}


// Called when  'Relaxing' begins
void scGenComp_PU_Abstract::
    RelaxingBegin_method()
{
            // Benchmarking must be done here: The end of the old cycle
            #if defined(BENCHMARK_TIME_ACTIVE) || defined (SC_BENCHMARK_TIME_ACTIVE)
                std::cerr
                    << "Relaxing  Computing   Delivering  (msec), operation #" << OperationCounter_Get() << "\n";
                #ifdef BENCHMARK_TIME_ACTIVE
                    std::cerr << std::fixed << std::setprecision(10) << std::setw(3) <<
                    TimeDuration_Relaxing_Get() << " " <<
                    TimeDuration_Computing_Get() << " " <<
                    TimeDuration_Delivering_Get() << " CLOCK ms" << std::endl;
                #endif
                #ifdef SC_BENCHMARK_TIME_ACTIVE
                    std::cerr << SC_TimeDuration_Relaxing_Get() << " "
                    << SC_TimeDuration_Computing_Get() << " "
                    << SC_TimeDuration_Delivering_Get() << " SC ms" << std::endl;
                #endif
            #endif // Any of the two benchmarks
                    // Here begins the new benchmarking
            #ifdef BENCHMARK_TIME_ACTIVE
                BENCHMARK_TIME_BEGIN(&t_Relaxing,&x_Relaxing);    // Begin the benchmarking here
            #endif // BENCHMARK_TIME_ACTIVE
            #ifdef SC_BENCHMARK_TIME_ACTIVE
                SC_BENCHMARK_TIME_BEGIN(&SC_t_Relaxing,&SC_x_Relaxing);    // Begin the benchmarking here
            #endif // SC_BENCHMARK_TIME_ACTIVE
                assert( gcsm_Delivering == StageFlag_Get()); // Must be set by 'DeliveringEnd'
                DEBUG_SC_EVENT(name(),"RCVD 'RelaxingBegin' in stage " << GenCompStagesString[mStageFlag] << "'");
    mRelaxingBeginTime = sc_core::sc_time_stamp();
    StageFlag_Set(gcsm_Relaxing);                 // Passing to statio 'Relaxing'
    ObserverNotify(gcob_ObserveRelaxingBegin);
    RelaxingBegin_Do();
    EVENT_GenComp.Heartbeat.notify(SC_ZERO_TIME); // Perform an immediate calculation
}

// Synchronization request from a fellow PU
void scGenComp_PU_Abstract::
    Synchronize_method(void)
{
                DEBUG_SC_EVENT(name(),"RCVD 'Synchronize' in " << GenCompStagesString[mStageFlag] << "'");
    if(gcsm_Delivering == StageFlag_Get())
    {
        DEBUG_SC_WARNING_LOCAL(scLocalTime_Get(),name(),"Synchronization is only possible if 'Relaxing' or 'Processing'");
        return;
    }
    // We received a legal syncronization request, immediate delivering follows
    CancelEvents();// Be sure there are no pending events
    StageFlag_Set(gcsm_Computing); // Must be set for DeliveringBegin()
                DEBUG_SC_EVENT(name(),"Implicit 'DeliveringBegin' in " << GenCompStagesString[mStageFlag] << "'");
    DeliveringBegin_method(); // Implicit 'DeliveringBegin'
}

#if 0
// A backlink from the registered units to the simulator
void scGenComp_PU_Abstract::
    RegisterSimulator(scGenComp_Simulator* Observer)
{
    MySimulator = Observer;
}
#endif //0
// Handles the observed events
void scGenComp_PU_Abstract::
    ObserverNotify(GenCompPUObservingBits_t  ObservedBit)
{
#if 0
    if(!MySimulator) return; // Module not registered
    if(ObservingBit_Get(ObservedBit))
        MySimulator->Observe(this,ObservedBit);
#endif
}


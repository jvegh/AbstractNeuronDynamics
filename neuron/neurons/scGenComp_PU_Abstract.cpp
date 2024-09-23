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

    scGenComp_PU_Abstract::
scGenComp_PU_Abstract(sc_core::sc_module_name nm
                          ,sc_core::sc_time FixedComputingTime
                          ,sc_core::sc_time FixedDeliveringTime
                          ,bool CentralClockMode
                          ): sc_core::sc_module( nm)
    ,mStageFlag(gcsm_Relaxing)
    ,mLocalTimeBase(sc_core::sc_time_stamp())   /// Remember the beginning of the operation
    ,mRelaxingBeginTime(sc_core::sc_time_stamp())  /// End of the previous operation
    ,mOperationCounter(0)               /// Count performed operations
    ,mFixedComputingTime(FixedComputingTime)
    ,mFixedDeliveringTime(FixedDeliveringTime)
    ,mInputsReceived(0)
    ,mCentralClockMode(CentralClockMode)
 {
    mObservedBits[gcob_ObserveGroup] = true;   // Enable this module for its group observing by default
    mObservedBits[gcob_ObserveModule] = true;   // Enable module observing by default

    Tracing_Initialize();
    // Verify if not using variable computing time with a central clock
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
                DEBUG_SC_EVENT(name(),"Cncl 'all events' in " << GenCompStatesString[mStateFlag] << "'");
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
    ComputingEnd_method();    // Imitate receiving 'ComputingBegin'
}

// Called when the state 'processing' begins
void scGenComp_PU_Abstract::
    ComputingBegin_method()
{
            assert(gcsm_Relaxing == StageFlag_Get()); // Must be set in stage 'Relaxing'
            assert(HaveEnoughInputs()); // And must receive input before computing
            DEBUG_SC_EVENT(name(),"RCVD 'ComputingBegin' in '" << GenCompStagesString[mStateFlag] << "'");
    mIdlePeriod = sc_core::sc_time_stamp() - mRelaxingBeginTime; // Remember idle time before this processing
    mLocalTimeBase = sc_core::sc_time_stamp();  // The clock is synchronized to the beginning of processing
    m_t = scLocalTime_Get().to_seconds()*1000.;
    StageFlag_Set(gcsm_Computing);   // Pass to "Computing"
        //    Inputs.clear();
    ObserverNotify(gcob_ObserveComputingBegin);
    ComputingBegin_Do();         // Now perform the activity in the derived classes
    if(mFixedComputingTime==sc_core::sc_time(SC_ZERO_TIME))
    {   // We use variable time and heartbeat
//        EVENT_GenComp.Heartbeat.notify(HEARTBEAT_TIME_DEFAULT);
        m_CorrectForFirstHeartbeat = true;  // Mark we need correction
                EVENT_GenComp.Heartbeat.notify(HEARTBEAT_TIME_RESOLUTION);  // Use a minimum time
                DEBUG_SC_EVENT_LOCAL(scLocalTime_Get(),name(),"Schd 'Heartbeat' in '"<< GenCompStatesString[mStateFlag]
                                    << "' @" << sc_time_String_Get(sc_time_stamp()+HEARTBEAT_TIME_RESOLUTION));
        Heartbeat_Computing_Do();    // Make the calculation for 0th HB, still in 'Relaxing' mode
                DEBUG_SC_EVENT_LOCAL(scLocalTime_Get(),name(),"Calculated in '"<< GenCompStatesString[mStateFlag] << "'");
     }
    else
    {
        EVENT_GenComp.ComputingEnd.notify(mFixedComputingTime);
                DEBUG_SC_EVENT_LOCAL(scLocalTime_Get(),name(),"Schd 'ComputingEnd' in '"<< GenCompStatesString[mStateFlag] << "' @" << sc_time_String_Get(sc_time_stamp()+mFixedComputingTime));
    }
}


// Called when the state 'processing' ends
void scGenComp_PU_Abstract::
    ComputingEnd_method()
{
    m_t = scLocalTime_Get().to_seconds()*1000.;
                DEBUG_SC_EVENT_LOCAL(scLocalTime_Get(),name(),"RCVD 'ComputingEnd' in '" << GenCompStatesString[mStateFlag] << "'");
                assert(gcsm_Computing == StageFlag_Get()); // Must be set by 'ComputingEnd'
    ComputingEnd_Do();          // Now perform the activity in the derived classes
    ObserverNotify(gcob_ObserveComputingEnd);
    mResultPeriod = scLocalTime_Get();
    EVENT_GenComp.DeliveringBegin.notify(SC_ZERO_TIME);
                DEBUG_SC_EVENT_LOCAL(scLocalTime_Get(),name(),"SENT 'DeliveringBegin' in '" << GenCompStatesString[mStateFlag]<< "'");
}

// Called when the stage 'delivering' begins
void scGenComp_PU_Abstract::
    DeliveringBegin_method()
{
    m_t = scLocalTime_Get().to_seconds()*1000.;
                assert(gcsm_Computing == StageFlag_Get()); // Must be set by 'ComputingEnd'
    DEBUG_SC_EVENT_LOCAL(scLocalTime_Get(),name(),"RCVD 'DeliveringBegin in " << GenCompStagesString[mStageFlag]<< "'");
    mDeliveringBeginTime = sc_core::sc_time_stamp();
    StageFlag_Set(gcsm_Delivering);   // Pass to "Delivering"
    ObserverNotify(gcob_ObserveDeliveringBegin);
    if(mFixedComputingTime==sc_core::sc_time(SC_ZERO_TIME))
    {   // We use variable time
 //       EVENT_GenComp.Heartbeat.cancel(); // Be sure there are no foreign heartbeats
 //       DEBUG_SC_EVENT_LOCAL(scLocalTime_Get(),name(),"Cncl 'Heartbeat' in '"<< GenCompStagesString[mStageFlag] << "'");
        m_CorrectForFirstHeartbeat = true;  // Mark we need correction
        EVENT_GenComp.Heartbeat.notify(HEARTBEAT_TIME_RESOLUTION);  // Use a minimum time
        DEBUG_SC_EVENT_LOCAL(scLocalTime_Get(),name(),"Schd 'Heartbeat' in '"<< GenCompStatesString[mStateFlag]
                                                                                << "' @" << sc_time_String_Get(sc_time_stamp()+HEARTBEAT_TIME_RESOLUTION));
//        EVENT_GenComp.Heartbeat.notify(HEARTBEAT_TIME_DEFAULT);
//                DEBUG_SC_EVENT_LOCAL(scLocalTime_Get(),name(),"Schd 'Heartbeat' in '"<< GenCompStagesString[mStageFlag]
//                                     << "' @" << sc_time_String_Get(sc_time_stamp()+HEARTBEAT_TIME_DEFAULT));
        DeliveringBegin_Do();;  // Make the calculation for 0th HB
                DEBUG_SC_EVENT_LOCAL(scLocalTime_Get(),name(),"Calculated in '"<< GenCompStagesString[mStageFlag] << "'");
    }
    else
    {
        EVENT_GenComp.DeliveringEnd.notify(mFixedDeliveringTime);
        DEBUG_SC_EVENT_LOCAL(scLocalTime_Get(),name(),"Schd 'DeliveringEnd' in '"<< GenCompStatesString[mStateFlag] << "' @" << sc_time_String_Get(sc_time_stamp()+mFixedDeliveringTime));
    }
}


void scGenComp_PU_Abstract::
    DeliveringEnd_method()
{
    m_t = scLocalTime_Get().to_seconds()*1000.;
                DEBUG_SC_EVENT_LOCAL(scLocalTime_Get(),name(),"RCVD 'DeliveringEnd' in " << GenCompStatesString[mStateFlag] << "'");
                assert(StageFlag_Get() == gcsm_Delivering); // Must be set by 'DeliveringBegin'
    DeliveringEnd_Do();
    ++mOperationCounter;        // This operation finished
    ObserverNotify(gcob_ObserveDeliveringEnd);  // Inform observer
    EVENT_GenComp.RelaxingBegin.notify(SC_ZERO_TIME);
                DEBUG_SC_EVENT_LOCAL(scLocalTime_Get(),name(),"SENT 'RelaxingBegin' in " << GenCompStatesString[mStateFlag] << "'");
 }


void scGenComp_PU_Abstract::
    Failed_method(void)
{
                DEBUG_SC_EVENT(name(),"RCVD 'Failed  in " << GenCompStatesString[mStateFlag] << "'");
                DEBUG_SC_EVENT(name(),"Implicit 'Initialize'  in " << GenCompStatesString[mStateFlag] << "'");
    Initialize_method();
    Failed_Do();
}


// Called when the state 'processing' ends
void scGenComp_PU_Abstract::
    Heartbeat_method()
{
                DEBUG_SC_EVENT_LOCAL(scLocalTime_Get(),name(),"RCVD 'Heartbeat' in '"<< GenCompStatesString[mStateFlag] << "'");
    ObserverNotify(gcob_ObserveHeartbeat);
    m_t = scLocalTime_Get().to_seconds()*1000.;
    OutputItem();
    bool StopHeartbeating = false;
    switch(StageFlag_Get())
    {
        case gcsm_Computing:
        {
            Heartbeat_Computing_Do();
            if((StopHeartbeating = Heartbeat_Computing_Stop()))
            {
                EVENT_GenComp.ComputingEnd.notify(SC_ZERO_TIME);
                    DEBUG_SC_EVENT_LOCAL(scLocalTime_Get(),name(),"SENT 'ComputingEnd' in " << GenCompStatesString[mStateFlag] << "'");
            }
            break;
        }
        case gcsm_Delivering:
        {
            Heartbeat_Delivering_Do();
            if((StopHeartbeating = Heartbeat_Delivering_Stop()))
            {
                EVENT_GenComp.DeliveringEnd.notify(SC_ZERO_TIME);
                    DEBUG_SC_EVENT_LOCAL(scLocalTime_Get(),name(),"SENT 'DeliveringEnd' in " << GenCompStatesString[mStateFlag] << "'");
            }
            break;
        }
        case gcsm_Relaxing:
        {
            Heartbeat_Relaxing_Do();
            if((StopHeartbeating = Heartbeat_Relaxing_Stop()))
            {
                m_Relaxing_Stopped = true;
                    DEBUG_SC_EVENT_LOCAL(scLocalTime_Get(),name(),"Relaxing finished in '" << GenCompStatesString[mStateFlag]
                                                                                   << "' @" << sc_time_String_Get(sc_time_stamp()));
            }
            break;
        }
        default: ; assert(0); break; // do nothing
    }
    if(!StopHeartbeating)
    {   // Send the heartbeat again
        sc_core::sc_time NewScheduledHeartbeat = HEARTBEAT_TIME_DEFAULT;
        if(m_CorrectForFirstHeartbeat)
            NewScheduledHeartbeat -= HEARTBEAT_TIME_RESOLUTION;
        m_CorrectForFirstHeartbeat = false;  // Mark we do NOT need correction any more
            EVENT_GenComp.Heartbeat.notify(NewScheduledHeartbeat); // Continue heartbeating
                DEBUG_SC_EVENT_LOCAL(scLocalTime_Get(),name(),"Schd 'Heartbeat' in '"<< GenCompStatesString[mStateFlag]
                                    << "' @" << sc_time_String_Get(sc_time_stamp()+NewScheduledHeartbeat));
    }
 }


// Puts the PU in its default state
// Usually triggered by EVENT_GenComp.Initialize
// but called from the contructor, too
void scGenComp_PU_Abstract::
    Initialize_method(void)
{
                DEBUG_SC_EVENT(name(),"RCVD 'Initialize' in state '" << GenCompStatesString[mStateFlag] << "'");
    ObserverNotify(gcob_ObserveInitialize);
    StageFlag_Set(gcsm_Relaxing);   // Pass to "Relaxing"
    mLocalTimeBase = sc_time_stamp();
    mOperationCounter = 0;    // Count the actions the unit makes
    mInputsReceived = 0;     // We did not receive any input yet
    m_CorrectForFirstHeartbeat = false;
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
                DEBUG_SC_EVENT_LOCAL(scLocalTime_Get(),name(),"Omit 'InputReceived' in '"<< GenCompStatesString[mStateFlag] << "'");
        return;
    }
                DEBUG_SC_EVENT(name(),"RCVD 'InputReceived' in '" << GenCompStatesString[mStateFlag] << "'");
    ObserverNotify(gcob_ObserveInput);
    InputReceived_Do();
    ++mInputsReceived;  // Now we have a valid input
    if(SC_ZERO_TIME == mFixedComputingTime)
    {// we are asynchronous
        if(!HaveEnoughInputs()) return;
        // Imitate 'ComputingBegin'; in bio PUs for the first 'InputReceived' only
        if(!(mInputsReceived>1))
        {
                    DEBUG_SC_EVENT(name(), "After 'InputReceived' in '" << GenCompStatesString[mStateFlag] << "', imitate 'ComputingBegin'");
            ComputingBegin_method();    // Imitate receiving 'ComputingBegin'
        }
    }
    else
    {   // We use fixed computing time
        if(mCentralClockMode) return;   // Some clock will trigger computing
    }
}


// Called when the state 'Relaxing' begins
void scGenComp_PU_Abstract::
    RelaxingBegin_method()
{
                assert( gcsm_Delivering == StageFlag_Get()); // Must be set by 'DeliveringEnd'
                DEBUG_SC_EVENT(name(),"RCVD 'RelaxingBegin' in " << GenCompStatesString[mStateFlag] << "'");
    mRelaxingBeginTime = sc_core::sc_time_stamp();
    StageFlag_Set(gcsm_Relaxing);                 // Passing to statio 'Relaxing'
    ObserverNotify(gcob_ObserveRelaxingBegin);
    RelaxingBegin_Do();
//    EVENT_GenComp.Heartbeat.cancel(); // Be sure there are no foreign heartbeats
//                DEBUG_SC_EVENT_LOCAL(scLocalTime_Get(),name(),"Cncl 'Heartbeat' in '"<< GenCompStatesString[mStateFlag] << "'");
    EVENT_GenComp.Heartbeat.notify(HEARTBEAT_TIME_DEFAULT);
                DEBUG_SC_EVENT_LOCAL(scLocalTime_Get(),name(),"Schd 'Heartbeat' in '"<< GenCompStatesString[mStateFlag]
                                                                                    << "' @" << sc_time_String_Get(sc_time_stamp()+HEARTBEAT_TIME_DEFAULT));
    Heartbeat_Relaxing_Do(); // Make the calculation for 0th HB
                DEBUG_SC_EVENT_LOCAL(scLocalTime_Get(),name(),"Calculated in '"<< GenCompStatesString[mStateFlag] << "'");
}

// Synchronization request from a fellow PU
void scGenComp_PU_Abstract::
    Synchronize_method(void)
{
                DEBUG_SC_EVENT(name(),"RCVD 'Synchronize' in " << GenCompStagesString[mStateFlag] << "'");
    if(gcsm_Delivering == StageFlag_Get())
    {
        DEBUG_SC_WARNING_LOCAL(scLocalTime_Get(),name(),"Synchronization is only possible if 'Relaxing' or 'Processing'");
        return;
    }
    // We received a legal syncronization request, immediate delivering follows
    CancelEvents();// Be sure there are no pending events
    StageFlag_Set(gcsm_Computing); // Must be set for DeliveringBegin()
                DEBUG_SC_EVENT(name(),"Implicit 'DeliveringBegin' in " << GenCompStagesString[mStateFlag] << "'");
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


/** @file scGenComp_PU_Tech.cpp
 *  @ingroup GENCOMP_MODULE_PROCESS
 *  @brief  The abstract processing unit for generalized computing
 */
/*
 *  @author János Végh (jvegh)
 *  @bug No known bugs.
*/

#include "scGenComp_PU_Tech.h"
// This section configures debug and log printing
//#define SUPPRESS_LOGGING // Suppress all log messages
#define DEBUG_EVENTS    ///< Print event debug messages  for this module
#define DEBUG_PRINTS    ///< Print general debug messages for this module
// Those defines must be located before 'DebugMacros.h", and are undefined in that file

#include "DebugMacros.h"

#define scLocalTimeLimit  50

// The units of general computing work in the same way, using general events
// \brief Implement handling the states of computing

//GenCompStates_Tech* TheGenCompStates_Tech;
    scGenComp_PU_Tech::
scGenComp_PU_Tech(sc_core::sc_module_name nm
                      ,sc_core::sc_time FixedComputingTime
                      ,sc_core::sc_time FixedDeliveringTime
                      ,bool ExternSynchronMode
                      ):
    scGenComp_PU_Abstract(nm, FixedComputingTime, FixedDeliveringTime,ExternSynchronMode)
//     ,mNoOfArgs(No)
{
//    typedef scGenComp_PU_Tech SC_CURRENT_USER_MODULE; // Needed if some routines are listed in this constructor
    // ** Do not reimplement any of the xxx_method functions
    // until you know what you are doing
}

scGenComp_PU_Tech::
    ~scGenComp_PU_Tech(void)
{
}

/*
 *   *
 * To initiate a single-shot operation while in 'Relaxing' state, different methods are used by the derived classes.
 * - PU_Bio needs a single input spike to begin, @see scGenComp_PU_Bio
 * - PU_Tech needs an external clock signal, @see scGenComp_PU_Tech
 */

// Puts the PU in its default state
void scGenComp_PU_Tech::
    Initialize_Do(void)
{
//                DEBUG_SC_EVENT_LOCAL(">>>   Intializing");
//    scGenComp_PU_Abstract::Initialize_method();
//        MachineState->Initialize(this);   // Change status to 'Ready'
    Initialize_Do(); // Initialize the unit, HW and temporary variables
}



// Heartbeat distribution happens in scGenComp_PU_Abstract
    /**
     * Handle heartbeats in 'Delivering' mode
     */
void scGenComp_PU_Tech::
    Heartbeat_Delivering_Do()
{
    if (scLocalTime_Get() < sc_time(8*m_Heartbeat_time_default))
    {   // We are still processing; re-issue the heartbeat
        // if the limit is not yet reached
        EVENT_GenComp.Heartbeat.notify(m_Heartbeat_time_default);
//        DEBUG_SC_EVENT_LOCAL("SENT    EVENT_GenComp.HeartBeat with BIO_HEARTBEAT_TIME");
    }
    else
    {   // We are about finishing processing
        EVENT_GenComp.ComputingEnd.notify(SC_ZERO_TIME);
//        DEBUG_SC_EVENT_LOCAL("SENT    EVENT_GenComp.DeliveringEnd");
    }
}

    /**
     * Handle heartbeats in 'Processing' mode
     */
void scGenComp_PU_Tech::
    Heartbeat_Processing_Do()
{
    if (Processing_Finished())
    {   // We are about finishing processing
        EVENT_GenComp.ComputingEnd.notify(SC_ZERO_TIME);
//                    DEBUG_SC_EVENT_LOCAL("SENT    EVENT_GenComp.ProcessingEnd");
     }
    else
    {   // We are still processing; re-issue the heartbeat
           // if the limit is not yet reached
        EVENT_GenComp.Heartbeat.notify(m_Heartbeat_time_default);
 //                   DEBUG_SC_EVENT_LOCAL("SENT    EVENT_GenComp.HeartBeat with mHeartbeat");
    }
}

    /**
     * Handle heartbeats in 'Ready' mode
     */
void scGenComp_PU_Tech::
    Heartbeat_Ready_Do()
{
    if (scLocalTime_Get() < 2*m_Heartbeat_time_default)
    {   // We are still processing; re-issue the heartbeat
        // if the limit is not yet reached
        EVENT_GenComp.Heartbeat.notify(m_Heartbeat_time_default);
                //DEBUG_SC_EVENT_LOCAL("SENT    EVENT_GenComp.HeartBeat with BIO_HEARTBEAT_TIME");
    }
    else
    {   // We are about finishing processing
        EVENT_GenComp.ComputingEnd.notify(SC_ZERO_TIME);
    }
}

    /**
     *
     * Handle heartbeats in 'Relaxing' mode
     */
void scGenComp_PU_Tech::
    Heartbeat_Relaxing_Do()
{
    if (scLocalTime_Get() < 8*m_Heartbeat_time_default)
    {   // We are still processing; re-issue the heartbeat
        // if the limit is not yet reached
        EVENT_GenComp.Heartbeat.notify(m_Heartbeat_time_default);
//                DEBUG_SC_EVENT_LOCAL("SENT    EVENT_GenComp.HeartBeat with BIO_HEARTBEAT_TIME");
    }
    else
    {   // We are about finishing processing
        EVENT_GenComp.RelaxingBegin.notify(SC_ZERO_TIME);
//                DEBUG_SC_EVENT_LOCAL("SENT    EVENT_GenComp.RelaxingEnd");
    }
}


/*
 * This routine makes actual input processing, although most of the job is done in Process() and Heartbeat()
 * It can be called in state 'Processing' (if not first input)
 * or in state 'Ready' if first input
 *
 * If it was the first spike, issue 'ComputingBegin' and re-issue

 */
void scGenComp_PU_Tech::
   InputReceived_Do(void)
{
    if(!(gcsm_Relaxing == mStageFlag)) return;
    // inputs are processed only in 'Relaxing' stage
                DEBUG_SC_EVENT_LOCAL(scLocalTime_Get(),name(),"RCVD EVENT_GenComp.InputReceived in mode '" << GenCompStagesString[mStageFlag] << "'");
//    scGenComp_PU_Abstract::InputReceived_Do();
    ObserverNotify(gcob_ObserveInput);
    // Now we are surely in 'Computing'
    if(1)// Need to check no of inputs
    {   // we are still in 'Relaxing' stage
        EVENT_GenComp.ComputingBegin.notify(SC_ZERO_TIME);
//        ComputingBegin_method();    // Imitate receiving 'ComputingBegin'
        assert(gcsm_Computing == mStageFlag);
    }
 }

/*
 * This virtual method makes ProcessingBegin activity
 * */
void scGenComp_PU_Tech::
    ProcessingBegin_Do()
{
    scGenComp_PU_Abstract::ComputingBegin_Do();  // Make default processing
    // Technically, just make sure that Heartbeat comes last:
    // 1st: ProcessingBegin
    // 2nd: Input
    // 3rd: Heartbeat
    EVENT_GenComp.Heartbeat.notify(2,SC_PS);
//                DEBUG_SC_EVENT_LOCAL("SENT 'EVENT_GenComp.HeartBeat' with 2 ps delay");
}

/*
void scGenComp_PU_Tech::
    ProcessingEnd_Do()
{
    scGenComp_PU_Abstract::ProcessingEnd_Do();
}*/

bool scGenComp_PU_Tech::
    Processing_Finished(void)
{   // Just to provide a sample
    return scLocalTime_Get() >= m_Heartbeat_time_default;
}

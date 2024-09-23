/** @file scGenComp_PU_Bio.cpp
 *  @ingroup GENCOMP_MODULE_PROCESS
 *  @brief  The abstract processing unit for generalized computing
 */
/*
 *  @author János Végh (jvegh)
 *  @bug No known bugs.
 */

#include "scGenComp_PU_Bio.h"
// This section configures debug and log printing; must be located AFTER the other includes
//#define SUPPRESS_LOGGING // Suppress all log messages
#define DEBUG_EVENTS    ///< Print event debug messages  for this module
#define DEBUG_PRINTS    ///< Print general debug messages for this module
// Those defines must be located before 'DebugMacros.h", and are undefined in that file
#include "DebugMacros.h"

#define PU_ComputingPeriod sc_core::sc_time(40,SC_US)
#define PU_DeliveringPeriod sc_core::sc_time(60,SC_US)

// The units of general computing work in the same way, using general events
// \brief Implement handling the states of computing
    scGenComp_PU_Bio::
scGenComp_PU_Bio(sc_core::sc_module_name nm
                     ):  // Heartbeat time
    scGenComp_PU_Abstract(nm
                            ) // Defaults to variable execution time, no central clock
{
        m_Membrane_V = 0;   // Working with relative potentials
}

void scGenComp_PU_Bio::
    DeliveringBegin_Do()
{
    scGenComp_PU_Abstract::DeliveringBegin_Do();
    m_SynapsesEnabled = false;
}
void scGenComp_PU_Bio::
    DeliveringEnd_Do()
{
    scGenComp_PU_Abstract::DeliveringEnd_Do();
    m_PeakReached = true; // If the AP overpassed the depolarized state
    m_SynapsesEnabled = true; //
}
void scGenComp_PU_Bio::
    Heartbeat_Relaxing_Do()
{
}

void scGenComp_PU_Bio::
    Heartbeat_Computing_Do()
{
}

void scGenComp_PU_Bio::
    Heartbeat_Delivering_Do()
{
}

/*
 * Initialize the GenComp unit.
 */
void scGenComp_PU_Bio::
    Initialize_Do(void)
{
                DEBUG_SC_EVENT(name(),"Initialized for BIO mode");
    // Reset synaptic input handling
    m_PeakReached = true; // If the AP overpassed the depolarized state
    m_SynapsesEnabled = true; //
}

/*
 * This routine makes actual input processing, although most of the job is done in Process() and Heartbeat()
 * It can be called in state 'Processing' (if not first input),
 * in state 'Ready' if first input,
 * or in state GenCompStateMachineType_t::gcsm_XXXRelaxing in the relative refractory period
 *
 * If it was the first spike, issue 'ComputingBegin' and re-issue
*/

void scGenComp_PU_Bio::
   InputReceived_Do(void)
{
//?    scGenComp_PU_Abstract::InputReceived_Do();
 }

void scGenComp_PU_Bio::
     OutputItem()
{
    cerr << "(" << sc_time_String_Get(sc_time_stamp()) << "," << MembraneRelativePotential_Get() << ")\n";
}









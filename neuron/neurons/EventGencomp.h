#ifndef EVENTGENCOMP_H
#define EVENTGENCOMP_H
#include <systemc>

/**
 *  @class EVENT_GenComp_type
 *  @brief The events used by scGenComp_PU_Abstract classes
 *
 *  The principle is that a SystemC event XXX triggers the XXX_method()
 *  handling method, which makes the necessary actions. The XXX_method()
 *  is a *non-virtual* private function, which is an envelope for the SystemC activity.
 *  It calls a *virtual* method XXX_Do(), which makes the extra activity
 *  needed by the downstream objects (by default, it makes nothing)
 *  and can be overloaded by users of the downstream classes.
 *  That is, the derived objects do not need
 *  to consider SystemC programming, except the format prescriptions.
 *  Actually, the above main duties govern processing of the abstract computing units.
 *  The operating principle of event processing:
 *
 * If the old and the new states are not compatible, that routine asserts. (wrong event)
 * Following a normal return, the HW specific operation of the \gls{PU} is executed.
 * The next state is invoked by issuing a EVENT_GenComp.XXX.notify() notification.
 * The length of operations is simulated by issuing a wait() call to SystemC kernel.
 * (technically, 'next_trigger()' is used, to mitigate system's non-payload activity.
 * In this way, only one event per active \gls{PU} loads the SystemC's scheduler.)
 *
 * There are continuous actions; they begin their activity in a XXXBegin_method
 * and terminate it in a XXXEnd_method. The event XXXBegin is issued by either another state's XXXEnd_method
 * or by the simulator scGenComp_Simulator. The XXXEnd events are generated during processing of the state.
 * The corresponding events are issued in form of a EVENT_GenComp.XXX.notify(SC_ZERO_TIME).
 * (the argument SC_ZERO_TIME means: immediately after activities at this time slot terminated).
 * The non-virtual methods XXXBegin_method and  XXXEnd_method should not be overridden
 * (because of the event handling method of SystemC); they use the virtual methods XXXBegin_Do and XXXEnd_Do.
 * Those latter methods can call the corresponding method of their superclass or replace them.
 *
 * The description is valid for state-implementing methods
 * - Compute
 * - Deliver
 * - Relax
 *
 * There are also one-time action similar methods where there are no XXXBegin_method
 * and XXXEnd_method; instead they have non-virtual XXX_method and similarly they call
 * the virtual XXX_Do() method.
 *
 * The description is valid for single-shot action-implementing methods
 * - Synchronize
 * - Fail
*/
/* Implement later
 *  @var EVENT_GenComp_type::TransmissionBegin
        *  Begin transmitting the result (externally); unattended
    *  @var EVENT_GenComp_type::TransmissionEnd
        *  Feedback from transmission unit; unattended
 */
class EVENT_GenComp_type {
public:
    sc_core::sc_event
        // Operation-related
        ClockDown,              // Temporary, to test clocked
        ClockUp,                // operating mode
        ComputingBegin,         // Begin data processing
        ComputingEnd,           // End data processing
        DeliveringBegin,        // Begin delivering the result (internally)
        DeliveringEnd,          // End delivering
        Failed,                 // Computing failed, start over
        Heartbeat,              // Refresh PU's state
        Initialize,             // Put the unit to its ground state
        InputReceived,          // New input received
        RelaxingBegin,          // Begin restoring the 'Ready' state
        Synchronize,            // External synhronize signal
        TransmissionBegin;      // Begin data transmission
};


/*! \var EVENT_GenComp_type::ClockDown
 * The external clock issued a "down" edge. Triggers scGenComp_PU_Abstract::scClockDown_method()
 */
/*! \var EVENT_GenComp_type::ClockUp
 * The external clock issued an "up" edge. Triggers scGenComp_PU_Abstract::scClockUp_method()
 */
/*! \var EVENT_GenComp_type::ComputingBegin
 *
 * Triggers scGenComp_PU_Abstract::ComputingBegin_method()
 */
/*! \var EVENT_GenComp_type::ComputingEnd
 *
 * Triggers scGenComp_PU_Abstract::ComputingEnd_method()
 */
/*! \var EVENT_GenComp_type::DeliveringBegin
 *
 * Triggers scGenComp_PU_Abstract::DeliveringBegin_method()
 */
/*! \var EVENT_GenComp_type::DeliveringEnd
 *
 * Triggers scGenComp_PU_Abstract::DeliveringEnd_method()
 */
/*! \var EVENT_GenComp_type::Failed
 *
 * Triggers scGenComp_PU_Abstract::Failed_method()
 */
/*! \var EVENT_GenComp_type::Heartbeat
 *
 * Triggers scGenComp_PU_Abstract::Heartbeat_method()
 */
/*! \var EVENT_GenComp_type::Initialize
 *
 * Triggers scGenComp_PU_Abstract::Initialize_method()
 */
/*! \var EVENT_GenComp_type::InputReceived
 *
 * Triggers scGenComp_PU_Abstract::InputReceived_method()
 */
/*! \var EVENT_GenComp_type::RelaxingBegin
 *
 * Triggers scGenComp_PU_Abstract::RelaxingBegin_method()
 */
/*! \var EVENT_GenComp_type::Synchronize
 *
 * Triggers scGenComp_PU_Abstract::Synchronize_method()
 */
/*! \var EVENT_GenComp_type::TransmissionBegin
 *
 * Triggers scGenComp_PU_Abstract::TransmissionBegin_method()
 */
#endif // EVENTGENCOMP_H

/** @file scGenComp_PU_Bio.h
 *  @ingroup GENCOMP_MODULE_BIOLOGY

 *  @brief Function prototypes for the bio computing module
 *  It is just event handling, no modules
 */
/*
 *  @author János Végh (jvegh)
 *  @bug No known bugs.
 */


#ifndef SCBIOGENCOMP_H
#define SCBIOGENCOMP_H
/* * @addtogroup GENCOMP_MODULE_PROCESS
 *  @{
 */

#include "scGenComp_PU_Abstract.h"


#ifndef NEURONDEMO_H

/*!
 * \class scGenComp_PU_Bio
 * \brief  Implements a general biological-type computing PU. Defaults to variable execution time, no central clock.
 *
 * In the corresponding states, the neuron is in one of its GenCompStateMachineType_t states
 *  - gcsm_Computing: computing (collects charge)
 *  - gcsm_Delivering: delivers action potential
 *  - gcsm_Relaxing: restores resting potential
 *
 *  The Heartbeat_method() handles an internal 'update' signal. In the corresponding states, the neuron is
 *  - gcsm_Relaxing: Until it reaches its resting potential, calls CalculateMembranePotential(). Upon receiving an input,
 *    changes to state gcsm_Computing. Inputs enabled: the relative refractory period.
 *  - gcsm_Computing: Until it reaches its threshold potential, calls CalculateMembranePotential().
 *    At that point, changes to state  to state gcsm_Delivering. Inputs enabled.
 *  - gcsm_Delivering: Until it reaches its lowest membrane potential
 *    (its increase turns from negative to positive), calls CalculateMembranePotential().
 *    At that point, changes to state  to state gcsm_Relaxing, without restoring membrane's potential.
 *    At the beginning issues signal 'Begin Transmitting'. Signal transmission happens
 *    in parallel with the further processing. Inputs disabled; the absolute refractory period.
 *
 *  The InputReceived_method(): the unit received new input (a momentary state).
 *  Only administrative action; the received input is handled in Heartbeat_method(),
 *  as described in CalculateMembranePotential(). Disabled in mode gcsm_Delivering.
 *
 * --These states below are momentary states: need action and passes to one of the above states
 * - Delivering: The unit is delivering its result to its output section
 *   - After some time, it Sends 'Begin Transmitting' @see scGenComp_PU_Abstract
 *     (Activates transmission unit to send computed result to its chained unit(s),
 *      then goes to XXXRelaxing
 * - Synchronizing: deliver result, anyhow ;  (a momentary state)
 *   - in biological mode, deliver immediate spike
 *   Passes to XXXRelaxing (after issuing 'End Computing')
 *
 *
\anchor NeuronStateMachine
@image html NeuronStateMachine.png "The neuronal state machine" width=500px

@latexonly
\begin{figure}
\includegraphics[width=.4\textwidth]{images/NeuronStateMachine.pdf}
\caption{The neuronal state machine}
\label{NeuronStateMachine}
\end{figure}
@endlatexonly
 */
#endif // NEURONDEMO_H

class scGenComp_PU_Bio : public scGenComp_PU_Abstract
{
  public:
    /*!
     * \brief Creates a biological general computing unit
     * @param[in] nm the SystemC name of the module
     *
     * Creates an abstract biological computing unit
     */
    scGenComp_PU_Bio(sc_core::sc_module_name nm   // Module name
                     );
      virtual ~scGenComp_PU_Bio(void){} // Must be overridden

    /** @brief The extra processing for beginning computing
     */
    virtual void ComputingBegin_Do()
    {
    }
    void DeliveringBegin_Do();
    void DeliveringEnd_Do();

    /**
     * @brief A new spike (or clamping) received; only in 'Relaxing' and 'Computing' states,
     * furthermore in "XXXRelaxing" state during the "relative refractory" period
     *
     * A spike arrived, store spike parameters. Receving an input is a momentary action, just administer its processing.
     * Most of the job is done in methods Heartbeat_Relaxing() and Heartbeat_Computing().
     * If it was the first spike, issue 'Begin_Computing'
     *
     * Called by scGenComp_PU_Abstract::InputReceived_method
     * Reimplemented given that in biology the first input also starts computing
     */
    virtual void InputReceived_Do();

    /**
     * @brief Heartbeat_method
     *
     * A periodic  signal as a timebase for solving differential equations.
     *
     * - The unit receives a signal EVENT_GenComp.HeartBeat and handles it
     *   differently in different modes
     * - In 'Computing' mode, re-calculates membrane's charge-up potential
     * - In 'Relaxing' mode, re-calculates membrane's decay potential
     * - In 'Delivering' mode, re-calculates membrane's decay potential
     */
    /**
     * @brief Adjust integration step size, to keep accuracy and computing time tolerable
     */
    virtual void Heartbeat_Adjust(void);
    //! Heartbeat activity for relaxing
    virtual void Heartbeat_Relaxing_Do();
    //! Heartbeat activity for computing
    virtual void Heartbeat_Computing_Do();
    //! Heartbeat activity for delivering
    virtual void Heartbeat_Delivering_Do();

    float MembraneAbsolutePotential_Get(){return m_Membrane_V+RestingMembranePotential;}
    float MembraneRelativePotential_Get(){return m_Membrane_V;}
  protected:
    //! Puts the PU to its default state
    virtual void Initialize_Do();
      void OutputItem();

 protected:
    float m_Membrane_V; //< The actual value of membrane potential, in mV
    float m_Membrane_dV;   //< The actual change in membrane potential, in mV
    bool m_PeakReached; //< If the AP overpassed the depolarized state
    bool m_SynapsesEnabled; //<
  };// of class scGenComp_PU_Bio
/* * @}*/

#endif // SCBIOGENCOMP_H

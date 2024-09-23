/** @file NeuronDemo.h
 *  @brief Function prototypes for the DEMO neuron PU  computing module
 */
/*
 *  @author János Végh (jvegh)
 *  @bug No known bugs.
*/


#ifndef NEURONDEMO_H
#define NEURONDEMO_H
/* * @addtogroup GENCOMP_MODULE_PROCESS
 *  @{
 */

#include "scGenComp_PU_Bio.h"

#define MembraneInputIncrease 6
#define MembraneRelaxDischarge 1
/*!
 * \class NeuronDemo
 * \brief  A simple demo class to implement the operation of an abstract biological neuron.
 * The operating principle of neuronal processing, and programming details:
 *
 * The class adjusts member scGenComp_PU_Bio::m_Membrane_V (the membrane potential).
 * Actually, it implements the \gls{AP} creation as shown in Figure @ref fig_AP_Conceptual.
 *
 * Given that the membrane's potential rather than time of instruction count
 * defines processing phases, method scGenComp_PU_Abstract::Heartbeat_method()
 * is called periodically, with HEARTBEAT_TIME_DEFAULT (simulated) time
 * (presently set to \f$10\ \mu s\f$).
 * That function internally calls one of its state-dependent Heartbeat_XXX_Do() methods.
 *
 * InputReceived_Do() imitates chargeup: membranes voltage increases promptly by \f$6\ mV\f$ after receiving a synaptic input

 * Heartbeat_Computing_Do() imitates membrane's discharge by decreasing its voltage by \f$1\ mV\f$
 *
 * Heartbeat_Delivering_Do() imitates creating a sawtooth-like \gls{AP} on the membrane
 *
 * Heartbeat_Relaxing_Do() imitates restoring potential to RestingMembranePotential by changing its value by  by \f$\pm 1\ mV\f$
 *
 * The states of the neuronal state machine are controlled by methods Heartbeat_XXX_Stop()
 *
 * Heartbeat_Computing_Stop() passes to phase 'Delivering' if membrane's voltage
 * exceeds the ThresholdPotential or passes to phase 'Relaxing'
 * if it falls below RestingMembranePotential (calls scGenComp_PU_Abstract#Failed_method() )
 *
 * Heartbeat_Delivering_Stop() passes to phase "Relaxing", practically at a constant time.
 * It increases membrane's voltage until \f$\approx 150\ mV\f$ reached,
 * then decreases in somewhat lower pace until ThresholdPotential reached
 * (imitate hyperpolarization)
 *
 * Heartbeat_Relaxing_Stop() decreases membrane potential until
 * -ThresholdPotential reached, then restores RestingMembranePotential reached
 *
 *
 * The class needs (empty) constructor and destructor.
 * Initialize_Do() is only called in constructor (preserves voltage between spikes)
 *

\anchor fig_AP_ConceptualDemo
@image html images/AP_Conceptual.png "The conceptual schematic Action Potential produced by NeuronDemo" width=600px


The conceptual schematics is implemented by the Neuron demo:
@latexonly
\begin{figure}
\includegraphics[width=.4\textwidth]{images/AP_ConceptualDemo.pdf}
\caption{The conceptual schematic Action Potential produced by NeuronDemo}
\label{fig_AP_ConceptualDemo}
\end{figure}
@endlatexonly

 */

class NeuronDemo : public scGenComp_PU_Bio
{
public:
    /*!
     * \brief Creates a demo-purpose neuron unit
     * @param nm the SystemC name of the module
     */
    NeuronDemo(sc_core::sc_module_name nm ):
        scGenComp_PU_Bio(nm){}

    virtual ~NeuronDemo(void)    {}// Must be overridden

    //! Heartbeat processing in 'Computing'
    virtual void Heartbeat_Computing_Do();
    //! Heartbeat processing in 'Delivering'
    virtual void Heartbeat_Delivering_Do();
    //! Heartbeat processing in 'Relaxing'
    virtual void Heartbeat_Relaxing_Do();

    //! Activity for intializing the unit
    virtual void Initialize_Do();

    //! Activity to imitate an input
    virtual void InputReceived_Do();
    //!  @return true if threshold exceeded
    virtual bool Heartbeat_Computing_Stop();
    //! @return true if delivering potential is below threshold
    virtual bool Heartbeat_Delivering_Stop();
    //! @return true if to stop heartbeats if resting potential sufficiently approached
    virtual bool Heartbeat_Relaxing_Stop();
    //! Write out one item to the screen
private:
    bool m_PeakReached;
 //   virtual void OutputItem(void);

};// of class NeuronDemo
/* * @}*/
#endif // NEURONDEMO_H

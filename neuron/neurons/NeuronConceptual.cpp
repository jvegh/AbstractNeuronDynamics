
#define SC_MAKE_TIME_BENCHMARKING  // uncomment to measure the time with benchmarking macros
#define MAKE_TIME_BENCHMARKING  // uncomment to measure the time with benchmarking macros


#include "NeuronConceptual.h"
//#include "Utils.h"

void NeuronConceptual::
    Initialize_Do()
{
    scGenComp_PU_Bio::Initialize_Do();   // Do also inherited initialization, if needed
    m_Membrane_V = 0;
}

// Imitate potential increase on the membrane, due to an input
void NeuronConceptual::
    InputReceived_Do()
{
    scGenComp_PU_Bio::InputReceived_Do();    // Do also inherited processing
    m_Membrane_V += MembraneInputIncrease; // Just to imitate potential increase
    OutputItem();
}

// Handle neuronal membrane's potential in 'Computing' mode
void NeuronConceptual::
    Heartbeat_Computing_Do()
{
    m_Membrane_V -= 1; // Imitate discharging
}

// Handle neuronal membrane's potential in 'Delivering' mode:
// Produce a sawtooth-like artificial charge-up curve, peaking at +150+ThresholdPotential mV @ +500 ms
void NeuronConceptual::
    Heartbeat_Delivering_Do()
{
    float MyTime = HeartbeatTimeInMicrosec_Get();
     if(LocalTimeInMicrosec_Get()<250)
    {// the voltage is still rising
        m_Membrane_V += 0.8*MyTime;
    }
    else
    {// The voltage is falling now
        m_Membrane_V -= 0.2*MyTime;
        m_PeakReached = true;
    }
}

// Handle heartbeats in 'Relaxing' mode: converge to the resting potential
void NeuronConceptual::
    Heartbeat_Relaxing_Do()
{
    if(m_PeakReached)
    {
        float MyTime = HeartbeatTimeInMicrosec_Get();
        m_Membrane_V -= 0.2*MyTime;
        m_PeakReached = (m_Membrane_V > -ThresholdPotential);
        return;
    }
    if(m_Membrane_V>0)
        m_Membrane_V -= AllowedRestingPotentialDifference/2;
    else
        m_Membrane_V += AllowedRestingPotentialDifference/2;
}

// return true if to stop heartbeating in 'Computing' mode
bool NeuronConceptual::
    Heartbeat_Computing_Stop()
{   if(m_Membrane_V < 0)
    {   // Charging failed
        ChargeupFailed();
        return true;
    }
    else //No obvious problem, continue computing
        return m_Membrane_V > ThresholdPotential;
}

// return true if to stop heartbeating in 'Delivering' mode
bool NeuronConceptual::
    Heartbeat_Delivering_Stop()
{
    if(m_Membrane_V >= ThresholdPotential)
    return false; // Continue delivering
    // Stop delivering, but imitate superpolarization
    m_PeakReached = true;
    return true;
 }

// return true if to stop heartbeating in 'Relaxing' mode
bool NeuronConceptual::
    Heartbeat_Relaxing_Stop()
{
    if(m_PeakReached) return false;
    if ((abs(m_Membrane_V )>= AllowedRestingPotentialDifference))
        return false; // Continue relaxing
    // Make sure the resting potential really zero
    m_Membrane_V = 0;
    return true;
}

/*
// Output an item (simulated time and membrane voltage) to the screen
void NeuronDemo::
    OutputItem(void)
{
    cerr << "(" << sc_time_String_Get(sc_time_stamp()) << "," << MembranePotential_Get() << ")\n";
}
*/

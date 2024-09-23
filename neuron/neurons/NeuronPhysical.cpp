/** @file NeuronPhysical.cpp
 *  @ingroup GENCOMP_MODULE_PROCESS
 *  @brief  The phsics-based abstract neuron
 */
/*
 *  @author János Végh (jvegh)
 *  @bug No known bugs.
 */

#include "NeuronPhysical.h"

// Define parameters for calculating membrane voltage's time derivative
#define Membrane_Amplitude 350.
#define Rushin_A 4
#define Rushin_B 0.2
// Define parameters for calculating axon's voltage's time derivative
#define Axon_Amplitude 40.
#define Axon_A 6.3
#define Axon_B 0.2
// RC parameters of the membrane
#define Membrane_tau .22
// Saved value = 2
#define Membrane_R  .20
#define Membrane_C tau/ m_R_membrane


// This section configures debug and log printing; must be located AFTER the other includes
//#define SUPPRESS_LOGGING // Suppress all log messages
#define DEBUG_EVENTS    ///< Print event debug messages  for this module
#define DEBUG_PRINTS    ///< Print general debug messages for this module
// Those defines must be located before 'DebugMacros.h", and are undefined in that file
#include "DebugMacros.h"

int OutputCounter= 0; // Just to help output, temporary
// The units of general computing work in the same way, using general events
// \brief Implement handling the states of computing
    NeuronPhysical::
NeuronPhysical(sc_core::sc_module_name nm ):
    scGenComp_PU_Bio(nm)
{
        m_tau = Membrane_tau;
        // Saved vaulue = 2
        m_R_membrane =Membrane_R; // Resistivity of the membrane
        m_C_membrane = m_tau/ m_R_membrane; // capacity, arbitrary
        Tracing_Initialize();
}

void NeuronPhysical::
    Tracing_Initialize()
{
    //scGenComp_PU_Abstract::Tracing_Initialize();
    sc_trace(m_tracefile,m_t,"LocalTime");
    sc_trace(m_tracefile,m_MembraneRushin_V,"Rushin_V");
    sc_trace(m_tracefile,m_Membrane_V,"V_out");
    sc_trace(m_tracefile,m_MembraneResulting_dVdt,"Resulting_dV/dt");
    sc_trace(m_tracefile,m_MembraneRushin_dVdt,"Rushin_dV/dt");
    sc_trace(m_tracefile,m_MembraneAIS_dVdt,"AIS_dV/dt");
//    sc_trace(m_tracefile,mStageFlag,"Stage");
    sc_trace(m_tracefile,mStateFlag,"State");
//    sc_trace(m_tracefile,mOperationCounter,"Operations");
    sc_trace(m_tracefile,m_SynapsesEnabled,"Synapses");
}


void NeuronPhysical::
    Initialize_Do()
{
    scGenComp_PU_Bio::Initialize_Do();   // Do also inherited initialization
            DEBUG_SC_EVENT(name(),"Initialized for NeuronPhysical");
    m_Membrane_V = 0;
    m_dt = HEARTBEAT_TIME_DEFAULT.to_seconds()*1000.;
    m_HasUnhandledInput = false;
    m_SynapsesEnabled = true;
}

// Imitate potential increase on the membrane, due to an input
void NeuronPhysical::
    InputReceived_Do()
{
    scGenComp_PU_Bio::InputReceived_Do();    // Do also inherited processing
             DEBUG_SC_EVENT_LOCAL(scLocalTime_Get(),name(),"RCVD 'InputReceived' in '" << GenCompStagesString[mStageFlag] << "'");
    m_HasUnhandledInput = true;
}


float NeuronPhysical::
    MembraneRushinTimeDerivative_Get()
{
    double DeliveryTime = m_t-mDeliveringBeginTime.to_seconds()*1000.;
    if(DeliveryTime<0) return 0.;
    double dVdt = Membrane_Amplitude*(
                      Rushin_A*exp(-Rushin_A*DeliveryTime-Rushin_B*DeliveryTime)
                      -Rushin_B*exp(-Rushin_B*DeliveryTime)*exp(1-exp(-Rushin_A*DeliveryTime))
                      );
    m_MembraneRushin_dVdt = dVdt; // For tracing
    return dVdt;
}

float NeuronPhysical::
    MembraneRushinVoltage_Get()
{
//    double LocalTime = m_t-mDeliveringBeginTime;  //
    double DeliveryTime = m_t-mDeliveringBeginTime.to_seconds()*1000.;
    if(DeliveryTime<0) return 0.;
    return Membrane_Amplitude*(1-exp(-Rushin_A*DeliveryTime))*exp(-Rushin_B*DeliveryTime);
}
/*   double A_term = (1-exp(-Rushin_A*LocalTime));
    double B_term = exp(-Rushin_B*LocalTime);
    double I_M = Membrane_Amplitude*(1-exp(-Rushin_A*LocalTime))*exp(-Rushin_B*LocalTime);

    double MembraneRushin_V = Membrane_Amplitude*A_term*B_term;
*/
//        Membrane_Amplitude*(1-exp(-Rushin_A*LocalTime))*exp(-Rushin_B*LocalTime);
//    return MembraneRushin_V;

float NeuronPhysical::
    AxonTimeDerivative_Get(float Delay)
{
    double LocalTime = m_t-Delay;  //
    double dVdt = Axon_Amplitude*(
                      Axon_A*exp(-Axon_A*LocalTime-Axon_B*LocalTime)
                      -Axon_B*exp(-Axon_B*LocalTime)*exp(1-exp(-Axon_A*LocalTime))
                      );
    return dVdt;
}

/**
     * @brief Calculate the membrane's new potential by solving a DE at
     * the new time after advancing time by the Heartbeat value
     */

    void NeuronPhysical::
    Calculate_Do()
{
    m_MembraneRushin_V = MembraneRushinVoltage_Get();
    m_dV_dt_membrane = MembraneRushinTimeDerivative_Get() ;
    m_SynapsesEnabled = m_Membrane_V < ThresholdPotential;
    switch(StageFlag_Get())
    {
        case gcsm_Relaxing:
        {
            if(m_Relaxing_Stopped)
                m_dV_dt_membrane = 0;
            else
                m_dV_dt_membrane = MembraneRushinTimeDerivative_Get();
            break;
        }
        case gcsm_Computing:
        {
            // Previous membrane!
            m_dV_dt_membrane = 0;
            m_MembraneRushin_V = 0;
            break;
        }
        case gcsm_Delivering:
        {
            m_PeakReached = m_dV_dt_membrane < 0;
            break;
        }
        default: assert(0); break;
    }
    double dV_dt_AIS = m_Membrane_V/m_R_membrane ;
    m_MembraneAIS_dVdt = dV_dt_AIS *m_dt   // The AIS dV
                         /m_C_membrane;

    m_MembraneRushin_dVdt =  m_dV_dt_membrane *m_dt   // The rush-in dV
                            /m_C_membrane;
    dV_dt_AIS = m_dV_dt_membrane-dV_dt_AIS;
    m_MembraneResulting_dVdt = dV_dt_AIS *m_dt   // The charge difference
                               /m_C_membrane;
    m_Membrane_V += .2+
                    (dV_dt_AIS)*m_dt   // The charge difference
        /m_C_membrane
        ;
    if(abs(m_Membrane_V)<2)
        dV_dt_AIS /=2;
    if(gcsm_Relaxing == StageFlag_Get())
    {
        m_PeakReached = dV_dt_AIS > 0;
    }
}

// Handle neuronal membrane's potential in 'Computing' mode
void NeuronPhysical::
    Heartbeat_Computing_Do()
{
    if(m_HasUnhandledInput)
    {
        OutputItem();
        m_Membrane_V += 8; // A temporary hack, just to imitate potential increase
        m_HasUnhandledInput = false;
    }
    Calculate_Do();
}

// Handle neuronal membrane's potential in 'Delivering' mode
void NeuronPhysical::
    Heartbeat_Delivering_Do()
{
    Calculate_Do();
}

// Handle heartbeats in 'Relaxing' mode
void NeuronPhysical::
    Heartbeat_Relaxing_Do()
{
    Calculate_Do();
}

// return true if to stop heartbeating in 'Computing' mode
bool NeuronPhysical::
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
bool NeuronPhysical::
    Heartbeat_Delivering_Stop()
{  
    if(m_Membrane_V >= ThresholdPotential)
        return false; // Continue delivering
    // Stop delivering, but imitate hyperpolarization
    m_PeakReached = false;
    return true;
}

// return true if to stop heartbeating in 'Relaxing' mode
bool NeuronPhysical::
    Heartbeat_Relaxing_Stop()
{
    if(m_HasUnhandledInput)return true;

    if ((abs(m_Membrane_V )>= AllowedRestingPotentialDifference))
        return false; // Continue relaxing
    if(!m_PeakReached) return false;
    // Make sure the resting potential really zero
    m_Membrane_V = 0; m_Relaxing_Stopped = true;
    return true;
}


 void  NeuronPhysical::
    OutputItem(void)
{
     if(0==OutputCounter++%5)
    cerr <<  sc_time_String_Get(scLocalTime_Get()) << ","
         << m_Membrane_V << ","
         << m_MembraneRushin_V/3 << ","
         << m_MembraneRushin_dVdt << ","
         << m_MembraneAIS_dVdt << ","
         <<m_MembraneResulting_dVdt << ","
         << m_SynapsesEnabled<< ","
         <<  "\n"; //<
 }


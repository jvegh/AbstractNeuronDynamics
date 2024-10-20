#ifndef SCGENCOMP_CONFIG_H
#define SCGENCOMP_CONFIG_H

// Config parameters for the scGenComp units
// The sc_core::sc_time(100,SC_NS) form must not be used
#define HEARTBEAT_TIME_DEFAULT 1000 //psec
//#define HEARTBEAT_TIME_RESOLUTION 10 //psec
#define HEARTBEAT_TIME_DEFAULT_BIO 50 //usec
//#define HEARTBEAT_TIME_RESOLUTION sc_core::sc_time(100,SC_PS)


// Config parameters for the scGenComp_Bio units
//How tightly the actual membrane potential must approach its resting value, in mV
#define AllowedRestingPotentialDifference 2.
// The absolute value of the resting potential
#define RestingMembranePotential -65
// The difference of threshold potential to the resting potential
#define ThresholdPotential 20


#endif // SCGENCOMP_CONFIG_H

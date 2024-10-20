/** @file NeuronPhysicalTest.h
 *  @ingroup GENCOMP_MODULE_TEST

 *  @brief A unit to implement a physics-based neuron-like processing unit
 *  It has the basic functionality of biological neurons,
 */
/*
 *  @author János Végh (jvegh)
 *  @bug No known bugs.
*/


#ifndef NEURONPHYSICALTEST_H
#define NEURONPHYSICALTEST_H
/** @addtogroup GENCOMP_MODULE_TEST
 *  @{
 */

#include "NeuronPhysical.h"

/**
 * \class NeuronPhysicalTEST
 * \brief  A simple Gtest class to implement the operation of a general neuron-like computing unit
 *
 *
 */

class NeuronPhysicalTEST : public NeuronPhysical
{
public:
    /*!
     * \brief Creates a test neuron-like general computing unit
     * @param[in] nm the SystemC name of the module
     *
     * Creates a class for testing the neuron-like general biological computing unit.
     * A template for developing and testing your own first units
     *
     * Solves PDE, but only imitates inputs
     */
    NeuronPhysicalTEST(sc_core::sc_module_name nm   // Module name
                         );
    ~NeuronPhysicalTEST(void);
//    void Initialize_Do();
    sc_core::sc_time PU_InitialDelayTime = sc_core::sc_time(100,SC_US);
    sc_core::sc_time PU_InputTime1       = sc_core::sc_time(190,SC_US); // Normal input
    sc_core::sc_time PU_InputTime2       = sc_core::sc_time(213,SC_US); // Normal input
    sc_core::sc_time PU_InputTime3       = sc_core::sc_time(245,SC_US); // Normal input
    sc_core::sc_time PU_InputTime4       = sc_core::sc_time(260,SC_US); // Normal input
    sc_core::sc_time BaseTime;
 };
/** @}*/

#endif // SCBIOGENCOMPNEURONDEMOTEST_H

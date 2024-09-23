/** @file NeuronDemoTest.h
 *  @ingroup GENCOMP_MODULE_TEST

 *  @brief A demo unit to demonstrate the operation of a neuron-like processing unit
 *  It has the basic functionality of biological neurons,
 */
/*
 *  @author János Végh (jvegh)
 *  @bug No known bugs.
*/


#ifndef NEURONDEMOTEST_H
#define NEURONDEMOTEST_H
/** @addtogroup GENCOMP_MODULE_TEST
 *  @{
 */

#include "NeuronDemo.h"

/**
 * \class NeuronDemoTEST
 * \brief  A simple Gtest class to implement the operation of a general neuron-like computing unit
 *
 * Operating principle of event processing:
 * The unit cycles through the states of processing
 *
 */

class NeuronDemoTEST : public NeuronDemo
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
    NeuronDemoTEST(sc_core::sc_module_name nm   // Module name
                         );
    ~NeuronDemoTEST(void);
 };
/** @}*/

#endif // SCBIOGENCOMPNEURONDEMOTEST_H

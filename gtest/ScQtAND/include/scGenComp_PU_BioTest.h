/** @file scGenComp_PU_BioTest.h
 *  @ingroup GENCOMP_MODULE_TEST

 *  @brief A demo unit to demonstrate the operation of a biological processing unit
 *  It has the basic functionality of biology
 */
/*
 *  @author János Végh (jvegh)
 *  @bug No known bugs.
*/


#ifndef SCBIOGENCOMPTEST_H
#define SCBIOGENCOMPTEST_H
/** @addtogroup GENCOMP_MODULE_TEST
 *  @{
 */

#include "scGenComp_PU_Bio.h"

/**
 * \class scGenComp_PU_BioTEST
 * \brief  A simple Gtest class to implement the operation of a general bio computing unit
 *
 * Operating principle of event processing:
 * The unit cycles through the statrs of processing
 *
 */

class scGenComp_PU_BioTEST : public scGenComp_PU_Bio
{
public:
    /*!
     * \brief Creates a test bio general computing unit
     * @param[in] nm the SystemC name of the module
     * @param[in] Heartbeat the integration time lap
     *
     * Creates a class for testing the  general biological computing unit.
     * A template for developing and testing your own first units
     *
     * Solve PDE, but only imitates inputs
     */
    scGenComp_PU_BioTEST(sc_core::sc_module_name nm   // Module name
                         );
    ~scGenComp_PU_BioTEST(void);

    /**
     * @brief What to do in state 'Delivering'
     *
     * Set delivering time, using delay
     */
    virtual void DeliveringBegin_Do();
    /**
     * @brief What to do in state 'Computing'
     *
     * Set computing time, using delay
     */
    virtual void ComputingBegin_Do();

    virtual void InputReceived_Do(void);
 };
/** @}*/

#endif // SCBIOGENCOMPTEST_H

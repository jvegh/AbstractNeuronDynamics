/** @file scGenComp_PU_AbstractTEST.h
 *  @ingroup GENCOMP_MODULE_TEST

 *  @brief A test unit to demonstrate the operation of an abstract processing unit
 */
/*
 *  @author János Végh (jvegh)
 *  @bug No known bugs.
*/


#ifndef SCABSTRACTGENCOMPTEST_H
#define SCABSTRACTGENCOMPTEST_H
/** @addtogroup GENCOMP_MODULE_TEST
 *  @{
 */

#include "scGenComp_PU_Abstract.h"

/**
 * \class scGenComp_AbstractTEST
 * \brief  A simple test class to implement the operation of a general abstract computing unit
 *
 * Operating principle of event processing:
 * The unit cycles through the states of processing, by issuing the corresponding events with some delay
 */

class scGenComp_PU_AbstractTEST : public scGenComp_PU_Abstract
{
  public:
    /*!
     * \brief Creates a test abstract general computing unit
     * @param[in] nm the SystemC name of the module
     * @param[in] Heartbeat the integration time lap
     *
     * Creates a demo abstract general computing unit.
     * A template for developing and testing your own first units
     */
      scGenComp_PU_AbstractTEST(sc_core::sc_module_name nm   // Module name
                                ,sc_core::sc_time FixedComputingTime = sc_core::sc_time(SC_ZERO_TIME)
                                ,sc_core::sc_time FixedDeliveringTime = sc_core::sc_time(SC_ZERO_TIME)
                                ,bool ExternSynchronMode =false
                                 );  // Heartbeat time
     ~scGenComp_PU_AbstractTEST(void);

};
/** @}*/

#endif // SCABSTRACTGENCOMPDEMO_H

/** @file scGenComp_PU_TechTest.h
 *  @ingroup GENCOMP_MODULE_TEST

 *  @brief A demo unit to demonstrate the operation of a technical processing unit
 *  It has the basic functionality of technics
 */
/*
 *  @author János Végh (jvegh)
 *  @bug No known bugs.
*/


#ifndef SCTECHGENCOMPTEST_H
#define SCTECHGENCOMPTEST_H
/** @addtogroup GENCOMP_MODULE_TEST
 *  @{
 */

#include "scGenComp_PU_Tech.h"

#define TECH_TEST_HEARTBEAT_TIME sc_core::sc_time(100,SC_US)
#define TECH_TEST_INITIAL_DELAY sc_core::sc_time(10,SC_MS)

/*
 * \class scGenComp_PU_TechTEST
 * \brief  A simple demo class to implement the operation of a general bio computing unit
 *
 * Operating principle of event processing:
 * The unit cycles through the statios of processing
 *
 */
class scGenComp_PU_TechTEST : public scGenComp_PU_Tech
{
  public:
    /*!
     * \brief Creates a demo technical general computing unit
     * @param[in] nm the SystemC name of the module
     * @param[in] Heartbeat the integration time lap
      *
     * Creates a demo abstract biological computing unit.
     * A template for developing and testing your own units
     */
      scGenComp_PU_TechTEST(sc_core::sc_module_name nm   // Module name
                            ,sc_core::sc_time FixedComputingTime = sc_core::sc_time(SC_ZERO_TIME)
                            ,sc_core::sc_time FixedDeliveringTime = sc_core::sc_time(SC_ZERO_TIME)
                            ,bool ExternSynchronMode =false
                            );
      ~scGenComp_PU_TechTEST(void);

    /**
      * Prepare events for the demo unit; run before the other 'method's
      *
      * Makes two full cycles, no functionaly, just proves that cycling is correct
      *
      */
    void Initialize_Do();

     /**
     * @brief What to do in statio 'Delivering'
     */
    virtual void DeliveringBegin_Do(){}
    /**
     * @brief What to do in statio 'Processing'
     */
    virtual void ProcessingBegin_Do(){}
    /**
     * @brief What to do in station 'Ready'
     */
    virtual void Ready_Do(){}
    /**
     * @brief What to do in statio 'Relaxing
     */
    virtual void RelaxingBegin_Do(){}
     #if 0
    /**
     * @brief Handle heartbeats in 'Processing' mode
     *
     * -  Re-issues EVENT_GenComp.Heartbeat until membrane potential at hillock reaches its threshold value
     * -  Starts to receive heartbeats after EVENT_GenComp.DeliveringBegin arrives
     * -  Stops generating heartbeats after EVENT_GenComp.DeliveringEnd arrives
     *     then issues EVENT_GenComp.RelaxingBegin
     * -  Heartbeat frequency is set to BIO_HEARTBEAT_TIME (10 us)
     */
      virtual void Heartbeat_Processing_Do();

    /**
      * Prepare events for the demo unit; run before the other 'method's
      *
      * See the operation in scGenComp_PU_BioDEMO.cpp, InitializeForDemo_method()
      */
    void Initialize_Do();


    /**
     * @brief Processing_Finished
     * @return true if processing finished and 'Delivering' follows
     *
     * (return 'true' @900 us
     */
    virtual bool Processing_Finished(void);
    #endif
};
/** @}*/

#endif // SCTECHGENCOMPTEST_H

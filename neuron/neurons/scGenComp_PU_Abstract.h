/** @file scGenComp_PU_Abstract.h
 *  @ingroup GENCOMP_MODULE_PROCESS
 *
 *  @brief Function prototypes for the computing module.
 *  Just event and stage handling, no real activity
 *  @todo Implement TransmittingEnd
  */
/*
 *  @author János Végh (jvegh)
 *  @bug No known bugs.
 */

#ifndef SCABSTRACTGENCOMP_H
#define SCABSTRACTGENCOMP_H
/* * @addtogroup GENCOMP_MODULE_PROCESS Modules for general computing
 *  @{
 */

#include <bitset>
//#include "../../3rdParty/Minuit/include/GaussRandomGen.h"
//#include "GaussRandomGen.h"
#include "EventGencomp.h"
using namespace sc_core; using namespace sc_dt; using namespace std;
//class scGenComp_Simulator;
#include "scGenComp_Config.h"
#include "Utils.h"

#define SC_MAKE_TIME_BENCHMARKING  // uncomment to measure the SystemCtime with benchmarking macros
#define MAKE_TIME_BENCHMARKING  // uncomment to measure the clock time with benchmarking macros
#ifdef SC_MAKE_TIME_BENCHMARKING
    #include "scMacroTimeBenchmarking.h"    // Must be after the define to have its effect
#endif
#ifdef MAKE_TIME_BENCHMARKING
    #include "MacroTimeBenchmarking.h"    // Must be after the define to have its effect
#endif


#ifndef SCBIOGENCOMP_H
#ifndef SCTECHGENCOMP_H // Just to exclude for Doxygen, to not repeat it

/*!
 * \class scGenComp_PU_Abstract
 * \brief  Class to deal with operations of an abstract processing unit (\gls{PU}),
 * implementing a single-shot elementary computation in the sense of the General Computing Paradigm
 * @cite VeghRevisingClassicComputing:2021. The stages of operation
 * are marked by event pairs (or single events) in the sense as the SystemC @cite SystemCBook:2010
 * programming engine uses that notion. Using those events and SystemC's internal
 * time scale, <i>the class isolates the simulated time and the clock time
 * the \gls{PU} needs to simulate the processing</i>. The code of this base class is a bit complex, in order to enable simple derived \gls{PU}s:
 * the complex event handling mechanism is confined in this class; the derived classes
 * are free from any SystemC specific programming
 * (for and example see NeuronDemo).
 * The \gls{PU} has correspondingly the stages 'Computing', 'Delivering' and 'Relaxing'
 * as the minimum necessary basic states. The module implements a single-shot normal operating mode,
 * that is, the operation is automatic between the corresponding xxxBegin and xxxEnd events,
 * and at the end of the xxxEnd events the corresponding next xxxBegin event is issued.
 * A distinguished (compared to other simulators) feature of the class is handling time faithfully.
 *
 * Biology measures time with some activity (such as changing neural membrane's potential).
 * In that case scGenComp_PU_Abstract#mFixedComputingTime and scGenComp_PU_Abstract::mFixedDeliveringTime are set to zero,
 * and the scGenComp_PU_Abstract::Heartbeat_method() checks periodically if the recent operating stage finished
 * using stage-specific methods XXX_stop().
 * (This is the operating mode for biology, including neuronal simulation.)
 * For details, see scGenComp_PU_Bio and NeuronDemo.
 *
 * In technical systems, the usual method is to use central-clock driven synchronized PUs.
 * If scGenComp_PU_Abstract::mCentralClockMode is set to true,
 * methods scGenComp_PU_Abstract::scClockUp_method() and scGenComp_PU_Abstract::scClockDown_method()
 * trigger the required events, i.e., the unit uses entirely external timing. (This is the central clock driven mode)
 *
 * If scGenComp_PU_Abstract::mCentralClockMode is set to false, the PU sets
 * scGenComp_PU_Abstract::mFixedComputingTime and scGenComp_PU_Abstract::mFixedDeliveringTime
 * and the corresponding XXX_Begin()
 * routines schedule the corresponding XXX_End() events compared to the input's arrival.
 * For details, see scGenComp_PU_Tech. (This is an asynchron mode.)
 *
 * Advancing simulated time
 *
 * In timed mode, the derived modules derived can use timed computing and delivering,
 * in which case XXXBegin methods issue an EVENT_GenComp.XXXEnd.notify(FixedTime) event.
 * The simulated time 'jumps', no smaller steps.
 * In untimed mode it issues EVENT_GenComp.Heartbeat.notify(sc_time(HeartbeatTime)) periodically.
 * The simulated time 'jumps' with the value of HeartbeatTime.
 * Either issuing an event EVENT_GenComp.XXXEnd (fixed time) or
 * the result of a calculation (variable time) in Heartbeat decides
 * if the corresponding stage transition reached.
 * The modules issue per module 'Heartbeat' signals; there is always only one signal scheduled.
 *
 * The heartbeating is not a central synchronization: it enables to consider module's stages
 * (such as neuronal analog voltage levels) and handles changing module's stages,
 * instead of external synchronization signals)
 *
 * In synchronized mode, central clock signals issue signals for 'Computing' and 'Delivering'.
 */

#endif //SCTECHGENCOMP_H
#endif //SCBIOGENCOMP_H


enum GenCompStageMachineType_t {gcsm_Sleeping,
                                 gcsm_Relaxing,
                                 gcsm_Computing,
                                 gcsm_Delivering,
                                 gcsm_Initializing,
                                 gcsm_InputReceived,
                                 gcsm_Synchronizing,
                                 gcsm_Failed
};

/*! \enum GenCompStageMachineType_t
 * The operation of an elementary computing unit of general computing is modelled as a multiple-state machine-like structure,
 * with internal state stored in state variable scGenComp_PU_Abstract::mStageFlag.
@verbatim

                   Sleeping<------> Relaxing     <---Initializing
                                   ^   ^     ^  \   ^
                                   |   HBeat |   \  |-InputReceived
                                   |         |   v  v
                                   |   Failed-   Computing
                                   | Synchronizing /
                                   | |            /
                                   | v           v
                                  Delivering    <-----Synchronizing
 @endverbatim

The stage changing are implemented in two steps. The non-virtual methods XXX_method()
(usually, hidden from the users of derived classes) process all needed activity
for the correct sequencing of stages and handling event-related activity.
(They are declared as 'private' to prevent non-intended activity of users.)
They call XXX_Do() functions (may be overloaded by users) which may implement
additional activity. If needed, the upstream object's XXX_Do()
can be called within those functions.

 * - The stages below are momentary stages: need action and pass to one of the above states
 * - Failing : Something went wrong, reset unit and pass to Relaxing
 * - Synchronizing: deliver result, anyhow ;  (a momentary stage)
 *   - in biological mode, deliver immediate spike
 *   - in technical mode, deliver immediate result (such and in a \gls{GPU})
 *   Passes to Relaxing (after issuing 'End Computing')
*/

/*! \var GenCompStageMachineType_t::gcsm_Sleeping
 *  Essentially, same as gcsm_Relaxing, but no Heartbeats,
 *  and even the HW unit can go to sleep (not yet implemented)
 *
 *  Begins in stage gcsm_Relaxing, when sleeping condition met
 *  (not yet entirely implemented), and returns to
 *   gcsm_Relaxing (temporarily, to  gcsm_Computing
 */

/*! \var GenCompStageMachineType_t::gcsm_Relaxing
  The abstract units are generated in this stage: everything is prepared, variables available.
  The unit is ready, waiting for EVENT_GenComp_type::ComputingBegin;
  after receiving it, passes to GenCompStageMachineType_t::gcsm_Computing.
  ComputingBegin can be provided by clock signal (if clock driven) or InputReceived.
  Important: changing to stage gcsm_Relaxing does not involve reseting!!

  Begins with   EVENT_GenComp_type::RelaxingBegin

    Normally passes to gcsm_Computing
*/

/*! \var GenCompStageMachineType_t::gcsm_Delivering
    Makes internal result externally available (delivering its result to its output section).
    Normally goes to gcsm_Relaxing.  An external Synchronizing may force immediate Delivering
 * - Delivering: The unit is
 *   - after some activity, passes to 'Relaxing'
 *   (it sends the result to its chained unit(s))
 *   it Sends 'Begin Transmitting', a parallel activity)
 *
 *  Spans time between EVENT_GenComp_type::DeliveringBegin and EVENT_GenComp_type::DeliveringEnd
 */

/*! \var GenCompStageMachineType_t::gcsm_Computing
  Computes (may receive inputs), then passes to  stage gcsm_Delivering.

  Spans time between EVENT_GenComp_type::ComputingBegin and EVENT_GenComp_type::ComputingEnd
*/

/*! \var GenCompStageMachineType_t::gcsm_Initializing
 *  Initilizes the unit. Passes to GenCompStageMachineType_t::gcsm_Relaxing
 *
 * A momentary stage
 *
 * Begins at EVENT_GenComp_type::Initialize.
 */

/*! \var GenCompStageMachineType_t::gcsm_InputReceived
 *  Receives input. (input arguments are delivered to the input section)
 *  If not clocked, may pass to GenCompStageMachineType_t::gcsm_Computing
 *
 *  A momentary stage
 *
 * Begins at EVENT_GenComp_type::InputReceived
 */

/*! \var GenCompStageMachineType_t::gcsm_Synchronizing
 *  Synchronizes the operation to fellow PUs
 *  (passes immediately to GenCompStageMachineType_t::gcsm_Delivering,
 *  except when in state GenCompStageMachineType_t::gcsm_Delivering).
 *
 * A momentary stage
 *
 * Begins at EVENT_GenComp_type::Synchronize
 */

/*! \var GenCompStageMachineType_t::gcsm_Failed
 *  Enables to simulate failed operations.
 *  (Actually, re-initializes unit)
 *
 * A momentary stage
 *
 * Begins at EVENT_GenComp_type::Failed
 */


extern string GenCompStagesString[];

/*! \var typedef GenCompPUObservingBits_t
 * \brief the names of the bits in the bitset describing
 * which signals in scGenComp_PU_Abstract are monitored by the scGenComp_Simulator
 */
typedef enum
{
    gcob_ObserveGroup,              ///< The module's group is observed (by the simulator)
    gcob_ObserveModule,             ///< The module is observed (by the simulator)
    gcob_ObserveComputingBegin,     ///< Observe 'ComputingBegin'
    gcob_ObserveComputingEnd,       ///< Observe 'ComputingEnd'
    gcob_ObserveDeliveringBegin,    ///< Observe 'DeliveringBegin'
    gcob_ObserveDeliveringEnd,      ///< Observe 'DeliveringEnd'
    gcob_ObserveHeartbeat,          ///< Observe 'Heartbeat's of the PU
    gcob_ObserveInput,              ///< Observe 'Input's of the PU
    gcob_ObserveInitialize,         ///< Observe 'Initialize' of the PU
    gcob_ObserveRelaxingBegin,      ///< Observe 'RelaxingBegin'
    gcob_ValueChanged,              ///< Observe 'Value changed'
    gcob_Max                        // just maintains the number of bits used
} GenCompPUObservingBits_t;
extern string GenCompObserveStrings[];


class scGenComp_PU_Abstract: public sc_core::sc_module
{
   public:
    /*!
     * \brief Create an abstract processing unit for the general computing paradigm
     * @param[in] nm the SystemC name of the module
     * @param[in] FixedComputingTime Time of computing
     * @param[in] FixedDeliveringTime Time of delivering
     * @param[in] ExternSynchronMode If external synchronization used
     */
    scGenComp_PU_Abstract(sc_core::sc_module_name nm
                            ,sc_core::sc_time FixedComputingTime = SC_ZERO_TIME
                            ,sc_core::sc_time FixedDeliveringTime = SC_ZERO_TIME
                            ,bool ExternSynchronMode =false
                             );

    virtual ~scGenComp_PU_Abstract(void); // Must be overridden

    /** @brief The extra processing for beginning computing
     */
    virtual void ComputingBegin_Do(){}

    /** @brief The extra processing for finishing computing
     */
    virtual void ComputingEnd_Do(){}

    /** @brief The extra activity for starting internal delivering
     */
    virtual void DeliveringBegin_Do(){}

    /** @brief The extra activity for finishing internal delivering
    */
    virtual void DeliveringEnd_Do(){}

    /** @brief The extra activity for handling event 'Failed'.
     *  Includes initialization.
     */
    virtual void Failed_Do(){}

    /** @brief Additional initialization activity
     * Called from Initialize_method()
     */
    virtual void Initialize_Do(){}

    /** @brief Code to overload actual input processing
     * Called from InputReceived_method()
     */
    virtual void InputReceived_Do(void){}

     // @return the number of the received inputs
    uint32_t NoOfInputsReceived_Get(){return mInputsReceived;}

    /** @brief Notify the observer about some important change
     * @param[in] ObservedBit The event that occured
     */
    void ObserverNotify(GenCompPUObservingBits_t ObservedBit);

    /*!
     * \brief Set an observing bit for this scGenComp_PU_Abstract
     * \param B is the bit to get in mObservedBits
     * \return is the requested value of the bit
     */
    bool ObservingBit_Get(GenCompPUObservingBits_t B)
    {
        assert(B < gcob_Max);  // Check if wrong bit
        return mObservedBits[B];
    }

    /*!
     * \brief Set an observing bit for this scGenComp_PU_Abstract
     * \param B is the bit to set  in mObservedBit
     * \param V is the requested value of the bit
     */
    void ObservingBit_Set(GenCompPUObservingBits_t B, bool V)
    {
        assert(B < gcob_Max);  // Check if wrong bit
        mObservedBits[B] = V;
    }
    /**
     * @brief OperationCounter_Get
     * @return how many operations the unit performed
     */
    int32_t OperationCounter_Get(void){return mOperationCounter;}

    void OperatingTimes_Set(sc_core::sc_time ComputingTime = SC_ZERO_TIME,
                            sc_core::sc_time DeliveringTime  = SC_ZERO_TIME)
    {
        mFixedComputingTime = ComputingTime;
        mFixedDeliveringTime = DeliveringTime;
    }

    /**
     * Save the address of the observer simulator in the PU
     */
    //void RegisterSimulator(scGenComp_Simulator*);

    // Do extra activity needed in mode gcsm_Relaxing
    virtual void RelaxingBegin_Do(){}

    /**
     * @brief scIdlePeriod_Get, can be called after an operation started
     * @return the idle time before the beginning of the last operation
     */
    inline sc_core::sc_time
    scIdlePeriod_Get() const {return mIdlePeriod;}
    /**
     * @brief ResultPeriod_Get, can be called after an operation finished
     * @return 
     */
    sc_core::sc_time scResultPeriod_Get() const { return mResultPeriod;}
    /**  @return the computing plus delivery time
     */
    sc_core::sc_time scProcessingPeriod_Get() const { return mRelaxingBeginTime - mLocalTimeBase;}
    //  @return the time of the beginning of delivery (firing)
    sc_core::sc_time scDeliveringTimeBegin_Get() const { return mDeliveringBeginTime ;}
    // @return The beginning of the simulated time of the recent operation
    sc_core::sc_time scLocalTimeBase_Get(void)const { return mLocalTimeBase;}
    /**
     * @brief scRelaxingBeginTime_Get
     * @return The time the last operation ended
     */
    sc_core::sc_time scRelaxingBeginTime_Get() const { return mRelaxingBeginTime;}
     EVENT_GenComp_type EVENT_GenComp;

    inline GenCompStageMachineType_t StageFlag_Get(void){ return mStageFlag;}
//    virtual void TransmissionBegin_Do(){}
protected:
    /**
     * @brief Cancels all pending events. Called from Initialize_method(),Synchronize_method(),
     */
    virtual void CancelEvents(void);

    // To sidestep private call
    void ChargeupFailed(void){Failed_method();};
    /**
     * @brief HaveEnoughInputs
     * @return true if the sufficient number of inputs arrived (say multi-input gates)
     */
    virtual bool HaveEnoughInputs(void){return true;}
    /**
     * @brief Adjust neuron's heartbeat (integration) time,
     * to keep the precision good and the coputing time tolerable.
     * When passing from one stage to another, changes the integration step back to default time;
     * then increases following the steps, attempts to increase it.
     */
    virtual void Heartbeat_Adjust(void);

    /**
     * @brief Handle Heartbeat in 'Computing' mode
     *
     * It should be overloaded (by default makes nothing).
     * Called by Heartbeat_method();
     */

     virtual void Heartbeat_Computing_Do(){}
    /**
     * @brief Handle Heartbeat in 'Delivering' mode
     *
     * It should be overloaded (by default makes nothing).
     * Called by Heartbeat_method()
     */
    virtual void Heartbeat_Delivering_Do(){};
    /**
     * @return true if to stop heartbeating in 'Computing' mode
     */
    virtual bool Heartbeat_Computing_Stop(){return scLocalTime_Get()>mFixedComputingTime;}
    /**
     * @return true if to stop heartbeating in 'Delivering' mode
     */
    virtual bool Heartbeat_Delivering_Stop(){return scLocalTime_Get()>mFixedComputingTime+mFixedDeliveringTime;}
    /**
     * @brief Handle Heartbeat in 'Relaxing' mode
     *
     * Called by Heartbeat_method();
     */
    virtual void Heartbeat_Relaxing_Do(){}
    /**
     * @brief Handle 'sleep' stage, i.e., if to continue heartbeats in 'Relaxing' mode
     */
    virtual bool Heartbeat_Relaxing_Stop(){return true;}

    /**
     * Access function to neuron's heartbeat (integration) time,     */
    sc_core::sc_time HeartbeatTime_Get(void){return m_Heartbeat_time;}
    void HeartbeatTime_Set(sc_core::sc_time HBT)
    {   m_Heartbeat_time = HBT;
        m_dt = m_Heartbeat_time.to_seconds()*1000.;
    }
    float HeartbeatTimeInMicrosec_Get(){    return m_Heartbeat_time.to_seconds()*1000.*1000.;}

    /**
     * @brief scLocalTime_Get
     * @return The simulated time since the beginning of the recent operation
     */
    sc_core::sc_time scLocalTime_Get() const {return sc_core::sc_time_stamp()-mLocalTimeBase;}
    // Return local time for calculations in derived classes, to avoid using direct SystemC calls
    float LocalTimeInMillisec_Get(){    return scLocalTime_Get().to_seconds()*1000.;}
    float LocalTimeInMicrosec_Get(){    return scLocalTime_Get().to_seconds()*1000.*1000.;}
    float LocalTimeInNanosec_Get(){    return scLocalTime_Get().to_seconds()*1000.*1000.*1000.;}
    /**
     * @brief scLocalTimeBase_Set
     * @param T The beginning of the simulated time of the recent operation
     */
    void scLocalTimeBase_Set(sc_core::sc_time T) { mLocalTimeBase = T;}
    /** @param S Set state machine to stage S */

    inline void StageFlag_Set(GenCompStageMachineType_t S){    mStageFlag = S;
        mStateFlag = mStageFlag;}
    GenCompStageMachineType_t mStageFlag;    //< Preserves recent stage
    float mStateFlag;
     /**
      * @return true if running in synchronized mode
      */
    bool CentralClockMode_Get(void){return mCentralClockMode;}
    /**
     * the PUs have a local time:
     * the time spent since starting recent processing
     */
    sc_core::sc_time
         mLocalTimeBase         //< The beginning of the local computing; time of 'ProcessingBegin'
                                //< aka, When !this! processing begun
        ,mDeliveringBeginTime   //< When the result was ready; aka when fired
        ,mRelaxingBeginTime       //< When this processing begun
        ,mResultPeriod       //< Last processing time duration (the result) (T(DeliveringBegin)-T(ProcessingBegin))
        ,mIdlePeriod         //< Time duration (ProcessingBeginTime - LastRelaxingBeginTime)
         ;
    /**
     * Store here which events the unit wants to be observed
     *
     * @see #GenCompPUObservingBits_t
     */
    bitset<gcob_Max> mObservedBits;   //< The bits of the GenComp_PU state
//    scGenComp_Simulator* MySimulator;     //< The simulator that observes us
    int32_t mOperationCounter;            //< Count the operations
    /**
     * @brief mFixedComputingTime,mFixedDeliveringTime
     *
     * If this value is NOT SC_ZERO_TIME, the system uses this value
     * for the time span of computing and delivering time, otherwise
     * uses scGenComp_PU_Abstract::Heartbeat_method() periodically to see if the operation finished.
     * The derived units must override Heartbeat_XXX_Stop()
     * to provide the appropriate stopping conditions
     */

    sc_core::sc_time
         mFixedComputingTime //< If to use fixed-time computing
        ,mFixedDeliveringTime //< If to use fixed-time delivering
        ,m_Heartbeat_time_default    //< The reset value of heartbeat time
        ,m_Heartbeat_time_resolution    //< The time tolerance
        ,m_Heartbeat_time; //< The size of the integration step size
    uint32_t mInputsReceived;        // No of inputs received(arguments or or spikes)
    bool m_Relaxing_Stopped;
    float m_dt; // The integration time step
    float m_t; // The local time
     /** If set, the class enters phases 'Computing' and 'Delivering ONLY if
     * an external clock rising and falling edge arrives.
     * Not to be confused with the inter-unit synchronization
     * see scGenComp_PU_Abstract::Synchronize_method()
     */

    /** If set, external clock starts computing and delivering */
    bool mCentralClockMode;       //< If module needs central clock synchrony signals for changing its states
 //??   bool m_FirstHeartbeatInStage;    //< If to correct for the 1st heartbeat time
    //** Output an item
    virtual void OutputItem(void){}
    sc_trace_file* m_tracefile;
    virtual void Tracing_Initialize();    // By default, makes nothing
    // Benchmarking stores
        #ifdef BENCHMARK_TIME_ACTIVE
            chrono::steady_clock::time_point t_Computing =chrono::steady_clock::now();
            std::chrono::duration< int64_t, nano> x_Computing,s_Computing = (std::chrono::duration< int64_t, nano>)0;
            chrono::steady_clock::time_point t_Delivering =chrono::steady_clock::now();
            std::chrono::duration< int64_t, nano> x_Delivering,s_Delivering = (std::chrono::duration< int64_t, nano>)0;
            chrono::steady_clock::time_point t_Relaxing =chrono::steady_clock::now();
            std::chrono::duration< int64_t, nano> x_Relaxing,s_Relaxing = (std::chrono::duration< int64_t, nano>)0;
            float TimeDuration_Computing_Get()
            {  return  x_Computing.count()/1000/1000.;}
            float TimeDuration_Delivering_Get()
            {  return x_Delivering.count()/1000/1000.;}
            float TimeDuration_Relaxing_Get()
            {  return x_Relaxing.count()/1000/1000.;}
            virtual void TimeDuration_Computing_Print()
            {  std::cerr  << "Computing took " << TimeDuration_Computing_Get() << " msec CLOCK time" << endl;}
            virtual void TimeDuration_Delivering_Print()
            {  std::cerr  << "Delivering took " << TimeDuration_Delivering_Get() << " msec CLOCK time" << endl;}
            virtual void TimeDuration_Relaxing_Print()
            {  std::cerr  << "Relaxing took " << TimeDuration_Relaxing_Get() << " msec CLOCK time" << endl;}
        #endif // BENCHMARK_TIME_ACTIVE
        #ifdef SC_BENCHMARK_TIME_ACTIVE
            sc_core::sc_time SC_t_Computing = SC_ZERO_TIME, SC_x_Computing, SC_s_Computing;
            sc_core::sc_time SC_t_Delivering = SC_ZERO_TIME, SC_x_Delivering, SC_s_Delivering;
            sc_core::sc_time SC_t_Relaxing = SC_ZERO_TIME, SC_x_Relaxing, SC_s_Relaxing;
            string SC_TimeDuration_Computing_Get()
            {
                return sc_time_String_Get(SC_x_Computing);
            }
            string SC_TimeDuration_Delivering_Get()
            {
                return sc_time_String_Get(SC_x_Delivering);
            }
            string SC_TimeDuration_Relaxing_Get()
            {
                return sc_time_String_Get(SC_x_Relaxing);
            }
            virtual void SC_TimeDuration_Computing_Print()
            {
                std::cerr << std::fixed << std::setfill (' ') << std::setprecision(3) << std::setw(7)
                << "Computing took " << sc_time_String_Get(SC_x_Computing) << " ms SC time " << endl;
            }
            virtual void SC_TimeDuration_Delivering_Print()
            {
                std::cerr << std::fixed << std::setfill (' ') << std::setprecision(3) << std::setw(7)
                << "Delivering took " << sc_time_String_Get(SC_x_Delivering) << " ms SC time " << endl;
            }
            virtual void SC_TimeDuration_Relaxing_Print()
            {
                std::cerr //<< std::fixed << std::setfill (' ') << std::setprecision(d) << std::setw(w)
                    << "Relaxing took " << sc_time_String_Get(SC_x_Relaxing) << " ms SC time " << endl;
            }
        #endif // BENCHMARK_SCTIME_ACTIVE
/*
    @endcode
*/
private:
    /**
     * @brief  Receive rising edge of an external clock and issue 'ComputingBegin'
     */
    void scClockUp_method(void);
    /**
     * @brief  Receive falling edge of an external clock and issue 'ComputingEnd'
     */
    void scClockDown_method(void);
     /**
     * @brief Enters stage 'Computing'
     *
     * Usually activated by EVENT_GenComp_type::ComputingBegin,
     * but also implicitly called from InputReceived_method
     *
     * @see ComputingBegin_Do
     * is issued and the stage changes to   GenCompStageMachineType_t#gcsm_Computing
     */
    void ComputingBegin_method();
    /**
     * @brief Computing has finished
     * Usually activated by      EVENT_GenComp_type::ComputingEnd
     */
    void ComputingEnd_method();
    /**
     *  Start internal result delivering. Usually activated by EVENT_GenComp_type::DeliveringBegin
     */
    void DeliveringBegin_method();
    /**
     *  Finish internal result delivering. Usually activated by EVENT_GenComp_type::DeliveringEnd */
    void DeliveringEnd_method();
    /**
     * @brief Module failed.
     *
     * Usually activated by EVENT_GenComp.Failed
     * - In biological computing, it means a failed charge-up; passes to 'Relaxing' stage.
     *
     * - In technical computing, it results in a failed operation
     */
    void Failed_method();
    /**
     * @brief A periodic signal as a timebase to refresh state. Used internally, to avoid using clock signals.
     *
     * Usually activated by EVENT_GenComp::Heartbeat. A heartbeat can be useful in states
     *  Relaxing, Processing, and Delivering
     *
     * Branches according to PU's recent stage and calls (see the corresponding state machine diagram)
     *    -  virtual void Heartbeat_Computing_Do()
     *    -  virtual void Heartbeat_Delivering_Do()
     *    -  virtual void Heartbeat_Relaxing_Do()
     *
     * These routines also calculate the new \gls{PU} parameters. Those parameters
     * enable to return the status if to stop processing.
     * Similarly, it branches according to PU's stage and calls
     *
     *    -  virtual bool Heartbeat_Computing_Stop()
     *    -  virtual bool Heartbeat_Delivering_Stop()
     *    -  virtual bool Heartbeat_Relaxing_Stop()
     *
     *    routines in the corresponding stage,
     *    which return true if the given phase is to terminate
     *
     * Usage
     *   - For technical computing, it is used to display time delays
     *   - For biological computing, it is used to solve PDEs.
     *  The same differential equation is solved, but the terms are depending on the stage
     */
    void Heartbeat_method();

    /**
     * @brief Initialize_method
     *
     * Usually activated by EVENT_GenComp.Initialize
     */
    void Initialize_method();
    /**
     * @brief InputReceived_method
     *
     * An external input received.
     * Inputs can be received in 'Relaxing', and 'Computing'; in stage 'Delivering' they are neglected
     * If sufficient number of inputs received and NOT using external synchnonization,
     * calls ComputingBegin_method()
     *
     * Usually activated by      EVENT_GenComp.InputReceived
     */
    void InputReceived_method();
    /**
     * @brief Relaxing has started
     *
     * Usually activated by event EVENT_GenComp.RelaxingBegin
     * issued in Delivering_End()
     * After delivered the result internally to the 'output section', resetting begins
     */
    void RelaxingBegin_method();
    /**
     * @brief Synchronize the PU to some external condition
     *
     * Usually activated by     EVENT_GenComp.Synchronize

     * - In biological computing, it results in issuing EVENT_GenComp.EndComputing (immediate spiking; passing to 'Delivering')
     * - In technical computing, it results in issuing EVENT_GenComp.ComputingBegin (starting computing; passing to 'Computing')
     */
    virtual void Synchronize_method();
    /**
     * @brief Transmission period has started
     *
     * Usually activated by DeliveringBegin
     * Happens in parallel with delivering
     */
    void TransmissionBegin_method();

};// of class scGenComp_PU_Abstract


/* * @}*/

#endif // SCABSTRACTGENCOMP_H

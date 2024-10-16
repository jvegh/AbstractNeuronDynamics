/** @file scMacroTimeBenchmarking.h
 *  @brief Macros for SystemC  (SIMULATED) time span passing between using the expanded codes
 *  @ingroup GENCOMP_MODULE_STUFF
 *
    These defined macros are benchmarking macros for measuring simulated (e.g. biological) time of executing certain instruction sequences between them.
    The "scMacroTimeBenchmarking.h" file shall be included in the header of the module in question.
    The results of the time measurements will be returned in user-defined variables.
    As the arguments are user-provided, with consistent use several independent
    measurement can be carried out within the source scope. The macros have source-module scope.
    All variables must be passed by reference.

    Notice: this is SIMULATED  time, returned by the SystemC engine; not CLOCK time returned by the processor.
    When using in cooperation (and in an analog way with the corresponding 'scXXX' macros, you may find out
    the CLOCK to SIMULATED time of the benchmarked activity

    The operating time offset due to benchmarking is approx 55-85 nanosecs, per macro pairs.

<b>Usage:</b>
    If the module includes this header, it must be preceded by defining
    @code{.cpp}
        #define SC_MAKE_TIME_BENCHMARKING  // uncomment to measure the SystemC time with benchmarking macros
    @endcode
   If this name is not defined, no code is generated as code expansion for the module.
    The variables, however, must be defined (although they will be
    optimized out as unused ones).
    Alternatively, generating those variables may be protected with bracketing them by pairs
    @code{.cpp}
        #ifdef SC_BENCHMARK_TIME_ACTIVE
        #endif
    @endcode

<b>Example:</b>
    in your module, between the '\#include' files write

    @code{.cpp}
        #define SC_MAKE_TIME_BENCHMARKING  // comment out if you do not want to benchmark
    @endcode
It is a good idea to use these macro expansions at the end of heading block:
the similar macro definitions in included other modules may overload them

     In your class (or member function) define
    @code{.cpp}
    sc_core::sc_time SC_t, SC_x, SC_s;
    @endcode
    and use the macros as as
    @code{.cpp}
    SC_BENCHMARK_TIME_RESET(&SC_t,&SC_x,&SC_s); // Reset at the very begining, say in the constructor
    @endcode
    Later put the benchmarked code between macros, used as brackets
    @code{.cpp}
    SC_BENCHMARK_TIME_BEGIN(&SC_t,&SC_x);    // Begin the benchmarking here
    your code
    SC_BENCHMARK_TIME_END(&SC_t,&SC_x,&SC_s);   // End benchmarking here
    @endcode
    After using these macros, the benchmarked time value (since BEGIN) x is returned:
    and the sum of all benchmarked time (since RESET) in s (nanoseconds).
    Access the result as
    @code{.cpp}
    std::cerr << std::fixed << std::setfill (' ') << std::setprecision(d) << std::setw(w)
              << SC_x.to_seconds()*1000.;
    @endcode
    or, itcomfortably
    can be accessed as
    @code{.cpp}
    sc_time_String_Get(SC_x, SC_TIME_UNIT_DEFAULT)
    @endcode
In an object, you can RESET in the constructor,
    in the member functions between BEGIN and END measure the
    one-time utilization, and in the destructor to read out the total benchmarked time.
Or to benchmark (in multiple variables) SIMULATED execution time about critical sections of your code
 */
/*  @author János Végh (jvegh)
 *  @bug No known bugs.
 */

/*!
  \def SC_BENCHMARK_TIME_END(t,x,s)
  Changes the user-provided  \a t timepoint.
  Sets the user-provided \a x to the duration since the last TIME_BEGIN \a t value.
  Sets the user-provided \a s to the duration since the last TIME_RESET \a t value.
  Multiple uses can deliver multiple results since the same SC_ENCHMARK_TIME_BEGIN
  ('t' is a don't care, BUT DON'T CHANGE, value).
  // Variables must be passed by reference
*/

/*!
  \def SC_BENCHMARK_TIME_BEGIN(t,x)
  Changes the user-provided  \a t timepoint
  Sets the user-provided \a x to the last TIME_BEGIN \a t value
*/

/*!
  \def SC_BENCHMARK_TIME_RESET(t,x,s)
  Clears the user-provided \a x and \a s duration values.
  Sets the user-provided \a x to the duration since the last TIME_BEGIN \a t value.
  Sets the user-provided \a s to the duration since the last TIME_RESET \a t value.
  @param t for internal use, the absolute beginning of time (do not touch)
  @param x deliver clock duration since TIME_BEGIN (only read)
  @param s deliver clock duration since TIME_RESET (only read)
*/

#ifdef SC_MAKE_TIME_BENCHMARKING
    #define SC_BENCHMARK_TIME_ACTIVE   // Time benchmarking required
//#include <systemc>
#define SC_BENCHMARK_TIME_END(t,x,s)\
*x = sc_time_stamp() - *t; \
    *t = sc_time_stamp(); *s += *x;

#define SC_BENCHMARK_TIME_RESET(t,x,s)\
SC_BENCHMARK_TIME_BEGIN(t,x); *s=*x;

#define SC_BENCHMARK_TIME_BEGIN(t,x)\
    *t = sc_time_stamp(); *x=sc_time(SC_ZERO_TIME);
#else // The time measurement not needed, do nothing
// The macros with empty functionality will be optimized out by the compiler
    #define SC_BENCHMARK_TIME_RESET(t,x,s)
    #define SC_BENCHMARK_TIME_BEGIN(x,t)
    #define SC_BENCHMARK_TIME_END(t,x,s)
    #undef SC_BENCHMARK_TIME_ACTIVE   // Time benchmarking required
#endif //SC_MAKE_TIME_BENCHMARKING
//#undef SC_BENCHMARK_TIME_ACTIVE

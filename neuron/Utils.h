/*! @file Utils.h
 *  @ingroup GENCOMP_MODULE_STUFF
 *  @brief Utility function prototypes for the cooperating electronic modules.
 *
 *  This contains the prototypes for some useful stuff for the simulator, utilities.
 */
/*  @author János Végh (jvegh)
 *  @bug No known bugs.
 */

#ifndef GENCOMP_UTILS_H
#define GENCOMP_UTILS_H
#include <systemc>
//#include <QString>
#include <iostream>     // std::cout, std::fixed
#include <iomanip>      // std::setprecision
//#define DEBUG_DISABLED  // Disable debugging macros for all files
using namespace std;
/** @addtogroup GENCOMP_MODULE_STUFF
 *  @{
 */
#if 0
    int
MaskToID(SC_GRIDPOINT_MASK_TYPE Mask); ///< Converts core mask to its sequence number
/**
 *  @brief Converts a core ID to the corresponding mask.
 *  It is assumed that ID is in the range 0 to the number of bits in integer
 *  If Mask > sizeof(int), Mask = 0
 */
SC_GRIDPOINT_MASK_TYPE IDtoMask(int ID);

  SC_GRIDPOINT_MASK_TYPE
MaskOfLength(int Length =GRIDPOINT_MASK_WIDTH); ///< Produces a mask of length Length

// Count ones in 'Mask', limited to 'Length' bits
// Return -1 if out of range or not found
    int
OnesInMask_Get(SC_GRIDPOINT_MASK_TYPE ClearMask=-1, int Length =GRIDPOINT_MASK_WIDTH);

// Get position of the first 1 (from right) in word Mask
    int
PositionOfFirstOne_Get(SC_GRIDPOINT_MASK_TYPE Mask=-1, int Length =GRIDPOINT_MASK_WIDTH);

    int
PositionOfFirstZero_Get(SC_GRIDPOINT_MASK_TYPE Mask=-1, int Length =GRIDPOINT_MASK_WIDTH);

// Return a character corresponding to the 1s in the children mask
    char
AssembleQTIDSubcode(SC_GRIDPOINT_MASK_TYPE ChildMask);

    string
IDtoString(int ID, int BitWidth=8);

// Return positive modulo even for negative x
int moduloN(int x,int N);

    int
YFromPosition_Get(int x, int y);
#endif
    // Define sc_time unit

#define SC_TIME_UNIT_PS 0
#define SC_TIME_UNIT_NS 1
#define SC_TIME_UNIT_US 2
#define SC_TIME_UNIT_MS 3
#define SC_TIME_UNIT_S  4
#define SC_TIME_UNIT_DEFAULT SC_TIME_UNIT_MS

    /**
     * @brief Return the string corresponding to sc_time T
     * @param T the SystemC time to convert
     * @param U the requested unit, see SC_TIME_UNIT_xx
     * @param d decimals (after period)
     * @param w width (field total width
     * @return the requested string
     */
    string
sc_time_String_Get( sc_core::sc_time T, int32_t U=SC_TIME_UNIT_DEFAULT, const int32_t d = 3, const int32_t w=7);

/*

int sb_fprintf(FILE *fp, const char *fmt, ...) ;
*/
    /** @}*/

#endif // GENCOMP_UTILS_H

/** @file Utils.cpp
 *  @ingroup GENCOMP_MODULE_STUFF
 *  @brief Utility function prototypes for inter-gridpoint simulator
 */
/*  @author János Végh (jvegh)
 *  @bug No known bugs.
 */

#include "Utils.h"
string SC_TIME_UNIT[] {"ps","ns","us","ms","s"};

#if 0
/**
 *  @brief  Converts core mask to its sequence number
 *  It is assumed that only one bit of mask is set
 *  Mask=0 results in MaxInt, Mask = -1 results in 31
 * @param[in] Mask a one-hot bitmask
 */
  int
MaskToID(SC_GRIDPOINT_MASK_TYPE Mask)
{
    unsigned int ID=(unsigned int)-1;
    while (Mask)	// Find out the position of the 'one hot' bit
        {  ++ID ;  Mask /= 2; }
    return ID;
}

  SC_GRIDPOINT_MASK_TYPE
IDtoMask(int ID)
{
    if(ID >= MAX_GRIDPOINTS) return 0;
    SC_GRIDPOINT_MASK_TYPE Mask = 1;
    while(ID)
        {   Mask += Mask; --ID; }
    return Mask;
}

  SC_GRIDPOINT_MASK_TYPE
MaskOfLength(const int Length)
{
  SC_GRIDPOINT_MASK_TYPE Mask = 0;
  for(int i=0; i<Length; i++)
    Mask += Mask+1;
  return Mask;
}

  /**
   * @brief Count ones in 'ClearMask', limited to 'Length' bits
   * @param[in] Length the length of the word to consider (low bits)
   * @param[in] ClearMask the mask to be cleared
   * @return No of ones in the word
   */
  int
OnesInMask_Get(const SC_GRIDPOINT_MASK_TYPE ClearMask, const int Length)
{
  // This corresponds to the total physically available cores
  SC_GRIDPOINT_MASK_TYPE Mask = MaskOfLength(Length) & ClearMask; // Now the clear mask bits are masked out
  int No = 0;
  while (Mask)
  {
    if(Mask & 1) No ++;
      Mask /=2;
  }
  return No;
}

  /**
   * @brief Get position of the first 1 (from right) in word Mask
   * @param[in] Length
   * @param[in] Mask The mask value to check
   * @return The position of the 1st '1' (from right) in the word
   */
  int
PositionOfFirstOne_Get(SC_GRIDPOINT_MASK_TYPE Mask, const int Length)
{
  if(Length<1 || Length>=MAX_GRIDPOINTS) return -1;
  Mask = MaskOfLength(Length) & Mask; // Now the clear mask bits are masked out
  int No = -1;
  while (Mask)
  {
     No ++;
     if(Mask & 1) break;
     Mask /=2;
  }
  return Length < No ?  -1 : No;
}

    /**
     * @brief Get position of the first 0 (from right) in word Mask
     * @param[in] Mask The mask value to check
     * @param[in] Length The length of the mask to check (from right)
     * @return The position of the 1st '0' (from right) in the word
     * Return -1 if out of range or not found
     */
  int
PositionOfFirstZero_Get(SC_GRIDPOINT_MASK_TYPE Mask, const int Length)
{
  if(Length<1 || Length>(int)sizeof(int)*8) return -1;
   Mask = (MaskOfLength(Length) & ~Mask); // Now the clear mask bits are masked out
      int No = -1;
      while (Mask)
      {
        No ++;
        if(Mask & 1) break;
        Mask /=2;
      }
      return Length < No ? -1 : No;
}

  // A utility function for QT_AssembleID
  // Reasonable only for 62 processors!!! (but the allowed max is 60)
    char
  AssembleQTIDSubcode(const SC_GRIDPOINT_MASK_TYPE ChildMask)
  {
//    char Plus = '0' + OnesInMask_Get(ChildMask,32);
    char Plus = '0' + ChildMask;
    if(Plus>'9') Plus += ('a'-'9')-1; // The subcodes are 0..9a...zA...Z
    if(Plus>'z') Plus += ('A'-'z')-1;
    return Plus;
  }
  // Convert an sc_module sequence ID to a string
    string
  IDtoString(const int ID, const int BitWidth)
  {

        ostringstream oss;
        oss << hex << setw((BitWidth+3)/4) << setfill('0') << dec << ID;
        return oss.str();
   }
   #endif
/* Convert the simulated time to string
 *  @param[in] U The unit of the returned number; by default SC_TIME_US
 *  @param[in] T the sc_time; by sefault sc_time_stamp()
 *  @param[in] d decimals; by default 2
 *  @param[in] w field width; by default
 */
   string sc_time_String_Get(sc_core::sc_time T, int32_t U, const int32_t d, const int32_t w)
   {   switch(U)
        {
        case SC_TIME_UNIT_PS: U = T.value(); break;
        case SC_TIME_UNIT_NS: U = 1000*1000*1000; break;
        case SC_TIME_UNIT_US: U = 1000*1000; break;
        case SC_TIME_UNIT_MS: U = 1000; break;
        case SC_TIME_UNIT_S : U = 1; break;
        default: U=-1;
        };
 //       if(T == sc_core::SC_ZERO_TIME) T = sc_core::sc_time_stamp(); // Allow to print also zero time
        ostringstream oss;
        oss << std::fixed << std::setprecision(d) << std::setw(w) << T.to_seconds()*U;
        return oss.str();
}

#if 0
 // Convert simulated time to nsecs
string sc_time_to_nsec_Get(sc_core::sc_time T, const int d, const int w)
{
    if(T == sc_core::SC_ZERO_TIME) T = sc_core::sc_time_stamp();
    ostringstream oss;
    oss << std::fixed << std::setprecision(d) << std::setw(w) << T.to_seconds()*1000.*1000*1000;
    return oss.str();
}


// Convert simulated time to nsecs
string sc_time_to_usec_Get(sc_core::sc_time T, const int d, const int w)
{
   if(T == sc_core::SC_ZERO_TIME) T = sc_core::sc_time_stamp();
   ostringstream oss;
   oss << std::fixed << std::setprecision(d) << std::setw(w) << T.to_seconds()*1000.*1000;
   return oss.str();
}

 // Convert simulated time to msecs
string sc_time_to_msec_Get(sc_core::sc_time T, const int d, const int w)
{
    if(T == sc_core::SC_ZERO_TIME) T = sc_core::sc_time_stamp();
    ostringstream oss;
    oss << std::fixed << std::setfill (' ') << std::setprecision(d) << std::setw(w) << T.to_seconds()*1000.;
    return oss.str();
}
#endif //0

// Return positive modulo even for negative x
int moduloN(int x,int N){
    return (x % N + N) %N;
}
// Return gridpoint Y coordinate from topology position
    int
YFromPosition_Get(int x, int y){ return (y-moduloN(x,2))/2;}

#include <stdio.h>
#include <stdarg.h>
#include <errno.h>

int sb_fprintf(FILE *fp, const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  int ret = 0;
  do {
    errno = 0;
    ret = vfprintf(fp, fmt, ap);
  } while (errno == EINTR);
  return ret;
}

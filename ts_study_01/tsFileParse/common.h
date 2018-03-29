#include "amcomdef.h"
inline MUInt16 to_UInt16(MByte* p);
inline MUInt8 to_UInt8(MByte* p);

MUInt8 get8(MByte*& p);
MUInt16 get16(MByte*& p);

#define FFMIN(a,b) ((a) > (b) ? (b) : (a))

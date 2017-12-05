#include "stdafx.h"
#include "common.h"

inline MUInt16 to_UInt16(MByte* p)
{
	MUInt16 n = ((MByte*)p)[0];
	n <<= 8;
	n += ((MByte*)p)[1]; return n;
}
inline MUInt8 to_UInt8(MByte* p)
{
	return *((MByte*)p);
}
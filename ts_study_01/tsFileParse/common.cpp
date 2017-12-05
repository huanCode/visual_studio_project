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

MUInt8 get8(MByte*& p)
{
	MByte* tmp = p;
	p++;
	return to_UInt8(tmp);

}

MUInt16 get16(MByte*& p)
{
	MByte* tmp = p;
	p += 2;
	return to_UInt16(tmp);
}

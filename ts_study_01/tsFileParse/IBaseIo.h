#pragma once
#ifndef _IBASEIO_H
#define _IBASEIO_H
#include "amcomdef.h"
class IBaseIo
{
public:
	IBaseIo(MPChar strType)
	{
		m_strType = strType;
	}

	virtual MBool Open(MPChar strUrl) = 0;
	virtual MInt32 IoRead(MPChar pBuf, MDWord dwSize, MInt64 llOffset = 0) = 0;
	virtual MVoid	IoClose() = 0;
private:
	MPChar	m_strType;


};



#endif // !_PARSEHLS_H#pragma once
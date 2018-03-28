#pragma once
#ifndef _HTTPIO_H
#define _HTTPIO_H
#include "amcomdef.h"
#include "IBaseIo.h"
class HttpIo:public IBaseIo
{
public:
	HttpIo();

	MBool Open(MPChar strUrl);
	MInt32 IoRead(MPChar pBuf, MDWord dwSize, MInt64 llOffset = 0);
	MVoid IoClose();
private:
	
	MHandle m_hHttp;

};




#endif // !HttpIo
#include "stdafx.h"
#include "HttpIo.h"
#include "mhttp.h"
HttpIo::HttpIo():
	IBaseIo("http")
{
	m_hHttp = MNull;
}


MBool HttpIo::Open(MPChar strUrl)
{
	MHandle m_hHttp = Http_Open(strUrl, HTTP_POST, 0);
	if (m_hHttp)
	{
		MInt32 ret = Http_Connect(m_hHttp);
		if (ret == 0)
		{
			return MTrue;
		}
	}

	return MFalse;
}

MInt32 HttpIo::IoRead(MPChar pBuf, MDWord dwSize, MInt64 llOffset)
{
	MInt32 lReadSize = 0;

	lReadSize = Http_Read(m_hHttp, (MByte*)pBuf, dwSize);
	return lReadSize;
}

MVoid HttpIo::IoClose()
{
	if (m_hHttp)
	{
		//Http_Close(m_hHttp);
		m_hHttp = MNull;
	}
}
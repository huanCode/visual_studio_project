#pragma once
#ifndef _SOURCEFRAME_H
#define _SOURCEFRAME_H
#include "amcomdef.h"
#include "ToolList.h"
#include "ToolBuffer.h"

enum IBaseIoType
{
	none,
	Http,
	File,
	Rtmp
};


class IBaseIo;
class SourceFrame
{
public:

	typedef MBool(*read_probe)(MPChar p_buffer, MUInt32 p_size);
	SourceFrame();
	
	MBool Open(MPChar strUrl);
	MInt32 IoRead(MByte* pBuf, MDWord dwSize);
	MInt32 IoReadLine(MChar** ppBuffer);
	MBool isFinish(){return m_isFinish;};
private:
	MBool createBaseIo(MPChar strUrl);
	IBaseIoType parseUrl(MPChar strUrl);
private:
	IBaseIoType		m_baseIoType;
	IBaseIo*		m_baseIo;
	ToolList<IBaseIo*>	m_baseIoList;
	ToolBuffer		m_buffer;
	MBool			m_isFinish;

	ToolList<read_probe>	m_probe;
};




#endif // !_SOURCEFRAME_H

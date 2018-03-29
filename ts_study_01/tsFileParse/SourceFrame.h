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


	SourceFrame();
	
	MBool Open(MPChar strUrl);
	MBool IoRead(MChar**, MDWord dwSize,MInt32& out_readSize);
	MInt32 IoReadLine(MChar** ppBuffer);
	MBool isFinish(){return m_isFinish;};
private:
	MBool createBaseIo(MPChar strUrl);
	IBaseIoType parseUrl(MPChar strUrl);
	MBool findParse(MPChar buffer,MInt32 bufSize);

	//IoBase读取的数据填满buffer
	MBool fillBuffer();
private:
	IBaseIoType		m_baseIoType;
	IBaseIo*		m_baseIo;
	ToolList<IBaseIo*>	m_baseIoList;
	ToolBuffer		m_buffer;
	MBool			m_isFinish;


};




#endif // !_SOURCEFRAME_H

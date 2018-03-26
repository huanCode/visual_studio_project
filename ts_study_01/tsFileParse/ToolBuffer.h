#pragma once
#ifndef _TOOLBUFFER_H
#define _TOOLBUFFER_H
#include "amcomdef.h"
#define BUFFER_SIZE (1024*32)
class ToolBuffer
{
public:
	ToolBuffer(MInt32 iBufferSize = BUFFER_SIZE);
	MBool Create(MInt32 iBufferSize);
	MBool Create();
	MPChar GetBuffer();
	MVoid Close();
	MInt32 GetWriteSize();
	MInt32 GetReadSize();
private:
	MVoid   SetBufferSize(MInt32 iBufferSize);
private:
	MPChar	m_bufferBegin;
	MPChar  m_curPos;
	MPChar	m_bufferEnd;
	MInt32	m_bufferSize;


};



#endif // !_TOOLBUFFER_H
#include "ToolBuffer.h"
#include "ammem.h"


ToolBuffer::ToolBuffer(MInt32 iBufferSize /*= BUFFER_SIZE*/)
{
	m_bufferBegin = MNull;
	SetBufferSize(iBufferSize);
	m_curPos = MNull;
}

MBool ToolBuffer::Create(MInt32 iBufferSize)
{
	if (m_bufferBegin)
	{
		return MTrue;
	}

	m_bufferBegin = (MPChar)MMemAlloc(MNull, iBufferSize);
	if (m_bufferBegin)
	{
		m_curPos = m_bufferBegin;
		m_bufferEnd = m_bufferEnd + iBufferSize;
		return MTrue;
	}
	
	return MFalse;
}

MBool ToolBuffer::Create()
{
	return Create(BUFFER_SIZE);
}

MPChar ToolBuffer::GetBuffer()
{
	return m_curPos;
}

MVoid ToolBuffer::Close()
{
	if (m_bufferBegin)
	{
		MMemFree(MNull, m_bufferBegin);
		m_bufferBegin = MNull;
		m_curPos = MNull;
	}
}

MInt32 ToolBuffer::GetWriteSize()
{
	if (m_bufferBegin)
	{
		return m_bufferEnd - m_curPos;
	}

	return 0;
}

MInt32 ToolBuffer::GetReadSize()
{
	if (m_bufferBegin)
	{
		return m_curPos - m_bufferBegin;
	}

	return 0;
}

MVoid ToolBuffer::SetBufferSize(MInt32 iBufferSize)
{
	if (iBufferSize > 0)
	{
		m_bufferSize = iBufferSize;
	}
}

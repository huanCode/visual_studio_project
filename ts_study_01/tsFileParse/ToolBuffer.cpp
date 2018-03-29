#include "stdafx.h"
#include "ToolBuffer.h"
#include "ammem.h"
#include "common.h"

ToolBuffer::ToolBuffer(MInt32 iBufferSize /*= BUFFER_SIZE*/)
{
	m_bufferBegin = MNull;
	SetBufferSize(iBufferSize);
	m_curPos = MNull;
	m_curWritePos = MNull;
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
		m_curWritePos = m_bufferBegin;
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




MVoid ToolBuffer::ReadSize(MInt32 size)
{
	if (m_bufferBegin &&(m_curPos - m_bufferBegin >= size) )
	{
		m_curPos -= size;
	}

}


MBool ToolBuffer::Read(MChar** pBuf,MDWord dwSize)
{
	MPChar result = *pBuf;
	if (dwSize <= (m_bufferEnd - m_bufferBegin))
	{
		if (dwSize <= m_bufferEnd - m_curPos)
		{
			MMemCpy(result, m_curPos, dwSize);
			m_curPos += dwSize;
			return MTrue;
		}
	} 
	else
	{

		//»º³åÇøÌ«Ð¡
		//m_bufferBegin = (MPChar)MMemRealloc(MNull, (MVoid*)m_bufferBegin, dwSize);
		//m_bufferEnd = m_bufferBegin + dwSize;
		
	}

	return MFalse;
}


MVoid ToolBuffer::Free()
{
	if (m_bufferBegin)
	{
		MMemFree(MNull, m_bufferBegin);
		m_bufferBegin = MNull;
		m_curPos = MNull;
	}
}


MVoid ToolBuffer::Reset()
{
	m_curPos = m_bufferBegin;
}

MInt32 ToolBuffer::GetBufferSize()
{
	return m_bufferSize;
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
		return m_bufferEnd - m_curPos;
	}

	return 0;
}

MVoid ToolBuffer::WriteSize(MInt32 size)
{
	MInt32 canWriteSize = m_bufferEnd - m_curPos;
	canWriteSize = FFMIN(canWriteSize, size);
	m_curPos += canWriteSize; 
}

MVoid ToolBuffer::SetBufferSize(MInt32 iBufferSize)
{
	if (iBufferSize > 0)
	{
		m_bufferSize = iBufferSize;
	}
}

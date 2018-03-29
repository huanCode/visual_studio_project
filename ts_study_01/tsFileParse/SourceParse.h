#pragma once
#ifndef _SOURCEPARSE_H
#define _SOURCEPARSE_H
#include "amcomdef.h"
#include "SourceFrame.h"
#include "ParseFrame.h"
class SourceParse
{
public:
	SourceParse();
	MBool Open(MPChar strUrl);
	MBool ReadFrame();


private:
	SourceFrame	m_sourceFrame;
	ParseFrame	m_parseFrame;
	MPChar		m_pBuffer;
	MInt32		m_iBufferCanReadSize;

};




#endif // !_SOURCEPARSE_H
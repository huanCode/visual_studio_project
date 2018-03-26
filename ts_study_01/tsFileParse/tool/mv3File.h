#pragma once
#ifndef _MV3FILE_H
#define _MV3FILE_H
#include "amcomdef.h"
#include <stdio.h>
class MV3StringExt;
class mv3File
{
public:
	mv3File();
	~mv3File();
	enum FileRight
	{
		stream_read,
		stream_write,
		stream_append,
		stream_rw,
		stream_wr,
	};


	/*	isExist
	*
	*	Description:
	*		�ж��ļ��Ƿ����
	*	Parameter:
	*		strFilePath:MV3_COMMON_PLATFORM_IOS��MV3_COMMON_PLATFORM_ANDROID
	*	Return:
	*		true: success
	*		false:fail
	*
	*/
	static MBool isNoExist(MPChar strFilePath);
	/*	WriteFile
	*
	*	Description:
	*		�ַ���д��ָ���ļ�,ÿ��д�붼���´򿪺͹ر��ļ�;
	*	Parameter:
	*		strFilePath:�ļ�·��
	*		strLength:�ַ�������
	*		str:�ַ���
	*	Return:
	*		true: success
	*		false:fail
	*
	*/

	static MBool WriteFile(MPChar strFilePath, MInt32 strLength, MPChar str);

	virtual MBool Open(MPChar strFilePath, FileRight fileRight);
	virtual MVoid Close();
	virtual MBool Read(MByte* buffer,MInt32 bufferSize,MInt32 &out_readByte);
	virtual MBool Write(MByte* buffer, MInt32 bufferSize);
	MInt32 GetFileSize();
	//MBool  GetFileSize(MV3StringExt& strFileSize);

	MInt32 GetFileCurPos();
	MBool  SetFileBeginPos();
	MBool  SetFileEndPos();
	MBool  SetFilePos(MInt32 iPos);
	MVoid  Flush() {
		if (m_pFile)
		{
			fflush(m_pFile);
		}
		
	}
private:
	MBool  SetFilePos(MInt32 mode, MInt32 iPos);
private:
	FILE*	m_pFile;
};



#endif // !_MV3FILE_H


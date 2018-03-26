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
	*		判断文件是否存在
	*	Parameter:
	*		strFilePath:MV3_COMMON_PLATFORM_IOS、MV3_COMMON_PLATFORM_ANDROID
	*	Return:
	*		true: success
	*		false:fail
	*
	*/
	static MBool isNoExist(MPChar strFilePath);
	/*	WriteFile
	*
	*	Description:
	*		字符串写入指定文件,每次写入都重新打开和关闭文件;
	*	Parameter:
	*		strFilePath:文件路径
	*		strLength:字符串长度
	*		str:字符串
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


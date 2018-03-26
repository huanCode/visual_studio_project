#include "stdafx.h"
#include "mv3File.h"
//#include "mv3StringExt.h"
//#include "MV2Trace.h"

#ifndef __IPHONE__
#define MVLOGD
#endif

mv3File::mv3File()
	:m_pFile(MNull)
{

}

mv3File::~mv3File()
{
	Close();
}

MBool mv3File::isNoExist(MPChar strFilePath)
{
	MBool ret = MTrue;
	FILE *pFile = fopen(strFilePath, "rb");

	if (pFile)
	{
		ret = MFalse;
		fclose(pFile);
	}
	return ret;
}


MBool mv3File::WriteFile(MPChar strFilePath, MInt32 strLength, MPChar str)
{
	MBool ret = MFalse;
	FILE *pFile = fopen(strFilePath, "wb");

	if (pFile)
	{
		MInt32 retSize = (MInt32)fwrite(str, 1, strLength, pFile);
		if (retSize == strLength)
		{
			ret = MTrue;
		}
	}

	fclose(pFile);
	return MFalse;
}

MBool mv3File::Open(MPChar strFilePath, FileRight fileRight)
{
	if (strFilePath == MNull)
	{
		return MFalse;
	}
	//ab 可以在指定位置追加
	//rb+ 正常读取，在指定位置开始写入内容覆盖原来内容
	//wb+ 正常读取,存在文件，会清空内容
	MChar   rgMode[6][4] = { "rb", "wb", "ab", "rb+", "wb+", "ab+" };
	m_pFile = fopen(strFilePath, rgMode[fileRight]);
	if (m_pFile)
	{
		return MTrue;
	}
	return MFalse;
}

MVoid mv3File::Close()
{
	if (m_pFile)
	{
		fclose(m_pFile);
		m_pFile = MNull;
	}
	
}

MBool mv3File::Read(MByte* buffer, MInt32 bufferSize,MInt32 &out_readByte)
{
	out_readByte = 0;
	if (m_pFile == MNull || buffer == MNull || bufferSize == 0)
	{
		return MFalse;
	}

	MInt32 trueReadByteSize = (MInt32)fread(buffer, 1, bufferSize, m_pFile);
	if (trueReadByteSize > bufferSize || trueReadByteSize == 0)
	{
		//真实读取的字节数大于要求时，肯定是错的
		return MFalse;
	}
	out_readByte = trueReadByteSize;
	return MTrue;
}


MBool mv3File::Write(MByte* buffer, MInt32 bufferSize)
{
	if (m_pFile == MNull || buffer == MNull || bufferSize == 0)
	{
		MVLOGD("[upload] mv3File::Write,parm is null \r\n");
		return MFalse;
	}

	MInt32 retSize = (MInt32)fwrite(buffer, 1, bufferSize, m_pFile);
	if (retSize == bufferSize)
	{
		MVLOGD("[upload] mv3File::Write -- ok ,retSiz = %d,bufferSize = %d \r\n",retSize,bufferSize);
		return MTrue;
	}

	MVLOGD("[upload] mv3File::Write -- fail,retSiz = %d,bufferSize = %d \r\n",retSize,bufferSize);
	return MFalse;
}

MInt32 mv3File::GetFileSize()
{
	MInt32 iFileCurPos = GetFileCurPos();
	if (iFileCurPos == -1)
	{
		return -1;
	}

	SetFileEndPos();

	MInt32 iFileSize = GetFileCurPos();
	
	SetFilePos(iFileCurPos);

	return iFileSize;
}


//MBool mv3File::GetFileSize(MV3StringExt& strFileSize)
//{
//	MInt32 iFileSize = mv3File::GetFileSize();
//	return strFileSize.InitDigit(iFileSize);
//}


MInt32 mv3File::GetFileCurPos()
{
	if (m_pFile == MNull)
	{
		return -1;
	}

	return ftell(m_pFile);
}



MBool mv3File::SetFilePos(MInt32 mode,MInt32 iPos)
{
	if (m_pFile == MNull)
	{
		return -1;
	}
	MInt32 ret = fseek(m_pFile, iPos, mode);
	if (ret == 0)
	{
		return MTrue;
	}
	return MFalse;
}

MBool mv3File::SetFileBeginPos()
{
	return SetFilePos(SEEK_SET,0);
}

MBool mv3File::SetFileEndPos()
{
	return SetFilePos(SEEK_END,0);
}

MBool mv3File::SetFilePos(MInt32 iPos)
{
	return SetFilePos(SEEK_SET, iPos);
}

#include "SourceFrame.h"
#include "HttpIo.h"
#include "amstring.h"
#include "ToolString.h"
#include "ParseHls.h"

#include <string.h>
#define URL_SCHEME_CHARS                        \
    "abcdefghijklmnopqrstuvwxyz"                \
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"                \
    "0123456789+-."

#define FFMIN(a,b) ((a) > (b) ? (b) : (a))
static inline int is_dos_path(const char *path)
{
#if HAVE_DOS_PATHS
	if (path[0] && path[1] == ':')
		return 1;
#endif
	return 0;
}


SourceFrame::SourceFrame()
{
	m_baseIoType = none;
	m_isFinish = MFalse;

	m_probe.AddNode(ParseHls::hls_probe);


}


MBool SourceFrame::Open(MPChar strUrl)
{
	if (strUrl == MNull)
	{
		return MFalse;
	}

	if (!createBaseIo(strUrl))
	{
		return MFalse;
	}

	if (!m_buffer.Create())
	{
		return MFalse;
	}

	return m_baseIo->Open(strUrl);
}



MInt32 SourceFrame::IoRead(MByte* pBuf, MDWord dwSize)
{
	if (pBuf == MNull || dwSize <=0)
	{
		return 0;
	}

	MInt32 readByteSize = 0;
	return 0;
}


MInt32 SourceFrame::IoReadLine(MChar** ppBuffer)
{
	if (*ppBuffer == MNull)
	{
		return 0;
	}


}

IBaseIoType SourceFrame::parseUrl(MPChar strUrl)
{

	char proto_str[128], proto_nested[128], *ptr;
	size_t proto_len = strspn(strUrl, URL_SCHEME_CHARS);
	int i;

	if (strUrl[proto_len] != ':' &&
		(strncmp(strUrl, "subfile,", 8) || !strchr(strUrl + proto_len + 1, ':')) ||
		is_dos_path(strUrl))
		strcpy(proto_str, "file");
	else
		ToolString::av_strlcpy(proto_str, strUrl,
			FFMIN(proto_len + 1, sizeof(proto_str)));

	if ((ptr = strchr(proto_str, ',')))
		*ptr = '\0';
	ToolString::av_strlcpy(proto_nested, proto_str, sizeof(proto_nested));
	if ((ptr = strchr(proto_nested, '+')))
		*ptr = '\0';

	IBaseIoType type = IBaseIoType::none;
	if (ptr)
	{
		if (!MStrCmp(ptr, "http"))
		{
			type = IBaseIoType::Http;
		}
		else if (!MStrCmp(ptr, "file"))
		{
			type = IBaseIoType::File;
		}
		else if (!MStrCmp(ptr, "rtmp"))
		{
			type = IBaseIoType::Rtmp;
		}
	}

	return type;

}


MBool SourceFrame::createBaseIo(MPChar strUrl)
{
	IBaseIoType type = IBaseIoType::none;

	if (type == IBaseIoType::Http)
	{
		m_baseIo = new HttpIo();
	}

	return m_baseIo ? MTrue : MFalse;
}
#pragma once
#ifndef _TOOLSTRING_H
#define _TOOLSTRING_H
#include "amcomdef.h"
class ToolString
{
public:
	static MInt32 Read_line(MChar*& srcBuffer, MInt32 iSrcLen, MPChar dstBuffer,MInt32 idstMaxLen);
	//static MInt32 av_strstart(MPChar str, MPChar pfx, MChar **ptr)
	//{
	//	while (*pfx && *pfx == *str) {
	//		pfx++;
	//		str++;
	//	}
	//	if (!*pfx && ptr)
	//		*ptr = str;
	//	return !*pfx;
	//}
	static MInt32 av_strlcpy(char *dst, const char *src, size_t size);
	static MInt32 av_strstart(MPChar str, MPChar pfx, MChar** ptr);
	static MInt32 av_strlcat(char *dst, const char *src, size_t size);

	//功能:根据参数s，重新创建一个大小刚好的字符串指针,然后返回
	static MPChar av_strdup(MPChar s);

	static void ff_make_absolute_url(MPChar buf, MInt32 size, MPChar base, MPChar rel);


	typedef void (*ff_parse_key_val_cb)(void* srcData, MPChar key, MInt32 keyLen, MPChar value);
	static void ff_parse_key_value(MPChar str, ff_parse_key_val_cb callback_get_buf, void *context);
private:
	static inline  MInt32 av_isspace(MInt32 c)
	{
		return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' ||
			c == '\v';
	}
};




#endif // !_TOOLSTRING_H
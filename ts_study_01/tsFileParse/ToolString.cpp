#include "stdafx.h"
#include "ToolString.h"
#include "amstring.h"
#include "ammem.h"
MInt32 ToolString::Read_line(MChar*& srcBuffer, MInt32 iSrcLen, MPChar dstBuffer, MInt32 idstMaxLen)
{

	int i = 0;
	char c;

	//MPChar tmpBuffer = MNull;
	//if (*srcBuffer == '\n' || *srcBuffer == '\r')
	//{
	//	tmpBuffer = srcBuffer + 2;
	//}
	//else
	//{
	//	tmpBuffer = srcBuffer;
	//}

	



	MInt32 len = iSrcLen >= idstMaxLen ? iSrcLen : idstMaxLen;

	do {
		c = *srcBuffer;
		srcBuffer++;
		if (c && i < len - 1) {
			dstBuffer[i] = c;
			i++;
		}
			
	} while (c != '\n' && c != '\r' && c);


	dstBuffer[i] = 0;

	while (i > 0 && av_isspace(dstBuffer[i - 1]))
		dstBuffer[--i] = '\0';

	while (*srcBuffer == '\n' || *srcBuffer == '\r')
	{
		srcBuffer++;
	}

	return i;
}


void ToolString::ff_parse_key_value(MPChar str, ff_parse_key_val_cb callback_get_buf,void *context)
{
	MPChar ptr = str;
	MChar tmpBuffer[256] = {0};
	/* Parse key=value pairs. */
	for (;;) {
		MPChar key = MNull;
		MPChar dest = tmpBuffer, dest_end = tmpBuffer + 256 -1;
		int key_len, dest_len = 0;

		/* Skip whitespace and potential commas. */
		while (*ptr && (ToolString::av_isspace(*ptr) || *ptr == ','))
			ptr++;
		if (!*ptr)
			break;

		key = ptr;

		if (!(ptr = MStrChr(key, '=')))
			break;
		ptr++;
		key_len = ptr - key;

		//callback_get_buf作用,用dest保存context里的bandwidth地址,然后在下面把值赋值给它
		//callback_get_buf(context, key, key_len, &dest, &dest_len);
		//dest_end = dest + dest_len - 1;
		

		if (*ptr == '\"') {
			//解析出【 bandwidth="200" 】这样模式的值
			ptr++;
			while (*ptr && *ptr != '\"') {
				if (*ptr == '\\') {
					if (!ptr[1])
						break;
					if (dest && dest < dest_end)
						*dest++ = ptr[1];
					ptr += 2;
				}
				else {
					if (dest && dest < dest_end)
						*dest++ = *ptr;
					ptr++;
				}
			}
			if (*ptr == '\"')
				ptr++;
		}
		else {
			//解析出【 bandwidth=200 】这样模式的值
			for (; *ptr && !(ToolString::av_isspace(*ptr) || *ptr == ','); ptr++)
				if (dest && dest < dest_end)
					*dest++ = *ptr;
		}
		if (dest)
 			*dest = 0;


		callback_get_buf(context,key,key_len, tmpBuffer);

	}
}


void ToolString::ff_make_absolute_url(MPChar buf, MInt32 size, MPChar base, MPChar rel)
{
	char *sep, *path_query;
	/* Absolute path, relative to the current server */
	if (base && MStrStr(base, "://") && rel[0] == '/') {
		if (base != buf)
			MStrNCpy(buf, base, size);
		sep = MStrStr(buf, "://");
		if (sep) {
			/* Take scheme from base url */
			if (rel[1] == '/') {
				sep[1] = '\0';
			}
			else {
				/* Take scheme and host from base url */
				sep += 3;
				sep = MStrChr(sep, '/');
				if (sep)
					*sep = '\0';
			}
		}
		ToolString::av_strlcat(buf, rel, size);
		return;
	}
	/* If rel actually is an absolute url, just copy it */
	if (!base || strstr(rel, "://") || rel[0] == '/') {
		ToolString::av_strlcpy(buf, rel, size);
		return;
	}
	if (base != buf)
		ToolString::av_strlcpy(buf, base, size);

	/* Strip off any query string from base */
	path_query = MStrChr(buf, '?');
	if (path_query)
		*path_query = '\0';

	/* Is relative path just a new query part? */
	if (rel[0] == '?') {
		ToolString::av_strlcat(buf, rel, size);
		return;
	}

	/* Remove the file name from the base url */
	sep = MStrRChr(buf, '/');
	if (sep)
		sep[1] = '\0';
	else
		buf[0] = '\0';
	while (ToolString::av_strstart(rel, "../", NULL) && sep) {
		/* Remove the path delimiter at the end */
		sep[0] = '\0';
		sep = MStrRChr(buf, '/');
		/* If the next directory name to pop off is "..", break here */
		if (!MStrCmp(sep ? &sep[1] : buf, "..")) {
			/* Readd the slash we just removed */
			ToolString::av_strlcat(buf, "/", size);
			break;
		}
		/* Cut off the directory name */
		if (sep)
			sep[1] = '\0';
		else
			buf[0] = '\0';
		rel += 3;
	}
	ToolString::av_strlcat(buf, rel, size);
}



MInt32 ToolString::av_strstart(MPChar str, MPChar pfx, MChar** ptr)
{
	while (*pfx && *pfx == *str) {
		pfx++;
		str++;
	}
	if (!*pfx && ptr)
		*ptr = str;
	return !*pfx;
}


MInt32 ToolString::av_strlcpy(char *dst, const char *src, size_t size)
{
	MInt32 len = 0;
	while (++len < size && *src)
		*dst++ = *src++;
	if (len <= size)
		*dst = 0;
	return len + MStrLen(src) - 1;
}

MInt32 ToolString::av_strlcat(char *dst, const char *src, size_t size)
{
	MInt32 len = MStrLen(dst);
	if (size <= len + 1)
		return len + MStrLen(src);
	return len + av_strlcpy(dst + len, src, size - len);
}


MPChar ToolString::av_strdup(MPChar s)
{
	MPChar ptr = NULL;
	if (s) {
		size_t len = MStrLen(s) + 1;
		ptr = (MPChar)MMemAlloc(NULL, len);
		if (ptr)
			MSCsNCpy(ptr, s, len);
	}
	return ptr;
}

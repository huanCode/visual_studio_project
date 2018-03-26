/*----------------------------------------------------------------------------------------------
*
* This file is ArcSoft's property. It contains ArcSoft's trade secret, proprietary and 		
* confidential information. 
* 
* The information and code contained in this file is only for authorized ArcSoft employees 
* to design, create, modify, or review.
* 
* DO NOT DISTRIBUTE, DO NOT DUPLICATE OR TRANSMIT IN ANY FORM WITHOUT PROPER AUTHORIZATION.
* 
* If you are not an intended recipient of this file, you must not copy, distribute, modify, 
* or take any action in reliance on it. 
* 
* If you have received this file in error, please immediately notify ArcSoft and 
* permanently delete the original and any copy of any file and any printout thereof.
*
*-------------------------------------------------------------------------------------------------*/
#ifndef __MHTTP_H__
#define __MHTTP_H__

#include "amcomdef.h"
#include "merror.h"
#include "amstream.h"

#define HTTP_GET		0
#define HTTP_POST		1
#define HTTP_GETPOST	2
#define HTTP_PUT        3

#define HTTPCFG_ISSTREAM					0x0010
#define HTTPCFG_RANGEPRESENTINZEROOFFSET	0x0011
#define HTTPCFG_CONTENTTYPE					0x0012
#define HTTPCFG_DLNALIMITSEEK				0x0013
#define HTTPCFG_STATUSCODE					0x0014
#define	HTTPCFG_NORANGE_MODE				0x0015	//do not send "Range" header ever.
#define	HTTPCFG_FLV_MODE					0x0016	//ignore the leading 13 bytes when seeking -- "FLV\1\1\0\0\0\x9\0\0\0\x9"
#define	HTTPCFG_REDIECTS					0x0017
#define	HTTPCFG_CONN_TIMEOUT				0x0018
#define	HTTPCFG_RECV_TIMEOUT				0x0019
#define	HTTPCFG_DNS_TIMEOUT					0x001A
#define	HTTPCFG_DNS_ERROR					0x001B
#define	HTTPCFG_SOCEKT_ERROR				0x001C
#define HTTPCFG_DNS_CACHE					0x001D
#define HTTPCFG_RETRY_COUNT					0x001E

//callback
typedef struct __tag_async_callback_data {
	// status 
	MDWord	dwStatus;
	MRESULT resLastErr;
	MInt64	llTotalSize;		//The total file size
	MInt64	llSegStart;			//The segment start postion
	MInt64	llSegEnd;			//The segment end position
	MInt64	llSegDownSize;		//The current segment donwload size
}ASYNCCBDATA, *LPASYNCCBDATA;

typedef MRESULT (*PFNASYNCCALLBACK) (LPASYNCCBDATA lpPlaybackData , MLong lUserData);

typedef enum __tag_http_type {
	COMMON = 0,
	DTCP,
	DLNA,
}HTYPE;

#ifdef __cplusplus
extern "C" {
#endif


MHandle Http_Open(const MChar* pszURL, MShort mode, MInt64 llSeekPos = 0);

MRESULT Http_Close(MHandle hHttp);

MRESULT	Http_SetType(MHandle hHttp, HTYPE type);

MRESULT Http_Connect(MHandle hHttp);

MLong	Http_Read(MHandle hHttp, MByte* pbuf, MLong len);

MLong	Http_Write(MHandle hHttp, MByte* pbuf, MLong len);

MRESULT Http_Seek(MHandle hHttp, MShort whence, MInt64 pos);

MInt64	Http_Size(MHandle hHttp);

MInt64	Http_Tell(MHandle hHttp);

MRESULT Http_SeekEx(MHandle hHttp, MInt64 start, MInt64 end);

MRESULT Http_SetLogLevel(MHandle hHttp, MDWord level);

MRESULT Http_SetHeaders(MHandle hHttp, MChar* szheaders);

MRESULT Http_SetBody(MHandle hHttp, MByte* pBody, MDWord dwBodyLen, MChar* pBodyType);

MRESULT Http_SetChunked(MHandle hHttp, MLong ischunked);

MRESULT	Http_GetConfig(MHandle hHttp, MDWord dwCfgType, MDWord* pdwValue);

MRESULT	Http_SetConfig(MHandle hHttp, MDWord dwCfgType, MDWord dwValue);

//just for test
MHandle Http_GetResHeader(MHandle hHttp, MDWord *len);

MRESULT	Http_GetSegRange(MHandle hHttp, MInt64* psegstart, MInt64* psegend);

MRESULT Http_SetInterrupt(MHandle hHttp, MBool binterrupt);

MRESULT Http_SetLocationUrl(MHandle hHttp, MChar* pUrl);

MRESULT	Http_GetLocationUrl(MHandle hHttp, MChar*& pUrl);

MRESULT Http_SetHosts(MHandle hHttp, MChar* pHosts);

MRESULT Http_GetHosts(MHandle hHttp, MChar*& pHosts);

MRESULT Http_GetHostIp(MHandle hHttp, MChar*& pIp);


#ifdef __cplusplus
}
#endif


#endif




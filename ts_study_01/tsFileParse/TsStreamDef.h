#ifndef _TSSTREAMDEF_H
#define _TSSTREAMDEF_H
#include "amcomdef.h"

/*PSI*/
#define PAT_PID                 0x0000
#define SDT_PID                 0x0011		//17
#define PMT_PID					0x1000		//4096
#define EMPTY_PID               0x1FFF

/* table ids */
#define PAT_TID   0x00
#define CAT_TID	  0x01
#define PMT_TID   0x02

#define MAX_SECTION_SIZE 4096
#define MAX_PES_PAYLOAD 200 * 1024
#define AV_INPUT_BUFFER_PADDING_SIZE 32
#define AV_RB16(x)  ((((const MUInt8*)(x))[0] << 8) | ((const MUInt8*)(x))[1])
#define AV_RL32(x)							\
	((((const MByte*)(x))[3] << 24)|	\
	(((const MByte*)(x))[2] << 16) |	\
	(((const MByte*)(x))[1] << 8)  |	\
	((const MByte*)(x))[0])


//enum MpegTSFilterType {
//	MPEGTS_PES,
//	MPEGTS_SECTION,
//	MPEGTS_PCR
//};

//typedef struct MpegTSSectionFilter {
//	int section_index;
//	int section_length;
//	int last_ver;
//	unsigned crc;
//	unsigned last_crc;
//	MByte *section_buf;
//	unsigned int check_crc : 1;
//	unsigned int end_of_section_reached : 1;
//	//SectionCallback *section_cb;
//	void *opaque;
//} MpegTSSectionFilter;
//
//typedef struct MpegTSPESFilter {
//	//PESCallback *pes_cb;
//	void *opaque;
//} MpegTSPESFilter;
//
//struct MpegTSFilter {
//	int pid;
//	int es_id;
//	int last_cc; /* last cc code (-1 if first packet) */
//	MInt64 last_pcr;
//	MpegTSFilterType type;
//	union {
//		MpegTSPESFilter pes_filter;
//		MpegTSSectionFilter section_filter;
//	} section_or_pes;
//};

enum AV_MediaType {
	AV_MEDIA_TYPE_UNKNOWN = -1,  ///< Usually treated as AVMEDIA_TYPE_DATA
	AV_MEDIA_TYPE_VIDEO,
	AV_MEDIA_TYPE_AUDIO,
	AV_MEDIA_TYPE_DATA,          ///< Opaque data information usually continuous
	AV_MEDIA_TYPE_SUBTITLE,
	AV_MEDIA_TYPE_ATTACHMENT,    ///< Opaque data information usually sparse
	AV_MEDIA_TYPE_NB
};

enum AV_CodecID {
	AV_CODEC_ID_NONE,
	AV_CODEC_ID_AAC,
	AV_CODEC_ID_H264
};

typedef struct StreamType {
	MUInt32 stream_type;
	enum AV_MediaType codec_type;
	enum AV_CodecID codec_id;
} StreamType;

static const StreamType ISO_types[] = {
	{ 0x0f, AV_MEDIA_TYPE_AUDIO, AV_CODEC_ID_AAC },
	{ 0x1b, AV_MEDIA_TYPE_VIDEO, AV_CODEC_ID_H264 },
	{ 0 }
};


enum MpegTSState {
	MPEGTS_HEADER = 0,
	MPEGTS_PESHEADER,
	MPEGTS_PESHEADER_FILL,
	MPEGTS_PAYLOAD,
	MPEGTS_SKIP,
};

/* enough for PES header + length */
#define PES_START_SIZE  6
#define PES_HEADER_SIZE 9
#define MAX_PES_HEADER_SIZE (9 + 255)

//static const StreamType ISO_types[] = {
//	{ 0x01, AVMEDIA_TYPE_VIDEO, AV_CODEC_ID_MPEG2VIDEO },
//	{ 0x02, AVMEDIA_TYPE_VIDEO, AV_CODEC_ID_MPEG2VIDEO },
//	{ 0x03, AVMEDIA_TYPE_AUDIO, AV_CODEC_ID_MP3 },
//	{ 0x04, AVMEDIA_TYPE_AUDIO, AV_CODEC_ID_MP3 },
//	{ 0x0f, AVMEDIA_TYPE_AUDIO, AV_CODEC_ID_AAC },
//	{ 0x10, AVMEDIA_TYPE_VIDEO, AV_CODEC_ID_MPEG4 },
//	/* Makito encoder sets stream type 0x11 for AAC,
//	* so auto-detect LOAS/LATM instead of hardcoding it. */
////#if !CONFIG_LOAS_DEMUXER
////	{ 0x11, AVMEDIA_TYPE_AUDIO, AV_CODEC_ID_AAC_LATM }, /* LATM syntax */
////#endif
//	{ 0x1b, AVMEDIA_TYPE_VIDEO, AV_CODEC_ID_H264 },
//	{ 0x1c, AVMEDIA_TYPE_AUDIO, AV_CODEC_ID_AAC },
//	{ 0x20, AVMEDIA_TYPE_VIDEO, AV_CODEC_ID_H264 },
//	{ 0x21, AVMEDIA_TYPE_VIDEO, AV_CODEC_ID_JPEG2000 },
//	{ 0x24, AVMEDIA_TYPE_VIDEO, AV_CODEC_ID_HEVC },
//	{ 0x42, AVMEDIA_TYPE_VIDEO, AV_CODEC_ID_CAVS },
//	{ 0xd1, AVMEDIA_TYPE_VIDEO, AV_CODEC_ID_DIRAC },
//	{ 0xea, AVMEDIA_TYPE_VIDEO, AV_CODEC_ID_VC1 },
//	{ 0 },
//};

#endif
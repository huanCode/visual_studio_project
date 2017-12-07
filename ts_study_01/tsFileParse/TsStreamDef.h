#include "amcomdef.h"
#define MAX_SECTION_SIZE	(4*1024)
/*PSI*/
#define PAT_PID                 0x0000
#define SDT_PID                 0x0011		//17
#define PMT_PID					0x1000		//4096
#define EMPTY_PID               0x1FFF

/* table ids */
#define PAT_TID   0x00
#define CAT_TID	  0x01
#define PMT_TID   0x02

enum MpegTSFilterType {
	MPEGTS_PES,
	MPEGTS_SECTION,
	MPEGTS_PCR,
};

typedef struct MpegTSSectionFilter {
	int section_index;
	int section_length;
	int last_ver;
	unsigned crc;
	unsigned last_crc;
	MByte *section_buf;
	unsigned int check_crc : 1;
	unsigned int end_of_section_reached : 1;
	//SectionCallback *section_cb;
	void *opaque;
} MpegTSSectionFilter;

typedef struct MpegTSPESFilter {
	//PESCallback *pes_cb;
	void *opaque;
} MpegTSPESFilter;

struct MpegTSFilter {
	int pid;
	int es_id;
	int last_cc; /* last cc code (-1 if first packet) */
	MInt64 last_pcr;
	enum MpegTSFilterType type;
	union {
		MpegTSPESFilter pes_filter;
		MpegTSSectionFilter section_filter;
	} section_or_pes;
};

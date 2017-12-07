#include "amcomdef.h"
#define FILTER_NUM	6
class tsFilter
{
public:
	enum MpegTSFilterType {
		MPEGTS_PES,
		MPEGTS_SECTION,
		MPEGTS_PCR,
	};
	tsFilter()
	{
		m_pid = -1;
		m_es_id = -1;
		m_last_cc = -1;
		m_last_pcr = -1;
	}
	virtual MUInt32 parse(MPByte p_buffer,MUInt32 p_buffer_size) = 0;
private:


protected:
	typedef struct SectionHeader {
		SectionHeader()
		{
			tid = 0;
			section_length = 0;
			id = 0;
			version = 0;
			sec_num = 0;
			last_sec_num = 0;
		}
		MUInt8 tid;
		MUInt8 section_length;
		MUInt16 id;
		MUInt8 version;
		MUInt8 sec_num;
		MUInt8 last_sec_num;
	} SectionHeader;
	MInt32 parse_section_header(MByte* buffer_section_header, SectionHeader &section_header);
private:
	MInt32 m_pid;
	MInt32 m_es_id;
	MInt32 m_last_cc; /* last cc code (-1 if first packet) */
	MInt64 m_last_pcr;
protected:
	enum MpegTSFilterType m_type;
};

class tsSectionPat:public tsFilter
{
public:
	tsSectionPat()
	{
		m_type = MPEGTS_SECTION;
	}
	MUInt32 parse(MPByte p_buffer, MUInt32 p_buffer_size);
};


class tsSectionPmt :public tsFilter
{
public:
	tsSectionPmt()
	{
		m_type = MPEGTS_SECTION;
	}
	MUInt32 parse(MPByte p_buffer, MUInt32 p_buffer_size);
private:
	MInt32	m_pcr_pid;
};

class tsSectionPes :public tsFilter
{
public:
	tsSectionPes()
	{
		m_type = MPEGTS_PES;
	}
	MUInt32 parse(MPByte p_buffer, MUInt32 p_buffer_size);
};

static tsFilter* CreateFilter(MInt32 pid);
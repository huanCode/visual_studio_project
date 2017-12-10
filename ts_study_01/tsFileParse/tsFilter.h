#include "amcomdef.h"
#include "TsStreamDef.h"
#define FILTER_NUM	6
class TsStream;
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
		m_description = MNull;
	}
	virtual MUInt32 parse(TsStream* p_tsStream, MPByte p_buffer,MUInt32 p_buffer_size) = 0;
	virtual MVoid	SetPid(MInt32 p_pid) = 0;
	MInt32 GetPid() { return m_pid; };
	MpegTSFilterType GetType() {
		return m_type;
	}
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

	MInt32 m_es_id;
	MInt32 m_last_cc; /* last cc code (-1 if first packet) */
	MInt64 m_last_pcr;
protected:
	MInt32 m_pid;
	MPChar	m_description;
	enum MpegTSFilterType m_type;
};

class tsSection :public tsFilter
{
public:
	MVoid write_section_data(TsStream* p_tsStream, const MByte *p_buf, MUInt32 p_buf_size, MBool p_is_start);
protected:
	MInt32 m_section_index;
	MInt32 m_section_length;
	MInt32 m_last_ver;
	MUInt32 m_crc;
	MUInt32 m_last_crc;
	MByte *m_section_buf;
	MBool	m_end_of_section_reached;
};


class tsSectionSdt :public tsSection
{
public:
	tsSectionSdt()
	{
		m_type = MPEGTS_SECTION;
		m_pid = SDT_PID;
		m_description = "sdt";
	}
	MUInt32 parse(TsStream* p_tsStream, MPByte p_buffer, MUInt32 p_buffer_size) { return 0; };
	MVoid	SetPid(MInt32 p_pid) {};
};


class tsSectionPat:public tsSection
{
public:
	tsSectionPat()
	{
		m_type = MPEGTS_SECTION;
		m_pid = PAT_PID;
		m_description = "pat";
	}
	MUInt32 parse(TsStream* p_tsStream, MPByte p_buffer, MUInt32 p_buffer_size);
	MVoid	SetPid(MInt32 p_pid) {};
};




class tsSectionPmt :public tsSection
{
public:
	tsSectionPmt()
	{
		m_type = MPEGTS_SECTION;
		m_description = "pmt";
	}
	MUInt32 parse(TsStream* p_tsStream, MPByte p_buffer, MUInt32 p_buffer_size);
	MVoid	SetPid(MInt32 p_pid) {
		m_pid = p_pid;
	};
private:
	MBool is_pes_stream(int stream_type, MUInt32 prog_reg_desc = 0);

private:
	MInt32	m_pcr_pid;
};

class tsSectionPes :public tsFilter
{
public:
	tsSectionPes()
	{
		m_type = MPEGTS_PES;
		m_description = "pes";
	}
	MUInt32 parse(TsStream* p_tsStream, MPByte p_buffer, MUInt32 p_buffer_size);
	MVoid	SetPid(MInt32 p_pid) {
		m_pid = p_pid;
	};
};


class FilterFactory
{
public:
	static tsFilter* CreateFilter(MInt32 pid);

};


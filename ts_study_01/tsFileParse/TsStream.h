#pragma once
#include "amcomdef.h"
#include "mv3File.h"
class TsStream
{
public:
	TsStream();
private:
	struct Packet
	{
		MInt32	pts;
		MInt32  dts;
		MByte*	buffer;
		MUInt32	size;
		MInt32	type;
	};


	struct ts_header
	{
		ts_header()
		{
			sync_byte = 0;
			transport_error = MFalse;
			bStart_payload = MFalse;
			pid = 0;
			has_adaptation = MFalse;
			has_payload = MFalse;
			continuity_counter = 0;
		}
		MUInt8	sync_byte;
		bool transport_error;
		bool	bStart_payload;
		MUInt16	pid;
		bool has_adaptation;
		bool has_payload;
		MUInt8 continuity_counter;
	};

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

	struct StreamData
	{
		MUInt32	stream_type;
		MUInt32 pid;
	};

	enum MpegTSState {
		MPEGTS_HEADER = 0,
		MPEGTS_PESHEADER,
		MPEGTS_PESHEADER_FILL,
		MPEGTS_PAYLOAD,
		MPEGTS_SKIP,
	};

public:
	MUInt32 mpegts_read_header();

private:
	MBool	probe(MPByte p_buffer,MUInt32 p_size);
	MUInt32 parse_ts(MByte* buffer_packet);
	MVoid parse_ts_header(MByte* buffer, ts_header &tsHeader);

	MUInt32 pat_cb(MByte* section_start_pos);
	MUInt32 pmt_cb(MByte* section_start_pos);
	MInt32 TsStream::parse_section_header(MByte* biffer_section_header, SectionHeader &section_header);
	MUInt32 parse_frame(MByte* buffer,MUInt32 buffer_size,MBool is_start);
	inline MInt64 ff_parse_pes_pts(const MUInt8 *buffer);
private:
	MUInt32		m_pcr_pid;
	StreamData	m_streamVideo;
	StreamData	m_streamAudio;

	MpegTSState	m_pes_state;
	Packet		m_packet;
	bool		m_bReadOneFrame;	//表示是否成功读完一帧
	MUInt32		m_frame_total_size;
	MUInt32		m_pes_header_size;
	MByte		m_buffer[512000];
	mv3File		m_fileWrite;

	MBool		m_isFoundPmt;
};

#include "stdafx.h"
#include "tsFilter.h"
#include "common.h"
#include "TsStreamDef.h"
#include "TsStream.h"
#define SECTION_HEADER_SIZE_8_BYTE		8



tsSection::tsSection()
{
	m_section_index = 0;
	m_section_length = 0;
	m_last_ver = -1;
	m_crc = 0;
	m_last_crc = 0;
	m_section_buf = new MByte[MAX_SECTION_SIZE];
	memset(m_section_buf,0, MAX_SECTION_SIZE);
}

MVoid tsSection::write_section_data(TsStream* p_tsStream, const MByte *p_buf, MUInt32 p_buf_size, MBool p_is_start)
{
	if (!m_section_buf)
	{
		return;
	}


	if (p_is_start) {
		//说明section大小小于payload
		memcpy(m_section_buf, p_buf, p_buf_size);
		m_section_index = p_buf_size;
		m_section_length = -1;
		m_end_of_section_reached = MFalse;

		/* compute section length if possible */
		if (m_section_length == -1 && m_section_index >= 3) {
			MUInt32 section_length = (AV_RB16(m_section_buf + 1) & 0xfff) + 3;	//计算section_length
			if (section_length > MAX_SECTION_SIZE)
				return;
			m_section_length = section_length;
		}
	}
	else {
		if (m_end_of_section_reached)
			return;
		MUInt32 remainder_len = MAX_SECTION_SIZE - m_section_index;
		if (p_buf_size < remainder_len)
		{
			memcpy(m_section_buf + m_section_index, p_buf, p_buf_size);
			m_section_index += p_buf_size;
		}

	}

	if (m_section_length != -1 && m_section_index >= m_section_length)
	{
		/*说明已经全部组合成*/
		MBool crc_valid = MFalse;
		m_end_of_section_reached = MTrue;
		/*省略crc验证*/

		parse(p_tsStream,m_section_buf, m_section_length);
	}
}

MInt32 tsFilter::parse_section_header(MByte* buffer_section_header, SectionHeader &section_header)
{
	int val;

	val = get8(buffer_section_header);	//0 byte

	if (val < 0)
		return -1;

	section_header.tid = val;

	section_header.section_length = get16(buffer_section_header) & 0xFFF;

	val = get16(buffer_section_header);	//3 byte
	if (val < 0)
		return val;

	section_header.id = val;

	val = get8(buffer_section_header);	//5 byte
	if (val < 0)
		return val;
	section_header.version = (val >> 1) & 0x1f;
	val = get8(buffer_section_header);	//6 byte
	if (val < 0)
		return val;
	section_header.sec_num = val;
	val = get8(buffer_section_header);	//7 byte
	if (val < 0)
		return val;
	section_header.last_sec_num = val;
	return 0;
}
/*========================================================================*/

MUInt32 tsSectionPat::parse(TsStream* p_tsStream, MPByte p_buffer, MUInt32 p_buffer_size)
{
	SectionHeader section_header;
	MByte* pat_data = p_buffer;
	MInt32 ret = parse_section_header(pat_data, section_header);
	if (ret != 0)
	{
		return -1;
	}
	pat_data += SECTION_HEADER_SIZE_8_BYTE;

	if (section_header.tid != PAT_TID)
		return 0;

	MInt32 sid = 0;
	MInt32 pmt_pid = 0;

	//10字节,包括从transport_stream_id到last_section_number，和CRC32
	MUInt16 program_length = (section_header.section_length - 9) / 4;
	for (MUInt32 i = 0; i < program_length; i++)
	{
		sid = get16(pat_data);
		if (sid < 0)
			break;

		if (sid == 0)
		{
			//sid == 0,表示这是NIT，可以跳过
			continue;
		}
		//sid节目号为0x0001时, 表示这是PMT，PID = 0x1000，即4096

		pmt_pid = get16(pat_data);
		if (pmt_pid < 0)
		{
			break;
		}
		pmt_pid &= 0x1fff;
		if (pmt_pid != PMT_PID)
		{
			return -1;
		}

		tsFilter* filter = p_tsStream->add_filter(pmt_pid);

		//目前只支持一个pmt,so break
		break;
	}
	return 0;
}

/*=========================================================================*/

MBool tsSectionPmt::is_pes_stream(int stream_type, MUInt32 prog_reg_desc)
{
	MInt32 y = AV_RL32("CUEI");
	return !(stream_type == 0x13 || (stream_type == 0x86 && prog_reg_desc == AV_RL32("CUEI")));
}


//MVoid tsSectionPmt::mpegts_find_stream_type(MInt32 stream_type, const StreamType *types)
//{
//	for (; types->stream_type; types++) {
//		if (stream_type == types->stream_type) {
//			if (st->codecpar->codec_type != types->codec_type ||
//				st->codecpar->codec_id != types->codec_id) {
//				st->codecpar->codec_type = types->codec_type;
//				st->codecpar->codec_id = types->codec_id;
//				st->internal->need_context_update = 1;
//			}
//			st->request_probe = 0;
//			return;
//		}
//	}
//}

MUInt32 tsSectionPmt::parse(TsStream* p_tsStream, MPByte p_buffer, MUInt32 p_buffer_size)
{
	SectionHeader section_header;
	MInt32 ret = parse_section_header(p_buffer, section_header);
	if (ret != 0)
	{
		return -1;
	}
	MByte* pData = p_buffer + SECTION_HEADER_SIZE_8_BYTE;
	MByte* pData_end = p_buffer + p_buffer_size - 4;
	if (section_header.tid != PMT_TID)
		return 0;


	m_pcr_pid = get16(pData);
	if (m_pcr_pid < 0)
		return 0;
	m_pcr_pid &= 0x1fff;

	MUInt16 program_info_length = get16(pData);
	//pData += 2;
	if (program_info_length < 0)
		return -1;
	program_info_length &= 0xfff;
	if (program_info_length >= 2)
	{
		return -1;
	}
	//跳过program_info部分
	pData += program_info_length;


	MInt32 stream_type = 0;
	MInt32 pid = 0;
	while (true)
	{
		//27 0x1b H264
		stream_type = get8(pData);
		pid = get16(pData) & 0x1fff;
		if (pid < 0)
		{
			return -1;
		}
		tsFilter* filter = p_tsStream->get_filter(pid);
		if (filter == MNull && is_pes_stream(stream_type))
		{
			filter = p_tsStream->add_filter(pid);
			if (filter == MNull)
			{
				return -1;
			}
			tsSectionPes* filterPes = (tsSectionPes*)filter;
			filterPes->mpegts_find_stream_type(stream_type, ISO_types);
		}
		else
		{
			return -1;
		}

		MUInt16  desc_list_len = get16(pData);
		if (desc_list_len < 0)
			return -1;
		desc_list_len &= 0xfff;
		pData += desc_list_len;
		if (pData_end < pData)
		{
			return -1;
		}
		else if (pData_end == pData)
		{
			return 0;
		}



		//15 0xF为AAC
		//m_streamAudio.stream_type = section_start_pos[17];
		//m_streamAudio.pid = to_UInt16(section_start_pos + 18) & 0x1fff;
		//desc_list_len = to_UInt16(section_start_pos + 20);
		//if (desc_list_len < 0)
		//	return -1;
		//desc_list_len &= 0xfff;
	}
}



/*==========================================================================*/
MUInt32 tsSectionPes::parse(TsStream* p_tsStream, MPByte p_buffer, MUInt32 p_buffer_size)
{
	//is_start = 1，表示这个时pes的开始
	//is_start = 0,表示这个时pes中的后续字节
	MBool start = MFalse;
	MBool b = p_tsStream->m_isStart;
	if (p_tsStream->m_isStart)
	{
		//表示上一帧读取完成
		//printf("PES  size = %d --------------------------------------\n", m_packet.size);
		//if (!m_fileWrite.Write(m_buffer, m_packet.size))
		//{
		//	int i = 1;
		//}

		//m_packet.size = 0;
		//m_pes_state = MPEGTS_HEADER;
		//start = MTrue;

		//memset(&m_buffer, 0, 512000);


	}

	MUInt16 length = 0;
	MInt32	code = 0;
	MUInt16 buffer_length = buffer_size;
	MPByte pBuffer = p_buffer;
	while (p_buffer_size > 0)
	{
		if (m_state == MPEGTS_HEADER)
		{
			//if (PES_START_SIZE > buffer_size)
			//{
			//	return -1;
			//}
			length = PES_START_SIZE - m_size;
			if (length > p_buffer_size)
				length = p_buffer_size;

			memcpy(m_header + m_size, pBuffer, length);
			m_size += length;
			p_buffer_size -= length;

			if (m_size == PES_START_SIZE)
			{
				//-----------------------------------------------------------------------------
				//| packet_start_code_prefix 3byte | stream_id 1byte | PES_packet_length 2byte
				//-------------------------------------------------------------------------------
				//packet_start_code_prefix : 0x00 00 01
				//stream_id	:

				/* we got all the PES or section header. We can now
				* decide */
				if (m_header[0] == 0x00 && m_header[1] == 0x00 && m_header[2] == 0x01)
				{
					/* it must be an MPEG-2 PES stream */
					code = m_header[3] | 0x100;
					m_stream_id = m_header[3];
					if (code == 0x1be)
					{
						//goto skip
					}
					m_total_size = AV_RB16(m_header + 4);	//get pes size
					if (m_total_size == 0)
					{
						//m_total_size  ,说明pes的大小是未知的
						m_total_size = MAX_PES_PAYLOAD;
					}
					m_buffer = new MByte[m_total_size + AV_INPUT_BUFFER_PADDING_SIZE];
					memset(m_buffer, 0, m_total_size + AV_INPUT_BUFFER_PADDING_SIZE);
					if (!m_buffer)
					{
						return -1;
					}

					if (code != 0x1bc && code != 0x1bf && /* program_stream_map, private_stream_2 */
						code != 0x1f0 && code != 0x1f1 && /* ECM, EMM */
						code != 0x1ff && code != 0x1f2 && /* program_stream_directory, DSMCC_stream */
						code != 0x1f8) {                  /* ITU-T Rec. H.222.1 type E stream */
						m_state = MPEGTS_PESHEADER;
	
					}
					else {
						m_pes_header_size = 6;
						m_state = MPEGTS_PAYLOAD;
						m_size = 0;
					}
				}
				else
				{
skip:
					m_state = MPEGTS_SKIP;
					continue;
				}

			}
	


		}
		else if (m_state == MPEGTS_PESHEADER)
		{
			length = PES_HEADER_SIZE - m_size;
			if (length < 0)
			{
				return -1;
			}

			if (length > p_buffer_size)
				length = p_buffer_size;

			memcpy(m_header + m_size, pBuffer, length);
			m_size += length;
			pBuffer -= length;
			p_buffer_size -= length;
			if (m_size == PES_HEADER_SIZE) {
				m_pes_header_size = m_header[8] + 9;
				m_state = MPEGTS_PESHEADER_FILL;
			}
			break;

		}
		else if (m_pes_state == MPEGTS_PESHEADER_FILL)
		{
			MUInt16 flags = buffer[7];

			if ((flags & 0xc0) == 0x80) {
				m_packet.pts = m_packet.dts = ff_parse_pes_pts(buffer + 9);
			}
			else if ((flags & 0xc0) == 0xc0) {
				m_packet.pts = ff_parse_pes_pts(buffer + 9);
				m_packet.dts = ff_parse_pes_pts(buffer + 14);
			}

			buffer_length = buffer_size - m_pes_header_size;
			m_pes_state = MPEGTS_PAYLOAD;
		}
		else if (m_pes_state == MPEGTS_PAYLOAD)
		{
			MUInt16	real_playload_size = (buffer_size - m_pes_header_size);

			memcpy(m_buffer + m_packet.size, buffer + m_pes_header_size, real_playload_size);
			m_packet.size += real_playload_size;
			if (start)
			{

				MUInt8 by1 = buffer[m_pes_header_size];
				MUInt8 by2 = buffer[m_pes_header_size + 1];
				MUInt8 by3 = buffer[m_pes_header_size + 2];
				MUInt8 by4 = buffer[m_pes_header_size + 3];
				MUInt8 by5 = buffer[m_pes_header_size + 5];
				printf("PES  nal %d 0x%d %x %d %d \n", by1, by2, by3, by4, by5);
			}

			m_pes_header_size = 0;


			break;
		}


	}





	return 1;
}




MVoid tsSectionPes::mpegts_find_stream_type(MInt32 stream_type, const StreamType *types)
{
	for (; types->stream_type; types++) {
		if (stream_type == types->stream_type) {
			if (m_mediaType != types->codec_type || m_mediaCodecID != types->codec_id) {
				m_mediaType = types->codec_type;
				m_mediaCodecID = types->codec_id;
			}
			return;
		}
	}
}

tsFilter* FilterFactory::CreateFilter(MInt32 pid)
{
	tsFilter* filter = MNull;
	switch (pid)
	{
	case SDT_PID:
		filter = new tsSectionSdt();
		break;
	case PAT_TID:
		filter = new tsSectionPat();
		break;
	case PMT_PID:
		filter = new tsSectionPmt();
		break;
	default:
		filter = new tsSectionPes();
		break;
	}
	if (filter)
	{
		filter->SetPid(pid);
	}

	return filter;
}

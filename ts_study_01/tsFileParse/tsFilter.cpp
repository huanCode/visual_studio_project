#include "stdafx.h"
#include "tsFilter.h"
#include "common.h"
#include "TsStreamDef.h"
#include "TsStream.h"
#define SECTION_HEADER_SIZE_8_BYTE		8



MVoid tsSection::write_section_data(TsStream* p_tsStream, const MByte *p_buf, MUInt32 p_buf_size, MBool p_is_start)
{



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

	val = buffer_section_header[0];	//0 byte
	buffer_section_header++;
	if (val < 0)
		return -1;

	section_header.tid = val;

	section_header.section_length = get16(buffer_section_header) & 0xFFF;

	val = get16(buffer_section_header);	//3 byte
	if (val < 0)
		return val;

	section_header.id = val;

	val = *buffer_section_header;	//5 byte
	if (val < 0)
		return val;
	section_header.version = (val >> 1) & 0x1f;
	val = *buffer_section_header;	//6 byte
	if (val < 0)
		return val;
	section_header.sec_num = val;
	val = *buffer_section_header;	//7 byte
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
	MByte* pData_end = p_buffer + p_buffer_size;
	if (section_header.tid != PMT_TID)
		return 0;


	m_pcr_pid = get16(pData);
	if (m_pcr_pid < 0)
		return 0;
	m_pcr_pid &= 0x1fff;

	MUInt16 program_info_length = get16(pData);
	pData += 2;
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
	return -1;
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

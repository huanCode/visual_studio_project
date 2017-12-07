#include "tsFilter.h"
#include "common.h"
#include "TsStreamDef.h"
#define SECTION_HEADER_SIZE_8_BYTE		8

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

MUInt32 tsSectionPat::parse(MPByte p_buffer, MUInt32 p_buffer_size)
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
	MInt32 pmt = 0;

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

		pmt = get16(pat_data);
		if (pmt < 0)
		{
			break;
		}
		pmt &= 0x1fff;
		if (pmt != PMT_PID)
		{
			return -1;
		}
		m_isFoundPmt = MTrue;
		//目前只支持一个pmt,so break
		break;
	}
}

/*=========================================================================*/
MUInt32 tsSectionPmt::parse(MPByte p_buffer, MUInt32 p_buffer_size)
{
	SectionHeader section_header;
	MInt32 ret = parse_section_header(p_buffer, section_header);
	if (ret != 0)
	{
		return -1;
	}
	MByte* pData = p_buffer + SECTION_HEADER_SIZE_8_BYTE;
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

	//跳过program_info部分
	pData += program_info_length;

	while (true)
	{
		//27 0x1b H264
		m_streamVideo.stream_type = *pData;
		pData++;
		m_streamVideo.pid = get16(pData) & 0x1fff;
		MUInt16  desc_list_len = get16(pData);
		if (desc_list_len < 0)
			return -1;
		desc_list_len &= 0xfff;



		//15 0xF为AAC
		m_streamAudio.stream_type = section_start_pos[17];
		m_streamAudio.pid = to_UInt16(section_start_pos + 18) & 0x1fff;
		desc_list_len = to_UInt16(section_start_pos + 20);
		if (desc_list_len < 0)
			return -1;
		desc_list_len &= 0xfff;
	}
}
/*==========================================================================*/
MUInt32 tsSectionPes::parse(MPByte p_buffer, MUInt32 p_buffer_size)
{

}


tsFilter* CreateFilter(MInt32 pid)
{
	tsFilter* filter = MNull;
	switch (pid)
	{
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

	return filter;
}

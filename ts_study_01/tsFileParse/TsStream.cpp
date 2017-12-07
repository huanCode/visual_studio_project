#include "stdafx.h"
#include "TsStream.h"
#include "common.h"
#include "tsFilter.h"

#define PACKET_SIZE	188
#define SECTION_HEADER_SIZE_8_BYTE		8
#define PROBE_BUFFER_SIZE	PACKET_SIZE * 10

#define TS_PACKET_SYNC_BYTE		0x47




//#define NIT_TID	  0xXX	私用表，tid自定义

#define M4OD_TID  0x05
#define SDT_TID   0x42


#define TS_PACKET_HEADER_SIZE	4
#define PES_START_SIZE  6
#define PES_HEADER_SIZE 9

#define TS_PACKET_SIZE 188

#define AV_RB16(x)  ((((const MUInt8*)(x))[0] << 8) | ((const MUInt8*)(x))[1])
//#define AV_RB32(x)  ((((const uint8_t*)(x))[0] << 24) | /
//(((const uint8_t*)(x))[1] << 16) | /
//(((const uint8_t*)(x))[2] << 8) | /
//((const uint8_t*)(x))[3])
//#define AV_RB32(x)  ((((const uint8_t*)(x))[0] << 24) | /
//(((const uint8_t*)(x))[1] << 16) | /
//(((const uint8_t*)(x))[2] << 8) | /
//((const uint8_t*)(x))[3])

TsStream::TsStream()
{
	m_pcr_pid = 0;
	m_pes_state = MPEGTS_HEADER;
	m_pes_header_size = 0;
	memset(&m_packet,0,sizeof(Packet));
	memset(&m_buffer, 0, 512000);
	MBool ret = m_fileWrite.Open("bigbuckbunny_480x272.h265", mv3File::stream_write);
	m_isFoundPmt = MFalse;

	memset(m_tsFilter, 0, sizeof(MpegTSFilter) * 6);
}

MUInt32 TsStream::mpegts_read_header()
{
	MUInt32 ret = 0;
	mv3File file;

	if (!file.Open("playlist-00001.ts", mv3File::stream_read))
	{
		return -1;
	}
	MByte packet[PACKET_SIZE] = { 0 };
	int i = 0;
	int length = file.GetFileSize();
	
	while (file.Read(packet, PROBE_BUFFER_SIZE))
	{
		//printf("i = %d ,curpos = %d\n",i++,file.GetFileCurPos());

		read_probe(packet, PROBE_BUFFER_SIZE);

		//ret = parse_ts(packet);
		//if (ret != 0)
		//{
		//	return ret;
		//}
		if (i == 32397)
		{
			int a = 1;
			m_fileWrite.Close();
		}

		printf("\n");
	}
}

MBool TsStream::read_probe(MPByte p_buffer, MUInt32 p_size)
{
	if (p_buffer != MNull || p_size < PROBE_BUFFER_SIZE)
	{
		return MFalse;
	}
	MUInt32 check_count = p_size / TS_PACKET_SIZE;
	MUInt32	standard_count = 10;
	if (check_count < standard_count)
	{
		return MFalse;
	}

	MUInt32 best_score = 0;
	MUInt32 pos = 0;
	for (MUInt32 i = 1;i<=check_count;i++)
	{
		if (p_buffer[pos]== TS_PACKET_SYNC_BYTE)
		{
			best_score++;
			pos = i * PACKET_SIZE;
		}
	}
	if (check_count == standard_count)
	{
		return MTrue;
	}
	return MFalse;
}



MInt32 TsStream::read_header(MPByte p_buffer_packet, MUInt32 p_size)
{
	if (p_buffer_packet == MNull || p_size != PACKET_SIZE)
	{
		return -1;
	}
	ts_packet_header tsHeader;
	parse_ts_packet_header(p_buffer_packet, tsHeader);

	if (tsHeader.sync_byte != TS_PACKET_SYNC_BYTE || tsHeader.transport_error)
	{
		return -1;
	}

	if (tsHeader.pid == 0x1FFF || tsHeader.has_payload == MFalse)
	{
		return 0;
	}

	MUInt16 adaptation_length = 0;
	if (tsHeader.has_adaptation)
	{
		adaptation_length = p_buffer_packet[4] + 1;
		if (adaptation_length > 1)
		{
			bool is_discontinuity = p_buffer_packet[5] & 0x80;
			if (is_discontinuity)
			{
				int i = 0;
			}
			//printf("is_discontinuity = %d =====================\n", is_discontinuity);
		}
		//printf("has_adaptation ,adaptation_length = %d\n", adaptation_length);
	}
	//printf("continuity_counter = %d\n", tsHeader.continuity_counter);


	MByte* buffer_packet_section_strat_pos = p_buffer_packet + TS_PACKET_HEADER_SIZE + adaptation_length + point_field_length;
	MUInt32	buffer_remain_size = PACKET_SIZE - adaptation_length - TS_PACKET_HEADER_SIZE - point_field_length;

	tsFilter* filter = add_filter(tsHeader.pid);
	if (!filter)
	{
		return -1;
	}


	if (filter->type == MPEGTS_SECTION)
	{
		if (tsHeader.bStart_payload)
		{
			/* pointer field present */
			MByte* point_field = p_buffer_packet + TS_PACKET_HEADER_SIZE + adaptation_length;
			MUInt16 point_field_length = point_field[0];
			MUInt32 expected_cc = tsHeader.has_payload ? (filter->last_cc + 1) & 0x0f : filter->last_cc;
			if (expected_cc == tsHeader.continuity_counter && point_field_length)
			{

			}
		}
		else
		{

		}
	}


	if(tsHeader.pid == PAT_PID)
	{
		//printf("PAT \n");
		pat_cb(buffer_packet_section_strat_pos);
	}
	else if (tsHeader.pid == PMT_PID && m_isFoundPmt)
	{
		//printf("PMT \n");
		pmt_cb(buffer_packet_section_strat_pos);
	}
	else if (tsHeader.pid == m_streamVideo.pid)
	{
		//printf("PES Video \n");
		parse_frame(buffer_packet_section_strat_pos, buffer_remain_size, tsHeader.bStart_payload);
	}
	else if (tsHeader.pid == m_streamAudio.pid)
	{
		//printf("PES Audio \n");
		parse_frame(buffer_packet_section_strat_pos, buffer_remain_size, tsHeader.bStart_payload);
	}
	

}



MVoid TsStream::write_section_data(MpegTSFilter *p_tsFilter, const MByte *p_buf, MUInt32 p_buf_size, MBool p_is_start)
{
	MpegTSSectionFilter *tss = &p_tsFilter->section_or_pes.section_filter;


	if (p_is_start) {
		//说明section大小小于payload
		memcpy(tss->section_buf, p_buf, p_buf_size);
		tss->section_index = p_buf_size;
		tss->section_length = -1;
		tss->end_of_section_reached = 0;

		/* compute section length if possible */
		if (tss->section_length == -1 && tss->section_index >= 3) {
			MUInt32 section_length = (AV_RB16(tss->section_buf + 1) & 0xfff) + 3;	//计算section_length
			if (section_length > MAX_SECTION_SIZE)
				return;
			tss->section_length = section_length;
		}
	}
	else {
		if (tss->end_of_section_reached)
			return;
		MUInt32 remainder_len = MAX_SECTION_SIZE - tss->section_index;
		if (p_buf_size < remainder_len)
		{
			memcpy(tss->section_buf + tss->section_index, p_buf, p_buf_size);
			tss->section_index += p_buf_size;
		}

	}

	if (tss->section_length != -1 && tss->section_index >= tss->section_length)
	{
		/*说明已经全部组合成*/
		MBool crc_valid = MFalse;
		tss->end_of_section_reached = 1;
		/*省略crc验证*/
	}

}


MUInt32 TsStream::parse_frame(MByte* buffer,MUInt32 buffer_size,MBool is_start)
{
	//is_start = 1，表示这个时pes的开始
	//is_start = 0,表示这个时pes中的后续字节
	MBool start = MFalse;
	if (is_start)
	{
		//表示上一帧读取完成
		printf("PES  size = %d --------------------------------------\n", m_packet.size);
		if (!m_fileWrite.Write(m_buffer, m_packet.size))
		{
			int i = 1;
		}

		m_packet.size = 0;
		m_pes_state = MPEGTS_HEADER;
		start = MTrue;
		
		memset(&m_buffer, 0, 512000);

		
	}

	MUInt16 length = 0;
	MUInt16 buffer_length = buffer_size;
	
	while (buffer_length > 0)
	{
		if (m_pes_state == MPEGTS_HEADER)
		{
			if (PES_START_SIZE > buffer_size)
			{
				return -1;
			}

			if (buffer[0] == 0x00 && buffer[1] == 0x00 && buffer[2] == 0x01)
			{
				MUInt16 stream_id = buffer[3];
				if (stream_id == 0xE0)
				{
					//video 0xe0
					m_pes_state = MPEGTS_PESHEADER;
				}
				else if (stream_id == 0xC0)
				{
					//audio 0xc0
					m_pes_state = MPEGTS_PESHEADER;
				}

				m_frame_total_size = to_UInt16(buffer + 4);

			}
			buffer_length = buffer_size - PES_START_SIZE;
			m_pes_header_size = 6;
		
		}
		else if (m_pes_state == MPEGTS_PESHEADER)
		{
			if (PES_HEADER_SIZE > buffer_size)
			{
				return -1;
			}

			m_pes_header_size = buffer[8] + PES_HEADER_SIZE;
			buffer_length = buffer_size - PES_HEADER_SIZE;
			m_pes_state = MPEGTS_PESHEADER_FILL;
		}
		else if (m_pes_state == MPEGTS_PESHEADER_FILL)
		{
			MUInt16 flags = buffer[7];

			if ((flags & 0xc0) == 0x80) {
				m_packet.pts = m_packet.dts = ff_parse_pes_pts(buffer+9);
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
			
			memcpy(m_buffer+ m_packet.size, buffer + m_pes_header_size, real_playload_size);
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



MVoid TsStream::parse_ts_packet_header(MByte* buffer, ts_packet_header &tsHeader)
{
	tsHeader.sync_byte = buffer[0];

	MUInt16 ts_header_23byte = to_UInt16(buffer + 1);

	//‘1’表示在相关的传输包中至少有一个不可纠正的错误位。
	tsHeader.transport_error = ts_header_23byte & 0x8000;

	//为1时，表示当前TS包的有效载荷中包含PES或者PSI的起始位置；
	//在前4个字节之后会有一个调整字节，其的数值为后面调整字段的长度length。因此有效载荷开始的位置应再偏移1 + [length]个字节。
	tsHeader.bStart_payload = ts_header_23byte & 0x4000;

	tsHeader.pid = ts_header_23byte & 0x1fff;


	//下面主要是判断adaptation_field_control（适配域控制标志）
	//表示包头是否有调整字段或有效负载。
	//‘00’为ISO/IEC未来使用保留；
	//‘01’仅含有效载荷，无调整字段；
	//‘10’ 无有效载荷，仅含调整字段；
	//‘11’ 调整字段后为有效载荷，调整字段中的前一个字节表示调整字段的长度length，有效载荷开始的位置应再偏移[length]个字节。
	//空包应为‘10’。
	tsHeader.has_adaptation = buffer[3] & 0x20;	//
	tsHeader.has_payload	= buffer[3] & 0x10;
	tsHeader.continuity_counter = buffer[3] & 0xf;

}

//MUInt32 TsStream::pat_cb(MByte* section_start_pos)
//{
//	SectionHeader section_header;
//	MByte* pat_data = section_start_pos;
//	MInt32 ret = parse_section_header(pat_data, section_header);
//	if (ret != 0)
//	{
//		return -1;
//	}
//	pat_data += SECTION_HEADER_SIZE_8_BYTE;
//
//	if (section_header.tid != PAT_TID)
//		return 0;
//
//	MInt32 sid = 0;
//	MInt32 pmt = 0;
//
//	//10字节,包括从transport_stream_id到last_section_number，和CRC32
//	MUInt16 program_length = (section_header.section_length - 9) / 4;
//	for(MUInt32 i = 0;i < program_length ;i++)
//	{
//		sid = get16(pat_data);
//		if (sid < 0)
//			break;
//
//		if (sid == 0)
//		{
//			//sid == 0,表示这是NIT，可以跳过
//			continue;
//		}
//		//sid节目号为0x0001时, 表示这是PMT，PID = 0x1000，即4096
//
//		pmt = get16(pat_data);
//		if (pmt < 0)
//		{
//			break;
//		}
//		pmt &= 0x1fff;
//		if (pmt != PMT_PID)
//		{
//			return -1;
//		}
//		m_isFoundPmt = MTrue;
//		//目前只支持一个pmt,so break
//		break;
//	}
//}

MUInt32 TsStream::pmt_cb(MByte* section_start_pos)
{


	//SectionHeader section_header;
	//MInt32 ret = parse_section_header(section_start_pos, section_header);
	//if (ret != 0)
	//{
	//	return -1;
	//}
	//MByte* pData = section_start_pos + SECTION_HEADER_SIZE_8_BYTE;
	//if (section_header.tid != PMT_TID)
	//	return 0;


	//m_pcr_pid = get16(pData);
	//if (m_pcr_pid < 0)
	//	return 0;
	//m_pcr_pid &= 0x1fff;

	//MUInt16 program_info_length = get16(pData);
	//pData += 2;
	//if (program_info_length < 0)
	//	return -1;
	//program_info_length &= 0xfff;

	////跳过program_info部分
	//pData += program_info_length;
	//
	//while (true)
	//{
	//	//27 0x1b H264
	//	m_streamVideo.stream_type = *pData;
	//	pData++;
	//	m_streamVideo.pid = get16(pData) & 0x1fff;
	//	MUInt16  desc_list_len = get16(pData);
	//	if (desc_list_len < 0)
	//		return -1;
	//	desc_list_len &= 0xfff;



	//	//15 0xF为AAC
	//	m_streamAudio.stream_type = section_start_pos[17];
	//	m_streamAudio.pid = to_UInt16(section_start_pos + 18) & 0x1fff;
	//	desc_list_len = to_UInt16(section_start_pos + 20);
	//	if (desc_list_len < 0)
	//		return -1;
	//	desc_list_len &= 0xfff;
	//}


}


//MInt32 TsStream::parse_section_header(MByte* buffer_section_header,SectionHeader &section_header)
//{
//	int val;
//
//	val = buffer_section_header[0];	//0 byte
//	buffer_section_header++;
//	if (val < 0)
//		return -1;
//
//	section_header.tid = val;
//
//	section_header.section_length = get16(buffer_section_header) & 0xFFF;	
//
//	val = get16(buffer_section_header);	//3 byte
//	if (val < 0)
//		return val;
//
//	section_header.id = val;
//
//	val = *buffer_section_header;	//5 byte
//	if (val < 0)
//		return val;
//	section_header.version = (val >> 1) & 0x1f;
//	val = *buffer_section_header;	//6 byte
//	if (val < 0)
//		return val;
//	section_header.sec_num = val;
//	val = *buffer_section_header;	//7 byte
//	if (val < 0)
//		return val;
//	section_header.last_sec_num = val;
//	return 0;
//}


inline MInt64 TsStream::ff_parse_pes_pts(const MUInt8 *buf) {
	return (MInt64)(*buf & 0x0e) << 29 |
		(AV_RB16(buf + 1) >> 1) << 15 |
		AV_RB16(buf + 3) >> 1;
}

tsFilter* TsStream::add_filter(MUInt32 pid)
{
	for (int i = 0;i<FILTER_NUM;i++)
	{
		if (m_filter[i] == MNull)
		{
			m_filter[i] = CreateFilter(pid);
			return m_filter[i];
		}
	}

	return MNull;
}

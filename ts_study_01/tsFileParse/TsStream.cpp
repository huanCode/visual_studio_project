#include "stdafx.h"
#include "TsStream.h"
#include "common.h"


#define PACKET_SIZE	188
#define SECTION_HEADER_SIZE_8_BYTE		8
#define PAT_PID                 0x0000
#define SDT_PID                 0x0011		//17
#define PMT_PID					0x1000		//4096
#define EMPTY_PID               0x1FFF

/* table ids */
#define PAT_TID   0x00
#define CAT_TID	  0x01
#define PMT_TID   0x02
//#define NIT_TID	  0xXX	˽�ñ�tid�Զ���

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
	
	while (file.Read(packet, PACKET_SIZE))
	{
		//printf("i = %d ,curpos = %d\n",i++,file.GetFileCurPos());
		ret = parse_ts(packet);
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

MBool TsStream::probe(MPByte p_buffer, MUInt32 p_size)
{
	MUInt32 check_count = p_size / TS_PACKET_SIZE;
	if (check_count <= 0)
	{
		return MFalse;
	}


}

MUInt32 TsStream::parse_ts(MByte* buffer_packet)
{
	ts_header tsHeader;
	parse_ts_header(buffer_packet, tsHeader);

	if (tsHeader.sync_byte != 0x47)
	{
		return -1;
	}

	if (tsHeader.transport_error)
	{
		return -1;
	}

	if (tsHeader.pid == 0x1FFF || tsHeader.has_payload == false)
	{
		return 0;
	}



	//if (tsHeader.has_adaptation)
	//{
	//	//������Ӧ�ֶ�
	//	//adaptation_field_length	1B	����Ӧ�򳤶ȣ�������ֽ�����������adaptation_field_length����������Ӧ����Ӧ��Ϊ adaptation_field_length + 1
	//	//flag						1B	ȡ0x50��ʾ����PCR��0x40��ʾ������PCR
	//	//PCR						5B	Program Clock Reference����Ŀʱ�Ӳο������ڻָ���������һ�µ�ϵͳʱ��ʱ��STC��System Time Clock����
	//	//stuffing_bytes			xB	����ֽڣ�ȡֵ0xff
	//}
	

	MUInt16 adaptation_length = 0;
	if (tsHeader.has_adaptation)
	{
		//����pesʱ��adaptation��������ֶ�
		
		adaptation_length = buffer_packet[4] + 1;
		if (adaptation_length > 1)
		{
			bool is_discontinuity = buffer_packet[5] & 0x80;
			if (is_discontinuity)
			{
				int i = 0;
			}
			//printf("is_discontinuity = %d =====================\n", is_discontinuity);
		}
		//printf("has_adaptation ,adaptation_length = %d\n", adaptation_length);
	}
	//printf("continuity_counter = %d\n", tsHeader.continuity_counter);

	

	MUInt16 point_field_length = 0;
	if (tsHeader.pid == SDT_PID || tsHeader.pid == PAT_PID || tsHeader.pid == PMT_PID )
	{
		//section	
 		if (tsHeader.bStart_payload)
		{
			MByte* point_field = buffer_packet + TS_PACKET_HEADER_SIZE + adaptation_length;
			point_field_length = point_field[0] + 1;	
			//printf("bStart_payload ,point_field_length = %d\n", point_field_length);
		}

	}


	MByte* buffer_packet_section_strat_pos = buffer_packet + TS_PACKET_HEADER_SIZE + adaptation_length + point_field_length;
	MUInt32	buffer_remain_size = PACKET_SIZE - adaptation_length - TS_PACKET_HEADER_SIZE - point_field_length;


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

MUInt32 TsStream::parse_frame(MByte* buffer,MUInt32 buffer_size,MBool is_start)
{
	//is_start = 1����ʾ���ʱpes�Ŀ�ʼ
	//is_start = 0,��ʾ���ʱpes�еĺ����ֽ�
	MBool start = MFalse;
	if (is_start)
	{
		//��ʾ��һ֡��ȡ���
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



MVoid TsStream::parse_ts_header(MByte* buffer, ts_header &tsHeader)
{
	tsHeader.sync_byte = buffer[0];

	MUInt16 ts_header_23byte = to_UInt16(buffer + 1);

	//��1����ʾ����صĴ������������һ�����ɾ����Ĵ���λ��
	tsHeader.transport_error = ts_header_23byte & 0x8000;

	//Ϊ1ʱ����ʾ��ǰTS������Ч�غ��а���PES����PSI����ʼλ�ã�
	//��ǰ4���ֽ�֮�����һ�������ֽڣ������ֵΪ��������ֶεĳ���length�������Ч�غɿ�ʼ��λ��Ӧ��ƫ��1 + [length]���ֽڡ�
	tsHeader.bStart_payload = ts_header_23byte & 0x4000;

	tsHeader.pid = ts_header_23byte & 0x1fff;


	//������Ҫ���ж�adaptation_field_control����������Ʊ�־��
	//��ʾ��ͷ�Ƿ��е����ֶλ���Ч���ء�
	//��00��ΪISO/IECδ��ʹ�ñ�����
	//��01��������Ч�غɣ��޵����ֶΣ�
	//��10�� ����Ч�غɣ����������ֶΣ�
	//��11�� �����ֶκ�Ϊ��Ч�غɣ������ֶ��е�ǰһ���ֽڱ�ʾ�����ֶεĳ���length����Ч�غɿ�ʼ��λ��Ӧ��ƫ��[length]���ֽڡ�
	//�հ�ӦΪ��10����
	tsHeader.has_adaptation = buffer[3] & 0x20;	//
	tsHeader.has_payload	= buffer[3] & 0x10;
	tsHeader.continuity_counter = buffer[3] & 0xf;

}

MUInt32 TsStream::pat_cb(MByte* section_start_pos)
{
	SectionHeader section_header;
	MByte* pat_data = section_start_pos;
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

	//10�ֽ�,������transport_stream_id��last_section_number����CRC32
	MUInt16 program_length = (section_header.section_length - 9) / 4;
	for(MUInt32 i = 0;i < program_length ;i++)
	{
		sid = get16(pat_data);
		if (sid < 0)
			break;

		if (sid == 0)
		{
			//sid == 0,��ʾ����NIT����������
			continue;
		}
		//sid��Ŀ��Ϊ0x0001ʱ, ��ʾ����PMT��PID = 0x1000����4096

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
		//Ŀǰֻ֧��һ��pmt,so break
		break;
	}
}

MUInt32 TsStream::pmt_cb(MByte* section_start_pos)
{


	SectionHeader section_header;
	MInt32 ret = parse_section_header(section_start_pos, section_header);
	if (ret != 0)
	{
		return -1;
	}
	MByte* pData = section_start_pos + SECTION_HEADER_SIZE_8_BYTE;
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

	//����program_info����
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



		//15 0xFΪAAC
		m_streamAudio.stream_type = section_start_pos[17];
		m_streamAudio.pid = to_UInt16(section_start_pos + 18) & 0x1fff;
		desc_list_len = to_UInt16(section_start_pos + 20);
		if (desc_list_len < 0)
			return -1;
		desc_list_len &= 0xfff;
	}


}


MInt32 TsStream::parse_section_header(MByte* buffer_section_header,SectionHeader &section_header)
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


inline MInt64 TsStream::ff_parse_pes_pts(const MUInt8 *buf) {
	return (MInt64)(*buf & 0x0e) << 29 |
		(AV_RB16(buf + 1) >> 1) << 15 |
		AV_RB16(buf + 3) >> 1;
}
// tsFileParse.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "mv3File.h"
#include "TsStream.h"
#include "ParseHls.h"
#include "ToolList.h"
#include "ToolString.h"
#include "SourceParse.h"
inline MUInt16 to_UInt16(MByte* p)
{
	MUInt16 n = ((MByte*)p)[0];
	n <<= 8; 
	n += ((MByte*)p)[1]; return n;
}
inline MUInt8 to_UInt8(MByte* p)
{
	return *((MByte*)p);
}

int main()
{
	//ToolList<int*>  ll;
	//int* i1 = new int();
	//int* i2 = new int();
	//int* i3 = new int();
	//*i1 = 1;
	//*i2 = 2;
	//*i3 = 3;
	//ll.AddNode(i1);
	//ll.AddNode(i2);
	//ll.AddNode(i3);
	//int* ii = ll.GetNodePtrByIndex(4);

	SourceParse sourse;
	sourse.Open("http://devimages.apple.com/iphone/samples/bipbop/bipbopall.m3u8");	//Ƕ��

	MPChar tmp = "shende/r/ndfe";
	MChar line[100] = {0};
	ToolString::Read_line(tmp, sizeof(tmp), line, 100);
	//TsStream ts;
	//ts.mpegts_read_header();


	//ParseHls ps;
	//ps.Parse();

	int a = 1;
	return 0;
	//TsStream ts;
	//ts.mpegts_read_header();



	//mv3File file;
	//
	//if (!file.Open("76.ts", mv3File::stream_read))
	//{
	//	return -1;
	//}

	//MByte packet[188] = {0};
	//MByte packet2[188] = { 0 };
	////file.Read(packet,188);
	////file.Read(packet2, 188);
	////file.Read(packet2, 188);
	//if (packet[0] != 0x47)            // ts sync byte
	//	return -1;

	//MUInt16 ts_header_23byte = to_UInt16(packet+1);

	////��1����ʾ����صĴ������������һ�����ɾ����Ĵ���λ��
	//bool transport_error				= ts_header_23byte & 0x8000;
	//if (transport_error)
	//{
	//	return -1;
	//}
	////Ϊ1ʱ����ʾ��ǰTS������Ч�غ��а���PES����PSI����ʼλ�ã�
	////��ǰ4���ֽ�֮�����һ�������ֽڣ������ֵΪ��������ֶεĳ���length�������Ч�غɿ�ʼ��λ��Ӧ��ƫ��1 + [length]���ֽڡ�
	//bool payload_unit_start_indicator	= ts_header_23byte & 0x4000;
	//MUInt16	pid = ts_header_23byte & 0x1fff;
	//MUInt8 ts_header_4byte = to_UInt8(packet + 3);
	////������Ҫ���ж�adaptation_field_control����������Ʊ�־��
	////��ʾ��ͷ�Ƿ��е����ֶλ���Ч���ء�
	////��00��ΪISO/IECδ��ʹ�ñ�����
	////��01��������Ч�غɣ��޵����ֶΣ�
	////��10�� ����Ч�غɣ����������ֶΣ�
	////��11�� �����ֶκ�Ϊ��Ч�غɣ������ֶ��е�ǰһ���ֽڱ�ʾ�����ֶεĳ���length����Ч�غɿ�ʼ��λ��Ӧ��ƫ��[length]���ֽڡ�
	////�հ�ӦΪ��10����
	//bool adaptation_field_exist		= ts_header_4byte & 0x20;	//
	//bool payload_data_exist			= ts_header_4byte & 0x10;


	////------------------------------------------
	//MUInt8 payload_data_length = to_UInt8(packet + 4);
    return 0;
}


// tsFileParse.cpp : 定义控制台应用程序的入口点。
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
	sourse.Open("http://devimages.apple.com/iphone/samples/bipbop/bipbopall.m3u8");	//嵌套

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

	////‘1’表示在相关的传输包中至少有一个不可纠正的错误位。
	//bool transport_error				= ts_header_23byte & 0x8000;
	//if (transport_error)
	//{
	//	return -1;
	//}
	////为1时，表示当前TS包的有效载荷中包含PES或者PSI的起始位置；
	////在前4个字节之后会有一个调整字节，其的数值为后面调整字段的长度length。因此有效载荷开始的位置应再偏移1 + [length]个字节。
	//bool payload_unit_start_indicator	= ts_header_23byte & 0x4000;
	//MUInt16	pid = ts_header_23byte & 0x1fff;
	//MUInt8 ts_header_4byte = to_UInt8(packet + 3);
	////下面主要是判断adaptation_field_control（适配域控制标志）
	////表示包头是否有调整字段或有效负载。
	////‘00’为ISO/IEC未来使用保留；
	////‘01’仅含有效载荷，无调整字段；
	////‘10’ 无有效载荷，仅含调整字段；
	////‘11’ 调整字段后为有效载荷，调整字段中的前一个字节表示调整字段的长度length，有效载荷开始的位置应再偏移[length]个字节。
	////空包应为‘10’。
	//bool adaptation_field_exist		= ts_header_4byte & 0x20;	//
	//bool payload_data_exist			= ts_header_4byte & 0x10;


	////------------------------------------------
	//MUInt8 payload_data_length = to_UInt8(packet + 4);
    return 0;
}


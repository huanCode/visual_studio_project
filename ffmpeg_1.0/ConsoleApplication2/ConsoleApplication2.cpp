// ConsoleApplication2.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#pragma comment(lib,"ws2_32.lib")
extern "C"
{
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
};
int main()
{
	av_register_all();
	AVOutputFormat *ofmt = NULL;
	//输入对应一个AVFormatContext，输出对应一个AVFormatContext
	//（Input AVFormatContext and Output AVFormatContext）
	AVFormatContext *ifmt_ctx = NULL, *ofmt_ctx = NULL;
	AVPacket pkt;
	const char *in_filename, *out_filename;
	int ret, i;
	//in_filename = "ad.mp4";
	//输入文件名（Input file URL）
						   //in_filename  = "cuc_ieschool1.flv";//输入文件名（Input file URL）
							//in_filename = "http://fnmm6ksw.vod1.hongshiyun.net/archive/hls/20160828/demo.m3u8";	//嵌套hls
						   //in_filename = "https://dco4urblvsasc.cloudfront.net/811/81095_ywfZjAuP/game/index.m3u8";	//嵌套hls
						   //in_filename = "http://hlsglsb.wasu.tv/1480682957527_561859.m3u8?action=hls&Contentid=CP23010020161201084109";
						   in_filename = "http://playertest.longtailvideo.com/adaptive/bipbop/gear4/prog_index.m3u8";
						   //in_filename = "rtmp://live.hkstv.hk.lxdns.com/live/hks";
	out_filename = "cuc_ieschool1.mp4";//输出文件名（Output file URL）	av_register_all();
https://dco4urblvsasc.cloudfront.net/811/81095_ywfZjAuP/game/500kbps.m3u8
	//输入（Input）
	if ((ret = avformat_open_input(&ifmt_ctx, in_filename, 0, 0)) < 0) {
		printf("Could not open input file.");
		return -1;
	}
	if ((ret = avformat_find_stream_info(ifmt_ctx, 0)) < 0) {
		printf("Failed to retrieve input stream information");
		goto end;
	}
	//av_dump_format(ifmt_ctx, 0, in_filename, 0);
	//输出（Output）
	//avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, out_filename);
	//if (!ofmt_ctx) {
	//	printf( "Could not create output context\n");
	//	ret = AVERROR_UNKNOWN;
	//	goto end;
	//}

	int videoindex = 0;
	for (i = 0; i < ifmt_ctx->nb_streams; i++) {
		if (ifmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoindex = i;
			break;
		}
	}
	AVCodecContext  *pCodecCtx;
	pCodecCtx = ifmt_ctx->streams[videoindex]->codec;

	AVCodec *pCodec = avcodec_find_decoder(AV_CODEC_ID_H264);
	avcodec_open2(pCodecCtx, pCodec, 0);

	AVFrame *pFrame;
	pFrame = av_frame_alloc();


	//ofmt = ofmt_ctx->oformat;
	//for (i = 0; i < ifmt_ctx->nb_streams; i++) {
	//	//根据输入流创建输出流（Create output AVStream according to input AVStream）
	//	AVStream *in_stream = ifmt_ctx->streams[i];
	//	AVStream *out_stream = avformat_new_stream(ofmt_ctx, in_stream->codec->codec);
	//	if (!out_stream) {
	//		printf( "Failed allocating output stream\n");
	//		ret = AVERROR_UNKNOWN;
	//		goto end;
	//	}
	//	//复制AVCodecContext的设置（Copy the settings of AVCodecContext）
	//	if (avcodec_copy_context(out_stream->codec, in_stream->codec) < 0) {
	//		printf( "Failed to copy context from input to output stream codec context\n");
	//		goto end;
	//	}
	//	out_stream->codec->codec_tag = 0;
	//	if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
	//		out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
	//}


	//输出一下格式------------------
	//av_dump_format(ofmt_ctx, 0, out_filename, 1);
	//打开输出文件（Open output file）
	//if (!(ofmt->flags & AVFMT_NOFILE)) {
	//	ret = avio_open(&ofmt_ctx->pb, out_filename, AVIO_FLAG_WRITE);
	//	if (ret < 0) {
	//		printf( "Could not open output file '%s'", out_filename);
	//		goto end;
	//	}
	//}
	//写文件头（Write file header）
	//if (avformat_write_header(ofmt_ctx, NULL) < 0) {
	//	printf( "Error occurred when opening output file\n");
	//	goto end;
	//}
	int frame_index = 0;
	int video_index = 0;
	int audio_index = 0;
	int64_t delta_dts = INT64_MIN;
	int got_picture = 0;
	while (1) {
		AVStream *in_stream, *out_stream;
		//获取一个AVPacket（Get an AVPacket）
		av_seek_frame(ifmt_ctx, 0, 14500, AVSEEK_FLAG_FRAME);
		ret = av_read_frame(ifmt_ctx, &pkt);
		if (ret < 0)
			break;

		frame_index++;
		av_free_packet(&pkt);

	}
	//写文件尾（Write file trailer）
	//av_write_trailer(ofmt_ctx);
end:
	avformat_close_input(&ifmt_ctx);
	/* close output */
	if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
		avio_close(ofmt_ctx->pb);
	avformat_free_context(ofmt_ctx);
	if (ret < 0 && ret != AVERROR_EOF) {
		printf("Error occurred.\n");
		return -1;
	}
	return 0;
}


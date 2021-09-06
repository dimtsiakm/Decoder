#pragma once
#include "pch.h"

#include "MasterClass.h"

static enum AVPixelFormat hw_pix_fmt;

static int read_packet(void* opaque, uint8_t* buf, int buf_size)
{
	struct buffer_data* bd = (struct buffer_data*)opaque;
	buf_size = FFMIN(buf_size, bd->size);

	printf("ptr:%p size:%zu\n", bd->ptr, bd->size);

	/* copy internal buffer data to buf */
	memcpy(buf, bd->ptr, buf_size);
	bd->ptr += buf_size;
	bd->size -= buf_size;

	return buf_size;
}
s_dimension* MasterClass::init_ffmpeg(uint8_t* buffer_in, int buffer_size_in)
{
	bd.ptr = buffer_in;
	bd.size = buffer_size_in;
	if (!(fmt_ctx = avformat_alloc_context())) {
		printf("Error : avformat_alloc_context\n");
		return NULL;
	}
	avio_ctx_buffer = static_cast<uint8_t*>(av_malloc(avio_ctx_buffer_size));
	if (!avio_ctx_buffer) {
		printf("Error : avio_ctx_buffer = av_malloc(avio_ctx_buffer_size)\n");
		return NULL;
	}
	avio_ctx = avio_alloc_context(avio_ctx_buffer, avio_ctx_buffer_size, 0, &bd, &read_packet, NULL, NULL);
	if (!avio_ctx) {
		printf("Error : avio_ctx = avio_alloc_context();\n");
		return NULL;
	}
	fmt_ctx->pb = avio_ctx;

	ret = avformat_open_input(&fmt_ctx, NULL, NULL, NULL);
	if (ret < 0) {
		printf("Could not open input\n");
		return NULL;
	}
	ret = avformat_find_stream_info(fmt_ctx, NULL);
	if (ret < 0) {
		printf("Could not find stream information\n");
		return NULL;
	}
	codec_ctx = avcodec_alloc_context3(NULL);

	if (ret = avcodec_parameters_to_context(codec_ctx, fmt_ctx->streams[0]->codecpar) < 0)
	{
		printf("Cannot get codec parameters\n");
		return NULL;
	}

	ret = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &Codec, 0);
	if (ret < 0) {
		printf("Could not find %s stream in input file\n");
		return NULL;
	}

	if (ret = avcodec_open2(codec_ctx, Codec, NULL) < 0)
	{
		printf("Cannot open video decoder\n");
		return NULL;
	}
	pkt = av_packet_alloc();
	av_init_packet(pkt);
	if (!pkt)
	{
		return NULL;
	}
	if (ret = av_read_frame(fmt_ctx, pkt) < 0)
	{
		printf("cannot read frame");
		return NULL;
	}

	frame = av_frame_alloc();

	struct_export = (s_decoded_frame*)malloc(sizeof(s_decoded_frame));
	dimension_export = (s_dimension*)malloc(sizeof(s_dimension));
	yuvPlanes = (sYUVplanes*)malloc(sizeof(sYUVplanes));

	dimension_export->width = codec_ctx->width;
	dimension_export->height = codec_ctx->height;

	return dimension_export;
}
sYUVplanes* MasterClass::decode_ffmpeg(uint8_t* data, int length)
{
	if (av_packet_from_data(pkt, data, length) < 0) {
		printf("Error : av_packet_from_data\n");
		return NULL;
	}
	return decode(codec_ctx, frame, pkt);
}
sYUVplanes* MasterClass::decode(AVCodecContext* dec_ctx, AVFrame* frame, AVPacket* pkt)
{
	ret = avcodec_send_packet(dec_ctx, pkt);
	if (ret < 0) {
		fprintf(stderr, "Error sending a packet for decoding\n");
		return NULL;
	}
	ret = avcodec_receive_frame(dec_ctx, frame);
	if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
		printf("AVERROR(EAGAIN) or AVERROR_EOF");
		return NULL;
	}
	else if (ret < 0) {
		fprintf(stderr, "Error during decoding\n");
		return NULL;
	}	
	yuvPlanes->Y = frame->data[0];
	yuvPlanes->U = frame->data[1];
	yuvPlanes->V = frame->data[2];

	return yuvPlanes;
}

void MasterClass::unref_YUVPlanes_structure(uint8_t* avframe) {
	printf("printf :: avframe, *avframe, &avframe = %p, %p, %p\n", avframe, *avframe, &avframe);
	//AVFrame** frame = (AVFrame**)avframe;
	//av_frame_free(frame);
	//free(avframe);
}
void MasterClass::clear_ffmpeg()
{
	av_file_unmap(buffer, buffer_size);
	av_free(codec_ctx);
	av_free(pkt);
	av_frame_free(&frame);

	avformat_close_input(&fmt_ctx);
	av_free(avio_ctx);
	av_free(avio_ctx_buffer);

	free(struct_export);
	free(dimension_export);
}
int MasterClass::check_hardware_device_support(int i)
{
	static const char* const hw_type_names[] = {
		 "cuda",
		 "drm",
		 "dxva2",
		 "d3d11va",
		 "opencl",
		 "qsv",
		 "vaapi",
		 "vdpau",
		 "videotoolbox",
		 "mediacodec",
	};
	if (i >= 0 && i < 10) {
		static AVBufferRef* hw_device_ctx = NULL;
		enum AVHWDeviceType t;
		t = av_hwdevice_find_type_by_name(hw_type_names[i]);
		int err = 0;
		if ((err = av_hwdevice_ctx_create(&hw_device_ctx, t,
			NULL, NULL, 0)) < 0) {
			fprintf(stderr, "Failed to create specified HW device.\n");
			return err;
		}
		return 100;//SUCCESS
	}
	else {
		return -99;//ERROR
	}
	
}
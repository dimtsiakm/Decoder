#pragma once
#pragma once
#include "pch.h"

#include <stdio.h>
#include <string.h>
#include <Windows.h>
#include <cstdint>
#include <chrono>

extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/frame.h>
#include <libavutil/file.h>
#include <libavformat/avio.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

#undef main

struct buffer_data {
	uint8_t* ptr;
	size_t size; ///< size left in the buffer
};
struct s_dimension {
	int width;
	int height;
};
struct s_decoded_frame {
	uint8_t* data; //data as rgb or yuv format; set the dependencies
	int linesize;
	int width;
	int height;
	int time_avcodec_send_packet;
	int time_avcodec_receive_frame;
	int time_avcodec_sws_scale;
	int buffer_size;
};
struct sYUVplanes {
	uint8_t* avframe;
	uint8_t* Y; //data as rgb or yuv format; set the dependencies
	uint8_t* U;
	uint8_t* V;
};

typedef std::chrono::high_resolution_clock Time;
typedef std::chrono::microseconds us;
typedef std::chrono::duration<float> fsec;

class HardwareAcceleration
{
public:
	s_dimension* init_ffmpeg(uint8_t*, int);
	s_dimension* init_ffmpeg_hw_acceleration(uint8_t*, int);
	uint8_t* decode_ffmpeg_hw_acceleration(uint8_t*, int);
private:
	AVBufferRef* hw_device_ctx = NULL;
	enum AVHWDeviceType type;

	AVCodec* Codec = NULL;
	AVCodecContext* codec_ctx = NULL;
	AVPacket* pkt = NULL;
	AVFormatContext* fmt_ctx = NULL;
	AVIOContext* avio_ctx = NULL;

	struct SwsContext* sws_ctx = NULL;

	uint8_t* buffer = NULL, * avio_ctx_buffer = NULL;
	int numBytes = 0;
	uint8_t* buffer_rgb;
	size_t buffer_size, avio_ctx_buffer_size = 1024 * 180;
	int ret = 0;
	struct buffer_data bd = { 0 };

	s_decoded_frame* struct_export;
	s_dimension* dimension_export;

	bool hardware_acceleration = false;
	const char* device_name = "d3d11va";

	uint8_t* decode_hw_acceleration(AVCodecContext*, AVFrame*, AVPacket*);
};



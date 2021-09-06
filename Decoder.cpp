#pragma once
#include "pch.h"
#include "Decoder.h"
#include "MasterClass.h"

MasterClass* pClass;

s_dimension* init(uint8_t* data, int length)
{
	pClass = new MasterClass();
	s_dimension* dims = pClass->init_ffmpeg(data, length);
	return dims;
}

sYUVplanes* decode(uint8_t* data, int length)
{
	sYUVplanes* pointer = pClass->decode_ffmpeg(data, length);
	return pointer;
}

void clear()
{
	pClass->clear_ffmpeg();
}

void unref_YUVPlanes_structure(uint8_t* avframe)
{
	pClass->unref_YUVPlanes_structure(avframe);
}

int test(int a,int b)
{
	return a + b;
}

int check_hardware_device_support(int i) {
	return pClass->check_hardware_device_support(i);
}
//
//s_dimension* init_hw_acceleration(uint8_t* data, int length)
//{
//	pClass = new MasterClass();
//	s_dimension* dims = pClass->init_ffmpeg_hw_acceleration(data, length);
//	return dims;
//}
//
//uint8_t* decode_hw_acceleration(uint8_t* data, int length)
//{
//	uint8_t* pointer = pClass->decode_ffmpeg_hw_acceleration(data, length);
//	return pointer;
//}
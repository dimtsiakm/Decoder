#pragma once

#include <stdio.h>
#include <string.h>
#include <Windows.h>

#include "MasterClass.h"
extern "C"
{
	
}

extern "C" __declspec(dllexport) s_dimension* init(uint8_t * data, int length);
//extern "C" __declspec(dllexport) s_dimension* init_hw_acceleration(uint8_t * data, int length);
extern "C" __declspec(dllexport) sYUVplanes* decode(uint8_t * data, int length);
//extern "C" __declspec(dllexport) uint8_t* decode_hw_acceleration(uint8_t * data, int length);
extern "C" __declspec(dllexport) void clear();
extern "C" __declspec(dllexport) void unref_YUVPlanes_structure(uint8_t* avframe);
extern "C" __declspec(dllexport) int test(int a, int b);
extern "C" __declspec(dllexport) int check_hardware_device_support(int i);

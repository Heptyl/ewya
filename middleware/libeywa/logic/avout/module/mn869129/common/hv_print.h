/*
 * Copyright (C) 2015-2020 Socionext Inc.
 */
#ifndef __HV_PRINT_H__
#define __HV_PRINT_H__

#include <stdio.h>
// please define "HV_DEBUG", if you want to print debug message.
//#define HV_DEBUG

#define MAX_LINE		256

#ifdef HV_DEBUG
	#define DBG(fmt, ...)	fprintf(stdout, fmt "\n", ##__VA_ARGS__)
	#define DBGn(fmt, ...)	fprintf(stdout, fmt, ##__VA_ARGS__)
	#define DBGM(fmt, ...)	fprintf(stdout, "%s:%d: " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
	#define DBGMn(fmt, ...)	fprintf(stdout, "%s:%d: " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
	#define DBG(fmt, ...)	{}
	#define DBGn(fmt, ...)	{}
	#define DBGM(fmt, ...)	{}
	#define DBGMn(fmt, ...)	{}
#endif // HV_DEBUG


#if HV_DEBUG
#define MES(fmt, ...)		fprintf(stdout, fmt "\n", ##__VA_ARGS__)
#define MESn(fmt, ...)		fprintf(stdout, fmt, ##__VA_ARGS__)
#define MESi(fmt, ...)		fprintf(stdout, "    " fmt "\n", ##__VA_ARGS__)
#define MESin(fmt, ...)		fprintf(stdout, "    " fmt, ##__VA_ARGS__)
#define MESii(fmt, ...)		fprintf(stdout, "        " fmt "\n", ##__VA_ARGS__)
#define MESiin(fmt, ...)	fprintf(stdout, "        " fmt , ##__VA_ARGS__)
#define MESiii(fmt, ...)	fprintf(stdout, "            " fmt "\n", ##__VA_ARGS__)
#define WAR(fmt, ...)		fprintf(stdout, "[WARNING] " fmt "\n", ##__VA_ARGS__)
#define ERR(fmt, ...)		fprintf(stderr, "[ERROR:%d] " fmt "\n", __LINE__, ##__VA_ARGS__)
#else
// #define MES(fmt, ...)       printf(fmt "\n", ##__VA_ARGS__)
// #define MESn(fmt, ...)
// #define MESi(fmt, ...)
// #define MESin(fmt, ...)
// #define MESii(fmt, ...)
// #define MESiin(fmt, ...)
// #define MESiii(fmt, ...)
// #define WAR(fmt, ...)
// #define ERR(fmt, ...)
#endif

#endif // __HV_PRINT_H__


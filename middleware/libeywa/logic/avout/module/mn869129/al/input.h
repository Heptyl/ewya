/*
 * Copyright (C) 2015-2020 Socionext Inc.
 */
#ifndef __INPUT_H__
#define __INPUT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#if defined(__linux)
#include <unistd.h>
#include <termios.h>
#endif // __linux

#if defined(__linux)
#define TERMINAL_MODE_DECLARATION				\
	struct termios cooked_mode;					\
	struct termios raw_mode;
#define TERMINAL_MODE_INIT() {					\
	tcgetattr(STDIN_FILENO, &cooked_mode);		\
	raw_mode = cooked_mode;						\
	cfmakeraw(&raw_mode);						\
	tcsetattr(STDIN_FILENO, 0, &raw_mode);		\
}
#define TERMINAL_MODE_EXIT() {					\
	tcsetattr(STDIN_FILENO, 0, &cooked_mode);	\
	putchar('\n');								\
}
#elif defined(_WIN32)
#define TERMINAL_MODE_DECLARATION
#define TERMINAL_MODE_INIT()
#define TERMINAL_MODE_EXIT()
#elif (1)
#define TERMINAL_MODE_DECLARATION
#define TERMINAL_MODE_INIT()
#define TERMINAL_MODE_EXIT()
#endif // __linux __XC16

uint32_t input(char *buf, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif // __INPUT_H__

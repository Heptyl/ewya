/*
 * Copyright (C) 2015-2020 Socionext Inc.
 */
#include <stdlib.h>
//#include <linux/string.h>

#include "input.h"
#include "hv_error.h"
#include "hv_print.h"

#if defined(__linux)
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#elif defined(_WIN32)
#elif defined(__XC16)
//#include "pic24_uart_ch1.h"
#endif // __linux __XC16

#if defined(__linux)
//------------------------------------------------------------------------------
static const char HISTORY_FILE_NAME[]=			"/.hvdiag_history";
static int hvdiag_fd = 0;
static char *hvdiag_buf = NULL;
static long hvdiag_size = 0;
static char history_file[MAX_LINE] = {0};
//------------------------------------------------------------------------------
static const char R_I_SEARCH[]=					"(reverse-i-search)`";
static const char FAILED_R_I_SEARCH[]=			"(failed reverse-i-search)`";
static const char I_SEARCH[]=					"(i-search)`";
static const char FAILED_I_SEARCH[]=			"(failed i-search)`";
static const char SEARCH_TAIL[]=				"': ";
#define R_I_SEARCH_SPOS							22
#define FAILED_R_I_SEARCH_SPOS					(R_I_SEARCH_SPOS + 7)
#define I_SEARCH_SPOS							14
#define FAILED_I_SEARCH_SPOS					(I_SEARCH_SPOS + 7)
#define SEARCH_TAIL_SIZE						3
//------------------------------------------------------------------------------
static uint32_t get_history(char *str, int index)
{
	long i;
	int idx;
	char *p;

	if (!hvdiag_fd) {
		if (history_file[0] == '\0') {
			strcpy(history_file, getenv("HOME"));
			strcat(history_file, HISTORY_FILE_NAME);
		}
		if((hvdiag_fd = open(history_file, (O_RDWR | O_CREAT),
											(O_SYNC | O_DSYNC | S_IRUSR | S_IWUSR))) < 0) {
			goto _err_end1;
		}
		if ((hvdiag_size = lseek(hvdiag_fd, 0, SEEK_END)) < 0) {
			goto _err_end2;
		}
		if (lseek(hvdiag_fd, 0, SEEK_SET) < 0) {
			goto _err_end2;
		}
		if (hvdiag_buf) {
			free(hvdiag_buf);
		}
		hvdiag_buf = (char *)malloc(hvdiag_size);
		if (read(hvdiag_fd, hvdiag_buf, hvdiag_size) < 0) {
			goto _err_end3;
		}
	}

	idx=0;
	p=hvdiag_buf;
	for (i=0; i<hvdiag_size; i++) {
		if (hvdiag_buf[i] == '\n' || hvdiag_buf[i] == '\0') {
			hvdiag_buf[i] = '\0';
			if (idx == index) {
				strcpy(str, p);
				break;
			}
			p = &hvdiag_buf[i+1];
			idx++;
		}
	}
	if (i>=hvdiag_size) {
		return ERR_NG;
	}

	return ERR_OK;

_err_end3:
	free(hvdiag_buf);
	hvdiag_buf = NULL;
_err_end2:
	close(hvdiag_fd);
_err_end1:
	hvdiag_fd = 0;
	return ERR_NG;
}
//------------------------------------------------------------------------------
static uint32_t set_history(char *str)
{
	int len, idx, j;
	long i;
	char *p, *buf = NULL;

	len = strlen(str);
	//  if enter only, omit it
	if (len == 0) {
		return ERR_OK;
	}

	if (!hvdiag_fd) {
		if (history_file[0] == '\0') {
			strcpy(history_file, getenv("HOME"));
			strcat(history_file, HISTORY_FILE_NAME);
		}
		if((hvdiag_fd = open(history_file, (O_RDWR | O_CREAT),
											(O_SYNC | O_DSYNC | S_IRUSR | S_IWUSR))) < 0) {
			goto _err_end1;
		}
		if ((hvdiag_size = lseek(hvdiag_fd, 0, SEEK_END)) < 0) {
			goto _err_end2;
		}
		if (lseek(hvdiag_fd, 0, SEEK_SET) < 0) {
			goto _err_end2;
		}
		if (hvdiag_buf) {
			free(hvdiag_buf);
		}
		hvdiag_buf = (char *)malloc(hvdiag_size);
		if (read(hvdiag_fd, hvdiag_buf, hvdiag_size) < 0) {
			goto _err_end3;
		}
	}

	buf = (char *)malloc(hvdiag_size + len + 1);
	// store str to the top-level row
	idx = 0;
	for (j=0; j<len; j++) {
		buf[idx] = str[j];	idx++;
	}
	buf[idx] = '\n';		idx++;

	p=hvdiag_buf;
	for (i=0; i<hvdiag_size; i++) {
		if (hvdiag_buf[i] == '\n' || hvdiag_buf[i] == '\0') {
			hvdiag_buf[i] = '\0';
			if (strcmp(str, p)) {
				len = strlen(p);
				if (len > 0) {
					for (j=0; j<len; j++) {
						buf[idx] = p[j];	idx++;
					}
					buf[idx] = '\n';		idx++;
				}
			}
			p = &hvdiag_buf[i+1];
		}
	}

	// save history file
	if (lseek(hvdiag_fd, 0, SEEK_SET) < 0) {
		goto _err_end4;
	}
	if (write(hvdiag_fd, buf, idx) < 0) {
		goto _err_end4;
	}

	free(buf);
	free(hvdiag_buf);
	hvdiag_buf = NULL;
	close(hvdiag_fd);
	hvdiag_fd = 0;
	return ERR_OK;

_err_end4:
	free(buf);
_err_end3:
	free(hvdiag_buf);
	hvdiag_buf = NULL;
_err_end2:
	close(hvdiag_fd);
_err_end1:
	hvdiag_fd = 0;
	return ERR_NG;
}
//------------------------------------------------------------------------------
static uint32_t up_search(char *buf, long len, char *key, long *idx, char **ptr, long *pos)
{
	long i, j, sz;

	*ptr = "\0";
	*pos = 0;
	sz = strlen(key);

	for (i=*idx; i>=0; i--) {
		if (buf[i] == '\n') {
			buf[i] = '\0';
		}
		if (!strncmp(&buf[i], key, sz)) {
			*idx = i;
			for (j=i; j>=0; j--) {
				if (buf[j]=='\n' || buf[j]=='\0') {
					*ptr = &buf[j+1];
					break;
				} else if (j==0) {
					*ptr = &buf[j];
					break;
				}
			}
			*pos = (long)(&buf[i]) - (long)(*ptr);
			break;
		}
	}
	if (i<0) {
		return ERR_NG;
	}

	return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t down_search(char *buf, long len, char *key, long *idx, char **ptr, long *pos)
{
	long i, j, sz;

	*ptr = "\0";
	*pos = 0;
	sz = strlen(key);

	for (i=*idx; i<len; i++) {
		if (!strncmp(&buf[i], key, sz)) {
			*idx = i;
			for (j=i; j<len; j++) {
				if (buf[j] == '\n') {
					buf[j] = '\0';
					break;
				}
			}
			for (j=i; j>=0; j--) {
				if (buf[j]=='\n' || buf[j]=='\0') {
					*ptr = &buf[j+1];
					break;
				} else if (j==0) {
					*ptr = &buf[j];
					break;
				}
			}
			*pos = (long)(&buf[i]) - (long)(*ptr);
			break;
		}
	}
	if (i>=len) {
		return ERR_NG;
	}

	return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t search(char *buf, uint32_t len, uint32_t sec)
{
	uint32_t c, ec, e=0, i=0, l;
	long idx, pos;
	char *ptr;
	char str[MAX_LINE];

	if (!hvdiag_fd) {
		if (history_file[0] == '\0') {
			strcpy(history_file, getenv("HOME"));
			strcat(history_file, HISTORY_FILE_NAME);
		}
		if((hvdiag_fd = open(history_file, (O_RDWR | O_CREAT),
											(O_SYNC | O_DSYNC | S_IRUSR | S_IWUSR))) < 0) {
			goto _err_end1;
		}
		if ((hvdiag_size = lseek(hvdiag_fd, 0, SEEK_END)) < 0) {
			goto _err_end2;
		}
		if (lseek(hvdiag_fd, 0, SEEK_SET) < 0) {
			goto _err_end2;
		}
		if (hvdiag_buf) {
			free(hvdiag_buf);
		}
		hvdiag_buf = (char *)malloc(hvdiag_size);
		if (read(hvdiag_fd, hvdiag_buf, hvdiag_size) < 0) {
			goto _err_end3;
		}
	}

	str[0]='\0';
	ec = sec;
	if (sec == 0x12) {
		printf("\e[3G\e[0K%s\e[%dG%s%s", R_I_SEARCH, R_I_SEARCH_SPOS, str, SEARCH_TAIL);
		idx = hvdiag_size;
	} else if (sec == 0x13) {
		printf("\e[3G\e[0K%s\e[%dG%s%s", I_SEARCH, I_SEARCH_SPOS, str, SEARCH_TAIL);
		idx = 0;
	} else {
		return ERR_NG;
	}

	while (!((c = fgetc(stdin)) == '\r' || c == '\n')) {
		if (c==0x1B) e=(e==0) ? (e+1) : 0;
		if (c==0x5B) e=(e==1) ? (e+1) : 0;
		if (c==0x41) e=(e==2) ? (e+1) : 0;			// Up
		if (c==0x42) e=(e==2) ? (e+1) : 0;			// Down
		if (c==0x43) e=(e==2) ? (e+1) : 0;			// Right
		if (c==0x44) e=(e==2) ? (e+1) : 0;			// Left
		if (c==0x31) e=(e==2 || e>=4) ? (e+2) : 0;
		if (c==0x32) e=(e==2 || e>=4) ? (e+2) : 0;
		if (c==0x33) e=(e==2 || e>=4) ? (e+2) : 0;
		if (c==0x34) e=(e==2 || e>=4) ? (e+2) : 0;
		if (c==0x35) e=(e==2 || e>=4) ? (e+2) : 0;
		if (c==0x36) e=(e==2 || e>=4) ? (e+2) : 0;
		if (c==0x37) e=(e==2 || e>=4) ? (e+2) : 0;
		if (c==0x38) e=(e==2 || e>=4) ? (e+2) : 0;
		if (c==0x39) e=(e==2 || e>=4) ? (e+2) : 0;
		if (e==0) {
			if (c==0x08 || c==0x7F) {				// BS or Delete
				if (i<=0) {i=0;	continue;}
				i--;
				str[i]='\0';
			} else if (c==0x12) {					// Ctrl+r
				idx--;
				ec = c;
			} else if (c==0x13) {					// Ctrl+s
				idx++;
				ec = c;
			} else if (c==0x1B) {					// when ESC x2
				ptr = "\0";
				break;
			} else {
				str[i]=c;
				i++;
			}
		} else if (e==1) {
			continue;
		} else if (e==2) {
			continue;
		} else if (e==3) {
			e=0;
			continue;
		} else if (e>=4) {
			if (c==0x7E) {e=0;}
			continue;
		}
		str[i]='\0';
		if (ec==0x12) {
			l = strlen(str);
			if (up_search(hvdiag_buf, hvdiag_size, str, &idx, &ptr, &pos)) {
				printf("\e[3G\e[0K%s\e[%dG%s%s%s\e[%ldG",
						FAILED_R_I_SEARCH, FAILED_R_I_SEARCH_SPOS, str, SEARCH_TAIL, ptr,
						(long)(FAILED_R_I_SEARCH_SPOS + l + SEARCH_TAIL_SIZE + pos));
			} else {
				printf("\e[3G\e[0K%s\e[%dG%s%s%s\e[%ldG",
						R_I_SEARCH, R_I_SEARCH_SPOS, str, SEARCH_TAIL, ptr,
						(long)(R_I_SEARCH_SPOS + l + SEARCH_TAIL_SIZE + pos));
			}
		}
		if (ec==0x13) {
			l = strlen(str);
			if (down_search(hvdiag_buf, hvdiag_size, str, &idx, &ptr, &pos)) {
				printf("\e[3G\e[0K%s\e[%dG%s%s%s\e[%ldG",
						FAILED_I_SEARCH, FAILED_I_SEARCH_SPOS, str, SEARCH_TAIL, ptr,
						(long)(FAILED_I_SEARCH_SPOS + l + SEARCH_TAIL_SIZE + pos));
			} else {
				printf("\e[3G\e[0K%s\e[%dG%s%s%s\e[%ldG",
						I_SEARCH, I_SEARCH_SPOS, str, SEARCH_TAIL, ptr,
						(long)(I_SEARCH_SPOS + l + SEARCH_TAIL_SIZE + pos));
			}
		}
	}
	strncpy(buf, ptr, len);
	buf[len-1] = '\0';

	return ERR_OK;

_err_end3:
	free(hvdiag_buf);
	hvdiag_buf = NULL;
_err_end2:
	close(hvdiag_fd);
_err_end1:
	hvdiag_fd = 0;
	return ERR_NG;
}
//------------------------------------------------------------------------------
uint32_t input(char *buf, uint32_t len)
{
	uint32_t c, e=0, i=0;
	uint32_t ret;
	int idx = -1;

	while (!((c = fgetc(stdin)) == '\r' || c == '\n')) {
		if (c==0x1B) e=(e==0) ? (e+1) : 0;
		if (c==0x5B) e=(e==1) ? (e+1) : 0;
		if (c==0x41) e=(e==2) ? (e+1) : 0;			// Up
		if (c==0x42) e=(e==2) ? (e+1) : 0;			// Down
		if (c==0x43) e=(e==2) ? (e+1) : 0;			// Right
		if (c==0x44) e=(e==2) ? (e+1) : 0;			// Left
		if (c==0x31) e=(e==2 || e>=4) ? (e+2) : 0;
		if (c==0x32) e=(e==2 || e>=4) ? (e+2) : 0;
		if (c==0x33) e=(e==2 || e>=4) ? (e+2) : 0;
		if (c==0x34) e=(e==2 || e>=4) ? (e+2) : 0;
		if (c==0x35) e=(e==2 || e>=4) ? (e+2) : 0;
		if (c==0x36) e=(e==2 || e>=4) ? (e+2) : 0;
		if (c==0x37) e=(e==2 || e>=4) ? (e+2) : 0;
		if (c==0x38) e=(e==2 || e>=4) ? (e+2) : 0;
		if (c==0x39) e=(e==2 || e>=4) ? (e+2) : 0;
		if (e==0) {
			if (c==0x08 || c==0x7F) {				// BS or Delete
				if (i<=0) {i=0;	continue;}
				printf("\b\e[1P");
				i--;
			} else if (c==0x12 || c==0x13) {		// Ctrl+r or Ctrl+s
				ret = search(buf, len, c);
				if (ret) {
					return ret;
				}
				if ((i = strlen(buf)) != 0) {
					i++;
					printf("\e[3G\e[0K%s", buf);
					break;
				} else {
					printf("\e[3G\e[0K%s", buf);
				}
			} else {
				putchar(c);
				buf[i]=c;
				i++;
			}
		} else if (e==1) {
			continue;
		} else if (e==2) {
			continue;
		} else if (e==3) {
			if (c==0x41) {							// Up
				idx++;
				if (!get_history(buf, idx)) {
					printf("\e[3G\e[0K%s", buf);
					i = strlen(buf);
				} else {
					idx--;
				}
			}
			if (c==0x42) {							// Down
				idx--;
				if (idx < 0) {
					idx = 0;
				}
				if (!get_history(buf, idx)) {
					printf("\e[3G\e[0K%s", buf);
					i = strlen(buf);
				}
			}
			e=0;
		} else if (e>=4) {
			if (c==0x7E) {e=0;}
		}
	}
	i = (i>=len) ? (len-1) : i;
	buf[i] = '\0';
	ret = set_history(buf);
	if (ret) {
		return ret;
	}
	putchar('\n');
	return ERR_OK;
}
//------------------------------------------------------------------------------
#elif defined(_WIN32)
uint32_t input(char *buf, uint32_t len)
{
	if(fgets(buf, len, stdin) == NULL){
		ERR("Input error occurs.");
		return ERR_NG;
	}
	return ERR_OK;
}
//------------------------------------------------------------------------------
#elif defined(__XC16)
/*
uint32_t input(char *buf, uint32_t len)
{
	if(pic24_gets(buf, len) == NULL){
		ERR("Input error occurs.");
		return ERR_NG;
	}
	return ERR_OK;
}
*/
//------------------------------------------------------------------------------
#endif // __linux _WIN32 __XC16

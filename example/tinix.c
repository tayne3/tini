/*
 * MIT License
 *
 * Copyright (c) 2026 tayne3
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * 1. The above copyright notice and this permission notice shall be included in
 *    all copies or substantial portions of the Software.
 *
 * 2. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *    SOFTWARE.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tini/tini.h"

// -------------------------[STATIC DECLARATION]-------------------------

// newline
#ifdef _MSC_VER
#define STR_NEWLINE "\r\n"
#else
#define STR_NEWLINE "\n"
#endif

// 打印命令说明
static inline void ix_shell_help(int status);
// 打印项目版本
static inline void ix_shell_version(void);

// -------------------------[GLOBAL DEFINITION]-------------------------

int main(int argc, char *argv[]) {
	if (argc < 2) {
		ix_shell_help(1);
	}

	if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
		ix_shell_help(0);
	}

	if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0) {
		ix_shell_version();
	}

	if (strcmp(argv[1], "get") == 0) {
		if (argc < 6) {
			printf("Invalid number of arguments for 'get' command. Use 'help' command for instructions." STR_NEWLINE);
			return 1;
		}

		tini_t *tini = tini_create(argv[2]);
		if (!tini) {
			printf("Failed to create tini object." STR_NEWLINE);
			return 1;
		}
		tini_section_t *sec = tini_find_section(tini, argv[3]);
		if (!sec) {
			printf("%s" STR_NEWLINE, argv[5]);
		} else {
			tini_key_t *key   = tini_section_find_key(sec, argv[4]);
			const char *value = key ? tini_key_get_value(key) : argv[5];
			printf("%s" STR_NEWLINE, value);
		}
		tini_destroy(tini);
		return 0;
	}

	if (strcmp(argv[1], "set") == 0) {
		if (argc < 6) {
			printf("Invalid number of arguments for 'set' command. Use 'help' command for instructions." STR_NEWLINE);
			return 1;
		}
		tini_t *tini = tini_create(argv[2]);
		if (!tini) {
			printf("Failed to create tini object." STR_NEWLINE);
			return 1;
		}
		tini_section_t *sec = tini_get_section(tini, argv[3]);
		if (!sec) {
			printf("Failed to get or create section" STR_NEWLINE);
			return 1;
		}
		tini_key_t *key = tini_section_add_key(sec, argv[4], argv[5]);
		if (!key) {
			printf("Failed to set value" STR_NEWLINE);
			return 1;
		}
		tini_save_to(tini, argv[2]);
		tini_destroy(tini);
		return 0;
	}

	if (strcmp(argv[1], "rm") == 0) {
		if (argc < 5) {
			printf("Invalid number of arguments for 'rm' command. Use 'help' command for instructions." STR_NEWLINE);
			return 1;
		}
		tini_t *tini = tini_create(argv[2]);
		if (!tini) {
			printf("Failed to create tini object." STR_NEWLINE);
			return 1;
		}
		tini_section_t *sec = tini_find_section(tini, argv[3]);
		if (!sec) {
			printf("Section not found" STR_NEWLINE);
			return 1;
		}
		const int code = tini_section_remove_key(sec, argv[4]);
		if (code != 0) {
			printf("Failed to remove key" STR_NEWLINE);
			return 1;
		}
		tini_save_to(tini, argv[2]);
		tini_destroy(tini);
		return 0;
	}

	printf("Invalid command. Use '--help' command for instructions." STR_NEWLINE);
	return 1;
}

// -------------------------[STATIC DEFINITION]-------------------------

static inline void ix_shell_help(int status) {
	// clang-format off
    fprintf(stdout, 
        STR_NEWLINE
        "Usage: tinix <command> [options]" STR_NEWLINE
        STR_NEWLINE
        "Commands:" STR_NEWLINE
        STR_NEWLINE
        "  -h, --help                            Display this help information and exit." STR_NEWLINE
        STR_NEWLINE
        "  -v, --version                         Display version information and exit." STR_NEWLINE
        STR_NEWLINE
        "  get <path> <section> <key> [default]  Retrieve the value for 'key' within 'section'" STR_NEWLINE
        "                                        in the ini file specified by 'path'. If the key" STR_NEWLINE
        "                                        does not exist, 'default' is printed instead." STR_NEWLINE
        STR_NEWLINE
        "  set <path> <section> <key> <value>    Set the value of 'key' within 'section' in the ini" STR_NEWLINE
        "                                        file specified by 'path' to 'value'." STR_NEWLINE
        STR_NEWLINE
        "  rm <path> <section> <key>             Remove 'key' from 'section' in the ini file" STR_NEWLINE
        "                                        specified by 'path'." STR_NEWLINE
        STR_NEWLINE
        "Examples:" STR_NEWLINE
        STR_NEWLINE
        "  tinix get /tmp/test.ini Section Key default_value" STR_NEWLINE
        "      Retrieve the value of 'Key' in 'Section' from '/tmp/test.ini', or 'default_value'" STR_NEWLINE
        "      if the key does not exist." STR_NEWLINE
        STR_NEWLINE
        "  tinix set /example/example.ini Section Key value" STR_NEWLINE
        "      Set the value of 'Key' in 'Section' in '/example/example.ini' to 'value'." STR_NEWLINE
        STR_NEWLINE
        "  tinix rm ./config.ini Section Key" STR_NEWLINE
        "      Remove 'Key' from 'Section' in './config.ini'." STR_NEWLINE
        STR_NEWLINE
    );
	// clang-format on
	exit(status);
}

static inline void ix_shell_version(void) {
#ifndef TINI_VERSION
#define TINI_VERSION "1.0.0"
#endif
#ifndef TINI_COMPILER_ID
#define TINI_COMPILER_ID "unknown"
#endif
#ifndef TINI_COMPILER_VERSION
#define TINI_COMPILER_VERSION "unknown"
#endif
#ifndef TINI_BUILD_DATE
#define TINI_BUILD_DATE "unknown"
#endif
	fprintf(stdout, "tinix " TINI_VERSION " of " TINI_BUILD_DATE " (" TINI_COMPILER_ID " " TINI_COMPILER_VERSION
					")." STR_NEWLINE);
	exit(0);
}

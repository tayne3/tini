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
#ifndef TINI_INTERNAL_H
#define TINI_INTERNAL_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tini/tini.h"

#ifdef _MSC_VER
#define strcasecmp  stricmp
#define strncasecmp strnicmp
#else
#include <strings.h>
#endif

// -------------------------[MACROS]-------------------------

#ifdef _MSC_VER
#define STR_NEWLINE "\r\n"
#else
#define STR_NEWLINE "\n"
#endif

#define STR_ISEMPTY(_s) (!(_s) || !*(const char *)(_s))

// -------------------------[ERROR CODES]-------------------------

#define TINI_ERROR_FOREACH(F)                  \
	F(0, Normal, "normal")                     \
	F(1, PathInvalid, "path invalid")          \
	F(2, FileOpen, "file open failed")         \
	F(3, KeyNotFound, "key not found")         \
	F(4, SectionNotFound, "section not found") \
	F(5, MemoryAlloc, "memory allocation failed")

enum tini_error {
#define F(code, name, describe) TinyError_##name = code,
	TINI_ERROR_FOREACH(F)
#undef F
		TinyError_Max,
};

// -------------------------[INTERNAL DATA STRUCTURES]-------------------------

struct tini_key {
	char            *key;
	char            *value;
	struct tini_key *next;
};

struct tini_section {
	char                *name;
	struct tini_key     *keys;
	struct tini_section *next;
};

struct tini_s {
	struct tini_section *sections;
	int                  last_error;
};

// -------------------------[INTERNAL UTILITY FUNCTIONS]-------------------------

/**
 * @brief Duplicate a string (internal helper)
 */
char *tini_strdup(const char *s);

/**
 * @brief Process quoted value: remove outer quotes if present
 * @param s Input string (may be quoted or unquoted)
 * @return Newly allocated string with quotes removed (caller must free)
 */
char *tini_process_quoted_value(const char *s);

/**
 * @brief Free a key object
 */
void tini_free_key(tini_key_t *key);

/**
 * @brief Free a section object
 */
void tini_free_section(tini_section_t *section);

#endif  // TINI_INTERNAL_H

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
#include "internal.h"

// -------------------------[LIFECYCLE MANAGEMENT]-------------------------

tini_ptr_t tini_create(const char *path) {
	tini_ptr_t self = (tini_ptr_t)malloc(sizeof(tini_t));
	if (!self) {
		return NULL;
	}

	self->sections   = NULL;
	self->last_error = TinyError_Normal;

	if (path && !STR_ISEMPTY(path)) {
		tini_load(self, path);
	}
	return self;
}

tini_ptr_t tini_empty(void) {
	return tini_create(NULL);
}

void tini_destroy(tini_ptr_t self) {
	if (!self) {
		return;
	}

	tini_section_t *section = self->sections;
	while (section) {
		tini_section_t *next = section->next;
		tini_free_section(section);
		section = next;
	}
	free(self);
}

void tini_clear(tini_ptr_t self) {
	if (!self) {
		return;
	}

	tini_section_t *section = self->sections;
	while (section) {
		tini_section_t *next = section->next;
		tini_free_section(section);
		section = next;
	}
	self->sections   = NULL;
	self->last_error = TinyError_Normal;
}

// -------------------------[FILE I/O]-------------------------

int tini_load(tini_ptr_t self, const char *path) {
	if (!self) {
		return -1;
	}

	FILE *fp = fopen(path, "r");
	if (!fp) {
		return self->last_error = TinyError_FileOpen;
	}

	char           *line            = NULL;
	size_t          line_cap        = 0;
	tini_section_t *current_section = NULL;

	// Simple dynamic line reading loop
	while (1) {
		// Ensure buffer capacity
		if (line_cap == 0) {
			line_cap = 128;
			line     = (char *)malloc(line_cap);
			if (!line) {
				self->last_error = TinyError_MemoryAlloc;
				break;
			}
		}

		size_t len = 0;
		int    c;
		while ((c = fgetc(fp)) != EOF && c != '\n') {
			if (c == '\r') {
				continue;  // ignore CR independently
			}
			if (len + 1 >= line_cap) {
				size_t new_cap  = line_cap * 2;
				char  *new_line = (char *)realloc(line, new_cap);
				if (!new_line) {
					self->last_error = TinyError_MemoryAlloc;
					free(line);
					fclose(fp);
					return -1;
				}
				line     = new_line;
				line_cap = new_cap;
			}
			line[len++] = (char)c;
		}
		line[len] = '\0';

		if (len == 0 && c == EOF) {
			break;  // End of file
		}

		// Process line - Strip inline comments (respecting quotes)
		int   in_quote      = 0;
		char  quote_char    = 0;
		char *comment_start = NULL;

		for (char *p = line; *p; p++) {
			if (in_quote) {
				if (*p == quote_char) {
					in_quote = 0;
				}
			} else {
				if (*p == '"' || *p == '\'') {
					in_quote   = 1;
					quote_char = *p;
				} else if (*p == ';' || *p == '#') {
					comment_start = p;
					break;
				}
			}
		}
		if (comment_start) {
			*comment_start = '\0';
			len            = comment_start - line;
		}

		// Trim trailing whitespace
		while (len > 0 && isspace((unsigned char)line[len - 1])) {
			line[--len] = '\0';
		}

		// Trim leading whitespace
		char *start = line;
		while (*start && isspace((unsigned char)*start)) {
			start++;
		}

		if (*start == '\0') {
			if (c == EOF) {
				break;
			}
			continue;  // Empty line
		}

		// Handle Section
		if (*start == '[') {
			char *end = start + strlen(start) - 1;
			if (*end == ']') {
				*end            = '\0';
				current_section = tini_get_section(self, start + 1);
			}
			if (c == EOF) {
				break;
			}
			continue;
		}

		// Handle Key=Value
		char *equals = strchr(start, '=');
		if (equals && current_section) {
			*equals     = '\0';
			char *key   = start;
			char *value = equals + 1;

			// Trim key end spaces
			char *kend = key + strlen(key) - 1;
			while (kend > key && isspace((unsigned char)*kend)) {
				*kend-- = '\0';
			}

			// Trim value start spaces
			while (*value && isspace((unsigned char)*value)) {
				value++;
			}
			// Value end spaces already trimmed by line trimmer

			tini_section_add_key(current_section, key, value);
		}

		if (c == EOF) {
			break;
		}
	}

	if (line) {
		free(line);
	}
	fclose(fp);
	return 0;
}

int tini_save_to(tini_ptr_t self, const char *path) {
	if (!self) {
		return -1;
	}
	if (STR_ISEMPTY(path)) {
		return self->last_error = TinyError_PathInvalid;
	}
	FILE *fp = fopen(path, "w");
	if (!fp) {
		return self->last_error = TinyError_FileOpen;
	}

	tini_section_t *section = self->sections;
	while (section) {
		fprintf(fp, "[%s]\n", section->name);
		tini_key_t *key = section->keys;
		while (key) {
			fprintf(fp, "%s=%s\n", key->key, key->value);
			key = key->next;
		}
		fprintf(fp, "\n");
		section = section->next;
	}

	fclose(fp);
	return 0;
}

// -------------------------[ERROR HANDLING]-------------------------

int tini_last_error(tini_cptr_t self) {
	return self ? self->last_error : -1;
}

const char *tini_error_string(int code) {
	const char *descriptions[] = {
#define F(code, name, desc) desc,
		TINI_ERROR_FOREACH(F)
#undef F
	};
	if (code < 0 || code >= TinyError_Max) {
		return "Unknown error";
	}
	return descriptions[code];
}

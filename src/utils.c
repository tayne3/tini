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

// -------------------------[UTILITY FUNCTIONS]-------------------------

char *tini_strdup(const char *s) {
	if (!s) {
		return NULL;
	}
	const size_t len = strlen(s);
	char        *dup = (char *)malloc(len + 1);
	if (dup) {
		memcpy(dup, s, len + 1);
	}
	return dup;
}

char *tini_process_quoted_value(const char *s) {
	if (!s) {
		return NULL;
	}

	const size_t len = strlen(s);

	// Check if string has matching quotes
	if (len >= 2) {
		const char first = s[0];
		const char last  = s[len - 1];

		// If quotes match (double or single), remove them
		if ((first == '"' && last == '"') || (first == '\'' && last == '\'')) {
			// Allocate space for the unquoted string
			char *result = (char *)malloc(len - 1);  // len - 2 + 1 for null terminator
			if (result) {
				memcpy(result, s + 1, len - 2);  // Copy content between quotes
				result[len - 2] = '\0';
			}
			return result;
		}
	}

	// No quotes or mismatched quotes - just duplicate the string
	return tini_strdup(s);
}

void tini_free_key(tini_key_t *key) {
	if (key) {
		if (key->key) {
			free(key->key);
		}
		if (key->value) {
			free(key->value);
		}
		free(key);
	}
}

void tini_free_section(tini_section_t *section) {
	if (section) {
		if (section->name) {
			free(section->name);
		}
		tini_key_t *key = section->keys;
		while (key) {
			tini_key_t *next = key->next;
			tini_free_key(key);
			key = next;
		}
		free(section);
	}
}

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

// Check if tail contains only whitespace
static inline bool tini__is_blank_tail(const char *s) {
	while (*s) {
		if (!isspace((unsigned char)*s)) {
			return false;
		}
		s++;
	}
	return true;
}

// -------------------------[KEY MANAGEMENT]-------------------------

tini_key_t *tini_section_get_key(tini_section_t *section, const char *key) {
	if (!section || !key) {
		return NULL;
	}

	tini_key_t *k = tini_section_find_key(section, key);
	if (k) {
		return k;
	}

	return tini_section_add_key(section, key, "");
}

tini_key_t *tini_section_find_key(tini_section_t *section, const char *key) {
	if (!section || !key) {
		return NULL;
	}

	tini_key_t *curr = section->keys;
	while (curr) {
		if (strcasecmp(curr->key, key) == 0) {
			return curr;
		}
		curr = curr->next;
	}
	return NULL;
}

bool tini_section_has_key(tini_section_t *section, const char *key) {
	return tini_section_find_key(section, key) != NULL;
}

tini_key_t *tini_section_add_key(tini_section_t *section, const char *key, const char *value) {
	if (!section || !key) {
		return NULL;
	}

	tini_key_t *k = tini_section_find_key(section, key);
	if (k) {
		tini_key_set_value(k, value);
		return k;
	}

	k = (tini_key_t *)malloc(sizeof(tini_key_t));
	if (!k) {
		return NULL;
	}

	k->key        = tini_strdup(key);
	k->value      = tini_process_quoted_value(value ? value : "");
	k->next       = section->keys;
	section->keys = k;

	return k;
}

int tini_section_remove_key(tini_section_t *section, const char *key) {
	if (!section || !key) {
		return -1;
	}

	tini_key_t *curr = section->keys;
	tini_key_t *prev = NULL;

	while (curr) {
		if (strcasecmp(curr->key, key) == 0) {
			if (prev) {
				prev->next = curr->next;
			} else {
				section->keys = curr->next;
			}
			tini_free_key(curr);
			return 0;
		}
		prev = curr;
		curr = curr->next;
	}
	return TinyError_KeyNotFound;
}

// -------------------------[KEY ITERATION]-------------------------

tini_key_t *tini_section_first_key(const tini_section_t *section) {
	return section ? section->keys : NULL;
}

tini_key_t *tini_key_next(const tini_key_t *key) {
	return key ? key->next : NULL;
}

const char *tini_key_name(const tini_key_t *key) {
	return key ? key->key : NULL;
}

// -------------------------[VALUE ACCESS & CONVERSION]-------------------------

void tini_key_set_value(tini_key_t *key, const char *value) {
	if (key) {
		if (key->value) {
			free(key->value);
		}
		key->value = tini_process_quoted_value(value ? value : "");
	}
}

const char *tini_key_get_value(const tini_key_t *key) {
	return key ? key->value : "";
}

const char *tini_key_get(const tini_key_t *key, const char *default_value) {
	return (key && key->value) ? key->value : default_value;
}

const char *tini_key_get_string(const tini_key_t *key, const char *default_value) {
	if (!key || !key->value) {
		return default_value;
	}
	const char *s = key->value;
	while (*s && isspace((unsigned char)*s)) {
		s++;
	}
	return *s ? key->value : default_value;
}

int tini_key_get_int(const tini_key_t *key, int default_value) {
	if (!key || !key->value) {
		return default_value;
	}
	char      *end;
	const long val = strtol(key->value, &end, 0);
	if (end == key->value || *end != '\0') {
		return default_value;
	}
	return (int)val;
}

double tini_key_get_double(const tini_key_t *key, double default_value) {
	if (!key || !key->value) {
		return default_value;
	}
	char        *end;
	const double val = strtod(key->value, &end);
	if (end == key->value || *end != '\0') {
		return default_value;
	}
	return val;
}

bool tini_key_get_bool(const tini_key_t *key, bool default_value) {
	if (!key || !key->value) {
		return default_value;
	}
	switch (key->value[0]) {
		case '1':
		case 'Y':
		case 'y':
		case 'T':
		case 't': {
			return true;
		}
		case '0':
		case 'N':
		case 'n':
		case 'F':
		case 'f': {
			return false;
		}
		default: {
			return default_value;
		}
	}
}

int64_t tini_key_get_i64(const tini_key_t *key, int64_t default_value) {
	if (!key || !key->value) {
		return default_value;
	}
	char         *end;
	const int64_t val = strtoll(key->value, &end, 0);
	if (end == key->value || !tini__is_blank_tail(end)) {
		return default_value;
	}
	return val;
}

uint64_t tini_key_get_u64(const tini_key_t *key, uint64_t default_value) {
	if (!key || !key->value) {
		return default_value;
	}
	// Reject negative values
	const char *s = key->value;
	while (*s && isspace((unsigned char)*s)) {
		s++;
	}
	if (*s == '-') {
		return default_value;
	}
	char          *end;
	const uint64_t val = strtoull(key->value, &end, 0);
	if (end == key->value || !tini__is_blank_tail(end)) {
		return default_value;
	}
	return val;
}

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

// -------------------------[SECTION MANAGEMENT]-------------------------

tini_section_t *tini_get_section(tini_ptr_t self, const char *name) {
	if (!self || !name) {
		return NULL;
	}

	tini_section_t *section = tini_find_section(self, name);
	if (section) {
		return section;
	}

	// Create new section
	section = (tini_section_t *)malloc(sizeof(tini_section_t));
	if (!section) {
		self->last_error = TinyError_MemoryAlloc;
		return NULL;
	}
	section->name  = tini_strdup(name);
	section->keys  = NULL;
	section->next  = self->sections;
	self->sections = section;

	return section;
}

tini_section_t *tini_find_section(tini_ptr_t self, const char *name) {
	if (!self || !name) {
		return NULL;
	}

	tini_section_t *curr = self->sections;
	while (curr) {
		if (strcasecmp(curr->name, name) == 0) {
			return curr;
		}
		curr = curr->next;
	}
	return NULL;
}

int tini_remove_section(tini_ptr_t self, const char *name) {
	if (!self || !name) {
		return -1;
	}

	tini_section_t *curr = self->sections;
	tini_section_t *prev = NULL;

	while (curr) {
		if (strcasecmp(curr->name, name) == 0) {
			if (prev) {
				prev->next = curr->next;
			} else {
				self->sections = curr->next;
			}
			tini_free_section(curr);
			return 0;
		}
		prev = curr;
		curr = curr->next;
	}
	return self->last_error = TinyError_SectionNotFound;
}

tini_key_t *tini_get_key(tini_ptr_t self, const char *section, const char *key) {
	if (!self || !key) {
		return NULL;
	}
	const char     *sec_name = STR_ISEMPTY(section) ? "" : section;
	tini_section_t *sec      = tini_get_section(self, sec_name);
	if (!sec) {
		return NULL;
	}
	return tini_section_get_key(sec, key);
}

tini_key_t *tini_find_key(tini_ptr_t self, const char *section, const char *key) {
	if (!self || !key) {
		return NULL;
	}
	const char     *sec_name = STR_ISEMPTY(section) ? "" : section;
	tini_section_t *sec      = tini_find_section(self, sec_name);
	if (!sec) {
		return NULL;
	}
	return tini_section_find_key(sec, key);
}

tini_section_t *tini_first_section(tini_cptr_t self) {
	return self ? self->sections : NULL;
}

tini_section_t *tini_section_next(const tini_section_t *section) {
	return section ? section->next : NULL;
}

const char *tini_section_name(const tini_section_t *section) {
	return section ? section->name : NULL;
}

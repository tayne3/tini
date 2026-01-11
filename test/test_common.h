#ifndef TINI_TEST_COMMON_H
#define TINI_TEST_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cunit.h"
#include "tini/tini.h"

#ifndef TEST_RESOURCE_DIR
#define TEST_RESOURCE_DIR "res"
#endif

#ifndef TEST_TEMP_DIR
#define TEST_TEMP_DIR "temp"
#endif

/**
 * @brief Generate a full path for a resource file.
 * @param filename Filename within the resource directory.
 * @return Static buffer containing the full path (not thread-safe).
 */
static inline const char *test_res_path(const char *filename) {
	static char path[1024];
	snprintf(path, sizeof(path), "%s/%s", TEST_RESOURCE_DIR, filename);
	return path;
}

/**
 * @brief Generate a full path for a temporary file.
 * @param filename Filename within the temp directory.
 * @return Static buffer containing the full path (not thread-safe).
 */
static inline const char *test_tmp_path(const char *filename) {
	static char path[1024];
	snprintf(path, sizeof(path), "%s/%s", TEST_TEMP_DIR, filename);
	return path;
}

/**
 * @brief Write content to a temporary INI file.
 * @param filename Filename within the temp directory.
 * @param content Content to write.
 */
static inline void test_write_temp_ini(const char *filename, const char *content) {
	FILE *fp = fopen(test_tmp_path(filename), "w");
	if (fp) {
		fputs(content, fp);
		fclose(fp);
	}
}

/**
 * @brief Remove a temporary INI file.
 * @param filename Filename within the temp directory.
 */
static inline void test_remove_temp_ini(const char *filename) {
	remove(test_tmp_path(filename));
}

/**
 * @brief Set a key-value pair in a section (creating the section if needed).
 * @return 0 on success, -1 if section creation fails.
 */
static inline int test_set_value(tini_ptr_t ini, const char *section, const char *key, const char *value) {
	tini_section_t *sec = tini_get_section(ini, section);
	if (!sec)
		return -1;
	tini_key_t *k = tini_section_add_key(sec, key, value);
	return k ? 0 : -1;
}

/**
 * @brief Get a value from a section/key, returning default if not found.
 */
static inline void test_get_value(tini_ptr_t ini, const char *section, const char *key, const char *default_value,
								  char *buffer, size_t size) {
	tini_section_t *sec = tini_find_section(ini, section);
	if (!sec) {
		strncpy(buffer, default_value, size - 1);
		buffer[size - 1] = '\0';
		return;
	}
	tini_key_t *k   = tini_section_find_key(sec, key);
	const char *val = k ? tini_key_get_value(k) : default_value;
	strncpy(buffer, val, size - 1);
	buffer[size - 1] = '\0';
}

/**
 * @brief Check if a key exists in a section.
 */
static inline bool test_contains(tini_ptr_t ini, const char *section, const char *key) {
	tini_section_t *sec = tini_find_section(ini, section);
	return sec ? tini_section_has_key(sec, key) : false;
}

/**
 * @brief Remove a key from a section.
 * @return 0 on success, -1 if section not found.
 */
static inline int test_remove_key(tini_ptr_t ini, const char *section, const char *key) {
	tini_section_t *sec = tini_find_section(ini, section);
	return sec ? tini_section_remove_key(sec, key) : -1;
}

#endif  // TINI_TEST_COMMON_H

#include "test_common.h"

void test_very_long_key(void) {
	const size_t SIZE = 10 * 1024;

	char *long_key = (char *)malloc(SIZE);
	assert_not_null(long_key);
	memset(long_key, 'k', SIZE - 1);
	long_key[SIZE - 1] = '\0';

	tini_ptr_t      ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");

	tini_key_t *key = tini_section_add_key(sec, long_key, "value");
	assert_not_null(key);
	assert_str_eq("value", tini_key_get_value(key));

	tini_key_t *found = tini_section_find_key(sec, long_key);
	assert_ptr_eq(key, found);

	free(long_key);
	tini_destroy(ini);
}

void test_very_long_value(void) {
	const size_t SIZE = 10 * 1024;

	char *long_value = (char *)malloc(SIZE);
	assert_not_null(long_value);
	memset(long_value, 'v', SIZE - 1);
	long_value[SIZE - 1] = '\0';

	tini_ptr_t      ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");

	tini_key_t *key = tini_section_add_key(sec, "key", long_value);
	assert_not_null(key);
	assert_str_eq(long_value, tini_key_get_value(key));

	free(long_value);
	tini_destroy(ini);
}

void test_very_long_section_name(void) {
	const size_t SIZE = 1024;

	char *long_name = (char *)malloc(SIZE);
	assert_not_null(long_name);
	memset(long_name, 's', SIZE - 1);
	long_name[SIZE - 1] = '\0';

	tini_ptr_t      ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, long_name);
	assert_not_null(sec);

	tini_section_t *found = tini_find_section(ini, long_name);
	assert_ptr_eq(sec, found);

	free(long_name);
	tini_destroy(ini);
}

void test_empty_string_value(void) {
	tini_ptr_t      ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");

	tini_key_t *key = tini_section_add_key(sec, "empty", "");
	assert_not_null(key);
	assert_str_eq("", tini_key_get_value(key));

	tini_destroy(ini);
}

void test_null_value(void) {
	tini_ptr_t      ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");

	tini_key_t *key = tini_section_add_key(sec, "null_val", NULL);
	assert_not_null(key);
	assert_str_eq("", tini_key_get_value(key));

	tini_destroy(ini);
}

void test_special_chars_in_value(void) {
	tini_ptr_t      ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "special");

	const char *values[] = {
		"value=with=equals", "value#with#hash", "value;with;semicolon", "!@#$%^&*()_+-=[]{}|\\", "brackets[test]here",
	};

	for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); ++i) {
		char key[32];
		snprintf(key, sizeof(key), "key%zu", i);

		tini_key_t *k = tini_section_add_key(sec, key, values[i]);
		assert_not_null(k);
		assert_str_eq(values[i], tini_key_get_value(k));
	}

	tini_destroy(ini);
}

void test_quoted_special_values(void) {
	tini_ptr_t      ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "quotes");

	tini_key_t *key1 = tini_section_add_key(sec, "key1", "\"  spaces  \"");
	assert_not_null(key1);
	assert_str_eq("  spaces  ", tini_key_get_value(key1));

	tini_key_t *key2 = tini_section_add_key(sec, "key2", "'single'");
	assert_not_null(key2);
	assert_str_eq("single", tini_key_get_value(key2));

	tini_key_t *key3 = tini_section_add_key(sec, "key3", "\"\"");
	assert_not_null(key3);
	assert_str_eq("", tini_key_get_value(key3));

	tini_destroy(ini);
}

void test_many_keys_in_section(void) {
	tini_ptr_t      ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "bulk");

	for (int i = 0; i < 1000; i++) {
		char key[32], value[32];
		snprintf(key, sizeof(key), "key_%d", i);
		snprintf(value, sizeof(value), "value_%d", i);
		tini_section_add_key(sec, key, value);
	}

	tini_key_t *key500 = tini_section_find_key(sec, "key_500");
	assert_not_null(key500);
	assert_str_eq("value_500", tini_key_get_value(key500));

	tini_destroy(ini);
}

void test_many_sections(void) {
	tini_ptr_t ini = tini_empty();

	for (int i = 0; i < 500; i++) {
		char sec_name[32];
		snprintf(sec_name, sizeof(sec_name), "section_%d", i);

		tini_section_t *sec = tini_get_section(ini, sec_name);
		tini_section_add_key(sec, "key", "value");
	}

	tini_section_t *sec250 = tini_find_section(ini, "section_250");
	assert_not_null(sec250);

	tini_destroy(ini);
}

void test_whitespace_only_value(void) {
	tini_ptr_t      ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");

	tini_key_t *key1 = tini_section_add_key(sec, "spaces", "   ");
	assert_not_null(key1);
	assert_str_eq("   ", tini_key_get_value(key1));

	tini_key_t *key2 = tini_section_add_key(sec, "tabs", "\t\t\t");
	assert_not_null(key2);
	assert_str_eq("\t\t\t", tini_key_get_value(key2));

	tini_key_t *key3 = tini_section_add_key(sec, "mixed", " \t \t ");
	assert_not_null(key3);
	assert_str_eq(" \t \t ", tini_key_get_value(key3));

	tini_destroy(ini);
}

int main(void) {
	cunit_init();

	CUNIT_SUITE_BEGIN("Long Strings", NULL, NULL)
	CUNIT_TEST("Very long key", test_very_long_key)
	CUNIT_TEST("Very long value", test_very_long_value)
	CUNIT_TEST("Very long section name", test_very_long_section_name)
	CUNIT_SUITE_END()

	CUNIT_SUITE_BEGIN("Empty/NULL Values", NULL, NULL)
	CUNIT_TEST("Empty string value", test_empty_string_value)
	CUNIT_TEST("NULL value", test_null_value)
	CUNIT_SUITE_END()

	CUNIT_SUITE_BEGIN("Special Characters", NULL, NULL)
	CUNIT_TEST("Special chars in value", test_special_chars_in_value)
	CUNIT_TEST("Quoted special values", test_quoted_special_values)
	CUNIT_SUITE_END()

	CUNIT_SUITE_BEGIN("Bulk Operations", NULL, NULL)
	CUNIT_TEST("Many keys in section", test_many_keys_in_section)
	CUNIT_TEST("Many sections", test_many_sections)
	CUNIT_SUITE_END()

	CUNIT_SUITE_BEGIN("Whitespace", NULL, NULL)
	CUNIT_TEST("Whitespace only value", test_whitespace_only_value)
	CUNIT_SUITE_END()

	return cunit_run();
}

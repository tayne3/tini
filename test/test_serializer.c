#include "test_common.h"

void test_save_basic(void) {
	const char *path = test_tmp_path("save_basic.ini");

	tini_t         *ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");
	tini_section_add_key(sec, "key1", "value1");
	tini_section_add_key(sec, "key2", "value2");

	assert_int_eq(0, tini_save_to(ini, path));

	tini_destroy(ini);
	remove(path);
}

void test_save_multiple_sections(void) {
	const char *path = test_tmp_path("save_multi.ini");

	tini_t *ini = tini_empty();

	tini_section_t *sec1 = tini_get_section(ini, "section1");
	tini_section_add_key(sec1, "key1", "value1");

	tini_section_t *sec2 = tini_get_section(ini, "section2");
	tini_section_add_key(sec2, "key2", "value2");

	tini_section_t *sec3 = tini_get_section(ini, "section3");
	tini_section_add_key(sec3, "key3", "value3");

	assert_int_eq(0, tini_save_to(ini, path));

	tini_destroy(ini);
	remove(path);
}

void test_roundtrip_basic(void) {
	const char     *path = test_tmp_path("roundtrip.ini");
	tini_t         *ini1 = tini_empty();
	tini_section_t *sec1 = tini_get_section(ini1, "data");
	tini_section_add_key(sec1, "name", "test");
	tini_section_add_key(sec1, "count", "42");
	tini_section_add_key(sec1, "ratio", "3.14159");
	tini_section_add_key(sec1, "enabled", "true");

	assert_int_eq(0, tini_save_to(ini1, path));
	tini_destroy(ini1);
	tini_t *ini2 = tini_create(path);
	assert_not_null(ini2);

	tini_section_t *sec2 = tini_find_section(ini2, "data");
	assert_not_null(sec2);

	assert_str_eq("test", tini_key_get_value(tini_section_find_key(sec2, "name")));
	assert_int_eq(42, tini_key_get_int(tini_section_find_key(sec2, "count"), 0));
	assert_true(tini_key_get_bool(tini_section_find_key(sec2, "enabled"), false));

	tini_destroy(ini2);
	remove(path);
}

void test_roundtrip_special_chars(void) {
	const char     *path = test_tmp_path("roundtrip_special.ini");
	tini_t         *ini1 = tini_empty();
	tini_section_t *sec1 = tini_get_section(ini1, "special");
	tini_section_add_key(sec1, "equals", "a=b=c");
	tini_section_add_key(sec1, "hash", "a#b#c");
	tini_section_add_key(sec1, "semicolon", "a;b;c");
	tini_section_add_key(sec1, "brackets", "[test]");

	assert_int_eq(0, tini_save_to(ini1, path));
	tini_destroy(ini1);
	tini_t *ini2 = tini_create(path);
	assert_not_null(ini2);

	tini_section_t *sec2 = tini_find_section(ini2, "special");
	assert_not_null(sec2);

	assert_str_eq("a=b=c", tini_key_get_value(tini_section_find_key(sec2, "equals")));

	tini_destroy(ini2);
	remove(path);
}

void test_roundtrip_empty_values(void) {
	const char *path = test_tmp_path("roundtrip_empty.ini");

	tini_t         *ini1 = tini_empty();
	tini_section_t *sec1 = tini_get_section(ini1, "empty");
	tini_section_add_key(sec1, "empty_val", "");
	tini_section_add_key(sec1, "with_val", "something");

	assert_int_eq(0, tini_save_to(ini1, path));
	tini_destroy(ini1);

	tini_t *ini2 = tini_create(path);
	assert_not_null(ini2);

	tini_section_t *sec2 = tini_find_section(ini2, "empty");
	assert_not_null(sec2);

	tini_key_t *empty_key = tini_section_find_key(sec2, "empty_val");
	assert_not_null(empty_key);
	assert_str_eq("", tini_key_get_value(empty_key));

	tini_destroy(ini2);
	remove(path);
}

void test_roundtrip_unicode(void) {
	const char *path = test_tmp_path("roundtrip_unicode.ini");

	tini_t         *ini1 = tini_empty();
	tini_section_t *sec1 = tini_get_section(ini1, "unicode");
	tini_section_add_key(sec1, "chinese", "中文测试");
	tini_section_add_key(sec1, "japanese", "日本語テスト");
	tini_section_add_key(sec1, "german", "Grüße");
	tini_section_add_key(sec1, "emoji", "🎉✨");

	assert_int_eq(0, tini_save_to(ini1, path));
	tini_destroy(ini1);

	tini_t *ini2 = tini_create(path);
	assert_not_null(ini2);

	tini_section_t *sec2 = tini_find_section(ini2, "unicode");
	assert_not_null(sec2);

	assert_str_eq("中文测试", tini_key_get_value(tini_section_find_key(sec2, "chinese")));
	assert_str_eq("日本語テスト", tini_key_get_value(tini_section_find_key(sec2, "japanese")));
	assert_str_eq("Grüße", tini_key_get_value(tini_section_find_key(sec2, "german")));
	assert_str_eq("🎉✨", tini_key_get_value(tini_section_find_key(sec2, "emoji")));

	tini_destroy(ini2);
	remove(path);
}

void test_save_large_file(void) {
	const char *path = test_tmp_path("large.ini");

	tini_t *ini = tini_empty();

	for (int i = 0; i < 50; i++) {
		char sec_name[32];
		snprintf(sec_name, sizeof(sec_name), "section_%d", i);

		tini_section_t *sec = tini_get_section(ini, sec_name);

		for (int j = 0; j < 20; j++) {
			char key[32], value[64];
			snprintf(key, sizeof(key), "key_%d", j);
			snprintf(value, sizeof(value), "value_%d_%d_with_some_extra_text", i, j);
			tini_section_add_key(sec, key, value);
		}
	}

	assert_int_eq(0, tini_save_to(ini, path));
	tini_t *ini2 = tini_create(path);
	assert_not_null(ini2);

	tini_section_t *sec25 = tini_find_section(ini2, "section_25");
	assert_not_null(sec25);

	tini_key_t *key10 = tini_section_find_key(sec25, "key_10");
	assert_not_null(key10);
	assert_str_eq("value_25_10_with_some_extra_text", tini_key_get_value(key10));

	tini_destroy(ini);
	tini_destroy(ini2);
	remove(path);
}

int main(void) {
	cunit_init();

	CUNIT_SUITE_BEGIN("Basic Save", NULL, NULL)
	CUNIT_TEST("Save basic", test_save_basic)
	CUNIT_TEST("Save multiple sections", test_save_multiple_sections)
	CUNIT_SUITE_END()

	CUNIT_SUITE_BEGIN("Round-trip", NULL, NULL)
	CUNIT_TEST("Round-trip basic", test_roundtrip_basic)
	CUNIT_TEST("Round-trip special chars", test_roundtrip_special_chars)
	CUNIT_TEST("Round-trip empty values", test_roundtrip_empty_values)
	CUNIT_TEST("Round-trip unicode", test_roundtrip_unicode)
	CUNIT_SUITE_END()

	CUNIT_SUITE_BEGIN("Large Files", NULL, NULL)
	CUNIT_TEST("Save large file", test_save_large_file)
	CUNIT_SUITE_END()

	return cunit_run();
}

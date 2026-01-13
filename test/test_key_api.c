#include "test_common.h"

void test_add_key(void) {
	tini_t         *ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");

	tini_key_t *key = tini_section_add_key(sec, "mykey", "myvalue");
	assert_not_null(key);
	assert_str_eq("myvalue", tini_key_get_value(key));

	tini_destroy(ini);
}

void test_get_key_creates(void) {
	tini_t         *ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");

	tini_key_t *key = tini_section_get_key(sec, "newkey");
	assert_not_null(key);

	tini_key_t *found = tini_section_find_key(sec, "newkey");
	assert_ptr_eq(key, found);

	tini_destroy(ini);
}

void test_find_key_does_not_create(void) {
	tini_t         *ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");

	tini_key_t *key = tini_section_find_key(sec, "nonexistent");
	assert_null(key);

	tini_destroy(ini);
}

void test_has_key(void) {
	tini_t         *ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");

	assert_false(tini_section_has_key(sec, "key1"));

	tini_section_add_key(sec, "key1", "value1");
	assert_true(tini_section_has_key(sec, "key1"));

	tini_destroy(ini);
}

void test_key_case_insensitivity(void) {
	tini_t         *ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");

	tini_section_add_key(sec, "MyKey", "value");

	assert_not_null(tini_section_find_key(sec, "MyKey"));
	assert_not_null(tini_section_find_key(sec, "mykey"));
	assert_not_null(tini_section_find_key(sec, "MYKEY"));
	assert_not_null(tini_section_find_key(sec, "mYkEy"));

	tini_destroy(ini);
}

void test_key_complex_names(void) {
	const char *names[] = {
		"key_with_underscore",
		"key-with-hyphen",
		"key.with.dots",
		"key with spaces",
	};

	tini_t         *ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");

	for (size_t i = 0; i < sizeof(names) / sizeof(names[0]); ++i) {
		tini_key_t *key = tini_section_add_key(sec, names[i], "value");
		assert_not_null(key);

		tini_key_t *found = tini_section_find_key(sec, names[i]);
		assert_ptr_eq(key, found);
	}

	tini_destroy(ini);
}

void test_key_set_value(void) {
	tini_t         *ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");

	tini_key_t *key = tini_section_add_key(sec, "key", "original");
	assert_str_eq("original", tini_key_get_value(key));

	tini_key_set_value(key, "modified");
	assert_str_eq("modified", tini_key_get_value(key));

	tini_destroy(ini);
}

void test_overwrite_key(void) {
	tini_t         *ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");

	tini_section_add_key(sec, "key", "value1");
	tini_section_add_key(sec, "key", "value2");

	tini_key_t *found = tini_section_find_key(sec, "key");
	assert_not_null(found);
	assert_str_eq("value2", tini_key_get_value(found));

	tini_destroy(ini);
}

void test_key_remove(void) {
	tini_t         *ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");

	tini_section_add_key(sec, "to_remove", "value");
	assert_true(tini_section_has_key(sec, "to_remove"));

	assert_int_eq(0, tini_section_remove_key(sec, "to_remove"));
	assert_false(tini_section_has_key(sec, "to_remove"));

	tini_destroy(ini);
}

void test_remove_key_not_found(void) {
	tini_t         *ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");

	assert_int_ne(0, tini_section_remove_key(sec, "nonexistent"));

	tini_destroy(ini);
}

void test_remove_key_case_insensitive(void) {
	tini_t         *ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");

	tini_section_add_key(sec, "MyKey", "value");

	assert_int_eq(0, tini_section_remove_key(sec, "mykey"));
	assert_false(tini_section_has_key(sec, "MyKey"));

	tini_destroy(ini);
}

void test_direct_get_key(void) {
	tini_t *ini = tini_empty();

	tini_key_t *key = tini_get_key(ini, "section", "key");
	assert_not_null(key);

	tini_key_set_value(key, "value");
	assert_str_eq("value", tini_key_get_value(key));

	tini_destroy(ini);
}

void test_direct_find_key(void) {
	tini_t *ini = tini_empty();

	assert_null(tini_find_key(ini, "section", "key"));

	tini_get_key(ini, "section", "key");

	tini_key_t *found = tini_find_key(ini, "section", "key");
	assert_not_null(found);

	assert_null(tini_find_key(ini, "section", "other"));

	tini_destroy(ini);
}

void test_direct_key_default_section(void) {
	tini_t     *ini       = tini_empty();
	tini_key_t *key_null  = tini_get_key(ini, NULL, "default_key");
	tini_key_t *key_empty = tini_get_key(ini, "", "default_key");

	assert_ptr_eq(key_null, key_empty);

	tini_destroy(ini);
}

int main(void) {
	cunit_init();

	CUNIT_SUITE_BEGIN("Add/Get/Find Key", NULL, NULL)
	CUNIT_TEST("Add key", test_add_key)
	CUNIT_TEST("Get key creates", test_get_key_creates)
	CUNIT_TEST("Find key does not create", test_find_key_does_not_create)
	CUNIT_TEST("Has key", test_has_key)
	CUNIT_TEST("Key case insensitivity", test_key_case_insensitivity)
	CUNIT_TEST("Key complex names", test_key_complex_names)
	CUNIT_SUITE_END()

	CUNIT_SUITE_BEGIN("Set/Modify Value", NULL, NULL)
	CUNIT_TEST("Set value", test_key_set_value)
	CUNIT_TEST("Overwrite key", test_overwrite_key)
	CUNIT_SUITE_END()

	CUNIT_SUITE_BEGIN("Remove Key", NULL, NULL)
	CUNIT_TEST("Remove key", test_key_remove)
	CUNIT_TEST("Remove key not found", test_remove_key_not_found)
	CUNIT_TEST("Remove key case insensitive", test_remove_key_case_insensitive)
	CUNIT_SUITE_END()

	CUNIT_SUITE_BEGIN("Direct Key Access", NULL, NULL)
	CUNIT_TEST("Direct get key", test_direct_get_key)
	CUNIT_TEST("Direct find key", test_direct_find_key)
	CUNIT_TEST("Direct key default section", test_direct_key_default_section)
	CUNIT_SUITE_END()

	return cunit_run();
}

#include "test_common.h"

void test_section_iter_empty(void) {
	tini_ptr_t ini = tini_empty();

	tini_section_t *sec = tini_first_section(ini);
	assert_null(sec);

	tini_destroy(ini);
}

void test_section_iter_single(void) {
	tini_ptr_t ini = tini_empty();
	tini_get_section(ini, "only");

	tini_section_t *sec = tini_first_section(ini);
	assert_not_null(sec);
	assert_str_eq("only", tini_section_name(sec));

	assert_null(tini_section_next(sec));

	tini_destroy(ini);
}

void test_section_iter_multiple(void) {
	tini_ptr_t ini = tini_empty();

	tini_get_section(ini, "a");
	tini_get_section(ini, "b");
	tini_get_section(ini, "c");

	int count = 0;
	for (tini_section_t *sec = tini_first_section(ini); sec; sec = tini_section_next(sec)) {
		assert_not_null(tini_section_name(sec));
		count++;
	}
	assert_int_eq(3, count);

	tini_destroy(ini);
}

void test_section_name(void) {
	tini_ptr_t ini = tini_empty();

	tini_section_t *sec = tini_get_section(ini, "TestSection");
	assert_str_eq("TestSection", tini_section_name(sec));

	tini_section_t *empty_sec = tini_get_section(ini, "");
	assert_str_eq("", tini_section_name(empty_sec));

	tini_destroy(ini);
}

void test_section_iter_null_safety(void) {
	assert_null(tini_first_section(NULL));
	assert_null(tini_section_next(NULL));
	assert_null(tini_section_name(NULL));
}

void test_key_iter_empty(void) {
	tini_ptr_t      ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");

	tini_key_t *key = tini_section_first_key(sec);
	assert_null(key);

	tini_destroy(ini);
}

void test_key_iter_single(void) {
	tini_ptr_t      ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");
	tini_section_add_key(sec, "only_key", "value");

	tini_key_t *key = tini_section_first_key(sec);
	assert_not_null(key);
	assert_str_eq("only_key", tini_key_name(key));
	assert_str_eq("value", tini_key_get_value(key));

	assert_null(tini_key_next(key));

	tini_destroy(ini);
}

void test_key_iter_multiple(void) {
	tini_ptr_t      ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");

	tini_section_add_key(sec, "key1", "v1");
	tini_section_add_key(sec, "key2", "v2");
	tini_section_add_key(sec, "key3", "v3");

	int count = 0;
	for (tini_key_t *key = tini_section_first_key(sec); key; key = tini_key_next(key)) {
		assert_not_null(tini_key_name(key));
		count++;
	}
	assert_int_eq(3, count);

	tini_destroy(ini);
}

void test_key_name(void) {
	tini_ptr_t      ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");

	tini_key_t *key = tini_section_add_key(sec, "MyKey", "value");
	assert_str_eq("MyKey", tini_key_name(key));

	tini_destroy(ini);
}

void test_key_iter_null_safety(void) {
	assert_null(tini_section_first_key(NULL));
	assert_null(tini_key_next(NULL));
	assert_null(tini_key_name(NULL));
}

void test_full_enumeration(void) {
	tini_ptr_t ini = tini_empty();

	for (int i = 1; i <= 3; i++) {
		char sec_name[32];
		snprintf(sec_name, sizeof(sec_name), "section%d", i);
		tini_section_t *sec = tini_get_section(ini, sec_name);

		for (int j = 1; j <= 2; j++) {
			char key_name[32], value[32];
			snprintf(key_name, sizeof(key_name), "key%d", j);
			snprintf(value, sizeof(value), "value_%d_%d", i, j);
			tini_section_add_key(sec, key_name, value);
		}
	}

	int section_count = 0;
	int total_keys    = 0;

	for (tini_section_t *sec = tini_first_section(ini); sec; sec = tini_section_next(sec)) {
		section_count++;
		for (tini_key_t *key = tini_section_first_key(sec); key; key = tini_key_next(key)) {
			total_keys++;
		}
	}

	assert_int_eq(3, section_count);
	assert_int_eq(6, total_keys);

	tini_destroy(ini);
}

int main(void) {
	cunit_init();

	CUNIT_SUITE_BEGIN("Section Iteration", NULL, NULL)
	CUNIT_TEST("Empty config", test_section_iter_empty)
	CUNIT_TEST("Single section", test_section_iter_single)
	CUNIT_TEST("Multiple sections", test_section_iter_multiple)
	CUNIT_TEST("Section name", test_section_name)
	CUNIT_TEST("NULL safety", test_section_iter_null_safety)
	CUNIT_SUITE_END()

	CUNIT_SUITE_BEGIN("Key Iteration", NULL, NULL)
	CUNIT_TEST("Empty section", test_key_iter_empty)
	CUNIT_TEST("Single key", test_key_iter_single)
	CUNIT_TEST("Multiple keys", test_key_iter_multiple)
	CUNIT_TEST("Key name", test_key_name)
	CUNIT_TEST("NULL safety", test_key_iter_null_safety)
	CUNIT_SUITE_END()

	CUNIT_SUITE_BEGIN("Full Enumeration", NULL, NULL)
	CUNIT_TEST("Nested iteration", test_full_enumeration)
	CUNIT_SUITE_END()

	return cunit_run();
}

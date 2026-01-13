#include "test_common.h"

void test_get_section_creates(void) {
	tini_t         *ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "new_section");
	assert_not_null(sec);
	tini_section_t *found = tini_find_section(ini, "new_section");
	assert_ptr_eq(sec, found);

	tini_destroy(ini);
}

void test_find_section_does_not_create(void) {
	tini_t         *ini = tini_empty();
	tini_section_t *sec = tini_find_section(ini, "nonexistent");
	assert_null(sec);
	tini_destroy(ini);
}

void test_section_case_insensitivity(void) {
	tini_t *ini = tini_empty();

	tini_section_t *sec = tini_get_section(ini, "TestSection");
	tini_section_add_key(sec, "key", "value");
	assert_not_null(tini_find_section(ini, "TestSection"));
	assert_not_null(tini_find_section(ini, "testsection"));
	assert_not_null(tini_find_section(ini, "TESTSECTION"));
	assert_not_null(tini_find_section(ini, "TeStsEcTiOn"));
	assert_ptr_eq(sec, tini_find_section(ini, "testsection"));

	tini_destroy(ini);
}

void test_section_empty_name(void) {
	tini_t         *ini       = tini_empty();
	tini_section_t *sec_empty = tini_get_section(ini, "");
	assert_not_null(sec_empty);
	tini_section_t *sec_null = tini_get_section(ini, NULL);
	assert_null(sec_null);

	tini_destroy(ini);
}

void test_section_complex_names(void) {
	const char *names[] = {
		"Section One", "Section_Two", "Section-Three", "Sec.tion.Four", "Sec@tion#Five",
	};

	tini_t *ini = tini_empty();

	for (size_t i = 0; i < sizeof(names) / sizeof(names[0]); ++i) {
		tini_section_t *sec = tini_get_section(ini, names[i]);
		assert_not_null(sec);
		tini_section_add_key(sec, "test", "value");

		tini_section_t *found = tini_find_section(ini, names[i]);
		assert_ptr_eq(sec, found);
	}

	tini_destroy(ini);
}

void test_remove_section(void) {
	tini_t *ini = tini_empty();

	tini_section_t *sec = tini_get_section(ini, "to_remove");
	tini_section_add_key(sec, "key", "value");

	assert_not_null(tini_find_section(ini, "to_remove"));

	assert_int_eq(0, tini_remove_section(ini, "to_remove"));

	assert_null(tini_find_section(ini, "to_remove"));

	tini_destroy(ini);
}

void test_remove_section_not_found(void) {
	tini_t *ini    = tini_empty();
	int     result = tini_remove_section(ini, "nonexistent");
	assert_int_ne(0, result);
	tini_destroy(ini);
}

void test_remove_section_case_insensitive(void) {
	tini_t *ini = tini_empty();

	tini_get_section(ini, "MySection");
	assert_int_eq(0, tini_remove_section(ini, "mysection"));

	assert_null(tini_find_section(ini, "MySection"));

	tini_destroy(ini);
}

void test_many_sections(void) {
	tini_t *ini = tini_empty();

	for (int i = 0; i < 100; i++) {
		char name[32];
		snprintf(name, sizeof(name), "section_%d", i);

		tini_section_t *sec = tini_get_section(ini, name);
		assert_not_null(sec);

		for (int j = 0; j < 10; j++) {
			char key[32], value[32];
			snprintf(key, sizeof(key), "key_%d", j);
			snprintf(value, sizeof(value), "value_%d_%d", i, j);
			tini_section_add_key(sec, key, value);
		}
	}
	tini_section_t *sec50 = tini_find_section(ini, "section_50");
	assert_not_null(sec50);

	tini_key_t *key5 = tini_section_find_key(sec50, "key_5");
	assert_not_null(key5);
	assert_str_eq("value_50_5", tini_key_get_value(key5));

	tini_destroy(ini);
}

int main(void) {
	cunit_init();

	CUNIT_SUITE_BEGIN("Get/Find Section", NULL, NULL)
	CUNIT_TEST("Get section creates", test_get_section_creates)
	CUNIT_TEST("Find section does not create", test_find_section_does_not_create)
	CUNIT_TEST("Section case insensitivity", test_section_case_insensitivity)
	CUNIT_TEST("Section empty name", test_section_empty_name)
	CUNIT_TEST("Section complex names", test_section_complex_names)
	CUNIT_SUITE_END()

	CUNIT_SUITE_BEGIN("Remove Section", NULL, NULL)
	CUNIT_TEST("Remove section", test_remove_section)
	CUNIT_TEST("Remove section not found", test_remove_section_not_found)
	CUNIT_TEST("Remove section case insensitive", test_remove_section_case_insensitive)
	CUNIT_SUITE_END()

	CUNIT_SUITE_BEGIN("Bulk Operations", NULL, NULL)
	CUNIT_TEST("Many sections", test_many_sections)
	CUNIT_SUITE_END()

	return cunit_run();
}

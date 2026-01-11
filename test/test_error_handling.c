#include "test_common.h"

void test_last_error_after_success(void) {
	tini_ptr_t ini = tini_empty();
	assert_not_null(ini);
	assert_int_eq(0, tini_last_error(ini));

	tini_destroy(ini);
}

void test_error_string(void) {
	const char *msg = tini_error_string(0);
	assert_not_null(msg);

	const char *unknown = tini_error_string(-999);
	assert_not_null(unknown);
}

void test_create_nonexistent_file(void) {
	tini_ptr_t ini = tini_create(test_res_path("this_file_does_not_exist.ini"));
	assert_not_null(ini);
	tini_destroy(ini);
}

void test_save_to_invalid_path(void) {
	tini_ptr_t ini = tini_empty();
	assert_not_null(ini);
	assert_int_ne(0, tini_save_to(ini, "/nonexistent/path/test.ini"));

	tini_destroy(ini);
}

void test_malformed_missing_section_close(void) {
	const char *content = "[unclosed\n"
						  "key = value\n";

	test_write_temp_ini("malformed1.ini", content);

	tini_ptr_t ini = tini_create(test_tmp_path("malformed1.ini"));
	if (ini) {
		tini_destroy(ini);
	}

	test_remove_temp_ini("malformed1.ini");
}

void test_malformed_key_without_value(void) {
	const char *content = "[section]\n"
						  "key_only\n"
						  "key2 = value2\n";

	test_write_temp_ini("malformed2.ini", content);

	tini_ptr_t ini = tini_create(test_tmp_path("malformed2.ini"));

	if (ini) {
		tini_section_t *sec = tini_find_section(ini, "section");
		if (sec) {
			tini_key_t *key2 = tini_section_find_key(sec, "key2");
			if (key2) {
				assert_str_eq("value2", tini_key_get_value(key2));
			}
		}
		tini_destroy(ini);
	}

	test_remove_temp_ini("malformed2.ini");
}

void test_malformed_duplicate_sections(void) {
	const char *content = "[section]\n"
						  "key1 = value1\n"
						  "[section]\n"
						  "key2 = value2\n";

	test_write_temp_ini("duplicate.ini", content);

	tini_ptr_t ini = tini_create(test_tmp_path("duplicate.ini"));
	assert_not_null(ini);

	tini_section_t *sec = tini_find_section(ini, "section");
	assert_not_null(sec);

	assert_not_null(tini_section_find_key(sec, "key1"));
	assert_not_null(tini_section_find_key(sec, "key2"));

	tini_destroy(ini);
	test_remove_temp_ini("duplicate.ini");
}

void test_malformed_duplicate_keys(void) {
	const char *content = "[section]\n"
						  "key = value1\n"
						  "key = value2\n";

	test_write_temp_ini("dupkey.ini", content);

	tini_ptr_t ini = tini_create(test_tmp_path("dupkey.ini"));
	assert_not_null(ini);

	tini_section_t *sec = tini_find_section(ini, "section");
	assert_not_null(sec);

	tini_key_t *key = tini_section_find_key(sec, "key");
	assert_not_null(key);

	tini_destroy(ini);
	test_remove_temp_ini("dupkey.ini");
}

void test_null_ini_operations(void) {
	tini_destroy(NULL);

	assert_null(tini_find_section(NULL, "section"));
	assert_null(tini_get_section(NULL, "section"));
	assert_int_ne(0, tini_remove_section(NULL, "section"));
	assert_int_ne(0, tini_save_to(NULL, "/tmp/test.ini"));
}

void test_null_section_operations(void) {
	assert_null(tini_section_find_key(NULL, "key"));
	assert_null(tini_section_get_key(NULL, "key"));
	assert_null(tini_section_add_key(NULL, "key", "value"));
	assert_int_ne(0, tini_section_remove_key(NULL, "key"));
	assert_false(tini_section_has_key(NULL, "key"));
}

void test_null_key_operations(void) {
	assert_str_eq("", tini_key_get_value(NULL));
	assert_str_eq("default", tini_key_get(NULL, "default"));
	assert_str_eq("default", tini_key_get_string(NULL, "default"));
	assert_int_eq(-1, tini_key_get_int(NULL, -1));
	assert_int64_eq(-1, tini_key_get_i64(NULL, -1));
	assert_uint64_eq(99, tini_key_get_u64(NULL, 99));
	assert_true(tini_key_get_bool(NULL, true));
	assert_false(tini_key_get_bool(NULL, false));
}

int main(void) {
	cunit_init();

	CUNIT_SUITE_BEGIN("Error Codes", NULL, NULL)
	CUNIT_TEST("Last error after success", test_last_error_after_success)
	CUNIT_TEST("Error string", test_error_string)
	CUNIT_SUITE_END()

	CUNIT_SUITE_BEGIN("File Errors", NULL, NULL)
	CUNIT_TEST("Create nonexistent file", test_create_nonexistent_file)
	CUNIT_TEST("Save to invalid path", test_save_to_invalid_path)
	CUNIT_SUITE_END()

	CUNIT_SUITE_BEGIN("Malformed Input", NULL, NULL)
	CUNIT_TEST("Missing section close", test_malformed_missing_section_close)
	CUNIT_TEST("Key without value", test_malformed_key_without_value)
	CUNIT_TEST("Duplicate sections", test_malformed_duplicate_sections)
	CUNIT_TEST("Duplicate keys", test_malformed_duplicate_keys)
	CUNIT_SUITE_END()

	CUNIT_SUITE_BEGIN("NULL Handling", NULL, NULL)
	CUNIT_TEST("NULL ini operations", test_null_ini_operations)
	CUNIT_TEST("NULL section operations", test_null_section_operations)
	CUNIT_TEST("NULL key operations", test_null_key_operations)
	CUNIT_SUITE_END()

	return cunit_run();
}

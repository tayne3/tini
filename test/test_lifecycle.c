#include "test_common.h"

void test_create_from_file(void) {
	tini_ptr_t ini = tini_create(test_res_path("basic.ini"));
	assert_not_null(ini);
	tini_destroy(ini);
}

void test_create_file_not_found(void) {
	tini_ptr_t ini = tini_create(test_res_path("nonexistent.ini"));
	assert_not_null(ini);
	tini_destroy(ini);
}

void test_create_empty(void) {
	tini_ptr_t ini = tini_empty();
	assert_not_null(ini);
	tini_destroy(ini);
}

void test_destroy_null(void) {
	tini_destroy(NULL);
}

void test_multiple_create_destroy_cycles(void) {
	for (int i = 0; i < 100; i++) {
		tini_ptr_t ini = tini_empty();
		assert_not_null(ini);

		tini_section_t *sec = tini_get_section(ini, "test");
		tini_section_add_key(sec, "key", "value");

		tini_destroy(ini);
	}
}

void test_save_to_file(void) {
	const char *path = test_tmp_path("save_test.ini");

	tini_ptr_t ini = tini_empty();
	assert_not_null(ini);

	tini_section_t *sec = tini_get_section(ini, "section1");
	tini_section_add_key(sec, "key1", "value1");
	tini_section_add_key(sec, "key2", "value2");

	assert_int_eq(0, tini_save_to(ini, path));

	tini_destroy(ini);
	remove(path);
}

void test_save_then_reload(void) {
	const char *path = test_tmp_path("reload_test.ini");

	tini_ptr_t      ini1 = tini_empty();
	tini_section_t *sec1 = tini_get_section(ini1, "data");
	tini_section_add_key(sec1, "name", "test");
	tini_section_add_key(sec1, "count", "42");

	assert_int_eq(0, tini_save_to(ini1, path));
	tini_destroy(ini1);

	tini_ptr_t ini2 = tini_create(path);
	assert_not_null(ini2);

	tini_section_t *sec2 = tini_find_section(ini2, "data");
	assert_not_null(sec2);

	tini_key_t *name = tini_section_find_key(sec2, "name");
	assert_not_null(name);
	assert_str_eq("test", tini_key_get_value(name));

	tini_key_t *count = tini_section_find_key(sec2, "count");
	assert_not_null(count);
	assert_int_eq(42, tini_key_get_int(count, 0));

	tini_destroy(ini2);
	remove(path);
}

void test_save_empty_ini(void) {
	const char *path = test_tmp_path("empty_save.ini");

	tini_ptr_t ini = tini_empty();
	assert_int_eq(0, tini_save_to(ini, path));
	tini_destroy(ini);

	tini_ptr_t ini2 = tini_create(path);
	assert_not_null(ini2);
	tini_destroy(ini2);

	remove(path);
}

void test_lifecycle_clear_and_reuse(void) {
	tini_ptr_t ini = tini_empty();
	assert_not_null(ini);

	tini_section_t *sec = tini_get_section(ini, "first");
	assert_not_null(sec);
	tini_section_add_key(sec, "key", "val");
	assert_true(test_contains(ini, "first", "key"));

	tini_clear(ini);
	assert_null(tini_first_section(ini));
	assert_null(tini_find_section(ini, "first"));
	assert_int_eq(0, tini_last_error(ini));

	sec = tini_get_section(ini, "second");
	tini_section_add_key(sec, "new_key", "new_val");

	assert_false(test_contains(ini, "first", "key"));
	assert_true(test_contains(ini, "second", "new_key"));

	tini_destroy(ini);
}

void test_lifecycle_clear_idempotency(void) {
	tini_ptr_t ini = tini_empty();

	tini_clear(ini);
	assert_null(tini_first_section(ini));

	tini_clear(ini);
	assert_null(tini_first_section(ini));
	assert_int_eq(0, tini_last_error(ini));

	tini_destroy(ini);
}

void test_lifecycle_load_merge_distinct(void) {
	char path1[1024];
	char path2[1024];
	snprintf(path1, sizeof(path1), "%s", test_tmp_path("part1.ini"));
	snprintf(path2, sizeof(path2), "%s", test_tmp_path("part2.ini"));

	test_write_temp_ini("part1.ini", "[A]\n"
									 "key1=1\n");
	test_write_temp_ini("part2.ini", "[B]\n"
									 "key2=2\n");

	tini_ptr_t ini = tini_create(path1);
	assert_not_null(ini);

	assert_int_eq(0, tini_load(ini, path2));
	assert_int_eq(0, tini_last_error(ini));

	assert_true(test_contains(ini, "A", "key1"));
	assert_true(test_contains(ini, "B", "key2"));

	tini_destroy(ini);

	remove(path1);
	remove(path2);
}

void test_lifecycle_load_merge_overwrite(void) {
	char path_def[1024];
	char path_ovr[1024];
	snprintf(path_def, sizeof(path_def), "%s", test_tmp_path("default.ini"));
	snprintf(path_ovr, sizeof(path_ovr), "%s", test_tmp_path("override.ini"));

	test_write_temp_ini("default.ini", "[config]\n"
									   "sharding=off\n"
									   "timeout=10\n");
	test_write_temp_ini("override.ini", "[config]\n"
										"sharding=on\n");

	tini_ptr_t ini = tini_create(path_def);
	assert_not_null(ini);

	assert_int_eq(0, tini_load(ini, path_ovr));

	char val[32];
	test_get_value(ini, "config", "sharding", "", val, sizeof(val));
	assert_str_eq("on", val);

	test_get_value(ini, "config", "timeout", "", val, sizeof(val));
	assert_str_eq("10", val);

	tini_destroy(ini);

	remove(path_def);
	remove(path_ovr);
}

void test_lifecycle_load_failure_preservation(void) {
	char path_ok[1024];
	snprintf(path_ok, sizeof(path_ok), "%s", test_tmp_path("ok.ini"));
	test_write_temp_ini("ok.ini", "[data]\n"
								  "id=1\n");

	tini_ptr_t ini = tini_create(path_ok);

	assert_int_ne(0, tini_load(ini, "nonexistent.ini"));
	assert_int_ne(0, tini_last_error(ini));

	assert_true(test_contains(ini, "data", "id"));

	tini_destroy(ini);

	remove(path_ok);
}

int main(void) {
	cunit_init();

	CUNIT_SUITE_BEGIN("Create/Destroy", NULL, NULL)
	CUNIT_TEST("Create from file", test_create_from_file)
	CUNIT_TEST("Create file not found", test_create_file_not_found)
	CUNIT_TEST("Create empty", test_create_empty)
	CUNIT_TEST("Destroy NULL", test_destroy_null)
	CUNIT_TEST("Multiple create/destroy cycles", test_multiple_create_destroy_cycles)
	CUNIT_SUITE_END()

	CUNIT_SUITE_BEGIN("Save", NULL, NULL)
	CUNIT_TEST("Save to file", test_save_to_file)
	CUNIT_TEST("Save then reload", test_save_then_reload)
	CUNIT_TEST("Save empty INI", test_save_empty_ini)
	CUNIT_SUITE_END()

	CUNIT_SUITE_BEGIN("Clear", NULL, NULL)
	CUNIT_TEST("Clear and reuse", test_lifecycle_clear_and_reuse)
	CUNIT_TEST("Clear idempotency", test_lifecycle_clear_idempotency)
	CUNIT_SUITE_END()

	CUNIT_SUITE_BEGIN("Load", NULL, NULL)
	CUNIT_TEST("Load merged (distinct)", test_lifecycle_load_merge_distinct)
	CUNIT_TEST("Load merged (overwrite)", test_lifecycle_load_merge_overwrite)
	CUNIT_TEST("Load failure preservation", test_lifecycle_load_failure_preservation)
	CUNIT_SUITE_END()

	return cunit_run();
}

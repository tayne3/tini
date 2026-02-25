#include "common.hpp"

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

TEST_CASE("Create/Destroy - Create from file", "[lifecycle]") {
	tini_t *ini = tini_create(test_res_path("basic.ini"));
	REQUIRE(ini != nullptr);
	tini_destroy(ini);
}

TEST_CASE("Create/Destroy - Create file not found", "[lifecycle]") {
	tini_t *ini = tini_create(test_res_path("nonexistent.ini"));
	REQUIRE(ini != nullptr);
	tini_destroy(ini);
}

TEST_CASE("Create/Destroy - Create empty", "[lifecycle]") {
	tini_t *ini = tini_empty();
	REQUIRE(ini != nullptr);
	tini_destroy(ini);
}

TEST_CASE("Create/Destroy - Destroy NULL", "[lifecycle]") {
	tini_destroy(nullptr);
}

TEST_CASE("Create/Destroy - Multiple create/destroy cycles", "[lifecycle]") {
	for (int i = 0; i < 100; i++) {
		tini_t *ini = tini_empty();
		REQUIRE(ini != nullptr);

		tini_section_t *sec = tini_get_section(ini, "test");
		tini_section_add_key(sec, "key", "value");

		tini_destroy(ini);
	}
}

TEST_CASE("Save - Save to file", "[lifecycle]") {
	const char *path = test_tmp_path("save_test.ini");

	tini_t *ini = tini_empty();
	REQUIRE(ini != nullptr);

	tini_section_t *sec = tini_get_section(ini, "section1");
	tini_section_add_key(sec, "key1", "value1");
	tini_section_add_key(sec, "key2", "value2");

	REQUIRE(tini_save_to(ini, path) == 0);

	tini_destroy(ini);
	remove(path);
}

TEST_CASE("Save - Save then reload", "[lifecycle]") {
	const char *path = test_tmp_path("reload_test.ini");

	tini_t         *ini1 = tini_empty();
	tini_section_t *sec1 = tini_get_section(ini1, "data");
	tini_section_add_key(sec1, "name", "test");
	tini_section_add_key(sec1, "count", "42");

	REQUIRE(tini_save_to(ini1, path) == 0);
	tini_destroy(ini1);

	tini_t *ini2 = tini_create(path);
	REQUIRE(ini2 != nullptr);

	tini_section_t *sec2 = tini_find_section(ini2, "data");
	REQUIRE(sec2 != nullptr);

	tini_key_t *name = tini_section_find_key(sec2, "name");
	REQUIRE(name != nullptr);
	REQUIRE(std::string(tini_key_get_value(name)) == "test");

	tini_key_t *count = tini_section_find_key(sec2, "count");
	REQUIRE(count != nullptr);
	REQUIRE(tini_key_get_int(count, 0) == 42);

	tini_destroy(ini2);
	remove(path);
}

TEST_CASE("Save - Save empty INI", "[lifecycle]") {
	const char *path = test_tmp_path("empty_save.ini");

	tini_t *ini = tini_empty();
	REQUIRE(tini_save_to(ini, path) == 0);
	tini_destroy(ini);

	tini_t *ini2 = tini_create(path);
	REQUIRE(ini2 != nullptr);
	tini_destroy(ini2);

	remove(path);
}

TEST_CASE("Clear - Clear and reuse", "[lifecycle]") {
	tini_t *ini = tini_empty();
	REQUIRE(ini != nullptr);

	tini_section_t *sec = tini_get_section(ini, "first");
	REQUIRE(sec != nullptr);
	tini_section_add_key(sec, "key", "val");
	REQUIRE(test_contains(ini, "first", "key") == true);

	tini_clear(ini);
	REQUIRE(tini_first_section(ini) == nullptr);
	REQUIRE(tini_find_section(ini, "first") == nullptr);
	REQUIRE(tini_last_error(ini) == 0);

	sec = tini_get_section(ini, "second");
	tini_section_add_key(sec, "new_key", "new_val");

	REQUIRE(test_contains(ini, "first", "key") == false);
	REQUIRE(test_contains(ini, "second", "new_key") == true);

	tini_destroy(ini);
}

TEST_CASE("Clear - Clear idempotency", "[lifecycle]") {
	tini_t *ini = tini_empty();

	tini_clear(ini);
	REQUIRE(tini_first_section(ini) == nullptr);

	tini_clear(ini);
	REQUIRE(tini_first_section(ini) == nullptr);
	REQUIRE(tini_last_error(ini) == 0);

	tini_destroy(ini);
}

TEST_CASE("Load - Load merged (distinct)", "[lifecycle]") {
	char path1[1024];
	char path2[1024];
	snprintf(path1, sizeof(path1), "%s", test_tmp_path("part1.ini"));
	snprintf(path2, sizeof(path2), "%s", test_tmp_path("part2.ini"));

	test_write_temp_ini("part1.ini", "[A]\n"
									 "key1=1\n");
	test_write_temp_ini("part2.ini", "[B]\n"
									 "key2=2\n");

	tini_t *ini = tini_create(path1);
	REQUIRE(ini != nullptr);

	REQUIRE(tini_load(ini, path2) == 0);
	REQUIRE(tini_last_error(ini) == 0);

	REQUIRE(test_contains(ini, "A", "key1") == true);
	REQUIRE(test_contains(ini, "B", "key2") == true);

	tini_destroy(ini);

	remove(path1);
	remove(path2);
}

TEST_CASE("Load - Load merged (overwrite)", "[lifecycle]") {
	char path_def[1024];
	char path_ovr[1024];
	snprintf(path_def, sizeof(path_def), "%s", test_tmp_path("default.ini"));
	snprintf(path_ovr, sizeof(path_ovr), "%s", test_tmp_path("override.ini"));

	test_write_temp_ini("default.ini", "[config]\n"
									   "sharding=off\n"
									   "timeout=10\n");
	test_write_temp_ini("override.ini", "[config]\n"
										"sharding=on\n");

	tini_t *ini = tini_create(path_def);
	REQUIRE(ini != nullptr);

	REQUIRE(tini_load(ini, path_ovr) == 0);

	char val[32];
	test_get_value(ini, "config", "sharding", "", val, sizeof(val));
	REQUIRE(std::string(val) == "on");

	test_get_value(ini, "config", "timeout", "", val, sizeof(val));
	REQUIRE(std::string(val) == "10");

	tini_destroy(ini);

	remove(path_def);
	remove(path_ovr);
}

TEST_CASE("Load - Load failure preservation", "[lifecycle]") {
	char path_ok[1024];
	snprintf(path_ok, sizeof(path_ok), "%s", test_tmp_path("ok.ini"));
	test_write_temp_ini("ok.ini", "[data]\n"
								  "id=1\n");

	tini_t *ini = tini_create(path_ok);

	REQUIRE(tini_load(ini, "nonexistent.ini") != 0);
	REQUIRE(tini_last_error(ini) != 0);

	REQUIRE(test_contains(ini, "data", "id") == true);

	tini_destroy(ini);

	remove(path_ok);
}

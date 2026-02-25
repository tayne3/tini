#include "common.hpp"

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

TEST_CASE("Add/Get/Find Key - Add key", "[key_api]") {
	tini_t         *ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");

	tini_key_t *key = tini_section_add_key(sec, "mykey", "myvalue");
	REQUIRE(key != nullptr);
	REQUIRE(std::string(tini_key_get_value(key)) == "myvalue");

	tini_destroy(ini);
}

TEST_CASE("Add/Get/Find Key - Get key creates", "[key_api]") {
	tini_t         *ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");

	tini_key_t *key = tini_section_get_key(sec, "newkey");
	REQUIRE(key != nullptr);

	tini_key_t *found = tini_section_find_key(sec, "newkey");
	REQUIRE(key == found);

	tini_destroy(ini);
}

TEST_CASE("Add/Get/Find Key - Find key does not create", "[key_api]") {
	tini_t         *ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");

	tini_key_t *key = tini_section_find_key(sec, "nonexistent");
	REQUIRE(key == nullptr);

	tini_destroy(ini);
}

TEST_CASE("Add/Get/Find Key - Has key", "[key_api]") {
	tini_t         *ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");

	REQUIRE(tini_section_has_key(sec, "key1") == false);

	tini_section_add_key(sec, "key1", "value1");
	REQUIRE(tini_section_has_key(sec, "key1") == true);

	tini_destroy(ini);
}

TEST_CASE("Add/Get/Find Key - Key case insensitivity", "[key_api]") {
	tini_t         *ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");

	tini_section_add_key(sec, "MyKey", "value");

	REQUIRE(tini_section_find_key(sec, "MyKey") != nullptr);
	REQUIRE(tini_section_find_key(sec, "mykey") != nullptr);
	REQUIRE(tini_section_find_key(sec, "MYKEY") != nullptr);
	REQUIRE(tini_section_find_key(sec, "mYkEy") != nullptr);

	tini_destroy(ini);
}

TEST_CASE("Add/Get/Find Key - Key complex names", "[key_api]") {
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
		REQUIRE(key != nullptr);

		tini_key_t *found = tini_section_find_key(sec, names[i]);
		REQUIRE(key == found);
	}

	tini_destroy(ini);
}

TEST_CASE("Set/Modify Value - Set value", "[key_api]") {
	tini_t         *ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");

	tini_key_t *key = tini_section_add_key(sec, "key", "original");
	REQUIRE(std::string(tini_key_get_value(key)) == "original");

	tini_key_set_value(key, "modified");
	REQUIRE(std::string(tini_key_get_value(key)) == "modified");

	tini_destroy(ini);
}

TEST_CASE("Set/Modify Value - Overwrite key", "[key_api]") {
	tini_t         *ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");

	tini_section_add_key(sec, "key", "value1");
	tini_section_add_key(sec, "key", "value2");

	tini_key_t *found = tini_section_find_key(sec, "key");
	REQUIRE(found != nullptr);
	REQUIRE(std::string(tini_key_get_value(found)) == "value2");

	tini_destroy(ini);
}

TEST_CASE("Remove Key - Remove key", "[key_api]") {
	tini_t         *ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");

	tini_section_add_key(sec, "to_remove", "value");
	REQUIRE(tini_section_has_key(sec, "to_remove") == true);

	REQUIRE(tini_section_remove_key(sec, "to_remove") == 0);
	REQUIRE(tini_section_has_key(sec, "to_remove") == false);

	tini_destroy(ini);
}

TEST_CASE("Remove Key - Remove key not found", "[key_api]") {
	tini_t         *ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");

	REQUIRE(tini_section_remove_key(sec, "nonexistent") != 0);

	tini_destroy(ini);
}

TEST_CASE("Remove Key - Remove key case insensitive", "[key_api]") {
	tini_t         *ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");

	tini_section_add_key(sec, "MyKey", "value");

	REQUIRE(tini_section_remove_key(sec, "mykey") == 0);
	REQUIRE(tini_section_has_key(sec, "MyKey") == false);

	tini_destroy(ini);
}

TEST_CASE("Direct Key Access - Direct get key", "[key_api]") {
	tini_t *ini = tini_empty();

	tini_key_t *key = tini_get_key(ini, "section", "key");
	REQUIRE(key != nullptr);

	tini_key_set_value(key, "value");
	REQUIRE(std::string(tini_key_get_value(key)) == "value");

	tini_destroy(ini);
}

TEST_CASE("Direct Key Access - Direct find key", "[key_api]") {
	tini_t *ini = tini_empty();

	REQUIRE(tini_find_key(ini, "section", "key") == nullptr);

	tini_get_key(ini, "section", "key");

	tini_key_t *found = tini_find_key(ini, "section", "key");
	REQUIRE(found != nullptr);

	REQUIRE(tini_find_key(ini, "section", "other") == nullptr);

	tini_destroy(ini);
}

TEST_CASE("Direct Key Access - Direct key default section", "[key_api]") {
	tini_t     *ini       = tini_empty();
	tini_key_t *key_null  = tini_get_key(ini, nullptr, "default_key");
	tini_key_t *key_empty = tini_get_key(ini, "", "default_key");

	REQUIRE(key_null == key_empty);

	tini_destroy(ini);
}

#include "test_common.h"

void test_inline_comments_semicolon(void) {
	const char *content = "[comments]\n"
						  "basic = value1 ; this is a comment\n"
						  "spacing = value2   ;   spaced comment\n"
						  "no_space = value3;attached comment\n";

	test_write_temp_ini("inline_semi.ini", content);

	tini_t *ini = tini_create(test_tmp_path("inline_semi.ini"));
	assert_not_null(ini);

	tini_section_t *sec = tini_find_section(ini, "comments");
	assert_not_null(sec);

	assert_str_eq("value1", tini_key_get_value(tini_section_find_key(sec, "basic")));
	assert_str_eq("value2", tini_key_get_value(tini_section_find_key(sec, "spacing")));
	assert_str_eq("value3", tini_key_get_value(tini_section_find_key(sec, "no_space")));

	tini_destroy(ini);
	test_remove_temp_ini("inline_semi.ini");
}

void test_inline_comments_hash(void) {
	const char *content = "[comments]\n"
						  "hash = value1 # this is a comment\n"
						  "hash_attach = value2#attached\n";

	test_write_temp_ini("inline_hash.ini", content);

	tini_t *ini = tini_create(test_tmp_path("inline_hash.ini"));
	assert_not_null(ini);

	tini_section_t *sec = tini_find_section(ini, "comments");
	assert_not_null(sec);

	assert_str_eq("value1", tini_key_get_value(tini_section_find_key(sec, "hash")));
	assert_str_eq("value2", tini_key_get_value(tini_section_find_key(sec, "hash_attach")));

	tini_destroy(ini);
	test_remove_temp_ini("inline_hash.ini");
}

void test_double_quotes(void) {
	const char *content = "[quotes]\n"
						  "basic = \"hello world\"\n"
						  "spaces = \"  spaced  \"\n"
						  "empty = \"\"\n";

	test_write_temp_ini("double_quotes.ini", content);

	tini_t *ini = tini_create(test_tmp_path("double_quotes.ini"));
	assert_not_null(ini);

	tini_section_t *sec = tini_find_section(ini, "quotes");
	assert_not_null(sec);

	assert_str_eq("hello world", tini_key_get_value(tini_section_find_key(sec, "basic")));
	assert_str_eq("  spaced  ", tini_key_get_value(tini_section_find_key(sec, "spaces")));
	assert_str_eq("", tini_key_get_value(tini_section_find_key(sec, "empty")));

	tini_destroy(ini);
	test_remove_temp_ini("double_quotes.ini");
}

void test_single_quotes(void) {
	const char *content = "[quotes]\n"
						  "basic = 'hello world'\n"
						  "spaces = '  spaced  '\n";

	test_write_temp_ini("single_quotes.ini", content);

	tini_t *ini = tini_create(test_tmp_path("single_quotes.ini"));
	assert_not_null(ini);

	tini_section_t *sec = tini_find_section(ini, "quotes");
	assert_not_null(sec);

	assert_str_eq("hello world", tini_key_get_value(tini_section_find_key(sec, "basic")));
	assert_str_eq("  spaced  ", tini_key_get_value(tini_section_find_key(sec, "spaces")));

	tini_destroy(ini);
	test_remove_temp_ini("single_quotes.ini");
}

void test_quotes_protect_comments(void) {
	const char *content = "[quotes]\n"
						  "semi = \"val;ue\"\n"
						  "hash = 'val#ue'\n"
						  "complex = \"val # ue\" ; real comment\n";

	test_write_temp_ini("quotes_comment.ini", content);

	tini_t *ini = tini_create(test_tmp_path("quotes_comment.ini"));
	assert_not_null(ini);

	tini_section_t *sec = tini_find_section(ini, "quotes");
	assert_not_null(sec);

	assert_str_eq("val;ue", tini_key_get_value(tini_section_find_key(sec, "semi")));
	assert_str_eq("val#ue", tini_key_get_value(tini_section_find_key(sec, "hash")));
	assert_str_eq("val # ue", tini_key_get_value(tini_section_find_key(sec, "complex")));

	tini_destroy(ini);
	test_remove_temp_ini("quotes_comment.ini");
}

void test_whitespace_trimming(void) {
	const char *content = "[whitespace]\n"
						  "  key1  =  value1  \n"
						  "\tkey2\t=\tvalue2\t\n";

	test_write_temp_ini("whitespace.ini", content);

	tini_t *ini = tini_create(test_tmp_path("whitespace.ini"));
	assert_not_null(ini);

	tini_section_t *sec = tini_find_section(ini, "whitespace");
	assert_not_null(sec);

	assert_not_null(tini_section_find_key(sec, "key1"));
	assert_not_null(tini_section_find_key(sec, "key2"));

	assert_str_eq("value1", tini_key_get_value(tini_section_find_key(sec, "key1")));
	assert_str_eq("value2", tini_key_get_value(tini_section_find_key(sec, "key2")));

	tini_destroy(ini);
	test_remove_temp_ini("whitespace.ini");
}

void test_preserved_whitespace_in_quotes(void) {
	const char *content = "[ws]\n"
						  "key1 = \"  spaced  \"\n"
						  "key2 = '\ttabbed\t'\n";

	test_write_temp_ini("ws_quotes.ini", content);

	tini_t *ini = tini_create(test_tmp_path("ws_quotes.ini"));
	assert_not_null(ini);

	tini_section_t *sec = tini_find_section(ini, "ws");
	assert_str_eq("  spaced  ", tini_key_get_value(tini_section_find_key(sec, "key1")));
	assert_str_eq("\ttabbed\t", tini_key_get_value(tini_section_find_key(sec, "key2")));

	tini_destroy(ini);
	test_remove_temp_ini("ws_quotes.ini");
}

void test_utf8_parsing(void) {
	tini_t *ini = tini_create(test_res_path("unicode.ini"));
	assert_not_null(ini);
	tini_destroy(ini);
}

void test_massive_line(void) {
	const size_t SIZE      = 4096;
	char        *large_val = (char *)malloc(SIZE + 1);
	memset(large_val, 'x', SIZE);
	large_val[SIZE] = '\0';

	char *line = (char *)malloc(SIZE + 100);
	sprintf(line, "key = %s\n", large_val);

	FILE *fp = fopen(test_tmp_path("massive.ini"), "w");
	if (fp) {
		fputs("[massive]\n", fp);
		fputs(line, fp);
		fclose(fp);
	}

	tini_t *ini = tini_create(test_tmp_path("massive.ini"));
	assert_not_null(ini);

	tini_section_t *sec = tini_find_section(ini, "massive");
	assert_not_null(sec);

	tini_key_t *key = tini_section_find_key(sec, "key");
	assert_not_null(key);
	assert_str_eq(large_val, tini_key_get_value(key));

	free(large_val);
	free(line);
	tini_destroy(ini);
	test_remove_temp_ini("massive.ini");
}

void test_global_keys(void) {
	const char *content = "global_key1 = value1\n"
						  "global_key2 = value2\n"
						  "[section]\n"
						  "key = value\n";

	test_write_temp_ini("global.ini", content);

	tini_t *ini = tini_create(test_tmp_path("global.ini"));
	assert_not_null(ini);

	tini_section_t *global = tini_find_section(ini, "");
	if (global) {
		tini_key_t *gk1 = tini_section_find_key(global, "global_key1");
		if (gk1) {
			assert_str_eq("value1", tini_key_get_value(gk1));
		}
	}

	tini_section_t *sec = tini_find_section(ini, "section");
	assert_not_null(sec);

	tini_destroy(ini);
	test_remove_temp_ini("global.ini");
}

int main(void) {
	cunit_init();

	CUNIT_SUITE_BEGIN("Inline Comments", NULL, NULL)
	CUNIT_TEST("Semicolon comments", test_inline_comments_semicolon)
	CUNIT_TEST("Hash comments", test_inline_comments_hash)
	CUNIT_SUITE_END()

	CUNIT_SUITE_BEGIN("Quote Handling", NULL, NULL)
	CUNIT_TEST("Double quotes", test_double_quotes)
	CUNIT_TEST("Single quotes", test_single_quotes)
	CUNIT_TEST("Quotes protect comments", test_quotes_protect_comments)
	CUNIT_SUITE_END()

	CUNIT_SUITE_BEGIN("Whitespace Handling", NULL, NULL)
	CUNIT_TEST("Whitespace trimming", test_whitespace_trimming)
	CUNIT_TEST("Preserved whitespace in quotes", test_preserved_whitespace_in_quotes)
	CUNIT_SUITE_END()

	CUNIT_SUITE_BEGIN("Encoding", NULL, NULL)
	CUNIT_TEST("UTF-8 parsing", test_utf8_parsing)
	CUNIT_SUITE_END()

	CUNIT_SUITE_BEGIN("Edge Cases", NULL, NULL)
	CUNIT_TEST("Massive line", test_massive_line)
	CUNIT_TEST("Global keys", test_global_keys)
	CUNIT_SUITE_END()

	return cunit_run();
}

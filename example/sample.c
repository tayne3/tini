#include <stdio.h>
#include <stdlib.h>

#include "tini/tini.h"

int main(void) {
	tini_t *ini = tini_empty();
	if (!ini) {
		fprintf(stderr, "Failed to create ini object\n");
		return 1;
	}

	tini_section_t *section = tini_get_section(ini, "section");
	tini_section_add_key(section, "key", "1");
	tini_section_add_key(section, "enabled", "true");
	tini_section_add_key(section, "timeout", "30.5");
	tini_section_add_key(section, "name", "tini");

	tini_key_t *key = tini_get_key(ini, "section", "key");
	printf("Int value: %d\n", tini_key_get_int(key, 0));
	printf("Bool value: %d\n", tini_key_get_bool(tini_find_key(ini, "section", "enabled"), false));
	tini_section_t *s = tini_find_section(ini, "section");
	printf("Double value: %.1f\n", tini_key_get_double(tini_section_find_key(s, "timeout"), 0.0));
	printf("String value: %s\n", tini_key_get_string(tini_find_key(ini, "section", "name"), "default"));

	size_t section_count = 0;
	for (tini_section_t *s = tini_first_section(ini); s; s = tini_section_next(s)) {
		section_count++;
	}
	printf("\nTotal sections: %zu\n", section_count);

	for (tini_section_t *s = tini_first_section(ini); s; s = tini_section_next(s)) {
		printf("[%s]\n", tini_section_name(s));
		for (tini_key_t *k = tini_section_first_key(s); k; k = tini_key_next(k)) {
			printf("  %s = %s\n", tini_key_name(k), tini_key_get_value(k));
		}
	}

	tini_destroy(ini);
	return 0;
}

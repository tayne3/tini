/*
 * MIT License
 *
 * Copyright (c) 2026 tayne3
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * 1. The above copyright notice and this permission notice shall be included in
 *    all copies or substantial portions of the Software.
 *
 * 2. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *    SOFTWARE.
 */
#ifndef TINI_TINI_H
#define TINI_TINI_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "tini/tini_export.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief tini Configuration Object
 */
typedef struct tini_s tini_t;
typedef tini_t       *tini_ptr_t;
typedef const tini_t *tini_cptr_t;

/**
 * @brief tini Section Object
 */
typedef struct tini_section tini_section_t;

/**
 * @brief tini Key Object
 */
typedef struct tini_key tini_key_t;

// -----------------------------------------------------------------------------
// Lifecycle Management
// -----------------------------------------------------------------------------

/**
 * @brief Create a tini object from a file
 * @param path Path to the INI file
 * @return Pointer to tini object, or NULL on failure
 */
TINI_EXPORT tini_ptr_t tini_create(const char *path);

/**
 * @brief Create an empty tini object
 * @return Pointer to tini object, or NULL on failure
 */
TINI_EXPORT tini_ptr_t tini_empty(void);

/**
 * @brief Destroy the tini object and free all associated memory
 * @param self Pointer to tini object
 */
TINI_EXPORT void tini_destroy(tini_ptr_t self);

/**
 * @brief Clear all sections and keys from a tini object
 * @param self Pointer to tini object
 */
TINI_EXPORT void tini_clear(tini_ptr_t self);

/**
 * @brief Load configuration from file into existing tini object
 * @param self Pointer to tini object
 * @param path Path to the INI file
 * @return 0 on success, error code otherwise
 * @note Merges with existing data. Use tini_clear() first if replacement is needed.
 */
TINI_EXPORT int tini_load(tini_ptr_t self, const char *path);

/**
 * @brief Save the configuration to a file
 * @param self Pointer to tini object
 * @param path Path to save the file
 * @return 0 on success, error code otherwise
 */
TINI_EXPORT int tini_save_to(tini_ptr_t self, const char *path);

// -----------------------------------------------------------------------------
// Section Management
// -----------------------------------------------------------------------------

/**
 * @brief Get a section by name, creating it if it doesn't exist
 * @param self Pointer to tini object
 * @param name Section name
 * @return Pointer to the section
 */
TINI_EXPORT tini_section_t *tini_get_section(tini_ptr_t self, const char *name);

/**
 * @brief Find a section by name (does not create)
 * @param self Pointer to tini object
 * @param name Section name
 * @return Pointer to the section, or NULL if not found
 */
TINI_EXPORT tini_section_t *tini_find_section(tini_ptr_t self, const char *name);

/**
 * @brief Remove a section by name
 * @param self Pointer to tini object
 * @param name Section name
 * @return 0 on success, error code otherwise
 */
TINI_EXPORT int tini_remove_section(tini_ptr_t self, const char *name);

/**
 * @brief Get a key by section and key name, creating both if they don't exist
 * @param self Pointer to tini object
 * @param section Section name (NULL or "" for default section)
 * @param key Key name
 * @return Pointer to the key, or NULL on failure
 */
TINI_EXPORT tini_key_t *tini_get_key(tini_ptr_t self, const char *section, const char *key);

/**
 * @brief Find a key by section and key name (does not create)
 * @param self Pointer to tini object
 * @param section Section name (NULL or "" for default section)
 * @param key Key name
 * @return Pointer to the key, or NULL if section or key not found
 */
TINI_EXPORT tini_key_t *tini_find_key(tini_ptr_t self, const char *section, const char *key);

// -----------------------------------------------------------------------------
// Key Management
// -----------------------------------------------------------------------------

/**
 * @brief Get a key in a section, creating it if it doesn't exist
 * @param section Pointer to section object
 * @param key Key name
 * @return Pointer to the key
 */
TINI_EXPORT tini_key_t *tini_section_get_key(tini_section_t *section, const char *key);

/**
 * @brief Find a key in a section (does not create)
 * @param section Pointer to section object
 * @param key Key name
 * @return Pointer to the key, or NULL if not found
 */
TINI_EXPORT tini_key_t *tini_section_find_key(tini_section_t *section, const char *key);

/**
 * @brief Check if a key exists in a section
 * @param section Pointer to section object
 * @param key Key name
 * @return true if exists, false otherwise
 */
TINI_EXPORT bool tini_section_has_key(tini_section_t *section, const char *key);

/**
 * @brief Add or update a key with a string value
 * @param section Pointer to section object
 * @param key Key name
 * @param value String value
 * @return Pointer to the key
 */
TINI_EXPORT tini_key_t *tini_section_add_key(tini_section_t *section, const char *key, const char *value);

/**
 * @brief Remove a key from a section
 * @param section Pointer to section object
 * @param key Key name
 * @return 0 on success, error code otherwise
 */
TINI_EXPORT int tini_section_remove_key(tini_section_t *section, const char *key);

/**
 * @brief Get the first section in the configuration
 * @param self Pointer to tini object
 * @return Pointer to the first section, or NULL if empty
 */
TINI_EXPORT tini_section_t *tini_first_section(tini_cptr_t self);

/**
 * @brief Get the next section after the given section
 * @param section Pointer to current section
 * @return Pointer to the next section, or NULL if at end
 */
TINI_EXPORT tini_section_t *tini_section_next(const tini_section_t *section);

/**
 * @brief Get the name of a section
 * @param section Pointer to section object
 * @return Section name (owned by section, do not free), or NULL if invalid
 */
TINI_EXPORT const char *tini_section_name(const tini_section_t *section);

/**
 * @brief Get the first key in a section
 * @param section Pointer to section object
 * @return Pointer to the first key, or NULL if section is empty
 */
TINI_EXPORT tini_key_t *tini_section_first_key(const tini_section_t *section);

/**
 * @brief Get the next key after the given key
 * @param key Pointer to current key
 * @return Pointer to the next key, or NULL if at end
 */
TINI_EXPORT tini_key_t *tini_key_next(const tini_key_t *key);

/**
 * @brief Get the name of a key
 * @param key Pointer to key object
 * @return Key name (owned by key, do not free), or NULL if invalid
 */
TINI_EXPORT const char *tini_key_name(const tini_key_t *key);

// -----------------------------------------------------------------------------
// Value Access & Conversion
// -----------------------------------------------------------------------------

/**
 * @brief Set the raw string value of a key
 * @param key Pointer to key object
 * @param value New string value
 */
TINI_EXPORT void tini_key_set_value(tini_key_t *key, const char *value);

/**
 * @brief Get the raw string value of a key
 * @param key Pointer to key object
 * @return String value (owned by the key, do not free)
 */
TINI_EXPORT const char *tini_key_get_value(const tini_key_t *key);

/**
 * @brief Get string value with default.
 * @param key Pointer to key object (may be NULL).
 * @param default_value Fallback if key is NULL or has no value.
 * @return Key value or default_value.
 */
TINI_EXPORT const char *tini_key_get(const tini_key_t *key, const char *default_value);

/**
 * @brief Get non-empty string value.
 * @param key Pointer to key object.
 * @param default_value Fallback if key is NULL, empty, or whitespace-only.
 * @return Key value or default_value.
 */
TINI_EXPORT const char *tini_key_get_string(const tini_key_t *key, const char *default_value);

/**
 * @brief Parse value as int.
 * @param key Pointer to key object.
 * @param default_value Fallback on parse failure.
 * @return Parsed integer or default_value.
 */
TINI_EXPORT int tini_key_get_int(const tini_key_t *key, int default_value);

/**
 * @brief Parse value as int64_t.
 * @param key Pointer to key object.
 * @param default_value Fallback on parse failure.
 * @return Parsed integer or default_value.
 * @note Supports decimal, hex (0x), and octal (0) prefixes.
 */
TINI_EXPORT int64_t tini_key_get_i64(const tini_key_t *key, int64_t default_value);

/**
 * @brief Parse value as uint64_t.
 * @param key Pointer to key object.
 * @param default_value Fallback on parse failure or negative input.
 * @return Parsed unsigned integer or default_value.
 * @note Rejects negative values. Supports decimal and hex prefixes.
 */
TINI_EXPORT uint64_t tini_key_get_u64(const tini_key_t *key, uint64_t default_value);

/**
 * @brief Convert key value to double
 * @param key Pointer to key object
 * @param default_value Value to return if conversion fails
 * @return Double value
 */
TINI_EXPORT double tini_key_get_double(const tini_key_t *key, double default_value);

/**
 * @brief Convert key value to bool
 * @param key Pointer to key object
 * @param default_value Value to return if conversion fails
 * @return Boolean value
 */
TINI_EXPORT bool tini_key_get_bool(const tini_key_t *key, bool default_value);

// -----------------------------------------------------------------------------
// Error Handling
// -----------------------------------------------------------------------------

/**
 * @brief Get the last error code
 * @param self Pointer to tini object
 * @return Error code
 */
TINI_EXPORT int tini_last_error(tini_cptr_t self);

/**
 * @brief Get the string description of an error code
 * @param code Error code
 * @return Error description
 */
TINI_EXPORT const char *tini_error_string(int code);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

#include <stdexcept>
#include <string>
#include <vector>

namespace tini {

class TINI_EXPORT Key {
public:
	explicit Key(tini_key_t *k) : d(k) {}
	bool        valid() const { return d != nullptr; }
	explicit    operator bool() const { return valid(); }
	const char *name() const { return tini_key_name(d); }
	Key         next() const { return Key(tini_key_next(d)); }

	void        setValue(const char *value) { tini_key_set_value(d, value); }
	const char *getValue() const { return tini_key_get_value(d); }
	const char *get(const char *def = nullptr) const { return tini_key_get(d, def); }
	const char *getString(const char *def = nullptr) const { return tini_key_get_string(d, def); }
	int         getInt(int def = 0) const { return tini_key_get_int(d, def); }
	int64_t     getI64(int64_t def = 0) const { return tini_key_get_i64(d, def); }
	uint64_t    getU64(uint64_t def = 0) const { return tini_key_get_u64(d, def); }
	double      getDouble(double def = 0.0) const { return tini_key_get_double(d, def); }
	bool        getBool(bool def = false) const { return tini_key_get_bool(d, def); }

private:
	tini_key_t *d;
};

class TINI_EXPORT Section {
public:
	explicit Section(tini_section_t *s) : d(s) {}
	bool        valid() const { return d != nullptr; }
	explicit    operator bool() const { return valid(); }
	const char *name() const { return tini_section_name(d); }
	Key         firstKey() const { return Key(tini_section_first_key(d)); }
	Section     next() const { return Section(tini_section_next(d)); }

	Key  getKey(const char *key) { return Key(tini_section_get_key(d, key)); }
	Key  findKey(const char *key) const { return Key(tini_section_find_key(d, key)); }
	bool hasKey(const char *key) const { return tini_section_has_key(d, key); }
	Key  addKey(const char *key, const char *value) { return Key(tini_section_add_key(d, key, value)); }
	int  removeKey(const char *key) { return tini_section_remove_key(d, key); }

	std::vector<std::string> keys() const {
		std::vector<std::string> result;
		for (Key k = firstKey(); k; k = k.next()) {
			result.emplace_back(k.name());
		}
		return result;
	}

private:
	tini_section_t *d;
};

class TINI_EXPORT Ini {
public:
	Ini() : d(tini_empty()) {
		if (!d) {
			throw std::bad_alloc();
		}
	}
	~Ini() { tini_destroy(d); }

	Ini(Ini &&o) noexcept : d(o.d) { o.d = nullptr; }
	Ini &operator=(Ini &&o) noexcept {
		if (this != &o) {
			tini_destroy(d);
			d   = o.d;
			o.d = nullptr;
		}
		return *this;
	}
	Ini(const Ini &)            = delete;
	Ini &operator=(const Ini &) = delete;

	int                lastError() const { return tini_last_error(d); }
	static const char *errorString(int code) { return tini_error_string(code); }

	int  load(const char *path) { return tini_load(d, path); }
	void clear() { tini_clear(d); }
	int  saveTo(const char *path) { return tini_save_to(d, path); }

	Section getSection(const char *name) { return Section(tini_get_section(d, name)); }
	Section findSection(const char *name) const { return Section(tini_find_section(d, name)); }
	int     removeSection(const char *name) { return tini_remove_section(d, name); }

	Key getKey(const char *section, const char *key) { return Key(tini_get_key(d, section, key)); }
	Key findKey(const char *section, const char *key) const { return Key(tini_find_key(d, section, key)); }

	Section firstSection() const { return Section(tini_first_section(d)); }

	std::vector<std::string> sections() const {
		std::vector<std::string> result;
		for (Section s = firstSection(); s; s = s.next()) {
			result.emplace_back(s.name());
		}
		return result;
	}

private:
	tini_ptr_t d;
};

}  // namespace tini

#endif

#endif  // TINI_TINI_H

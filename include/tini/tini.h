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

#ifndef TINI_API
#if defined(_WIN32) || defined(__CYGWIN__)
#if defined(TINI_LIB_EXPORT)
#define TINI_API __declspec(dllexport)
#elif defined(TINI_SHARED)
#define TINI_API __declspec(dllimport)
#endif
#elif defined(__GNUC__) || defined(__clang__)
#if defined(FMT_LIB_EXPORT) || defined(FMT_SHARED)
#define TINI_API __attribute__((visibility("default")))
#endif
#endif
#endif
#ifndef TINI_API
#define TINI_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tini_s         tini_t;
typedef struct tini_section_s tini_section_t;
typedef struct tini_key_s     tini_key_t;

// -----------------------------------------------------------------------------
// Lifecycle Management
// -----------------------------------------------------------------------------

/**
 * @brief Create a tini object from file
 * @param path INI file path, or NULL to create empty
 * @return tini object, or NULL on failure
 */
TINI_API tini_t *tini_create(const char *path);

/**
 * @brief Create an empty tini object
 * @return tini object, or NULL on failure
 */
TINI_API tini_t *tini_empty(void);

/**
 * @brief Destroy the tini object and free all memory
 * @param self tini object
 */
TINI_API void tini_destroy(tini_t *self);

/**
 * @brief Clear all sections and keys
 * @param self tini object
 */
TINI_API void tini_clear(tini_t *self);

/**
 * @brief Load configuration from file
 * @param self tini object
 * @param path INI file path
 * @return 0 on success, error code otherwise
 * @note Merges with existing data; use tini_clear() first if replacement needed
 */
TINI_API int tini_load(tini_t *self, const char *path);

/**
 * @brief Save configuration to file
 * @param self tini object
 * @param path output file path
 * @return 0 on success, error code otherwise
 */
TINI_API int tini_save_to(tini_t *self, const char *path);

// -----------------------------------------------------------------------------
// Error Handling
// -----------------------------------------------------------------------------

/**
 * @brief Get the last error code
 * @param self tini object
 * @return error code, or -1 if self is NULL
 */
TINI_API int tini_last_error(const tini_t *self);

/**
 * @brief Get error description string
 * @param code error code
 * @return error description
 */
TINI_API const char *tini_error_string(int code);

// -----------------------------------------------------------------------------
// Section Operations
// -----------------------------------------------------------------------------

/**
 * @brief Get section by name, creating if not exists
 * @param self tini object
 * @param name section name
 * @return section, or NULL on failure
 */
TINI_API tini_section_t *tini_get_section(tini_t *self, const char *name);

/**
 * @brief Find section by name
 * @param self tini object
 * @param name section name
 * @return section, or NULL if not found
 */
TINI_API tini_section_t *tini_find_section(tini_t *self, const char *name);

/**
 * @brief Remove section by name
 * @param self tini object
 * @param name section name
 * @return 0 on success, error code otherwise
 */
TINI_API int tini_remove_section(tini_t *self, const char *name);

/**
 * @brief Get first section for iteration
 * @param self tini object
 * @return first section, or NULL if empty
 */
TINI_API tini_section_t *tini_first_section(const tini_t *self);

/**
 * @brief Get next section in iteration
 * @param section current section
 * @return next section, or NULL if at end
 */
TINI_API tini_section_t *tini_section_next(const tini_section_t *section);

/**
 * @brief Get section name
 * @param section section object
 * @return section name (owned by section), or NULL if invalid
 */
TINI_API const char *tini_section_name(const tini_section_t *section);

// -----------------------------------------------------------------------------
// Key Operations
// -----------------------------------------------------------------------------

/**
 * @brief Get key in section, creating if not exists
 * @param section section object
 * @param key key name
 * @return key, or NULL on failure
 */
TINI_API tini_key_t *tini_section_get_key(tini_section_t *section, const char *key);

/**
 * @brief Find key in section
 * @param section section object
 * @param key key name
 * @return key, or NULL if not found
 */
TINI_API tini_key_t *tini_section_find_key(tini_section_t *section, const char *key);

/**
 * @brief Check if key exists in section
 * @param section section object
 * @param key key name
 * @return true if exists
 */
TINI_API bool tini_section_has_key(tini_section_t *section, const char *key);

/**
 * @brief Add or update key with value
 * @param section section object
 * @param key key name
 * @param value string value
 * @return key, or NULL on failure
 */
TINI_API tini_key_t *tini_section_add_key(tini_section_t *section, const char *key, const char *value);

/**
 * @brief Remove key from section
 * @param section section object
 * @param key key name
 * @return 0 on success, error code otherwise
 */
TINI_API int tini_section_remove_key(tini_section_t *section, const char *key);

/**
 * @brief Get first key in section for iteration
 * @param section section object
 * @return first key, or NULL if empty
 */
TINI_API tini_key_t *tini_section_first_key(const tini_section_t *section);

/**
 * @brief Get next key in iteration
 * @param key current key
 * @return next key, or NULL if at end
 */
TINI_API tini_key_t *tini_key_next(const tini_key_t *key);

/**
 * @brief Get key name
 * @param key key object
 * @return key name (owned by key), or NULL if invalid
 */
TINI_API const char *tini_key_name(const tini_key_t *key);

// -----------------------------------------------------------------------------
// Convenience Functions
// -----------------------------------------------------------------------------

/**
 * @brief Get key by section and key name, creating both if not exist
 * @param self tini object
 * @param section section name (NULL or "" for default)
 * @param key key name
 * @return key, or NULL on failure
 */
TINI_API tini_key_t *tini_get_key(tini_t *self, const char *section, const char *key);

/**
 * @brief Find key by section and key name
 * @param self tini object
 * @param section section name (NULL or "" for default)
 * @param key key name
 * @return key, or NULL if not found
 */
TINI_API tini_key_t *tini_find_key(tini_t *self, const char *section, const char *key);

// -----------------------------------------------------------------------------
// Value Access & Conversion
// -----------------------------------------------------------------------------

/**
 * @brief Set raw string value
 * @param key key object
 * @param value new value
 */
TINI_API void tini_key_set_value(tini_key_t *key, const char *value);

/**
 * @brief Get raw string value
 * @param key key object
 * @return value (owned by key), or "" if invalid
 */
TINI_API const char *tini_key_get_value(const tini_key_t *key);

/**
 * @brief Get string value with default
 * @param key key object (may be NULL)
 * @param default_value fallback value
 * @return value or default_value
 */
TINI_API const char *tini_key_get(const tini_key_t *key, const char *default_value);

/**
 * @brief Get non-empty string value with default
 * @param key key object
 * @param default_value fallback if NULL, empty, or whitespace-only
 * @return value or default_value
 */
TINI_API const char *tini_key_get_string(const tini_key_t *key, const char *default_value);

/**
 * @brief Parse value as int
 * @param key key object
 * @param default_value fallback on parse failure
 * @return parsed value or default_value
 */
TINI_API int tini_key_get_int(const tini_key_t *key, int default_value);

/**
 * @brief Parse value as int64_t
 * @param key key object
 * @param default_value fallback on parse failure
 * @return parsed value or default_value
 * @note Supports decimal, hex (0x), octal (0) prefixes
 */
TINI_API int64_t tini_key_get_i64(const tini_key_t *key, int64_t default_value);

/**
 * @brief Parse value as uint64_t
 * @param key key object
 * @param default_value fallback on parse failure or negative input
 * @return parsed value or default_value
 * @note Rejects negative values
 */
TINI_API uint64_t tini_key_get_u64(const tini_key_t *key, uint64_t default_value);

/**
 * @brief Parse value as double
 * @param key key object
 * @param default_value fallback on parse failure
 * @return parsed value or default_value
 */
TINI_API double tini_key_get_double(const tini_key_t *key, double default_value);

/**
 * @brief Parse value as bool
 * @param key key object
 * @param default_value fallback on parse failure
 * @return parsed value or default_value
 * @note Accepts: 1/0, Y/N, y/n, T/F, t/f
 */
TINI_API bool tini_key_get_bool(const tini_key_t *key, bool default_value);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace tini {

class Key {
public:
	explicit Key(tini_key_t *k) : d(k) {}
	bool        valid() const { return d != nullptr; }
	explicit    operator bool() const { return valid(); }
	std::string name() const { return tini_key_name(d); }
	Key         next() const { return Key(tini_key_next(d)); }

	std::string   get(const std::string &def = "") const { return tini_key_get(d, def.c_str()); }
	std::string   getValue() const { return tini_key_get_value(d); }
	std::string   getString(const std::string &def = "") const { return tini_key_get_string(d, def.c_str()); }
	int           getInt(int def = 0) const { return tini_key_get_int(d, def); }
	long          getInteger(long def = 0) const { return tini_key_get_i64(d, def); }
	unsigned long getUnsigned(unsigned long def = 0) const { return tini_key_get_u64(d, def); }
	int64_t       getI64(int64_t def = 0) const { return tini_key_get_i64(d, def); }
	uint64_t      getU64(uint64_t def = 0) const { return tini_key_get_u64(d, def); }
	double        getDouble(double def = 0.0) const { return tini_key_get_double(d, def); }
	bool          getBool(bool def = false) const { return tini_key_get_bool(d, def); }

	void set(const std::string &value) { tini_key_set_value(d, value.c_str()); }
	void setValue(const std::string &value) { set(value); }
	void setValue(const char *value) { set(value); }
	template <typename T> typename std::enable_if<std::is_arithmetic<T>::value, void>::type setValue(T value) {
		set(std::to_string(value));
	}

private:
	tini_key_t *d;
};

class Section {
public:
	explicit Section(tini_section_t *s) : d(s) {}
	bool        valid() const { return d != nullptr; }
	explicit    operator bool() const { return valid(); }
	std::string name() const { return tini_section_name(d); }
	Key         firstKey() const { return Key(tini_section_first_key(d)); }
	Section     next() const { return Section(tini_section_next(d)); }

	Key  getKey(const std::string &key) { return Key(tini_section_get_key(d, key.c_str())); }
	Key  findKey(const std::string &key) const { return Key(tini_section_find_key(d, key.c_str())); }
	bool hasKey(const std::string &key) const { return tini_section_has_key(d, key.c_str()); }
	Key  addKey(const std::string &key, const std::string &value = "") {
        return Key(tini_section_add_key(d, key.c_str(), value.c_str()));
	}
	int removeKey(const std::string &key) { return tini_section_remove_key(d, key.c_str()); }

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

class Ini {
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

	int  load(const std::string &path) { return tini_load(d, path.c_str()); }
	void clear() { tini_clear(d); }
	int  saveTo(const std::string &path) { return tini_save_to(d, path.c_str()); }

	Section getSection(const std::string &name) { return Section(tini_get_section(d, name.c_str())); }
	Section findSection(const std::string &name) const { return Section(tini_find_section(d, name.c_str())); }
	int     removeSection(const std::string &name) { return tini_remove_section(d, name.c_str()); }

	Key getKey(const std::string &section, const std::string &key) {
		return Key(tini_get_key(d, section.c_str(), key.c_str()));
	}
	Key findKey(const std::string &section, const std::string &key) const {
		return Key(tini_find_key(d, section.c_str(), key.c_str()));
	}

	Section firstSection() const { return Section(tini_first_section(d)); }

	std::vector<std::string> sections() const {
		std::vector<std::string> result;
		for (Section s = firstSection(); s; s = s.next()) {
			result.emplace_back(s.name());
		}
		return result;
	}

private:
	tini_t *d;
};

}  // namespace tini

#endif

#endif  // TINI_TINI_H

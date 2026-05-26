// Copyright 2025 Dave R. Smith
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Utilities/Json.h>
#include <optional>
#include <string>

namespace ml
{
    /**
     * @brief Read text or JSON data from a file.
     *
     * ### Usage
     * @code
     * // Read JSON
     * auto data = ml::FileReader::readJson(path);
     * if (data) {
     *     std::string title = (*data)["title"];
     * }
     *
     * // Read plain text
     * auto text = ml::FileReader::readText(path);
     * if (text) { ... }
     * @endcode
     */
    class MALENA_API FileReader
    {
    public:
        /**
         * @brief Parse a JSON file.
         * @return The parsed @c ml::json, or @c std::nullopt if the file could
         *         not be opened or contains invalid JSON.
         */
        static std::optional<json> readJson(const std::string& path);

        /**
         * @brief Read the entire contents of a file as a string.
         * @return The file contents, or @c std::nullopt if the file could not be opened.
         */
        static std::optional<std::string> readText(const std::string& path);
    };

} // namespace ml

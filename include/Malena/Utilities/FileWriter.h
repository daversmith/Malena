// Copyright 2025 Dave R. Smith
// SPDX-License-Identifier: PolyForm-Noncommercial-1.0.0

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Utilities/Json.h>
#include <string>

namespace ml
{
    /**
     * @brief Write text or JSON data to a file.
     *
     * ### Usage
     * @code
     * // Write JSON
     * ml::json data = {{"title", "CS3A Quiz"}, {"questions", ml::json::array()}};
     * bool ok = ml::FileWriter::writeJson(path, data);
     *
     * // Write plain text
     * ml::FileWriter::writeText(path, "Hello, world!");
     * @endcode
     */
    class MALENA_API FileWriter
    {
    public:
        /**
         * @brief Serialize @p data to a pretty-printed JSON file.
         * @return @c true on success, @c false if the file could not be opened.
         */
        static bool writeJson(const std::string& path, const json& data, int indent = 4);

        /**
         * @brief Write a raw string to a file, overwriting any existing content.
         * @return @c true on success, @c false if the file could not be opened.
         */
        static bool writeText(const std::string& path, const std::string& text);
    };

} // namespace ml

// Copyright 2025 Dave R. Smith
// SPDX-License-Identifier: Apache-2.0

#include <Malena/Utilities/FileReader.h>
#include <fstream>
#include <sstream>

namespace ml
{

std::optional<json> FileReader::readJson(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open()) return std::nullopt;
    try
    {
        return json::parse(file);
    }
    catch (const json::parse_error&)
    {
        return std::nullopt;
    }
}

std::optional<std::string> FileReader::readText(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open()) return std::nullopt;
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

} // namespace ml

// Copyright 2025 Dave R. Smith
// SPDX-License-Identifier: PolyForm-Noncommercial-1.0.0

#include <Malena/Utilities/FileWriter.h>
#include <fstream>

namespace ml
{

bool FileWriter::writeJson(const std::string& path, const json& data, int indent)
{
    std::ofstream file(path);
    if (!file.is_open()) return false;
    file << data.dump(indent);
    return file.good();
}

bool FileWriter::writeText(const std::string& path, const std::string& text)
{
    std::ofstream file(path);
    if (!file.is_open()) return false;
    file << text;
    return file.good();
}

} // namespace ml

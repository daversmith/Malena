// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

#include <Malena/Utilities/Paths.h>

#include <cstdlib>
#include <filesystem>
#include <system_error>
#include <vector>

#if defined(__APPLE__)
#  include <mach-o/dyld.h>          // _NSGetExecutablePath
#elif defined(_WIN32)
#  include <windows.h>              // GetModuleFileNameW
#else
#  include <unistd.h>               // readlink
#endif

namespace ml {

namespace {

#if defined(_WIN32)
constexpr char kSep = '\\';
#else
constexpr char kSep = '/';
#endif

// Absolute path of the running executable, or "" when the platform call fails.
std::string findExecutablePath()
{
#if defined(__APPLE__)
    // Two-call idiom: the first tells us the buffer size it needs.
    uint32_t size = 0;
    _NSGetExecutablePath(nullptr, &size);
    std::vector<char> buf(size + 1, '\0');
    if (_NSGetExecutablePath(buf.data(), &size) != 0) return {};
    std::error_code ec;
    // Resolves symlinks so a symlinked launcher still locates its own bundle.
    const std::filesystem::path p = std::filesystem::canonical(buf.data(), ec);
    return ec ? std::string(buf.data()) : p.string();

#elif defined(_WIN32)
    std::vector<wchar_t> buf(MAX_PATH);
    for (;;)
    {
        const DWORD n = GetModuleFileNameW(nullptr, buf.data(),
                                           static_cast<DWORD>(buf.size()));
        if (n == 0) return {};
        // Truncation is reported by filling the buffer exactly — grow and retry.
        if (n < buf.size()) break;
        buf.resize(buf.size() * 2);
    }
    return std::filesystem::path(buf.data()).string();

#else
    std::error_code ec;
    const std::filesystem::path p = std::filesystem::read_symlink("/proc/self/exe", ec);
    return ec ? std::string{} : p.string();
#endif
}

// Home directory from the environment ("" when unset).
std::string homeDir()
{
#if defined(_WIN32)
    if (const char* p = std::getenv("USERPROFILE")) return p;
    return {};
#else
    if (const char* p = std::getenv("HOME")) return p;
    return {};
#endif
}

} // namespace

const std::string& Paths::executablePath()
{
    static const std::string path = findExecutablePath();
    return path;
}

const std::string& Paths::resourceDir()
{
    static const std::string dir = []() -> std::string
    {
        const std::string exe = executablePath();
        if (exe.empty()) return ".";   // keep the dev build's CWD-relative behaviour

        const std::filesystem::path exeDir = std::filesystem::path(exe).parent_path();

#if defined(__APPLE__)
        // Inside a bundle the executable sits at Foo.app/Contents/MacOS/Foo, so
        // Contents/Resources is one level up and across. Outside a bundle (a bare
        // CLI binary, e.g. the build tree) that structure isn't there — fall
        // through to the executable's own directory.
        const std::filesystem::path contents = exeDir.parent_path();
        if (exeDir.filename() == "MacOS" && contents.filename() == "Contents")
        {
            std::error_code ec;
            const std::filesystem::path res = contents / "Resources";
            if (std::filesystem::is_directory(res, ec)) return res.string();
        }
#endif
        return exeDir.string();
    }();
    return dir;
}

std::string Paths::userDataDir(const std::string& appName)
{
    std::filesystem::path base;

#if defined(__APPLE__)
    const std::string home = homeDir();
    if (!home.empty()) base = std::filesystem::path(home) / "Library" / "Application Support";
#elif defined(_WIN32)
    if (const char* appdata = std::getenv("APPDATA")) base = appdata;
    else if (const std::string home = homeDir(); !home.empty()) base = home;
#else
    if (const char* xdg = std::getenv("XDG_CONFIG_HOME"); xdg && *xdg) base = xdg;
    else if (const std::string home = homeDir(); !home.empty())
        base = std::filesystem::path(home) / ".config";
#endif

    if (base.empty()) return ".";   // no home and no platform var — stay where we are

    const std::filesystem::path dir = base / appName;
    std::error_code ec;
    std::filesystem::create_directories(dir, ec);
    if (ec && !std::filesystem::is_directory(dir)) return ".";
    return dir.string();
}

std::string Paths::join(const std::string& a, const std::string& b)
{
    if (a.empty()) return b;
    if (b.empty()) return a;
    if (a.back() == kSep || a.back() == '/') return a + b;
    return a + kSep + b;
}

} // namespace ml

// Copyright 2025 Dave R. Smith
// SPDX-License-Identifier: PolyForm-Noncommercial-1.0.0

#include <Malena/Utilities/FileDialog.h>
#include <nfd.h>

#ifdef __APPLE__
#import <AppKit/AppKit.h>
namespace {
    // Schedule focusing the dialog's filename text field shortly after the
    // modal run loop starts. NFD's [NSApp runModalForWindow:] / sheet loop
    // processes GCD main-queue blocks, so this fires while the dialog is open.
    void prepareDialog()
    {
        dispatch_after(
            dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.05 * NSEC_PER_SEC)),
            dispatch_get_main_queue(),
            ^{
                NSWindow* kw = [NSApp keyWindow];
                if (kw) [kw makeFirstResponder:[kw initialFirstResponder]];
            });
    }
}
#else
namespace { void prepareDialog() {} }
#endif

namespace ml
{

namespace
{
    // Build a temporary NFD filter list from our Filter structs.
    // The returned vector's c_str() pointers stay valid as long as the
    // source Filter vector lives (same call frame).
    //
    // These must be the U8 (char) item type, not the "native" one: every call
    // site below uses NFD's ...U8 entry points. On macOS/Linux nfdnchar_t IS
    // char so the two are the same type and the mismatch is invisible, but on
    // Windows the native type is wchar_t and the build fails (C2664).
    std::vector<nfdu8filteritem_t> makeNFDFilters(const std::vector<FileDialog::Filter>& filters)
    {
        std::vector<nfdu8filteritem_t> out;
        out.reserve(filters.size());
        for (const auto& f : filters)
            out.push_back({f.name.c_str(), f.spec.c_str()});
        return out;
    }
}

std::string FileDialog::open(const std::vector<Filter>& filters,
                              const std::string& defaultPath)
{
    NFD_Init();
    prepareDialog();

    const auto nfdFilters = makeNFDFilters(filters);
    nfdchar_t* raw = nullptr;
    const nfdresult_t result = NFD_OpenDialogU8(
        &raw,
        nfdFilters.empty() ? nullptr : nfdFilters.data(),
        static_cast<nfdfiltersize_t>(nfdFilters.size()),
        defaultPath.empty() ? nullptr : defaultPath.c_str());

    std::string path;
    if (result == NFD_OKAY)
    {
        path = raw;
        NFD_FreePathU8(raw);
    }

    NFD_Quit();
    return path;
}

std::string FileDialog::save(const std::vector<Filter>& filters,
                              const std::string& defaultPath,
                              const std::string& defaultName)
{
    NFD_Init();
    prepareDialog();

    const auto nfdFilters = makeNFDFilters(filters);
    nfdchar_t* raw = nullptr;
    const nfdresult_t result = NFD_SaveDialogU8(
        &raw,
        nfdFilters.empty() ? nullptr : nfdFilters.data(),
        static_cast<nfdfiltersize_t>(nfdFilters.size()),
        defaultPath.empty() ? nullptr : defaultPath.c_str(),
        defaultName.empty() ? nullptr : defaultName.c_str());

    std::string path;
    if (result == NFD_OKAY)
    {
        path = raw;
        NFD_FreePathU8(raw);
    }

    NFD_Quit();
    return path;
}

std::vector<std::string> FileDialog::openMultiple(const std::vector<Filter>& filters,
                                                   const std::string& defaultPath)
{
    NFD_Init();
    prepareDialog();

    const auto nfdFilters = makeNFDFilters(filters);
    const nfdpathset_t* pathSet = nullptr;
    const nfdresult_t result = NFD_OpenDialogMultipleU8(
        &pathSet,
        nfdFilters.empty() ? nullptr : nfdFilters.data(),
        static_cast<nfdfiltersize_t>(nfdFilters.size()),
        defaultPath.empty() ? nullptr : defaultPath.c_str());

    std::vector<std::string> paths;
    if (result == NFD_OKAY)
    {
        nfdpathsetsize_t count = 0;
        NFD_PathSet_GetCount(pathSet, &count);
        paths.reserve(count);

        for (nfdpathsetsize_t i = 0; i < count; ++i)
        {
            nfdchar_t* p = nullptr;
            if (NFD_PathSet_GetPathU8(pathSet, i, &p) == NFD_OKAY)
            {
                paths.emplace_back(p);
                NFD_PathSet_FreePathU8(p);
            }
        }
        NFD_PathSet_Free(pathSet);
    }

    NFD_Quit();
    return paths;
}

} // namespace ml

// Copyright 2025 Dave R. Smith
// SPDX-License-Identifier: PolyForm-Noncommercial-1.0.0

#pragma once

#include <Malena/Core/malena_export.h>
#include <string>
#include <vector>

namespace ml
{
    /**
     * @brief Thin wrapper around the native OS file-open / file-save dialog.
     *
     * All methods are blocking — the SFML event loop is suspended while the
     * dialog is open, which is normal behaviour for desktop applications.
     * Returns an empty string (or empty vector) when the user cancels.
     *
     * ### Usage
     * @code
     * // Open a single file
     * std::string path = ml::FileDialog::open({{"Quiz Files", "json,quiz"}});
     * if (!path.empty()) loadQuiz(path);
     *
     * // Save a file
     * std::string out = ml::FileDialog::save({{"JSON", "json"}}, "", "quiz.json");
     *
     * // Open multiple files
     * auto paths = ml::FileDialog::openMultiple({{"Images", "png,jpg"}});
     * @endcode
     */
    class MALENA_API FileDialog
    {
    public:
        struct Filter
        {
            std::string name; ///< Display label, e.g. "Quiz Files"
            std::string spec; ///< Comma-separated extensions, e.g. "json,quiz"
        };

        /** @brief Show an open-file dialog. Returns the selected path or "". */
        static std::string open(
            const std::vector<Filter>& filters     = {},
            const std::string&         defaultPath = "");

        /** @brief Show a save-file dialog. Returns the chosen path or "". */
        static std::string save(
            const std::vector<Filter>& filters      = {},
            const std::string&         defaultPath  = "",
            const std::string&         defaultName  = "");

        /** @brief Show a multi-select open dialog. Returns selected paths or {}. */
        static std::vector<std::string> openMultiple(
            const std::vector<Filter>& filters     = {},
            const std::string&         defaultPath = "");
    };

} // namespace ml

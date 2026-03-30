// Copyright 2025 Dave R. Smith
// SPDX-License-Identifier: Apache-2.0

#include <Malena/Resources/ThemeManager.h>
#include <Malena/Traits/Themeable.h>

namespace ml
{
    void ThemeManager::notify()
    {
        // Iterate a copy — a subscriber's onThemeApplied could theoretically
        // add or remove subscribers, which would invalidate the iterator.
        const std::vector<Themeable*> copy = _subscribers;
        for (Themeable* t : copy)
            t->onThemeApplied(*_active);
    }

} // namespace ml

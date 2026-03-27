// Copyright 2025 Dave R. Smith
// SPDX-License-Identifier: Apache-2.0

#include <Malena/Traits/Selectable.h>
#include <Malena/Engine/Events/Fireable.h>
#include <Malena/Engine/Events/Event.h>

namespace ml
{
    // ── onSelected ────────────────────────────────────────────────────────────

    void Selectable::onSelected(std::function<void()> callback)
    {
        Fireable::addCallback(ml::Event::SELECTED, this,
            [cb = std::move(callback)](const std::optional<sf::Event>&){ cb(); });
    }

    void Selectable::onSelected(std::function<void(const std::optional<sf::Event>&)> callback)
    {
        Fireable::addCallback(ml::Event::SELECTED, this, std::move(callback));
    }

    // ── onDeselected ──────────────────────────────────────────────────────────

    void Selectable::onDeselected(std::function<void()> callback)
    {
        Fireable::addCallback(ml::Event::DESELECTED, this,
            [cb = std::move(callback)](const std::optional<sf::Event>&){ cb(); });
    }

    void Selectable::onDeselected(std::function<void(const std::optional<sf::Event>&)> callback)
    {
        Fireable::addCallback(ml::Event::DESELECTED, this, std::move(callback));
    }

    // ── Programmatic triggers ─────────────────────────────────────────────────

    void Selectable::select()
    {
        process(ml::Event::SELECTED, std::nullopt);
    }

    void Selectable::deselect()
    {
        process(ml::Event::DESELECTED, std::nullopt);
    }

} // namespace ml

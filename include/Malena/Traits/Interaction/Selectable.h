//
// Selectable.h
//

#ifndef SELECTABLE_H
#define SELECTABLE_H

#pragma once

#include <Malena/Engine/Events/EventReceiver.h>
#include <Malena/Engine/Events/EventDispatcher.h>
#include <Malena/Engine/Events/Fireable.h>
#include <Malena/Engine/Events/Event.h>
#include <functional>
#include <optional>
#include <SFML/Window/Event.hpp>

namespace ml
{
    /**
     * @brief Trait that adds selection and deselection callbacks to any @c Core object.
     * @ingroup Traits
     *
     * @c Selectable is an opt-in trait for components that represent a choosable
     * option — radio buttons, dropdown items, list items, tab headers, and similar
     * controls. It provides @c onSelected and @c onDeselected callback hooks that
     * fire when the component is programmatically selected or deselected.
     *
     * Unlike @c Clickable, @c Selectable does not react to SFML events directly.
     * Selection is always triggered programmatically — typically by a container
     * such as @c RadioGroup or @c Dropdown calling @c select() or @c deselect()
     * on a specific component instance. The dispatcher exists only to register
     * the event keys with the framework; it never fires on its own.
     *
     * ### Usage
     * @code
     * class MyItem : public ml::ComponentWith<MyManifest>, public ml::Selectable
     * {
     * public:
     *     MyItem()
     *     {
     *         onSelected([this]{ setFillColor(sf::Color::Blue); });
     *         onDeselected([this]{ setFillColor(sf::Color::White); });
     *     }
     * };
     *
     * // Programmatic selection — fires the callbacks
     * item.select();
     * item.deselect();
     * @endcode
     *
     * To unsubscribe:
     * @code
     * item.unsubscribe(ml::Event::SELECTED);
     * item.unsubscribe(ml::Event::DESELECTED);
     * @endcode
     *
     * @see RadioButton, ml::Event::SELECTED, ml::Event::DESELECTED, Clickable
     */
    class Selectable : public EventReceiver
    {
    public:
        // ── onSelected ────────────────────────────────────────────────────────

        /**
         * @brief Register a no-argument callback invoked when this component
         *        is selected.
         *
         * @param callback Function invoked with no arguments on selection.
         */
        void onSelected(std::function<void()> callback);

        /**
         * @brief Register a callback invoked when this component is selected,
         *        receiving the raw SFML event (always @c std::nullopt for
         *        programmatic selection).
         *
         * @param callback Function invoked with the SFML event on selection.
         */
        void onSelected(std::function<void(const std::optional<sf::Event>&)> callback);

        // ── onDeselected ──────────────────────────────────────────────────────

        /**
         * @brief Register a no-argument callback invoked when this component
         *        is deselected.
         *
         * @param callback Function invoked with no arguments on deselection.
         */
        void onDeselected(std::function<void()> callback);

        /**
         * @brief Register a callback invoked when this component is deselected,
         *        receiving the raw SFML event (always @c std::nullopt for
         *        programmatic deselection).
         *
         * @param callback Function invoked with the SFML event on deselection.
         */
        void onDeselected(std::function<void(const std::optional<sf::Event>&)> callback);

        // ── Programmatic triggers ─────────────────────────────────────────────

        /**
         * @brief Fire the @c onSelected callbacks on this component.
         *
         * Called by container types (@c RadioGroup, @c Dropdown, etc.) to
         * notify the component that it has been chosen. Does not interact with
         * the @c Flag or @c State system — that remains the responsibility of
         * the concrete component class.
         */
        void select();

        /**
         * @brief Fire the @c onDeselected callbacks on this component.
         *
         * Called by container types to notify the component that it is no
         * longer the chosen option.
         */
        void deselect();
    };

} // namespace ml

#endif // SELECTABLE_H

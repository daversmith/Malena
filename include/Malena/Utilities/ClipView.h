// Copyright (c) 2025 Dave R. Smith.
// Malena Framework — Licensed under PolyForm Noncommercial 1.0.0; commercial use requires a paid license. See LICENSE.

#ifndef MALENA_CLIPVIEW_H
#define MALENA_CLIPVIEW_H

#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/Rect.hpp>

namespace ml
{
    /**
     * @brief Render @p body with an @c sf::View clipped to @p rect (world coords).
     * @ingroup Utilities
     *
     * Wraps the standard "map a world-space rectangle to viewport pixels,
     * push a temporary view, run the body, restore the previous view"
     * pattern used wherever a component wants its children clipped to a
     * sub-region (@c ScrollPane, @c TabbedPanel content, @c SplitPanel
     * panes).
     *
     * @code
     * ml::withClipView(target, contentRect(), [&]{
     *     ComponentBase::draw(target, states);  // children, clipped
     * });
     * @endcode
     *
     * @note Maps through the @b active view so it composes correctly when
     *       nested inside another clipped region.
     * @note If @p rect has non-positive width or height, @p body runs
     *       without modifying the view (clip is degenerate).
     */
    template<typename F>
    void withClipView(sf::RenderTarget& target, sf::FloatRect rect, F&& body)
    {
        if (rect.size.x <= 0.f || rect.size.y <= 0.f)
        {
            body();
            return;
        }

        const auto  sz = target.getSize();
        const float tw = static_cast<float>(sz.x);
        const float th = static_cast<float>(sz.y);
        const sf::View saved = target.getView();

        const sf::Vector2i tl = target.mapCoordsToPixel(rect.position, saved);
        const sf::Vector2i br = target.mapCoordsToPixel(
            {rect.position.x + rect.size.x, rect.position.y + rect.size.y}, saved);

        sf::View clip;
        clip.setCenter({rect.position.x + rect.size.x / 2.f,
                        rect.position.y + rect.size.y / 2.f});
        clip.setSize(rect.size);
        clip.setViewport(sf::FloatRect{
            {tl.x / tw, tl.y / th},
            {(br.x - tl.x) / tw, (br.y - tl.y) / th}
        });

        target.setView(clip);
        body();
        target.setView(saved);
    }

} // namespace ml

#endif // MALENA_CLIPVIEW_H

#ifndef SCROLLPANE_H
#define SCROLLPANE_H

#include <Malena/Core/malena_export.h>
#include <SFML/Graphics.hpp>
#include <vector>
#include <Malena/Core/Component.h>
#include <Malena/Graphics/Primitives/Rectangle.h>
#include <Malena/Manifests/ManifestResources.h>
#include "ScrollPaneManifest.h"

namespace ml
{
    class MALENA_API ScrollPane : public ml::Component<ScrollPaneManifest>
    {
    public:
        using App   = ml::ManifestResources<ScrollPaneManifest>;
        using State = ScrollPaneManifest::State;
        using Flag  = ScrollPaneManifest::Flag;

        ScrollPane(float width, float height);

        void addComponent(ml::Core& component);
        void removeComponent(ml::Core& component);

        // ── Styling ───────────────────────────────────────────────────────────
        void setBackgroundColor(sf::Color color);
        void setScrollBarColor(sf::Color color);
        void setScrollBarWidth(float width);

        // ── Layout ────────────────────────────────────────────────────────────
        void setPosition(const sf::Vector2f& position) override;
        sf::Vector2f  getPosition()     const override;
        sf::FloatRect getGlobalBounds() const override;
        sf::RenderStates getRenderStates() const override;
        void setSize(float width, float height);

        // ── External scroll control ───────────────────────────────────────────

        /** @brief Return the current vertical scroll offset in pixels. */
        [[nodiscard]] float getScrollOffsetY() const;

        /** @brief Programmatically set the vertical scroll offset (clamped). */
        void setScrollOffsetY(float y);

        /**
         * @brief Override the content height used for scroll calculations.
         *
         * Use when content is rendered externally (e.g. TextArea canvas).
         * Pass 0 to revert to summing children heights.
         */
        void setContentHeight(float height);

        /**
         * @brief Silence this ScrollPane from the event system entirely.
         *
         * Call this when using ScrollPane as a private member of another
         * component rather than as a standalone @c addComponent() citizen.
         * Removes ALL event subscriptions from both the pane and its internal
         * scrollbar thumb so they cannot steal focus or intercept events
         * from the host component.
         *
         * ScrollPane remains fully functional as a draw helper —
         * @c setPosition, @c setSize, @c setScrollOffsetY, @c setContentHeight,
         * and @c draw() all continue to work normally.
         *
         * Scroll wheel and thumb drag become the host component's
         * responsibility when embedded.
         *
         * ### Usage
         * @code
         * class MyWidget : public ml::Component<>
         * {
         *     ml::ScrollPane _pane{400.f, 300.f};
         *
         *     MyWidget() {
         *         _pane.embed(); // silence before registering any other events
         *     }
         * };
         * @endcode
         *
         * @see unsubscribeAll
         */
        void embed();

    protected:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    private:
        float _width;
        float _height;
        float _scrollOffsetY      = 0.f;
        float _scrollOffsetX      = 0.f;
        float _scrollSpeed        = 20.f;
        float _scrollBarWidth     = 8.f;
        float _contentHeightOverride = 0.f; ///< Non-zero overrides getTotalContentHeight()

        sf::Vector2f _position = {0.f, 0.f};

        sf::RectangleShape         _background;
        mutable sf::RectangleShape _scrollBarTrack;
        mutable ml::Rectangle      _scrollBarThumb;

        mutable sf::RenderTexture  _renderTexture;
        std::vector<ml::Core*>     _children;

        sf::Color _scrollBarTrackColor = sf::Color(200, 200, 200, 150);
        sf::Color _scrollBarThumbColor = sf::Color(100, 100, 100, 200);

        void  stackChildren()           const;
        float getTotalContentHeight()   const;
        float getTotalContentWidth()    const;
        void  updateScrollBar()         const;
    };
}

#endif //SCROLLPANE_H

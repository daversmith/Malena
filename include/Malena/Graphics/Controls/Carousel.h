//
// Carousel.h
//

#ifndef CAROUSEL_H
#define CAROUSEL_H

#include <Malena/Core/Core.h>
#include <Malena/Traits/Customizable.h>
#include <Malena/Manifests/Manifest.h>
#include <Malena/Graphics/Primitives/Rectangle.h>
#include <Malena/Resources/FontManager.h>
#include <list>
#include <optional>
#include <Malena/Utilities/ShaderLibrary.h>
#include <Malena/Core/Component.h>

namespace ml
{
    /**
     * @brief Manifest for the @c Carousel component.
      * @ingroup GraphicsControls
     *
     * Declares the flags, states, and supporting enums that drive
     * carousel behavior. These are gathered automatically into the
     * @c Carousel component's flag and state stores.
     */
    class CarouselManifest : public ml::Manifest
    {
    public:
        /**
         * @brief Runtime boolean flags for the carousel.
         *
         * | Flag | Effect |
         * |------|--------|
         * | @c SCROLLING    | Set while a scroll animation is in progress |
         * | @c SHOW_ARROWS  | Controls whether navigation arrows are visible |
         */
        enum class Flag  { SCROLLING, SHOW_ARROWS };

        /**
         * @brief Scroll animation states.
         *
         * | State | Meaning |
         * |-------|---------|
         * | @c SCROLL_IDLE  | No scroll in progress |
         * | @c SCROLL_LEFT  | Scrolling toward previous items |
         * | @c SCROLL_RIGHT | Scrolling toward next items |
         */
        enum class State { SCROLL_IDLE, SCROLL_LEFT, SCROLL_RIGHT };

        /**
         * @brief Controls where navigation arrows are placed relative to the content.
         *
         * | Value | Placement |
         * |-------|-----------|
         * | @c SIDES   | Arrows flank the content on the left and right (default) |
         * | @c TOP     | Both arrows sit above the carousel |
         * | @c OVERLAY | Arrows float on top of the content near the edges |
         */
        enum class ArrowPlacement { SIDES, TOP, OVERLAY };

        /// Identifies which navigation arrow to target in setter calls.
        enum class ArrowSide { LEFT, RIGHT };

        /// Shader visual style — aliases @c ShaderLibrary::Style.
        using Style = ShaderLibrary::Style;
    };

    /**
     * @brief A scrollable container that presents a collection of @c Core objects.
     *
     * @c Carousel renders its child components into an internal
     * @c sf::RenderTexture and displays a windowed view of them, supporting
     * smooth animated scrolling, optional navigation arrows, and GLSL shader
     * effects via @c ShaderLibrary.
     *
     * ### Adding and removing items
     * @code
     * ml::Carousel carousel;
     * carousel.setSize({800.f, 200.f});  // via setPreviewCount + item sizes
     * carousel.setPosition({50.f, 100.f});
     * carousel.add(thumbnailA);
     * carousel.add(thumbnailB);
     * carousel.add(thumbnailC);
     * addComponent(carousel);
     * @endcode
     *
     * ### Navigation
     * @code
     * carousel.next();      // scroll right by one item
     * carousel.previous();  // scroll left by one item
     *
     * // Or show built-in arrows
     * carousel.showArrows(true);
     * carousel.setArrowPlacement(ml::CarouselManifest::ArrowPlacement::SIDES);
     * @endcode
     *
     * ### Shader effects
     * @code
     * carousel.setStyle(ml::CarouselManifest::Style::ARC); // built-in shader
     *
     * // Or supply a custom shader
     * carousel.setShader(&myShader, [](sf::Shader& s){
     *     s.setUniform("uTime", clock.getElapsedTime().asSeconds());
     * });
     * @endcode
     *
     * @see CarouselManifest, ComponentWith, ShaderLibrary
     */
    class Carousel : public ComponentWith<CarouselManifest>
    {
    public:
        /// @brief Convenience alias for @c CarouselManifest::Flag.
        using Flag           = CarouselManifest::Flag;
        /// @brief Convenience alias for @c CarouselManifest::State.
        using State          = CarouselManifest::State;
        /// @brief Convenience alias for @c CarouselManifest::Style.
        using Style          = CarouselManifest::Style;
        /// @brief Convenience alias for @c CarouselManifest::ArrowPlacement.
        using ArrowPlacement = CarouselManifest::ArrowPlacement;
        /// @brief Convenience alias for @c CarouselManifest::ArrowSide.
        using ArrowSide      = CarouselManifest::ArrowSide;

    private:
        std::list<ml::Core*> _components;
        sf::RenderTexture    _renderTexture;
        sf::Sprite           _sprite;
        sf::View             _carouselView;

        // Shader
        CarouselManifest::Style          _currentStyle = CarouselManifest::Style::DEFAULT;
        sf::Shader*                      _shader = nullptr;
        std::unique_ptr<sf::Shader>      _ownedShader;
        std::function<void(sf::Shader&)> _shaderUniformSetter;
        float                            _shaderTime = 0.f;
        ShaderLibrary::Params            _shaderParams;

        // Scroll state
        float _spacing      = 10.f;
        float _speed        = 1.f;
        float _viewX        = 0.f;
        float _targetViewX  = 0.f;
        int   _previewCount = 5;

        sf::Vector2f _position = {0.f, 0.f};

        // Arrow controls
        ml::Rectangle              _leftArrow;
        ml::Rectangle              _rightArrow;
        sf::Text                   _leftLabel;
        sf::Text                   _rightLabel;
        ArrowPlacement             _arrowPlacement  = ArrowPlacement::SIDES;
        float                      _arrowSize       = 40.f;
        sf::Color                  _arrowColor      = sf::Color(80,  80,  80,  200);
        sf::Color                  _arrowHoverColor = sf::Color(120, 120, 120, 230);
        std::optional<sf::Texture> _leftArrowTexture;
        std::optional<sf::Texture> _rightArrowTexture;
        std::optional<sf::Vector2f> _leftArrowManualPos;
        std::optional<sf::Vector2f> _rightArrowManualPos;

        void updateView();
        void wrapComponents();
        void updateRenderTextureSize();
        void updateArrowPositions();
        void drawTexture();
        void loadShaderForStyle(Style style);
        void initArrows();
        bool isOnLeftArrow(sf::Vector2f screenPos) const;
        bool isOnRightArrow(sf::Vector2f screenPos) const;

    protected:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    public:
        Carousel();

        // ── Item management ──────────────────────────────────────────────────

        /**
         * @brief Add a @c Core object to the carousel.
         *
         * Items are displayed in insertion order. The carousel takes a
         * non-owning pointer; the caller is responsible for the item's lifetime.
         *
         * @param component The component to add.
         */
        void add(ml::Core& component);

        /**
         * @brief Remove a @c Core object from the carousel.
         *
         * @param component The component to remove. Safe to call even if
         *                  @p component is not currently in the carousel.
         */
        void remove(ml::Core& component);

        // ── Positionable overrides ───────────────────────────────────────────

        void          setPosition(const sf::Vector2f& position) override;
        sf::Vector2f  getPosition() const override;
        sf::FloatRect getGlobalBounds() const override;
        sf::RenderStates getRenderStates() const override;

        // ── Navigation ───────────────────────────────────────────────────────

        /**
         * @brief Scroll to the next item (right).
         *
         * Animates the view toward the next item at the current @c speed.
         * Sets @c Flag::SCROLLING and @c State::SCROLL_RIGHT while animating.
         */
        void next();

        /**
         * @brief Scroll to the previous item (left).
         *
         * Animates the view toward the previous item at the current @c speed.
         * Sets @c Flag::SCROLLING and @c State::SCROLL_LEFT while animating.
         */
        void previous();

        // ── Content settings ─────────────────────────────────────────────────

        /**
         * @brief Set the pixel gap between items.
         * @param spacing Gap in pixels. Defaults to @c 10.
         */
        void  setSpacing(float spacing);

        /** @brief Return the current item spacing in pixels. */
        float getSpacing() const;

        /**
         * @brief Set the scroll animation speed.
         * @param speed Pixels per frame (or per tick). Defaults to @c 1.
         */
        void  setSpeed(float speed);

        /** @brief Return the current scroll speed. */
        float getSpeed() const;

        /**
         * @brief Set how many items are visible in the carousel window at once.
         * @param count Number of visible items. Defaults to @c 5.
         */
        void setPreviewCount(int count);

        /** @brief Return the current visible item count. */
        int  getPreviewCount() const;

        // ── Shader settings ──────────────────────────────────────────────────

        /**
         * @brief Set a built-in shader style from @c ShaderLibrary.
         *
         * Loads and activates one of the pre-built GLSL shaders. Replaces
         * any previously set custom shader.
         *
         * @param style The @c ShaderLibrary::Style to apply.
         */
        void  setStyle(Style style);

        /** @brief Return the currently active built-in style. */
        Style getStyle() const;

        /**
         * @brief Set a custom GLSL shader with a per-frame uniform setter.
         *
         * The @p uniformSetter is called once per frame before rendering so
         * you can update time-dependent or state-dependent uniforms.
         *
         * @param shader         Pointer to the @c sf::Shader to use. Must
         *                       remain valid for the lifetime of the carousel.
         * @param uniformSetter  Callback invoked each frame with the shader
         *                       reference, used to push uniform values.
         */
        void setShader(sf::Shader* shader, std::function<void(sf::Shader&)> uniformSetter);

        /**
         * @brief Set parameters forwarded to the active @c ShaderLibrary style.
         * @param params Parameter struct with style-specific fields.
         */
        void setShaderParams(const ShaderLibrary::Params& params);

        /**
         * @brief Return a mutable reference to the current shader parameters.
         * @return Reference to the internal @c ShaderLibrary::Params.
         */
        ShaderLibrary::Params& getShaderParams();

        // ── Arrow settings ───────────────────────────────────────────────────

        /**
         * @brief Show or hide the navigation arrows.
         * @param show @c true to show arrows, @c false to hide.
         */
        void showArrows(bool show);

        /**
         * @brief Set the layout strategy for navigation arrows.
         * @param placement One of @c SIDES, @c TOP, or @c OVERLAY.
         */
        void setArrowPlacement(ArrowPlacement placement);

        /**
         * @brief Set the size of both navigation arrows.
         * @param size Arrow width/height in pixels.
         */
        void setArrowSize(float size);

        /**
         * @brief Set the default fill color of the navigation arrows.
         * @param color Fill color when the mouse is not hovering.
         */
        void setArrowColor(sf::Color color);

        /**
         * @brief Set the fill color of a navigation arrow on hover.
         * @param color Fill color when the mouse is hovering over an arrow.
         */
        void setArrowHoverColor(sf::Color color);

        /**
         * @brief Replace the default arrow rectangle with a custom texture.
         * @param side    Which arrow to replace (@c LEFT or @c RIGHT).
         * @param texture The @c sf::Texture to use.
         */
        void setArrowTexture(ArrowSide side, sf::Texture texture);

        /**
         * @brief Override the auto-computed position of one arrow.
         *
         * Bypasses the @c ArrowPlacement preset for the specified side.
         * Call @c clearArrowPosition to revert.
         *
         * @param side     Which arrow to reposition.
         * @param position Absolute world-space position for the arrow.
         */
        void setArrowPosition(ArrowSide side, sf::Vector2f position);

        /**
         * @brief Revert an arrow to its placement-preset position.
         * @param side Which arrow to reset.
         */
        void clearArrowPosition(ArrowSide side);
    };

} // namespace ml

#endif // CAROUSEL_H

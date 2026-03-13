//
// Carousel.h
//

#ifndef CAROUSEL_H
#define CAROUSEL_H

#include <Malena/Interfaces/Core.h>
#include <Malena/Traits/Customizable.h>
#include <Malena/Manifests/Manifest.h>
#include <Malena/Graphics/Rectangle.h>
#include <Malena/Managers/FontManager.h>
#include <list>
#include <optional>
#include <Malena/Utilities/ShaderLibrary.h>
#include "Malena/Components/Component.h"

namespace ml
{
    class CarouselManifest : public ml::Manifest
    {
    public:
        enum class Flag  { SCROLLING, SHOW_ARROWS };
        enum class State { SCROLL_IDLE, SCROLL_LEFT, SCROLL_RIGHT };

        enum class ArrowPlacement
        {
            SIDES,    // arrows flank the carousel content (default)
            TOP,      // both arrows sit above the carousel
            OVERLAY   // arrows float on top of the content near edges
        };

        enum class ArrowSide { LEFT, RIGHT };

        using Style = ShaderLibrary::Style;
    };

    class Carousel : public ComponentWith<CarouselManifest>
    {
    public:
        using Flag           = CarouselManifest::Flag;
        using State          = CarouselManifest::State;
        using Style          = CarouselManifest::Style;
        using ArrowPlacement = CarouselManifest::ArrowPlacement;
        using ArrowSide      = CarouselManifest::ArrowSide;

    private:
        std::list<ml::Core*> _components;
        sf::RenderTexture    _renderTexture;
        sf::Sprite           _sprite;
        sf::View             _carouselView;

        // Shader
        CarouselManifest::Style         _currentStyle = CarouselManifest::Style::DEFAULT;
        sf::Shader*                     _shader = nullptr;
        std::unique_ptr<sf::Shader>     _ownedShader;
        std::function<void(sf::Shader&)> _shaderUniformSetter;
        float                           _shaderTime = 0.f;
        ShaderLibrary::Params           _shaderParams;

        // Scroll state
        float _spacing      = 10.f;
        float _speed        = 1.f;
        float _viewX        = 0.f;
        float _targetViewX  = 0.f;
        int   _previewCount = 5;

        sf::Vector2f _position = {0.f, 0.f};

        // Arrow controls
        ml::Rectangle                _leftArrow;
        ml::Rectangle                _rightArrow;
        sf::Text                     _leftLabel;
        sf::Text                     _rightLabel;
        ArrowPlacement               _arrowPlacement   = ArrowPlacement::SIDES;
        float                        _arrowSize        = 40.f;
        sf::Color                    _arrowColor       = sf::Color(80,  80,  80,  200);
        sf::Color                    _arrowHoverColor  = sf::Color(120, 120, 120, 230);
        std::optional<sf::Texture>   _leftArrowTexture;
        std::optional<sf::Texture>   _rightArrowTexture;
        std::optional<sf::Vector2f>  _leftArrowManualPos;
        std::optional<sf::Vector2f>  _rightArrowManualPos;

        // Internal helpers
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

        void add(ml::Core& component);
        void remove(ml::Core& component);

        void         setPosition(const sf::Vector2f& position) override;
        sf::Vector2f getPosition() const override;
        sf::FloatRect getGlobalBounds() const override;
        sf::RenderStates getRenderStates() const override;

        void next();
        void previous();

        // Content settings
        void  setSpacing(float spacing);
        float getSpacing() const;
        void  setSpeed(float speed);
        float getSpeed() const;
        void  setPreviewCount(int count);
        int   getPreviewCount() const;

        // Shader settings
        void                  setStyle(Style style);
        Style                 getStyle() const;
        void                  setShader(sf::Shader* shader, std::function<void(sf::Shader&)> uniformSetter);
        void                  setShaderParams(const ShaderLibrary::Params& params);
        ShaderLibrary::Params& getShaderParams();

        // Arrow settings
        void showArrows(bool show);
        void setArrowPlacement(ArrowPlacement placement);
        void setArrowSize(float size);
        void setArrowColor(sf::Color color);
        void setArrowHoverColor(sf::Color color);
        void setArrowTexture(ArrowSide side, sf::Texture texture);
        void setArrowPosition(ArrowSide side, sf::Vector2f position);  // manual override
        void clearArrowPosition(ArrowSide side);                        // revert to placement preset
    };

} // namespace ml

#endif //CAROUSEL_H
//
// Carousel.h
//

#ifndef CAROUSEL_H
#define CAROUSEL_H

#include <Malena/Interfaces/UIComponent.h>
#include <Malena/Traits/Customizable.h>
#include <Malena/Manifests/Manifest.h>
#include <list>
#include <Malena/Utilities/ShaderLibrary.h>

namespace ml
{
	class CarouselManifest : public ml::Manifest
	{
	public:
	    enum class Flags {SCROLLING};
	    enum class State {SCROLL_IDLE, SCROLL_LEFT, SCROLL_RIGHT};
	    using Style = ShaderLibrary::Style;
	};

	class Carousel : public ml::Customizable<ml::UIComponent, CarouselManifest>
	{
	public:
	    typedef CarouselManifest::Flags Flag;
	    typedef CarouselManifest::State State;
	    typedef CarouselManifest::Style Style;
	private:
	    std::list<ml::UIComponent*> _components;
	    sf::RenderTexture _renderTexture;
	    sf::Sprite _sprite;
	    sf::View _carouselView;
	    CarouselManifest::Style _currentStyle = CarouselManifest::Style::DEFAULT;
	    sf::Shader* _shader = nullptr;
	    std::unique_ptr<sf::Shader> _ownedShader;
	    std::function<void(sf::Shader&)> _shaderUniformSetter;
	    float _shaderTime = 0.f;  // For animated shaders
	    float _spacing = 10.f;
	    float _speed = 1.f;
	    float _viewX = 0.f;
	    float _targetViewX = 0.f;
	    int _previewCount = 5.f;
	    ShaderLibrary::Params _shaderParams;

	    sf::Vector2f _position = {0.f, 0.f};

	    void updateView();
	    void wrapComponents();
	    void updateRenderTextureSize();
	    void drawTexture();
	    void loadShaderForStyle(Style style);


	protected:
	    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

	public:
	    Carousel();

	    void add(ml::UIComponent& component);
	    void remove(ml::UIComponent& component);

	    void setPosition(const sf::Vector2f &position) override;
	    sf::Vector2f getPosition() const override;
	    sf::FloatRect getGlobalBounds() const override;

	    void next();
	    void previous();

	    void setSpacing(float spacing);
	    float getSpacing() const;

	    void setSpeed(float speed);
	    float getSpeed() const;

	    void setPreviewCount(int count);
	    int getPreviewCount() const;
	    void setStyle(Style style);
	    Style getStyle() const;
	    void setShader(sf::Shader* shader, std::function<void(sf::Shader&)> uniformSetter);
	    void setShaderParams(const ShaderLibrary::Params& params);
	    ShaderLibrary::Params& getShaderParams();
	};
}

#endif //CAROUSEL_H
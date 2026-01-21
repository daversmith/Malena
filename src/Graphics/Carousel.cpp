//
// Carousel.cpp
//

#include <Malena/Graphics/Carousel.h>

namespace ml
{

	Carousel::Carousel() :
	    _renderTexture({720, 480}),
	    _sprite(_renderTexture.getTexture())
	{
	    setState(State::SCROLL_IDLE);
	    _sprite.setPosition(_position);
	    _sprite.setColor(sf::Color::White);

	    onUpdate([this]()
	    {
	        if (checkFlag(Flag::SCROLLING))
	        {
	            updateView();
	            wrapComponents();
	        }
	        drawTexture();
	    });
	}

	void Carousel::add(ml::UIComponent &component)
	{
	    if (_components.empty())
	    {
	        component.setPosition({0, 0});
	    }
	    else
	    {
	        // Position to the right of the last component
	        auto* last = _components.back();
	        float newX = last->getPosition().x + last->getGlobalBounds().size.x + _spacing;
	        component.setPosition({newX, 0});
	    }

	    _components.push_back(&component);
	    updateRenderTextureSize();
	}

	void Carousel::remove(ml::UIComponent &component)
	{
	    _components.remove(&component);
	    updateRenderTextureSize();
	}

	void Carousel::setPosition(const sf::Vector2f &position)
	{
	    _position = position;
	    _sprite.setPosition(position);
	}

	sf::Vector2f Carousel::getPosition() const
	{
	    return _position;
	}

	sf::FloatRect Carousel::getGlobalBounds() const
	{
	    return _sprite.getGlobalBounds();
	}

	void Carousel::next()
	{
	    if (checkFlag(Flag::SCROLLING) || _components.empty()) return;
	    if (_components.size() <= _previewCount) return;
	    setState(State::SCROLL_RIGHT);

	    // Calculate distance to scroll (width of first visible component)
	    float scrollDistance = _components.front()->getGlobalBounds().size.x + _spacing;
	    _targetViewX += scrollDistance;

	    enableFlag(Flag::SCROLLING);
	}

	void Carousel::previous()
	{
	    if (checkFlag(Flag::SCROLLING) || _components.empty()) return;
	    if (_components.size() <= _previewCount) return;
	    setState(State::SCROLL_LEFT);

	    // Calculate distance to scroll (width of last component in buffer)
	    float scrollDistance = _components.back()->getGlobalBounds().size.x + _spacing;
	    _targetViewX -= scrollDistance;

	    enableFlag(Flag::SCROLLING);
	}

	void Carousel::wrapComponents()
	{
	    if (_components.size() <= 1) return;

	    float viewLeft = _viewX;
	    float viewRight = _viewX + _sprite.getTextureRect().size.x;

	    // Check if front component has scrolled off to the left
	    auto* front = _components.front();
	    if (front->getPosition().x + front->getGlobalBounds().size.x < viewLeft - _spacing)
	    {
	        // Move to the right of the last component
	        auto* back = _components.back();
	        float newX = back->getPosition().x + back->getGlobalBounds().size.x + _spacing;
	        front->setPosition({newX, 0});

	        // Rotate list
	        _components.push_back(front);
	        _components.pop_front();
	    }

	    // Check if back component has scrolled off to the right
	    auto* back = _components.back();
	    if (back->getPosition().x > viewRight + _spacing)
	    {
	        // Move to the left of the front component
	        auto* newFront = _components.front();
	        float newX = newFront->getPosition().x - back->getGlobalBounds().size.x - _spacing;
	        back->setPosition({newX, 0});

	        // Rotate list
	        _components.push_front(back);
	        _components.pop_back();
	    }
	}
	void Carousel::setSpacing(float spacing)
	{
	    _spacing = spacing;
	}

	float Carousel::getSpacing() const
	{
	    return _spacing;
	}

	void Carousel::setSpeed(float speed)
	{
	    _speed = speed;
	}

	float Carousel::getSpeed() const
	{
	    return _speed;
	}

	void Carousel::setPreviewCount(int count)
	{
	    if (count < 1) count = 1;
	    _previewCount = count;
	    updateRenderTextureSize();
	}

	int Carousel::getPreviewCount() const
	{
	    return _previewCount;
	}

	void Carousel::updateRenderTextureSize()
	{
	    if (_components.empty()) return;

	    // Calculate visible width
	    float visibleWidth = 0;
	    auto iter = _components.begin();
	    for (int i = 0; i < std::min(_previewCount, (int)_components.size()); i++)
	    {
	        visibleWidth += (*iter)->getGlobalBounds().size.x;
	        if (i < _previewCount - 1)
	            visibleWidth += _spacing;
	        ++iter;
	    }

	    float height = _components.front()->getGlobalBounds().size.y;

	    // Texture is just the visible size - no extra width needed
	    _renderTexture.resize(sf::Vector2u(static_cast<unsigned int>(visibleWidth),
	                                       static_cast<unsigned int>(height)));

	    // View matches the texture size
	    _carouselView.setSize({visibleWidth, height});
	    _carouselView.setCenter({visibleWidth / 2.f, height / 2.f});

	    _viewX = 0.f;
	    _targetViewX = 0.f;

	    _sprite.setTexture(_renderTexture.getTexture(), true);
	}
	void Carousel::updateView()
	{
	    float diff = _targetViewX - _viewX;

	    if (std::abs(diff) < 0.1f)
	    {
	        // Snap when very close
	        _viewX = _targetViewX;
	        disableFlag(Flag::SCROLLING);
	        setState(State::SCROLL_IDLE);
	    }
	    else
	    {
	        // Move at constant velocity based on speed
	        float velocity = _speed;  // pixels per frame
	        float step = std::min(std::abs(diff), velocity);
	        _viewX += (diff > 0) ? step : -step;
	    }

	    // Update view center
	    float viewWidth = _carouselView.getSize().x;
	    float viewHeight = _carouselView.getSize().y;
	    _carouselView.setCenter({_viewX + viewWidth / 2.f, viewHeight / 2.f});
	}

	void Carousel::drawTexture()
	{
	    _renderTexture.setView(_carouselView);  // Apply the scrolling view
	    _renderTexture.clear(sf::Color::Transparent);

	    for (const auto* component : _components)
	    {
	        _renderTexture.draw(*component);
	    	// component->draw(_renderTexture, component->getRenderStates());
	    }

	    _renderTexture.display();
	}


	void Carousel::setStyle(Style style)
	{
	    _currentStyle = style;
	    loadShaderForStyle(style);
	}
	void Carousel::setShader(sf::Shader* shader, std::function<void(sf::Shader&)> uniformSetter)
	{
	    _ownedShader.reset();  // Release owned shader
	    _shader = shader;  // Point to user's shader (now works!)
	    _shaderUniformSetter = uniformSetter;
	    _currentStyle = Style::CUSTOM;
	}

	void Carousel::draw(sf::RenderTarget &target, sf::RenderStates states) const
	{
	    if (_currentStyle == Style::DEFAULT || !_shader)
	    {
	        target.draw(_sprite, sf::RenderStates::Default);
	        return;
	    }

	    sf::RenderStates shaderStates;
	    shaderStates.shader = _shader;
	    _shader->setUniform("texture", sf::Shader::CurrentTexture);

	    if (_currentStyle == Style::CUSTOM)
	    {
	        if (_shaderUniformSetter)
	        {
	            _shaderUniformSetter(*_shader);
	        }
	    }
	    else
	    {
	        // Update dynamic parameters
	        auto params = _shaderParams;
	        params.textureSize = sf::Vector2f(_renderTexture.getSize());
	        params.totalWidth = _sprite.getGlobalBounds().size.x;
	        params.time = _shaderTime;

	        // Apply uniforms
	        ShaderLibrary::applyUniforms(*_shader, _currentStyle, params);

	        // Update time for animated shaders
	        if (_currentStyle == Style::WAVE_DISTORTION || _currentStyle == Style::ZOOM_PULSE)
	        {
	            const_cast<Carousel*>(this)->_shaderTime += 0.05f;
	        }
	    }

	    target.draw(_sprite, shaderStates);
	}


	void Carousel::loadShaderForStyle(Style style)
	{
	    // Reset shader pointer first
	    _shader = nullptr;

	    // Handle styles that don't need shaders
	    if (style == Style::CUSTOM || style == Style::DEFAULT)
	    {
	        _ownedShader.reset();
	        return;
	    }

	    // Check if shaders are supported
	    if (!sf::Shader::isAvailable())
	    {
	        std::cerr << "Shaders not available on this system!" << std::endl;
	        _ownedShader.reset();
	        return;
	    }

	    // Create a new shader that we own
	    _ownedShader = std::make_unique<sf::Shader>();
	    std::string_view shaderCode;

	    // Get the appropriate shader code from ShaderLibrary
	    switch (style)
	    {
	        case Style::FADE_EDGES:
	            shaderCode = ShaderLibrary::fadeEdges();
	            break;

	        case Style::SCALE_CENTER:
	            shaderCode = ShaderLibrary::scaleCenter();
	            break;

	        case Style::BLUR_UNFOCUSED:
	            shaderCode = ShaderLibrary::blurUnfocused();
	            break;

	        case Style::COVERFLOW_3D:
	            shaderCode = ShaderLibrary::coverflow3D();
	            break;

	        case Style::COLOR_TINT:
	            shaderCode = ShaderLibrary::colorTint();
	            break;

	        case Style::WAVE_DISTORTION:
	            shaderCode = ShaderLibrary::waveDistortion();
	            break;

	        case Style::DEPTH_OF_FIELD:
	            shaderCode = ShaderLibrary::depthOfField();
	            break;

	        case Style::VIGNETTE:
	            shaderCode = ShaderLibrary::vignette();
	            break;

	        case Style::GRAYSCALE_EDGES:
	            shaderCode = ShaderLibrary::grayscaleEdges();
	            break;

	        case Style::REFLECTION:
	            shaderCode = ShaderLibrary::reflection();
	            break;

	        case Style::ROTATION_3D:
	            shaderCode = ShaderLibrary::rotation3D();
	            break;

	        case Style::PARALLAX:
	            shaderCode = ShaderLibrary::parallax();
	            break;

	        case Style::GLOW_CENTER:
	            shaderCode = ShaderLibrary::glowCenter();
	            break;

	        case Style::SLIDE_ANGLE:
	            shaderCode = ShaderLibrary::slideAngle();
	            break;

	        case Style::SEPIA_TONE:
	            shaderCode = ShaderLibrary::sepiaTone();
	            break;

	        case Style::ZOOM_PULSE:
	            shaderCode = ShaderLibrary::zoomPulse();
	            break;

	        default:
	            std::cerr << "Unknown shader style: " << static_cast<int>(style) << std::endl;
	            _ownedShader.reset();
	            return;
	    }

	    // Try to load the shader
	    if (_ownedShader->loadFromMemory(std::string(shaderCode), sf::Shader::Type::Fragment))
	    {
	        // Success - point to the owned shader
	        _shader = _ownedShader.get();
	    }
	    else
	    {
	        // Failed to compile
	        std::cerr << "ERROR: Failed to compile shader for style " << static_cast<int>(style) << std::endl;
	        _ownedShader.reset();
	        _shader = nullptr;
	    }
	}

}



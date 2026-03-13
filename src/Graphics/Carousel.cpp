//
// Carousel.cpp
//

#include <Malena/Graphics/Carousel.h>

namespace ml
{
    Carousel::Carousel() :
        _renderTexture({720, 480}),
        _sprite(_renderTexture.getTexture()),
        _leftLabel(FontManager<>::getDefault()),
        _rightLabel(FontManager<>::getDefault())
    {
        setState(State::SCROLL_IDLE);
        _sprite.setPosition(_position);
        _sprite.setColor(sf::Color::White);

        initArrows();
        enableFlag(Flag::SHOW_ARROWS);

        onUpdate([this]()
        {
            if (checkFlag(Flag::SCROLLING))
            {
                updateView();
                wrapComponents();
            }
            drawTexture();
        });

        // Arrow clicks — use raw subscribe so it fires even when mouse is outside
        // the carousel content area (e.g. SIDES placement)
        subscribe("click", [this](const std::optional<sf::Event>& event)
        {
            if (!event || !checkFlag(Flag::SHOW_ARROWS)) return;
            const auto* pressed = event->getIf<sf::Event::MouseButtonPressed>();
            if (!pressed || pressed->button != sf::Mouse::Button::Left) return;

            sf::Vector2f screenPos = {
                static_cast<float>(pressed->position.x),
                static_cast<float>(pressed->position.y)
            };

            if (isOnLeftArrow(screenPos))  { previous(); return; }
            if (isOnRightArrow(screenPos)) { next();     return; }
        });

        // Click — forward to children
        onClick([this](const std::optional<sf::Event>& event)
        {
            if (!event) return;
            const auto* pressed = event->getIf<sf::Event::MouseButtonPressed>();
            if (!pressed) return;

            sf::Vector2f screenPos = {
                static_cast<float>(pressed->position.x),
                static_cast<float>(pressed->position.y)
            };

            float localX = screenPos.x - _position.x + _viewX;
            float localY = screenPos.y - _position.y;

            for (auto* child : _components)
            {
                if (child->getGlobalBounds().contains({localX, localY}))
                {
                    auto* sub = dynamic_cast<Subscribable*>(child);
                    if (sub) EventsManager::fire("click",
                        [sub](Subscribable& ep) { return &ep == sub; },
                        nullptr, event);
                }
            }
        });

        // Hover — forward to children in carousel-local space
        onHover([this](const std::optional<sf::Event>& event)
        {
            if (!event) return;
            const auto* moved = event->getIf<sf::Event::MouseMoved>();
            if (!moved) return;

            float localX = static_cast<float>(moved->position.x) - _position.x + _viewX;
            float localY = static_cast<float>(moved->position.y) - _position.y;

            for (auto* child : _components)
            {
                auto* sub = dynamic_cast<Subscribable*>(child);
                if (!sub) continue;

                if (child->getGlobalBounds().contains({localX, localY}))
                    EventsManager::fire("hover",
                        [sub](Subscribable& ep) { return &ep == sub; },
                        nullptr, event);
                else
                    EventsManager::fire("unhover",
                        [sub](Subscribable& ep) { return &ep == sub; },
                        nullptr, event);
            }
        });
    }

    // ── Arrow Setup ───────────────────────────────────────────────────────────

    void Carousel::initArrows()
    {
        // Background rectangles
        _leftArrow.setSize({_arrowSize, _arrowSize});
        _rightArrow.setSize({_arrowSize, _arrowSize});
        _leftArrow.setFillColor(_arrowColor);
        _rightArrow.setFillColor(_arrowColor);

        // Use simple ASCII chevrons — guaranteed in any font
        _leftLabel.setString("<");
        _rightLabel.setString(">");
        _leftLabel.setCharacterSize(static_cast<unsigned int>(_arrowSize * 0.6f));
        _rightLabel.setCharacterSize(static_cast<unsigned int>(_arrowSize * 0.6f));
        _leftLabel.setFillColor(sf::Color::White);
        _rightLabel.setFillColor(sf::Color::White);

        // Subscribe arrows directly — they handle their own clicks regardless
        // of whether the mouse is inside the carousel's content bounds
        _leftArrow.onClick([this]()  { previous(); });
        _rightArrow.onClick([this]() { next();      });

        _leftArrow.onHover([this]()   { _leftArrow.setFillColor(_arrowHoverColor);  });
        _leftArrow.onUnhover([this]() { _leftArrow.setFillColor(_arrowColor);       });
        _rightArrow.onHover([this]()  { _rightArrow.setFillColor(_arrowHoverColor); });
        _rightArrow.onUnhover([this](){ _rightArrow.setFillColor(_arrowColor);      });
    }

    void Carousel::updateArrowPositions()
    {
        float contentWidth  = _sprite.getGlobalBounds().size.x;
        float contentHeight = _sprite.getGlobalBounds().size.y;
        float halfArrow     = _arrowSize / 2.f;

        sf::Vector2f leftPos, rightPos;

        switch (_arrowPlacement)
        {
            case ArrowPlacement::SIDES:
                leftPos  = { _position.x - _arrowSize - 8.f,
                              _position.y + contentHeight / 2.f - halfArrow };
                rightPos = { _position.x + contentWidth + 8.f,
                              _position.y + contentHeight / 2.f - halfArrow };
                break;

            case ArrowPlacement::TOP:
                leftPos  = { _position.x,
                              _position.y - _arrowSize - 8.f };
                rightPos = { _position.x + contentWidth - _arrowSize,
                              _position.y - _arrowSize - 8.f };
                break;

            case ArrowPlacement::OVERLAY:
                leftPos  = { _position.x + 8.f,
                              _position.y + contentHeight / 2.f - halfArrow };
                rightPos = { _position.x + contentWidth - _arrowSize - 8.f,
                              _position.y + contentHeight / 2.f - halfArrow };
                break;
        }

        // Apply manual overrides if set
        if (_leftArrowManualPos)  leftPos  = *_leftArrowManualPos;
        if (_rightArrowManualPos) rightPos = *_rightArrowManualPos;

        _leftArrow.setPosition(leftPos);
        _rightArrow.setPosition(rightPos);

        // Center chevron labels within arrows
        auto centerLabel = [](sf::Text& label, sf::Vector2f arrowPos, float size)
        {
            sf::FloatRect lb = label.getLocalBounds();
            label.setPosition({
                arrowPos.x + (size - lb.size.x) / 2.f - lb.position.x,
                arrowPos.y + (size - lb.size.y) / 2.f - lb.position.y
            });
        };

        centerLabel(_leftLabel,  leftPos,  _arrowSize);
        centerLabel(_rightLabel, rightPos, _arrowSize);
    }

    bool Carousel::isOnLeftArrow(sf::Vector2f screenPos) const
    {
        return _leftArrow.getGlobalBounds().contains(screenPos);
    }

    bool Carousel::isOnRightArrow(sf::Vector2f screenPos) const
    {
        return _rightArrow.getGlobalBounds().contains(screenPos);
    }

    // ── Public Arrow API ──────────────────────────────────────────────────────

    void Carousel::showArrows(bool show)
    {
        show ? enableFlag(Flag::SHOW_ARROWS) : disableFlag(Flag::SHOW_ARROWS);
    }

    void Carousel::setArrowPlacement(ArrowPlacement placement)
    {
        _arrowPlacement = placement;
        updateArrowPositions();
    }

    void Carousel::setArrowSize(float size)
    {
        _arrowSize = size;
        _leftArrow.setSize({size, size});
        _rightArrow.setSize({size, size});
        _leftLabel.setCharacterSize(static_cast<unsigned int>(size * 0.6f));
        _rightLabel.setCharacterSize(static_cast<unsigned int>(size * 0.6f));
        updateArrowPositions();
    }

    void Carousel::setArrowColor(sf::Color color)
    {
        _arrowColor = color;
        _leftArrow.setFillColor(color);
        _rightArrow.setFillColor(color);
    }

    void Carousel::setArrowHoverColor(sf::Color color)
    {
        _arrowHoverColor = color;
    }

    void Carousel::setArrowTexture(ArrowSide side, sf::Texture texture)
    {
        if (side == ArrowSide::LEFT)
        {
            _leftArrowTexture = std::move(texture);
            _leftArrow.setTexture(&*_leftArrowTexture);
        }
        else
        {
            _rightArrowTexture = std::move(texture);
            _rightArrow.setTexture(&*_rightArrowTexture);
        }
    }

    void Carousel::setArrowPosition(ArrowSide side, sf::Vector2f position)
    {
        if (side == ArrowSide::LEFT)  _leftArrowManualPos  = position;
        else                          _rightArrowManualPos = position;
        updateArrowPositions();
    }

    void Carousel::clearArrowPosition(ArrowSide side)
    {
        if (side == ArrowSide::LEFT)  _leftArrowManualPos.reset();
        else                          _rightArrowManualPos.reset();
        updateArrowPositions();
    }

    // ── Content API ───────────────────────────────────────────────────────────

    void Carousel::add(ml::Core& component)
    {
        if (_components.empty())
            component.setPosition({0.f, 0.f});
        else
        {
            auto* last = _components.back();
            float newX = last->getPosition().x + last->getGlobalBounds().size.x + _spacing;
            component.setPosition({newX, 0.f});
        }
        _components.push_back(&component);
        updateRenderTextureSize();
    }

    void Carousel::remove(ml::Core& component)
    {
        _components.remove(&component);
        updateRenderTextureSize();
    }

    // ── Positioning ───────────────────────────────────────────────────────────

    void Carousel::setPosition(const sf::Vector2f& position)
    {
        _position = position;
        _sprite.setPosition(position);
        updateArrowPositions();
    }

    sf::Vector2f  Carousel::getPosition()     const { return _position; }
    sf::FloatRect Carousel::getGlobalBounds() const { return _sprite.getGlobalBounds(); }
    sf::RenderStates Carousel::getRenderStates() const { return sf::RenderStates::Default; }

    // ── Navigation ────────────────────────────────────────────────────────────

    void Carousel::next()
    {
        if (checkFlag(Flag::SCROLLING) || _components.empty()) return;
        if ((int)_components.size() <= _previewCount) return;
        setState(State::SCROLL_RIGHT);
        _targetViewX += _components.front()->getGlobalBounds().size.x + _spacing;
        enableFlag(Flag::SCROLLING);
    }

    void Carousel::previous()
    {
        if (checkFlag(Flag::SCROLLING) || _components.empty()) return;
        if ((int)_components.size() <= _previewCount) return;
        setState(State::SCROLL_LEFT);
        _targetViewX -= _components.back()->getGlobalBounds().size.x + _spacing;
        enableFlag(Flag::SCROLLING);
    }

    // ── Settings ──────────────────────────────────────────────────────────────

    void  Carousel::setSpacing(float s)    { _spacing = s; }
    float Carousel::getSpacing()     const { return _spacing; }
    void  Carousel::setSpeed(float s)      { _speed = s; }
    float Carousel::getSpeed()       const { return _speed; }

    void Carousel::setPreviewCount(int count)
    {
        if (count < 1) count = 1;
        _previewCount = count;
        updateRenderTextureSize();
    }
    int Carousel::getPreviewCount() const { return _previewCount; }

    void Carousel::setStyle(Style style)
    {
        _currentStyle = style;
        loadShaderForStyle(style);
    }
    CarouselManifest::Style Carousel::getStyle() const { return _currentStyle; }

    void Carousel::setShader(sf::Shader* shader, std::function<void(sf::Shader&)> uniformSetter)
    {
        _ownedShader.reset();
        _shader = shader;
        _shaderUniformSetter = uniformSetter;
        _currentStyle = Style::CUSTOM;
    }

    void Carousel::setShaderParams(const ShaderLibrary::Params& params) { _shaderParams = params; }
    ShaderLibrary::Params& Carousel::getShaderParams() { return _shaderParams; }

    // ── Draw ──────────────────────────────────────────────────────────────────

    void Carousel::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        if (_currentStyle == Style::DEFAULT || !_shader)
            target.draw(_sprite, sf::RenderStates::Default);
        else
        {
            sf::RenderStates shaderStates;
            shaderStates.shader = _shader;
            _shader->setUniform("texture", sf::Shader::CurrentTexture);

            if (_currentStyle == Style::CUSTOM)
            {
                if (_shaderUniformSetter) _shaderUniformSetter(*_shader);
            }
            else
            {
                auto params = _shaderParams;
                params.textureSize = sf::Vector2f(_renderTexture.getSize());
                params.totalWidth  = _sprite.getGlobalBounds().size.x;
                params.time        = _shaderTime;
                ShaderLibrary::applyUniforms(*_shader, _currentStyle, params);

                if (_currentStyle == Style::WAVE_DISTORTION || _currentStyle == Style::ZOOM_PULSE)
                    const_cast<Carousel*>(this)->_shaderTime += 0.05f;
            }
            target.draw(_sprite, shaderStates);
        }

        // Draw arrow controls
        if (checkFlag(Flag::SHOW_ARROWS))
        {
            target.draw(_leftArrow,  states);
            target.draw(_rightArrow, states);
            // Only draw chevron label if no custom texture is set
            if (!_leftArrowTexture)  target.draw(_leftLabel,  states);
            if (!_rightArrowTexture) target.draw(_rightLabel, states);
        }
    }

    // ── Private Helpers ───────────────────────────────────────────────────────

    void Carousel::drawTexture()
    {
        _renderTexture.setView(_carouselView);
        _renderTexture.clear(sf::Color::Transparent);
        for (auto* component : _components)
            _renderTexture.draw(*dynamic_cast<sf::Drawable*>(component));
        _renderTexture.display();
    }

    void Carousel::updateRenderTextureSize()
    {
        if (_components.empty()) return;

        float visibleWidth = 0;
        auto  iter         = _components.begin();
        for (int i = 0; i < std::min(_previewCount, (int)_components.size()); i++)
        {
            visibleWidth += (*iter)->getGlobalBounds().size.x;
            if (i < _previewCount - 1) visibleWidth += _spacing;
            ++iter;
        }

        float height = _components.front()->getGlobalBounds().size.y;

        _renderTexture.resize({
            static_cast<unsigned int>(visibleWidth),
            static_cast<unsigned int>(height)
        });

        _carouselView.setSize({visibleWidth, height});
        _carouselView.setCenter({visibleWidth / 2.f, height / 2.f});
        _viewX = 0.f;
        _targetViewX = 0.f;

        _sprite.setTexture(_renderTexture.getTexture(), true);
        updateArrowPositions();
    }

    void Carousel::updateView()
    {
        float diff = _targetViewX - _viewX;
        if (std::abs(diff) < 0.1f)
        {
            _viewX = _targetViewX;
            disableFlag(Flag::SCROLLING);
            setState(State::SCROLL_IDLE);
        }
        else
        {
            float step = std::min(std::abs(diff), _speed);
            _viewX += (diff > 0) ? step : -step;
        }

        float w = _carouselView.getSize().x;
        float h = _carouselView.getSize().y;
        _carouselView.setCenter({_viewX + w / 2.f, h / 2.f});
    }

    void Carousel::wrapComponents()
    {
        if (_components.size() <= 1) return;

        float viewLeft  = _viewX;
        float viewRight = _viewX + _sprite.getTextureRect().size.x;

        auto* front = _components.front();
        if (front->getPosition().x + front->getGlobalBounds().size.x < viewLeft - _spacing)
        {
            auto* back = _components.back();
            front->setPosition({back->getPosition().x + back->getGlobalBounds().size.x + _spacing, 0.f});
            _components.push_back(front);
            _components.pop_front();
        }

        auto* back = _components.back();
        if (back->getPosition().x > viewRight + _spacing)
        {
            auto* newFront = _components.front();
            back->setPosition({newFront->getPosition().x - back->getGlobalBounds().size.x - _spacing, 0.f});
            _components.push_front(back);
            _components.pop_back();
        }
    }

    void Carousel::loadShaderForStyle(Style style)
    {
        _shader = nullptr;
        if (style == Style::CUSTOM || style == Style::DEFAULT) { _ownedShader.reset(); return; }
        if (!sf::Shader::isAvailable()) { _ownedShader.reset(); return; }

        _ownedShader = std::make_unique<sf::Shader>();
        std::string_view shaderCode;

        switch (style)
        {
            case Style::FADE_EDGES:       shaderCode = ShaderLibrary::fadeEdges();       break;
            case Style::SCALE_CENTER:     shaderCode = ShaderLibrary::scaleCenter();     break;
            case Style::BLUR_UNFOCUSED:   shaderCode = ShaderLibrary::blurUnfocused();   break;
            case Style::COVERFLOW_3D:     shaderCode = ShaderLibrary::coverflow3D();     break;
            case Style::COLOR_TINT:       shaderCode = ShaderLibrary::colorTint();       break;
            case Style::WAVE_DISTORTION:  shaderCode = ShaderLibrary::waveDistortion();  break;
            case Style::DEPTH_OF_FIELD:   shaderCode = ShaderLibrary::depthOfField();    break;
            case Style::VIGNETTE:         shaderCode = ShaderLibrary::vignette();        break;
            case Style::GRAYSCALE_EDGES:  shaderCode = ShaderLibrary::grayscaleEdges();  break;
            case Style::REFLECTION:       shaderCode = ShaderLibrary::reflection();      break;
            case Style::ROTATION_3D:      shaderCode = ShaderLibrary::rotation3D();      break;
            case Style::PARALLAX:         shaderCode = ShaderLibrary::parallax();        break;
            case Style::GLOW_CENTER:      shaderCode = ShaderLibrary::glowCenter();      break;
            case Style::SLIDE_ANGLE:      shaderCode = ShaderLibrary::slideAngle();      break;
            case Style::SEPIA_TONE:       shaderCode = ShaderLibrary::sepiaTone();       break;
            case Style::ZOOM_PULSE:       shaderCode = ShaderLibrary::zoomPulse();       break;
            default:
                std::cerr << "Unknown shader style: " << static_cast<int>(style) << std::endl;
                _ownedShader.reset();
                return;
        }

        if (_ownedShader->loadFromMemory(std::string(shaderCode), sf::Shader::Type::Fragment))
            _shader = _ownedShader.get();
        else
        {
            std::cerr << "ERROR: Failed to compile shader for style " << static_cast<int>(style) << std::endl;
            _ownedShader.reset();
        }
    }

} // namespace ml
//
// Created by Dave R. Smith on 3/5/25.
//

#include <Malena/Engine/App/AppManager.h>

#include <Malena/Engine/Events/Fireable.h>
#include <Malena/Engine/Events/EventManager.h>
#include <Malena/Engine/Networking/NetworkManager.h>

namespace ml
{
    AppManager::AppManager(const sf::VideoMode& videoMode,
                           const std::string& title,
                           sf::RenderWindow& window,
                           Architecture architecture,
                           std::uint32_t windowStyle)
        : window(&window)
        , _architecture(architecture)
        , _videoMode(videoMode)
        , _title(title)
        , _windowStyle(windowStyle)
    {
        window.create(videoMode, title, windowStyle);
        window.setFramerateLimit(_framerateLimit);
        _instance = this;
    }

    // ── Window appearance ─────────────────────────────────────────────────────

    void AppManager::setBackgroundColor(sf::Color color)
    {
        _clearColor = color;
    }

    void AppManager::setTitle(const std::string& title)
    {
        _title = title;
        window->setTitle(title);
    }

    void AppManager::setIcon(const sf::Image& icon)
    {
        window->setIcon(icon);
    }

    void AppManager::setWindowPosition(int x, int y)
    {
        window->setPosition({ x, y });
    }

    void AppManager::setWindowStyle(std::uint32_t style)
    {
        _windowStyle = style;
        window->create(_videoMode, _title, style);
        window->setFramerateLimit(_framerateLimit);
    }

    // ── Timing ────────────────────────────────────────────────────────────────

    void AppManager::setFramerateLimit(unsigned int limit)
    {
        _framerateLimit = limit;
        window->setFramerateLimit(limit);
    }

    void AppManager::setVSync(bool enabled)
    {
        window->setVerticalSyncEnabled(enabled);
    }

    // ── Pause / resume ────────────────────────────────────────────────────────

    void AppManager::pause()  { _paused = true;  }
    void AppManager::resume() { _paused = false; }

    // ── Lifecycle hooks ───────────────────────────────────────────────────────

    void AppManager::onPreRender(std::function<void()> hook)
    {
        _preRenderHook = std::move(hook);
    }

    void AppManager::onPostRender(std::function<void()> hook)
    {
        _postRenderHook = std::move(hook);
    }

    void AppManager::onClose(std::function<bool()> handler)
    {
        _closeHandler = std::move(handler);
    }

    void AppManager::onResize(std::function<void(unsigned int, unsigned int)> handler)
    {
        _resizeHandler = std::move(handler);
    }

    // ── Core loop helpers ─────────────────────────────────────────────────────

    void AppManager::flushDeferredUnloads()
    {
        if (_deferredUnloads.empty()) return;

        // Swap-and-clear so that ops added during flushing are deferred to next frame
        std::vector<std::function<void()>> pending;
        pending.swap(_deferredUnloads);
        for (auto& op : pending)
            op();
    }

    void AppManager::draw()
    {
        _isDrawing = true;
        window->clear(_clearColor);

        if (_preRenderHook)
            _preRenderHook();

        for (auto& c : getComponents())
        {
            if (!c->checkFlag(Flag::HIDDEN))
            {
                auto* drawable = dynamic_cast<sf::Drawable*>(c);
                if (drawable)
                    window->draw(*drawable, c->getRenderStates());
            }
        }

        if (_postRenderHook)
            _postRenderHook();

        window->display();
        _isDrawing = false;
    }

    void AppManager::fireUpdateEvents()
    {
        for (auto* dispatcher : Fireable::_fireables_frame)
        {
            if (dispatcher->occurred())
                dispatcher->fire();
        }
    }

    void AppManager::run()
    {
        onInit();
        onReady();

        _clock.restart();

        while (window->isOpen())
        {
            _deltaTime = _clock.restart().asSeconds();

            while (const std::optional event = window->pollEvent())
            {
                if (event->is<sf::Event::Closed>())
                {
                    bool shouldClose = _closeHandler ? _closeHandler() : true;
                    if (shouldClose)
                    {
                        window->close();
                        break;
                    }
                    continue;
                }

                if (event->is<sf::Event::Resized>())
                {
                    if (_resizeHandler)
                    {
                        const auto* resized = event->getIf<sf::Event::Resized>();
                        if (resized)
                            _resizeHandler(resized->size.x, resized->size.y);
                    }
                }

                fireInputEvents(event);
            }

            if (!_paused)
                fireUpdateEvents();

            NetworkManager::flush();

            draw();
            flushDeferredUnloads();
        }

        // Join all background networking threads before static destructors run.
        // Without this, ~vector<thread> on gWorkers calls std::terminate().
        NetworkManager::shutdown();
    }

    void AppManager::fireInputEvents(const std::optional<sf::Event>& event)
    {
        for (auto* dispatcher : Fireable::_fireables)
        {
            if (dispatcher->occurred(event))
                dispatcher->fire(event);
        }
    }

} // namespace ml

//
// Created by Dave R. Smith on 3/5/25.
//

#include <Malena/Engine/App/AppManager.h>

#include <Malena/Engine/Events/Fireable.h>
#include <Malena/Engine/Events/EventManager.h>
#include <Malena/Engine/Networking/NetworkManager.h>
#include <Malena/Animation/AnimationManager.h>
#include <SFML/Graphics/View.hpp>
#include <algorithm>

#ifdef __APPLE__
#import <AppKit/AppKit.h>
#import <objc/runtime.h>

static BOOL ml_acceptsFirstMouse(id, SEL, NSEvent*) { return YES; }

static void ml_patchAcceptsFirstMouse(NSWindow* nsWin)
{
    NSView* cv = [nsWin contentView];
    if (!cv) return;
    Class cls = [cv class];
    class_replaceMethod(cls,
                        @selector(acceptsFirstMouse:),
                        (IMP)ml_acceptsFirstMouse,
                        "c@:@");
}
#endif

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
#ifdef __APPLE__
        // VSync synchronizes frame submission with the display refresh, preventing
        // Metal command buffer accumulation when multiple GL apps run simultaneously.
        // setFramerateLimit() uses sf::sleep() which is imprecise and lets frames
        // pile up, exhausting Metal's transient memory pool (MTLCommandBufferError Code=8).
        window.setVerticalSyncEnabled(true);
#else
        window.setFramerateLimit(_framerateLimit);
#endif
        _instance = this;
#ifdef __APPLE__
        ml_patchAcceptsFirstMouse((NSWindow*)window.getNativeHandle());
#endif
    }

    // ── Exclusive interaction ─────────────────────────────────────────────────

    void AppManager::setExclusiveOwner(Core* owner)  { _exclusiveOwner = owner; }
    void AppManager::clearExclusiveOwner()            { _exclusiveOwner = nullptr; }

    bool AppManager::isUnderExclusiveOwner(Core* component)
    {
        if (!_exclusiveOwner) return true;
        return component == _exclusiveOwner
            || Core::isDescendantOf(_exclusiveOwner, component);
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
#ifdef __APPLE__
        window->setVerticalSyncEnabled(true);
#else
        window->setFramerateLimit(_framerateLimit);
#endif
#ifdef __APPLE__
        ml_patchAcceptsFirstMouse((NSWindow*)window->getNativeHandle());
#endif
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

        // Top popup layer: drawn after the whole component tree so a transient
        // popup (e.g. an open Select dropdown) escapes its container's stacking
        // order and is never occluded by later-drawn siblings or host overlays.
        if (_activePopup && !_activePopup->checkFlag(Flag::HIDDEN))
            if (auto* popup = dynamic_cast<sf::Drawable*>(_activePopup))
                window->draw(*popup, _activePopup->getRenderStates());

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

#ifdef __APPLE__
        [[NSApplication sharedApplication] activateIgnoringOtherApps:YES];
#endif

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
                    const auto* resized = event->getIf<sf::Event::Resized>();
                    if (resized)
                    {
                        // Keep a 1:1 view so coordinates equal window pixels (crisp,
                        // no scaling/letterbox); apps reflow their layout in onResize.
                        window->setView(sf::View(sf::FloatRect(
                            {0.f, 0.f},
                            {static_cast<float>(resized->size.x),
                             static_cast<float>(resized->size.y)})));

                        // Broadcast so responsive components reflow to the new size.
                        for (auto* c : getComponents())
                            if (c) c->onWindowResize(resized->size.x, resized->size.y);

                        if (_resizeHandler) _resizeHandler(resized->size.x, resized->size.y);
                    }
                }

                fireInputEvents(event);
            }

            if (!_paused)
            {
                fireUpdateEvents();
                AnimationManager::advance(_deltaTime);   // delta-time animation tick
            }

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

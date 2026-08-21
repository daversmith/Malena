
#include <Malena/Engine/App/Application.h>

#include "Malena/Engine/Messaging/MessageManager.h"
#include "Malena/Resources/ThemeManager.h"

#ifdef __APPLE__
#import <AppKit/AppKit.h>
// Ensure NSApp is SFApplication before any sf::GlResource is constructed.
//
// On macOS 14+, NSOpenGLContext creation fails silently when NSApp is
// uninitialized, leaving SFML's SharedContext with a nil m_context. That nil
// causes a false-positive in makeCurrent(true), poisoning currentContext.id and
// later making gladLoadGL run with no real GL context — all GLAD pointers stay
// null and the first GL call crashes at PC=0.
//
// SFML's own fix (setUpProcess) runs too late — after WindowManager::getWindow()
// already triggered SharedContext creation as a default argument. We must beat it.
//
// SFApplication is registered with the ObjC runtime during image loading, which
// happens before any static initializer. NSClassFromString finds it here safely.
// Calling [SFApplication sharedApplication] installs SFML's subclass as NSApp,
// which is exactly what setUpProcess() would do — so setUpProcess() becomes a
// no-op when it runs later during window.create().
namespace {
    struct NSAppEnsurer {
        NSAppEnsurer() {
            Class cls = NSClassFromString(@"SFApplication");
            if (!cls) cls = [NSApplication class];
            [cls sharedApplication];
        }
    };
    [[maybe_unused]] static NSAppEnsurer _ensureNSApp;
}
#endif

namespace ml
{
    ApplicationBase::ApplicationBase(const sf::VideoMode& videoMode,
                                     const std::string& title,
                                     sf::RenderWindow& window,
                                     AppManager::Architecture architecture,
                                     std::uint32_t windowStyle)
        : AppManager(videoMode, title, window, architecture, windowStyle)
    {
    }

    ApplicationBase::ApplicationBase(unsigned int screenWidth,
                                     unsigned int screenHeight,
                                     unsigned int bitDepth,
                                     const std::string& title,
                                     std::uint32_t windowStyle)
        : ApplicationBase(sf::VideoMode({screenWidth, screenHeight}, bitDepth),
                          title,
                          WindowManager::getWindow(),
                          AppManager::MVC,
                          windowStyle)
    {
    }

    void ApplicationBase::addComponent(Core& component)
    {
        CoreManager<Core>::addComponent(component);
    }

    void ApplicationBase::clearEvents()
    {
        EventManager::clear();
    }

    void ApplicationBase::reset()
    {
        EventManager::clear();
        MessageManager::clear();
        CoreManager<Core>::clear();
    }

    ApplicationBase::~ApplicationBase()
    {
        ml::ThemeManager::shutdown();
    }

} // namespace ml

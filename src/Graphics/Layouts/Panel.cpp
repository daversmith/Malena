//
// Created by Dave Smith on 4/1/26.
//

#include <Malena/Graphics/Layouts/Panel.h>
#include <Malena/Manifests/Theme.h>

namespace ml {

    Panel::Panel()
    {
        // Seed the fill color from whatever theme is active right now.
        // onThemeApplied keeps it in sync for all subsequent theme changes.
        setFillColor(ThemeManager::get().surface);

        // Panel is a pure drawing/layout container — it must not participate in
        // the focus system. ComponentCore() registers empty onClick/onHover/onDrag
        // handlers for every Core-derived type, and ClickableDispatcher processes
        // ALL registered components. Because Panel's bounds cover the full pane,
        // it would fire AFTER any child widgets in the subscriber list and steal
        // focus away from them. Removing all subscriptions here prevents that.
        unsubscribeAll();
    }

    void Panel::onThemeApplied(const Theme& theme)
    {
        if (isThemeLocked()) return;
        setFillColor(theme.surface);
    }

    void Panel::addRef(Core& child)
    {
        // No position tracking — caller manages the child's position entirely.
        // Just register it with CoreManager so it appears in the draw pass.
        CoreManager<Core>::addComponent(child);
    }

    bool Panel::removeComponent(Core& child)
    {
        _relativePositions.erase(&child);
        _fillChildren.erase(&child);
        return CoreManager<Core>::removeComponent(child);
    }

    void Panel::setSize(const sf::Vector2f& size)
    {
        RectangleWith<PanelManifest>::setSize(size);
        for (auto& [child, resizeFn] : _fillChildren)
            if (resizeFn) resizeFn(size);
    }

    void Panel::setPosition(const sf::Vector2f& newPos)
    {
        RectangleWith<PanelManifest>::setPosition(newPos);
        for (auto* c : getComponents())
        {
            auto it = _relativePositions.find(c);
            if (it != _relativePositions.end())
                c->setPosition(newPos + it->second);
        }
    }

    void Panel::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        RectangleWith<PanelManifest>::draw(target, states);
        for (auto* component : getComponents())
        {
            auto* drawable = dynamic_cast<sf::Drawable*>(component);
            if (drawable)
                target.draw(*drawable, states);
        }
    }

} // ml

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

    void Panel::addUntracked(Core& child)
    {
        CoreManager<Core>::addComponent(child);
        Core::linkChild(this, &child);
        child.setParentEnabled(isEnabled());
    }

    void Panel::addRef(Core& child) { addUntracked(child); }

    bool Panel::removeComponent(Core& child)
    {
        _relativePositions.erase(&child);
        _fillChildren.erase(&child);
        return CoreManager<Core>::removeComponent(child);
    }

    void Panel::clear()
    {
        for (auto* c : getComponents())
            c->setParentEnabled(false);
        _relativePositions.clear();
        _fillChildren.clear();
        CoreManager<Core>::clear();
    }

    void Panel::setSize(const sf::Vector2f& size)
    {
        RectangleWith<PanelManifest>::setSize(size);
        for (auto& [child, resizeFn] : _fillChildren)
            if (resizeFn) resizeFn(size);
    }

    void Panel::setPosition(const sf::Vector2f& newPos)
    {
        const sf::Vector2f delta = newPos - getPosition();
        RectangleWith<PanelManifest>::setPosition(newPos);
        for (auto* c : getComponents())
        {
            if (_relativePositions.count(c))
                c->setPosition(c->getPosition() + delta);
        }
    }

    void Panel::setEnabled(bool enabled)
    {
        Core::setEnabled(enabled);
        bool effective = isEnabled();
        for (auto* c : getComponents())
            c->setParentEnabled(effective);
    }

    void Panel::setParentEnabled(bool enabled)
    {
        Core::setParentEnabled(enabled);
        bool effective = isEnabled();
        for (auto* c : getComponents())
            c->setParentEnabled(effective);
    }

    void Panel::setVisible(bool visible)
    {
        Core::setVisible(visible);
        for (auto* c : getComponents())
            c->setVisible(visible);
    }

    void Panel::setActive(bool active)
    {
        Core::setActive(active);
        for (auto* c : getComponents())
            c->setActive(active);
    }

    void Panel::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        RectangleWith<PanelManifest>::draw(target, states);
        for (auto* component : getComponents())
        {
            if (component->checkFlag(ml::Flag::HIDDEN)) continue;
            auto* drawable = dynamic_cast<sf::Drawable*>(component);
            if (drawable)
                target.draw(*drawable, states);
        }
    }

} // ml

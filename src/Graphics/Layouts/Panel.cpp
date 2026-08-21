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
        Core::addComponent(child);
    }

    void Panel::addRef(Core& child) { addUntracked(child); }

    bool Panel::removeComponent(Core& child)
    {
        _relativePositions.erase(&child);
        _fillChildren.erase(&child);
        Core::removeComponent(child);
        return true;
    }

    void Panel::clear()
    {
        // Snapshot — Core::removeComponent mutates _children, which is what
        // getChildren() returns.
        const auto kids = getChildren();
        for (const auto& e : kids) e.component->setParentEnabled(false);
        _relativePositions.clear();
        _fillChildren.clear();
        for (const auto& e : kids) Core::removeComponent(*e.component);
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
        for (const auto& e : getChildren())
        {
            if (_relativePositions.count(e.component))
                e.component->setPosition(e.component->getPosition() + delta);
        }
    }

    // Enable cascade is handled by Core::setEnabled / Core::setParentEnabled
    // — Panel no longer needs to override them. Visibility, however, does NOT
    // cascade automatically (Core::setVisible just toggles its own HIDDEN
    // flag), so Panel keeps explicit overrides for setVisible / setActive to
    // preserve the existing "hide the panel, hide its children" semantics.

    void Panel::setVisible(bool visible)
    {
        Core::setVisible(visible);
        for (const auto& e : getChildren())
            if (!e.component->isVisibilityIndependent())   // opted-out children manage their own visibility
                e.component->setVisible(visible);
    }

    void Panel::setActive(bool active)
    {
        Core::setActive(active);
        for (const auto& e : getChildren())
            if (!e.component->isVisibilityIndependent())
                e.component->setActive(active);
    }

    void Panel::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        RectangleWith<PanelManifest>::draw(target, states);   // background
        drawChildren(target, states);                          // children loop
    }

} // ml

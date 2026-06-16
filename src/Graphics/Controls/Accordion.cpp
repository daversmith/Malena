
#include <Malena/Graphics/Controls/Accordion.h>
#include <Malena/Engine/Window/WindowManager.h>
#include <SFML/Window/Mouse.hpp>

namespace ml
{
    Accordion::Accordion(const sf::Font& font_)
    {
        AccordionTheme::applyFrom(ThemeManager::get());
        this->font = &font_;

        onClick([this]{
            const sf::Vector2f wp = WindowManager::getWindow().mapPixelToCoords(
                sf::Mouse::getPosition(WindowManager::getWindow()));
            const int hit = hitTestHeader(wp);
            if (hit >= 0 && !_sections[static_cast<std::size_t>(hit)].header->isEndHit(wp))
                toggle(static_cast<std::size_t>(hit));
        });
    }

    void Accordion::onThemeApplied(const Theme& theme)
    {
        if (isThemeLocked()) return;
        AccordionTheme::applyFrom(theme);
    }

    // ── Layout ────────────────────────────────────────────────────────────────

    void Accordion::layout()
    {
        float y = _position.y;
        for (auto& s : _sections)
        {
            s.header->setWidth(_width);
            s.header->setPosition({_position.x, y});
            y += headerHeight;

            // Drive the HIDDEN flag from expanded-state so collapsed content is
            // excluded from BOTH drawing and hit-testing (otherwise a collapsed
            // section's controls stay click-testable at their stale positions).
            s.content->setVisible(s.expanded);

            if (s.expanded)
            {
                s.content->setWidth(_width);
                s.content->setPosition({_position.x, y});
                y += s.content->getTotalHeight();
            }
        }
    }

    int Accordion::hitTestHeader(const sf::Vector2f& wp) const
    {
        float y = _position.y;
        for (int i = 0; i < static_cast<int>(_sections.size()); ++i)
        {
            if (sf::FloatRect{{_position.x, y}, {_width, headerHeight}}.contains(wp))
                return i;
            y += headerHeight;
            if (_sections[i].expanded)
                y += _sections[i].content->getTotalHeight();
        }
        return -1;
    }

    // ── Draw ──────────────────────────────────────────────────────────────────

    void Accordion::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        float y = _position.y;

        for (const auto& s : _sections)
        {
            sf::RectangleShape divider({_width, 1.f});
            divider.setPosition({_position.x, y});
            divider.setFillColor(dividerColor);
            target.draw(divider, states);

            target.draw(*s.header, states);
            y += headerHeight;

            if (s.expanded)
            {
                sf::RectangleShape contentRect({_width, s.content->getTotalHeight()});
                contentRect.setPosition({_position.x, y});
                contentRect.setFillColor(contentBg);
                target.draw(contentRect, states);

                target.draw(*s.content, states);
                y += s.content->getTotalHeight();
            }
        }

        sf::RectangleShape bottomDivider({_width, 1.f});
        bottomDivider.setPosition({_position.x, y});
        bottomDivider.setFillColor(dividerColor);
        target.draw(bottomDivider, states);
    }

    // ── Section management ────────────────────────────────────────────────────

    Accordion::SectionRef Accordion::addSection(const std::string& label, float contentHeight)
    {
        auto header  = std::make_unique<ListItem>(*font);
        auto content = std::make_unique<List>(*font);

        header->setLabel(label);

        ListItem& headerRef  = *header;
        List&     contentRef = *content;

        _sections.push_back({ std::move(header), std::move(content), contentHeight, false });

        // Register with Core so isDescendantOf / enable cascade see the section
        // children. addComponent also syncs their _parentEnabled to ours.
        addComponent(headerRef);
        addComponent(contentRef);

        layout();

        return { headerRef, contentRef };
    }

    void Accordion::expand(std::size_t index)
    {
        if (index >= _sections.size()) return;
        if (exclusive)
            for (auto& s : _sections) s.expanded = false;
        _sections[index].expanded = true;
        layout();
        if (_onSectionChanged) _onSectionChanged(index, true);
    }

    void Accordion::collapse(std::size_t index)
    {
        if (index >= _sections.size()) return;
        _sections[index].expanded = false;
        layout();
        if (_onSectionChanged) _onSectionChanged(index, false);
    }

    void Accordion::toggle(std::size_t index)
    {
        if (index >= _sections.size()) return;
        _sections[index].expanded ? collapse(index) : expand(index);
    }

    bool Accordion::isExpanded(std::size_t index) const
    {
        if (index >= _sections.size()) return false;
        return _sections[index].expanded;
    }

    void Accordion::removeSection(std::size_t index)
    {
        if (index >= _sections.size()) return;
        auto& s = _sections[index];
        if (s.header)  Core::removeComponent(*s.header);
        if (s.content) Core::removeComponent(*s.content);
        _sections.erase(_sections.begin() + static_cast<std::ptrdiff_t>(index));
        layout();
    }

    void Accordion::onSectionChanged(std::function<void(std::size_t, bool)> cb)
    {
        _onSectionChanged = std::move(cb);
    }

    // ── Sizing ────────────────────────────────────────────────────────────────

    void Accordion::setWidth(float width)
    {
        _width = width;
        layout();
    }

    float Accordion::getTotalHeight() const
    {
        float h = 0.f;
        for (const auto& s : _sections)
        {
            h += headerHeight;
            if (s.expanded) h += s.content->getTotalHeight();
        }
        return h;
    }

    // ── Positionable ──────────────────────────────────────────────────────────

    void Accordion::setPosition(const sf::Vector2f& position)
    {
        _position = position;
        layout();
    }

    sf::Vector2f Accordion::getPosition() const { return _position; }

    sf::FloatRect Accordion::getGlobalBounds() const
    {
        return { _position, { _width, getTotalHeight() } };
    }

} // namespace ml

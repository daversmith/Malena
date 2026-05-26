
#ifndef MALENA_ACCORDION_H
#define MALENA_ACCORDION_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Core/Component.h>
#include <Malena/Core/Core.h>
#include <Malena/Manifests/Manifest.h>
#include <Malena/Resources/FontManager.h>
#include <Malena/Traits/Settings/AccordionSettings.h>
#include <Malena/Traits/Theme/AccordionTheme.h>
#include <Malena/Traits/Theme/Themeable.h>
#include <Malena/Graphics/Controls/ListItem.h>
#include <Malena/Graphics/Controls/List.h>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace ml
{
    class MALENA_API AccordionManifest : public ml::Manifest
    {
    public:
        enum class Flag  {};
        enum class State {};
    };

    /**
     * @brief A vertically stacked set of collapsible sections.
     * @ingroup GraphicsControls
     *
     * Each section has a @c ListItem header and an owned @c List that is
     * shown or hidden when the header is clicked. By default only one
     * section can be open at a time (@c exclusive = true).
     *
     * The accordion creates and owns both the header and list — @c addSection
     * returns references to both so the caller can configure them freely.
     *
     * ### Usage
     * @code
     * ml::Accordion accordion;
     * accordion.setWidth(400.f);
     * accordion.setPosition({40.f, 100.f});
     *
     * auto [header, list] = accordion.addSection("Students", 180.f);
     * header.setDescription("3 enrolled");
     * list.addItem("Alice");
     * list.addItem("Bob");
     *
     * accordion.onSectionChanged([](std::size_t i, bool expanded){});
     * addComponent(accordion);
     * @endcode
     *
     * @see AccordionSettings, AccordionTheme, ListItem, List
     */
    class MALENA_API Accordion : public ComponentWith<AccordionManifest>,
                                 public AccordionSettings,
                                 public AccordionTheme,
                                 public Themeable
    {
    public:
        using Flag  = AccordionManifest::Flag;
        using State = AccordionManifest::State;

        struct SectionRef
        {
            ListItem& header;
            List&     list;
        };

    private:
        struct Section
        {
            std::unique_ptr<ListItem> header;
            std::unique_ptr<List>     content;
            float                     contentHeight = 120.f;
            bool                      expanded      = false;
        };

        std::vector<Section> _sections;

        sf::Vector2f _position = {0.f, 0.f};
        float        _width    = 300.f;

        std::function<void(std::size_t, bool)> _onSectionChanged;

        // ── Internal ──────────────────────────────────────────────────────────
        void layout();
        int  hitTestHeader(const sf::Vector2f& mousePos) const;

    protected:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        void onThemeApplied(const Theme& theme) override;

    public:
        explicit Accordion(const sf::Font& font = FontManager<>::getDefault());

        Accordion(const Accordion&)            = delete;
        Accordion& operator=(const Accordion&) = delete;

        // ── Apply ─────────────────────────────────────────────────────────────

        template<typename S>
        void applySettings(const S& s)
        {
            static_assert(std::is_base_of_v<AccordionSettings, S>,
                "applySettings() requires AccordionSettings");
            static_cast<AccordionSettings&>(*this) = s;
            layout();
        }

        template<typename T>
        void applyTheme(const T& t)
        {
            static_assert(std::is_base_of_v<AccordionTheme, T>,
                "applyTheme() requires AccordionTheme");
            static_cast<AccordionTheme&>(*this) = t;
        }

        template<typename St>
        void applyStyle(const St& s)
        {
            static_assert(std::is_base_of_v<AccordionSettings, St> &&
                          std::is_base_of_v<AccordionTheme, St>,
                "applyStyle() requires AccordionSettings and AccordionTheme");
            static_cast<AccordionSettings&>(*this) = s;
            static_cast<AccordionTheme&>(*this)    = s;
            layout();
        }

        // ── Section management ────────────────────────────────────────────────

        /**
         * @brief Add a section, creating its @c ListItem header and @c List content.
         *
         * @param label         Header label text.
         * @param contentHeight Height of the list area when the section is expanded.
         * @return References to the created header and list for further configuration.
         *
         * @code
         * auto [header, list] = accordion.addSection("Students", 180.f);
         * header.setDescription("3 enrolled");
         * list.addItem("Alice");
         * @endcode
         */
        SectionRef addSection(const std::string& label, float contentHeight = 120.f);

        /** @brief Remove a section by index. */
        void removeSection(std::size_t index);

        /** @brief Expand a section by index. Collapses others if exclusive. */
        void expand(std::size_t index);

        /** @brief Collapse a section by index. */
        void collapse(std::size_t index);

        /** @brief Toggle a section's expanded state. */
        void toggle(std::size_t index);

        /** @brief Return whether a section is expanded. */
        [[nodiscard]] bool isExpanded(std::size_t index) const;

        /** @brief Return the number of sections. */
        [[nodiscard]] std::size_t sectionCount() const { return _sections.size(); }

        /** @brief Fired when a section is expanded or collapsed. */
        void onSectionChanged(std::function<void(std::size_t index, bool expanded)> cb);

        // ── Sizing ────────────────────────────────────────────────────────────

        void  setWidth(float width);
        void  setSize(const sf::Vector2f& size) { setWidth(size.x); }
        [[nodiscard]] float getWidth()       const { return _width; }
        [[nodiscard]] float getTotalHeight() const;

        // ── Positionable ──────────────────────────────────────────────────────

        void          setPosition(const sf::Vector2f& position) override;
        sf::Vector2f  getPosition()     const override;
        sf::FloatRect getGlobalBounds() const override;
    };

    template<typename MANIFEST>
    class AccordionWith : public Accordion, public Customizable<MANIFEST>
    { public: using Accordion::Accordion; };

} // namespace ml

#endif // MALENA_ACCORDION_H

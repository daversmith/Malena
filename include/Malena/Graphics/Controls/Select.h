//
// Select.h
//

#ifndef SELECT_H
#define SELECT_H

#pragma once

#include <Malena/Core/Component.h>
#include <Malena/Core/Core.h>
#include <Malena/Manifests/Manifest.h>
#include <Malena/Resources/FontManager.h>
#include <Malena/Traits/Settings/SelectSettings.h>
#include <Malena/Traits/Theme/SelectTheme.h>
#include <Malena/Traits/Theme/Themeable.h>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <functional>
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <type_traits>

namespace ml
{
    class SelectManifest : public ml::Manifest
    {
    public:
        enum class Flag  { OPEN, DISABLED };
        enum class State { IDLE, HOVERED, OPEN, DISABLED };
    };

    struct SelectOptionStyle
    {
        std::optional<sf::Color>       color;
        std::optional<sf::Color>       bgColor;
        std::optional<const sf::Font*> font;
        std::optional<unsigned int>    charSize;
        std::optional<bool>            bold;
        std::optional<bool>            italic;
        std::string                    description;
        std::optional<sf::Color>       descColor;
        std::optional<unsigned int>    descCharSize;
        const sf::Texture*             iconTexture = nullptr;
        sf::IntRect                    iconRect;
        sf::Vector2f                   iconSize    = {0.f, 0.f};
    };

    struct SelectOption
    {
        std::string       label;
        std::string       value;
        SelectOptionStyle style;
        bool              enabled  = true;
        bool              selected = false;
        ml::Core*         customComponent = nullptr;

        explicit SelectOption(const std::string& label,
                              const std::string& value = "",
                              const SelectOptionStyle& style = {})
            : label(label), value(value.empty() ? label : value), style(style) {}

        explicit SelectOption(ml::Core& component, const std::string& value = "")
            : label(""), value(value), customComponent(&component) {}
    };

    /**
     * @brief A dropdown select control with rich per-item content.
     * @ingroup GraphicsControls
     *
     * Inherits @c SelectSettings (geometry/behaviour) and @c SelectTheme
     * (colors/font) as first-class members.
     *
     * @see SelectSettings, SelectTheme, SelectStyle
     */
    class Select : public ComponentWith<SelectManifest>,
                   public SelectSettings,
                   public SelectTheme,
                   public Themeable
    {
    public:
        using Flag  = SelectManifest::Flag;
        using State = SelectManifest::State;

    private:
        std::vector<SelectOption> _options;
        int                       _selectedIndex = -1;

        sf::RectangleShape        _trigger;
        sf::Text                  _triggerLabel;
        sf::Text                  _arrow;
        std::string               _placeholder   = "Select...";
        sf::Vector2f              _position      = {0.f, 0.f};

        sf::RectangleShape        _panel;
        float                     _scrollOffset  = 0.f;
        mutable sf::RenderTexture _panelCanvas;
        mutable int               _hoveredIndex  = -1;
        mutable bool              _openAbove     = false;

        std::function<void(const std::string&, std::size_t)> _onSelectionChanged;

        void syncTrigger();
        void syncTriggerColors();
        void syncPanel();
        void openPanel();
        void closePanel();
        void commitSelection(int index);
        void resizePanelCanvas();

        float panelHeight() const;
        float panelY()      const;

        int  hitTestPanel(const sf::Vector2f& worldPos) const;
        void drawOption(sf::RenderTarget& target,
                        const sf::RenderStates& states,
                        const SelectOption& option,
                        float itemY, bool hovered, bool selected) const;

    protected:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        void onThemeApplied(const Theme& theme) override;

    public:
        explicit Select(const sf::Font& font = FontManager<>::getDefault());

        // ── Apply ─────────────────────────────────────────────────────────────

        template<typename S>
        void applySettings(const S& s)
        {
            static_assert(std::is_base_of_v<SelectSettings, S>,
                "applySettings() requires a type derived from SelectSettings");
            static_cast<SelectSettings&>(*this) = s;
            syncTrigger();
        }

        template<typename T>
        void applyTheme(const T& t)
        {
            static_assert(std::is_base_of_v<SelectTheme, T>,
                "applyTheme() requires a type derived from SelectTheme");
            static_cast<SelectTheme&>(*this) = t;
            syncTriggerColors();
        }

        template<typename St>
        void applyStyle(const St& s)
        {
            static_assert(std::is_base_of_v<SelectSettings, St> &&
                          std::is_base_of_v<SelectTheme, St>,
                "applyStyle() requires SelectSettings and SelectTheme");
            static_cast<SelectSettings&>(*this) = s;
            static_cast<SelectTheme&>(*this)    = s;
            syncTrigger();
        }

        // ── Adding options ────────────────────────────────────────────────────

        void addOption(const std::string& label,
                       const std::string& value = "",
                       const SelectOptionStyle& style = {});
        void add(ml::Core& component, const std::string& value = "");
        void clearOptions();
        [[nodiscard]] std::size_t optionCount() const;

        // ── Selection ─────────────────────────────────────────────────────────

        void selectIndex(std::size_t index);
        void selectValue(const std::string& value);
        void clearSelection();
        [[nodiscard]] int         getSelectedIndex() const;
        [[nodiscard]] std::string getSelectedLabel() const;
        [[nodiscard]] std::string getSelectedValue() const;

        // ── Open / close ──────────────────────────────────────────────────────

        void open();
        void close();
        [[nodiscard]] bool isOpen() const;

        // ── Callback ─────────────────────────────────────────────────────────

        void onSelectionChanged(
            std::function<void(const std::string&, std::size_t)> callback);

        // ── Misc ──────────────────────────────────────────────────────────────

        void setPlaceholder(const std::string& text);
        [[nodiscard]] std::string getPlaceholder() const;

        void setEnabled(bool enabled);
        [[nodiscard]] bool isEnabled() const;
        void setOptionEnabled(std::size_t index, bool enabled);

        void setFont(const sf::Font& f);
        void setFont(const sf::Font&&) = delete;
        [[nodiscard]] unsigned int getCharacterSize() const;

        // ── Positionable ──────────────────────────────────────────────────────

        void          setPosition(const sf::Vector2f& position) override;
        sf::Vector2f  getPosition()     const override;
        sf::FloatRect getGlobalBounds() const override;
    };

    template<typename MANIFEST>
    class SelectWith : public Select, public Customizable<MANIFEST>
    { public: using Select::Select; };

} // namespace ml

#endif // SELECT_H
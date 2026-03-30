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

namespace ml
{
    // ── SelectManifest ────────────────────────────────────────────────────────

    /**
     * @brief Manifest for the @c Select dropdown component.
     * @ingroup GraphicsControls
     */
    class SelectManifest : public ml::Manifest
    {
    public:
        /**
         * | Flag       | Effect                                  |
         * |------------|-----------------------------------------|
         * | @c OPEN      | Dropdown panel is currently visible   |
         * | @c DISABLED  | Cannot be interacted with             |
         */
        enum class Flag { OPEN, DISABLED };

        /**
         * | State     | Meaning                             |
         * |-----------|-------------------------------------|
         * | @c IDLE    | Default resting state              |
         * | @c HOVERED | Mouse is over the trigger button   |
         * | @c OPEN    | Dropdown panel is showing          |
         * | @c DISABLED| Cannot be interacted with         |
         */
        enum class State { IDLE, HOVERED, OPEN, DISABLED };
    };

    // ── SelectOptionStyle ─────────────────────────────────────────────────────

    /**
     * @brief Per-item visual styling for a @c Select option.
     *
     * All fields are optional — unset fields inherit the @c Select default.
     * Use the builder-style setters to apply only the properties you need:
     * @code
     * ml::SelectOptionStyle style;
     * style.color    = sf::Color(255, 100, 100);
     * style.bold     = true;
     * style.iconTexture = &myTexture;
     *
     * select.addOption("Danger Zone", "danger", style);
     * @endcode
     *
     * @see Select, SelectOption
     */
    struct SelectOptionStyle
    {
        std::optional<sf::Color>      color;       ///< Override label color
        std::optional<sf::Color>      bgColor;     ///< Override item background
        std::optional<const sf::Font*> font;       ///< Override font
        std::optional<unsigned int>   charSize;    ///< Override character size
        std::optional<bool>           bold;        ///< Bold label
        std::optional<bool>           italic;      ///< Italic label
        std::string                   description; ///< Optional secondary line below label
        std::optional<sf::Color>      descColor;   ///< Description text color
        std::optional<unsigned int>   descCharSize;///< Description text size
        const sf::Texture*            iconTexture  = nullptr; ///< Optional icon
        sf::IntRect                   iconRect;    ///< Sub-rect of icon texture
        sf::Vector2f                  iconSize     = {0.f, 0.f}; ///< Icon size (0 = native)
    };

    // ── SelectOption ──────────────────────────────────────────────────────────

    /**
     * @brief A single option inside a @c Select dropdown.
     *
     * Created internally by @c addOption, or wraps a user-provided
     * @c ml::Core component when passed to @c add().
     *
     * @see Select, SelectOptionStyle
     */
    struct SelectOption
    {
        std::string        label;
        std::string        value;
        SelectOptionStyle  style;
        bool               enabled  = true;
        bool               selected = false;

        /// If set, this component is drawn in the item slot instead of the
        /// built-in label layout. The Select positions it; do NOT call
        /// addComponent on it separately.
        ml::Core*          customComponent = nullptr;

        explicit SelectOption(const std::string&       label,
                              const std::string&       value = "",
                              const SelectOptionStyle& style = {})
            : label(label),
              value(value.empty() ? label : value),
              style(style)
        {}

        /// Constructor for custom component items.
        explicit SelectOption(ml::Core& component,
                              const std::string& value = "")
            : label(""),
              value(value),
              customComponent(&component)
        {}
    };

    // ── Select ────────────────────────────────────────────────────────────────

    /**
     * @brief A dropdown select control with rich per-item content.
     * @ingroup GraphicsControls
     *
     * @c Select renders a trigger button showing the current selection.
     * Clicking opens a panel listing all options. Each option can have its
     * own font, color, description line, icon, and background color — or be
     * replaced entirely by a custom @c ml::Core component.
     *
     * The panel flips above the trigger automatically when there is
     * insufficient space below on screen.
     *
     * ### Usage — plain labels
     * @code
     * ml::Select difficulty;
     * difficulty.setSize({200.f, 36.f});
     * difficulty.setPosition({100.f, 100.f});
     * difficulty.setPlaceholder("Select difficulty...");
     * difficulty.addOption("Easy");
     * difficulty.addOption("Normal");
     * difficulty.addOption("Hard");
     *
     * difficulty.onSelectionChanged([](const std::string& value, std::size_t i){
     *     std::cout << "Selected: " << value << "\n";
     * });
     * addComponent(difficulty);
     * @endcode
     *
     * ### Usage — rich styling
     * @code
     * ml::SelectOptionStyle easyStyle;
     * easyStyle.color       = sf::Color(100, 220, 100);
     * easyStyle.description = "Recommended for beginners";
     * easyStyle.bold        = true;
     *
     * ml::SelectOptionStyle hardStyle;
     * hardStyle.color       = sf::Color(220, 80, 80);
     * hardStyle.description = "For experienced players";
     * hardStyle.iconTexture = &skullTexture;
     * hardStyle.iconSize    = {20.f, 20.f};
     *
     * select.addOption("Easy", "easy", easyStyle);
     * select.addOption("Hard", "hard", hardStyle);
     * @endcode
     *
     * ### Usage — custom components
     * @code
     * ml::Rectangle myRow;
     * myRow.setFillColor(sf::Color::Blue);
     *
     * select.add(myRow, "custom_value");
     * // do NOT addComponent(myRow) — Select draws it
     * @endcode
     *
     * ### With a manifest
     * @code
     * class DifficultySelect : public ml::SelectWith<GameManifest> {};
     * @endcode
     *
     * @see SelectManifest, SelectOption, SelectOptionStyle
     */
    class Select : public ComponentWith<SelectManifest>
    {
    public:
        using Flag  = SelectManifest::Flag;
        using State = SelectManifest::State;

    private:
        // ── Options ───────────────────────────────────────────────────────────
        std::vector<SelectOption> _options;
        int                       _selectedIndex = -1;

        // ── Trigger button ────────────────────────────────────────────────────
        sf::RectangleShape _trigger;
        sf::Text           _triggerLabel;
        sf::Text           _arrow;
        std::string        _placeholder   = "Select...";
        sf::Vector2f       _size          = {200.f, 36.f};
        sf::Vector2f       _position      = {0.f,   0.f};
        float              _padding       = 10.f;

        // ── Panel ─────────────────────────────────────────────────────────────
        sf::RectangleShape        _panel;
        mutable sf::RenderTexture _panelCanvas;
        float                     _itemHeight    = 40.f;
        float                     _maxVisible    = 6.f;
        float                     _scrollOffset  = 0.f;
        mutable bool              _openAbove     = false;
        int                       _hoveredIndex  = -1;

        // ── Colors — trigger ──────────────────────────────────────────────────
        sf::Color _trigBgIdle      = sf::Color(40,  40,  40);
        sf::Color _trigBgHover     = sf::Color(55,  55,  55);
        sf::Color _trigBgOpen      = sf::Color(50,  50,  50);
        sf::Color _trigBgDisabled  = sf::Color(30,  30,  30);
        sf::Color _trigBorder      = sf::Color(100, 100, 100);
        sf::Color _trigBorderOpen  = sf::Color(70,  130, 230);
        sf::Color _trigBorderDis   = sf::Color(60,  60,  60);
        float     _trigBorderThick = 1.5f;
        sf::Color _labelColor      = sf::Color::White;
        sf::Color _labelDisabled   = sf::Color(80,  80,  80);
        sf::Color _phColor         = sf::Color(120, 120, 120);
        sf::Color _arrowColor      = sf::Color(180, 180, 180);

        // ── Colors — panel ────────────────────────────────────────────────────
        sf::Color _panelBg           = sf::Color(45,  45,  45);
        sf::Color _panelBorder       = sf::Color(80,  80,  80);
        sf::Color _itemHoverBg       = sf::Color(70,  70,  70);
        sf::Color _itemSelectedBg    = sf::Color(70,  130, 230, 80);
        sf::Color _itemDefaultLabel  = sf::Color::White;
        sf::Color _itemDisabledColor = sf::Color(80,  80,  80);
        sf::Color _itemDescColor     = sf::Color(150, 150, 150);
        float     _panelBorderThick  = 1.f;

        // ── Font ──────────────────────────────────────────────────────────────
        const sf::Font* _font     = &FontManager<>::getDefault();
        unsigned int    _charSize = 14;
        unsigned int    _descSize = 11;

        // ── Callback ─────────────────────────────────────────────────────────
        std::function<void(const std::string&, std::size_t)> _onSelectionChanged;

        // ── Internal helpers ──────────────────────────────────────────────────
        void syncTrigger();
        void syncTriggerColors();
        void syncPanel();
        void openPanel();
        void closePanel();
        void commitSelection(int index);
        void resizePanelCanvas();

        float panelHeight()  const;
        float panelY()       const;

        int  hitTestPanel(const sf::Vector2f& worldPos) const;
        void drawOption(sf::RenderTarget& target,
                        const sf::RenderStates& states,
                        const SelectOption& option,
                        float itemY,
                        bool  hovered,
                        bool  selected) const;

    protected:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    public:
        explicit Select(const sf::Font& font = FontManager<>::getDefault());

        // ── Adding options ────────────────────────────────────────────────────

        /**
         * @brief Add an option by label and optional value/style.
         *
         * @param label  Display label shown in the dropdown and trigger.
         * @param value  Logical value. Defaults to @p label.
         * @param style  Optional per-item styling (color, description, icon…).
         */
        void addOption(const std::string&       label,
                       const std::string&       value = "",
                       const SelectOptionStyle& style = {});

        /**
         * @brief Add a custom component as a dropdown option.
         *
         * The component is drawn and positioned by the @c Select.
         * Do NOT call @c addComponent on it separately.
         * The component must outlive the @c Select.
         *
         * @param component Custom drawable component for this row.
         * @param value     Logical value returned on selection.
         */
        void add(ml::Core& component, const std::string& value = "");

        /** @brief Remove all options and clear the selection. */
        void clearOptions();

        /** @brief Return the number of options. */
        [[nodiscard]] std::size_t optionCount() const;

        // ── Selection ─────────────────────────────────────────────────────────

        /**
         * @brief Programmatically select by zero-based index.
         * No effect if out of range or disabled.
         */
        void selectIndex(std::size_t index);

        /**
         * @brief Programmatically select the first option whose value matches.
         */
        void selectValue(const std::string& value);

        /** @brief Clear the selection and show the placeholder. */
        void clearSelection();

        /** @brief Return the selected index, or @c -1. */
        [[nodiscard]] int getSelectedIndex() const;

        /** @brief Return the selected label, or @c "". */
        [[nodiscard]] std::string getSelectedLabel() const;

        /** @brief Return the selected value, or @c "". */
        [[nodiscard]] std::string getSelectedValue() const;

        // ── Open / close ──────────────────────────────────────────────────────

        void open();
        void close();
        [[nodiscard]] bool isOpen() const;

        // ── Callback ─────────────────────────────────────────────────────────

        /**
         * @brief Register a callback fired when the selection changes.
         * @param callback Invoked with the selected value and index.
         */
        void onSelectionChanged(
            std::function<void(const std::string&, std::size_t)> callback);

        // ── Placeholder ───────────────────────────────────────────────────────

        void setPlaceholder(const std::string& text);
        [[nodiscard]] std::string getPlaceholder() const;

        // ── Enabled / disabled ────────────────────────────────────────────────

        void setEnabled(bool enabled);
        [[nodiscard]] bool isEnabled() const;

        /**
         * @brief Enable or disable a specific option by index.
         * A disabled option is shown greyed out and cannot be clicked.
         */
        void setOptionEnabled(std::size_t index, bool enabled);

        // ── Layout ────────────────────────────────────────────────────────────

        void setSize(const sf::Vector2f& size);
        [[nodiscard]] sf::Vector2f getSize() const;

        /** @brief Set the height of each item row. Defaults to @c 40. */
        void setItemHeight(float height);

        /**
         * @brief Set the max number of visible rows before scrolling.
         * Defaults to @c 6.
         */
        void setMaxVisibleItems(float count);

        void setPadding(float padding);

        // ── Styling — trigger ─────────────────────────────────────────────────

        void setBackgroundColor(const sf::Color& color);
        void setBackgroundHoverColor(const sf::Color& color);
        void setBackgroundOpenColor(const sf::Color& color);
        void setBorderColor(const sf::Color& color);
        void setBorderOpenColor(const sf::Color& color);
        void setBorderThickness(float thickness);
        void setLabelColor(const sf::Color& color);
        void setPlaceholderColor(const sf::Color& color);
        void setArrowColor(const sf::Color& color);

        // ── Styling — panel ───────────────────────────────────────────────────

        void setPanelBackgroundColor(const sf::Color& color);
        void setPanelBorderColor(const sf::Color& color);
        void setItemHoverColor(const sf::Color& color);
        void setItemSelectedColor(const sf::Color& color);
        void setItemLabelColor(const sf::Color& color);
        void setItemDescriptionColor(const sf::Color& color);

        // ── Font ──────────────────────────────────────────────────────────────

        void setFont(const sf::Font& font);
        void setFont(const sf::Font&&) = delete;
        void setCharacterSize(unsigned int size);
        void setDescriptionCharacterSize(unsigned int size);
        [[nodiscard]] unsigned int getCharacterSize() const;

        // ── Positionable ──────────────────────────────────────────────────────

        void          setPosition(const sf::Vector2f& position) override;
        sf::Vector2f  getPosition()     const override;
        sf::FloatRect getGlobalBounds() const override;
    };

    // ── SelectWith ────────────────────────────────────────────────────────────

    template<typename MANIFEST>
    class SelectWith : public Select, public Customizable<MANIFEST>
    {
    public:
        using Select::Select;
    };

} // namespace ml

#endif // SELECT_H

//
// ToggleGroup.h
//

#ifndef MALENA_TOGGLEGROUP_H
#define MALENA_TOGGLEGROUP_H

#pragma once

#include <Malena/Core/malena_export.h>
#include <Malena/Core/Component.h>
#include <Malena/Manifests/Manifest.h>
#include <Malena/Graphics/Controls/PillToggle.h>
#include <Malena/Graphics/Controls/SegmentToggle.h>
#include <Malena/Graphics/Controls/ButtonToggle.h>
#include <Malena/Traits/Style/PillStyle.h>
#include <Malena/Traits/Style/SegmentStyle.h>
#include <Malena/Traits/Style/ButtonStyle.h>
#include <functional>
#include <string>
#include <vector>
#include <memory>

namespace ml
{
    class MALENA_API ToggleGroupManifest : public ml::Manifest
    {
    public:
        enum class Flag  {};
        enum class State { IDLE };
    };

    /**
     * @brief A vertical group of toggle switches.
     * @ingroup GraphicsControls
     *
     * Lays out multiple toggles vertically with consistent spacing.
     * Supports @c PillToggle, @c SegmentToggle, and @c ButtonToggle items.
     * All toggles are independent — multiple can be on simultaneously.
     *
     * Toggles created via @c addToggle() inherit the group's default style
     * for that type. Set defaults before adding, or call @c applySettingsToAll()
     * / @c applyThemeToAll() / @c applyStyleToAll() to update existing toggles.
     *
     * @code
     * ml::ToggleGroup settings;
     * settings.setPosition({200.f, 100.f});
     * settings.addToggle("Dark mode",  "dark_mode",  true);
     * settings.addToggle("Subtitles",  "subtitles");
     * settings.addToggle("Fullscreen", "fullscreen");
     *
     * settings.onAnyToggled([](const std::string& key, bool on){
     *     std::cout << key << ": " << on << "\n";
     * });
     * addComponent(settings);
     * @endcode
     *
     * @see PillToggle, SegmentToggle, ButtonToggle, PillStyle
     */
    class MALENA_API ToggleGroup : public ComponentWith<ToggleGroupManifest>
    {
    public:
        using Flag  = ToggleGroupManifest::Flag;
        using State = ToggleGroupManifest::State;

    private:
        enum class ItemType { PILL, SEGMENT, BUTTON };

        struct Item
        {
            ItemType       type;
            std::string    key;
            PillToggle*    pill    = nullptr;
            SegmentToggle* segment = nullptr;
            ButtonToggle*  button  = nullptr;
            bool           owned   = false;

            bool          isOn()  const;
            void          setOn(bool on);
            sf::Vector2f  pos()   const;
            void          setPos(const sf::Vector2f& p);
            sf::FloatRect bounds() const;
        };

        // unique_ptr<Item> keeps Items at stable heap addresses
        std::vector<std::unique_ptr<Item>>          _items;
        std::vector<std::unique_ptr<PillToggle>>    _ownedPills;
        std::vector<std::unique_ptr<SegmentToggle>> _ownedSegments;
        std::vector<std::unique_ptr<ButtonToggle>>  _ownedButtons;

        sf::Vector2f _position = {0.f, 0.f};
        float        _spacing  = 12.f;

        // Default styles applied to newly created owned toggles
        PillSettings    _defaultPillSettings;
        PillTheme       _defaultPillTheme;
        SegmentSettings _defaultSegmentSettings;
        SegmentTheme    _defaultSegmentTheme;
        ButtonSettings  _defaultButtonSettings;
        ButtonTheme     _defaultButtonTheme;

        std::function<void(const std::string&, bool)> _onAnyToggled;

        void relayout();
        void attach(Item& item);

    protected:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    public:
        explicit ToggleGroup();
        ~ToggleGroup();

        ToggleGroup(const ToggleGroup&)            = delete;
        ToggleGroup& operator=(const ToggleGroup&) = delete;

        // ── Adding owned toggles ──────────────────────────────────────────────

        /**
         * @brief Create and add a new @c PillToggle with a right label.
         *
         * Inherits the current default @c PillSettings and @c PillTheme.
         * @return Reference to the created toggle for further customisation.
         */
        PillToggle& addToggle(const std::string& label,
                              const std::string& key,
                              bool               on = false);

        /**
         * @brief Create and add a new @c SegmentToggle.
         * @return Reference to the created toggle.
         */
        SegmentToggle& addSegmentToggle(const std::string& offLabel,
                                        const std::string& onLabel,
                                        const std::string& key,
                                        bool               on = false);

        /**
         * @brief Create and add a new @c ButtonToggle.
         * @return Reference to the created toggle.
         */
        ButtonToggle& addButtonToggle(const std::string& label,
                                      const std::string& key,
                                      bool               on = false);

        // ── Adding external toggles ───────────────────────────────────────────

        void add(PillToggle&    toggle, const std::string& key);
        void add(SegmentToggle& toggle, const std::string& key);
        void add(ButtonToggle&  toggle, const std::string& key);

        void clearToggles();

        // ── State ─────────────────────────────────────────────────────────────

        [[nodiscard]] bool isOn(const std::string& key) const;
        void setOn(const std::string& key, bool on);
        void setAllOn();
        void setAllOff();
        [[nodiscard]] std::vector<std::string> getOnKeys() const;

        // ── Callbacks ─────────────────────────────────────────────────────────

        void onAnyToggled(std::function<void(const std::string&, bool)> callback);

        // ── Default styles ────────────────────────────────────────────────────

        /** @brief Set the default settings applied to new owned PillToggles. */
        void setDefaultSettings(const PillSettings& s);
        void setDefaultSettings(const SegmentSettings& s);
        void setDefaultSettings(const ButtonSettings& s);

        /** @brief Set the default theme applied to new owned PillToggles. */
        void setDefaultTheme(const PillTheme& t);
        void setDefaultTheme(const SegmentTheme& t);
        void setDefaultTheme(const ButtonTheme& t);

        /** @brief Set default settings and theme together. */
        void setDefaultStyle(const PillStyle& s);
        void setDefaultStyle(const SegmentStyle& s);
        void setDefaultStyle(const ButtonStyle& s);

        // ── Bulk apply to owned toggles ───────────────────────────────────────

        void applySettingsToAll(const PillSettings& s);
        void applySettingsToAll(const SegmentSettings& s);
        void applySettingsToAll(const ButtonSettings& s);

        void applyThemeToAll(const PillTheme& t);
        void applyThemeToAll(const SegmentTheme& t);
        void applyThemeToAll(const ButtonTheme& t);

        void applyStyleToAll(const PillStyle& s);
        void applyStyleToAll(const SegmentStyle& s);
        void applyStyleToAll(const ButtonStyle& s);

        // ── Convenience ───────────────────────────────────────────────────────

        /** @brief Set font on the default pill theme and all owned pills. */
        void setFont(const sf::Font& font);
        void setFont(const sf::Font&&) = delete;

        /** @brief Set character size on the default pill theme and all owned pills. */
        void setCharacterSize(unsigned int size);

        void setSpacing(float spacing);

        // ── Positionable ──────────────────────────────────────────────────────

        void          setPosition(const sf::Vector2f& position) override;
        sf::Vector2f  getPosition()     const override;
        sf::FloatRect getGlobalBounds() const override;
    };

    template<typename MANIFEST>
    class ToggleGroupWith : public ToggleGroup, public Customizable<MANIFEST>
    { public: using ToggleGroup::ToggleGroup; };

} // namespace ml

#endif // MALENA_TOGGLEGROUP_H
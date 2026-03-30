//
// ToggleGroup.h
//

#ifndef TOGGLEGROUP_H
#define TOGGLEGROUP_H

#pragma once

#include <Malena/Core/Component.h>
#include <Malena/Manifests/Manifest.h>
#include <Malena/Graphics/Controls/PillToggle.h>
#include <Malena/Graphics/Controls/SegmentToggle.h>
#include <Malena/Graphics/Controls/ButtonToggle.h>
#include <functional>
#include <string>
#include <vector>
#include <list>
#include <memory>

namespace ml
{
    class ToggleGroupManifest : public ml::Manifest
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
     * @see PillToggle, SegmentToggle, ButtonToggle
     */
    class ToggleGroup : public ComponentWith<ToggleGroupManifest>
    {
    public:
        using Flag  = ToggleGroupManifest::Flag;
        using State = ToggleGroupManifest::State;

    private:
        // Flat parallel arrays — avoids complex struct with multiple
        // base-class pointer casts that cause heap corruption on reallocation
        enum class ItemType { PILL, SEGMENT, BUTTON };

        struct Item
        {
            ItemType    type;
            std::string key;

            // At most one is non-null
            PillToggle*    pill    = nullptr;
            SegmentToggle* segment = nullptr;
            ButtonToggle*  button  = nullptr;

            bool owned = false;

            // Convenience accessors
            bool         isOn()   const;
            void         setOn(bool on);
            sf::Vector2f pos()    const;
            void         setPos(const sf::Vector2f& p);
            sf::FloatRect bounds() const;
        };

        std::list<Item>                          _items;
        std::vector<std::unique_ptr<PillToggle>> _ownedPills;

        sf::Vector2f _position = {0.f, 0.f};
        float        _spacing  = 12.f;

        const sf::Font* _font     = &FontManager<>::getDefault();
        unsigned int    _charSize = 14;

        std::function<void(const std::string&, bool)> _onAnyToggled;

        void relayout();
        void attach(Item& item);

    protected:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    public:
        explicit ToggleGroup(const sf::Font& font = FontManager<>::getDefault());

        // ── Adding toggles ────────────────────────────────────────────────────

        /**
         * @brief Create and add a new @c PillToggle with a right label.
         * @return Reference to the created toggle for further customisation.
         */
        PillToggle& addToggle(const std::string& label,
                              const std::string& key,
                              bool               on = false);

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

        // ── Styling ───────────────────────────────────────────────────────────

        void setSpacing(float spacing);
        void setFont(const sf::Font& font);
        void setFont(const sf::Font&&) = delete;
        void setCharacterSize(unsigned int size);

        // ── Positionable ──────────────────────────────────────────────────────

        void          setPosition(const sf::Vector2f& position) override;
        sf::Vector2f  getPosition()     const override;
        sf::FloatRect getGlobalBounds() const override;
    };

    template<typename MANIFEST>
    class ToggleGroupWith : public ToggleGroup, public Customizable<MANIFEST>
    { public: using ToggleGroup::ToggleGroup; };

} // namespace ml

#endif // TOGGLEGROUP_H
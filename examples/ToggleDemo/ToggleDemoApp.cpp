// Copyright 2025 Dave R. Smith
// SPDX-License-Identifier: Apache-2.0
//
// ToggleDemo — showcases PillToggle, SegmentToggle, ButtonToggle,
// ToggleGroup, the Theme/Settings system, and Manifest integration.

#include <Malena/Engine/App/Application.h>
#include <Malena/Manifests/Manifest.h>
#include <Malena/Manifests/Theme.h>
#include <Malena/Resources/ThemeManager.h>
#include <Malena/Graphics/Controls/PillToggle.h>
#include <Malena/Graphics/Controls/SegmentToggle.h>
#include <Malena/Graphics/Controls/ButtonToggle.h>
#include <Malena/Graphics/Controls/ToggleGroup.h>
#include <Malena/Graphics/Primitives/Rectangle.h>
#include <Malena/Graphics/Text/Text.h>
#include <Malena/Utilities/Align.h>
#include <Malena/Traits/Settings/PillSettings.h>
#include <Malena/Traits/Style/PillStyle.h>

// ── Custom themes ─────────────────────────────────────────────────────────────

struct NeonTheme : ml::Theme
{
    NeonTheme()
    {
        primary     = sf::Color(0,   255, 180);
        secondary   = sf::Color(0,   200, 140);
        surface     = sf::Color(10,  15,  30);
        background  = sf::Color(5,   8,   20);
        onSurface   = sf::Color(220, 255, 245);
        onPrimary   = sf::Color(5,   8,   20);
        border      = sf::Color(0,   140, 100);
        borderFocus = sf::Color(0,   255, 180);
        muted       = sf::Color(80,  160, 130);
        radius      = 4.f;
    }
};

struct WarmTheme : ml::Theme
{
    WarmTheme()
    {
        primary     = sf::Color(220, 100, 50);
        secondary   = sf::Color(240, 150, 80);
        surface     = sf::Color(45,  35,  30);
        background  = sf::Color(25,  18,  15);
        onSurface   = sf::Color(255, 240, 220);
        onPrimary   = sf::Color::White;
        border      = sf::Color(120, 80,  60);
        borderFocus = sf::Color(220, 100, 50);
        muted       = sf::Color(150, 110, 90);
        radius      = 12.f;
    }
};

// ── Custom settings / styles ──────────────────────────────────────────────────

// GreenPillStyle — sets both a color (PillTheme) and geometry (PillSettings)
// so it inherits PillStyle which combines both layers.
struct GreenPillStyle : ml::PillStyle
{
    GreenPillStyle()
    {
        trackOnColor = sf::Color(70, 200, 100);  // PillTheme
        trackSize    = {56.f, 28.f};             // PillSettings
        setPillLabels("OFF", "ON");              // PillSettings
    }
};

// LargePillSettings — geometry/behaviour only, no colors changed.
struct LargePillSettings : ml::PillSettings
{
    LargePillSettings()
    {
        trackSize    = {70.f, 34.f};
        setPillLabels("NO", "YES");
        animSpeed    = 6.f;
    }
};

// ── Manifest ──────────────────────────────────────────────────────────────────

class ToggleDemoManifest : public ml::Manifest
{
public:
    enum class Themes   { Dark, Neon, Warm, Light };
    enum class Styles   { GreenPill };
    enum class Settings { LargePill };

	inline static const auto _ = [](){
		set(Themes::Dark,  ml::DarkTheme());
		set(Themes::Neon,  NeonTheme());
		set(Themes::Warm,  WarmTheme());
		set(Themes::Light, ml::LightTheme());

		set(Styles::GreenPill,   GreenPillStyle());
		set(Settings::LargePill, LargePillSettings());
		return 0;
	}();
};

// ── ToggleDemoApp ─────────────────────────────────────────────────────────────

class ToggleDemoApp : public ml::ApplicationWith<ToggleDemoManifest>
{
    using Themes   = ToggleDemoManifest::Themes;
    using Styles   = ToggleDemoManifest::Styles;
    using Settings = ToggleDemoManifest::Settings;

    // ── Background ────────────────────────────────────────────────────────────
    ml::Rectangle _bg;

    // ── Theme switcher ────────────────────────────────────────────────────────
    ml::Text         _themeHeading;
    ml::ButtonToggle _btnDark;
    ml::ButtonToggle _btnNeon;
    ml::ButtonToggle _btnWarm;
    ml::ButtonToggle _btnLight;

    // ── PILL section ──────────────────────────────────────────────────────────
    ml::Text       _pillHeading;
    ml::PillToggle _pillDefault;      // default theme colors
    ml::PillToggle _pillGreen;        // GreenPillStyle from manifest
    ml::PillToggle _pillLarge;        // LargePillSettings, starts ON
    ml::PillToggle _pillInsideLabels; // inside OFF/ON labels
    ml::PillToggle _pillDisabled;     // disabled state
    ml::PillToggle _pillLocked;       // locked — ignores theme changes

    // ── SEGMENT section ───────────────────────────────────────────────────────
    ml::Text          _segHeading;
    ml::SegmentToggle _segDefault;
    ml::SegmentToggle _segHotels;
    ml::SegmentToggle _segDisabled;

    // ── BUTTON section ────────────────────────────────────────────────────────
    ml::Text         _btnHeading;
    ml::ButtonToggle _btnSave;
    ml::ButtonToggle _btnMute;
    ml::ButtonToggle _btnDisabled;

    // ── GROUP section ─────────────────────────────────────────────────────────
    ml::Text        _groupHeading;
    ml::ToggleGroup _group;

    // ── Status bar ────────────────────────────────────────────────────────────
    ml::Text _status;

    void setStatus(const std::string& msg) { _status.setString(msg); }

    void setupHeading(ml::Text& t, const std::string& str, float x, float y)
    {
        t.setString(str);
        t.setCharacterSize(11);
        t.setFillColor(sf::Color(160, 160, 160));
        t.setStyle(sf::Text::Style::Bold);
        t.setPosition({x, y});
        addComponent(t);
    }

    void setupThemeBtn(ml::ButtonToggle& btn, const std::string& label, float x)
    {
        btn.setSize({80.f, 28.f});
        btn.setOffLabel(label);
        btn.setOnLabel(label);
        btn.setButtonRadius(14.f);
        btn.setPosition({x, 30.f});
        addComponent(btn);
    }

public:
    ToggleDemoApp()
        : ml::ApplicationWith<ToggleDemoManifest>(900, 660, 32, "Malena - Toggle Demo")
    {}

    void onInit() override
    {
        // Background
        _bg.setSize({860.f, 620.f});
        _bg.setFillColor(sf::Color(18, 18, 24));
        _bg.setPosition({20.f, 20.f});
        addComponent(_bg);

        // Theme switcher
        setupHeading(_themeHeading, "THEME", 40.f, 36.f);
        setupThemeBtn(_btnDark,  "Dark",  120.f);
        setupThemeBtn(_btnNeon,  "Neon",  210.f);
        setupThemeBtn(_btnWarm,  "Warm",  300.f);
        setupThemeBtn(_btnLight, "Light", 390.f);
        _btnDark.setOn(true);

        // ── PILL ──────────────────────────────────────────────────────────────
        const float COL1 = 40.f;
        const float COL2 = 300.f;
        const float COL3 = 560.f;

        setupHeading(_pillHeading, "PILL TOGGLES", COL1, 80.f);

        _pillDefault.setRightLabel("Default (theme colors)");
        _pillDefault.setPosition({COL1, 104.f});
        addComponent(_pillDefault);

        _pillGreen.applyStyle(
            ml::Manifest::getConfig<Styles, GreenPillStyle>(Styles::GreenPill));
        _pillGreen.setRightLabel("Green (manifest style)");
        ml::Align::setBelow(_pillDefault, _pillGreen, 16.f);
        addComponent(_pillGreen);

        _pillLarge.applySettings(
            ml::Manifest::getConfig<Settings, LargePillSettings>(Settings::LargePill));
        _pillLarge.setRightLabel("Large (starts ON)");
        _pillLarge.setOn(true);
        ml::Align::setBelow(_pillGreen, _pillLarge, 16.f);
        addComponent(_pillLarge);

        _pillInsideLabels.setPillLabels("OFF", "ON");
        _pillInsideLabels.setTrackSize({64.f, 28.f});
        _pillInsideLabels.setRightLabel("Inside labels");
        ml::Align::setBelow(_pillLarge, _pillInsideLabels, 16.f);
        addComponent(_pillInsideLabels);

        _pillDisabled.setRightLabel("Disabled");
        _pillDisabled.setEnabled(false);
        ml::Align::setBelow(_pillInsideLabels, _pillDisabled, 16.f);
        addComponent(_pillDisabled);

        _pillLocked.setTrackOnColor(sf::Color(255, 80, 80));
        _pillLocked.setTrackOffColor(sf::Color(80, 80, 80));
        _pillLocked.lockTheme();
        _pillLocked.setRightLabel("Locked (ignores theme)");
        ml::Align::setBelow(_pillDisabled, _pillLocked, 16.f);
        addComponent(_pillLocked);

        // ── SEGMENT ───────────────────────────────────────────────────────────
        setupHeading(_segHeading, "SEGMENT TOGGLES", COL2, 80.f);

        _segDefault.setSegmentLabels("Off", "On");
        _segDefault.setSize({180.f, 36.f});
        _segDefault.setPosition({COL2, 104.f});
        addComponent(_segDefault);

        _segHotels.setSegmentLabels("Hotels", "Apartments");
        _segHotels.setSize({200.f, 36.f});
        ml::Align::setBelow(_segDefault, _segHotels, 16.f);
        addComponent(_segHotels);

        _segDisabled.setSegmentLabels("Yes", "No");
        _segDisabled.setSize({160.f, 36.f});
        _segDisabled.setEnabled(false);
        ml::Align::setBelow(_segHotels, _segDisabled, 16.f);
        addComponent(_segDisabled);

        // ── BUTTON ────────────────────────────────────────────────────────────
        setupHeading(_btnHeading, "BUTTON TOGGLES", COL3, 80.f);

        _btnSave.setOffLabel("Save");
        _btnSave.setOnLabel("Saved");
        _btnSave.setSize({110.f, 36.f});
        _btnSave.setPosition({COL3, 104.f});
        addComponent(_btnSave);

        _btnMute.setOffLabel("Unmuted");
        _btnMute.setOnLabel("Muted");
        _btnMute.setOnColor(sf::Color(200, 60, 60));
        _btnMute.setOnTextColor(sf::Color::White);
        _btnMute.setSize({110.f, 36.f});
        ml::Align::setBelow(_btnSave, _btnMute, 16.f);
        addComponent(_btnMute);

        _btnDisabled.setOffLabel("Disabled");
        _btnDisabled.setOnLabel("Disabled");
        _btnDisabled.setSize({110.f, 36.f});
        _btnDisabled.setEnabled(false);
        ml::Align::setBelow(_btnMute, _btnDisabled, 16.f);
        addComponent(_btnDisabled);

        // ── GROUP ─────────────────────────────────────────────────────────────
        setupHeading(_groupHeading, "TOGGLE GROUP", COL1, 370.f);

        _group.setPosition({COL1, 394.f});
        _group.setSpacing(14.f);
        _group.addToggle("Dark mode",  "dark_mode",  true);
        _group.addToggle("Subtitles",  "subtitles");
        _group.addToggle("Fullscreen", "fullscreen");
        _group.addToggle("Show FPS",   "show_fps",   true);
        _group.addToggle("VSync",      "vsync",      true);
        addComponent(_group);

        // ── Status ────────────────────────────────────────────────────────────
        _status.setCharacterSize(12);
        _status.setFillColor(sf::Color(160, 160, 160));
        _status.setString("Click any toggle — use theme buttons to switch themes");
        _status.setPosition({40.f, 600.f});
        addComponent(_status);
    }

    void onReady() override
    {
        // Apply initial theme — must be here, not onInit(), to ensure
        // static manifest initialization has fully completed first.
        ml::ThemeManager::apply<ToggleDemoManifest>(Themes::Dark);

        // Theme buttons — mutual exclusion, no re-entrancy guard needed
        _btnDark.onToggled([&](bool on){
            if (!on) return;
            _btnNeon.setOn(false); _btnWarm.setOn(false); _btnLight.setOn(false);
            ml::ThemeManager::apply<ToggleDemoManifest>(Themes::Dark);
            setStatus("Theme: Dark");
        });
        _btnNeon.onToggled([&](bool on){
            if (!on) return;
            _btnDark.setOn(false); _btnWarm.setOn(false); _btnLight.setOn(false);
            ml::ThemeManager::apply<ToggleDemoManifest>(Themes::Neon);
            setStatus("Theme: Neon — notice the locked red pill doesn't change");
        });
        _btnWarm.onToggled([&](bool on){
            if (!on) return;
            _btnDark.setOn(false); _btnNeon.setOn(false); _btnLight.setOn(false);
            ml::ThemeManager::apply<ToggleDemoManifest>(Themes::Warm);
            setStatus("Theme: Warm");
        });
        _btnLight.onToggled([&](bool on){
            if (!on) return;
            _btnDark.setOn(false); _btnNeon.setOn(false); _btnWarm.setOn(false);
            ml::ThemeManager::apply<ToggleDemoManifest>(Themes::Light);
            setStatus("Theme: Light");
        });

        // Pill callbacks
        _pillDefault.onToggled([&](bool on){
            setStatus(std::string("Default pill: ") + (on ? "ON" : "OFF")); });
        _pillGreen.onToggled([&](bool on){
            setStatus(std::string("Green pill: ") + (on ? "ON" : "OFF")); });
        _pillLarge.onToggled([&](bool on){
            setStatus(std::string("Large pill: ") + (on ? "YES" : "NO")); });
        _pillInsideLabels.onToggled([&](bool on){
            setStatus(std::string("Inside labels: ") + (on ? "ON" : "OFF")); });
        _pillLocked.onToggled([&](bool on){
            setStatus(std::string("Locked pill: ") + (on ? "ON" : "OFF")); });

        // Segment callbacks
        _segDefault.onToggled([&](bool on){
            setStatus(std::string("Segment: ") + (on ? "On" : "Off")); });
        _segHotels.onToggled([&](bool on){
            setStatus(std::string("View: ") + (on ? "Apartments" : "Hotels")); });

        // Button callbacks
        _btnSave.onToggled([&](bool on){
            setStatus(on ? "Saved!" : "Save cleared"); });
        _btnMute.onToggled([&](bool on){
            setStatus(on ? "Audio muted" : "Audio unmuted"); });

        // Group callback
        _group.onAnyToggled([&](const std::string& key, bool on){
            setStatus(key + ": " + (on ? "on" : "off")); });
    }
};

// ── main ──────────────────────────────────────────────────────────────────────

int main()
{
    ToggleDemoApp app;
    app.run();
    return 0;
}
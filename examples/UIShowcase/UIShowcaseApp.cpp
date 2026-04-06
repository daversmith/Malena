// Copyright 2025 Dave R. Smith
// SPDX-License-Identifier: Apache-2.0
//
// UIShowcase:
//   LEFT  — TabbedPanel  (Tab 1: Toggles | Tab 2: Selections)
//   RIGHT — SplitPanel   (Left pane: Inputs | Right pane: List)
//   TOP   — MenuBar + Toolbar
//   SIDE  — SideMenu

#include <Malena/Engine/App/Application.h>
#include <Malena/Manifests/Manifest.h>
#include <Malena/Manifests/Theme.h>
#include <Malena/Resources/ThemeManager.h>
#include <Malena/Graphics/Controls/PillToggle.h>
#include <Malena/Graphics/Controls/SegmentToggle.h>
#include <Malena/Graphics/Controls/ButtonToggle.h>
#include <Malena/Graphics/Controls/Checkbox.h>
#include <Malena/Graphics/Controls/CheckboxGroup.h>
#include <Malena/Graphics/Controls/RadioButton.h>
#include <Malena/Graphics/Controls/RadioGroup.h>
#include <Malena/Graphics/Text/TextInput.h>
#include <Malena/Graphics/Text/TextArea.h>
#include <Malena/Graphics/Controls/Select.h>
#include <Malena/Graphics/Controls/List.h>
#include <Malena/Graphics/Controls/ListItem.h>
#include <Malena/Graphics/Controls/TabbedPanel.h>
#include <Malena/Graphics/Controls/SplitPanel.h>
#include <Malena/Graphics/Layouts/Panel.h>
#include <Malena/Graphics/Controls/MenuBar.h>
#include <Malena/Graphics/Controls/Toolbar.h>
#include <Malena/Graphics/Controls/SideMenu.h>
#include <Malena/Graphics/Primitives/Rectangle.h>
#include <Malena/Graphics/Text/Text.h>

// ── Themes ────────────────────────────────────────────────────────────────────

struct NeonTheme : ml::Theme { NeonTheme() {
    primary=sf::Color(0,255,180);  secondary=sf::Color(0,200,140);
    surface=sf::Color(10,15,30);   background=sf::Color(5,8,20);
    onSurface=sf::Color(220,255,245); onPrimary=sf::Color(5,8,20);
    border=sf::Color(0,140,100);   borderFocus=sf::Color(0,255,180);
    muted=sf::Color(80,160,130);   radius=4.f;
}};
struct WarmTheme : ml::Theme { WarmTheme() {
    primary=sf::Color(220,100,50); secondary=sf::Color(240,150,80);
    surface=sf::Color(45,35,30);   background=sf::Color(25,18,15);
    onSurface=sf::Color(255,240,220); onPrimary=sf::Color::White;
    border=sf::Color(120,80,60);   borderFocus=sf::Color(220,100,50);
    muted=sf::Color(150,110,90);   radius=12.f;
}};
struct OceanTheme : ml::Theme { OceanTheme() {
    primary=sf::Color(30,144,255); secondary=sf::Color(0,191,255);
    surface=sf::Color(15,25,45);   background=sf::Color(8,15,30);
    onSurface=sf::Color(220,235,255); onPrimary=sf::Color::White;
    border=sf::Color(40,80,140);   borderFocus=sf::Color(30,144,255);
    muted=sf::Color(80,110,160);   radius=6.f;
}};

class ShowcaseManifest : public ml::Manifest {
public:
    enum class Themes { Dark, Neon, Warm, Ocean, Light };
    inline static const auto _ = [](){
        set(Themes::Dark,  ml::DarkTheme());
        set(Themes::Neon,  NeonTheme());
        set(Themes::Warm,  WarmTheme());
        set(Themes::Ocean, OceanTheme());
        set(Themes::Light, ml::LightTheme());
        return 0;
    }();
};

// ── UIShowcaseApp ─────────────────────────────────────────────────────────────

class UIShowcaseApp : public ml::ApplicationWith<ShowcaseManifest>
{
    using Themes = ShowcaseManifest::Themes;

    // Layout
    static constexpr float MENU_H    = 30.f;
    static constexpr float TOOLBARH = 44.f;
    static constexpr float CONTENT_Y = MENU_H + TOOLBARH + 8.f;  // 82
    static constexpr float WIN_W     = 1680.f;
    static constexpr float WIN_H     = 860.f;
    static constexpr float COL_H     = WIN_H - CONTENT_Y - 26.f;  // 752
    static constexpr float TAB_H     = 36.f;   // TabbedPanel strip
    static constexpr float DIV_T     = 5.f;    // SplitPanel divider

    // TabbedPanel: left half
    static constexpr float TP_X      = 20.f;
    static constexpr float TP_W      = 800.f;
    static constexpr float TP_CX     = TP_X + 14.f;           // content left edge
    static constexpr float TP_CY     = CONTENT_Y + TAB_H + 10.f; // content top edge

    // SplitPanel: right half
    static constexpr float SP_X      = 840.f;
    static constexpr float SP_W      = WIN_W - SP_X - 20.f;   // 820
    static constexpr float SP_PANE_W = (SP_W - DIV_T) / 2.f;  // ~407
    static constexpr float SP_CY     = CONTENT_Y + 10.f;
    static constexpr float SP_L_CX   = SP_X + 14.f;           // left pane content x
    static constexpr float SP_R_CX   = SP_X + SP_PANE_W + DIV_T + 14.f; // right pane x

    // ── Background ────────────────────────────────────────────────────────────
    ml::Rectangle _bg;


    // ── TabbedPanel + SplitPanel ──────────────────────────────────────────────
    ml::TabbedPanel _tabs;
    ml::SplitPanel  _split;

    // ─────────────────────────────────────────────────────────────────────────
    // TAB 1 — TOGGLES
    // ─────────────────────────────────────────────────────────────────────────
    ml::Text          _pillHead, _segHead, _btnHead;
    ml::PillToggle    _pillDefault, _pillGreen, _pillLarge,
                      _pillInside, _pillDisabled, _pillLocked;
    ml::SegmentToggle _segDefault, _segHotels;
    ml::ButtonToggle  _btnSave, _btnMute, _btnBtnDisabled;

    // ─────────────────────────────────────────────────────────────────────────
    // TAB 2 — SELECTIONS
    // ─────────────────────────────────────────────────────────────────────────
    ml::Text          _checkHead, _checkGroupHead, _radioHead;
    ml::Checkbox      _chkOne, _chkTwo, _chkThree, _chkDisabled;
    ml::CheckboxGroup _checkGroup;
    ml::RadioGroup    _radioGroup;

    // ─────────────────────────────────────────────────────────────────────────
    // LEFT PANE — INPUTS
    // ─────────────────────────────────────────────────────────────────────────
    ml::Text      _inputHead, _areaHead, _selectHead;
    ml::TextInput _inputDefault, _inputPassword, _inputDisabled, _inputError;
    ml::TextArea  _textArea;
    ml::Select    _select;

    // ─────────────────────────────────────────────────────────────────────────
    // RIGHT PANE — LIST
    // ─────────────────────────────────────────────────────────────────────────
    ml::Text      _listHead;
    ml::List      _list;
    ml::Rectangle _iconWifi, _iconBt, _iconDark, _iconNote, _iconAlert;
    ml::PillToggle _toggleWifi, _toggleDark;
    ml::Checkbox  _chkList;
    ml::ButtonToggle _btnListAction;

    // ── Status + overlays ─────────────────────────────────────────────────────
    ml::Text     _status;
    ml::MenuBar  _menuBar;
    ml::Toolbar  _toolbar;
    ml::SideMenu _sideMenu;

    // ── Helpers ───────────────────────────────────────────────────────────────
    void setStatus(const std::string& s) { _status.setString(s); }

    void heading(ml::Text& t, const std::string& s, float x, float y, ml::Panel& panel)
    {
        t.setString(s);
        t.setCharacterSize(11);
        t.setFillColor(sf::Color(160,160,160));
        t.setStyle(sf::Text::Style::Bold);
        t.setPosition({x, y});
        panel.addComponent(t);
    }

    void icon(ml::Rectangle& r, const sf::Color& c)
    { r.setSize({22.f,22.f}); r.setRadius(4.f); r.setFillColor(c); }

public:
    UIShowcaseApp()
        : ml::ApplicationWith<ShowcaseManifest>(
            static_cast<unsigned int>(WIN_W),
            static_cast<unsigned int>(WIN_H), 32, "Malena - UI Showcase")
    {}

    void onInit() override
    {
        ml::ThemeManager::apply<ShowcaseManifest>(Themes::Dark);

        // ── Background ────────────────────────────────────────────────────────
        _bg.setSize({WIN_W-20.f, WIN_H-20.f});
        _bg.setFillColor(sf::Color(18,18,24));
        _bg.setPosition({10.f, 10.f});
        addComponent(_bg);

        // ── TabbedPanel setup — panels are positioned before children are added ──
        _tabs.setSize({TP_W, COL_H});
        _tabs.setPosition({TP_X, CONTENT_Y});
        _tabs.setCloseable(true);
        _tabs.contentBg      = sf::Color::Transparent;
        _tabs.tabBarThickness = 1.f;

        auto& tab1 = _tabs.addTab("Toggles",    std::make_unique<ml::Panel>());
        auto& tab2 = _tabs.addTab("Selections", std::make_unique<ml::Panel>());

        _tabs.onTabChanged([this](std::size_t, const std::string& lbl){
            setStatus("Tab: " + lbl);
        });
        addComponent(_tabs);

        // ─────────────────────────────────────────────────────────────────────
        // TAB 1 — TOGGLES  (positioned at TP_CX, TP_CY)
        // ─────────────────────────────────────────────────────────────────────
        {
            float y = TP_CY;
            const float C1 = TP_CX;

            heading(_pillHead,"PILL", C1, y, tab1); y+=22.f;

            _pillDefault.setRightLabel("Default");
            _pillDefault.setPosition({C1,y}); tab1.addComponent(_pillDefault); y+=32.f;

            _pillGreen.setTrackOnColor(sf::Color(70,200,100));
            _pillGreen.lockTheme();
            _pillGreen.setRightLabel("Custom green (locked)");
            _pillGreen.setPosition({C1,y}); tab1.addComponent(_pillGreen); y+=32.f;

            _pillLarge.setTrackSize({70.f,34.f});
            _pillLarge.setPillLabels("NO","YES");
            _pillLarge.setOn(true);
            _pillLarge.setRightLabel("Large / starts ON");
            _pillLarge.setPosition({C1,y}); tab1.addComponent(_pillLarge); y+=44.f;

            _pillInside.setPillLabels("OFF","ON");
            _pillInside.setTrackSize({64.f,28.f});
            _pillInside.setRightLabel("Inside labels");
            _pillInside.setPosition({C1,y}); tab1.addComponent(_pillInside); y+=32.f;

            _pillDisabled.setRightLabel("Disabled");
            _pillDisabled.setEnabled(false);
            _pillDisabled.setPosition({C1,y}); tab1.addComponent(_pillDisabled); y+=32.f;

            _pillLocked.setTrackOnColor(sf::Color(220,60,60));
            _pillLocked.setTrackOffColor(sf::Color(70,70,70));
            _pillLocked.lockTheme();
            _pillLocked.setRightLabel("Locked red");
            _pillLocked.setPosition({C1,y}); tab1.addComponent(_pillLocked); y+=42.f;

            heading(_segHead,"SEGMENT", C1, y, tab1); y+=22.f;
            _segDefault.setSegmentLabels("Off","On");
            _segDefault.setSize({200.f,34.f});
            _segDefault.setPosition({C1,y}); tab1.addComponent(_segDefault); y+=46.f;

            _segHotels.setSegmentLabels("Hotels","Apartments");
            _segHotels.setSize({220.f,34.f});
            _segHotels.setPosition({C1,y}); tab1.addComponent(_segHotels); y+=50.f;

            heading(_btnHead,"BUTTON TOGGLE", C1, y, tab1); y+=22.f;
            _btnSave.setOffLabel("Save"); _btnSave.setOnLabel("Saved");
            _btnSave.setSize({120.f,34.f});
            _btnSave.setPosition({C1,y}); tab1.addComponent(_btnSave); y+=46.f;

            _btnMute.setOffLabel("Unmuted"); _btnMute.setOnLabel("Muted");
            _btnMute.setOnColor(sf::Color(200,60,60));
            _btnMute.setSize({120.f,34.f});
            _btnMute.setPosition({C1,y}); tab1.addComponent(_btnMute); y+=46.f;

            _btnBtnDisabled.setOffLabel("Disabled");
            _btnBtnDisabled.setSize({120.f,34.f});
            _btnBtnDisabled.setEnabled(false);
            _btnBtnDisabled.setPosition({C1,y}); tab1.addComponent(_btnBtnDisabled);
        }

        // ─────────────────────────────────────────────────────────────────────
        // TAB 2 — SELECTIONS
        // ─────────────────────────────────────────────────────────────────────
        {
            float y = TP_CY;
            const float C2 = TP_CX;

            heading(_checkHead,"CHECKBOX", C2, y, tab2); y+=22.f;
            for (auto [cb, lbl, chk, dis] :
                std::initializer_list<std::tuple<ml::Checkbox*,const char*,bool,bool>>{
                    {&_chkOne,     "Enable fullscreen",  false, false},
                    {&_chkTwo,     "Enable VSync",        false, false},
                    {&_chkThree,   "Show FPS counter",    true,  false},
                    {&_chkDisabled,"Disabled option",     false, true }})
            {
                cb->setLabel(lbl);
                if (chk) cb->check();
                if (dis) cb->setEnabled(false);
                cb->setPosition({C2,y}); tab2.addComponent(*cb); y+=30.f;
            }
            y+=16.f;

            heading(_checkGroupHead,"CHECKBOX GROUP", C2, y, tab2); y+=22.f;
            _checkGroup.addOption("Dark mode", true);
            _checkGroup.addOption("Subtitles");
            _checkGroup.addOption("Auto-save", true);
            _checkGroup.addOption("Analytics");
            _checkGroup.showBackground = true;
            _checkGroup.padding = 10.f;
            _checkGroup.spacing = 4.f;
            _checkGroup.setPosition({C2,y}); tab2.addComponent(_checkGroup); y+=180.f;

            heading(_radioHead,"RADIO GROUP", C2, y, tab2); y+=22.f;
            _radioGroup.addOption("Easy");
            _radioGroup.addOption("Normal");
            _radioGroup.addOption("Hard");
            _radioGroup.addOption("Impossible");
            _radioGroup.setOptionEnabled(3,false);
            _radioGroup.selectFirst();
            _radioGroup.showBackground = true;
            _radioGroup.padding = 10.f;
            _radioGroup.spacing = 4.f;
            _radioGroup.setPosition({C2,y}); tab2.addComponent(_radioGroup);
        }

        // ── SplitPanel setup — panels are positioned before children are added ──
        _split.setSize({SP_W, COL_H});
        _split.setPosition({SP_X, CONTENT_Y});
        _split.setDividerThick(DIV_T);
        _split.paneBg = sf::Color::Transparent;

        auto& leftPane  = _split.addPane(std::make_unique<ml::Panel>(), SP_PANE_W);
        auto& rightPane = _split.addPane(std::make_unique<ml::Panel>(), SP_PANE_W);

        _split.setPaneMinSize(0, 200.f);
        _split.setPaneMinSize(1, 200.f);
        _split.onDividerMoved([this](std::size_t, float pos){
            setStatus("Divider moved to " + std::to_string(static_cast<int>(pos)) + "px");
        });
        addComponent(_split);

        // ─────────────────────────────────────────────────────────────────────
        // LEFT PANE — INPUTS
        // ─────────────────────────────────────────────────────────────────────
        {
            float y = SP_CY;
            const float C3 = SP_L_CX;
            const float INPUT_W = SP_PANE_W - 28.f;

            heading(_inputHead,"TEXT INPUT", C3, y, leftPane); y+=22.f;

            _inputDefault.setSize({INPUT_W,34.f});
            _inputDefault.setPlaceholder("Default input...");
            _inputDefault.setPosition({C3,y}); leftPane.addComponent(_inputDefault); y+=46.f;

            _inputPassword.setSize({INPUT_W,34.f});
            _inputPassword.setPlaceholder("Password...");
            _inputPassword.setPasswordMode(true);
            _inputPassword.setPosition({C3,y}); leftPane.addComponent(_inputPassword); y+=46.f;

            _inputDisabled.setSize({INPUT_W,34.f});
            _inputDisabled.setValue("Disabled field");
            _inputDisabled.setEnabled(false);
            _inputDisabled.setPosition({C3,y}); leftPane.addComponent(_inputDisabled); y+=46.f;

            _inputError.setSize({INPUT_W,34.f});
            _inputError.setPlaceholder("Error state...");
            _inputError.setError(true);
            _inputError.setPosition({C3,y}); leftPane.addComponent(_inputError); y+=54.f;

            heading(_areaHead,"TEXT AREA", C3, y, leftPane); y+=22.f;
            _textArea.setSize({INPUT_W,130.f});
            _textArea.setPlaceholder("Multi-line text area...");
            _textArea.setPosition({C3,y}); leftPane.addComponent(_textArea); y+=146.f;

            heading(_selectHead,"SELECT", C3, y, leftPane); y+=22.f;
            _select.size = {INPUT_W, 34.f};
            _select.setPlaceholder("Choose difficulty...");
            { ml::SelectOptionStyle s; s.color=sf::Color(100,220,100); s.description="For beginners";
              _select.addOption("Easy","easy",s); }
            { ml::SelectOptionStyle s; s.description="Balanced";
              _select.addOption("Normal","normal",s); }
            { ml::SelectOptionStyle s; s.color=sf::Color(220,120,60); s.description="Experienced players";
              _select.addOption("Hard","hard",s); }
            { ml::SelectOptionStyle s; s.color=sf::Color(220,60,60); s.description="Permadeath";
              _select.addOption("Impossible","impossible",s); }
            _select.setOptionEnabled(3,false);
            _select.setPosition({C3,y}); leftPane.addComponent(_select);
        }

        // ─────────────────────────────────────────────────────────────────────
        // RIGHT PANE — LIST
        // ─────────────────────────────────────────────────────────────────────
        {
            float y = SP_CY;
            const float C4    = SP_R_CX;
            const float LIST_W = SP_PANE_W - 28.f;

            heading(_listHead,"LIST — ALL SLOTS", C4, y, rightPane); y+=22.f;

            icon(_iconWifi,  sf::Color(70,130,230));
            icon(_iconBt,    sf::Color(150,80,220));
            icon(_iconDark,  sf::Color(60,180,120));
            icon(_iconAlert, sf::Color(220,70,70));

            _toggleWifi.setOn(true);
            _toggleDark.setOn(true);
            _chkList.check();
            _btnListAction.setOffLabel("Details");
            _btnListAction.setOnLabel("Details");
            _btnListAction.setSize({80.f,28.f});

            _list.setWidth(LIST_W);
            _list.showBackground = true;
            _list.showDividers   = true;

            { auto& i=_list.addItem("Notifications");
              i.onClick([this]{ setStatus("Notifications clicked"); }); }
            { auto& i=_list.addItem("Account","Manage profile");
              i.onClick([this]{ setStatus("Account clicked"); }); }
            { auto& i=_list.addItem("Bluetooth");
              i.setStart(_iconBt); }
            { auto& i=_list.addItem("Wi-Fi");
              i.setEnd(_toggleWifi); }
            { auto& i=_list.addItem("Dark mode","Reduces eye strain");
              i.setStart(_iconDark); i.setEnd(_toggleDark); }
            { auto& i=_list.addItem("Alerts","Critical only");
              i.setStart(_iconAlert); i.setEnd(_btnListAction); }
            { auto& i=_list.addItem("Remember login");
              i.setStart(_chkList);
              i.onClick([this]{ _chkList.toggle(); }); }
            { auto& i=_list.addItem("Restricted","Requires subscription");
              i.setStart(_iconWifi); i.setEnabled(false); }

            _list.setPosition({C4, y});
            rightPane.addComponent(_list);
        }

        // ── Status bar ────────────────────────────────────────────────────────
        _status.setCharacterSize(11);
        _status.setFillColor(sf::Color(140,140,140));
        _status.setString("Interact with any component  |  Drag the divider between panes  |  Switch tabs at top-left");
        _status.setPosition({20.f, WIN_H - 20.f});
        addComponent(_status);

        // ─────────────────────────────────────────────────────────────────────
        // OVERLAY COMPONENTS — added LAST so they draw on top
        // ─────────────────────────────────────────────────────────────────────

        // ── MenuBar ───────────────────────────────────────────────────────────
        _menuBar.setPosition({0.f, 0.f});
        _menuBar.leftInset = 36.f; // leave room for the hamburger button
        _menuBar.addMenu("File", {
            ml::MenuItem::item("New",   [this]{ setStatus("New"); },   "Ctrl+N"),
            ml::MenuItem::item("Open",  [this]{ setStatus("Open"); },  "Ctrl+O"),
            ml::MenuItem::sep(),
            ml::MenuItem::item("Save",  [this]{ setStatus("Save"); },  "Ctrl+S"),
            ml::MenuItem::sep(),
            ml::MenuItem::item("Quit",  [this]{ setStatus("Quit"); },  "Ctrl+Q"),
        });
        _menuBar.addMenu("Edit", {
            ml::MenuItem::item("Undo",  [this]{ setStatus("Undo"); }, "Ctrl+Z"),
            ml::MenuItem::item("Redo",  [this]{ setStatus("Redo"); }, "Ctrl+Y"),
            ml::MenuItem::sep(),
            ml::MenuItem::sub("Insert", {
                ml::MenuItem::item("Table"),
                ml::MenuItem::item("Image"),
            }),
        });
        _menuBar.addMenu("Theme", {
            ml::MenuItem::item("Dark",  [this]{ ml::ThemeManager::apply<ShowcaseManifest>(Themes::Dark);  setStatus("Theme: Dark"); }),
            ml::MenuItem::item("Neon",  [this]{ ml::ThemeManager::apply<ShowcaseManifest>(Themes::Neon);  setStatus("Theme: Neon"); }),
            ml::MenuItem::item("Warm",  [this]{ ml::ThemeManager::apply<ShowcaseManifest>(Themes::Warm);  setStatus("Theme: Warm"); }),
            ml::MenuItem::item("Ocean", [this]{ ml::ThemeManager::apply<ShowcaseManifest>(Themes::Ocean); setStatus("Theme: Ocean"); }),
            ml::MenuItem::item("Light", [this]{ ml::ThemeManager::apply<ShowcaseManifest>(Themes::Light); setStatus("Theme: Light"); }),
        });
        _menuBar.addMenu("View", {
            ml::MenuItem::item("Zoom In",  [this]{ setStatus("Zoom In"); },  "Ctrl++"),
            ml::MenuItem::item("Zoom Out", [this]{ setStatus("Zoom Out"); }, "Ctrl+-"),
        });
        _menuBar.addMenu("Help", {
            ml::MenuItem::item("About", [this]{ setStatus("Malena Framework"); }),
        });
        // ── Toolbar ───────────────────────────────────────────────────────────
        _toolbar.setPosition({0.f, MENU_H});
        _toolbar.addButton("New",   [this]{ setStatus("New"); });
        _toolbar.addButton("Open",  [this]{ setStatus("Open"); });
        _toolbar.addButton("Save",  [this]{ setStatus("Save"); });
        _toolbar.addSeparator();
        _toolbar.addButton("Undo",  [this]{ setStatus("Undo"); });
        _toolbar.addButton("Redo",  [this]{ setStatus("Redo"); });
        _toolbar.addSeparator();
        _toolbar.addButton("Dark",  [this]{ ml::ThemeManager::apply<ShowcaseManifest>(Themes::Dark); });
        _toolbar.addButton("Neon",  [this]{ ml::ThemeManager::apply<ShowcaseManifest>(Themes::Neon); });
        _toolbar.addButton("Warm",  [this]{ ml::ThemeManager::apply<ShowcaseManifest>(Themes::Warm); });
        _toolbar.addButton("Ocean", [this]{ ml::ThemeManager::apply<ShowcaseManifest>(Themes::Ocean); });
        _toolbar.addButton("Light", [this]{ ml::ThemeManager::apply<ShowcaseManifest>(Themes::Light); });

        addComponent(_toolbar);
        addComponent(_menuBar); // registered after toolbar so dropdown draws on top

        // ── SideMenu ──────────────────────────────────────────────────────────
        _sideMenu.hamburgerSize = 22.f;
        _sideMenu.setHamburgerPosition({8.f, 4.f});
        {
            auto& list = _sideMenu.getList();
            list.addItem("Home",     "Dashboard");
            list.addItem("Projects", "Browse all");
            list.addItem("Settings", "Preferences");
            list.addItem("Profile",  "Your account");
            list.addItem("Help",     "Documentation");
        }
        _sideMenu.onOpen([this]{  setStatus("SideMenu opened"); });
        _sideMenu.onClose([this]{ setStatus("SideMenu closed"); });
        addComponent(_sideMenu);
    }

    void onReady() override
    {
        // Toggles
        _pillDefault.onToggled([&](bool on){ setStatus("Pill: "      +std::string(on?"ON":"OFF")); });
        _pillGreen.onToggled([&](bool on){   setStatus("Green pill: " +std::string(on?"ON":"OFF")); });
        _pillLarge.onToggled([&](bool on){   setStatus("Large pill: " +std::string(on?"YES":"NO")); });
        _pillInside.onToggled([&](bool on){  setStatus("Inside: "     +std::string(on?"ON":"OFF")); });
        _pillLocked.onToggled([&](bool on){  setStatus("Locked: "     +std::string(on?"ON":"OFF")); });
        _segDefault.onToggled([&](bool on){  setStatus("Segment: "    +std::string(on?"On":"Off")); });
        _segHotels.onToggled([&](bool on){   setStatus("View: "       +std::string(on?"Apartments":"Hotels")); });
        _btnSave.onToggled([&](bool on){ setStatus(on?"Saved!":"Save cleared"); });
        _btnMute.onToggled([&](bool on){ setStatus(on?"Muted":"Unmuted"); });

        // Checkboxes
        _chkOne.onSelected([&]{ setStatus("Fullscreen: on"); });
        _chkOne.onDeselected([&]{ setStatus("Fullscreen: off"); });
        _chkTwo.onSelected([&]{ setStatus("VSync: on"); });
        _chkTwo.onDeselected([&]{ setStatus("VSync: off"); });
        _checkGroup.onSelectionChanged([&](const std::vector<std::string>& v){
            std::string s="Group: ";
            for(std::size_t i=0;i<v.size();++i){if(i)s+=", ";s+=v[i];}
            if(v.empty())s="Group: none";
            setStatus(s);
        });
        _radioGroup.onSelectionChanged([&](const std::string& l, std::size_t){
            setStatus("Difficulty: "+l);
        });

        // Inputs
        _inputDefault.onChange([&](const std::string& v){ setStatus("Input: "+v); });
        _inputDefault.onSubmit([&](const std::string& v){ setStatus("Submitted: "+v); });
        _textArea.onChange([&](const std::string& v){
            setStatus("TextArea: "+std::to_string(v.size())+" chars"); });
        _select.onSelectionChanged([&](const std::string& v, std::size_t){
            setStatus("Selected: "+v); });

        // List
        _toggleWifi.onToggled([&](bool on){ setStatus("Wi-Fi: "     +std::string(on?"on":"off")); });
        _toggleDark.onToggled([&](bool on){ setStatus("Dark mode: " +std::string(on?"on":"off")); });
    }
};

int main()
{
    UIShowcaseApp app;
    app.run();
    return 0;
}

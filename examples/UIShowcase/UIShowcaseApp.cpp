// Copyright 2025 Dave R. Smith
// SPDX-License-Identifier: Apache-2.0
//
// UIShowcase — demonstrates every Malena UI component.
// Layout: MenuBar (30px) + Toolbar (44px) + 4 content columns.

#include <Malena/Engine/App/Application.h>
#include <Malena/Manifests/Manifest.h>
#include <Malena/Manifests/Theme.h>
#include <Malena/Resources/ThemeManager.h>
#include <Malena/Graphics/Controls/PillToggle.h>
#include <Malena/Graphics/Controls/SegmentToggle.h>
#include <Malena/Graphics/Controls/ButtonToggle.h>
#include <Malena/Graphics/Controls/ToggleGroup.h>
#include <Malena/Graphics/Controls/Checkbox.h>
#include <Malena/Graphics/Controls/CheckboxGroup.h>
#include <Malena/Graphics/Controls/RadioButton.h>
#include <Malena/Graphics/Controls/RadioGroup.h>
#include <Malena/Graphics/Text/TextInput.h>
#include <Malena/Graphics/Text/TextArea.h>
#include <Malena/Graphics/Controls/Select.h>
#include <Malena/Graphics/Controls/List.h>
#include <Malena/Graphics/Controls/ListItem.h>
#include <Malena/Graphics/Primitives/Rectangle.h>
#include <Malena/Graphics/Text/Text.h>
#include <Malena/Graphics/Controls/MenuBar.h>
#include <Malena/Graphics/Controls/Toolbar.h>
#include <Malena/Graphics/Controls/SideMenu.h>

// ── Custom themes ─────────────────────────────────────────────────────────────

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

// ── Manifest ──────────────────────────────────────────────────────────────────

class ShowcaseManifest : public ml::Manifest
{
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

    // Layout constants
    static constexpr float MENU_H    = 30.f;
    static constexpr float TOOLBARH = 44.f;
    static constexpr float CONTENT_Y = MENU_H + TOOLBARH + 8.f;  // where columns start
    static constexpr float WIN_W     = 1680.f;
    static constexpr float WIN_H     = 860.f;
    static constexpr float COL_H     = WIN_H - CONTENT_Y - 30.f;  // available column height

    // Column X positions
    static constexpr float C1 = 60.f;   // Toggles
    static constexpr float C2 = 460.f;  // Selections
    static constexpr float C3 = 850.f;  // Inputs
    static constexpr float C4 = 1240.f; // List
    static constexpr float LW = 410.f;  // List width

    // ── Background ────────────────────────────────────────────────────────────
    ml::Rectangle _bg;

    // ── Column dividers ───────────────────────────────────────────────────────
    ml::Rectangle _div1, _div2, _div3;

    // ── Section headings ──────────────────────────────────────────────────────
    ml::Text _headToggles, _headSelections, _headInputs, _headList;

    // ── COL 1 — TOGGLES ──────────────────────────────────────────────────────
    ml::Text          _pillHead, _segHead, _btnHead;
    ml::PillToggle    _pillDefault, _pillGreen, _pillLarge,
                      _pillInside, _pillDisabled, _pillLocked;
    ml::SegmentToggle _segDefault, _segHotels;
    ml::ButtonToggle  _btnSave, _btnMute, _btnBtnDisabled;

    // ── COL 2 — SELECTIONS ───────────────────────────────────────────────────
    ml::Text          _checkHead, _checkGroupHead, _radioHead;
    ml::Checkbox      _chkOne, _chkTwo, _chkThree, _chkDisabled;
    ml::CheckboxGroup _checkGroup;
    ml::RadioGroup    _radioGroup;

    // ── COL 3 — INPUTS ───────────────────────────────────────────────────────
    ml::Text      _inputHead, _areaHead, _selectHead;
    ml::TextInput _inputDefault, _inputPassword, _inputDisabled, _inputError;
    ml::TextArea  _textArea;
    ml::Select    _select;

    // ── COL 4 — LIST ─────────────────────────────────────────────────────────
    ml::Text      _listHead;
    ml::List      _list;
    ml::Rectangle _iconWifi, _iconBt, _iconDark, _iconNote, _iconAlert;
    ml::PillToggle _toggleWifi, _toggleDark;
    ml::Checkbox  _chkList;
    ml::ButtonToggle _btnListAction;

    // ── Status bar ────────────────────────────────────────────────────────────
    ml::Text _status;

    // ── Overlay components — added LAST ───────────────────────────────────────
    ml::MenuBar  _menuBar;
    ml::Toolbar  _toolbar;
    ml::SideMenu _sideMenu;

    bool _themeChanging = false;

    // ── Helpers ───────────────────────────────────────────────────────────────

    void setStatus(const std::string& s) { _status.setString(s); }

    void heading(ml::Text& t, const std::string& s, float x, float y)
    {
        t.setString(s);
        t.setCharacterSize(11);
        t.setFillColor(sf::Color(160,160,160));
        t.setStyle(sf::Text::Style::Bold);
        t.setPosition({x, y});
        addComponent(t);
    }

    void icon(ml::Rectangle& r, const sf::Color& c)
    {
        r.setSize({22.f, 22.f}); r.setRadius(4.f); r.setFillColor(c);
    }

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
        _bg.setSize({WIN_W - 20.f, WIN_H - 20.f});
        _bg.setFillColor(sf::Color(18,18,24));
        _bg.setPosition({10.f, 10.f});
        addComponent(_bg);

        // ── Column dividers ───────────────────────────────────────────────────
        for (auto [div, x] : std::initializer_list<std::pair<ml::Rectangle*, float>>{
            {&_div1, C2-10.f}, {&_div2, C3-10.f}, {&_div3, C4-10.f}})
        {
            div->setSize({1.f, COL_H});
            div->setFillColor(sf::Color(60,60,60));
            div->setPosition({x, CONTENT_Y});
            addComponent(*div);
        }

        // ── Section headings ──────────────────────────────────────────────────
        heading(_headToggles,    "TOGGLES",    C1,  CONTENT_Y);
        heading(_headSelections, "SELECTIONS", C2,  CONTENT_Y);
        heading(_headInputs,     "INPUTS",     C3,  CONTENT_Y);
        heading(_headList,       "LIST",       C4,  CONTENT_Y);

        // ─────────────────────────────────────────────────────────────────────
        // COL 1 — TOGGLES
        // ─────────────────────────────────────────────────────────────────────
        float y = CONTENT_Y + 20.f;

        heading(_pillHead, "PILL", C1, y); y += 22.f;

        _pillDefault.setRightLabel("Default");
        _pillDefault.setPosition({C1,y}); addComponent(_pillDefault); y+=32.f;

        _pillGreen.setTrackOnColor(sf::Color(70,200,100));
        _pillGreen.lockTheme();
        _pillGreen.setRightLabel("Custom green");
        _pillGreen.setPosition({C1,y}); addComponent(_pillGreen); y+=32.f;

        _pillLarge.setTrackSize({70.f,34.f});
        _pillLarge.setPillLabels("NO","YES");
        _pillLarge.setOn(true);
        _pillLarge.setRightLabel("Large / ON");
        _pillLarge.setPosition({C1,y}); addComponent(_pillLarge); y+=44.f;

        _pillInside.setPillLabels("OFF","ON");
        _pillInside.setTrackSize({64.f,28.f});
        _pillInside.setRightLabel("Inside labels");
        _pillInside.setPosition({C1,y}); addComponent(_pillInside); y+=32.f;

        _pillDisabled.setRightLabel("Disabled");
        _pillDisabled.setEnabled(false);
        _pillDisabled.setPosition({C1,y}); addComponent(_pillDisabled); y+=32.f;

        _pillLocked.setTrackOnColor(sf::Color(220,60,60));
        _pillLocked.setTrackOffColor(sf::Color(70,70,70));
        _pillLocked.lockTheme();
        _pillLocked.setRightLabel("Locked (red)");
        _pillLocked.setPosition({C1,y}); addComponent(_pillLocked); y+=42.f;

        heading(_segHead, "SEGMENT", C1, y); y+=22.f;
        _segDefault.setSegmentLabels("Off","On");
        _segDefault.setSize({180.f,34.f});
        _segDefault.setPosition({C1,y}); addComponent(_segDefault); y+=46.f;

        _segHotels.setSegmentLabels("Hotels","Apartments");
        _segHotels.setSize({200.f,34.f});
        _segHotels.setPosition({C1,y}); addComponent(_segHotels); y+=50.f;

        heading(_btnHead, "BUTTON TOGGLE", C1, y); y+=22.f;
        _btnSave.setOffLabel("Save"); _btnSave.setOnLabel("Saved");
        _btnSave.setSize({110.f,34.f});
        _btnSave.setPosition({C1,y}); addComponent(_btnSave); y+=46.f;

        _btnMute.setOffLabel("Unmuted"); _btnMute.setOnLabel("Muted");
        _btnMute.setOnColor(sf::Color(200,60,60));
        _btnMute.setSize({110.f,34.f});
        _btnMute.setPosition({C1,y}); addComponent(_btnMute); y+=46.f;

        _btnBtnDisabled.setOffLabel("Disabled");
        _btnBtnDisabled.setSize({110.f,34.f});
        _btnBtnDisabled.setEnabled(false);
        _btnBtnDisabled.setPosition({C1,y}); addComponent(_btnBtnDisabled);

        // ─────────────────────────────────────────────────────────────────────
        // COL 2 — SELECTIONS
        // ─────────────────────────────────────────────────────────────────────
        y = CONTENT_Y + 20.f;
        heading(_checkHead, "CHECKBOX", C2, y); y+=22.f;

        auto addCb = [&](ml::Checkbox& cb, const char* lbl, bool chk, bool dis){
            cb.setLabel(lbl);
            if (chk) cb.check();
            if (dis) cb.setEnabled(false);
            cb.setPosition({C2,y}); addComponent(cb); y+=30.f;
        };
        addCb(_chkOne,     "Enable fullscreen",  false, false);
        addCb(_chkTwo,     "Enable VSync",        false, false);
        addCb(_chkThree,   "Show FPS counter",    true,  false);
        addCb(_chkDisabled,"Disabled option",     false, true);
        y+=16.f;

        heading(_checkGroupHead,"CHECKBOX GROUP", C2, y); y+=22.f;
        _checkGroup.addOption("Dark mode",true);
        _checkGroup.addOption("Subtitles");
        _checkGroup.addOption("Auto-save",true);
        _checkGroup.addOption("Analytics");
        _checkGroup.showBackground = true;
        _checkGroup.padding = 10.f;
        _checkGroup.spacing = 4.f;
        _checkGroup.setPosition({C2,y}); addComponent(_checkGroup); y+=180.f;

        heading(_radioHead,"RADIO GROUP", C2, y); y+=22.f;
        _radioGroup.addOption("Easy");
        _radioGroup.addOption("Normal");
        _radioGroup.addOption("Hard");
        _radioGroup.addOption("Impossible");
        _radioGroup.setOptionEnabled(3,false);
        _radioGroup.selectFirst();
        _radioGroup.showBackground = true;
        _radioGroup.padding = 10.f;
        _radioGroup.spacing = 4.f;
        _radioGroup.setPosition({C2,y}); addComponent(_radioGroup);

        // ─────────────────────────────────────────────────────────────────────
        // COL 3 — INPUTS
        // ─────────────────────────────────────────────────────────────────────
        y = CONTENT_Y + 20.f;
        heading(_inputHead, "TEXT INPUT", C3, y); y+=22.f;

        auto addInput = [&](ml::TextInput& inp, const char* ph, bool pass, bool dis, bool err){
            inp.setSize({350.f,34.f});
            inp.setPlaceholder(ph);
            if (pass) inp.setPasswordMode(true);
            if (dis)  { inp.setValue("Disabled field"); inp.setEnabled(false); }
            if (err)  inp.setError(true);
            inp.setPosition({C3,y}); addComponent(inp); y+=46.f;
        };
        addInput(_inputDefault,  "Default input...",  false, false, false);
        addInput(_inputPassword, "Password...",        true,  false, false);
        addInput(_inputDisabled, "",                   false, true,  false);
        addInput(_inputError,    "Error state...",     false, false, true);

        heading(_areaHead,"TEXT AREA", C3, y); y+=22.f;
        _textArea.setSize({350.f,110.f});
        _textArea.setPlaceholder("Multi-line text area...");
        _textArea.setPosition({C3,y}); addComponent(_textArea); y+=126.f;

        heading(_selectHead,"SELECT", C3, y); y+=22.f;
        _select.size = {350.f,34.f};
        _select.setPlaceholder("Choose difficulty...");
        { ml::SelectOptionStyle s; s.color=sf::Color(100,220,100);
          s.description="For beginners";
          _select.addOption("Easy","easy",s); }
        { ml::SelectOptionStyle s; s.description="Balanced";
          _select.addOption("Normal","normal",s); }
        { ml::SelectOptionStyle s; s.color=sf::Color(220,120,60);
          s.description="Experienced players";
          _select.addOption("Hard","hard",s); }
        { ml::SelectOptionStyle s; s.color=sf::Color(220,60,60);
          s.description="Permadeath";
          _select.addOption("Impossible","impossible",s); }
        _select.setOptionEnabled(3,false);
        _select.setPosition({C3,y}); addComponent(_select);

        // ─────────────────────────────────────────────────────────────────────
        // COL 4 — LIST
        // ─────────────────────────────────────────────────────────────────────
        y = CONTENT_Y + 20.f;
        heading(_listHead,"LIST - ALL SLOTS", C4, y); y+=22.f;

        icon(_iconWifi,  sf::Color(70,130,230));
        icon(_iconBt,    sf::Color(150,80,220));
        icon(_iconDark,  sf::Color(60,180,120));
        icon(_iconNote,  sf::Color(220,160,40));
        icon(_iconAlert, sf::Color(220,70,70));

        _toggleWifi.setOn(true);
        _toggleDark.setOn(true);
        _chkList.check();
        _btnListAction.setOffLabel("Details");
        _btnListAction.setOnLabel("Details");
        _btnListAction.setSize({80.f,28.f});

        _list.setWidth(LW);
        _list.showBackground = true;
        _list.showDividers   = true;

        { auto& i=_list.addItem("Notifications");
          i.onClick([this]{ setStatus("Notifications clicked"); }); }
        { auto& i=_list.addItem("Account","Manage profile and preferences");
          i.onClick([this]{ setStatus("Account clicked"); }); }
        { auto& i=_list.addItem("Bluetooth");
          i.setStart(_iconBt); }
        { auto& i=_list.addItem("Wi-Fi");
          i.setEnd(_toggleWifi); }
        { auto& i=_list.addItem("Dark mode","Reduces eye strain");
          i.setStart(_iconDark); i.setEnd(_toggleDark); }
        { auto& i=_list.addItem("Alerts","Critical notifications only");
          i.setStart(_iconAlert); i.setEnd(_btnListAction); }
        { auto& i=_list.addItem("Remember login");
          i.setStart(_chkList);
          i.onClick([this]{ _chkList.toggle(); }); }
        { auto& i=_list.addItem("Restricted","Requires subscription");
          i.setStart(_iconWifi); i.setEnabled(false); }

        _list.setPosition({C4, y});
        addComponent(_list);

        // ── Status bar ────────────────────────────────────────────────────────
        _status.setCharacterSize(11);
        _status.setFillColor(sf::Color(140,140,140));
        _status.setString("Interact with any component — overlay components (MenuBar/Toolbar/SideMenu) added last");
        _status.setPosition({20.f, WIN_H - 22.f});
        addComponent(_status);

        // ─────────────────────────────────────────────────────────────────────
        // OVERLAY COMPONENTS — must be added LAST to draw on top
        // ─────────────────────────────────────────────────────────────────────

        // ── MenuBar (y=0, fills window width) ─────────────────────────────────
        _menuBar.setPosition({0.f, 0.f});
        _menuBar.addMenu("File", {
            ml::MenuItem::item("New",   [this]{ setStatus("File > New"); },   "Ctrl+N"),
            ml::MenuItem::item("Open",  [this]{ setStatus("File > Open"); },  "Ctrl+O"),
            ml::MenuItem::sep(),
            ml::MenuItem::item("Save",  [this]{ setStatus("File > Save"); },  "Ctrl+S"),
            ml::MenuItem::item("Export",[this]{ setStatus("File > Export"); }),
            ml::MenuItem::sep(),
            ml::MenuItem::item("Quit",  [this]{ setStatus("Quit!"); },        "Ctrl+Q"),
        });
        _menuBar.addMenu("Edit", {
            ml::MenuItem::item("Undo",  [this]{ setStatus("Undo"); }, "Ctrl+Z"),
            ml::MenuItem::item("Redo",  [this]{ setStatus("Redo"); }, "Ctrl+Y"),
            ml::MenuItem::sep(),
            ml::MenuItem::item("Cut",   [this]{ setStatus("Cut"); },   "Ctrl+X"),
            ml::MenuItem::item("Copy",  [this]{ setStatus("Copy"); },  "Ctrl+C"),
            ml::MenuItem::item("Paste", [this]{ setStatus("Paste"); }, "Ctrl+V"),
            ml::MenuItem::sep(),
            ml::MenuItem::sub("Insert", {
                ml::MenuItem::item("Table"),
                ml::MenuItem::item("Image"),
                ml::MenuItem::item("Code block"),
            }),
        });
        _menuBar.addMenu("View", {
            ml::MenuItem::item("Zoom In",  [this]{ setStatus("Zoom In"); },  "Ctrl++"),
            ml::MenuItem::item("Zoom Out", [this]{ setStatus("Zoom Out"); }, "Ctrl+-"),
            ml::MenuItem::sep(),
            ml::MenuItem::disabled("Full Screen  (coming soon)"),
        });
        _menuBar.addMenu("Theme", {
            ml::MenuItem::item("Dark",  [this]{ ml::ThemeManager::apply<ShowcaseManifest>(Themes::Dark);  setStatus("Theme: Dark"); }),
            ml::MenuItem::item("Neon",  [this]{ ml::ThemeManager::apply<ShowcaseManifest>(Themes::Neon);  setStatus("Theme: Neon"); }),
            ml::MenuItem::item("Warm",  [this]{ ml::ThemeManager::apply<ShowcaseManifest>(Themes::Warm);  setStatus("Theme: Warm"); }),
            ml::MenuItem::item("Ocean", [this]{ ml::ThemeManager::apply<ShowcaseManifest>(Themes::Ocean); setStatus("Theme: Ocean"); }),
            ml::MenuItem::item("Light", [this]{ ml::ThemeManager::apply<ShowcaseManifest>(Themes::Light); setStatus("Theme: Light"); }),
        });
        _menuBar.addMenu("Help", {
            ml::MenuItem::item("Documentation",[this]{ setStatus("Docs"); }),
            ml::MenuItem::item("About Malena", [this]{ setStatus("Malena Framework"); }),
        });


        // ── Toolbar (y = MENU_H) ──────────────────────────────────────────────
        _toolbar.setPosition({0.f, MENU_H});
        _toolbar.addButton("New",   [this]{ setStatus("Toolbar: New"); });
        _toolbar.addButton("Open",  [this]{ setStatus("Toolbar: Open"); });
        _toolbar.addButton("Save",  [this]{ setStatus("Toolbar: Save"); });
        _toolbar.addSeparator();
        _toolbar.addButton("Undo",  [this]{ setStatus("Toolbar: Undo"); });
        _toolbar.addButton("Redo",  [this]{ setStatus("Toolbar: Redo"); });
        _toolbar.addSeparator();
        _toolbar.addButton("Cut",   [this]{ setStatus("Toolbar: Cut"); });
        _toolbar.addButton("Copy",  [this]{ setStatus("Toolbar: Copy"); });
        _toolbar.addButton("Paste", [this]{ setStatus("Toolbar: Paste"); });
        _toolbar.addSeparator();
        _toolbar.addButton("Dark",  [this]{ ml::ThemeManager::apply<ShowcaseManifest>(Themes::Dark);  });
        _toolbar.addButton("Neon",  [this]{ ml::ThemeManager::apply<ShowcaseManifest>(Themes::Neon);  });
        _toolbar.addButton("Warm",  [this]{ ml::ThemeManager::apply<ShowcaseManifest>(Themes::Warm);  });
        _toolbar.addButton("Ocean", [this]{ ml::ThemeManager::apply<ShowcaseManifest>(Themes::Ocean); });
        _toolbar.addButton("Light", [this]{ ml::ThemeManager::apply<ShowcaseManifest>(Themes::Light); });
        addComponent(_toolbar);
    	addComponent(_menuBar);
        // ── SideMenu (hamburger top-left, 60px down to clear MenuBar+Toolbar) ─
        _sideMenu.setHamburgerPosition({8.f, MENU_H + TOOLBARH * 0.5f - 24.f});
        {
            auto& list = _sideMenu.getList();
            list.addItem("Home",     "Dashboard");
            list.addItem("Projects", "Browse all projects");
            list.addItem("Settings", "Preferences and config");
            list.addItem("Profile",  "Your account");
            list.addItem("Help",     "Documentation");
            addComponent(list);
        }
        _sideMenu.onOpen([this]{  setStatus("SideMenu opened"); });
        _sideMenu.onClose([this]{ setStatus("SideMenu closed"); });
        addComponent(_sideMenu);
    }

    void onReady() override
    {
        _pillDefault.onToggled([&](bool on){ setStatus(std::string("Pill: ")+(on?"ON":"OFF")); });
        _pillGreen.onToggled([&](bool on){   setStatus(std::string("Green pill: ")+(on?"ON":"OFF")); });
        _pillLarge.onToggled([&](bool on){   setStatus(std::string("Large pill: ")+(on?"YES":"NO")); });
        _pillInside.onToggled([&](bool on){  setStatus(std::string("Inside: ")+(on?"ON":"OFF")); });
        _pillLocked.onToggled([&](bool on){  setStatus(std::string("Locked: ")+(on?"ON":"OFF")); });
        _segDefault.onToggled([&](bool on){  setStatus(std::string("Seg: ")+(on?"On":"Off")); });
        _segHotels.onToggled([&](bool on){   setStatus(std::string("View: ")+(on?"Apartments":"Hotels")); });
        _btnSave.onToggled([&](bool on){ setStatus(on?"Saved!":"Save cleared"); });
        _btnMute.onToggled([&](bool on){ setStatus(on?"Muted":"Unmuted"); });

        _chkOne.onSelected([&]{   setStatus("Fullscreen: on"); });
        _chkOne.onDeselected([&]{ setStatus("Fullscreen: off"); });
        _chkTwo.onSelected([&]{   setStatus("VSync: on"); });
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
        _inputDefault.onChange([&](const std::string& v){ setStatus("Input: "+v); });
        _inputDefault.onSubmit([&](const std::string& v){ setStatus("Submitted: "+v); });
        _textArea.onChange([&](const std::string& v){
            setStatus("TextArea: "+std::to_string(v.size())+" chars");
        });
        _select.onSelectionChanged([&](const std::string& v, std::size_t){
            setStatus("Selected: "+v);
        });
        _toggleWifi.onToggled([&](bool on){ setStatus(std::string("Wi-Fi: ")+(on?"on":"off")); });
        _toggleDark.onToggled([&](bool on){ setStatus(std::string("Dark mode: ")+(on?"on":"off")); });
    }
};

int main()
{
    UIShowcaseApp app;
    app.run();
    return 0;
}

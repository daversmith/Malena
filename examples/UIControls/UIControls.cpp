//
// Created by Dave Smith on 3/27/26.
//

#include "UIControls.h"

#include "Malena/Graphics/Controls/ListItem.h"
UIControls::UIControls() :
	ml::ApplicationWith<UIControlsManifest>(1080, 420 , 32, "UI Controls Test"), pane(300, 200)
{
	setupLists();
	setupScrollPane();
	setupRadio();
	setupCheckboxes();
	setupTextInputs();
	setupTextAreas();
	setupSelect();
	setupToggles();

	layout();
}
void UIControls::setupScrollPane()
{
	pane.addComponent(list);
	pane.addComponent(list2);
	addComponent(pane);
}
void UIControls::setupLists()
{


	auto a = new ml::ListItem("This is a test");
	auto b = new ml::ListItem("This is a test");
	auto c = new ml::ListItem("This is a test");
	// todo: figure out wht set size needs to be called inorder for things to position correctly
	a->setSize({500, 50});
	b->setSize({500, 50});
	c->setSize({500, 50});
	list.addItem(a);
	list.addItem(b);
	list.addItem(c);

	auto x = new ml::ListItem("This is a test");
	auto y = new ml::ListItem("This is a test");
	auto z = new ml::ListItem("This is a test");
	x->setSize({500, 50});
	y->setSize({500, 50});
	z->setSize({500, 50});
	list2.addItem(x);
	list2.addItem(y);
	list2.addItem(z);
	list.addItem(&list2);
}
void UIControls::setupRadio()
{
	// ── Simple usage — group creates buttons internally ────────────────
	_difficulty.addOption("Easy");
	_difficulty.addOption("Medium");
	_difficulty.addOption("Hard");
	_difficulty.setPosition({100.f, 100.f});
	_difficulty.selectFirst();

	_difficulty.onSelectionChanged([](const std::string &label, std::size_t index) {
		std::cout << "Difficulty: " << label << " (index " << index << ")\n";
	});

	// ── Styling ───────────────────────────────────────────────────────
	_difficulty.setDotColor(sf::Color(70, 130, 230));
	_difficulty.setLabelColor(sf::Color::White);
	_difficulty.setSpacing(24.f);

	// ── Optional background ───────────────────────────────────────────
	_difficulty.showBackground(true);
	_difficulty.setBackgroundColor(sf::Color(40, 40, 40, 220));
	_difficulty.setBackgroundRadius(8.f);
	_difficulty.setPadding(12.f);

	addComponent(_difficulty);

	// ── Custom buttons — user creates them ────────────────────────────
	_yes.setLabel("Yes");
	_no.setLabel("No");

	_confirm.add(_yes);
	_confirm.add(_no);
	_confirm.setPosition({400.f, 100.f});
	_confirm.selectFirst();

	_confirm.onSelectionChanged(
			[](const std::string &label, std::size_t) { std::cout << "Confirm: " << label << "\n"; });

	_confirm.setBelow(pane);
	_difficulty.setRightOf(pane);
	addComponent(_confirm);
}
void UIControls::setupCheckboxes()
{
	// ── Simple usage — group creates checkboxes internally ────────────
	_settings.addOption("Fullscreen");
	_settings.addOption("VSync",     true);   // pre-checked
	_settings.addOption("Show FPS");
	_settings.addOption("Subtitles", true);   // pre-checked
	_settings.setPosition({100.f, 100.f});

	_settings.onSelectionChanged([](const std::vector<std::string>& checked){
		std::cout << "Active settings:\n";
		for (const auto& label : checked)
			std::cout << "  - " << label << "\n";
	});

	// ── Optional background ───────────────────────────────────────────
	_settings.showBackground(true);
	_settings.setBackgroundColor(sf::Color(40, 40, 40, 220));
	_settings.setBackgroundRadius(8.f);
	_settings.setPadding(12.f);

	addComponent(_settings);

	// ── Programmatic control ──────────────────────────────────────────
	_settings.checkAll();
	_settings.uncheckAll();
	_settings.check(1);      // check "VSync" by index

	// ── Custom checkboxes ─────────────────────────────────────────────
	_audio.setLabel("Music");
	_sfx.setLabel("Sound Effects");

	_sound.add(_audio);
	_sound.add(_sfx);
	_sound.setBoxCheckedColor(sf::Color(100, 200, 100));

	_sound.onSelectionChanged([](const std::vector<std::string>& checked){
		std::cout << "Sound options: ";
		for (const auto& l : checked) std::cout << l << " ";
		std::cout << "\n";
	});
	_settings.setRightOf(_difficulty);
	_sound.setRightOf(_confirm);

	addComponent(_sound);

	_testCheck.setLabel("Test");
	_testCheck.setPosition({100.f, 400.f});
	addComponent(_testCheck);
	_testCheck.setBelow(_confirm);
}
void UIControls::setupTextInputs()
{
	// ── Single-line TextInput ─────────────────────────────────────────────
        _nameField.setSize({350.f, 38.f});
        _nameField.setPlaceholder("Enter your name...");

        _nameField.onChange([](const std::string& value){
            std::cout << "name: " << value << "\n";
        });

        _nameField.onSubmit([](const std::string& value){
            std::cout << "submitted: " << value << "\n";
        });

        addComponent(_nameField);
        //
        // // ── Pre-populate with styled text ─────────────────────────────────────
        _nameField.setValue("Hello Malena");

        // Style the word "Malena" — characters 6 to 12
        _nameField.setSelection(6, 12);
        _nameField.setSelectionColor(sf::Color(70, 200, 130));
        _nameField.setSelectionBold(true);
        _nameField.setSelectionCharSize(20);

        // ── Password field ────────────────────────────────────────────────────
        _password.setSize({350.f, 38.f});
        _password.setPlaceholder("Password...");
        _password.setPasswordMode(true);
        _password.setMaxLength(32);

        addComponent(_password);

        // ── Disabled field ────────────────────────────────────────────────────
        _readOnly.setSize({350.f, 38.f});
        _readOnly.setValue("This field is read-only");
        _readOnly.setReadOnly(true);

        addComponent(_readOnly);

        // ── Error state ───────────────────────────────────────────────────────
        _email.setSize({350.f, 38.f});
        _email.setPlaceholder("Email address...");

        _email.onChange([this](const std::string& value){
            // Simple email validation
            const bool valid = value.find('@') != std::string::npos;
            _email.setError(!value.empty() && !valid);
        });

        addComponent(_email);


}
void UIControls::setupTextAreas()
{
	// ── Multi-line TextArea ───────────────────────────────────────────────
	_bio.setSize({500.f, 160.f});
	_bio.setBelow(_email);
	_bio.setPlaceholder("Tell us about yourself...\n(Ctrl+Enter to submit)");

	_bio.onSubmit([](const std::string& value){
		std::cout << "bio submitted:\n" << value << "\n";
	});

	_bio.onChange([](const std::string& value){
		std::cout << "bio length: " << value.size() << "\n";
	});

	// Scrollbar styling
	_bio.setScrollBarColor(sf::Color(70, 130, 230));
	_bio.setScrollBarWidth(6.f);

	// _bio.setBelow(_testCheck);

	addComponent(_bio);

	// ── Pre-populate TextArea with mixed styling ───────────────────────────
	_bio.setValue("Malena is a C++ UI framework.\n"
				  "It builds on top of SFML 3.");

	// Make "Malena" bold and colored
	_bio.setSelection(0, 6);
	_bio.setSelectionBold(true);
	_bio.setSelectionColor(sf::Color(70, 200, 130));

	// Make "SFML 3" a different size
	_bio.setSelection(47, 53);
	_bio.setSelectionCharSize(20);
	_bio.setSelectionColor(sf::Color(70, 130, 230));
}
void UIControls::setupSelect()
{
	difficulty.setSize({240.f, 36.f});
	difficulty.setPlaceholder("Choose difficulty...");
	difficulty.setItemHeight(52.f);  // taller rows for descriptions

	// Plain option
	difficulty.addOption("Easy", "easy");

	// Rich option — colored label + description
	ml::SelectOptionStyle normalStyle;
	normalStyle.color       = sf::Color(100, 200, 100);
	normalStyle.description = "Balanced challenge";
	difficulty.addOption("Normal", "normal", normalStyle);

	// Rich option — icon + colored label + description
	ml::SelectOptionStyle hardStyle;
	hardStyle.color       = sf::Color(220, 80, 80);
	hardStyle.bold        = true;
	hardStyle.description = "For experienced players";
	hardStyle.iconTexture = &get(Images::SKULL);
	hardStyle.iconSize    = {24.f, 24.f};
	difficulty.addOption("Hard", "hard", hardStyle);

	// Disabled option
	difficulty.addOption("Nightmare", "nightmare");
	difficulty.setOptionEnabled(3, false);

	difficulty.onSelectionChanged([](const std::string& value, std::size_t){
		std::cout << "Difficulty: " << value << "\n";
	});

	addComponent(difficulty);
}
void UIControls::layout()
{
	pane.setPosition({0, 0});
	_nameField.setRightOf(pane, PADDING);
	_password.setBelow(_nameField);
	_readOnly.setBelow(_password);
	_email.setBelow(_readOnly);

	_difficulty.setRightOf(_nameField, PADDING);
	_confirm.setBelow(_difficulty, PADDING);

	_sound.setRightOf(_difficulty, PADDING);
	_settings.setRightOf(_sound, PADDING);
	difficulty.setBelow(_email);
	_bio.setBelow(_confirm);

	settings.setRightOf(_testCheck);
	wifi.setRightOf(settings);
	sound.setBelow(wifi);
	terms.setBelow(sound);
}
void UIControls::setupToggles()
{
	// PILL with right label
	wifi.setStyle(ml::Toggle::Style::PILL);
	wifi.setRightLabel("Wi-Fi");
	wifi.setLeftLabel("Network:");  // optional left label
	wifi.onToggled([](bool on){ });
	addComponent(wifi);

	// CHECKBOX style
	terms.setStyle(ml::Toggle::Style::CHECKBOX);
	terms.setRightLabel("Accept terms");
	terms.setBoxSize({18.f, 18.f});
	addComponent(terms);

	// BUTTON style
	sound.setStyle(ml::Toggle::Style::BUTTON);
	sound.setButtonOnLabel("Sound ON");
	sound.setButtonOffLabel("Sound OFF");
	sound.setSize({120.f, 36.f});
	addComponent(sound);

	// Group
	settings.setDefaultStyle(ml::Toggle::Style::PILL);
	settings.addToggle("Dark mode",  "dark_mode");
	settings.addToggle("Subtitles",  "subtitles");
	settings.addToggle("Fullscreen", "fullscreen", true); // starts on
	settings.onAnyToggled([](const std::string& key, bool on){ });

	// Mix styles in a group
	custom.setStyle(ml::Toggle::Style::BUTTON);
	settings.add(custom, "custom_key");

	addComponent(settings);
}

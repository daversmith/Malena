//
// Created by Dave Smith on 3/27/26.
//

#ifndef UICONTROLS_H
#define UICONTROLS_H


#include <Malena/Engine/App/Application.h>

#include <Malena/Graphics/Controls/List.h>
#include <Malena/Graphics/Controls/ScrollPane.h>

#include <Malena/Graphics/Controls/Checkbox.h>
#include <Malena/Graphics/Controls/CheckboxGroup.h>
#include <Malena/Graphics/Controls/RadioGroup.h>
#include "Malena/Graphics/Controls/RadioButton.h"

#include "Malena/Graphics/Text/TextInput.h"
#include "Malena/Graphics/Text/TextArea.h"
#include <Malena/Graphics/Controls/Select.h>
#include <Malena/Graphics/Controls/Toggle.h>
class UIControlsManifest : public ml::Manifest
{
public:
	enum class Images {SKULL};
	inline static const float PADDING = 10.f;
private:
	inline static const auto _ = []() {
		set(Images::SKULL, "assets/images/skull.png");
		return 0;
	}();


};
class UIControls : public ml::ApplicationWith<UIControlsManifest>
{
private:
	ml::ScrollPane pane;
	ml::List list, list2;
	ml::RadioGroup _difficulty;
	ml::RadioGroup _confirm;
	ml::RadioButton _yes, _no;
	ml::CheckboxGroup _settings;
	ml::CheckboxGroup _sound;
	ml::Checkbox      _audio, _sfx;
	ml::Checkbox _testCheck;
	ml::TextInput _nameField;
	ml::TextInput _password;
	ml::TextInput _readOnly;
	ml::TextInput _email;
	ml::TextArea  _bio;
	ml::Select difficulty;
	ml::Toggle wifi, terms, sound,custom;
	ml::ToggleGroup settings;
	void setupScrollPane();
	void setupLists();
	void setupRadio();
	void setupCheckboxes();
	void setupTextInputs();
	void setupTextAreas();
	void setupSelect();
	void setupToggles();
	void layout();
public:
	UIControls();

};



#endif //UICONTROLS_H

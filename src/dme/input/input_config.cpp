#include <dme/input/input_config.h>
namespace dme::input {

	const InputSettingType InputSettingType::NoneType = InputSettingType("None");
	InputSettingType::InputSettingType(const stdstr& type) : type(type) {
	}

	InputSetting::InputSetting(InputKey key, InputAction inputAction, stdstr name, const InputSettingType* type) : key(key), inputAction(inputAction), name(name), type(type), index(static_cast<uint>(inputAction)) {
		if (type == null) {
			type = &InputSettingType::NoneType;
		}
	}
	const stdstr& InputSetting::getName() const {
		return name;
	}
	const stdstr& InputSetting::getType() const {
		return type->type;
	}
	InputKey InputSetting::getKey() const {
		return key;
	}
	InputAction InputSetting::getInputAction() const {
		return inputAction;
	}
	uint InputSetting::getIndex() const {
		return index;
	}

	//输入配置类型
	static const InputSettingType Normal = InputSettingType("Normal");
	static const InputSettingType GameSystem = InputSettingType("Game System");
	static const InputSettingType Movement = InputSettingType("Movement");
	static const InputSettingType Inventory = InputSettingType("Inventory");
	static const InputSettingType Action = InputSettingType("Action");
	static const InputSettingType Test = InputSettingType("Test");

	DynArr<InputSetting> InputConfig::defaultSettings = {
		InputSetting(InputKey::none, InputAction::None, "None", &Normal),
		InputSetting(InputKey::mouseLeftButton, InputAction::ClickMouseLeftButton, "Click Mouse Left Button", &Normal),
		InputSetting(InputKey::mouseRightButton, InputAction::ClickMouseRightButton, "Click Mouse Right Button", &Normal),
		InputSetting(InputKey::mouseMiddleButton, InputAction::ClickMouseMiddleButton, "Click Mouse Middle Button", &Normal),

		InputSetting(InputKey::escape, InputAction::QuitGame, "Quit Game", &GameSystem),
		InputSetting(InputKey::t, InputAction::Test, "Test", &GameSystem),
		InputSetting(InputKey::y, InputAction::Test2, "Test2", &GameSystem),
		InputSetting(InputKey::f9, InputAction::MinimizeWindow, "Minimize Window", &GameSystem),
		InputSetting(InputKey::f10, InputAction::MaximizeWindow, "Maximize Window", &GameSystem),
		InputSetting(InputKey::f11, InputAction::FullScreen, "Full Screen", &GameSystem),

		InputSetting(InputKey::w, InputAction::Forward, "Forward", &Movement),
		InputSetting(InputKey::s, InputAction::Backward, "Backward", &Movement),
		InputSetting(InputKey::a, InputAction::Left, "Left", &Movement),
		InputSetting(InputKey::d, InputAction::Right, "Right", &Movement),
		InputSetting(InputKey::space, InputAction::Jump, "Jump", &Movement),
		InputSetting(InputKey::leftShift, InputAction::Sneak, "Sneak", &Movement),

		InputSetting(InputKey::e, InputAction::Item, "Item", &Inventory),
		InputSetting(InputKey::r, InputAction::Sort, "Sort", &Inventory),
		InputSetting(InputKey::k1, InputAction::Hotbar1, "Hotbar 1", &Inventory),
		InputSetting(InputKey::k2, InputAction::Hotbar2, "Hotbar 2", &Inventory),
		InputSetting(InputKey::k3, InputAction::Hotbar3, "Hotbar 3", &Inventory),
		InputSetting(InputKey::k4, InputAction::Hotbar4, "Hotbar 4", &Inventory),
		InputSetting(InputKey::k5, InputAction::Hotbar5, "Hotbar 5", &Inventory),
		InputSetting(InputKey::k6, InputAction::Hotbar6, "Hotbar 6", &Inventory),
		InputSetting(InputKey::k7, InputAction::Hotbar7, "Hotbar 7", &Inventory),
		InputSetting(InputKey::k8, InputAction::Hotbar8, "Hotbar 8", &Inventory),
		InputSetting(InputKey::k9, InputAction::Hotbar9, "Hotbar 9", &Inventory),
		InputSetting(InputKey::k0, InputAction::Hotbar10, "Hotbar 10", &Inventory),

		InputSetting(InputKey::mouseLeftButton, InputAction::Destroy, "Destroy", &Action),
		InputSetting(InputKey::mouseRightButton, InputAction::Place, "Place", &Action),
		InputSetting(InputKey::e, InputAction::Use, "Use", &Action),
		InputSetting(InputKey::f, InputAction::Fly, "Fly", &Action),
		InputSetting(InputKey::c, InputAction::Collision, "Collision", &Action),

		InputSetting(InputKey::left, InputAction::CursorMoveLeft1px, "CursorMoveLeft1px", &Test),
		InputSetting(InputKey::right, InputAction::CursorMoveRight1px, "CursorMoveRight1px", &Test),
		InputSetting(InputKey::up, InputAction::CursorMoveUp1px, "CursorMoveUp1px", &Test),
		InputSetting(InputKey::down, InputAction::CursorMoveDown1px, "CursorMoveDown1px", &Test),
	};
	std::unordered_map<InputAction, InputKey> InputConfig::actionMap = {};
	std::unordered_map<InputKey, DynArr<InputSetting>> InputConfig::config = {};

	InputKey InputConfig::GetKey(InputAction inputAction) {
		return actionMap.find(inputAction)->second;
	}

}
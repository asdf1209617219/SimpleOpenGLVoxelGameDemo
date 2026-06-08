#include <dme/input/input.h>

namespace dme::input {

	static double _nativeOldCursorPosX = static_cast<double>(Config::default_win_width) / 2;
	static double _nativeOldCursorPosY = static_cast<double>(Config::default_win_height) / 2;
	static double _nativeCursorPosX = _nativeOldCursorPosX;
	static double _nativeCursorPosY = _nativeOldCursorPosY;

	//std::unordered_map<InputAction, uint> Input::inputActionCache = {};
	static uint _inputActionCache[static_cast<uint>(InputAction::InputAction_Max)] = {0};

	float Input::cursorSensitivity = 0.001f;
	bool Input::cursorIsForceCenter = true;
	Double2 Input::oldCursorPosition = Double2();
	Double2 Input::cursorPosition = Double2();
	Double2 Input::cursorOffset = Double2();
	Double2 Input::scrollOffset = Double2();
	bool Input::isScrolled = false;

	static std::unordered_set<sint> _pressingKey;
	static DynArr<sint> _deleteKey;

	/*
	void Input::updateCursorPosition() {
		glfwGetCursorPos(Game::win, &_nativeCursorPosX, &_nativeCursorPosY);
		cursorPosition.x = _nativeCursorPosX - Game::getWinWidthHalf();
		cursorPosition.y = Game::getWinHeightHalf() - _nativeCursorPosY;
	}
	*/

	static constexpr uint input_mask = uint_max ^ 0b11;

	void Input::pressKeyAction(sint key) {
		auto configIt = InputConfig::config.find(InputKeyMap::allKeyMap.find(key)->second.key);
		for (auto& setting : configIt->second) {
			//auto it = inputActionCache.find(setting.inputAction);
			//it->second |= 1;

			//_inputActionCache[static_cast<uint>(setting.inputAction)] |= 1;
			auto& a = _inputActionCache[static_cast<uint>(setting.inputAction)];
			auto i = a & input_mask;
			a = (a & i) | 1;
		}
		_pressingKey.emplace(key);
	}
	void Input::releaseKeyAction(sint key) {
		auto configIt = InputConfig::config.find(InputKeyMap::allKeyMap.find(key)->second.key);
		for (auto& setting : configIt->second) {
			//auto it = inputActionCache.find(setting.inputAction);
			//it->second |= 2;

			//_inputActionCache[static_cast<uint>(setting.inputAction)] |= 2;
			auto& a = _inputActionCache[static_cast<uint>(setting.inputAction)];
			auto i = a & input_mask;
			a = (a & i) | 2;
		}
		_pressingKey.erase(key);
	}
	void Input::pressMouseButtonAction(sint button) {
		auto configIt = InputConfig::config.find(InputKeyMap::allMouseButtonMap.find(button)->second.key);
		for (auto& setting : configIt->second) {
			//auto it = inputActionCache.find(setting.inputAction);
			//it->second |= 1;

			//_inputActionCache[static_cast<uint>(setting.inputAction)] |= 1;
			auto& a = _inputActionCache[static_cast<uint>(setting.inputAction)];
			auto i = a & input_mask;
			a = (a & i) | 1;
		}
	}
	void Input::releaseMouseButtonAction(sint button) {
		auto configIt = InputConfig::config.find(InputKeyMap::allMouseButtonMap.find(button)->second.key);
		for (auto& setting : configIt->second) {
			//auto it = inputActionCache.find(setting.inputAction);
			//it->second |= 2;

			//_inputActionCache[static_cast<uint>(setting.inputAction)] |= 2;
			auto& a = _inputActionCache[static_cast<uint>(setting.inputAction)];
			auto i = a & input_mask;
			a = (a & i) | 2;
		}
	}

	bool Input::IsPress(InputAction inputAction) {
		//uint& r = inputActionCache.find(inputAction)->second;
		uint& r = _inputActionCache[static_cast<uint>(inputAction)];
		return (r & 1) && ((r & 0b1100) != 0b100); //(r & 1)当前帧按下，(r & 12) != 4上一帧没按下
	}
	bool Input::IsRelease(InputAction inputAction) {
		//return inputActionCache.find(inputAction)->second & 0b10; // & 2 当前帧松开
		return _inputActionCache[static_cast<uint>(inputAction)] & 0b10; // & 2 当前帧松开
	}
	bool Input::IsDown(InputAction inputAction) {
		//return inputActionCache.find(inputAction)->second & 1;
		return _inputActionCache[static_cast<uint>(inputAction)] & 1;
	}
	InputStatus Input::GetInputStatus(InputAction inputAction) {
		//uint& r = inputActionCache.find(inputAction)->second;
		uint& r = _inputActionCache[static_cast<uint>(inputAction)];
		return InputStatus( r & 1, r & 0b10, (r & 1) && ((r & 0b1100) != 0b100) );
	}

	void Input::DisableCursor() {
		cursorIsForceCenter = true;
		glfwSetInputMode(Game::win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetCursorPos(Game::win, _nativeCursorPosX, _nativeCursorPosY);
		cursorOffset.x = 0;
		cursorOffset.y = 0;
	}
	void Input::EnableCursor() {
		cursorIsForceCenter = false;
		glfwSetInputMode(Game::win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		glfwSetCursorPos(Game::win, _nativeCursorPosX, _nativeCursorPosY);
		cursorOffset.x = 0;
		cursorOffset.y = 0;
	}
	void Input::ChangeCursorIsForceCenter() {
		if (cursorIsForceCenter) {
			EnableCursor();
		}
		else {
			DisableCursor();
		}
	}
	bool Input::GetCursorIsForceCenter() {
		return cursorIsForceCenter;
	}

	Double2 Input::GetCursorOffset() {
		return cursorOffset;
	}
	Float2 Input::GetCursorOffsetF() {
		return cursorOffset.cast<float>();
	}

	Double2 Input::GetCursorPosition() {
		return cursorPosition;
	}
	Float2 Input::GetCursorPositionF() {
		return cursorPosition.cast<float>();
	}

	void Input::MoveLeftCursorPosition1px() {
		double x, y;
		glfwGetCursorPos(Game::win, &x, &y);
		x -= 1;
		glfwSetCursorPos(Game::win, x, y);
	}
	void Input::MoveRightCursorPosition1px() {
		double x, y;
		glfwGetCursorPos(Game::win, &x, &y);
		x += 1;
		glfwSetCursorPos(Game::win, x, y);
	}
	void Input::MoveUpCursorPosition1px() {
		double x, y;
		glfwGetCursorPos(Game::win, &x, &y);
		y -= 1;
		glfwSetCursorPos(Game::win, x, y);
	}
	void Input::MoveDownCursorPosition1px() {
		double x, y;
		glfwGetCursorPos(Game::win, &x, &y);
		y += 1;
		glfwSetCursorPos(Game::win, x, y);
	}

	Double2 Input::GetCursorInUIPosition() {
		//return Double2(
		//  std::floor(cursorPosition.x + tod(Game::GetWinWidth()) / 2) + 0.5,
		//	std::floor(cursorPosition.y + tod(Game::GetWinHeight()) / 2) + 0.5);
		return Double2(
			cursorPosition.x + Game::GetWinWidth() / 2,
			cursorPosition.y + Game::GetWinHeight() / 2);
	}
	Float2 Input::GetCursorInUIPositionF() {
		return GetCursorInUIPosition().cast<float>();
	}
	Double2 Input::GetScrollOffset() {
		return scrollOffset;
	}

	void Input::HandleInputActionCache() {
		//输入缓存处理
		/*
		for (auto it = inputActionCache.begin(); it != inputActionCache.end(); ++it) {
			it->second <<= 2;
			if ((it->second & 4) && !(it->second & 8)) {
				it->second |= 1;
			}
		}
		*/
		for (uint i = 0; i < static_cast<uint>(InputAction::InputAction_Max); i++) {
			uint& r = _inputActionCache[i];
			r <<= 2;
			if ((r & 0b100) && !(r & 0b1000)) {
				r |= 1;
			}
		}

		//glfw处理事件函数
		glfwPollEvents();
		//解决输入法卡输入的问题 
		// TODO 目前手段无法解决输入法卡键的问题，问题出于glfw在某个键按下并且同时弹出输入法时，
		// glfw无法检测到该键的松开事件，考虑切换到SDL3
		for (auto& key : _pressingKey) {
			if (glfwGetKey(Game::win, key) == GLFW_RELEASE) {
				_deleteKey.add(key);
				//std::cout << "key id: " << key << "; key status: " << glfwGetKey(Game::win, key) << "\n";
			}
		}
		for (auto& key : _deleteKey) {
			releaseKeyAction(key);
		}
		_deleteKey.clear();

		//if (Input::IsPress(InputAction::Test)) { //TODO test
		//	std::cout << "test space status:" << glfwGetKey(Game::win, GLFW_KEY_SPACE) << "\n";
		//}
		/*
		//TODO test 记录一下卡键的情况
		for (auto& key : _pressingKey) {
			auto mapIt = InputKeyMap::allMouseButtonMap.find(key);
			if (mapIt == InputKeyMap::allMouseButtonMap.end()) {
				continue;
			}
			auto configIt = InputConfig::config.find(mapIt->second.key);
			if (configIt == InputConfig::config.end()) {
				continue;
			}
			for (auto& setting : configIt->second) {
				auto& a = _inputActionCache[static_cast<uint>(setting.inputAction)];
				if ((a & 1) && ((a & 0b1100) == 0b100)) { //当前帧和上一帧按下
					std::cout << "key name: " << mapIt->second.name << "; key status: " << glfwGetKey(Game::win, key) << "\n";
				}
			}
		}
		//test end
		*/

		if (!isScrolled) {
			scrollOffset.x = 0;
			scrollOffset.y = 0;
		}
		isScrolled = false;
		//计算鼠标偏移量
		_nativeOldCursorPosX = _nativeCursorPosX;
		_nativeOldCursorPosY = _nativeCursorPosY;
		glfwGetCursorPos(Game::win, &_nativeCursorPosX, &_nativeCursorPosY); //glfw中窗口左上角坐标为(0,0)，右下角为(winWidth,winHeight)
		cursorOffset.x = _nativeCursorPosX - _nativeOldCursorPosX; //鼠标向右_nativeCursorPosX会增加
		cursorOffset.y = _nativeOldCursorPosY - _nativeCursorPosY; //鼠标向下_nativeCursorPosY会增加
		if (cursorIsForceCenter) {
			//强制居中
			_nativeCursorPosX = Game::GetWinWidthHalf();
			_nativeCursorPosY = Game::GetWinHeightHalf();
			glfwSetCursorPos(Game::win, _nativeCursorPosX, _nativeCursorPosY);
			oldCursorPosition = cursorPosition;
			cursorPosition.x = 0;
			cursorPosition.y = 0;
		}
		else {
			//坐标转换
			oldCursorPosition = cursorPosition;
			cursorPosition.x = _nativeCursorPosX - Game::GetWinWidthHalf();
			cursorPosition.y = Game::GetWinHeightHalf() - _nativeCursorPosY;
		}
	}
	float Input::GetCursorSensitivity() {
		return cursorSensitivity;
	}

}
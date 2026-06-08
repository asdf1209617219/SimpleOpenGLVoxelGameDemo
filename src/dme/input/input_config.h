#pragma once
#include <dme/input/input_key.h>
namespace dme::input {
	class InputConfig;
	//输入配置类型
	struct InputSettingType {
		const stdstr type;
		static const InputSettingType NoneType;
		InputSettingType(const stdstr& type);
	};

	//这里添加完动作后，需要在InputConfig::defaultSettings处添加默认按键，并且索引要与值相同
	enum class InputAction : uint {
		//最小值
		InputAction_Min = 0, //最小值 //该值必须放在最开始且不能改动，否则数组会越界
		//Normal
		None = 0, //该值不能改动，否则数组会越界
		ClickMouseLeftButton,
		ClickMouseRightButton,
		ClickMouseMiddleButton,
		//Game System
		QuitGame,
		Test,
		Test2,
		MinimizeWindow,
		MaximizeWindow,
		FullScreen,
		//Movement
		Forward,
		Backward,
		Left,
		Right,
		Jump,
		Sneak,
		//Inventory
		Item,
		Sort,
		Hotbar1,
		Hotbar2,
		Hotbar3,
		Hotbar4,
		Hotbar5,
		Hotbar6,
		Hotbar7,
		Hotbar8,
		Hotbar9,
		Hotbar10,
		//Action
		Destroy, //破坏方块
		Place, //放置方块
		Use, //与方块互动
		Fly, //切换是否飞行状态
		Collision, //切换是否检测碰撞

		//Test
		CursorMoveLeft1px,
		CursorMoveRight1px,
		CursorMoveUp1px,
		CursorMoveDown1px,

		//最大值
		InputAction_Max, //最大值 //该值必须放在最后
	};

	//输入配置，动作与虚拟按键的绑定
	//TODO 组合键的支持
	class InputSetting {
		friend Input;
		friend InputConfig;
		friend graph::Content;
	private:
		InputKey key;
		InputAction inputAction;
		const stdstr name;
		const InputSettingType* type;
		uint index;

		InputSetting(InputKey key, InputAction inputAction, stdstr name, const InputSettingType* type);
	public:
		const stdstr& getName() const;
		const stdstr& getType() const;
		InputKey getKey() const;
		InputAction getInputAction() const;

		uint getIndex() const;
	};

	//配置的集合
	class InputConfig {
		friend Input;
		friend InputSetting;
		friend graph::Content;
	private:
		//默认配置集合
		static DynArr<InputSetting> defaultSettings;
		//当前动作配置，在graph::Content中初始化
		static std::unordered_map<InputAction, InputKey> actionMap;
		//当前输入配置，在graph::Content中初始化
		static std::unordered_map<InputKey, DynArr<InputSetting>> config;
	public:
		//获取动作的按键
		static InputKey GetKey(InputAction inputAction);
		//设置动作的按键
		//static void setKey(InputAction inputAction, InputKey key);
		//将某个配置重置为默认配置
		//static void resetToDefaultKey(InputAction inputAction);
	};

}
#pragma once
#include <dme/core.h>

namespace dme {
	namespace graph {
		class Content;
	}
	namespace input {
		class Input;
		//此处添加完按键后，需要在AllInputKeyMap里下标相同的位置添加映射
		enum class InputKey : uint {
			none = 0,
			a, b, c, d, e, f, g, h, i, j,
			k, l, m, n, o, p, q, r, s, t,
			u, v, w, x, y, z,
			k0, k1, k2, k3, k4, k5, k6, k7, k8, k9,
			f1, f2, f3, f4, f5, f6, f7, f8, f9, f10,
			f11, f12, f13, f14, f15, f16, f17, f18, f19, f20,
			f21, f22, f23, f24, f25,
			space,	/*   */
			grave,	/* ` */
			minus,	/* - */
			equal,	/* = */
			leftBracket,	/* [ */
			rightBracket,	/* ] */
			backslash,	/* \ */
			semicolon,	/* ; */
			apostrophe,	/* ' */
			comma,	/* , */
			period,	/* . */
			slash,	/* / */

			leftShift,
			leftControl,
			leftAlt,
			leftSuper,	/* win */
			rightShift,
			rightControl,
			rightAlt,
			rightSuper,	/* win */
			menu,	/* right click */

			escape,
			tab,
			backspace,	/* <- */
			enter,

			up, down, left, right,
			insert,
			home,
			del,	/* delete */
			end,
			pageUp,
			pageDown,
			printScreen,
			pause,

			capsLock,
			scrollLock,
			numLock,
			/* Keypad */
			kp0, kp1, kp2, kp3, kp4, kp5, kp6, kp7, kp8, kp9,
			kpDivide,	/* / */
			kpMultiply,	/* * */
			kpSubtract,	/* - */
			kpAdd,		/* + */
			kpEqual,	/* = */
			kpDecimal,	/* . */
			kpEnter,

			mouseLeftButton,
			mouseRightButton,
			mouseMiddleButton,
			mouseButton4,
			mouseButton5,
			mouseButton6,
			mouseButton7,
			mouseButton8,

			AllInputKeyCount,
		};

		enum class InputKeyType : uint {
			none = 0,
			keyboard = 1,
			mouse,
		};

		struct InputKeyMap {
			friend graph::Content;
			friend Input;
		public:
			const InputKey key;
			const sint real;
			const InputKeyType type;
			const stdstr name;

			InputKeyMap(const InputKey key, stdstr name, const sint real, InputKeyType type = InputKeyType::keyboard);
			static const InputKeyMap& ToMap(InputKey key);
		private:
			static constexpr uint allInputKeyCount = static_cast<uint>(InputKey::AllInputKeyCount);
			//虚拟key映射到真实key
			static const InputKeyMap allInputKeyMap[allInputKeyCount];

			//真实键盘key映射到虚拟key，在graph::Content中初始化
			static std::unordered_map<sint, const InputKeyMap> allKeyMap;
			//真实鼠标key映射到虚拟key，在graph::Content中初始化
			static std::unordered_map<sint, const InputKeyMap> allMouseButtonMap;
		};
	}
}
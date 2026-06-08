#pragma once
#include <dme/input/input_config.h>

namespace dme {
	namespace graph {
		class Content;
	}
	namespace input {
		class InputStatus {
		private:
			uchar v;
		public:
			InputStatus() noexcept : v(0) {}
			InputStatus(bool down, bool press, bool release) noexcept : v(
				static_cast<uchar>(down) |
				(static_cast<uchar>(press) << 1) |
				(static_cast<uchar>(release) << 2)
			) {}

			bool isDown() const noexcept {
				return v & 1;
			}
			bool isPress() const noexcept {
				return v & 2;
			}
			bool isRelease() const noexcept {
				return v & 4;
			}
		};

		class Input {
		private:
			friend graph::Content;
			//friend Game; //仅在updateWin中调用updateCursorPosition
			//static void updateCursorPosition();

			//保存当前帧与上一帧的动作对应的按键是否被按下和松开，如当前帧按下则最右位为1，松开则右数第2位为1，每帧结束后左移2bit。需要在graph::Content中初始化
			//static std::unordered_map<InputAction, uint> inputActionCache;

			//触发键盘按下按键动作
			static void pressKeyAction(sint key);
			//触发键盘松开按键动作
			static void releaseKeyAction(sint key);
			//触发鼠标按下按键动作
			static void pressMouseButtonAction(sint button);
			//触发鼠标松开按键动作
			static void releaseMouseButtonAction(sint button);
			//光标灵敏度
			static float cursorSensitivity;
			//光标是否强制居中
			static bool cursorIsForceCenter;
			//上一帧光标位置
			static Double2 oldCursorPosition;
			//当前帧光标位置
			static Double2 cursorPosition;
			//当前帧光标偏移量
			static Double2 cursorOffset;
			//当前帧滚动偏移量
			static Double2 scrollOffset;
			static bool isScrolled;
		public:
			//只有上一帧松开，这一帧按住时才会判断成功
			static bool IsPress(InputAction inputAction);
			//只有上一帧按住，这一帧松开时才会判断成功
			static bool IsRelease(InputAction inputAction);
			//只判断当前帧是否按下
			static bool IsDown(InputAction inputAction);
			//获取按键的状态
			static InputStatus GetInputStatus(InputAction inputAction);

			//禁用光标
			static void DisableCursor();
			//启用光标
			static void EnableCursor();
			//更改光标是否强制居中
			static void ChangeCursorIsForceCenter();
			//获取光标是否强制居中
			static bool GetCursorIsForceCenter();

			//获取光标当前帧偏移量（只有光标强制居中时可用，右为x轴正向，上为y轴正向，窗口中心为(0,0)）
			static Double2 GetCursorOffset();
			//获取光标当前帧偏移量并转换为Float2类型（只有光标强制居中时可用，右为x轴正向，上为y轴正向，窗口中心为(0,0)）
			static Float2 GetCursorOffsetF();

			//获取光标当前坐标（右为x轴正向，上为y轴正向，窗口中心为(0,0)）
			static Double2 GetCursorPosition();
			//获取光标当前坐标并转换为Float2类型（右为x轴正向，上为y轴正向，窗口中心为(0,0)）
			static Float2 GetCursorPositionF();

			//获取UI坐标系中光标当前坐标(窗口左下角返回(0.0, 0.0))
			static Double2 GetCursorInUIPosition();
			//获取UI坐标系中光标当前坐标并转换为Float2类型(窗口左下角返回(0.0, 0.0))
			static Float2 GetCursorInUIPositionF();

			//向左移动光标1像素（测试用）
			static void MoveLeftCursorPosition1px();
			//向右移动光标1像素（测试用）
			static void MoveRightCursorPosition1px();
			//向上移动光标1像素（测试用）
			static void MoveUpCursorPosition1px();
			//向下移动光标1像素（测试用）
			static void MoveDownCursorPosition1px();

			//获取当前帧滚动偏移量
			static Double2 GetScrollOffset();

			//处理输入相关逻辑
			static void HandleInputActionCache();
			//获取光标灵敏度
			static float GetCursorSensitivity();
		};
	}
}
#pragma once
#include <dme/core/all.h>
#include <dme/engine/core/all.h>
#include <dme/math/all.h>
struct GLFWwindow;
namespace dme {
	namespace entity {
		class Player;
	}
	namespace graph {
		class Camera;
		class Content;
		class CubeFrame;
		class CuboidFrame;
		class FBO;
		class CameraF;
		class Ray;
		class Skybox;
		class VertexArray;
	}
	namespace input {
		class Input;
	}
	namespace math {
		class Seed;
		class PerlinNoise;
	}


	class Game : Unconstructible {
	private:
		friend Scene;
		friend graph::Content;
		friend input::Input;

		static double deltaTime, currentFrame, previousFrame;
		static bool isFullScreen, isMaximizeWindow, isMinimizeWindow;
		static Int2 winWidthHeight;
		static Double2 winWidthHeightHalf;
		static Int2 screenWidthHeight;
		static GLFWwindow* win;

		//区块加载半径
		static uint chunkLoadRadius;

		static inline math::Seed _seed = math::Seed::CreateSeed();
		static inline math::PerlinNoise _perlin = math::PerlinNoise(_seed);
	public:
		static Scene& MainScene; //主循环相关逻辑，不允许被删除、停用等操作
		static Scene& WorldScene; //世界相关逻辑，不允许被删除、停用等操作
		static Scene& UIScene; //UI相关逻辑，不允许被删除、停用等操作

		static SceneContent* MainSceneMainContent;
		static SceneContent* WorldSceneMainContent;
		static SceneContent* UISceneMainContent;

		static inline math::Seed& Seed = _seed;
		static inline math::PerlinNoise& PerlinNoise = _perlin;

		static graph::CameraF& CameraF;
		static graph::CubeFrame& CubeFrame;
		static graph::CuboidFrame& CuboidFrame;
		static graph::CuboidFrame& CuboidFrame2;
		static graph::CuboidFrame& CuboidFrame3;
		static graph::FBO& ScreenFBO; //离屏渲染对应的FBO
		static graph::Ray& Ray;
		static graph::Skybox& Skybox;
		static graph::VertexArray& ScreenVAO; //表示整个屏幕的四边形

		//获取当前操控玩家
		static entity::Player& GetControlling();

		//游戏准备阶段，加载资源
		static void Init();
		//游戏主循环
		static void Run();
		//离开游戏，卸载资源
		static void Exit();

		//游戏名
		static stdstr GetName() noexcept;
		//获取上一帧到当前帧的时间，最大不会超过一秒
		static double GetDeltaTime() noexcept;
		//获取上一帧到当前帧的时间并转为float，最大不会超过一秒
		static float GetDeltaTimeF() noexcept;
		//获取当前帧的时间
		static double GetCurrentFrame() noexcept;
		//获取上一帧的时间
		static double GetPreviousFrame() noexcept;
		//计算上一帧到当前帧的时间，每帧开始时调用
		static void CalculateFrameTime();
		//更改是否全屏模式
		static void ChangeFullScreen();
		//更改是否最大化窗口
		static void ChangeMaximizeWindow();
		//最小化窗口
		static void ChangeMinimizeWindow();
		//获取窗口
		static GLFWwindow* GetWin() noexcept;
		static sint GetWinWidth() noexcept;
		static sint GetWinHeight() noexcept;
		static Int2 GetWinWidthHeight() noexcept;
		static double GetWinWidthHalf() noexcept;
		static double GetWinHeightHalf() noexcept;
		static Double2 GetWinWidthHeightHalf() noexcept;
		//获取窗口宽高比
		static float GetWinAspectRatio() noexcept;
		//更新窗口大小
		static void UpdateWinSize(sint width, sint height);
		//获取屏幕大小
		static sint GetScreenWidth() noexcept;
		static sint GetScreenHeight() noexcept;
		static Int2 GetScreenWidthHeight() noexcept;

		//获取当前场景
		static Scene& GetNowScene() noexcept;
		//获取区块加载半径
		static uint GetChunkLoadRadius() noexcept;
	};

}
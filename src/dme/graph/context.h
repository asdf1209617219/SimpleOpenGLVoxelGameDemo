#pragma once
#include <dme/core/base.h>
#include <glad/include/glad/glad.h> //glad和glfw的头文件引用顺序不要改变
#include <glfw/glfw3.h>
namespace dme::graph {
	// 显卡API相关操作
	class Content final {
	private:
		Content() = delete;

		//改变窗口大小回调
		static void framebufferSizeCallback(GLFWwindow* window, sint width, sint height);
		//键盘按键回调
		static void keyCallback(GLFWwindow* window, sint key, sint scancode, sint action, sint mods);
		//文字输入回调
		static void charCallback(GLFWwindow* window, uint codepoint);
		//鼠标按键回调
		static void mouseButtonCallback(GLFWwindow* window, sint button, sint action, sint mods);
		//滚轮滚动回调
		static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	public:

		//获取对应参数是否开启
		static bool GetIsEnabled(uint cap);

		//启用深度测试
		static void EnableDepthTest();
		//禁用深度测试
		static void DisableDepthTest();
		//是否开启深度测试
		static bool GetIsEnabledDepthTest();

		//启用混合
		static void EnableBlend();
		//禁用混合
		static void DisableBlend();
		//是否启用混合
		static bool GetIsEnabledBlend();

		//启用面剔除
		static void EnableCullFace();
		//禁用面剔除
		static void DisableCullFace();
		//是否启用面剔除
		static bool GetIsCullFace();

		//设置颜色缓冲清除后的颜色
		static void SetClearColor(float r, float g, float b, float a);
		//设置默认的颜色缓冲清除后的颜色
		static void SetDefaultClearColor();

		//清除颜色缓冲
		static void ClearColorBuffer();
		//清除深度缓冲
		static void ClearDepthBuffer();
		//清除模板缓冲
		static void ClearStencilBuffer();
		//清除颜色深度缓冲
		static void ClearColorDepthBuffer();
		//清除颜色模板缓冲
		static void ClearColorStenciBuffer();
		//清除深度模板缓冲
		static void ClearDepthStencilBuffer();
		//清除颜色深度模板缓冲
		static void ClearColorDepthStencilBuffer();

		//最大纹理尺寸
		static sint GetMaxTextureSize();

		//opengl窗口一系列初始化操作，返回0表示成功，非0表示失败
		static uint OpenglStart();
		//opengl窗口关闭及资源释放操作，默认在Game::Exit中调用
		static void OpenglEnd();


		//多重采样级别
		static constexpr sint MSAALevel = 4;
	};

}

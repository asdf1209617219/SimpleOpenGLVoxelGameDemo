#include <dme/graph/context.h>
#include <dme/graph/shader.h>
#include <dme/input/input.h>
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

namespace dme::graph {

    void Content::framebufferSizeCallback(GLFWwindow* window, sint width, sint height) {
        Game::UpdateWinSize(width, height);
    }
    void Content::keyCallback(GLFWwindow* window, sint key, sint scancode, sint action, sint mods) {
        /*
        //glfwGetKey(Game::win, key) == GLFW_PRESS
        if (action == GLFW_PRESS) {
            //input::Input::pressKeyAction(key);
            std::cout << "key:" << key << " is press\n";
        }
        else if (action == GLFW_RELEASE) {
            std::cout << "key:" << key << " is release\n";
        }
        else if (action == GLFW_REPEAT) {
            std::cout << "key:" << key << " is repeat\n";
        }
        */
        if (action == GLFW_RELEASE) {
            input::Input::releaseKeyAction(key);
            //std::cout << "key:" << key << " is release\n";
        }
        else if (action == GLFW_PRESS) {
            input::Input::pressKeyAction(key);
            //std::cout << "key:" << key << " is press\n";
        }
        //else if (action == GLFW_REPEAT) {
            //input::Input::pressKeyAction(key);
            //std::cout << "key:" << key << " is repeat\n";
        //}
    }
    void Content::charCallback(GLFWwindow* window, uint codepoint) {
        std::cout << "input: " << codepoint << "\n";
    }
    void Content::mouseButtonCallback(GLFWwindow* window, sint button, sint action, sint mods) {
        /*
        if (glfwGetMouseButton(Game::win, button) == GLFW_PRESS) {
            input::Input::pressMouseButtonAction(button);
        }
        */
        if (action == GLFW_RELEASE) {
            input::Input::releaseMouseButtonAction(button);
        }
        else if (action == GLFW_PRESS) {
            input::Input::pressMouseButtonAction(button);
        }
    }
    void Content::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
        input::Input::scrollOffset.x = xoffset;
        input::Input::scrollOffset.y = yoffset;
        input::Input::isScrolled = true;
    }

    bool Content::GetIsEnabled(uint cap) {
        return glIsEnabled(cap);
    }

    void Content::EnableDepthTest() {
        glEnable(GL_DEPTH_TEST);
    }
    void Content::DisableDepthTest() {
        glDisable(GL_DEPTH_TEST);
    }
    bool Content::GetIsEnabledDepthTest() {
        return glIsEnabled(GL_DEPTH_TEST);
    }

    void Content::EnableBlend() {
        glEnable(GL_BLEND);
    }
    void Content::DisableBlend() {
        glDisable(GL_BLEND);
    }
    bool Content::GetIsEnabledBlend() {
        return glIsEnabled(GL_BLEND);
    }

    void Content::EnableCullFace() {
        glEnable(GL_CULL_FACE);
    }
    void Content::DisableCullFace() {
        glDisable(GL_CULL_FACE);
    }
    bool Content::GetIsCullFace() {
        return glIsEnabled(GL_CULL_FACE);
    }

    void Content::SetClearColor(float r, float g, float b, float a) {
        glClearColor(r, g, b, a);
    }
    void Content::SetDefaultClearColor() {
        glClearColor(0.6f, 0.8f, 1.0f, 1.0f);
    }

    void Content::ClearColorBuffer() {
        glClear(GL_COLOR_BUFFER_BIT);
    }
    void Content::ClearDepthBuffer() {
        glClear(GL_DEPTH_BUFFER_BIT);
    }
    void Content::ClearStencilBuffer() {
        glClear(GL_STENCIL_BUFFER_BIT);
    }
    void Content::ClearColorDepthBuffer() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    void Content::ClearColorStenciBuffer() {
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }
    void Content::ClearDepthStencilBuffer() {
        glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }
    void Content::ClearColorDepthStencilBuffer() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    static sint _maxTextureSize;
    sint Content::GetMaxTextureSize() {
        return _maxTextureSize;
    }

    uint Content::OpenglStart() {
        // glfw: initialize and configure
        // ------------------------------
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if DME_OSX
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
        //启用包含MSAALevel个样本的多重采样缓冲，以用于开启抗锯齿
        glfwWindowHint(GLFW_SAMPLES, MSAALevel);
        //窗口设置为黑色
        glfwWindowHint(GLFW_RED_BITS, 0);
        glfwWindowHint(GLFW_GREEN_BITS, 0);
        glfwWindowHint(GLFW_BLUE_BITS, 0);
        // glfw window creation
        // --------------------
        Game::win = glfwCreateWindow(Game::winWidthHeight.x, Game::winWidthHeight.y, Game::GetName().c_str(), NULL, NULL);
        if (!Game::win) {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return 1;
        }
        //生成窗口上下文
        glfwMakeContextCurrent(Game::win);
        // glad: load all OpenGL function pointers
        // ---------------------------------------
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cout << "Failed to initialize GLAD" << std::endl;
            glfwTerminate();
            return 2;
        }
        SetClearColor(0, 0, 0, 1);
        //获取当前显示器模式
        const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        //获取窗口当前大小
        //sint windowWidth, windowHeight;
        //glfwGetWindowSize(Game::win, &windowWidth, &windowHeight);
        //获取当前屏幕大小
        // TODO 当游戏进行中显示设备或系统设置的屏幕大小发生变化后需要变更，并触发窗口大小更改事件
        Game::screenWidthHeight = Int2(mode->width, mode->height);

        //将窗口最大化
        //glfwMaximizeWindow(Game::win);
        //刷新两帧，防止打开后显示白屏
        ClearColorDepthStencilBuffer();
        glfwSwapBuffers(Game::win);
        ClearColorDepthStencilBuffer();
        glfwSwapBuffers(Game::win);
        //绑定窗口改变大小回调函数
        glfwSetFramebufferSizeCallback(Game::win, framebufferSizeCallback);
        //绑定鼠标按键回调函数
        glfwSetMouseButtonCallback(Game::win, mouseButtonCallback);
        //绑定滚动回调函数
        glfwSetScrollCallback(Game::win, scrollCallback);
        //绑定键盘按键回调函数
        glfwSetKeyCallback(Game::win, keyCallback);
        //绑定文字输入回调函数
        //glfwSetCharCallback(Game::win, charCallback);

        //设置窗口在屏幕中间
        glfwSetWindowPos(Game::win, (Game::screenWidthHeight.x - Game::winWidthHeight.x) / 2, (Game::screenWidthHeight.y - Game::winWidthHeight.y) / 2);
        //设置加载和写入图片时y轴正方向向上（默认的坐标为y轴向下）
        stbi_set_flip_vertically_on_load(true);
        stbi_flip_vertically_on_write(true);
        //启用深度测试
        EnableDepthTest();
        glDepthFunc(GL_LESS);
        //启用面剔除
        glEnable(GL_CULL_FACE);
        //只剔除正向面（顶点顺序为逆时针时为正向面）
        glCullFace(GL_FRONT);
        //glCullFace(GL_BACK); 

        //启用模板测试
        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glStencilMask(0x00);
        //启用混合
        EnableBlend();
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //启用修改点大小
        glEnable(GL_PROGRAM_POINT_SIZE);
        glPointSize(10.0f);
        //修改线段大小
        glLineWidth(2.0f);
        //设置多边形偏移参数
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1.0f, 1.0f);
        //启用多重采样抗锯齿MSAA(由于目前画面是离屏渲染而MSAA只对边框进行多重采样，所以frameBuffer中还要进行一系列设置)
        glEnable(GL_MULTISAMPLE);
        //设置光标居中
        glfwSetCursorPos(Game::win, Game::winWidthHeightHalf.x, Game::winWidthHeightHalf.y);
        //glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        SetDefaultClearColor();

        //获取最大纹理尺寸
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &_maxTextureSize);

        //初始化虚拟按键映射
        for (auto& map : input::InputKeyMap::allInputKeyMap) {
            if (map.type == input::InputKeyType::keyboard) {
                input::InputKeyMap::allKeyMap.emplace(map.real, map);
            }
            else if (map.type == input::InputKeyType::mouse) {
                input::InputKeyMap::allMouseButtonMap.emplace(map.real, map);
            }
            //初始化按键绑定
            input::InputConfig::config.emplace(map.key, DynArr<input::InputSetting>());
        }
        //初始化按键绑定
        for (auto& setting : input::InputConfig::defaultSettings) {
            input::InputConfig::actionMap.emplace(setting.inputAction, setting.key);
            input::InputConfig::config.find(setting.key)->second.add(setting);
            //input::Input::inputActionCache.emplace(setting.inputAction, 0);
        }
        return 0;
    }
    void Content::OpenglEnd() {
        //graph::ShaderBase::DeleteAllProgram();
        //std::cin.get(); //test
        //Sleep(2000);
        glfwTerminate();
    }

}
#include <dme/core.h>
#include <dme/input/input.h>
#include <dme/graph/shader.h>

static_assert(sizeof(void*) == 8, "Only supports 64-bit platforms.");
using namespace dme;

uint main() {
#ifdef DME_DEBUG
#ifdef DME_WINDOWS_FAMILY
    //启用内存泄漏检测
//  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//  _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
#endif // DME_WINDOWS_FAMILY
#endif // DEBUG

    uint startResult = graph::Content::OpenglStart();
    if (startResult) {
        return startResult;
    }
    Game::Init();
    auto win = Game::GetWin();

    std::cout << "\n\n------------------------------------------\n\n";

    while (!glfwWindowShouldClose(win)) {
        //TestTimer t;
        Game::CalculateFrameTime();
        Game::Run();

        // 交换缓冲并查询IO事件
        glfwSwapBuffers(win);
        input::Input::HandleInputActionCache();
    }

    Game::Exit();
    return 0;
}
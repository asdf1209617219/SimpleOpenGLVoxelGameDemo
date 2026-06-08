#include <dme/game.h>
#include <dme/core.h>
#include <dme/graph/buffer/frame_buffer.h>
#include <dme/graph/camera.h>
#include <dme/graph/cube_frame.h>
#include <dme/graph/cuboid_frame.h>
#include <dme/graph/fix_camera.h>
#include <dme/graph/ray.h>
#include <dme/graph/skybox.h>
#include <dme/graph/shader.h>
#include <dme/graph/texture/tex2d_array.h>
#include <dme/graph/texture/tex2d_multisample.h>
#include <dme/graph/vertex/vertex_array.h>
#include <dme/block/chunk/chunk.h>
#include <dme/block/proto/all.h>
#include <dme/entity/player.h>
#include <dme/event/event.h>
#include <dme/input/input.h>
#include <dme/item/proto/all.h>
#include <dme/ui/item/hotbar.h>
#include <dme/ui/item/block_item_icon.h>
#include <dme/ui/crosshair.h>
#include <dme/ui/widget/button.h>

#include <dme/world/world.h>

namespace dme {
	double Game::deltaTime = 0.0;
	double Game::currentFrame = 0.0;
	double Game::previousFrame = 0.0;
	bool Game::isFullScreen = false;
	bool Game::isMaximizeWindow = false;
	bool Game::isMinimizeWindow = false;
	Int2 Game::winWidthHeight = Int2(Config::default_win_width, Config::default_win_height);
	Double2 Game::winWidthHeightHalf = Double2(static_cast<double>(Config::default_win_width) / 2, static_cast<double>(Config::default_win_height) / 2);
	Int2 Game::screenWidthHeight = Int2(Config::default_win_width, Config::default_win_height);
	GLFWwindow* Game::win = null;

	uint Game::chunkLoadRadius = 5;

	Scene& Game::MainScene = *Scene::firstScene;
	Scene& Game::WorldScene = *Scene::Create("World Scene");
	Scene& Game::UIScene = *Scene::Create("UI Scene");

	SceneContent* Game::MainSceneMainContent = null;
	SceneContent* Game::WorldSceneMainContent = null;
	SceneContent* Game::UISceneMainContent = null;


	graph::CameraF& Game::CameraF = DME_SVAR_REF(graph::CameraF::CreatePerspectiveCamera(70.0f, 0.1f, 10000.0f,
		Fix3(), Float3c::Forward, Float3c::Up));

	graph::CubeFrame& Game::CubeFrame = DME_SINIT(graph::CubeFrame);
	graph::CuboidFrame& Game::CuboidFrame = DME_SINIT(graph::CuboidFrame);
	graph::CuboidFrame& Game::CuboidFrame2 = DME_SINIT(graph::CuboidFrame);
	graph::CuboidFrame& Game::CuboidFrame3 = DME_SINIT(graph::CuboidFrame);

	graph::FBO& Game::ScreenFBO = graph::FBO::Allocate();

	graph::Ray& Game::Ray = DME_SINIT(graph::Ray);

	graph::Skybox& Game::Skybox = DME_SINIT(graph::Skybox, graph::TexCube(
		DME_PATH_TEX_SKYBOX "right.jpg",
		DME_PATH_TEX_SKYBOX "left.jpg",
		DME_PATH_TEX_SKYBOX "front.jpg",
		DME_PATH_TEX_SKYBOX "back.jpg",
		DME_PATH_TEX_SKYBOX "top.jpg",
		DME_PATH_TEX_SKYBOX "bottom.jpg"
	));

	entity::Player& Game::GetControlling() {
		//static entity::Player _player = *entity::Player::Create("Me", Fix3(16777216, 16777216, 0));
		static entity::Player& _player = *entity::Player::Create("Me", Fix3(0, 0, 0));
		return _player;
	}

	graph::VertexPos2Tex _screenVertexArray[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
			graph::VertexPos2Tex(-1.0f, -1.0f,  0.0f, 0.0f),
			graph::VertexPos2Tex(-1.0f,  1.0f,  0.0f, 1.0f),
			graph::VertexPos2Tex(1.0f,  1.0f,  1.0f, 1.0f),

			graph::VertexPos2Tex(-1.0f, -1.0f,  0.0f, 0.0f),
			graph::VertexPos2Tex(1.0f,  1.0f,  1.0f, 1.0f),
			graph::VertexPos2Tex(1.0f, -1.0f,  1.0f, 0.0f),
	};
	graph::VertexArray& Game::ScreenVAO = DME_SINIT(graph::VertexArray, graph::VertexPos2Tex::GetVertexType(), &_screenVertexArray, 6);


	static bool _contentLoadEnd = false;

	ui::Button* uiButton0;
	static void _testClickFunc() {
		Log::Info("ui button: test click func.");
	};

	void Game::Init() {
		if (_contentLoadEnd) {
			return;
		}
		_contentLoadEnd = true;

		//proto初始化
		block::AllBlockProto::Init();
		item::AllItemProto::Init();
		//注册事件
		event::ItemUseEvent::Manager::Register([](item::ItemProto& proto, event::ItemUseData& data) {
			proto.use(data);
		});
		//event::BlockDestroyEvent::Manager::Register([](block::BlockProto& proto, event::BlockDestroyData& data) {
		//	proto.d
		//});

		//初始化静态变量
		core::_static_init_func_::_init_all_var();
		//初始化纹理图集
		graph::TexAtlas::Default().merge();
		//初始化纹理数组
		graph::Tex2dArray::Cube().merge();
		//强制更新一次窗口宽高
		UpdateWinSize(winWidthHeight.x, winWidthHeight.y);
		//初始化UI::Root
		ui::UI::Root();

		CuboidFrame.setColor(Float3(0.04f, 0.28f, 0.26f));
		CuboidFrame.display();

		CuboidFrame2.setColor(Float3(1, 0, 0));
		CuboidFrame2.display();

		CuboidFrame3.setColor(Float3(0, 1, 0));
		CuboidFrame3.display();

		//十字准星
		ui::Crosshair& crosshair = ui::Crosshair::Create();
		//FBO初始化
		ScreenFBO.init();

		input::Input::DisableCursor();

		auto& player = Game::GetControlling();
		player.setWorldId(Config::default_world);
		//玩家位置以及摄像头初始化
		Int3 spawnPoint = player.getWorld()->getSpawnPoint();
		player.setBottomPos(Fix3(spawnPoint, Float3(0.5f, 0.5f, 0.0f)));
		//player.setBottomPos(Fix3(Int3(player.getCenterPos().i.xy(), 33), Float3(0.5f, 0.5f, 0.0f)));
		Game::CameraF.updatePosition(player.getCameraFPos());
		player.init();

		item::ItemStack stone = item::ItemStack(*item::AllItemProto::Stone, 100);
		item::ItemStack dirt = item::ItemStack(*item::AllItemProto::Dirt, 100);
		item::ItemStack grassBlock = item::ItemStack(*item::AllItemProto::GrassBlock, 100);
		item::ItemStack plank = item::ItemStack(*item::AllItemProto::Plank, 100);
		item::ItemStack log = item::ItemStack(*item::AllItemProto::Log, 100);
		item::ItemStack leaves = item::ItemStack(*item::AllItemProto::Leaves, 100);
		item::ItemStack grass = item::ItemStack(*item::AllItemProto::Grass, 100);

		item::ItemStack apple = item::ItemStack(*item::AllItemProto::Apple, 100);

		/*
		*/

		player.getInventory().addItem(stone);
		player.getInventory().addItem(dirt);
		player.getInventory().addItem(grassBlock);
		player.getInventory().addItem(plank);
		player.getInventory().addItem(log);
		player.getInventory().addItem(leaves);
		player.getInventory().addItem(grass);

		player.getInventory().addItem(apple);

		
		ui::Hotbar& hotbar = ui::Hotbar::Default();
		hotbar.setParentIsRoot();

		

		/*
		uiButton0 = &ui::Button::Create(_testClickFunc);
		uiButton0->setPivotAndAnchorsOnLowerLeft()
			.setWidthHeight(50_px, 50_px)
			.setParentIsRoot();
		*/

		//auto& testBlockItemIcon = ui::BlockItemIcon::Create(stone);
		//testBlockItemIcon.setParentIsRoot().setZIndex(10);
		

		Game::MainSceneMainContent = MainScene.addContent([]() {
			using input::Input;
			using input::InputAction;

			Game::ScreenFBO.startRender();
			if (Input::IsPress(InputAction::QuitGame)) {
				glfwSetWindowShouldClose(Game::GetWin(), true);
			}
			//显示禁用光标
			if (Input::IsPress(InputAction::Item)) {
				Input::ChangeCursorIsForceCenter();
			}
			/*
			if (!Input::GetCursorIsForceCenter()) {
				if (Input::IsPress(InputAction::CursorMoveLeft1px)) {
					Input::MoveLeftCursorPosition1px();
				}
				if (Input::IsPress(InputAction::CursorMoveRight1px)) {
					Input::MoveRightCursorPosition1px();
				}
				if (Input::IsPress(InputAction::CursorMoveUp1px)) {
					Input::MoveUpCursorPosition1px();
				}
				if (Input::IsPress(InputAction::CursorMoveDown1px)) {
					Input::MoveDownCursorPosition1px();
				}
			}
			*/

			if (Input::IsPress(InputAction::MinimizeWindow)) {
				Game::ChangeMinimizeWindow();
			}
			if (Input::IsPress(InputAction::MaximizeWindow)) {
				Game::ChangeMaximizeWindow();
			}
			if (Input::IsPress(InputAction::FullScreen)) {
				Game::ChangeFullScreen();
			}

			//if (Input::IsPress(InputAction::Test)) {
			//	auto& player = GetControlling();
			//	player.getWorld()->getChunk(player.getChunkPos())->unloadChunkMesh();
			//}
		});

		Game::WorldSceneMainContent = WorldScene.addContent([]() {

			world::World::UpdateAll();
			block::Chunk::DrawAll();

			Game::Skybox.draw(Game::CameraF.getProjection(), Game::CameraF.getView());
			Game::CubeFrame.draw(Game::CameraF);
			//Game::CuboidFrame.draw(projection, view);
			//Game::CuboidFrame2.draw(projection, view);
			//Game::CuboidFrame3.draw(projection, view);
			//Game::Ray.draw(projection, view);

			if (input::Input::IsPress(input::InputAction::Test)) {
				auto& player = Game::GetControlling();
				Log::Info("player position: ", player.getBottomPos());
				player.getWorld()->test(player.getBottomPos().i);
			}
			/*
			if (input::Input::IsPress(input::InputAction::Test2)) {
				auto& player = Game::GetControlling();
				auto pos = block::ToChunkPos(player.getBottomPos().i);
				Log::Info("unload chunk, chunk pos: ", pos, ", success: ", player.getWorld()->unloadChunk(pos));
			}
			*/
		});

		Game::UISceneMainContent = UIScene.addContent([]() {
			ScreenFBO.endRender();
			//因为只渲染一个四边形，所以不需要深度测试
			//graph::Content::DisableDepthTest();
			//graph::Content::DisableBlend();
			graph::Content::ClearColorDepthStencilBuffer(); //清除默认的颜色深度缓冲

			//切换FramebuffersScreen Shader
			graph::Shaders::FramebuffersScreen().use();
			//绑定framebuffer的纹理
			graph::Shaders::FramebuffersScreen().setTex2d("screenTexture", ScreenFBO.getTex(), 0);
			//渲染整个屏幕的四边形
			Game::ScreenVAO.draw();

			ui::UITree::Update();
			ui::UITree::Draw();

			/*
			if (!input::Input::getCursorOffset().isZero()) {
				std::cout << "cursor offset: " << input::Input::getCursorOffset() << "\n";
			}
			if (!input::Input::getCursorPosition().isZero()) {
				std::cout << "cursor pos: " << input::Input::getCursorPosition() << "\n";
			}
			*/
		});

		//计算游戏时间
		CalculateFrameTime();
		CalculateFrameTime();
	}

	void Game::Run() {
		Scene::nowScene = Scene::firstScene;

		auto nowScene = Scene::nowScene;
		while (nowScene) {
			if (nowScene && !nowScene->disable) nowScene->run();
			nowScene = nowScene->next;
		}
	}

	void Game::Exit() {
		world::World::deleteAllWorld();
		graph::Content::OpenglEnd();
	}

	stdstr Game::GetName() noexcept {
		return "MyGame";
	}
	double Game::GetDeltaTime() noexcept {
		return deltaTime;
	}
	float Game::GetDeltaTimeF() noexcept {
		return static_cast<float>(deltaTime);
	}
	double Game::GetCurrentFrame() noexcept {
		return currentFrame;
	}
	double Game::GetPreviousFrame() noexcept {
		return previousFrame;
	}
	void Game::CalculateFrameTime() {
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - previousFrame;
		deltaTime = math::Clamp(deltaTime, 0.0001, 0.1); //令其不会超过0.1秒，防止打断点时等待时间过长导致1帧内移动距离过长
		previousFrame = currentFrame;
	}
	static sint preWidth, preHeight, preXPos, preYPos;
	void Game::ChangeFullScreen() {
		if (isFullScreen) {
			glfwSetWindowMonitor(win, NULL, 0, 0, preWidth, preHeight, 0);
			glfwSetWindowPos(win, preXPos, preYPos);
			isFullScreen = false;
		}
		else {
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);
			glfwGetWindowSize(win, &preWidth, &preHeight);
			glfwGetWindowPos(win, &preXPos, &preYPos);
			glfwSetWindowMonitor(win, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
			isFullScreen = true;
		}
	}
	void Game::ChangeMaximizeWindow() {
		if (isMaximizeWindow) {
			glfwRestoreWindow(win);
			isMaximizeWindow = false;
		}
		else {
			glfwMaximizeWindow(win);
			isMaximizeWindow = true;
		}
	}
	void Game::ChangeMinimizeWindow() {
		//glfwHideWindow(win);
		glfwIconifyWindow(win);
	}
	GLFWwindow* Game::GetWin() noexcept {
		return win;
	}
	sint Game::GetWinWidth() noexcept {
		return winWidthHeight.x;
	}
	sint Game::GetWinHeight() noexcept {
		return winWidthHeight.y;
	}
	Int2 Game::GetWinWidthHeight() noexcept {
		return winWidthHeight;
	}
	double Game::GetWinWidthHalf() noexcept {
		return winWidthHeightHalf.x;
	}
	double Game::GetWinHeightHalf() noexcept {
		return winWidthHeightHalf.y;
	}
	Double2 Game::GetWinWidthHeightHalf() noexcept {
		return winWidthHeightHalf;
	}
	float Game::GetWinAspectRatio() noexcept {
		if (winWidthHeight.x == 0 || winWidthHeight.y == 0) { //当屏幕最小化时，屏幕高度宽度为0，会导致投影矩阵产生错误
			return 1.0f;
		}
		else {
			return static_cast<float>(winWidthHeight.x) / static_cast<float>(winWidthHeight.y);
		}
	}
	void Game::UpdateWinSize(sint width, sint height) {
		winWidthHeight = Int2(width, height);
		winWidthHeightHalf = Double2(static_cast<double>(width) / 2, static_cast<double>(height) / 2);

		glViewport(0, 0, width, height);
		//input::Input::updateCursorPosition();
		if (width == 0 || height == 0) return;
		//更新相机的投影矩阵
		auto r = GetWinAspectRatio();
		//Camera.updateAspectRatio(r);
		CameraF.updateAspectRatio(r);
		//更新framebuffer大小
		graph::FBO::UpdateWinSize(width, height);
		//标记UI需要更新
		ui::UI::_UpdateWinSize(width, height);
	}
	sint Game::GetScreenWidth() noexcept {
		return screenWidthHeight.x;
	}
	sint Game::GetScreenHeight() noexcept {
		return screenWidthHeight.y;
	}
	Int2 Game::GetScreenWidthHeight() noexcept {
		return screenWidthHeight;
	}
	Scene& Game::GetNowScene() noexcept {
		return *Scene::nowScene;
	}
	uint Game::GetChunkLoadRadius() noexcept {
		return chunkLoadRadius;
	}

}
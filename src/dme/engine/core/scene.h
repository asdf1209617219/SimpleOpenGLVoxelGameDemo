#pragma once
#include <dme/core/all.h>
namespace dme {
	class Game;
	namespace eng {
		class Scene;
		//场景中的可执行内容
		class SceneContent : private ObjPool<SceneContent> {
			friend Scene;
			friend ObjPool<SceneContent>;
		private:
			Scene* scene;
			SceneContent* previous;
			SceneContent* next;
			void (*updateFunc)();
			void (*removeFunc)();

			SceneContent(Scene* scene, void (*updateFunc)(), void (*removeFunc)()) noexcept : scene(scene), previous(null), next(null), updateFunc(updateFunc), removeFunc(removeFunc) {}

			static SceneContent* Create(Scene* scene_, void (*updateFunc_)(), void (*removeFunc_)()) noexcept {
				SceneContent* ptr = ObjPool<SceneContent>::Add(scene_, updateFunc_, removeFunc_);
				return ptr;
			}

		public:
			Scene* getScene() const noexcept {
				return scene;
			}
			SceneContent* getPrevious() const noexcept {
				return previous;
			}
			SceneContent* getNext() const noexcept {
				return next;
			}
			void (*getUpdateFunc() const noexcept) () {
				return updateFunc;
			}
			void (*getRemoveFunc() const noexcept) () {
				return removeFunc;
			}

			//在前面创建内容
			void insertToPrevious(void (*updateFunction)(), void (*removeFunction)() = null) noexcept;
			//在后面创建内容
			void insertToNext(void (*updateFunction)(), void (*removeFunction)() = null) noexcept;
			//将当前内容替换，会触发removeFunc
			void replace(void (*updateFunction)(), void (*removeFunction)() = null) noexcept;
			//删除内容，触发removeFunc
			static void Remove(SceneContent& content) noexcept;
		};

		//管理资源的处理过程
		class Scene : private ObjPool<Scene> {
			friend Game;
			friend SceneContent;
			friend ObjPool<Scene>;
		private:
			bool disable;
			bool disconnected;
			bool fixed;
			Scene* parent;
			Scene* previous;
			Scene* next;
			Scene* firstChild;
			SceneContent* firstContent;
			void (*tailFunc)(); //在run函数中，运行完content和子场景的run函数后，调用该函数
			strid name;


			void run() noexcept;
			Scene(strid name, Scene* parent, bool fixed = false) noexcept
				: disable(false), disconnected(false), fixed(fixed),
				parent(parent), previous(null), next(null), firstChild(null), firstContent(null), tailFunc(null),
				name(name) {
			}
			Scene(strid name, Scene* parent, Scene* previous, bool fixed = false) noexcept
				: disable(false), disconnected(false), fixed(fixed),
				parent(parent), previous(previous), next(null), firstChild(null), firstContent(null), tailFunc(null),
				name(name)
			{
				if (previous) {
					previous->next = this;
				}
			}

			static inline auto sceneMap = FlatMap<strid, Scene*>(32);
			static inline Scene* firstScene = null;
			//仅用于初始化个别不允许修改的场景
			static inline Scene* init(strid name, Scene* parent) noexcept {
				Scene* ptr = ObjPool<Scene>::Add(name, parent, true);
				sceneMap.addIfAbsent(name, ptr);
				if (!firstScene) {
					firstScene = ptr;
					return ptr;
				}
				Scene* _nowScene = firstScene;
				while (true) {
					if (_nowScene->next) {
						_nowScene = _nowScene->next;
					}
					else {
						ptr->previous = _nowScene;
						_nowScene->next = ptr;
						return ptr;
					}
				}
			}
			static inline Scene* nowScene = Scene::init(strid("Main Scene"), null);

			static void _removeChild(Scene* scene) noexcept;
		public:
			//在第一层级创建，并添加到同级的最后
			static Scene* Create(strid name) noexcept;
			//在父节点下创建，并添加到同级的最后
			static Scene* Create(strid name, Scene& parent) noexcept;
			//删除场景及其子场景，触发removeFunc函数
			static void Remove(Scene& scene) noexcept;

			Scene* getParent() const noexcept {
				return parent;
			}
			Scene* getPrevious() const noexcept {
				return previous;
			}
			Scene* getNext() const noexcept {
				return next;
			}
			Scene* getFirstChild() const noexcept {
				return firstChild;
			}
			SceneContent* getFirstContent() const noexcept {
				return firstContent;
			}
			//在run函数中，运行完content和子场景的run函数后，调用该函数
			void (*getTailFunc() const noexcept)() {
				return tailFunc;
			}
			//在run函数中，运行完content和子场景的run函数后，调用该函数
			void setTailFunc(void (*tailFunc_)()) noexcept {
				tailFunc = tailFunc_;
			}
			strid getName() const noexcept {
				return name;
			}

			bool getDisable() const noexcept {
				return disable;
			}
			void setDisable(bool disable_) noexcept {
				if (fixed) return;
				disable = disable_;
			}
			bool getDisconnected() const noexcept {
				return disconnected;
			}

			//将当前场景移出父场景，不参与循环，移出时不触发removeFunc
			void disconnect() noexcept;

			//将当前场景重新加入父场景最后，只有调用disconnect后才可调用该函数
			void connect(Scene& parentScene) noexcept;
			//将当前场景重新加入该场景前，只有调用disconnect后才可调用该函数
			void connectToPrevious(Scene& scene) noexcept;
			//将当前场景重新加入该场景后，只有调用disconnect后才可调用该函数
			void connectToNext(Scene& scene) noexcept;

			//在前面创建场景
			void insertToPrevious(strid name) noexcept;
			//在后面创建场景
			void insertToNext(strid name) noexcept;

			//是否存在子场景（不会递归查找更下一层）
			bool existChild(Scene& child) noexcept;

			//创建Content在最后
			SceneContent* addContent(void (*updateFunction)(), void (*removeFunction)() = null) noexcept;
			//移除Content，触发removeFunc函数
			void removeContent(SceneContent& content) noexcept;
			//通过字符串获取场景
			static Scene* GetScene(strid name) noexcept;
		};
	}
	using eng::Scene;
	using eng::SceneContent;
}
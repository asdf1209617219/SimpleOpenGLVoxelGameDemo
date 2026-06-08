#include <dme/engine/core/scene.h>
#include <dme/game.h>
namespace dme::eng {
	void SceneContent::insertToPrevious(void (*updateFunction)(), void (*removeFunction)()) noexcept {
		auto ptr = SceneContent::Create(scene, updateFunction, removeFunction);
		if (previous) {
			previous->next = ptr;
		}
		else {
			scene->firstContent = ptr;
		}
		ptr->previous = previous;
		ptr->next = this;
		previous = ptr;
	}
	void SceneContent::insertToNext(void (*updateFunction)(), void (*removeFunction)()) noexcept {
		auto ptr = SceneContent::Create(scene, updateFunction, removeFunction);
		if (next) {
			next->previous = ptr;
		}
		ptr->previous = this;
		ptr->next = next;
		next = ptr;
	}
	void SceneContent::replace(void (*updateFunction)(), void (*removeFunction)()) noexcept {
		if (removeFunc) {
			removeFunc();
		}
		updateFunc = updateFunction;
		removeFunc = removeFunction;
	}
	void SceneContent::Remove(SceneContent& content) noexcept {
		if (content.previous) {
			content.previous->next = content.next;
		}
		else if (content.scene->firstContent == &content) {
			content.scene->firstContent = content.next;
		}
		if (content.next) {
			content.next->previous = content.previous;
		}
		if (content.removeFunc) {
			content.removeFunc();
		}
		ObjPool<SceneContent>::Remove(&content);
	}

	Scene* Scene::Create(strid name) noexcept {
		if (sceneMap.contain(name)) { //TODO 目前不允许重名
			return null;
		}

		Scene* _nowScene = firstScene;
		Scene* ptr = ObjPool<Scene>::Add(name, null);
		sceneMap.addIfAbsent(name, ptr);
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
	Scene* Scene::Create(strid name, Scene& parent) noexcept {
		if (sceneMap.contain(name)) { //TODO 目前不允许重名
			return null;
		}

		Scene* _nowScene = parent.firstChild;
		Scene* ptr = ObjPool<Scene>::Add(name, &parent);
		sceneMap.addIfAbsent(name, ptr);
		if (!_nowScene) {
			parent.firstChild = ptr;
			return ptr;
		}
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
	void Scene::_removeChild(Scene* scene) noexcept {
		Scene* nowChild = scene->firstChild;
		while (nowChild) {
			Scene::_removeChild(nowChild);
			nowChild = nowChild->next;
		}
		SceneContent* nowContent = scene->firstContent;
		while (nowContent) {
			if (nowContent->removeFunc) {
				nowContent->removeFunc();
			}
			nowContent = nowContent->next;
		}
		ObjPool<Scene>::Remove(scene);
	}
	void Scene::Remove(Scene& scene) noexcept {
		if (scene.fixed) return;
		if (scene.previous) {
			scene.previous->next = scene.next;
		}
		else if (!scene.disconnected) {
			if (scene.parent) {
				scene.parent->firstChild = scene.next;
			}
			else {
				Scene::firstScene = &scene;
			}
		}
		if (scene.next) {
			scene.next->previous = scene.previous;
		}
		Scene* nowChild = scene.firstChild;
		while (nowChild) {
			Scene::_removeChild(nowChild);
			nowChild = nowChild->next;
		}
		SceneContent* nowContent = scene.firstContent;
		while (nowContent) {
			if (nowContent->removeFunc) {
				nowContent->removeFunc();
			}
			nowContent = nowContent->next;
		}
		ObjPool<Scene>::Remove(&scene);
	}

	void Scene::disconnect() noexcept {
		if (fixed || disconnected) return;
		disconnected = true;
		if (previous) {
			previous->next = next;
		}
		else if (parent) {
			parent->firstChild = next;
		}
		if (next) {
			next->previous = previous;
		}
		parent = null;
		previous = null;
		next = null;
	}
	void Scene::connect(Scene& parentScene) noexcept {
		if (!disconnected) return;
		disconnected = false;
		Scene* nowChild = parentScene.firstChild;
		if (nowChild) {
			while (true) {
				if (nowChild->next) {
					nowChild = nowChild->next;
				}
				else {
					nowChild->next = this;
					previous = nowChild;
					parent = &parentScene;
					break;
				}
			}
		}
		else {
			parentScene.firstChild = this;
		}
	}
	void Scene::connectToPrevious(Scene& scene) noexcept {
		if (!scene.disconnected) return;
		scene.disconnected = false;
		if (previous) {
			previous->next = &scene;
		}
		else if (parent && parent->firstChild == this) {
			parent->firstChild = &scene;
		}
		scene.parent = parent;
		scene.previous = previous;
		scene.next = this;
		previous = &scene;
	}
	void Scene::connectToNext(Scene& scene) noexcept {
		if (!scene.disconnected) return;
		scene.disconnected = false;
		if (next) {
			next->previous = &scene;
		}
		scene.parent = parent;
		scene.previous = this;
		scene.next = next;
		next = &scene;
	}
	void Scene::insertToPrevious(strid name) noexcept {
		if (sceneMap.contain(name)) { //TODO 目前不允许重名
			return;
		}

		Scene* ptr = ObjPool<Scene>::Add(name, parent);
		sceneMap.addIfAbsent(name, ptr);
		if (previous) {
			previous->next = ptr;
		}
		else if (parent && parent->firstChild == this) {
			parent->firstChild = ptr;
		}
		ptr->previous = previous;
		ptr->next = this;
		previous = ptr;
	}
	void Scene::insertToNext(strid name) noexcept {
		if (sceneMap.contain(name)) { //TODO 目前不允许重名
			return;
		}

		Scene* ptr = ObjPool<Scene>::Add(name, parent);
		sceneMap.addIfAbsent(name, ptr);
		if (next) {
			next->previous = ptr;
		}
		ptr->previous = this;
		ptr->next = next;
		next = ptr;
	}

	bool Scene::existChild(Scene& child) noexcept {
		Scene* ptr = &child;
		Scene* nowChild = firstChild;
		if (nowChild) {
			while (true) {
				if (nowChild == ptr) {
					return true;
				}
				if (nowChild->next) {
					nowChild = nowChild->next;
				}
				else {
					return false;
				}
			}
		}
		return false;
	}

	SceneContent* Scene::addContent(void (*updateFunction)(), void (*removeFunction)()) noexcept {
		SceneContent* ptr = SceneContent::Create(this, updateFunction, removeFunction);
		if (firstContent) {
			SceneContent* content = firstContent;
			while (true) {
				if (content->next) {
					content = content->next;
				}
				else {
					content->next = ptr;
					ptr->previous = content;
					break;
				}
			}
		}
		else {
			firstContent = ptr;
		}
		return ptr;
	}
	void Scene::removeContent(SceneContent& content) noexcept {
		if (!firstContent) {
			return;
		}
		SceneContent* ptr = &content;
		SceneContent* nowContent = firstContent;
		while (true) {
			if (nowContent == ptr) {
				SceneContent::Remove(content);
				return;
			}
			if (nowContent->next) {
				nowContent = nowContent->next;
			}
			else {
				return;
			}
		}
	}

	Scene* Scene::GetScene(strid name) noexcept {
		auto r = sceneMap.get(name);
		return r ? r->val : null;
	}

	void Scene::run() noexcept {
		nowScene = this;

		auto nowContent = firstContent;
		while (nowContent) {
			if (nowContent->updateFunc) nowContent->updateFunc();
			nowContent = nowContent->next;
		}

		auto nowChild = firstChild;
		while (nowChild) {
			if (nowChild && !nowChild->disable) nowChild->run();
			nowChild = nowChild->next;
		}

		if (tailFunc) {
			nowScene = this;
			tailFunc();
		}
	}

}
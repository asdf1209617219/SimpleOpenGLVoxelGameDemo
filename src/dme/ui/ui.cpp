#include <dme/ui/ui.h>
#include <dme/graph/texture/tex2d_array.h>
#include <dme/graph/context.h>
#include <dme/input/input.h>

#include <dme/ui/item/block_item_icon.h>
namespace dme::ui {
	UIEvent::UIEvent(UI& source, const Float2& triggerPos, UIEventType eventType, input::InputAction inputAction) noexcept
		: source(&source), triggerPos(triggerPos), eventType(eventType), inputAction(inputAction) {}

	bool UI::isGrandchildOf(const UI& u) const noexcept {
		auto r = this;
		while (r->parent) {
			r = r->parent;
			if (r == &u) {
				return true;
			}
		}
		return false;
	}

	UI* UI::getThisTreeRoot() noexcept {
		auto r = this;
		while (r->parent) {
			r = r->parent;
		}
		return r;
	}
	const UI* UI::getThisTreeRoot() const noexcept {
		auto r = this;
		while (r->parent) {
			r = r->parent;
		}
		return r;
	}

	void UI::updateBoxPosAndSize(const Abox2& parentBox) noexcept {
		if (!parent) {
			box = parentBox;
		}
		else {
			Float2 parentWidthHeightPx;
			if (hasAnyFlag(UIFlag::anchorsOnRoot)) {
				parentWidthHeightPx = Float2(tof(Game::GetWinWidth()), tof(Game::GetWinHeight()));
			}
			else {
				parentWidthHeightPx = parentBox.sideLength();
			}
			Float2 widthHeightPx;
			if (hasAnyFlag(UIFlag::minSideRatio)) {
				parentWidthHeightPx = math::Min(parentWidthHeightPx.x, parentWidthHeightPx.y);
				widthHeightPx = widthHeight * parentWidthHeightPx;
			}
			else {
				if (hasAnyFlag(UIFlag::widthIsPixel)) {
					widthHeightPx.x = widthHeight.x;
				}
				else {
					widthHeightPx.x = widthHeight.x * parentWidthHeightPx.x;
				}
				if (hasAnyFlag(UIFlag::heightIsPixel)) {
					widthHeightPx.y = widthHeight.y;
				}
				else {
					widthHeightPx.y = widthHeight.y * parentWidthHeightPx.y;
				}
			}

			if (hasAnyFlag(UIFlag::anchorsXIsPixel)) {
				box.min.x = parentBox.min.x + anchors.x;
			}
			else {
				box.min.x = parentBox.min.x + anchors.x * parentWidthHeightPx.x;
			}
			if (hasAnyFlag(UIFlag::anchorsYIsPixel)) {
				box.min.y = parentBox.min.y + anchors.y;
			}
			else {
				box.min.y = parentBox.min.y + anchors.y * parentWidthHeightPx.y;
			}

			if (hasAnyFlag(UIFlag::pivotXIsPixel)) {
				box.min.x -= pivot.x;
			}
			else {
				box.min.x -= pivot.x * widthHeightPx.x;
			}
			if (hasAnyFlag(UIFlag::pivotYIsPixel)) {
				box.min.y -= pivot.y;
			}
			else {
				box.min.y -= pivot.y * widthHeightPx.y;
			}

			box.max = box.min + widthHeightPx;
		}
		if (needBVH()) {
			UI::_UpdateBVH(*this);
		}
		else {
			UI::_RemoveBVH(*this);
		}
	}
	void UI::updateBoxImpl(const Abox2& parentBox) noexcept {
		updateBoxPosAndSize(parentBox);
		removeFlag(UIFlag::needUpdateBox);
		if (getIsRendering()) {
			_meshGroup.update(*this);
		}
		else {
			UI::_RemoveMesh(*this);
		}
		uint count = children.count();
		for (uint i = 0; i < count; ++i) {
			children[i]->updateBoxImpl(box);
		}
	}
	void UI::updateBox() noexcept {
		updateBoxImpl(_ParentBox(*this));
	}
	void UI::updateBoxIfNeedImpl(const Abox2& parentBox) noexcept {
		updateBoxPosAndSize(parentBox);
		removeFlag(UIFlag::needUpdateBox);
		if (getIsRendering()) {
			_meshGroup.update(*this);
		}
		else {
			UI::_RemoveMesh(*this);
		}
		uint count = children.count();
		for (uint i = 0; i < count; ++i) {
			if (children[i]->hasAnyFlag(UIFlag::needUpdateBox)) {
				children[i]->updateBoxIfNeedImpl(box);
			}
		}
	}
	void UI::updateBoxIfNeed() noexcept {
		if (!hasAnyFlag(UIFlag::needUpdateBox)) {
			return;
		}
		UI* ptr = this;
		while (ptr->parent && ptr->parent->hasAnyFlag(UIFlag::needUpdateBox)) {
			ptr = ptr->parent;
		}
		ptr->updateBoxIfNeedImpl(_ParentBox(*ptr));
	}

	void UI::addFlag(UIFlag flag) noexcept {
		flags |= flag;
	}
	void UI::addThisAndChildrenFlag(UIFlag flag) noexcept {
		addFlag(flag);
		uint count = children.count();
		for (uint i = 0; i < count; ++i) {
			children[i]->addThisAndChildrenFlag(flag);
		}
	}
	void UI::setThisAndChildrenFlag(UIFlag mask, UIFlag flag) noexcept {
		setFlag(mask, flag);
		uint count = children.count();
		for (uint i = 0; i < count; ++i) {
			children[i]->setThisAndChildrenFlag(mask, flag);
		}
	}
	void UI::setThisAndChildrenFlagUntilHasAnyFlag(UIFlag mask, UIFlag flag, UIFlag untilFlag) noexcept {
		if (hasAnyFlag(untilFlag)) {
			return;
		}
		setFlag(mask, flag);
		uint count = children.count();
		for (uint i = 0; i < count; ++i) {
			children[i]->setThisAndChildrenFlag(mask, flag);
		}
	}
	void UI::removeFlag(UIFlag flag) noexcept {
		flags &= ~flag;
	}
	void UI::removeThisAndChildrenFlag(UIFlag flag) noexcept {
		removeFlag(flag);
		uint count = children.count();
		for (uint i = 0; i < count; ++i) {
			children[i]->removeThisAndChildrenFlag(flag);
		}
	}
	bool UI::hasAnyFlag(UIFlag flag) const noexcept {
		return static_cast<uint>(flags) & static_cast<uint>(flag);
	}
	bool UI::hasAllFlag(UIFlag flag) const noexcept {
		return (flags & flag) == flag;
	}
	UIFlag UI::getFlag(UIFlag flag) const noexcept {
		return flags & flag;
	}
	void UI::setFlag(UIFlag mask, bool isTrue) noexcept {
		UIFlag f = isTrue ? mask : UIFlag::none;
		flags = (flags & ~mask) | f;
	}
	void UI::setFlag(UIFlag mask, UIFlag flag) noexcept {
		flags = (flags & ~mask) | flag;
	}


	void UI::addThisAndChildrenFlagIfNotHas(UIFlag flag) noexcept {
		if (hasAnyFlag(flag)) {
			return;
		}
		addFlag(flag);
		uint count = children.count();
		for (uint i = 0; i < count; ++i) {
			children[i]->addThisAndChildrenFlagIfNotHas(flag);
		}
	}

		
	void UI::addThisAndChildrenRenderingFlagUntilTypeIsHidden() noexcept {
		if (displayType == UIDisplayType::hidden) {
			return;
		}
		setFlag(UIFlag::isRendering, displayType == UIDisplayType::display);
		uint count = children.count();
		for (uint i = 0; i < count; ++i) {
			children[i]->addThisAndChildrenRenderingFlagUntilTypeIsHidden();
		}
	}

	void UI::changeParentSetFlagsImpl(UIFlag flag) noexcept {
		if (displayType == UIDisplayType::hidden) {
			flag |= UIFlag::isHidden;
		}
		bool rendering = (flag & UIFlag::isHidden) == UIFlag::none && displayType == UIDisplayType::display;
		setFlag(UIFlag::isInRootTree | UIFlag::needUpdateBox | UIFlag::isHidden | UIFlag::isRendering,
			flag | (rendering ? UIFlag::isRendering : UIFlag::none));
		uint count = children.count();
		for (uint i = 0; i < count; ++i) {
			children[i]->changeParentSetFlagsImpl(flag);
		}
	}
	void UI::changeParentSetFlags() noexcept {
		changeParentSetFlagsImpl(parent ? parent->getFlag(UIFlag::isInRootTree | UIFlag::needUpdateBox | UIFlag::isHidden) : UIFlag::none);
	}

	bool UI::isInNeedUpdateSet() const noexcept {
		return hasAnyFlag(UIFlag::needUpdateBox) && (!parent || !parent->hasAnyFlag(UIFlag::needUpdateBox));
	}

	bool UI::needBVH() const noexcept {
		return hasAllFlag(UIFlag::isRendering | UIFlag::isTriggerable);
	}
	void UI::needUpdate() noexcept {
		UITree::NeedUpdate(*this);
	}

	Abox2 UI::_ParentBox(const UI& u) noexcept {
		return u.parent ? u.parent->box : Abox2(Float2(), Float2(tof(Game::GetWinWidth()), tof(Game::GetWinHeight())));
	}
	void UI::_RemoveChildren(bool parentIsInNeedUpdateSet, UI& u) {
		//从后往前删除子节点
		for (uint i = u.children.count(); i > 0; ) {
			--i;
			UI& child = *u.children[i];
			if (!parentIsInNeedUpdateSet) {
				parentIsInNeedUpdateSet = child.hasAnyFlag(UIFlag::needUpdateBox);
				if (parentIsInNeedUpdateSet) {
					UITree::RemoveNeedUpdate(u);
				}
			}
			UI::_RemoveChildren(parentIsInNeedUpdateSet, child);
			UI::_RemoveMesh(child);
			u.children.remove();
			child.childrenIndex.setNull();
			child.parent = null;
			child.removeImpl();
		}
	}
	void UI::_RemoveThisAndChildrenMesh(UI& u) {
		uint count = u.children.count();
		for (uint i = 0; i < count; ++i) {
			UI::_RemoveThisAndChildrenMesh(*u.children[i]);
		}
		_meshGroup.remove(u);
	}
	void UI::_RemoveMesh(UI& u) {
		_meshGroup.remove(u);
	}
	void UI::_UpdateThisAndChildrenMesh(UI& u) {
		if (!u.getIsRendering()) {
			return;
		}
		uint count = u.children.count();
		for (uint i = 0; i < count; ++i) {
			UI::_UpdateThisAndChildrenMesh(*u.children[i]);
		}
		_meshGroup.update(u);
	}
	void UI::_UpdateMesh(UI& u) {
		if (!u.getIsRendering()) {
			return;
		}
		_meshGroup.update(u);
	}

	void UI::_RemoveBVH(UI& u) noexcept {
		_bvh.remove(u);
	}
	void UI::_UpdateBVH(UI& u) noexcept {
		_bvh.updateAbox(u);
	}
	void UI::_UpdateThisAndChildrenBVH(UI& u) noexcept {
		//_bvh.updateThisAndAllChildrenAbox(u);
	}
	void UI::_DisconnectBVH(UI& u) noexcept {
		//_bvh.disconnect(u);
	}

	void UI::removeThis() {
		UI::Remove(*this);
	}
	void UI::Remove(UI& u) {
		if (&u == &UI::Root()) [[unlikely]] {
			return;
		}
		bool inNeedUpdateSet = u.isInNeedUpdateSet();
		if (inNeedUpdateSet) {
			UITree::RemoveNeedUpdate(u);
		}
		UI::_RemoveBVH(u);
		//删除子节点
		UI::_RemoveChildren(inNeedUpdateSet, u);
		//删除自身
		UI::_RemoveMesh(u);
		if (u.parent) {
			u.parent->children.last()->childrenIndex = u.childrenIndex;
			u.parent->children.removeSwapTail(u.childrenIndex);
			u.parent = null;
			u.childrenIndex.setNull();
		}
		u.removeImpl();
	}

	uid UI::getUIArrayIndex() const noexcept {
		return uiArrayIndex;
	}
	UIMeshKey UI::getUIMeshKey() const noexcept {
		return UIMeshKey(zIndex, meshType);
	}

	UI& UI::setGroupZIndex(const sint z) {
		_meshGroup.setGroupZIndex(*this, z); //这里由UIMesh来调用UITree::NeedUpdate
		return *this;
	}
	UI& UI::setZIndex(const sint z) {
		_meshGroup.setZIndex(*this, z); //这里由UIMesh来调用UITree::NeedUpdate
		return *this;
	}
	sint UI::getZIndex() const noexcept {
		return zIndex;
	}

	UI& UI::setParent(UI& u) {
		UI* ptr = &u;
		do {
			if (ptr == this) { //形成环路
				return *this;
			}
			ptr = ptr->parent;
		} while (ptr);
		bool inNeedUpdateSet = isInNeedUpdateSet();
		if (parent) {
			parent->children.last()->childrenIndex = childrenIndex;
			parent->children.removeSwapTail(childrenIndex);
		}
		parent = &u;
		childrenIndex = parent->children.count();
		parent->children.add(this);
		changeParentSetFlags();
		if (parent->hasAnyFlag(UIFlag::needUpdateBox)) {
			if (inNeedUpdateSet) {
				UITree::RemoveNeedUpdate(*this);
			}
		}
		else if (!inNeedUpdateSet) {
			UITree::NeedUpdate(*this);
		}
		return *this;
	}
	UI& UI::setParentIsRoot() {
		return setParent(UI::Root());
	}

	UI& UI::disconnectParent() {
		if (parent) {
			parent->children.last()->childrenIndex = childrenIndex;
			parent->children.removeSwapTail(childrenIndex);
			parent = null;
			childrenIndex.setNull();
			changeParentSetFlags();
			UITree::NeedUpdate(*this);
		}
		return *this;
	}
	UI* UI::getParent() noexcept {
		return parent;
	}
	const UI* UI::getParent() const noexcept {
		return parent;
	}

	UI& UI::addChild(UI& child) {
		child.setParent(*this);
		return *this;
	}
	UI& UI::removeChild(UI& child) {
		if (child.parent == this) {
			UI::Remove(child);
		}
		return *this;
	}
	const DynArr<const UI*>& UI::getChildren() const noexcept {
		return reinterpret_cast<const DynArr<const UI*>&>(children);
	}

	UI& UI::setMinSideRatio(const bool minSideRatio) noexcept {
		setFlag(UIFlag::minSideRatio, minSideRatio);
		UITree::NeedUpdate(*this);
		return *this;
	}
	bool UI::getMinSideRatio() const noexcept {
		return hasAnyFlag(UIFlag::minSideRatio);
	}


	UIDisplayType UI::getDisplayType() const noexcept {
		return displayType;
	}
	UI& UI::setDisplayType(UIDisplayType displayType_) noexcept {
		if(displayType == displayType_) {
			return *this;
		}
		displayType = displayType_;
		if (displayType_ == UIDisplayType::display || displayType_ == UIDisplayType::layout) {
			if (!(parent && parent->hasAnyFlag(UIFlag::isHidden))) {
				addThisAndChildrenRenderingFlagUntilTypeIsHidden();
			}
		}
		else if (displayType_ == UIDisplayType::hidden) {
			if (!hasAnyFlag(UIFlag::isHidden)) {
				setThisAndChildrenFlag(UIFlag::isHidden | UIFlag::isRendering, UIFlag::isHidden);
			}
		}
		UITree::NeedUpdate(*this);
		return *this;
	}

	bool UI::getIsRendering() const noexcept {
		return hasAnyFlag(UIFlag::isRendering);
	}


	UI& UI::setIsTriggerable(bool isTriggerable) noexcept {
		setFlag(UIFlag::isTriggerable, isTriggerable);
		UITree::NeedUpdate(*this);
		return *this;
	}
	bool UI::getIsTriggerable() const noexcept {
		return hasAnyFlag(UIFlag::isTriggerable);
	}

	UIut UI::getWidth() const noexcept {
		return UIut(hasAnyFlag(UIFlag::widthIsPixel), widthHeight.x);
	}
	UIut UI::getHeight() const noexcept {
		return UIut(hasAnyFlag(UIFlag::heightIsPixel), widthHeight.y);
	}
	Float2& UI::refWidthHeightVal() noexcept {
		return widthHeight;
	}
	UI& UI::setWidth(UIut width) noexcept {
		widthHeight.x = width.val();
		setFlag(UIFlag::widthIsPixel, width.ispx());
		UITree::NeedUpdate(*this);
		return *this;
	}
	UI& UI::setHeight(UIut height) noexcept {
		widthHeight.y = height.val();
		setFlag(UIFlag::heightIsPixel, height.ispx());
		UITree::NeedUpdate(*this);
		return *this;
	}
	UI& UI::setWidthHeight(UIut width, UIut height) noexcept {
		widthHeight.x = width.val();
		widthHeight.y = height.val();
		setFlag(UIFlag::widthIsPixel | UIFlag::heightIsPixel,
			(width.ispx() ? UIFlag::widthIsPixel : UIFlag::none)
			| (height.ispx() ? UIFlag::heightIsPixel : UIFlag::none)
		);
		UITree::NeedUpdate(*this);
		return *this;
	}
	bool UI::getWidthIsPixel() const noexcept {
		return hasAnyFlag(UIFlag::widthIsPixel);
	}
	UI& UI::setWidthIsPixel(const bool widthIsPixel) noexcept {
		setFlag(UIFlag::widthIsPixel, widthIsPixel);
		UITree::NeedUpdate(*this);
		return *this;
	}
	bool UI::getHeightIsPixel() const noexcept {
		return hasAnyFlag(UIFlag::heightIsPixel);
	}
	UI& UI::setHeightIsPixel(const bool heightIsPixel) noexcept {
		setFlag(UIFlag::heightIsPixel, heightIsPixel);
		UITree::NeedUpdate(*this);
		return *this;
	}

	UIut UI::getAnchorsX() const noexcept {
		return UIut(hasAnyFlag(UIFlag::anchorsXIsPixel), anchors.x);
	}
	UIut UI::getAnchorsY() const noexcept {
		return UIut(hasAnyFlag(UIFlag::anchorsYIsPixel), anchors.y);
	}
	Float2& UI::refAnchorsVal() noexcept {
		return anchors;
	}
	UI& UI::setAnchorsX(UIut anchors_) noexcept {
		anchors.x = anchors_.val();
		setFlag(UIFlag::anchorsXIsPixel, anchors_.ispx());
		UITree::NeedUpdate(*this);
		return *this;
	}
	UI& UI::setAnchorsY(UIut anchors_) noexcept {
		anchors.y = anchors_.val();
		setFlag(UIFlag::anchorsYIsPixel, anchors_.ispx());
		UITree::NeedUpdate(*this);
		return *this;
	}
	UI& UI::setAnchors(UIut anchorsX, UIut anchorsY) noexcept {
		anchors.x = anchorsX.val();
		anchors.y = anchorsY.val();
		setFlag(UIFlag::anchorsXIsPixel | UIFlag::anchorsYIsPixel,
			(anchorsX.ispx() ? UIFlag::anchorsXIsPixel : UIFlag::none)
			| (anchorsY.ispx() ? UIFlag::anchorsYIsPixel : UIFlag::none)
		);
		UITree::NeedUpdate(*this);
		return *this;
	}
	bool UI::getAnchorsXIsPixel() const noexcept {
		return hasAnyFlag(UIFlag::anchorsXIsPixel);
	}
	UI& UI::setAnchorsXIsPixel(const bool anchorsXIsPixel) noexcept {
		setFlag(UIFlag::anchorsXIsPixel, anchorsXIsPixel);
		UITree::NeedUpdate(*this);
		return *this;
	}
	bool UI::getAnchorsYIsPixel() const noexcept {
		return hasAnyFlag(UIFlag::anchorsYIsPixel);
	}
	UI& UI::setAnchorsYIsPixel(const bool anchorsYIsPixel) noexcept {
		setFlag(UIFlag::anchorsYIsPixel, anchorsYIsPixel);
		UITree::NeedUpdate(*this);
		return *this;
	}

	UIut UI::getPivotX() const noexcept {
		return UIut(hasAnyFlag(UIFlag::pivotXIsPixel), pivot.x);
	}
	UIut UI::getPivotY() const noexcept {
		return UIut(hasAnyFlag(UIFlag::pivotYIsPixel), pivot.y);
	}
	Float2& UI::refPivotVal() noexcept {
		return pivot;
	}
	UI& UI::setPivotX(UIut pivot_) noexcept {
		pivot.x = pivot_.val();
		setFlag(UIFlag::pivotXIsPixel, pivot_.ispx());
		UITree::NeedUpdate(*this);
		return *this;
	}
	UI& UI::setPivotY(UIut pivot_) noexcept {
		pivot.y = pivot_.val();
		setFlag(UIFlag::pivotYIsPixel, pivot_.ispx());
		UITree::NeedUpdate(*this);
		return *this;
	}
	UI& UI::setPivot(UIut pivotX, UIut pivotY) noexcept {
		pivot.x = pivotX.val();
		pivot.y = pivotY.val();
		setFlag(UIFlag::pivotXIsPixel | UIFlag::pivotYIsPixel,
			(pivotX.ispx() ? UIFlag::pivotXIsPixel : UIFlag::none)
			| (pivotY.ispx() ? UIFlag::pivotYIsPixel : UIFlag::none)
		);
		UITree::NeedUpdate(*this);
		return *this;
	}
	bool UI::getPivotXIsPixel() const noexcept {
		return hasAnyFlag(UIFlag::pivotXIsPixel);
	}
	UI& UI::setPivotXIsPixel(const bool pivotXIsPixel) noexcept {
		setFlag(UIFlag::pivotXIsPixel, pivotXIsPixel);
		UITree::NeedUpdate(*this);
		return *this;
	}
	bool UI::getPivotYIsPixel() const noexcept {
		return hasAnyFlag(UIFlag::pivotYIsPixel);
	}
	UI& UI::setPivotYIsPixel(const bool pivotYIsPixel) noexcept {
		setFlag(UIFlag::pivotYIsPixel, pivotYIsPixel);
		UITree::NeedUpdate(*this);
		return *this;
	}


	bool UI::getAnchorsOnRoot() const noexcept {
		return hasAnyFlag(UIFlag::anchorsOnRoot);
	}
	UI& UI::setAnchorsOnRoot(const bool anchorsOnRoot) noexcept {
		setFlag(UIFlag::anchorsOnRoot, anchorsOnRoot);
		UITree::NeedUpdate(*this);
		return *this;
	}


	UI& UI::setPivotAndAnchorsOnLowerLeft() noexcept {
		anchors = Float2();
		pivot = Float2();
		removeFlag(UIFlag::anchorsXIsPixel | UIFlag::anchorsYIsPixel | UIFlag::pivotXIsPixel | UIFlag::pivotYIsPixel);
		UITree::NeedUpdate(*this);
		return *this;
	}
	UI& UI::setPivotAndAnchorsOnUpperLeft() noexcept {
		anchors = Float2(0, 1);
		pivot = Float2(0, 1);
		removeFlag(UIFlag::anchorsXIsPixel | UIFlag::anchorsYIsPixel | UIFlag::pivotXIsPixel | UIFlag::pivotYIsPixel);
		UITree::NeedUpdate(*this);
		return *this;
	}
	UI& UI::setPivotAndAnchorsOnLowerRight() noexcept {
		anchors = Float2(1, 0);
		pivot = Float2(1, 0);
		removeFlag(UIFlag::anchorsXIsPixel | UIFlag::anchorsYIsPixel | UIFlag::pivotXIsPixel | UIFlag::pivotYIsPixel);
		UITree::NeedUpdate(*this);
		return *this;
	}
	UI& UI::setPivotAndAnchorsOnUpperRight() noexcept {
		anchors = Float2(1, 1);
		pivot = Float2(1, 1);
		removeFlag(UIFlag::anchorsXIsPixel | UIFlag::anchorsYIsPixel | UIFlag::pivotXIsPixel | UIFlag::pivotYIsPixel);
		UITree::NeedUpdate(*this);
		return *this;
	}
	UI& UI::setPivotAndAnchorsOnCenter() noexcept {
		anchors = Float2(0.5, 0.5);
		pivot = Float2(0.5, 0.5);
		removeFlag(UIFlag::anchorsXIsPixel | UIFlag::anchorsYIsPixel | UIFlag::pivotXIsPixel | UIFlag::pivotYIsPixel);
		UITree::NeedUpdate(*this);
		return *this;
	}
	UI& UI::setPivotAndAnchorsOnLeft() noexcept {
		anchors = Float2(0, 0.5);
		pivot = Float2(0, 0.5);
		removeFlag(UIFlag::anchorsXIsPixel | UIFlag::anchorsYIsPixel | UIFlag::pivotXIsPixel | UIFlag::pivotYIsPixel);
		UITree::NeedUpdate(*this);
		return *this;
	}
	UI& UI::setPivotAndAnchorsOnRight() noexcept {
		anchors = Float2(1, 0.5);
		pivot = Float2(1, 0.5);
		removeFlag(UIFlag::anchorsXIsPixel | UIFlag::anchorsYIsPixel | UIFlag::pivotXIsPixel | UIFlag::pivotYIsPixel);
		UITree::NeedUpdate(*this);
		return *this;
	}
	UI& UI::setPivotAndAnchorsOnLower() noexcept {
		anchors = Float2(0.5, 0);
		pivot = Float2(0.5, 0);
		removeFlag(UIFlag::anchorsXIsPixel | UIFlag::anchorsYIsPixel | UIFlag::pivotXIsPixel | UIFlag::pivotYIsPixel);
		UITree::NeedUpdate(*this);
		return *this;
	}
	UI& UI::setPivotAndAnchorsOnUpper() noexcept {
		anchors = Float2(0.5, 1);
		pivot = Float2(0.5, 1);
		removeFlag(UIFlag::anchorsXIsPixel | UIFlag::anchorsYIsPixel | UIFlag::pivotXIsPixel | UIFlag::pivotYIsPixel);
		UITree::NeedUpdate(*this);
		return *this;
	}

	const Abox2& UI::getBox() noexcept {
		updateBoxIfNeed();
		return box;
	}
	const Abox2& UI::getBox() const noexcept {
		return box;
	}

	bool UI::pointInUI(const Float2& point) const noexcept {
		return true;
	}

	void UI::editMesh(graph::MeshEditorBase& meshEditor, const bool isAdd) noexcept {
		 //auto& editor = meshEditor.cast<graph::RectUIVertex>();
		 //editor.resize({ 4, 6 });
	}


	static UI* _clickUI = null;
	UI* UI::GetClickUI() noexcept {
		return _clickUI;
	}
	static UI* _holdUI = null;
	UI* UI::GetHoldUI() noexcept {
		return _holdUI;
	}
	static UI* _releaseUI = null;
	UI* UI::GetReleaseUI() noexcept {
		return _releaseUI;
	}
	static UI* _hoverUI = null;
	UI* UI::GetHoverUI() noexcept {
		return _hoverUI;
	}
	static UI* _leaveUI = null;
	UI* UI::GetLeaveUI() noexcept {
		return _leaveUI;
	}

	static Matrix4& _Projection() noexcept;

	void UI::_UpdateWinSize(sint width, sint height) noexcept {
		auto& _projection = _Projection();
		_projection[0][0] = 2.0f / tof(width);
		_projection[1][1] = 2.0f / tof(height);

		//计算sizeFactor
		/*
		float sizeRatio; //整个屏幕与窗口大小的比值，根据该值来调整UI大小 //TODO 应该考虑比值有个最小值(根据屏幕大小来)，否则UI太小看不清
		if (width < height) {
			sizeRatio = Game::GetScreenWidth() / tof(width);
		}
		else {
			sizeRatio = Game::GetScreenHeight() / tof(height);
		}
		*/

		//TODO 分为3档
		/*
		if (sizeRatio < 1.51f) {
			sizeFactor = sizeRatio;
		}
		else if(sizeRatio < 2.01f) {
			sizeFactor = 2.0f;
		}
		else {
			sizeFactor = 3.0f;
		}
		*/

		UITree::NeedUpdate(UI::Root());
	}

	static Matrix4& _Projection() noexcept {
		static Matrix4 _projection = Matrix4::Orthographic(
			0.0f, tof(Game::GetWinWidth()),
			0.0f, tof(Game::GetWinHeight()),
			-1048576.0f, 1048576.0f
		);
		return _projection;
	}

	const Matrix4& UI::GetProjection() noexcept {
		return _Projection();
	}

	UI& UI::Root() noexcept {
		static UI& _root = UI::_InitRoot();
		return _root;
	}

	void UITree::RemoveNeedUpdate(UI& u) noexcept {
		needUpdateUISet.remove(&u);
	}
	void UITree::NeedUpdateChildren(UI* u) noexcept {
		if (u->hasAnyFlag(UIFlag::needUpdateBox)) {
			needUpdateUISet.remove(u);
			return;
		}
		u->addFlag(UIFlag::needUpdateBox);
		uint count = u->children.count();
		for (uint i = 0; i < count; ++i) {
			NeedUpdateChildren(u->children[i]);
		}
	}
	void UITree::NeedUpdate(UI& u) noexcept {
		if (u.hasAnyFlag(UIFlag::needUpdateBox)) {
			return;
		}
		uint count = u.children.count();
		for (uint i = 0; i < count; ++i) {
			NeedUpdateChildren(u.children[i]);
		}
		u.addFlag(UIFlag::needUpdateBox);
		needUpdateUISet.add(&u);
	}

	//记录上一帧和当前帧光标在UI范围内悬停的控件
	static UI* _frame_hover_ui[2] = { null, null };
	//记录上一帧和当前帧光标在UI范围内按住的控件
	static UI* _frame_hold_ui[2] = { null, null };
	//_frameHoldUI数组的下标
	static bool _now_frame_index = false;


	void UITree::Update() {
		ui::BlockItemIcon::UpdateMVP();

		needUpdateUISet.each([&](UI*& u) {
			u->updateBox();
		});
		needUpdateUISet.clear();

		UI* u = null;
		Float2 cursorPos;
		bool _pre_frame_index = _now_frame_index;
		_now_frame_index = !_now_frame_index; //当前帧变为上一帧
		_frame_hover_ui[_now_frame_index] = null; //清除上上帧
		_frame_hold_ui[_now_frame_index] = null;
		if (!input::Input::GetCursorIsForceCenter()) {
			cursorPos = input::Input::GetCursorInUIPositionF();
			u = UI::_bvh.queryCollision(cursorPos);
			_frame_hover_ui[_now_frame_index] = u;


			//if (input::Input::IsPress(input::InputAction::ClickMouseLeftButton)) {
			//	UI* TESTu = UI::_bvh.queryCollision(cursorPos);
			//}
		}
		//触发顺序 leave hover click release hold
		if (_frame_hover_ui[_pre_frame_index] && _frame_hover_ui[_pre_frame_index] != _frame_hover_ui[_now_frame_index]) {
			_frame_hover_ui[_pre_frame_index]->leave(UIEvent(*_frame_hover_ui[_pre_frame_index], cursorPos, UIEventType::Leave));
			_leaveUI = _frame_hover_ui[_pre_frame_index];
		}
		if (u) {
			u->hover(UIEvent(*u, cursorPos, UIEventType::Hover));
			_hoverUI = u;

			bool maybeHold = true;
			if (input::Input::IsPress(input::InputAction::ClickMouseLeftButton)) {
				maybeHold = false;
				u->click(UIEvent(*u, cursorPos, UIEventType::Click, input::InputAction::ClickMouseLeftButton));
				_clickUI = u;
			}
			if (input::Input::IsRelease(input::InputAction::ClickMouseLeftButton)) {
				maybeHold = false;
				u->release(UIEvent(*u, cursorPos, UIEventType::Release, input::InputAction::ClickMouseLeftButton));
				_releaseUI = u;
			}

			if (maybeHold && input::Input::IsDown(input::InputAction::ClickMouseLeftButton)) {
				_frame_hold_ui[_now_frame_index] = u;
				u->hold(UIEvent(*u, cursorPos, UIEventType::Hold, input::InputAction::ClickMouseLeftButton));
				_holdUI = u;
			}
		}
	}

	void UITree::Draw() {
		graph::Content::DisableDepthTest();
		graph::Content::DisableCullFace();

		UI::_meshGroup.draw();

		graph::Content::EnableDepthTest();
		graph::Content::EnableCullFace();
	}
}
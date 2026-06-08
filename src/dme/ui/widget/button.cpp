#include <dme/ui/widget/button.h>
namespace dme::ui {
	graph::TexIndex Button::texNormal = DME_SVAR_DELAY_FUNC(Button::texNormal, graph::TexAtlas::Default().addIfAbsent, DME_PATH_TEX_UI "button.png");
	graph::TexIndex Button::texHover = DME_SVAR_DELAY_FUNC(Button::texHover, graph::TexAtlas::Default().addIfAbsent, DME_PATH_TEX_UI "button_hover.png");
	graph::TexIndex Button::texDisabled = DME_SVAR_DELAY_FUNC(Button::texDisabled, graph::TexAtlas::Default().addIfAbsent, DME_PATH_TEX_UI "button_disabled.png");

	Button::Button(uint meshType, void (*clickFunc)(), ushort border) noexcept : UI(meshType), buttonState(ButtonState::normal), border(border), clickFunc(clickFunc) {}

	Button& Button::setButtonNormal() noexcept {
		buttonState = ButtonState::normal;
		UITree::NeedUpdate(*this);
		return *this;
	}
	Button& Button::setButtonHover() noexcept {
		buttonState = ButtonState::hover;
		UITree::NeedUpdate(*this);
		return *this;
	}
	Button& Button::setButtonDisabled() noexcept {
		buttonState = ButtonState::disabled;
		UITree::NeedUpdate(*this);
		return *this;
	}
	Button& Button::setClickFunc(void (*clickFunc_)()) {
		clickFunc = clickFunc_;
		return *this;
	}
	void (*Button::getClickFunc() const noexcept)() {
		return clickFunc;
	}

	void Button::click(const UIEvent& e) {
		if (buttonState != ButtonState::disabled && clickFunc) {
			clickFunc();
		}
	}
	void Button::hold(const UIEvent& e) {}
	void Button::release(const UIEvent& e) {}
	void Button::hover(const UIEvent& e) {
		if (buttonState != ButtonState::disabled) {
			setButtonHover();
		}
	}
	void Button::leave(const UIEvent& e) {
		if (buttonState != ButtonState::disabled) {
			setButtonNormal();
		}
	}

	void Button::editMesh(graph::MeshEditorBase& meshEditor, const bool isAdd) noexcept {
		auto& editor = meshEditor.cast<graph::RectUIVertex>();
		if (isAdd) {
			/*
				12 13 14 15
				 8  9 10 11
				 4  5  6  7
				 0  1  2  3
			*/
			editor.setIndex({
				 0,  4,  5,  0,  5,  1,
				 1,  5,  6,  1,  6,  2,
				 2,  6,  7,  2,  7,  3,
				 4,  8,  9,  4,  9,  5,
				 5,  9, 10,  5, 10,  6,
				 6, 10, 11,  6, 11,  7,
				 8, 12, 13,  8, 13,  9,
				 9, 13, 14,  9, 14, 10,
				10, 14, 15, 10, 15, 11,
			});
		}
		graph::TexIndex texIndex;
		switch (buttonState) {
		case ButtonState::normal:
			texIndex = texNormal;
			break;
		case ButtonState::hover:
			texIndex = texHover;
			break;
		case ButtonState::disabled:
			texIndex = texDisabled;
			break;
		}
		auto texPos = graph::TexAtlas::Default().getPos(texIndex);
		auto& box = getBox();

		float x0 = box.min.x;
		float x1 = x0 + border;
		float x2 = box.max.x - border;
		if (x2 < x1) x2 = x1;
		float x3 = x2 + border;

		float y0 = box.min.y;
		float y1 = y0 + border;
		float y2 = box.max.y - border;
		if (y2 < y1) y2 = y1;
		float y3 = y2 + border;

		sint tz = texPos.z();

		sint tx0 = texPos.x0();
		sint tx1 = tx0 + border;
		sint tx3 = texPos.x1();
		sint tx2 = tx3 - border;

		sint ty0 = texPos.y0();
		sint ty1 = ty0 + border;
		sint ty3 = texPos.y1();
		sint ty2 = ty3 - border;

		editor.setVertex({
			graph::RectUIVertex(x0, y0, tx0, ty0, tz),
			graph::RectUIVertex(x1, y0, tx1, ty0, tz),
			graph::RectUIVertex(x2, y0, tx2, ty0, tz),
			graph::RectUIVertex(x3, y0, tx3, ty0, tz),

			graph::RectUIVertex(x0, y1, tx0, ty1, tz),
			graph::RectUIVertex(x1, y1, tx1, ty1, tz),
			graph::RectUIVertex(x2, y1, tx2, ty1, tz),
			graph::RectUIVertex(x3, y1, tx3, ty1, tz),

			graph::RectUIVertex(x0, y2, tx0, ty2, tz),
			graph::RectUIVertex(x1, y2, tx1, ty2, tz),
			graph::RectUIVertex(x2, y2, tx2, ty2, tz),
			graph::RectUIVertex(x3, y2, tx3, ty2, tz),

			graph::RectUIVertex(x0, y3, tx0, ty3, tz),
			graph::RectUIVertex(x1, y3, tx1, ty3, tz),
			graph::RectUIVertex(x2, y3, tx2, ty3, tz),
			graph::RectUIVertex(x3, y3, tx3, ty3, tz),
		});
	}
}
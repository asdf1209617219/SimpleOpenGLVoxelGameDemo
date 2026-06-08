#pragma once
#include <dme/ui/ui.h>
namespace dme::ui {
	enum class ButtonState : uchar {
		normal = 0,
		hover = 1,
		disabled = 2,
	};

	class Button : public UI, private ObjPool<Button> {
		friend ObjPool<Button>;
	private:
		virtual void removeImpl() noexcept override {
			ObjPool<Button>::Remove(this);
		}

		static graph::TexIndex texNormal;
		static graph::TexIndex texHover;
		static graph::TexIndex texDisabled;
	protected:
		Button(uint meshType, void (*clickFunc)(), ushort border = 2) noexcept;

		ButtonState buttonState;
		ushort border;
		void (*clickFunc)();
	public:
		static constexpr size_t obj_pool_size = 16;
		static void ShaderReadyFunc(const graph::Shader<graph::RectUIVertex>& shader) {
			shader.setTexAtlas("aTexture", graph::TexAtlas::Default(), 0);
			shader.setInt("aTextureSize", graph::TexAtlas::Default().getSize());
			shader.setMatrix("projection", UI::GetProjection());
		}
		static const auto& GetShader() {
			return graph::Shaders::UI();
		}
		static inline const graph::VertexIndexCount UseVertexIndexCount = graph::VertexIndexCount(36, 54);
		static graph::IUIMesh* CreateMesh() {
			return graph::IUIMesh::Create(GetShader().getType(), 576, 864, !UseVertexIndexCount.empty());
		}
		static uint GetMeshType() {
			return graph::UIMeshBase<Button>::GetMeshType();
		}
		static Button& Create(void (*clickFunc)()) noexcept {
			auto& r = *ObjPool<Button>::Add(Button::GetMeshType(), clickFunc);
			return r;
		}

		Button& setButtonNormal() noexcept;
		Button& setButtonHover() noexcept;
		Button& setButtonDisabled() noexcept;

		Button& setClickFunc(void (*clickFunc)());
		void (*getClickFunc() const noexcept)();

		virtual void click(const UIEvent& e) override;
		virtual void hold(const UIEvent& e) override;
		virtual void release(const UIEvent& e) override;
		virtual void hover(const UIEvent& e) override;
		virtual void leave(const UIEvent& e) override;

		virtual void editMesh(graph::MeshEditorBase& meshEditor, const bool isAdd) noexcept override;
	};
}
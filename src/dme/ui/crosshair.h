#pragma once
#include <dme/graph/buffer/frame_buffer.h>
#include <dme/graph/texture/tex2d.h>
#include <dme/ui/ui.h>
namespace dme::ui {
	class Crosshair : public UI, private ObjPool<Crosshair> {
	private:
		friend ObjPool<Crosshair>;

		virtual void removeImpl() noexcept override {
			ObjPool<Crosshair>::Remove(this);
		}
	protected:
		Crosshair(uint meshType) noexcept;
	public:

		static void ShaderReadyFunc(const graph::Shader<graph::VertexPos2>& shader) {
			shader.setMatrix("projection", UI::GetProjection());
			shader.setTex2d("screenTexture", Game::ScreenFBO.getTex(), 0);
		}
		static const auto& GetShader() {
			return graph::Shaders::ReverseColor();
		}
		static inline const graph::VertexIndexCount UseVertexIndexCount = graph::VertexIndexCount(8, 12);
		static graph::IUIMesh* CreateMesh() {
			return graph::IUIMesh::Create(GetShader().getType(), 8, 12, !UseVertexIndexCount.empty());
		}
		static uint GetMeshType() {
			return graph::UIMeshBase<Crosshair>::GetMeshType();
		}
		static Crosshair& Create() noexcept {
			auto& r = *ObjPool<Crosshair>::Add(Crosshair::GetMeshType());
			r.setIsTriggerable(false)
				.setZIndex(-100000)
				.setPivotAndAnchorsOnCenter()
				.setWidthHeight(50_px, 50_px)
				.setParentIsRoot()
				;
			return r;
		}

		virtual void editMesh(graph::MeshEditorBase& meshEditor, const bool isAdd) noexcept override;
	};
}

#include <dme/ui/crosshair.h>
#include <dme/graph/shader.h>
#include <dme/graph/buffer/frame_buffer.h>

namespace dme::ui {
	using graph::ShaderBase;
	using graph::VertexIndexCount;
	using graph::VertexIndexEditor;
	using graph::VertexPos2;

	Crosshair::Crosshair(uint meshType) noexcept : UI(meshType) {}
	void Crosshair::editMesh(graph::MeshEditorBase& meshEditor, const bool isAdd) noexcept {
		auto& editor = meshEditor.cast<graph::VertexPos2>();
		if (isAdd) {
			editor.setIndex({
				0, 1, 2, 0, 2, 3,
				4, 5, 6, 4, 6, 7,
			});
		}
		auto& box = getBox();
		float halfLength = math::Clamp((box.max.x - box.min.x) * 0.5f, 10.0f, 20.0f);
		float halfWidth = math::Clamp(halfLength * 0.05f, 0.5f, 1.5f);
		float midX = box.min.x + (box.max.x - box.min.x) * 0.5f;
		float midY = box.min.y + (box.max.y - box.min.y) * 0.5f;

		editor.setVertex({
			graph::VertexPos2(midX - halfLength, midY - halfWidth),
			graph::VertexPos2(midX - halfLength, midY + halfWidth),
			graph::VertexPos2(midX + halfLength, midY + halfWidth),
			graph::VertexPos2(midX + halfLength, midY - halfWidth),

			graph::VertexPos2(midX - halfWidth, midY - halfLength),
			graph::VertexPos2(midX - halfWidth, midY + halfLength),
			graph::VertexPos2(midX + halfWidth, midY + halfLength),
			graph::VertexPos2(midX + halfWidth, midY - halfLength),
		});
	}

}
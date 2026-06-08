#include <dme/graph/mesh/mesh_editor.h>
#include <dme/ui/ui.h>
namespace dme::graph {
	MeshEditorBase::MeshEditorBase(IUIMesh* mesh, ui::UI* u, const bool isFixedCount) noexcept
		: mesh(mesh), u(u), isFixedCount(isFixedCount), start(u->meshSegment.vertexStart) {}

	const stdstr& MeshEditorBase::_uiName(ui::UI* u) noexcept {
		return u->name.str();
	}
	const MeshSegment& MeshEditorBase::_meshSegment(ui::UI* u) noexcept {
		return u->meshSegment;
	}
}
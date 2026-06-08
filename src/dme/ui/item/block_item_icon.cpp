#include <dme/ui/item/block_item_icon.h>
#include <dme/item/proto/block_item_proto.h>
#include <dme/block/proto/block_proto.h>

namespace dme::ui {
	void BlockItemIcon::UpdateMVP() noexcept {
		float delta = Game::GetDeltaTimeF();
		_camera_model.rotateZThis(-delta);
		_model.rotateZThis(delta);
		_MVP() = UI::GetProjection() * _view * _model;
	}
	const Matrix4& BlockItemIcon::GetMVP() noexcept {
		return _MVP();
	}

	void BlockItemIcon::ShaderReadyFunc(const graph::Shader<graph::VertexBlockItemIcon>& shader) {
		shader.setTex2dArray("textureArray", graph::Tex2dArray::Cube(), 0);
		shader.setMatrix("projection", UI::GetProjection());
		shader.setMatrix("mvp", BlockItemIcon::GetMVP());
		shader.setFloat3("cameraPos", (_camera_model * Float4(1, -1, 1, 1)).vxyz);

		shader.setFloat("material.specular", 1.0f);
		shader.setFloat("material.shininess", 32.0f);
		shader.setFloat3("dirLight.direction", (_camera_model * Float4(-math::sqrt3_inv, math::sqrt3_inv, -math::sqrt3_inv, 1)).vxyz);
		shader.setFloat3("dirLight.ambient", Float3(0.4f));
		shader.setFloat3("dirLight.diffuse", Float3(0.8f));
		shader.setFloat3("dirLight.specular", Float3(1.0f));
	}

	void BlockItemIcon::removeImpl() noexcept {
		ObjPool<BlockItemIcon>::Remove(this);
	}
	BlockItemIcon::BlockItemIcon(uint meshType, const item::ItemStack& itemStack) noexcept : ItemIcon(meshType, itemStack) {}

	BlockItemIcon& BlockItemIcon::Create(const item::ItemStack& itemStack) noexcept {
		auto& r = *ObjPool<BlockItemIcon>::Add(BlockItemIcon::GetMeshType(), itemStack);
		return r;
	}

	void BlockItemIcon::editMesh(graph::MeshEditorBase& meshEditor, const bool isAdd) noexcept {
		auto& stack = getItemStack();
		if (stack.isEmpty()) {
			return;
		}
		auto& itemProto = stack.getProto();
		auto& blockItemProto = dynamic_cast<const item::BlockItemProto&>(itemProto);
		auto& proto = blockItemProto.getBlockProto();
		if (proto.getRenderType() == block::BlockRenderType::None) {
			return;
		}
		auto& editor = meshEditor.cast<graph::VertexBlockItemIcon>();
		if (proto.getRenderType() == block::BlockRenderType::Cube) {
			if (isAdd) {
				editor.resize({ 24, 36 });
				editor.setRectIndices(6);
			}
			float texPos0 = tof(proto.getTexArrIndex(Face3::Right).get());
			float texPos1 = tof(proto.getTexArrIndex(Face3::Left).get());
			float texPos2 = tof(proto.getTexArrIndex(Face3::Front).get());
			float texPos3 = tof(proto.getTexArrIndex(Face3::Back).get());
			float texPos4 = tof(proto.getTexArrIndex(Face3::Top).get());
			float texPos5 = tof(proto.getTexArrIndex(Face3::Bottom).get());
			auto& box = getBox();
			float size = box.max.x - box.min.x;
			Float2 u = box.min + size / 2;
			float len = size * math::sqrt2_inv; //防止超出边框

			//Log::Info("block item icon box: center: ", box.center(), ", half: ", box.extents(), ", len: ", len);

			editor.setVertex({
				//x+
				graph::VertexBlockItemIcon(Float3( 0.5f,-0.5f,-0.5f) * len, u, { 1, 0, 0 }, { 0, 0, texPos0}),
				graph::VertexBlockItemIcon(Float3( 0.5f,-0.5f, 0.5f) * len, u, { 1, 0, 0 }, { 0, 1, texPos0}),
				graph::VertexBlockItemIcon(Float3( 0.5f, 0.5f, 0.5f) * len, u, { 1, 0, 0 }, { 1, 1, texPos0}),
				graph::VertexBlockItemIcon(Float3( 0.5f, 0.5f,-0.5f) * len, u, { 1, 0, 0 }, { 1, 0, texPos0}),
				//x-
				graph::VertexBlockItemIcon(Float3(-0.5f, 0.5f,-0.5f) * len, u, {-1, 0, 0 }, { 0, 0, texPos1}),
				graph::VertexBlockItemIcon(Float3(-0.5f, 0.5f, 0.5f) * len, u, {-1, 0, 0 }, { 0, 1, texPos1}),
				graph::VertexBlockItemIcon(Float3(-0.5f,-0.5f, 0.5f) * len, u, {-1, 0, 0 }, { 1, 1, texPos1}),
				graph::VertexBlockItemIcon(Float3(-0.5f,-0.5f,-0.5f) * len, u, {-1, 0, 0 }, { 1, 0, texPos1}),
				//y+
				graph::VertexBlockItemIcon(Float3( 0.5f, 0.5f,-0.5f) * len, u, { 0, 1, 0 }, { 0, 0, texPos2}),
				graph::VertexBlockItemIcon(Float3( 0.5f, 0.5f, 0.5f) * len, u, { 0, 1, 0 }, { 0, 1, texPos2}),
				graph::VertexBlockItemIcon(Float3(-0.5f, 0.5f, 0.5f) * len, u, { 0, 1, 0 }, { 1, 1, texPos2}),
				graph::VertexBlockItemIcon(Float3(-0.5f, 0.5f,-0.5f) * len, u, { 0, 1, 0 }, { 1, 0, texPos2}),
				//y-
				graph::VertexBlockItemIcon(Float3(-0.5f,-0.5f,-0.5f) * len, u, { 0,-1, 0 }, { 0, 0, texPos3}),
				graph::VertexBlockItemIcon(Float3(-0.5f,-0.5f, 0.5f) * len, u, { 0,-1, 0 }, { 0, 1, texPos3}),
				graph::VertexBlockItemIcon(Float3( 0.5f,-0.5f, 0.5f) * len, u, { 0,-1, 0 }, { 1, 1, texPos3}),
				graph::VertexBlockItemIcon(Float3( 0.5f,-0.5f,-0.5f) * len, u, { 0,-1, 0 }, { 1, 0, texPos3}),
				//z+
				graph::VertexBlockItemIcon(Float3(-0.5f,-0.5f, 0.5f) * len, u, { 0, 0, 1 }, { 0, 0, texPos4}),
				graph::VertexBlockItemIcon(Float3(-0.5f, 0.5f, 0.5f) * len, u, { 0, 0, 1 }, { 0, 1, texPos4}),
				graph::VertexBlockItemIcon(Float3( 0.5f, 0.5f, 0.5f) * len, u, { 0, 0, 1 }, { 1, 1, texPos4}),
				graph::VertexBlockItemIcon(Float3( 0.5f,-0.5f, 0.5f) * len, u, { 0, 0, 1 }, { 1, 0, texPos4}),
				//z-
				graph::VertexBlockItemIcon(Float3(-0.5f, 0.5f,-0.5f) * len, u, { 0, 0,-1 }, { 0, 0, texPos5}),
				graph::VertexBlockItemIcon(Float3(-0.5f,-0.5f,-0.5f) * len, u, { 0, 0,-1 }, { 0, 1, texPos5}),
				graph::VertexBlockItemIcon(Float3( 0.5f,-0.5f,-0.5f) * len, u, { 0, 0,-1 }, { 1, 1, texPos5}),
				graph::VertexBlockItemIcon(Float3( 0.5f, 0.5f,-0.5f) * len, u, { 0, 0,-1 }, { 1, 0, texPos5}),
			});
		}
	}
}

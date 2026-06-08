#pragma once
#include <dme/ui/item/item_icon.h>

namespace dme::ui {

	class BlockItemIcon : public ItemIcon, private ObjPool<BlockItemIcon> {
		friend ObjPool<BlockItemIcon>;
		friend UI;
	private:
		virtual void removeImpl() noexcept override;

		static inline Matrix4 _view = Matrix4::LookFront(Float3(), Float3(0, 1, -0.5), Float3(0, 0.5, 1));
		static inline Matrix4 _model = Matrix4::Identity();
		static inline Matrix4 _camera_model = Matrix4::Identity();
		static Matrix4& _MVP() noexcept {
			static Matrix4 _mvp = UI::GetProjection() * _view;
			return _mvp;
		}
	protected:
		BlockItemIcon(uint meshType, const item::ItemStack& itemStack) noexcept;
	public:
		//更新旋转矩阵
		static void UpdateMVP() noexcept;
		//正交投影矩阵乘观察矩阵乘旋转矩阵
		static const Matrix4& GetMVP() noexcept;

		static constexpr size_t obj_pool_size = 64;
		static constexpr bool Is3D = true;
		static void ShaderReadyFunc(const graph::Shader<graph::VertexBlockItemIcon>& shader);
		static const auto& GetShader() {
			return graph::Shaders::BlockItem();
		}
		static inline const graph::VertexIndexCount UseVertexIndexCount = graph::VertexIndexCount();

		static graph::IUIMesh* CreateMesh() {
			return graph::IUIMesh::Create(GetShader().getType(), 256, 384, !UseVertexIndexCount.empty());
		}
		static uint GetMeshType() {
			return graph::UIMeshBase<BlockItemIcon>::GetMeshType();
		}
		static BlockItemIcon& Create(const item::ItemStack& itemStack) noexcept;

		virtual void editMesh(graph::MeshEditorBase& meshEditor, const bool isAdd) noexcept override;
	};

}
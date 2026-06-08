#pragma once
#include <dme/graph/texture/tex2d.h>
#include <dme/graph/texture/tex2d_array.h>
#include <dme/graph/vertex/vertex_array_manage.h>
#include <dme/ui/ui.h>
#include <dme/item/item_stack.h>
namespace dme {
	namespace item {
		class ItemProto;
	}
	namespace ui{
		class InventoryUI;
		class ItemIcon : public UI, private ObjPool<ItemIcon> {
			friend class ObjPool<ItemIcon>;
			friend class InventoryUI; //由InventoryUI控制ItemIcon的大小和位置
		private:
			const item::ItemStack* itemStack;

			virtual void removeImpl() noexcept override;
		protected:
			const item::ItemStack& getItemStack() const noexcept;

			ItemIcon(uint meshType, const item::ItemStack& itemStack);
		public:
			static constexpr size_t obj_pool_size = 64;
			static void ShaderReadyFunc(const graph::Shader<graph::RectUIVertex>& shader) {
				shader.setTexAtlas("aTexture", graph::TexAtlas::Default(), 0);
				shader.setInt("aTextureSize", graph::TexAtlas::Default().getSize());
				shader.setMatrix("projection", UI::GetProjection());
			}
			static const auto& GetShader() {
				return graph::Shaders::UI();
			}
			static inline const graph::VertexIndexCount UseVertexIndexCount = graph::VertexIndexCount(4, 6);
			static graph::IUIMesh* CreateMesh() {
				return graph::IUIMesh::Create(GetShader().getType(), 256, 384, !UseVertexIndexCount.empty());
			}
			static uint GetMeshType() {
				return graph::UIMeshBase<ItemIcon>::GetMeshType();
			}
			static ItemIcon& Create(const item::ItemStack& itemStack) noexcept;

			virtual void click(const UIEvent& e) override;
			virtual void hover(const UIEvent& e) override;
			virtual void leave(const UIEvent& e) override;
			virtual void editMesh(graph::MeshEditorBase& meshEditor, const bool isAdd) noexcept override;
		};
	}
}

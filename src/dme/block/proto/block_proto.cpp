#include <dme/block/proto/block_proto.h>
#include <dme/entity/entity.h>
#include <dme/world/world.h>
#include <dme/item/item_stack.h>
#include <dme/ui/item/block_item_icon.h>
namespace dme::block {

    BlockProto::BlockProto(uid id, strid name, strid tex,
        uint flags, BlockRenderType renderType, BlockCollisionType collisionType)
        : id(id), type(BlockType::Solid), name(name), 
        flags(flags), renderType(renderType), collisionType(collisionType)
    {
        texArrIndex[Face3::Right] = graph::Tex2dArray::Cube().addIfAbsent(tex);
        texIndex[Face3::Right] = graph::TexAtlas::Default().addIfAbsent(tex);
        texArrIndex[Face3::Left] = texArrIndex[Face3::Right];
        texIndex[Face3::Left] = texIndex[Face3::Right];
        texArrIndex[Face3::Front] = texArrIndex[Face3::Right];
        texIndex[Face3::Front] = texIndex[Face3::Right];
        texArrIndex[Face3::Back] = texArrIndex[Face3::Right];
        texIndex[Face3::Back] = texIndex[Face3::Right];
        texArrIndex[Face3::Top] = texArrIndex[Face3::Right];
        texIndex[Face3::Top] = texIndex[Face3::Right];
        texArrIndex[Face3::Bottom] = texArrIndex[Face3::Right];
        texIndex[Face3::Bottom] = texIndex[Face3::Right];
    }

    BlockProto::BlockProto(uid id, strid name, strid texSide, strid texTopBottom,
        uint flags, BlockRenderType renderType, BlockCollisionType collisionType)
        : id(id), type(BlockType::Solid), name(name),
        flags(flags), renderType(renderType), collisionType(collisionType)
    {
        texArrIndex[Face3::Right] = graph::Tex2dArray::Cube().addIfAbsent(texSide);
        texIndex[Face3::Right] = graph::TexAtlas::Default().addIfAbsent(texSide);
        texArrIndex[Face3::Left] = texArrIndex[Face3::Right];
        texIndex[Face3::Left] = texIndex[Face3::Right];
        texArrIndex[Face3::Front] = texArrIndex[Face3::Right];
        texIndex[Face3::Front] = texIndex[Face3::Right];
        texArrIndex[Face3::Back] = texArrIndex[Face3::Right];
        texIndex[Face3::Back] = texIndex[Face3::Right];
        texArrIndex[Face3::Top] = graph::Tex2dArray::Cube().addIfAbsent(texTopBottom);
        texIndex[Face3::Top] = graph::TexAtlas::Default().addIfAbsent(texTopBottom);
        texArrIndex[Face3::Bottom] = texArrIndex[Face3::Top];
        texIndex[Face3::Bottom] = texIndex[Face3::Top];
    }

    BlockProto::BlockProto(uid id, strid name, strid texSide, strid texTop, strid texBottom,
        uint flags, BlockRenderType renderType, BlockCollisionType collisionType)
        : id(id), type(BlockType::Solid), name(name),
        flags(flags), renderType(renderType), collisionType(collisionType)
    {
        texArrIndex[Face3::Right] = graph::Tex2dArray::Cube().addIfAbsent(texSide);
        texIndex[Face3::Right] = graph::TexAtlas::Default().addIfAbsent(texSide);
        texArrIndex[Face3::Left] = texArrIndex[Face3::Right];
        texIndex[Face3::Left] = texIndex[Face3::Right];
        texArrIndex[Face3::Front] = texArrIndex[Face3::Right];
        texIndex[Face3::Front] = texIndex[Face3::Right];
        texArrIndex[Face3::Back] = texArrIndex[Face3::Right];
        texIndex[Face3::Back] = texIndex[Face3::Right];
        texArrIndex[Face3::Top] = graph::Tex2dArray::Cube().addIfAbsent(texTop);
        texIndex[Face3::Top] = graph::TexAtlas::Default().addIfAbsent(texTop);
        texArrIndex[Face3::Bottom] = graph::Tex2dArray::Cube().addIfAbsent(texBottom);
        texIndex[Face3::Bottom] = graph::TexAtlas::Default().addIfAbsent(texBottom);
    }

    BlockProto::BlockProto(uid id, strid name, strid texRight, strid texLeft, strid texFront, strid texBack, strid texTop, strid texBottom,
        uint flags, BlockRenderType renderType, BlockCollisionType collisionType)
        : id(id), type(BlockType::Solid), name(name),
        flags(flags), renderType(renderType), collisionType(collisionType)
    {
        texArrIndex[Face3::Right] = graph::Tex2dArray::Cube().addIfAbsent(texRight);
        texIndex[Face3::Right] = graph::TexAtlas::Default().addIfAbsent(texRight);
        texArrIndex[Face3::Left] = graph::TexAtlas::Default().addIfAbsent(texLeft);
        texIndex[Face3::Left] = graph::TexAtlas::Default().addIfAbsent(texLeft);
        texArrIndex[Face3::Front] = graph::TexAtlas::Default().addIfAbsent(texFront);
        texIndex[Face3::Front] = graph::TexAtlas::Default().addIfAbsent(texFront);
        texArrIndex[Face3::Back] = graph::TexAtlas::Default().addIfAbsent(texBack);
        texIndex[Face3::Back] = graph::TexAtlas::Default().addIfAbsent(texBack);
        texArrIndex[Face3::Top] = graph::Tex2dArray::Cube().addIfAbsent(texTop);
        texIndex[Face3::Top] = graph::TexAtlas::Default().addIfAbsent(texTop);
        texArrIndex[Face3::Bottom] = graph::Tex2dArray::Cube().addIfAbsent(texBottom);
        texIndex[Face3::Bottom] = graph::TexAtlas::Default().addIfAbsent(texBottom);
    }


    uid BlockProto::getId() const noexcept {
        return id;
    }
    void BlockProto::setId(BlockProto* proto, uid id_) noexcept {
        proto->id = id_;
    }
    BlockType BlockProto::getType() const noexcept {
        return type;
    }
    BlockRenderType BlockProto::getRenderType() const noexcept {
        return renderType;
    }
    BlockCollisionType BlockProto::getCollisionType() const noexcept {
        return collisionType;
    }
    strid BlockProto::getName() const noexcept {
        return name;
    }
    bool BlockProto::getTransparent() const noexcept {
        return flags & flags_transparent;
    }
    graph::TexIndex BlockProto::getTexArrIndex(Face3 face) const noexcept {
        return texArrIndex[face];
    }
    graph::TexIndex BlockProto::getTexIndex(Face3 face) const noexcept {
        return texIndex[face];
    }


    void BlockProto::placeBlock(Chunk& chunk, Block& block, entity::Entity& entity) const {
        block = Block::createCube(id);
    }
    void BlockProto::destroyBlock(Chunk& chunk, Block& block, entity::Entity& entity) const {
    
    }
    void BlockProto::useBlock(Chunk& chunk, Block& block, entity::Entity& entity) const {
        //普通方块没有互动功能
    }

    bool BlockProto::place(const BPos& pos, entity::Entity& entity) const {
        if (pos.isNull() || pos.getBlock()->notNull()) return false;
        //TODO 需更加完善的碰撞逻辑，以及碰撞体的抽象
        bool entityCollision = false;
        auto blockPos = pos.getBlockPos();
        auto box = math::Abox3f(Fix3(blockPos), Fix3(blockPos + 1));
        auto callback = [&](bool& isBreak, uid index, const util::BVH3f::Node& node) {
            if (box.intersect(entity.getAbox())) { //由于entity会移动，bvh里的包围盒会比真实的更大，所以这里再细判一次
                entityCollision = true;
                isBreak = true;
            }
        };
        pos.getWorld()->getBVH().queryCollision(box, callback);
        if (entityCollision) return false;
        placeBlock(*pos.getChunk() , *pos.getBlock(), entity);
        return true;
    }
    bool BlockProto::place(Chunk& chunk, Block& block, entity::Entity& entity) const {
        if (block.notNull()) return false;
        //TODO 需更加完善的碰撞逻辑，以及碰撞体的抽象
        bool entityCollision = false;
        auto blockPos = chunk.getBlockPos(block);
        auto box = math::Abox3f(Fix3(blockPos), Fix3(blockPos + 1));
        auto callback = [&](bool& isBreak, uid index, const util::BVH3f::Node& node) {
            if (box.intersect(entity.getAbox())) { //由于entity会移动，所以在bvh里的包围盒并不是真实的包围盒，这里再细判一次
                entityCollision = true;
                isBreak = true;
            }
        };
        chunk.getWorld()->getBVH().queryCollision(box, callback);
        if (entityCollision) return false;
        placeBlock(chunk, block, entity);
        return true;
    }
    bool BlockProto::destroy(const BPos& pos, entity::Entity& entity) const {
        if (pos.isNull() || pos.getBlock()->isNull()) return false;
        destroyBlock(*pos.getChunk(), *pos.getBlock(), entity);
        return true;
    }
    bool BlockProto::destroy(Chunk& chunk, Block& block, entity::Entity& entity) const {
        if (block.isNull()) return false;
        destroyBlock(chunk, block, entity);
        return true;
    }
    bool BlockProto::use(const BPos& pos, entity::Entity& entity) const {
        if (pos.isNull() || pos.getBlock()->notNull()) return false;
        useBlock(*pos.getChunk(), *pos.getBlock(), entity);
        return true;
    }
    bool BlockProto::use(Chunk& chunk, Block& block, entity::Entity& entity) const {
        if (block.isNull()) return false;
        useBlock(chunk, block, entity);
        return true;
    }

    ui::ItemIcon& BlockProto::createIcon(const item::ItemStack& itemStack) const {
        return ui::BlockItemIcon::Create(itemStack);
    }

    bool BlockProto::operator ==(const BlockProto& a) const noexcept {
        return id == a.id;
    }

}
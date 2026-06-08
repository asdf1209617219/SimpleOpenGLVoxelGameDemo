#include <dme/block/bpos.h>
#include <dme/block/chunk/chunk.h>
#include <dme/world/world.h>
namespace dme::block {


    BPos::BPos() noexcept : chunk(null), block(null) {};
    BPos::BPos(Chunk& chunk, CBPos pos) noexcept : chunk(&chunk), block(chunk.blocks + pos) {};
    BPos::BPos(const Int3& blockPos, world::World& world) noexcept : chunk(world.getChunk(block::ToChunkPos(blockPos))), block(null) {
        if (!chunk) return;
        block = chunk->blocks + block::ToCBPos(blockPos);
    };
    BPos::BPos(const Int3& blockPos, sid worldId) noexcept : chunk(null), block(null) {
        auto world = world::World::GetWorld(worldId);
        if (!world) return;
        chunk = world->getChunk(block::ToChunkPos(blockPos));
        if (!chunk) return;
        block = chunk->blocks + block::ToCBPos(blockPos);
    };
    BPos::BPos(const Int4& blockPos) noexcept : chunk(null), block(null) {
        auto world = world::World::GetWorld(blockPos.w);
        if (!world) return;
        chunk = world->getChunk(block::ToChunkPos(blockPos.vxyz));
        if (!chunk) return;
        block = chunk->blocks + block::ToCBPos(blockPos.vxyz);
    };

    bool BPos::isNull() const noexcept {
        return !chunk;
    }
    bool BPos::notNull() const noexcept {
        return chunk;
    }

    world::World* BPos::getWorld() const noexcept {
        return chunk ? chunk->world : null;
    }
    Chunk* BPos::getChunk() const noexcept {
        return chunk;
    }
    Block* BPos::getBlock() const noexcept {
        return block;
    }
    Int3 BPos::getBlockPos() const noexcept {
        return chunk ? block::ToBlockPos(chunk->getPos(), CBPos(tous(block - chunk->blocks))) : Int3();
    }
    CBPos BPos::getCBPos() const noexcept {
        return chunk ? CBPos(tous(block - chunk->blocks)) : CBPos();
    }

}
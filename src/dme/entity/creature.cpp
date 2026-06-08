#include <dme/entity/creature.h>
#include <dme/block/proto/all.h>
#include <dme/item/proto/all.h>
#include <dme/world/world.h>
#include <dme/graph/cuboid_frame.h>
namespace dme::entity {

    Creature::Creature(strid name, Fix3 centerPos, Float3 half, world::World* worldId_, uint inventorySize, float speed)
        : Entity(centerPos, worldId_, true),
        name(name),
        half(half),
        inventory(inventorySize),
        inHand(0),
        isJumping(false),
        isFloating(false),
        hasVelocity(false),
        isFly(false),
        velocity(),
        speed(speed), yaw(0), yawSin(0), yawCos(1), pitch(0), pitchSin(0), pitchCos(1),
        front(Float3c::Forward),
        up(Float3c::Up)
    {
    }

    strid Creature::getName() const {
        return name;
    }
    Fix3 Creature::getBottomPos() const {
        return centerPos.plusZ(-half.z);
    }
    void Creature::setBottomPos(const Fix3& bottomPos_) {
        centerPos = bottomPos_.plusZ(half.z);
    }
    Float3 Creature::getVelocity() const {
        return velocity;
    }
    void Creature::setVelocity(const Float3& velocity_) {
        velocity.x = math::Clamp(velocity_.x, -max_speed, max_speed);
        velocity.y = math::Clamp(velocity_.y, -max_speed, max_speed);
        velocity.z = math::Clamp(velocity_.z, -max_speed, max_speed);
        hasVelocity = !velocity.isZero();
    }
    void Creature::setVelocityX(float velocityX) {
        velocity.x = math::Clamp(velocityX, -max_speed, max_speed);
        hasVelocity = !velocity.isZero();
    }
    void Creature::setVelocityY(float velocityY) {
        velocity.y = math::Clamp(velocityY, -max_speed, max_speed);
        hasVelocity = !velocity.isZero();
    }
    void Creature::setVelocityZ(float velocityZ) {
        velocity.z = math::Clamp(velocityZ, -max_speed, max_speed);
        hasVelocity = !velocity.isZero();
    }
    void Creature::addVelocity(const Float3& velocityDelta) {
        velocity.x += math::Clamp(velocityDelta.x, -max_speed, max_speed);
        velocity.y += math::Clamp(velocityDelta.y, -max_speed, max_speed);
        velocity.z += math::Clamp(velocityDelta.z, -max_speed, max_speed);
        hasVelocity = !velocity.isZero();
    }
    void Creature::addVelocityX(float velocityXDelta) {
        velocity.x += math::Clamp(velocityXDelta, -max_speed, max_speed);
        hasVelocity = !velocity.isZero();
    }
    void Creature::addVelocityY(float velocityYDelta) {
        velocity.y += math::Clamp(velocityYDelta, -max_speed, max_speed);
        hasVelocity = !velocity.isZero();
    }
    void Creature::addVelocityZ(float velocityZDelta) {
        velocity.z += math::Clamp(velocityZDelta, -max_speed, max_speed);
        hasVelocity = !velocity.isZero();
    }
    float Creature::getSpeed() const {
        return speed;
    }
    void Creature::setSpeed(float speed_) {
        speed = math::Clamp(speed_, 0.0f, max_speed);
    }
    void Creature::setIsFloating(bool isFloating_) {
        isFloating = isFloating_;
    }
    item::Inventory& Creature::getInventory() {
        return inventory;
    }
    void Creature::addYawPitch(float yawDelta, float pitchDelta) {
        yaw = math::circle(yaw + yawDelta); //只允许在-pi~pi
        pitch = math::Clamp(pitch + pitchDelta, -math::piover2, math::piover2);

        pitchSin = std::sin(pitch);
        pitchCos = std::cos(pitch);
        yawSin = std::sin(yaw);
        yawCos = std::cos(yaw);

        front.z = pitchSin;
        front.x = yawSin * pitchCos;
        front.y = yawCos * pitchCos;

        up.x = -pitchSin * yawSin;
        up.y = -pitchSin * yawCos;
        up.z = pitchCos;
    }
    Float3 Creature::getDelta() {
        //TODO 需加入生物移动AI
        if (hasVelocity) {
            return velocity * Game::GetDeltaTimeF();
        }
        return Float3();
    }
    static inline float _checkDirectionAtLine(uint index1, uint index2, const Fix3& oldEndCorner, const Fix3& newEndCorner, const Int2& p0) {
        return Fix2::DirectionAtLine(oldEndCorner.get(index1, index2), newEndCorner.get(index1, index2), p0);
    }
    static inline float _checkDirectionAtLine(uint index1, uint index2, const Fix3& oldEndCorner, const Fix3& newEndCorner, const Int3& p0) {
        return Fix2::DirectionAtLine(oldEndCorner.get(index1, index2), newEndCorner.get(index1, index2), p0.get(index1, index2));
    }
    void Creature::resolveCollisionPos(uint index, sint moveDirection, sint blockPos) {
        //FixPos3 pos_old = centerPos;

        centerPos.set(index, block::CollideBlockSide(blockPos, moveDirection), -moveDirection * half[index]);

        velocity[index] = 0;
        hasVelocity = !velocity.isZero();

        /*
        if (pos_old.distance(centerPos) > 0.5f) {
            //瞬移
            FixPos3 pos_new = centerPos;

            std::cout << "old: " << pos_old << "\n";
            std::cout << "new: " << pos_new << "\n";
            std::cout << "index: " << index << "\n";
            std::cout << "direction: " << moveDirection << "\n";
            sint ttt = 0;
        }
        */
    }
    bool Creature::blockIntersect(
        Int3& pos,
        Bool3& r,
        const block::AABBMove& move
    ) {
        block::Block* ptr = world->getBlock(pos);
        if (!ptr) { //不允许前往未加载的区块
            return true;
        }
        block::Block b = *ptr;
        if (b.isNull()) {
            return false;
        }
        block::BlockProto* proto = b.getProto();
        if (!proto || proto->getCollisionType() == block::BlockCollisionType::None) {
            return false;
        }

        switch (b.getShape()) {
        case block::BlockShape::Cube:
            break;
        case block::BlockShape::Half:
            break;
        case block::BlockShape::Ramp:
            break;
        }
        return true;
    }

    void Creature::blockCollisionResolve(const block::AABBMove& move) {
        Int3 pos;
        Bool3 r;
        uint index;
        float direction;
        //x
        for (pos.x = move.inner.x; math::compareDirection(pos.x, move.blockEnd.x, move.unit.x); pos.x += move.unit.x) {
            for (pos.y = move.blockStart.y; math::compareDirection(pos.y, move.inner.y, move.unit.y); pos.y += move.unit.y) {
                for (pos.z = move.blockStart.z; math::compareDirection(pos.z, move.inner.z, move.unit.z); pos.z += move.unit.z) {
                    if (blockIntersect(pos, r, move)) {
                        r.x = true;
                        resolveCollisionPos(0, move.unit.x, pos.x);
                        goto y;
                    }
                }
            }
        }
    y:
        for (pos.y = move.inner.y; math::compareDirection(pos.y, move.blockEnd.y, move.unit.y); pos.y += move.unit.y) {
            for (pos.x = move.blockStart.x; math::compareDirection(pos.x, move.inner.x, move.unit.x); pos.x += move.unit.x) {
                for (pos.z = move.blockStart.z; math::compareDirection(pos.z, move.inner.z, move.unit.z); pos.z += move.unit.z) {
                    if (blockIntersect(pos, r, move)) {
                        r.y = true;
                        resolveCollisionPos(1, move.unit.y, pos.y);
                        goto z;
                    }
                }
            }
        }
    z:
        for (pos.z = move.inner.z; math::compareDirection(pos.z, move.blockEnd.z, move.unit.z); pos.z += move.unit.z) {
            for (pos.x = move.blockStart.x; math::compareDirection(pos.x, move.inner.x, move.unit.x); pos.x += move.unit.x) {
                for (pos.y = move.blockStart.y; math::compareDirection(pos.y, move.inner.y, move.unit.y); pos.y += move.unit.y) {
                    if (blockIntersect(pos, r, move)) {
                        r.z = true;
                        resolveCollisionPos(2, move.unit.z, pos.z);
                        goto xy;
                    }
                }
            }
        }
    xy:
        if (r.x || r.y) goto xz;
        for (pos.x = move.inner.x; math::compareDirection(pos.x, move.blockEnd.x, move.unit.x); pos.x += move.unit.x) {
            for (pos.y = move.inner.y; math::compareDirection(pos.y, move.blockEnd.y, move.unit.y); pos.y += move.unit.y) {
                for (pos.z = move.blockStart.z; math::compareDirection(pos.z, move.inner.z, move.unit.z); pos.z += move.unit.z) {
                    if (blockIntersect(pos, r, move)) {
                        direction = Fix2::DirectionAtLine(
                            move.oldPosEnd.x, move.oldPosEnd.xf,
                            move.oldPosEnd.y, move.oldPosEnd.yf,
                            move.newPosEnd.x, move.newPosEnd.xf,
                            move.newPosEnd.y, move.newPosEnd.yf,
                            block::CollideBlockSide(pos.x, move.unit.x),
                            block::CollideBlockSide(pos.y, move.unit.y)
                        );
                        if (direction < 0 || (direction == 0 && move.absDelta.x > move.absDelta.y)) {
                            index = 1;
                        }
                        else {
                            index = 0;
                        }
                        r[index] = true;
                        resolveCollisionPos(index, move.unit[index], pos[index]);
                        goto xz;
                    }
                }
            }
        }
    xz:
        if (r.x || r.z) goto yz;
        for (pos.x = move.inner.x; math::compareDirection(pos.x, move.blockEnd.x, move.unit.x); pos.x += move.unit.x) {
            for (pos.z = move.inner.z; math::compareDirection(pos.z, move.blockEnd.z, move.unit.z); pos.z += move.unit.z) {
                for (pos.y = move.blockStart.y; math::compareDirection(pos.y, move.inner.y, move.unit.y); pos.y += move.unit.y) {
                    if (blockIntersect(pos, r, move)) {
                        direction = Fix2::DirectionAtLine(
                            move.oldPosEnd.x, move.oldPosEnd.xf,
                            move.oldPosEnd.z, move.oldPosEnd.zf,
                            move.newPosEnd.x, move.newPosEnd.xf,
                            move.newPosEnd.z, move.newPosEnd.zf,
                            block::CollideBlockSide(pos.x, move.unit.x),
                            block::CollideBlockSide(pos.z, move.unit.z)
                        );
                        if (direction < 0 || (direction == 0 && move.absDelta.x > move.absDelta.z)) {
                            index = 2;
                        }
                        else {
                            index = 0;
                        }
                        r[index] = true;
                        resolveCollisionPos(index, move.unit[index], pos[index]);
                        goto yz;
                    }
                }
            }
        }
    yz:
        if (r.y || r.z) goto xyz;
        for (pos.y = move.inner.y; math::compareDirection(pos.y, move.blockEnd.y, move.unit.y); pos.y += move.unit.y) {
            for (pos.z = move.inner.z; math::compareDirection(pos.z, move.blockEnd.z, move.unit.z); pos.z += move.unit.z) {
                for (pos.x = move.blockStart.x; math::compareDirection(pos.x, move.inner.x, move.unit.x); pos.x += move.unit.x) {
                    if (blockIntersect(pos, r, move)) {
                        direction = Fix2::DirectionAtLine(
                            move.oldPosEnd.y, move.oldPosEnd.yf,
                            move.oldPosEnd.z, move.oldPosEnd.zf,
                            move.newPosEnd.y, move.newPosEnd.yf,
                            move.newPosEnd.z, move.newPosEnd.zf,
                            block::CollideBlockSide(pos.y, move.unit.y),
                            block::CollideBlockSide(pos.z, move.unit.z)
                        );
                        if (direction < 0 || (direction == 0 && move.absDelta.y > move.absDelta.z)) {
                            index = 2;
                        }
                        else {
                            index = 1;
                        }
                        r[index] = true;
                        resolveCollisionPos(index, move.unit[index], pos[index]);
                        goto xyz;
                    }
                }
            }
        }
    xyz:
        if (r.x || r.y || r.z) goto check_float;
        for (pos.x = move.inner.x; math::compareDirection(pos.x, move.blockEnd.x, move.unit.x); pos.x += move.unit.x) {
            for (pos.y = move.inner.y; math::compareDirection(pos.y, move.blockEnd.y, move.unit.y); pos.y += move.unit.y) {
                for (pos.z = move.inner.z; math::compareDirection(pos.z, move.blockEnd.z, move.unit.z); pos.z += move.unit.z) {
                    if (blockIntersect(pos, r, move)) {
                        direction = Fix2::DirectionAtLine(
                            move.oldPosEnd.x, move.oldPosEnd.xf,
                            move.oldPosEnd.y, move.oldPosEnd.yf,
                            move.newPosEnd.x, move.newPosEnd.xf,
                            move.newPosEnd.y, move.newPosEnd.yf,
                            block::CollideBlockSide(pos.x, move.unit.x),
                            block::CollideBlockSide(pos.y, move.unit.y)
                        );
                        if (direction < 0 || (direction == 0 && move.absDelta.x > move.absDelta.y)) {
                            index = 1;
                        }
                        else {
                            index = 0;
                        }
                        direction = Fix2::DirectionAtLine(
                            move.oldPosEnd.i[index], move.oldPosEnd.f[index],
                            move.oldPosEnd.z, move.oldPosEnd.zf,
                            move.newPosEnd.i[index], move.newPosEnd.f[index],
                            move.newPosEnd.z, move.newPosEnd.zf,
                            block::CollideBlockSide(pos[index], move.unit[index]),
                            block::CollideBlockSide(pos.z, move.unit.z)
                        );
                        if (direction < 0 || (direction == 0 && move.absDelta.x > move.absDelta.z)) {
                            index = 2;
                        }
                        r[index] = true;
                        resolveCollisionPos(index, move.unit[index], pos[index]);
                        goto check_float;
                    }
                }
            }
        }
    check_float:
        //判断下方是否有方块，如果是飞行或刚才检测时z轴下方已经发生碰撞则直接设置为不浮空
        if (isFly || (r.z && move.unit.z < 0)) {
            setVelocityZ(0);
            setIsFloating(false);
            return;
        }
        //判断是否贴在地面上
        Fix bottom = centerPos.get(2) - half.z;
        if (bottom.xf == 0) { //底部贴在地面上
            pos.z = bottom.x - 1;
            for (pos.x = move.blockStart.x; math::compareDirection(pos.x, move.inner.x, move.unit.x); pos.x += move.unit.x) {
                for (pos.y = move.blockStart.y; math::compareDirection(pos.y, move.inner.y, move.unit.y); pos.y += move.unit.y) {
                    if (blockIntersect(pos, r, move)) {
                        setVelocityZ(0);
                        setIsFloating(false);
                        return;
                    }
                }
            }
        }
        setIsFloating(true);
        //重力
        float velZ = -Entity::g * Game::GetDeltaTimeF();
        addVelocityZ(velZ);
    }
    void Creature::doCollision(const Float3& delta, bool isTrigger) {
        //Float3 deltaAbs = delta.abs();
        bool moving = (!delta.isZero() || isFloating) && world != null;
        bool check = checkCollision && (moving || isTrigger);
        if (check) {
            /*
            Int3 unit = delta.toSignInt3(); //与delta每个轴的朝向相同的单位向量
            Float3 absDelta = delta.abs();
            block::FixCuboid oldFixCuboid = block::FixCuboid(centerPos, half);
            centerPos += delta;
            block::FixCuboid newFixCuboid = block::FixCuboid(centerPos, half);

            block::BlockCuboid oldBlockCuboid = block::BlockCuboid(oldFixCuboid);
            block::BlockCuboid newBlockCuboid = block::BlockCuboid(newFixCuboid);

            //test
            //处理正好处于边缘的情况
            Int3 oldPos = Int3(
                oldEndCorner.x > oldStartCorner.x && oldEndCorner.xf == 0 ? oldEndCorner.x - 1 : oldEndCorner.x,
                oldEndCorner.y > oldStartCorner.y && oldEndCorner.yf == 0 ? oldEndCorner.y - 1 : oldEndCorner.y,
                oldEndCorner.z > oldStartCorner.z && oldEndCorner.zf == 0 ? oldEndCorner.z - 1 : oldEndCorner.z
            );

            Int3 frameSize = cuboid.max - cuboid.min;
            frameSize.x++;
            frameSize.y++;
            frameSize.z++;

            Game::CuboidFrame.setPos(cuboid.min.toFloat3());
            Game::CuboidFrame.changeVertices(frameSize.toFloat3());

            Float3 frameSize2 = Float3(1, 1, 1);
            Int3 framePos2 = newEndCorner.getInt3();
            if (unit.x > 0 && newEndCorner.xf == 0) framePos2.x--;
            if (unit.y > 0 && newEndCorner.yf == 0) framePos2.y--;
            if (unit.z > 0 && newEndCorner.zf == 0) framePos2.z--;

            Float3 frameSize3 = Float3(0.8f, 0.8f, 0.8f);
            Int3 framePos3 = oldEndCorner.getInt3();
            if (unit.x > 0 && oldEndCorner.xf == 0) framePos3.x--;
            if (unit.y > 0 && oldEndCorner.yf == 0) framePos3.y--;
            if (unit.z > 0 && oldEndCorner.zf == 0) framePos3.z--;

            Game::CuboidFrame2.setPos(framePos2.toFloat3());
            Game::CuboidFrame2.changeVertices(frameSize2);

            Game::CuboidFrame3.setPos((centerPos - delta - half).int3PlusFloat3());
            Game::CuboidFrame3.changeVertices(half * 2);
            */

            Fix3 oldCenterPos = centerPos;
            //处理碰撞
            blockCollisionResolve(block::AABBMove::Move(centerPos, half, delta));
            Float3 newDelta = centerPos.vector(oldCenterPos);
            if (!newDelta.isZero()) {
                updateBVH(newDelta);
            }
        }
        else if (moving) {
            centerPos += delta;
            updateBVH(delta);
        }
    }
    math::Abox3f Creature::getAbox() const noexcept {
        return math::Abox3f::CreateCH(centerPos, half);
    }
    void Creature::init() {
        triggerPhysics(Float3(), Float3());
    }
    void Creature::update() {
        Float3 delta = getDelta();
        Creature::doCollision(delta);
    }
    void Creature::triggerPhysics(const Float3& posDelta, const Float3& velocityDelta) {
        addVelocity(velocityDelta);
        Creature::doCollision(posDelta, true);
    }

}
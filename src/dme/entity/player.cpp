#include <dme/entity/player.h>
#include <dme/input/input.h>
#include <dme/world/world.h>
#include <dme/event/data/item_use_data.h>
#include <dme/event/event.h>
#include <dme/ui/item/hotbar.h>
#include <dme/graph/cube_frame.h>
#include <dme/graph/camera.h>
#include <dme/graph/fix_camera.h>

#include <dme/item/proto/all.h>

namespace dme::entity {
    using input::Input;
    using input::InputAction;

    Player::Player(strid name, Fix3 centerPos, world::World* world)
        : Creature(name, centerPos, Float3(player_half_width, player_half_width, player_half_height), world, 30, 16.0f),
        hotbarIndex(0)
    {
        isAdmin = true; //TODO 暂时开启管理员模式
        isFly = false;
        checkCollision = true;
    }

    Float3 Player::getCameraPos() const {
        Float3 p = centerPos.castToFloat();
        p.z += player_camera_zoffset;
        return p;
    }
    Fix3 Player::getCameraFPos() const {
        return centerPos.plusZ(player_camera_zoffset);
    }
    uint Player::getHotbarIndex() const {
        return hotbarIndex;
    }
    void Player::setHotbarIndex(uint hotbarIndex) {
        hotbarIndex = math::Clamp(hotbarIndex, 0u, inventory.getSize() - 1);
    }

    void Player::checkRay(graph::CubeFrame& cubeFrame, math::Ray3fTarget& rayTarget) {
        Fix3 origin = getCameraFPos(); //摄像机位置
        block::Block* b = null;
        auto callback = [&](bool& isBreak, const math::Ray3fTarget& target) {
            block::Block* b = world->getBlock(target.gridPos);
            if (b && b->notNull()) {
                isBreak = true;
                rayTarget = target;
                rayTarget.hasTarget = true;
            }
        };
        math::Ray3f::RayCastVoxel(origin, front, front.inversion(), player_max_select_range, callback);
        if (rayTarget.hasTarget) {
            //更新选择框
            cubeFrame.display();
            cubeFrame.setPos(rayTarget.gridPos);
        }
        else {
            cubeFrame.hide();
        }
    }

    Float3 Player::getDelta() {
        Float3 delta = Float3();
        if (Input::IsDown(InputAction::Forward)) {
            delta.y += 1;
        }
        if (Input::IsDown(InputAction::Backward)) {
            delta.y += -1;
        }
        if (Input::IsDown(InputAction::Left)) {
            delta.x += -1;
        }
        if (Input::IsDown(InputAction::Right)) {
            delta.x += 1;
        }
        if (isFly) {
            if (Input::IsDown(InputAction::Jump)) {
                delta.z += 1;
            }
            if (Input::IsDown(InputAction::Sneak)) {
                delta.z += -1;
            }
            if (!delta.isZero()) {
                float da = delta.x;
                float db = delta.y;
                delta.x = da * yawCos + db * yawSin; //运动量绕z轴逆时针旋转yaw对应的角度，这里使用二维向量旋转公式
                delta.y = db * yawCos - da * yawSin;

                delta = delta.normalize() * (speed * Game::GetDeltaTimeF());
            }
        }
        else {
            //if (Input::isPress(InputAction::Jump) && !isFloating) {
            if (Input::IsDown(InputAction::Jump) && !isFloating) {
                //addVelocityZ(player_jump_velocity_z * Game::getDeltaTimeF());
                addVelocityZ(player_jump_velocity_z);
            }
            if (delta.x != 0 || delta.y != 0) {
                float da = delta.x;
                float db = delta.y;
                delta.x = da * yawCos + db * yawSin; //运动量绕z轴逆时针旋转yaw对应的角度，这里使用二维向量旋转公式
                delta.y = db * yawCos - da * yawSin;

                delta = delta.normalize() * (speed * Game::GetDeltaTimeF());
            }
            if (hasVelocity) {
                delta += velocity * Game::GetDeltaTimeF();
            }
        }
        return delta;
    }

    void Player::update() {
        bool forceDoCollision = false;
        if (Input::IsPress(InputAction::Fly)) {
            isFly = !isFly;
            forceDoCollision = true;
        }
        if (Input::IsPress(InputAction::Collision)) {
            checkCollision = !checkCollision;
            forceDoCollision = true;
        }
        if (!checkCollision) {
            isFly = true;
        }

        bool changeCamera = false;
        bool cursorIsForceCenter = input::Input::GetCursorIsForceCenter();

        if (cursorIsForceCenter) {
            //滚轮切换快捷栏物品 
            double scrollY = input::Input::GetScrollOffset().y;
            if (scrollY == 1) { //滚轮向上
                inHand = ui::Hotbar::Default().scollActiveIndex(true);
            }
            else if (scrollY == -1) { //滚轮向下
                inHand = ui::Hotbar::Default().scollActiveIndex(false);
            }
            //通过键盘切换快捷栏
            if (Input::IsPress(InputAction::Hotbar1)) {
                inHand = ui::Hotbar::Default().setActiveIndex(0);
            }
            else if (Input::IsPress(InputAction::Hotbar2)) {
                inHand = ui::Hotbar::Default().setActiveIndex(1);
            }
            else if (Input::IsPress(InputAction::Hotbar3)) {
                inHand = ui::Hotbar::Default().setActiveIndex(2);
            }
            else if (Input::IsPress(InputAction::Hotbar4)) {
                inHand = ui::Hotbar::Default().setActiveIndex(3);
            }
            else if (Input::IsPress(InputAction::Hotbar5)) {
                inHand = ui::Hotbar::Default().setActiveIndex(4);
            }
            else if (Input::IsPress(InputAction::Hotbar6)) {
                inHand = ui::Hotbar::Default().setActiveIndex(5);
            }
            else if (Input::IsPress(InputAction::Hotbar7)) {
                inHand = ui::Hotbar::Default().setActiveIndex(6);
            }
            else if (Input::IsPress(InputAction::Hotbar8)) {
                inHand = ui::Hotbar::Default().setActiveIndex(7);
            }
            else if (Input::IsPress(InputAction::Hotbar9)) {
                inHand = ui::Hotbar::Default().setActiveIndex(8);
            }
            else if (Input::IsPress(InputAction::Hotbar10)) {
                inHand = ui::Hotbar::Default().setActiveIndex(9);
            }

            //移动视角
            auto cursorOffset = input::Input::GetCursorOffset();
            if (!cursorOffset.isZero()) {
                changeCamera = true;
                float sensitivity = input::Input::GetCursorSensitivity();

                addYawPitch(tof(cursorOffset.x) * sensitivity, tof(cursorOffset.y) * sensitivity);
            }
        }
        //射线
        math::Ray3fTarget rayTarget;
        checkRay(Game::CubeFrame, rayTarget);

        if (cursorIsForceCenter) {
            //这里不允许同时使用左键和右键
            if (Input::IsPress(InputAction::Destroy)) { //物品使用左键
                auto& item = inventory[hotbarIndex + inHand];
                event::ItemUseData d = event::ItemUseData(item, item::ItemFunc::f1, *this, rayTarget);
                //event::ItemUseEvent::trigger(item.getProtoId(), d); //TODO 目前不通过事件调用，直接通过proto调用

                item.getProto().use(d);
            }
            else if (Input::IsPress(InputAction::Place)) { //物品使用右键
                auto& item = inventory[hotbarIndex + inHand];
                event::ItemUseData d = event::ItemUseData(item, item::ItemFunc::f2, *this, rayTarget);
                //event::ItemUseEvent::trigger(item.getProtoId(), d);

                item.getProto().use(d);
            }
        }

        Float3 delta = getDelta();

        if (!delta.isZero() && world != null) {
            changeCamera = true;
        }
        Int3 oldChunkPos = block::ToChunkPos(centerPos.i);
        //碰撞
        doCollision(delta, forceDoCollision);
        loadChunk(oldChunkPos, block::ToChunkPos(centerPos.i));

        if (changeCamera) {
            //std::cout << "player center pos: " << centerPos << "\n";
            //Game::Camera.updateView(getCameraPos(), front, up);
            Game::CameraF.updateView(getCameraFPos(), front, up);
        }
    }

    void Player::loadChunk(const Int3& oldChunkPos, const Int3& newChunkPos) {
        Int3 unit = newChunkPos - oldChunkPos;
        if (unit.isZero()) {
            return;
        }
        sint r = Game::GetChunkLoadRadius();
        //移动后新的加载范围与旧的加载范围的差集就是需要加载的区块
        unit = unit.toSign();
        Int3 outerStart;
        Int3 inner;
        Int3 outerEnd;
        for (auto axis = 0; axis < Int3::axis; axis++) {
            sint i1 = oldChunkPos.v[axis] - r, i2 = oldChunkPos.v[axis] + r,
                o1 = newChunkPos.v[axis] - r, o2 = newChunkPos.v[axis] + r;
            if (o1 <= i2 && o2 >= i2) {
                outerStart.v[axis] = o1;
                inner.v[axis] = i2 + 1;
                outerEnd.v[axis] = o2 + 1;
            }
            else if (o1 <= i1 && o2 >= i1) {
                outerStart.v[axis] = o2;
                inner.v[axis] = i1 - 1;
                outerEnd.v[axis] = o1 - 1;
            }
            else [[unlikely]] {
                for (sint x = newChunkPos.x - r; x < newChunkPos.x + r + 1; x++) {
                    for (sint y = newChunkPos.y - r; y < newChunkPos.y + r + 1; y++) {
                        for (sint z = newChunkPos.z - r; z < newChunkPos.z + r + 1; z++) {
                            world->loadChunkAsync(Int3(x, y, z));
                        }
                    }
                }
                return;
            }
        }
        sint count = 0;
        for (sint x = inner.x; math::compareDirection(x, outerEnd.x, unit.x); x += unit.x) {
            for (sint y = outerStart.y; math::compareDirection(y, outerEnd.y, unit.y); y += unit.y) {
                for (sint z = outerStart.z; math::compareDirection(z, outerEnd.z, unit.z); z += unit.z) {
                    count++;
                    world->loadChunkAsync(Int3(x, y, z));
                }
            }
        }
        for (sint x = outerStart.x; math::compareDirection(x, inner.x, unit.x); x += unit.x) {
            for (sint y = inner.y; math::compareDirection(y, outerEnd.y, unit.y); y += unit.y) {
                for (sint z = outerStart.z; math::compareDirection(z, outerEnd.z, unit.z); z += unit.z) {
                    count++;
                    world->loadChunkAsync(Int3(x, y, z));
                }
            }
        }
        for (sint x = outerStart.x; math::compareDirection(x, inner.x, unit.x); x += unit.x) {
            for (sint y = outerStart.y; math::compareDirection(y, inner.y, unit.y); y += unit.y) {
                for (sint z = inner.z; math::compareDirection(z, outerEnd.z, unit.z); z += unit.z) {
                    count++;
                    world->loadChunkAsync(Int3(x, y, z));
                }
            }
        }
        util::Log::Info("load chunk count: ", count);
    }
    void Player::init() {
        if (!world) {
            return;
        }
        sint r = Game::GetChunkLoadRadius();
        Int3 centerChunkPos = block::ToChunkPos(centerPos.i);
        Int3 startChunkPos = centerChunkPos - r;
        Int3 endChunkPos = centerChunkPos + (r + 1);
        util::Log::Info("init startChunkPos: ", startChunkPos);
        util::Log::Info("init endChunkPos: ", endChunkPos);
        sint count = 0;
        for (sint x = startChunkPos.x; x < endChunkPos.x; x++) {
            for (sint y = startChunkPos.y; y < endChunkPos.y; y++) {
                for (sint z = startChunkPos.z; z < endChunkPos.z; z++) {
                    count++;
                    world->loadChunkAsync(Int3(x, y, z));
                }
            }
        }
        util::Log::Info("init count: ", count);
        updateBVH();
        //触发物理更新，初始化bvh节点、重力等
        triggerPhysics(Float3(), Float3());
    }

}
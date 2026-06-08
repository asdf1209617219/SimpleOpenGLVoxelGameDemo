#include <dme/block/block_cuboid.h>
#include <dme/block/bpos.h>
namespace dme::block {

    constexpr CompareFlag BlockPosCompare::x() const noexcept {
        return static_cast<CompareFlag>(value & 0b11);
    }
    constexpr CompareFlag BlockPosCompare::y() const noexcept {
        return static_cast<CompareFlag>((value >> 2) & 0b11);
    }
    constexpr CompareFlag BlockPosCompare::z() const noexcept {
        return static_cast<CompareFlag>((value >> 4) & 0b11);
    }
    constexpr uchar BlockPosCompare::notEqualCount() const noexcept {
        return value >> 6;
    }
    constexpr bool BlockPosCompare::in() const noexcept {
        return value == 0;
    }
    constexpr bool BlockPosCompare::xEqual() const noexcept {
        return (value & 0b11) == 0;
    }
    constexpr bool BlockPosCompare::yEqual() const noexcept {
        return (value & 0b1100) == 0;
    }
    constexpr bool BlockPosCompare::zEqual() const noexcept {
        return (value & 0b110000) == 0;
    }
    constexpr bool BlockPosCompare::xLess() const noexcept {
        return value & 0b01;
    }
    constexpr bool BlockPosCompare::yLess() const noexcept {
        return value & 0b0100;
    }
    constexpr bool BlockPosCompare::zLess() const noexcept {
        return value & 0b010000;
    }
    constexpr bool BlockPosCompare::xGreater() const noexcept {
        return value & 0b10;
    }
    constexpr bool BlockPosCompare::yGreater() const noexcept {
        return value & 0b1000;
    }
    constexpr bool BlockPosCompare::zGreater() const noexcept {
        return value & 0b100000;
    }

    constexpr sint BlockCuboidCompare::x() const noexcept {
        if (value & 0b01) {
            return -1;
        }
        if (value & 0b10) {
            return 0;
        }
        return 1;
    }
    constexpr sint BlockCuboidCompare::y() const noexcept {
        if (value & 0b0100) {
            return -1;
        }
        if (value & 0b1000) {
            return 0;
        }
        return 1;

    }
    constexpr sint BlockCuboidCompare::z() const noexcept {
        if (value & 0b010000) {
            return -1;
        }
        if (value & 0b100000) {
            return 0;
        }
        return 1;
    }
    constexpr sint BlockCuboidCompare::i(uint index) const noexcept {
        uint mask = 0b1 << (index << 1);
        if (value & mask) {
            return -1;
        }
        if (value & (mask << 1)) {
            return 0;
        }
        return 1;
    }
    /*
    constexpr sint BlockCuboidCompare::x() const noexcept {
        if (value & 0b01) {
            return 1;
        }
        if (value & 0b10) {
            return 0;
        }
        return -1;
    }
    constexpr sint BlockCuboidCompare::y() const noexcept {
        if (value & 0b0100) {
            return 1;
        }
        if (value & 0b1000) {
            return 0;
        }
        return -1;

    }
    constexpr sint BlockCuboidCompare::z() const noexcept {
        if (value & 0b010000) {
            return 1;
        }
        if (value & 0b100000) {
            return 0;
        }
        return -1;
    }
    constexpr sint BlockCuboidCompare::i(uint index) const noexcept {
        uint mask = 0b1 << (index << 1);
        if (value & mask) {
            return 1;
        }
        if (value & (mask << 1)) {
            return 0;
        }
        return -1;
    }
    */

    constexpr uint BlockCuboidCompare::xv() const noexcept {
        return value & 0b11;
    }
    constexpr uint BlockCuboidCompare::yv() const noexcept {
        return (value >> 2) & 0b11;
    }
    constexpr uint BlockCuboidCompare::zv() const noexcept {
        return (value >> 4) & 0b11;
    }
    constexpr uint BlockCuboidCompare::iv(uint index) const noexcept {
        return (value >> (index << 1)) & 0b11;
    }
    constexpr uint BlockCuboidCompare::notEqualCount() const noexcept {
        return (value >> 6) & 0b11;
    }
    constexpr bool BlockCuboidCompare::xGreaterEqual() const noexcept {
        return value & 0b11;
    }
    constexpr bool BlockCuboidCompare::yGreaterEqual() const noexcept {
        return value & 0b1100;
    }
    constexpr bool BlockCuboidCompare::zGreaterEqual() const noexcept {
        return value & 0b110000;
    }
    constexpr bool BlockCuboidCompare::iGreaterEqual(uint index) const noexcept {
        return value & (0b11 << (index << 1));
    }
    constexpr Uint3 BlockCuboidCompare::faces() const noexcept {
        return Uint3(
            (value >> 8) & 0b11,
            (value >> 10) & 0b11,
            (value >> 12) & 0b11
        );
    }
    constexpr bool BlockCuboidCompare::in() const noexcept {
        return (value & 0b11000000) == 0;
    }

    constexpr FixCuboidCompare FixCuboid::compare(const Int3& blockPos) const noexcept {
        uint value = 0;
        Fix3 pos;
        uint face = 0b100100, count = 0;
        if (blockPos.x > max.x) {
            count++;
            pos.x = max.x;
        }
        else if (blockPos.x < min.x) {
            value |= 0b01;
            count++;
            pos.x = min.x;
        }
        else {
            value |= 0b10;
            pos.x = max.x;
        }

        if (blockPos.y > max.y) {
            if (count == 0) {
                face = 0b100001;
            }
            count++;
            pos.y = max.y;
        }
        else if (blockPos.y < min.y) {
            value |= 0b0100;
            if (count == 0) {
                face = 0b100001;
            }
            count++;
            pos.y = min.y;
        }
        else {
            value |= 0b1000;
            pos.y = max.y;
        }

        if (blockPos.z > max.z) {
            if (count == 1) {
                face = ((face & 0b1111) << 2) | 0b10;
            }
            else if (count == 0) {
                face = 0b010010;
            }
            else {
                face = 0b100100;
            }
            count++;
            pos.z = max.z;
        }
        else if (blockPos.z < min.z) {
            value |= 0b010000;
            if (count == 1) {
                face = ((face & 0b1111) << 2) | 0b10;
            }
            else if (count == 0) {
                face = 0b010010;
            }
            else {
                face = 0b100100;
            }
            count++;
            pos.z = min.z;
        }
        else {
            value |= 0b100000;
            pos.z = max.z;
        }
        return FixCuboidCompare(value | (count << 6) | (face << 8), pos);
    }
    constexpr Fix3 FixCuboid::getResultPos(const BlockCuboidCompare& result) const noexcept {
        Fix3 pos;
        if (result.xGreaterEqual()) {
            pos.x = max.x;
            pos.xf = max.xf;
        }
        else {
            pos.x = min.x;
            pos.xf = min.xf;
        }
        if (result.yGreaterEqual()) {
            pos.y = max.y;
            pos.yf = max.yf;
        }
        else {
            pos.y = min.y;
            pos.yf = min.yf;
        }
        if (result.zGreaterEqual()) {
            pos.z = max.z;
            pos.zf = max.zf;
        }
        else {
            pos.z = min.z;
            pos.zf = min.zf;
        }
        return pos;
    }
    constexpr bool FixCuboid::operator ==(const FixCuboid& a) const noexcept {
        return min == a.min && max == a.max;
    }
    constexpr bool FixCuboid::operator !=(const FixCuboid& a) const noexcept {
        return min != a.min || max != a.max;
    }
    std::ostream& block::operator<<(std::ostream& os, const FixCuboid& a) {
        return os << "min-x:" << a.min.x << ", min-y: " << a.min.y << ", min-z: " << a.min.z << ", max-x: " << a.max.x << ", max-y: " << a.max.y << ", max-z: " << a.max.z;
    }



    static void _outputAxis(std::ostream& os, uint axis, bool isPositive) {
        if (isPositive) {
            os << "+";
        }
        else {
            os << "-";
        }

        switch (axis) {
        default:
        case 0:
            os << "x";
            break;
        case 1:
            os << "y";
            break;
        case 2:
            os << "z";
            break;
        }
    }

    std::ostream& block::operator<<(std::ostream& os, const BlockCuboidCompare& a) {
        Uint3 faces = a.faces();
        switch (a.notEqualCount()) {
        default:
        case 0:
            return os << "pos: inside;";
        case 1:
            os << "pos: face: ";
            _outputAxis(os, faces.x, a.iGreaterEqual(a.iGreaterEqual(0)));
            return os;
        case 2:
            os << "pos: side: ";
            _outputAxis(os, faces.x, a.iGreaterEqual(a.iGreaterEqual(0)));
            os << ", ";
            _outputAxis(os, faces.y, a.iGreaterEqual(a.iGreaterEqual(1)));
            return os;
        case 3:
            os << "pos: corner: ";
            _outputAxis(os, faces.x, a.iGreaterEqual(a.iGreaterEqual(0)));
            os << ", ";
            _outputAxis(os, faces.y, a.iGreaterEqual(a.iGreaterEqual(1)));
            os << ", ";
            _outputAxis(os, faces.z, a.iGreaterEqual(a.iGreaterEqual(2)));
            return os;
        }
    }

    constexpr BlockCuboidCompare BlockCuboid::compare(const Int3& blockPos) const noexcept {
        uint value = 0;
        uint face = 0b100100, count = 0;
        if (blockPos.x > max.x) {
            count++;
        }
        else if (blockPos.x < min.x) {
            value |= 0b01;
            count++;
        }
        else {
            value |= 0b10;
        }

        if (blockPos.y > max.y) {
            if (count == 0) {
                face = 0b100001;
            }
            count++;
        }
        else if (blockPos.y < min.y) {
            value |= 0b0100;
            if (count == 0) {
                face = 0b100001;
            }
            count++;
        }
        else {
            value |= 0b1000;
        }

        if (blockPos.z > max.z) {
            if (count == 1) {
                face = ((face & 0b1111) << 2) | 0b10;
            }
            else if (count == 0) {
                face = 0b010010;
            }
            else {
                face = 0b100100;
            }
            count++;
        }
        else if (blockPos.z < min.z) {
            value |= 0b010000;
            if (count == 1) {
                face = ((face & 0b1111) << 2) | 0b10;
            }
            else if (count == 0) {
                face = 0b010010;
            }
            else {
                face = 0b100100;
            }
            count++;
        }
        else {
            value |= 0b100000;
        }
        return BlockCuboidCompare(value | (count << 6) | (face << 8));
    }
    /*
    constexpr BlockCuboidCompare BlockCuboid::compare(const Int3& blockPos) const noexcept {
        uint value = 0;
        uint face = 0b100100, count = 0;
        if (blockPos.x > max.x) {
            value |= 0b01;
            count++;
        }
        else if (blockPos.x < min.x) {
            count++;
        }
        else {
            value |= 0b10;
        }

        if (blockPos.y > max.y) {
            value |= 0b0100;
            if (count == 0) {
                face = 0b100001;
            }
            count++;
        }
        else if (blockPos.y < min.y) {
            if (count == 0) {
                face = 0b100001;
            }
            count++;
        }
        else {
            value |= 0b1000;
        }

        if (blockPos.z > max.z) {
            value |= 0b010000;
            if (count == 1) {
                face = (face << 2) & 0b10;
            }
            else if (count == 0) {
                face = 0b010010;
            }
            else {
                face = 0b100100;
            }
            count++;
        }
        else if (blockPos.z < min.z) {
            if (count == 1) {
                face = (face << 2) & 0b10;
            }
            else if (count == 0) {
                face = 0b010010;
            }
            else {
                face = 0b100100;
            }
            count++;
        }
        else {
            value |= 0b100000;
        }
        return BlockCuboidCompare(value | (count << 6) | ((face & 0b111111) << 8));
    }
    */

    constexpr Int3 BlockCuboid::getRealPos(const BlockCuboidCompare& result) const noexcept {
        Int3 pos;
        if (result.xGreaterEqual()) {
            pos.x = max.x + 1;
        }
        else {
            pos.x = min.x;
        }
        if (result.yGreaterEqual()) {
            pos.y = max.y + 1;
        }
        else {
            pos.y = min.y;
        }
        if (result.zGreaterEqual()) {
            pos.z = max.z + 1;
        }
        else {
            pos.z = min.z;
        }
        return pos;
    }

    constexpr bool BlockCuboid::operator ==(const BlockCuboid& a) const noexcept {
        return min == a.min && max == a.max;
    }
    constexpr bool BlockCuboid::operator !=(const BlockCuboid& a) const noexcept {
        return min != a.min || max != a.max;
    }
    std::ostream& block::operator<<(std::ostream& os, const BlockCuboid& a) {
        return os << "min-x:" << a.min.x << ", min-y: " << a.min.y << ", min-z: " << a.min.z << ", max-x: " << a.max.x << ", max-y: " << a.max.y << ", max-z: " << a.max.z;
    }

    constexpr BlockCuboidVectorDiff BlockCuboidVector::diff(const BlockCuboidVector& a) const noexcept {
        return BlockCuboidVectorDiff();
    }


}
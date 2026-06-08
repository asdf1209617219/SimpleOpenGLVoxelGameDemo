#include <dme/math/seed.h>
#include <dme/core/utils.h>
namespace dme::math {

    Seed::Seed() : name(), value(0) {}
    Seed::Seed(const stdstr& name, ulong value, std::mt19937_64&& func)
        : name(name), value(value), func(std::move(func))
    {
        //TODO test
        std::cout << "Seed name: " << name << ", value: " << value << std::endl;
    }

    stdstr& Seed::getName() {
        return name;
    }
    ulong Seed::getValue() const {
        return value;
    }
    std::mt19937_64& Seed::getFunc() {
        return func;
    }

    NumGen<schar> Seed::scharGen(schar max, schar min) {
        return NumGen<schar>(func, min, max);
    }
    NumGen<uchar> Seed::ucharGen(uchar max, uchar min) {
        return NumGen<uchar>(func, min, max);
    }
    NumGen<sshort> Seed::sshortGen(sshort max, sshort min) {
        return NumGen<sshort>(func, min, max);
    }
    NumGen<ushort> Seed::ushortGen(ushort max, ushort min) {
        return NumGen<ushort>(func, min, max);
    }
    NumGen<sint> Seed::sintGen(sint max, sint min) {
        return NumGen<sint>(func, min, max);
    }
    NumGen<uint> Seed::uintGen(uint max, uint min) {
        return NumGen<uint>(func, min, max);
    }
    NumGen<slong> Seed::slongGen(slong max, slong min) {
        return NumGen<slong>(func, min, max);
    }
    NumGen<ulong> Seed::ulongGen(ulong max, ulong min) {
        return NumGen<ulong>(func, min, max);
    }
    NumGen<float> Seed::floatGen(float max, float min) {
        return NumGen<float>(func, min, max);
    }
    NumGen<double> Seed::doubleGen(double max, double min) {
        return NumGen<double>(func, min, max);
    }


    schar Seed::ranSchar(schar max, schar min) {
        return static_cast<schar>(std::uniform_int_distribution<sshort>(min, max)(func));
    }
    uchar Seed::ranUchar(uchar max, uchar min) {
        return static_cast<uchar>(std::uniform_int_distribution<ushort>(min, max)(func));
    }
    sshort Seed::ranSshort(sshort max, sshort min) {
        return std::uniform_int_distribution<sshort>(min, max)(func);
    }
    ushort Seed::ranUshort(ushort max, ushort min) {
        return std::uniform_int_distribution<ushort>(min, max)(func);
    }
    sint Seed::ranSint(sint max, sint min) {
        return std::uniform_int_distribution<sint>(min, max)(func);
    }
    uint Seed::ranUint(uint max, uint min) {
        return std::uniform_int_distribution<uint>(min, max)(func);
    }
    slong Seed::ranSlong(slong max, slong min) {
        return std::uniform_int_distribution<slong>(min, max)(func);
    }
    ulong Seed::ranUlong(ulong max, ulong min) {
        return std::uniform_int_distribution<ulong>(min, max)(func);
    }
    float Seed::ranFloat(float max, float min) {
        return std::uniform_real_distribution<float>(min, max)(func);
    }
    double Seed::ranDouble(double max, double min) {
        return std::uniform_real_distribution<double>(min, max)(func);
    }

    Seed Seed::CreateSeed() {
        // 获取当前时间
        //time_t now = time(NULL);
        //stdstr str = std::to_string(now);

        // 使用 std::random_device 通过硬件获取真随机数
        std::random_device rd;
        //随机两个uint拼成ulong
        //ulong randomNum = UintToUlong(rd(), rd());
        uint randomNum = rd();
        //种子名
        stdstr seedName = std::to_string(randomNum);
        size_t seedHash = Hasher(seedName);
        return Seed(seedName, seedHash, std::mt19937_64(seedHash));
    }
    Seed Seed::CreateSeed(const stdstr& seedName) {
        size_t seedHash = Hasher(seedName);
        return Seed(seedName, seedHash, std::mt19937_64(seedHash));
    }

}
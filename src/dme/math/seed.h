#pragma once
#include <dme/core/utils.h>
namespace dme {
	namespace math {
		class Seed;

		template<typename T>
		class NumGen {
			friend Seed;
		private:
			//如果T是uchar则使用ushort，如果T是schar则使用sshort，否则使用T本身
			using type = std::conditional_t<std::is_same_v<uchar, T>, ushort,
				std::conditional_t<std::is_same_v<schar, T>, sshort, T>>;
			//如果T是整数类型则使用std::uniform_int_distribution，否则使用std::uniform_real_distribution
			using distribution_t = std::conditional_t<std::is_integral_v<T>, std::uniform_int_distribution<type>, std::uniform_real_distribution<type>>;

			distribution_t gen;
			std::mt19937_64* seed;

			NumGen(std::mt19937_64& seed, T min, T max) noexcept : seed(&seed), gen(distribution_t(min, max)) {}
		public:
			NumGen(const NumGen& a) noexcept : seed(a.seed), gen(a.gen) {};
			NumGen(NumGen&& a) noexcept : seed(std::move(a.seed)), gen(std::move(a.gen)) {
				a.seed = null;
			};
			NumGen& operator =(const NumGen& a) noexcept {
				if (this != &a) {
					gen = a.gen;
					seed = a.seed;
				}
				return *this;
			}
			NumGen& operator =(NumGen&& a) noexcept {
				if (this != &a) {
					gen = a.gen;
					seed = a.seed;

					a.seed = null;
				}
				return *this;
			}

			T rand() {
				return static_cast<T>(gen(*seed));
			}
		};

		class Seed {
		private:
			stdstr name;
			ulong value;
			std::mt19937_64 func;

			Seed(const stdstr& name, ulong value, std::mt19937_64&& func);
		public:
			Seed();

			//种子名
			stdstr& getName();
			//种子hash值
			ulong getValue() const;
			//随机数引擎
			std::mt19937_64& getFunc();

			NumGen<schar> scharGen(schar max = schar_max, schar min = schar_min);
			NumGen<uchar> ucharGen(uchar max = uchar_max, uchar min = uchar_min);
			NumGen<sshort> sshortGen(sshort max = sshort_max, sshort min = sshort_min);
			NumGen<ushort> ushortGen(ushort max = ushort_max, ushort min = ushort_min);
			NumGen<sint> sintGen(sint max = sint_max, sint min = sint_min);
			NumGen<uint> uintGen(uint max = uint_max, uint min = uint_min);
			NumGen<slong> slongGen(slong max = slong_max, slong min = slong_min);
			NumGen<ulong> ulongGen(ulong max = ulong_max, ulong min = ulong_min);
			NumGen<float> floatGen(float max = float_max, float min = float_min);
			NumGen<double> doubleGen(double max = double_max, double min = double_min);

			schar ranSchar(schar max = schar_max, schar min = schar_min);
			uchar ranUchar(uchar max = uchar_max, uchar min = uchar_min);
			sshort ranSshort(sshort max = sshort_max, sshort min = sshort_min);
			ushort ranUshort(ushort max = ushort_max, ushort min = ushort_min);
			sint ranSint(sint max = sint_max, sint min = sint_min);
			uint ranUint(uint max = uint_max, uint min = uint_min);
			slong ranSlong(slong max = slong_max, slong min = slong_min);
			ulong ranUlong(ulong max = ulong_max, ulong min = ulong_min);
			float ranFloat(float max = float_max, float min = float_min);
			double ranDouble(double max = double_max, double min = double_min);

			//随机生成一个数字作为种子名
			static Seed CreateSeed();
			//根据种子名生成种子值
			static Seed CreateSeed(const stdstr& seedName);
		};
	}
}
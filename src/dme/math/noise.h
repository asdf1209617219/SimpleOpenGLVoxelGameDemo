#pragma once
#include <dme/math/seed.h>
namespace dme {
	namespace math {
		class Noise {
		private:
		public:
			//梯度函数计算伪随机之间的点积(1维)
			static inline double Grad(uchar hash, double x) noexcept;
			//梯度函数计算伪随机之间的点积(2维)
			static inline double Grad(uchar hash, double x, double y) noexcept;
			//梯度函数计算伪随机之间的点积(3维)
			static inline double Grad(uchar hash, double x, double y, double z) noexcept;
			//梯度函数计算伪随机之间的点积(4维)
			static inline double Grad(uchar hash, double x, double y, double z, double w) noexcept;

			//Ken Perlin 定义的函数. 这缓动了坐标值，让坐标值趋向边界
			static inline double Fade(double t) noexcept;
			//线性插值
			static inline double Lerp(double a, double b, double t) noexcept;
		};

		//柏林噪声生成器
		class PerlinNoise {
		private:
			uchar p[512];
		public:
			PerlinNoise(Seed& seed);

			//1维噪音，输出范围不会超出[-1, 1]
			double gen(double x) const noexcept;
			//2维噪音，输出范围不会超出[-1, 1]
			double gen(double x, double y) const noexcept;
			//3维噪音，输出范围不会超出[-1, 1]
			double gen(double x, double y, double z) const noexcept;
			/**
			* 1维分形布朗运动 (fBm) 算法，通过振幅和频率生成更自然的噪音
			* @param x      输入坐标
			* @param octaves      八度数量（循环次数），通常取 4 到 8
			* @param persistence  振幅衰减比例，推荐 0.5
			* @param lacunarity   频率递进倍数，推荐 2.0
			*/
			double octave1(double x, uint octaves, double persistence = 0.5, double lacunarity = 2.0) const noexcept;
			/**
			* 2维分形布朗运动 (fBm) 算法，通过振幅和频率生成更自然的噪音
			* @param x, y      输入坐标
			* @param octaves      八度数量（循环次数），通常取 4 到 8
			* @param persistence  振幅衰减比例，推荐 0.5
			* @param lacunarity   频率递进倍数，推荐 2.0
			*/
			double octave2(double x, double y, uint octaves, double persistence = 0.5, double lacunarity = 2.0) const noexcept;
			/**
			* 3维分形布朗运动 (fBm) 算法，通过振幅和频率生成更自然的噪音
			* @param x, y, z      输入坐标
			* @param octaves      八度数量（循环次数），通常取 4 到 8
			* @param persistence  振幅衰减比例，推荐 0.5
			* @param lacunarity   频率递进倍数，推荐 2.0
			*/
			double octave3(double x, double y, double z, uint octaves, double persistence = 0.5, double lacunarity = 2.0) const noexcept;
		};
	}
}
#include <dme/math/noise.h>
#include <dme/math/utils.h>

namespace dme::math {

	double Noise::Fade(double t) noexcept {
		return t * t * t * (t * (t * 6 - 15) + 10); // 6t^5 - 15t^4 + 10t^3
	}
	double Noise::Lerp(double a, double b, double t) noexcept {
		return a + t * (b - a);
	}

	double Noise::Grad(uchar hash, double x) noexcept {
		return (hash & 1) == 0 ? x : -x;
	}
	double Noise::Grad(uchar hash, double x, double y) noexcept {
		switch (hash & 0x3) {
		case 0x0: return  x + y;
		case 0x1: return -x + y;
		case 0x2: return  x - y;
		case 0x3: return -x - y;
		default: return 0; // never happens
		}
	}
	double Noise::Grad(uchar hash, double x, double y, double z) noexcept {
		switch (hash & 0xF) {
		case 0x0: return  x + y;
		case 0x1: return -x + y;
		case 0x2: return  x - y;
		case 0x3: return -x - y;
		case 0x4: return  x + z;
		case 0x5: return -x + z;
		case 0x6: return  x - z;
		case 0x7: return -x - z;

		case 0x8: return  y + z;
		case 0x9: return -y + z;
		case 0xA: return  y - z;
		case 0xB: return -y - z;
		case 0xC: return  y + x;
		case 0xD: return -y + z;
		case 0xE: return  y - x;
		case 0xF: return -y - z;
		default: return 0; // never happens
		}
	}
	double Noise::Grad(uchar hash, double x, double y, double z, double t) noexcept {
		switch (hash & 0x1F) {
		case 0x0: return  x + y + z;
		case 0x1: return -x + y + z;
		case 0x2: return  x - y + z;
		case 0x3: return -x - y + z;
		case 0x4: return  x + y - z;
		case 0x5: return -x + y - z;
		case 0x6: return  x - y - z;
		case 0x7: return -x - y - z;

		case 0x8: return  x + y + t;
		case 0x9: return -x + y + t;
		case 0xA: return  x - y + t;
		case 0xB: return -x - y + t;
		case 0xC: return  x + y - t;
		case 0xD: return -x + y - t;
		case 0xE: return  x - y - t;
		case 0xF: return -x - y - t;

		case 0x10: return  x + z + t;
		case 0x11: return -x + z + t;
		case 0x12: return  x - z + t;
		case 0x13: return -x - z + t;
		case 0x14: return  x + z - t;
		case 0x15: return -x + z - t;
		case 0x16: return  x - z - t;
		case 0x17: return -x - z - t;

		case 0x18: return  y + z + t;
		case 0x19: return -y + z + t;
		case 0x1A: return  y - z + t;
		case 0x1B: return -y - z + t;
		case 0x1C: return  y + z - t;
		case 0x1D: return -y + z - t;
		case 0x1E: return  y - z - t;
		case 0x1F: return -y - z - t;
		default: return 0; // never happens
		}
	}

	PerlinNoise::PerlinNoise(Seed& seed) : p{
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
		10, 11, 12, 13, 14, 15, 16 , 17, 18, 19,
		20, 21, 22, 23, 24, 25, 26 , 27, 28, 29,
		30, 31, 32, 33, 34, 35, 36 , 37, 38, 39,
		40, 41, 42, 43, 44, 45, 46 , 47, 48, 49,
		50, 51, 52, 53, 54, 55, 56 , 57, 58, 59,
		60, 61, 62, 63, 64, 65, 66 , 67, 68, 69,
		70, 71, 72, 73, 74, 75, 76 , 77, 78, 79,
		80, 81, 82, 83, 84, 85, 86 , 87, 88, 89,
		90, 91, 92, 93, 94, 95, 96 , 97, 98, 99,
		100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
		110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
		120, 121, 122, 123, 124, 125, 126, 127, 128, 129,
		130, 131, 132, 133, 134, 135, 136, 137, 138, 139,
		140, 141, 142, 143, 144, 145, 146, 147, 148, 149,
		150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
		160, 161, 162, 163, 164, 165, 166, 167, 168, 169,
		170, 171, 172, 173, 174, 175, 176, 177, 178, 179,
		180, 181, 182, 183, 184, 185, 186, 187, 188, 189,
		190, 191, 192, 193, 194, 195, 196, 197, 198, 199,
		200, 201, 202, 203, 204, 205, 206, 207, 208, 209,
		210, 211, 212, 213, 214, 215, 216, 217, 218, 219,
		220, 221, 222, 223, 224, 225, 226, 227, 228, 229,
		230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
		240, 241, 242, 243, 244, 245, 246, 247, 248, 249,
		250, 251, 252, 253, 254, 255,
	} {
		for (ushort i = 0; i < 256; i++) {
			//洗牌
			ushort j = i + seed.ranUshort(255 - i);
			uchar temp = p[i];
			p[i] = p[j];
			p[j] = temp;
			//后半部分与前半部分相同
			p[i + 256] = p[i];
		}
	}
	double PerlinNoise::gen(double x) const noexcept {
		sint X = floori(x);
		x -= X;
		X &= 0xff;
		double u = Noise::Fade(x);
		return Noise::Lerp(Noise::Grad(p[X], x), Noise::Grad(p[X + 1], x - 1), u) * 2;
	}
	double PerlinNoise::gen(double x, double y) const noexcept {
		sint X = floori(x);
		sint Y = floori(y);
		x -= X;
		y -= Y;
		X &= 0xff;
		Y &= 0xff;
		double u = Noise::Fade(x);
		double v = Noise::Fade(y);
		uchar A = p[X] + Y;
		uchar B = p[X + 1] + Y;
		return Noise::Lerp(
			Noise::Lerp(Noise::Grad(p[A], x, y), Noise::Grad(p[B], x - 1, y), u),
			Noise::Lerp(Noise::Grad(p[A + 1], x, y - 1), Noise::Grad(p[B + 1], x - 1, y - 1), u),
		v);
	}
	double PerlinNoise::gen(double x, double y, double z) const noexcept {
		sint X = floori(x);
		sint Y = floori(y);
		sint Z = floori(z);
		// 现在 x,y,z已经变成了原来的小数部分
		x -= X;
		y -= Y;
		z -= Z;
		// 这里&0xff作用是让 X 值范围为0-255，注意我们的目的是求Hash，这个操作只是减小随机范围
		X &= 0xff;
		Y &= 0xff;
		Z &= 0xff;
		//计算lerp函数的t参数，让显示更平滑
		double u = Noise::Fade(x);
		double v = Noise::Fade(y);
		double w = Noise::Fade(z);

		sint A = p[X] + Y;
		sint B = p[X + 1] + Y;
		sint AA = p[A] + Z;
		sint BA = p[B] + Z;
		sint AB = p[A + 1] + Z;
		sint BB = p[B + 1] + Z;
		// 这些都是取Hash操作，在三维中，我们临近整数点有8个，现在得到的类似8个伪随机向量
		// 这里AAA实际上不是向量而是一个伪随机数值，但是后面的 Grad 操作其实会做与伪随机向量类似的事情
		// 原因参见 https://mrl.cs.nyu.edu/~perlin/paper445.pdf
		uchar AAA = p[AA];
		uchar BAA = p[BA];
		uchar ABA = p[AB];
		uchar BBA = p[BB];
		uchar AAB = p[AA + 1];
		uchar BAB = p[BA + 1];
		uchar ABB = p[AB + 1];
		uchar BBB = p[BB + 1];

		double x1, x2, y1, y2;
		// 依次在x,y,z方向插值
		// 次序其实没有关系，但是需要对应，比如下面这句（x,y,z）和（x-1,y,z）说明是在x方向插值
		x1 = Noise::Lerp(Noise::Grad(AAA, x, y, z), Noise::Grad(BAA, x - 1, y, z), u);
		x2 = Noise::Lerp(Noise::Grad(ABA, x, y - 1, z), Noise::Grad(BBA, x - 1, y - 1, z), u);
		y1 = Noise::Lerp(x1, x2, v);

		x1 = Noise::Lerp(Noise::Grad(AAB, x, y, z - 1), Noise::Grad(BAB, x - 1, y, z - 1), u);
		x2 = Noise::Lerp(Noise::Grad(ABB, x, y - 1, z - 1), Noise::Grad(BBB, x - 1, y - 1, z - 1), u);
		y2 = Noise::Lerp(x1, x2, v);
		return Noise::Lerp(y1, y2, w);
	}

	double PerlinNoise::octave1(double x, uint octaves, double persistence, double lacunarity) const noexcept {
		double total = 0;
		double frequency = 1; //频率
		double amplitude = 1; //振幅
		double maxValue = 0;  // 通常范围在 0.0 - 1.0
		for (uint i = 0; i < octaves; i++) {
			total += gen(x * frequency) * amplitude;

			maxValue += amplitude;

			amplitude *= persistence;
			frequency *= lacunarity;
		}
		return total / maxValue;
	}

	double PerlinNoise::octave2(double x, double y, uint octaves, double persistence, double lacunarity) const noexcept {
		double total = 0;
		double frequency = 1; //频率
		double amplitude = 1; //振幅
		double maxValue = 0;  // 通常范围在 0.0 - 1.0
		for (uint i = 0; i < octaves; i++) {
			total += gen(x * frequency, y * frequency) * amplitude;

			maxValue += amplitude;

			amplitude *= persistence;
			frequency *= lacunarity;
		}
		return total / maxValue;
	}

	double PerlinNoise::octave3(double x, double y, double z, uint octaves, double persistence, double lacunarity) const noexcept {
		double total = 0;
		double frequency = 1; //频率
		double amplitude = 1; //振幅
		double maxValue = 0;  // 通常范围在 0.0 - 1.0
		for (uint i = 0; i < octaves; i++) {
			total += gen(x * frequency, y * frequency, z * frequency) * amplitude;

			maxValue += amplitude;

			amplitude *= persistence;
			frequency *= lacunarity;
		}
		return total / maxValue;
	}

}
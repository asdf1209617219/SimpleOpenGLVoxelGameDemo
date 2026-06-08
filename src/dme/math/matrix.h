#pragma once
#include <dme/math/vector.h>
namespace dme {
	namespace math {
		//4x4矩阵，列主序
		struct Matrix4 {
			using type = typename float;
			static constexpr axis_t col_axis = 4;
			static constexpr axis_t row_axis = 4;

			union {
				struct {
					float x0; float y0; float z0; float w0; //第一列
					float x1; float y1; float z1; float w1; //第二列
					float x2; float y2; float z2; float w2; //第三列
					float x3; float y3; float z3; float w3; //第四列
				};
				Float4 col[col_axis];
				float v[col_axis * row_axis];
			};

			//返回零矩阵
			constexpr Matrix4() noexcept :
				x0(0), x1(0), x2(0), x3(0),  //第一行
				y0(0), y1(0), y2(0), y3(0),
				z0(0), z1(0), z2(0), z3(0),
				w0(0), w1(0), w2(0), w3(0) {}
			//为矩阵的主对角线赋值，其他的值为0
			explicit constexpr Matrix4(float a) noexcept :
				x0(a), x1(0), x2(0), x3(0),  //第一行
				y0(0), y1(a), y2(0), y3(0),
				z0(0), z1(0), z2(a), z3(0),
				w0(0), w1(0), w2(0), w3(a) {}
			//前四个参数为第一列
			constexpr Matrix4(
				float x1, float y1, float z1, float w1, //第一列
				float x2, float y2, float z2, float w2,
				float x3, float y3, float z3, float w3,
				float x4, float y4, float z4, float w4
			) noexcept :
				x0(x1), x1(x2), x2(x3), x3(x4), //第一行
				y0(y1), y1(y2), y2(y3), y3(y4),
				z0(z1), z1(z2), z2(z3), z3(z4),
				w0(w1), w1(w2), w2(w3), w3(w4) {}
			//按照列赋值
			constexpr Matrix4(const Float4& c0, const Float4& c1, const Float4& c2, const Float4& c3) noexcept : col{ c0, c1, c2, c3 } {}

			//返回第一列第一个数的指针
			constexpr const float* ptr() const noexcept {
				return v;
			}
			//返回第一列第一个数的指针
			constexpr float* ptr() noexcept {
				return v;
			}
			//返回行
			constexpr Float4 row(uint index) const noexcept {
				if constexpr (_math_safe_index_) {
					index = (index % col_axis) * col_axis;
					return Float4(v[index], v[index + 1], v[index + 2], v[index + 3]);
				}
				else {
					index = index * col_axis;
					return Float4(v[index], v[index + 1], v[index + 2], v[index + 3]);
				}
			}
			//返回第1行
			constexpr Float4 row0() const noexcept {
				return Float4(x0, x1, x2, x3);
			}
			//返回第2行
			constexpr Float4 row1() const noexcept {
				return Float4(y0, y1, y2, y3);
			}
			//返回第3行
			constexpr Float4 row2() const noexcept {
				return Float4(z0, z1, z2, z3);
			}
			//返回第4行
			constexpr Float4 row3() const noexcept {
				return Float4(w0, w1, w2, w3);
			}

			//返回单位矩阵
			constexpr static Matrix4 Identity() noexcept {
				return Matrix4(1);
			}
			//正射投影矩阵
			constexpr static Matrix4 Orthographic(float left, float right, float bottom, float top, float zNear, float zFar) noexcept {
				return Matrix4(
					2.0f / (right - left), 0, 0, 0,
					0, 2.0f / (top - bottom), 0, 0,
					0, 0, 2.0f / (zFar - zNear), 0,

					-(right + left) / (right - left),
					-(top + bottom) / (top - bottom),
					-(zFar + zNear) / (zFar - zNear),
					1.0f
				);
			}
			//透视投影矩阵
			static Matrix4 Perspective(float fovy, float aspect, float near, float far) noexcept {
				float tanHalfFovy = std::tan(fovy / 2.0f);
				return Matrix4(
					Float4(1.0f / (aspect * tanHalfFovy), 0, 0, 0),
					Float4(0, 1.0f / tanHalfFovy, 0, 0),
					Float4(0, 0, -(far + near) / (far - near), -1.0f),
					Float4(0, 0, -(2.0f * far * near) / (far - near), 0)
				);
			}
			//向前观察矩阵
			constexpr static Matrix4 LookFront(const Float3& pos, const Float3& front, const Float3& up) noexcept {
				Float3 f = front.normalize();
				Float3 r = f.cross(up).normalize();
				Float3 u = r.cross(f);
				return Matrix4(
					r.x, u.x, -f.x, 0,
					r.y, u.y, -f.y, 0,
					r.z, u.z, -f.z, 0,
					-r.dot(pos), -u.dot(pos), f.dot(pos), 1
				);
			}
			//向前观察矩阵仅更新位置
			constexpr Matrix4& lookFrontUpdatePos(const Float3& pos) noexcept {
				x3 = -(x0 * pos.x + x1 * pos.y + x2 * pos.z);
				y3 = -(y0 * pos.x + y1 * pos.y + y2 * pos.z);
				z3 = -(z0 * pos.x + z1 * pos.y + z2 * pos.z);
				return *this;
			}

			//缩放矩阵
			constexpr static Matrix4 Scale(const Float3& a) noexcept {
				return Matrix4(
					a.x,   0,   0,   0,
					  0, a.y,   0,   0,
					  0,   0, a.z,   0,
					  0,   0,   0,   1
				);
			}
			//缩放当前矩阵
			constexpr Matrix4& scaleThis(const Float3& a) noexcept {
				col[0] *= a.x;
				col[1] *= a.y;
				col[2] *= a.z;
				return *this;
			}

			//旋转Z轴矩阵
			static Matrix4 RotateZ(float radian) noexcept {
				float c = std::cos(radian);
				float s = std::sin(radian);
				return Matrix4(
					c, s, 0, 0,
				   -s, c, 0, 0,
					0, 0, 1, 0,
					0, 0, 0, 1
				);
			}
			//将矩阵乘以旋转Z轴矩阵
			static Matrix4 RotateZ(const Matrix4& mat, float radian) noexcept {
				float c = std::cos(radian);
				float s = std::sin(radian);
				return Matrix4(
					mat.x0 * c + mat.x1 * s, mat.y0 * c + mat.y1 * s, mat.z0 * c + mat.z1 * s, mat.w0 * c + mat.w1 * s,
					mat.x0 * -s + mat.x1 * c, mat.y0 * -s + mat.y1 * c, mat.z0 * -s + mat.z1 * c, mat.w0 * -s + mat.w1 * c,
					mat.x2, mat.y2, mat.z2, mat.w2,
					mat.x3, mat.y3, mat.z3, mat.w3
				);
			}
			//当前矩阵乘以旋转Z轴矩阵
			Matrix4& rotateZThis(float radian) noexcept {
				float c = std::cos(radian);
				float s = std::sin(radian);
				Float4 t1 = col[0] * c + col[1] * s;
				Float4 t2 = col[0] * -s + col[1] * c;
				col[0] = t1;
				col[1] = t2;
				return *this;
			}

			//旋转Y轴矩阵
			static Matrix4 RotateY(float radian) noexcept {
				float c = std::cos(radian);
				float s = std::sin(radian);
				return Matrix4(
					c, 0,-s, 0,
					0, 1, 0, 0,
					s, 0, c, 0,
					0, 0, 0, 1
				);
			}
			//将矩阵乘以旋转Y轴矩阵
			static Matrix4 RotateY(const Matrix4& mat, float radian) noexcept {
				float c = std::cos(radian);
				float s = std::sin(radian);
				return Matrix4(
					mat.x0 * c + mat.x2 * -s, mat.y0 * c + mat.y2 * -s, mat.z0 * c + mat.z2 * -s, mat.w0 * c + mat.w2 * -s,
					mat.x1, mat.y1, mat.z1, mat.w1,
					mat.x0 * s + mat.x2 * c, mat.y0 * s + mat.y2 * c, mat.z0 * s + mat.z2 * c, mat.w0 * s + mat.w2 * c,
					mat.x3, mat.y3, mat.z3, mat.w3
				);
			}
			//当前矩阵乘以旋转Y轴矩阵
			Matrix4& rotateYThis(float radian) noexcept {
				float c = std::cos(radian);
				float s = std::sin(radian);
				Float4 t1 = col[0] * c + col[2] * -s;
				Float4 t2 = col[0] * s + col[2] * c;
				col[0] = t1;
				col[2] = t2;
				return *this;
			}

			//旋转X轴矩阵
			static Matrix4 RotateX(float radian) noexcept {
				float c = std::cos(radian);
				float s = std::sin(radian);
				return Matrix4(
					1, 0, 0, 0,
					0, c,-s, 0,
					0, s, c, 0,
					0, 0, 0, 1
				);
			}
			//将矩阵乘以旋转X轴矩阵
			static Matrix4 RotateX(const Matrix4& mat, float radian) noexcept {
				float c = std::cos(radian);
				float s = std::sin(radian);
				return Matrix4(
					mat.x0, mat.y0, mat.z0, mat.w0,
					mat.x1 * c + mat.x2 * -s, mat.y1 * c + mat.y2 * -s, mat.z1 * c + mat.z2 * -s, mat.w1 * c + mat.w2 * -s,
					mat.x1 * s + mat.x2 * c, mat.y1 * s + mat.y2 * c, mat.z1 * s + mat.z2 * c, mat.w1 * s + mat.w2 * c,
					mat.x3, mat.y3, mat.z3, mat.w3
				);
			}
			//当前矩阵乘以旋转X轴矩阵
			Matrix4& rotateXThis(float radian) noexcept {
				float c = std::cos(radian);
				float s = std::sin(radian);
				Float4 t1 = col[1] * c + col[2] * -s;
				Float4 t2 = col[1] * s + col[2] * c;
				col[1] = t1;
				col[2] = t2;
				return *this;
			}

			//位移矩阵
			constexpr static Matrix4 Translate(const Float3& a) noexcept {
				return Matrix4(
					1, 0, 0, 0,
					0, 1, 0, 0,
					0, 0, 1, 0,
					a.x, a.y, a.z, 1
				);
			}
			//当前矩阵乘以位移矩阵
			constexpr Matrix4& translateThis(const Float3& a) noexcept {
				x3 += x0 * a.x + x1 * a.y + x2 * a.z;
				y3 += y0 * a.x + y1 * a.y + y2 * a.z;
				z3 += z0 * a.x + z1 * a.y + z2 * a.z;
				w3 += w0 * a.x + w1 * a.y + w2 * a.z;
				return *this;
			}
			//移除移动量
			constexpr Matrix4& removeTranslate() noexcept {
				x3 = 0;
				y3 = 0;
				z3 = 0;
				return *this;
			}

			constexpr Matrix4& operator =(float a) noexcept {
				col[0] = a;
				col[1] = a;
				col[2] = a;
				col[3] = a;
				return *this;
			}
			constexpr Matrix4& operator =(const Float4& a) noexcept {
				col[0] = a;
				col[1] = a;
				col[2] = a;
				col[3] = a;
				return *this;
			}
			constexpr Matrix4& operator =(const Matrix4& a) noexcept {
				col[0] = a.col[0];
				col[1] = a.col[1];
				col[2] = a.col[2];
				col[3] = a.col[3];
				return *this;
			}

			constexpr Matrix4 operator +(float a) const noexcept {
				return Matrix4(
					col[0] + a,
					col[1] + a,
					col[2] + a,
					col[3] + a
				);
			}
			constexpr Matrix4& operator +=(float a) noexcept {
				col[0] += a;
				col[1] += a;
				col[2] += a;
				col[3] += a;
				return *this;
			}

			constexpr Matrix4 operator +(const Matrix4& a) const noexcept {
				return Matrix4(
					col[0] + a.col[0],
					col[1] + a.col[1],
					col[2] + a.col[2],
					col[3] + a.col[3]
				);
			}
			constexpr Matrix4& operator +=(const Matrix4& a) noexcept {
				col[0] += a.col[0];
				col[1] += a.col[1];
				col[2] += a.col[2];
				col[3] += a.col[3];
				return *this;
			}

			constexpr Matrix4 operator -() const noexcept {
				return Matrix4(
					-col[0],
					-col[1],
					-col[2],
					-col[3]
				);
			}

			constexpr Matrix4 operator -(float a) const noexcept {
				return Matrix4(
					col[0] - a,
					col[1] - a,
					col[2] - a,
					col[3] - a
				);
			}
			constexpr Matrix4& operator -=(float a) noexcept {
				col[0] -= a;
				col[1] -= a;
				col[2] -= a;
				col[3] -= a;
				return *this;
			}

			constexpr Matrix4 operator -(const Matrix4& a) const noexcept {
				return Matrix4(
					col[0] - a.col[0],
					col[1] - a.col[1],
					col[2] - a.col[2],
					col[3] - a.col[3]
				);
			}
			constexpr Matrix4& operator -=(const Matrix4& a) noexcept {
				col[0] -= a.col[0];
				col[1] -= a.col[1];
				col[2] -= a.col[2];
				col[3] -= a.col[3];
				return *this;
			}

			constexpr Matrix4 operator *(float a) const noexcept {
				return Matrix4(
					col[0] * a,
					col[1] * a,
					col[2] * a,
					col[3] * a
				);
			}
			constexpr Matrix4& operator *=(float a) noexcept {
				col[0] *= a;
				col[1] *= a;
				col[2] *= a;
				col[3] *= a;
				return *this;
			}

			constexpr Float4 operator *(const Float4& a) const noexcept {
				return Float4(
					x0 * a.x + x1 * a.y + x2 * a.z + x3 * a.w,
					y0 * a.x + y1 * a.y + y2 * a.z + y3 * a.w,
					z0 * a.x + z1 * a.y + z2 * a.z + z3 * a.w,
					w0 * a.x + w1 * a.y + w2 * a.z + w3 * a.w
				);
			}

			constexpr Matrix4 operator *(const Matrix4& a) const noexcept {
				return Matrix4(
					*this * a.col[0],
					*this * a.col[1],
					*this * a.col[2],
					*this * a.col[3]
				);
			}
			constexpr Matrix4& operator *=(const Matrix4& a) noexcept {
				return *this = *this * a;
			}

			constexpr Float4& operator [](axis_t a) noexcept {
				if constexpr (_math_safe_index_) {
					return col[a % col_axis];
				}
				else {
					return col[a];
				}
			}
			constexpr const Float4& operator [](axis_t a) const noexcept {
				if constexpr (_math_safe_index_) {
					return col[a % col_axis];
				}
				else {
					return col[a];
				}
			}
			constexpr bool operator ==(const Matrix4& a) const noexcept {
				return col[0] == a.col[0] && col[1] == a.col[1] && col[2] == a.col[2] && col[3] == a.col[3];
			}
			friend std::ostream& operator<<(std::ostream& os, const Matrix4& a) {
				return os
					<< "x0: " << a.x0 << ", x1: " << a.x1 << ", x2: " << a.x2 << ", x3: " << a.x3 << ",\n"
					<< "y0: " << a.y0 << ", y1: " << a.y1 << ", y2: " << a.y2 << ", y3: " << a.y3 << ",\n"
					<< "z0: " << a.z0 << ", z1: " << a.z1 << ", z2: " << a.z2 << ", z3: " << a.z3 << ",\n"
					<< "w0: " << a.w0 << ", w1: " << a.w1 << ", w2: " << a.w2 << ", w3: " << a.w3 << ";\n";
			}
			friend constexpr Hash Hasher(const Matrix4& a) noexcept {
				return Hash::Combine(a.col[0], a.col[1], a.col[2], a.col[3]);
			}
		};
	}

	using math::Matrix4;
}

namespace std {
	template<>
	struct hash<dme::math::Matrix4> {
		size_t operator ()(const dme::math::Matrix4& a) const noexcept {
			return dme::Hasher(a);
		}
	};
}
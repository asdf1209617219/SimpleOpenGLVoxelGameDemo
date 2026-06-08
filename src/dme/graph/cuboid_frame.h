#pragma once
#include <dme/block/cube_face.h>

namespace dme {
	class Game;
	namespace graph {
		class CuboidFrame {
		private:
			friend Game;

			uint vao;
			uint vbo;
			uint ibo;
			bool isDisplay;
			Float3 pos;
			Float3 color;
			DynArr<Float3> vertices;
			Float3 size;

			void init();

			constexpr static float aa = 0.05f;
			constexpr static uint vertexCount = 8 * 2 + 8 * 6;
			constexpr static uint indexCount = 8 * 6 * 6;
			static const uint indices[indexCount];

		public:
			void changeVertices(const Float3& size);

			bool getIsDisplay() const;
			void display();
			void hide();

			Float3 getPos() const;
			void setPos(const Float3& pos);

			Float3 getColor() const;
			void setColor(const Float3& color);

			void draw(const Matrix4& projection, const Matrix4& view) const;

			CuboidFrame();
			~CuboidFrame();
		};

	}
}
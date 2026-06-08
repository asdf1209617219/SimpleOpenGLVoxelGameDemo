#pragma once
#include <dme/block/cube_face.h>

namespace dme {
	class Game;
	namespace graph {
		class CameraF;
		//block被选中时的边框
		class CubeFrame {
		private:
			friend Game;

			uint vao;
			uint vbo;
			bool isDisplay;
			//block的位置
			Int3 pos;


			constexpr static uint vertexCount = 8 * 3;
			static const Float3 vertices[vertexCount];
		public:
			CubeFrame();

			//申请vao和vbo并将顶点传给显存
			void init();
			//删除申请的vao和vbo
			void release();

			bool getIsDisplay() const;
			void display();
			void hide();

			Int3 getPos() const;
			void setPos(const Int3& pos);

			void draw(const CameraF& camera) const;

		};

	}
}
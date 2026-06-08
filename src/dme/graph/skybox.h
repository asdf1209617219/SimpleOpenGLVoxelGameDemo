#pragma once
#include <dme/graph/texture/tex_cube.h>
#include <dme/graph/vertex/vertex_type.h>
#include <dme/graph/shader.h>

namespace dme {
	class Game;
	namespace graph {
		class Skybox {
			friend Game;
		private:
			graph::TexCube texCube;
			uint vao;
			uint vbo;
		public:
			//void draw(const glm::mat4& projection, const glm::mat4& view) const;
			void draw(const Matrix4& projection, const Matrix4& view) const;
			// 删除vao和vbo和纹理
			void release();

			Skybox();
			//初始化天空盒，绑定纹理
			Skybox(graph::TexCube texCube);
		};

	}
}
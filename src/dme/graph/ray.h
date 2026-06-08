#pragma once
#include <dme/core.h>

namespace dme {
	class Game;
	namespace graph {
		class Ray {
		private:
			friend Game;

			uint vao;
			uint vbo;
			bool isDisplay;
			Float3 start;
			Float3 end;

			void init();
		public:
			bool getIsDisplay() const;
			void display();
			void hide();
			void updateStart(const Float3& start);
			void updateEnd(const Float3& end);
			void draw(const Matrix4& projection, const Matrix4& view) const;

			Ray();
			~Ray();
		};

	}
}
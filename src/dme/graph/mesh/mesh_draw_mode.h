#pragma once
#include <dme/core.h>
namespace dme::graph {
	enum class MeshDrawMode : uchar {
		points = GL_POINTS,
		lines = GL_LINES,
		lineLoop = GL_LINE_LOOP,
		lineStrip = GL_LINE_STRIP,
		triangles = GL_TRIANGLES,
		trianglesStrip = GL_TRIANGLE_STRIP,
		trianglesFan = GL_TRIANGLE_FAN,
	};
}
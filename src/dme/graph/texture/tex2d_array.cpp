#include <dme/graph/texture/tex2d_array.h>
namespace dme::graph {
	Tex2dArray::Tex2dArray(TexChannel texChannel) noexcept : Tex(), texChannel(texChannel) {}

	TexIndex Tex2dArray::addIfAbsent(strid path) noexcept {
		auto r = texIndexMap.addIfAbsent(path, TexIndex(pathArr.count()));
		if (r->val == pathArr.count()) {
			pathArr.add(path);
		}
		return r->val;
	}
	TexIndex Tex2dArray::getIndex(strid path) const noexcept {
		auto r = texIndexMap.get(path);
		if (r) {
			return r->val;
		}
		return TexIndex();
	}

	bool Tex2dArray::merge() {
		if (pathArr.count() == dataArr.count()) {
			return true;
		}
		sint width = 0, height = 0;
		if (dataArr.count() == 0) {
			dataArr.add(Tex::LoadImage(pathArr[0], TexTransform::N, texChannel));
			if (!dataArr[0].data()) {
				dataArr.remove();
				return false;
			}
		}
		width = dataArr.last().width();
		height = dataArr.last().height();

		for (uint i = dataArr.count(); i < pathArr.count(); ++i) {
			dataArr.add(Tex::LoadImage(pathArr[i], TexTransform::N, texChannel));
			auto& texData = dataArr.last();
			if (!texData.data()) {
				dataArr.remove();
				return false;
			}
			if (width != texData.width() || height != texData.height()) { //每个纹理必须大小相同
				Log::Error("Texture: 2D texture array cannot use the different sizes, index: ", i,
					", previous sizes: ", width, "*", height, 
					", current sizes: ", texData.width(), "*", texData.height(),
					", path: ", pathArr[i]);
				dataArr.remove();
				return false;
			}
		}

		Tex::release();
		Tex::init();

		glGenTextures(1, &id.v);
		glBindTexture(GL_TEXTURE_2D_ARRAY, id);

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //GL_LINEAR_MIPMAP_LINEAR 会导致纹理不显示
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		sint glFormat = Tex::ChannelToGLFormat(texChannel);

		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, glFormat, width, height, dataArr.count(), 0, glFormat, GL_UNSIGNED_BYTE, NULL);

		for (uint i = 0; i < dataArr.count(); ++i) {
			auto& d = dataArr[i];
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, 1, glFormat, GL_UNSIGNED_BYTE, d.data());
		}
		return true;
	}

	void Tex2dArray::bind(sint location) const {
		glActiveTexture(GL_TEXTURE0 + location);
		glBindTexture(GL_TEXTURE_2D_ARRAY, id);
	}

	Tex2dArray& Tex2dArray::Cube() {
		static Tex2dArray _cube;
		return _cube;
	}
}